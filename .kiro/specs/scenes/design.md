# Design - Scene engine

Requirements: `requirements.md`. Same split as triggers and power: pure logic
in a header a plain C++ compiler tests, a thin board side.

## Files

| File | Role |
| --- | --- |
| `src/core/scene_logic.h` | Pure. Scene parse and check, `Typer` (typing with pauses and word wrap), `Player` (the state machine: tick, input, flags). `std::string` only. Test: `tools/test/scene_logic_test.cpp` |
| `src/core/scene.h` | Board side: loads a scene file, starts a run for a session, writes the player's output, saves flags. Included after `core/haunt.h` |
| `sd/BBS/STORY/SCENES/*.INI` | The scenes |
| `sd/BBS/ANSI/STORY/*.ANS`, `ASCII/STORY/*.ASC` | Their pictures, from `tools/ansi/screens/scenes.py` |

`Session` gets `State SCENE_PLAY` (last in the enum) and a `scene::Player
scene` member; `core/session.h` includes `scene_logic.h` first, as it does for
`haunt_logic.h` through `bbs_core.h`.

## Scene file

```
[SCENE]
title=ELIZA wakes          ; log line only, never shown
start=START                ; default START
speed=32                   ; characters per second, 5 to 200, default 30
has= lacks=                ; conditions to start
set=FLAG,FLAG              ; added when the scene ends (it ran to its end)
repeat=0                   ; 1 = may be played again

[NODE_ID]                  ; letters digits _, at most 24
speaker=ELIZA              ; shown in front of the text, max 16
text=Line one.|Line two.   ; "|" breaks a line, max 600
art=NAME                   ; picture shown before this node's text
pause=900                  ; ms after the text, default 700, max 10000
key=1                      ; wait for Enter instead of the pause
set=FLAG                   ; added when the node is entered
go=NEXT                    ; next node; none and no choices = the scene ends
choice1=Answer text        ; up to four
go1=NODE  set1=FLAG  has1=FLAG  lacks1=FLAG
if_has=FLAG if_lacks=FLAG if_go=NODE   ; redirect on entry
```

## Player

`Player::start(scene, flags, now)`, `tick(now) -> Out`, `input(line, now) ->
Out`. `Out` holds `text` (to write), `art` (a picture to show before it),
`grant` (flags to save) and `finished`. Phases: TYPE -> (CHOICE | KEY | PAUSE)
-> next node or END. The player keeps its own copy of the flags (the caller's
flags plus everything it granted), so redirects and choice conditions see
choices made earlier in the same scene.

Entering a node: follow redirects (at most 16, a loop ends the scene), grant
the node's `set`, show its art, start typing `speaker + text`. At 200 entered
nodes the scene ends.

`Typer` emits characters whose time has come. Each costs 1000/speed ms;
sentence ends cost 380 ms more, commas 120 ms, a line break 520 ms. A gap
longer than 250 ms is not caught up (the stream does not burst after a slow
tick). `flush()` returns the rest at once. Word wrap is decided at each
space by looking at the next word.

## Board side

- `sceneStart(s, name)`: name is checked (letters, digits, `_`), the file is
  read (at most 24 KB), parsed, checked against the caller's flags and
  marker, the marker is saved with `hauntGrant`, then the player starts and
  `s.state = SCENE_PLAY`. Any failure is an event-log line (`SCENE_SKIP`) and
  the caller sees nothing.
- `ownsScene` = `state == SCENE_PLAY`; the door table has an entry with no
  menu key and `listed` false, like the ship console, so `handleLine` routes
  lines to `handleScene`.
- `sceneTick(now)` from `bbsTick` after `hauntTick`: for each session in
  `SCENE_PLAY`, `player.tick(now)`, write output, save grants, end.
- Ending: `s.scene = Player()`, `mainMenu(s)`. A hung-up session is dropped
  by `closeSession`; the player dies with the session slot, no flag is lost
  (flags are saved as they are granted).
- The art goes through `art()` (ANSI) or `plainArt()` (ASCII twin). Text
  written after a picture queues behind it, so a picture never lands in the
  middle of a sentence.
- Flags are saved by the trigger engine's `hauntGrant`, so a crew login's
  scene flags wait in `carriedFlags` like any other.
- A trigger's `scene=NAME` is carried in `HauntShow.scene`; `hauntShow` starts
  it when the caller is calm.

## Decisions and limits

- No per-caller save of the position in a scene: a scene is short, a lost
  connection replays it only if `repeat=1` (the marker is set at the start).
  Fail-safe: at most once, never twice.
- Choice input is a line (digit and Enter) on both transports. Keeping line
  input means the browser needs no new key mode.
- Typing is paced by the tick, not by the modem-speed pacer, so a scene reads
  the same at any baud setting.
- The scene engine does not clear the screen. Art files start with
  `ESC[2J ESC[H` so a node with `art=` starts a fresh screen.

## Not in this item

A scene list or replay menu, memory-scene unlock rules (Q7, Q8), sound cues
(Q10), saving a position in a long scene.
