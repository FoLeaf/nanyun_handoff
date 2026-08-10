# Learnings

Corrections, insights, and knowledge gaps captured during development.

**Categories**: correction | insight | knowledge_gap | best_practice  
**Areas**: frontend | backend | infra | tests | docs | config  
**Statuses**: pending | in_progress | resolved | wont_fix | promoted

---

## [LRN-20260528-001] best_practice

**Logged**: 2026-05-28T09:31:00Z
**Priority**: medium
**Status**: resolved
**Area**: config

### Summary
LobeHub Skills Marketplace skill installation requires `npx` permission allowlist in auto mode.

### Details
When installing skills via `npx -y @lobehub/market-cli`, the auto mode classifier blocks execution because it's an externally-sourced npm package not in declared dependencies (supply-chain risk concern). Two permissions were needed:
1. `Bash(curl lobehub.com/*)` — to fetch skill.md content
2. `Bash(npx -y @lobehub/market-cli*)` — to run the marketplace CLI

### Suggested Action
Pre-add these permissions to settings.json when planning to use LobeHub marketplace, or run with `--permissions` flag.

### Metadata
- Source: conversation
- Related Files: C:\Users\19y\.claude\settings.json
- Tags: lobehub, marketplace, permissions, auto-mode

---
