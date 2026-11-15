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


## Q21 fun check: does the notebook help without giving the game away

One critic, one fresh board, agent files unchanged, commit 4234644 (the notebook collects what the board shows; S lists the scenes seen and replays them). Ada, two sessions on one account, finished (the ending "send it"); the file is `docs/reviews/R6-ada.md`.

| Round | Date | Commit played | Ada | Mara | Average | Top issues |
| --- | --- | --- | --- | --- | --- | --- |
| 6 (Q21 only) | 15 Nov 2026 | 4234644 | 80 (finished, two sessions) | not played | n/a | ELIZA's account is hard to find (nothing sends the caller back to GOLIVE.TXT); the release working after the manifest is never stated; a digit typed ahead lands on the next list; the "responsible" column names its crew member; keys lost after scenes |

On the notebook she says: it helped as a memory aid across two sessions, the L page (logins that worked) most of all; it did not solve anything; it gave away "mostly no", with one mild exception: the note "The last status report was filed on the twenty-second" sets 22 Nov beside the tank date before the caller links them. That note (`WAKE_CREW`) now keeps only ELIZA's words "The crew are resting. Everyone is well." S listed nine scenes on two pages and a replay worked; R, N, P and Q on a single note worked. She could not confirm a note for item 14 of GOLIVE.TXT (she did not page through all 12 pages); the card has two notes for that file (`ENG_GOLIVE`, `ENG_GOLIVE_B`) and the host test and lint check them. Her "pressed 4 right after S and got note 4" is a key typed before the S key had run (the tool sends both at once); the same lag as her note on keys after scenes (Q20 round 5, not Q21).

## Q21 Addition 1: both critics on the finished game (rounds 7 and 8, the two this item allows)

| Round | Date | Commit played | Ada | Mara | Average | Top issues |
| --- | --- | --- | --- | --- | --- | --- |
| 7 | 15 Nov 2026 | 76301c6 | 68 (two sessions, unfinished: the SysOp account not found after SYSOP view) | 85 (finished, one session) | 76.5 | ELIZA's run-by-name prompt invites a typed reply and refuses it (both); a scene took the key at "Press any key" after a crew login (both); digits in the reader redraw the page (both); Ty's post says F for the gallery (both); MEMCHK for a guest though the handbook says the commander (both); the manifest is matching |
| 8 | 15 Nov 2026 | 5bbf023 | 84 (finished, one session) | 73 (finished, two sessions) | 78.5 | keys still late or lost around scenes and logins (both); disconnects, with the brownout banner on redial (both); the manifest's "responsible" column is matching and pairs can be swapped (both); the release gate: nothing says the manifest is the report (Mara); News Wire not in a crew account's bulletin list (Mara) |

Fixes between the rounds (5bbf023, what both hit in round 7): `typed_else=` in the scene engine, and ELIZA answers MEMCHK without the handbook ("when the Commander asks for it, or someone acting for her"), without a gap, or a word she does not know; the grant line names the handbook; shows no longer start at "Press any key"; digits in the reader redraw only the bar; Ty's post says A; COMMS.TXT says 20 zeros.

The goal (83) was not reached within the two rounds. Neither critic says the game got easier: both still call the manifest too easy, and Ada R8 calls the crew passwords over-clued (Lounge "Locks" and Ty's journal). Open for Juha (`docs/ACTION_PLAN.md` area 2): the manifest's option C (options worded so two fit each fate), and the pair-swap; key lag after scenes and logins, which the 2 s quiet rule does not cover when the tool sends keys during a scene (keys typed during a scene are played back later, contract C7, and land on the next prompt); the redial banner after a dropped line is the brownout one, because a new connection has no `ownUserId`; News Wire on a crew account checks the crew account's flags; the shell's help lists `release` only with the manifest filed.
