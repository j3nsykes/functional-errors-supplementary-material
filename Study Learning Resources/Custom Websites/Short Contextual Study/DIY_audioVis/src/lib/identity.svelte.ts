import { browser } from '$app/environment';

// One UUID per browser, generated silently on first load, reused forever.
// Handover §3.

const KEY = 'wsk:participant';
const UUID_RE = /^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$/i;

class Identity {
	id = $state<string | null>(null);
	isNew = $state(false);

	init() {
		if (!browser || this.id) return;

		const stored = localStorage.getItem(KEY);
		const fromUrl = new URLSearchParams(location.search).get('p');

		// Resolution order, highest priority first:
		//   1. a known-good local identity — never overwrite it
		//   2. ?p=<uuid> recovery hatch, only if it is a valid UUID
		//   3. a brand new participant
		let id = stored;
		if (!id && fromUrl && UUID_RE.test(fromUrl)) id = fromUrl;
		if (!id) {
			id = crypto.randomUUID();
			this.isNew = true;
		}

		localStorage.setItem(KEY, id);
		this.id = id;
	}
}

export const identity = new Identity();

// The id shown on screen and kept in localStorage is the raw UUID. What gets
// written to the database is a one-way SHA-256 hash of it, formatted as a UUID so
// it still fits the `uuid` columns. Hashing is deterministic, so every answer a
// participant gives shares the same stored id — you can follow one respondent
// from Task 1 to Exit — but the stored id can't be reversed to the on-screen one.
let hashCache: { id: string; value: Promise<string> } | null = null;

async function sha256Uuid(input: string): Promise<string> {
	const bytes = new TextEncoder().encode(input);
	const digest = await crypto.subtle.digest('SHA-256', bytes);
	const hex = Array.from(new Uint8Array(digest))
		.map((b) => b.toString(16).padStart(2, '0'))
		.join('')
		.slice(0, 32);
	return `${hex.slice(0, 8)}-${hex.slice(8, 12)}-${hex.slice(12, 16)}-${hex.slice(16, 20)}-${hex.slice(20, 32)}`;
}

/** The hashed id to store in Supabase, or null until identity resolves. */
export function participantId(): Promise<string> | null {
	if (!browser || !identity.id) return null;
	if (!hashCache || hashCache.id !== identity.id) {
		hashCache = { id: identity.id, value: sha256Uuid(identity.id) };
	}
	return hashCache.value;
}
