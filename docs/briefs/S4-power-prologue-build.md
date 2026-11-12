# Brief S4 - Build the power prologue

From: Claude Code (planner). To: Kiro (builder). First real engine slice.
Write `.kiro/specs/power-prologue/tasks.md` from the existing requirements
and design, commit it, then implement the tasks that the design maps to
S4. Follow `.kiro/steering/tech.md` strictly: portable core, no ESP32 or
Linux headers in `src/core`, `src/bbs`, `src/doors`.

## Scope (S4 only)

- Power state machine BROWNOUT / POWERED with the RTG decay fall-back.
  Persisted under `DATA/STATE` with `putRecord`, so it survives a reboot.
- `STORY/POWER.INI`: all timings and the maintenance account, as in the
  design. The fall-back is configurable: `decay_after_idle_min=15`, and
  `0` means power never falls back (one-off). Juha has not decided which;
  ship 15 as the default.
- Brownout front end: boot banner art, login prompt that accepts only the
  maintenance account (no New), per-session random drop timer (window
  from `POWER.INI`), the "SUNSIDE ENDS - CARRIER LOST" line, then a real
  disconnect on both transports.
- SHIP door (`src/doors/ship.h`, one line in `doors.h`), command table
  from `STORY/SHIP.INI`, reachable only by the maintenance account. Power
  to RTG flips the board to POWERED for everyone; ELIZA's wake-up screen
  and line follow; the caller is told to make their own account.
- SysOp command to force BROWNOUT or POWERED (for testing and demos).
- Read-only account flag for `admin` (and later the crew accounts): no
  post, delete, upload, edit or password change; concurrent logins allowed
  for read-only accounts only. If concurrent logins of one account touch
  per-user files, make those writes no-ops for read-only accounts.
- Art with `tools/ansi/`, rendered and looked at: brownout boot banner
  (BIOS-like, decayed), ship systems console frame, ELIZA wake-up screen.
  Keep the HALCYON style; add Ty's tag only where the story says he drew
  it (the console is engineering, not his).

Not in S4: triggers, ghosts, crew logins granting flags, private ELIZA
mail beyond the wake-up line. Leave clean hooks, do not build them.

## Verify

1. `pio run` passes.
2. The server build cannot run on Windows. Instead write a small host-side
   test of the power state machine and the drop window if it can compile
   with a plain C++ compiler here; if no compiler is available, say so.
   Never claim the server build passed.
3. Walk through the prologue by reading the code paths for both transports
   and list them.
4. Juha will test on the board; write a short manual test script in
   `docs/TESTING.md` (connect, wait for the drop, log in as admin, switch
   power, see ELIZA, make an account, force brownout as SysOp, wait for
   the decay with a short test setting).

## Done when

Tasks ticked, committed in small commits, `pio run` green, `docs/TESTING.md`
has the prologue script. Reply with the files changed, the test results,
and anything that is still a guess about the engine.
