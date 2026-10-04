<script>
  import { onMount } from "svelte";
  import CodeEditor from "$lib/components/CodeEditor.svelte";
  import Preview from "$lib/components/Preview.svelte";
  import WebcamCapture from "$lib/components/WebcamCapture.svelte";
  import OnboardingModal from "$lib/components/OnboardingModal.svelte";
  import { DRAW_PROMPT } from "$lib/prompts.js";

  // `data.modelLabel` comes from +page.server.js (reflects the active provider).
  let { data } = $props();

  // Invisible proof-of-work bot check. The browser solves a small SHA-256 puzzle
  // in the background to earn the clearance cookie /api/chat requires — no captcha UI, no
  // third party, no interaction. re-run it a few times on failure before
  // surfacing a helpful message.
  let verifyTries = 0;
  const MAX_VERIFY_TRIES = 3;
  let verifying = false;
  const _enc = new TextEncoder();

  /** SHA-256 hex of a string, via the browser's crypto.
   * @param {string} str */
  async function sha256Hex(str) {
    const buf = await crypto.subtle.digest("SHA-256", _enc.encode(str));
    return [...new Uint8Array(buf)]
      .map((b) => b.toString(16).padStart(2, "0"))
      .join("");
  }

  /** Fetch a challenge and brute-force the number that solves it. */
  async function solveChallenge() {
    const res = await fetch("/api/challenge");
    if (!res.ok) return null;
    const { challenge, salt, maxnumber, signature } = await res.json();
    for (let number = 0; number <= maxnumber; number++) {
      if ((await sha256Hex(`${salt}${number}`)) === challenge) {
        return { challenge, number, salt, signature };
      }
    }
    return null;
  }

  /** Solve a challenge and exchange it for the clearance cookie. */
  async function verify() {
    if (verifying) return;
    verifying = true;
    try {
      const solution = await solveChallenge();
      if (!solution) return;
      const res = await fetch("/api/verify", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ solution }),
      });
      if (res.ok) verifyTries = 0; // cleared once we hold a valid pass
    } catch {
      /* ignore — the API will 403 and we re-verify on demand */
    } finally {
      verifying = false;
    }
  }

  // Re-run verification, up to MAX_VERIFY_TRIES. Returns false once we've given
  // up (so the caller can show a helpful message instead of looping).
  function retryVerify() {
    if (verifyTries >= MAX_VERIFY_TRIES) return false;
    verifyTries += 1;
    verify();
    return true;
  }

  // First-run tour: auto-open once, remembered in localStorage.
  const ONBOARD_KEY = "dsc-onboarding-seen";
  let onboardingOpen = $state(false);

  onMount(() => {
    try {
      if (!localStorage.getItem(ONBOARD_KEY)) onboardingOpen = true;
    } catch {
      /* localStorage unavailable — skip auto-open */
    }
    verify();
  });

  function markOnboardingSeen() {
    try {
      localStorage.setItem(ONBOARD_KEY, "1");
    } catch {
      /* ignore */
    }
  }

  const DEFAULT_CODE = `function setup() {
  createCanvas(400, 400);
}

function draw() {
  background('#FEFDFB');
  fill('#F0503C');
  noStroke();
  ellipse(width / 2, height / 2, 200, 200);
}`;

  // Conversation state, kept in Anthropic message format for multi-turn context.
  let history = $state(/** @type {Array<{role: string, content: any}>} */ ([]));

  // The attached image (data URL) and its media type. Sent once, then kept in history.
  let image = $state(/** @type {string | null} */ (null));
  let imageMediaType = $state("image/jpeg");
  let imageSent = $state(false);

  // The model's latest teaching reply (structured).
  let explanation = $state("");
  let concepts = $state(
    /** @type {Array<{concept: string, why: string}>} */ ([]),
  );
  // True once a sketch has been generated (gates edit chips + follow-up grounding).
  let hasSketch = $state(false);

  let loading = $state(false);
  let errorMsg = $state("");

  // Bound to the editor; set to the code the model returns.
  let code = $state(DEFAULT_CODE);

  let webcamOpen = $state(false);
  let fileInput = $state(/** @type {HTMLInputElement | null} */ (null));

  // Output panel: which tab is showing, the "run again" nonce, copy feedback.
  let tab = $state(/** @type {'preview' | 'code'} */ ("preview"));
  let runNonce = $state(0);
  let copied = $state(false);

  async function copyCode() {
    try {
      await navigator.clipboard.writeText(code);
      copied = true;
      setTimeout(() => (copied = false), 1500);
    } catch {
      /* clipboard blocked — ignore */
    }
  }

  /** Wipe the conversation but leave the image untouched. */
  function resetSession() {
    history = [];
    imageSent = false;
    explanation = "";
    concepts = [];
    errorMsg = "";
    hasSketch = false;
    code = DEFAULT_CODE;
  }

  /** Full reset back to the empty state. */
  function restart() {
    resetSession();
    image = null;
  }

  /** A new image starts a fresh session. */
  function setImage(
    /** @type {string} */ dataUrl,
    /** @type {string} */ mediaType,
  ) {
    resetSession();
    image = dataUrl;
    imageMediaType = mediaType || "image/jpeg";
  }

  /** @param {Event & { currentTarget: HTMLInputElement }} event */
  function handleUpload(event) {
    const file = event.currentTarget.files?.[0];
    if (!file) return;
    if (
      !["image/jpeg", "image/png", "image/webp", "image/gif"].includes(
        file.type,
      )
    ) {
      errorMsg = `Unsupported file type "${file.type}". Use JPEG, PNG, WebP, or GIF.`;
      event.currentTarget.value = "";
      return;
    }
    const reader = new FileReader();
    reader.onload = () =>
      setImage(/** @type {string} */ (reader.result), file.type);
    reader.readAsDataURL(file);
    event.currentTarget.value = "";
  }

  /** @param {string} text */
  async function handleSubmit(text) {
    const trimmed = text?.trim();
    if (!trimmed || loading) return;
    errorMsg = "";
    explanation = "";
    concepts = [];

    let content;
    if (image && !imageSent) {
      // First turn: attach the image; the model draws from it.
      const base64 = image.split(",")[1] ?? "";
      content = [
        {
          type: "image",
          source: { type: "base64", media_type: imageMediaType, data: base64 },
        },
        { type: "text", text: trimmed },
      ];
      imageSent = true;
    } else if (hasSketch) {
      // Follow-up: edit the code currently in the editor (respects hand-edits).
      content = `${trimmed}\n\nHere is the current sketch to edit — keep the parts I didn't ask you to change:\n\`\`\`js\n${code}\n\`\`\``;
    } else {
      content = trimmed;
    }

    history = [...history, { role: "user", content }];
    loading = true;

    try {
      const res = await fetch("/api/chat", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({
          messages: history,
          sessionId: getSessionId(),
        }),
      });

      const data = await res.json().catch(() => ({}));
      if (res.status === 403 && data.error === "verification-required") {
        errorMsg = retryVerify()
          ? "Just verifying you’re human — give it a second and try again."
          : "We couldn’t verify your browser automatically. Strict privacy settings or private browsing can block this — try turning off content blockers for this site, or open it in a normal window.";
        return;
      }
      if (!res.ok || data.error) {
        errorMsg = data.error || `Request failed (${res.status}).`;
        return;
      }

      explanation = data.explanation || "";
      concepts = Array.isArray(data.concepts) ? data.concepts : [];
      if (data.code) code = data.code;
      hasSketch = true;
      runNonce += 1; // re-run the preview with the new sketch

      // Keep history truthful: store what the model produced.
      history = [
        ...history,
        { role: "assistant", content: "```js\n" + (data.code || "") + "\n```" },
      ];
    } catch (/** @type {any} */ err) {
      errorMsg = err?.message || "Something went wrong talking to the API.";
    } finally {
      loading = false;
    }
  }

  // Per-tab research session id (new tab = new id). Never contains personal info.
  function getSessionId() {
    try {
      let id = sessionStorage.getItem("dsc-session-id");
      if (!id) {
        id = crypto.randomUUID();
        sessionStorage.setItem("dsc-session-id", id);
      }
      return id;
    } catch {
      return null;
    }
  }
