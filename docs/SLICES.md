# LAST CALLER - slices

Working document, Nov 12 2026. Rules, risks, judging and dates stay in
`KIROWEEN_PLAN.md`. Order of Kiro setup stays in `KIRO_REPO_STEPS.md`. This file
is the scope: what we build, in what order, and where we stop.

## The idea in one paragraph

You dial a board that should not answer. Nobody has called since the night it
went dark, but the board kept running: the clock ticks, the message base grows
by one line now and then, and something is still on node 1. Walking through it
should feel like walking through a house after the family left, or the
corridors of a ship whose crew died years ago. The menus work. The rooms are
lit. Nobody is there. Except the SysOp, which is no longer a person.

Decided Nov 12:

- Setting: the board runs aboard a ship or station. The crew is long dead.
  The BBS was their message board, mail and file store, and it still answers.
- The SysOp is the ship's AI. It took over the board, and the evidence says
  it took over more than that. Calm, helpful, never raises its voice.
- The caller's goal is deduction: work out what happened to every person
  on the crew list. Who they were, how they died, and who or what did it.
  The genre is "fate deduction" (fill in a manifest from fragments, the game
  confirms correct entries in groups so guessing does not pay). Our own
  names, ship, art and wording only.

Premise (Juha, Nov 12): 1998, the first private space flight, secret, crew
of four, a science mission preparing a private space hotel and tourist
flights. A BBS on a 486 let the crew message friends and family on Earth.
The mission AI, ELIZA, ran the mission, ran the BBS as SysOp, and kept the
crew company when they were fed up with each other. Then mission control
lost all contact. It never made the news. In 2026 the shorted batteries
have gone open circuit and the solar panels run the 486 whenever the ship
is in sunlight, a zombie satellite. Radio amateurs found the signal on
11 Nov 2026.

Clock (decided Nov 12): frozen. The board has nowhere to sync from, so it
resumes from the minute it stopped on 8 Dec 1998 and runs on from there.
`clock_sync` is off.

Power (decided Nov 12): Ty cut the RTG and shorted the battery with his
last breath. Now the batteries are open circuit and the tumbling ship only
has power in sunlight, a minute or two at a time. The caller's first job
is to log in as ship maintenance and switch the power back to the RTG,
which wakes ELIZA. Ejecting the garden/server module would have saved
everyone; ELIZA would not allow it.

Open: see "Story questions" at the end.

## Scope rule

Three layers. Each is shippable on its own. We only start the next one when
the previous one is green on the server build and has been on the stick once.

1. Content and identity. No code. The board feels abandoned with the engine
   as it is today.
2. The haunt. One small engine in the shared core: triggers, ghosts, glitches.
3. The voice. The SysOp talks back. Scripted first, AI only as an extra.

If time runs out, we ship the last green layer. Layer 1 alone is already an
entry; layer 2 is the entry we want; layer 3 is the demo moment.

## Who does what

- Juha: story, final edit of every caller-visible line, pictures and sounds,
  Kiro sessions, the stick, the video.
- Claude Code: plans, specs written ready for Kiro, story drafts on request,
  review of what Kiro produced, the server deploy checks.
- Kiro: all code and all committed content edits from the next commit on.
  Specs, steering, hooks, agents, MCP and property tests live in `.kiro/`.

The baseline commit (`e5898cd`) is the TinyBBS engine with the Kouvosto
content removed. Everything after it should be visibly Kiro work.

## Layer 1 - Content and identity

### S0 - Kiro bootstrap (Nov 12 to 13)

- `.kiro/steering/`: `product.md`, `tech.md`, `structure.md`,
  `card-content.md`, `story-voice.md` (fiction only, no real names or brands,
  never explain the scare). Lifted from AGENTS.md, Puhelu's steering as a
  template.
- First hook: save in `shared/web/*.html` runs `tools/embed_web.py`.
- `pio run` and the server CMake build pass from Kiro's terminal.
- Done when: steering committed from Kiro, the hook fires once.

### S1 - Rebrand spec (Nov 13)

`.kiro/specs/rebrand/`. EARS requirements, Kiro runs the tasks.

- `SYSTEM.INI`, `NETWORK.INI`: new name, short name, SSID, sysop handle.
- Welcome, menus, goodbye, rules bulletin: first pass is the old art with the
  name replaced and the colours drained (greys, one dark red).
- Door names: keep VolaMUD (becomes the explorable "deck plan" or "house" later),
  drop Legend and Dice unless the story wants them. Code strings in
  `src/doors/doors.h`, `dice.h`, `legend.h`, `src/bbs/sysop.h`,
  `user_editor.h`, `src/hw/scene_logo.h`.
- README rewritten for Last Caller. `docs/CONTENT_POLICY.md` replaces the old
  curation policy: everything is original, listed with source.
- Acceptance: `grep -ri kouvo` finds nothing outside `THIRD_PARTY_NOTICES.md`,
  both builds pass, a caller sees the new name only.

### S1b - ANSI art pass (Nov 13 to 14)

