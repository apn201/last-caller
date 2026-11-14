# Brief Q21 - The notebook keeps everything

From: Claude Code (planner), at Juha's request, 15 Nov 2026. To: Kiro.

Q20 was ticked before its Additions 7 and 8 were built (they arrived
during its last run). This item builds them. Read
`docs/briefs/Q20-action-plan-and-ux.md`, Additions 6, 7 and 8, and follow
them exactly; this brief only sets the order and the checks.

1. **Hints collected on show (Addition 8).** Every clue, code, password,
   handle, name, number or direction the game shows goes into the
   notebook the moment it is on screen, with its source and in the order
   seen. Marked on the card (`note=` keys or a `NOTES.INI` table), saved
   as a flag on the caller's own account, carried from crew logins.
   Keeps what was seen, never adds answers. This also answers the
   critics' "manifest too easy" item in `docs/ACTION_PLAN.md`: the
   notebook records evidence, not conclusions.
2. **Replay (Additions 7 and 8).** Scenes seen, ELIZA's pages and story
   art can be shown again from the notebook as they were; a replay grants
   nothing and changes no flag; Q goes back to the notebook. Messages and
   files are pointed to (area and number) or opened.
3. **Checks on the story.** Every clue in `docs/PUZZLE_CHAIN.md` and the
   story bible has a note; the story lint gets `check_notes` and fails on
   a clue without one. Host tests: a noted line saves its flag once; a
   replay grants nothing.
4. **Port list.** The engine part (notes table, collect on show, notebook
   viewer, replay) goes on the "Port to TinyBBS" list in
   `docs/PLATFORM_UX.md`; the notes are HALCYON.
5. **Fun check.** One critic play-through (Ada or Mara, agent files
   unchanged, a fresh board, recorded in `docs/reviews/LOOP.md`): does the
   notebook help without giving the game away? If she says it gives
   answers away, trim the notes that do and say which in the tick.

Rules as in Q20: card content not code, the real `SYSOP` flag never
granted, never show an option that does not work, no heap growth in hot
paths (RAM and flash before and after). After the build: `pio run`, host
tests, story lint, the staging check with the e2e finishing the game.
Push `main` to `staging` when they pass. Small commits prefixed `Q21:`.
Never claim a check passed if it did not run.

## Done when

All of the above is built and checked, and the tick in
`docs/briefs/QUEUE.md` gives: what the notebook collects (count of notes
by source), what replays, the critic's score and comments on the
notebook, RAM and flash, the staging result, and the commit range. The
tick ends with the commit Juha can give OpenClaw to deploy.

## Addition 1 (Juha, 15 Nov 2026) - finish the game items, go for gold

Juha's decision: take every recommendation in `docs/ACTION_PLAN.md`. Do
all of area 2 (game) that is still open or marked "new item" for Kiro,
in this item, after the notebook:

- Manifest: option B, the notebook records evidence ("suit log: 14
  refusals"), not conclusions; filing must take thinking again.
- Front page: option A, keep it as it is.
- Idle limit: a warning line at 90 s, and 10 minutes for callers.
- Boot banner: a second banner for a powered caller (no more ELIZA NOT
  RESPONDING and SOLAR BUS ONLY after the RTG), via `tools/ansi/`.
- Timeline mismatches (bulletin 4 date, Diagnostics 300 vs DOWNLINK.CFG,
  Anders's mark, 8 MB vs 16384K, mail #115 while the ground line is
  down): check each against `docs/PUZZLE_CHAIN.md` and the story bible;
  fix every one that is not a clue, leave clues alone and say why.
- The other area-2 honesty fixes in the schedule (Diagnostics line,
  filed-report feedback, scripted ELIZA reply, row status line, News
  Wire) and any area-2 row owned by Kiro that is not done yet.

Story or canon changes go in the story bible and `docs/PUZZLE_CHAIN.md`;
mark each row done in `ACTION_PLAN.md` with its commit. The critic
play-through in step 5 comes after these, so it plays the finished game:
run both critics (Ada and Mara, one at a time, fresh boards) instead of
one, recorded as the next round in `docs/reviews/LOOP.md`. Goal: average
83 or more without making the game easier. If it falls short, fix what
both hit and run one more round (at most two rounds in this item).
Then the checks, push, and the tick with the deploy commit.
