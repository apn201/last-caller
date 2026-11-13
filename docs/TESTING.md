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

## Manifest and endings (S8, Q6)

Wording is draft. The real evidence flags come with the content (Q7 to Q9); until then the manifest can only be reached by giving the account the flags, so on a card:

1. As SysOp, user editor: add `KHI,KAI,KTI,KVI,KHF,KAF,KTF,KVF,KHC,KAC,KTC,KVC,KZI,KZO,KZW` to a test account (the e2e rule does this when Diagnostics is entered). For the endings also open the shell (`NOWHERE_OPEN`).
2. Main menu: `R Crew manifest` appears only now (not on a crew login, not for `admin`).
3. `R`: four rows, no ELIZA row. `N` is the notebook. Open a row, a blank, pick; `0` clears. Leave and sign in again: picks are kept.
4. Fill one row right and another wrong, `S`: `Rows confirmed: 0 of 4.` and no names. Fill a second row right, `S`: the two are named and locked. Wrong rows are never named.
5. Confirm all four: the ELIZA row opens. A wrong ELIZA row says only the count. Goodbye now: ELIZA thanks you for your interest (partial ending).
6. Fill the ELIZA row right: `MANFULL` plays. Doors, `N`, `release outbound`: the `ENDING` scene. Leave it queued: you are on the crew list; another caller sees your handle in `W`. Run it again and send: return traffic, and `W` says the downlink is open.
7. Browser and Telnet alike; without ANSI the ASCII art shows.

Host test: `zig c++ -std=c++17 -Wall -Wextra -I src/core tools/test/manifest_logic_test.cpp -o manifest_logic_test` then `./manifest_logic_test sd/BBS/STORY/MANIFEST.INI`.

## Act 1 (Q7)

Wording is draft. On the stick and on the server, over Telnet and the browser, as a caller who has never seen the board:

1. Prologue as above, make an account in the ship console, log in. The Rules and the Welcome bulletins show; the `ELIZA_WAKE` scene plays. The main menu has no `R Crew manifest` yet.
2. `B`, bulletin 3 (The Crew): four bios, five accounts named, Ty's birthday (14 Mar 1974). Back at the menu `R Crew manifest` is there. `R` lists the four crew rows. Without ANSI the same bulletins show as plain text.
3. `U`: the crew accounts with their last calls (CMDR 27 Nov, GARDEN 1 Dec, CHAOS 3 Dec, TYVANCE 8 Dec 03:47, MCC 22 Nov 14:08, ELIZA). TYVANCE is "All decks (VIP)".
4. `M`: the Lounge offers Post; post something. `A`, 1 (Mission Log): no Post, no Reply key, `P` says the account is read-only. Garden Notes and Mission Log read like a diary. Radio Room and the two Spoilers areas take posts.
5. `F`: Ty's Gallery (journal, Orbit 1, Orbit 2, dates in 1998) and the Library (`COMMS.TXT` lists the query codes, `README.1ST`, `GALLEY.TXT`). Orbit 1 and 2 look right in the browser and over Telnet.
6. Stay two minutes without visiting Ty: ELIZA pages about his account. `J` does not list `Mail: TYVANCE`.
7. Goodbye, then log in as `TYVANCE`. A wrong guess is refused. The password is his birthday as six digits: day, month, year (140374). The `TY_DESK` scene plays in his voice. `J` lists `Mail: TYVANCE`; four of his mails are from his mother, his uncle's office and `lumen`. No Post anywhere. Goodbye returns to `Login:`.
8. Log in as yourself: ELIZA's mail about Ty's account is in your inbox, and `J` now lists `Mail: TYVANCE` for you.
9. Story flags: after steps 1 to 8 `DATA/STATE/STORY.DAT` holds a line for your account (with `T_`, `SC_` and `K` flags) and the flag field in the user editor shows only access flags. An older card with story flags in the flag field moves them on the next login (event `STORY_MOVED`).
10. SysOp: `U` for the user editor shows `READONLY` on the crew accounts. Post in Mission Log as SysOp: allowed.

Host tests of the new logic (flags only grow through grants, carries and migration): `tools/test/haunt_logic_test.cpp`, same command as the other host tests.

## Act 2 (Q8)

Wording is draft. After act 1, as the same caller, over Telnet and the browser:

