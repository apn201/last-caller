# Brief Q20 - Action plan from the judges, then a usability loop for the platform

From: Claude Code (planner), at Juha's request, 14 Nov 2026. To: Kiro.

Inputs: `docs/reviews/JUDGES-1.md` (agreed 34/50), `docs/reviews/PANEL-1.md`,
the Q18 reviews (`docs/reviews/R3-ada.md`, `R3-mara.md`) and Juha's notes
below. Submission is 3 Dec 2026.

## Part A - The full action plan

Write `docs/ACTION_PLAN.md`: every action from the judges, the panel and the
round-3 critics, deduplicated, in one table per area, each with: what, why
(which review and criterion), expected effect, cost, risk to the soul,
owner (Kiro or Juha), and the queue item it belongs to. Areas:

1. **Platform (TinyBBS engine)**: anything about menus, keys, prompts,
   pager, scenes eating input, chat exits. These go to Part B, and from
   there to both repositories.
2. **Game (Last Caller only)**: story, puzzles, clues, ELIZA lines.
3. **Devpost text and README.**
4. **Video plan.**
5. **Repository and Kiro story.**

Then a dated schedule to 3 Dec: what is done in which order, what must be
done before filming, what waits until after submission. Put the open
decisions for Juha at the top, each with the options and a recommendation.
Known ones:

- **Framing: DECIDED by Juha (14 Nov 2026), not open.** Frame it clearly
  as a game, on the Resurrection theme: "We resurrected BBS technology to
  study its feasibility as a game engine." Other uses worth studying (a
  LoRa/Meshtastic text interface, FidoNet-style sync over a mesh, etc.)
  are named as possible directions that were out of scope for this
  hackathon. So the first paragraph leads with the game; the mesh is one
  honest "what's next" line, not a second product. Plan the Devpost and
  README rewrite to this framing.
- **Kouvosto is missing.** The Q19 brief asked for `kouvosto.helppox.com` as
  the reference board (the same TinyBBS engine as an ordinary BBS, seeded
  with about 1000 real FidoNet messages), and it is in neither
  `DEVPOST.md` nor `README.md`. The panel then proposes deleting the FidoNet
  credits as content that "does not ship". Both are partly right: the
  FidoNet seed does not ship on HALCYON, but it is live on Kouvosto, which
  proves the engine is a real BBS. The plan must resolve this so every
  claim is accurate: say which content is on which board.
- The invented judges' names must not appear on the real Devpost page.

Commit the plan before starting Part B. Do not apply the Devpost, README,
game or video items in this item: Juha reviews the plan first. The
platform items are the exception (Part B).

## Part B - Usability and consistency loop for the platform

Juha's notes (14 Nov 2026), confirmed by the judges:

- Command letters are not consistent: the same letter (M, P, F, ...) means
  different things in different menus. Free letters may be scarce, but the
  same action should have the same key everywhere.
- Sometimes a key acts at once, sometimes it needs ENTER.
- The pager is used only for multi-page texts, so the controls change with
  the length of the document. Use the pager always, for one consistent
  experience.
- These quirks are period correct: real BBSes were built from pieces that
  did not quite fit (like a spaceship; see Apollo 13's square and round CO2
  filters). To a modern user they are only a hindrance. Keep the 1990s
  look, the ANSI art and the voice; remove the friction.

Do this as its own loop:

1. **Audit.** Every screen, menu, prompt, pager and door in the engine
   (start from `docs/MENU_AUDIT.md`): its keys, whether each needs ENTER,
   what Q, ENTER, ESC and ? do, how text is shown, how you leave. Record it
   as a table in `docs/PLATFORM_UX.md` with every inconsistency numbered.
2. **The contract.** Write one interaction contract in
   `docs/PLATFORM_UX.md` and as steering (`.kiro/steering/interaction.md`):
   which keys are global and mean the same everywhere (back, help, quit,
   next, previous, main menu), single key versus ENTER (one rule, said on
   the screen when it matters), every text display through the same pager
   with the same footer and keys whatever its length, how a scene or a
   timed event may never swallow a key or cut a line, how every chat and
   door is left. Where letters clash, keep the same key for the same kind
   of action and move the odd one out; record each moved key. Both
   transports (Telnet, browser) follow the same contract. The browser
   terminal may add buttons, never different rules.
3. **Build it** in the shared engine (`src/core`, `src/bbs`, `src/doors`,
   `shared/web/`), small commits prefixed `Q20 UX:`. Update
   `docs/MENU_AUDIT.md`, the in-world help screens and every story file
   that names a key (`sd/BBS/`: change only the key names in text, not the
   story), the e2e and the `bbs-caller` tool if they depend on old keys.
   Story puzzles that rely on a key (for example `S` replaying at modem
   speed) keep working; if a key moves, every clue that names it moves too
   and the story lint must check it.
