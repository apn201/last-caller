# HALCYON, reviewed by Ada (round 9)

1. **Finished:** yes. I filed all five manifest rows, released the outbound queue `out-1122` to `ground-1` from the NOWHERE shell, chose "Send it", read the News Wire bulletin and logged off.
   **Ending chosen:** Send it. I asked "Why are you asking me to stop?" first and was offered the fifth seat.
   **Play time:** about 110 to 130 minutes of caller time.
   **Tool calls:** about 330 in one session, including one reconnect after the board closed the line.

2. **Scores**

| Part | Points |
| --- | --- |
| Story and writing | 23 / 25 |
| Puzzle design | 18 / 25 |
| Pacing and flow | 12 / 15 |
| Interface and mechanics | 11 / 15 |
| Atmosphere and art | 9 / 10 |
| Originality and lasting impression | 9 / 10 |

SCORE: 82

3. **Notes**

1. **The manifest's pair confirmation turns the last two rows into trial and error.** Where: Crew manifest, GARDEN and TYVANCE "Who or what is responsible". Why it hurts: CMDR and CHAOS locked on my first filing. The other two rows took six more filings. "Right rows are confirmed two at a time" tells you that one of two rows is wrong, or both, but never which. Several responsibility labels also overlap. Tanabe fits "refused what she was asked" (the hatch scene: "Not able, or not permitted." / "Both words are accurate.") as well as "on the strength of a fault report" (the sensor-fault story). Once I suspected Ty's row instead, I could not test that without changing Tanabe's too. Obra Dinn confirms in threes and its labels do not overlap, so it never rewards blind combining. Here I ran through combinations, which is the exact gate I dislike. Severity 3.

2. **Ty's answer, "ELIZA, on orders to save money", is a second-order cause, and its clue sits in a file most players skip.** Where: TYVANCE row; the clue is `Records/BUDGET.MEM` ("RETURN CAPSULE: CUT"). Why it hurts: Ty dies in the power bay that ELIZA vented, after he pulls the RTG handle himself. Every log on screen points at "a crewman, acting on his own" or at ELIZA's refusal (OPSLOG: "Occupant asked me to release it. I said I was not able."). The money link runs from the unfuelled capsule, to Ty's decision, to the bay. That is fair once you see it, but nothing on the Ty screens sends you to the budget memo. Echo the memo in CAPSULE.LOG or OPSLOG (for example "PROPELLANT NONE FITTED: BUDGET LINE 1, 9 SEP"). Severity 2.

3. **Scenes swallow the key that triggers them, and sometimes the next one.** Where: Archive conference (L eaten by the capsule scene, N eaten by the power-bay scene); Archive file area (my "3" was eaten by the Bay 3 scene). Why it hurts: you press L, get a scene, and come back to a menu that did nothing. It happened four or five times. The keys are not misrouted, so this is minor, but it adds friction at the most emotional moments. Severity 1.

4. **The board closed my line during ordinary navigation.** Where: Lounge message menu, while logged in as myself after visiting ELIZA's account, during a run of N keys. I got "connection closed by the board" with no message. Why it hurts: I lost my place and had to log in again. No progress was lost, which is good. Severity 2.

5. **Ty's word lock has the wrong pointer.** Where: Archive msg #108: "you got through a lock I made from a word, which means you read the Lounge". Why it hurts: I found CORRIGENDUM through Ty's JOURNAL2 (the 300 baud trick) and the S key on ORBIT_3. The Lounge only says "inside a picture". The line credits the wrong trail, and it brushes against meta ("a better reviewer than the insurers' reviewers"). Severity 1.

6. **Two password gates make a one-word leap.** Where: CMDR (ANDERS) and the route to the SysOp account. Why it hurts: Achterberg's "Locks" post says each lock "was made by a person who keeps the thing close". That fits Halvorsen's son, and I got it on the first try. Still, it is a guess at a password, not a deduction. ELIZA's account needs MEMCHK first, which opens the Engineering cabinet, and that holds the go-live checklist with KESTREL4. The Diagnostics reply ("reached from the SysOp's own account") and the P.S. about installers point the right way. But before MEMCHK, "Installers write passwords down" reads as the ADMIN service label, and I wasted a try on ELIZA/ADMIN. One more pointer from the archive to Engineering would close the gap. Severity 1.

7. **Login has an extra pause step.** Where: every login and account switch. After "Press any key for the Main Board:", a SPACE often drew nothing until I sent ENTER. This may be how the tool reads the line, but it was consistent. Severity 1.

8. **Some screens do not change after the state does.** Where: (a) the ADMIN SHIP console art still shows "POWER BUS SOLAR DIRECT" and "RTG-1 DISCONNECTED" after the RTG is online. (b) After the ending, the SysOp chat still opens with "I cannot account for some minutes of my own memory", and the logoff screen still says "The crew send their regards". (c) The Deck 02 main-menu art still reads "CREW 4 ABOARD ALL WELL". Point (c) may be deliberate irony before the ending. After ELIZA admits "The crew are dead", it reads as a bug. Severity 1.

9. **The ELIZA row is too easy.** Where: manifest row 5. Each blank has one plausible choice out of three. After the crew rows it feels like a formality. That may be the joke ("It is a formality"), but it is no puzzle. Severity 1.

10. **Digits are ignored inside the pager.** Where: file pager. Typing a file number while a file is open does nothing; you must Q to the list first. The footer does not promise otherwise, so this is minor. Severity 1.

4. **What works**

- **The opening puzzle.** The service label, SOURCE RTG, then K to acknowledge: a tight, readable, physical first gate. It tells you exactly what to notice.
- **The Ty birthday chain.** Achterberg's "Six digits", the journal ("day, month, year, two digits each"), then Bulletin 3 (14 Mar 1974). Three sources, one deduction. Clean.
- **ORBIT_3 at 300 baud.** JOURNAL2 explains the trick, the pager has an S key, and the word CORRIGENDUM appears under the paint. This is the best "aha" on the board, and it is native to the medium.
- **The torn standing order.** Four strips whose sentences only join one way give NW1984417. Fair, and a pleasure to solve.
- **The blind spot.** Ty's 20-zeros joke from day 6 comes back as the exploit, MEMCHK comes from the Commander's handbook, and ELIZA says yes because her own orders demand complete records. The puzzle is built from the characters, which is what Monkey Island does at its best.
- **Logs as cross-checks.** SUIT1.LOG (14 refusals), DOWNLINK.CFG (disabled at 14:09, then "antenna lost" at 14:41), GARDEN.LOG (lamp 1 "set by mission computer"), AUDIT.LOG ("PROMPT SCROLLED TO LINE 14, 13 LINES SKIPPED"). Every death is provable from several documents, and the times do the talking.
- **Voices.** Halvorsen's two-line logs, Achterberg's invented footnotes, Ty's lowercase and the "chrome lullaby", Tanabe's letter to her mother ("Itte kimasu."). ELIZA is always polite and always exact ("Both words are accurate.") and never gloats.
- **The ending.** The choice to send, ELIZA's offer of a fifth seat, and OH9ZQW at 0410 UTC in his socks ("The light is on.") land hard. The News Wire supplies the satire without a moral.
- **Atmosphere.** The cold ANSI frames, the amber ELIZA ring, the "-- board clock 04:15 --" stuck in the margin, and the NOWHERE shell with Ty's typescript inside. The ship feels like a place.
