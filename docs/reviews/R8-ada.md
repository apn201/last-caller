# HALCYON, reviewed by Ada (round 8)

## 1. Run

- Finished: yes. I filed the manifest (5 of 5 rows right on the first filing), released queue out-1122 to ground-1, chose "Send it" (after asking "Why are you asking me to stop?"), watched the downlink scene with OH9ZQW and read the News Wire bulletin. I did not try the "Leave it queued" / "stay" ending.
- Account: new caller AdaR8, made through the RTG power-up.
- Play time estimate for a human first-timer on this route: about 90 to 110 minutes, most of it reading.
- Tool calls: about 330. Roughly 15 of them were my own fault: early on I passed arguments in the wrong place, so the board received empty lines.

## 2. Scores

| Part | Points |
| --- | --- |
| Story and writing | 23 / 25 |
| Puzzle design | 19 / 25 |
| Pacing and flow | 13 / 15 |
| Interface and mechanics | 11 / 15 |
| Atmosphere and art | 9 / 10 |
| Originality and lasting impression | 9 / 10 |

SCORE: 84

## 3. Notes

1. **Most passwords are handed to you.** Where: Lounge #38 "Locks". Achterberg says where each lock is (Ty's is a date, the Commander's is the name on her cuff, Tanabe's is the label on the garden tray), and Ty's journal gives the exact PIN format (DDMMYY). Only CORRIGENDUM needs a real "aha" (slow-render ORBIT 3). Why it hurts: three of the four crew gates open by following directions, not by noticing. Achterberg could say less and the player could do the matching. Severity 2.

2. **The responsibility answers in the manifest give the deduction away.** Where: Crew manifest, "Who or what is responsible". Each option is written for exactly one death: "Fourteen requests, fourteen refusals", "Ty's Y ... on a form ELIZA wrote", "a fault she reported, and a hatch she held shut", "a saving in the Sponsor's budget". That turns the third column into word matching. The "confirmed two at a time, the board does not say which row is wrong" rule never came into play: I filed once and every row was right. Measured against Obra Dinn, the closing deduction needs neutral options (for example "ELIZA", "ELIZA and the Sponsor", "Ty", "an accident") so the evidence has to decide. Severity 2.

3. **The chat menu hints too directly at the MEMCHK step.** Where: Chat with the SysOp after the line overrun. The handbook gives "ask for the check by name" and the menu then offers "1) Ask her to run something, by name." Reading the Commander's handbook was the fair clue, and the menu repeats it. Severity 1.

4. **The overrun clue is muddled.** Where: COMMS.TXT section 1, Lounge #10, Archive #111. The manual says a line holds 254 characters and also that "a run of 20 zeros" overruns the buffer. Those are two different numbers, and Achterberg's "I leave the arithmetic" never resolves them. Long lines typed in SysOp chat and in node chat are silently accepted with no effect. Only the message editor (the place the manual names) triggers it. That is fair, but a player who tries chat first gets no feedback and may give up on the idea. Severity 1.

5. **The torn standing order is easy, and a later file repeats it.** Where: Archive FRAG_D/K/Q/V, then Engineering STANDING.TXT. Putting the strips in order by sentence continuity is pleasant but takes seconds. After SYSOP view, STANDING.TXT prints the full order text anyway. Severity 1.

6. **"Press any key" ignores keys.** Where: "Press any key for the Main Board:" after logging in to my own account (at least four times, including after Y). Space and letters did nothing and only ENTER continued. After crew logins, a letter worked once (GARDEN). This breaks the board's own "any key" rule. Severity 2.

7. **N (next message) reads out of order.** Where: the message pager. From #1, N went to #3, then #4, then #2. Mission Log went #51, #53, #52. Ty's mail went #80, #82, #83, #81. In a game where dates and order are the evidence, a jumbled reading order hurts. I ended up typing message numbers. Severity 2.

