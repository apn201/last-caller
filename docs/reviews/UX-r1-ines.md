# UX test, round 1: Ines

I played through bbs-caller only, about 75 tool calls. I connected once and hung up through the board's own Goodbye.

## 1. Tasks

| # | Task | Result | Calls |
| --- | --- | --- | --- |
| 1 | Connect, make an account, reach the main menu | done with trouble (severe) | about 27 |
| 2 | Read a bulletin | done | 5 |
| 3 | Find a message and read it | done | 4 |
| 4 | Reply to it | done | 5 |
| 5 | Post a new message of 2-3 lines | done with trouble (my mistake, see log) | 8 |
| 6 | List files, open and read one | done | 6 |
| 7 | Open a door and leave it | done | 5 |
| 8 | Open node chat, say something, leave | done with trouble | 8 |
| 9 | Find the help screen | done | 1 |
| 10 | Back to the main menu from deep inside | done (X) | 4 |
| 11 | Log off | done (G) | 1 |

## 2. Trouble log

1. **S. No way to make an account.** The first screen only says `Login:`. I typed `new`, `NEW`, `Ines`, `?` and `guest`. Each time I got "Unknown caller. No such account on this bus. The maintenance login is on the service label." That sentence sent me to the label, so I logged in as ADMIN/ADMIN, which I would not do on a real system. The ADMIN screen is an "engineering console" with a HELP list that has no sign-up. I tried COMMS and EXIT (which dropped me back at Login). I then went back in and typed `SOURCE RTG`, K, and only after that did the board say "TYPE CREW TO REGISTER". A customer would have given up. About 20 calls.
2. **M. After registering, "Account created. Log in with it."** and the screen is the old power-fault screen again ("RESTART AFTER POWER LOSS", "MAINT ACCT DEFAULTS NOT CHANGED"). It looked as if nothing had changed. I then typed my handle and it worked.
3. **M. Numbered choices in ELIZA's scenes do not act on the digit.** After the welcome scene (options 1 to 3) and again in chat, I pressed `1` or `2` alone and nothing happened. Only a following ENTER made it work. The help screen says "A letter works at once... A number is typed, then ENTER", so it is consistent with help, but the prompt `>` does not say ENTER.
4. **M. "[Enter]" and "Press any key" differ.** After ELIZA's lines, the board shows `[Enter]`. I sent a space and nothing happened, then ENTER worked. The bulletin and post-save prompts say "Press any key", and any key worked there.
5. **M. Node chat is not chat with people.** The menu says `[C] Node chat`, but it opens "SYSOP CHAT" with ELIZA and three canned choices. When I typed a sentence it answered "Choose 1 to 3, or Q to leave." Only then did I learn how to leave. The leave key was not shown on entry. Q also needed ENTER here, while Q acts at once on every other screen.
6. **M. Typing while the board shows a scene.** ELIZA's panel appeared on top of the bulletin list ("Good evening. Your account has been created...") and I had to press ENTER before the list came back. It was fine, but I did not know it was waiting for me.
7. **M. My own error, but a trap.** I accidentally sent three inputs at once while writing the second message. Letters of a sentence landed at the "Message command?" prompt. The board answered "Not a key here. ? shows the keys." and left me in an unclear state (a "board clock" line appeared). I recovered with Q. Message #119 was saved with two lines.
8. **L.** The message menu said "2 new" right after I posted my own two messages. Own posts count as new.
9. **L.** The Lounge list prompt says "ENTER = first new" while there were 0 new.
10. **L.** The Snake door needs ENTER after each move (`Moves (W A S D, Q quits)>`). The prompt names Q but not ENTER. The help says letters work at once.
11. **L.** Bulletin and file pagers show a long footer (`SPACE next B back S slow Q back X main ? keys`) that is cut off at the right edge in the message reader (`... E edit  Q`).
12. **Pleasure.** Help screen (`?`) is short and clear. `X` takes you to the main menu from anywhere, as the help says. Menu letters act at once, which felt good. Replying with `R` and finishing with `/S` was easy, with the commands printed on screen.

## 3. System Usability Scale

1. I would like to use this frequently: 3
2. Unnecessarily complex: 3
3. Easy to use: 3
4. Need a technical person's support: 4
5. Functions well integrated: 4
6. Too much inconsistency: 3
7. Most people would learn quickly: 3
8. Very cumbersome: 2
9. Felt very confident: 3
10. Needed to learn a lot first: 4

Odd items: 2+2+3+2+2 = 11. Even items: 2+1+2+3+1 = 9. Total 20 x 2.5.

SUS: 50

SEVERE: no
