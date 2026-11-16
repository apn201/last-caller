# Platform UX: audit, contract, port

Q20 part B. The engine came from TinyBBS and is a pile of pieces that did not
quite fit, like a spaceship. To a 1990s caller that is period correct. To a
caller today it is friction. This file records what was there (the audit),
what the engine promises now (the contract) and what to carry back into the
TinyBBS repository (the port). The look, the ANSI art and the voice stay.

Contents: 1 Audit (before Q20). 2 Contract. 3 Moved keys. 4 Port to TinyBBS.

## 1. Audit (state before Q20, checked in the code on 14 Nov 2026)

"Key" means one character; "line" means characters plus ENTER. Telnet and the
browser use the same board code, so the table holds for both. The browser
added one difference: it sends whole lines, except in the reader and the
viewers, where it sends every key.

### 1.1 Screens

| Screen | Input today | ENTER does | Q does | ? does | Text shown how | Leave by |
| --- | --- | --- | --- | --- | --- | --- |
| Login (`Login:`) | line | empty at a new-account prompt: cancel | n/a | n/a | art, then prompt | G on the old N/L/G prompt, close |
| Login choice (old cards) | line, one letter | nothing (asks again) | n/a | n/a | art | G |
| After login | line | "Press ENTER for Main Board" | n/a | n/a | bulletins in place | ENTER |
| Main board | line | redraws the menu | unknown command | help line, then "Press ENTER" | menu art once, then title and options | G |
| Bulletin (one) | line | "Press ENTER", returns to main | n/a | n/a | in place | ENTER |
| Bulletin list | line, number | back | back | none | in place | ENTER or Q |
| Who, system info, page SysOp | line | "Press ENTER", returns to main | n/a | n/a | in place | ENTER |
| User list | line | next page; at the last page it redraws | to main | none | in place | Q |
| Message menu | line | back to main | back to main | "Use R, N, P or Q." | menu | Q or ENTER |
| Area list | line, number | stay in this area | none | none | in place | ENTER or number |
| Message list | line, number | first new message | back | none | in place | Q |
| A short message (20 rows or fewer) | line | next message | back to area menu | none | in place, one prompt line of keys | Q |
| A long message | key, immediate | next page, at the end next message | back to area menu | none | pager | Q |
| Editor | line, text | adds an empty line | none | `/?` | prompt `NN:` | `/S` or `/A` |
| Yes/no (kill, private) | line | no | n/a | n/a | prompt | Y or N, then ENTER |
| File cabinets | line, number | back | back | none | list | Q or ENTER |
| File list | line | back | back one folder, then to the cabinets | none | list | Q or ENTER |
| Text, ANSI, ZIP viewer | key, immediate | next page | back | none | pager | Q |
| Image, music (browser) | key | none | ESC, N, P | none | viewer | ESC |
| ANSI gallery | line | back | back | none | menu | Q or ENTER |
| Door menu | line | back | back | none | menu | Q or ENTER |
| Node chat | line, text | empty line, redraws prompt | sent as text | none | scroll region and bar | `/Q` |
| ELIZA scene in chat | key (Q, digits), no ENTER | nothing (G1) | Q skips the scene; a number answers (after 1.5 s) | none | typed out by the scene | the scene ends, main menu |
| Manifest | line | rows view: back | back | none | list | Q |
| Diagnostics, ship console, shell, MUD | line, words | varies | QUIT or Q (varies per door) | HELP | in place | QUIT, Q, EXIT |
| Snake | key, several at once | plays again | quits | none | board | Q |
| SysOp console and editors | line | back or keeps | back | none | in place | Q or ENTER |

### 1.2 Numbered inconsistencies

Severity: S = severe (a caller is stuck or acts wrongly), M = moderate (a
caller is unsure), L = low (polish).

