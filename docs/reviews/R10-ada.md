# HALCYON, review R10, Ada

## 1. Status

- **Finished:** no. I stopped at the last gate, about 245 tool calls in.
- **Ending chosen:** none reached.
- **Play time:** about 75 to 90 minutes of play for a first-time caller, estimated.
- **Tool calls:** about 245.
- **What I did:**
  - Powered the ship through the service label (ADMIN/ADMIN, SOURCE RTG, K) and made my own account.
  - Opened Ty's account from a six-digit birth date.
  - Opened CHAOS from the word CORRIGENDUM, hidden in a portrait.
  - Assembled the Founder's torn order and got "Authorisation accepted" in Diagnostics.
  - Opened CMDR with ANDERS.
  - Ran MEMCHK in SYSOP mode after causing the memory gap.
  - Filled three of four manifest rows. Ty's row is still blank.
- **Where I stopped:** the diagnostics door says "Host NOWHERE-1 is open. It is reached from the SysOp's own account, in her door menu." I never found that door menu. Doors & games listed only Diagnostics and Snake, in every account and state I tried. I did not get Tanabe's (GARDEN) password, and I did not reach the "release QUEUE DESTINATION" step. A tester may have missed one step. The board does not tell the player which one (note 1).

## 2. Scores

| Part | Points |
| --- | --- |
| Story and writing | 23 / 25 |
| Puzzle design | 20 / 25 |
| Pacing and flow | 11 / 15 |
| Interface and mechanics | 10 / 15 |
| Atmosphere and art | 9 / 10 |
| Originality and lasting impression | 9 / 10 |

## 3. Total

SCORE: 82

## 4. Notes

1. **The last gate has no signpost (severity 3).**
   - Where: after the Diagnostics message "reached from the SysOp's own account, in her door menu".
   - What: I had SYSOP view on, MEMCHK done, ELIZA's mailbox readable, and the Founder code accepted. Doors & games still showed only Diagnostics and Snake, as Ada and as CMDR. I could not tell whether I lacked a step, an account, a timing window, or a menu refresh.
   - Why it hurts: it is the only place I felt stuck with nothing left to read. The game was fair until here, so this reads as a bug or a missing hint, not as a puzzle.
   - Fix: say which account counts as "hers", or show a locked door entry that names what it needs.

2. **Unexplained disconnects (severity 2).**
   - Where: three times the board closed my connection ("[connection closed by the board]") right after I pressed one key. Examples: the first key in the manifest, `W` in the message menu, and `C` right after logging in as CMDR.
   - What: some may be the goodbye scene or a scripted event, but nothing marked them as such. One key press and the line was gone. I lost a login each time (about 4 calls).
   - Fix: if it is scripted, show a line ("CARRIER LOST"). If it is a fault, fix it.

3. **The login screen contradicts the state (severity 1).**
   - What: after the ship has power and I have an account, the login screen still says "A new caller has no account, and the ship has no power for them. Your first task...". The service label still shows ADMIN/ADMIN.
   - Why it hurts: it is a leftover instruction that is now false.

4. **The "slow" key hides the Professor's lock (severity 1 to 2).**
   - What: the ORBIT_3 picture shows a "work in progress" placeholder. Pressing S ("slow") redraws it with KEY CORRIGENDUM. The Professor's post says "a person who slows down will find it", so it is fair, and I liked the idea.
   - Why it hurts: the pager footer never says what "slow" does. In my capture the redraw smeared the frame twice on one line.
   - Fix: the footer is fine as it is. Check the smeared redraw.

5. **Pager shows overlapping frames (severity 1).**
   - Where: Library COMMS.TXT (the last pages), and some message pages with a repeated "Go to #" status line on screen.
   - What: SPACE re-prints the previous lines with new ones, and "Go to #" fragments pile up in the status line. A person reading this may not see the problem. In the terminal it looked cluttered.

6. **Free-text chat is unreliable (severity 2).**
   - What: ELIZA's chat takes menus. When I typed "restore" or a long sentence, it answered "Choose 1 to 3" and ignored me. Typing MEMCHK worked only under "Ask her to run something, by name", and in the multi-node chat as CMDR.
   - Why it hurts: the prompt says "Or type a reply, then ENTER", so the player expects it to work. The chat has three modes (menu, free chat, run-by-name) with no signal of which one I am in.
   - Fix: say "type a command name" in the prompt where that is the intent.

7. **The 20-zeros step is easy to miss (severity 2).**
   - What: the manual says "a run of 20 zeros, which the line tool fills in, overruns the line buffer". The Professor says "I leave the arithmetic to whoever comes after". So the clue is split across three places: the manual, a Lounge post, and Ty's old joke. It works, and the "MESSAGE LOST" screen followed by the scare scene is a great payoff.
   - Why it hurts: "which the line tool fills in" is odd wording, and the manual number (254 characters) and the 20 zeros do not obviously agree. I got it only because I tried the text literally. A player who types 254 characters may not.

8. **Two near-identical Q/X key habits (severity 1).**
   - Where: message reader, file pager, manifest. `Q` in a reader goes back one level and `A` does nothing. I pressed `A` in a reader several times and nothing happened, with no hint.
   - What: this is a small friction. The rest of the key system is consistent.

9. **Ty's manifest row is not solvable from what I read (severity 1 to 2).**
   - What: the choices are "Died alone in a bay after the ship's power was cut" or "Left the ship in the return capsule". I have Orbit 9 (power bay, "not saved"), JOURNAL2 and CAPSULE.LOG listed but unread. I could not close this row before running out of budget.
   - Why it hurts: I cannot say if the puzzle is fair. The three other rows were.

10. **Some mood in the Y/G handling (severity 1).**
    - What: "Y back to Ada_R10" and "G goodbye (to Login:)" on crew accounts is clear. As the real account, `G` disconnects, as it should. It took me one try to realise.

## 5. What works

- **The opening gate** is a perfect Monkey Island move. The service label is the puzzle, the answer is in plain sight, and the ack key (K, NO ENTER) is a fine small moment.
- **The deduction chain** is fair and layered, and each lock fits its owner.
  - Ty: his birth date, fixed by the Professor's "a man of twenty-four, one date".
  - Achterberg: a word in a portrait, behind the slow pager.
  - Halvorsen: ANDERS on a cuff patch in Ty's picture.
  - The Founder's order: four torn strips that must be put in text order, not file order. That is a real "aha". NW1 98 44 17 was accepted at once.
- **The evidence is cross-checked.** The airlock requests, the audit log lines ("PROMPT SCROLLED TO LINE 14, 13 LINES SKIPPED", CONFIRM Y in 0.6 s), Tanabe's last notes and the Professor's Archive each tell the same death from a different side. The Ty confirmation is a quiet, precise horror.
- **The writing** is the strongest part. Each voice stays in its own register. Achterberg's footnoted insults, Halvorsen's times and numbers, Tanabe's plants, Ty's lowercase, the Founder's capitals. The funniest page is the Y2K memo ("NEITHER DO I, ON A MONDAY"). The saddest is "the lock" mail to Anders ("P.S. Units").
- **ELIZA is calm, polite and wrong.** Lines like "He always forgot to log off", the mail "Status: All crew are well", and the hole scene with "14:09 14:09 14:09" are exactly right.
- **The radio room** gives the 2026 frame with restraint. The porch light line is a good small cut.
- **The manifest and notebook** are good interface design: three blanks per person, a rule that tells you how many rows are right without telling which, and an automatic notebook of what you saw.
- **The ANSI art** is clean and cold. The service label, the RTG tie screen, the deck frames and the torn-strip scans all read well on a terminal.