4. **Test with new users.** Write two usability-tester agents
   (`ux-tester-1.md`, `ux-tester-2.md`): invented people who never used a
   BBS or a terminal (base them on Hans and Anna, but with fresh sessions
   and no game goals). Each round, on a fresh staging board, each tester
   does a fixed task list without help: log in as a new caller, read a
   bulletin, find and read a message, reply to it, post a new one, list and
   view a file, open and leave a door, use node chat and leave it, find
   help, go back to the main menu from anywhere, log off. They note every
   moment they were unsure which key to press, pressed a key and nothing
   happened, needed ENTER or did not, or got lost; then score the System
   Usability Scale (10 questions, 0 to 100). Keep the task list and
   testers fixed across rounds.
5. **Loop.** Fix, retest with fresh sessions. Done when both testers'
   SUS average is 80 or more and no tester hit an inconsistency rated
   severe, or after round 4 (then report what is still in the way).
   Record rounds in `docs/reviews/UX-LOOP.md`.

Rules: the story never grants the real `SYSOP` flag; never show a menu
option that does not work for that caller on that transport; content on
the card; no ESP32 heap growth in hot paths (report RAM and flash before
and after). After each fix round: `pio run`, host tests, story lint, the
staging check; the scripted e2e still finishes the game. Never claim a
check passed if it did not run.

## Part C - Hand-over to the TinyBBS repository

The engine came from `../TinyBBS` (GitHub `apn201/TinyBBS`; Kouvosto runs
it). In `docs/PLATFORM_UX.md`, end with a section "Port to TinyBBS": each
platform change, its Last Caller commits, the files it touches, what is
engine (port it) and what is HALCYON story (do not port), and anything in
TinyBBS that differs and needs care (Kouvosto doors such as Legend, dice
and VolaMUD still exist there). Do not edit `../TinyBBS` in this item; the
port is a separate item run inside that repository.

## Done when

`docs/ACTION_PLAN.md` is committed; the UX loop reached SUS 80 with no
severe inconsistency, or ran 4 rounds; all checks pass; `PLATFORM_UX.md`
has the audit, the contract and the port section; and the tick in
`docs/briefs/QUEUE.md` gives: the top decisions for Juha, the UX rounds and
scores, keys that moved, RAM and flash before and after, and the commit
range.

## Addition (Juha, 14 Nov 2026) - do this before UX round 2

The UX testers could not get past `Login:` (no way to register is shown).
Juha's decision: the login screen (front page) says somewhere, in the
board's voice and art, that this is a puzzle game and the first task is to
work out how to fix the power and create an account. Keep the prologue a
puzzle: do not name the RTG, the switch or the command. Card text only
(`sd/BBS/`), ANSI via `tools/ansi/` if the art changes; story lint and the
e2e must still pass. Note it in `docs/ACTION_PLAN.md` area 2 (already marked
DECIDED) and in the UX-LOOP round 2 fixes.

## Addition 2 (Juha, 14 Nov 2026) - fix everything, then round 2

- Fix every round-1 finding before round 2, the game ones too: the login
  hint above, and the old power-fault screen that returns after CREW (say
  plainly that the account is made and what to do next). Then run UX
  round 2 and carry on the loop as written.
- Everything that is not HALCYON story (engine, menus, keys, pager, prompts,
  chat, doors such as Snake, the browser terminal, the e2e and caller
  tools) goes on the "Port to TinyBBS" list in `docs/PLATFORM_UX.md` as you
  make it, with its commits and files, not only at the end. Update the list
  after every round, so nothing found in testing is lost for `../TinyBBS`.
  Still do not edit `../TinyBBS` in this item.

## Addition 3 (Juha, 14 Nov 2026) - keep the puzzles hard, then the gamers again

- Ulf and Ines are deliberately impatient. They are the right test for
  the platform: keys, prompts, pager, leaving things. They are the wrong
  judges of the puzzles. When they are stuck on a puzzle (the prologue,
  a clue, a gate), that is the game working. Fix the friction around a
  puzzle (a key that does nothing, an unclear prompt, no way out), never
  the puzzle itself: no hint that names the answer, no shortcut past a
  gate. Juha's front-page rule stands: say it is a puzzle and where to
  start, not how to solve it.
- When the UX loop is done (SUS 80 or round 4), run one more round of the
  Q18 critics, Ada and Mara, before ticking Q20: the agent files and
  rubric unchanged, fresh boards, one critic at a time, recorded in
  `docs/reviews/LOOP.md` as round 4. The point is to check the usability
  work did not take the fun out. Round 3 averaged 82.5. If the average
  drops, or either critic says the game got easier, more hand-held or
  more explained, find which Q20 change did it and undo or soften that
  change, then run the critics again (at most two critic rounds here).
  Put both critic scores in the Q20 tick.

## Addition 4 (Juha, 14 Nov 2026) - no second login after CREW

When CREW creates the caller's account, the caller is logged in to it
straight away: no second `Login:`. Keep the account-made note, then go on
as a fresh login of that account would (welcome, new mail, main menu).
The real `SYSOP` flag is still never granted. Update the e2e, the
`bbs-caller` flow, the story lint if it checks the login, and any story
text or clue that tells the caller to log in again. Engine part (log a
session in to a newly made account) goes on the TinyBBS port list if it is
not HALCYON-only.
