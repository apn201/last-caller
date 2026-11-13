# Tasks - Triggers (queue item Q2, slice S5)

Requirements: `requirements.md`. Design: `design.md`. Wording of all
caller-visible text is a draft until the author's final edit.

## Group 1 - Portable logic

- [x] 1. `src/core/haunt_logic.h`: rule table, parser from plain triples,
  `due()`, flag helpers (R1, R2, R4, R6)
  - [x] 1.1 Flag helpers: `flagsHas`, `flagsAdd` (upper case, no repeats,
    cap 255, reports what did not fit)
  - [x] 1.2 Parser: sections, events, numbers, caps, 300 rules, skip counter
  - [x] 1.3 `due()`: events, match, baud, amount, has, lacks, level, spent
- [x] 2. Host-side tests, `tools/test/haunt_logic_test.cpp`, run with
  `zig c++` (R4, R6): flags only grow (property), idempotence, no chaining,
  every event and condition, 24 malformed rules, limits, 3000 fuzz rounds,
  and the card's own `TRIGGERS.INI` when given as an argument

## Group 2 - Board side

- [x] 3. Raise the flag cap to 255 in `cleanFlags` (R4.4)
- [x] 4. `src/core/haunt.h`: load `TRIGGERS.INI`, caller context, apply
  rules, sync sessions, effects (R3, R4, R5)
- [x] 5. `Session.ownUserId`, `Session.carriedFlags`; `finishLogin` sets and
  merges them (R5)
- [x] 6. Hooks: login, crew_login, read (with `add=`), file, art, door,
  idle, minutes (R1); crew Goodbye back to the login prompt (R3.8)
- [x] 7. `STORY/TRIGGERS.INI` with the format and Ty's visit (draft text)
- [x] 8. `pio run` passes. Server build not run: this PC is Windows and Q1
  (the Docker build) is blocked
- [ ] 9. Board test (on the stick or the server, after the C1 crew
  accounts exist): log in as TYVANCE, Goodbye, sign in as yourself, find
  ELIZA's private mail in Main Board; read it twice, get it once; check
  `T_TY_VISIT_MAIL`, `CREW` and `VISIT_TY` on your record in the user editor

## Group 3 - Close

- [x] 10. `docs/archive/SLICES.md` S5 status, `docs/MENU_AUDIT.md` (crew Goodbye),
  tick Q2 in `docs/briefs/QUEUE.md`
