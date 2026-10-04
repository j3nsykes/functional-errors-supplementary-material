<script lang="ts">
	import { onMount } from 'svelte';

	// Guided explainer over the feedback-shield artwork. One SVG whose viewBox adds
	// a margin around the board (drawn via <image>). Every arrow lives entirely in
	// the white margin and stops at the nearest board edge, so it stays plain black.

	const OX = 450;
	const OY = 280;
	const SW = 1134.39;
	const SH = 966.39;
	const RIGHT = 490;
	const BOTTOM = 290;
	const W = OX + SW + RIGHT;
	const H = OY + SH + BOTTOM;

	const L = OX;
	const Rt = OX + SW;
	const T = OY;
	const B = OY + SH;

	const bx = (x: number) => OX + x;
	const by = (y: number) => OY + y;

	const RED = '#e02424';
	const GREEN = '#16a34a';
	const BLUE = '#2563eb';
	const OFF = '#41454b'; // unlit LED — the real shield leaves LEDs dark with no signal

	// LED artwork x-positions, PIN13 (index 0) … PIN2 (index 11).
	const LEDX = [
		144, 213.76, 282.1, 352.17, 421.96, 491.74, 561.67, 630.15, 700.08, 770.01, 839.8, 909.58
	];
	const topOverlays = LEDX.map((x, i) => {
		let fill = OFF;
		let anim = '';
		if (i === 0) {
			fill = RED;
			anim = 'blink'; // PIN13 blinks red/green
		} else if (i === 4) fill = BLUE; // PIN9 -> PWM blue
		else if (i === 9) fill = GREEN; // PIN4 -> HIGH green
		else if (i === 10) fill = RED; // PIN3 -> LOW red
		return { cx: bx(x), cy: by(45.18), r: 18, fill, anim };
	});
	const pinaOverlays = [778, 850, 921, 991].map((x, i) => ({
		cx: bx(x),
		cy: by(915),
		r: 20,
		fill: i === 0 ? BLUE : OFF,
		anim: i === 0 ? 'fade' : '' // PINA0 -> analog input, fading blue
	}));
	const overlays = [...topOverlays, ...pinaOverlays];

	// Pin screen x-positions used as arrow targets.
	const PIN13 = bx(144);
	const PIN9 = bx(421.96);
	const PIN4 = bx(770.01);
	const PIN3 = bx(839.8);
	const PINA0 = bx(778);
	const SWITCH = bx(360);
	const topTip = T - 6;

	type Line = { t: string; sub?: boolean };
	type Callout = { d: string; delay: number; lx: number; ly: number; lw: number; lines: Line[] };

	// Clockwise from the intro (left), up and around.
	const callouts: Callout[] = [
		{
			// 1. Intro (left edge)
			d: `M 370 690 H ${L - 6}`,
			delay: 0.3,
			lx: 30,
			ly: 611,
			lw: 340,
			lines: [
				{ t: 'The shield helps you' },
				{ t: 'visualise your pins.' },
				{ t: 'Place it on top of', sub: true },
				{ t: 'your Arduino.', sub: true }
			]
		},
		{
			// 2. General (PIN13, top-left)
			d: `M ${PIN13} 144 V ${topTip}`,
			delay: 0.9,
			lx: PIN13 - 125,
			ly: 48,
			lw: 250,
			lines: [{ t: 'Each LED shows' }, { t: "a pin's state" }]
		},
		{
			// 3. PWM (PIN9)
			d: `M ${PIN9} 108 V ${topTip}`,
			delay: 1.5,
			lx: PIN9 - 98,
			ly: 48,
			lw: 196,
			lines: [{ t: 'blue = PWM' }]
		},
		{
			// 4. HIGH green (PIN4) — elbow: out of the label's right, rounded turn down
			d: `M 1184 76 H ${PIN4 - 22} Q ${PIN4} 76 ${PIN4} 98 V ${topTip}`,
			delay: 2.1,
			lx: 980,
			ly: 48,
			lw: 204,
			lines: [{ t: 'green = HIGH' }]
		},
		{
			// 5. LOW red (PIN3) — elbow: out of the label's left, rounded turn down
			d: `M 1332 76 H ${PIN3 + 22} Q ${PIN3} 76 ${PIN3} 98 V ${topTip}`,
			delay: 2.7,
			lx: 1332,
			ly: 48,
			lw: 186,
			lines: [{ t: 'red = LOW' }]
		},
		{
			// 6. Pins (right edge at the header row)
			d: `M 1660 ${by(147)} H ${Rt + 6}`,
			delay: 3.3,
			lx: 1660,
			ly: by(147) - 79,
			lw: 390,
			lines: [
				{ t: 'Use your pins as normal' },
				{ t: 'and wire components in.' },
				{ t: 'These map to your', sub: true },
				{ t: 'Arduino pins below.', sub: true }
			]
		},
		{
			// 7. Analog input (PINA0, bottom)
			d: `M ${PINA0} 1300 V ${B + 6}`,
			delay: 3.9,
			lx: PINA0 - 165,
			ly: 1304,
			lw: 330,
			lines: [{ t: 'blue = analog input' }]
		},
		{
			// 8. Power switch (bottom, elbow up)
			d: `M 690 1330 H ${SWITCH - 22} Q ${SWITCH} 1330 ${SWITCH} 1308 V ${B + 6}`,
			delay: 4.5,
			lx: 210,
			ly: 1296,
			lw: 480,
			lines: [{ t: "Turn the shield ON to 'listen'" }, { t: 'to your Arduino below', sub: true }]
		}
	];

	// Hold the draw-in until the diagram scrolls into view (scroll check, since
	// IntersectionObserver does not fire in the in-app preview pane).
	let figure: HTMLElement;
	let playing = $state(false);

	onMount(() => {
		const check = () => {
			const r = figure.getBoundingClientRect();
			if (r.top < window.innerHeight * 0.82 && r.bottom > 0) {
				playing = true;
				window.removeEventListener('scroll', check);
			}
		};
		check();
		window.addEventListener('scroll', check, { passive: true });
		return () => window.removeEventListener('scroll', check);
	});