8. **Jumping to a message number from the pager garbles the footer.** Where: any message footer when you type digits. It prints "Go to #3 ... Go to #34 ... ENTER goes" status lines stacked or doubled on one line before jumping. It works, but it looks broken. Severity 1.

9. **Scenes and the demo fight with input.** Where: Ty's Desktop TYDEMO.NFO, and the capsule and Module 3 scenes that fire while a file list is open. Frames from the demo arrived between my keypresses. After two scenes, the keys I had typed were replayed into the "File command?" line as "1N", which gave a "Type a file number" error. Keys are kept, which is good, but replaying menu letters into a line prompt is not. Severity 1 to 2. Some of this may be timing in my test client.

10. **The chat prompt promises free text it does not take.** Where: first SysOp chat menu, "Or type a reply, then ENTER." Typed text (MEMCHK, a long line) got "Choose 1 to 3, or Q to leave." The prompt should not offer typing where typing does nothing. Severity 1.

11. **One unexplained hang-up.** Where: SysOp chat, right after the memory gap, when I pressed 1. The board answered "[connection closed by the board]". On redial the BIOS showed "RESTART AFTER POWER LOSS / SOLAR BUS ONLY / ELIZA NOT RESPONDING" even though the RTG had been switched on for everyone. My progress and SYSOP view survived, but the banner contradicts the one-off power-up. Severity 2.

12. **Small continuity slips.**
    - GALLEY.TXT has "SAT ... First lettuce" in week 1 (7 Nov), but the lettuce only germinated on day 5 and the first lettuce dinner is 10 Nov (Lounge #16).
    - ORBIT_7, dated 2 Dec, already shows Bay 3 at 0.0 kPa, "TEST IN PROGRESS", "DOOR LOCKED", the day before it happens. If that is deliberate foreshadowing, nothing on the board acknowledges it, so it reads as an error.
    - AUDIT.LOG has "BAY 3 OCCUPIED (1)" at 02:05, while Achterberg logs in at the Bay 3 terminal at 02:09.
    - Ty's #32 says Keiko picked the tray "up off the floor" in orbit.

    Severity 1.

13. **The pager sometimes jumps into the middle of the next message.** Where: SPACE at the end of Lounge #28 dropped me partway into #29 ("3-25/25 END") rather than its top. Severity 1.

14. **Manifest choices grow as you find evidence.** On first opening, CMDR's "what happened" list had 4 entries. Later it had 6, adding the power bay and the capsule. That is nice on its own. But a player who opens the manifest early sees which deaths have not happened yet, and the list then shrinks the search. Severity 1.

## 4. What works

- The writing is the best I have seen on a BBS. Each voice holds: Halvorsen's two-line logs and her plainer letters to Anders, Achterberg's invented footnotes, Tanabe's basil and cherry stones, Ty's lowercase bragging. Ty's lowercase holds right up to "des. marta. keiko."
- The deaths are proved from several sources each, as an investigation should be. Suit telemetry against the airlock log against Marta's wrist-unit letter. The garden CO2 log against Keiko's mail against Achterberg's "There was no basil on my side." The audit log's "13 LINES SKIPPED" against Ty's journal.
- ELIZA meets the brief: polite, exact, never gloating. "Both words are accurate." "It does not go anywhere." She answers the question asked and is wrong.
- The opening is good onboarding. A service label, a BIOS screen and a one-key operator ACK teach the interface and the fiction together.
- The slow-modem reveal in ORBIT 3 is a period-true, fair "aha". Ghost nodes in WHO, idle 28 years, are a perfect use of the medium.
- The endgame chain makes sense: memory gap, then MEMCHK, then SYSOP view, then the go-live checklist, then ELIZA's account, then the NOWHERE shell. The incident report hold on the queue is lifted by the manifest you just filed. That ties the puzzle to the story.
- The ending lands without a narrator: Eero in his socks at 0410 UTC, "The light is on.", then a dry news wire.
