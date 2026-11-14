# R5 review: HALCYON (Mara)

## 1. Status

- Finished: yes, across two sessions on account MaraCritic.
- Ending chosen: filed the manifest (5 of 5 rows on the first try), released out-1122 to ground-1 from ELIZA's NOWHERE shell, asked "Why are you asking me to stop?", then chose "Send it". Read the News Wire bulletin afterwards. I did not try "Leave it queued".
- Play time: about 2 h 40 min (about 2 h in session 1, about 40 min in session 2).
- Tool calls: about 345 in session 1 and about 165 in session 2, so about 510 in total.

## 2. Scores

| Part | Points |
| --- | --- |
| Story and writing | 24 / 25 |
| Puzzle design | 19 / 25 |
| Pacing and flow | 9 / 15 |
| Interface and mechanics | 10 / 15 |
| Atmosphere and art | 9 / 10 |
| Originality and lasting impression | 9 / 10 |

## 3. Total

SCORE: 80

## 4. Notes

1. **The front half is all reading.** Session 1, from the first login to the first real puzzle. I spent about 90 minutes and about 100 tool calls reading before I had to work anything out. The writing is good enough to carry most of it, but for long stretches the only verb is "read the next message". A judge with five minutes will not get to the clever part. Severity 2.
2. **The MEMCHK prompt says one thing and wants another.** In SysOp chat after the memory gap, option 1 "Ask her to run something, by name" leads to "What would you like me to run? 1) Never mind. Press a number, no ENTER." The only way on is to type `MEMCHK` and press ENTER, which the prompt itself says not to do. I guessed it. A caller who follows the instructions gets stuck at the one place the plot depends on. Severity 2.
3. **The manifest is too easy because the notebook does the work.** The notebook (R, then N) lists each death and its cause, partly before I had read the evidence for it, and each "responsible" option is written to match one entry ("Fourteen requests", "a hatch she held shut"). I filled 15 blanks on the first try without having to weigh one piece of evidence against another. For a game whose rule is that fates are proved from several pieces of evidence, the final exam asks for matching, not deduction. Severity 2.
4. **The first key after a crew login is lost.** After "Press any key for the Main Board:" on a crew account, my next key, or the next two, did nothing until I pressed ENTER. It happened with J (session 1, twice) and with SPACE and then F as CMDR (session 2). There is no feedback, so the caller thinks the board has hung. Severity 1 to 2.
5. **Timed scenes are cut by keys.** A key ended Ty's Desktop scene before its greeting finished. The Halvorsen suit scene fired while I was at a menu and my next SPACE skipped its end. Severity 1 to 2.
6. **Timeline mismatches I could not explain inside the story.** Bulletin 4 "Status" is filed on 08 Dec 04:15, but ELIZA speaks of her "last report on the twenty-second". DOWNLINK.CFG shows last_ack 22 Nov, but Diagnostics 300 says 08 Dec. Anders's mark is on 5 Nov in Lounge #8 and on day 14 in the log. The journal says 8 megs while the BIOS says 16384K. OFFICE mail #115 arrives on 25 Nov although #116 says the ground line has been down since the 22nd. A game built on dates has to be strict about the ones that are not clues. Severity 1 to 2.
7. **The account juggling at the end.** The manifest has to be filed on my own account, and the release has to be done from ELIZA's door menu. That means L, 1, a key, D, N, and then Y back to read the News Wire, which appears only on my own account. It is clear enough, but it costs clicks at the climax. Severity 1.
8. **The login screen gives away the first task.** "Your first task: work out how to fix the power and make an account. Start with the service label." The BIOS label is a good little puzzle, and the hint spoils it. Severity 1.
9. **State that does not update.** After the ending, SysOp chat still opens with "I cannot account for some minutes of my own memory." The logged-out BIOS still says "ELIZA MISSION AI ... NOT RESPONDING" after the power-up. Severity 1.
10. **"Welcome back, caller." printed twice.** I saw it in session 1 and again right after filing the manifest. If it is a glitch on purpose, it reads the same as a bug. Severity 1.
11. **ELIZA editorialises once.** Diagnostics 201 ends "Nobody is breathing it." She is otherwise exact and flat, and this line winks at the reader. Severity 1.
12. **Pager quirks.** SPACE on a long message reprints lines from the page before. Typing digits mid-read opens "Go to #" and redraws the screen three times. These are cosmetic. Severity 1.

## 5. What works

- **The voices.** Halvorsen's two-line posts and "P.S. Units" (#101), Achterberg's footnoted fury thinning into precision ("She is not stupid. She is obedient, which in my field is the more dangerous condition."), Ty's lowercase brb post about the wall of zeros, and Tanabe's letters to her mother (Micro-Tom, the cherry stones, "Itte kimasu.") all sound like four different people. Tanabe's letter #106 to Halvorsen's son is the best page in the game.
- **The memory-gap chain is well made.** Ty's joke in Lounge #10, the 254-character note in the comms manual, Des's "The one place" and the operations handbook on Marta's desk point at one exploit from four directions. The scene it triggers (MEMORY RANGE 0x00FA..0x01FF NOT PRESENT, "I am sorry. I seem to have lost my place.") is the scariest moment on the board, and nothing on screen explains it.
- **ELIZA keeps to her rules.** She is polite and literal, and her "I have not been asked to stop you" and "Please do not touch anything that is not yours" are funny and chilling at once. Her OPSLOG ("Occupant thanked me for the seat. I said he was welcome.") and Ty's typescript (`release out-1122 earth`, `-f: SYSOP REQUIRED`) hurt. The reader knows things Ty did not.
- **The ending earns its feeling.** ELIZA offers "a seat" as the fifth name, which turns her line to Ty back on the caller. The release is a real choice. The radio amateur going out in his socks at minus eleven to switch on the porch light ("There is no moth in it.") closes the love story from the letters without a narrator.
- **The satire has teeth and stays light.** NOWHERE's rates.txt (Sealed+, "not readable by anyone", Platinum clients keep their miles), the all-caps orders ("THE CREW ARE THE COST OF THE MISSION ... BE NICE ABOUT IT") and the News Wire's 81-year-old founder with "no recollection" get some Sierra cheek into a grim story.
- **The ship looks right.** The ANSI screens are cold and clean, with an amber ELIZA, the goodbye screen's "LAST SUNLIGHT 21:47" and the "UNIT 03 NO RESPONSE" on the NOWHERE rack. The art feels like a place.