</script>

<div class="sc-page">
  <div class="sc-screen">
    <div class="sc-topbar">
      <h1 class="sc-title">Sculpting<br />code<span class="sc-dot">.</span></h1>
      <button class="sc-nav-link" onclick={() => (onboardingOpen = true)}
        >how it works <span class="sc-nav-arrow"> →</span></button
      >
      <div class="sc-blob sc-blob--mobile" aria-hidden="true"></div>
    </div>

    <!-- full width, part of the header block  -->
    <div class="sc-intro-row">
      <p class="sc-lead">
        Draw or sculpt a shape, capture or upload a photo of it. Learn the
        fundamentals of P5JS through making changes to your shape. Start simple
        and add to your drawings or sculpture.
      </p>
      <div class="sc-blob sc-blob--tablet" aria-hidden="true"></div>
    </div>

    <!-- standard 3-column grid, well below the lead text: blob flexes,
         controls/panel stay fixed-width. CSS Grid guarantees this never
         overflows down to the breakpoint. -->
    <div class="sc-body-grid">
      <!-- decorative morphing blob (CSS border-radius keyframes — the
           browser interpolates this natively and smoothly) -->
      <div class="sc-blob" aria-hidden="true"></div>

      <!-- MIDDLE: capture / draw / functions-used -->
      <div class="sc-controls">
        <div class="sc-controls__row">
          <button
            class="sc-btn sc-btn--capture"
            onclick={() => (webcamOpen = true)}
            disabled={loading}>+ capture()</button
          >
          <button
            class="sc-btn"
            onclick={() => fileInput?.click()}
            disabled={loading}>+ upload()</button
          >
        </div>
        <input
          bind:this={fileInput}
          type="file"
          accept="image/png,image/jpeg,image/webp,image/gif"
          hidden
          onchange={handleUpload}
        />

        <button
          class="sc-btn--cta"
          onclick={() => handleSubmit(DRAW_PROMPT)}
          disabled={loading || !image}
        >
          {loading ? "drawing…" : "Draw my shape"}
        </button>

        <section
          class="sc-functions"
          aria-label="Functions used in the generated code"
        >
          <div class="sc-functions__head">
            <span
              >functions used <span class="dim">/ {concepts.length}</span></span
            >
            <span class="sc-input-thumb" title="captured input thumbnail">
              {#if image}<img src={image} alt="Your shape" />{:else}<span
                  class="ph"
                ></span>{/if}
            </span>
          </div>
          <div class="sc-functions__list">
            {#if errorMsg}
              <p class="sc-functions__error">{errorMsg}</p>
            {:else if concepts.length}
              {#each concepts as c, i (i)}
                <div class="sc-functions__item">
                  <div class="sc-functions__name">{c.concept}</div>
                  <div class="sc-functions__desc">{c.why}</div>
                </div>
              {/each}
            {:else}
              <p class="sc-functions__hint">
                {image
                  ? "// hit Draw my shape to generate your code"
                  : "// capture or upload a shape to begin"}
              </p>
            {/if}
          </div>
        </section>
      </div>

      <!-- RIGHT: Preview / Code panel -->
      <div class="sc-panel" data-active={tab}>
        <div class="sc-panel__head">
          <div
            class="sc-tabs"
            role="tablist"
            aria-label="Canvas preview or source code"
          >
            <button
              class="sc-tab"
              role="tab"
              aria-selected={tab === "preview"}
              onclick={() => (tab = "preview")}>Preview</button
            >
            <button
              class="sc-tab"
              role="tab"
              aria-selected={tab === "code"}
              onclick={() => (tab = "code")}>Code</button
            >
          </div>
          {#if tab === "code"}
            <button class="sc-panel__action" onclick={copyCode}
              >{copied ? "copied ✓" : "⧉ copy code"}</button
            >
          {:else}
            <button class="sc-panel__action" onclick={() => (runNonce += 1)}
              >↻ run again</button
            >
          {/if}
        </div>

        <div class="sc-panel__body">
          <div class="sc-body--preview sc-stage">
            <Preview {code} nonce={runNonce} />
          </div>
          <div class="sc-body--code sc-codewrap">
            <CodeEditor bind:code />
          </div>
        </div>
      </div>
    </div>
  </div>
</div>

<WebcamCapture
  bind:open={webcamOpen}
  oncapture={(/** @type {string} */ dataUrl) => setImage(dataUrl, "image/jpeg")}
/>

<OnboardingModal
  bind:open={onboardingOpen}
  onFinish={markOnboardingSeen}
  onSkip={markOnboardingSeen}
/>
