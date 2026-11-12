# Requirements - Manifest and endings

Slice S8 in `docs/SLICES.md`, queue item Q6 in `docs/briefs/QUEUE.md`. Story:
`.kiro/specs/story-bible/` (R1 manifest facts, R3 confirmation, R4 endings,
R13 solo game). All caller-visible wording in `MANIFEST.INI`, the ending
scenes and the ending art is a draft until the author's final edit.

## Glossary

- Manifest: the crew manifest screen. Crew rows: `CMDR`, `CHAOS`, `GARDEN`,
  `TYVANCE`. The last row: ELIZA.
- Fact: one blank in a row (three per row). Its canonical answer is one
  option on the card.
- Evidence flag: a flag on the caller's account that says the caller has
  found evidence about a fact. Content (Q7 to Q9) grants them with triggers.
- Confirmed row: a row that was filed right and is locked. Its flag
  (`M_H`, `M_A`, `M_T`, `M_V`, `M_Z`) is on the account for good.
- Notebook: the list of what the caller has found, one line per evidence flag
  that has a note on the card.
- Sticky handle: a finished caller's handle, listed in WHO for later callers.

## Requirements

### R1 - Rows and pick lists

1. THE manifest SHALL be defined on the card in `STORY/MANIFEST.INI`: rows,
   pick-list options, notebook notes and the board's wording. A card without
   the file SHALL have no manifest.
2. EACH row SHALL have three blanks, each filled by choosing one option from
   a pick list. A caller SHALL be able to change a blank until its row is
   confirmed, and clear it.
3. A row SHALL be shown only to a caller who holds the row's evidence flags,
   and an option only to a caller who holds the option's flags.
4. THE main menu SHALL offer the manifest only to a caller who can use it:
   an account that can save (not a read-only crew login, not the maintenance
   login) and at least one row is shown.
5. A caller's picks SHALL be kept per account and survive logging off. IF
   the picks cannot be saved, THEN the pick SHALL NOT change and the caller
   SHALL be told.

### R2 - Filing and confirmation

1. WHEN the caller files the manifest and at least two unconfirmed crew rows
   are fully right, THE board SHALL confirm and lock two of them, the first
   two in manifest order, and repeat while two more remain.
2. THE board SHALL save the confirmed rows' flags before it says anything
   about them. IF the save fails, THEN nothing SHALL be confirmed or shown.
3. THE board SHALL NOT name, mark or count a wrong or partly filled row, on
   filing or anywhere else. It SHALL say only the names of rows it has just
   confirmed and how many rows are confirmed in all.
4. ONE right crew row alone SHALL NOT be confirmed.
5. THE ELIZA row SHALL be shown only after all four crew rows are confirmed.
   It SHALL be confirmed on its own when it is fully right.
6. WHEN the ELIZA row is confirmed, THE board SHALL play the scene named in
   `full_scene=` (ELIZA says what she has not said before).
7. FILING SHALL NOT need waiting or repetition: a caller who knows the
   answers files once. Guessing SHALL NOT pay: a confirmation needs two
   exact rows at once, and the lists hold wrong options that look right.

### R3 - Notebook

1. THE notebook SHALL list, for the caller, each note on the card whose
   flag the caller holds, with the crew handle it is about.
2. THE notebook SHALL NOT list a note the caller has not earned and SHALL
   NOT say which rows are right.

### R4 - Endings

1. Transmit: WHEN the ELIZA row is confirmed and the caller types the final
   command in the NOWHERE shell (`final_cmd=` in `SHELL.INI`), THE board
   SHALL start the scene named in `final_scene=`. In it ELIZA argues, politely,
   with orders and an offer, and the caller chooses to send or to leave the
   queue. Sending sets `END1` and shows the return traffic (the 2026 note and
   the families' mail).
2. Join the crew: IF the caller leaves the queue in that scene, THEN `END2` SHALL
   be set and ELIZA SHALL add the caller's handle to the crew list.
3. Partial: WHEN a caller who has been offered the manifest says goodbye
   without having finished (neither `END1` nor `END2`), THE board SHALL show
   ELIZA thanking them for their interest, with no further reveal.
4. THE final scene SHALL be playable again, so a caller who left the queue
   can send it later. Flags only grow.
5. A finished caller (`END1` or `END2`) SHALL be listed in WHO for later
   callers as a ghost row, with the activity text from `MANIFEST.INI`. The
   record SHALL be kept by the board in `DATA/STATE/DONE.DAT` and SHALL NOT
   take a node from the pool.

### R5 - Platform and card

1. THE manifest, the notebook and the endings SHALL work the same over
   Telnet and the browser. A caller without ANSI SHALL see the ASCII twins
   of the art.
2. NO screen SHALL show a path on the card.
3. THE manifest SHALL NOT change an account that cannot save.
4. EVERY new screen SHALL ship with ANSI art made with `tools/ansi/`,
   rendered to PNG and looked at.
5. THE flags this item uses SHALL be few and short: the account's flag
   field holds 255 characters in all.
