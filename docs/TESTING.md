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
## Music and sound (Q10)

Wording is draft. The tunes and sounds come from `tools/mod/` (`python tools/mod/gen_mods.py`, `python tools/mod/gen_sfx.py`; the same bytes every run). Check the modules in the card's own libopenmpt build with `node tools/mod/openmpt_check.js <files>` (channels, orders, length, peak level).

1. Browser, signed in as a caller: `F`, Ty's Gallery. `SUNSIDE.MOD` (9 Nov) and `THERMO.MOD` (5 Nov) are listed after the journals. Open `SUNSIDE.MOD`: the player plays it and lists the sample names under the pattern view in three columns (stops at 0100, greets, the poem, "eliza says nice tune"). `N` goes to `THERMO.MOD`. Tap or `ESC` back to the list.
2. Telnet: the same file opens as a text view (title, `M.K.`, 4 channels, orders, patterns, the sample names), `SPACE` pages, `N`/`P` step to the next tune, `Q` back to the list. No raw bytes reach the terminal.
3. Ty's own account (`TYVANCE`, `140374`): Ty's Desktop holds `HARBOUR.MOD` (5 Dec), six channels. The bassline runs, the rest is sketched, the lead stops mid-phrase, the song ends on a break. A visitor does not see the area.
4. Delete the player from the card (`ASSETS/PLAYER/player.js`): the browser opens the same text view instead of an error.
5. Cues, browser only, volume up: a new caller switches the ship to the RTG in the console: the reboot chirp. Let a maintenance session run out its two minutes: a burst of static with the carrier lost screen. The first login of a new account: a low hum as ELIZA wakes. Opening `POWER.LOG`: a relay click as the memory scene starts. None of them repeat. Telnet hears nothing and sees no escape text. Remove `ASSETS/SFX`: no error, no sound.
6. `/sfx/CHIRP.WAV` in the address bar downloads a WAV; `/sfx/NOPE.WAV` is 404.

Host test: `zig c++ -std=c++17 -Wall -Wextra -I src/core tools/test/modinfo_logic_test.cpp -o modinfo_logic_test`, then `./modinfo_logic_test sd/BBS/FILES/001_ANSI_GALLERY/SUNSIDE.MOD sd/BBS/FILES/001_ANSI_GALLERY/THERMO.MOD sd/BBS/FILES/006_TY_DESKTOP/HARBOUR.MOD`. The scene test covers the `sound=` key.

## Easter eggs (Q11)

Wording is draft. None of these touches the game; check that last.

1. Signed-in caller, main menu. `ATI` prints `28800` and `OK`; `+++` and `ATZ` print `OK`; `ATFOO` prints `ERROR`; `ATDT` prints `NO DIALTONE`; `ATDT 555-0199` is `BUSY`, `ATDT 555-0188` is `NO ANSWER`. After each, the prompt is back.
2. `ATDT 555-0172` (the number is in the Library's `NODELIST.TXT`): `DIALING`, `CONNECT 2400`, the SUBLEVEL 2 screen and the away message, `NO CARRIER`, then the prompt. In the browser, with the volume up, the distant modem plays once (`MODEM.WAV`). Telnet hears nothing and sees no escape text.
3. `ATH0` (or `ATH`): `NO CARRIER` and the connection closes. At the login prompt `AT` answers `OK`, `ATH` hangs up, and a name that merely starts with AT (`atlas`) is still just an unknown name; a real account named like a modem command still logs in.
4. Main menu: `XYZZY` answers; `FORTUNE` and `/FORTUNE` print a different line now and then; `POST` shows the 486's self test (BIOS 06/14/94, memory test 8200K for 8192K installed, the Turbo switch on); `REBOOT` does the same. None is in the menu or in `?`.
5. Node chat: type `open the pod bay doors`. ELIZA answers about hatches and does not repeat the line. `shall we play a game` and `daisy` get a line each (once per caller).
6. Files: Library `BROCHURE.TXT` and `NODELIST.TXT`; Records (after the crew visit) `Y2K.MEM` and `TARIFF.TXT`; Ty's Desktop (Ty's account) `TYDEMO.NFO`. Opening `TYDEMO.NFO` and going back to the menu plays Ty's demo: three frames, the scroller, the crash on frame 3 at the same word every time; Enter skips; you are at the main menu after it.
7. Doors: `S`, "Snake (by tyv)", for any caller with power (also a crew login). W A S D, several at once; an empty line goes on. Walls, the bulkhead, the rack and yourself end it; the crew's table shows with `YOU` in it; ENTER plays again, Q leaves. Nothing is saved: sign in again and the table is the crew's.
8. Nothing changed in the game: the manifest, the notebook, the flags (`SC_TY_DEMO` and `T_EV_TYDEMO` are the only traces) and the endings are as before. The e2e test plays the whole game and then uses the eggs.

