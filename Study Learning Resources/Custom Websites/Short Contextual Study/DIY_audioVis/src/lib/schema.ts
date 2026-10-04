// The questionnaire is data, not markup. Reword a question here without touching
// a component. Handover §7.
//
// Question ids are what tie answers together in the database (the row key is
// participant + day + stage + question_id). Keep an id STABLE once data has been
// collected. Repeating an id across stages/days on purpose — e.g. the
// `confidence_*` questions asked in Pre and again at Exit — lets you measure how
// an answer shifts over the workshop (filter by day/stage in analysis).

export type Question =
	| {
			id: string;
			type: 'scale';
			label: string;
			min: number;
			max: number;
			minLabel: string;
			maxLabel: string;
	  }
	| { id: string; type: 'choice'; label: string; options: { value: string; label: string }[] }
	| { id: string; type: 'text'; label: string; placeholder?: string; hint?: string };

export type Stage = { slug: string; title: string; questions: Question[] };

// Shown beneath the "what you've learnt" box on task stages.
const REASSURE =
	"This isn't a test. You don't need technical terms, and there's no right or wrong answer.";

// ---- question builders ----

/** 1–5 confidence scale. `id` varies so the same theme can be tracked over time. */
function confidence(id: string, label: string): Question {
	return { id, type: 'scale', label, min: 1, max: 5, minLabel: 'Not at all', maxLabel: 'Very' };
}

/** 1–5 "how well did you understand" scale. */
function understood(label: string): Question {
	return {
		id: 'understood',
		type: 'scale',
		label,
		min: 1,
		max: 5,
		minLabel: 'Unconfident',
		maxLabel: 'Confident'
	};
}

/** 1–5 ease scale. */
function ease(label: string): Question {
	return { id: 'ease', type: 'scale', label, min: 1, max: 5, minLabel: 'Hard', maxLabel: 'Easy' };
}

/** Yes / No. */
function yesno(id: string, label: string): Question {
	return {
		id,
		type: 'choice',
		label,
		options: [
			{ value: 'yes', label: 'Yes' },
			{ value: 'no', label: 'No' }
		]
	};
}

function reflection(id: string, label: string, placeholder = '', hint?: string): Question {
	return { id, type: 'text', label, placeholder, hint };
}

// The five recurring confidence questions, asked before and after. Reused so the
// ids stay identical everywhere they appear.
const confArduino = () => confidence('confidence_arduino', 'How confident are you using Arduino?');
const confProcessing = () =>
	confidence('confidence_processing', 'How confident are you using Processing (or similar)?');
const confCircuits = () =>
	confidence('confidence_circuits', 'How confident are you using electronic circuits?');
const confTroubleshootCode = () =>
	confidence('confidence_troubleshoot_code', 'How confident are you troubleshooting code?');
const confTroubleshootCircuits = () =>
	confidence('confidence_troubleshoot_circuits', 'How confident are you troubleshooting circuits?');

/** A task stage — same shape every time, so build them from one template. */
function taskStage(slug: string, title: string): Stage {
	return {
		slug,
		title,
		questions: [
			understood('How well did you understand the concept of this task?'),
			reflection('learnt', "Describe what you've learnt, in your own words.", '', REASSURE)
		]
	};
}

export const workshop: Record<number, Stage[]> = {
	1: [
		{
			slug: 'pre',
			title: 'Before we start',
			questions: [
				yesno('done_physical_computing', 'Have you done any physical computing before?'),
				yesno(
					'done_creative_coding',
					'Have you done any creative coding with p5 or Processing before?'
				),
				confArduino(),
				confProcessing(),
				confCircuits(),
				confTroubleshootCode(),
				confTroubleshootCircuits()
			]
		},
		taskStage('task1', 'Task 1'),
		taskStage('task2', 'Task 2'),
		taskStage('task3', 'Task 3'),
		{
			slug: 'exit',
			title: 'Before you go',
			questions: [
				ease('How easy did you find using the shield?'),
				reflection('challenges', 'What challenges did you face using the shield?'),
				reflection('worked_well', 'What worked well when using the shield?'),
				reflection('improve', 'What could be improved about the shield?'),
				confArduino(),
				confCircuits(),
				confTroubleshootCode(),
				confTroubleshootCircuits()
			]
		}
	],
	2: [
		taskStage('task1', 'Task 1'),
		taskStage('task2', 'Task 2'),
		taskStage('task3', 'Task 3'),
		taskStage('task4', 'Task 4'),
		taskStage('task5', 'Task 5'),
		{
			slug: 'exit',
			title: 'Before you go',
			questions: [
				ease('How easy did you find using the sculpting code website?'),
				reflection('challenges', 'What challenges did you face using the sculpting code website?'),
				reflection('worked_well', 'What worked well when using the sculpting code website?'),
				reflection('improve', 'What could be improved about the sculpting code website?'),
				confArduino(),
				confProcessing(),
				confCircuits(),
				confTroubleshootCode(),
				confTroubleshootCircuits()
			]
		}
	]
};

// ---- helpers used across routing / resume ----

export function stagesFor(day: number): Stage[] {
	return workshop[day] ?? [];
}

export function isValidDay(day: number): boolean {
	return day in workshop;
}

export function stageIndex(day: number, slug: string): number {
	return stagesFor(day).findIndex((s) => s.slug === slug);
}

export function getStage(day: number, slug: string): Stage | undefined {
	return stagesFor(day).find((s) => s.slug === slug);
}
