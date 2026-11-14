# UX round 3: Ulf

Tester: Ulf (persona). Handle tried: UlfThree. Board: staging, Telnet, 127.0.0.1:12323.
Another tester may have been on the board at the same time.

I never got past the login. After about 108 keyboard calls I stopped, because the
rules allow about 150 and nothing else on the list can be done without an account.

## 1. Tasks

| # | Task | Result | Calls |
| --- | --- | --- | --- |
| 1 | Connect, make an account, reach the main menu | not done | ~108 |
| 2 | Read a bulletin | not done (blocked by 1) | 0 |
| 3 | Find and read a message | not done (blocked by 1) | 0 |
| 4 | Reply to that message | not done (blocked by 1) | 0 |
| 5 | Post a new message | not done (blocked by 1) | 0 |
| 6 | List files, open and read one | not done (blocked by 1) | 0 |
| 7 | Open a door and leave it | not done (blocked by 1) | 0 |
| 8 | Node chat, say something, leave | not done (blocked by 1) | 0 |
| 9 | Find the help screen | not done (blocked by 1). The SHIP> console's HELP did work. | 0 |
| 10 | Back to main menu from deep | not done (blocked by 1) | 0 |
| 11 | Log off | not done. I hung up with the tool; no logoff key was ever on screen. | 1 |

## 2. Trouble log

1. **M.** The first screen says "You have no account... work out how to fix the power
   and make an account. Start with the service label." I typed my own handle, then
   NEW. Both gave "Unknown caller". There is no "type NEW" hint. I only got further
   when I used the label login ADMIN.
2. **L.** The first time, typing ADMIN at Login took me straight to the SHIP console
   without asking for a password. Later the same ADMIN asked for "Password:". The same
   login behaved differently on different connections.
3. **S.** At the RTG warning, the screen says "TYPE [K] AND PRESS ENTER TO ACKNOWLEDGE."
   I did exactly that more than ten times, in every way I could think of: K plus ENTER
   in one go, K alone, then ENTER on its own, lowercase k, and waiting 8 to 10 seconds
   before typing. The usual answer was "UNKNOWN COMMAND - TYPE HELP" back at SHIP>, as
   if the ACK prompt was already gone before my K arrived. Sometimes I got a bare
   "SHIP>" and STATUS still said "SOLAR DIRECT". The prompt never once confirmed the
   acknowledgement.
4. **S.** Typing a line with ENTER and typing the same text and then pressing ENTER
   gave different results. With "SOURCE RTG" plus ENTER in one go, the board often
   answered "SWITCH ABORTED. NO CHANGE." straight away and drew no warning screen, as
   if a second, empty ENTER had answered the ACK prompt for me. When I typed the text
   first and pressed ENTER as a separate key, the warning screen appeared every time.
   The login and password prompts showed the same split: one way worked and the other
   skipped a prompt.
5. **S.** The replies I saw often did not belong to what I had just typed. Some examples:
   - "ADMIN" at the Password prompt answered "UNKNOWN COMMAND - TYPE HELP".
   - "EXIT" answered "SWITCH ABORTED".
   - "POWER" answered "SWITCH ABORTED".
   - "Turku" at Location answered "Unknown caller... Login:".
   - The screen showed "Login:", I typed my handle, and the reply was "SHIP>".
   - "No account made." turned up one step after I had already moved on.

   I could never tell which prompt I was really at.
6. **M.** The power state did not stay put. At one point SOURCE RTG answered "RTG
   ALREADY ON THE BUS", but STATUS and the console art still said SOLAR DIRECT, and
   the next login showed the unpowered boot screen again. This may be the other
   tester, but from my chair the power switch I had been told to fix kept undoing
   itself.
7. **M.** Once, EXIT from the console led to "Handle unavailable." and then "Handle
   (3-16 chars, ENTER cancels):". I never typed a handle that could be taken, so I
   don't know which handle it meant. The handle prompt only showed up after EXIT,
   never at Login, so it was hard to find again. I typed UlfThree and got to the
   Password prompt, then lost the flow. One cause was my own mistake: I pressed a
   stray ENTER, which the prompt promised would cancel, and it printed "No account
   made". Another time, two K plus ENTER tries later, I was thrown back to Login.
8. **L.** One K plus ENTER at the ACK prompt dropped the connection ("[not
   connected]") with no message.
9. **M.** No step ever told me I was "in" or showed a main menu. The only menu-like
   screen was the SHIP console, where everything is a typed command. For someone who
   likes "press a letter" menus, that is homework.

Pleasures:
- The SHIP console's HELP gave a short, clear list of commands at once.
- The RTG warning art and the boot screen look good, and the ACK screen states its
  rule plainly. It just did not work for me.

## 3. SUS

Answers (1 = strongly disagree, 5 = strongly agree):

1. I would like to use this system frequently: 1
2. Unnecessarily complex: 4
3. Easy to use: 1
4. Need a technical person: 4
5. Functions well integrated: 2
6. Too much inconsistency: 5
7. Most people would learn it quickly: 1
8. Very cumbersome: 5
9. Felt very confident: 1
10. Needed to learn a lot first: 4

Arithmetic:
- Odd items (answer minus 1): 0 + 0 + 1 + 0 + 0 = 1
- Even items (5 minus answer): 1 + 1 + 0 + 0 + 1 = 3
- Total: 4 x 2.5 = 10

SUS: 10

SEVERE: yes
