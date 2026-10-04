<script lang="ts">
	import { tick } from 'svelte';
	import { page } from '$app/state';
	import { goto, afterNavigate } from '$app/navigation';
	import { identity } from '$lib/identity.svelte';
	import { answers } from '$lib/answers.svelte';
	import { getStage, stagesFor, stageIndex, isValidDay } from '$lib/schema';
	import ScaleField from '$lib/components/ScaleField.svelte';
	import ChoiceField from '$lib/components/ChoiceField.svelte';
	import TextField from '$lib/components/TextField.svelte';

	const day = $derived(Number(page.params.day));
	const slug = $derived(page.params.stage ?? '');
	const stage = $derived(getStage(day, slug));
	const idx = $derived(stageIndex(day, slug));
	const stages = $derived(stagesFor(day));
	const isLast = $derived(idx >= 0 && idx === stages.length - 1);
	const isFirst = $derived(idx <= 0);

	// Guard: an unknown day/stage bounces to the start rather than showing nothing.
	$effect(() => {
		if (!isValidDay(day) || !stage) {
			goto('/survey/1/pre', { replaceState: true });
		}
	});

	function next() {
		if (isLast) {
			goto(`/survey/done?day=${day}`);
		} else {
			goto(`/survey/${day}/${stages[idx + 1].slug}`);
		}
	}

	function back() {
		if (!isFirst) goto(`/survey/${day}/${stages[idx - 1].slug}`);
	}

	// Fresh stage starts at the top; identity is ensured on entry.
	afterNavigate(async () => {
		identity.init();
		answers.init();
		await tick();
		if (typeof window !== 'undefined') window.scrollTo({ top: 0 });
	});
</script>

{#if stage}
	<article>
		<p class="daymark">Day {day}</p>
		<h1 class="title">{stage.title}</h1>

		{#each stage.questions as question (question.id)}
			{#if question.type === 'scale'}
				<ScaleField {day} stage={slug} {question} />
			{:else if question.type === 'choice'}
				<ChoiceField {day} stage={slug} {question} />
			{:else}
				<TextField {day} stage={slug} {question} />
			{/if}
		{/each}

		<button type="button" class="next" onclick={next}>
			{isLast ? 'Done for today' : 'Next'}
		</button>

		{#if !isFirst}
			<button type="button" class="back" onclick={back}>Back</button>
		{/if}
	</article>
{/if}

<style>
	.daymark {
		margin: 0 0 0.5rem;
		font-family: var(--mono);
		font-size: 0.72rem;
		letter-spacing: 0.18em;
		text-transform: uppercase;
		color: var(--graphite);
	}
	.title {
		font-family: var(--serif);
		font-weight: 500;
		font-size: clamp(2.25rem, 8vw, 2.9rem);
		letter-spacing: -0.025em;
		line-height: 1.05;
		margin: 0 0 2.75rem;
		color: var(--ink);
	}
	.next {
		display: inline-flex;
		align-items: center;
		justify-content: center;
		gap: 0.6rem;
		width: 100%;
		padding: 1.05rem 1.25rem;
		margin-top: 1rem;
		font-size: 0.8rem;
		font-weight: 500;
		letter-spacing: 0.18em;
		text-transform: uppercase;
		color: var(--ink);
		background: transparent;
		border: 1.5px solid var(--ink);
		border-radius: var(--radius);
		transition:
			background-color 140ms ease,
			color 140ms ease;
	}
	.next::after {
		content: '→';
		font-size: 1rem;
		letter-spacing: 0;
	}
	.next:hover {
		background: var(--ink);
		color: var(--ground);
	}
	.back {
		display: inline-flex;
		align-items: center;
		gap: 0.5rem;
		width: 100%;
		justify-content: center;
		margin-top: 0.85rem;
		padding: 0.6rem;
		font-size: 0.72rem;
		font-weight: 500;
		letter-spacing: 0.18em;
		text-transform: uppercase;
		color: var(--graphite);
		background: transparent;
		border: 0;
		transition: color 140ms ease;
	}
	.back::before {
		content: '←';
		font-size: 1rem;
		letter-spacing: 0;
	}
	.back:hover {
		color: var(--ink);
	}
</style>
