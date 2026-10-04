<script lang="ts">
	import { onMount } from 'svelte';
	import { answers } from '$lib/answers.svelte';

	// Read ?day= on the client. The query string isn't known at prerender time, so
	// this can't be a render-time derived — default to 1 for the static shell.
	let day = $state(1);
	const pending = $derived(answers.pendingCount);

	onMount(() => {
		const d = Number(new URLSearchParams(location.search).get('day'));
		if (d) day = d;
		// A last nudge so anything typed on the final stage is on its way. The work
		// is already saved locally — this screen is a full stop, not a submit.
		answers.flush();
	});
</script>

<section class="done">
	<h1 class="headline">Saved.</h1>
	<p class="sub">{day >= 2 ? "That's the workshop done. Thank you." : 'See you tomorrow.'}</p>

	<p class="note">
		{#if pending > 0}
			Your answers are stored on this device and finishing syncing now. You can close the tab.
		{:else}
			Everything you wrote is stored and synced. You can close the tab.
		{/if}
	</p>

	<a class="back" href={`/survey/${day}`}>Back to today's questions</a>
</section>

<style>
	.done {
		padding-top: 2rem;
	}
	.headline {
		font-family: var(--serif);
		font-weight: 500;
		font-size: clamp(2.25rem, 8vw, 2.9rem);
		letter-spacing: -0.025em;
		margin: 0 0 0.4rem;
		color: var(--ink);
	}
	.sub {
		font-family: var(--serif);
		font-size: 1.25rem;
		font-weight: 400;
		margin: 0 0 1.75rem;
		color: var(--graphite);
	}
	.note {
		font-size: 0.95rem;
		color: var(--graphite);
		max-width: 26rem;
		margin: 0 0 2rem;
	}
	.back {
		font-family: var(--mono);
		font-size: 0.8rem;
		color: var(--graphite);
		text-decoration: underline;
		text-underline-offset: 3px;
	}
</style>
