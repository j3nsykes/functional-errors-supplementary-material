// Prerendered to static pages (like the worksheets), so there's no serverless
// function to fail on Vercel. ssr is on: only the static questions render at build
// time — there's no participant data server-side (it all lives in the browser),
// and the client loads saved answers from localStorage on mount. The dynamic
// [day]/[stage] routes are enumerated via `entries` in their +page.ts.
export const ssr = true;
export const prerender = true;
