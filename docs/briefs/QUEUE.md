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
- `pio run` must pass. From Q1 on, the server build and the end-to-end
  check must pass too, on the staging host via `tools/staging-check.ps1`
  (never claim it passed if it did not run).
- Every new screen gets ANSI art made with `tools/ansi/` and checked as PNG.
- All caller-visible text is a draft for Juha's final edit; keep it in
  card files, not in code. Follow `.kiro/steering/writing.md` (copy it
  from `docs/briefs/WRITING_RULES.md` in the next run if it is missing).
  The plot must be good as literature, not just correct as a puzzle.
- Game design (Juha, Nov 13): the game must be finishable, take a good
  amount of time (target 60 to 120 minutes for a first-time player), and
  have no grinding: no credit farming, no repeated actions to fill a meter,
  no waiting except the 2-minute prologue. Every gate opens by noticing
  something or thinking, never by repetition. From Q7 on, extend the e2e
  test so a scripted player finishes everything built so far.
- Tick the item here when done, with the commit range, and stop. One item
  per run.

## Items

- [x] **Q0 - S4 follow-up.** (done, e6ec1a0..c76ec9f; `pio run` passes, server build not run: Windows.) Power-up is one-off: default
  `decay_after_idle_min=0` and make that the documented behaviour; keep
  the SysOp force command for demos, and let the SysOp log in during
  brownout (today `POWER OFF` locks the SysOp out). Commit
  `docs/STORY_EXTRAS.md` and this queue. Social board: public conferences
  (Lounge, Spoilers, No Spoilers, Radio Room) work like a normal BBS with
  posting; story conferences, crew mail and logs are read-only. Fold
  `docs/STORY_EXTRAS.md` into the story-bible spec (love story A as the
  side story, B as Ty's texture; scenes; easter eggs as tasks; marketing
  voice into `product.md`).
- [x] **Q1 - Server build and end-to-end check on staging.** (done, b733fe5..HEAD of the Q1 commits; staging check PASS: server build, phase-1 smoke, /api/board, terminal page, Telnet e2e 12 passed, 0 failed, 1 skipped (crew login: no crew account on the card yet, run with `-Crew` once C1 exists); `pio run` passes; the build and test found no fault in the shared core. Placeholder on 8086 is swapped out for the run and restored. Docker is
  out: no WSL on this PC, by choice.) Staging host, set up Nov 12:
  `ssh lastcaller@helppox.com` (port 22, key auth from this PC already
  works, no sudo), Rocky Linux 9.8, g++ 11.5, cmake 3.31, civetweb-devel
  1.16. Work only under `/home/lastcaller/`. Bind only to `127.0.0.1`;
  the allowed HTTP port is 8086 (use a localhost-only Telnet port of your
  choice for tests). Never touch anything outside the home folder; a
  Kouvosto board runs on the same host.
  1. Create a bare repo `~/last-caller.git` on the host, add it as git
     remote `staging`, push `main`.
  2. `tools/staging-check.ps1`: push to `staging`, then over SSH: check
     out into `~/lastcaller-staging/src`, `cmake` and build the server,
     copy `sd/` to a fresh disposable card tree, run the phase-1 smoke
     check, start the server on 127.0.0.1:8086 with that card, then run
     an end-to-end test (a Python script in `tools/test/`, run on the
     host, using only the standard library) over localhost Telnet:
     brownout banner, a drop after the window (use a short test window),
     `admin`/`admin`, ship console, power to RTG, ELIZA's wake-up, create
     a caller account, see the main menu, ELIZA_WAKE scene plays, log in
     as a crew account once that exists. Also `curl` `/api/board` and
     the terminal page. Stop the server and print a clear PASS/FAIL
     summary.
  3. Fix whatever the server build or the test finds, in the shared core,
     keeping `pio run` green.
  4. `docs/TESTING.md`: how to run it. From now on this is the second
     build for every item.
- [x] **Q2 - Triggers (S5).** (done, e79fa6e..fe4cc9b plus the tick; `pio run`
  passes, host tests pass with `zig c++`, server build not run: Windows, Q1
  blocked; board test still to do once the C1 crew accounts exist.)
  `src/core/haunt.h`, `STORY/TRIGGERS.INI`:
  events, conditions, effects as in SLICES S5, plus: private ELIZA mail
  to one caller, and crew-login visits granting flags to the caller's own
  account. Idempotent, flags only grow. Host-side tests.
- [x] **Q3 - Scene engine.** (done, a222ce7..832d528 plus the tick; `pio run` passes, host tests pass with `zig c++`, server build not run: Windows, Q1 blocked; board test still to do.) The visual novel door from STORY_EXTRAS:
  portrait art, typed dialogue with pauses, choices that set flags, scenes
  as card files in `STORY/SCENES/`. Works on Telnet and browser.
