import { json } from '@sveltejs/kit';
import { issuePass, sameOrigin } from '$lib/server/security.js';
import { verifySolution } from '$lib/server/pow.js';

// Verifies a solved proof-of-work challenge and, on success, issues the signed
// clearance cookie that /api/chat and /api/log require.
/** @type {import('./$types').RequestHandler} */
export async function POST(event) {
	if (!sameOrigin(event)) return json({ ok: false, error: 'Bad origin.' }, { status: 403 });

	const { solution } = await event.request.json().catch(() => ({}));
	if (!verifySolution(solution)) {
		return json({ ok: false, error: 'Verification failed.' }, { status: 403 });
	}

	issuePass(event.cookies);
	return json({ ok: true });
}
