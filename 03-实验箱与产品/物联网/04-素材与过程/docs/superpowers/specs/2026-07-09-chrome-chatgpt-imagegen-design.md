# Chrome ChatGPT ImageGen Design

## Goal

Replace the CentBrowser-based image generation skill with a Chrome-only skill that uses a dedicated local Chrome profile to generate and download ChatGPT web images without keeping the browser in the foreground.

## Scope

The skill generates and downloads raster image assets, writes a manifest, and returns paths for downstream document or presentation work. It does not insert images into Word, PowerPoint, spreadsheets, or HTML.

## Architecture

Create a new `chrome-chatgpt-imagegen` skill under `C:\Users\19y\.codex\skills`. The skill uses Playwright with the user's local Chrome executable and a dedicated persistent profile. A setup script opens ChatGPT visibly once for manual login. The runner script later launches the same profile in a minimized headed Chrome session, submits prompts through DOM/ARIA selectors, waits for generated image candidates, downloads images through official download controls when available, and falls back to extracting visible generated images from the page context.

Keep the old `centbrowser-chatgpt-imagegen` skill as a small compatibility shim that points to the Chrome-only replacement.

## Components

- `SKILL.md`: concise Chrome-only workflow, boundaries, output contract, and fallback order.
- `scripts/setup_profile.mjs`: opens the dedicated Chrome profile for first-time ChatGPT login.
- `scripts/chatgpt_imagegen_runner.mjs`: batch prompt runner, downloader, verifier, and manifest writer.
- `scripts/watch_downloads.py`: fallback watcher for browser-managed downloads and standalone verification.
- `references/browser-strategy.md`: operational notes for profile isolation, background mode, and troubleshooting.
- `references/prompt-patterns.md`: reusable image prompts for embedded lab proposal assets.
- `agents/openai.yaml`: UI metadata matching the new skill name.

## Constraints

- Chrome only; remove CentBrowser workflow text and automation paths.
- Use a dedicated profile by default: `C:\Users\19y\.codex\browser-profiles\chatgpt-imagegen-chrome`.
- Do not ask for passwords, inspect cookies, bypass login or CAPTCHA, call private ChatGPT endpoints, or scrape hidden APIs.
- Prefer DOM/ARIA automation and browser download events over coordinate or screenshot automation.
- Run prompts sequentially to avoid ChatGPT conversation and download ambiguity.
- Write a manifest with prompt metadata, output path, file size, hash, dimensions where detectable, and download method.

## Testing

Validate the new skill folder with `quick_validate.py`. Run script-level checks for argument parsing, dry-run prompt loading, Chrome path detection, and download watcher manifest generation. Live ChatGPT generation remains optional because it depends on the user's logged-in web session.
