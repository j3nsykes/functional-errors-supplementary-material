<script lang="ts">
	import { onMount } from 'svelte';
	import { page } from '$app/state';
	import { goto } from '$app/navigation';
	import { identity } from '$lib/identity.svelte';
	import { answers } from '$lib/answers.svelte';
	import { isValidDay } from '$lib/schema';
	import { resumeWithinDay } from '$lib/resume';

	// The handed-out links are …/1 and …/2 (handover §11). This bare-day route
	// resumes to the furthest stage the student reached within that day.
	onMount(() => {
		identity.init();
		answers.init();
		const day = Number(page.params.day);
		goto(isValidDay(day) ? resumeWithinDay(day) : '/survey/1/pre', { replaceState: true });
	});
</script>

<p class="wait">Opening day {page.params.day}…</p>

<style>
	.wait {
		font-family: var(--mono);
		font-size: 0.85rem;
		color: var(--graphite);
	}
</style>
