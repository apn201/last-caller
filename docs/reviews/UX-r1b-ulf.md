# UX round 2: Ulf

About 62 tool calls.

## 1. Tasks

| # | Task | Result | Calls |
|---|------|--------|-------|
| 1 | Connect, make account, reach main menu | done with trouble | ~24 |
| 2 | Read a bulletin | done | 5 |
| 3 | Find and read a message | done | 4 |
| 4 | Reply | done with trouble | 5 |
| 5 | Post a new message | done | 7 |
| 6 | List files, open and read one | done | 5 |
| 7 | Open a door, leave it | done | 3 |
| 8 | Node chat, say something, leave | done with trouble | 5 |
| 9 | Help screen | done | 1 |
| 10 | Back to main menu from deep | done (X from a file, a door menu and a message) | 3 |
| 11 | Log off | done | 1 |

## 2. Trouble log

1. S. Login screen has no way to make an account. I tried `new`, `NEW`, `?`, `ulf`, `guest`: each said "Unknown caller" and pointed at the service label. The only visible way forward was to use the ADMIN/ADMIN label. HELP in the console lists no register command. I only found registration after SOURCE RTG, then K, then CREW. I would have left. A normal caller has no hint that registering is hidden behind this.
2. M. SOURCE RTG asks "TYPE [K] AND PRESS ENTER" then "ANY OTHER ENTRY ABORTS". I did not know whether this was the account step or a risk. Clear enough once read, but scary on a first call.
3. M. After "Account created. Log in with it." the login screen comes back unchanged, still advertising ADMIN. Handle then password is fine, but I had to guess I should type my own handle.
4. M. After login, a long ELIZA scene with a numbered menu (1/2/3) appears before the main menu. A line I typed during it vanished (I sent "ulf" and got "Password:" back, probably my own parallel-call mistake, but a key typed during the scene did not obviously land anywhere). The "[any key]" at the end then needed a second key press to reach the menu.
5. M. Bulletin list: "Bulletin #, then ENTER (Q = back)". Number plus ENTER is stated, fine. But the ELIZA guest-account scene popped up over it on first entry and ate my next key until [any key].
6. M. Saving a reply ("/S") showed "Message #118 posted. Press any key:". I pressed a key and got blank. I then pressed `?` and got nothing for a few seconds. ENTER finally redrew the message. Two keys seemed to do nothing. Saving a new message (#119) went straight back to the menu with no "Press any key", so the two saves behave differently.
7. L. ELIZA says "If you need anything, type C and I will be there." at the bulletin screen. C is node chat on the main menu, not on that screen.
8. L. "Node chat" first opens an ELIZA conversation with 1/2/3 numbered choices where Q leaves with no ENTER. Choosing 1 turns it into a line chat where `/Q` plus ENTER is needed. Both say how to leave, but the key differs (Q vs /Q). It told me when entering, so this is only odd.
9. L. In the Snake door, Q alone quit with no ENTER although the prompt looks like a line prompt "Moves (W A S D, Q quits)>". Good surprise.
10. L. The file list says "[#] Open (number, ENTER)  [V] View". I pressed V, then a number plus ENTER. Worked; V needing a second prompt for the number is a little slow.
11. Pleasure: the `?` keys screen is short and says plainly "A letter works at once. You do not press ENTER." That answered my main worry. X to the main menu worked everywhere I tried. The main menu lists every key on one screen. Snake was fun to open.

## 3. SUS

Answers:
1. 3
2. 3
3. 3
4. 3
5. 4
6. 2
7. 2
8. 3
9. 3
10. 3

Odd items: 2+2+3+1+2 = 10. Even items: 2+2+3+2+2 = 11. Total 21 x 2.5.

SUS: 52.5

SEVERE: no
