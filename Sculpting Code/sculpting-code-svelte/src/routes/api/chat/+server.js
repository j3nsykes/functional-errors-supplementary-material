import Anthropic from '@anthropic-ai/sdk';
import { env } from '$env/dynamic/private';
import { error, json } from '@sveltejs/kit';
import { MODEL } from '$lib/config.js';
import { SYSTEM_PROMPT } from '$lib/prompts.js';
import { sameOrigin, hasValidPass, withinHourlyCap } from '$lib/server/security.js';
import { saveGeneratedCode } from '$lib/server/research.js';

// Anthropic accepts up to ~5MB per image (base64). Guard a little under that.
const MAX_IMAGE_BYTES = 5 * 1024 * 1024;
// Bound the conversation so a direct API caller can't run up input-token cost
// with a giant payload (the UI never sends anything near these).
const MAX_MESSAGES = 40;
const MAX_TEXT_CHARS = 100_000;

// Shared output schema. The model returns a teaching-friendly object so the UI
// can show a plain-language explanation + concept list and route code to the editor.
const JSON_SCHEMA = {
	type: 'object',
	properties: {
		explanation: {
			type: 'string',
			description: 'One friendly sentence describing what the sketch draws. No code.'
		},
		concepts: {
			type: 'array',
			description: 'The 2-4 most important p5.js constructs used, for teaching.',
			items: {
				type: 'object',
				properties: {
					concept: { type: 'string', description: 'Short name, e.g. "rect()", "for loop".' },
					why: { type: 'string', description: 'Plain-language reason it was used.' }
				},
				required: ['concept', 'why'],
				additionalProperties: false
			}
		},
		code: { type: 'string', description: 'The full p5.js sketch (setup + draw).' }
	},
	required: ['explanation', 'concepts', 'code'],
	additionalProperties: false
};

/** Small helper so provider code can throw an error carrying an HTTP status.
 * @param {number} status @param {string} message */
function httpError(status, message) {
	const err = new Error(message);
	/** @type {any} */ (err).status = status;
	return err;
}

/** Validate + normalise the model's JSON string into our response shape.
 * @param {string} raw */
function parseResult(raw) {
	let data;
	try {
		data = JSON.parse(raw);
	} catch {
		console.error('[api/chat] could not parse structured output:', (raw || '').slice(0, 200));
		throw httpError(502, 'Your image was too hard to draw in code. Please try again.');
	}
	return {
		explanation: typeof data.explanation === 'string' ? data.explanation : '',
		concepts: Array.isArray(data.concepts) ? data.concepts : [],
		code: typeof data.code === 'string' ? data.code : ''
	};
}

/** Default provider: Anthropic (Claude), with native structured outputs.
 * @param {any[]} messages @param {string} systemPrompt */
async function runAnthropic(messages, systemPrompt) {
	if (!env.ANTHROPIC_API_KEY || env.ANTHROPIC_API_KEY.includes('REPLACE_ME')) {
		throw httpError(500, 'ANTHROPIC_API_KEY is not set. Add it to your .env file (see .env.example).');
	}
	const client = new Anthropic({ apiKey: env.ANTHROPIC_API_KEY });
	const response = await client.messages.create({
		model: MODEL,
		max_tokens: 4096,
		system: systemPrompt,
		messages,
		output_config: { format: /** @type {any} */ ({ type: 'json_schema', schema: JSON_SCHEMA }) }
	});
	if (response.stop_reason === 'refusal') {
		throw httpError(400, 'Claude declined this request. Try a different image or prompt.');
	}
	const textBlock = response.content.find((b) => b.type === 'text');
	return parseResult(textBlock && 'text' in textBlock ? textBlock.text : '');
}

/** Translate our Anthropic-shaped messages into OpenAI/OpenRouter format.
 * @param {any[]} messages @param {string} systemPrompt */
function toOpenAIMessages(messages, systemPrompt) {
	const out = [{ role: 'system', content: systemPrompt }];
	for (const m of messages) {
		if (typeof m.content === 'string') {
			out.push({ role: m.role, content: m.content });
		} else if (Array.isArray(m.content)) {
			const parts = m.content
				.map((/** @type {any} */ b) => {
					if (b.type === 'text') return { type: 'text', text: b.text };
					if (b.type === 'image' && b.source?.type === 'base64') {
						return {
							type: 'image_url',
							image_url: { url: `data:${b.source.media_type};base64,${b.source.data}` }
						};
					}
					return null;
				})
				.filter(Boolean);
			out.push({ role: m.role, content: parts });
		}
	}
	return out;
}

/** Alternative provider: OpenRouter (OpenAI-compatible), for open-source models.
 * @param {any[]} messages @param {string} systemPrompt */
