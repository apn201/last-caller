# Hooks

Active:

1. **embed-web** - On save of `shared/web/*.html`, run
   `python tools/embed_web.py` to regenerate `src/net/web_terminal.h` and
   `src/net/sysop_page.h`.

Planned (S11 in `docs/SLICES.md`):

- **story-lint** - planned. On save in `sd/BBS/STORY/`, run the story lint.
- **server-build-properties** - planned. On save in `src/`, run the server
  build and the property tests.
- **name-brand-scan** - planned. Before commit, scan caller-visible text for
  real names and brands.
