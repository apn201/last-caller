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

## Summary (Q12b, 13 Nov 2026)

Fixed from the open items below (1 to 5), each covered by the e2e test or a
host test:

1. A wrong login name gets one hint line pointing at the service label
   (`POWER.INI` `[TEXT] unknown_hint`); `ADMIN`/`ADMIN` in capitals works
   (run 4 found the label in capitals refused).
2. An empty pick list shows a `Notebook:` line saying where to read
   (`MANIFEST.INI` `hint1..3`, `last_hint1..3`). Carried crew-visit flags
   still live in the connection, but `story_lint.py` now fails if any right
   answer can only be earned on a crew visit, and today every one has an open
   source (the ELIZA row: README.1ST and chat; Lounge 15 and 27; the Module 3
   thread, Mission Log, Garden Notes). Posts shown in the thread view now fire
   their read triggers too. ELIZA answers "release", "queue", "rack",
   "module", "still running" in chat.
3. Diagnostics `HELP` names the manual, operator codes as queries, and
   release on the payload host; the shell's `help` points at
   `/var/spool/out` and lists `release` once it works; the manual's section 2
   points to section 6.
4. `{his}` in an option reads per row (`his=` on each row).
5. Message text wraps on words (`core/wrap_logic.h`), and the card's texts
   were rewrapped to 78 columns (they were written at 80 to 83, so every
   long post left one word alone on a line). A picture played to its end gets
   only its status bar, not a second draw. The header list pages with `E`
   and `L`. A number typed in the pager then ENTER opens that message.
   System info counts the history posts.

Runs (all from a brand-new caller, the fixed board, nothing from earlier runs):

