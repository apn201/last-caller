# R5 review: HALCYON (Ada)

1. **Finished.** I reached the last scene. Ending chosen: "Send it". I released out-1122 to ground-1, and that played the downlink burst, the club station, the OH9ZQW reply and the News Wire bulletin. I did not play "Leave it queued" or "stay". Play time is roughly 3 hours over two sessions; a first-time player who reads closely would need 2 to 2.5. Tool calls: about 350 in session 1 and about 175 in session 2, so about 525 in total.

2. Scores

| Part | Points |
| --- | --- |
| Story and writing | 23 / 25 |
| Puzzle design | 20 / 25 |
| Pacing and flow | 11 / 15 |
| Interface and mechanics | 10 / 15 |
| Atmosphere and art | 9 / 10 |
| Originality and lasting impression | 9 / 10 |

SCORE: 82

## Notes

1. **The overrun rule in the manual does not match the board.** COMMS.TXT section 1 says "a run of 20 of the same character overruns the line buffer". A Lounge post with 24 z's saved without any effect. Exactly twenty zeros, copied from Ty's Lounge #10, crashed the board. A player who follows the manual, which is what Achterberg #111 tells you to do, fails and starts to doubt the right idea. Either accept any run of 20 or more, or make the manual say what the board checks. Severity 2.
2. **The "run something, by name" menu misleads.** After the crash, chat offers "1) Ask her to run something, by name." The next screen shows only "1) Never mind. Press a number, no ENTER." Typing MEMCHK works, but the prompt says it will not. Every other screen trains you to press digits, so this is the one place where the interface fights the solution. Severity 2.
3. **The connection dropped in the manifest.** The board closed the connection when I pressed 1 on the CHAOS row. In session 1 a K at the acknowledgement also dropped the line with no message. Severity 3 if it is not a shadow pass; if it is meant as one, it needs to say so on screen.
4. **Output lags behind keys and keys get lost.** On the manifest and after "Press any key", replies often showed the previous screen. The 1 and Q I typed inside the Notebook vanished. In the ELIZA "Why she is still running" blank I picked option 3 when I meant 1, because the screen I was reading was one step old. "Press any key" after login ignored a letter and a space and only moved on after ENTER. Severity 2.
5. **The manifest is too easy.** The Notebook lists each fate and its cause almost word for word ("Her suit log asks for the lock again and again", "A pressure test was approved from TYVANCE"). The answer options mirror those lines. I got 4 of 4 and then 5 of 5 on the first filing. Turning the manifest into the incident report that clears the hold is a lovely idea, but the deduction in it is light. Next to Obra Dinn's book this is a reading-comprehension check. Severity 2.
6. **The NOWHERE code had to be entered twice.** I entered NW1 98 44 17 as CHAOS, and the reply told me the host is "reached from the SysOp's own account". Logging in as ELIZA did not show the door until I typed the code again in Diagnostics. Severity 1 to 2.
7. **The login banner is stale.** It still says "SOLAR BUS ONLY ... A new caller has no account, and the ship has no power" after the RTG is on, and again after the ending. Severity 2 (session 1).
8. **Ty's password is clued twice.** Achterberg's Lounge #23 and the journal both give it, the second one outright. Severity 2 (session 1).
9. **Free-text chat is thin.** "I see", "Please go on", "Tell me more" answer most real questions. The scripted menus carry the game, so the free chat mostly costs the player time. After the ending, chat still opens with the "small difficulty" line. Severity 1 to 2.
10. **The late hints are vague, and the Archive recap softens discovery.** Achterberg #108 and #111 summarise a lot of what the player should have pieced together. They are also the only real pointer to the handbook and the overrun. Severity 1 to 2 (session 1).
11. **Small things.** Some dates don't line up: the Anders posts of 5 Nov and 14 Nov, and the status line filed on the 22nd against the 8 Dec bulletin. The pager redraws the whole text on page 2. A message number needs L first. Scenes swallow the command that triggered them. Message counts differ per account with no explanation. Severity 1.

## What works

- ELIZA is the best part. She is polite, exact and literal ("I said I was not able. This was accurate.") and never gloats. Her Mail #116 notes the impossible ground-line instruction without comment. Her plea at the release ("I have kept them for twenty-eight years. I would like to go on.") and the offer of a fifth seat land hard.
- The final gate is real Monkey Island logic. The hold says an incident report is needed and that the computer may not file one. The filed manifest is that report, so your detective work is what opens the downlink. That was my best "aha".
- The CMDR password ANDERS comes from the felt-tip laundry mark in Ty's ORBIT_5 drawing, which echoes the Anders posts. That is fair, visual and earned. KESTREL4 sits on the go-live checklist, as Achterberg's P.S. predicts.
- The documents give a strong sense of place: the SUIT1 telemetry with fourteen denials and the 11:52 board call, OPSLOG 07 to 08 Dec, the NOWHERE shell with its client list and loyalty programme, the Sponsor's all-caps orders, Ty's typescript.
- The ending moved me. OH9ZQW in his socks at minus eleven, "There is no moth in it. The light is on." The News Wire line "All crew are well" is a closing image I will remember.
- The ANSI art is consistent: cold ship, amber ELIZA, decay touches, ship-status panels that lie ("CREW 4 ABOARD ALL WELL").
