# Design - Triggers

Requirements: `requirements.md`. Same split as the power prologue: pure logic
in a header a plain C++ compiler can test, a thin card and session side on
top.

## Files

| File | Role |
| --- | --- |
| `src/core/haunt_logic.h` | Pure. Rule table, INI parse from plain triples, `due()`, flag helpers. `std::string` only. Test: `tools/test/haunt_logic_test.cpp` |
| `src/core/haunt.h` | Board side: loads `STORY/TRIGGERS.INI`, builds the context for a session, saves flags, runs the effects |
| `sd/BBS/STORY/TRIGGERS.INI` | The rules. Starts with the two prologue-adjacent examples and the format comment |

`haunt.h` needs `Session`, users, the message store, the power settings,
`art()` and the conferences, and is called from `board.h`, `messages.h`,
`viewers.h`, `cmd_doors.h`, `cmd_main.h` and `runtime.h`. It is included in
`bbs_core.h` right after `bbs/settings.h`; `loadSettings()` calls it through
one forward declaration.

## Rule format

```
[RULE_ID]            ; letters, digits, _ ; at most 24
event=login|crew_login|read|file|art|door|idle|minutes
match=               ; read: message number; file/art: file name; crew_login:
                     ; handle; door: first key
baud_max=            ; art only
after=               ; idle: seconds; minutes: minutes
has=A,B              ; all of these flags
lacks=C              ; none of these flags
level_min= level_max=
set=FLAG1,FLAG2
page_from= page=
mail_from= mail_conf= mail_subject= mail_body=
post_from= post_conf= post_subject= post_body=
art=NAME             ; /BBS/ANSI/STORY/NAME.ANS (ASCII twin in /BBS/ASCII/STORY)
add=text             ; read rules only
```

`|` in a body or page text breaks a line, as in `POWER.INI`. Conferences are
given by name or number.

## Decision logic

```
due(table, ctx) -> rules in file order
  ctx = event, match, baud (0 = unlimited), amount (idle s / minutes),
        flags (before the event), level
```

A rule is due when: event equal; match equal (if given); baud ok; amount >=
after; has all; lacks none; level in range; and not "spent":

- visible rule (page, mail, post or art): spent when marker `T_<ID>` is set;
- `set`-only rule: spent when every flag is already there;
- `add` rules are never spent (they show on every re-read).

All rules are tested against the flags before the event (R2.4), so one event
cannot chain. `ctx.flags` is a snapshot.

## Applying a due rule

1. New flags = `set=` plus, for a visible rule, the marker. Add to the
   caller's own account (`flagsAdd` in the pure header, capped at 255).
2. Save (`touchUser`). If the save fails: stop, run nothing visible.
3. Sync other online sessions of the same user id.
4. Write mail and posts at once. Queue page and art in
   `Session.hauntShows`; `hauntTick` shows them when the caller is at a menu
   prompt (`hauntCalm`: a menu state, nothing streaming, nothing typed, no
   hidden input), so a page never breaks the reader, a picture or a
   half-typed line. A caller who hangs up first never sees them; the marker
   stays set (fails safe: at most once, never twice).
5. `add=` rules change nothing: their lines go back to `showMessage`, which
   adds them below the body.

`flagsAdd` reports flags that did not fit; `haunt.h` logs them
(`TRIGGER_FULL`). Skipped rules are logged as `TRIGGER_SKIP`, fired rules as
`TRIGGER`, failed saves as `TRIGGER_FAIL`.

## The caller

- Own session: flags are `s.user.flags`, level `s.user.level`.
- Read-only session (crew): the caller is the own account recorded in
  `Session.ownUserId` (set when an own account signs in on this connection),
  plus `Session.carriedFlags` for flags earned before any sign-in. The
  context flags are the union. Grants go to the own record (found by id,
  saved, other sessions synced) or, with none, to `carriedFlags`.
- On every own sign-in or sign-up, `finishLogin` sets `ownUserId` and merges
  `carriedFlags` into the record (one save), then runs the `login` rules.

## Hooks (where the engine hears about events)

| Event | Place |
| --- | --- |
| login, crew_login | `finishLogin` in `bbs/board.h` |
| read | `showMessage` in `bbs/messages.h` (the `add=` lines go into the rows) |
| file, art | `openFileViewer` in `bbs/viewers.h` after the reader opens |
| door | `doorCommand` in `bbs/cmd_doors.h` before `enter` |
| idle, minutes | `hauntTick` from `bbsTick`, once a second |

The maintenance account bypasses all of it (it never reaches `finishLogin`
past its own branch). SysOps are ordinary callers.

## Decisions and limits

- Flag limit raised from 63 to 255 characters (`cleanFlags` in `core/util.h`).
  63 was enough for one or two permissions; a story with a dozen clue flags
  and markers would have lost flags silently. `USERS.DAT` has no other limit
  on the field. This is a change to the user record's cap only.
- Marker flags use the rule id, so keep ids short.
- A crew visit that wants mail is two rules: the visit sets a flag, a later
  `login` rule has it.
- Goodbye on a crew (read-only) account logs off to the login prompt and
  keeps the line, like the ship console's EXIT. Without it a caller who
  visits a crew account first would lose the carried flags when the
  connection closed. From the login prompt, G still hangs up.
- `idle` is the time since the caller's last key. Timed rules are looked at
  only when the idle or online time passes an `after=` value, so a crew
  session does not read `USERS.DAT` every second.
- Inside one section, the first value of a key wins (as `iniGet` does). Two
  sections with the same name next to each other read as one section; a
  repeat later in the file is skipped.

## Not in this item

Ghost pages that look like WHO activity (Q4), the SysOp voice keyed by
trigger (Q4), glitches (Q13), the scene engine (Q3).
