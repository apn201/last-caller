# Hooks

Active:

1. **embed-web** - On save of `shared/web/*.html`, run
   `python tools/embed_web.py` to regenerate `src/net/web_terminal.h` and
   `src/net/sysop_page.h`.
2. **Story Lint** (`verification.json`) - On save in `sd/BBS/STORY`, `MSGBASE`,
   `BULLETINS`, `CONFIG` or `FILES`, run `python tools/story_lint.py --quiet`.
3. **Host Logic And Property Tests** (`verification.json`) - On save of a
   `*_logic.h`, `haunt.h` or a test in `tools/test/`, run
   `python tools/run_host_tests.py`.

Kiro runs file hooks for changes the agent makes, not for hand edits.

Planned:

- **name-brand-scan** - Before commit, scan caller-visible text for real names
  and brands.