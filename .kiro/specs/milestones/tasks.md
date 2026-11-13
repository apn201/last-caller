# Tasks - Milestones

Requirements: `requirements.md`. Design: `design.md`. Content drafts:
`content-drafts.md`. Chain: `docs/PUZZLE_CHAIN.md`.

## Q12c - design (this item)

- [x] 1. Story bible: Ty's last fight, fates V-F and V-C, timeline, chain
  M1 to M9, scenes, matrix E41 to E48, act gates audit
- [x] 2. `docs/PUZZLE_CHAIN.md`
- [x] 3. This spec (requirements, design, tasks, content drafts)
- [x] 4. Story lint passes on the current card (no card change in Q12c)

## Q12d - build (next item; order matters, small commits)

- [x] 1. Verify in a host test or by reading `haunt.h` that an ELIZA-style
  crew session reads the caller's own flags (`hauntOwner`), for the door check
  in R6.4. Record the answer in the commit message.
- [x] 2. Flags and gates: `messageVisible`, `NEEDS.BBS`, `storyPass`, the
  protected list; host tests (R2, R3). No card change yet.
- [x] 3. Lint: read `Needs:`/`NEEDS.BBS`, the chain table, the protected-flag
  check; property test for flags that only grow and a story that never sets a
  protected flag (R8.1, R8.2).
- [x] 4. Card: `Needs:` lines per the story bible audit, `ACT2`/`ACT3` in the
  visit triggers; lint and e2e still pass with the old chain.
- [x] 5. Overrun: `core/overrun_logic.h`, host test, the post path, `MEM_GLITCH`,
  `OVR`, the event log line (R4).
- [x] 6. Chat scene: `then=CHAT`, `ELZ_TALK`, `ELZ_GRANT`, `SYSVW`; `SYSOP.INI`
  lines with `has=OVR` (R5).
- [x] 7. ELIZA's account: password in `ACCOUNTS.INI`, `VISIT_ELIZA`, mail,
  ops log, `ELZ_IN`, `ELZ_OPS`; door key `account=` and the shell moved into
  her menu (R6).
- [x] 8. Card content from `content-drafts.md`: E41, ELIZA's reply, E42, the
  Bay 3 postscript, E43 (CMDR only, `EV_HANDBOOK` sets `CURE`), ENGINEERING with
  `GOLIVE.TXT` and `STANDING.ORD`, the board manual page 3 "limits" line, the
  Day 21 note, two more small wrong notes, logs, typescript, `MEM_BAY` (R1, R7).
- [x] 9. ANSI art for the five new screens, rendered to PNG and looked at.
- [x] 10. Re-run the matrix check on the real card (story bible C9) and update
  its tables.
- [x] 11. e2e plays the new chain, the 19-zero near miss, an early SYSOP
  request, a gated post read through SYSOP view, the ELIZA login and the shell
  from her door (R8.3). `docs/TESTING.md`, `docs/MENU_AUDIT.md` updated.
- [x] 12. `pio run`, host tests, `python tools/story_lint.py`, staging check PASS.
- [x] 13. `playtester` run 5 from a brand-new caller; `docs/PLAYTEST.md` with
  play time against the 60 to 120 minute goal.
- [ ] 14. Board test by hand: `docs/TESTING.md` "Milestones". (Not done: needs the stick and a person.)
