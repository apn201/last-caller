# Requirements: refactor pass (Q15)

Brief: `docs/briefs/Q15-refactor.md`. Goal: smaller, plainer, easier to read, with no change to what a caller sees.

1. When a change touches `src/`, `server/`, `shared/web/` or `tools/`, the system shall produce the same caller output as before (e2e: 63 passed, 0 failed, 0 skipped).
2. When code, a setting or a document has no user anywhere in `src`, `server`, `shared`, `tools`, `deploy` or `sd/BBS`, the change shall remove it and record the search that proved it.
3. While a header is in `src/`, it shall start with a comment saying what it does.
4. When a planning document or brief is finished, it shall move to `docs/archive/` with one line at the top naming what replaced it, and every link to it shall be updated.
5. When a document disagrees with the tree, the document shall be corrected.
6. The refactor shall not edit text, art or posts under `sd/BBS/`, third-party code or the generated page headers.
7. If a finding is a bug, the refactor shall list it as an open item and not fix it.
8. The refactor shall keep ESP32 and Linux headers out of `src/core`, `src/bbs` and `src/doors`, and add no heap use to hot paths.
