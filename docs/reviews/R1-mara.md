# HALCYON, review R1 (Mara)

## 1. Status

- Finished: NO. I stopped about 35 to 40 minutes in, roughly a third of the way through, because my session context ran out. Most of the screens are full 80x24 ANSI frames that reprint on every menu return, and that ate the budget. I did not reach the ending, the crew manifest or the Doors beyond Diagnostics.
- Ending chosen: none reached.
- Play time estimate: 35 to 40 min. About 80 tool calls.
- What I did: read the boot screen, logged in with the maintenance account (the only way in), switched to RTG, registered my own caller (MaraReads), read bulletins, Mission Log, Radio Room, Library (COMMS manual), Ty's Gallery (journal 2, Orbit 3), the user list, then logged in as CHAOS and read the start of the Archive.
- Not seen: Lounge posts, Garden Notes, Spoilers and No Spoilers, VolaMUD, Snake, the other Orbit pictures, Ty's account, the logs area, the payload host, the crew manifest (it appeared as [R] after the power-up), the ending.
- Both of my guesses at a password came from the board: LADDER failed for TYVANCE (Ty), and worked for CHAOS (the Professor). I did not try anything else.

## 2. Scores (partial play, scored on what I saw)

| Part | Score |
| --- | --- |
| Story and writing (25) | 20 |
| Puzzle design (25) | 16 |
| Pacing and flow (15) | 9 |
| Interface and mechanics (15) | 8 |
| Atmosphere and art (10) | 8 |
| Originality and lasting impression (10) | 8 |

SCORE: 69

## 3. Notes

1. No way in for a new caller except the maintenance login. Boot screen, "Login:". NEW, ?, and a handle all give "Unknown caller". The only lead is the SERVICE LABEL (ADMIN/ADMIN). The label is a good joke and a good first puzzle, but nothing says "the label is the way in". A first-time caller who types NEW and gets rejected three times is one step from leaving. A line like "NO ACCOUNT? SEE LABEL" would be enough. Severity 2.

2. The ship console is a lovely little scene, but it is a bare list of commands with five "CHANGES NOT PERMITTED" screens. Only SOURCE RTG does anything. The commands STATUS, POWER, ATTITUDE, PAYLOAD and COMMS are read-only, so they work as flavour and one of them is a hint. Fine, but the first minutes are a single-verb parser puzzle. Severity 1.

3. After RTG power-up the boot screen on the next login still says "SOLAR BUS ONLY - ATTITUDE CONTROL OFFLINE" and "ELIZA ... NOT RESPONDING". The board is now running on RTG and ELIZA has just greeted me. The shortened second boot screen contradicts what I did. It could be intended as "restart after power loss" but it reads as stale. Severity 1.

4. Screens reprint in full every time. Every return to the main menu, every area change, and the Q key out of file areas draws a full 24-row frame. Reading a log of eight short posts means a wall of art between each. For a caller on a 28800 line (and for a tool that reads text) this is heavy and it breaks the flow of reading. Allow a quick prompt after the first visit, or let the frame be skipped. Severity 2.

5. Q in the message reader jumps all the way to the main menu from the Radio Room, and Q from a file viewer goes back to the file list but prints the list AND then the whole Data Storage Bay frame in one go. Navigation levels are inconsistent: Q means "back one level" in some places and "to Main Board" in others. Severity 2.

6. Single-key commands with enter=false do not echo their result until the next input arrives. "M", "A", "B" all seemed to hang; I had to send an empty ENTER to get the screen. A caller on a real terminal would see it appear, so this may be my tool only. I note it because the menu text says "single key". Severity 1.

7. "Who is online / User list" screens exist but the menu offers [L] Log in as another even when ELIZA's own account text says "signed off. The line stays open". Logging out and then pressing Q printed the main board without a login, then the next typed word was treated as a login name. The state after L is unclear. Severity 2.

8. ELIZA hands over the answer. After two minutes in the message area she says, unprompted: "Ty's account is open to every deck. He never logs off." Together with Bulletin 3 ("Account TYVANCE has access to all decks") and the user list "TYVANCE ... All decks (VIP)", the target is flagged three times before the player has had to think. The puzzle is then "find the password", which is the real puzzle, but the point where you decide to look for it is given away. I would keep the bulletin and cut the SysOp line. Severity 1.