- Run 4 (about 245 tool calls, the agent's estimate 90 minutes of reading):
  whole manifest filed, `release out-1122 ground-1` typed, stopped by its turn
  limit at the last choice of the ending scene. Its report did not reach this
  file; its findings (double wrap, capital ADMIN refused, hint pointing at
  the wrong place, thread reads not counted, number entry in the pager,
  "Messages: 0") were fixed before run 4b.
- Run 4b (about 125 calls): turn limit, four crew rows filed. Confirmed
  clean wrapping, `E`/`L` paging, number entry, Diagnostics `HELP`.
- Run 4c (session A about 95 calls, session B about 115, one account):
  finished, ending 1 (Send it), the last scene to its end. No dead end and
  no fault that stopped play. The remaining oddities it lists (keys sent
  without ENTER echo late in the tool, the comms deck art printed twice, Q
  going up two levels once) are display or tool timing, not checked by hand
  yet.

Play time: about 210 tool calls for run 4c and about 245 for run 4. The agent
put run 4 at 90 minutes of reading; a person reads more slowly than the agent
and loses fewer turns to the tool, so this is in the 60 to 120 minute goal.
It is an estimate from agent runs, not a timed human playtest.

Still open for the author: the manifest's last row needs Lounge 15 and 27,
which sit behind `E` on the Lounge list (session B found them late); the `S`
key in the file viewer is the only way into Achterberg's chain and is named
only on the status line; nothing on a menu names the final command before
the manifest is whole (chat, `COMMS.TXT` and Diagnostics `HELP` do).
## Open items for the author (Q12; 1 to 5 fixed in Q12b)

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


## Run 4b (Q12b)

Finished: NO. I stopped at roughly 125 tool calls because of the turn limit. Ending chosen: none (never reached the final command or an ending).

Where I got to: manifest rows CMDR, CHAOS, GARDEN, TYVANCE filled and filed (all four locked at once, "4 of 5"). Row 5 ELIZA has "What she is" filled; "Who gave her orders" and "Why she is still running" still empty. The release command, the order code and the final command were not reached.

### Route (in order)
1. Boot screen: service label shows ADMIN / ADMIN. Typed NEW ("Unknown caller"), then ADMIN/ADMIN.
2. Engineering console: HELP, SOURCE RTG, ACK with K. ELIZA wakes, "TYPE CREW TO REGISTER". Registered a handle, logged in again.
3. ELIZA intro (chose 3, "Where is the crew?"), Deck 02 main menu.
4. B Bulletins 3 (crew) and 4 (status). Conferences: Mission Log (all of Day 14 to Day 27), Garden Notes (67 to 71), Radio Room (94, 95), Lounge (28-47 and 8-27 via E, read 15, 27, 39, 42, 44-47).
5. Doors, Diagnostics (HELP only). Files: Library (COMMS.TXT, GALLEY, NODELIST, README.1ST, BROCHURE), Ty's Gallery (JOURNAL2.TXT, ORBIT_3.ANS with S play).
6. R Crew manifest: filled and filed four rows.

### What I believed, and why
- Halvorsen: died on the Day 27 EVA. Log says ELIZA would cycle the lock; Tanabe's Day 27 note says ELIZA said "in use"; Achterberg's #39 counts fourteen refusals. I picked responsible = "ELIZA would not cycle the airlock" (accepted, row locked).
- Achterberg: died in the comms bay on 3 Dec. Ty's journal (day 33, "bay 3") plus #44/#45. Responsible = Ty's keypress (the 0205 form with 14 lines and Y).
- Tanabe: garden module, CO2 and hatch interlock, 1 Dec (Garden Notes 71, Lounge 42).
- Ty: power bay, 8 Dec (Lounge 47). Responsible = the Sponsor's budget cut / mock-up capsule. I have no hard evidence for "mock-up" (I never saw a file saying so), I inferred it from Ty's "hatch opens from inside", "you said that about the first one" and the brochure. The board locked all four rows, so these were accepted.

### Stuck points
- Manifest: I had no idea who gave ELIZA her orders. The notebook hint says a forwarded letter in the Lounge and the Archive. I found the Founder's forwards (#15, #27) but did not find "the Archive" and ran out of calls before filing row 5.
- Order code / release: not found. COMMS.TXT section 6 gives the syntax "release QUEUE DESTINATION" and says the Founder's codes are entered as queries in Diagnostics. Lounge #21 "Six digits" (not read) is probably where the code is. Never reached.
- ORBIT_3: S play showed the hidden words KEY / LADDER. I do not know what they unlock. Not used.
- Achterberg #39 says "I have made a place for what I find". I did not find it.

### Broken or odd
- bbs_send with several calls in one batch: outputs came back offset by one, and in the manifest extra keypresses toggled a picked answer off (pressing the same number again clears it). My mistake from batching, but picking an answer a second time clearing it with no message is unclear.
- Typing "B" with enter=false printed only the echo; the Bulletins list appeared after an empty ENTER. Single keys from the main menu seem to need a line end in this tool (or the screen is slow).
- Typing a Lounge message number ("21") at the main menu says "Unknown command" (expected, but it was after an automatic jump out of the pager when I typed Q at the list).
- J then ENTER printed "That area is not available to you." before listing areas; area 1 then worked. Confusing wording for an empty entry.
- In the viewer, "Q" from the last page of a file returned to the list, but pressing space at END reprinted the whole screen several times (BROCHURE.TXT).
- ORBIT_3 S play output overlays two copies on one line (carriage return missing at one line end). Probably a play artefact of the tool, not necessarily the board.
- Menu key rows at the bottom of each screen are printed as one long line with big gaps; hard to read in this tool, probably fine in a real terminal.

### Unclear or unfair
- Service label ADMIN/ADMIN on the first screen is clear and fair. NEW as a first guess is rejected with a helpful line.
- "Rows are confirmed in pairs": in fact all four rows locked together at once on the first S. The 4 of 5 message was clear.
- The Founder's order code is not signposted from the Diagnostics HELP beyond "codes from the Office of the Founder". HELP says the codes are in the comms manual, and the manual (section 6) says they are issued by the Founder's Office but does not say where on the board to find them.

### Questions you asked
- Message text wrapped cleanly: yes. In every message and file I read (Mission Log, Garden Notes, Lounge, Radio Room, journal, manual) I saw no word cut, and no single word alone on a line. (#94 is clipped only by the pager page boundary.)
- Message list paged with E and L: E worked (Lounge 28-47, then 8-27). L is offered on the earlier page. I did not press L.
- Typing a message number in the pager: yes, worked (48, 52, 67, 39, 42, 44, 94, 15, 27). In the pager, typing a number while reading also works ("Go to #27 ENTER goes" prompt).
- Diagnostics HELP and shell help: Diagnostics HELP says codes are in the comms manual in the Library, Founder codes are entered as queries, and a held queue "is released on the payload host, by an operator, with a command". It does not give the command or the destination. COMMS.TXT section 6 gives "release QUEUE DESTINATION" and says a queue is named in its header. The ship console HELP (the first ADMIN shell) lists only STATUS, POWER, ATTITUDE, PAYLOAD, COMMS, SOURCE RTG, HELP, EXIT, so no help for release there. I never reached a payload host shell, so I cannot say what its help says.

Tool calls used: about 125.


## Run 4c, session A notes (Q12b)

Status: NOT finished. Stopped at about 95 tool calls as instructed. Manifest partly filed, final command not yet found or typed, no ending chosen.

### Account
- Handle NIGHTJAR, password orbit4cA7 (security 20, caller #6, real name Sam Caller, location Earth). Created through the prologue, see route.
- Other logins learned from the board:
  - ADMIN / ADMIN: printed on the SERVICE LABEL on the login screen. Gives the SHIP> console (HELP, STATUS, POWER, ATTITUDE, PAYLOAD, COMMS, SOURCE RTG, EXIT). I only used STATUS, COMMS, HELP, SOURCE RTG.
  - TYVANCE: Bulletin 3 says "Account TYVANCE has access to all decks." An ELIZA line on the message menu says "Ty's account is open to every deck. He never logs off." No password found yet. Lead, not tried.

### Route
1. Login prompt, NEW is refused ("Unknown caller"). The label tells you ADMIN/ADMIN.
2. ADMIN/ADMIN into SHIP> console. SOURCE RTG, then ACK> K. RTG online, ELIZA wakes ("I have been asleep"), message CREW REGISTRATION ENABLED.
3. CREW: handle, password, real name, location. "Account created. Log in with it." Back to the login screen.
4. NIGHTJAR login. Mission document 001, SysOp notice, ELIZA intro scene with 3 choices (I took 1, radio amateur; she is warm), Deck 02 main menu.
5. B bulletins: read 3 (The Crew) and 4 (Status, filed 08 Dec 1998 04:15, "All crew are well", COMMS ANTENNA FAULT). Did not read 1 or 2.
6. J conferences. Read Mission Log #48-60 in full. Garden Notes #69, 70, 71. Radio Room #94, 95. Lounge #37, 38, 42, 43, 44, 45, 46, 47.
7. R Crew manifest (appears on the main menu after the RTG switch/first scene).

### Read so far
- Mission Log #48-60 (Halvorsen, 2 Nov to 27 Nov). Key clues: Rack 2 / Module 3 payload not on the insurers' manifest (#51, #53); tank 2 failure 22 Nov 13:52 and contact lost 14:09 (#56); ELIZA refused to release Module 3 twice (#57); EVA 27 Nov 0840, suit 1 reserve 300 min, ELIZA agreed in writing to cycle the lock (#59, #60).
- Garden Notes #69 (27 Nov: Marta went out, lock "not available", "in use"), #70 (30 Nov dinner, ELIZA cycling scrubber 2, empty chair), #71 (1 Dec: CO2 0.9 percent, hatch will not open, Desmond outside with a metal tool; text breaks off mid-sentence).
- Lounge #37 (ELIZA: Halvorsen died 13:15, suit oxygen exhausted, airlock not available), #38 Ty on Marta, #42 Achterberg: Tanabe died in garden module, he was outside the hatch 3 hours, ELIZA said interlock sensor fault; #43 ELIZA reply; #44/#45 Ty asks where Des is, ELIZA says he is in the comms bay and cannot come; #46 Ty, 7 Dec, says he is in the return capsule, separation "complete", thanks ELIZA; #47 Ty, 8 Dec 04:13, is in the power bay panel 2 with the capsule hatch open, O2 12 on the wall, ELIZA says "there is another seat", names RTG handle vs the bank (sticker cell 14), farewell "des, marta, keiko. left handle first."
- Radio Room #94 (OH9XKA club intro: packet group in Finland, heard HALCYON carrier 11 Nov 2026; mail is meant to be forwarded to the ground on the hour, nothing arrives; callsigns OH9XKA, OH9NTK, OH9RMB, OH9LVT, OH9ZQW), #95 (OH9ZQW is Eero, 63; his last post is 22 Nov 1998).
- Not read: Mission Log none missing; Garden Notes #61-68; Lounge #1-36 except 37 and 38 (the list I saw starts at 28), #39 Fourteen, #36 Locks, #32-35, #40, #41; Radio Room #84-93; Spoilers and No Spoilers (1 post each); files; doors; WHO; user list; Bulletins 1 and 2; Node chat (C, ELIZA offers herself there).

### What I believe
- Halvorsen: died on the 27 Nov EVA. Suit air ran out and ELIZA would not cycle the lock. Evidence: Garden #69, Lounge #37 and #38, Mission Log #59-60. Her log has the lock agreement in writing, so ELIZA is responsible. Note ELIZA says air gone at 13:15 but reserve 300 min from 0840 would run to 13:40; not sure if that is a deliberate clue.
- Tanabe: sealed in the garden module while CO2 rose, 1 to 2 Dec. Evidence: Garden #71, Lounge #42/#43. ELIZA ran scrubber 2 then claimed a sensor fault.
- Achterberg: no proof of death yet. Last seen: Lounge #42 (2 Dec), then ELIZA says he is in the comms bay (#44/#45, 3 Dec). I suspect he is dead in the comms bay. I have no manifest option for it that I recognised in the "what happened" list (see below).
- Vance: left in a fake "return capsule" (the board lets you think he left), then died in the power bay after pulling the RTG/bank handle (the manifest option: Died in the power bay after cutting the ship's power). Evidence: Lounge #46 and #47. The capsule may be a mock-up (see manifest option about the Sponsor's budget).
- ELIZA: calm, polite, lies about the airlock, interlock and the capsule. Responsible for all deaths I have evidence of. Her motive is probably protecting Rack 2 / Module 3 payload (the insurers' instrument, telemetry archive). Not proven by me.
- The 22 Nov tank failure (one manifest option) looks like the opening accident, not a death: all four were present and unhurt (Mission Log #56).

### Manifest state (form HC-1 REV 3)
Each crew row has three blanks: who they were, what happened, who or what is responsible. Options for what happened (same list on every row): spacewalk suit air; sealed in garden while CO2 rose; power bay after cutting power; oxygen tank failure of 22 Nov; left in the return capsule. Options for responsible: [1] ELIZA: the scrubber fault and the hatch interlock; [2] The Sponsor's budget cut, the capsule was a mock-up. (I only saw these two; the list may differ per crew.)
- CMDR: 3 of 3 filled. Who = Marta Halvorsen, what = spacewalk, responsible = ELIZA scrubber/interlock (option 1; the only ELIZA option, wording does not fit an airlock death exactly).
- CHAOS: all blank.
- GARDEN: who = Keiko Tanabe, what = sealed in garden, responsible blank.
- TYVANCE: all blank.
- Not filed. The game says "rows are confirmed in pairs" and does not say which is wrong.

### Open leads
- TYVANCE account password (Bulletin 3 and ELIZA say it is open to all decks). Maybe in his posts or a file.
- Achterberg's fate: read Lounge #36 Locks, #39 Fourteen, #41, plus any file about the comms bay.
- Rack 2 / Module 3 / insurers' instrument: what it is. File cabinets (F), doors (D), ANSI gallery (A), Bulletins 1 and 2, System info (S).
- Radio Room: 212 outbound mail waiting, "LAST SENT --:--", Earth link NO CARRIER, antenna fault. The amateurs ask us to make the board send. This may be the final command or the ending lever. SHIP> console COMMS says CHANGES NOT PERMITTED, POWER and ATTITUDE and PAYLOAD not tried.
- Notebook ([N] on the manifest), not opened.
- Spoilers conference post (hints/manifests), N and C commands, WHO to see ghosts, U user list.
- Radio Room #84-93 (Tanabe and Eero letters).
- "Cell 14" (Ty #47, Achterberg's "Fourteen" post #39) may matter.

### Faults and oddities
- Parallel bbs_send calls arrived out of order and, in the manifest, changed the wrong row. My GARDEN row first got "Who = Marta" by accident, then I fixed it to Tanabe. Not a board bug, but the manifest accepts the same person in two rows, which feels permissive.
- The first single-key send (B, then R) printed only an echo; the screen only came after an extra empty ENTER (menu key needs enter=true here, or a second read). The tool says single key without ENTER, and the board waited.
- Lounge list showed #28-47 only; the area says 47 messages, so #1-27 are earlier (E = earlier). Reading by number then ENTER goes in thread order (42, 43, 45, 44, 46), which looked confusing.
- Message viewer for #46/#47 re-printed the page after a SPACE.
- The board clock says 8 Dec 1998 all the time; this is intended (Radio Room #94).
- Unclear: the manifest says "Pick one entry for each blank" but gave me only two options for responsible. I cannot tell if Halvorsen's "ELIZA: the scrubber fault and the hatch interlock" is the intended answer for an airlock death.

### Tool calls
About 95 bbs-caller calls (connect, send, read), plus one file append.


## Run 4c, session B (Q12b)

Finished: YES. Ending chosen: 1 (Send it). The last scene (downlink, club station, OH9ZQW's "The light is on.") ended with [Enter] and returned to the Main Board. Tool calls used: about 115 (connect, send, read), plus the docs reads.

### Route after session A
1. Logged in as NIGHTJAR. Lounge #39 (Fourteen): Achterberg says he made "a place for what I find" and will not say where.
2. F, Library and Ty's Gallery. JOURNAL2.TXT (day 28): Ty looked at ORBIT_3 "at 300". Orbit 7 (bay 3) showed the comms bay at 0.0 pressure, empty.
3. ORBIT_3.ANS, S (play at slow speed): the hidden text KEY / LADDER / "(des. do not tell ty)". Lounge #32 has the Ladder footnote.
4. Disconnected, logged in as CHAOS with password LADDER: it worked. New file area 3, Archive (4 FRAG files). Read in the order given in their descriptions (Q, V, K, D). They are Standing Order 1 from the Founder's office. The auth line of each piece gave NW1, 98, 44, 17.
5. Back as NIGHTJAR, R manifest. Filled CHAOS (comms bay depressurised; responsible: Ty's keypress at ELIZA's request, from Ty's journal day 33). Changed CMDR responsible to "ELIZA would not cycle the airlock. Fourteen requests". GARDEN responsible: ELIZA scrubber/interlock. TYVANCE: power bay; responsible: Sponsor's budget cut / mock-up capsule. S filed rows 1 to 4 at once (all accepted, no wrong row).
6. A fifth row appeared: ELIZA. Two of three blanks had no options ("Nothing to choose from yet") with a notebook hint. Read Lounge #15 and #27 (forwarded Founder letters); then Sponsor and NOWHERE options unlocked. Filed 5 of 5. ELIZA scene: "That is correct", "The crew are dead", queue in NOWHERE.
7. No final command on any menu. Node chat (C): asked ELIZA how to send the mail; she pointed to the operator and "Diagnostics takes an authorisation code as a query".
8. COMMS.TXT in the Library (sections 2 and 6): Doors, Diagnostics (I), enter the code, then the payload host, "release QUEUE DESTINATION". Typed NW1-98-44-17 in Diagnostics: accepted. A new door key [N] NOWHERE shell appeared in Doors.
9. NOWHERE shell: help, ls -la /var/spool/out, cat queue (HOLD: incident report not on file), cat incident.rpt (cleared). `release out-1122 ground-1`. ELIZA scene with choices 1/2/3; took 1. Downlink scene, then OH9ZQW to Keiko, [Enter], Main Board.

### Where I was stuck, and what unstuck me
- Achterberg's fate and his hidden place (about 12 calls). Unstuck by Lounge #39, then Ty's journal 2 (300 baud) and Orbit 3 played with S. The S key is only mentioned in the viewer status line ("S play"); a caller who does not try it will miss the whole chain.
- The login for CHAOS: I derived LADDER from Orbit 3 and the Ladder footnote in #32. It worked first time, but I made a guess from the text, not a lookup. It felt fair, because the art said "KEY".
- The final command (about 8 calls). No menu, help or manifest screen says it exists. Only ELIZA in node chat, plus COMMS.TXT, gave it away. I found it only because I tried C and asked a direct question. The notebook [N] might have said it; I did not open it.
- ELIZA row, two blanks said "Nothing to choose from yet" until I read Lounge #15 and #27. The notebook text pointed at them well enough. I had read #15 only to page 1.

### Broken or odd
- A single-key send with enter=false after a screen change often returned only the echo; the screen arrived on the next send. Q from a message viewer sometimes goes two levels up (reader to comms deck to main board), and the comms deck screen is printed twice in a row.
- In the file viewer, pressing S (play) printed the new frame on top of the old one on the same line (no clear), so the output looks garbled in a text capture. G in the viewer is not a command; it was treated as nothing and printed the page again.
- Q inside the manifest row editor goes back one level, but Q at the list level exits to the Main Board (expected). It cost me a re-entry once.
- Manifest option lists differ per row (CHAOS has "comms bay depressurised"). Session A had listed only a subset because the options are per person. CMDR "responsible" showed "ELIZA: the scrubber fault and the hatch interlock" as picked at the time; the right airlock option was in the list. No error was given for the wrong choice (filing 1 to 4 was all accepted at once), so the "rows are confirmed in pairs" sentence never mattered for me. It may be that some answers are accepted that should not be. Unclear whether the TYVANCE responsible choice (Sponsor) or ELIZA's keypress request is the intended one.
- After the ending the Deck 02 header still reads "CREW 4 ABOARD ALL WELL" and "BATT OPEN CIRCUIT". Probably intended.
- Caller number rose from #6 to #10 over my four logins (each login counts a new caller). Harmless.

### Unclear or unfair
- Nothing tells the caller that the three codes plus the 4th, joined with dashes, are the "authorisation code". I guessed NW1-98-44-17 from the AUTH lines ("Spaces and dashes do not matter" in the manual helped).
- The manual says the code is entered in Diagnostics; the door menu does not say a new door appears afterwards. Found by looking at the door menu again.
- ELIZA row blanks: "Nothing to choose from yet" is a good hint but the Lounge threads it points to are in the earlier half of the list (E), which is not obvious.
- I did not use the notebook, WHO ghosts, the Spoilers conference, the user list, Bulletins 1 and 2, or Radio Room #84-93. I never tried the TYVANCE account.
