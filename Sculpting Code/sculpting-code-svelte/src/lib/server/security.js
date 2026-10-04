import crypto from 'node:crypto';
import { dev } from '$app/environment';
import { env } from '$env/dynamic/private';

// Server-only security helpers: a signed "pass" cookie issued after a passed
// proof-of-work challenge (see pow.js), an origin check, and a per-session
// hourly cap backed by Supabase. Everything no-ops safely when the relevant
// env vars are absent, so dev / unconfigured deploys still work.

const PASS_COOKIE = 'dsc_pass';
const PASS_TTL_S = 8 * 60 * 60; // clearance lasts a workshop session (8h)
const GEN_CAP = Number(env.GENERATION_HOURLY_CAP) || 40; // generations / session / hour

/** The pass gate is enforced only when a server secret is configured. */
export function passEnabled() {
	return !!env.POW_SECRET;
}

/** @param {string} data */
function sign(data) {
	return crypto
		.createHmac('sha256', env.POW_SECRET || 'dev-secret')
		.update(data)
		.digest('base64url');
}

/** Issue a signed, HttpOnly clearance cookie after a passed challenge. */
export function issuePass(/** @type {import('@sveltejs/kit').Cookies} */ cookies) {
	const exp = Math.floor(Date.now() / 1000) + PASS_TTL_S;
	const payload = String(exp);
	cookies.set(PASS_COOKIE, `${payload}.${sign(payload)}`, {
		path: '/',
		httpOnly: true,
		secure: !dev,
		sameSite: 'strict',
		maxAge: PASS_TTL_S
	});
}

/** True if the request carries a valid, unexpired clearance cookie. */
export function hasValidPass(/** @type {import('@sveltejs/kit').Cookies} */ cookies) {
	if (!passEnabled()) return true; // not configured → don't block
	const token = cookies.get(PASS_COOKIE);
	if (!token) return false;
	const [payload, sig] = token.split('.');
	if (!payload || !sig) return false;
	const expected = sign(payload);
	const a = new Uint8Array(Buffer.from(sig));
	const b = new Uint8Array(Buffer.from(expected));
	if (a.length !== b.length || !crypto.timingSafeEqual(a, b)) return false;
	const exp = Number(payload);
	return Number.isFinite(exp) && exp * 1000 > Date.now();
}

/** Reject cross-origin requests (a cheap filter on top of the pass gate). */
export function sameOrigin(/** @type {import('@sveltejs/kit').RequestEvent} */ event) {
	const origin = event.request.headers.get('origin');
	if (!origin) return true; // same-origin navigations may omit Origin
	return origin === event.url.origin;
}

/**
 * Per-session hourly cap via Supabase. Returns true if the request is allowed
 * (and records it). No-ops (allows) when Supabase or the session id is absent.
 * @param {string | undefined} sessionId
 */
export async function withinHourlyCap(sessionId) {
	const url = env.SUPABASE_URL;
	const key = env.SUPABASE_SERVICE_ROLE_KEY;
	if (!url || !key || !sessionId) return true;

	const since = new Date(Date.now() - 3600_000).toISOString();
	const headers = { apikey: key, Authorization: `Bearer ${key}` };

	try {
		const countRes = await fetch(
			`${url}/rest/v1/generations?session_id=eq.${encodeURIComponent(sessionId)}&created_at=gte.${since}&select=id`,
			{ headers: { ...headers, Prefer: 'count=exact', Range: '0-0' } }
		);
		const total = Number((countRes.headers.get('content-range') || '').split('/')[1]) || 0;
		if (total >= GEN_CAP) return false;

		await fetch(`${url}/rest/v1/generations`, {
			method: 'POST',
			headers: { ...headers, 'Content-Type': 'application/json', Prefer: 'return=minimal' },
			body: JSON.stringify({ session_id: String(sessionId).slice(0, 64) })
		});
		return true;
	} catch (err) {
		console.error('[security] hourly cap check failed (allowing):', err);
		return true; // fail open — never break the app on a logging/cap hiccup
	}
}
