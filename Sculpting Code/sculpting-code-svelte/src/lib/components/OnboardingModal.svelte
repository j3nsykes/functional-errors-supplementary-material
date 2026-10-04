<script>
  /**
   * OnboardingModal — a centered, animated onboarding tour.
   * Svelte 4 / Svelte 5 (legacy) compatible. Self-contained styles.
   *
   * Props:
   *   open      {boolean}   whether the modal is shown (bindable)
   *   startStep {number}    index to start on (default 0)
   *   onFinish  {function}  called when "get started" is clicked
   *   onSkip    {function}  called when skipped / dismissed
   *
   * localStorage "seen" tracking is intentionally left to the parent.
   */
  import { untrack } from 'svelte';

  let {
    open = $bindable(true),
    startStep = 0,
    onFinish = () => {},
    onSkip = () => {}
  } = $props();

  let step = $state(untrack(() => startStep));

  // body = array of segments: a plain string, {hl} (accent) or {hlk} (bold ink)
  /** @type {Array<{ id: string; label: string; title: string; body: any[] }>} */
  const steps = [
    {
      id: 'welcome',
      label: 'welcome()',
      title: 'Turn your shapes into code',
      body: [
        'New here? Take a 20-second tour: capture a shape, watch it be created in code. Run it, and remix by asking questions.'
      ]
    },
    {
      id: 'capture',
      label: 'step 1 / 5 · capture',
      title: 'Snap or upload your shape',
      body: [
        'Sculpt or draw a simple shape and hit ',
        { hl: 'capture()' },
        ', or upload a photo. Your shape pops into the workspace for you to start coding with.'
      ]
    },
    {
      id: 'generate',
      label: 'step 2 / 5 · generate',
      title: 'Watch it become code',
      body: [
        'Your image is analysed and an LLM generates the code to draw it. Follow exactly how your shape becomes functions like ',
        { hl: 'ellipse()' },
        ' and ',
        { hl: 'fill()' },
        '.'
      ]
    },
    {
      id: 'render',
      label: 'step 3 / 5 · render',
      title: 'See it run on the canvas',
      body: [
        'Your code renders instantly in a preview canvas. Tweak your code, change a number, hit ',
        { hl: 'run again' },
        ', and watch the shape update.'
      ]
    },
    {
      id: 'remix',
      label: 'step 4 / 5 · remix',
      title: 'Remix freely',
      body: [
        'Change your drawing, add a new shape or mould a new one. See your code update and understand what has changed.'
      ]
    },
    {
      id: 'data',
      label: 'step 5 / 5 · data',
      title: 'Your data',
      body: [
        'Only the ',
        { hlk: 'generated code' },
        " is saved. Each output is kept anonymous for research to improve this tool. Your images are never stored, and we don't collect IP addresses or any personal information."
      ]
    }
  ];

  const dotCount = steps.length - 1;
  const current = $derived(steps[step]);
  const isFirst = $derived(step === 0);
  const isLast = $derived(step === steps.length - 1);

  function next() {
    if (isLast) finish();
    else step += 1;
  }
  function back() {
    if (step > 0) step -= 1;
  }
  function skip() {
    open = false;
    onSkip();
  }
  function finish() {
    open = false;
    onFinish();
  }
  /** @param {KeyboardEvent} e */
  function onKey(e) {
    if (!open) return;
    if (e.key === 'Escape') skip();
    else if (e.key === 'ArrowRight') next();
    else if (e.key === 'ArrowLeft') back();
  }
</script>

<svelte:window onkeydown={onKey} />

