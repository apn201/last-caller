# Tasks - Scene engine (queue item Q3)

Requirements: `requirements.md`. Design: `design.md`. Wording of all
caller-visible text is a draft until the author's final edit.

## Group 1 - Portable logic

- [x] 1. `src/core/scene_logic.h`: parser and check, `Typer`, `Player` (R1, R2, R3)
- [x] 2. Host-side tests, `tools/test/scene_logic_test.cpp`, run with `zig c++`

## Group 2 - Board side

- [x] 3. `State SCENE_PLAY`, `Session.scene`, `HauntShow.scene`; `scene=` in
  the trigger rules (R4.1)
- [x] 4. `src/core/scene.h`: start, tick, input, end; door-table entry (R2, R4)
- [x] 5. `pio run` passes. Server build not run: this PC is Windows and Q1
  is blocked

## Group 3 - Demo scene

- [x] 6. Art `tools/ansi/screens/scenes.py`: ELIZA's ring waking, rendered to
  PNG and looked at (R5.2)
- [x] 7. `STORY/SCENES/ELIZA_WAKE.INI` and the trigger that starts it
  (draft text)
- [ ] 8. Board test (on the stick or the server, after Q1): sign up, see
  the scene on the first calm prompt, try Enter during typing, each choice,
  a re-login (no second play), over Telnet and in the browser

## Group 4 - Close

- [x] 9. `docs/archive/SLICES.md`, `docs/MENU_AUDIT.md` notes; tick Q3 in
  `docs/briefs/QUEUE.md`
