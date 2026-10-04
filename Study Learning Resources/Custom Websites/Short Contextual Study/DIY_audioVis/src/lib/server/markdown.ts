import MarkdownIt from 'markdown-it';
// Full bundle: includes arduino/cpp/etc. Runs only at build time (this module is
// server-only), so its size never affects what ships to the browser.
import { createHighlighter, type Highlighter } from 'shiki/bundle/full';

// Server-only ($lib/server) so neither markdown-it nor shiki ever reach the
// client bundle. Worksheet pages are prerendered, so this runs at build time and
// the highlighted HTML is baked into the static output.

const THEME = 'github-light';
const LANGS = [
	'javascript',
	'typescript',
	'bash',
	'shell',
	'json',
	'html',
	'css',
	'c',
	'cpp',
	'java',
	'python',
	'ini'
];
// Fence languages that map onto a bundled grammar. Arduino sketches are C++;
// Processing sketches are Java.
const ALIAS: Record<string, string> = {
	arduino: 'cpp',
	ino: 'cpp',
	processing: 'java',
	pde: 'java',
	sh: 'bash'
};

let renderer: Promise<MarkdownIt> | null = null;

async function build(): Promise<MarkdownIt> {
	const highlighter: Highlighter = await createHighlighter({ themes: [THEME], langs: LANGS });
	const loaded = new Set(highlighter.getLoadedLanguages());

	const md = new MarkdownIt({
		html: false, // markdown authored by the facilitator; keep raw HTML out
		linkify: true, // bare URLs become links
		typographer: true,
		highlight(code, lang) {
			const resolved = ALIAS[lang] ?? lang;
			const language = loaded.has(resolved) ? resolved : 'text';
			return highlighter.codeToHtml(code, { lang: language, theme: THEME });
		}
	});

	// A ```shield fenced block becomes a mount point for the ShieldExplainer
	// component (filled in on the client). Anything else falls through to code.
	const defaultFence = md.renderer.rules.fence!;
	md.renderer.rules.fence = (tokens, idx, options, env, self) => {
		if (tokens[idx].info.trim() === 'shield') {
			return '<div class="embed" data-embed="shield"></div>\n';
		}
		return defaultFence(tokens, idx, options, env, self);
	};

	// External links open in a new tab.
	const defaultLinkOpen =
		md.renderer.rules.link_open ??
		((tokens, idx, options, _env, self) => self.renderToken(tokens, idx, options));
	md.renderer.rules.link_open = (tokens, idx, options, env, self) => {
		const href = tokens[idx].attrGet('href') ?? '';
		if (/^https?:\/\//i.test(href)) {
			tokens[idx].attrSet('target', '_blank');
			tokens[idx].attrSet('rel', 'noopener noreferrer');
		}
		return defaultLinkOpen(tokens, idx, options, env, self);
	};

	return md;
}

export async function renderMarkdown(source: string): Promise<string> {
	renderer ??= build();
	return (await renderer).render(source);
}
