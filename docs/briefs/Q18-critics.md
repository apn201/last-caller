# Brief Q18 - Critics' loop to 83 %

From: Claude Code (planner), at Juha's request, 14 Nov 2026. To: Kiro.

Two respected adventure-game critics play HALCYON from a brand-new caller
through the `bbs-caller` MCP tool and score it. You fix what they found and
run two new critics, with no memory of the earlier rounds. Repeat until the
average of the two scores in one round is **83 or more**.

## 1. The critics (write these first, then never change them)

Write two Kiro agents in `.kiro/agents/`, modelled on `playtester.md` (check
the current Kiro agent format; do not guess): `critic-ada.md` and
`critic-mara.md`. Both are invented people, not real reviewers. Both are
senior reviewers who specialise in adventure games and have played
everything from Infocom and Sierra to Myst, Grim Fandango, Return of the
Obra Dinn, Her Story, Outer Wilds and Disco Elysium.

- **Ada** reviews for a long-running adventure magazine. Her favourite
  game is Return of the Obra Dinn. She cares most about puzzle design:
  deduction, fairness, logic, clueing, the "aha" moment, whether each gate
  opens by noticing and thinking, whether the interface fights her.
- **Mara** writes long-form criticism of games as narrative. Her favourite
  series is Leisure Suit Larry: she loves Sierra-era cheek, comic timing,
  characters with flaws, and a game that does not take itself too
  seriously. She cares most about story, characters, humour, prose, pacing,
  tone and the ending: does it earn its emotions, is it ever fun, does
  anything feel forced, padded, po-faced or cheap.
- Both love The Secret of Monkey Island (its wit, its fair puzzles, its
  sense of place) and measure adventure games against it.

So in every round one critic is the Obra Dinn fan and one the Larry fan.

Each critic, while playing, keeps notes of everything she does not like:
story, mechanics, anything annoying, repetitive or forced, puzzles that are
too easy, too difficult or illogical, text that is clumsy, screens that
break. Also what works, briefly.

Both use the same rubric, written into both agent files, totals 100:

| Part | Points |
| --- | --- |
| Story and writing (plot, characters, prose, ending) | 25 |
| Puzzle design (logic, fairness, clueing, difficulty curve) | 25 |
| Pacing and flow (no padding, grind, dead time or forced steps) | 15 |
| Interface and mechanics (menus, commands, feedback, no friction) | 15 |
| Atmosphere and art (ANSI art, sound of the BBS, the ship) | 10 |
| Originality and lasting impression | 10 |

Score honestly, as for publication. 83 is a strong recommendation, not a
courtesy. An unfinished game is scored on what she saw, with the reason she
stopped.

Hard rules in the agent files (same as `playtester.md` and more): no
reading `sd/`, `src/`, `server/`, `tools/`, `.kiro/specs/`, `.kiro/steering/`,
`docs/` (except writing her own review file), the git history, or any
earlier review. Play only through `bbs-caller`. No brute force. A fresh
caller account each time. The critic may use two sessions on the same
account if one runs out; her own notes file carries over between those two
sessions only.

Output: `docs/reviews/R<round>-<critic>.md` with: finished or not, ending,
play time estimate and tool calls, the scores per rubric part, the total on
its own line as `SCORE: NN`, then the numbered notes (each: what, where,
why it hurts, how bad 1-3), then a short "what works".

Commit the two agent files and the rubric before round 1. After that the
agent files, the rubric and the critics' rules do not change, so the
scores of different rounds are comparable. Never tell a critic the
target, the round number, earlier scores or what was changed.

## 2. One round

1. Fresh staging board from the pushed main (`tools/staging-playtest.ps1`,
   a new disposable card; nothing left from the previous critic).
2. Critic Ada plays in a fresh agent session with no history. Then a fresh
   board, then Mara the same way. (Run them one after the other on separate
   fresh boards, so neither sees the other's posts on the social board.)
3. Average the two `SCORE:` lines. Record the round in
   `docs/reviews/LOOP.md`: date, commit played, both scores, average, and
   the top issues.
4. If the average is 83 or more: stop, go to "Done".
5. Otherwise fix (section 3), then start the next round.

Stop after round 6 even if 83 is not reached, and report why the score is
stuck.

## 3. Fixing

- Read both reviews. Fix first what both critics hit, then the notes rated
  3, then what costs the most points. Do not chase single phrasings.
- Anything may change when there is a good reason: puzzles, gates, clues,
  story beats, text, art, menus, pacing, even the ending. Make the game
  better, not easier: "too easy" also costs points, and adding hints until
  every puzzle is trivial is not a fix.
- Every change goes in `docs/reviews/CHANGES.md`: round, what changed, which
  critic notes it answers, why. Juha reads this to review the story changes
  in the morning; story or canon changes also go into the story bible
  (`.kiro/specs/story-bible/`) and `docs/PUZZLE_CHAIN.md`.
- Keep the rules that stay: everything in `AGENTS.md` and the QUEUE rules;
  the game is finishable, 60 to 120 minutes, no grinding; the story never
  grants the real `SYSOP` access flag; never show a menu option that does
  not work; content on the card, not in code; new screens get ANSI art via
  `tools/ansi/`.
- After each fix round: `pio run`, `python tools/run_host_tests.py`,
  `python tools/story_lint.py`, and the staging check
  (`tools/staging-check.ps1`). Update the e2e so the scripted player still
  finishes the game. All must pass before the next round. Push `main` to
  `staging` (the staging scripts build only the pushed main). Small commits,
  prefixed `Q18 R<n>:`.

## 4. Resuming

This item will take more than one Kiro run. Keep the loop state in
`docs/reviews/LOOP.md` (current round, step, which critic is done, commit
played) and commit it after each step, so a new run reads it and carries
on exactly where the last one stopped. A critic that was cut off halfway
is rerun from a fresh board.

## Done

When one round averages 83 or more, or round 6 is finished. Tick Q18 in
`docs/briefs/QUEUE.md` with: rounds played, the scores per round, the commit
range, the biggest changes, anything in `CHANGES.md` Juha should look at
first, and what the last two critics still disliked.
