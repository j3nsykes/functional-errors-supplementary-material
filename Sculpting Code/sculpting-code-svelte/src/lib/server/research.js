import { env } from '$env/dynamic/private';

// Anonymous research capture: stores the code a "Draw my shape" generation
// produced, keyed only by the per-session UUID. No images, no IP, no personal
// info. Safely no-ops when Supabase isn't configured, and never throws — a
// logging hiccup must never break a generation.

const MAX_CODE_LEN = 20000; // generous ceiling for a beginner sketch

/**
 * Save one generated sketch to Supabase for research. Fire safely — awaiting it
 * is fine (it swallows its own errors and resolves either way).
 * @param {string | undefined} sessionId
 * @param {{ code?: string; model?: string }} output
 */
export async function saveGeneratedCode(sessionId, output = {}) {
	const url = env.SUPABASE_URL;
	const key = env.SUPABASE_SERVICE_ROLE_KEY;
	const { code, model } = output;
	if (!url || !key || !sessionId || !code) return;

	const row = {
		session_id: String(sessionId).slice(0, 64),
		code: String(code).slice(0, MAX_CODE_LEN),
		model: model ? String(model).slice(0, 128) : null
	};

	try {
		const res = await fetch(`${url}/rest/v1/code_outputs`, {
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
			console.error('[research] code save failed:', res.status, await res.text().catch(() => ''));
		}
	} catch (/** @type {any} */ err) {
		console.error('[research] code save error:', err?.message ?? err);
	}
}
