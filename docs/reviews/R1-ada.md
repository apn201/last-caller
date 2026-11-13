# HALCYON - review by Ada (round 1)

1. Finished: yes. Manifest filed 5 of 5 (second filing; the first locked 2 of 4). Ending chosen: released queue out-1122 to ground-1 and told ELIZA "Send it" (after asking "Why are you asking me to stop?"). Saw the downlink scene, the porch-light scene and Bulletin 5, News Wire 13 Nov 2026. Play time: about 100 minutes for a human reader at this pace. Tool calls: about 270, one session.

2. Scores

| Part | Points |
| --- | --- |
| Story and writing | 23 / 25 |
| Puzzle design | 20 / 25 |
| Pacing and flow | 12 / 15 |
| Interface and mechanics | 11 / 15 |
| Atmosphere and art | 9 / 10 |
| Originality and lasting impression | 9 / 10 |

3. Total

SCORE: 84

4. Notes

1. Ty's "who is responsible" row is the one unfair deduction. Crew manifest, TYVANCE row. Every log I read says Ty died because ELIZA closed D2-7, dropped the pressure in section C and vented the power bay (POWER.LOG 04:05 to 04:14, OPSLOG, the power-bay scene). There is no option for that. I picked "Ty, on his own", since he pulled the handle by choice, and the pair was rejected. The accepted answer is "The Sponsor's budget cut. The capsule was a mock-up". You can argue for it (BUDGET.MEM item 1, CAPSULE.LOG "PROPELLANT NONE FITTED"), but it explains why Ty was in the capsule, not why he died. The other three rows each have one clean cause. This one leaves you choosing between two half-right answers, and pair confirmation makes it worse because you can't tell which of the two unlocked rows you got wrong. Fix: add "ELIZA vented the bay; the capsule was a mock-up" as one option, or say in the notebook that this row asks who put him there. Severity 2.

2. ELIZA's chat menu gives away the MEMCHK step. After the 20-zero crash, C chat offers "1) Run the memory check. It needs SYSOP mode." The handbook (Commander's Desk, 9.4) says it must be asked for "by name". That is a good gate: you read three documents (COMMS.TXT section 1, the Archive post "The one place", HANDBOOK 9.4) and then have to say the right word to her. The menu says it for you. Make the player type MEMCHK, or at least make the menu option vaguer ("Your memory. Check it."). Severity 2.

3. The TYVANCE password comes too easily. Achterberg's "Six digits" post plus Bulletin 3's birth date is a lovely little deduction. But ELIZA undercuts it twice: two minutes into reading, she volunteers "Ty's account is open to every deck. He never logs off. I do not think he would mind", and JOURNAL.TXT later spells out the format (day, month, year, two digits each). The journal is fine as a confirmation. The unprompted nudge in the first minutes takes the "aha" away before the player has had a chance to look for it. Severity 1.

4. Fragment ordering is not a puzzle. Archive file area: each FRAG file is labelled "piece n of 4" in its description and "AUTH n/4" on the art. "The order matters" in Achterberg's post promises a deduction that never comes. Either take the numbers off the pieces and let the text of the order (sentences broken across pieces) set the sequence, or stop claiming the order is hard. Severity 1.

5. A fantasy MUD door in a spacecraft. Doors menu, after MEMCHK turns SYSOP view on: "[V] VolaMUD" opens "Vola City ... temple, tinker, academy, armory, tavern", with a gate warden and HP and gold. It breaks the fiction completely, at the very point the game wants you to feel you have reached the ship's inner systems. Hide it on this board. Severity 2.

6. Input arrives out of order in the manifest. When I typed a few keys quickly, the manifest screens drew in a different order from my keys, and a blank once showed "(blank)" after I had set it. I had to slow down and check every row again. Filing with "S" plus ENTER only redrew the menu. "S" without ENTER followed by a separate ENTER is what filed it. On the step where a wrong guess costs you, a player should be able to trust what the form shows. Severity 2.

