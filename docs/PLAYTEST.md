# Playtests

## Summary (Q12, 13 Nov 2026)

Played by the `playtester` agent (`.kiro/agents/playtester.md`) through the
`bbs-caller` MCP server (`tools/mcp/bbs_caller.py`, Kiro tool calls
`bbs_connect`, `bbs_send`, `bbs_read`), over Telnet, against the staging
board (`tools/staging-playtest.ps1`: the committed main, built on the host,
a fresh disposable card, the card's own two-minute timer, SSH tunnel to
127.0.0.1). The agent was told not to read the story files and used only
what the board showed it.

- Run 1: stopped at the login prompt. Cause: only the first connection since
  start-up got the boot banner with the service label; every later caller got
  the short banner that says "SEE LABEL" and shows no label. The label also
  read `PASSWORD DEFAULT` while the password is `admin`. Fixed in
  `tools/ansi/screens/power.py` (the short banner carries the label; the
  password line reads ADMIN).
- Run 2 (two sessions, about 450 tool calls): all four crew rows filed;
  stuck on the third blank of ELIZA's row. The account it used had made the
  crew visits on an earlier account, so its own account held none of that
  evidence. No ending.
- Run 3 (about 95 tool calls): finished. Crew visits first, then its own
  account, the manifest whole, the NOWHERE shell, `release out-1122 ground-1`,
  ending 1 (Send it), bulletin 5. Endings 2 and 3 were not played by the
  agent (the end-to-end test plays both).

Tool front end: the MCP stdio server inside Kiro. Calls were made by the
agent; nothing was scripted. The server build and `ctest` were not part of
the playtest; they run in the staging check.

## Open items for the author (not fixed here)

1. "SEE LABEL" cost the first run a full dead end. The label is fixed; a
   caller who types a wrong name still gets only "Unknown caller.".
2. ELIZA's last blank has no visible source on the caller's own account. A
   new account that has not carried crew-visit evidence sees "Nothing to
   choose from yet" and no hint where to look. Consider one line from ELIZA
   or the notebook.
3. `release` is in no `help` list in the shell. COMMS.TXT section 6 names it,
   on its last page. Same for the order code being a Diagnostics query.
4. Fate and responsibility option wording is for one person ("her suit",
   "the garden module") but offered on every row.
5. Message bodies wrapping mid-word at 79 columns, the pager redrawing a
   message when paging on, and the Lounge list starting at #28 were reported
   by the agent. Not checked by hand yet.
6. The agent reported `[30m` fragments and quick single keys landing one
   step late. The first was most likely the tool splitting a colour code
   across two network reads; the tool now holds back a half-arrived code
   (`bbs_caller_selftest.py` covers it). Not confirmed on a second run.
   The late keys are the board answering key presses without ENTER while the
   tool returns after a quiet gap; use a longer `quiet_ms`.
7. "SOLAR BUS ONLY" on the short banner after the RTG was switched on
   somewhere else is by design (power is per caller).

The agent's own reports follow, unedited.
## Playtest 13 Nov 2026 (Q12)

Result: not finished. No manifest filed, no ending chosen. About 25 tool
calls in all, 2 connections, all over Telnet.

Route: connect, BIOS boot screen (HALCYON-486 FLIGHT BIOS REV 2.04, solar bus
only, ELIZA not responding, "MAINT ACCT DEFAULTS NOT CHANGED - SEE LABEL"),
then the `Login:` prompt. I never got past it.

Stuck (the whole session): the login prompt.
- Every name got "Unknown caller.": new, ?, help, guest, maint, MAINT,
  maintenance, service, label, sysop, halcyon, operator, and an empty line.
- `eliza` is the only known name. It asks for a password. Pressing ENTER on
  the blank password gives "Bad password." I did not guess any passwords.
- The boot screen points at a "label", but nothing a Telnet caller sees
  shows one: no label text, no art, no help key. Then I stopped guessing
  names.
- I waited about 60 s at the prompt and nothing arrived. No prologue scene
  and no timer showed. My brief said there would be a two-minute prologue
  timer at the start. I saw none, so either it starts after login or it
  never started.

