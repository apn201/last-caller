# UX round 3, Ines (handle IneThree, commit 4cacb98)

## 1. Tasks

Calls are approximate, counted per task (tool calls to the board).

| # | Task | Result | Calls |
| --- | --- | --- | --- |
| 1 | Connect, make account, log in, reach main menu | Done with trouble | 16 |
| 2 | Read a bulletin | Done | 5 |
| 3 | Find a message and read it | Done | 4 |
| 4 | Reply to that message | Done | 3 |
| 5 | Post a new message | Done with trouble | 8 |
| 6 | List files, open and read a file | Done | 5 |
| 7 | Open a door, leave it | Done | 3 |
| 8 | Node chat, say something, leave | Done with trouble (only SysOp chat found) | 3 |
| 9 | Find the help screen | Done | 1 |
| 10 | Back to main menu from deep | Done (X) | 3 |
| 11 | Log off | Done | 1 |

## 2. Trouble log

1. M. Login screen. I typed my handle IneThree and got "Unknown caller". The screen has no "new user" option. The only way forward is the service label (ADMIN / ADMIN). A finance person would take that to be someone else's account and might stop. I followed it only because it was the only text on the screen.
2. M. After ADMIN login the SHIP> console has no menu. I typed HELP (the screen said to). SOURCE RTG then asks "TYPE [K] AND PRESS ENTER". It worked, but I would not have found CREW without the line "TYPE CREW TO REGISTER". It was shown, so recovery was quick.
3. L. After registering, the board shows the whole login screen again and tells me to log in with the same handle. It is clear, but I typed the details a second time.
4. L. The password I typed was not echoed and there was no mask, so I could not tell whether it was accepted. The next prompt just appeared.
5. M. Asking for bulletins (B), the ELIZA greeting scene appeared on top of the "Bulletin #" prompt. "[any key]" brought the prompt back and nothing was lost. I was unsure for a moment what I was answering.
6. L. First ELIZA scene shows "> " with numbered choices 1, 2, 3. Pressing 1 alone worked with no ENTER. Good.
7. L. The bulletin viewer footer says "SPACE next B back S slow Q back X main". It is not clear that Q takes me back to the bulletin list. It did.
8. L. At the "Bulletin #, then ENTER" prompt, a stray ELIZA line printed ("type C and I will be there"), then the prompt repeated. Harmless.
9. M. Messages. In the list, new messages have a "*" but the prompt says "ENTER = first new". I typed the number 119 and it worked.
10. L. After "/S" in the message editor, the screen showed the menu at once. It confirmed "Message #120 posted". Clear. The editor tells me /S saves, but its first prompt line "1:" does not say that an empty line is not an end. I kept typing lines.
11. M. The main menu says "[C] Chat: SysOp, callers". I pressed C and got only a chat with ELIZA (choices 1, 2, 3, Q leaves). I could not find any chat with other callers. I said "Hello, ELIZA" and left with Q. I do not know if I missed a caller chat. Task 8 is only partly done.
12. L. Main menu header shows "Press a letter. No ENTER needed." Clear and helpful the first time, but the text is not shown again on later returns to the menu.
13. L. In the message menu, X is not listed, but it works. Q goes back to the main board, so I never needed X.
14. L. In my own testing, I sent three commands at the same time by mistake (my error, not the board's). The board kept them in order but the screen looked confused. I tested "/A" afterwards and got "Not a key here. ? shows the keys." Fine.
15. L. Snake (door S) was not tried, only Diagnostics. Diagnostics asked for a "query code". I typed QUIT and it left. The screen said QUIT was possible. Good.

Pleasures:
- P1. "Press a letter. No ENTER needed. ? shows the keys, X comes back here." is on the main menu. It told me what to do.
- P2. The ? help screen is short and states the same rules the board follows.
- P3. Every viewer has a footer line with its keys.
- P4. X from a file and from the message menu took me to the main menu at once.

## 3. SUS

Answers (1 to 5):

1. 3
2. 2
3. 4
4. 2
5. 4
6. 2
7. 3
8. 2
9. 3
10. 3

Arithmetic:
- Odd items (answer minus 1): 2 + 3 + 2 + 2 + 2 = 11
- Even items (5 minus answer): 3 + 3 + 3 + 3 + 2 = 14
- Sum 25, times 2.5 = 62.5

SUS: 62.5

SEVERE: no
