# Build queue - to deploy-ready

Order of work for Kiro, set by Claude Code (planner), Nov 12 2026. Goal:
everything in the repo, ready for an agent to deploy on the server with
one prompt. The hosted board is the product; the stick is the collector's
edition.

Rules for every item:
- Read `docs/archive/SLICES.md`, `.kiro/specs/story-bible/`,
  `.kiro/specs/power-prologue/`, `docs/STORY_EXTRAS.md` and the steering.
- Write or extend a Kiro spec for the item (requirements in EARS, design,
  tasks), commit it, then do the tasks in small commits.
- `pio run` must pass. From Q1 on, the server build and the end-to-end
  check must pass too, on the staging host via `tools/staging-check.ps1`
  (never claim it passed if it did not run).
- Every new screen gets ANSI art made with `tools/ansi/` and checked as PNG.
- All caller-visible text is a draft for Juha's final edit; keep it in
  card files, not in code. Follow `.kiro/steering/writing.md` (copy it
  from `docs/archive/briefs/WRITING_RULES.md` in the next run if it is missing).
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
- [x] **Q12c - Milestones, design.** (done, 05a1676 plus the tick; story lint clean on the current card (60 rules, 10 scenes), no engine or card change. Spec in `.kiro/specs/milestones/` (requirements, design, tasks, `content-drafts.md`), `docs/PUZZLE_CHAIN.md`, story bible updated: Ty's last fight with an hour table, V-F and V-C, M1 to M9, matrix E41 to E48, act gate audit. Decisions for the author: act gates are per post (`Needs:`) and per file (`NEEDS.BBS`); H-C loses its only open item, so a Day 21 note in the Mission Log (draft) gives it one; ELIZA's password in the draft checklist is `KESTREL4`; `SYSVW` bypasses story gates only, never the protected flags. All text is draft.) Read `docs/archive/briefs/S12c-milestones.md`
  and do its Q12c part: story-bible update (Ty's last fight, timeline,
  fates, unlock chain M1 to M9, scenes), `docs/PUZZLE_CHAIN.md`, the spec
  `.kiro/specs/milestones/`, content drafts. No engine code. Done when the
  spec is committed and the story lint still passes on the current card.