Host test: `zig c++ -std=c++17 -Wall -Wextra -I src/core tools/test/egg_logic_test.cpp -o egg_logic_test`, then `./egg_logic_test` (modem parser and snake rules).
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

## Verification (Q12)

On the Windows PC, no board needed:

- `python tools/story_lint.py` : every flag granted, every fact and ending
  reachable, no softlock, scenes sound, targets exist, evidence with one
  source and banned words as warnings. Runs on save of card story files (hook).
- `python tools/run_host_tests.py` : builds and runs the host tests of the
  shared logic, including `story_property_test` (random orders of play over the
  card's own triggers and manifest). Runs on save of the logic headers (hook).
  This is not the server build.
- `python tools/test/bbs_caller_selftest.py` : the bbs-caller MCP server
  against a fake board.

The server build runs the same tests through CTest (`ctest --test-dir
server/build`); `tools/staging-check.ps1` does it and fails if one fails.

Playing it with the `playtester` agent:

1. Commit, then `powershell tools/staging-playtest.ps1` (leave it running). It
   builds the pushed main on the host, starts the board on a fresh card with the
   card's own timers and forwards 127.0.0.1:12323 here.
2. In Kiro pick the `playtester` agent (`.kiro/agents/playtester.md`); the
   `bbs-caller` MCP server comes from `.kiro/settings/mcp.json`. Without MCP:
   `python tools/mcp/bbs_caller.py cli connect|send "text"|keys "K"|read|stop`.
3. Stop the board: `ssh lastcaller@helppox.com "touch ~/lastcaller-staging/stop"`.

Record in `docs/PLAYTEST.md`. The run of 13 Nov is there.

A first-time game takes the agent about 200 to 250 tool calls, more than one
agent session holds. Play it as one caller over two sessions: session A makes
the account and appends its notes (handle, password, what it read, leads);
session B reads only that section and finishes (run 4c, Q12b).

## Playtest fixes (Q12b)

By hand, on a fresh card:

1. At `Login:` type a wrong name: "Unknown caller." and one line pointing at
   the service label (`POWER.INI` `[TEXT] unknown_hint`). `ADMIN`/`ADMIN` in
   capitals works too.
2. Read a long Lounge post: no word is cut at the edge and no single word is
   left alone on a line. Typing a number in the pager then ENTER goes there.
3. Lounge, `R`: the newest 20 and `E = earlier`; `E` lists the 20 before,
   `L` goes back. Play `ORBIT_3` with `S` to its end: the picture is not drawn
   a second time.
4. Manifest, the ELIZA row, a blank with nothing in it: "Nothing to choose
   from yet." and a `Notebook:` line saying where to read. The fate "Died on
   a spacewalk" reads "his" on the men's rows.
5. Diagnostics `HELP`: the manual, operator codes as queries, release on the
   payload host. Shell `help`: `/var/spool/out`; after the manifest also the
   `release` line.
6. `python tools/story_lint.py` fails if a right answer can only be earned on
   a crew visit (`open sources`).

## Milestones (Q12d)

The chain M1 to M9 (`docs/PUZZLE_CHAIN.md`). On a fresh card, over Telnet and
in the browser:

1. A new caller (after the prologue) sees `ELIZA_WAKE`, then `ELZ_WELCOME`.
   The Lounge lists nothing after 22 Nov 11:00 (no "Notice", no "Module 3").
2. Sign in as `TYVANCE` (140374): the Lounge, Mission Log and Garden Notes
   now go to 1 Dec. As `CHAOS` (LADDER): 2 Dec onward.
3. Post in the Lounge: 200 zeros on line 1, 54 on line 2: posted. Again with
   55: `MESSAGE LOST`, a glitch, then `MEM_GLITCH`. The post is in nobody's
   list. The event log has `OVERRUN`.
4. `C`: ELIZA says she cannot account for some minutes. "I can fix that.
   Give me SYSOP." gets a calm refusal; "Something else." and "Say something
   else." open the typed chat; `open the pod bay doors` still answers.
5. `L` (Log in as another), `CMDR`/`ANDERS`, Files, Commander's Desk,
   `HANDBOOK.TXT`. `G`, sign in as yourself.
6. `C`: "Run the memory check. It needs SYSOP mode." `ELZ_GRANT` plays; the
   main menu says SYSOP view is on. `Z` is still refused; the SysOp console
   conference is not listed; Engineering is. `GOLIVE.TXT` item 14.
7. `L`, `ELIZA`/`KESTREL4`: `ELZ_OPS`; Files, Mission Computer, `OPSLOG.TXT`;
   Doors: the NOWHERE shell (after the order code in Diagnostics). In the
   shell `cat /home/tyvance/typescript`: `SYSOP REQUIRED`, no `ground-1`.
8. Open `POWER.LOG` (Ship Logs): `MEM_BAY` plays the fight to "Ty?".
9. With the manifest whole, the final command works in ELIZA's shell; `G`
   back to your own account for bulletin 5 and Eero's mail.
10. As the real SysOp: `C` opens the chat directly; every post is listed.

Checks without a board: `python tools/story_lint.py` (the chain, its gates and
dependencies, no story grant of an access flag), `python tools/run_host_tests.py`
(`overrun_logic_test`, `gatePass` in `haunt_logic_test`, `then=` in
`scene_logic_test`, P8 to P10 in `story_property_test`).
## Polish (Q13)

Art: `HALCYON` in profile at three stages of decay (`tools/ansi/screens/_profile.py`): bulletin 2 (2 Nov 1998, all lit), bulletin 4 (8 Dec 1998, dish dark), main menu (2026, dish and half the lower wing gone, one lit window). Ty's tag `.tyv.98.` is on his snake and his demo frames.

Glitches (needs the board; the logic is covered by `python tools/run_host_tests.py glitch_logic_test`):

1. For a quick look, copy the card and set `first_sec=5` and `gap_sec=60` in `STORY/GLITCH.INI`; `RELOAD` in the SysOp tools picks it up.
2. Sign in as your own account (not `admin`, not a crew login). Stay at the main menu. After about 5 s one dim grey line appears under the menu (the browser may play HUM or STATIC; Telnet rings the bell). The next one comes no sooner than a minute later, and is a different one.
3. Open a message, the reader or a door: nothing is printed while it is open. A glitch waits for the next menu prompt.
4. Event log: `GLITCH <id> <handle> node <n>` lines. A broken section is named in `GLITCH_SKIP` (the staging check fails on it).

Mobile (browser terminal, reviewed from the page's code and styles; no phone was at hand): the header wraps, the canvas scales to the width, and on a narrow screen it is smoothed instead of pixelated. On touch screens (`pointer:coarse`) the buttons are 44 px high. Still to check on a real phone: ENTER and BACKSPACE with the on-screen keyboard open, rotating to landscape, and how readable 80 columns are at 390 px wide.

## Deploy (Q14)

The hosted install is in `docs/DEPLOY.md`; `deploy/AGENT_PROMPT.md` is the prompt for an agent. Checks that can run without root: `bash -n deploy/*.sh`; render `deploy/apache-*.conf.in` and run `httpd -t -f` on a throwaway config on the staging host; run `deploy/bootstrap-sysop.py --port N` against a disposable server card (exit 0, a second run exits 3). The install, the reset, the unit and certbot need root on a real host: the first run there is the test. Chat cap: in the chat, type a line of 300 characters; the others see 240.

