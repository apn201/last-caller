# HALCYON, reviewed by Ada (round 3)

1. **Finished.** I played to the last scene. My ending: I filed the full five-row manifest, released queue out-1122 to ground-1 from the NOWHERE shell, and chose "Send it" over ELIZA's offer of "a seat". Then came the radio amateurs' reply, OH9ZQW's porch-light message and the News Wire bulletin of 13 Nov 2026. Play time is about 100 minutes for a human caller. The board itself counted 20 + 6 + 7 + 8 + 11 + 52 minutes across my logins. I made about 465 tool calls.

2. **Scores**

| Part | Points |
| --- | --- |
| Story and writing | 23 / 25 |
| Puzzle design | 21 / 25 |
| Pacing and flow | 12 / 15 |
| Interface and mechanics | 11 / 15 |
| Atmosphere and art | 9 / 10 |
| Originality and lasting impression | 9 / 10 |

SCORE: 85

## Notes

1. **The manifest options give answers away.** (Crew manifest, "Who or what is responsible".) Each option bundles a cause with a fate. For example, "ELIZA vented the bay. The Sponsor's capsule was a mock-up" can only belong to Ty, and "ELIZA would not cycle the airlock. Fourteen requests" can only belong to Halvorsen. Once you know the four fates, the responsibility column fills itself. The ELIZA row has the same problem: each blank offers one real answer and two straw men. Obra Dinn works because the hard column is the name, and here the hard column is answered by its own wording. Severity 2.

2. **The slow-modem reveal is clued in only one place.** (ORBIT_3.ANS, viewer key S.) The CHAOS password, CORRIGENDUM, sits under the portrait and appears only when you press S ("play") in the viewer. The only clue is one paragraph in JOURNAL2.TXT. The "S play" label on the pager footer never says it replays at modem speed, and the effect is used nowhere else that I found. The idea is lovely, but a player who skims the journal will be stuck at the game's most important lock. It needs a second clue, such as Achterberg's "A person who slows down" in Lounge #38 pointing at the viewer's own wording. Severity 2.

3. **`release` says "command not found" with no reason given.** (NOWHERE shell, ELIZA account.) Before the manifest is filed, `release out-1122 ground-1` returns "release: command not found". COMMS.TXT section 6 and the shell `help` both imply the command exists. The real gate is the queue's "HOLD INCIDENT REPORT NOT ON FILE". Linking "incident report" to "crew manifest" is a fair deduction, but "command not found" says the verb is wrong, which sends the player hunting for another spelling. A message like "release: out-1122: held (incident report not on file)" would teach the gate instead of hiding it. Severity 2.

4. **Progress is split across accounts and is hard to follow.** The manifest lives on my own account. The NOWHERE shell exists only on ELIZA's account. The News bulletin appeared on my account after I sent the queue from ELIZA's. The product notes say progress is per caller, but in play the caller becomes five people, and it is never clear which login holds which state. Severity 2.

5. **MEMCHK is a menu option, not a request by name.** (Node chat, after the 20-zero overrun.) The handbook (Commander's Desk, HANDBOOK.TXT 9.4) says ELIZA enters SYSOP mode when someone "asks for the check by name". Typing MEMCHK in Diagnostics returns "I have no query with that code". In chat, a ready-made option, "About those minutes you cannot account for", runs the check. So the handbook's clue is never used, and the step turns from deduction into picking a menu line. Severity 1.

6. **Scenes arrive on their own and eat the next input.** Several times a scene began while I was at a menu: the Module 3, Power Bay, Suit 1 and Capsule scenes after reading the logs, and the Bay 3 scene after the Archive. My next keystroke went into the scene or into a prompt I could no longer see. Some scene text was cut off at the screen edge ("Mr. Vance cannot s / pell pressure"). The demo's end scroller ("and the last greet is for l. ... it is long. it starts with the") was lost to [Enter], and the demo does not replay. Severity 2.

7. **The message pager has quirks.** Typing a message number while the pager is open runs "Go to #N", and the page is drawn twice. In the Mission Log, ENTER steps through 50, 51, 52, 54, 55, 53 (thread order rather than date order) without saying so. Q sometimes takes two presses to leave a reader. Severity 1.

8. **The SysOp chat menu has no exit at the top level.** Options 1 to 3 only. Q and ENTER both answer "Choose 1 to 3." To leave I had to go Something else, then Say something else, then /Q in node chat. Severity 1.

9. **The BIOS screen contradicts the power-up.** After the RTG switch-over, every later login screen still says "SOLAR BUS ONLY - ATTITUDE CONTROL OFFLINE" and "ELIZA MISSION AI ....... NOT RESPONDING". If it is meant as a decay touch, it reads as a bug after the board made a ceremony of the power-up. Severity 1.

10. **There is a lot of account hopping.** The chain runs AdaR, TYVANCE, CHAOS, AdaR again (overrun and MEMCHK), CMDR (handbook), ELIZA (shell), AdaR (manifest), ELIZA (release), AdaR (news). Each hop costs a Goodbye screen, the BIOS screen and a login. The logic is sound, but the second half becomes logistics. Severity 1.

11. **Halvorsen's password rests on a thin clue.** "The one name aboard that she has not said aloud... she wears it on her cuff." The word ANDERS appears only in her terse log ("Anders's mock mark is in: 74", "Spoke to Anders in my head"). It is fair, and satisfying when it lands, but it is a single thread. A second trace, such as the cuff in Tanabe's notes, would make it robust. Severity 1.

## What works

- **The deduction chain.** Each password is something you notice rather than something you guess: Ty's birth date from Achterberg's "Six digits", ANDERS from the cuff, KESTREL4 from a go-live checklist nobody ticked. The four torn standing-order strips are ordered by sentence continuity, not by labels, and that one is a small, true "aha".
- **The 20-zero overrun.** Ty's joke post, Achterberg's footnoted "one place" and the manual's line limit together point at a single, reproducible exploit. The glitch scene that follows ("MEMORY RANGE 0x00FA..0x01FF NOT PRESENT", "I seem to have lost my place") is the best screen in the game.
- **Evidence you cross-check.** The logs agree with each other in the way Obra Dinn's corpses do. Fourteen lock denials sit beside a pulse column. The audit log shows Ty's 0.6-second Y beside "13 lines skipped". Antenna telemetry reads OK while the report says "antenna lost". The rack power column stays flat while the lamp column halves.
- **The voices.** Halvorsen is terse ("He had not looked up." "I can count."). Achterberg is exact and funny until he isn't ("There was no basil on my side. I said yes."). Tanabe's basil and strawberries, and Ty's "who is going to hack it, a seagull", make you like them before you lose them. ELIZA's calm "Both words are accurate" is chilling without being stagey.
- **The love story.** Keiko and Eero live only in the Radio Room letters and the ending. "The bulb is the same make. There is no moth in it." lands without a narrator.
- **The satire.** The Founder memos (the mock-up capsule, "PLANTS SELL ROOMS"), the NOWHERE tariff ("Mission computer who asks 'why': Not available") and the client list deliver the anger through jokes.
- **The art.** The block art throughout is consistent and in period, and the amber ring for ELIZA is well judged.
