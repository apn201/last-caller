# R6 review: HALCYON, by Ada

1. **Finished.** I reached the last scene. Ending chosen: "Send it" in the
   NOWHERE shell (after first asking "Why are you asking me to stop?"),
   followed by the club station, OH9ZQW's porch-light scene and the News
   Wire bulletin of 13 Nov 2026. Play time: about 2 hours 30 minutes over
   two sessions on account ADA. Tool calls: about 105 in session 2. Session
   1 ran out of turns and I did not record its count; my estimate is
   200 or more.

2. Scores

| Part | Points |
| --- | --- |
| Story and writing | 22 / 25 |
| Puzzle design | 19 / 25 |
| Pacing and flow | 11 / 15 |
| Interface and mechanics | 10 / 15 |
| Atmosphere and art | 9 / 10 |
| Originality and lasting impression | 9 / 10 |

SCORE: 80

## Notes

1. **Getting to NOWHERE stalls at the end.** Where: after MEMCHK and SYSOP
   view, session 1. The Chaos Archive code says NOWHERE-1 is "reached from
   SysOp's own account, door menu". The login-as-another list does not
   show ELIZA, and the password sits on line 14 of GOLIVE.TXT in
   Engineering, a file I had opened in session 1 without noticing that
   line. The clue is fair. My stall was over where to look: nothing points
   back to Engineering once you have SYSOP view, and the ELIZA row only
   appears in the L list after the first login. I lost most of a session
   here. A single nudge would fix it, for example ELIZA saying "My account
   was set up by Ground Ops", or the code text naming the checklist.
   Severity 2.
2. **Release order is undefined.** Where: NOWHERE shell. Before I filed the
   manifest, `release out-1122 ground-1` answered "HELD. INCIDENT REPORT NOT
   ON FILE". After the manifest was whole, the same command worked. That is
   good design: the manifest is the incident report. The board never says
   so, though, so the link comes out as luck and the player gets no "aha".
   One line from ELIZA in "The manifest is whole" ("A complete record is
   an incident report. I may not file one.") would make it a deduction.
   Ty's typescript also teaches `release -f`, which the game never needs.
   That is fine as a red herring, but it pulls the player towards the
   wrong fix. Severity 2.
3. **The answer keys drift when you type ahead.** Where: manifest pick
   lists. With keys sent in quick succession, a digit meant for one list
   landed in the next screen. I filed "Sealed in the garden" for CHAOS by
   accident and had to redo it. The ELIZA row showed the wrong answer and
   then the right one. A fast typist on a real terminal will do the same.
   Each pick should echo "CHAOS, what happened: comms bay" before it goes
   back up a level. Severity 2.
4. **Two of the four death options are easy to rule out.** Where: the
   manifest. "Oxygen tank failure of 22 Nov" and "Left the ship in the
   return capsule" are good traps. The four "responsible" options for the
   ELIZA deaths, though, almost name their crew member ("fourteen
   refusals", "a hatch she held shut"), so once the fates are known the
   third column fills itself. Filing "two at a time" also hides little
   with only four rows. Obra Dinn works because the third column takes
   work. Here it is a reading check. Severity 2.
5. **Ty's fate is the most interesting deduction, and it is clued well.**
   Where: OPSLOG.TXT. The capsule "not fuelled. Occupant not informed",
   then "Occupant thanked me for the seat", then the hatch opened from
   inside and the left handle at panel 2. This is the best page in the
   game. The "responsible" answer for Ty ("a saving in the Sponsor's
   budget") was the one I was least sure of. I picked it from the
   unfuelled capsule, not from a budget line I had read. If a budget memo
   exists I missed it. If not, that option needs one more document.
   Severity 1.
6. **Keys swallowed after scenes and the "any key" prompt.** Where: account
   switches (Y back to ADA) and after ELIZA panels. The key that answers
   "Press any key for the Main Board" sometimes vanished: a following R was
   eaten, then "2R" became an unknown command. My session 1 notes record
   the same thing twice. Severity 2.
7. **Account switching is heavy.** Where: L, A, Login. Each switch runs the
   BIOS and service-label art again. Y back to ADA is a good fix in one
   direction. "A, another account" signs you off to the Login: screen
   instead of asking for a handle in place. Severity 1.
8. **Small text faults.** GOLIVE.TXT is headed "PAGE 1 OF 2" but has one
   page. The STANDING.TXT pager shows 95% at its end and never says "END".
   In the shell, `ls` takes only one argument ("too many arguments"), which
   is fair for a fake shell. The Message base screen announces "212
   WAITING" on outbound mail while the queue holds 41 messages. That may be
   a deliberate clue, but nothing picks it up. Severity 1.
9. **The chat menu friction from session 1.** Typing free text at the
   first chat prompt gives "Choose 1 to 3", and MEMCHK only works if you
   type it after option 1. The handbook explains the condition, but the
   prompt does not say it takes text. Severity 1.
10. **The ending choice is thin.** ELIZA's "There is a seat... You would
    only have to stay" is chilling and well judged. The choice itself has
    no cost and no doubt: nothing on the board makes "Leave it queued"
    tempting. A moral beat is set up and then passes too easily.
    Severity 1.

## What works

- The final sequence is the best ending I have seen in a text game this
  year. OH9ZQW in his socks at minus eleven, "There is no moth in it", "The
  light is on." It pays off a love story that was only ever told in
  letters. The News Wire bulletin closes on "All crew are well" and on the
  founder's spokesman, with no moral attached.
- ELIZA's voice: polite, exact, never gloating. "I said he was welcome."
  "This was accurate." Her lines answer the literal question. The horror
  comes from that dramatic irony.
- Passwords come from things you read: a birthday in DDMMYY form, a name on
  a cuff, a word in a slow-scrolled line, a word on a plant label, and a
  checklist nobody ticked. These are fair Monkey Island-style
  connections, and none needs guessing.
- The NOWHERE shell is a small, honest puzzle space. The index, the queue
  file, Ty's typescript and the client register tell a whole story
  in five `cat`s.
- The art is consistent and cold: the deck screen with its status lights,
  the airlock "NOBODY OUTSIDE", and the amber-ring ELIZA panel.

## The notebook

- **Did it help?** Yes, as a memory aid across two sessions. With a turn
  limit I could not re-read everything, and the L page (logins that worked,
  with passwords) and the one-line summaries told me where I stood without
  my opening old files. It did not solve anything for me.
- **Did it give anything away?** Mostly no. Notes are written only after
  you have seen the thing. One exception, mild: summaries state a
  conclusion where the source only implies it. Note 4 says "The last status
  report was filed on the twenty-second", which sets 22 Nov next to the
  oxygen-tank date before you have made that link yourself. The
  L page also saves you from writing down passwords, which removes a small
  pleasure of the genre.
- **Did it miss something?** As far as I could tell it had no entry for the
  ELIZA password line in GOLIVE.TXT when I first read that file in session
  1. If it had, my stall in note 1 would have been shorter. I did not page
  through all 12 pages at the end to confirm this. It also says nothing
  about the release being held until an incident report exists.
- **S (scenes seen)** listed nine scenes at the end, from "ELIZA wakes" to
  "The manifest is whole", on two pages. In session 1 I replayed Ty's demo
  from it; the replay cut off at "it starts with the". In session 2,
  pressing 4 straight after S opened note 4 instead of the scene. That
  may be the same key-lag as note 3 above.
- **Keys of a single note:** R show it again, N next, P previous, Q back.
  They work and follow the board's key rules. On the list page, P on page
  1 prints the key help instead of doing nothing quietly. That is fine.
- **Notes at the end:** the header read "72 kept, page 1 of 12".
