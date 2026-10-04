// Worksheets are static content, so — unlike the client-only survey — we render
// and prerender them at build time. Shiki highlighting is then baked into the
// HTML and no markdown/highlighter code ships to the browser.
export const ssr = true;
export const prerender = true;
