<script lang="ts">
	import { answers } from '$lib/answers.svelte';
	import { stagesFor } from '$lib/schema';

	let { day, currentSlug }: { day: number; currentSlug: string } = $props();

	const stages = $derived(stagesFor(day));
</script>

<!-- One dot per stage of the current day, filled when that stage has any answer.
     It encodes real completion state, nothing else. §9 -->
<ol class="rail" aria-label="Progress through today's stages">
	{#each stages as stage (stage.slug)}
		{@const touched = answers.stageTouched(day, stage.slug)}
		{@const current = stage.slug === currentSlug}
		<li
			class="node"
			data-touched={touched}
			data-current={current}
			aria-current={current ? 'step' : undefined}
		>
			<span class="dot"></span>
			<span class="tick">{stage.title}</span>
		</li>
	{/each}
</ol>

<style>
	.rail {
		display: flex;
		align-items: center;
		gap: 0;
		list-style: none;
		margin: 0;
		padding: 0;
		width: 100%;
	}
	.node {
		display: flex;
		align-items: center;
		flex: 1 1 0;
		min-width: 0;
	}
	.node:not(:first-child)::before {
		content: '';
		height: 1.5px;
		flex: 1 1 auto;
		background: var(--line);
	}
	.dot {
		position: relative;
		width: 13px;
		height: 13px;
		border-radius: 50%;
		border: 1.5px solid var(--line);
		background: var(--ground);
		flex: 0 0 auto;
		transition:
			background-color 200ms ease,
			border-color 200ms ease;
	}
	.node[data-touched='true'] .dot {
		background: var(--ink);
		border-color: var(--ink);
	}
	/* Current stage: a bullseye — black ring, white gap, black centre. */
	.node[data-current='true'] .dot {
		background: var(--ground);
		border-color: var(--ink);
	}
	.node[data-current='true'] .dot::after {
		content: '';
		position: absolute;
		inset: 2.5px;
		border-radius: 50%;
		background: var(--ink);
	}
	/* The label of the current stage is exposed to screen readers; visually the
	   rail stays a quiet row of dots. */
	.tick {
		position: absolute;
		width: 1px;
		height: 1px;
		overflow: hidden;
		clip: rect(0 0 0 0);
		white-space: nowrap;
	}
</style>
