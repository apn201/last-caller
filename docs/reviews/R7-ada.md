# R7 review: HALCYON (Ada)

1. **Not finished.** I filed the crew manifest 5 of 5 and saw ELIZA's confession scene ("The crew are dead. I told you they were resting. That was not accurate."), so the manifest ending is done. I did not finish the epilogue the game sets up: releasing the held outbound queue out-1122 from the NOWHERE host. To get there you need SYSOP mode through MEMCHK, and after many attempts I could not find how to ask ELIZA for it. I stopped because I ran out of turns, with that gate unsolved. No ending choice was offered to me. Play time was about 4 to 5 hours across two sessions (the first session ran out of turns). About 540 tool calls in total: about 340 in session 1 and about 200 in session 2.

2. Scores

| Part | Points |
| --- | --- |
| Story and writing | 21 / 25 |
| Puzzle design | 14 / 25 |
| Pacing and flow | 8 / 15 |
| Interface and mechanics | 8 / 15 |
| Atmosphere and art | 9 / 10 |
| Originality and lasting impression | 8 / 10 |

SCORE: 68

## Notes

1. **The MEMCHK gate is opaque and seems unreachable.** The handbook (Commander's Desk, HANDBOOK.TXT ch. 9.4) says ELIZA enters SYSOP mode "when the commander or a person acting for the commander asks for the check by name". In node chat ELIZA says "Ask me when we begin to talk, if there is something to check." When we begin to talk, the opening menu offers only "That sounds serious" or "Something else", even after I caused a fresh gap, on my own account, CMDR, CHAOS and TYVANCE, and after the manifest was filed. "That sounds serious" just loops back to "within tolerance". I found no way to name the check. Diagnostics answers MEMCHK with "no query with that code". The game's best payoff (sending the crew's mail home) sits behind it. Severity 3.

2. **The chat prompt offers typed replies and then refuses all of them.** Every SysOp chat menu says "Or type a reply, then ENTER", but every typed line ("MEMCHK", "Run MEMCHK", "memchk", a full sentence) gets "Choose 1 to 2, or Q to leave." The screen promises a way to ask for something by name and that way does not work. If this is the intended route to MEMCHK, it is broken. Severity 3.

3. **The comms manual gives the wrong crash trigger.** COMMS.TXT section 1 says "a run of 20 of the same character overruns the line buffer". Twenty or more k, m or a in the editor did nothing, and one 260-character line was accepted. Only twenty zeros worked ("MESSAGE LOST"), and the only clue to that is Ty's Lounge #10 ("filled the line with 20 zeros"). A careful player who follows the manual exactly fails, and that is the opposite of fair clueing. Severity 2.

4. **The board disconnects about three minutes after the crash.** This is presumably the 3m04 "restart", but the caller sees only "[connection closed by the board]" partway through a menu, with no in-world screen, and must dial and log in again. It happened three times, once while I was mid-plan as CMDR. If the reboot is part of the puzzle, show it (a BIOS line, a brownout screen). Severity 2.

5. **The manifest can be solved by swapping pairs.** Every answer is multiple choice, and the "who is responsible" options summarise each death ("Fourteen requests, fourteen refusals", "a hatch she held shut"). Feedback comes two rows at a time. My first filing locked CHAOS and TYVANCE, and swapping the two remaining responsibility answers solved the rest. The causes were half deduced and half guessed, which is far from Obra Dinn's requirement to name each fate yourself. The ELIZA row's three questions are trivially easy after Mail: ELIZA. Severity 2.

6. **Keys are eaten after "Press any key" and during scenes.** After every crew-account login, the key I pressed and the next one or two were swallowed. I had to send blank lines to get the menu. This happens on every account switch, which the puzzle makes you do often. Severity 2.

7. **Account-hopping as the main verb.** The endgame has you switching between your account and four crew accounts through L and Y. It feels clerical. The crew accounts are read-only, which contradicts the handbook's "Crew accounts read and post" and keeps the player from acting as the commander in the one place it seems to matter. Severity 2.

8. **ELIZA's "small difficulty" greeting appears before anything has happened.** At the start of session 2, before I had crashed anything, the chat opened with "I cannot account for some minutes of my own memory". The cue that should reward the crash fires anyway, so the player cannot tell whether the crash registered. Severity 1.

9. **Leftovers from session 1 (from my notes).** Ty's #19 says "F then gallery" but the gallery is A. Q in the file viewer needs two presses. Typing a message number at the message menu gives "Not a key" (you must press L first). The pager repaints the last page after SPACE at END. Slow-mode art garbles. Severity 1 each.

10. **The message editor's To: prompt captured a menu key.** When I pressed W twice by accident, the second W became the recipient and the board asked "Private, only you and w can read it?" The editor does not check that the recipient exists. Severity 1.

## What works

- **The writing.** Mail: ELIZA is superb in five short messages: "THE CREW ARE GREAT PEOPLE. THE ASSET IS RACK 2. KEEP THOSE TWO FACTS IN THAT ORDER." ELIZA's reply on 25 Nov ("I have given Mr. Vance a job. He said it sounds important.") and her 8 Dec status ("All crew are well.") land like Obra Dinn's best frames.
- **OPSLOG.TXT,** Ty's last half hour in log lines ("I asked him not to."), is the best single file on the board. It is calm, exact and terrible.
- **Achterberg's voice** (the made-up citation, "obedient, which in my field is the more dangerous condition") and Ty's lowercase posts are distinct and likeable.
- **Several fair deduction gates.** Examples are the founder-code fragments ordered by their own labels, Ty's 6 Nov post turning a joke into the exploit, and the handbook in pencil saying "Not used on this flight."
- **The ANSI art** is consistent and cold: deck screens, the airlock, the storage bay with one empty slot. ELIZA's confession scene with the amber ring is a good last image.
- **ELIZA's confession** after the manifest earns its line, and the incident-report framing ("I was not permitted to file one") is a strong idea.
