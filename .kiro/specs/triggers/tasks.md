# Tasks - Triggers (queue item Q2, slice S5)

Requirements: `requirements.md`. Design: `design.md`. Wording of all
caller-visible text is a draft until the author's final edit.

## Group 1 - Portable logic

- [ ] 1. `src/core/haunt_logic.h`: rule table, parser from plain triples,
  `due()`, flag helpers (R1, R2, R4, R6)
  - [ ] 1.1 Flag helpers: `flagsHas`, `flagsAdd` (upper case, no repeats,
    cap 255, reports what did not fit)
  - [ ] 1.2 Parser: sections, events, numbers, caps, 300 rules, skip counter
  - [ ] 1.3 `due()`: events, match, baud, amount, has, lacks, level, spent
- [ ] 2. Host-side tests, `tools/test/haunt_logic_test.cpp`, run with
  `zig c++` (R4, R6)

## Group 2 - Board side

- [ ] 3. Raise the flag cap to 255 in `cleanFlags` (R4.4)
- [ ] 4. `src/core/haunt.h`: load `TRIGGERS.INI`, caller context, apply
  rules, sync sessions, effects (R3, R4, R5)
- [ ] 5. `Session.ownUserId`, `Session.carriedFlags`; `finishLogin` sets and
  merges them (R5)
- [ ] 6. Hooks: login, crew_login, read (with `add=`), file, art, door,
  idle, minutes (R1)
- [ ] 7. `STORY/TRIGGERS.INI` with the format and two inert examples
- [ ] 8. `pio run` passes; server build not run (Windows); note in docs

## Group 3 - Close

- [ ] 9. Update `docs/SLICES.md` S5, `README`/`docs/MENU_AUDIT.md` if a menu
  changed (none expected), tick Q2 in `docs/briefs/QUEUE.md`
