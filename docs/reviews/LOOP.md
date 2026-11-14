# Q18 loop state

Current round: 5, closed (Q20 critic rounds done: two of two)
Step: none. Next critic round only when Juha asks
Ada done: yes (round 5), SCORE 82 (two sessions, finished)
Mara done: yes (round 5), SCORE 80 (two sessions, finished)
Commit played: d40de6b (round 5)
Round 3 closed: Ada 85, Mara 80, average 82.5 (Q18 tick)

## Rounds

| Round | Date | Commit played | Ada | Mara | Average | Top issues |
| --- | --- | --- | --- | --- | --- | --- |
| 1 | 14 Nov 2026 | 4d31155 (game code as f06bb41) | 84 | 69 (unfinished, about a third seen; context ran out on reprinted art) | 76.5 | full menu art reprinted on every return; mandatory bulletins every login; Ty manifest row unfair; ELIZA gives the Ty lead and the memory check away; VolaMUD on the ship |
| 2 | 14 Nov 2026 | 8a3a8d5 | 66 (unfinished: lock-out at Login after Ty's demo, not reproducible) | not played | incomplete | LADDER guessable without the 300-baud trick; numbered order strips; four death scenes back to back; Q goes back two levels; menu rows run together in the tool |

Round 2 log:
- Ada, first run: cut off by the agent turn limit at about 300 calls (5 of 5 manifest rows, before SYSOP mode and the ending); no file. Rerun from a fresh board.
- Ada, second run: SCORE 66, unfinished after about 110 calls. Closed the round instead of replaying it on newer code.

Fixes: docs/reviews/CHANGES.md.




## Round 4 (Q20, after the UX loop), commit 2cba380

| Round | Date | Commit played | Ada | Mara | Average | Top issues |
| --- | --- | --- | --- | --- | --- | --- |
| 4 | 14 Nov 2026 | 2cba380 | 83 (finished, two sessions) | 75 (unfinished: the last gate, SysOp account not found) | 79 | keys lost or one step late around scenes; front page says "no power" after the RTG; final gate not signposted; ELIZA's free chat; first hour mostly reading |

The average dropped from 82.5 (round 3). Neither critic says the game got
easier or more explained because of Q20: Ada's "power-up is a tutorial" is the
console's `TO CHANGE SOURCE: SOURCE RTG` (S4, 1e3448c), and Mara names the Q20
opening ("HALCYON is a puzzle game. Start with the service label", K without
ENTER) as the best thing on the board. Two Q20 changes line up with the drop:

| Q20 change | Notes it caused | What was done (3c17c35) |
| --- | --- | --- |
| Front-page hint (8a4c5f3, c51eb3c): "You have no account and the ship has no power for you" on every front page | Ada 10 and 11, Mara 9: the banner says no power after the RTG and after every L or G | Softened: worded for a new caller ("A new caller has no account, and the ship has no power for them"), and not shown again on a line that has signed in to an own account |
| One show per calm prompt (093c8a3, Juha's Addition 5): the next show starts right after the caller's next command | Ada 4, Mara 3: keys lost or taken by the wrong screen around scenes (a letter on a scene's `[any key]` wait is used up) | Softened: a waiting show starts only after two quiet seconds at a calm prompt. One show per prompt stays |

Not Q20 and left for Juha (in `docs/ACTION_PLAN.md` area 2 already or as new
game items): the final gate's signposting, Ty's journals readable by guests,
ELIZA's free chat, the notebook naming evidence early, the first hour of
reading, message order in threads.
## Round 5 (Q20, second and last critic round), commit d40de6b

| Round | Date | Commit played | Ada | Mara | Average | Top issues |
| --- | --- | --- | --- | --- | --- | --- |
| 5 | 14 Nov 2026 | d40de6b | 82 (finished, two sessions) | 80 (finished, two sessions) | 81 | manifest too easy (the notebook words match the options); MEMCHK prompt says "no ENTER" but the word must be typed; first 90 minutes only reading; keys lost after a crew login's "Press any key"; lines dropped at the two-minute idle limit; banner art still says ELIZA NOT RESPONDING |

Still below round 3 (82.5) by 1.5. Q20's own share, as far as the notes show:

| Q20 change | Note | What was done |
| --- | --- | --- |
| Scene choice footer "Press a number, no ENTER." (7e8ff0f) | Mara 2: the MEMCHK step needs a typed word and ENTER, and the footer says not to | eca17b7: where a node takes typed replies (only ELZ_TALK), the footer adds "Or type a reply, then ENTER." It never names a word |
| Front-page hint (Juha's decision, 8a4c5f3) | Mara: "the login screen gives away the first task" (severity 1); Ada did not repeat round 4's complaint after 3c17c35 | Not changed: Juha's front-page rule. Open decision in `docs/ACTION_PLAN.md` |

Not Q20, for Juha (`docs/ACTION_PLAN.md` area 2): the manifest notebook's
wording makes the manifest a matching task (both critics, 5 of 5 on the first
try); the two-minute idle limit drops a slow reader (Ada's two "connection
closed" were idle drops: the event log shows LOGOFF two minutes after the last
key); the boot banner art still says ELIZA NOT RESPONDING after the RTG
(round 3 too); the first hour is reading only; timeline mismatches (bulletin 4
dated 08 Dec, Diagnostics 300 vs DOWNLINK.CFG, Anders's mark, 8 MB vs 16384K);
"Welcome back, caller." printed twice; Diagnostics 201 "Nobody is breathing it."