# Design - Playtest fixes

- `core/wrap_logic.h`: `wrap::rows(line, width)`, pure, host-tested
  (`tools/test/wrap_logic_test.cpp`). `bodyRows()` (messages), `diagLines()` and
  the manifest hint use it.
- `core/reader.h`: the status bar is `readerBar()`; `readerStopped()` writes
  only the bar when a picture played to its end.
- `bbs/messages.h`: `listMessages(s, what)` shows a window of 20 by position in
  the area (0 newest, `E` earlier, `L` later); `Session.listFrom/listShown/
  listTotal` remember the window; `listPrompt()` offers E and L only where they work.
- `core/manifest_logic.h`: `Row.his`, `Table.hint/lastHint`, `optionText()`,
  `emptyHint()`; `doors/manifest.h` uses them.
- `core/shell_logic.h`: `Table.help`, `Table.helpFinal`.
- `doors/diag.h`: HELP / ? prints `[DIAG] help`.
- `core/power.h`, `bbs/cmd_login.h`: `unknownHint` from `[TEXT] unknown_hint`.
- `tools/story_lint.py`: `check_open_sources()`. A source is open unless it is a
  crew login, a read of a subject that lives only in a conference with a
  `required_flag`, or a file in an area with a `required_flag`.
- Carried flags stay in the connection (as in the power-prologue spec). Because
  every needed fact has an open source (lint), losing a crew visit costs time,
  never the ending.