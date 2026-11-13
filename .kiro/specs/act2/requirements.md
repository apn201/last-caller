# Requirements - Act 2 content

Queue item Q8 in `docs/briefs/QUEUE.md`. Story: `.kiro/specs/story-bible/`
(fates, evidence matrix E16 to E38, R5 unlock chain, R11 the module).
Voice: `.kiro/steering/writing.md`, `story-voice.md`. Every caller-visible
line is a draft for the author's final edit.

## Glossary

- Overstrike: a row drawn, then a bare CR, then a second row drawn over it.
- Memory scene: a scene (`STORY/SCENES/MEM_*.INI`) that replays a moment the
  mission computer recorded, started by reading the item that proves it.

## Requirements

### R1 - Day 22 onward

1. THE Lounge, Mission Log, Garden Notes, Radio Room and Ty's mailbox SHALL
   continue from 22 Nov 1998 to 7 Dec 1998, each writer in their own voice,
   dated on the timeline with the real weekday and before the epoch.
2. THE board SHALL hold the module thread: Achterberg's proposal of 23 Nov
   (E36), ELIZA's polite refusal (E37) and `MODULE.LOG` (E38).
3. THE last post of each crew member SHALL fit their death: Halvorsen's log
   before the EVA, Tanabe's diary mid-sentence on 1 Dec, Achterberg's archive
   mid-sentence on 3 Dec 02:09, Ty's post from the seat on 7 Dec.
4. THE board SHALL never explain a scare: no narrator, no summary.

### R2 - Logs and records

1. WHERE a caller holds `CREW`, THE file areas Ship Logs (E17 to E22, E33,
   E38) and Records (E28, E29) SHALL be listed; ELSE they SHALL be hidden.
2. THE logs SHALL be readable as 1998 telemetry: times, values, events.

### R3 - The other crew accounts

1. THE `CMDR`, `GARDEN` and `CHAOS` passwords SHALL each be hinted by at least
   two items, never shown in clear in an open item:
   - `CMDR` = `ANDERS`: the Lounge post Locks (a name on her cuff), Orbit 5
     (the patch), her posts about her son.
   - `GARDEN` = `HANAMI`: Locks (a word on a label on a garden tray), Garden
     Notes 24 Nov (the label), Orbit 6 (the tray).
   - `CHAOS` = `LADDER`: Locks (inside a picture, for a person who slows
     down), Orbit 3 played slowly, Ty's journal 2 (the 300 baud trick), the
     word in his posts.
2. WHEN a caller signs in to one of them, THE board SHALL open that account's
   mailbox (or, for `CHAOS`, the Archive) for the caller's own account.
3. THE `TYVANCE` account SHALL see Ty's Desktop (Orbit 9, E35); a visitor's
   own account SHALL NOT.

### R4 - Overpainted ANSI (E16)

1. WHEN an ANSI picture draws over a row with a bare CR, THE reader SHALL
   show the final picture on its still pages and SHALL send the covered row,
   a bare CR and the final row while the picture plays.
2. AT a slow modem speed the covered row SHALL be on screen long enough to
   read; at full speed it MAY flash past.

### R5 - Jigsaw (E27)

1. THE Archive file area SHALL hold the order in four pieces whose file names
   are out of order; each piece SHALL carry its number and part of a code.
2. THE pieces in order SHALL give the authorisation code for act 3.

### R6 - Evidence

1. EVERY manifest fact SHALL be granted by at least two act 2 or act 1 items
   in at least two media; no single item SHALL be required.
2. THE manifest SHALL be finishable from the card alone: the staging card
   SHALL NOT need a shortcut rule.

### R7 - Memory scenes

1. Reading `SUIT1.LOG`, `GARDEN.LOG`, `CAPSULE.LOG` and the Archive's last
   message SHALL each play a memory scene once per caller.

### R8 - Verification

1. THE e2e test SHALL play acts 1 and 2 as a caller and then finish the
   manifest and both endings with the flags the content granted.
2. `pio run` and the staging check SHALL pass.
