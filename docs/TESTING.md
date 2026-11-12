# Testing on the board

## Power prologue (slice S4)

Wording on screen is draft. Do this on the stick and, later, on the server;
once over the browser and once over Telnet.

Set up: copy the card (`tools/prepare-sd.ps1`), so `STORY/POWER.INI`,
`STORY/SHIP.INI`, `ANSI/POWER` and `ASCII/POWER` are on it. The default
`decay_after_idle_min=0` makes the power-up one-off; for the decay test (step
14) set `decay_after_idle_min=1` in `STORY/POWER.INI` first. To make the drop
quick, `window_min=15` and `window_max=25`. Delete `DATA/STATE/POWER.DAT` on a
disposable card to start from brownout.

1. Connect. You see the BIOS banner (SOLAR BUS ONLY, ELIZA NOT RESPONDING,
   the service label) and `Login:`. No new-account choice.
2. Type a wrong name (`bob`): `MAINTENANCE ONLY`, `Login:` again.
3. Wait without doing anything. Within the window (60 to 120 s, or what you
   set) you see `SUNSIDE ENDS - CARRIER LOST` and the connection closes (the
   browser says carrier lost, Telnet disconnects).
4. Connect again. This time the short reboot banner. Another drop time.
5. Log in as `admin` / `admin`. The ship console opens, no bulletins, no menu.
   The prompt is `SHIP>`.
6. Try `HELP`, `STATUS`, `POWER`, `ATTITUDE`, `PAYLOAD`, `COMMS`. Try a
   refused one (`ATTITUDE HOLD`) and nonsense (`FOO`): terse answer, never a
   dead end.
7. `SOURCE RTG`. The warning screen appears and asks for `[K]` + ENTER. Type
   `X`: `SWITCH ABORTED. NO CHANGE.` Do `SOURCE RTG` again, type `K`.
8. `RTG ONLINE / BUS STABLE / ATTITUDE HOLD`, ELIZA's wake screen, her line
   in amber, "Make your own account", `Press ENTER`.
9. Press ENTER. Login prompt with `(N)ew user, (L)ogin, or (G)oodbye?`.
   No more drops: wait three minutes to check.
10. Open a second connection before step 9 (a second tab or Telnet) and leave
    it at the `Login:`. When you switch the power in step 7 it prints
    `RTG ONLINE. THE SHIP HAS POWER.` and shows the login with New.
11. Make your own account. WELCOME bulletin, main menu. The Doors menu
    shows VolaMUD, not SHIP.
12. Log in as `admin` again from another connection while your own account is
    online: both work (admin is read-only and may be on many times). In the
    main menu, as a regular account, log in twice with the same handle: the
    first session is told `NO CARRIER` (takeover).
13. As SysOp (`Z`): the menu shows `POWER ON / POWER OFF`. Type `POWER` for the
    state. `POWER OFF`: every other session ends within the window; yours stays.
    A new connection shows the banner again. Log in as SYSOP from it: the
    SysOp gets in during brownout and `POWER ON` restores the ship.
14. Decay: with `decay_after_idle_min=1`, power up (steps 5 to 8), log out
    and disconnect everyone. Wait 1 minute plus. Connect: the banner is back
    and `Login:` only. With the default `decay_after_idle_min=0` it stays powered for good.
15. Reboot the stick while powered: it comes back powered. While in
    brownout: it comes back in brownout.
16. Read-only check: log in as `admin`, then as SysOp open the user editor
    (`U`) and look at the `admin` record before and after: calls and
    last-used values are unchanged by admin logins. `READONLY` is listed in
    the flags.
17. Without ANSI (a Telnet client with ANSI off): the same steps show the
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
2. copies `sd/BBS` to a fresh card `~/lastcaller-staging/card` (window 4 to
   8 s, `decay_after_idle_min=0`, no `DATA/STATE`) and runs the phase-1 smoke
   check on a throwaway copy;
3. starts `tinybbs-server` on `127.0.0.1:8086` with Telnet on
   `127.0.0.1:2323` (nothing is stopped:
   anything already on the HTTP or Telnet port aborts the run);
4. `curl`s `/api/board` and the terminal page;
5. runs `tools/test/e2e_telnet.py` (standard library only): brownout banner,
   wrong name, drop after the window, reconnect, `admin`/`admin`, ship console,
   `SOURCE RTG` (abort, then `K`), ELIZA's wake-up, power holds, new account,
   the `ELIZA_WAKE` scene, the doors menu, Diagnostics (query codes), the NOWHERE shell (its gate is opened on the disposable card, `--shell-open`), a second login, Goodbye, and a crew login when
   `-Crew` is given (SKIP otherwise);
6. stops the server and prints `STAGING CHECK: PASS` or `FAIL` (exit code 0 or 1).

The server log is `~/lastcaller-staging/server.log`; the last e2e output is
`e2e.out`. Wording is draft, so the e2e script matches short markers; when
text changes, change the marker in `tools/test/e2e_telnet.py`.

The report for each item must say whether this ran. "Not run" is never "passed".
