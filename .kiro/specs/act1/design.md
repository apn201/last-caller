# Design - Act 1

## The story record

`src/core/story.h`, included after `core/users.h`. `User` gets a `story`
string that is never written into `USERS.DAT`. Reading functions:

- `storyRead(id)`, `storyLoad(User&)`, `storySave(User&)` (one `putRecord`).
- `userFlagList(User&)`: the flag field and the story record as one list,
  for conditions.
- `storyOpen(User&)`: load, then migrate. `haunt::splitFlags` (pure,
  tested) puts `SYSOP`, `READONLY`, `BACKROOM` and `VAULT` in the access
  part and everything else in the story part.

`hasFlag(User, flag)` looks at both fields. Everything that read
`user.flags` for the story reads `userFlagList` now (triggers, scenes,
ghosts, doors). `hauntGrant` writes the story record. `hauntOwnLogin` adds
the flags a crew visit carried. `refreshLiveUser` keeps the live session's
story string.

`haunt::flagsAdd` takes a cap, default `MAX_STORY_FLAGS` (4000). The old
255 stays as `MAX_FLAGS`, the limit of the flag field.

Crew accounts keep their own access flags in the flag field (`READONLY`,
and the mailbox flag of their own mail, e.g. `VISIT_TY` on `TYVANCE`).

## Accounts

`STORY/ACCOUNTS.INI`, one section per handle: `name`, `location`,
`password` (empty = nobody can log in), `level`, `flags`, `calls`, `last`
(`YYYY-MM-DD HH:MM`). `ensureStoryAccounts()` (in `bbs/accounts.h`) runs at
start-up after `ensureMaintAccount()` and creates what is missing. User
record field 21 is `last`; older lines read as empty. An own login sets it
to the board clock; a read-only login never saves.

Passwords on the card are drafts: `TYVANCE` = `140374` (14 Mar 1974),
`CHAOS` = `LADDER`; the others wait for act 2, which plants their hints.

## Conferences

| Id | Name | Posting | Gate |
| --- | --- | --- | --- |
| 0 | Lounge | callers | none |
| 1 | Mission Log | read-only | none |
| 2 | Garden Notes | read-only | none |
| 3 | Mail: TYVANCE | read-only | `VISIT_TY` |
| 4 | Spoilers | callers | none |
| 5 | No Spoilers | callers | none |
| 6 | Radio Room | callers | none |
| 7 | SysOp Console | callers | SYSOP |

History messages (act 1 content) are `MSGBASE/<id>/*.TXT`. The directory
is now tracked by git; `.gitignore` no longer hides `sd/BBS/MSGBASE` and
`sd/BBS/FILES` (they held the old board's curated content, which is gone).
The `FidoNet echo` row is removed from the message view.

`read_only=1` in a conference section: `canPost(Session)` is false for
everyone but the SysOp. The post and reply keys and the editor save use it.

## Triggers added

- `event=screen`: `match=USERS` (user list shown) or `match=BULLETIN
  <TITLE>` (a bulletin shown).
- `event=read` with `match=~SUBJECT` (first 39 characters of the subject,
  upper case): a message by its subject, so authors never count message
  numbers.

## Evidence in act 1

| Item | How it is reached | Grants |
| --- | --- | --- |
| E01 The Crew (bulletin) | B | `KHI KAI KTI KVI KZI` |
| E02 User list | U | `KHI KAI KTI KVI` |
| E10 Orbit 1 | Gallery | `KHI KAI KTI KVI` |
| E09 Ty's journal | Gallery | `KVI` |
| E04 ELIZA in chat | C | `KZI` (SYSOP.INI) |
| E05 to E07 first posts | Mission Log, Lounge, Garden Notes | `KHI`, `KAI`, `KTI` |

Fates and causes come with act 2 and 3.

## The lead

Welcome bulletin (mandatory) says what a guest may do. The user list shows
`TYVANCE` as "all decks" with his last call a minute before the clock
stopped. Achterberg's Lounge post of 16 Nov says Ty uses the same six digits
for everything. The crew bulletin has the birthday. Ty's journal says how
he writes it. After two minutes online without a visit, ELIZA pages.

## Files

`FILES/001_ANSI_GALLERY` (Ty's Gallery): `ORBIT_1.ANS`, `ORBIT_2.ANS`,
`JOURNAL.TXT`, `DATES.BBS`. `FILES/002_LIBRARY`: `COMMS.TXT`, `README.1ST`,
`GALLEY.TXT`. `DATES.BBS` lines are `NAME YYYY-MM-DD`.

## Scenes

`TY_DESK`: Ty's account login, in his voice, over his art. Played once, on
the first `TYVANCE` visit. `ELIZA_WAKE` stays.

## Art

`tools/ansi/screens/act1.py`: bulletins WELCOME, CREW, STATUS, Orbit 1,
Orbit 2, `TY_DESK`, the file cabinets header. Rendered and looked at.
