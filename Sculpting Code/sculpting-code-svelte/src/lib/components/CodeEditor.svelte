<script>
	import { onMount, onDestroy } from 'svelte';
	import { EditorView, basicSetup } from 'codemirror';
	import { javascript } from '@codemirror/lang-javascript';

	// `code` (p5.js) is editable and drives the preview.
	let { code = $bindable('') } = $props();

	let editorEl = $state(/** @type {HTMLDivElement | null} */ (null));
	/** @type {EditorView | undefined} */
	let view;
	let applyingExternal = false;

	onMount(() => {
		if (!editorEl) return;
		view = new EditorView({
			doc: code,
			parent: editorEl,
			extensions: [
				basicSetup,
				javascript(),
				EditorView.updateListener.of((update) => {
					if (update.docChanged && !applyingExternal) {
						code = update.state.doc.toString();
					}
				})
			]
		});
	});

	onDestroy(() => view?.destroy());

	// Keep the editor in sync when code changes from outside (a new generation).
	$effect(() => {
		const text = code ?? '';
		if (view && text !== view.state.doc.toString()) {
			applyingExternal = true;
			view.dispatch({ changes: { from: 0, to: view.state.doc.length, insert: text } });
			applyingExternal = false;
		}
	});
</script>

<div class="sc-editor" bind:this={editorEl}></div>
