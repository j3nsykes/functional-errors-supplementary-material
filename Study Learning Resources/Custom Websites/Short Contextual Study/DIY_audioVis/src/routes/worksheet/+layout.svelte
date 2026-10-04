<script lang="ts">
	let { children } = $props();
</script>

<div class="wshell">
	<nav class="wnav">
		<a class="home" href="/">Workshop home</a>
	</nav>
	<main class="wmain">
		{@render children()}
	</main>
</div>

<style>
	.wshell {
		min-height: 100dvh;
		display: flex;
		flex-direction: column;
	}
	.wnav {
		width: 100%;
		max-width: var(--maxw);
		margin: 0 auto;
		padding: 1.25rem 1.25rem 0;
	}
	.home {
		font-family: var(--mono);
		font-size: 0.72rem;
		letter-spacing: 0.14em;
		text-transform: uppercase;
		color: var(--graphite);
		text-decoration: none;
	}
	.home::before {
		content: '← ';
	}
	.home:hover {
		color: var(--ink);
	}
	.wmain {
		flex: 1 1 auto;
		width: 100%;
		max-width: var(--maxw);
		margin: 0 auto;
		padding: 1.5rem 1.25rem 4rem;
	}
</style>
