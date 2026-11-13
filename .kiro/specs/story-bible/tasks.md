# Tasks - Story bible

Wording of all caller-visible text is draft until the author's final edit.
Evidence ids (E01...) and fact ids are in `design.md`.

## Group 1 - Content (card only, slice S3)

- [x] C1. (Q7, accounts as built: `STORY/ACCOUNTS.INI`; `CMDR`/`GARDEN` passwords are placeholders for act 2.) Accounts and user list. Seven accounts: `CMDR`, `CHAOS`, `GARDEN`,
  `TYVANCE`, `ELIZA` (SysOp), `MCC` (ground relay, inbound mail stops
  22 Nov) and `admin` (ship maintenance, password `admin`, not the SysOp).
  `TYVANCE`, `CMDR`, `GARDEN`, `CHAOS` and `admin` are read-only crew
  accounts (see N13). Last-call dates per the timeline (Ty's last: 8 Dec).
  `TYVANCE` password is his birthday as digits. `CHAOS` password is
  `LADDER`. Evidence: E02.
- [ ] C2. Board epoch. Done in S3b: `epoch=1998-12-08 04:17`, `clock_sync=`
  (empty). Posts are all earlier, so nothing is future-dated. Check on the
  board that the clock reads 8 Dec and resumes after a reboot.
- [x] C3. (Q7, Q8; CREW MAIL is one read-only conference per account, the ARCHIVE is conference 10, gated by `ACH`.) Conferences and posts: LOUNGE (E06, E08, E36, E37), MISSION LOG
  (E05), GARDEN NOTES (E07), CREW MAIL private per account (E23, E24, E25),
  ARCHIVE hidden (E26). Dated posts 2 Nov to 7 Dec 1998, weekdays checked.
  Normal life first, small wrong notes, then silence. Last post unfinished.
  Achterberg's jettison post is Mon 23 Nov; ELIZA's reply is polite.
- [~] C4. (Q7: WELCOME, CREW, STATUS done; the rest is act 2.) Bulletins: WELCOME (ELIZA), CREW bios (E01), RULES, and the
  8 Dec 04:15 status line ("All crew are well. Thank you for your
  patience."), the last timestamp on the board.
- [~] C5. (Q8: LOGS as Ship Logs, LIBRARY memos as Records, ARCHIVE; MUSIC waits for Q10.) File areas with files and DIZ dates:
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
- [x] C7. (Q8: Orbit 3 to 8 in the gallery, Orbit 9 on Ty's Desktop; Orbit 8 is the camcorder still, E15.) Ty's art as evidence. Plan and build the Orbit gallery series:
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

## Group 3 - Social board, side story, scenes, easter eggs (folded in at Q0)

Source: `docs/STORY_EXTRAS.md`. Queue items in `docs/briefs/QUEUE.md` do the
work. All wording is draft for the author.

- [x] S1. (Q7) Public conferences Lounge, Spoilers, No Spoilers, Radio Room with
  posting; story conferences, crew mail and logs read-only. Needs a
  `read_only` key in `CONFERENCES.INI` (R13). Q7.
- [ ] S2. Side story A: Keiko and the radio amateur, Nov 1998 posts through
  the packet gateway, his last message 22 Nov, the 2026 club on the Radio
  Room, his last line (R14). Q7 to Q9.
- [ ] S3. Side story B as Ty's texture: greets in .NFO files, hidden
  messages in his art, the end scroller of the demo (R14.4). Q7, Q11.
- [ ] S4. Scene engine (R15.1), then scenes: ELIZA conversations, five
  memory scenes, side story scenes, the ending sequence. Q3, Q7 to Q9.
- [ ] S5. Marketing voice in README, bulletins and the Devpost page, kept
  in `.kiro/steering/product.md` (done in Q0 for the steering; copy in Q13).
- [ ] X1. Modem commands: `ATH0`, `+++`, `ATDT` to a hidden node, `ATI`. Q11.
- [ ] X2. Hidden commands: `xyzzy`, an arrow-key cheat code, `/fortune` with
  ship fortunes, ROT13 posts in the Lounge. Q11.
- [ ] X3. ELIZA's dry original reply to the famous request to a film
  computer; the caller types it, ELIZA never quotes. Q4, Q11.
- [ ] X4. The 486 POST screen: Turbo button, BIOS date, a memory test that
  counts the wrong amount. Q11.
- [ ] X5. The Y2K memo from the Sponsor's IT. Q11.
- [ ] X6. The hotel brochure. Q11.
- [ ] X7. NOWHERE price list per megabyte per orbit, with a loyalty
  programme. Q9, Q11.
- [ ] X8. Ty's demo: ANSI animation with his MOD, an end scroller with greets
  to invented groups, crashes at the same frame every time. Q11.
- [ ] X9. Ty's snake door with a high score table of the dead crew. Q11.
- [ ] X10. Nodelist fragment, .sig files, an original excuse generator, a hex
  dump with a message, a GIF of the crew patch. Q11.
- [ ] X11. ELIZA's own `README.1ST`, perfectly polite. Q7.
- [ ] X12. Hidden ANSI by Ty visible only at 300 baud, for jokes as well as
  clues (the overpaint mechanic of E16). Q8, Q11.
## Group 4 - Milestones (S12c, queue items Q12c and Q12d)

Brief: `docs/briefs/S12c-milestones.md`. Spec: `.kiro/specs/milestones/`. Chain:
`docs/PUZZLE_CHAIN.md`. Q12c is design only; the rest is Q12d.

- [x] M-1. (Q12c) Ty's last fight, timeline, fates V-F and V-C, unlock chain
  M1 to M9, scene list and act-gate audit in `design.md`.
- [ ] M-2. (Q12d) Card: the fight in `POWER.LOG`, `BULKHEAD.LOG`, the shell
  typescript, `MEM_BAY`; find and fix any card line that still says he acted
  with his last breath after giving up.
- [ ] M-3. (Q12d) Card: the overrun post and reply, Archive "blind spot", the
  handbook page, ENGINEERING, ELIZA's mail and ops log, the new scenes.
- [ ] M-4. (Q12d) Re-run the matrix check on the real card after the act
  gates (C9).
