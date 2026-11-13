# Tasks: refactor pass (Q15)

- [x] 1. Docs: archive `KIROWEEN_PLAN`, `SLICES`, `KIRO_REPO_STEPS`, `STORY_BIBLE_DRAFT`, `briefs/WRITING_RULES` and the done briefs S0, S1, S1b, S1c, S3a, S3b, S4, S12c to `docs/archive/`, with a header line each. Update links in specs, agents, steering and `docs/`. Evidence: `git grep` for each name before and after.
- [x] 2. Docs: `structure.md` said `FILES` and `MSGBASE` are git-ignored (they are tracked: `git ls-files sd/BBS/FILES` lists 69 files); add `deploy/`, fix the `docs/` and `tools/` rows, drop "planned".
- [x] 3. Docs: `README.md` said `sd/BBS/FILES` is ignored and `MSGBASE` is not in the repository (both tracked), and its map listed a missing `art/` folder; map rewritten. `AGENTS.md` had a lost `t` in `tools/staging-playtest.ps1` (a tab).
- [x] 4. Docs: `playtester.md` forbids reading `docs/archive/` (the old rule named `docs/SLICES.md` and `docs/STORY_*`).
- [x] 5. Dead code: `historyEcho()` and the `historyEchoes` vector in `src/core/history.h`. Evidence: `git grep historyEcho` finds only its definition; the vector was only filled and cleared. Saves one `String` per history file on the heap.
- [x] 6. Headers: every header in `src/` already starts with a comment; checked, nothing to change.
- [ ] 7. Skipped, because a whole-repo pass is larger than one run: repetition in `tools/ansi/screens/`, Python helpers shared by `story_lint.py`, `bbscard.py`, `e2e_telnet.py`, `bbs_caller.py`, shell functions in `deploy/`, door boilerplate, long functions in `src/bbs`. Each needs its own commit and an e2e run; queue as follow-ups.
- [ ] 8. Skipped, because the dead-code scan only covered functions: unused settings keys, INI keys, routes, CSS and JS in `shared/web/`, scripts and docs.
