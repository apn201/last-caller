# Requirements - Playtest fixes

Queue item Q12b in `docs/briefs/QUEUE.md`. Source: the open items 1 to 5 in
`docs/PLAYTEST.md` after the Q12 playtests. Caller-visible wording is a draft
for the author's final edit and lives in card files.

## Requirements

### R1 - A wrong login name gets a hint

1. WHEN a caller types a name the board does not know at the brownout prompt,
   THE board SHALL print "Unknown caller." and one more line from
   `STORY/POWER.INI` `[TEXT] unknown_hint`, then the prompt again.
2. WHERE the card has no `unknown_hint`, THE board SHALL print only the first line.

### R2 - ELIZA's last blank has a visible source

1. WHEN a pick list has no options, THE manifest SHALL print "Nothing to choose
   from yet." and, where `MANIFEST.INI` has one, a notebook line for that blank
   (`hint1..3`, `last_hint1..3`) that says where to read and never the answer.
2. THE lint SHALL report an error for every flag a right answer needs that no
   source open to an own account can grant (a crew visit, an account-gated
   conference or file area does not count), so that a caller who loses a crew
   visit can always earn the fact again without it.
3. ELIZA's node chat SHALL answer the words "still running", "rack", "module",
   "payload" with a line that points at what the crew wrote, and nothing more.

### R3 - release and the order code can be found

1. WHEN a caller types HELP or ? in Diagnostics, THE door SHALL print
   `[DIAG] help` from the card: where the codes are, that codes from the Office
   of the Founder are entered as queries, that a held queue is released by an
   operator on the payload host.
2. THE NOWHERE shell `help` SHALL print `help=` from the card and, only for a
   caller the final command works for, `help_final=` (the release line). Before
   that the final command stays "command not found".
3. THE comms manual SHALL point from the Diagnostics section to section 6.

### R4 - Fate and cause options read right on every row

1. WHERE an option text holds `{his}`, THE manifest SHALL show the row's `his=`
   word (her, his, their) in its place, in the pick list and in the row view.

### R5 - Reading

1. THE message reader SHALL wrap text at 79 columns on a space, never inside a
   word; a word longer than a row is cut at the edge; colour codes take no width.
2. WHEN an animated picture has played to its end, THE reader SHALL print only
   its status bar, not the page again.
3. THE message header list SHALL show the newest 20 and offer E for the 20
   before the ones shown and L for the 20 after, each only where it works.
4. Long lines in Diagnostics and in the manifest hint SHALL wrap on a space.

### R6 - Run 4

1. THE playtester agent SHALL play from a brand-new caller with no hints from
   earlier runs and finish the game with no fault; run 4 and its play time
   against the 60 to 120 minute goal are recorded in `docs/PLAYTEST.md`.