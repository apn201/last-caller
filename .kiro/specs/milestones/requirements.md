# Requirements - Milestones (queue items Q12c design, Q12d build)

Brief: `docs/archive/briefs/S12c-milestones.md`. Story: `.kiro/specs/story-bible/`
(design: Ty's last fight, unlock chain M1 to M9, act gates audit). Chain and
hints: `docs/PUZZLE_CHAIN.md`. Voice: `.kiro/steering/writing.md`,
`story-voice.md`. All caller-visible wording is a draft for the author's final
edit.

## Glossary

- Milestone (M1 to M9): a gate in the chain that opens the next stretch of the
  1998 evidence and plays a scene.
- Act 1, 2, 3: the evidence from 31 Oct to 22 Nov 13:51, from 22 Nov 13:52 to
  1 Dec, and from 2 Dec to 8 Dec. Act 2 opens with M3, act 3 with M4.
- Overrun: a message body with at least 20 `0` characters in a row, line breaks
  ignored.
- SYSOP view: the story flag `SYSVW`. It opens reading and nothing else.
- Cure: the flag `CURE`, set by reading the memory-check page of the operations
  handbook.
- Gate flag: `ACT2`, `ACT3`, `SYSVW` or a flag named in a `Needs:` line.

## Requirements

### R1 - Ty's last fight on the card

1. THE card SHALL show Ty's 8 Dec attempt in the order of the hour table in the
   story bible: the shell attempt, the refusal, the bulkhead, the falling
   pressure, the manual override, suit comms off, the vent, the cut.
2. `POWER.LOG` and a new `BULKHEAD.LOG` SHALL carry those hours with times
   that fit `CAPSULE.LOG`, Ty's 04:13 post and ELIZA's 04:15 bulletin.
3. THE shell SHALL hold `/home/tyvance/typescript` with a near miss: a wrong
   destination, then `SYSOP REQUIRED`. IT SHALL NOT contain the working command
   or the working destination.
4. `MEM_BAY` SHALL play the fight: ELIZA's polite lines against Ty's short ones,
   ending at the cut. It SHALL NOT explain what Ty understood beyond what he
   says.
5. NO caller-visible line SHALL say he acted "with his last breath" after
   giving up.

### R2 - Act gates

1. A message in `MSGBASE/<id>/*.TXT` MAY carry a header line `Needs: FLAG`. THE
   board SHALL show the message only to a caller who has that flag.
2. A file in a file area MAY be listed in `NEEDS.BBS` (`NAME FLAG`). THE board
   SHALL list and open it only for a caller who has that flag.
3. THE gated posts SHALL be the ones in the story bible's "Act gates audit":
   those after 22 Nov 13:52 behind `ACT2`, those from 2 Dec behind `ACT3`.
4. WHEN a caller has `SYSVW`, every `Needs:` and every `required_flag` of a
   story conference or file area SHALL pass, including crew mail.
5. A message count, a new-message scan, a thread view and a search SHALL NOT
   reveal a hidden message or its subject.
6. THE card SHALL keep the matrix: each fact keeps an open item and four
   items in four media (the audit table).

### R3 - Flags for the chain

1. THE visit to `TYVANCE` SHALL set `ACT2`; the visit to `CHAOS` SHALL set
   `ACT3`. Both flags only grow and carry to the caller's own account like the
   other crew visit flags.
2. THE story SHALL NOT set `SYSOP`, `READONLY`, `BACKROOM`, `VAULT`, a level or
   a board setting. A test SHALL fail if any rule, scene or line on the card
   sets one of them.
3. THE new flags SHALL be short names in the story record (not the access
   flags), and `tools/story_lint.py` SHALL know them.

### R4 - M5: the overrun

1. WHEN a caller posts a message in a public conference whose body holds an
   overrun, THE board SHALL NOT store it, SHALL show a "message lost" line and
   a short screen glitch, and SHALL set `OVR` on the caller. Nobody else SHALL
   see anything.
2. THE count SHALL ignore line breaks, so lines of zeros joined add up. 19 SHALL
   NOT trigger it: it SHALL show one garbled ELIZA line, store the post and
   set no flag. Fewer than 19 SHALL do nothing.
3. WHEN `OVR` is set, THE scene `MEM_GLITCH` SHALL play once: fragments out of
   order, one of them a moment the caller has not read yet. Nothing on the
   board SHALL really be corrupted.
4. WHEN `OVR` is set, ELIZA's scripted lines (`SYSOP.INI`) MAY differ (a line
   with `has=OVR`). No line SHALL lose a clue the caller already had.
