# Tasks - Ghosts and ELIZA's voice (queue item Q4)

Requirements: `requirements.md`. Design: `design.md`. Wording of all
caller-visible text is a draft until the author's final edit.

## Group 1 - Scene echo (Q3 review)

- [ ] 1. `Player::wantsEcho()` in `scene_logic.h`, tests (R6)
- [ ] 2. `input()` drops and does not echo keys while a scene is not at a
  choice (R6)

## Group 2 - Portable logic

- [ ] 3. `src/core/ghost_logic.h`: parsers, visibility, keyword matching, idle
  text, typing times (R1, R2, R4, R5, R7)
- [ ] 4. Host-side tests, `tools/test/ghost_logic_test.cpp`, run with `zig c++`

## Group 3 - Board side

- [ ] 5. `Session` fields, `signedAt` set in `finishLogin`
- [ ] 6. `src/core/ghost.h`: load, WHO rows with an idle column, pages,
  trigger lines, chat queue and tick; hooks in `settings.h`, `haunt.h`,
  `chat.h`, `runtime.h`, `sysop.h` (R1 to R5)
- [ ] 7. `pio run` passes. Server build not run: this PC is Windows and Q1
  is blocked

## Group 4 - Content (draft text)

- [ ] 8. `STORY/GHOSTS.INI`: CMDR, CHAOS and GARDEN after 5 minutes (E03),
  TYVANCE in the power bay (E34), ELIZA at node 0 with her page inside 30
  seconds and in chat (E04)
- [ ] 9. `STORY/SYSOP.INI`: join line, keywords for what she is, a fallback,
  a line keyed by `TY_VISIT_MAIL`
- [ ] 10. Board test (on the stick or the server, after Q1): WHO shows the
  ghosts and the idle column, ELIZA pages inside 30 s, chat answers with
  delay and does not break a half-typed line, a key typed in a scene is not
  echoed; over Telnet and in the browser

## Group 5 - Close

- [ ] 11. `docs/SLICES.md`, `docs/MENU_AUDIT.md` notes; tick Q4 in
  `docs/briefs/QUEUE.md`