Faults / unclear:
- Dead end on the first screen for a Telnet caller. The maint login is
  hinted at ("SEE LABEL") but never shown. If the label is only on the
  browser page or on the physical stick, the Telnet transport breaks the
  "same on both transports" rule. A first-time caller has no way in.
- "Unknown caller." gives no guidance and no new-user path.
- Check that the prologue or label screen fires on a Telnet connect.

## Run 2 (after the banner fix)

Two sessions by the same tester, all over Telnet. Session A ran out of turns.
Session B picked up from notes on 14 Nov 2026 (my session date).

Result: not finished. Four crew rows were filed and locked. The ELIZA row
stopped at 2 of 3: "Why she is still running" kept saying "Nothing to choose
from yet". The NOWHERE `release` command was never available, so I got no
final scene and chose no ending. Session B took about 240 tool calls.
Session A took roughly as many (exact count lost).

### Session A (from notes)

Route: BIOS boot with the service label (this fixed the Q12 dead end), admin
login, engineering console, SOURCE RTG and [K] ack, CREW registration
(account WREN), main board, bulletins, Mission Log, Garden Notes, Lounge, Ty's
Gallery, Library (COMMS.TXT), Diagnostics, manifest. Picks made: CMDR
(Halvorsen, spacewalk, airlock refused fourteen times), TYVANCE (Vance, power
bay, Sponsor's budget cut / mock-up capsule), CHAOS identity and fate.

Faults seen:
- Output lag with quick single keys. A key sent with no ENTER often got no
  reply until the next line, so the next input went to the wrong prompt.
- Raw SGR text leaking into screens, e.g. `[30m`, `[0m`, `[46m`, `[1;30;47m`.
  Session B saw this too: main menu frame (before CALLER ACCESS), mission
  document footer, airlocks art, file-cabinet frame, ORBIT_7.
- Q in a message area sometimes went all the way to the main board.
- The manifest option lists grow as clues come in. Fine in itself, but the
  numbers move, so a remembered number picks a different line.
- The fate option "Died on a spacewalk when her suit air ran out" says "her"
  and is offered for every row, men included. "Sealed in the garden module"
  is likewise worded only for Tanabe.
- TYDEMO.NFO describes a demo that I could not reproduce.
- Nothing says the order code (NW1 98 44 17) is entered as a Diagnostics
  query. COMMS.TXT section 6 does say so, but only on its last page.

### Session B

I did not know WREN's password, so I logged in as admin. The RTG was already
on and CREW gave me a new account (Wren3). Route: ELIZA intro (1 = radio
amateur), bulletin 3, manifest (all blank, options locked), Mission Log 48-60,
Garden Notes 67-71, Library COMMS.TXT (release syntax in section 6), Doors,
Diagnostics (200, then NW1 98 44 17 opened the NOWHERE shell), NOWHERE (help,
ls, cat readme, var/log/outbound, var/spool/out/queue and index,
etc/orders.cfg, srv/rack2), JOURNAL2.TXT, ORBIT_8, ORBIT_7, Lounge 39, 46, 47,
manifest (all four crew rows filled, [S] locked them, "Rows confirmed: 4 of
5", ELIZA row opened), ELIZA row 1 and 2 filled, then the search for blank 3.

What I tried for ELIZA blank 3, all with no new option:
- Diagnostics 100, 101, 300, 400, 500, and LADDER.
- NOWHERE: all files listed (clients.lst, rates.txt, hosts, motd,
  orders.cfg), uname, whoami.
- Node chat with ELIZA: "Why are you still running?" got "I see.", "What
  are your orders?" got a refusal, "What is in rack 2?" got "I see."
- ORBIT_3 with [S] play shows "KEY LADDER (des. do not tell ty)". Logging in
  as CHAOS / LADDER worked and showed a third file area, Archive, with four
  standing-order fragments (FRAG_Q, V, K, D). The pieces join to the order
  code. The CHAOS account has no [R] Crew manifest, and Wren3 cannot see the
  Archive. So whatever these pieces unlock does not reach the account that
  holds the manifest. This is where I think the chain breaks for a single
  caller.
- Bulletin 4, System info, Library (all five files), Radio Room 94 and 95,
  Lounge 28, 32, 45, VolaMUD (two rooms, generic).
- [S] File with ELIZA at 2 of 3 only repeats "Rows confirmed: 4 of 5".

Release command: COMMS.TXT gives `release QUEUE DESTINATION`. The queue file
names the queue out-1122 and the destination ground-1, held because
"INCIDENT REPORT NOT ON FILE". `release out-1122 ground-1` (tried in / and in
/var/spool/out) gives "release: command not found". I assume it unlocks once
the manifest is whole. `help` lists only ls, cat, cd, pwd, whoami, uname and
exit, so a player cannot tell that release will appear later.

Faults and unclear points in session B:
- Lag got worse in the manifest. With replies a step behind, "2" twice
  filled one blank and then opened another menu, and once a row showed a
  value before I had picked it. I had to send an empty read to resync.
- Message bodies wrap mid-word at 79 columns ("cab / in", "a / nd"), and the
  line after the break holds only the end of the word. Seen in almost every
  long post.
- Pager redraw: going past the first page prints the whole message again,
  and ORBIT_7 and ORBIT_3 [S] were drawn twice. Typing a message number
  ("46") inside the pager is read as keys, so it moves to the next message.
- The message list always starts at #28 in the Lounge (47 messages). There
  was no visible way to list 1-27.
- After logoff the BIOS screen goes back to "SOLAR BUS ONLY", even though the
  RTG is on and Diagnostics 100 says "Bus: RTG".
- The notebook showed lines I had not read on this account (suit log, hatch
  log, power log, capsule log). Either they come from the shared board state
  or from posts I only skimmed. It was not clear which.
- Where to use LADDER is guesswork. I tried Diagnostics and the file-area
  prompt before trying a CHAOS login. Lounge 39 ("I have made a place")
  is the only hint.
- Unfair: the last manifest blank has no visible source on the caller's own
  account, and nothing says where to look.

### Run 3

Finished: yes. Ending: option 1, "Send it" (the other two, "Why are you asking me to stop?" and "Leave it queued", not tried). About 95 tool calls this run, over Telnet.

Route: reconnect (session was gone), CHAOS visit (Archive messages 106-109, Bay 3 scene, Archive file area FRAG_Q, V, K, D, Library BROCHURE.TXT and COMMS.TXT, Mission Log 53-59, Garden Notes 64-70, Diagnostics 400), Goodbye, wren3, Crew manifest row 5, third blank option 1 (guard NOWHERE), file (S), ELIZA scene, Doors, NOWHERE shell, Diagnostics code, shell again, release, ending, News Wire bulletin 5.

Stuck points:
- The third ELIZA blank offered options after one CHAOS visit. I did not find a separate "rack log" or "prospectus" by name, so I cannot say which read unlocked it (candidates: Module 3 in Mission Log 57, rack in Garden Notes 64, Diagnostics 400, Archive).
- NOWHERE shell: `help` never lists `release`, before or after the manifest was whole. As guest the shell is read-only exploring. The way forward was COMMS.TXT section 6 plus Archive FRAG pieces (AUTH 1/4 to 4/4: NW1, 98, 44, 17), typed in Diagnostics as NW1-98-44-17 ('Host NOWHERE-1 is available'). After that `help` still showed no release. I then read var/spool/out/queue (HOLD: incident report not on file), incident.rpt (hold CLEARED), and `release out-1122 ground-1` worked. A first-time caller may not guess that `release` is hidden from help.

Faults and unclear points:
- Single-key sends with enter=false sometimes echo only the key and the screen arrives on the next call; once a bare `M` followed by `A` produced "Unknown command" on the main menu. Menu keys appear to act without ENTER, so a follow-up key can land on the wrong screen.
- Status panel on the main board still reads "CREW 4 ABOARD ALL WELL" and the Status bulletin is unchanged after the ending (intentional? It matches the news wire's last status line).
- Stray raw escape fragments in the capture (e.g. "[30m", "[46m", a replacement character in the Airlocks art) are likely my client's ANSI stripping, not necessarily the board.
- I made two parallel tool calls once by mistake (two Q keys); both worked.
- Archive message 109 and the Bay 3 recording both end mid-sentence ("tell them the"), which reads as intended.
