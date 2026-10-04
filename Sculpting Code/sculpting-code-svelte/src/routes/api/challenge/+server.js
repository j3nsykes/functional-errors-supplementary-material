import { json } from '@sveltejs/kit';
import { createChallenge } from '$lib/server/pow.js';
import { sameOrigin } from '$lib/server/security.js';

// Issues a fresh proof-of-work challenge for the browser to solve. The solved
// result is posted to /api/verify, which then grants the clearance cookie.
/** @type {import('./$types').RequestHandler} */
export function GET(event) {
	if (!sameOrigin(event)) return json({ error: 'Bad origin.' }, { status: 403 });
	return json(createChallenge());
}
