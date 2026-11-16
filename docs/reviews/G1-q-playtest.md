# G1 Q-skip playtest (short run)

Telnet only, through `bbs-caller` on the staging board at 127.0.0.1:12323.
The tool has no browser mode, so I could not test the browser behaviour.
About 80 tool calls. Accounts: probe1, probe2. I did not read any story
files.

## Scenes covered

| Scene | Reached | Q during typing | Q at numbered choice | Key leak |
| --- | --- | --- | --- | --- |
| ELIZA_WAKE | yes, twice | not tested (see below) | refused, dim line shown | none |
| TY_DESK (Ty's account scene) | yes | not tested, scene had already finished typing | none in this scene | none |
| Guest-account scene (first command after WAKE) | yes, twice | **worked** | none in this scene | none |
| MEM_BAY | **no** | - | - | - |

### ELIZA_WAKE
- probe1: the scene started straight after the welcome bulletin's
  "Press any key", before I could type anything. Both art frames and the
  text typed out, then the numbered reply. I pressed `q` at the choice and
  got the dim line `Q skips the talk, not the choice. Pick a number.`
  The prompt stayed open. `3` then worked normally.
- probe2: I pressed SPACE at the bulletin, then `7`, `q`, `x`, `7`, `f`
  without Enter. **The scene did not start.** The Main Board showed nothing:
  no echo and no reaction for about 30 s. When I pressed ENTER the board
  said `Unknown command. Type ? for help.` and *then* ELIZA_WAKE played
  in full. So `7` opened a number entry at the main menu, as the interaction
  rules say it should. It then took `q`, `x` and `f` silently into that line.
  The scene seems to wait for that line to end. The caller sees a frozen
  menu with no prompt and no echo. My stray key started this, but a real
  caller can do the same thing.
- At the choice I pressed `q` again and got the same dim line. `1` worked.
  At `[any key]`, `q` worked as the any key and went to the Main Board.

### Guest-account scene ("MISSION AI GUEST ACCOUNT")
- It plays on the first command after WAKE. Both times the key that started
  it (`x` in the Library, meant to go to the Main Board) was used up. After
  the scene the caller is still in the Library, not on the Main Board.
- probe2: while it typed I pressed `7`, waited about 0.5 s, then `q`. Q
  ended the scene in the middle of a sentence. The line `crew list.` was
  printed, then `+---- Library ----+` came right under it with no blank
  line between. The picture had already drawn in full before Q. After the
  scene: `File command?`, then the grey
  `(Keys typed during the scene were not used.)`, then `File command?` a
  second time. The `7` did not leak.

### TY_DESK
- Reached by logging in as TYVANCE (the password comes from the journal
  and the bulletin). The scene played on `j` (Conferences) from Ty's main
  menu, and that `j` was used up. It had finished typing before I could
  press anything, so I could not test Q during the typing. At `[any key]`,
  `q` went on to the **Message Bases** menu, not the Conferences menu.
  That is the menu I had left just before (`m`, `q`, `j`), so the board
  redrew an old screen. The grey "keys not used" line appeared and
  `Message command?` was printed twice, as above. Nothing leaked.
- Side issue: after the TYVANCE login I typed the password a second time by
  mistake. It ended up as a new `Login:` handle (`TYVANCE` then
  `Password:`). The board was already showing "Press any key", so this
  came from my own extra input, not from a board bug.

### MEM_BAY
I did not reach it. Within the call budget I found no Bay 3 post or
POWER.LOG in the Lounge, the Library or Ty's Gallery as probe1. Untested.

## Findings
1. A digit at the Main Board starts a silent number entry. The letters typed
   after it get no echo, and a pending scene waits until ENTER. To the
   caller the board looks hung.
2. The key that starts a scene is used up (`x` stayed in the Library; `j`
   ended on Message Bases). The caller does not get the screen they asked
   for.
3. After Q skips a scene, the redraw follows the cut line with no blank
   line. The prompt then appears twice, around the grey line.
4. The dim line at a numbered choice is correct and clear. Q never picked
   an option.
5. No stray key reached the next prompt in any scene tested.
6. Not tested: Q while a picture is drawing, Q in MEM_BAY, the browser.
