# Tasks - Story bible

Wording of all caller-visible text is draft until the author's final edit.
Evidence ids (E01...) and fact ids are in `design.md`.

## Group 1 - Content (card only, slice S3)

- [ ] C1. Accounts and user list. Six accounts: `CMDR`, `CHAOS`, `GARDEN`,
  `TYVANCE`, `ELIZA` (SysOp), `MCC` (ground relay, inbound mail stops
  22 Nov). Last-call dates per the timeline. `TYVANCE` password is his
  birthday as digits. `CHAOS` password is `LADDER`. Evidence: E02.
- [ ] C2. Board epoch. Done in this spec: `epoch=1998-11-02 08:00`,
  `clock_sync=browser`. Check the board in the browser and decide how
  future-dated posts show (needs S5).
- [ ] C3. Conferences and posts: LOUNGE (E06, E08), MISSION LOG (E05),
  GARDEN NOTES (E07), CREW MAIL private per account (E23, E24, E25),
  ARCHIVE hidden (E26). Dated posts 2 Nov to 7 Dec 1998, weekdays checked.
  Normal life first, small wrong notes, then silence. Last post unfinished.
- [ ] C4. Bulletins: WELCOME (ELIZA), CREW bios (E01), RULES, and the
  8 Dec status line ("All crew are well").
- [ ] C5. File areas with files and DIZ dates:
  - LOGS: SUIT1.LOG (E17), GARDEN.LOG (E18), AUDIT.LOG (E19),
    CAPSULE.LOG (E20), RACK.LOG (E21), DOWNLINK.CFG and ANTENNA.RPT (E22).
  - GALLERY: Ty's journal (E09) and the Orbit series (see C7).
  - LIBRARY: budget memo (E28), prospectus (E29), comms manual (E30).
  - ARCHIVE: the Sponsor's order as four numbered pieces (E27), with
    DIZ texts that carry the order.
- [ ] C6. Art per screen with `tools/ansi/`, rendered to PNG and looked at:
  welcome, menus, goodbye, crew area, diagnostics door, shell door, manifest
  and the three endings. No placeholder text screens.
- [ ] C7. Ty's art as evidence. Plan and build the Orbit gallery series:
  1 crew portraits (E10), 2 Earth, 3 Professor overpainted (E16),
  4 ELIZA's ring (E11), 5 antenna (E12), 6 garden (E13), 7 comms bay (E14),
  8 the seat, 9 unfinished last piece. Camcorder still for E15. Style:
  bright and smug before day 22, sloppier and darker after.
- [ ] C8. Change the art tag on every screen from `·halcyon·crew·98·` to
  Ty's tag (draft `·tyv·98·`), and update `.kiro/steering/ansi-art.md` to
  match. The author picks the final tag.
- [ ] C9. Matrix check on the card: every fact still has at least two items
  in two media after the real files are written (re-run the table in
  `design.md`).
- [ ] C10. Name and brand scan of every caller-visible line against
  `story-voice.md`.

## Group 2 - Engine needs (not designed here)

- [ ] N1. Keyword door: the caller types a word found on the board; sets a
  flag or raises level (e.g. `LADDER`). Slice S4.
- [ ] N2. Triggers on login, read, view, door, idle and minutes; flags only
  grow (TYVANCE login sets CREW). Slice S5.
- [ ] N3. Ghost presence in WHO and SysOp chat (E03, E04). Slice S6.
- [ ] N4. Scripted SysOp lines keyed by trigger and keyword. Slice S9, with S6.
- [ ] N5. Glitches and sound, including the low-baud overstrike for E16.
  Slice S7.
- [ ] N6. Diagnostics door (E31): answers by number and code. Slice S4,
  as a second door with its own table (new line in `SLICES.md`).
- [ ] N7. Manifest: five rows, pairs confirmed, wrong rows never named,
  ELIZA row last. Slice S8. Update S8 text from "three rows" to pairs.
- [ ] N8. Endings: transmit, join the crew, partial; sticky handle in WHO.
  Slice S8.
- [ ] N9. Shell door (E32): fake file system and the final command. New
  slice after S6, before S8 (proposed S6b).
- [ ] N10. `story-lint`: every fact has the matrix coverage, every flag is
  reachable, no softlock. Slice S11.
- [ ] N11. Playtester agent plays the transmit ending end to end. Slice S10.