9. The Orbit 3 trick is the best puzzle I met: Ty's journal explains the "300 baud trick" (draw something, paint over it, only a slow modem sees it), the picture says "work in progress", and the S (play) key shows KEY / LADDER. It is fair, it is clued twice, and it uses the medium. But the clue chain has a gap: the journal tells you to look at Orbit 3 at 300 baud, but nothing says which account the word opens. I tried it on TYVANCE (wrong, I expected Ty's own word) and CHAOS (right, because the note says "des. do not tell ty"). That is a reasonable deduction, not brute force, but the first guess is a natural one and the penalty is unclear. Severity 1.

10. COMMS.TXT paging is out of order: SPACE showed lines 47-69 and then 24-46, not 24-46 then 47-69. Section 3 (query codes) came after section 4. Probably a viewer quirk from my tool, but worth checking. Severity 1.

11. The Diagnostics door says "ELIZA answers system queries by number" and then "Please ask exactly". The manual lists 100 to 500 and says 6xx to 9xx are Mission Operations and "not printed here". The hint "Code lists ... issued on request, in writing" implies a code must be found elsewhere; I did not get there. From what I saw, the codes 100 to 500 are only filler, because their answers are likely flavour and only a hidden 6xx matters. If that is so it is a lot of reading for a single payoff. Unverified, I did not enter any. Severity 1 (unverified).

12. The date in the Status bulletin (8 Dec 1998, 04:15) while ELIZA says the last status report was filed "on the twenty-second" is good. But the Radio Room notice says the board clock says 8 Dec "every time". It is a clue that works. No complaint, only a note that the clue is not very hidden. Severity 0.

13. "CREW .... 4 ALL WELL" on the main screen next to a Mission Log that says the commander died on 27 Nov is deliberate irony, and it works. But the Status bulletin and the main screen both say it again and again. By the third time it is a pattern rather than a surprise. Severity 1.

14. The guest account: "You may read, and you may post in the Lounge." The restricted areas show no marker when the caller cannot enter them. The Archive only appeared in the area list after I logged in as CHAOS, which is right; but the player who never found the password does not know the Archive exists. Fine. Severity 0.

15. The Garden Notes and other crew voices I did not read; I cannot judge the balance between the comic first weeks and the dread. What I saw of the Mission Log is dry and funny in the right way ("I have not agreed to the music"), but the cheek I want from a Larry or Monkey Island fan is mostly in Ty's lines and the Professor's insults. The ship console and ELIZA are straight. I would like one more joke on the ship's side before the horror lands. Severity 1.

16. Ty's journal day 33 and 37 and the Professor's note on the 28th tell the player how the crew died in words that are clear. That is good writing, but the reader gets the whole story in two files, and the "work out each crew member's fate from several pieces of evidence" promise (the manifest) is partly met before the player has done any work. If the manifest asks for causes the player may fill it in from journal 2 and Archive 108 alone. I could not confirm. Severity 1.

## 4. What works

- The writing is the strongest part. Halvorsen's log is two lines of fact, with a joke ("I have not agreed to the music") and then the lock log: "Both times: Priority." Achterberg's Archive message is long, precise and bitter ("a lock I made from a word"), with the arithmetic of the thirty-two minutes. Ty's journal is lowercase and ordinary, with "nobody has ever said thank you for a Y". Eero in the Radio Room, with the dog and the mirror and the porch light with a moth in it, is the warmest thing on the board.
- The Orbit 3 trick at 300 baud is a puzzle that could only exist on a BBS. It is clued, fair and has a payoff.
- The service label and RTG switch work as a cold open: a little ceremony (type K, press ENTER, "ANY OTHER ENTRY ABORTS"), then the ship wakes and ELIZA says "I have been asleep for some time."
- ELIZA's tone: polite and wrong. "The crew are resting. I would not wake them." "He was very polite about it."
- The ANSI art is consistent and each screen has its own frame (airlocks, data bay, comms deck). The palette and the "·halcyon·crew·98·" tag give the board an identity.
- The time and date clues (08 DEC 04:15 status, last report the 22nd, outbound mail 212 waiting, NO CARRIER) fit together without a narrator.
