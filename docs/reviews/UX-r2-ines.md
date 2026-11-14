# UX round 2: Ines (handle IneTwo)

Staging board 127.0.0.1:12323, commit 395f0f5. About 50 tool calls in total. Call counts below are approximate.

## 1. Tasks

| # | Task | Result | Calls |
| --- | --- | --- | --- |
| 1 | Connect, make account, log in, reach main menu | Done with trouble | 17 |
| 2 | Read a bulletin | Done | 5 |
| 3 | Find a message and read it | Done | 4 |
| 4 | Reply to that message | Done | 3 |
| 5 | Post a new message (2 or 3 lines) | Done | 6 |
| 6 | List files, open and read a file | Done | 5 (one wasted, see trouble 7) |
| 7 | Open a door, then leave it | Done | 5 |
| 8 | Open node chat, say something, leave | Done with trouble | 3 |
| 9 | Find the help screen | Done | 2 |
| 10 | Back to main menu from deep (file, message, door) | Done | 3 |
| 11 | Log off | Done | 1 |

## 2. Trouble log

1. M. Login. I typed my own name, IneTwo, at "Login:". The board said "Unknown caller." I was not told how to make an account. The only hint is a line in the banner ("work out how to fix the power and make an account") and a label with ADMIN / ADMIN. I logged in as ADMIN. A finance person would think this is a trap or someone else's account. The next screen (SHIP> console) is a different interface with no menu. I typed HELP and found SOURCE RTG, then K + ENTER, then CREW. It took 10 calls. It is the longest path in the test.
2. L. Registration. After the real name and location, the board threw me back to the start banner. It said "Log in with the handle and password you just chose." That was clear enough, but I had to type it all again.
3. L. At "ACK>" the board says "TYPE [K] AND PRESS ENTER". Clear. The square brackets around K could make someone type the brackets.
4. L. Several screens say "[any key]" or "Press any key" and then print a lot of ELIZA text on top of the prompt, mixed with the previous screen. After the welcome bulletin the "Press any key" line ran straight into the ELIZA panel. I was unsure whether the board was waiting.
5. L. At "Bulletin #, then ENTER (Q = back)" Q worked with no ENTER. Good, but the prompt makes you think you need ENTER for everything. Same at the Snake prompt "Moves (W A S D, Q quits)>": I sent D with ENTER, and Q alone worked. Fine, just not clear if ENTER is needed.
6. L. ELIZA's panel appeared after I pressed Q in the bulletins and printed "If you need anything, type C". It appeared after the menu had redrawn, below the prompt. I was unsure whether the board was waiting on me.
7. M. Task 8. "Node chat" (C) opens a chat with ELIZA, the SysOp, titled "SYSOP CHAT". It is not a chat with other callers. I could answer only by pressing 1, 2 or 3. I pressed 2 ("Hello, ELIZA") and she replied. I did not type free text, so I am not sure I could. The menu name does not match what it opens. Left with Q, which worked and was said on the screen.
8. L. File area list: "File area #, then ENTER" is clear. Earlier, I pressed F, and then (my own mistake, two commands sent together) Q. That is on me, not the board. The board accepted F again and printed "Type an area number, then ENTER. Q goes back." and no list, because I had already left the area list. Harmless.
9. L. Message list on the first visit shows message numbers from 10, then from 12 on the second visit. Messages 1 to 9 are not shown. I did not know why. Also my own posts are numbered 118 and 119, which looks odd after #29.
10. L. The reply screen starts at "1:" with a block of slash commands (/S saves, /A aborts, ...). I had to read it to find /S. It is the only place where a command starts with a slash. After I typed one line it did not repeat the hint, but I found /S from the first screen.
11. L. Posting a new message after replying: the "Message command?" prompt reappeared and I had to look for W. It is on screen, fine.
12. L. Help (?) works from the main menu. It says "A letter works at once. You do not press ENTER." and "A number or a word is typed, then ENTER." That is exactly what I needed, and I would have liked to see it before the first menu.

Pleasures: after login the main menu lists every command in plain words with letters in brackets. Q and X worked every time I tried them, from a bulletin, a file, a message, a door and the chat. The "?" help is short and says what ENTER is for. Messages read well and the file listing is tidy.

## 3. SUS

Answers:

1. 3
2. 2
3. 4
4. 2
5. 4
6. 3
7. 3
8. 2
9. 3
10. 3

Arithmetic: odd items (answer minus 1): 2 + 3 + 3 + 2 + 2 = 12. Even items (5 minus answer): 3 + 3 + 2 + 3 + 2 = 13. Sum 25, times 2.5 = 62.5.

SUS: 62.5

SEVERE: no
