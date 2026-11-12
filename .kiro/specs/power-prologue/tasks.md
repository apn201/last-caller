# Tasks - Power prologue (slice S4)

Requirements: `requirements.md`. Design: `design.md`. Brief:
`docs/briefs/S4-power-prologue-build.md`. Wording of all caller-visible text
is a draft until the author's final edit.

Decisions taken from the brief, where the design left a choice:

- The fall-back setting is `decay_after_idle_min` in `STORY/POWER.INI`
  (the design called it `grace_minutes`). Default 15; `0` = power never
  falls back.
- The maintenance account is created by the board itself on first boot from
  `POWER.INI` (`maint_account`, `maint_password`), with the `READONLY` flag.
- Not in S4: triggers, ghosts, crew logins granting flags (R8.4), private
  ELIZA mail beyond the wake-up line, sound effects, fixed last-call dates.
  The hooks are left clean and named in the code.

## Group 1 - Engine

- [x] 1. Portable power logic, `src/core/power_logic.h`: no Arduino types,
  so a plain C++ compiler can test it (R1.3, R6.3, R6.5, R9.3)
  - [x] 1.1 `powerWindowMs`: random window between `window_min` and
    `window_max`, always inside the bounds
  - [x] 1.2 `PowerMachine`: BROWNOUT / POWERED, the decay timer (counts only
    while no session is connected, a connection stops it, `0` = never)
  - [x] 1.3 Host test `tools/test/power_logic_test.cpp`
- [x] 2. Board state, `src/core/power.h` (R6.1, R6.2, R6.4)
  - [x] 2.1 `STORY/POWER.INI` loader
  - [x] 2.2 `DATA/STATE/POWER.DAT` through `putRecord`; a missing file means
    `start=` from the INI (default brownout)
  - [x] 2.3 `powerSet()` writes the file and tells the sessions
- [x] 3. Session timers in `bbsTick` (R1.3, R1.6, R6.3)
  - [x] 3.1 Per-session drop deadline, picked on connect (and when a SysOp
    forces brownout under live sessions)
  - [x] 3.2 "SUNSIDE ENDS - CARRIER LOST", then `closeSession` on both
    transports
  - [x] 3.3 Idle limits paused in brownout; the decay timer ticks
- [x] 4. Brownout front end (R1, R2.1, R2.2)
  - [x] 4.1 `loginPrompt`: boot banner the first time, reboot banner after,
    `Login:` only, no New
  - [x] 4.2 `loginCommand`: only the maintenance account in brownout; New
    refused if power fell under a session at the choice prompt
  - [x] 4.3 Maintenance account created at boot, `READONLY`, not SysOp
  - [x] 4.4 `finishLogin`: maintenance account goes to the SHIP console, no
    bulletins, no menu, no counters
- [x] 5. SHIP door (R2.2, R3, R4)
  - [x] 5.1 `src/doors/ship.h`, one line in `doors.h`; hidden from the door
    menu
  - [x] 5.2 Command table from `STORY/SHIP.INI`
  - [x] 5.3 Source switch with the confirming key
  - [x] 5.4 Power on: wake screen, ELIZA's line, other sessions told, back
    to the login prompt with New offered (R4.3, R4.4)
- [x] 6. SysOp `POWER ON` / `POWER OFF` (R6.4)
- [x] 7. Read-only accounts (R8.1 to R8.3, R8.5)
  - [x] 7.1 `READONLY` flag, `canWrite(s)`, listed in the user editor
  - [x] 7.2 Writers ask `canWrite`: post, reply, edit, delete, upload,
    door saves, read pointers, login counters, download counters
  - [x] 7.3 Menus hide what a read-only caller cannot do
  - [x] 7.4 Concurrent logins for read-only accounts only; a second login
    of any other account takes over the first

## Group 2 - Card

- [x] 8. `sd/BBS/STORY/POWER.INI` and `sd/BBS/STORY/SHIP.INI`
- [x] 9. Art with `tools/ansi/`, rendered and looked at
  - [x] 9.1 Brownout boot banner (BIOS-like, decayed) and reboot banner
  - [x] 9.2 "Sunside ends" screen
  - [x] 9.3 Ship systems console frame
  - [x] 9.4 RTG switch warning
  - [x] 9.5 ELIZA wake-up screen
  - [x] 9.6 ASCII versions for callers without ANSI

## Group 3 - Proof

- [x] 10. `pio run` passes
- [x] 11. Host test of the power state machine and the drop window passes
- [x] 12. Code-path walk-through for both transports (in the report)
- [x] 13. `docs/TESTING.md`: manual prologue script for the board
- [x] 14. Docs: `docs/SLICES.md` S4 status, `README.md` if it lists the card
  files
