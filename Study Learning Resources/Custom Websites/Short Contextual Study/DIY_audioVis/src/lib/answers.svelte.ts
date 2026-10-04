import { browser } from '$app/environment';
import { PUBLIC_SUPABASE_URL, PUBLIC_SUPABASE_ANON_KEY } from '$env/static/public';
import { getSupabase } from './supabase';
import { participantId } from './identity.svelte';



const ANSWERS_KEY = 'wsk:answers';
const OUTBOX_KEY = 'wsk:outbox';

const DEBOUNCE_MS = 800;
const BACKOFF_START = 1000;
const BACKOFF_MAX = 30_000;

type OutboxRow = {
	day: number;
	stage: string;
	question_id: string;
	value: string;
};

export type SaveStatus = 'idle' | 'saving' | 'offline';
export type FieldState = 'empty' | 'pending' | 'saved';

function key(day: number, stage: string, qid: string): string {
	return `${day}:${stage}:${qid}`;
}

function load<T>(k: string, fallback: T): T {
	if (!browser) return fallback;
	try {
		const raw = localStorage.getItem(k);
		return raw ? (JSON.parse(raw) as T) : fallback;
	} catch {
		return fallback;
	}
}

class Answers {

	answers = $state<Record<string, string>>({});
	outbox = $state<Record<string, OutboxRow>>({});
	status = $state<SaveStatus>('idle');

	#started = false;
	#debounce: ReturnType<typeof setTimeout> | null = null;
	#retry: ReturnType<typeof setTimeout> | null = null;
	#backoff = BACKOFF_START;
	#flushing = false;
	// The resolved (hashed) participant id, cached the first time a flush resolves it.
	// Held so the pagehide handler can post synchronously as the tab unloads, when
	// there's no time to await the async hash.
	#pidCache: string | null = null;

	get pendingCount(): number {
		return Object.keys(this.outbox).length;
	}

	init() {
		if (!browser || this.#started) return;
		this.#started = true;

		this.answers = load<Record<string, string>>(ANSWERS_KEY, {});
		this.outbox = load<Record<string, OutboxRow>>(OUTBOX_KEY, {});

		// Warm the hashed id so the pagehide keepalive post can run synchronously
		// even if the very first answer is closed on quickly.
		participantId().then((pid) => {
			if (pid) this.#pidCache = pid;
		});

		// Fire a flush on the moments a tab is most likely to disappear. On a plain
		// tab-switch the normal async flush has time to finish. On pagehide (close /
		// navigate away) it would be cancelled mid-request, so we use a keepalive POST
		// the browser is required to deliver even as the page unloads. This is the fix
		// for "the paragraph typed just before Next never reached the database".
		document.addEventListener('visibilitychange', () => {
			if (document.visibilityState === 'hidden') this.flush();
		});
		window.addEventListener('pagehide', () => this.#flushKeepalive());
		window.addEventListener('online', () => this.flush());
		window.addEventListener('offline', () => {
			this.status = 'offline';
		});

		// Recover the "closed the laptop with no wifi" case.
		if (this.pendingCount > 0) this.flush();
	}

	value(day: number, stage: string, qid: string): string {
		return this.answers[key(day, stage, qid)] ?? '';
	}

	stateOf(day: number, stage: string, qid: string): FieldState {
		const k = key(day, stage, qid);
		if (k in this.outbox) return 'pending';
		const v = this.answers[k];
		return v !== undefined && v !== '' ? 'saved' : 'empty';
	}

	/** True if any question in this day+stage has an answer. Used by the resume logic. */
	stageTouched(day: number, stage: string): boolean {
		const prefix = `${day}:${stage}:`;
		return Object.keys(this.answers).some((k) => k.startsWith(prefix) && this.answers[k] !== '');
	}

	set(day: number, stage: string, qid: string, value: string) {
		const k = key(day, stage, qid);

		// 1. + 2. update state and persist the source of truth synchronously.
		this.answers[k] = value;
		this.#persistAnswers();

		// 3. one outbox entry per field — repeated edits collapse onto it.
		this.outbox[k] = { day, stage, question_id: qid, value };
		this.#persistOutbox();

		// 4. schedule a flush.
		this.#scheduleFlush();
	}

	#persistAnswers() {
		if (browser) localStorage.setItem(ANSWERS_KEY, JSON.stringify($state.snapshot(this.answers)));
	}

	#persistOutbox() {
		if (browser) localStorage.setItem(OUTBOX_KEY, JSON.stringify($state.snapshot(this.outbox)));
	}

	#scheduleFlush() {
		if (this.#debounce) clearTimeout(this.#debounce);
		this.#debounce = setTimeout(() => this.flush(), DEBOUNCE_MS);
	}

