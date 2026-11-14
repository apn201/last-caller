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
| CREW | ship console, after the RTG | The only place an account is made (handle, password, real name, location). ENTER at the handle or password returns to the console. Not in HELP and an unknown command before the RTG. Q20: the new account is logged in at once (no second `Login:`). |
| ACK> (SOURCE RTG) | ship console | Q20: one key, no ENTER (contract C5). The confirm key from `POWER.INI` switches; any other key aborts. |

## Main board

| Key | Shown when | Notes |
| --- | --- | --- |
| M, N, A, D, C, W, U, S, ?, G | always | W opens the node monitor (kick, M) for the SysOp. G on a read-only crew account logs off to the login prompt and keeps the line (Q2: what the visit found goes to the caller's own login); Q20: there it reads "Goodbye (to Login:)", and the crew login says once "This is a crew account, read-only. G goes back to Login: for your own account." (plus the Y line when Y works, and "Your crew manifest is on your own account."). Q4: W lists ghosts too, with an Idle column; a ghost's node is "no caller on that node" for the monitor. |
| F | an area is open to the caller (an empty upload area counts in the browser) | |
| B | a bulletin is visible to the caller | |
| J | more than one message area can be joined | Q7: a conference with a `required_flag` (Mail: TYVANCE, `VISIT_TY`) is listed only for callers who hold it, from the account flag field or the story record. |
| O | a SysOp is online | Q20: was P. |
| R | the caller can save (not a crew login, not `admin`) and holds evidence about at least one crew row (`STORY/MANIFEST.INI`) | Q6. G with an unfinished manifest shows ELIZA's thanks (partial ending). |
| Z | SysOp only | Hidden for the SysOp: `USER`, `RELOAD`. |
| L | an own account (not a crew login, not `admin`) | Q12d: "Log in as another". Signs off to `Login:` and keeps the line, so a crew login after it reads the caller's own flags (the shell in ELIZA's menu needs `NOWHERE_OPEN` and `M_Z` from the own account). Q20 (Addition 6): once a crew account has opened for this caller (`KNOWN_<HANDLE>` on the own account), L lists those accounts: a number (no ENTER) logs in without the password, A goes to `Login:` for any other handle, Q or X back. With none known L goes straight to `Login:`. Never on a crew account. |
| Y | a crew account, with the caller's own account signed in earlier on this line | Q18: "Back to <handle>", no password. |
| (notebook) | manifest, N | Q21: see the Manifest section. The crew logins that worked (Q20, Addition 6) are on the notebook's L page: "Logged in with 140374. L on the main menu opens it." |
| C | always | Label "Chat with the SysOp" for a caller, "Node chat" for the SysOp (Q20 UX round 3; round 2 tried "Chat: SysOp, callers", and testers looked for callers that the scene hides behind "Something else"). A lone Q, QUIT, EXIT or /Q leaves the typed chat. Q12d: for a caller (not the SysOp) node chat opens as the scene `ELZ_TALK`: numbered replies, the memory-check reply only with `OVR` and `CURE`; "Say something else." opens the typed chat (keywords, eggs). A card without the scene opens the chat directly. |
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
| Snake (Ty's) | W A S D and Q act at once (Q20); Q door menu, X main menu; after a game over ENTER plays again | In the door menu (key S) for every caller who is not the maintenance login and when the card has its title art. Saves nothing, so read-only accounts have it too. |
| Diagnostics | a query code, then ENTER; HELP or ?; Q, QUIT, EXIT, BYE door menu; X main menu (Q20 UX round 3) | Line door: the intro says "Enter a query code, or QUIT." |
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
| Notebook (notes) | 1 to 6 open, N next page, P previous page, S scenes seen, L logins, Q back to the rows, X main menu | Q21: what the board has shown the caller (a message, a file, a picture, a scene line, a page of ELIZA's, an answer of Diagnostics or the shell), one short line each with its source, in the order seen, six to a page. Saved as flags `N_<ID>` on the caller's own account. S shows only when a scene has been seen, L only when a crew login has worked, N and P only when there is a next or previous page. The notebook keeps what was shown and adds nothing. |
| Notebook (one note) | R show it again, N next, P previous, Q back to the list | The source, the line, and where to read it again (message: area and number; file: area and name). R is listed only for a note that can be replayed (a scene, a page of ELIZA's, a story picture) and only when the card still has it. |
| Scenes seen | 1 to 9 replay, N, P, Q | Scene titles from `title=` in each scene file, newest last. A replay shows the scene as it was, saves nothing and changes no flag; choices are shown and followed but set nothing; Q (or the end) comes back to this list. |
| Logins that worked | any key | The crew accounts that opened for this caller and their passwords (Q20, Addition 6). |
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