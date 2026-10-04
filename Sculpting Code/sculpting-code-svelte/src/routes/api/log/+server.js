import { env } from '$env/dynamic/private';
import { json } from '@sveltejs/kit';
import { sameOrigin, hasValidPass } from '$lib/server/security.js';

// Research logging: stores freestyle prompts + a per-session UUID in Supabase.
// No images, no IP, no personal info. Safely no-ops if Supabase isn't configured.
const MAX_PROMPT_LEN = 2000;

/** @type {import('./$types').RequestHandler} */
export async function POST(event) {
	const { request } = event;

	// Same abuse protection as /api/chat (both no-op safely when unconfigured).
	if (!sameOrigin(event)) return json({ ok: false, error: 'Bad origin.' }, { status: 403 });
	if (!hasValidPass(event.cookies)) return json({ ok: false }, { status: 403 });

	const url = env.SUPABASE_URL;
	const key = env.SUPABASE_SERVICE_ROLE_KEY;

	// Not configured → do nothing (keeps the app working without logging set up).
	if (!url || !key) return new Response(null, { status: 204 });

	const { sessionId, prompt, model } = await request.json().catch(() => ({}));

	if (!sessionId || typeof prompt !== 'string' || !prompt.trim()) {
		return json({ ok: false, error: 'sessionId and a non-empty prompt are required.' }, { status: 400 });
	}

	const row = {
		session_id: String(sessionId).slice(0, 64),
		prompt: prompt.slice(0, MAX_PROMPT_LEN),
		model: model ? String(model).slice(0, 128) : null
	};

	try {
		const res = await fetch(`${url}/rest/v1/prompts`, {
			method: 'POST',
			headers: {
				apikey: key,
				Authorization: `Bearer ${key}`,
				'Content-Type': 'application/json',
				Prefer: 'return=minimal'
			},
			body: JSON.stringify(row)
		});
		if (!res.ok) {
			console.error('[api/log] Supabase insert failed:', res.status, await res.text().catch(() => ''));
			return json({ ok: false }, { status: 502 });
		}
		return json({ ok: true });
	} catch (/** @type {any} */ err) {
		console.error('[api/log] error:', err?.message ?? err);
		return json({ ok: false }, { status: 500 });
	}
}
