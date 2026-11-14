# Menu audit (slice 9)

Every screen, the keys it advertises, and who gets them. The rule: never show
an option that does not work for that caller on that transport. Checked
against the code on 2026-11-12. "Browser" and "Telnet" are the two
transports; "SysOp" is level 255 with the SYSOP flag.

Q20 UX (14 Nov 2026): the keys below follow the contract in `docs/PLATFORM_UX.md`.
A letter acts at once at every menu, list and pager (no ENTER). Q goes back one
level, X goes to the main menu, ? lists the keys, N and P are next and previous,
ENTER redraws a menu and never leaves it. Messages, bulletins, WHO, system info
and help open in the pager. Moved keys: P (page the SysOp) is O; P (post) is W;
R (read or list) is L; E and L in the message list (earlier, later) are P and N.
The tables below name the screen's keys; where they still show an old key the
code wins and PLATFORM_UX.md section 3 lists the change.

Hidden commands (typed but not advertised) are listed where they exist. They
are allowed: they never appear on screen.

## Login

| Keys | Who | Notes |
| --- | --- | --- |
| (Login: only) | everyone | Q5b: the front page is the brownout banner and `Login:`; there is no New and no Goodbye. `admin`/`admin` opens the ship console with a power timer; registered callers go straight to the board. A card without `POWER.INI` keeps the old N, L, G prompt. |
| CREW | ship console, after the RTG | The only place an account is made (handle, password, real name, location). ENTER at the handle or password returns to the console. Not in HELP and an unknown command before the RTG. |

## Main board

| Key | Shown when | Notes |
| --- | --- | --- |
| M, N, A, D, C, W, U, S, ?, G | always | W opens the node monitor (kick, M) for the SysOp. G on a read-only crew account logs off to the login prompt and keeps the line (Q2: what the visit found goes to the caller's own login). Q4: W lists ghosts too, with an Idle column; a ghost's node is "no caller on that node" for the monitor. |
| F | an area is open to the caller (an empty upload area counts in the browser) | |
| B | a bulletin is visible to the caller | |
| J | more than one message area can be joined | Q7: a conference with a `required_flag` (Mail: TYVANCE, `VISIT_TY`) is listed only for callers who hold it, from the account flag field or the story record. |
| O | a SysOp is online | Q20: was P. |
| R | the caller can save (not a crew login, not `admin`) and holds evidence about at least one crew row (`STORY/MANIFEST.INI`) | Q6. G with an unfinished manifest shows ELIZA's thanks (partial ending). |
| Z | SysOp only | Hidden for the SysOp: `USER`, `RELOAD`. |
| L | an own account (not a crew login, not `admin`) | Q12d: "Log in as another". Signs off to `Login:` and keeps the line, so a crew login after it reads the caller's own flags (the shell in ELIZA's menu needs `NOWHERE_OPEN` and `M_Z` from the own account). |
| C | always | Label "Chat: SysOp, callers" (Q20 UX round 2: was "Node chat"). A lone Q, QUIT, EXIT or /Q leaves the typed chat. Q12d: for a caller (not the SysOp) node chat opens as the scene `ELZ_TALK`: numbered replies, the memory-check reply only with `OVR` and `CURE`; "Say something else." opens the typed chat (keywords, eggs). A card without the scene opens the chat directly. |
| (hint line) | once per call, under the first main menu | Q20 UX round 2: "Press a letter. No ENTER needed. ? shows the keys, X comes back here." |
| (marker line) | the caller holds `SYSVW` | Q12d: "SYSOP view is on: every area is open to read." No command comes with it. |

## Messages

| Screen | Keys | Notes |
| --- | --- | --- |
| Area menu | L, N, W, Q; A when more than one area | Q20: R is L, P is W. W only where the caller can post. |
| Area list | number, ENTER | |
| Reading (any length) | N, P, R, T, Q, X; K, E for the author and SysOp; H when edited | Q20: every message opens in the pager. |
| Reading (long message, reader) | same keys in the status bar | k, e, h used to leave the reader for a message the caller cannot change; now they only act when offered, and k scrolls otherwise. **Fixed.** |
| Editor | /S /A /L /? /H /C /D n /R n | |

## Files

| Screen | Keys | Notes |
| --- | --- | --- |
| File menu | area numbers, Q | |
| File list | #, V #, Q; D # in the browser where downloads are on; U in the browser where uploads are on; N/P with more than one page | |
| Text and ANSI reader | SPACE B arrows S N/P Q | S plays at full speed on Telnet. A picture that paints over a row (a bare CR, Q8: Orbit 3) shows the covered row only while it plays; at 300 baud in the browser it can be read. |
| ZIP contents | SPACE B arrows N/P Q | |
| Image viewer | N P ESC, arrows, taps | Browser only; Telnet is told pictures open in the browser. |
| Music player | N P SPACE ESC, arrows, taps | Browser only, and only when the player is on the card. It lists the module's sample names that hold text. |
| Tune as text (`.MOD`) | SPACE B arrows N/P Q | Where the player cannot run (Telnet, or a browser with no player on the card): title, format, channels, orders, patterns and the sample names, in the reader. Other tune types (S3M, XM, IT, SID) still tell Telnet that music plays in the browser. |
| Sound cues | none | Browser only, sent by the board at the RTG switch (`CHIRP`), the brownout drop (`STATIC`) and a scene's `sound=` key. No menu option; Telnet gets nothing. |
| Files that cannot be viewed | | Browser: "D #". Telnet: "from the browser terminal". |
| Upload | description, CHOOSE FILE | Browser only. |

## ANSI gallery

