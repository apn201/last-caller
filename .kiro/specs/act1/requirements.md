# Requirements - Act 1 content and the story record

Queue item Q7 in `docs/briefs/QUEUE.md`. Story: `.kiro/specs/story-bible/`
(R5 unlock chain, R7 first-time caller, R13 social board, R14 side story).
Voice: `.kiro/steering/writing.md` and `story-voice.md`. All caller-visible
wording is a draft until the author's final edit.

## Glossary

- Access flag: a flag on the account record that opens something by itself
  (`SYSOP`, `READONLY`, `BACKROOM`, `VAULT`).
- Story flag: every other flag: evidence (`KHI`...), markers (`T_`, `SC_`,
  `G_`, `E_`), `M_` rows, `END1`, `END2`, `CREW`, `VISIT_TY`.
- Story record: one line per account in `DATA/STATE/STORY.DAT`
  (`id|FLAG,FLAG`), written with `putRecord`. It holds the account's story
  flags.
- Open evidence: evidence a caller can reach without a crew login (E01 to
  E15 of the matrix that exist before day 22).

## Requirements

### R1 - The story record

1. THE board SHALL keep an account's story flags in its story record, not in
   the 255-character flag field of `USERS.DAT`.
2. THE story record SHALL hold at least 4000 characters of flags.
3. WHEN an own account signs in and its flag field holds story flags, THE
   board SHALL copy them to the story record and then remove them from the
   flag field. IF the story record cannot be written, THEN the flag field
   SHALL stay as it was.
4. WHEN the board checks whether an account holds a flag (conferences,
   bulletins, file areas, triggers, scenes, doors, the manifest), IT SHALL
   look at both the flag field and the story record.
5. Flags SHALL only grow: after any grant, migration or carry, every flag
   that was held is still held, and a flag that does not fit is named in the
   event log and not silently lost.
6. A grant SHALL be saved before anything is shown for it. IF the save
   fails, THEN nothing SHALL be shown.

### R2 - Accounts and the user list

1. A card SHALL be able to name accounts in `STORY/ACCOUNTS.INI`: handle,
   real name, location, password, level, flags, calls and last call. THE
   board SHALL create each one that is missing at start-up and SHALL leave
   an existing account alone.
2. THE card SHALL name `CMDR`, `CHAOS`, `GARDEN`, `TYVANCE` (read-only crew
   accounts), `ELIZA` (the SysOp, nobody can log in as her) and `MCC`
   (ground relay).
3. THE user list SHALL show each caller's last call. Last-call dates SHALL
   fit the timeline in the story bible.
4. A read-only login SHALL NOT change its account's last call.

### R3 - Conferences

1. THE board SHALL have the public conferences Lounge, Spoilers, No Spoilers
   and Radio Room, where callers read and post.
2. THE board SHALL have the story conferences Mission Log and Garden Notes
   and the mail of the crew, where nobody posts but the SysOp.
3. A conference SHALL be read-only when `CONFERENCES.INI` says
   `read_only=1`. WHERE a conference is read-only, THE board SHALL NOT offer
   Post or Reply to anyone but the SysOp.
4. A crew mailbox conference SHALL open for a caller who has visited that
   crew account.
5. History messages SHALL NOT be labelled as a FidoNet echo.

### R4 - Open evidence

1. THE board SHALL hold the identity of every crew member and of ELIZA in
   at least two open items of different media (bulletin, user list, art,
   message, file, chat).
2. WHEN a caller opens an item that proves a fact, THE board SHALL grant
   that fact's evidence flag, once, and the manifest SHALL then offer it.
3. A trigger SHALL be able to fire on a bulletin shown, the user list shown
   and a message read by its subject, as well as the events that exist.
4. THE posts of 2 to 21 Nov 1998 SHALL show everyday life first: food, noise,
   the thermostat, music at night, an exam. Small wrong notes SHALL be
   things a reader notices later, never explained.
5. EVERY dated item SHALL fit the timeline, with the real 1998 weekday, and
   SHALL be dated before the epoch.

### R5 - The lead

1. WITHIN two minutes of the main menu a first-time caller SHALL have a
   named crew account and a reason to try it: the user list and the crew
   bulletin say so, and ELIZA says so if the caller has not tried.
2. THE `TYVANCE` password SHALL be hinted by at least two items. It SHALL
   NOT be shown in clear.
3. WHEN a caller signs in to `TYVANCE`, THE board SHALL grant `CREW` and
   `VISIT_TY` to the caller's own account (carried to the next own login)
   and a short scene in Ty's voice SHALL play.

### R6 - Files

1. THE file listing SHALL show a file's date from the card's `DATES.BBS`
   when it names one, else as before.
2. THE card SHALL hold the areas Gallery (Ty's art and journal, Orbit 1 and
   2) and Library (the comms manual, ELIZA's `README.1ST`).
3. THE comms manual SHALL list the diagnostics query codes that exist.

### R7 - Verification

1. THE host tests SHALL show that story flags only grow through splitting,
   migration, grants and carries, for all inputs of a property test.
2. THE end-to-end test SHALL play act 1 as a fresh caller would: make the
   account in the console, read the open evidence, see the manifest appear
   for it, post in a public conference, find no post key in a story
   conference, sign in as `TYVANCE` with the birthday, sign in again as
   themselves, and find the crew mailbox open.
3. `pio run` and the staging check SHALL pass.
