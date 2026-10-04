import crypto from 'node:crypto';
import { env } from '$env/dynamic/private';

// Self-hosted, Altcha-style proof-of-work. Instead of judging "human vs bot"
// (which can demand an interactive challenge), this makes every request cost a
// little CPU: the browser must find a number N such that
//   sha256(`${salt}${N}`) === challenge
// The challenge is HMAC-signed so the server can verify a solution statelessly,
// and the salt carries an expiry so old challenges can't be replayed.
//
// No third party, no cookies of its own, no user interaction. Enforced only when
// POW_SECRET is set, so dev / unconfigured deploys stay open.

// Light difficulty: avg ~MAX/2 hashes to solve. Kept low so mobile solves it in
// the background quickly — the spend cap is the real cost backstop, not this.
const MAX_NUMBER = Number(env.POW_MAX_NUMBER) || 60_000;
const CHALLENGE_TTL_MS = 5 * 60 * 1000; // a fresh challenge is valid for 5 min

function secret() {
	return env.POW_SECRET || 'dev-secret';
}

/** Proof-of-work is enforced only when a server secret is configured. */
export function powEnabled() {
	return !!env.POW_SECRET;
}

/** @param {string} data */
function hmac(data) {
	return crypto.createHmac('sha256', secret()).update(data).digest('hex');
}

/** @param {string} data */
function sha256(data) {
	return crypto.createHash('sha256').update(data).digest('hex');
}

/**
 * Build a fresh challenge for the client to solve.
 * @returns {{ algorithm: 'SHA-256'; challenge: string; salt: string; maxnumber: number; signature: string }}
 */
export function createChallenge() {
	const exp = Date.now() + CHALLENGE_TTL_MS;
	const salt = `${crypto.randomBytes(12).toString('hex')}.${exp}`;
	const number = crypto.randomInt(0, MAX_NUMBER + 1);
	const challenge = sha256(`${salt}${number}`);
	return {
		algorithm: 'SHA-256',
		challenge,
		salt,
		maxnumber: MAX_NUMBER,
		signature: hmac(challenge)
	};
}

/**
 * Verify a solved challenge. Returns true only if the signature is ours, the
 * challenge hasn't expired, and the submitted number actually hashes to it.
 * @param {unknown} solution decoded { challenge, number, salt, signature }
 */
export function verifySolution(solution) {
	if (!powEnabled()) return true;
	if (!solution || typeof solution !== 'object') return false;

	const { challenge, number, salt, signature } = /** @type {any} */ (solution);
	if (typeof challenge !== 'string' || typeof salt !== 'string' || typeof signature !== 'string') {
		return false;
	}
	if (!Number.isInteger(number) || number < 0 || number > MAX_NUMBER) return false;

	// 1. Was this challenge issued by us (and untampered)?
	const expected = hmac(challenge);
	const a = new Uint8Array(Buffer.from(signature));
	const b = new Uint8Array(Buffer.from(expected));
	if (a.length !== b.length || !crypto.timingSafeEqual(a, b)) return false;

	// 2. Has it expired? (expiry is baked into the salt, which the signature covers)
	const exp = Number(salt.split('.')[1]);
	if (!Number.isFinite(exp) || exp < Date.now()) return false;

	// 3. Does the number actually solve the puzzle?
	return sha256(`${salt}${number}`) === challenge;
}
