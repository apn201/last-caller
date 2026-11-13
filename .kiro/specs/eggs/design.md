# Design - Easter eggs and nerd stuff

## Pieces

| Piece | File | Notes |
| --- | --- | --- |
| Modem command parser | `src/core/egg_logic.h` | pure logic, no String; host test `tools/test/egg_logic_test.cpp` |
| Snake rules | `src/core/snake_logic.h` | pure logic: board, steps, food, score; host test `tools/test/snake_logic_test.cpp` |
| Egg commands | `src/bbs/eggs.h` | `eggCommand(Session&, line, cmd)`, called first in `handleLine` for the four prompt states |
| Snake door | `src/doors/snake.h` | `enter`, `owns`, `handle`; state `SNAKE_PLAY`; one line in `DOORS` |
| Text | `sd/BBS/STORY/EGGS.INI` | modem strings, nodes (number, art, text), the POST lines, fortunes, snake table |
| Art | `tools/ansi/screens/eggs.py` | `ANSI/DOORS/POST`, `SNAKE`, `NODE_LUMEN`; `ANSI/STORY/TY_DEMO_1..3`; ASCII twins |
| ELIZA | `sd/BBS/STORY/SYSOP.INI` | chat `keys=` lines for the film requests |
| Files | `sd/BBS/FILES/...` | Y2K memo and price list (Records), brochure and nodelist (Library), `TYDEMO.NFO` (Ty's Desktop) |
| Demo | `sd/BBS/STORY/SCENES/TY_DEMO.INI`, `TRIGGERS.INI` rule `EV_TYDEMO` | opening `TYDEMO.NFO` starts the scene |

## Where the commands hook in

`handleLine` calls `eggCommand` before the normal dispatch, only in these
states: `LOGIN_CHOICE`, `LOGIN_HANDLE`, `MAIN`, `DOOR_MENU`, `BULLETIN` (the
"Press ENTER" prompt). Everywhere else (password entry, message editor, chat,
file commands, doors, scenes, the ship console) a line is what it always was.
`eggCommand` returns true only if it answered; it then writes the prompt of
the state again, so the caller is where they were.

Modem commands work in all five states. `XYZZY`, `FORTUNE`, `POST` and
`REBOOT` only in `MAIN`.

`ATH0` sets the state to `GOODBYE` like Goodbye does; the session closes when
the output has drained. A crew (read-only) login simply hangs up.

## Why nothing here can shortcut the game

The eggs write nothing: no flag, no user record, no story record. `eggs.h`
does not include the haunt or manifest code. The demo is an ordinary scene
started by an ordinary trigger rule that only has `scene=`, so its only
trace is the engine's own markers (R1.1). Snake is a door that saves nothing
and is listed with `saves=false`. The prologue's maintenance login never
reaches these states except the login prompt, where the commands only talk
or hang up.

## Dialling

`STORY/EGGS.INI`:

```
[MODEM]   ok=, error=, nocarrier=, busy=, noanswer=, nodialtone=, connect=, id0=..id9=
[NODE_X]  number=5550172  art=NODE_LUMEN  text=...
```

A node is any section whose name starts with `NODE_`. Digits are compared
after dropping spaces, dashes, commas and brackets. The node's art is
`ANSI/DOORS/<ART>.ANS` (`ASCII/DOORS/<ART>.ASC` for plain callers).

## Snake

A 30 x 11 station, the head `@`, the body `o`, food `*`, hull `#`. The
caller types a line of `W A S D`; every letter is one step in that direction,
a reversal into the neck is ignored, an empty line repeats the last
direction once. The game logic has its own xorshift generator seeded from
`bbsRandom()`, so the test is deterministic. Eating adds 1 to the score and
a segment. The table is the crew's scores from `[SNAKE]` in `EGGS.INI` plus
the caller's, sorted; the caller is shown as `YOU`.

## ELIZA

The film requests are keys in `SYSOP.INI` (chat `keys=`, answered once,
unquoted). Their text never repeats the request.
