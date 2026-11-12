# Build queue - to deploy-ready

Order of work for Kiro, set by Claude Code (planner), Nov 12 2026. Goal:
everything in the repo, ready for an agent to deploy on the server with
one prompt. The hosted board is the product; the stick is the collector's
edition.

Rules for every item:
- Read `docs/SLICES.md`, `.kiro/specs/story-bible/`,
  `.kiro/specs/power-prologue/`, `docs/STORY_EXTRAS.md` and the steering.
- Write or extend a Kiro spec for the item (requirements in EARS, design,
  tasks), commit it, then do the tasks in small commits.
- `pio run` must pass. From Q1 on, the server build must pass too, in the
  Docker container from Q1 (never claim it passed if it did not run).
- Every new screen gets ANSI art made with `tools/ansi/` and checked as PNG.
- All caller-visible text is a draft for Juha's final edit; keep it in
  card files, not in code.
- Tick the item here when done, with the commit range, and stop. One item
  per run.

## Items

- [ ] **Q0 - S4 follow-up.** Power-up is one-off: default
  `decay_after_idle_min=0` and make that the documented behaviour; keep
  the SysOp force command for demos, and let the SysOp log in during
  brownout (today `POWER OFF` locks the SysOp out). Commit
  `docs/STORY_EXTRAS.md` and this queue. Social board: public conferences
  (Lounge, Spoilers, No Spoilers, Radio Room) work like a normal BBS with
  posting; story conferences, crew mail and logs are read-only. Fold
  `docs/STORY_EXTRAS.md` into the story-bible spec (love story A as the
  side story, B as Ty's texture; scenes; easter eggs as tasks; marketing
  voice into `product.md`).
- [ ] **Q1 - Server build in Docker.** A `server/Dockerfile` (Rocky Linux
  9 or the closest image with `civetweb-devel` from EPEL) that builds the
  server and runs the phase-1 smoke check against a disposable copy of
  `sd/`. A script `tools/server-check.ps1` runs it. Docker Desktop is
  installed but may not be running: start it, or report that Juha must.
  From now on this is the second build.
- [ ] **Q2 - Triggers (S5).** `src/core/haunt.h`, `STORY/TRIGGERS.INI`:
  events, conditions, effects as in SLICES S5, plus: private ELIZA mail
  to one caller, and crew-login visits granting flags to the caller's own
  account. Idempotent, flags only grow. Host-side tests.
- [ ] **Q3 - Scene engine.** The visual novel door from STORY_EXTRAS:
  portrait art, typed dialogue with pauses, choices that set flags, scenes
  as card files in `STORY/SCENES/`. Works on Telnet and browser.
- [ ] **Q4 - Ghosts and ELIZA's voice (S6, S9).** Ghost callers in WHO,
  pages and chat with typing delays, ELIZA's scripted lines keyed by
  trigger and chat keyword (`STORY/GHOSTS.INI`, `STORY/SYSOP.INI`).
- [ ] **Q5 - Diagnostics and shell doors (S4b, S6b).**
- [ ] **Q6 - Manifest and endings (S8).** Pairs confirmed, ELIZA's row
  last, three endings, sticky ghost handle, notebook view.
- [ ] **Q7 - Content act 1.** Accounts and user list, bulletins, public
  and story conferences, crew posts 2 to 21 Nov, Ty's journal and Orbit
  1 to 2, the open evidence (E01 to E15 and the new open items), first
  scenes. A fresh caller gets a lead in two minutes.
- [ ] **Q8 - Content act 2.** Day 22 onward: logs, crew mail, the module
  jettison thread, telemetry, audit log, overpainted ANSI, jigsaw, memory
  scenes.
- [ ] **Q9 - Content act 3.** NOWHERE file system, orders, client list,
  final command, ending scenes, the 2026 bulletin, the love story to its
  last line.
- [ ] **Q10 - Music and sound.** `tools/mod/` generator writing original
  ProTracker MODs; Ty's two or three tunes with text in sample names; the
  player shows sample names (browser) and a text view on Telnet; short
  original sound effects and their browser cues.
- [ ] **Q11 - Easter eggs and nerd stuff.** From STORY_EXTRAS: modem
  commands, hidden commands, POST screen, Y2K memo, brochure, price list,
  Ty's demo (ANSI animation with his MOD), Ty's snake door, fortunes.
- [ ] **Q12 - Verification.** `story-lint` (every fact reachable, no
  softlock, every ending reachable), property tests in the server build,
  the `bbs-caller` MCP server and a `playtester` agent that finishes the
  game; hooks for lint and tests; custom agents (`ghostwriter`,
  `puzzle-smith`, `firmware-guard`, `playtester`).
- [ ] **Q13 - Polish.** Art pass 3 (Ty's tag on his screens, a real
  spacecraft silhouette, decay over the timeline), glitches (S7), the
  1998 marketing copy in README and bulletins, mobile check of the
  browser terminal.
- [ ] **Q14 - Deploy package.** `deploy/`: build and install script,
  systemd unit, Apache vhost (HTTP and WebSocket proxy, TLS), hardening
  (uploads off, SysOp page behind auth, rate limits, chat length), a
  reset script for demos, `docs/DEPLOY.md`, and `deploy/AGENT_PROMPT.md`:
  one prompt another agent can follow to deploy on a fresh Rocky Linux
  host at a subdomain of helppox.com, alongside the existing board
  without touching it.
