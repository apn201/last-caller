# Requirements - Scene engine

Queue item Q3 in `docs/briefs/QUEUE.md`; the idea is in `docs/STORY_EXTRAS.md`
("Visual novel scenes"). Triggers (`.kiro/specs/triggers/`) start scenes.
All caller-visible wording in scene files is a draft until the author's final
edit.

## Glossary

- Scene: one card file, `STORY/SCENES/<NAME>.INI`: a small graph of nodes.
- Node: one beat: optional art, a speaker, text that types out, then either a
  pause and the next node, a wait for Enter, or a choice.
- Choice: a numbered answer that goes to a node and may set flags.
- Caller flags: the flags on the caller's own account (as in the triggers).
- Marker: the flag `SC_<SCENE>`, set when a scene starts.

## Requirements

### R1 - Scenes are data

1. THE engine SHALL read every scene from `STORY/SCENES/<NAME>.INI`. No scene
   text, name, branch or flag SHALL be in firmware.
2. A scene file SHALL have a `[SCENE]` section (title, start node, typing
   speed, conditions, flags set at the end, repeat) and one section per node.
3. A node SHALL give: `speaker`, `text`, `art`, `pause`, `key`, `set`, `go`,
   up to four choices (`choiceN`, `goN`, `setN`, `hasN`, `lacksN`), and an
   optional redirect (`if_has`, `if_lacks`, `if_go`).
4. A scene with a broken node, a missing start node, a `go` to a node that
   does not exist or more than 64 nodes SHALL NOT load; the reason SHALL be
   in the event log and the caller SHALL see nothing.
5. THE engine SHALL ignore keys it does not know, so a newer card loads on
   older firmware.

### R2 - Playing

1. Text SHALL type out character by character at the scene's speed, with a
   longer pause after a sentence end and at a line break.
2. Words SHALL NOT be split across lines (76 columns, 2-column indent).
3. A node with `art` SHALL show that picture (ANSI/STORY/NAME.ANS, or
   ASCII/STORY/NAME.ASC for a caller without ANSI) before its text.
4. Enter during typing SHALL show the rest of the text at once. It SHALL NOT
   skip a choice.
5. After the text, a node with choices SHALL list them numbered from 1 and
   wait for a number. A node with `key=1` SHALL wait for Enter. Any other
   node SHALL continue after `pause` milliseconds.
6. A node with no `go` and no choices SHALL end the scene.
7. A scene SHALL end at once, with nothing lost, when the caller hangs up.
8. A scene SHALL NOT loop for ever: it ends after 200 nodes.

### R3 - Flags

1. A choice SHALL show only when the caller's flags satisfy its `has` and
   `lacks`. Numbers SHALL be given to the shown choices only, so the caller
   never sees a gap or an option that does not work.
2. A node with `if_has` / `if_lacks` and `if_go` SHALL send the scene to
   `if_go` at once when the condition is true (so later lines depend on
   earlier choices).
3. Flags set by a node, a choice or the scene end SHALL go to the caller's
   own account (or to the carried flags of a crew login), through the same
   path the triggers use. Flags only grow.
4. A scene SHALL start only when the caller's flags satisfy the scene's `has`
   and `lacks`, and, unless `repeat=1`, only when the caller lacks the marker.
   The marker SHALL be saved before anything is shown; if the save fails the
   scene SHALL NOT start.

### R4 - Starting a scene

1. A trigger rule SHALL start a scene with `scene=NAME`. The scene starts
   when the caller is at a calm menu prompt (as pages and art do) and the
   caller returns to the main menu when it ends.
2. A scene SHALL work the same over Telnet and the browser; it uses only
   lines of text and the existing art path.
3. The maintenance account SHALL NOT get scenes.

### R5 - Quality

1. The pure logic SHALL be in a header a plain C++ compiler tests
   (`src/core/scene_logic.h`) with host-side tests run with `zig c++`.
2. The demo scene SHALL ship with ANSI art made with `tools/ansi/`, checked as
   PNG, and an ASCII twin.
3. `pio run` SHALL pass. The server build is not run on this PC.
