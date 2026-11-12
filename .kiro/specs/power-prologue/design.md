# Design - Power prologue

Requirements: `requirements.md`. Brief: `docs/briefs/S3b-power-prologue.md`.
Story: `.kiro/specs/story-bible/design.md` ("The module and the power").
No code yet. All caller-visible wording is a draft until the author's final
edit.

## What the engine already has

Read before writing this (files in `src/`):

| Need | What exists | Gap |
| --- | --- | --- |
| Login flow | `bbs/cmd_login.h`: states `LOGIN_CHOICE`, `LOGIN_HANDLE`, `LOGIN_PASSWORD`, `NEW_*`; handle length minimum is 3, so `admin` fits. Login ends in `board.h` (`s.authenticated`, `touchUser`, mandatory bulletins) | No brownout state in front of it; the login choice always offers New |
| Session pool | `core/session.h`: `Session sessions[MAX_NODES]`, same pool for Telnet and browser; `closeSession` in `board.h`; `bbsTick` in `bbs/runtime.h` already closes sessions on idle (`LOGIN_TIMEOUT_MS` 120 s before login, 30 min after) | No random per-session drop time; no "carrier lost" text; no board-wide state |
| Forced disconnect | SysOp kick in `bbs/user_editor.h` (says `NO CARRIER`, then `closeSession`) | Reusable pattern only |
| Random numbers | `platform/platform.h` has `bbsRandom()` | None |
| Board clock | `core/clock.h`: `epoch=` start, saved to `DATA/STATE/CLOCK.DAT` once a minute, restarts only when the epoch text changes | Done in S3b: `epoch=1998-12-08 04:17`, `clock_sync=` empty. A brownout reboot loses at most one minute |
| Global state that survives restart | `DATA/STATE/` holds `CLOCK.DAT` and per-user `PTR_<id>.DAT`; `core/storage.h` has `putRecord` for safe rewrites | No power state file; add `POWER.DAT` |
| Users and flags | `core/users.h` `User` with `flags` (comma list, `hasFlag`, `cleanFlags`), `locked`, `level`. `bbs/user_editor.h` lists every flag the board checks | No READONLY flag; no last-call field exists (E02 needs fixed last-call dates) |
| Concurrent logins | I found no check in the login code that blocks the same account twice. `sessionOnline` in `user_editor.h` only reports it | Verify on the board and server in S4; per-user files (`PTR_<id>.DAT`, door state keyed by user id, `touchUser`) assume one writer |
| Private messages | `core/messagebase.h`: records have from, to and a private flag; `bbs/messages.h` shows "(private)" | A way for the core to post "as ELIZA" to one caller (S5 effect "post message as a named user") |
| Config | `core/ini.h`, `sd/BBS/CONFIG/*.INI`, `sd/BBS/STORY/*.INI` loaded at boot | New file `STORY/POWER.INI` |
| Doors | `doors/doors.h` `Door` table (`enter`, `owns`, `handle`) | New door SHIP; the table is the menu, so SHIP must hide from callers who cannot use it |
| Browser media | `ASSETS/PLAYER` plays MOD and SID; `OSC 9` side channel exists | Sound effects need a new `OSC 9` tag (S7) |

Summary: sessions, login, clock, INI, flags and the door table exist. Missing
and needed: the power state and its timers, the brownout front end, the
read-only flag and its enforcement, concurrent crew logins without shared
writes, and private ELIZA messages.

## Power per caller (Q5b, Nov 13; replaces the board-wide state machine)

There is no board-wide state and no file. Every connection starts in brownout:
the front page is the boot banner and `Login:` (no New).

```
connect -> banner -> Login:
   registered account -> board (powered, no timer)
   admin/admin -> SHIP console, timer starts (timer_sec, 120)
        timer runs out -> SUNSIDE screen, session ends
        SOURCE RTG + key -> session powered, timer stops, ELIZA wakes,
                            console gains CREW
        CREW -> handle, password, real name, location -> "Log in with it"
             -> Login: -> own account -> ELIZA_WAKE scene -> main menu
```