| # | Inconsistency | Where | Sev |
| --- | --- | --- | --- |
| U1 | ENTER means "back" in menus (message, file, door, gallery, SysOp), "next" in the message reader and the user list, "continue" at "Press ENTER", "first new" in the message list, "stay" in the area list, "skip" in a scene and "do nothing" in chat. | everywhere | S |
| U2 | A letter acts at once in the pager, the viewers and Snake, and needs ENTER in every menu. Nothing on the screen says which. | everywhere | S |
| U3 | P is Page SysOp (main), Post (message menu), Previous (message reader, file list, user list, pager). | main, messages, files | S |
| U4 | N is New scan (main), Read new (message menu), Next (readers, file list), Next page. | main, messages, files | M |
| U5 | R is Crew manifest (main), Read/list (message menu), Reply (reader), Reload (SysOp console). | main, messages, SysOp | S |
| U6 | L is Log in as another (main), List (message menu, file list), Later (message list), Logs (SysOp), Login (old login prompt). | main, messages, files, SysOp | M |
| U7 | E is Earlier in the message list and Edit in the reader; L is Later there. Earlier and later are P and N in every other list. | message list | M |
| U8 | U is User list (main), Upload (file list), Users (SysOp). D is Doors (main), Download (file list), Delete (user editor). | main, files, SysOp | M |
| U9 | S is System info (main), Slow replay (pager), Save (`/S`), Snake (doors), Stats (SysOp). | everywhere | M |
| U10 | Q backs out one level only. There is no key that returns to the main menu from deep inside (cabinet, folder, message). At the main menu Q is "Unknown command". | everywhere | S |
| U11 | ? is help only on the main board. Message menu answers "Use R, N, P or Q." The main board's error says "Type ? for help" in menus where ? does nothing. | most menus | M |
| U12 | Short documents (messages up to 20 rows, bulletins, who, system info, help) print in place with their own prompt line. Long ones open in the pager with different keys and a different footer. The keys change with the length of the text. | messages, bulletins, lists | S |
| U13 | Three different "continue" prompts: "Press ENTER:", "Press ENTER for Main Board:", the prompt of the egg handler. | main, login | M |
| U14 | Yes/no needs Y plus ENTER. Deleting an account needs the word YES. A kill needs `[y/N]` plus ENTER. | messages, SysOp | M |
| U15 | A scene drops every key typed while it types out (they are neither echoed nor kept) and ends at the main menu from wherever it started, also from a bulletin menu. | scenes | S |
| U16 | Chat is left with `/Q`. A plain Q is sent to the others as text. In the ELIZA scene "Q" leaves. The bar says `/Q quit` only on an ANSI screen. | chat, scenes | M |
| U17 | Doors say nothing about how to leave. Diagnostics, MUD and Snake take Q or QUIT, the shell takes `exit`, `logout`, `quit` or `bye`, the ship console has its own word. A caller typed I and R into Diagnostics. | doors | S |
| U18 | Numbers: a bare number opens a file or a message, but `V 3`, `D3`, `D 3` also work in the file list and nothing says so; digits typed in the message pager wait for ENTER and "any other key cancels". | files, messages | M |
| U19 | The prompts do not agree: "Main Board Command?", "Message command?", "File command?", "Door command?", "Gallery command?", "SysOp command?", "File area?", "Area #", "Bulletin #". | all menus | L |
| U20 | Menus draw their art on first visit and a bare title afterwards, so the same menu looks different the second time. | all menus | L |
| U21 | Error replies differ: "Use R, N, P or Q.", "Choose an area number, or Q.", "Try A, F, or Q.", "Unknown command. Type ? for help." | all menus | L |
| U22 | Escape does something only in the browser's pager and viewers (it is Q there). In every menu it does nothing. Backspace is not sent in key mode. | browser | M |
| U23 | After `R` in the area list the line "Area:" can name another conference than the list just shown (Anna's report). | messages | M |
| U24 | A Telnet client that sends lines (ENTER after each letter) and one that sends keys both work in the pager, but only the first works in menus. | Telnet | M |
| U25 | The slash commands of the editor (`/S /A /L /? /H /C /D /R`) are a third convention, next to letters and words. Fine for typing mode, but the footer does not say it. | editor | L |
| U26 | Hidden words (AT..., XYZZY, FORTUNE, POST, REBOOT) are lines in a menu that otherwise takes keys. | main, doors | L |

### 1.3 Letters by account kind (Q20 Addition 5, checked in the code on 14 Nov 2026)

Four kinds: own (a caller's account), crew (a read-only story account: TYVANCE,
CMDR, CHAOS, GARDEN, ELIZA), admin (the ship's maintenance login) and SysOp.
A letter never means two things across kinds; only whether it is shown changes.

| Screen | Key | Own | Crew | Admin | SysOp | Why it differs |
| --- | --- | --- | --- | --- | --- | --- |
| Main | L Log in as another | yes (a list once crew accounts have opened) | no | no menu | yes | A crew account is shared by every caller; G is its way back |
| Main | Y Back to own | no | when the own account signed in earlier on the line | no menu | no | Only a crew visit has an own account to return to |
| Main | G | Goodbye (hangs up) | "Goodbye (to Login:)" (keeps the line) | no menu (EXIT at SHIP>) | Goodbye | The crew G keeps the line so the visit's finds go to the own account; the label says so |
| Main | R Crew manifest | with evidence | no (the crew login says the manifest is on the own account) | no menu | with evidence | A read-only account cannot file |
| Main | C | "Chat with the SysOp" | same | no menu | "Node chat" | The SysOp has no ELIZA scene before the chat |
| Main | O Page SysOp | when a SysOp is on | same | no menu | no | |
| Main | Z SysOp console | no | no | no | yes | |
| Messages | W write, R reply, K kill, E edit | where the area takes posts | no (W says why) | no menu | yes, everywhere | A read-only account saves nothing |
| Files | U upload | browser, where uploads are on | no | no menu | yes | same |
| Doors | doors that save (manifest, MUD) | yes | no | no menu | yes | `saves` in the door table |
| SHIP> | words (HELP, STATUS, SOURCE RTG, CREW, EXIT) | no | no | yes | no | The prologue console, a line door |

Nothing found here gives one letter two meanings. The open item was L and G on
crew accounts (Addition 5 item 2), changed in ee6db49 and 916f58c.

## 2. Contract (what every screen promises, Telnet and browser alike)

The same text is in `.kiro/steering/interaction.md`.

**C1. Keys and lines, one rule.** At a menu, a list, a pager or a yes/no
question, a letter acts at once. No ENTER. At a prompt that wants text or a
number, ENTER ends the line, and the prompt says "number, ENTER" or "ENTER
sends". Digits start a number: they are never a command, and ENTER ends them.
A menu prompt always names itself `keys:` and never says "ENTER" to leave.

**C2. Global keys.** On every key screen, and never inside text entry:

| Key | Meaning |
| --- | --- |
| Q | back one level |
| X | main menu, from anywhere |
| ? | the keys of this screen (one in-world line, then the screen again) |
| N | next (page, message, file page, user page) |
| P | previous |
| SPACE | next page, at the end next item |
| ENTER | the screen's default forward step (next page, next message, first new); in a menu it draws the menu again. It never leaves a level. |
| ESC | the same as Q (browser, where the key arrives; Telnet clients that send ESC alone) |
| B | back one page, in the pager |

**C3. Same kind of action, same key.** One letter, one action, in every menu
that has it. L list or read, W write a post, R reply, N next or new, P
previous, O page the operator, K kill, E edit, H history, T thread, D
download, V view, U upload. Where a screen would need a second meaning for a
letter, the odd action moves to another letter (section 3).

**C4. One pager.** Every text the caller reads goes through the pager: a
message of one line or of a hundred, a bulletin, a file, WHO, the stats, the
help. The footer is the same: title, rows, `SPACE next  B back  N P  Q quit  ? keys`
(a screen adds its own keys in front: `R reply  T thread`). The pager does not
change with the length of the text. Menus and lists of options are not
documents: they are screens of keys.

**C5. Yes or no is one key.** `[Y/N]`, no ENTER. Destructive confirmations that
need a word (delete an account) keep the word and say "type YES, then ENTER".

**C6. Continue is any key.** The words "Press ENTER" are gone. The prompt is
"Press any key" and a key does it (ENTER too).

**C7. A scene or a timed event never lands a key on a prompt it was not meant
for, and never cuts a line.** Keys typed while a scene types out are not
echoed and are dropped when it ends, and one grey line under the prompt says
so: "(Keys typed during the scene were not used.)" (Q22; before, they were
played back and arrived at the next prompt as a surprise). G1: **only Q skips a scene.** ENTER
and other keys do not flush the typing or end a pause. Q jumps to the end and still grants the
scene's flags, its then= target and its notebook lines; it stops at a numbered choice, which the
caller still answers. Each scene opens with one dim line, "Q skips". A numbered choice and an
[any key] wait take keys only 1.5 s after they appear (earlier keys are dropped, not queued). At
scene start the half-typed line and any buffered keys are dropped. A lone Q waits the hotkey
moment, so a word typed ahead does not skip. A timed event (a page, a ghost, a
glitch) waits for the screen to be at a prompt, as before, and never writes
into the middle of a line the caller typed. A scene started from a menu
returns the caller to that menu's level, not the top.

**C8. How a chat or a door is left.** Every chat and door says how on entry,
in the room's own voice, and takes Q (in a typing screen `/Q`, for the same
reason slash commands exist in the editor) and the words QUIT and EXIT.
Doors that read words take the same three. HELP and ? list the commands.

**C9. The browser adds buttons, never rules.** A button sends the same key a
keyboard would. Keys arrive one at a time in a menu, as in the pager.

**C10. Hidden words.** A word typed at a menu (AT..., XYZZY, FORTUNE) still
works when it arrives in one go (a paste, a tool, a Telnet client in line
mode), and when the caller starts it with a slash: `/ATZ`. Single letters act
at once.

## 3. Moved keys

Filled in as the build goes on. Format: old, new, where, why, which story or
tool text changed.

| Screen | Old | New | Why |
| --- | --- | --- | --- |
| Main | P page the SysOp | O | P is previous everywhere else (U3) |
| Message menu | P post | W | same (U3); W is write |
| Message menu | R read or list | L | R is reply and the crew manifest (U5) |
| Message list | E earlier, L later | P, N | previous and next are P and N in every list (U7) |
| Main, doors, files, SysOp | ENTER leaves | ENTER redraws, Q leaves | U1 |
| Main, any pager | (none) | X main menu, ? keys | U10, U11 |
| All "Press ENTER" | ENTER | any key | U13 |
| Yes or no | y or n, then ENTER | one key | U14 |
| File list | V #, D # | V and D ask for the number; a bare number still opens | U18 |
| Chat | /Q only | /Q, and Q, QUIT, EXIT as the whole line | U16 |
| Shell | exit, logout, quit, bye | and q | U17 |

Not moved, on purpose: R at the main menu stays the crew manifest (the story
names the manifest, not the key, but the game is built around it); the user
list (U) and upload (U) never share a screen; D for doors and D for download
likewise; S is system info at the main menu and slow replay in the pager. The
story text never names a key that moved, and `tools/story_lint.py` checks it
(`check_keys`).

## 4. Port to TinyBBS

Kept current after every round (Addition 2). Do not edit `../TinyBBS` in Q20;
the port is its own item run inside that repository. "Engine" means port it;
"HALCYON" means story, do not port. Commits are Last Caller commits.

| # | Change | Commits | Files | Port? | Care in TinyBBS |
| --- | --- | --- | --- | --- | --- |
| P1 | Interaction contract and audit | 4406e13 | `.kiro/steering/interaction.md`, this file | Engine (docs) | Rewrite the audit rows for Kouvosto's screens and doors |
| P2 | Letters act at once at every menu, list, pager and yes/no (hotkey tick, browser key mode 3, Backspace) | 6b5d486 | `src/bbs/commands.h`, `src/bbs/runtime.h`, `src/core/stream.h`, `src/core/session.h`, `shared/web/terminal.html` | Engine | The `src/core/haunt.h` part is the calm-prompt hook: port only the `hotKey` check. Regenerate `web_terminal.h` with `tools/embed_web.py` |
| P3 | ENTER never leaves a level; Q back, X main, ? keys everywhere; one pager for messages, bulletins, WHO and stats; P page -> O, P post -> W, R read -> L; scene type-ahead and return to the interrupted screen | 0e0fd5e, 28934d2 | `src/bbs/board.h`, `cmd_*.h`, `eggs.h`, `help.h`, `messages.h`, `sysop.h`, `files.h`, `viewers.h`, `chat.h`, `src/core/reader.h`, `src/core/session.h`, `src/core/stream.h` | Engine | Kouvosto's doors (Legend, dice, VolaMUD) need Q, QUIT, EXIT and a "how to leave" line on entry; check their own key loops against section 2. Scene parts (`src/core/scene.h`) exist only if TinyBBS has the haunt engine: otherwise skip |
| P4 | Manifest keys, shell leave words | 28934d2 | `src/doors/manifest.h`, `src/doors/shell.h`, `src/core/shell_logic.h` | HALCYON | Do not port |
| P5 | `story_lint.py` `check_keys` | 28934d2 | `tools/story_lint.py` | HALCYON | TinyBBS has no story lint; port the idea only if it gets help texts that name keys |
| P6 | W in a read-only area says why it takes no post | 7416445 | `src/bbs/cmd_messages.h` | Engine | |
| P7 | e2e follows the new keys and the pager | 0e0fd5e, 3eb7196, d46bcfd, a788055, 2c51055, 7ab5e37, cbb892f, f17933d, 8a4c5f3 | `tools/test/e2e_telnet.py` | Engine parts | Kouvosto's e2e (if any) needs the same key changes; the story steps are HALCYON |
| P8 | Numbered scene choices (1 to 9) and Q act at once; `[any key]` waits | f17933d, 240ea43 | `src/bbs/commands.h`, `src/core/scene_logic.h`, `src/core/stream.h` | Engine if scenes exist | |
| P9 | Snake: moves and Q act at once, Q door menu, X main | f17933d | `src/doors/snake.h` | Engine | Check Kouvosto's Snake copy is the same file |
| P10 | Single-page documents drop the row counter; Q and X first in footers | f17933d | `src/core/reader.h` | Engine | |
| P11 | Chat: banner says `Q leaves.`; a lone Q, QUIT, EXIT, /Q leave; a lone Q acts at once | 8a4c5f3, 3e35651 | `src/bbs/chat.h`, `src/bbs/cmd_main.h`, `src/bbs/commands.h`, `src/core/stream.h` | Engine | |
| P12 | Reply save ends like a post (back to the message menu) | 8a4c5f3 | `src/bbs/messages.h` | Engine | |
| P13 | A caller's own posts are never "new" to them | 3e35651 | `src/bbs/messages.h` | Engine | `mine()` checks `s.readOnly` (a HALCYON crew login); in TinyBBS drop that part if there is no read-only login |
| P14 | Front page puzzle hint, account-made note after CREW, "Start with the service label" | 8a4c5f3, c51eb3c | `sd/BBS/STORY/POWER.INI`, `src/bbs/power_ui.h`, `src/core/power.h` | HALCYON | Do not port |
| P15 | A queued page under a prompt draws that prompt again (`eggPrompt` covers every calm prompt) | c51eb3c | `src/core/haunt.h`, `src/bbs/eggs.h` | Engine | `eggPrompt` is used by the eggs too; TinyBBS has the eggs, port both together |
| P16 | A scene starts on a fresh line, never on the prompt it interrupts | c51eb3c | `src/core/scene.h` | Engine if scenes exist | |
| P17 | Key hint once per call under the first main menu ("Press a letter. No ENTER needed. ? shows the keys, X comes back here.") | c51eb3c | `src/bbs/board.h` | Engine | Uses `s.menusSeen`, no new field |
| P18 | Number prompts say "Q goes back at once" | c51eb3c | `src/bbs/cmd_main.h`, `src/bbs/messages.h`, `src/bbs/cmd_files.h`, `src/bbs/eggs.h` | Engine | The e2e matches only the "Bulletin #", "Area #", "File area #" prefix |
| P19 | Main menu C label "Chat: SysOp, callers" | c51eb3c | `src/bbs/board.h`, `tools/ansi/render.py` | Engine (label) | On Kouvosto, with a human SysOp, "Node chat" may read better; decide there |
| P20 | UX tester agents and the fixed task list | f17933d | `.kiro/agents/ux-tester-1.md`, `ux-tester-2.md` | Engine (tooling) | They point at HALCYON staging; change the host and the board name |
| P21 | Pager bar: the keys always fit in 79 columns; a long title gives way first, then the row counter | 191660b | `src/core/reader.h` | Engine | |
| P22 | X leaves the Diagnostics door to the main menu | 191660b | `src/doors/diag.h` | HALCYON door, engine rule | TinyBBS line doors (Legend, dice, VolaMUD) need the same X, unless X is a game word there |
| P23 | Testers run one at a time: the `bbs-caller` MCP server holds one Telnet connection, so parallel sub-agents share it | (round 3) | `tools/mcp/bbs_caller.py`, `docs/reviews/UX-LOOP.md` | Engine (tooling) | Same in TinyBBS if the tool is copied |
| P24 | Scenes and pages never on a number prompt (`hauntCalm` drops BULLETIN_LIST, JOIN_CONF, FILE_MENU) | 7e8ff0f | `src/core/haunt.h` | Engine if the haunt engine exists | |
| P25 | Scene choice list says "Press a number, no ENTER." | 7e8ff0f | `src/core/scene_logic.h` | Engine if scenes exist | |
| P26 | Password prompts echo a star per key; Backspace erases one | 7e8ff0f | `src/bbs/commands.h` | Engine | The browser already draws stars in its local line draft (`secret`); Telnet now matches |
| P27 | C label: "Chat with the SysOp" for callers, "Node chat" for the SysOp | 7e8ff0f | `src/bbs/board.h`, `tools/ansi/render.py` | HALCYON label | On Kouvosto keep "Node chat": there is no ELIZA scene before it |
| P28 | A sign-up made inside a door logs the session in to the new account at once (note, then the normal login: welcome, mail, main menu); no second `Login:` | 824782e, 8d9d4eb, 3769688 | `src/bbs/power_ui.h` (`crewAccountDone`), `sd/BBS/STORY/POWER.INI`, `tools/test/e2e_telnet.py`, `docs/TESTING.md` | Engine (the rule: `claimAccount` then `finishLogin` after a sign-up); the CREW console is HALCYON | TinyBBS's plain N)ew sign-up already logs in at once; port only if it gets a door-made account |
| P29 | A door's yes-or-no confirm is one key (the RTG `ACK>` takes K or aborts at once; the line says "PRESS K TO ACKNOWLEDGE. NO ENTER.") | d1e33c3 | `src/core/stream.h` (`hotkeyState`), `src/doors/ship.h`, `sd/BBS/STORY/SHIP.INI` | Engine rule (hotkey states for confirms); the ship door is HALCYON | Kouvosto doors with Y/N confirms: add their confirm state to `hotkeyState` |
| P30 | (open, from round 4) Snake prompt should say moves need no ENTER | none yet | `src/doors/snake.h` | Engine | Ines typed moves with ENTER; one word in the prompt |
| P31 | (open, from round 4) In node chat the caller's own line is echoed after a fast reply, so the order looks jumbled | none yet | `src/bbs/chat.h` | Engine | Low; check echo order when a scripted or SysOp reply comes in under 1 s |
| P32 | One page, picture or scene per calm prompt; the rest wait for the caller's next command (a login never prints a pile of shows) | 093c8a3, 301fdbf | `src/core/haunt.h` (`hauntShow`), `tools/test/story_property_test.cpp` (P11), `tools/test/e2e_telnet.py` | Engine if the haunt engine exists | P11 reads the card's TRIGGERS.INI: HALCYON test, port the idea only |
| P33 | "Log in as another" lists the accounts this caller has opened before (number, no ENTER; A for any handle), logging in without the password; the knowledge is a `KNOWN_<HANDLE>` flag on the caller's own account | ee6db49, 567e9f1 | `src/bbs/accounts.h` (`crewKnown*`), `src/bbs/cmd_main.h` (`crewPick`, `crewSwitch`, state `CREW_PICK`), `src/core/session.h`, `src/core/stream.h`, `src/bbs/commands.h` | HALCYON (crew accounts are story accounts); the switch-without-password is safe only for read-only accounts | TinyBBS has no read-only story accounts; do not port unless Kouvosto gets shared guest accounts |
| P34 | A read-only shared account has no L; its G reads "Goodbye (to Login:)" and its login says once how it is left | ee6db49, 916f58c | `src/bbs/board.h` | Engine rule (labels say what a key does on that account; menu labels fit the column width, 20 characters in MAIN) | Kouvosto's guest account, if any |
| P35 | Manifest notebook writes down the passwords of crew accounts that opened | ee6db49 | `src/doors/manifest.h` | HALCYON | Do not port |
| P36 | A waiting page or scene starts only after two quiet seconds at a calm prompt (keys no longer land on a scene's `[any key]`) | 3c17c35 | `src/core/haunt.h` (`hauntTick`), `tools/test/e2e_telnet.py` (`advance` every 2.5 s) | Engine if the haunt engine exists | |
| P37 | The front-page hint is not shown again on a line that has signed in | 3c17c35 | `src/bbs/power_ui.h`, `sd/BBS/STORY/POWER.INI` | HALCYON | Do not port |
| P38 | A scene choice list that also takes typed replies says "Or type a reply, then ENTER." | eca17b7 | `src/core/scene_logic.h` | Engine if scenes exist | |
| P39 | The notebook: a notes table (`STORY/NOTES.INI`, one section per note: `on=`, `match=`, `src=`, `text=`), collected the moment the thing is on screen and saved as the flag `N_<ID>` on the caller's own account, once (`notesOn`, called from `hauntEvent` for read, file, art, screen, door, login; from the scene player per node; from `hauntShow` for pages; from ELIZA's chat lines; from WHO ghost rows; from the diagnostics and shell doors) | 1d5773f, 337c535 | `src/core/notes_logic.h`, `src/core/notes.h`, `src/core/haunt.h`, `src/core/ghost.h`, `src/core/scene.h`, `src/core/session.h` (`HauntShow.noteKind`, `GhostLine.id`), `src/doors/diag.h`, `src/doors/shell.h`, `src/core/shell_logic.h` (`Result.read`), `tools/test/notes_logic_test.cpp` | Engine if the haunt engine exists | The hooks are one line each. Kouvosto has no ghosts, scenes or shell: take `notes_logic.h`, `notes.h` and the `hauntEvent` line, and add `notesOn` where the board shows a message, a file or a bulletin. About 20 KB of flash, 216 bytes of static RAM |
| P40 | The notebook screens (six notes a page, one note with where it came from and where to read it again, scenes seen, logins that worked); keys 1 to 6 or 9, N, P, S, L, R, Q, X | 337c535 | `src/bbs/notebook.h`, `src/doors/manifest.h` (N calls it), `src/core/session.h` | Engine (the screens); the manifest door that hosts them is HALCYON | Host them in any door or menu. They need `doorFlagsOf` (the caller's own flags) and the notes table |
| P41 | Replay: a scene, a page of ELIZA's or a story picture shown again from the notebook, as it was; grants nothing, saves no marker and no flag, `then=` is not followed, Q leaves at once (`scene::Player::start(..., replay)`, `sceneReplayStart`, `Session.sceneReplay`) | 1d5773f, 337c535 | `src/core/scene_logic.h`, `src/core/scene.h`, `src/bbs/commands.h`, `tools/test/scene_logic_test.cpp` | Engine if scenes exist | The Q-at-once line in `commands.h` sits in the quiet branch for a scene that is typing |
| P42 | The notes themselves (124 lines, one or more per clue of the chain and the story bible) and the `check_notes` lint (every note points at something on the card; every E, M and P id of the chain has a note) | 337c535 | `sd/BBS/STORY/NOTES.INI`, `tools/story_lint.py` | HALCYON | Do not port the notes. Port the idea of a lint that fails on a clue with no note, if Kouvosto gets a notebook |
| P43 | Idle: a warning line 30 s before the line closes, the prompt and the typed line drawn again under it; 10 minutes at Login: on a line that has signed in; any key (not only ENTER) keeps the line | f6d81b3 | `src/bbs/runtime.h`, `src/bbs/commands.h`, `src/core/session.h` (`lastPrompt`, `idleWarned`), `src/core/globals.h` | Engine | `prompt()` now keeps the last prompt; a prompt written with `write()` is not drawn again |
| P44 | A scene choice node may take `typed_else=NODE`: a typed reply that matches no word goes there instead of "Choose 1 to n" | 5bbf023 | `src/core/scene_logic.h`, `tools/story_lint.py`, `tools/test/scene_logic_test.cpp` | Engine if scenes exist | |
| P45 | Pages and scenes no longer start at "Press any key" (BULLETIN): the key meant for the prompt was taken by the scene | 5bbf023 | `src/core/haunt.h` (`hauntCalm`) | Engine if the haunt engine exists | |
| P46 | Digits typed in the message reader redraw only the bar ("Go to #12"), not the page once per digit | 5bbf023 | `src/bbs/messages.h` | Engine | |
| P47 | The viewer's ? line names S as "S at 300 baud" | 76301c6 | `src/core/reader.h` | Engine | Kouvosto may prefer "S slow" |
| P48 | C7 changed: keys typed during a scene are dropped when it ends, not played back into the next prompt | Q22 | `src/bbs/commands.h` (`hotkeyTick`, `input`), `src/core/session.h` (`typeAhead`) | Engine if scenes exist | Contract C7 and `.kiro/steering/interaction.md` follow |
| P49 | SPACE at "Press any key" is a key (was echoed as text and waited for ENTER); a maintenance login on a line that has signed in gets the powered console (`CONSOLE_ON`, no timer); Telnet keeps output a full send buffer did not take; LOGOFF says why the line closed | Q22 | `src/bbs/commands.h`, `src/bbs/board.h`, `src/doors/ship.h`, `src/net/telnet.h` | Engine (console: HALCYON) | |
| P50 | The idle check, idle triggers, ghosts and glitches measure time with `msSince(now, at)`: a key run in the same tick stamps `lastAt` after the tick's `now`, and plain unsigned `now - lastAt` wrapped to about 49 days and closed the line at once (the "connection closed by the board" the critics saw, LOGOFF "last key 0 s ago"), and could fire an idle trigger right after a key | Q22 | `src/core/util.h`, `src/bbs/runtime.h`, `src/core/haunt.h`, `src/core/ghost.h`, `src/core/glitch.h` | Engine | Check every `now - x` in TinyBBS where x can be stamped later in the same tick |
| P51 | Only Q skips a scene (G1): ENTER and other keys never flush typing or end a pause; Q grants flags, `then` and notebook lines as if played and stops at a choice; "Q skips" line after the first picture; numbered choices and [any key] take keys after a 1.5 s grace (`armed()`); the half-typed line and buffered keys are dropped at scene start; a lone Q waits `HOTKEY_MS` so a word typed ahead is not a Q | G1 | `src/core/scene_logic.h`, `src/core/scene.h`, `src/core/stream.h`, `src/bbs/commands.h`, `tools/test/scene_logic_test.cpp`, `tools/test/e2e_telnet.py` | Engine, if TinyBBS has the scene engine | `hotkeyState` returns true for a scene, so the browser sends every key at once |
| P52 | Passwords stay case-sensitive (G1, rev. 3): the login screen always carries `WARNING: CAPS LOCK OFF` and a miss that the same word in capitals would have fixed answers `ACCESS DENIED. CAPS LOCK OFF?` (`passMatchesCapitals`; POWER.INI `caps_warning`, `caps_denied`); a wrong password on a story account shows that account's own line (ACCOUNTS.INI `badpass`, `badpass2`, `badpass3`, ELIZA's `page` on the third); no answer twice in a row in node chat (`chat=again`), scenes (`text2=`, `text3=`), Diagnostics, the shell and the login (`core/repeat.h`) | G1 | `src/core/util.h`, `src/core/repeat.h`, `src/core/ghost_logic.h`, `src/core/scene_logic.h`, `src/bbs/cmd_login.h`, `src/bbs/accounts.h`, `src/doors/diag.h`, `src/doors/shell.h` | Engine | Hashes of other boards on the same code: the lower-case and exact checks keep every old password working. Content (the lines) is HALCYON only |
| P53 | One chat (G2): node chat is free text from the first line; `?`, `help`, `suggest` or `what can I do` list the options that fit now as numbers (`option=` on a SYSOP.INI keys line); a number or the option's words pick it; `then=SCENE` on a line starts a scene once it is said; a new caller line first shows at once what she had not finished, so answers never come late or out of order; typing 20 ms a character, no gap over about 1.4 s. A RiveScript reader (subset: triggers with `* # _ [opt] (a\|b) @array`, weights, `%` previous, `@` redirect, topics with includes, conditions, `<set>`/`<get>` saved per account in `DATA/STATE/ELIZA.DAT`, `<star>`, `<person>`, `<formal>`, `{random}`, `! sub`, `! person`, `! array`, `! var`; no macros) answers what no card line took; story flags are readable as `<get flag_NAME>`, never writable | G2 | `src/core/rive_logic.h`, `src/core/ghost_logic.h`, `src/core/ghost.h`, `src/bbs/chat.h`, `src/bbs/cmd_main.h`, `tools/test/rive_logic_test.cpp`, `tools/test/ghost_logic_test.cpp`, `tools/story_lint.py` (`check_rive`) | Engine (the reader); content (the brain, the options) is HALCYON only | The ESP32 link needed `-mtext-section-literals` (one translation unit outgrew the literal pool); `std::stable_sort` is avoided in the reader |
