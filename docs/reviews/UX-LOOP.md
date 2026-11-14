# Q20 usability loop

Two invented testers who never used a BBS: Ulf (`.kiro/agents/ux-tester-1.md`,
console gamer, based on Hans) and Ines (`.kiro/agents/ux-tester-2.md`,
finance, based on Anna). Same fixed task list every round (11 tasks: log in as
a new caller, read a bulletin, find and read a message, reply, post, list and
view a file, open and leave a door, node chat and leave it, find help, back to
the main menu from deep inside, log off). Each plays on a fresh staging board
(server build on the staging host, `tools/staging-playtest.ps1`), through the
`bbs-caller` tool, with no help, and scores the System Usability Scale (SUS).
Done when the SUS average is 80 or more and no tester hit a severe
inconsistency, or after round 4.

## Rounds

| Round | Date | Commit played | Ulf | Ines | Average | Severe |
| --- | --- | --- | --- | --- | --- | --- |
| 1 | 14 Nov 2026 | cbb892f | 47.5 | 50 | 48.75 | none in keys; one S in both: no way to register on the front page (game, not platform) |
| 2 | 14 Nov 2026 | 8a4c5f3 (Ulf), 395f0f5 (Ines) | 67.5 | 62.5 | 65 | none |

## Round 1

Reports: `UX-r1-ulf.md`, `UX-r1-ines.md`. Baseline for the loop: the engine
after the first build (hotkeys, X, ?, pager for messages, bulletins, WHO).

Found, and what was done for round 2:

| Finding | Who | Fix |
| --- | --- | --- |
| Numbered replies in ELIZA's scenes need ENTER; the help says letters act at once | both | digits 1 to 9 and Q act at once in a scene choice |
| `[Enter]` waits in scenes want ENTER; a space did nothing | Ines | the wait says `[any key]` and any key goes on |
| Leaving node chat (the ELIZA scene, the typed chat) is not shown | both | the choice list says `Q leaves.`; the chat banner says `/Q leaves.`; plain Q, QUIT, EXIT also leave |
| Snake needs ENTER after every move; X goes to the door menu only | both | Snake moves and Q act at once; Q goes to the door menu, X to the main menu |
| Message footer cut off at the right edge | Ines | single-page documents drop the row counter; Q and X come before K, E, H |
| Nothing says how to register at `Login:` | both, S | game decision (the front page is the prologue). Recorded in `docs/ACTION_PLAN.md` area 2. Not changed here |
| After CREW the old power-fault screen comes back | both, M | game screen. Recorded with the row above |
| Three calls sent at once landed in one message | both | tester error: the tool joins them. The testers are told to send one call at a time |


## Round 2

Reports: `UX-r2-ulf.md`, `UX-r2-ines.md`. All 11 tasks done by both. Ulf
played 8a4c5f3; Ines played 395f0f5 (the same plus the lone-Q chat fix and
"own posts are not new"). A first Ines attempt hit a broken staging card: two
staging launches overlapped and the second re-copied the card under the
running server, so the account write failed. That attempt is not counted;
the rerun on a clean board is the score above. Lesson: one staging launch at
a time.

Found, and what was done for round 3 (commit c51eb3c):

| Finding | Who | Fix |
| --- | --- | --- |
| In the typed chat a lone Q went into the line; on the scene before it Q left at once | Ulf, M | a lone Q acts at once in the chat too (3e35651) |
| My own post counts as new | Ulf, L | own posts are never new (3e35651) |
| Login path is long; ADMIN looks like someone else's account | both, M | the front page now ends "Start with the service label." It still does not name the source, the switch or the command (Juha's rule) |
| "Node chat" opened a chat with ELIZA, not other callers | Ines, M | the main menu says "Chat: SysOp, callers" |
| An ELIZA page printed under a prompt, then nothing: is it waiting? | Ines, L | after a page the prompt is drawn again |
| A scene began on the same line as "Press any key" | Ines, L | a scene starts two lines down |
| "Bulletin #, then ENTER (Q = back)": does Q need ENTER? | Ines, L | "(Q goes back at once)" in every number prompt |
| The keys rule is only in `?` | Ines, L | one line under the first main menu of a call: "Press a letter. No ENTER needed. ? shows the keys, X comes back here." |
| Re-login after CREW; "[K]" brackets; slash commands in the editor; message numbers jump | both, L | not changed: game flow or period editor; recorded in `docs/ACTION_PLAN.md` area 2 if game |

Checks after the fixes (c51eb3c): `pio run` passed (RAM 62148 bytes, flash
1646879 bytes), host tests 12 of 12, story lint 0 errors, staging check PASS
(server build, ctest, e2e 63 passed of 63).