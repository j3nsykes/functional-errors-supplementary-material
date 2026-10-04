<script>
  // Renders the p5.js sketch in a sandboxed iframe. `code` comes from the editor;
  // the tab's "run again" button forces a re-render even when
  // the code is unchanged.
  let { code = "", nonce = 0 } = $props();

  let iframeEl = $state(/** @type {HTMLIFrameElement | null} */ (null));

  $effect(() => {
    if (!iframeEl) return;
    const run = nonce; // referenced so the effect re-runs on "run again"
    iframeEl.srcdoc = `<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8" />
	<meta name="viewport" content="width=device-width, initial-scale=1.0" />
	<!-- run ${run} -->
	<script src="https://cdnjs.cloudflare.com/ajax/libs/p5.js/1.9.4/p5.min.js"><\/script>
	<style>html,body{margin:0;display:flex;align-items:center;justify-content:center;height:100%;background:#fff}</style>
	<script>${code ?? ""}<\/script>
</head>
<body></body>
</html>`;
  });
</script>

<div class="sc-canvas">
  <iframe bind:this={iframeEl} title="p5.js preview" sandbox="allow-scripts"
  ></iframe>
</div>
<!-- .sc-stage wrapper is provided by the page so this stays a pure canvas -->
