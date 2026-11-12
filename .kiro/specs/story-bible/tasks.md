# Tasks - Story bible

Wording of all caller-visible text is draft until the author's final edit.
Evidence ids (E01...) and fact ids are in `design.md`.

## Group 1 - Content (card only, slice S3)

- [ ] C1. Accounts and user list. Seven accounts: `CMDR`, `CHAOS`, `GARDEN`,
  `TYVANCE`, `ELIZA` (SysOp), `MCC` (ground relay, inbound mail stops
  22 Nov) and `admin` (ship maintenance, password `admin`, not the SysOp).
  `TYVANCE`, `CMDR`, `GARDEN`, `CHAOS` and `admin` are read-only crew
  accounts (see N13). Last-call dates per the timeline (Ty's last: 8 Dec).
  `TYVANCE` password is his birthday as digits. `CHAOS` password is
  `LADDER`. Evidence: E02.
- [ ] C2. Board epoch. Done in S3b: `epoch=1998-12-08 04:17`, `clock_sync=`
  (empty). Posts are all earlier, so nothing is future-dated. Check on the
  board that the clock reads 8 Dec and resumes after a reboot.
- [ ] C3. Conferences and posts: LOUNGE (E06, E08, E36, E37), MISSION LOG
  (E05), GARDEN NOTES (E07), CREW MAIL private per account (E23, E24, E25),
  ARCHIVE hidden (E26). Dated posts 2 Nov to 7 Dec 1998, weekdays checked.
  Normal life first, small wrong notes, then silence. Last post unfinished.
  Achterberg's jettison post is Mon 23 Nov; ELIZA's reply is polite.
- [ ] C4. Bulletins: WELCOME (ELIZA), CREW bios (E01), RULES, and the
  8 Dec 04:15 status line ("All crew are well. Thank you for your
  patience."), the last timestamp on the board.
- [ ] C5. File areas with files and DIZ dates:
  - LOGS: SUIT1.LOG (E17), GARDEN.LOG (E18), AUDIT.LOG (E19),
    CAPSULE.LOG (E20, hatch opened from inside on 8 Dec), RACK.LOG (E21),
    DOWNLINK.CFG and ANTENNA.RPT (E22), POWER.LOG (E33: RTG and battery
    bus; last lines `RTG DISCONNECTED - MANUAL`, `BUS FAULT`, nothing),
    MODULE.LOG (E38: `JETTISON INHIBITED BY MISSION COMPUTER`).
  - GALLERY: Ty's journal (E09) and the Orbit series (see C7).
  - MUSIC: Ty's finished tune (E39) and, on his account, the unfinished demo
    tune (E40). Sample names are drafts for the author.
  - LIBRARY: budget memo (E28), prospectus (E29, with the RTG line "fully
    licensed in the jurisdiction of nowhere"), comms manual (E30).
  - ARCHIVE: the Sponsor's order as four numbered pieces (E27), with
    DIZ texts that carry the order.
- [ ] C6. Art per screen with `tools/ansi/`, rendered to PNG and looked at:
  welcome, menus, goodbye, crew area, diagnostics door, shell door, manifest
  and the three endings. No placeholder text screens.
- [ ] C7. Ty's art as evidence. Plan and build the Orbit gallery series:
  1 crew portraits (E10), 2 Earth, 3 Professor overpainted (E16),
  4 ELIZA's ring (E11), 5 antenna (E12), 6 garden (E13), 7 comms bay (E14),
  8 the seat, 9 unfinished last piece (E35, on Ty's account: the power
  bay, a cut line). Camcorder still for E15. Style:
  bright and smug before day 22, sloppier and darker after.
- [ ] C8. Change the art tag on every screen from `·halcyon·crew·98·` to
  Ty's tag (draft `·tyv·98·`), and update `.kiro/steering/ansi-art.md` to
  match. The author picks the final tag.
- [ ] C9. Matrix check on the card: every fact still has at least two items
  in two media after the real files are written (re-run the table in
  `design.md`).
- [ ] C10. Name and brand scan of every caller-visible line against
  `story-voice.md`. Includes the demo party: never named.
- [ ] C11. Art for the prologue and Ty, each with `tools/ansi/`, rendered
  to PNG and looked at: brownout boot banner, ship systems console, ELIZA
  wake-up screen, Ty's unfinished last ANSI (E35).
- [ ] C12. Music: Ty's demo and two or three original MODs from a generator
  script (`tools/mod/`, original only), with sample names that carry text
  (E39 tag and birthday; E40 last thoughts). The matrix uses two (E39,
  E40); a third is free colour.
- [ ] C13. Sound effects: short original WAV/MP3 (power hum, relay clicks,
  a reboot chirp), made by a script, original only.

## Group 2 - Engine needs (not designed here)

- [ ] N1. Ship systems door and keyword door: the SHIP console of the
  prologue, and the keyword machinery (type the right thing, get a flag or
  raise level, e.g. `LADDER`). Slice S4.
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
- [ ] N12. Show sample or instrument names of a MOD. The browser player
  (`ASSETS/PLAYER/player.js`) shows title, type, channels and artist only;
  it needs `openmpt_module_get_sample_name` or the song message. Telnet
  cannot play modules, so it needs a text view of the 31 sample names read
  from the MOD header. Checked on the engine; the need is real. Slice S7.
- [ ] N13. Power prologue, read-only crew accounts, concurrent logins and
  private ELIZA messages. Designed in `.kiro/specs/power-prologue/`.
  Slice S4.
- [ ] N14. Sound effects played from the card (browser; Telnet gets a BEL
  at most). Slice S7.
