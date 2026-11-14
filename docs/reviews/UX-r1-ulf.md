# UX round 1: Ulf

About 75 tool calls in total. Played only through bbs-caller.

## 1. Tasks

| # | Task | Result | Calls |
| --- | --- | --- | --- |
| 1 | Connect, make an account, reach main menu | done with trouble (S) | about 25 |
| 2 | Read a bulletin | done | 4 |
| 3 | Find a message and read it | done | 4 |
| 4 | Reply to it | done | 4 |
| 5 | Post a new message | done with trouble (my own parallel calls, see log) | 9 |
| 6 | List files, open and read one | done | 4 |
| 7 | Open a door, leave it | done with trouble | 5 |
| 8 | Node chat, say something, leave | done with trouble (it is a chat with ELIZA, the way out is not shown) | 6 |
| 9 | Find help | done | 1 |
| 10 | Back to main menu from deep | done with trouble (X in a file, a message list and the door menu works; X inside Snake does not) | 6 |
| 11 | Log off | done | 1 |

## 2. Trouble log

1. **S** Login. The first screen says only "Login:". I tried NEW, ?, ulf and an empty line. Each gave "Unknown caller. No such account on this bus. The maintenance login is on the service label." Nothing says how a new person gets an account. I gave up after four tries and used the ADMIN/ADMIN label because it was the only hint. That gave a "SHIP>" console, not a menu. HELP there listed STATUS, POWER and so on, and nothing about accounts. I tried EXIT (back to Login), then went in again and tried SOURCE RTG, only because it was the one command that sounded like it did something. After it ("TYPE [K] AND PRESS ENTER") the board said "TYPE CREW TO REGISTER". A normal person would have left.
2. **M** After Account created the board says "Log in with it" and shows the old login screen, which still says ELIZA "NOT RESPONDING" and still shows the ADMIN label. Slightly confusing, harmless.
3. **M** The first ELIZA choice screen (1/2/3 with a ">" prompt): I pressed 1 alone and nothing happened. I had to press ENTER. The Help screen says "a letter works at once". A numbered list that needs ENTER looks like a menu.
4. **M** Same in ELIZA chat: "2" alone did nothing, ENTER needed.
5. **M** Leaving the ELIZA chat: nothing on screen says how. I guessed Q then ENTER and it worked. The list says "Say something else / Hello / Give me SYSOP". I never typed free text.
6. **M** Snake: the prompt says "Q quits". Q alone did nothing, Q then ENTER worked. Everywhere else a letter acts at once.
7. **M** X inside Snake went back only to the door menu (needed ENTER too), not to the main menu as the Help says ("X the main menu, from anywhere"). A second X from the door menu got me out.
8. **M** A pop-up from ELIZA ("[Enter]") appeared over the bulletin menu while I was choosing. I had to press ENTER to get the menu back. Twice more during the session. It breaks the flow but I found my way.
9. **M** "Press any key for the Main Board:" scrolled away behind an ELIZA scene, and I never saw whether my key was needed. I ended on a ">" prompt with three choices. I did not know that I was not yet at the main menu.
10. **L** Typing text lines in the message editor: the first screen explains /S, /A and so on. It was a lot of text, but I only needed /S. Good that it is shown.
11. **L** After posting, "Press any key" took me back to the message I replied to, not to the list. Fine.
12. **L** From a message, Q goes to the message menu, not to the list. I expected the list.
13. **L** My own error: I sent three calls at once and the board took them as lines of my message. The message was posted as #119 with "/S" as line 3. Not the board's fault.
14. **L** In the Messages menu `/L` said "Not a key here. ? shows the keys." Good message.
15. **L** The Help screen says "The keys of this screen are on it" and it is right.

Pleasures: the main menu lists all keys and a letter works at once. The footer of a message or file ("SPACE next, Q back, X main, ? keys") told me what to press every time. "Bulletin #, then ENTER (Q = back)" is the right kind of prompt. The bulletins and ANSI screens look good.

## 3. System Usability Scale

1. I would like to use this system frequently: 2
2. Unnecessarily complex: 3
3. Easy to use: 3
4. Would need a technical person: 3
5. Functions well integrated: 3
6. Too much inconsistency: 3
7. Most people would learn it very quickly: 3
8. Very cumbersome: 2
9. Felt very confident: 3
10. Needed to learn a lot first: 4

Odd items: 1 + 2 + 2 + 2 + 2 = 9. Even items: 2 + 2 + 2 + 3 + 1 = 10. Total 19, times 2.5.

SUS: 47.5

SEVERE: no
