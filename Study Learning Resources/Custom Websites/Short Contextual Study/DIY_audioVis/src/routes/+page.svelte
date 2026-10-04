<script lang="ts">
	import { workshop, stagesFor } from '$lib/schema';

	const days = Object.keys(workshop)
		.map(Number)
		.sort((a, b) => a - b);
</script>

<svelte:head>
	<title>DIY Audio Visual Controllers</title>
</svelte:head>

<main class="hub">
	<p class="kicker">Two-day workshop</p>
	<h1 class="title">DIY Audio Visual Controllers</h1>
	<p class="lede">Worksheets and a short feedback form for each day.</p>

	<ul class="days">
		{#each days as day (day)}
			<li class="day">
				<span class="daynum">Day {day}</span>
				<div class="links">
					<a class="card" href={`/worksheet/${day}`}>
						<span class="card-label">Worksheet</span>
						<span class="card-sub">{stagesFor(day).length} stages · instructions &amp; code</span>
					</a>
					<a class="card" href={`/survey/${day}`}>
						<span class="card-label">Feedback</span>
						<span class="card-sub">Fill in after each activity</span>
					</a>
				</div>
			</li>
		{/each}
	</ul>

	<section class="notice">
		<h2 class="notice-q">
			What will happen to the results of the feedback you provide and the data collected?
		</h2>
		<p class="notice-body">
			Analysis from the digital feedback forms, including quotations from you, will contribute to an
			ongoing research evaluation around creative coding education tools. Your answers may also be
			used in academic reports, papers or conference presentations. These may appear online. However,
			your answers will remain entirely anonymous. When inputting feedback via the online webpages,
			you may see a randomly generated ID at the bottom of the page. It is important to note these IDs
			are encrypted before being stored alongside your data to maintain anonymity. You are welcome to
			opt out of this process, in which case you should not complete the feedback form element of the
			day. Thank you.
		</p>
	</section>
</main>

<style>
	.hub {
		width: 100%;
		max-width: var(--maxw);
		margin: 0 auto;
		padding: 4rem 1.25rem 4rem;
	}
	.kicker {
		margin: 0 0 0.6rem;
		font-family: var(--mono);
		font-size: 0.72rem;
		letter-spacing: 0.18em;
		text-transform: uppercase;
		color: var(--graphite);
	}
	.title {
		font-family: var(--serif);
		font-weight: 500;
		font-size: clamp(2.5rem, 9vw, 3.25rem);
		letter-spacing: -0.03em;
		line-height: 1;
		margin: 0 0 1rem;
		color: var(--ink);
	}
	.lede {
		margin: 0 0 3rem;
		font-size: 1.1rem;
		color: var(--ink);
		max-width: 30rem;
	}
	.days {
		list-style: none;
		margin: 0;
		padding: 0;
		display: flex;
		flex-direction: column;
		gap: 2.5rem;
	}
	.daynum {
		display: block;
		margin-bottom: 0.85rem;
		font-family: var(--mono);
		font-size: 0.75rem;
		letter-spacing: 0.16em;
		text-transform: uppercase;
		color: var(--graphite);
	}
	.links {
		display: grid;
		grid-template-columns: 1fr 1fr;
		gap: 0.85rem;
	}
	.card {
		display: flex;
		flex-direction: column;
		gap: 0.35rem;
		padding: 1.1rem 1.2rem;
		border: 1px solid var(--line);
		border-radius: var(--radius);
		text-decoration: none;
		transition:
			border-color 140ms ease,
			background-color 140ms ease,
			transform 140ms ease;
	}
	.card:hover {
		border-color: var(--ink);
		transform: translateY(-2px);
	}
	.card-label {
		font-size: 1.05rem;
		font-weight: 600;
		color: var(--ink);
	}
	.card-sub {
		font-size: 0.82rem;
		color: var(--graphite);
	}

	.notice {
		margin-top: 8rem;
		padding-top: 2rem;
		border-top: 1px solid var(--line);
		max-width: 34rem;
	}
	.notice-q {
		margin: 0 0 0.6rem;
		font-size: 0.9rem;
		font-weight: 600;
		line-height: 1.4;
		color: var(--graphite);
	}
	.notice-body {
		margin: 0;
		font-size: 0.85rem;
		line-height: 1.6;
		color: var(--graphite);
	}

	@media (max-width: 30rem) {
		.links {
			grid-template-columns: 1fr;
		}
	}
</style>