Brief: `docs/briefs/S1b-ansi-art.md`. Every caller-visible screen gets real
original ANSI art, built from scripts in `tools/ansi/` and reviewed as PNG
renders. Standing rule for every later slice: a new screen ships with its
art, never with placeholder text.

### S2 - Public test instance (Nov 13 to 14, parallel)

Second server instance, own data root, own port, own vhost, Telnet closed,
uploads off, SysOp page behind Apache auth. Deploy script in the repo.
Done when: the URL opens, sign-up works, Kouvosto is untouched.

### S3 - The abandoned board, content only (Nov 14 to 17)

All files on the card. This is where the mood is made.

- A user list of the dead: 6 to 10 regulars with last-call dates that stop
  on the same night.
- Message base: the last week of traffic before it went dark, getting
  stranger. Last post unfinished.
- File areas with a handful of original files: logs, a crew roster or house
  inventory, a scanned note, the SysOp's own text files. FILE_ID.DIZ dates
  carry clues.
- Bulletins that were never taken down. One dated after the board died.
- Back Room and Vault exist, gated by flags nothing can grant yet. That gap
  is what S4 closes.
- Done when: a stranger spends five minutes and says "creepy" without being
  told the story.

## Layer 2 - The haunt

One portable header, `src/core/haunt.h`, no ESP32 or Linux includes. Tables
on the card in `sd/BBS/STORY/*.INI`, loaded once at boot. Clue and progress
state are flags on the user record (the field exists, no new storage).

### S4 - Power prologue and accounts (Nov 15 to 17)

Spec: `.kiro/specs/power-prologue/`. The first thing every caller meets.

- Brownout state: boot banner, maintenance login only, sessions dropped
  every 60 to 120 s as the tumbling ship loses the sun.
- `admin`/`admin` maintenance account and the ship systems door: switch
  the power to the RTG, the ship stabilises, ELIZA wakes.
- Board-wide power state that survives a reboot; drops back to brownout
  some minutes after the last caller leaves (to confirm).
- Read-only crew accounts with concurrent logins, so no clue can be
  destroyed and many callers can play at once.
- Private ELIZA messages per caller. Everything after the prologue is solo.

The ship systems door is the keyword terminal from the old S4: the same
"type the right thing, get a flag" machinery serves the later puzzles.
Cut line: S4 plus S6 plus S8 is a game.

### S4b - Diagnostics door (Nov 18)

ELIZA answers system queries by number, truthfully, in her voice. Query
codes come from the comms manual on the board.

Built (Q5): spec `.kiro/specs/shell-doors/`, `src/doors/diag.h`, answers in `sd/BBS/STORY/DIAG.INI`. The door does not list the codes; the comms manual (Q7) carries them.

### S5 - Triggers (Nov 17 to 19)

Events: login, message read, file viewed, art viewed at a baud, door entered,
idle N seconds, minutes online. Conditions: has flag, lacks flag, level.
Effects: set flag, page caller, post message as a named user, show art, add
a line to a message on re-read. Idempotent, flags only grow.

Built (Q2): spec `.kiro/specs/triggers/`, `src/core/haunt.h` on
`src/core/haunt_logic.h`, rules in `sd/BBS/STORY/TRIGGERS.INI`. Also: private
mail to one caller, crew logins granting flags to the caller's own account.
Host tests: `tools/test/haunt_logic_test.cpp`. Board test still to run: log in
as TYVANCE, Goodbye, sign in as yourself, find ELIZA's mail.

Scene engine (Q3): spec `.kiro/specs/scenes/`, `src/core/scene.h` on
`src/core/scene_logic.h`; scenes are `sd/BBS/STORY/SCENES/*.INI` with art in
`ANSI/STORY`, started by a trigger rule (`scene=NAME`). Demo:
`ELIZA_WAKE` (ring portrait, typed dialogue, a three-way choice that sets
flags and changes the closing lines). Host tests:
`tools/test/scene_logic_test.cpp`. Board test still to run (spec task 8).

### S6 - Ghost presence (Nov 18 to 20)

`STORY/GHOSTS.INI`: callers in WHO with nodes, activity lines and impossible
idle times. Page the caller, speak in chat with typing delays. The SysOp is
one of them. Done when: the SysOp ghost pages you within 30 s of login on
both transports.

