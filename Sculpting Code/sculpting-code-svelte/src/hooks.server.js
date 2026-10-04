// Security headers applied to every response.
//
// The CSP is enforced. It was first shipped in Report-Only mode and verified
// clean in production (site load + capture + Draw my shape + the sandboxed p5
// preview running p5 from cdnjs — no violations), then switched on here. If you
// ever need to debug a new violation without breaking the app, temporarily
// rename the header back to 'Content-Security-Policy-Report-Only'.

const CSP = [
	"default-src 'self'",
	"base-uri 'self'",
	"object-src 'none'",
	"frame-ancestors 'self'",
	"form-action 'self'",
	"img-src 'self' data: blob:",
	"font-src 'self' https://fonts.gstatic.com data:",
	"style-src 'self' 'unsafe-inline' https://fonts.googleapis.com",
	// cdnjs + inline needed by the sandboxed p5 preview iframe (srcdoc inherits this policy)
	"script-src 'self' 'unsafe-inline' https://cdnjs.cloudflare.com",
	"connect-src 'self'"
].join('; ');

/** @type {import('@sveltejs/kit').Handle} */
export async function handle({ event, resolve }) {
	const response = await resolve(event);
	const h = response.headers;

	h.set('X-Content-Type-Options', 'nosniff');
	h.set('Referrer-Policy', 'strict-origin-when-cross-origin');
	h.set('X-Frame-Options', 'SAMEORIGIN');
	// Camera is allowed for the webcam capture; everything else off.
	h.set('Permissions-Policy', 'camera=(self), microphone=(), geolocation=(), payment=(), usb=()');
	h.set('Content-Security-Policy', CSP);

	return response;
}
