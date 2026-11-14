# UX round 3: Ines

Tester: Ines (finance, has never used a BBS or a terminal). Handle IneThree.
Board: staging, 127.0.0.1:12323, Telnet through the `bbs-caller` tool.
About 128 tool calls in total. About 80 of them went on task 1.

## 1. Tasks

| # | Task | Result | Calls |
|---|------|--------|-------|
| 1 | Connect, make an account, reach the main menu | done with trouble (severe) | ~83 |
| 2 | Read a bulletin | done | 4 |
| 3 | Find a message and read it | done | 3 |
| 4 | Reply to that message | done | 3 |
| 5 | Post a new message (2-3 lines) | done | 7 (2 of them my own mistake, see item 16) |
| 6 | List the files, open and read one | done | 4 |
| 7 | Open a door, then leave it | done | 6 |
| 8 | Node chat: say something, leave | not done (found only the SysOp chat) | 6 |
| 9 | Find the help screen | done | 1 |
| 10 | Back to the main menu from deep | done from a file and a message; trouble from a door | 9 |
| 11 | Log off | done | 2 |

## 2. Trouble log

1. **S** Login screen, new caller. The screen says "You have no account... work out how to fix the power and make an account. Start with the service label." My own handle got "Unknown caller." There is no NEW option (I typed NEW and got "Unknown caller." again). I logged in with ADMIN / ADMIN from the label and reached a `SHIP>` console. From there I had to find SOURCE RTG, then an ACK, then CREW. I see that the puzzle is on purpose. Still, a customer who just wants an account would call support at this point.
2. **S** RTG ACK prompt. The screen says "TYPE [K] AND PRESS ENTER TO ACKNOWLEDGE." I typed K and pressed ENTER. About half the time it worked ("RTG ONLINE"). The other half the board answered "UNKNOWN COMMAND - TYPE HELP" at `SHIP>`, as if the console had taken my K and the ACK prompt had never seen it. The next SOURCE RTG then answered "SWITCH ABORTED. NO CHANGE." straight away, with no warning screen. Typing K alone and then ENTER separately also failed once. I did the same thing every time and got two different results.
3. **S** The replies stopped matching what I typed. Examples: I typed STATUS and got "SWITCH ABORTED. NO CHANGE."; I typed SOURCE RTG at what looked like `SHIP>` and got "Unknown caller. ... Login:"; at Login I typed ADMIN and went straight into the console with no Password prompt; another time ADMIN at Login gave "UNKNOWN COMMAND - TYPE HELP". I could not tell from the screen what the board was waiting for. (The brief said another tester might be on the board. If their typing shared my line, that would explain this, but nothing on screen showed it.)
4. **S** Registration said it worked when it had not. Twice, CREW went Handle, then "Real name:" with no password question, then "Your account is made and the power is yours. Log in with the handle and password you just chose." I had not chosen a password. Logging in as IneThree then gave "Unknown caller." Once, Handle followed by ENTER gave "No account made." with no reason. On the fourth try the full flow appeared (Handle, Password 6+ chars, Real name, Location) and the login worked.
5. **M** "SUNSIDE ENDS - CARRIER LOST" cut the line twice, once during the ACK and once just before CREW, with no warning. Each time the power was back on SOLAR after I reconnected and I had to redo ADMIN, SOURCE RTG, K. One time the line was gone with no message at all ("[not connected]").
6. **M** After a successful power-up the login screen repeats the old boot text ("SOLAR BUS ONLY", "NOT RESPONDING", "You have no account and the ship has no power for you") above "Your account is made". The screen contradicts itself.
7. **L** Two styles of input in one product. The ship console wants whole words and ENTER (SOURCE RTG, CREW, QUIT). The board menus act on one letter. Both styles are stated on screen, so I coped once I was inside.
8. **M** Node chat. The main menu says "[C] Chat: SysOp, callers". C took me straight to ELIZA with three numbered answers and "Q leaves." I saw no choice to talk to other callers, the help screen does not mention it, and Who is online showed only me live (the others idle "28y"). I gave up on task 8. Chatting with ELIZA worked fine and Q left cleanly.
9. **M** The help screen says "X the main menu, from anywhere". In the Diagnostics door (`QUERY>` prompt) I typed X and ENTER and got "I have no query with that code." Typing QUIT worked, and that prompt does say "Enter a query code, HELP, or QUIT." So I recovered, but "from anywhere" is not true there.
10. **L** On the bulletin prompt an ELIZA scene appeared on top of "Bulletin #, then ENTER". After "[any key]" the bulletin list came back and my next entry worked. Odd, but harmless.
11. **L** The main menu art has an empty "COMMANDS" box, and the commands are printed under the picture instead of inside it.
12. **L** Later on the main menu changed: a new "[R] Crew manifest" appeared and every other letter moved to a new place in the grid. The letters stayed the same, so no harm.
13. **L** My reply became "Message #118" although the area says "30 messages". The list jumps from 29 to 118.
14. **L** The footer under a message is cut at the right edge: "... X main  K kill  E". I could not see what E does.
15. **L** G logs off at once, with no "are you sure". That was fine for me, but one wrong key ends the call.
16. **L** (my own fault) In task 5 I typed my lines faster than the board answered, and sent one /S too many. The board said "Not a key here. ? shows the keys." That message was clear and both lines were saved.

Pleasures:
- The main menu line "Press a letter. No ENTER needed. ? shows the keys, X comes back here." This is exactly what I needed.
- Prompts that want a number say so: "Bulletin #, then ENTER (Q goes back at once)", "Message #, then ENTER". No guessing.
- Every reading screen has the same footer (SPACE next, Q back, X main). X from a file and from a message went straight to the main menu.
- The reply editor tells you /S saves and /A aborts before you type a word.
- Snake moved on one key and Q quit back to the doors list.
- The help screen is short and plain.

## 3. System Usability Scale

Answers (1 = strongly disagree, 5 = strongly agree):

1. I think I would like to use this system frequently: 2
2. I found the system unnecessarily complex: 4
3. I thought the system was easy to use: 3
4. I would need the support of a technical person: 4
5. The functions were well integrated: 3
6. There was too much inconsistency: 4
7. Most people would learn it very quickly: 2
8. I found the system very cumbersome: 3
9. I felt very confident using the system: 2
10. I needed to learn a lot before I could get going: 4

Odd items (answer minus 1): (2-1)+(3-1)+(3-1)+(2-1)+(2-1) = 1+2+2+1+1 = 7
Even items (5 minus answer): (5-4)+(5-4)+(5-4)+(5-3)+(5-4) = 1+1+1+2+1 = 6
Sum 13, times 2.5 = 32.5

SUS: 32.5

Once inside, the menus would score far higher. Getting an account pulls the score down.

SEVERE: yes