</script>

<figure class="shield" bind:this={figure} class:play={playing}>
	<svg
		class="diagram"
		viewBox="0 0 {W} {H}"
		preserveAspectRatio="xMidYMid meet"
		role="group"
		aria-label="Feedback shield explainer: each LED shows a pin's state — red is LOW, green is HIGH, blue is PWM, and blue on the analog pins is an analog input; unlit pins stay dark; place the shield on top of your Arduino; wire components into the pins beside the LEDs; turn the shield on to listen to your Arduino."
	>
		<defs>
			<marker
				id="chevron"
				markerUnits="userSpaceOnUse"
				markerWidth="44"
				markerHeight="44"
				viewBox="0 0 44 44"
				refX="29"
				refY="22"
				orient="auto-start-reverse"
			>
				<path d="M14 10 L29 22 L14 34" fill="none" stroke="#141414" stroke-width="3.2" stroke-linecap="round" stroke-linejoin="round" />
			</marker>
		</defs>

		<image href="/worksheet/shield.svg" x={OX} y={OY} width={SW} height={SH} />

		{#each overlays as o, i (i)}
			<circle class={o.anim} cx={o.cx} cy={o.cy} r={o.r} fill={o.fill} />
		{/each}

		{#each callouts as c, i (i)}
			<path class="ink" d={c.d} pathLength="1" marker-end="url(#chevron)" style="animation-delay:{c.delay}s" />
			<g class="label" style="animation-delay:{c.delay + 1.1}s">
				<rect x={c.lx} y={c.ly} width={c.lw} height={22 + c.lines.length * 34} rx="12" />
				{#each c.lines as line, li (li)}
					<text class={line.sub ? 'sub' : 'title'} x={c.lx + 22} y={c.ly + 36 + li * 34}>{line.t}</text>
				{/each}
			</g>
		{/each}
	</svg>

	<figcaption class="hint">An annotated tour of the shield.</figcaption>
</figure>

<style>
	.shield {
		margin: 1.5rem 0 2rem;
		background: var(--surface);
		border-radius: var(--radius);
	}
	.diagram {
		display: block;
		width: 100%;
		height: auto;
	}

	/* PIN13 blinks LOW/HIGH; PINA0 fades like a varying analog input. */
	.blink {
		animation: blink 1.8s steps(1, end) infinite;
	}
	@keyframes blink {
		0% {
			fill: #e02424;
		}
		50% {
			fill: #16a34a;
		}
	}
	.fade {
		animation: fade-led 1.8s ease-in-out infinite;
	}
	@keyframes fade-led {
		0%,
		100% {
			opacity: 1;
		}
		50% {
			opacity: 0.25;
		}
	}

	.ink {
		fill: none;
		stroke: #141414;
		stroke-width: 3.2;
		stroke-linecap: round;
		stroke-linejoin: round;
		stroke-dasharray: 1;
		stroke-dashoffset: 1;
		animation: draw 1.1s ease-in-out forwards;
		animation-play-state: paused;
	}
	@keyframes draw {
		to {
			stroke-dashoffset: 0;
		}
	}

	.label {
		opacity: 0;
		animation: appear 0.4s ease-out forwards;
		animation-play-state: paused;
	}
	.label rect {
		fill: #fff;
		stroke: #e2e2e2;
		stroke-width: 1.5;
	}
	.label text {
		font-family: 'Inter', system-ui, sans-serif;
	}
	.label .title {
		font-size: 26px;
		font-weight: 600;
		fill: #141414;
	}
	.label .sub {
		font-size: 21px;
		fill: #6a6a6a;
	}
	@keyframes appear {
		to {
			opacity: 1;
		}
	}

	.play .ink,
	.play .label {
		animation-play-state: running;
	}

	.hint {
		margin-top: 0.5rem;
		font-family: 'Inter', system-ui, sans-serif;
		font-size: 0.75rem;
		color: var(--graphite);
		text-align: center;
	}

	@media (prefers-reduced-motion: reduce) {
		.blink {
			animation: none;
			fill: #16a34a;
		}
		.fade {
			animation: none;
		}
		.ink,
		.label {
			animation: none;
			stroke-dashoffset: 0;
			opacity: 1;
		}
	}
</style>
