import { answers } from './answers.svelte';
import { stagesFor } from './schema';

// Where should a returning student land? The furthest stage that already has an
// answer — never the first page if they've done work. Handover §8.

/** Furthest touched stage within a single day, or that day's first stage. */
export function resumeWithinDay(day: number): string {
	const stages = stagesFor(day);
	if (stages.length === 0) return '/survey/1/pre';
	let target = stages[0].slug;
	for (const s of stages) {
		if (answers.stageTouched(day, s.slug)) target = s.slug;
	}
	return `/survey/${day}/${target}`;
}