- [x] **Q4 - Ghosts and ELIZA's voice (S6, S9).** (done, 14ff630..0c0c5fb plus the tick; `pio run` passes, host tests pass with `zig c++`, server build not run: Windows, Q1 blocked; board test still to do.) Ghost callers in WHO,
  pages and chat with typing delays, ELIZA's scripted lines keyed by
  trigger and chat keyword (`STORY/GHOSTS.INI`, `STORY/SYSOP.INI`).
  Also fix from the Q3 review: keys typed while a scene types out are
  echoed into the middle of the text; suppress the echo while a scene
  types (Enter still flushes).
- [x] **Q5 - Diagnostics and shell doors (S4b, S6b).** (done, see `git log` from "Q5: staging fixes" to "Q5: e2e: ls order" plus the tick; `pio run` passes, host tests pass with `zig c++`, staging check PASS: server build, smoke, e2e 16 passed, 0 failed, 1 skipped (crew login); board test still to do, spec shell-doors task 8; the comms manual with the query codes and the haven tree come with Q7 and Q9.) Plus two staging
  fixes from the Q1 review: the port-8086 placeholder is gone for good
  (it was a leftover test server), so remove the code in
  `tools/staging-remote.sh` that stops and restores it (anything on the
  port should just abort the run); and `tools/staging-check.ps1` fails in
  Windows PowerShell 5.1 because `git push` writes to stderr under
  `$ErrorActionPreference='Stop'`: check `$LASTEXITCODE` instead.
- [x] **Q5b - Prologue per caller (Juha, Nov 13).** (done, 9e6c012..4bb92f8 plus the tick; `pio run` passes, host test passes with `zig c++`, staging check PASS: server build, smoke, e2e 18 passed, 0 failed, 1 skipped (crew login); board test still to do, TESTING.md steps 1 to 15; the timer is `timer_sec` in `POWER.INI`.) Replaces the
  board-wide power state. Every new caller gets the brownout; nobody's
  game depends on anyone else's.
  - Front page: the brownout boot banner and a login prompt. No "New"
    option on the front page any more.
  - Registered callers log in normally and go straight to the powered
    board (they restored the power when they joined).
  - No account: log in as `admin`/`admin` (read-only, many at once). The
    session gets a 2-minute power timer (in `POWER.INI`). If the caller
    has not switched the power to the RTG when it runs out, the session
    ends with the power-glitch screen ("SUNSIDE ENDS - CARRIER LOST").
  - Switching to the RTG stops that caller's timer, ELIZA wakes for that
    caller, and the ship console gains one new option: create a crew
    account (handle, password, the usual new-user steps). Account
    creation lives only here. After creating it the caller is told to
    log in with it; the ELIZA_WAKE scene plays at that first login.
  - Remove the global power state, the RTG decay, and the SysOp POWER
    ON/OFF commands, or reduce them to what is still needed; keep the
    code and card files small. Update the power-prologue spec, `TESTING.md`,
    `MENU_AUDIT.md` and the e2e test (timer drop, power restored, account
    created in the console, log in with it, registered caller skips the
    prologue). Staging check must PASS.
- [x] **Q6 - Manifest and endings (S8).** (done, dee42ff..HEAD of the Q6 commits plus the tick; `pio run` passes, host tests pass with `zig c++`, staging check PASS: server build, smoke, e2e 31 passed, 0 failed, 1 skipped (crew login), event log clean; board test still to do, spec manifest task 9. The evidence flags `KHI` to `KZW` and the real final command come with Q7 to Q9; the e2e grants the flags on the disposable card.) Pairs confirmed, ELIZA's row
  last, three endings, sticky ghost handle, notebook view.
- [x] **Q7 - Content act 1.** (done, db7a836..HEAD of the Q7 commits plus the tick; `pio run` passes, host tests pass with `zig c++`, staging check PASS: server build, smoke, e2e 39 passed, 0 failed, 0 skipped (act 1 played as a fresh caller, crew login `TYVANCE`/`140374` included), event log clean; board test still to do, `docs/TESTING.md` "Act 1". Story record in `DATA/STATE/STORY.DAT`; accounts in `STORY/ACCOUNTS.INI`; `sd/BBS/FILES` and `sd/BBS/MSGBASE` are now tracked (they hold the story). Not in act 1, left for Q8: the passwords of `CMDR` and `GARDEN` are placeholders, fates and causes, Orbit 3 to 9, logs, the other crew mailboxes, the 22 Nov radio message.) First, from the Q6 review: the 255-char
  user flag field will overflow once 40 evidence items, trigger and scene
  markers exist. Move story progress (T_, SC_, evidence and manifest
  flags) to a per-caller story record under `DATA/` (via `putRecord`),
  with the user flags field kept for access flags; migrate existing story
  flags on load; property test that flags still only grow. Then: Accounts and user list, bulletins, public
  and story conferences, crew posts 2 to 21 Nov, Ty's journal and Orbit
  1 to 2, the open evidence (E01 to E15 and the new open items), first
  scenes. A fresh caller gets a lead in two minutes.