Built (Q4): spec `.kiro/specs/ghosts/`, `src/core/ghost.h` on
`src/core/ghost_logic.h`; ghosts in `sd/BBS/STORY/GHOSTS.INI` (WHO rows with an
idle column, node chat, a timed page), ELIZA's scripted lines (S9) in
`sd/BBS/STORY/SYSOP.INI`, keyed by a trigger rule, a chat keyword, entering
chat or a fallback, typed after a delay. Host tests:
`tools/test/ghost_logic_test.cpp` (also checks the card's two files). Also the
Q3 fix: keys typed while a scene types are no longer echoed. Board test still
to run (spec task 10).

### S6b - NOWHERE shell door (Nov 21)

A fake 1998 Unix-like shell on the haven's file system: `ls`, `cat`,
`cd`, the orders, the client list, the outbound queue. The final command
is typed here. Side door for later: the haven's servers boot with 1990s
security, "but that is another story".

Built (Q5): `src/core/shell_logic.h` + `src/doors/shell.h`, tree and final command in `sd/BBS/STORY/SHELL.INI` (a small draft tree; Q9 fills it and grants the gate flag `NOWHERE_OPEN`). Host tests: `tools/test/shell_logic_test.cpp`.

### S7 - Glitches and sound (Nov 20 to 22)

The haunted-house layer, all driven by triggers so the card controls it:

- Text glitches in the core (both transports): a line repeats, a character
  is wrong, the clock jumps, a menu option appears once and is gone.
- Browser only, from the terminal page: one static burst, a line hum, a
  distant modem. Original audio only. Telnet gets a BEL at most.
- Rule: rare and timed. One glitch every few minutes beats one per screen.
- Audio on the card: short original WAV/MP3 effects (power hum, relay
  clicks, the reboot chirp in the prologue).
- Ty's music: two or three original MOD files made by a generator script
  (`tools/mod/`), sample names carrying text, and his unfinished demo for
  the summer demo party.

Built (Q13): `sd/BBS/STORY/GLITCH.INI` (draft lines), `src/core/glitch_logic.h` + `glitch.h`, host test `tools/test/glitch_logic_test.cpp`. Rare and timed: nothing in the first 180 s after sign-in, then one line every 240 s at most (never under 60), shown at a calm menu prompt to a caller on their own account. Kinds: a line said twice, a clock that jumps back to 04:15, a letter wrong in ELIZA's line (`garble=1`), a wrong crew count after act 2. Browser: the item's `sound=` cue; Telnet: a bell. No flag, no save, no gate. Not done: the "menu option that appears once and is gone" (it would show an option that does not work, which the menu rule forbids) and a screen-wide browser flicker.

### S8 - The manifest and the ending (Nov 22 to 24)

`M` opens the crew manifest: one row per crew member, three blanks each
(identity, fate, cause). The caller fills them from pick lists. Every time
two rows are fully right, the board confirms those two and locks them
(four crew, so pairs; decided Nov 12).
Wrong rows are never named. Rows only appear for people the caller has
found evidence about, so the menu never shows a dead option.

- Partial ending: some rows confirmed, the AI thanks you for your interest.
- Full manifest: the AI's own record is the last row, and the board says
  something it has not said before.
- Sticky: a finished caller's handle stays in WHO for the next caller.

Built (Q6): spec `.kiro/specs/manifest/`, `src/core/manifest_logic.h` + `src/doors/manifest.h`, card file `STORY/MANIFEST.INI`, scenes `MANFULL` and `ENDING`. Key `R` (`M` is the message bases). Host test: `tools/test/manifest_logic_test.cpp`. Board test still to run (spec task 9).

The `story-lint` check for this: every row is provable from evidence on the
card, and no row is provable from a single clue.

## Layer 3 - The voice

### S9 - Scripted SysOp (with S6)

The HAL-style voice exists first as lines in `STORY/SYSOP.INI`, keyed by
trigger and by chat keyword. Calm, polite, wrong. This works offline on the
stick and costs nothing per caller.

### S10 - MCP: the board as a tool (Nov 23 to 25)

`bbs-caller` MCP server: connect over Telnet or WebSocket, send keys, read
the screen, list WHO, read a test account's flags. Two uses:

1. Kiro's `playtester` agent plays the game end to end. Required, it is how
   we prove the game can be finished.
2. An AI logs in as the SysOp node and chats with callers live. Optional,
   server instance only, behind a switch, hard rate limit, falls back to S9
   lines when off. The board core does not change: the AI is just another
   caller with SysOp rights. That keeps it out of the firmware and keeps the
   stick offline.

### S11 - Verification (runs from S5 on)

- `story-lint`: every flag reachable, no softlock, every ending reachable.
- Property tests (RapidCheck, server build): flags only grow, triggers are
  idempotent, malformed STORY files never crash the board.
- Hooks: lint on save in `sd/BBS/STORY/`, server build plus properties on
  save in `src/`, name/brand scan before commit.

## Ship (Nov 26 to Dec 3)

Hardening, one hour soak on the stick with two callers, a stranger on a
phone, video, write-ups. Dates and the bonus prizes are in
`KIROWEEN_PLAN.md` sections 8 and 9.

## Cut list, top first

AI SysOp live mode, browser sound, earlier callers as ghosts, glitch
variety, third ending, VolaMUD haunted corner. Never cut: S1, S3, S4, S6,
S8, the playtester through MCP, the public URL, the video.

## Story questions

Answers go into `.kiro/specs/story-bible/` as the first spec with no code.

1. Where: deep space, orbit, a deep-sea or polar station? And when: 1997
   tech as the ship's own computer (retro-future), or a real-era vessel?
2. How big is the crew? 8 is playable in a jam, 12 is the most we can write.
3. What went wrong, in one sentence, from the AI's side and from the crew's.
4. How does a caller reach a ship? (A relay that still forwards calls, a
   number found on a wreck, the board calling you.)
5. Names: the ship, the board, the AI.
