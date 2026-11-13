# Brief Q15 - Refactor pass

From: Claude Code (planner), at Juha's request, 13 Nov 2026. To: Kiro.

Go through the whole repository and make it smaller, plainer and easier to
read, without changing what a caller sees or does. The game is feature
complete for Kiroween (submission 3 Dec 2026), so this pass is about
quality, not features.

## Scope

Everything that is ours: `src/`, `server/`, `shared/web/`, `tools/`
(including `tools/ansi/`, `tools/mcp/`, `tools/test/`), `deploy/`, `docs/`,
`.kiro/` (steering, specs, hooks, agents), `README.md`, `AGENTS.md`.

Out of scope: third-party code (`server/platform/miniz*`, anything listed in
`THIRD_PARTY_NOTICES.md`), generated headers (`src/net/web_terminal.h`,
`src/net/sysop_page.h`: change the HTML instead), and all caller-visible
text on the card (`sd/BBS/`): those are Juha's drafts. Do not reword, rewrap
or move story text, art or posts.

## What to look for

1. **Repetition.** Code doing the same thing in several places: menu and
   prompt drawing, INI lookups, flag checks, pager and wrap logic, record
   rewrites, door boilerplate (`enter`/`owns`/`handle`), the `*_logic.h`
   pure helpers and their tests, ANSI screen builders in
   `tools/ansi/screens/` (shared parts belong in `_common.py`), Python
   helpers duplicated between `tools/story_lint.py`, `tools/bbscard.py`,
   `tools/test/e2e_telnet.py` and `tools/mcp/bbs_caller.py`, shell functions
   in `deploy/` (shared ones belong in `deploy/lib.sh`), PowerShell scripts in
   `tools/`. Pull the common part into one reusable function, header or
   module.
2. **Dead code.** Functions, globals, settings keys, INI keys, routes, CSS,
   JS, scripts, test helpers and docs that nothing uses. Prove it with a
   search across `src`, `server`, `shared`, `tools`, `deploy` and the card
   (`sd/BBS`, including `SYSTEM.INI` and `STORY/*.INI`) before removing. Keep
   anything the hardware build alone uses (`src/hw`, `esp32.h`, LCD).
3. **Simplification.** Long functions that can be split, deep nesting that
   early returns remove, magic numbers that want a named constant, hand-made
   parsing that an existing helper already does.
4. **Documentation.**
   - Fix docs that disagree with the code. Known example: `.kiro/steering/
     structure.md` says `sd/BBS/FILES` and `MSGBASE` are git-ignored, while
     `AGENTS.md` says they are tracked (they are). Check every table of
     folders, files, commands and keys against the tree.
   - Every header in `src/` starts with a short comment: what it does, what
     it depends on, who calls it.
   - `docs/` has grown by slice. Planning documents that are finished
     (`KIROWEEN_PLAN.md`, `SLICES.md`, `KIRO_REPO_STEPS.md`,
     `STORY_BIBLE_DRAFT.md`, older briefs) move to `docs/archive/` with one
     line at the top saying what replaced them; do not delete them, the
     Devpost write-up shows the process. Update links to them.
   - `README.md` and `AGENTS.md` stay the two entry points: what it is, how
     to build, test and deploy, where things are. Short, current, no history.
   - Specs in `.kiro/specs/` stay as they are (they are the record of the
     work); fix only wrong file names or paths in them.

## Rules

- **No behaviour change.** The e2e must play the same game with the same
  output. If a refactor would change a byte of caller output, do not do it,
  or note it as a question for Juha in the tick instead.
- Keep the constraints in `AGENTS.md`: transport-independent core, no Linux
  or ESP32 headers in `src/core`, `src/bbs`, `src/doors`; `routes.h` and
  `telnet.h` stay shared; content stays on the card.
- Watch RAM and flash on the ESP32: no new heap use in hot paths, no `std::`
  containers where the code avoids them today. Report the `pio run` RAM and
  flash figures before and after in the tick.
- First write `.kiro/specs/refactor/` (requirements in EARS, design, tasks)
  with the findings: one task per change, each with its evidence (for dead
  code, the search that found no user). Commit it. Then do the tasks in
  small commits, one area per commit, each one building.
- After each area: `pio run`, `python tools/run_host_tests.py`,
  `python tools/story_lint.py`. At the end: the staging check
  (`tools/staging-check.ps1`): server build, ctest, smoke, e2e. Never claim a
  check passed if it did not run.
- If a finding is a real bug, do not fix it inside a refactor commit: list it
  in the tick as an open item so Juha can queue it.

## Done when

The spec is committed, the tasks are done or marked "skipped, because...",
`pio run`, host tests, story lint and the staging check pass, the e2e count
is unchanged (63 passed, 0 failed, 0 skipped, or more if tests were added),
and the tick in `docs/briefs/QUEUE.md` gives: commit range, lines removed and
added per area, RAM and flash before and after, what was archived, and open
bugs or questions found.