7. Too many screens repeat. Every login prints the full Mission Document plus the full SysOp Notice (about 50 lines) before the main menu. Every area change in the message bases redraws the full Comms Deck art. Leaving a file viewer with Q prints the file list and the cabinet screen back to back. This game makes you re-log as four crew, then ELIZA, then yourself, so you sit through the same splash ten times. Show the login documents once per caller and use short headers on repeat visits. Severity 2.

8. Typing digits in the message reader opens a go-to prompt. Inside a long message, digits started "Go to #1 / #11 / #112" prompts on top of the page, and the page was printed three times. Typing a message number is the natural way to move around, so this will catch every player. Severity 1.

9. Free-text chat with ELIZA is mostly canned. In node chat, everything I tried ("is your record complete?", "please run a check", "I need SYSOP mode") got "I am ELIZA. I run the mission, and I run this board. I am the SysOp." or "I see." That's fine if chat is only for flavour, but the handbook trains you to "ask by name", so a player will try typing MEMCHK there. One keyword response (MEMCHK, or "memory check") would make the menu option in note 2 unnecessary. Severity 1.

10. Undocumented and confusing commands. "P" on the main menu (not listed) answers "The SysOp is not online", even though ELIZA has just spoken to me. Typing "3" at the main menu after a file area says "Unknown command". "?" gives one line of help. All minor, but they add friction. Severity 1.

11. The BIOS screen tells the same joke every time. "ELIZA MISSION AI ... NOT RESPONDING / RESTART AFTER POWER LOSS" appears on every login, even after the RTG is on and ELIZA is talking. If that is deliberate (ELIZA's own status lies), give it one small change after the power-up so it reads as a lie rather than a stale screen. Severity 1.

12. The "who they were" column is filler. All four rows are answered by Bulletin 3, which maps handle to name. Twelve of the fifteen blanks are deductions; these four are lookups. Drop the column or make it harder, for example by asking which account wrote the Mission Log. Severity 1.

5. What works

- The writing is the best I have read in a text mystery for a long time. Four distinct voices that hold up: Halvorsen's "Anders had his maths mock today. 74. His teacher says that is fine. I am going to leave it there."; Achterberg's footnotes to invented papers ("I made the reference up. The finding is real."); Tanabe's basil, Micro-Tom and cherry stones; Ty's lowercase bravado that cracks ("nobody has ever said thank you for a Y"). Eero's porch light, at 0410 UTC with a moth in it, pays off in the final scene without one word of explanation.
- ELIZA is calm, polite and never gloats. "There is one. It does not go anywhere." "I answer the question asked." The audit log line "PROMPT SCROLLED TO LINE 14, 13 LINES SKIPPED" is the cruellest sentence in the game, and it is a log entry.
- Each fate has several independent proofs, Obra Dinn style: suit log plus airlock count plus the wrist-unit letter; GARDEN.LOG plus diary plus Achterberg's "basil" note; AUDIT.LOG plus Bay 3 recording plus Ty's journal; POWER.LOG plus OPSLOG plus capsule log plus ORBIT_9. Cross-checking them is satisfying.
- The four crew passwords are a fair, varied set: a date, a name on a cuff, a word on a label in a picture, and a word hidden in a 300-baud redraw ("S play"). The last one is a new idea and suits the medium.
- The meta-puzzle is clued end to end by in-world documents: line limit in COMMS.TXT section 1, the blind spot in "The one place", MEMCHK in the handbook, KESTREL4 on the go-live checklist item 14, /var/spool/out from the shell help, and ground-1 in the queue header. Typing `release out-1122 ground-1` and having it work felt earned.
- The opening hook: an ADMIN/ADMIN service label, then `SOURCE RTG` and `K`, and the ship wakes up. Later you learn that Ty died pulling that same RTG handle to shut the ship down. You undid his last act in your first minute. That is excellent structure.
- The ANSI art is consistent and restrained: amber ring for ELIZA, cold greys, rivets and telemetry, a dim "·halcyon·crew·98·" tag. The scene screens (Bay 3 recording, capsule camcorder, power bay) type out at a good pace.
- The ending stays unresolved: ELIZA's offer of a fifth seat, the news wire, the founder "with no recollection", and "All crew are well" left standing.