1. `M` in the Lounge: posts go on from 22 Nov (the Founder's memo, ELIZA's notice, the module thread of 23 Nov and her refusal, Locks, Fourteen, the deaths, Ty's last post from the seat on 7 Dec). Mission Log ends on 27 Nov 06:10, Garden Notes on 1 Dec mid-sentence, Radio Room on Eero's porch light (22 Nov). None take posts.
2. `F`, Ty's Gallery: Orbit 3 to 8 and `JOURNAL2.TXT`, dated 18 Nov to 7 Dec. Orbit 5 shows ANDERS on the cuff, Orbit 6 the tray labelled HANAMI. Orbit 3 shows a covered board. In the browser set SPEED to 300 and press S in the reader: the covered row (LADDER) draws first, then the cover over it. Over Telnet it flashes past; the still page never shows it.
3. `F`, Ship Logs and Records are listed (they need `CREW`). Opening `SUIT1.LOG`, `GARDEN.LOG` and `CAPSULE.LOG` each plays a memory scene at the next menu prompt (`MEM_EVA`, `MEM_GARDEN`, `MEM_CAPSULE`). Archive and Ty's Desktop are not listed.
4. Goodbye, `CMDR` / `ANDERS`: Mail: CMDR, her message from the wrist unit (27 Nov 11:52). `GARDEN` / `HANAMI`: Mail: GARDEN, her last letter (1 Dec 18:20). `CHAOS` / `LADDER`: the Archive conference. Wrong guesses are refused.
5. Log in as yourself: Mail: CMDR, Mail: GARDEN and Archive are listed. Reading "Bay 3" in the Archive plays `MEM_REC`. `F`, Archive: four pieces of the order; read in piece order their codes give `NW1-98-44-17` (for act 3).
6. `TYVANCE` again: Ty's Desktop with `ORBIT_9.ANS`, unfinished. As yourself it is not listed.
7. `R`: every crew row and the ELIZA row can now be filled from what was read. No shortcut rule is on the staging card any more.
## Act 3 (Q9)

Wording is draft. After act 2, as the same caller, over Telnet and the browser:

1. `M`, Lounge: the last post is Ty's, "power bay", 8 Dec 04:13. Radio Room ends with two posts from the club of 2026 (stamped 8 Dec 1998 04:22 and 04:31); Eero's "Porch light" of 22 Nov is the last 1998 post.
2. `F`, Ship Logs, `POWER.LOG`: the `MEM_BAY` scene plays at the next menu prompt and stops on "Ty?".
3. `D`: the doors menu lists Diagnostics, not the NOWHERE shell. `I`, type `NW1-98-44-17` (any case, spaces or none): ELIZA accepts it. Back in the doors menu the shell (`N`) is listed. A wrong code is "no query with that code".
4. `N`: `ls`, `cat /etc/orders.cfg` (order 5), `cat /srv/rack2/clients.lst`, `cat /var/spool/out/queue` (hold: incident report not on file), `cat /var/spool/out/index`. `release out-1122 ground-1` now: "command not found". `incident.rpt` is not there.
5. Finish the manifest (`R`). In the shell `ls /var/spool/out` now lists `incident.rpt`. `release out-1122` and `release out-1122 earth` print only the usage line. `release out-1122 ground-1` starts the `ENDING` scene.
6. Leave it queued: the crew list. Run it again and send: the club station, ELIZA files the wire, Eero's last line "The light is on." Then `B`: bulletin 5, News Wire, in ANSI and (ANSI off) plain. Sign in again: a private message from OH9ZQW in the Lounge, once.
7. Without ANSI the same screens show their plain twins. A caller who never sent has no bulletin 5.

Host test of the shell logic (usage line): `zig c++ -std=c++17 -Wall -Wextra -I src/core tools/test/shell_logic_test.cpp -o shell_logic_test`, then `./shell_logic_test sd/BBS/STORY/SHELL.INI`.
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
5. runs `tools/test/e2e_telnet.py` (standard library only; since Q7 it also plays act 1: bulletins, user list, a post in the Lounge, no post in Mission Log, file areas and the journal, `TYVANCE` with a wrong and then the right password, his desk scene and mailbox, back to the own account): front page (no New), wrong name,
   `admin`/`admin` and the timer drop, reconnect, ship console (no `CREW`),
   `SOURCE RTG` (abort, then `K`), ELIZA's wake-up, the timer stopped, `CREW`,
   (cancel, then an account made in the console), log in with it, the `ELIZA_WAKE` scene, the doors menu, Diagnostics (query codes), the NOWHERE shell (its gate is opened on the disposable card, `--shell-open`), the crew manifest and the endings (`--evidence`: on the disposable card, entering Diagnostics grants the evidence flags; see below), a registered caller with no prologue and no timer, `admin` twice, Goodbye, and a crew login when
   `-Crew` is given (SKIP otherwise);
6. checks the event log for story failures (flags that did not fit, scenes or story files that failed);
7. stops the server and prints `STAGING CHECK: PASS` or `FAIL` (exit code 0 or 1).

The server log is `~/lastcaller-staging/server.log`; the last e2e output is
`e2e.out`. Wording is draft, so the e2e script matches short markers; when
text changes, change the marker in `tools/test/e2e_telnet.py`.

The report for each item must say whether this ran. "Not run" is never "passed".