- State lives in the `Session`: `powered`, `powerTimerOn`, `powerAt`,
  `makingCrew`. `bbsTick` ends a session whose timer is due
  (`powerSessionsTick`, `powerTimerDue`).
- `admin` is read-only and many sessions may use it at once, each with its
  own timer. Nobody's prologue depends on anybody's.
- Account creation lives only in the console: the existing `NEW_*` prompts
  are reused with `makingCrew` set, and end at the login prompt, not in a
  login. ENTER at the handle or password returns to the console.
- A card without `POWER.INI` has no prologue (usual New user prompt).
- The SysOp logs in like anyone else. No `POWER ON/OFF`.

Everything below this section that speaks of a board-wide state, `POWER.DAT`,
the decay, the grace timer, the window or `POWER ON/OFF` is superseded.

## The prologue, step by step (draft wording)

1. Connect. The boot banner plays (BIOS memory count, `SOLAR BUS ONLY -
   ATTITUDE CONTROL OFFLINE`, `ELIZA ... NOT RESPONDING`, `MAINT ACCT
   DEFAULTS NOT CHANGED - SEE LABEL`), then `Login:`. Only login is offered.
2. `admin` / `admin`. The SHIP console opens. No bulletins, no mandatory
   screens, no main menu.
3. Console, 1998 engineering style: `STATUS`, `POWER`, `ATTITUDE`,
   `PAYLOAD`, `COMMS`, `HELP`, `EXIT`. `POWER` shows the bus: source SOLAR
   DIRECT, RTG DISCONNECTED - MANUAL, battery bank OPEN CIRCUIT.
