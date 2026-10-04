<script>
	// Controlled modal: parent sets `open`; emits a JPEG data URL via `oncapture`.
	let { open = $bindable(false), oncapture } = $props();

	let imgSrc = $state(/** @type {string | null} */ (null));
	let errorMsg = $state('');
	let videoEl = $state(/** @type {HTMLVideoElement | null} */ (null));
	/** @type {MediaStream | null} */
	let stream = null;

	async function startCamera() {
		errorMsg = '';
		try {
			stream = await navigator.mediaDevices.getUserMedia({ video: true, audio: false });
			if (videoEl) {
				videoEl.srcObject = stream;
				await videoEl.play();
			}
		} catch (/** @type {any} */ err) {
			errorMsg = 'Could not access the webcam. Please check browser permissions.';
			console.error(err);
		}
	}

	function stopStream() {
		if (stream) {
			for (const track of stream.getTracks()) track.stop();
			stream = null;
		}
	}

	// Start the camera when the modal opens on the live step; stop on close.
	$effect(() => {
		if (open && !imgSrc) {
			startCamera();
		}
		return () => stopStream();
	});

	function capture() {
		if (!videoEl) return;
		const canvas = document.createElement('canvas');
		canvas.width = videoEl.videoWidth || 600;
		canvas.height = videoEl.videoHeight || 600;
		const ctx = canvas.getContext('2d');
		if (!ctx) return;
		ctx.drawImage(videoEl, 0, 0, canvas.width, canvas.height);
		imgSrc = canvas.toDataURL('image/jpeg', 0.9);
		stopStream();
	}

	function retake() {
		imgSrc = null;
		startCamera();
	}

	function accept() {
		if (!imgSrc) return;
		oncapture?.(imgSrc);
		close();
	}

	function close() {
		stopStream();
		imgSrc = null;
		errorMsg = '';
		open = false;
	}
</script>

{#if open}
	<div class="cc-overlay" role="dialog" aria-modal="true">
		<div class="cc-modal-inner">
			<section class="cc-panel cc-card">
				<div class="cc-panel__head">
					<span class="cc-panel__title">
						{#if imgSrc}
							<span class="cc-step cc-step--2">step_02</span>review.frame()
						{:else}
							<span class="cc-step cc-step--1">step_01</span>webcam.stream()
						{/if}
					</span>
					<button class="cc-close" onclick={close} aria-label="Close">✕</button>
				</div>

				<div class="cc-viewport">
					{#if errorMsg}
						<span class="cc-answer cc-answer--error">{errorMsg}</span>
					{:else if imgSrc}
						<span class="cc-badge">✓ captured</span>
						<img class="cc-shot" src={imgSrc} alt="Captured frame" />
					{:else}
						<span class="cc-viewport__tag"><span class="cc-dot cc-dot--primary cc-pulse"></span>rec · live</span>
						<!-- svelte-ignore a11y_media_has_caption -->
						<video bind:this={videoEl} playsinline></video>
						<span class="cc-guide cc-guide--bl"></span>
						<span class="cc-guide cc-guide--tr"></span>
					{/if}
				</div>

				{#if imgSrc}
					<div class="cc-card__foot cc-card__foot--split">
						<button class="cc-btn" onclick={retake}>↺ retake()</button>
						<button class="cc-btn cc-btn--secondary cc-btn--grow" onclick={accept}>use(frame) →</button>
					</div>
				{:else}
					<div class="cc-card__foot cc-card__foot--stack">
						<button class="cc-btn cc-btn--primary" onclick={capture} disabled={!!errorMsg}>
							▸ capture()
						</button>
						<span class="cc-card__tip">// hold the shape still against a plain wall</span>
					</div>
				{/if}
			</section>
		</div>
	</div>
{/if}
