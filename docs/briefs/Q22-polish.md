# Brief Q22 - Polish: the last critic items

From: Claude Code (planner), at Juha's request, 15 Nov 2026. To: Kiro.

Juha: "Go for gold." Take the recommendations. Q21 ended at 78.5 (Ada 84,
Mara 73). Fix the items in `docs/ACTION_PLAN.md` area 2 row "Q21 critic
rounds 7 and 8" (read `docs/reviews/R7-*.md`, `R8-*.md`, `LOOP.md`):

1. **Manifest: option C.** Reword the options so at least two fit each
   fate on the face of it and only the evidence decides; the right pairs
   can no longer be found by swapping. The notebook keeps evidence, not
   conclusions. Story bible and `docs/PUZZLE_CHAIN.md` follow; the story
   lint and the property test stay green.
2. **Crew passwords over-clued** (Ada R8): trim so each password needs
   noticing, not just reading one line. Keep at least one fair route.
3. **Keys late or lost around scenes and logins** (both): a key typed
   during a scene or a login must never land on the next prompt as a
   surprise. Drop type-ahead at the end of a scene and after a login
   banner, or show it, whichever the contract (C7) allows; update the
   contract and steering if it changes. Engine: port list.
4. **Redial after a dropped line** shows the brownout banner; a caller
   with a powered account (or a powered board) sees the powered banner.
5. **News Wire** in a crew account's bulletin list checks the caller's
   own flags as other crew-login lists do.
6. **The shell's help** lists `release` (with its usage) before the
   manifest is filed, and says the manifest is the report (Mara R8).
7. **Disconnects** both critics saw: find the cause in the staging event
   log; fix it if it is the board, say so if it is the tool.

Then both critics, one at a time, fresh boards, the next round in
`LOOP.md`. Goal 83 without making the game easier. At most two rounds;
between them fix what both hit. Mark rows done in `ACTION_PLAN.md`;
engine items on the TinyBBS port list in `docs/PLATFORM_UX.md`.

Checks as always: `pio run`, host tests, story lint, staging check with
the e2e finishing the game; push `main` to `staging`. Commits prefixed
`Q22:`. Re-read this brief before ticking. The tick gives the scores, what
changed, RAM and flash, the staging result and the deploy commit.