5. THE hints SHALL be in the open Lounge post of 6 Nov (E41) and the Archive
   post of 2 Dec (E42), with ELIZA's reply under Ty's post. The exact rule
   SHALL be written in the post (the number 20).

### R5 - M6: the SysOp chat and the grant

1. THE SysOp chat SHALL open as a scene (`ELZ_TALK`) with numbered replies. One
   reply SHALL drop to the free typing chat so the existing keyword lines and
   easter eggs keep working.
2. WHEN `OVR` is set, ELIZA SHALL say, politely, that she cannot account for some
   minutes of her own memory.
3. WHEN the caller has `OVR` and `CURE`, a reply SHALL appear that offers to run
   the memory check if she gives SYSOP view. ELIZA SHALL weigh it against her
   orders (her records must be complete) and grant it "for the duration of the
   check". SHE SHALL answer the request made, not the one meant.
4. Any other reply to a request for SYSOP SHALL get a calm refusal and return to
   the hub. No reply SHALL end the chat or lock anything.
5. `SYSVW` SHALL be permanent once granted (flags only grow). ELIZA's line that
   she will close it after the check SHALL stay unkept, with no further comment
   from the board.
6. `CURE` SHALL be set by reading the memory-check page, in the operations
   handbook visible only on the CMDR account (E43), and by nothing else.
7. THE SYSOP view SHALL give none of: the SysOp console, the SysOp page, user
   editing, deleting, editing messages, or any setting. A menu option that
   needs them SHALL NOT show.

### R6 - M7 and M8: ELIZA's account and the shell

1. THE `ELIZA` account SHALL be a story account like the other crew accounts:
   read-only, level below SysOp, no `SYSOP` flag. Its password SHALL be set to
   the one in the go-live checklist (E44).
2. THE go-live checklist SHALL be in a file area ENGINEERING that needs `SYSVW`.
   Name, area and date SHALL make it findable once there.
3. A visit to `ELIZA` SHALL set `ELZ_IN`, open her mail (E47 and her
   correspondence) and her ops log, and play `ELZ_OPS` once.
4. THE NOWHERE shell door SHALL show only in ELIZA's door menu, and only when the
   caller's account also has `NOWHERE_OPEN`. The shell's own entry (the order
   code in Diagnostics) SHALL stay.
5. THE ELIZA session SHALL NOT show an option that does not work for that
   caller on that transport.

### R7 - Scenes

1. New scenes SHALL be card files in `STORY/SCENES/`: `ELZ_WELCOME`,
   `MEM_GLITCH`, `ELZ_TALK`, `ELZ_GRANT`, `ELZ_OPS`; `MEM_BAY` SHALL be
   rewritten.
2. Each new screen SHALL have ANSI art made with `tools/ansi/`, rendered to
   PNG and looked at.
3. `ELZ_TALK` SHALL be repeatable; every other scene SHALL play once.

### R8 - Verification (Q12d)

1. `tools/story_lint.py` SHALL know the chain: every gate flag reachable, no
   softlock, every ending reachable, no step needs a flag only a later step
   grants, and no hidden post is the only source of an answer.
2. A property test SHALL show that no rule, scene or line sets a flag outside
   the story set, and that story flags only grow (R3.2).
3. THE e2e SHALL play the new chain from a fresh caller, M1 to M9, and the
   branches that matter: the 19-zero near miss, an early SYSOP request, SYSOP
   view reading a gated post, ELIZA login, shell from her door.
4. `pio run` and the staging check SHALL pass. `playtester` run 5, from a
   brand-new caller, SHALL finish the game; play time SHALL be recorded in
   `docs/PLAYTEST.md`.

### R9 - Design done (Q12c)

1. THE spec (this folder) SHALL be committed before any content or engine change.
2. NO engine code SHALL change in Q12c.
3. `python tools/story_lint.py` SHALL pass on the current card at the end of Q12c.