{#if open}
  <div class="cc-onb" role="dialog" aria-modal="true" aria-label="Getting started tour">
    <button class="cc-onb__scrim" type="button" aria-label="Close tour" onclick={skip}></button>

    <div class="cc-onb__card">
      {#key step}
        <div class="cc-onb__illo">
          {#if current.id === 'welcome'}
            <div class="illo illo-welcome">
              <div class="wtile" style="--i:0"><span class="blob sm"></span></div>
              <span class="arr">→</span>
              <div class="wtile wtile--dark" style="--i:1"><span class="mono code">&lt;/&gt;</span></div>
              <span class="arr">→</span>
              <div class="wtile" style="--i:2"><span class="rundot"></span><span class="blob sm"></span></div>
              <svg class="spark" style="left:120px;top:44px;width:20px;height:20px" viewBox="0 0 24 24"><path d="M12 0 L14 10 L24 12 L14 14 L12 24 L10 14 L0 12 L10 10 Z" fill="var(--cc-color-primary,#F0503C)"/></svg>
              <svg class="spark" style="right:132px;bottom:40px;width:15px;height:15px;animation-delay:.6s" viewBox="0 0 24 24"><path d="M12 0 L14 10 L24 12 L14 14 L12 24 L10 14 L0 12 L10 10 Z" fill="#3B3AD6"/></svg>
            </div>

          {:else if current.id === 'capture'}
            <div class="illo illo-capture">
              <span class="tag-live"><span class="dot dot--live"></span>capturing…</span>
              <span class="ring ring--2"></span>
              <span class="ring ring--1"></span>
              <span class="blob blob--pop"></span>
              <svg class="spark" style="left:170px;top:52px;width:22px;height:22px" viewBox="0 0 24 24"><path d="M12 0 L14 10 L24 12 L14 14 L12 24 L10 14 L0 12 L10 10 Z" fill="var(--cc-color-primary,#F0503C)"/></svg>
              <svg class="spark" style="right:150px;top:70px;width:16px;height:16px;animation-delay:.5s" viewBox="0 0 24 24"><path d="M12 0 L14 10 L24 12 L14 14 L12 24 L10 14 L0 12 L10 10 Z" fill="#3B3AD6"/></svg>
              <svg class="spark" style="right:176px;bottom:44px;width:18px;height:18px;animation-delay:1s" viewBox="0 0 24 24"><path d="M12 0 L14 10 L24 12 L14 14 L12 24 L10 14 L0 12 L10 10 Z" fill="var(--cc-color-primary,#F0503C)"/></svg>
              <span class="guide guide--bl"></span>
              <span class="guide guide--tr"></span>
            </div>

          {:else if current.id === 'generate'}
            <div class="illo illo-generate">
              <div class="codecard">
                <div class="codecard__head">
                  <span class="mono">sketch.js</span>
                  <span class="gen">generating<span class="gd"></span><span class="gd"></span><span class="gd"></span></span>
                </div>
                <div class="codecard__lines mono">
                  <div class="ln"><span class="type t0"><span class="kw">function</span> <span class="fn">draw</span>() &lbrace;</span></div>
                  <div class="ln ln--indent"><span class="type t1">fill(<span class="lit">'#F0503C'</span>);</span></div>
                  <div class="ln ln--indent"><span class="type t2">ellipse(<span class="lit">200</span>, <span class="lit">200</span>,</span><span class="caret"></span></div>
                </div>
              </div>
              <svg class="spark" style="left:74px;top:44px;width:20px;height:20px" viewBox="0 0 24 24"><path d="M12 0 L14 10 L24 12 L14 14 L12 24 L10 14 L0 12 L10 10 Z" fill="var(--cc-color-primary,#F0503C)"/></svg>
            </div>

          {:else if current.id === 'render'}
            <div class="illo illo-render">
              <span class="tag-live tag-live--ok"><span class="dot dot--ok"></span>running</span>
              <span class="pulse pulse--1"></span>
              <span class="pulse pulse--2"></span>
              <span class="canvas"><span class="blob"></span></span>
              <svg class="spark" style="right:96px;top:52px;width:16px;height:16px" viewBox="0 0 24 24"><path d="M12 0 L14 10 L24 12 L14 14 L12 24 L10 14 L0 12 L10 10 Z" fill="#3B3AD6"/></svg>
            </div>

          {:else if current.id === 'remix'}
            <div class="illo illo-remix">
              <div class="scaler">
                <span class="blob blob--sm"></span>
                <span class="arr">→</span>
                <span class="rgroup">
                  <span class="blob blob--grow"></span>
                  <span class="blob blob--add"></span>
                </span>
              </div>
              <svg class="spark" style="right:120px;top:58px;width:18px;height:18px" viewBox="0 0 24 24"><path d="M12 0 L14 10 L24 12 L14 14 L12 24 L10 14 L0 12 L10 10 Z" fill="#3B3AD6"/></svg>
              <svg class="spark" style="left:128px;bottom:52px;width:15px;height:15px;animation-delay:.6s" viewBox="0 0 24 24"><path d="M12 0 L14 10 L24 12 L14 14 L12 24 L10 14 L0 12 L10 10 Z" fill="var(--cc-color-primary,#F0503C)"/></svg>
            </div>

          {:else if current.id === 'data'}
            <div class="illo illo-data">
              <div class="panel">
                <div class="panel__head mono">🔒 what's saved</div>
                <div class="panel__rows mono">
                  <div class="row" style="--i:0"><span class="ic ic--ok">✓</span><span class="lbl">generated code</span><span class="rtag rtag--ok">saved</span></div>
                  <div class="row" style="--i:1"><span class="ic ic--no">✕</span><span class="lbl">your images</span><span class="rtag rtag--no">never stored</span></div>
                  <div class="row" style="--i:2"><span class="ic ic--no">✕</span><span class="lbl">IP address &amp; personal info</span><span class="rtag rtag--no">never collected</span></div>
                </div>
              </div>
            </div>
          {/if}
        </div>
      {/key}

      <div class="cc-onb__body">
        <span class="cc-onb__label">{current.label}</span>
        <h2 class="cc-onb__title">{current.title}</h2>
        <p class="cc-onb__text">
          {#each current.body as seg, i (i)}
            {#if typeof seg === 'string'}{seg}{:else if seg.hl}<span class="hl">{seg.hl}</span>{:else}<span class="hlk">{seg.hlk}</span>{/if}
          {/each}
        </p>
      </div>

      <div class="cc-onb__foot">
        {#if isFirst}
          <span class="cc-onb__count">{dotCount} quick steps</span>
          <div class="cc-onb__actions">
            <button class="cc-onb__link" type="button" onclick={skip}>skip</button>
            <button class="cc-onb__btn cc-onb__btn--primary" type="button" onclick={next}>start tour →</button>
          </div>
        {:else}
          <div class="cc-onb__dots" aria-hidden="true">
            {#each Array(dotCount) as _, i (i)}
              <span class="cc-onb__dot" class:on={i === step - 1}></span>
            {/each}
          </div>
          <div class="cc-onb__actions">
            <button class="cc-onb__link" type="button" onclick={back}>back</button>
            {#if isLast}
              <button class="cc-onb__btn cc-onb__btn--primary" type="button" onclick={finish}>get started ✓</button>
            {:else}
              <button class="cc-onb__btn cc-onb__btn--ink" type="button" onclick={next}>next →</button>
            {/if}
          </div>
        {/if}
      </div>
    </div>
  </div>
{/if}

<style>
  /* ---- Shell ------------------------------------------------- */
  .cc-onb {
    position: fixed;
    inset: 0;
    z-index: 1000;
    display: flex;
    align-items: center;
    justify-content: center;
    padding: 20px;
    font-family: var(--cc-font-mono, "JetBrains Mono", ui-monospace, monospace);
  }
  .cc-onb__scrim {
    position: absolute;
    inset: 0;
    border: 0;
    padding: 0;
    cursor: pointer;
    background: rgba(17, 16, 23, 0.62);
    animation: cc-fade 0.2s ease both;
  }
  .cc-onb__card {
    position: relative;
    width: min(580px, 100%);
    background: var(--cc-color-surface, #ffffff);
    border: 1.5px solid var(--cc-color-ink, #20201c);
    animation: cc-card-in 0.3s cubic-bezier(0.2, 0.7, 0.3, 1) both;
  }

  /* ---- Illustration frame ------------------------------------ */
  .cc-onb__illo {
    position: relative;
    height: 236px;
    background: var(--cc-color-screen, #f3f3f5);
    border-bottom: 1.5px solid var(--cc-color-ink, #20201c);
    overflow: hidden;
  }
  .illo {
    position: absolute;
    inset: 0;
    display: flex;
    align-items: center;
    justify-content: center;
    gap: 16px;
  }
  .mono { font-family: var(--cc-font-mono, "JetBrains Mono", ui-monospace, monospace); }
  .blob {
    border-radius: 48% 52% 44% 56% / 55% 48% 52% 45%;
    background: var(--cc-color-primary, #f0503c);
    width: 104px;
    height: 98px;
  }
  .blob.sm { width: 52px; height: 49px; }
  .spark { position: absolute; animation: cc-twinkle 2s ease-in-out infinite; }

  /* ---- Body -------------------------------------------------- */
  .cc-onb__body {
    padding: 22px 26px 18px;
    display: flex;
    flex-direction: column;
    gap: 10px;
  }
  .cc-onb__label {
    font-weight: 700;
    font-size: 12px;
    letter-spacing: 0.04em;
    color: var(--cc-color-primary, #f0503c);
  }
  .cc-onb__title {
    margin: 0;
    font-family: var(--cc-font-display, "Space Mono", ui-monospace, monospace);
    font-weight: 700;
    font-size: 22px;
    line-height: 26px;
    color: var(--cc-color-ink, #20201c);
  }
  .cc-onb__text {
    margin: 0;
    font-size: 13.5px;
    line-height: 21px;
    color: var(--cc-color-text-muted, #5a554b);
  }
  .cc-onb__text .hl { color: var(--cc-color-primary, #f0503c); }
  .cc-onb__text .hlk { color: var(--cc-color-ink, #20201c); font-weight: 700; }

  /* ---- Footer ------------------------------------------------ */
  .cc-onb__foot {
    display: flex;
    align-items: center;
    justify-content: space-between;
    padding: 16px 26px;
    border-top: 1.5px solid var(--cc-color-ink, #20201c);
  }
  .cc-onb__count { font-size: 12px; color: var(--cc-color-text-faint, #8a8474); }
  .cc-onb__dots { display: flex; align-items: center; gap: 8px; }
  .cc-onb__dot {
    width: 6px;
    height: 6px;
    background: var(--cc-color-border-input, #d8d6dc);
    transition: width 0.2s ease, background 0.2s ease;
  }
  .cc-onb__dot.on { width: 22px; background: var(--cc-color-primary, #f0503c); }
  .cc-onb__actions { display: flex; align-items: center; gap: 12px; }
  .cc-onb__link {
    font-family: inherit;
    font-weight: 500;
    font-size: 13px;
    color: var(--cc-color-text-faint, #8a8474);
    background: none;
    border: 0;
    padding: 10px 8px;
    cursor: pointer;
  }
  .cc-onb__btn {
    font-family: inherit;
    font-weight: 700;
    font-size: 13.5px;
    padding: 11px 22px;
    border: 1.5px solid var(--cc-color-ink, #20201c);
    cursor: pointer;
  }
  .cc-onb__btn--primary { background: var(--cc-color-primary, #f0503c); color: #fff; }
  .cc-onb__btn--ink { background: var(--cc-color-ink, #20201c); color: #fff; }

  /* ---- Welcome ----------------------------------------------- */
  .illo-welcome .wtile {
    position: relative;
    width: 96px;
    height: 96px;
    background: #fff;
    border: 1.5px solid var(--cc-color-ink, #20201c);
    display: flex;
    align-items: center;
    justify-content: center;
    opacity: 0;
    animation: cc-pop-up 0.45s cubic-bezier(0.2, 0.8, 0.3, 1) forwards;
    animation-delay: calc(var(--i) * 0.12s);
  }
  .illo-welcome .wtile--dark { background: var(--cc-color-ink, #20201c); }
  .illo-welcome .code { font-weight: 700; font-size: 22px; color: var(--cc-color-primary, #f0503c); }
  .illo-welcome .rundot {
    position: absolute; left: 8px; top: 6px;
    width: 6px; height: 6px; border-radius: 50%;
    background: var(--cc-color-success, #3b8a5e);
  }
  .illo-welcome .arr {
    font-family: var(--cc-font-display, "Space Mono", monospace);
    font-weight: 700; font-size: 20px;
    color: var(--cc-color-text-faint, #8a8474);
    opacity: 0;
    animation: cc-fade 0.4s ease 0.3s forwards;
  }

  /* ---- Capture ----------------------------------------------- */
  .illo-capture .tag-live {
    position: absolute; left: 18px; top: 16px;
    display: flex; align-items: center; gap: 7px;
    font-weight: 700; font-size: 11px;
    color: var(--cc-color-primary, #f0503c);
  }
  .dot { width: 8px; height: 8px; border-radius: 50%; }
  .dot--live { background: var(--cc-color-primary, #f0503c); animation: cc-blink 1.4s steps(1) infinite; }
  .dot--ok { background: var(--cc-color-success, #3b8a5e); animation: cc-blink 1.4s steps(1) infinite; }
  .illo-capture .ring { position: absolute; border-radius: 50%; }
  .illo-capture .ring--1 {
    width: 186px; height: 186px;
    border: 2px dashed var(--cc-color-primary, #f0503c);
    opacity: 0.45;
    animation: cc-spin 9s linear infinite;
  }
  .illo-capture .ring--2 {
    width: 230px; height: 230px;
    border: 1.5px dashed #c4c3ee;
    opacity: 0.5;
    animation: cc-breathe 2.6s ease-in-out infinite;
  }
  .illo-capture .blob--pop { animation: cc-pop-in 0.5s cubic-bezier(0.2, 1.4, 0.4, 1) both; }
  .guide { position: absolute; width: 26px; height: 26px; }
  .guide--bl { left: 16px; bottom: 16px; border-left: 2px solid #b8b2a6; border-bottom: 2px solid #b8b2a6; }
  .guide--tr { right: 16px; top: 44px; border-right: 2px solid #b8b2a6; border-top: 2px solid #b8b2a6; }

  /* ---- Generate ---------------------------------------------- */
  .illo-generate .codecard {
    width: min(432px, 84%);
    background: #fff;
    border: 1.5px solid var(--cc-color-ink, #20201c);
  }
  .codecard__head {
    display: flex; align-items: center; justify-content: space-between;
    padding: 9px 14px; border-bottom: 1.5px solid var(--cc-color-ink, #20201c);
    font-weight: 700; font-size: 12px; color: var(--cc-color-ink, #20201c);
  }
  .gen { display: flex; align-items: center; gap: 5px; font-weight: 400; font-size: 11px; color: #3b3ad6; }
  .gd { width: 4px; height: 4px; border-radius: 50%; background: #3b3ad6; animation: cc-gdot 1.2s infinite; }
  .gd:nth-child(2) { animation-delay: 0.2s; }
  .gd:nth-child(3) { animation-delay: 0.4s; }
  .codecard__lines { padding: 12px 16px; font-size: 12.5px; line-height: 22px; color: var(--cc-color-ink, #20201c); }
  .ln--indent { padding-left: 18px; }
  .type {
    display: inline-block;
    white-space: nowrap;
    clip-path: inset(0 100% 0 0);
    animation: cc-type 0.55s steps(20) both;
  }
  .t0 { animation-delay: 0.15s; }
  .t1 { animation-delay: 0.7s; }
  .t2 { animation-delay: 1.25s; }
  .kw { color: #3b3ad6; }
  .fn { color: var(--cc-color-primary, #f0503c); }
  .lit { color: #b0714a; }
  .caret {
    display: inline-block; width: 8px; height: 15px;
    background: var(--cc-color-primary, #f0503c);
    margin-left: 2px; vertical-align: -2px;
    opacity: 0;
    animation: cc-blink 1s steps(1) 1.7s infinite;
  }

  /* ---- Render ------------------------------------------------ */
  .illo-render .tag-live { position: absolute; left: 18px; top: 16px; display: flex; align-items: center; gap: 7px; font-weight: 700; font-size: 11px; }
  .illo-render .tag-live--ok { color: var(--cc-color-success, #3b8a5e); }
  .illo-render .canvas {
    width: 170px; height: 170px; background: #fff;
    border: 1.5px solid var(--cc-color-ink, #20201c);
    display: flex; align-items: center; justify-content: center;
  }
  .illo-render .pulse {
    position: absolute; width: 170px; height: 170px; border-radius: 50%;
    border: 1.5px solid var(--cc-color-success, #3b8a5e);
    opacity: 0;
    animation: cc-pulse 2.4s ease-out infinite;
  }
  .illo-render .pulse--2 { animation-delay: 1.2s; }

  /* ---- Remix ------------------------------------------------- */
  .illo-remix .scaler { display: flex; align-items: center; gap: 20px; }
  .illo-remix .arr { font-family: var(--cc-font-display, "Space Mono", monospace); font-weight: 700; font-size: 20px; color: var(--cc-color-text-faint, #8a8474); }
  .illo-remix .blob--sm { width: 44px; height: 42px; }
  .illo-remix .rgroup { position: relative; display: inline-flex; align-items: flex-end; gap: 10px; }
  .illo-remix .blob--grow { width: 84px; height: 80px; transform-origin: center; animation: cc-grow 2.4s ease-in-out infinite; }
  /* a second shape popping in = "add a new shape"; indigo so it reads as new */
  .illo-remix .blob--add { width: 40px; height: 38px; background: #3b3ad6; animation: cc-pop-in 0.5s cubic-bezier(0.2, 1.4, 0.4, 1) 0.35s both; }

  /* ---- Data -------------------------------------------------- */
  .illo-data .panel { width: min(428px, 86%); background: #fff; border: 1.5px solid var(--cc-color-ink, #20201c); }
  .illo-data .panel__head { padding: 9px 14px; border-bottom: 1.5px solid var(--cc-color-ink, #20201c); font-weight: 700; font-size: 12px; white-space: nowrap; color: var(--cc-color-ink, #20201c); }
  .illo-data .panel__rows { padding: 6px 14px; }
  .illo-data .row {
    display: flex; align-items: center; gap: 12px; padding: 9px 0;
    opacity: 0;
    animation: cc-row-in 0.4s ease forwards;
    animation-delay: calc(var(--i) * 0.14s + 0.1s);
  }
  .illo-data .row + .row { border-top: 1px solid #ede8e0; }
  .illo-data .ic { width: 24px; height: 24px; flex-shrink: 0; display: flex; align-items: center; justify-content: center; font-weight: 700; font-size: 12px; }
  .illo-data .ic--ok { background: var(--cc-color-success, #3b8a5e); color: #fff; }
  .illo-data .ic--no { background: #fff; border: 1.5px solid var(--cc-color-border-input, #d8d6dc); color: #a69e8c; }
  .illo-data .lbl { flex: 1; font-size: 13px; color: var(--cc-color-ink, #20201c); }
  .illo-data .rtag { font-weight: 700; font-size: 11px; }
  .illo-data .rtag--ok { color: var(--cc-color-success, #3b8a5e); }
  .illo-data .rtag--no { color: var(--cc-color-text-faint, #8a8474); }

  /* ---- Keyframes --------------------------------------------- */
  @keyframes cc-fade { from { opacity: 0; } to { opacity: 1; } }
  @keyframes cc-card-in { from { opacity: 0; transform: translateY(12px) scale(0.98); } to { opacity: 1; transform: none; } }
  @keyframes cc-pop-up { from { opacity: 0; transform: translateY(10px) scale(0.9); } to { opacity: 1; transform: none; } }
  @keyframes cc-pop-in { 0% { transform: scale(0); opacity: 0; } 60% { transform: scale(1.08); } 100% { transform: scale(1); opacity: 1; } }
  @keyframes cc-spin { to { transform: rotate(360deg); } }
  @keyframes cc-breathe { 0%, 100% { transform: scale(1); opacity: 0.5; } 50% { transform: scale(1.06); opacity: 0.25; } }
  @keyframes cc-twinkle { 0%, 100% { transform: scale(0.6); opacity: 0.3; } 50% { transform: scale(1); opacity: 1; } }
  @keyframes cc-type { to { clip-path: inset(0 0 0 0); } }
  @keyframes cc-blink { 0%, 50% { opacity: 1; } 50.01%, 100% { opacity: 0; } }
  @keyframes cc-gdot { 0%, 60%, 100% { opacity: 0.2; } 30% { opacity: 1; } }
  @keyframes cc-pulse { 0% { transform: scale(1); opacity: 0.4; } 100% { transform: scale(1.7); opacity: 0; } }
  @keyframes cc-grow { 0%, 100% { transform: scale(0.72); } 50% { transform: scale(1); } }
  @keyframes cc-row-in { from { opacity: 0; transform: translateX(-8px); } to { opacity: 1; transform: none; } }

  /* ---- Respect reduced motion -------------------------------- */
  @media (prefers-reduced-motion: reduce) {
    .cc-onb *,
    .cc-onb *::before,
    .cc-onb *::after {
      animation: none !important;
      transition: none !important;
    }
    .cc-onb__card,
    .illo-welcome .wtile,
    .illo-welcome .arr,
    .illo-data .row { opacity: 1 !important; transform: none !important; }
    .type { clip-path: none !important; }
    .caret { opacity: 1 !important; }
  }
</style>
