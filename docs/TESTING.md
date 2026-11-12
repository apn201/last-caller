# Testing on the board

## Power prologue (slice S4)

Wording on screen is draft. Do this on the stick and, later, on the server;
once over the browser and once over Telnet.

Set up: copy the card (`tools/prepare-sd.ps1`), so `STORY/POWER.INI`,
`STORY/SHIP.INI`, `ANSI/POWER` and `ASCII/POWER` are on it. For the decay test
set `decay_after_idle_min=1` in `STORY/POWER.INI` first. To make the drop
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
    state. `POWER OFF`: every session ends within the window, yours too. A
    new connection shows the banner again.
14. Decay: with `decay_after_idle_min=1`, power up (steps 5 to 8), log out
    and disconnect everyone. Wait 1 minute plus. Connect: the banner is back
    and `Login:` only. With `decay_after_idle_min=0` it stays powered.
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
