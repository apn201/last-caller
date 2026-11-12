# Requirements - Power prologue

Spec with no code yet (requirements and design only). Brief:
`docs/briefs/S3b-power-prologue.md`. Story: `.kiro/specs/story-bible/`.
Slice: S4 in `docs/SLICES.md`. All caller-visible wording here is a draft
until the author's final edit.

## Glossary

- Brownout: the board's state while the ship runs on the solar bus only.
- Powered: the state after a caller switches the power source to the RTG.
- Prologue: everything a caller does in brownout, up to ELIZA waking.
- Maintenance account: `admin`, password `admin`, a ship account, not the
  SysOp.
- Ship systems console: the one menu the maintenance account sees.
- Crew accounts: `TYVANCE`, `CMDR`, `GARDEN`, `CHAOS` and `admin`.
- Read-only account: an account that cannot change anything on the board.
- Window: the 60 to 120 seconds a session lasts in brownout.

## Requirements

### R1 - Brownout state

1. WHILE the board is in brownout, THE board SHALL show on connect a boot
   banner with a BIOS-style memory count, the line "SOLAR BUS ONLY -
   ATTITUDE CONTROL OFFLINE", ELIZA as "NOT RESPONDING" and a hint that the
   maintenance account defaults are unchanged (for example "MAINT ACCT
   DEFAULTS NOT CHANGED - SEE LABEL").
2. WHILE in brownout, THE board SHALL offer a login prompt and nothing else:
   no new accounts, no guest, no menus, no bulletins.
3. WHILE in brownout, EVERY session SHALL end after a random time between
   60 and 120 seconds from connect, with the line "SUNSIDE ENDS - CARRIER
   LOST".
4. WHEN a caller reconnects in brownout, THE board SHALL show the reboot
   banner again.
5. THE window SHALL be long enough to solve the power-up in two or three
   tries (a first-time caller, 60 seconds at worst).
6. WHILE in brownout, THE board SHALL treat the caller's time as paused
   for idle limits: only the window ends a session.

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

### R4 - Power on

1. WHEN the confirmed switch succeeds, THE board SHALL change to powered,
   stop dropping sessions and show "POWER SOURCE: RTG" with the systems
   stabilising (attitude holding).
2. WHEN power is restored, ELIZA's first line SHALL appear on the
   switching caller's screen: "Good evening. I have been asleep for some
   time." (draft).
3. WHEN power is restored, EVERY other session in brownout SHALL be told
   the ship has power and SHALL return to the login prompt without being
   disconnected.
4. WHEN the switching caller's console session ends, THE caller SHALL reach
   the login prompt with the new-account option shown.

### R5 - Then the board proper

1. WHILE the board is powered, THE login prompt SHALL offer login and new
   account.
2. WHEN a caller creates an account, THE board SHALL start the story-bible
   game for that caller: the WELCOME bulletin from ELIZA and the main menu.
3. THE game SHALL NOT start for a caller who only used `admin`.

### R6 - Shared power state

1. THE power state SHALL be board-wide: one caller's power-up SHALL open the
   board for everyone online and for everyone who connects while it
   holds.
2. THE power state SHALL survive a restart of the board (stick or server).
3. WHILE the board is powered, WHEN the last caller leaves, THE board SHALL
   fall back to brownout after 15 minutes (draft; recommended, Juha to
   confirm).
4. THE SysOp SHALL have a command to force brownout or powered at any time.
5. IF a caller connects while the board is in the 15 minutes of grace,
   THEN THE grace timer SHALL stop and powered SHALL stay.

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
