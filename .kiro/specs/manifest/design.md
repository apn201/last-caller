# Design - Manifest and endings

Requirements: `requirements.md`. The story: `.kiro/specs/story-bible/design.md`
(Fates table, Endings). Wording on the card is draft.

## Pieces

| File | Role |
| --- | --- |
| `src/core/manifest_logic.h` | Pure logic: parser for `MANIFEST.INI` (from key/value triples), row and option visibility, picks as text, filing (pairs, the last row), notebook, the text of the answer to a filing. No String, no SD card. Host test: `tools/test/manifest_logic_test.cpp` |
| `src/doors/manifest.h` | Board side: loads the card, the main menu command `R`, the screens, saving picks (`DATA/STATE/MAN.DAT`) and flags, the sticky list (`DATA/STATE/DONE.DAT`), the goodbye text, the ending hook |
| `sd/BBS/STORY/MANIFEST.INI` | Rows, options, notes, wording |
| `sd/BBS/STORY/SCENES/MANFULL.INI`, `ENDING.INI` | ELIZA when the manifest is whole; the final scene (argue, send, stay) |
| `tools/ansi/screens/manifest.py` | `ANSI/DOORS/MANIFEST.ANS` and the ending art `ANSI/STORY/MANFULL, END_SENT, END_CREW, END_PARTIAL` |

Hooks in existing code, each one line or a few:

- `Session`: `manMode`, `manRow`, `manSlot`, `manFills`; `State` MANIFEST_PROMPT.
- Door table: a door with no menu key (`""`) and `listed` false, like the
  scene door; the main menu command `R` enters it.
- `mainMenu`: `R Crew manifest` when `manifestListed(s)`.
- `cmd_main.h` G: after the goodbye art, `manifestPartial(s)`.
- `scene.h` `sceneEnd`: `endingRecord(s)` before the main menu (the sticky list).
- `ghost.h` `ghostWhoRows`: `manifestStickyRows(viewer, rows)`.
- `shell_logic.h` / `shell.h`: `final_scene=` in `[SHELL]`; the door starts it
  after the final text, and does not print the prompt over it.
- `settings.h`: `loadManifest()` at boot and on RELOAD.

## The card file

```
[MANIFEST]  title, slot1..3 (labels of the three blanks), last_slot1..3 (the
            last row's), full_scene, count, filed, intro
[ROW_ID]    handle, kind=crew|last, needs=, needs_any=, done=FLAG,
            a1= a2= a3= (the canonical option ids)
[OPT_ID]    slot=1|2|3, group=crew|last, text=, has=
[NOTE_ID]   has=FLAG, about=HANDLE, text=
[PARTIAL]   text=   (goodbye without finishing; "|" breaks a line)
[STICKY]    sent_activity, sent_idle, crew_activity, crew_idle
```

Rules checked when it loads (a broken row, option or note is skipped and named
in the event log, MANIFEST_SKIP; the rest still load): ids are letters, digits
and `_`; at most 8 rows, 80 options, 40 notes; at most one `last` row and it is
the last in the file; a row's `a1..a3` name existing options of the right slot
and group; `done=` is one flag.

## Flags

Evidence flags (granted by content triggers, Q7 to Q9): `KHI KHF KHC KAI KAF
KAC KTI KTF KTC KVI KVF KVC KZI KZO KZW` (K, the person, I identity, F fate,
C cause; Z: I what she is, O orders, W why). Confirmed rows: `M_H M_A M_T M_V
M_Z`. Endings: `END1` (sent), `END2` (stayed). Scene markers: `SC_MANFULL`.
The whole set is about 130 characters of the account's 255, next to the
trigger markers, so content must not add a flag per clue: one flag per fact,
and notes may reuse them.

An evidence flag is granted at the caller's first piece of evidence about the
fact. It shows the answer and its look-alikes in the pick list; it does not
prove the answer. A fact is provable from two or more pieces (story bible R2),
and the decoy options are written to fit the first piece alone.

## Filing (`manifest::file`)

Input: the table, the caller's flags, the picks. Output: the flags to add, the
handles just confirmed, the count, whether the last row was confirmed.

1. Crew rows that are shown, not confirmed and fully right, in file order.
2. While two or more: confirm the first two (add their `done` flags).
3. The last row, if it is now shown, not confirmed and fully right: confirm it.
4. The text: the names just confirmed, then `count` with `{n}` and `{total}`.
   `{total}` is the number of crew rows, plus the last row once it is shown.
   Nothing else is said. A wrong row changes no word of the answer.

Three right and one wrong: the first two in file order are confirmed, the
third waits. All four right at once: two pairs.

## Screens (80 columns, state MANIFEST_PROMPT)

- Overview: art, rows `[1] CMDR  CONFIRMED | 3 of 3 picked | 1 of 3 picked`,
  `S` file the manifest, `N` notebook, `Q` back. The ELIZA row only when
  shown. Picks of a locked row are shown on its row screen, not edited.
- Row: the three blanks with the current pick, `1 2 3` open a blank, `Q` back.
  A confirmed row says LOCKED and offers no blank.
- Pick list: the options for this row and blank that the caller may see,
  numbered, `0` clears, `Q` back.
- Notebook: notes, `Q` back.

Input is lines, like every other door: a number or a letter and Enter.

## Endings

- `MANFULL` (scene): ELIZA's first plain statements. Its art is `MANFULL`.
- `ENDING` (scene, `repeat=1`, `has=M_Z`): argue; "why" (one reply); decide;
  send sets `END1` and shows the return traffic; leave sets `END2`.
- The final command is in `SHELL.INI` (`final_has=M_Z`, `final_scene=ENDING`).
  Q6 puts a draft command in; Q9 writes the real one and the puzzles around it.
- Partial: `manifestPartial`, on goodbye, shows `END_PARTIAL` and the text.

## Sticky

`endingRecord(s)` is called when a scene ends: for an account with `END1` or
`END2` it writes `id|handle|kind` to `DATA/STATE/DONE.DAT` with `putRecord`
(replaces the account's line; the newest ending wins). WHO shows the last six
lines as rows on nodes from 60 up, skipping a handle that is online.

## What is not here

The evidence triggers and the real content (Q7 to Q9), the final command
puzzle (Q9), the real 2026 bulletin and the love story's last line (Q9),
hints that cost something (not wanted: no grinding), `story-lint` (Q12).