- [x] **Q8 - Content act 2.** (done, 81c0647..HEAD of the Q8 commits plus the tick; `pio run` passes, staging check PASS: server build, smoke, e2e 48 passed, 0 failed, 0 skipped (acts 1 and 2 played as a fresh caller, then the manifest and both endings on the flags the content grants; the `E2E_EVIDENCE` shortcut is gone from the staging card), event log clean; host tests not re-run (no logic header changed); board test still to do, `docs/TESTING.md` "Act 2". Passwords: `CMDR` ANDERS, `GARDEN` HANAMI, `CHAOS` LADDER, each with three hints. The reader now plays overstruck rows (Orbit 3 at 300 baud). Found on the way: `.ANS` files were LF on Linux checkouts and drew as a staircase on the server; `.gitattributes` now keeps them byte for byte. The spec `.kiro/specs/act2/` was written after the content commit, not before. Left for Q9: the order code `NW1-98-44-17` (ORD1 to ORD4) and the shell gate, the 22 Nov radio message and the 2026 club, Ty's last post in the power bay.) Day 22 onward: logs, crew mail, the module
  jettison thread, telemetry, audit log, overpainted ANSI, jigsaw, memory
  scenes.
- [x] **Q9 - Content act 3.** (done, 757f90e..HEAD of the Q9 commits plus the tick; `pio run` passes, host shell test passes with `zig c++`, staging check PASS: server build, smoke, e2e 49 passed, 0 failed, 0 skipped (the whole game as a fresh caller: acts 1 and 2, the order code in Diagnostics, the haven with no edit on the card, the manifest, the usage line and the final command `release out-1122 ground-1`, both endings, bulletin 5, Eero's mail), event log clean; board test still to do, `docs/TESTING.md` "Act 3". Ty's last post and `MEM_BAY`, the club posts, the news wire and Eero's last line are drafts. Eero's answer is the scene plus a private message, not a Radio Room post: the room's history is shared and a hosted board has many finishers. New engine key `final_usage=`. Left: Q10 music and sound.) NOWHERE file system, orders, client list,
  final command, ending scenes, the 2026 bulletin, the love story to its
  last line.
- [x] **Q10 - Music and sound.** (done, 3037aa6..HEAD of the Q10 commits plus the tick; `pio run` passes, host tests pass with `zig c++` (modinfo, scene), staging check PASS: server build, smoke, `/sfx` served and refused, e2e 49 passed, 0 failed, 0 skipped (the tunes opened as a text view over Telnet), event log clean; board test still to do, `docs/TESTING.md` "Music and sound". Tunes: `SUNSIDE` and `THERMO` in Ty's Gallery, `HARBOUR` (six channels, unfinished) in Ty's Desktop; the three load in the card's libopenmpt (`node tools/mod/openmpt_check.js`) but nobody has listened to them yet, so the tunes themselves need the author's ears. Cues: `CHIRP` at the RTG, `STATIC` at the brownout drop, scene key `sound=` (`HUM` at ELIZA's wake, `RELAY` in `MEM_BAY`); `MODEM.WAV` is made but no cue uses it yet (Q11). No gate depends on a tune. All sample names are drafts.) `tools/mod/` generator writing original
  ProTracker MODs; Ty's two or three tunes with text in sample names; the
  player shows sample names (browser) and a text view on Telnet; short
  original sound effects and their browser cues.
- [x] **Q11 - Easter eggs and nerd stuff.** (done, cf6e35a..HEAD of the Q11 commits plus the tick; `pio run` passes, host test `egg_logic_test` passes with `zig c++`, staging check PASS: server build, smoke, e2e 55 passed, 0 failed, 0 skipped (the whole game, then the eggs: modem commands, hidden words, files, ELIZA in chat, Ty's demo scene, snake, `ATH0`), event log clean; board test still to do, `docs/TESTING.md` "Easter eggs". `MODEM.WAV` plays when a browser caller dials. No egg sets a flag; the demo leaves only the scene's own markers. Not done: the arrow-key cheat code, ROT13 lounge posts, hex dump, excuse generator, the demo does not play Ty's MOD (its line points to SUNSIDE in the gallery). Door menu slots raised to 5. The hidden node is Ty's group board (number in `NODELIST.TXT`). All text is draft.) From STORY_EXTRAS: modem
  commands, hidden commands, POST screen, Y2K memo, brochure, price list,
  Ty's demo (ANSI animation with his MOD), Ty's snake door, fortunes.
- [x] **Q12 - Verification.** (done, b950bc0..HEAD of the Q12 commits plus the tick; `pio run` passes, host tests pass with `zig c++` (`tools/run_host_tests.py`), staging check PASS: server build, `ctest` (host logic tests, the new story property test, story lint), smoke, e2e 55 passed, 0 failed, 0 skipped, event log clean; board test not needed beyond `docs/TESTING.md` "Verification". The `playtester` agent finished the game through the `bbs-caller` MCP server on the staging board in run 3 (ending 1; runs 1 and 2 stopped at faults, see `docs/PLAYTEST.md`). Fixed from the playtest: the short boot banner had no service label and the label read `PASSWORD DEFAULT` (it is `admin`). Open items for the author are listed in `docs/PLAYTEST.md`: ELIZA's last blank has no visible source on a fresh account, `release` and the order-code query are in no help list, fate option wording is for one person. Endings 2 and 3 were played by the e2e, not the agent. Formats checked in the Kiro docs: hooks v1, `.kiro/agents/*.md`, `.kiro/settings/mcp.json`.) `story-lint` (every fact reachable, no
  softlock, every ending reachable), property tests in the server build,
  the `bbs-caller` MCP server and a `playtester` agent that finishes the
  game; hooks for lint and tests; custom agents (`ghostwriter`,
  `puzzle-smith`, `firmware-guard`, `playtester`). The `bbs-caller` MCP
  server connects to the staging board (an SSH tunnel to 127.0.0.1 on the
  host is fine), and the `playtester` agent must actually finish the game
  through it; record a transcript summary in `docs/PLAYTEST.md`. Check
  the current Kiro formats for agents, hooks and MCP config; do not guess.
- [x] **Q12b - Playtest fixes.** (done, 6b2e3b8..HEAD of the Q12b commits plus the tick; `pio run` passes, host tests pass with `zig c++` (new `wrap_logic_test`), story lint clean with the new open-sources check, staging check PASS on 048bbcc: server build, ctest, smoke, e2e 55 passed, 0 failed, 0 skipped, event log clean. Run 4c finished the game from a brand-new caller (ending 1) over two agent sessions on one account, about 210 tool calls; run 4 reached the ending scene in about 245 calls, which the agent put at 90 minutes of reading, inside the 60 to 120 minute goal (an agent estimate, not a timed human). No dead end; the display oddities left are in `docs/PLAYTEST.md`. The card texts were rewrapped to 78 columns. All new caller-visible lines are drafts.) Fix open items 1 to 5 in
  `docs/PLAYTEST.md`: a wrong login name at the brownout prompt gets one
  in-world hint line; ELIZA's last blank gets a visible in-world source
  for a fresh account (an ELIZA line or the notebook pointing at where the
  evidence is, without giving the answer), and if crew-visit evidence can
  be earned in a way that does not reach the caller's own account, fix
  that so it cannot softlock; `release` and the order-code query appear
  in the in-world help (ELIZA may be cagey, but a careful reader must be
  able to find them); fate and responsibility options get wording per
  row; check and fix mid-word wrapping at 79 columns, the pager redraw and
  the Lounge list start. Then run the `playtester` agent again from a
  brand new caller with no hints from earlier runs and record run 4 in
  `docs/PLAYTEST.md`. Done when run 4 finishes the game with no fault and
  the staging check passes; note its play time against the 60 to 120
  minute goal.
- [x] **Q12c - Milestones, design.** (done, 05a1676 plus the tick; story lint clean on the current card (60 rules, 10 scenes), no engine or card change. Spec in `.kiro/specs/milestones/` (requirements, design, tasks, `content-drafts.md`), `docs/PUZZLE_CHAIN.md`, story bible updated: Ty's last fight with an hour table, V-F and V-C, M1 to M9, matrix E41 to E48, act gate audit. Decisions for the author: act gates are per post (`Needs:`) and per file (`NEEDS.BBS`); H-C loses its only open item, so a Day 21 note in the Mission Log (draft) gives it one; ELIZA's password in the draft checklist is `KESTREL4`; `SYSVW` bypasses story gates only, never the protected flags. All text is draft.) Read `docs/briefs/S12c-milestones.md`
  and do its Q12c part: story-bible update (Ty's last fight, timeline,
  fates, unlock chain M1 to M9, scenes), `docs/PUZZLE_CHAIN.md`, the spec
  `.kiro/specs/milestones/`, content drafts. No engine code. Done when the
  spec is committed and the story lint still passes on the current card.
- [ ] **Q12d - Milestones, build.** Build the Q12d part of
  `docs/briefs/S12c-milestones.md` from `.kiro/specs/milestones/`. The
  story never grants the real `SYSOP` access flag. Done when lint, the
  property test, `pio run` and the staging check pass, the e2e plays the
  new chain, and `playtester` run 5 finishes from a brand-new caller
  (recorded in `docs/PLAYTEST.md` with play time).
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