async function runOpenRouter(messages, systemPrompt) {
	if (!env.OPENROUTER_API_KEY) {
		throw httpError(500, 'OPENROUTER_API_KEY is not set. Add it to your .env file (see .env.example).');
	}
	const model = env.OPENROUTER_MODEL;
	if (!model) {
		throw httpError(500, 'OPENROUTER_MODEL is not set. Add a vision-capable model slug to your .env.');
	}

	const res = await fetch('https://openrouter.ai/api/v1/chat/completions', {
		method: 'POST',
		headers: {
			Authorization: `Bearer ${env.OPENROUTER_API_KEY}`,
			'Content-Type': 'application/json',
			'X-Title': 'Draw, Sculpt + Code'
		},
		body: JSON.stringify({
			model,
			max_tokens: 4096,
			messages: toOpenAIMessages(messages, systemPrompt),
			response_format: {
				type: 'json_schema',
				json_schema: { name: 'sketch_response', strict: true, schema: JSON_SCHEMA }
			}
		})
	});

	if (!res.ok) {
		const body = await res.text().catch(() => '');
		let message = `OpenRouter request failed (${res.status}).`;
		try {
			message = JSON.parse(body)?.error?.message || message;
		} catch {
			/* keep default */
		}
		throw httpError(res.status, message);
	}

	const body = await res.json();
	const content = body?.choices?.[0]?.message?.content;
	const raw = typeof content === 'string' ? content : JSON.stringify(content ?? '');
	return parseResult(raw);
}

/** @type {import('./$types').RequestHandler} */
export async function POST(event) {
	const { request } = event;

	// Bot / abuse protection (all no-op safely when unconfigured).
	if (!sameOrigin(event)) return json({ error: 'Bad origin.' }, { status: 403 });
	if (!hasValidPass(event.cookies)) {
		return json({ error: 'verification-required' }, { status: 403 });
	}

	// The system prompt is owned by the server, never trusted from the client —
	// otherwise the endpoint would be an open LLM proxy for arbitrary prompts.
	const { messages, sessionId } = await request.json();

	if (!Array.isArray(messages) || messages.length === 0) {
		throw error(400, 'Request must include a non-empty "messages" array.');
	}
	if (messages.length > MAX_MESSAGES) {
		return json({ error: 'Conversation too long. Please restart.' }, { status: 413 });
	}

	// Cap total text so a huge prompt can't run up input-token cost (images are
	// bounded separately below).
	let textChars = 0;
	for (const msg of messages) {
		if (typeof msg.content === 'string') textChars += msg.content.length;
		else if (Array.isArray(msg.content)) {
			for (const block of msg.content) {
				if (block?.type === 'text' && typeof block.text === 'string') textChars += block.text.length;
			}
		}
	}
	if (textChars > MAX_TEXT_CHARS) {
		return json({ error: 'That request is too large. Please shorten it.' }, { status: 413 });
	}

	if (!(await withinHourlyCap(sessionId))) {
		return json(
			{ error: 'Hourly limit reached for this session. Please try again later.' },
			{ status: 429 }
		);
	}

	// Reject oversized images early with a clear message (base64 length * 0.75 ≈ bytes).
	for (const msg of messages) {
		if (!Array.isArray(msg.content)) continue;
		for (const block of msg.content) {
			if (block?.type === 'image' && block.source?.type === 'base64') {
				const bytes = Math.floor((block.source.data?.length ?? 0) * 0.75);
				if (bytes > MAX_IMAGE_BYTES) {
					return json(
						{
							error: `That image is too large (~${(bytes / 1024 / 1024).toFixed(1)} MB). Please use an image under 5 MB.`
						},
						{ status: 413 }
					);
				}
			}
		}
	}

	const provider = (env.LLM_PROVIDER || 'anthropic').toLowerCase();

	try {
		const data =
			provider === 'openrouter'
				? await runOpenRouter(messages, SYSTEM_PROMPT)
				: await runAnthropic(messages, SYSTEM_PROMPT);

		// Anonymous research capture of the generated code (never blocks/breaks the
		// response — the helper swallows its own errors and no-ops if unconfigured).
		await saveGeneratedCode(sessionId, {
			code: data.code,
			model: provider === 'openrouter' ? env.OPENROUTER_MODEL : MODEL
		});

		return json(data);
	} catch (/** @type {any} */ err) {
		console.error('[api/chat] request failed:', err?.status ?? '', err?.message ?? err);
		const status = typeof err?.status === 'number' ? err.status : 500;
		const message = err?.error?.error?.message ?? err?.message ?? 'The request failed.';
		return json({ error: message }, { status });
	}
}
