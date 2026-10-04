<script lang="ts">
	import { answers } from '$lib/answers.svelte';
	import type { Question } from '$lib/schema';

	let {
		day,
		stage,
		question
	}: { day: number; stage: string; question: Extract<Question, { type: 'scale' }> } = $props();

	const current = $derived(answers.value(day, stage, question.id));
	const state = $derived(answers.stateOf(day, stage, question.id));

	const options = $derived(
		Array.from({ length: question.max - question.min + 1 }, (_, i) => question.min + i)
	);

	function choose(n: number) {
		answers.set(day, stage, question.id, String(n));
	}
</script>

<fieldset class="field">
	<legend class="label">{question.label}</legend>

	<div class="scale" role="radiogroup" aria-label={question.label}>
		{#each options as n (n)}
			<button
				type="button"
				class="pip"
				role="radio"
				aria-checked={current === String(n)}
				data-selected={current === String(n)}
				onclick={() => choose(n)}
			>
				{n}
			</button>
		{/each}
	</div>

	<div class="ends">
		<span>{question.minLabel}</span>
		<span>{question.maxLabel}</span>
	</div>

	<div class="trace" data-state={state}></div>
</fieldset>

<style>
	.field {
		border: 0;
		margin: 0 0 2.75rem;
		padding: 0;
	}
	.label {
		display: block;
		padding: 0;
		margin-bottom: 1.25rem;
		font-size: 1.2rem;
		font-weight: 500;
		letter-spacing: -0.01em;
		color: var(--ink);
	}
	.scale {
		display: flex;
		justify-content: space-between;
		align-items: center;
		gap: 0.5rem;
	}
	.pip {
		width: clamp(56px, 15vw, 72px);
		height: clamp(56px, 15vw, 72px);
		flex: 0 0 auto;
		font-size: 1.2rem;
		font-weight: 400;
		color: var(--graphite);
		background: transparent;
		border: 1.5px solid var(--line);
		border-radius: 50%;
		transition:
			background-color 140ms ease,
			border-color 140ms ease,
			color 140ms ease;
	}
	.pip:hover {
		border-color: var(--ink);
		color: var(--ink);
	}
	.pip[data-selected='true'] {
		background: var(--ink);
		border-color: var(--ink);
		color: #fff;
	}
	.ends {
		display: flex;
		justify-content: space-between;
		margin: 0.9rem 0 0.65rem;
		font-size: 0.68rem;
		letter-spacing: 0.14em;
		color: var(--graphite);
		text-transform: uppercase;
	}
</style>
