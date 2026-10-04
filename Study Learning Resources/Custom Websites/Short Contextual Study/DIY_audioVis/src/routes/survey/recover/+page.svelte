<script lang="ts">
	import { onMount } from 'svelte';
	import { identity } from '$lib/identity.svelte';
	import { answers } from '$lib/answers.svelte';

	let started = $state(false);
	let queued = $state(0);

	const pending = $derived(answers.pendingCount);

	// Derived state machine: 'sending' until the outbox drains, then 'done'.
	const phase = $derived(
		!started ? 'idle' : queued === 0 ? 'empty' : pending > 0 ? 'sending' : 'done'
	);

	onMount(() => {
		identity.init();
		answers.init();
	});

	function resend() {
		queued = answers.resyncAll();
		started = true;
	}
</script>

<section class="recover">
	<p class="eyebrow">Recovery</p>
	<h1 class="headline">Re-save your answers</h1>

	<p class="body">
		If your written answers didn't save properly before, tap the button below. It re-sends
		everything stored on this device, for every day. You only need to do this once, and it's safe
		to tap more than once.
	</p>

	{#if phase === 'idle'}
		<button class="action" onclick={resend}>Re-save my answers</button>
	{:else if phase === 'sending'}
		<button class="action" disabled>Re-sending {queued}…</button>
		<p class="status">Sending {queued} answers. Keep this page open until it finishes.</p>
	{:else if phase === 'done'}
		<p class="status done">Done. {queued} answers re-sent from this device. You can close the tab.</p>
		<button class="action ghost" onclick={resend}>Re-save again</button>
	{:else if phase === 'empty'}
		<p class="status">
			No saved answers were found on this device. This is likely a different laptop or browser, or
			the storage was cleared.
		</p>
	{/if}

	<a class="back" href="/survey/1">Back to the survey</a>
</section>

<style>
	.recover {
		padding-top: 1.5rem;
		max-width: 32rem;
	}
	.eyebrow {
		font-family: var(--mono);
		font-size: 0.7rem;
		letter-spacing: 0.18em;
		text-transform: uppercase;
		color: var(--graphite);
		margin: 0 0 0.6rem;
	}
	.headline {
		font-family: var(--serif);
		font-weight: 500;
		font-size: clamp(2rem, 7vw, 2.6rem);
		letter-spacing: -0.025em;
		margin: 0 0 1.1rem;
		color: var(--ink);
	}
	.body {
		font-size: 1.05rem;
		line-height: 1.6;
		color: var(--graphite);
		margin: 0 0 2rem;
	}
	.action {
		display: inline-block;
		font-family: var(--mono);
		font-size: 0.9rem;
		padding: 0.9rem 1.4rem;
		color: var(--surface);
		background: var(--ink);
		border: 1px solid var(--ink);
		border-radius: var(--radius);
		cursor: pointer;
		transition: opacity 140ms ease;
	}
	.action:disabled {
		opacity: 0.55;
		cursor: default;
	}
	.action.ghost {
		color: var(--ink);
		background: transparent;
		margin-top: 1rem;
	}
	.status {
		font-size: 0.95rem;
		line-height: 1.55;
		color: var(--graphite);
		margin: 1.25rem 0 0;
	}
	.status.done {
		color: var(--ink);
	}
	.back {
		display: block;
		margin-top: 2.5rem;
		font-family: var(--mono);
		font-size: 0.8rem;
		color: var(--graphite);
		text-decoration: underline;
		text-underline-offset: 3px;
	}
</style>
