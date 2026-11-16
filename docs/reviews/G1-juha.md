# G1 - Juha's test, 16 Nov 2026 (public board, G1 card)

Written down by Claude Code from Juha's message.

## Findings

1. **"q did not skip cutscene."** Confirms the G1 agent's unconfirmed finding
   (a Q that did not stop the Bay 3 recording). Must be fixed before G2 is
   ticked.

Which scene: "It was Ty and ELIZA", so the Bay 3 recording (`MEM_BAY`, Ty
and ELIZA talking, no choices), the same scene the G1 agent named. Possibly
`ELIZA_WAKE` too (it has a numbered choice).

2. "Mostly works." Could not release the queue in NOWHERE: two of the four
   manifest rows were still locked (expected, M_Z missing). But "I do not
   understand the story behind it: why would NOWHERE care about the
   manifest?" **For G3 (story design):** the hold must make sense on its own.
   Planner's proposal for Juha: the 41 messages are the crew's last letters
   home. A rule of the mission (and of real crewed flight) holds personal mail
   from a crew in an incident until next of kin are notified, and next of kin
   are notified from the confirmed crew manifest: who was aboard, and what
   happened to each. ELIZA may not file it (order 5); the player does, by
   making the manifest whole. The queue header, `help_held`, `final_held`,
   MANFULL and one crew post say it in those words ("No personal mail leaves
   before the families are told. The families are told from the manifest.").
   Juha decides in G3.

## Planner's read of the code (for Kiro; verify, do not trust)

Lead suspect, for MEM_BAY: the scene opens with `art=MEM_BAY`.
`sceneWrite` draws it with `art()`, a paced stream, so `streaming(s)` is true
while it draws, and `input()` in `src/bbs/commands.h:34-49` hands every key to
the stream: Q only skips the picture (SPACE pauses it) and never reaches the
scene. Meanwhile the player keeps typing on its own clock behind the picture.
In SCENE_PLAY, Q must go to the scene even while its art streams (skip the
picture and the scene together).

Other suspects, in order:

- `src/bbs/commands.h:75`: a Q becomes the skip key only while
  `s.typeAhead` is empty. Any other key pressed earlier in the same scene
  (an ENTER does not count, but a letter, a space or a digit does) goes into
  `typeAhead`, and from then on every Q is added to it too. One stray key and
  Q is dead for the rest of the scene. The "word typed ahead" guard should
  look only at the keys right before the Q (the HOTKEY_MS window), not at the
  whole scene.
- `src/core/scene_logic.h:325`: at a numbered choice Q does nothing. That is
  by design (the choice must be made), but the caller cannot tell. At a
  choice, Q should at least answer in a dim line ("Q skips the talk, not the
  choice. Pick a number.") or skip to the choice's default if one exists.
- If the Bay 3 recording (or any other "cutscene") is played as a paced file
  stream or by the reader rather than by `scene::Player`, Q goes to the
  stream's key handling (pause/skip a page) and not to the scene. Every
  cutscene the caller sees must stop on Q the same way.

## Test

A host test per suspect: Q after a stray key; Q at a choice; Q during each
kind of cutscene (scene, recording, ghost page). The playtester presses a
random key early in a scene and then Q, in the browser and over Telnet.
