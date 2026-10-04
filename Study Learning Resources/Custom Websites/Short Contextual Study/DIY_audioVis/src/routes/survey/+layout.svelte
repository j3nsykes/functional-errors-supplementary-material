<script lang="ts">
	import { onMount } from 'svelte';
	import { page } from '$app/state';
	import { afterNavigate, replaceState } from '$app/navigation';
	import { browser } from '$app/environment';
	import { identity, participantId } from '$lib/identity.svelte';
	import { answers } from '$lib/answers.svelte';
	import { getSupabase } from '$lib/supabase';
	import { isValidDay } from '$lib/schema';
	import ProgressRail from '$lib/components/ProgressRail.svelte';
	import SaveIndicator from '$lib/components/SaveIndicator.svelte';

	let { children } = $props();

	// Only stage routes carry :day/:stage — index and confirmation don't.
	const day = $derived(Number(page.params.day));
	const stage = $derived(page.params.stage ?? '');
	const showRail = $derived(isValidDay(day) && stage !== '');

	// Keep ?p=<id> in the address bar as a second copy of the identity — the one
	// place a student can screenshot or read aloud if their storage is wiped. §3.
	function mirrorId() {
		if (!browser || !identity.id) return;
		const url = new URL(page.url);
		if (url.searchParams.get('p') === identity.id) return;
		url.searchParams.set('p', identity.id);
		try {
			replaceState(url, page.state);
		} catch {
			// Router not ready yet; afterNavigate will catch the next opportunity.
		}
	}

	onMount(async () => {
		identity.init();
		answers.init();

		// First load only: seed a participants row so we have a started-vs-completed
		// denominator later. No .select(), no foreign key — a race here must never
		// cost an answer. §3, §4. Scoped to the survey — worksheet pages are public
		// and never create a participant. The stored id is the hashed form.
		if (identity.isNew && identity.id) {
			const pid = await participantId();
			if (pid) await getSupabase().from('participants').insert({ id: pid });
		}

		mirrorId();
	});

	afterNavigate(() => mirrorId());
</script>

<div class="shell">
	{#if showRail}
		<header class="chrome">
			<div class="chrome-inner">
				<div class="rail-wrap">
					<ProgressRail {day} currentSlug={stage} />
				</div>
				<SaveIndicator />
			</div>
		</header>
	{/if}

	<main class="main">
		{@render children()}
	</main>

	{#if browser && identity.id}
		<footer class="foot">
			<span class="pid">id {identity.id.slice(0, 8)}</span>
			<a class="recover-link" href="/survey/recover">Answers didn't save? Re-save them</a>
		</footer>
	{/if}
</div>

<style>
	.shell {
		min-height: 100dvh;
		display: flex;
		flex-direction: column;
	}
	.chrome {
		position: sticky;
		top: 0;
		z-index: 10;
		background: color-mix(in srgb, var(--ground) 90%, transparent);
		backdrop-filter: blur(6px);
		border-bottom: 1px solid var(--line);
	}
	.chrome-inner {
		width: 100%;
		max-width: var(--maxw);
		margin: 0 auto;
		padding: 0.85rem 1.25rem;
		display: flex;
		align-items: center;
		gap: 1rem;
	}
	.rail-wrap {
		flex: 1 1 auto;
		min-width: 0;
	}
	.main {
		flex: 1 1 auto;
		width: 100%;
		max-width: var(--maxw);
		margin: 0 auto;
		padding: 1.75rem 1.25rem 4rem;
	}
	.foot {
		width: 100%;
		max-width: var(--maxw);
		margin: 0 auto;
		padding: 0 1.25rem 1.5rem;
		display: flex;
		align-items: baseline;
		justify-content: space-between;
		gap: 1rem;
	}
	.pid {
		font-family: var(--mono);
		font-size: 0.7rem;
		color: var(--graphite);
		opacity: 0.7;
	}
	.recover-link {
		font-family: var(--mono);
		font-size: 0.7rem;
		color: var(--graphite);
		opacity: 0.7;
		text-decoration: underline;
		text-underline-offset: 2px;
	}
</style>
