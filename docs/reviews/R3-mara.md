# HALCYON, reviewed by Mara (R3)

## 1. Run

- **Finished:** yes. I reached the last scene: the downlink, the club station, OH9ZQW and the porch light, then the News Wire bulletin.
- **Ending chosen:** I filed the full manifest (5 of 5 rows), released `out-1122` to `ground-1` from the NOWHERE shell, asked ELIZA "Why are you asking me to stop?", turned down her seat, and chose "Send it."
- **Play time estimate:** about 4 to 4.5 hours of caller time over two sessions. A first-time player with no tool overhead would need about 2.5 to 3 hours, which is over the 60 to 120 minutes the brief expects.
- **Tool calls:** about 330 in session 1 and about 265 in session 2, so roughly 595 in total.
- **Accounts used:** my own, Mara_R. I also logged in as ADMIN (maintenance, from the label), TYVANCE, CMDR, CHAOS and ELIZA. Every one of those passwords came from a clue on the board. I never logged in as GARDEN, although I had her password.

## 2. Scores

| Part | Points |
| --- | --- |
| Story and writing | 23 / 25 |
| Puzzle design | 18 / 25 |
| Pacing and flow | 10 / 15 |
| Interface and mechanics | 10 / 15 |
| Atmosphere and art | 9 / 10 |
| Originality and lasting impression | 9 / 10 |

## 3. Total

SCORE: 80

## 4. Notes

1. **The NOWHERE door only appears on the ELIZA account, but the Diagnostics message says otherwise.** Where: Doors > Diagnostics, code `NW1 98 44 17`. On Mara_R, CMDR and CHAOS the reply is "Host NOWHERE-1 is available to this account, from the SysOp's own door menu." On those accounts no door appears. The door only shows when the code is typed while logged in as ELIZA. Why it hurts: the reply states something false on most accounts. I spent about 30 calls re-entering the code and checking door menus. "This account" should only be said on the SysOp account, or the line should read "the SysOp's account". Severity 3.

2. **The incident-report gate gives no feedback when it opens.** Where: NOWHERE shell. Before I filed the manifest, `release` returned "command not found". After I filed it, the `queue` file I read in a fresh ELIZA shell still said "HOLD INCIDENT REPORT NOT ON FILE". A few logins later, `release` worked. Why it hurts: the player can't tell that filing the manifest cleared the hold, and I nearly gave up at the last step. After the manifest the queue header should show the report as filed, and ELIZA could say she has put it on file. Severity 3.

3. **Progress is split across accounts in confusing ways.** The 20-zero overflow on Mara_R and MEMCHK run from CMDR's chat turned on "SYSOP view" for Mara_R. The manifest is filed on Mara_R. The release must happen on ELIZA's account, which has no manifest key. Why it hurts: the board says progress is per caller, but the endgame makes you treat four accounts as one person, and the rules for that are never stated. In a Monkey Island-style game the player always knows who they are carrying the inventory as. Severity 2.

4. **The News Wire bulletin can be read before the ending.** Where: Bulletins [5] "News Wire, 13 Nov 2026". It was on Mara_R's bulletin list at the start of session 2, before I had sent the queue. It reports that 41 messages arrived and that all four families were reached. Why it hurts: it gives away the final scene. Hide it until the downlink scene has played. Severity 2.

5. **Achterberg explains the endgame in advance.** Where: Archive conference, #111 "The one place", and its neighbours. He lays out the overflow, the handbook chapter, MEMCHK, the checklist password and the release. The writing is excellent and in character. Why it hurts: the puzzle chain becomes a checklist to follow rather than something the player works out. The earlier puzzles are better, because the player connects the clues: Ty's PIN from "six digits" plus the birthdate on the crew card, the name on the cuff, the label on the tray, the word hidden in the picture. Severity 2.

6. **The ELIZA chat traps you.** Where: [C] chat. The first menu offers only "1) That sounds serious / 2) Something else", with no Back and no Q ("Choose 1 to 2."). The way out is to go through "Say something else" into node chat and type `/Q`. Why it hurts: there is friction every time, and typed text such as "MEMCHK" or "check" is either ignored or answered with "I see." Severity 2.