4. `SOURCE RTG` (also accepted: `RTG`, `CONNECT RTG`) shows a warning screen
   and asks for a key. The warning names the key in a line of plain text
   (draft: "REARM REQUIRES OPERATOR ACK. PRESS [K] TO ACKNOWLEDGE. ANY OTHER
   KEY ABORTS."). Ty never read this kind of screen. The caller does.
5. Wrong command: `NOT PERMITTED` or `UNKNOWN COMMAND - TYPE HELP`. Never a
   dead end; `HELP` always lists what works.
6. Confirm: `RTG ONLINE`, `BUS STABLE`, `ATTITUDE HOLD`, `RELAY CLICKS` (audio
   cue, S7). Pause. ELIZA's line in amber: "Good evening. I have been
   asleep for some time."
7. The caller reaches the login prompt (new account offered). They create
   their own account, then the WELCOME bulletin and the main menu: the
   story-bible game begins (S3, S5).

Time budget: reading the banner, logging in and reading the warning is
about 40 seconds for a first-time caller, inside the 60 second worst case.
The console prompt should be reachable in under 15 seconds of banner (the
banner plays at modem speed; keep it under 3 s).

## What lives on the card

| Item | File | Notes |
| --- | --- | --- |
| Boot banner art | `ANSI/POWER/BOOT.ANS` | `tools/ansi/`; BIOS count, status lines, the label hint |
| Reboot banner | `ANSI/POWER/REBOOT.ANS` | Shorter; shown on reconnect |
| Carrier lost screen | `ANSI/POWER/SUNSIDE.ANS` | One line, one flicker |
| Ship systems console | `ANSI/POWER/CONSOLE.ANS` | Frame and status panel |
| Switch warning | `ANSI/POWER/WARNING.ANS` | Names the confirming key |
| ELIZA wake screen | `ANSI/POWER/WAKE.ANS` | Amber ring, her first line |
| Console text | `STORY/SHIP.INI` | Command table: name, aliases, answer text, effect |
| Timings and account | `STORY/POWER.INI` | See below |
| Sounds | `ASSETS/SFX/*.WAV` or `.MP3` | Power hum, relay clicks, reboot chirp |

`STORY/POWER.INI` (draft):

```
[POWER]
window_min=60
window_max=120
grace_minutes=15
maint_account=admin
confirm_key=K
start=brownout
```

No path is shown to a caller. Names of the screens, the account and the key
are read from the card, so the stick and the server share them.

## What needs engine code

| Item | Where | Note |
| --- | --- | --- |
| Power state and `POWER.DAT` | `src/core/power.h` (new), included before `bbs/` | Pure logic; no platform headers. Read and written through the existing storage helpers |
| Window timer | `bbs/runtime.h` `bbsTick` and `Session` (deadline field) | Uses `bbsRandom()` and the existing `closeSession` |
| Grace timer | `bbsTick` | Counts sessions in the pool |
| Brownout front end | `bbs/cmd_login.h` and `loginPrompt` | In brownout: show banner, offer login only, hide New |
| SHIP door | `src/doors/ship.h` plus one line in `doors/doors.h` | Console commands from `SHIP.INI`; the door `owns` the session of the maintenance account; reachable only by it, so the door menu never shows a dead option |
| Read-only flag | `READONLY` in `User.flags`, listed in `bbs/user_editor.h` | Checked in one function `canWrite(s)` used by every writer |
| Enforcement | post, reply, delete, edit (`bbs/messages.h`, `cmd_messages.h`), upload (`uploads.h`), password change, profile edit, door saves, `touchUser` in `board.h`, `savePointer` in `session.h` | Each writer asks `canWrite`. No read-only account leaves a trace |
| Concurrent crew logins | login code and per-user state | Allow a second session for read-only accounts; keep `PTR_<id>.DAT` and door state out of the picture (see below) |
| Fixed last-call dates | `User` display in the user list (`bbs/sysop.h`) | There is no last-call field. Add an optional `LASTCALL` line for accounts, or a card table; needed for E02 |
| Private ELIZA messages | `core/messagebase.h` plus the S5 effect "post as a named user" | One private message per caller, to the caller's handle |
| Maintenance rules | `canSee`/menu code | `admin` sees only the SHIP console; main menu hidden |
| SysOp command | `bbs/cmd_sysop.h` | `POWER ON`, `POWER OFF` (force POWERED, force BROWNOUT) |
| Sounds | `src/net/routes.h`, `terminal.html` | A new OSC 9 tag; Telnet gets BEL at most (S7) |
| Wake-up flag | `User.flags` `WOKEN`-style progress flag | Set when ELIZA's greeting has been shown to that caller |

## Shared state: the RTG decay

Power is board-wide. One caller's power-up opens the board for everyone
online and after. Decided Nov 12 (Q0): the power-up is one-off. The default
is `decay_after_idle_min=0`: power never falls back by itself. The decay
timer stays in the code for tests and special setups. A SysOp command forces
either state for demos. The SysOp can log in during brownout and a SysOp
session is not dropped by the window, so `POWER OFF` never locks the SysOp
out.

Earlier recommendation, superseded: fall back to brownout 15 minutes after
the last caller leaves, so every new caller gets the prologue.

Why: most new callers (including a judge who finds the board later) get the
prologue, and nobody is locked out by someone else's progress. Without a
fall-back, the first visitor would take the opening away from everybody
after. With a fall-back, a caller can never be stuck in brownout: the
prologue is solvable in under a minute.

Alternative (not recommended): stay powered forever. The first caller
spoils the opening for all later ones.

Open for Juha: the 15 minutes and the "grace" wording. If a judge opens the
page with several tabs, the board stays powered while any are connected.

## Solo play

Everything after the prologue is per caller.

- ELIZA's pages, chat lines and mail go to that caller only, as private
  messages addressed to the caller's handle, or as session-only text.
- Clue and progress state are flags on the caller's own record (the field
  exists, flags only grow).
- Shared things stay shared and read-only: the message base, the file
  areas, the user list and the WHO ghosts.
