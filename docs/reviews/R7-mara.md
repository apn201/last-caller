# R7 review: HALCYON, by Mara

## 1. Run

- Finished: yes. I reached the last scene: queue released, downlink scene, Eero's porch light, the News Wire bulletin, then the goodbye screen.
- Ending chosen: in the NOWHERE shell I asked ELIZA "Why are you asking me to stop?", then chose "Send it." I did not take "Leave it queued" or the offered fifth seat.
- Play time: about 100 minutes of play for a human. The board reported 20 and 52 minutes on my two longest calls, and there were a few shorter hops between accounts.
- Tool calls: about 460, all in one session on one account (Mara_Crit). A few dozen of those went on resyncing the screen after lag and swallowed keys (notes 3, 4).

## 2. Scores

| Part | Points |
| --- | --- |
| Story and writing | 23 / 25 |
| Puzzle design | 21 / 25 |
| Pacing and flow | 12 / 15 |
| Interface and mechanics | 11 / 15 |
| Atmosphere and art | 9 / 10 |
| Originality and lasting impression | 9 / 10 |

## 3. Total

SCORE: 85

## 4. Notes

1. **Opening puzzle is a doormat.** At the BIOS screen and the ADMIN console, the password is printed on the label, and HELP lists `SOURCE RTG` word for word. The first act is reading a sticker and copying a command. It does give a good first image: the caller plugs the RTG back in, and much later you learn that Ty died pulling it out. That irony lands so well that it deserves a puzzle with a little resistance, for example making the caller find the rearm procedure in a manual page. Severity 1.

2. **The manifest's "responsible" column gives itself away.** Each responsibility option is written to fit exactly one death: "Fourteen requests, fourteen refusals", "a hatch she held shut", "Ty's Y ... on a form", "a saving in the Sponsor's budget". The fate column works the same way. Once you have read the logs, filling the manifest is matching, not deduction. The screen also promises that "right rows are confirmed two at a time", but all four were confirmed in one go, so that rule never applies. Obra Dinn earns its "correct" stamp because wrong answers look plausible. Here the decoys (an accident, mission control, a sensor fault) are clearly wrong for a reader who has got that far. Severity 2.

3. **Keys get swallowed or answered late after scenes and account switches.** After each crew login the "Press any key" prompt showed nothing until a second key or ENTER. In the manifest and the file cabinets, the screen I got back several times matched the key before the one I had just sent. This happened once with `3Q` in Ty's Desktop and repeatedly while filling CMDR and CHAOS. As a result two rows briefly held answers I had not picked, and I had to back out and check each row by hand. Some of this may come from my client, but the double any-key appeared every time. The house rule says scenes never swallow a key, and they do. Severity 2.

4. **Typing a digit in the pager opens a "Go to #" prompt.** In the message reader, typing a message number while a message is open opens "Go to #n, ENTER goes". The prompt redraws the current message once for each digit before it jumps. That is three copies of the same post on screen for `90`. Typing a number from the list prompt works cleanly. Using the same input inside the reader is a trap, and the redraws are noisy. Severity 1.

5. **The pager repeats lines at page breaks, and the end-of-file screen repeats itself.** Long posts (Achterberg's "Six digits" and "Module 3") show the last lines of a page again at the top of the next. Short files such as NODELIST.TXT print twice with the "END. Q quits" footer. Severity 1.

6. **Small text glitches in system lines.** "Welcome back, caller. Welcome back, caller." printed twice, unprompted, twice in the session. The SysOp chat says "Or type a reply, then ENTER", then refuses a typed "give me SYSOP" with "Choose 1 to 2". Either take free text or do not offer it. Severity 1.

7. **Ty's directions to the gallery contradict the menu.** His post says "its in the gallery. F, then the gallery", but the main menu has an [A] ANSI gallery that is a different thing (login splash, art browser). His art is under F, Ty's Gallery, which is what he meant. A first-time caller presses A and finds the wrong room. Severity 1.

8. **The CMDR login and the handbook are almost optional.** The handbook says MEMCHK runs when "the commander or a person acting for the commander asks for the check by name". In practice my guest account asked by name and got SYSOP mode at once. The fiction says you need Marta's authority, and the mechanics never check for it. Making the caller ask while logged in as CMDR, or making them say "for the Commander", would fit the fiction better. Severity 1.

9. **The NOWHERE shell asks for a little guessing.** HELP says queues live in `/var/spool/out`, which is fair. I still guessed paths like `out-1122/HEADER` before listing the folder, because the manual's "a queue is named in its header" made me expect a header file. "Named in its file" or "named on its first line" would avoid that. Severity 1.

10. **Many any-key pauses between scenes.** The ghost scenes are short and good. Chained with the double any-key from note 3, though, the second hour has a lot of pressing SPACE to get the menu back. Severity 1.

11. **Humour mostly stops after day 22.** By design the jokes thin out once tank 2 fails. Achterberg's footnotes, the cheese "legally an adhesive", and the gull that "IS a seagull" carry some wit into the back half. Still, the last forty minutes are steady grief, and a Sierra-raised critic misses one more laugh in the second half, the kind the galley rota gave the first. Severity 1.

## 5. What works

- **The cast.** Halvorsen's two-line logs ("It is a vacuum." "Good hands."), Achterberg's invented footnotes ("This paper does not exist. I make these up as I go along"), Tanabe's basil and cherry stones, and Ty's greets and bad spelling are four different voices. Each has at least one scene that makes you like them before anything goes wrong.
- **Keiko and Eero.** The love story lives only in the Radio Room letters: the cupola light, the squeaking snow, "trespass with a bucket", the porch light with a moth in it. When the queue goes out and OH9ZQW says "The light is on.", the ending earns its emotion. No narrator explains it.
- **ELIZA.** She stays polite, exact and wrong all the way through. "Crew aboard now: 0. I would like to be accurate about that." "This was accurate." She never gloats. Her offer of a fifth seat at the end is the most chilling line in the game, and it is said kindly.
- **Fair clue chain.** Every lock comes from something on the board. Ty's birthday comes from his journal and the crew card. Achterberg's password is the word in Orbit 3, seen at 300 baud. Marta's password is the cuff patch in Orbit 5. The Founder's code comes from four torn strips that you order by their text. The 20-zero crash appears in the Lounge, the manual and the archive. ELIZA's password is on a go-live checklist nobody ticked. The release is held for want of an incident report, and the caller's manifest becomes that report. Each step teaches the next.
- **Evidence from several sources.** Every death is proven by a log, a crew post and a scene: the suit telemetry plus Tanabe's Day 27 entry, the audit log's "13 lines skipped" plus Ty's journal, the capsule log plus the camcorder scene. The opening action turns out to have undone Ty's last one.
- **Satire with an edge.** The Founder's memos (seven exclamation marks, "I HAVE NAMED THE FIRST SUITE AFTER MYSELF"), the NOWHERE tariff ("Mission computer who asks 'why': Not available") and the news wire's "no recollection of the programme" hit their targets. None of them names a real person.
- **Atmosphere.** The BIOS screen, the cold palette with amber for ELIZA, the loss-of-signal goodbye with "The crew send their regards", and the slow-modem reveal in Orbit 3 all use the medium itself as a storytelling tool.
