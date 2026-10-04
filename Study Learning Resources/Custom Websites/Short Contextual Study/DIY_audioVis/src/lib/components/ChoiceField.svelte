<script lang="ts">
	import { answers } from '$lib/answers.svelte';
	import type { Question } from '$lib/schema';

	let {
		day,
		stage,
		question
	}: { day: number; stage: string; question: Extract<Question, { type: 'choice' }> } = $props();

	const current = $derived(answers.value(day, stage, question.id));
	const traceState = $derived(answers.stateOf(day, stage, question.id));

	function choose(value: string) {
		answers.set(day, stage, question.id, value);
	}
</script>

<fieldset class="field">
	<legend class="label">{question.label}</legend>

	<div class="choices" role="radiogroup" aria-label={question.label}>
		{#each question.options as option (option.value)}
			<button
				type="button"
				class="opt"
				role="radio"
				aria-checked={current === option.value}
				data-selected={current === option.value}
				onclick={() => choose(option.value)}
			>
				{option.label}
			</button>
		{/each}
	</div>

	<div class="trace" data-state={traceState}></div>
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
	.choices {
		display: flex;
		gap: 0.75rem;
		margin-bottom: 0.9rem;
	}
	.opt {
		flex: 1 1 0;
		min-height: 56px;
		padding: 0 1rem;
		font-size: 1rem;
		font-weight: 500;
		color: var(--ink);
		background: transparent;
		border: 1.5px solid var(--line);
		border-radius: var(--radius);
		transition:
			background-color 140ms ease,
			border-color 140ms ease,
			color 140ms ease;
	}
	.opt:hover {
		border-color: var(--ink);
	}
	.opt[data-selected='true'] {
		background: var(--ink);
		border-color: var(--ink);
		color: #fff;
	}
</style>