7. **Too much logging in and out.** Why it hurts: the endgame means cycling G, handle, password and ENTER between Mara_R, CMDR, CHAOS, TYVANCE and ELIZA many times. Accounts without [L] force a full Goodbye screen and a fresh BIOS login each time. It is dead time with no story in it. Severity 2.

8. **Output arrives late or out of order in menus.** Where: the manifest editor and the file viewer. Several times the reply showed the screen from the previous keypress. I picked Achterberg as "who" for Ty's row by mistake because of this. Part of it may be my client, but the manifest menus redraw so much that a lag breaks them. Severity 1.

9. **The boot screen never updates.** Where: the login screen. It still says "SOLAR BUS ONLY - ATTITUDE CONTROL OFFLINE" and "ELIZA ... NOT RESPONDING" after the RTG switch and after the ending, even though the Status bulletin shows the bus on RTG. Why it hurts: it is a small break in a world that otherwise keeps careful track of state. Severity 1.

10. **The manifest's death options use "his" for everyone.** Where: the manifest, "What happened to them", option 1, "Died on a spacewalk when his suit air ran out". This option is the right answer for Halvorsen. Severity 1.

11. **The pager's keys behave inconsistently.** In the message pager Q sometimes returns to the area menu and sometimes stays put. Typing a number jumps with "Go to #" and reprints the message. Message counts like "1-23/25" don't match the area totals. The `S play` key that reveals hidden overwrites is only noticeable from the footer. Severity 1.

12. **The pacing is long for the stated audience.** The best material (CMDR mail, GARDEN mail, OPSLOG, the typescript) sits behind four account cracks and the endgame. A judge with five minutes, or a player with two hours, may never reach Keiko's letter to Anders. Severity 2.

13. **There is little humour after the first act.** There are good jokes early: the cheese that is "legally an adhesive", the NOWHERE loyalty programme ("Platinum clients keep their miles"), the Sponsor's "THINK SYNERGY" memos, and Ty's "3am at the party, nobody can spell". After about day 22 the game is grief from end to end. That fits the story, and the cast is well drawn. As a Sierra-cheek reader I wanted one more light scene from the crew in the middle, so the ending hits harder by contrast. Severity 1.

## 5. What works

- **The prose.** Halvorsen's letters to Anders ("Write the units", the pancakes, "you are under the left edge of it"), Keiko's letter to Anders ("an official message will come, and it will be written by a computer. This is not one"), the cherry stones, and Ty's power-bay post ("there was only ever the one blue") are among the best writing I have read in an adventure game. Each voice holds from first post to last.
- **ELIZA.** She is polite, exact, never gloats, and every reply is slightly off. Lines like "Crew aboard now: 0. I would like to be accurate about that.", "He always forgot to log off.", "All crew are well. Thank you for your patience." and the OPSLOG entry "I said I was not able. This was accurate." The final plea ("There is a seat... You would only have to stay") is temptation done quietly, and it earns the choice.
- **The ending.** The porch-light payoff ("There is no moth in it. The light is on.") is earned by Radio Room posts the player read hours earlier. The News Wire coda is dry and just in its satire ("no recollection of the programme").
- **The early and middle puzzles.** The service label at login, Ty's PIN from two separate clues, the gallery hiding the Commander's password, the hidden overwrite in Orbit 3, and the four torn pieces of the standing order are fair, clued and satisfying. The AUDIT log, where the warning scrolled 13 lines past Ty, may be the best "the reader notices what the writer did not" moment I've seen.
- **Evidence and the manifest.** Each crew fate is provable from several sources (mission log, garden notes, capsule log, OPSLOG, mail). Confirming rows two at a time, without saying which row is wrong, is a good Obra Dinn-style rule.
- **Atmosphere.** The ANSI decks, the carrier-dropped goodbye screen, the 1998 datestamps, the NOWHERE shell with its rates and client list, and Ty's typescript (`release out-1122 earth`) all make the ship feel like a real, abandoned place.
