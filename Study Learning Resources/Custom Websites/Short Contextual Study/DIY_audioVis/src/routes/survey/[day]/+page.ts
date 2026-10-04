import type { EntryGenerator } from './$types';

// Prerender both day landing routes (/survey/1, /survey/2) to static shells.
export const entries: EntryGenerator = () => [{ day: '1' }, { day: '2' }];
