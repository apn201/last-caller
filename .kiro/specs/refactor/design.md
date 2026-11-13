# Design: refactor pass (Q15)

Small commits, one area each. After each: `pio run`, `python tools/run_host_tests.py`, `python tools/story_lint.py`. At the end: `tools/staging-check.ps1`.

Dead code is found by listing every `static`/`inline` function in `src` and `server` (not the generated headers or miniz) and counting word matches across all of them; a name with one match is checked again with `git grep` over the whole tree.

Docs are checked against the tree: folder tables in `README.md`, `AGENTS.md` and `.kiro/steering/structure.md`, links to moved files, and the git-tracked state of `sd/BBS/FILES` and `MSGBASE`.

Archive: finished plans and done briefs go to `docs/archive/` (briefs in `docs/archive/briefs/`). Specs keep their text; only paths are updated.
