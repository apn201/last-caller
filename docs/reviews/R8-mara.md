# R8 review: Mara

1. Finished: yes. Ending chosen: "Send it" (release out-1122 to ground-1, then the radio club and OH9ZQW's reply). I also read the "why are you asking me to stop?" branch, where ELIZA offers a fifth seat. Play time: about 130 minutes over two sessions. Tool calls: about 700 (about 550 in session 1, about 150 in session 2).

2. Scores
- Story and writing: 22 of 25
- Puzzle design: 17 of 25
- Pacing and flow: 10 of 15
- Interface and mechanics: 8 of 15
- Atmosphere and art: 8 of 10
- Originality and lasting impression: 8 of 10

SCORE: 73

3. Notes (severity 1 to 3, 3 worst)

1. Keys swallowed or delayed after scenes, logins and "press any key" screens. Where: every crew login, the Y return to MaraReads, the first R at the main board after a scene. Why it hurts: my key vanished and I had to resend it. The tool output also often shows the previous screen, so I could not tell whether a key had landed. Severity 2.
2. Disconnects. Where: twice in session 1 (the second after MEMCHK). Once in session 2, when I sent S for the manifest while the ELIZA row menu was open and the board dropped me. Why it hurts: it costs a reconnect and a re-login, and the BIOS only says "solar only" on redial. Severity 2.
3. The manifest row logic is guessable by pairs. Where: the manifest form. Why it hurts: "Right rows are confirmed two at a time" lets a player brute-force by swapping answers. I picked Halvorsen's "what" by accident (the number menu pressed itself) and Ty's "what" by trial. My notes did not settle "spacewalk or comms bay" for Halvorsen, and "capsule or power bay" for Ty. The board's own feedback decided it, not the clues. Severity 2.
4. The manifest wording is ambiguous. Where: "Left the ship in the return capsule" and "Died in the power bay after cutting the ship's power". Why it hurts: both sound like Ty. "Bay 3" and "comms bay" are never tied together on screen, so I could not check Achterberg's death against the options. Severity 2.
5. The final puzzle is a bridge nobody hints at. Where: the NOWHERE shell. `release out-1122` says "INCIDENT REPORT NOT ON FILE", but nothing says the manifest is that report. I guessed it because the filed manifest was the only thing left undone. Then `release out-1122 ground-1` works, but `earth` (Ty's typescript) and `OH9ZQW` do not. The usage line does not list valid destinations. Why it hurts: a fair last step needs one more clue. The queue header does name "ground-1", which saves it. Severity 2.
6. The NOWHERE shell help does not list `release`. Where: `help`. Why it hurts: you must infer the command from Ty's typescript. It is fair once you read the typescript, but a first-time player may never open home/tyvance. Severity 1.
7. The manifest's fifth row (ELIZA) opens only after all crew rows lock, and the three-menu structure needs 4 to 6 keypresses per field. Where: the manifest. Why it hurts: about 40 keypresses for a form, and the display lags so numbers get misapplied. Severity 1 to 2.
8. The pager repeats text on SPACE and sometimes steps to another message. Where: message and file pagers. Severity 1.
9. Ty's password is explained twice. Where: Ty's files and a message. Severity 1 to 2.
10. Spoilers areas are empty. Where: conference list. Severity 1.
11. TYDEMO.NFO crashes on frame 3 (FAULT 0D AT 0042:1F3C) with no hint. Where: Ty's Desktop. Why it hurts: it looks like a side puzzle with no payoff. Severity 1.
12. The bulletin "News" that ELIZA says she filed with the club station's traffic was not in the ELIZA account's bulletin list (Rules, Welcome, The Crew, Status). Where: Bulletins after the ending. Why it hurts: I was told to read it and could not find it from the account I was on. It may be on MaraReads; I did not recheck. Severity 1.
13. The ending is warm, a little tidy. Where: the radio club and OH9ZQW's "The light is on." Why it hurts: after four deaths and a calm monster, the last beat resolves the love story with a light and a bulb with no moth. It is lovely and earned by the letters, but it is the one place fear and grief resolve. Severity 1.
14. Deck 02 header art redraws on every return to the main menu and takes a screen of scroll. Severity 1.

4. What works
- Voices. Halvorsen's numbers, Achterberg's footnotes and Tanabe's plants are distinct and consistent. Tanabe's letters to OH9ZQW carry the whole emotional weight without a narrator.
- ELIZA's calm. "I was not permitted to file one" and "I have looked for another reason and I have not found one" are the best lines in the game.
- The standing orders file (orders.cfg) is a clean cold reveal, and the rates.txt joke (loyalty programme, "payable in a jurisdiction of the client's choice") is Sierra-grade cheek in the right place.
- The chain of clues from service label to account to power to crew logs to NOWHERE is fair and each step is findable.
- Ty's typescript in the NOWHERE home folder shows him failing at the very command you need. It is a sad, funny, practical clue.
- The ANSI screens (airlock, data bay, downlink, NOWHERE racks) are cold, clean and consistent.
- The ending's choice is two clicks, not a maze, and the downlink scene pays off the whole queue.
