<script lang="ts">
	import '../app.css';

	let { children } = $props();
</script>

<!-- Minimal root shell. Survey chrome + identity live in (survey)/+layout.svelte;
     worksheets and the hub are plain public pages. -->
{@render children()}
