import { env } from '$env/dynamic/private';
import { MODEL } from '$lib/config.js';

// Add the active model to the header pill. Provider is chosen in .env.
export function load() {
	const provider = (env.LLM_PROVIDER || 'anthropic').toLowerCase();
	const modelLabel = provider === 'openrouter' ? env.OPENROUTER_MODEL || 'openrouter' : MODEL;
	return { modelLabel };
}
