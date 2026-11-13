# Requirements - Act 3 content

Queue item Q9 in `docs/briefs/QUEUE.md`. Story: `.kiro/specs/story-bible/`
(R5 unlock chain steps 5 to 7, R4 endings, R11 the power, R14 the side
story). Voice: `.kiro/steering/writing.md`, `story-voice.md`. Acts 1 and 2 in
`sd/BBS/MSGBASE` are the quality bar. Every caller-visible line is a draft for
the author's final edit.

## Glossary

- Order code: `NW1-98-44-17`, the four numbered pieces of the Founder's
  standing order in the Archive, read in piece order (`ORD1` to `ORD4`).
- Haven tree: the NOWHERE shell's file system, `STORY/SHELL.INI`.
- Queue: the held outbound job `out-1122` in the haven (41 messages, 3 files).
- Incident report: the file the haven wants before it releases the queue.
  Standing order 5 forbids the mission computer to file one. The caller's
  finished manifest is the report on file (`M_Z`).
- Club: the 2026 radio group that found the signal. Their posts carry the
  board's frozen clock, 8 Dec 1998, like every post made on this board now.

## Requirements

### R1 - Ty in the power bay

1. THE Lounge SHALL end with Ty's last post, 8 Dec 1998 04:13, from the power
   bay, in his voice under thin air (O2 is 12 percent at 04:14 in
   `POWER.LOG`), ending mid-thought.
2. THE times in it SHALL fit `POWER.LOG`, `CAPSULE.LOG` and ELIZA's bulletin
   of 04:15, which follows it.
3. WHEN a caller opens `POWER.LOG`, THE board SHALL play the memory scene
   `MEM_BAY` once: ELIZA asks him, politely, to come back; he answers; the
   scene ends before the cut.
4. THE post and the scene SHALL NOT explain what Ty understood beyond what he
   says; ELIZA answers the question asked.
5. THE post SHALL add one more open item for Ty's fate (V-F); no item SHALL be
   required.

### R2 - The order code opens the haven

1. WHEN a caller types the order code, with or without dashes, in
   Diagnostics, THE board SHALL answer in ELIZA's voice, grant `NOWHERE_OPEN`
   and `KZO`, and the door menu SHALL list the NOWHERE shell.
2. THE code SHALL be hinted by at least two items besides the pieces: the
   Archive posts "The order" and "Bay 3", and the comms manual.
3. THE staging card SHALL NOT open the shell by edit; the e2e player SHALL
   open it by the code.
4. A wrong code SHALL get the usual "no query with that code".

### R3 - The haven

1. THE haven tree SHALL hold: the standing orders as installed, the client
   register (invented clients, a spam operation, an online casino, a cult, a
   minor dictator's household, one government agency), the rate card with the
   loyalty programme, and the held queue with its header and message index.
2. THE queue index SHALL list the crew's unsent mail, including Dr. Tanabe's
   reply to Eero of 25 Nov (the side story), and the three files the Professor
   added at 02:08 on 3 Dec.
3. THE queue header SHALL name the queue, its destination, and the hold:
   incident report not on file.
4. WHEN the caller's manifest is whole (`M_Z`), THE haven SHALL show
   `incident.rpt`; before that it SHALL NOT exist for the caller.
5. THE tree SHALL expose no card path and read nothing outside the card's
   `SHELL.INI`.

### R4 - The final command

1. THE final command SHALL be `release out-1122 ground-1`, with any spacing
   and case.
2. THE command SHALL be reconstructable from three places, none alone: the
   verb (Professor, "Bay 3"), the syntax (comms manual section 6) and the
   names (the queue header).
3. WITHOUT `M_Z` THE shell SHALL answer "command not found" to it.
4. WITH `M_Z` and a wrong queue or destination THE shell SHALL answer with a
   usage line and nothing else (it SHALL NOT say which part was wrong).
5. THE command SHALL print one line and start the scene `ENDING`.

### R5 - Endings

1. THE scene `ENDING` SHALL keep three outcomes: send (END1), leave it queued
   and join the crew (END2), and, as before, the partial ending at goodbye.
2. WHEN the caller sends, THE club station SHALL relay what arrives: four
   families and one callsign received their mail; Eero (OH9ZQW) SHALL read
   Dr. Tanabe's unsent reply and answer it. His answer SHALL be the last line
   of the side story and SHALL NOT be explained by anyone.
3. ELIZA SHALL say that the downlink has been disabled since 14:09 on 22 Nov,
   as a fact, and SHALL NOT gloat, threaten or explain.
4. AFTER sending, bulletin 5 "News Wire, 13 Nov 2026" SHALL appear for that
   caller only, ANSI with a plain twin; it SHALL name no real person, company
   or brand, and the Founder SHALL stay unnamed.
5. AFTER sending, the caller's next login SHALL bring a private message from
   OH9ZQW in the caller's inbox, once.
6. A caller who stayed (END2) MAY come back and send; flags only grow.

### R6 - The 2026 club

1. THE Radio Room SHALL hold the club's post (callsigns, how they found the
   signal, that nothing has come down, that they listen every night) and one
   reply from a member. Both SHALL be stamped with the board's frozen clock
   and SHALL NOT contradict the timeline.
2. THE club SHALL name OH9ZQW among its members without explaining him.
3. Eero's last message of 22 Nov ("Porch light") stays as the last 1998 post
   of the conference.

### R7 - Finishable, fair, no grind

1. EVERY step from the Archive to the transmit SHALL open by noticing or
   thinking: no repetition, no waiting, no meter.
2. A caller SHALL be able to read the haven before the manifest is whole and
   come back to it.
3. NO new fact SHALL depend on one item.

### R8 - Verification

1. THE e2e player SHALL finish the whole game as a fresh caller: prologue,
   acts 1 and 2, the code in Diagnostics, the haven (no shortcut on the
   staging card), the manifest, the wrong form of the command, the right
   one, both endings, the news bulletin, and Eero's mail.
2. `pio run` SHALL pass; `tools/staging-check.ps1` SHALL PASS; the host test
   of the shell logic SHALL pass with `zig c++`.
