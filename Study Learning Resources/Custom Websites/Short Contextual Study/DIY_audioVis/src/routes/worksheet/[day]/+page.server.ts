import { error } from '@sveltejs/kit';
import { renderMarkdown } from '$lib/server/markdown';
import type { PageServerLoad, EntryGenerator } from './$types';

// Raw markdown files, resolved at build time.
const files = import.meta.glob('/src/content/*.md', { query: '?raw', import: 'default' });

// Tell the prerenderer which days exist.
export const entries: EntryGenerator = () => [{ day: '1' }, { day: '2' }];

export const load: PageServerLoad = async ({ params }) => {
	const loader = files[`/src/content/day-${params.day}.md`];
	if (!loader) throw error(404, `No worksheet for day ${params.day}`);

	const raw = (await loader()) as string;
	const html = await renderMarkdown(raw);

	return { html, day: params.day };
};
