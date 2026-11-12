# Requirements - Power prologue

Q5b (Nov 13) made power per caller; R1, R4, R5 and R6 below are the current
rules. Brief:
`docs/briefs/S3b-power-prologue.md`. Story: `.kiro/specs/story-bible/`.
Slice: S4 in `docs/SLICES.md`. All caller-visible wording here is a draft
until the author's final edit.

## Glossary

- Brownout: what every connection starts in: the ship runs on the solar bus only.
- Powered: a session after its caller has switched the power source to the RTG,
  or any session of a registered caller. Power is per session, never board-wide.
- Prologue: everything a caller does in brownout, up to ELIZA waking.
- Maintenance account: `admin`, password `admin`, a ship account, not the
  SysOp.
- Ship systems console: the one menu the maintenance account sees.
- Crew accounts: `TYVANCE`, `CMDR`, `GARDEN`, `CHAOS` and `admin`.
- Read-only account: an account that cannot change anything on the board.
- Power timer: the 2 minutes (`timer_sec` in `POWER.INI`) the maintenance login has to switch to the RTG.

## Requirements

### R1 - Front page and the power timer

1. ON every connection, THE board SHALL show a boot banner with a BIOS-style
   memory count, the line "SOLAR BUS ONLY - ATTITUDE CONTROL OFFLINE", ELIZA
   as "NOT RESPONDING" and a hint that the maintenance account defaults are
   unchanged (for example "MAINT ACCT DEFAULTS NOT CHANGED - SEE LABEL"),
   then a login prompt. THE first connection since start-up SHALL get the
   full banner, later ones the short reboot banner.
2. THE front page SHALL offer a login prompt and nothing else: no "New", no
   guest. A registered caller who logs in SHALL go straight to the board.
3. WHEN the maintenance account logs in, THE board SHALL start a power timer
   for that session only, of `timer_sec` seconds (default 120).
4. IF the timer runs out before that caller has switched to the RTG, THEN
   THE session SHALL end with the line "SUNSIDE ENDS - CARRIER LOST".
5. WHEN the caller switches to the RTG, THE timer SHALL stop.
6. THE timer SHALL be the only thing that ends a session early: no other
   session, account or state affects it.

### R2 - Maintenance login

1. THE maintenance account SHALL be `admin`, password `admin`.
2. THE maintenance account SHALL NOT have SysOp rights and SHALL NOT reach
   any menu other than the ship systems console.
3. THE boot banner SHALL be the only hint the board gives for the account.
4. THE board SHALL NOT let `admin` change its password.

### R3 - Ship systems console

1. THE console SHALL be a plain 1998 engineering console with four
   subsystems: power bus status, attitude, payload and comms.
2. THE power bus status SHALL show the source, the RTG state and the
   battery bank state, each as the story says them (draft: source SOLAR
   DIRECT, RTG DISCONNECTED - MANUAL, battery bank OPEN CIRCUIT).
3. WHEN the caller asks to switch the source to the RTG, THE console SHALL
   show a warning and ask for a confirming key that the warning names. Any
   other key SHALL abort the switch without a penalty.
4. WHEN the caller gives a wrong or unknown command, THE console SHALL
   answer with a terse refusal and SHALL show how to ask for help. THE
   console SHALL NOT reach a dead end.
5. THE attitude, payload and comms subsystems SHALL show read-only status
   (attitude: tumbling; payload: powered at low draw; comms: downlink
   disabled) and refuse changes with a terse line.
6. WHEN a caller reads the warning at step 3, THE caller SHALL be able to
   find the confirming key without any outside knowledge.

### R4 - Power on (this caller)

1. WHEN the confirmed switch succeeds, THE session SHALL be powered: its
   timer stops, and the console shows "POWER SOURCE: RTG" with the systems
   stabilising (attitude holding).
2. WHEN power is restored, ELIZA's first line SHALL appear on that caller's
   screen: "Good evening. I have been asleep for some time." (draft).
3. THE switch SHALL NOT change any other session.
4. WHEN power is restored, THE console SHALL gain one more option, CREW, to
   create a crew account. BEFORE the switch THE option SHALL NOT be listed
   and SHALL answer as an unknown command.

### R5 - Account creation and the board proper

1. THE only place an account is made SHALL be the CREW option of the ship
   console (handle, password, real name, location). ENTER at the handle or
   password SHALL cancel and return to the console.
2. WHEN the account is made, THE caller SHALL be told to log in with it and
   SHALL be back at the login prompt.
3. AT the first login with that account, THE board SHALL start the
   story-bible game for that caller: the `ELIZA_WAKE` scene, then the main
   menu.
4. THE game SHALL NOT start for a caller who only used `admin`.
5. WHERE a card has no `POWER.INI`, THE board SHALL have no prologue and
   the usual New user prompt.

### R6 - Nothing is shared

1. THE board SHALL have no board-wide power state, no `POWER.DAT`, no decay
   timer and no SysOp `POWER ON` or `POWER OFF` command.
2. NO caller's progress through the prologue SHALL depend on another
   caller. Any number of sessions MAY be in the prologue at once, each with
   its own timer, all as `admin`.
3. THE SysOp SHALL log in like any registered caller.

### R7 - Solo play

1. EVERYTHING after the prologue SHALL be per caller. ELIZA's pages and
   mail SHALL go to that caller privately.
2. CLUE and progress state SHALL be flags on that caller's own record.
3. NO caller SHALL be able to spoil, block or change another caller's game.

### R8 - Read-only crew accounts

1. `TYVANCE`, `CMDR`, `GARDEN`, `CHAOS` and `admin` SHALL accept many
   callers logged in at once.
2. THESE accounts SHALL NOT post, reply, delete, edit, upload, change
   their password, change their profile or be edited by anyone but the
   SysOp.
3. A crew account's data SHALL NOT change through use: no counters, no
   last-call time, no read pointers, no door state (clues can never be
   destroyed).
4. WHEN a caller logs in to a crew account, THE board SHALL record what the
   caller unlocked on the caller's own account, not on the crew account:
   IF the connection has a signed-in or earlier signed-in own account, THEN
   its record gets the flags at once; OTHERWISE the connection remembers
   them and the caller's next own login or sign-up in that connection gets
   them (for example CREW, or a "you were here" flag).
5. THE board SHALL NOT show a menu option that does not work for a
   read-only account.

### R9 - Transports

1. THE prologue SHALL work the same over Telnet and the browser.
2. ON the stick, THE same state machine SHALL run. The real solar story
   SHALL make no difference to it.
3. THE state machine SHALL live in the shared core, with the platform seam
   unchanged (no ESP32 or Linux headers in `src/core`, `src/bbs`,
   `src/doors`).

### R10 - Content on the card

1. THE banner art, console text, timings, the maintenance account and the
   confirming key SHALL be on the card, not in the firmware.
2. EVERY new screen SHALL be ANSI art made with `tools/ansi/`, rendered and
   looked at before commit.
3. NO caller-visible text SHALL expose a card path.
