# Testing on the board

## Power prologue (S4, per caller since Q5b)

Wording on screen is draft. Do this on the stick and on the server; once over
the browser and once over Telnet. Power is per caller: there is no board-wide
state, no `POWER.DAT`, and no SysOp power command.

Set up: copy the card (`tools/prepare-sd.ps1`), so `STORY/POWER.INI`,
`STORY/SHIP.INI`, `ANSI/POWER` and `ASCII/POWER` are on it. To make the drop
quick set `timer_sec=20` in `STORY/POWER.INI`.

1. Connect. You see the BIOS banner (SOLAR BUS ONLY, ELIZA NOT RESPONDING,
   the service label) and `Login:`. No new-account choice.
2. Type a wrong name (`bob`): `Unknown caller.`, `Login:` again.
3. Wait at `Login:`. Nothing drops you (only the idle limit of two minutes).
4. Log in as `admin` / `admin`. The ship console opens, no bulletins, no
   menu. The prompt is `SHIP>`. The timer runs. Wait for `timer_sec`: you
   see `SUNSIDE ENDS - CARRIER LOST` and the connection closes.
5. Connect again. This time the short reboot banner. Log in as `admin`.
6. Try `HELP`, `STATUS`, `POWER`, `ATTITUDE`, `PAYLOAD`, `COMMS`. Try a
   refused one (`ATTITUDE HOLD`) and nonsense (`FOO`): terse answer, never a
   dead end. `HELP` does not list `CREW` and `CREW` is an unknown command.
7. `SOURCE RTG`. The warning screen appears and asks for `[K]` + ENTER. Type
   `X`: `SWITCH ABORTED. NO CHANGE.` Do `SOURCE RTG` again, type `K`.
8. `RTG ONLINE / BUS STABLE / ATTITUDE HOLD`, ELIZA's wake screen, her line
   in amber, "CREW REGISTRATION ENABLED". Wait past `timer_sec`: no drop.
   `HELP` now lists `CREW`.
9. `CREW`, then ENTER at the handle: back at `SHIP>`, `No account made.`
   `CREW` again: handle, password, real name, location. `Account created. Log
   in with it.` and `Login:`.
10. Log in with the new account. The `ELIZA_WAKE` scene plays, then the main
    menu. The Doors menu shows VolaMUD and Diagnostics, not SHIP.
11. Open a second connection at any point (a second tab or Telnet) and play
    the prologue in it: its timer is its own. Switching to the RTG in one
    does nothing to the other.
12. Log in as a registered account from a new connection: straight to the
    board, no console, no drop after `timer_sec`. Log in twice with the same
    handle: the first session is told `NO CARRIER` (takeover). `admin` can be
    on many times at once.
13. As SysOp: log in normally at `Login:`. `Z` shows no power commands.
14. Read-only check: log in as `admin`, then as SysOp open the user editor
    (`U`) and look at the `admin` record before and after: calls and
    last-used values are unchanged by admin logins. `READONLY` is listed in
    the flags.
15. Without ANSI (a Telnet client with ANSI off): the same steps show the
    plain ASCII versions.

## Host test of the power logic

`src/core/power_logic.h` has no platform types:

```sh
zig c++ -std=c++17 -Wall -Wextra -Wno-nullability-completeness -I src/core tools/test/power_logic_test.cpp -o power_logic_test
./power_logic_test      # prints "all passed"
```

(`g++` or `clang++` work with the same flags.)

## Server build and end-to-end check (second build, from Q1)

The server cannot be built on this Windows PC. The staging host builds and
tests the committed `main` (Rocky Linux 9, `lastcaller@helppox.com`, key auth,
no sudo, home folder only, 127.0.0.1 only):

```powershell
powershell -File tools/staging-check.ps1            # push main, build, test
powershell -File tools/staging-check.ps1 -Crew HANDLE:PASSWORD   # once a crew account exists
```

Commit first: only the pushed `main` is tested. The script pushes to the
`staging` remote (`lastcaller@helppox.com:last-caller.git`), then runs
`tools/staging-remote.sh` on the host, which:

1. checks out into `~/lastcaller-staging/src` and builds with cmake;
2. copies `sd/BBS` to a fresh card `~/lastcaller-staging/card` (`timer_sec=8`,
   no `DATA/STATE`) and runs the phase-1 smoke
   check on a throwaway copy;
3. starts `tinybbs-server` on `127.0.0.1:8086` with Telnet on
   `127.0.0.1:2323` (nothing is stopped:
   anything already on the HTTP or Telnet port aborts the run);
4. `curl`s `/api/board` and the terminal page;
5. runs `tools/test/e2e_telnet.py` (standard library only): front page (no New), wrong name,
   `admin`/`admin` and the timer drop, reconnect, ship console (no `CREW`),
   `SOURCE RTG` (abort, then `K`), ELIZA's wake-up, the timer stopped, `CREW`,
   (cancel, then an account made in the console), log in with it, the `ELIZA_WAKE` scene, the doors menu, Diagnostics (query codes), the NOWHERE shell (its gate is opened on the disposable card, `--shell-open`), a registered caller with no prologue and no timer, `admin` twice, Goodbye, and a crew login when
   `-Crew` is given (SKIP otherwise);
6. stops the server and prints `STAGING CHECK: PASS` or `FAIL` (exit code 0 or 1).

The server log is `~/lastcaller-staging/server.log`; the last e2e output is
`e2e.out`. Wording is draft, so the e2e script matches short markers; when
text changes, change the marker in `tools/test/e2e_telnet.py`.

The report for each item must say whether this ran. "Not run" is never "passed".
