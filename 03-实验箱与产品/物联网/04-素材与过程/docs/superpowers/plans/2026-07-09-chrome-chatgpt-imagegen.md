# Chrome ChatGPT ImageGen Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build a Chrome-only ChatGPT web image-generation skill with a dedicated profile, background DOM automation, downloads, and manifests.

**Architecture:** Add a new `chrome-chatgpt-imagegen` skill and keep the old CentBrowser skill as a compatibility shim. Use Playwright against local Chrome with a persistent profile; separate first-login setup from repeatable batch image generation.

**Tech Stack:** Codex skill markdown, Node.js ESM, Playwright, Python fallback watcher.

## Global Constraints

- Chrome only; no CentBrowser workflow in the active skill.
- Default profile path: `C:\Users\19y\.codex\browser-profiles\chatgpt-imagegen-chrome`.
- No password capture, cookie extraction, CAPTCHA bypass, private ChatGPT API calls, or hidden endpoint scraping.
- Use DOM/ARIA selectors and browser download events before any visual fallback.
- Keep the skill focused on image generation and download; downstream skills insert images.

---

### Task 1: Create Chrome Skill Skeleton

**Files:**
- Create: `C:\Users\19y\.codex\skills\chrome-chatgpt-imagegen\SKILL.md`
- Create: `C:\Users\19y\.codex\skills\chrome-chatgpt-imagegen\agents\openai.yaml`
- Create: `C:\Users\19y\.codex\skills\chrome-chatgpt-imagegen\references\prompt-patterns.md`
- Create: `C:\Users\19y\.codex\skills\chrome-chatgpt-imagegen\references\browser-strategy.md`

**Interfaces:**
- Produces: a discoverable skill named `chrome-chatgpt-imagegen`.

- [ ] Write concise Chrome-only workflow documentation.
- [ ] Add UI metadata whose default prompt invokes `$chrome-chatgpt-imagegen`.
- [ ] Add embedded laboratory prompt patterns.
- [ ] Add browser strategy notes for dedicated profile and background operation.

### Task 2: Implement Setup And Runner Scripts

**Files:**
- Create: `C:\Users\19y\.codex\skills\chrome-chatgpt-imagegen\scripts\setup_profile.mjs`
- Create: `C:\Users\19y\.codex\skills\chrome-chatgpt-imagegen\scripts\chatgpt_imagegen_runner.mjs`
- Create: `C:\Users\19y\.codex\skills\chrome-chatgpt-imagegen\scripts\watch_downloads.py`

**Interfaces:**
- Consumes: prompts JSON array or object list with `prompt`, `id`, `count`, and `filename_prefix`.
- Produces: generated image files and `manifest.json`.

- [ ] Implement Chrome path detection and bundled Playwright loading.
- [ ] Implement visible setup-profile login helper.
- [ ] Implement runner argument parsing and `--dry-run`.
- [ ] Implement sequential prompt submission with DOM selectors.
- [ ] Implement download-button path, visible-image fallback, hashing, dimension detection, and manifest writing.
- [ ] Copy and extend the Python download watcher with prompt metadata support.

### Task 3: Add Compatibility Shim And Validate

**Files:**
- Modify: `C:\Users\19y\.codex\skills\centbrowser-chatgpt-imagegen\SKILL.md`
- Modify: `C:\Users\19y\.codex\skills\centbrowser-chatgpt-imagegen\agents\openai.yaml`

**Interfaces:**
- Produces: old skill invocation redirects users to `$chrome-chatgpt-imagegen`.

- [ ] Replace old CentBrowser workflow with a short deprecation shim.
- [ ] Run quick validation on the new skill and old shim.
- [ ] Run runner dry-run and watcher smoke tests.
