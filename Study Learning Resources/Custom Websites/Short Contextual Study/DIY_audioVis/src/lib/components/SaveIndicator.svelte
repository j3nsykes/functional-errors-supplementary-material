<script lang="ts">
	import { answers } from '$lib/answers.svelte';

	const status = $derived(answers.status);
	const pending = $derived(answers.pendingCount);

	const label = $derived(
		status === 'offline'
			? pending > 0
				? `Offline, ${pending} to save`
				: 'Offline'
			: pending > 0
				? 'Saving…'
				: 'All saved'
	);
</script>

<div class="indicator" data-status={status} aria-live="polite">
	<span class="dot" data-status={status} data-pending={pending > 0}></span>
	<span class="text">{label}</span>
</div>

<style>
	.indicator {
		display: inline-flex;
		align-items: center;
		gap: 0.5rem;
		font-family: var(--mono);
		font-size: 0.78rem;
		letter-spacing: 0.02em;
		color: var(--graphite);
		white-space: nowrap;
	}
	/* Saved = filled black; saving/offline = hollow ring. */
	.dot {
		width: 9px;
		height: 9px;
		border-radius: 50%;
		background: var(--ink);
		border: 1.5px solid var(--ink);
		flex: 0 0 auto;
	}
	.dot[data-pending='true'] {
		background: transparent;
		border-color: var(--graphite);
	}
	.dot[data-status='offline'] {
		background: transparent;
		border-color: var(--graphite);
	}
</style>
