# Requirements - Diagnostics and shell doors

Slices S4b and S6b in `docs/archive/SLICES.md`, queue item Q5 in `docs/briefs/QUEUE.md`.
Story: `.kiro/specs/story-bible/` (R5 unlock chain: NOWHERE file system, final
command). All caller-visible wording in `DIAG.INI` and `SHELL.INI` is a draft
until the author's final edit.

## Glossary

- Query code: a short code (letters and digits) the caller types in the
  diagnostics door. Printed in the comms manual (act 1 content, Q7).
- Haven tree: the fake file system of the NOWHERE shell, defined by `[/path]`
  sections in `STORY/SHELL.INI`.
- Gate: the flags a caller needs before a door is in the door menu.

## Requirements

### R1 - Diagnostics door

1. THE board SHALL have a Diagnostics door (menu key `I`) that answers a query
   code with the text on the card (`STORY/DIAG.INI`), in ELIZA's voice.
2. THE door SHALL be in the door menu only while the ship has power, and not
   for the maintenance login.
3. THE door SHALL NOT list the query codes.
4. A code SHALL match without regard to case, spaces and dashes.
5. WHEN a code is unknown, THE door SHALL say so and ask again; WHEN a query
   needs flags the caller lacks, THE door SHALL say it is not open to them.
6. WHEN a query grants flags, THE board SHALL save them before showing the
   answer; IF the save fails, THEN nothing SHALL be given or shown.
7. Answers SHALL be true of the ship as the story bible has it.

### R2 - NOWHERE shell door

1. THE board SHALL have a shell door (menu key `N`) with a Unix-like prompt on
   the haven tree: `ls [-la]`, `cat`, `cd`, `pwd`, `whoami`, `hostname`,
   `uname`, `help`, `exit`.
2. THE door SHALL be in the door menu only for callers holding the flags in
   `needs=`; the maintenance login SHALL never reach it.
3. A file or directory with `has=` SHALL be absent ("No such file or
   directory") for callers without those flags, never "permission denied".
4. THE shell SHALL NOT show card paths or read anything outside the tree on
   the card.
5. THE final command SHALL be one line from `final_cmd=`; without `final_has=`
   it SHALL be "command not found"; with them it SHALL grant `final_set=`
   (saved first) and print `final_text=`.
6. A broken entry SHALL be skipped and named in the event log (SHELL_SKIP).

### R3 - Both transports

1. BOTH doors SHALL behave the same over Telnet and the browser, with ANSI art
   for ANSI callers and an ASCII twin for the others.
2. Both doors SHALL be in the door table; neither SHALL put ESP32 or Linux
   headers in the core.

### R4 - Staging fixes (Q1 review)

1. `tools/staging-remote.sh` SHALL NOT stop or restore anything on port 8086:
   anything already listening on the HTTP or Telnet port aborts the run.
2. `tools/staging-check.ps1` SHALL check `$LASTEXITCODE` for native commands
   and not rely on `$ErrorActionPreference='Stop'` (Windows PowerShell 5.1).
