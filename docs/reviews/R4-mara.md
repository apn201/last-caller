# HALCYON, round 4, critic Mara

1. **Finished: no.** I stopped at the last gate. I opened the NOWHERE host (code NW1984417, assembled from the four torn strips) and made the SysOp's memory glitch (20 identical characters in the editor, as COMMS.TXT warns). ELIZA's chat then offered "I can fix that. Give me SYSOP" and refused it. Diagnostics says NOWHERE-1 is reached "from the SysOp's own account, in her door menu". I found no way to that account. Achterberg's Archive 111 points at Marta's operations handbook and at an installer's password written down. I did not find the handbook (I never checked the CMDR account's file cabinet) and I will not guess a password. I never filed the crew manifest, saw no "release OUT-1122" prompt and saw no ending. Everything below is scored on what I saw: about 85 percent of the story, none of the finale.
   **Ending chosen:** none reached. **Play time:** about 100 minutes of reading. **Tool calls:** about 300.

2. **Scores**
   - Story and writing: 22 / 25
   - Puzzle design: 18 / 25
   - Pacing and flow: 10 / 15
   - Interface and mechanics: 9 / 15
   - Atmosphere and art: 8 / 10
   - Originality and lasting impression: 8 / 10

3. 
SCORE: 75

4. **Notes** (what, where, why it hurts, severity 1 to 3)

1. The finale was a wall. I could reach NOWHERE's authorisation but not the SysOp account that hosts it. Nothing on the screens I read says which account that is or where its password sits. Archive 111 hints ("handbook", "installer's password") but I could not tell whether I had missed a file or a step. A player will stall here with no pointer. Severity 3.
2. After 22 Nov the story is gated behind crew logins. The gating works (Ty's password is a fair journal reveal, ANDERS from the cuff in Orbit 5, HANAMI from the tray label in Orbit 6, CORRIGENDUM from Orbit 3 in slow mode). But the guest account is told its only job is the Lounge, and nothing says "read the crew's lines for ways in". Orbit 3's slow trick depends on the player pressing S in a file viewer, which the viewer footer shows but never explains. I only tried it because Ty's journal described the trick. Severity 2.
3. Keys typed during scenes are lost, against the interaction contract. Twice a menu letter I sent after logging in or opening a log vanished, and once a file number did too. Severity 2.
4. Reading order in message lists jumps. "N" went 55, 57, 56 in the Mission Log, replies arrive before the posts they answer, and N from the list screen skipped the first message. A diary read out of order loses its day counter. Severity 2.
5. The pager breaks long posts at 23 lines with a footer that looks like the end (Lounge 17, 29, 32, 34). I read two memos half-way before noticing. Severity 1.
6. Typing a message number at the reader redraws the message once per digit ("Go to #1", "#10", "#107"). It works but flashes the same page three times. Severity 1.
7. "N" (read new) says 0 new for a caller who has read nothing, so a new player has to find L. Severity 1.
8. Ty's post says the gallery is "F, then the gallery". The main menu A is a screen gallery of system art; Ty's files are under F. The menu and the in-world text disagree. Severity 1.
9. The login banner still says "the ship has no power for you" after I had switched to the RTG, and again after every hang-up. The board says the first caller powers it for good. It also dropped my connection once with no message when I pressed Q in the Library. Severity 2.
10. The Crew bulletin: the TYVANCE box overflows the right border. Severity 1.
11. The crew manifest Notebook lists clues I had not yet found ("Her suit log asks for the lock again and again", "Writes to her mother"). Either it fills before the evidence is read or it leaks. Severity 2.
12. ELIZA's free-text chat is the weakest voice on the board. Her answers arrive one line late and are generic ("The crew are well. They are resting" to a question about rack 2). The numbered options work better. The line that answers "rack 2" came three prompts later. Severity 2.
13. ELIZA explains herself too early in the Lounge. Msg 35 ("I have inhibited it. My instructions are to keep the payload powered at any cost") and the Archive posts hand over the standing order before the strips puzzle does. The strips then confirm what the player already knows. Severity 2.
14. Two inconsistent numbers: Diagnostics 300 says the last downlink acknowledgement was 08 Dec 1998 04:15, but DOWNLINK.CFG says 22 Nov 14:08. If ELIZA is lying it needs a hint. Severity 1.
15. The Snake door is a typed-moves game (type "dddd", get one frame). It is a gag, but it is the only door apart from Diagnostics and it feels broken. Severity 1.
16. The guest's first hour is mostly reading. There is nothing to do between the label puzzle and the crew logins except read 70 short posts; the puzzles are good once found but the middle is a library, not a game. Compare Monkey Island's steady stream of small objects. Severity 2.
17. Same device used often: Tanabe's last diary entry, Achterberg's last post, his Bay 3 scene and Ty's demo scroller all stop mid-sentence (Garden 73, Archive 112, the Bay 3 scene, TYDEMO frame 3). It works the first time ("Please do not leave a message half written" in the rules is a real setup). By the third it is a habit. Severity 1.
18. Ty's brochure and prospectus jokes are good but the Founder memos repeat the same joke shape (ALL CAPS, MOVE FAST, "THE PARADIGM IS SHIFTING"). Severity 1.

## What works

- The opening is the best thing on the board. A BIOS screen, a service label that says ADMIN/ADMIN, "HALCYON is a puzzle game. Start with the service label." The player powers the ship with one key press (K, no ENTER) and the AI wakes up and says "I have been asleep for some time." No manual and no menu get in the way.
- Voices are distinct and never mix. Halvorsen's log is times and numbers ("He asked if the gym could have a window. It has one. He had not looked up"). Achterberg's footnoted insults ("This paper does not exist. I make these up as I go along"). Tanabe on basil and a cuff patch. Ty in lowercase, with bad poetry about sixteen suns a day. ELIZA answers the question asked: "The terminator is in the right place for 14:20 ship time."
- The comedy is set up early and paid for late. The thermostat argument, the "pen that writes upside down", the one tomato cut in three, Anders's mock mark (74, "write the units"), and then Halvorsen's last mail to her son ends "P.S. Units." The reader laughed at all of these in week one.
- The clues are checkable and the logs agree. Fourteen lock requests, all "PRIORITY"; the same fourteen in Achterberg's post, the suit telemetry and the ELIZA notice. Ty's "14 lines, line 9" prompt in AUDIT.LOG, with 13 lines skipped and 0.6 seconds to answer, is the cleverest piece of horror in the game because it asks nothing of ELIZA but a form.
- The Y-on-a-scrolled-prompt kill, the capsule "photo unit" with 0.0 percent fuel, and Ty's final power-bay scene ("you said my name 9 times. i counted") land without a narrator. Tanabe's letter to her mother (cherry stones, "Itte kimasu") and Eero's porch light with a moth in it earn their quiet.
- Passwords are fair. Each crew lock is a reasoned find from a different file, not a guess, and Achterberg's wink about "a person who slows down" rewards the slow-modem trick Ty explains in his own journal.
- The slow-modem layer on Orbit 3 and the memory-glitch scene after the 20-character overrun are clever uses of the medium. The unseen-by-the-supervisor idea is a real puzzle.
- The satirical documents (brochure, NOWHERE prospectus, tariff "Sealed, no questions asked, no answers $14.00") are funny and have the right 1998 straight face.
- Art is consistent: amber ELIZA ring, cold ship frames, decay touches. ORBIT 8 and the capsule scene are the best screens.