- [x] **Q12d - Milestones, build.** (done, 964a494..HEAD of the Q12d commits plus the tick; `pio run` passes, host tests pass with `zig c++` (new `overrun_logic_test`, `gatePass` and `then=` cases, property test P8 to P10), story lint clean and now checks the chain M1 to M9, its gates and dependencies and that the story grants no access flag; staging check PASS on 1b7850f: server build, ctest, smoke, e2e 63 passed, 0 failed, 0 skipped (the new chain from a fresh caller: act gates, the 254-zero near miss and the overrun, an early SYSOP request, the handbook, the grant, SYSOP view reading Engineering and Mail: ELIZA, ELIZA's login, the shell from her door, both endings from her account), event log clean. `playtester` run 5b finished from a brand-new caller (ending 1) over two sessions on one account, about 480 tool calls; the agent puts a first-time reader at 120 to 150 minutes, over the 60 to 120 goal (an estimate, not a timed human). Run 5 and session A of 5b stopped at the overrun: the editor cut lines to 200 and then 254 characters; a typed line with 255 zeros now overruns at once. New: main menu `L` (log in as another, keeps the line: the shell in ELIZA's menu reads the caller's own flags). `SYSVW` never opens a SysOp tool; the real `SYSOP` flag is never granted. Board test by hand still to do, `docs/TESTING.md` "Milestones". Open items in `docs/PLAYTEST.md`. All text is draft.) Build the Q12d part of
  `docs/archive/briefs/S12c-milestones.md` from `.kiro/specs/milestones/`. The
  story never grants the real `SYSOP` access flag. Done when lint, the
  property test, `pio run` and the staging check pass, the e2e plays the
  new chain, and `playtester` run 5 finishes from a brand-new caller
  (recorded in `docs/PLAYTEST.md` with play time).
- [x] **Q12e - Shorter overrun.** (done, 1d733c7 plus the tick; `pio run` passes, host tests pass with `zig c++` (`overrun_logic_test` now has 19/20 cases and `nearMiss`), story lint clean, staging check PASS on 1d733c7: server build, ctest, smoke, e2e 63 passed, 0 failed, 0 skipped, event log clean. The overrun is `RUN = 20`; 19 zeros post normally with one garbled ELIZA line (`OVERRUN_NEAR`, no flag), fewer do nothing. Numbers changed in COMMS.TXT, Ty's Lounge post, the Archive post, the milestones spec, the story bible, `PUZZLE_CHAIN.md`, `TESTING.md`. The commit also swept in the untracked `docs/DEVPOST.md` and `docs/briefs/OPENCLAW-halcyon.md` ("Commit before push" hook). The garbled line is in code, a draft; no handbook line gave the number. Board test by hand still to do, `docs/TESTING.md` "Milestones".) The author's call: 255 zeros is
  overkill. Make the overrun a run of 20 zeros (the nod is to an old SSH
  bug; keep it a nod, no real product or CVE named). Update the engine,
  every hint and handbook line that gives the number, the near-miss e2e
  check (19 zeros gives a small glitch only: a flicker or one garbled
  line from ELIZA, no flag, so a caller who miscounted knows they are on
  the right track; fewer than 19 does nothing) and `docs/PUZZLE_CHAIN.md`. Done when lint,
  host tests, `pio run` and the staging check pass.
- [x] **Q13 - Polish.** (done, e2dc20d..HEAD of the Q13 commits plus the tick; `pio run` passes, host tests pass with `zig c++` (new `glitch_logic_test`), story lint clean, staging check PASS: server build, ctest, smoke, e2e 63 passed, 0 failed, 0 skipped, event log clean; board test still to do, `docs/TESTING.md` "Polish (Q13)". Art: the ship in profile at three stages of decay (bulletins 2 and 4, main menu), Ty's tag on his snake and demo. Glitches: `STORY/GLITCH.INI`, one dim line every 4 minutes at most after the first 3, no flag; the 'menu option that appears once' is not built (it would break the menu rule). Marketing: the 1998 copy is at the top of the README; no new bulletin, because the bulletin numbers are fixed in the e2e and the story. Mobile: no phone and headless Edge hung, so it is a code review plus two CSS changes (smoothing, 44 px buttons), not a device test. All text is draft.) Art pass 3 (Ty's tag on his screens, a real
  spacecraft silhouette, decay over the timeline), glitches (S7), the
  1998 marketing copy in README and bulletins, mobile check of the
  browser terminal.
- [x] **Q14 - Deploy package.** (done, 5cb4acd..6bfe651 plus the tick; `pio run` passes, staging check PASS on 9b468ee: server build, ctest, smoke, e2e 63 passed, 0 failed, 0 skipped, event log clean. Checked on the staging host: `bash -n` on the scripts, both Apache templates with `httpd -t` on a throwaway config, the SysOp bootstrap against a disposable card (second run exits 3). NOT run: `install.sh` and `reset.sh` themselves, the unit, certbot (no root on staging, no shellcheck): the first real run on the target host is their test. Chat lines are capped at 240 characters in the shared core (`CHAT_LINE_MAX`). Defaults: HTTP port 8087, loopback Telnet 2327, 16 nodes, uploads off, `/sysop` behind basic auth. No per-address rate limit in Apache (needs a module that would reach the other board) and no firewall change: both listed in `docs/DEPLOY.md`. A caller can reach the board for a few seconds during a reset before SYSOP is made again.) `deploy/`: build and install script,
  systemd unit, Apache vhost (HTTP and WebSocket proxy, TLS), hardening
  (uploads off, SysOp page behind auth, rate limits, chat length), a
  reset script for demos, `docs/DEPLOY.md`, and `deploy/AGENT_PROMPT.md`:
  one prompt another agent can follow to deploy on a fresh Rocky Linux
  host at a subdomain of helppox.com, alongside the existing board
  without touching it.
- [x] **Q14b - Deploy fixes from the first real install.** (done, eb885be plus the tick; `bash -n` passes on all scripts, the module check ran 50 times against a fake `httpd` with a 6000-line list on staging with 0 failures (`tools/test/module_check_test.sh`, also covers a missing module and the certbot lock wait), staging check PASS on eb885be: server build, ctest, smoke, e2e 63 passed, 0 failed, 0 skipped. The same pipe in `port_busy` is fixed; `reset.sh` had none. certbot is retried while it reports its lock (15 s, 20 times), never touching the lock file. A re-run after a stopped SysOp step now verifies the login and records it instead of asking for hand work. NOT run: `install.sh` itself, no root on staging.) OpenClaw ran
  `deploy/install.sh` on helppox (halcyon.helppox.com is live). Two faults:
  (1) `httpd -M 2>/dev/null | grep -q "${m} "` (line 113) dies about one run
  in ten under `set -o pipefail`: `grep -q` exits early and `httpd` gets
  SIGPIPE. Capture `httpd -M` once into a variable and match against it;
  check `reset.sh` and `lib.sh` for the same pattern. (2) The first run
  stopped on a certbot lock and the re-run had to finish the TLS step; make
  a re-run after any failed step finish the HTTP vhost, certificate and TLS
  vhost without manual help (wait for or report the certbot lock, do not
  delete it). Note both in `docs/DEPLOY.md`. Done when `bash -n` passes, the
  module check is run 50 times in a loop on staging without a failure (no
  root needed: a fake `httpd` script that prints a long module list), and
  the staging check passes.
- [x] **Q15 - Refactor pass.** (PARTIAL, 3b16d60..711507e plus the tick. Done: spec in `.kiro/specs/refactor/`; docs archive; doc fixes; one dead-code removal. Lines (added/removed): docs, steering, specs and agents path fixes and archive headers, about 70/45 (13 files moved, 8 of them done briefs); README and AGENTS 12/12 (FILES and MSGBASE are tracked, not ignored; repo map had a missing `art/`; lost `t` in AGENTS); `src/core/history.h` 1/7 (unused `historyEcho` and the `historyEchoes` vector). `pio run` RAM 62004 -> 61996 bytes, flash 1638395 -> 1638211 bytes. Host tests 12 passed (zig c++), story lint 0 errors, staging check PASS on 711507e: server build, ctest, smoke, e2e 63 passed, 0 failed, 0 skipped. Archived to `docs/archive/`: KIROWEEN_PLAN, SLICES, KIRO_REPO_STEPS, STORY_BIBLE_DRAFT and in `archive/briefs/` WRITING_RULES, S0, S1, S1b, S1c, S3a, S3b, S4, S12c, each with a one-line header; links updated, `playtester.md` now forbids `docs/archive/`. All headers in `src/` already had a comment. NOT done, listed as tasks 7 and 8 in the spec: repetition in `tools/ansi/screens`, Python helpers across the tools, `deploy/lib.sh`, door boilerplate, long functions, and a dead-code scan of INI keys, routes, CSS, JS and scripts (the scan covered functions only). Those need their own commits and e2e runs; queue a Q15b. No bugs found. Question for Juha: the untracked `docs/ELIZA_EYE.md`, `docs/LAUNCH_KIT.md` and `docs/briefs/Q16-launch.md` were left alone.) Brief text follows:
  Read `docs/briefs/Q15-refactor.md` and do
  it: go through the whole repository for repetition, dead code,
  simplification and documentation, with no change to caller output and no
  edit to card story text. Spec first in `.kiro/specs/refactor/`, then small
  commits per area. Done when `pio run`, host tests, story lint and the
  staging check pass with the e2e count unchanged, and the tick lists lines
  removed and added, RAM and flash before and after, what was archived, and
  any bugs found.
- [x] **Q15b - Refactor pass, code.** (PARTIAL, 6784d66..HEAD of the Q15b commits, `git log --oneline 396bde0..` plus this tick. Done: (1) `tools/ansi/screens/`: `ascii_screen`, `leader`, `scene_frame` into `_common.py`, a shadowed second `ship_mini` removed; regenerated `sd/BBS` is byte-identical; 53/143 lines added/removed. (2) `tools/telnet_wire.py` shared by `e2e_telnet.py` and `bbs_caller.py`; 59/55 with the new module. (4, 6) the Legend and dice doors were not in the door table: removed with their states and session fields (the purse file code stays in `src/doors/legend.h`), plus unused `boardDay`; `src` 27/497, README and MENU_AUDIT 2/8. (5) `SECS_PER_DAY` and `MS_PER_MINUTE` replace 86400 and 60000 in 7 files. `pio run` RAM 61996 -> 61452 bytes, flash 1638211 -> 1635907 bytes. Host tests 12 passed, story lint 0 errors, staging check PASS on the pushed main: server build, ctest, smoke, e2e 63 passed, 0 failed, 0 skipped. Archived: nothing new. NOT done, each with its reason in `.kiro/specs/refactor/tasks.md` 7 and 8: (3) `deploy/lib.sh` already holds every shared function, nothing to move; door `enter`/`owns`/`handle` boilerplate (no safe common part); splitting long functions (cannot be checked byte for byte without the board); `bootstrap-sysop.py` keeps its own `Conn` because it is installed alone. The dead-code scan of INI keys, CSS, JS and scripts found nothing. No bugs found. Questions for Juha: `/status` and `/api/hangup` have no caller in the repository but are listed in `docs/APACHE_PORT.md`, kept; rename `src/doors/legend.h` to `purse.h`? The server build and ctest ran on the staging host, not on this Windows PC. Brief text follows:) Finish tasks 7 and 8 of
  `.kiro/specs/refactor/` under the same rules as `docs/briefs/Q15-refactor.md`
  (no change to caller output, no edit to card story text, ESP32 RAM and
  flash reported before and after). Work through every area, one commit
  each, building and running host tests and story lint after each: (1)
  repetition in `tools/ansi/screens/` into `_common.py`; (2) Python helpers
  duplicated across `tools/story_lint.py`, `tools/bbscard.py`,
  `tools/test/e2e_telnet.py`, `tools/mcp/bbs_caller.py` and the other tools
  into one shared module; (3) shell functions in `deploy/` into
  `deploy/lib.sh` (`bash -n` on every script); (4) door boilerplate and the
  `*_logic.h` helpers in `src/`, menu, prompt, INI and flag code repeated
  across `src/bbs` and `src/doors`; (5) long functions split, deep nesting
  flattened, magic numbers named; (6) dead-code scan beyond functions:
  `SYSTEM.INI` and `STORY/*.INI` keys the code never reads (and code reading
  keys no card has), routes, CSS and JS in `shared/web/`, scripts in `tools/`
  nothing calls, each removal with the search that proves it. Prefer doing
  all six in this run; if one cannot be finished, finish the others, run the
  staging check, and tick it PARTIAL naming exactly what is left. Done when
  `pio run`, host tests, story lint and the staging check pass with the e2e
  count unchanged, and the tick gives commit range, lines removed and added
  per area, RAM and flash before and after, and any bugs found (listed, not
  fixed).
  Q15b answer (Juha): `src/doors/legend.h` renamed to `purse.h` with `git mv`; include, header comment and every doc reference updated; data files and function names unchanged. `pio run`, host tests and staging check pass.
- [ ] **Q16 - Launch prep.** Brief: `docs/briefs/Q16-launch.md` (Claude
  Code, 13 Nov 2026). Docs truth pass before the repo goes public (README,
  content policy, notices, counted numbers in DEVPOST.md, secret check),
  press-kit screenshots from e2e captures (`tools/presskit.py`), a `?rec=1`
  recording mode in the browser terminal, judge savegame accounts
  (`tools/make-savegame.py`), and one merged board test
  checklist for the stick. No story or puzzle changes.
- [ ] **Q17 - ELIZA's eye on the stick.** Brief: `docs/briefs/Q17-eliza-lcd.md`
  (Claude Code, 13 Nov 2026, Juha's idea). The LCD shows ELIZA's segmented
  eye and a VFD-style status line, driven by board state (boot, asleep,
  brownout, listening, speaking, page); `lcd_mode=` on the card and a
  stick-only SysOp switch bring back the old panel (the BOOT button is broken off). Plus a parametric OpenSCAD PLA sleeve for the
  Deluxe Edition stick, with board measurements left for Juha.
- [ ] **Q18 - Critics' loop to 83 %.** Brief: `docs/briefs/Q18-critics.md`
  (Claude Code, 14 Nov 2026, Juha's request). Two invented adventure-game
  critics (Ada: puzzles, Mara: story) play from a brand-new caller through
  `bbs-caller` with no memory of earlier rounds and score on a fixed
  100-point rubric. Fix what they found, run two new critics, repeat until
  one round averages 83 or more (stop after round 6). Anything may change
  with a good reason; every change goes in `docs/reviews/CHANGES.md`.
  Loop state in `docs/reviews/LOOP.md` so later runs resume.
  PAUSED by Juha, 14 Nov 2026, after round 3 (Ada 85, Mara 80, average 82.5); round-3 fixes in `bb94e79`, staging check cut off (rerun in Q19). Do not resume until Juha says so.
- [x] **Q19 - Judges' panel, then the critics' plan.** Brief:
  `docs/briefs/Q19-judges.md` (Claude Code, 14 Nov 2026, Juha's request).
  Rerun the staging check on `bb94e79`; add the mesh/FidoNet use case to
  `docs/DEVPOST.md` and `README.md`; two invented judges with no BBS
  background (Hans, ex-PlayStation; Anna, finance) read the Devpost text,
  play about 40 minutes through `bbs-caller`, look at the repo and `.kiro/`,
  and agree scores on the five Kiroween criteria (`docs/reviews/JUDGES-1.md`);
  then Ada and Mara turn that into a ranked action list that keeps the
  game's soul (`docs/reviews/PANEL-1.md`). No game changes in this item.
  DONE 14 Nov 2026. Staging check on the round-3 code (served commit
  `b842f3d`, docs-only on top of `bb94e79`): PASS (server build, ctest, smoke,
  e2e 63 passed 0 failed). Use case added to `DEVPOST.md` and `README.md`.
  Judges (`JUDGES-1.md`), agreed: Impact 5, Technological Implementation 8,
  Design 8, Innovation/Idea 8, Presentation 5, total 34/50 (Hans 35, Anna 34).
  Hans played about 165 calls and locked 4 of 5 manifest rows (stopped at the
  ELIZA row, out of time); Anna played about 60 calls (20 to 25 minutes) and
  stopped at the manifest screen, unsure how to progress without guessing.
  Both Telnet only. Panel (`PANEL-1.md`) top five: (1) rewrite the README to the
  board that ships, +1 Presentation; (2) rewrite the Devpost "In short" with one
  plain BBS sentence first, +1 Presentation; (3) counts match the folder, fill
  or cut every [fill], name who plays and pays, +0.5 each; (4) make the film,
  boot banner first, +1 to +1.5 Presentation, large; (5) small game honesty
  fixes (Diagnostics "this account" line, filed-report feedback, boot banner
  after power-up, scripted "are they alive" reply plus fallback, row status
  line), about +1 Design. Note: the two panel agents were not in the registry,
  so the panel turns ran through the general agent reading those files. The
  panel proposes dropping "feasibility study" and "product" from the first
  paragraph, which is Juha's framing: his call. The PANEL-1 appendix names the
  invented judges; cut that before it goes on the real page.
- [x] **Q20 - Action plan, then the platform usability loop.** (DONE by the 4-round limit, not by SUS 80. Commits 755d0fe..HEAD of Q20, `git log --oneline 6b913af..` plus this tick. **Top decisions for Juha** (`docs/ACTION_PLAN.md`): D1 framing is decided (a game; the mesh one "what next" line); D2 Kouvosto as the reference board, FidoNet credits move there, each claim says which board; D3 invented judge names stay off the real page; D4a film with browser footage now; open game items: manifest too easy (recommend the notebook records evidence, not conclusions), chat with other callers behind ELIZA's choice 1 (recommend renaming the choice), the front page names the first task (recommend keep), the two-minute idle limit drops slow readers (recommend a warning line and 10 minutes), the boot banner after the RTG, timeline mismatches. **UX rounds** (SUS, Ulf / Ines / average; no severe inconsistency in any counted round): 1 47.5 / 50 / 48.75; 2 67.5 / 62.5 / 65; 3 60 / 62.5 / 61.25; 4 72.5 / 70 / 71.25. Still in the way after round 4: the prologue is a puzzle (by design), chat with callers behind ELIZA's scene, Snake does not say moves need no ENTER (port list P30), chat echo order (P31). **Critics** (Addition 3, `docs/reviews/LOOP.md`): round 3 was 82.5; round 4 Ada 83, Mara 75 (unfinished), average 79; the Q20 changes behind it were softened in 3c17c35 (front-page hint for new callers only, shows wait for two quiet seconds); round 5 Ada 82, Mara 80, average 81, and eca17b7 fixed the one Q20 cause found (the scene footer said "no ENTER" where MEMCHK must be typed). Neither critic says the game got easier through Q20; Mara calls the front-page hint a small giveaway (Juha's rule, left as is). Two critic rounds used, the maximum. **Keys that moved** (`docs/PLATFORM_UX.md` section 3): P page SysOp to O, P post to W, R read to L, message list E/L to P/N, ENTER no longer leaves, X main menu and ? keys everywhere, any key for "Press ENTER", yes/no one key (the RTG ACK too, d1e33c3), V and D ask for a file number, chat and shell leave with Q, QUIT, EXIT; Addition 6: L on the own account lists the crew accounts already opened (no password again), no L on a crew account, crew G reads "Goodbye (to Login:)". **Addition 4**: CREW logs the new account in at once (824782e). **Addition 5**: (1) a Ty login fired only Ty's rules already (story property P11 proves it for every crew handle), but his page and his desk scene printed together; fixed by one show per calm prompt (093c8a3); (2) L and G on crew accounts per Addition 6 (ee6db49, 916f58c), R is left out on a crew login with the line "Your crew manifest is on your own account."; (3) letters by account kind in `docs/PLATFORM_UX.md` 1.3: no letter has two meanings. **Addition 6**: `KNOWN_<HANDLE>` on the caller's own account at the first crew login that works; the notebook writes down the card password while it still opens the account; Y (back to the own account on this line) is kept next to G, since it works per line. **RAM and flash** (`pio run`): before Q20 (6b913af) RAM 61716 bytes, flash 1641879 bytes; after RAM 62148 bytes (+432, static only: no heap growth in a hot path), flash 1653071 bytes. Host tests 12 of 12, story lint 0 errors, staging check PASS on eca17b7: server build, ctest, smoke, e2e 63 passed. The server build ran on the staging host, not on this Windows PC. Port list for `../TinyBBS`: `docs/PLATFORM_UX.md` section 4, P1 to P38, nothing edited there.) Brief:
  `docs/briefs/Q20-action-plan-and-ux.md` (Claude Code, 14 Nov 2026, Juha's
  request). Part A: `docs/ACTION_PLAN.md` from the judges, the panel and the
  critics, with decisions for Juha on top (framing, Kouvosto as reference
  board, FidoNet credits). Part B: audit keys, ENTER rules and the pager
  across the engine, write one interaction contract, build it, and test with
  two fresh no-BBS usability testers until SUS 80 (max 4 rounds). Part C:
  the port list for `../TinyBBS` in `docs/PLATFORM_UX.md` (no edits there).
- [x] **Q21 - The notebook keeps everything.** Brief: `docs/briefs/Q21-notebook.md`
  DONE by the two-round limit of Addition 1, not by the 83 goal (rounds 7 and 8 averaged 76.5 and
  78.5). Commits 1d5773f..9d2d2c0 (`git log --oneline ea14565..`) plus this tick.
  **What the notebook collects:** 124 notes in `sd/BBS/STORY/NOTES.INI`, saved as `N_<ID>` on the
  caller's own account the moment the thing is on screen (once; a crew login carries them), listed in
  the order seen with their source: 34 from messages (Lounge, Mission Log, Garden Notes, the four
  mailboxes, the Archive), 42 from files (logs, records, Engineering, the handbook, the journals, the
  tunes, the four order strips), 17 from scene lines, 8 from pictures (Orbit 3 only at 300 baud),
  5 from bulletins and the user list, 5 from ELIZA's chat lines, 4 from WHO ghosts, 4 from
  Diagnostics, 4 from the NOWHERE shell, 1 from the front page. The notes keep evidence, not
  conclusions; the manifest's own conclusion notes are gone (action plan option B).
  **What replays:** every scene seen (S, "Scenes seen", titles from the card, newest last) and, from a
  single note (R), the 17 scene notes and the 5 ELIZA lines. A replay grants nothing, saves no
  marker and no flag, does not follow `then=`, and Q goes back to the notebook (host test in
  `scene_logic_test`, e2e step). Messages and files show where to read them again (area and number,
  or area and file name). The engine also replays trigger pages, ghost pages and story art (`page:`,
  `ghost:`, `art:`); no note on the card uses them yet.
  **Checks on the story:** `check_notes` in the story lint fails on a clue of `docs/PUZZLE_CHAIN.md`
  (M1 to M9, P1 to P12) or of the story bible's evidence matrix (E01 to E48, E06b) with no note, and
  on a note that points at nothing on the card. Host test `notes_logic_test`: a noted line saves its
  flag once, area= keeps repeated subjects apart, the card's file loads.
  **Critic on the notebook (R6, Ada, 80):** it helped as a memory aid across two sessions, the logins
  page most; it solved nothing; it gave away "mostly no": one note set 22 Nov beside the tank date,
  and that note (`WAKE_CREW`) was trimmed to ELIZA's words. Mara R8: "The notebook that collects clues
  automatically is very good interface design."
  **Addition 1:** every open area-2 row of `docs/ACTION_PLAN.md` is done and marked with its commit
  (idle warning and 10 minutes, ONLINE banner, timeline fixes, Diagnostics 201, row status line,
  filed-report line, scripted "are they alive" and one fallback, Ty's card border, second clue to S);
  "Welcome back, caller." twice is a glitch line and stays. Round 7 fixes of what both critics hit:
  5bbf023. Rounds: 7 Ada 68 / Mara 85, 8 Ada 84 / Mara 73. Neither says the game got easier; both
  still call the manifest matching. **Open for Juha** (ACTION_PLAN, new row): manifest option C, and
  the small items both critics still hit (keys typed during a scene or login, the redial banner after
  a dropped line, News Wire on a crew account, `release` in the shell's help).
  **RAM and flash** (`pio run`): before (Q20 tick) RAM 62148 bytes, flash 1653071 bytes; after RAM
  62572 bytes (+424, static only), flash 1675159 bytes (+22088). Notes are collected per shown
  thing, not per tick; the idle check allocates only when it prints.
  **Checks:** `pio run` SUCCESS; `python tools/run_host_tests.py` all 13 passed (zig c++, Windows);
  `python tools/story_lint.py` 0 errors, 0 warnings; `tools/staging-check.ps1` at 9d2d2c0: STAGING
  CHECK PASS (server build, ctest 14 of 14, E2E 63 passed, 0 failed, the game finished). The server
  build itself cannot run on this Windows PC; it ran on the staging host. `main` is pushed to
  `staging`. **Deploy commit for OpenClaw: 9d2d2c0** (the tick commit after it changes only this file).
  Was: (Claude Code, 15 Nov 2026, Juha's request). Builds Q20 Additions 7 and 8:
  every hint collected when shown, scenes and pages replay from the
  notebook, story lint `check_notes`; then Addition 1 (every open game
  item in `ACTION_PLAN.md` area 2, Kiro's recommendations) and two critics
  to 83. Re-read the brief before ticking: do not tick with Addition 1 undone.
- [x] **Q22 - Polish: the last critic items.** Brief: `docs/briefs/Q22-polish.md`.
  Done 15 Nov 2026 (Kiro). **Scores:** round 9 Ada 82 (finished) / Mara 76 (turn limit at MEMCHK),
  average 79; round 10 Ada 82 (stopped at the last gate) / Mara 82 (turn limit after KESTREL4),
  average 82. **Goal 83 not reached in the two rounds**; neither critic calls the game easier.
  **What changed:** manifest option C (options worded by kind, two fit each fate, the evidence
  decides; story bible and PUZZLE_CHAIN follow); Locks and Six digits no longer say where each
  password is; contract C7 changed: keys typed during a scene are dropped and a grey line says so
  (PLATFORM_UX, steering); SPACE at "Press any key" is a key; the powered banner on a redial once
  the RTG is on, and the powered ship console on a signed-in line (CONSOLE_ON art); News Wire checks
  the caller's own flags; the shell's help lists `release` with its usage and says the manifest is
  the report; Ty's cause and the capsule tied to the phase one budget. **Disconnects: the board.**
  The idle check did `now - lastAt` unsigned while a key in the same tick stamped `lastAt` later,
  wrapping to about 49 days and closing the line at once (`msSince`, 814b876); LOGOFF now logs
  why. Also Telnet keeps output a full send buffer did not take. Port list P48 to P50.
  **RAM and flash** (`pio run`): before (Q21 tick) RAM 62572, flash 1675159; after RAM 62596
  (+24), flash 1676087 (+928). **Checks:** `pio run` SUCCESS at 814b876; host tests all 13 passed;
  story lint 0 errors, 0 warnings; `tools/staging-check.ps1` at 814b876 STAGING CHECK PASS (server
  build, ctest, E2E 63 passed, the game finished). The server build cannot run on this Windows PC;
  it ran on the staging host. `main` pushed to `staging`. **Deploy commit: 333bf19.**
  Open for Juha (ACTION_PLAN, new row): ELIZA's free chat, MEMCHK through option 1 only, the pair
  rule with option C, a scene taking the BUDGET.MEM open.
  Was: Brief: `docs/briefs/Q22-polish.md`
  (Claude Code, 15 Nov 2026, Juha's request: take the recommendations, go
  for gold). Manifest option C, crew-password clues, key lag around scenes,
  redial banner, News Wire, shell help, disconnects; two critics to 83.
- **GOLD: Q23 and Q24 in gated slices G1 to G9 (rev. 2).** Read
  `docs/reviews/G0-juha.md`, then `docs/briefs/Q23-Q24-SLICES.md` (wins over
  the two briefs). Difficult, never frustrating. Each slice: build, one agent
  playtest, stop; Juha tests once; then the next.
- [ ] **G1 - Friction.** Built b73b000..ccb06eb (only Q skips, grace, M7
  pointer); still to do: case-blind passwords, crew logins answer wrong
  passwords, no identical answer twice, the playtest.
- [ ] **G2 - One chat (free text, help lists options, RiveScript brain).**
- [ ] **G3 - Story pass, design (crew's plan as trail, intent ladder, darker acts, new verbs).**
- [ ] **G4 - Story pass, build.**
- [ ] **G5 - Posting and the radio club.**
- [ ] **G6 - Three trials and ELIZA's orders.**
- [ ] **G7 - Forged order and the ending confrontation.**
- [ ] **G8 - Hints (the safety net).**
- [ ] **G9 - Ambience (options first, Juha picks).**