- Nothing a caller does can spoil another caller's game. Posting is allowed
  on the public conferences, as the engine does today; story evidence
  lives in read-only places and cannot be edited or deleted by callers.

## Read-only crew accounts

Accounts: `TYVANCE`, `CMDR`, `GARDEN`, `CHAOS`, `admin`. Flag `READONLY`.

Rules (R8):

- Many sessions at once.
- Cannot post, reply, delete, edit, upload, change the password or profile.
- Use never changes the record. That means no `touchUser` counters or time,
  no read pointers saved, no door state saved, no "last call" change, no
  credit changes. The user list keeps the dates the card wrote (E02).
- Menu options that would not work are hidden (for example Post, Upload,
  Password, door games that save).

### What a crew login unlocks and where it is recorded

Two designs.

Option A (recommended): the unlock lands on the caller, not on the crew
account. A connection remembers which crew accounts it has seen
(`visited`, in the session). When the connection signs in or signs up as a
real account, or already did earlier, that account gets the flags the
crew login grants (for example `CREW`, and a "you were here" flag). The
crew session itself gets the crew account's access for the session only.
So a caller can do the natural thing: sign up, log out, log in as
`TYVANCE` with the birthday, look around, log in as themselves, and find
the CREW option on their own account. A caller who only ever used a crew
login in the connection has the progress held in the connection, and
applies it when they sign up there.

Option B: an in-session "switch user". The caller stays signed in as
themselves and a crew password is typed inside the board; the board opens
a read-only view of that account and sets the flags on the caller. No
second session is needed, so no concurrency questions arise at all.

Simpler in this engine: B needs no change to login and no concurrent
sessions, but it breaks the obvious move (log out, log in as TYVANCE),
hides the puzzle in a hidden command, and needs a new prompt. A reuses the
login the callers already know, and the only new parts are the `visited`
set and the read-only flag, which are needed in any case. Choose A. If
concurrent logins turn out to be hard on the stick (per-user files), fall
back to B for the crew accounts and keep `admin` as the only multi-login
account.

The `visited` set is lost when the connection drops. That is acceptable:
the passwords are on the board and replayable, and a caller who has signed
up gets the flags at once.

## Mapping to slices (`docs/SLICES.md`)

| Piece | Slice |
| --- | --- |
| Clock fixed (`epoch`, `clock_sync=`) | Done in S3b |
| Power state, `POWER.DAT`, `power.h`, window and grace timers | S4 |
| Brownout front end, SHIP door, `SHIP.INI`, `POWER.INI` | S4 |
| Banner, console, warning, wake screens (ANSI) | S4 (art with each screen) |
| `READONLY` flag, `canWrite`, concurrent crew logins, fixed last-call | S4 |
| SysOp `POWER ON/OFF` | S4 |
| Private ELIZA mail and the wake line as a message | S5 (post as a named user) and S9 (scripted lines) |
| ELIZA wake line spoken in her voice | S9, with a plain fallback in S4 |
| Sound effects (hum, relay, chirp) | S7 |
| Ty's tunes and sample names | S7 and story-bible C12 |
| Test of the prologue on the board and the server, both transports | S4 done-when; S10 playtester repeats it |

Done when for S4 (proposed): from a cold connection, a stranger finds the
maintenance hint, logs in as `admin`, reads the warning, switches to the
RTG within two or three tries, sees ELIZA's line, signs up, and reaches the
main menu, on the stick and the server, over Telnet and the browser.

## Verification

- Both builds must pass after any change in `src/core`, `src/bbs`,
  `src/doors` or `src/platform`. The Windows machine cannot run the server
  build; say so in the report.
- Properties to test (server build): the window is always within the INI
  bounds; powered state survives restart; a read-only record is bit for bit
  unchanged after any sequence of logins and commands; flags only grow.
- Timeline check for the story: `epoch` is 1998-12-08 04:17, after the
  last post (04:15 bulletin).
