import { workshop } from '$lib/schema';
import type { EntryGenerator } from './$types';

// Enumerate every day+stage so each survey URL is prerendered to its own static
// shell (e.g. /survey/2/task5). Derived from the schema, so adding a stage there
// automatically adds its prerendered page.
export const entries: EntryGenerator = () =>
	Object.entries(workshop).flatMap(([day, stages]) =>
		stages.map((stage) => ({ day, stage: stage.slug }))
	);
