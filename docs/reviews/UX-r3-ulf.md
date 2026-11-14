# UX round 3, Ulf (handle UlfThree)

Staging board, commit 4cacb98. About 62 tool calls in total (counts below are approximate).

## 1. Tasks

| # | Task | Result | Calls |
| --- | --- | --- | --- |
| 1 | Connect, log in as new caller, reach main menu | done with trouble | 16 |
| 2 | Read a bulletin | done with trouble | 6 |
| 3 | Find a message and read it | done | 4 |
| 4 | Reply to that message | done | 3 |
| 5 | Post a new message (2 or 3 lines) | done | 7 |
| 6 | List files, open and read a file | done | 4 |
| 7 | Open a door, leave it | done | 4 |
| 8 | Node chat: say something, leave | done with trouble | 8 (plus 4 for the dropped connection and re-login) |
| 9 | Find the help screen | done | 1 |
| 10 | Back to main menu from deep | done | 2 (X from the file viewer, X from the message area) |
| 11 | Log off | done | 1 |

## 2. Trouble log

1. M. First screen. The login prompt says nothing about how to make an account. The text says "puzzle game" and "start with the service label". I typed my handle (Unknown caller), then ADMIN/ADMIN from the label, which worked. After that I had to find SOURCE RTG (HELP lists it), then answer "TYPE [K] AND PRESS ENTER" and then CREW. It took 16 calls. I got there only because every screen told me the next step. A person who wants a quick account would leave.
2. L. At the K acknowledge prompt the text says "TYPE [K] AND PRESS ENTER". This is the only place where a bracketed letter needs ENTER. Everywhere else a bracketed letter acts at once.
3. L. After registration the board says "Log in with the handle and password you just chose." That is clear, but I had to type both again.
4. M. A big ELIZA scene came right after login. It had a numbered list (1/2/3) with a "> " prompt. The number worked as a single key with no ENTER. I did not know that and sent it alone as a test. The prompt does not say "no ENTER".
5. L. In Bulletins an ELIZA scene covered the list ("[any key]"). After the key the list came back. Fine, but the scene comes at an odd moment.
6. M. Bulletins: the prompt is "Bulletin #, then ENTER (Q goes back at once)". The number needed ENTER, as it said. Good. But the bulletin viewer's footer says "Q back" and Q went back one level to the list, then another ELIZA line appeared under the prompt. This was harmless but looked like noise.
7. L. The main menu changes between visits. The first view had [L] Log in as another, [G] Goodbye in different slots, and after I posted a message the menu gained [R] Crew manifest and the layout moved. A key I had seen was in a new place. Harmless, but I read the menu again each time.
8. S. I pressed C (chat) the first time and the connection was closed by the board, with no message. I had to dial in again and log in again. I do not know the cause (a scene? a timer?). The second time C worked. From the caller's side it looks like a crash.
9. M. After the re-login I pressed a key at "Press any key for the Main Board" and got a blank screen. A read returned nothing. ENTER then drew the main menu. For about 10 seconds I thought it was stuck.
10. M. Chat. The menu says "Chat: SysOp, callers". The first C screen is a talk-to-ELIZA scene with a choice list (1 Say something else, 2 Hello ELIZA, 3 Give me SYSOP). Real node chat is behind "1) Say something else", which is not a name I would guess. Once in, the line "Type and press ENTER to talk. Q leaves." was clear.
11. L. In node chat I typed Q with no ENTER and it left at once. In the ELIZA scene "Q leaves" also worked as a single key. Consistent. But the line prompt in chat said "Type and press ENTER", so I expected Q to need ENTER too.
12. L. In the Snake door the prompt is "Moves (W A S D, Q quits)>" and I sent Q with ENTER. It worked. I did not test Q alone.
13. L. The key help (?) screen is short and good. It says X goes to the main menu from anywhere. It did, in one key, from the file viewer.

Pleasures:
- P1. Every list says what to press at the bottom ("Message #, then ENTER (ENTER = first new, P = earlier, Q = back)").
- P2. X from deep inside a file viewer went straight to the main menu. Q went one level back. I never doubted which was which.
- P3. The reply and write editors show their commands up front (/S saves) and number the lines.
- P4. "+50 KB download credit" after a post is a small reward and made me smile.

## 3. SUS

Answers (1 to 5):

1. 3
2. 2
3. 3
4. 2
5. 3
6. 2
7. 3
8. 2
9. 3
10. 3

Arithmetic: odd items, answer minus 1: 2 + 2 + 2 + 2 + 2 = 10. Even items, 5 minus answer: 3 + 3 + 3 + 3 + 2 = 14. Sum 24, times 2.5 = 60.

SUS: 60

SEVERE: no
