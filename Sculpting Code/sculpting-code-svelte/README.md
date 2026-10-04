# Draw, Sculpt + Code 🎨

A [SvelteKit](https://svelte.dev/docs/kit) app that helps beginners learn [p5.js](https://p5js.org/)
by talking to a **vision-capable Claude model**. Draw or sculpt something, upload a photo (or capture from a
webcam), ask structured questions about it, and Claude returns p5.js code that renders live in an
editor + preview window.

This was ported from an earlier Next.js / React proof-of-concept that used Replicate (Llama 2 / LLaVA).
It now calls the [Anthropic Claude API](https://docs.claude.com/) directly with no Replicate API, and images are
sent straight to Claude as base64 (no third-party upload service).

## Build

- **SvelteKit** + **Svelte 5**
- **CodeMirror 6** editor with a sandboxed `<iframe>` running p5.js from a CDN
- **`@anthropic-ai/sdk`** streaming from a server route (`src/routes/api/chat/+server.js`)
- **Model:** `claude-sonnet-5` (vision) set in `src/lib/config.js`; used by the API route and
  shown in the header. Swap for any other Claude model there.

## Usage

### Remote Access

The live version of the web application can be found at [sculpting-code](https://sculpting-code.vercel.app/)

### Running Locally

Install dependencies:

```bash
npm install
```

Add your [Anthropic API key](https://console.anthropic.com/settings/keys) to a `.env` file
(copy `.env.example`):

```
ANTHROPIC_API_KEY=sk-ant-...
```

Run the development server:

```bash
npm run dev
```

Open the printed local URL in your browser.

## Notes on the update from earlier versions

- **Audio (Salmonn) removed** the original supported audio via Replicate; Claude has no audio model,
  so only image (vision) input is supported.
- **Bytescale upload widget removed** images are read in the browser as base64 and sent to Claude
  directly, so there's no upload API key to manage.
- **Deployment** the project uses `@sveltejs/adapter-auto`, which selects the Vercel adapter
  automatically when deployed to Vercel. Set `ANTHROPIC_API_KEY` as an environment variable in your
  Vercel project settings.

## Project structure

```
src/
  routes/
    +layout.svelte            imports the design-system CSS
    +page.svelte              main workspace (capture/ask + editor/preview)
    api/chat/+server.js       Anthropic streaming endpoint
  lib/
    config.js                 MODEL id (shared by API route + header label)
    prompts.js                system prompt, preset questions, code extractor
    styles/
      tokens.css              design tokens (colour/type/spacing)
      clay-code.css           component styles (from the design system)
      app.css                 app-specific additions (states, editor, iframe, modal)
    components/
      CodeEditor.svelte       CodeMirror editor panel
      Preview.svelte          live p5.js preview panel (sandboxed iframe)
      WebcamCapture.svelte    native getUserMedia capture modal (two-step)
```
