# Design - Diagnostics and shell doors

## Parts

| File | Role |
| --- | --- |
| `src/core/shell_logic.h` | Pure logic (no String, no SD): `parse` of `SHELL.INI`, path `resolve`, `list`, `exec` for the commands, the final command. Host-tested. |
| `src/doors/shell.h` | Board wrapper: loads the table on entry, prompt, art, flag grants through `sceneGrant`, door-table functions. |
| `src/doors/diag.h` | Diagnostics door: loads `DIAG.INI` on entry, code matching, `has=`/`set=`, door-table functions. Also `doorFlagsOf`, the caller's flags as the triggers see them (own account, or the flags a crew login carries). |
| `src/doors/doors.h` | Two table lines: `I` Diagnostics, `N` NOWHERE shell, each with a `listed` gate. |
| `session.h` | States `DIAG_PROMPT`, `SHELL_PROMPT`; `shellCwd`. |
| `sd/BBS/STORY/DIAG.INI`, `SHELL.INI` | Content (drafts). |
| `tools/ansi/screens/diag.py` | `ANSI/DOORS/DIAG.ANS`, `NOWHERE.ANS` and `ASCII/DOORS` twins. |

## Decisions

- Both doors are plain door-table entries; the gate is `listed()` so a menu
  never shows a dead option. The gate for the shell is a flag
  (`needs=NOWHERE_OPEN`) that act 3 content (Q9) grants. Until then the door is
  hidden on a normal card; staging opens it on the disposable card.
- Doors are not `saves`: crew accounts may use them. Flags go to the caller's
  own account (or are carried), through the same `sceneGrant` path as scenes.
- The shell never reads real files: the tree is the INI. Directories exist when
  declared or when a visible file lives below them, so a hidden file hides its
  directory too.
- Query codes are not listed by the door; the comms manual (Q7) carries them.
  `DIAG.INI` holds a small draft set (100, 101, 200, 201, 300, 301, 400, 500).
- Limits (stick RAM): 120 shell entries, 1200 characters of text each, 4000
  characters of output per command.

## Tests

- Host: `tools/test/shell_logic_test.cpp` (also checks the card's `SHELL.INI`
  and `DIAG.INI`).
- End to end over Telnet: `tools/test/e2e_telnet.py` (doors menu, Diagnostics,
  shell with `--shell-open`), run by `tools/staging-check.ps1`.
- Board: spec tasks below.
