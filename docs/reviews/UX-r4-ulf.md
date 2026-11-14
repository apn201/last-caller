# UX round 4 - Ulf (handle UlfFour)

About 58 tool calls in all, including 2 that failed because I left out the tool id (my mistake, not the board's).

## 1. Tasks

| # | Task | Result | Calls |
| --- | --- | --- | --- |
| 1 | Connect, make an account, reach main menu | done with trouble | 14 |
| 2 | Read a bulletin | done | 4 |
| 3 | Find and read a message | done | 4 |
| 4 | Reply | done | 3 |
| 5 | Post a new message | done | 7 |
| 6 | List files, open and read one | done | 4 |
| 7 | Open a door, leave it | done | 3 |
| 8 | Node chat, say something, leave | done with trouble | 5 |
| 9 | Help screen | done | 1 |
| 10 | Back to main from deep | done (X from a file and from inside Snake) | 2 |
| 11 | Log off | done | 1 |

## 2. Trouble log

1. M. Login screen. It says "You have no account" and tells me to find out how to make one. I typed my handle and got "Unknown caller". Nothing on that screen says how to register. I only got through because the label showed ADMIN/ADMIN and I tried it. A person who does not read the label closely would be stuck. (Task 1)
2. M. After ADMIN login I was at a "SHIP>" prompt, not a menu. HELP listed commands, but registration was not among them. CREW only appeared after SOURCE RTG and K. I guessed SOURCE RTG from the help list. I did not know whether HELP was meant to be typed. (Task 1)
3. L. "PRESS [K]. NO ENTER." was clear. Good.
4. L. The registration prompts say what is needed ("3-16 chars, ENTER cancels"). Good.
5. L. After registering, a long document screen, a notice screen and an ELIZA scene came one after another. The "Press any key" prompts worked. The ELIZA scene wanted a number with no ENTER and said so. Fine, but it is a lot before the main menu.
6. L. Bulletins: "Bulletin #, then ENTER (Q goes back at once)". A digit does not act at once here, though a letter does. The prompt says so, so it was not a problem. (Task 2)
7. L. Bulletin pager: Q went back to the bulletin list, not the main menu. Another Q went to the main menu. This matches "back one level".
8. L. After Q at the bulletin list, an ELIZA scene popped up over the main menu and printed the menu twice. Odd, but a key press carried on fine.
9. M. Task 8 asks for node chat. The main menu only has "Chat with the SysOp". I pressed C and got ELIZA's numbered scene (1/2/3). Option 1 "Say something else" led to the real "Multi-Node Chat" where I type a line. So chat needs two steps and the first one looks like a quiz. I did not know a free-text chat existed until I picked 1. Q left it with one key, which is good.
10. L. In chat my line echoed as "> hello, anyone here?" after ELIZA's reply, so the order looked jumbled. ELIZA answered a bit late. Harmless.
11. L. Snake says "several at once if you like" and prompts "Moves (W A S D, Q quits)>". Q alone quit with no ENTER. I did not play.
12. L. Opening a file: "[#] Open (number, ENTER)". Worked as described. The "File area #, then ENTER" prompt is consistent.
13. L. Message area: the reply editor explains /S, /A and so on up front. I got it right the first time. Posting gave "+50 KB download credit", which I did not understand but did not matter.
14. Pleasure: X went to the main menu from inside a file and from inside Snake with one key. The "Press a letter. No ENTER needed. ? shows the keys, X comes back here." line under the menu is what I needed.
15. Pleasure: the ? screen is short and tells me what works everywhere.

No S items.

## 3. SUS

1. 4
2. 2
3. 4
4. 1
5. 4
6. 2
7. 4
8. 2
9. 4
10. 2

Odd items: 3+1+3+3+3 = 13. Even items: 3+4+3+3+3 = 16. Total 29 x 2.5.

SUS: 72.5

SEVERE: no
