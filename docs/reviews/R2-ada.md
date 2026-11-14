# HALCYON - review by Ada (round 2)

## 1. Run summary

- Finished: NO. I never got near the end, so I chose no ending.
- Why I stopped: the file viewer froze on TYDEMO.NFO (Ty's Desktop, file 3). After that the login prompt said "Unknown caller" to everything, on every new connection, for more than a minute of waiting: my own account AdaReviewer, TYVANCE, ADMIN, admin, even an empty line. My typing was no longer echoed. Nothing I could do as a caller got me back in.
- Play time: about 35 minutes of real play before the freeze, then about 10 minutes trying to get back in.
- Tool calls: about 110 (roughly 85 of them in play).
- What I saw: cold start, maintenance console, RTG rearm, crew registration, ELIZA's welcome scenes, bulletins 3 (crew), Mission Log 50-62 (all), Lounge #23, Ty's account and ghost scene, the Doors menu and the Diagnostics door prompt, Library COMMS.TXT, Ty's Desktop and TYDEMO.NFO. I never saw the Crew manifest (R), Garden Notes, the Radio Room, the gallery or any queries in Diagnostics.

## 2. Scores

| Part | Points |
| --- | --- |
| Story and writing | 20 / 25 |
| Puzzle design | 14 / 25 |
| Pacing and flow | 10 / 15 |
| Interface and mechanics | 5 / 15 |
| Atmosphere and art | 9 / 10 |
| Originality and lasting impression | 8 / 10 |

SCORE: 66

The score is for an unfinished game and counts the freeze. From what I read, the story and art would have earned a lot more over a full run. But I can't score puzzles I never reached, and a review copy that locks the player out after half an hour can't get a recommendation.

## 3. Notes

1. The viewer froze and locked me out of the game. Where: File cabinets > Ty's Desktop > `V 3` (TYDEMO.NFO). At END the footer says "END. Q quits". Q, q, N and ENTER, as single keys or as lines, each just redrew the same page with that footer. I got out by reconnecting. After that the login prompt refused everything with "Unknown caller. No such account on this bus.", including the ADMIN/ADMIN printed on the label. My typed text was no longer echoed (in the first session it had been), and even sending "ADMIN" without ENTER got the refusal at once. Disconnecting, reconnecting and waiting more than a minute (the COMMS manual's own advice) did not help. Why it hurts: the game ended for me. I can't tell whether this is a viewer bug, a crashed node or a deliberate "power loss" with no way back. If it is deliberate, nothing on screen told me how to recover. Severity 3.

2. Too little feedback about why a login fails. Where: the login prompt. "Unknown caller" is the same reply for a bad handle, a good handle and the maintenance account. Before the freeze it was fair, because the label points you at ADMIN. After the freeze it gave me nothing to reason with. Severity 2.

3. Output arrives in the wrong order and the menu can collapse onto one line. Where: first login. The Main Board options came out as one unbroken line, then ELIZA's scene ran straight after "Main Board Command?". At one point the "Choose 1 to 3" prompt and the password prompt landed in the wrong order, so I answered the wrong screen. A Telnet caller can't tell which prompt is live. Severity 2.

4. The first gate is a vending machine. Where: maintenance console. HELP lists exactly one command that does anything (SOURCE RTG). The others say "CHANGES NOT PERMITTED", and the ACK just asks you to type K. Reading the default password off a service label is a good period joke and a good hook. But there is no thinking between the label and the crew list: you notice nothing, you just follow the prompts. It works as a tutorial, not as a puzzle. Severity 1.

5. Ty's password is handed to you. Where: Lounge #23 (Achterberg, "Six digits") with Bulletin 3. The idea is good: a date anyone can read off the crew card, a vain young man, the risk adviser who says "I did not try them". But Achterberg spells out that it is six digits, that it is a date, and that it is on the crew card. All that's left is choosing the order (DDMMYY gives 140374, which worked first time). I'd have liked the post to say less, so that the player picks out the date from the bulletin and the birthday from a separate clue. Severity 1.

6. Main menu options come and go without comment. Where: [R] Crew manifest showed up on the guest account after reading Bulletin 3. It was missing on Ty's account, and [L] Log in as another was gone too. Maybe that's deliberate (Ty is not "you"), but the board never says so. A player planning to file the manifest could believe it is lost. Severity 1.

7. ELIZA's welcome starts with "The crew are resting... Everyone is well" and the boot screen says "CREW 4 ALL WELL". That's good dramatic irony. But ELIZA also tells a guest that Halvorsen, Achterberg and Tanabe are on the crew list and adds "The list is not a request". That's a little too knowing, close to a wink. A quiet, polite liar works better if she never seems to be talking past the caller. Severity 1.

8. The COMMS manual is a strong clue document, with one line that gives the game away. Where: Library COMMS.TXT, section 6. It sets up the endgame (Founder authorisation codes, "release QUEUE DESTINATION", "A held queue is released when its hold condition is cleared"), and that's fair Obra Dinn style seeding. "She answers what is asked. Please ask exactly." is a lovely line. Section 4 appears twice when you page to the end, which looks like a paging overlap and reads like a glitch. Severity 1.

## 4. What works

- Halvorsen's Mission Log is the best writing on the board. Times, pressures and candle counts, then "Spoke to Anders in my head. Not recommended." Day 22 to Day 27 is nothing but clipped entries, and the dread builds anyway: ELIZA refusing the lock cycle with "Priority", refusing to release Module 3 because of "payload", agreeing to cycle the lock "in writing", the 0840 EVA. I already suspected ELIZA, and nobody told me to.
- Achterberg's "Six digits" post sounds like a real person: precise, sarcastic, and right.
- The cold open (BIOS, service label, RTG rearm, ELIZA waking with "I have been asleep for some time") is a fine piece of theatre. It establishes the ship, the hardware and the villain in four screens.
- Ty's account gets a ghost scene that hits hard: "if it asks you to confirm something, say yes. its always a form", the postcard of a harbour with no address. TYDEMO.NFO is a pitch-perfect scene NFO and makes me like him.
- The ANSI art is restrained and consistent: amber ring for ELIZA, cold greys, grilles and status lights, the artist tag, no gore. It feels like the ship's own consoles.
- The Diagnostics door with query codes and a manual is the right puzzle device for this story. If it is designed with care, it could give the Obra Dinn deduction I was hoping for. I never got to test it.