| Key | Shown when | Notes |
| --- | --- | --- |
| A, F | always | |
| G | an ANSI area is open to the caller | |
| Q | always | |

## Eggs (Q11)

| Where | Words | Notes |
| --- | --- | --- |
| Login, main menu, door menu, Press ENTER | AT, ATZ, ATE0 and the other setup commands, +++, ATI0-9, ATH, ATD<number> | In no menu. At the login prompt only real commands count, never a name on the board. Writes nothing. |
| Main menu only | XYZZY, FORTUNE, /FORTUNE, POST, REBOOT | In no menu and not in ?. |
| Node chat | famous requests (STORY/SYSOP.INI keys) | ELIZA answers; no flag. |

## Doors

| Screen | Keys | Notes |
| --- | --- | --- |
| Door menu | one per door in the door table, Q | |
| Snake (Ty's) | W A S D (several at once), ENTER goes on, Q; after a game over ENTER plays again | In the door menu (key S) for every caller who is not the maintenance login and when the card has its title art. Saves nothing, so read-only accounts have it too. |
| VolaMUD | HELP lists LOOK, N/S/E/W, GO, TALK, BUY/SELL, INV, WEAR, TASK, BANK, SAY, FIGHT, WHO, STATS, QUIT | All handled. |

## SysOp

| Screen | Keys | Notes |
| --- | --- | --- |
| Console | U D W I L S R Q; F in the browser | L shows the event log, L <text> filters it. |
| Node chat | type to talk; /Q /W; /H for the SysOp | A bar on the last row shows who is here and the commands. Q4: ghosts with `chat=1` (ELIZA) are named in the bar and in /W for callers who can see them, and answer after a typing delay. |
| User editor list | #, handle, A, Q; ENTER and P with more than one page | |
| User record | 1-11 (11 = gold, shared by all doors), N, P, Q; D for other accounts | D used to show on your own record and then refuse. **Fixed.** |
| Node monitor | node #, M <node or ALL> text, ENTER | |
| Door settings | F handle count, V handle, Q | |
| Upload queue | A #, D #, Q | A and D only when something waits. |

## Manifest (Q6)

| Screen | Keys | Notes |
| --- | --- | --- |
| Rows | row number, S, N, Q | S files the manifest. The ELIZA row is listed only after the four crew rows are locked. |
| A row | 1 2 3, Q | A locked row offers no blank. |
| Pick list | option number, 0, Q | Only options the caller has evidence for. |
| Notebook | Q | Notes the caller has earned. |
| NOWHERE shell | final command | `release out-1122 ground-1` needs the full manifest (`M_Z`); otherwise it is `command not found`. With `M_Z` the verb with other arguments prints the usage line only, and `incident.rpt` exists. The door itself is listed only for a caller holding `NOWHERE_OPEN`, which the order code in Diagnostics grants (Q9). The `ENDING` scene follows. |

## Message areas (Q7)

| Key | Shown when | Notes |
| --- | --- | --- |
| W (write), R (reply) | the caller can save and the conference is not `read_only=1` (the SysOp may post anywhere) | Q7. Mission Log, Garden Notes and the mailboxes are read-only; Lounge, Spoilers, No Spoilers and Radio Room take posts. |
| B | a bulletin is visible; a `.ANS` bulletin shows its `.ASC` twin under `ASCII/` to a caller without ANSI | Q7. Bulletin 5 (News Wire) needs `END1`: listed only after the queue was sent (Q9). |
| U | always | Shows the last call of each account (`1998-12-08 03:47` as `08 Dec 03:47`). |

## Act 2 areas (Q8)

| Area | Shown when | Notes |
| --- | --- | --- |
| Files: Ship Logs, Records | the caller holds `CREW` (Ty's account, or carried from it) | Read-only areas; `.LOG .CFG .RPT .MEM` open in the text reader. |
| Files: Archive | `ACH` (the `CHAOS` account, or carried from it) | The four pieces of the order. |
| Files: Ty's Desktop | `TY_HOME`: on the `TYVANCE` account only, never carried | Orbit 9. |
| Messages: Mail: CMDR, Mail: GARDEN | `VISIT_CMDR`, `VISIT_GARDEN` (their logins, carried) | Read-only. |
| Messages: Archive | `ACH` | Read-only. |

## Milestones (Q12d)

| Area | Shown when | Notes |
| --- | --- | --- |
| Posts with `Needs: ACT2` / `Needs: ACT3` | the caller holds the flag (Ty's and Achterberg's visits) or `SYSVW` | Not listed, counted, scanned or shown in a thread before; a real SysOp sees them. |
| Files: Engineering | `SYSVW` | `GOLIVE.TXT` (ELIZA's password, item 14), `STANDING.TXT`. |
| Files: Commander's Desk | `CMDR_HOME`: the `CMDR` account only (or `SYSVW`) | `HANDBOOK.TXT`, chapter 9: reading it sets `CURE`. |
| Files: Mission Computer | `ELZ_HOME`: the `ELIZA` account only (or `SYSVW`) | `OPSLOG.TXT`. |
| Messages: Mail: ELIZA | `VISIT_ELIZA` (her login, carried) or `SYSVW` | Read-only. |
| Door: NOWHERE shell | the `ELIZA` account (`SHELL.INI` `account=`) and the caller's own `NOWHERE_OPEN` | Gone from every other menu. |
| SYSOP view | never | `SYSVW` opens no SysOp tool: `Z`, `USER`, `RELOAD`, the SysOp console conference, the SysOp page and every `SYSOP`, `READONLY`, `BACKROOM`, `VAULT` gate stay shut. |