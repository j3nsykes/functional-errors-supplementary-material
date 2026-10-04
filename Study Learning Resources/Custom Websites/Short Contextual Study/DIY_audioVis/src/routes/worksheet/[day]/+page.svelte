<script lang="ts">
	import { mount, unmount, tick } from 'svelte';
	import ShieldExplainer from '$lib/components/ShieldExplainer.svelte';
	import type { PageData } from './$types';

	let { data }: { data: PageData } = $props();

	let article = $state<HTMLElement | null>(null);

	// The markdown renderer leaves <div data-embed="shield"> placeholders inside the
	// {@html} content. Mount the interactive component into each one on the client.
	const registry: Record<string, typeof ShieldExplainer> = { shield: ShieldExplainer };

	$effect(() => {
		// Re-run when the rendered html changes (e.g. navigating day 1 -> day 2).
		void data.html;
		const mounted: ReturnType<typeof mount>[] = [];
		tick().then(() => {
			if (!article) return;
			for (const node of article.querySelectorAll<HTMLElement>('[data-embed]')) {
				const Cmp = registry[node.dataset.embed ?? ''];
				if (Cmp) mounted.push(mount(Cmp, { target: node }));
			}
		});
		return () => mounted.forEach((m) => unmount(m));
	});
</script>

<svelte:head>
	<title>Day {data.day} · Worksheet</title>
</svelte:head>

<article class="sheet" bind:this={article}>
	<p class="daymark">Day {data.day} · Worksheet</p>
	<!-- Rendered at build time from src/content/day-{n}.md -->
	<div class="prose">
		{@html data.html}
	</div>
</article>

<style>
	.daymark {
		margin: 0 0 1.5rem;
		font-family: var(--mono);
		font-size: 0.72rem;
		letter-spacing: 0.18em;
		text-transform: uppercase;
		color: var(--graphite);
	}
</style>