	async flush() {
		if (!browser) return;
		if (this.#debounce) {
			clearTimeout(this.#debounce);
			this.#debounce = null;
		}
		if (this.#flushing) return;
		this.#flushing = true;

		// The id written to Supabase is a one-way hash of the on-screen UUID. It is
		// awaited, so the re-entry guard above is set *before* this await to keep two
		// flushes from overlapping.
		const pid = await participantId();
		if (!pid) {
			// Identity not resolved yet — try again shortly rather than waiting for
			// the next keystroke, so pending work still syncs on its own.
			this.#flushing = false;
			this.#scheduleRetry();
			return;
		}
		this.#pidCache = pid;

		// Snapshot the keys we are about to send. New edits may arrive mid-flight,
		// so on success we delete exactly these keys, not the whole outbox.
		const snapshotKeys = Object.keys(this.outbox);
		if (snapshotKeys.length === 0) {
			this.#flushing = false;
			this.status = 'idle';
			return;
		}

		if (!navigator.onLine) {
			this.#flushing = false;
			this.status = 'offline';
			this.#scheduleRetry();
			return;
		}

		this.status = 'saving';

		// Snapshot the value we send for each key, so a mid-flight edit to the same
		// field is not silently discarded when we clear the outbox on success.
		const sent = snapshotKeys.map((k) => ({ k, row: { ...this.outbox[k] } }));

		const results = await Promise.allSettled(sent.map(({ row }) => this.#syncRow(pid, row)));

		this.#flushing = false;

		let anyFailed = false;
		results.forEach((res, i) => {
			const ok = res.status === 'fulfilled' && res.value === true;
			if (!ok) {
				anyFailed = true;
				return;
			}
			// Confirmed — but only clear the entry if it hasn't been re-edited since.
			const { k, row } = sent[i];
			if (this.outbox[k] && this.outbox[k].value === row.value) delete this.outbox[k];
		});
		this.#persistOutbox();

		if (anyFailed) {
			this.status = navigator.onLine ? 'saving' : 'offline';
			this.#scheduleRetry();
			return;
		}

		this.#backoff = BACKOFF_START;

		if (this.pendingCount > 0) {
			// More arrived (or was re-edited) while we were sending; keep going.
			this.status = 'saving';
			this.flush();
		} else {
			this.status = 'idle';
		}
	}

	/**
	 * Persist one answer as its own INSERT — append-only. Every save is an
	 * independent row that either lands or is retried; nothing depends on a
	 * follow-up UPDATE succeeding (an UPDATE can silently touch zero rows and look
	 * like success, which is how whole paragraphs were lost). At analysis time the
	 * latest row per question wins (see supabase/schema.sql and ANALYSIS.md).
	 *
	 * The unique(participant_id, day, stage, question_id) constraint must be dropped
	 * for a clean append (schema.sql does this). While it might still be present,
	 * we keep the old INSERT-then-UPDATE fallback so nothing is lost mid-migration.
	 * No call chains .select(), so the dataset stays unreadable. Returns true only
	 * when the value is confirmed stored.
	 */
	async #syncRow(pid: string, row: OutboxRow): Promise<boolean> {
		const now = new Date().toISOString();
		const supabase = getSupabase();

		const ins = await supabase
			.from('responses')
			.insert({ participant_id: pid, ...row, updated_at: now });

		if (!ins.error) return true;
		if (ins.error.code !== '23505') return false; // real error (auth/network) → retry

		// Constraint still present (pre-migration): fall back to an UPDATE so the
		// fuller value still replaces the earlier fragment.
		const upd = await supabase
			.from('responses')
			.update({ value: row.value, updated_at: now })
			.eq('participant_id', pid)
			.eq('day', row.day)
			.eq('stage', row.stage)
			.eq('question_id', row.question_id);

		return !upd.error;
	}

	/**
	 * Last attempt delivery as the tab unloads. A normal fetch is cancelled on
	 * pagehide; a keepalive fetch is guaranteed to be sent. supabase-js doesn't
	 * expose keepalive, so this posts to the REST endpoint directly with the same
	 * anon key. Fire-and-forget: the outbox is left intact so the next open (or the
	 * recovery button) re-sends anything that didn't make it. Append-only, so a
	 * duplicate here is harmless — analysis keeps the latest per question.
	 */
	#flushKeepalive() {
		if (!browser) return;
		const pid = this.#pidCache;
		if (!pid) return;
		const keys = Object.keys(this.outbox);
		if (keys.length === 0) return;

		const now = new Date().toISOString();
		const rows = keys.map((k) => ({ participant_id: pid, ...this.outbox[k], updated_at: now }));

		try {
			fetch(`${PUBLIC_SUPABASE_URL}/rest/v1/responses`, {
				method: 'POST',
				keepalive: true,
				headers: {
					apikey: PUBLIC_SUPABASE_ANON_KEY,
					Authorization: `Bearer ${PUBLIC_SUPABASE_ANON_KEY}`,
					'Content-Type': 'application/json',
					Prefer: 'return=minimal'
				},
				body: JSON.stringify(rows)
			});
		} catch {
			// Nothing to do as the page dies; the local copy is the backstop.
		}
	}

	/**
	 * Re-queue every answer held on this device and push it. Powers the recovery
	 * button: local storage always captured the full text, so this re-sends the
	 * complete answers even when the database only ever received a fragment.
	 * Returns how many answers were queued.
	 */
	resyncAll(): number {
		if (!browser) return 0;
		let n = 0;
		for (const k of Object.keys(this.answers)) {
			const value = this.answers[k];
			if (value === '' || value == null) continue;
			// key is `${day}:${stage}:${qid}`; split on the first two colons only so
			// a question id containing a colon would still parse.
			const c1 = k.indexOf(':');
			const c2 = k.indexOf(':', c1 + 1);
			if (c1 < 0 || c2 < 0) continue;
			this.outbox[k] = {
				day: Number(k.slice(0, c1)),
				stage: k.slice(c1 + 1, c2),
				question_id: k.slice(c2 + 1),
				value
			};
			n++;
		}
		this.#persistOutbox();
		this.flush();
		return n;
	}

	#scheduleRetry() {
		if (this.#retry) clearTimeout(this.#retry);
		const delay = this.#backoff;
		this.#backoff = Math.min(this.#backoff * 2, BACKOFF_MAX);
		this.#retry = setTimeout(() => this.flush(), delay);
	}
}

export const answers = new Answers();
