import adapter from '@sveltejs/adapter-vercel';
import { vitePreprocess } from '@sveltejs/vite-plugin-svelte';

/** @type {import('@sveltejs/kit').Config} */
const config = {
	preprocess: vitePreprocess(),
	kit: {
		// Vercel adapter: static content (hub, worksheets, /done) is prerendered; the
		// client-only survey routes (ssr=false, prerender=false) are served by a small
		// function that returns the app shell, so deep links like /survey/1/task2 work.
		adapter: adapter()
	}
};

export default config;
