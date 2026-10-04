<script lang="ts">
	import { answers } from '$lib/answers.svelte';
	import type { Question } from '$lib/schema';

	let {
		day,
		stage,
		question
	}: { day: number; stage: string; question: Extract<Question, { type: 'text' }> } = $props();

	// Make sure the saved answers are loaded before we seed the box below — this can
	// be the first component to touch the store on a fresh load. init() is idempotent.
	answers.init();

	const traceState = $derived(answers.stateOf(day, stage, question.id));
	const fieldId = $derived(`q-${day}-${stage}-${question.id}`);
	const fieldKey = $derived(`${day}:${stage}:${question.id}`);

	let el: HTMLTextAreaElement | null = $state(null);

	// Local, composition-safe model for the box. We bind:value to this rather than
	// re-writing the DOM value from the store on every keystroke. That controlled
	// pattern fights mobile predictive-text / IME keyboards (and, on slower phones,
	// races fast typing) and was silently clipping long answers.
	//
	// `text` holds the CURRENT field's answer. It's seeded from the store on mount and
	// re-seeded ONLY when the field identity changes. Because SvelteKit reuses this
	// component across stages that share a question id (e.g. `learnt` in every task),
	// without that re-seed task 1's answer would linger in task 2's box. The store is
	// never read back while the same field is on screen, so typing can't be clobbered.
	let text = $state('');
	let shownKey = '';

	$effect(() => {
		if (fieldKey !== shownKey) {
			shownKey = fieldKey;
			text = answers.value(day, stage, question.id);
		}
	});

	function grow() {
		if (!el) return;
		el.style.height = 'auto';
		el.style.height = `${el.scrollHeight}px`;
	}

	// Resize to fit whenever the content changes (typing, or the resumed seed).
	// Deferred a frame so the field has its final width before we measure
	// scrollHeight — measuring earlier makes an empty box's placeholder wrap and
	// inflate the height.
	$effect(() => {
		text;
		const id = requestAnimationFrame(grow);
		return () => cancelAnimationFrame(id);
	});

	function handle(e: Event) {
		answers.set(day, stage, question.id, (e.currentTarget as HTMLTextAreaElement).value);
	}
</script>

<div class="field">
	<label class="label" for={fieldId}>{question.label}</label>

	<textarea
		id={fieldId}
		bind:this={el}
		class="box"
		rows="3"
		placeholder={question.placeholder ?? ''}
		bind:value={text}
		oninput={handle}
		aria-describedby={question.hint ? `${fieldId}-hint` : undefined}
	></textarea>

	<div class="trace" data-state={traceState}></div>

	{#if question.hint}
		<p class="hint" id={`${fieldId}-hint`}>{question.hint}</p>
	{/if}
</div>

<style>
	.field {
		margin: 0 0 2.75rem;
	}
	.label {
		display: block;
		margin-bottom: 1rem;
		font-size: 1.2rem;
		font-weight: 500;
		letter-spacing: -0.01em;
		color: var(--ink);
	}
	.box {
		display: block;
		width: 100%;
		min-height: 5rem;
		resize: none;
		overflow: hidden;
		padding: 0.7rem 0.85rem;
		color: var(--ink);
		background: var(--surface);
		border: 1px solid var(--line);
		border-radius: var(--radius);
		font-size: 1.15rem;
		line-height: 1.55;
		transition: border-color 140ms ease;
	}
	.box::placeholder {
		color: var(--graphite);
		opacity: 0.65;
	}
	.box:focus {
		outline: none;
		border-color: var(--ink);
	}
	.box:focus-visible {
		outline: 2px solid var(--ink);
		outline-offset: 2px;
	}
	.trace {
		margin-top: 0.55rem;
	}
	.hint {
		margin: 0.7rem 0 0;
		font-size: 0.8rem;
		color: var(--graphite);
	}
</style>
