# Requirements: Rebrand to HALCYON BBS (S1)

Source: `docs/archive/briefs/S1-rebrand.md`.

## Introduction

The TinyBBS engine shipped with its old board name, Finnish place names, a
live URL and two doors (Legend, Dice cabinet) that do not belong
on the ship. This slice makes the board HALCYON BBS, the ship's BBS run by
ELIZA, with first-pass art, neutral wording and honest docs. No plot is
added.

Names: board `HALCYON BBS`, short name `HALCYON`, SSID `HALCYON`, location
`LOW EARTH ORBIT`, SysOp `ELIZA`. The funder is never named ("the Sponsor" at
most).

## Requirements

### 1. Identity on the card

**User story:** As a caller, I want the board to present one name, so that
the fiction holds.

1. THE card SHALL set `name=HALCYON BBS`, `short_name=HALCYON`,
   `location=LOW EARTH ORBIT` and `sysop=ELIZA` in `CONFIG/SYSTEM.INI`.
2. THE card SHALL keep `epoch=1997-03-14 21:00`.
3. THE card SHALL set `ssid=HALCYON` in `CONFIG/NETWORK.INI`.
4. WHEN a caller connects, THE board SHALL show only the new name.

### 2. Doors

**User story:** As a caller, I want the door menu to list only doors that
work.

1. THE `DOORS` table SHALL contain VolaMUD and no Legend or Dice row.
2. THE `[DOORS]` section of `SYSTEM.INI` SHALL name VolaMUD only.
3. THE door menu SHALL list only doors that work.
4. WHERE VolaMUD uses the shared purse in `legend.h`, THE code it needs
   SHALL stay compiled.
5. WHEN no reachable code uses the two Legend door art files in `ANSI/DOORS`, THE files SHALL be deleted.

### 3. Bulletins

1. THE `BULLETINS.INI` SHALL hold two placeholder bulletins in the story
   voice, with no plot.
2. THE bulletin text SHALL be marked as draft for the author's final edit.

### 4. Art

1. THE login, menu, goodbye and rules art SHALL carry no old name.
2. THE art SHALL use greys with one dark red accent.
3. THE menu option areas SHALL stay empty and the row/col layout in
   `MENUS.INI` SHALL keep working.

### 5. Code strings

1. THE caller-visible strings in `volamud.h`, `legend.h`, `sysop.h` and
   `user_editor.h` SHALL use neutral wording.
2. THE built-in LCD splash SHALL not show the old name.
3. THE help text of `tools/make-lcd-logo.py` SHALL not show the old name.
4. WHEN `pio run` runs, THE build SHALL succeed.
5. WHERE the change touches `src/doors`, THE server build SHALL be checked
   by reading only, because it cannot run on this PC.

### 6. Docs

1. THE `README.md` SHALL describe Last Caller: the haunted-ship premise in
   two sentences, the engine overview, and accurate build and card setup.
2. THE `docs/CONTENT_POLICY.md` SHALL list everything on the card as
   original or licensed, with source.
3. THE `ROADMAP.md`, `MENU_AUDIT.md`, `VOLA_MUD.md`, `APACHE_PORT.md` and
   `AGENTS.md` SHALL not refer to the old board or its live URL.
4. THE `THIRD_PARTY_NOTICES.md` SHALL gain one line for `ASSETS/DIALUP.MP3`:
   "source to be confirmed by the author".

### 7. Acceptance

1. WHEN `git grep -i kouvo` runs, THE result SHALL be empty except
   `THIRD_PARTY_NOTICES.md`, `docs/archive/KIROWEEN_PLAN.md`,
   `docs/archive/KIRO_REPO_STEPS.md` and `docs/archive/SLICES.md`.
2. WHEN `pio run` runs, THE build SHALL succeed.
3. WHEN a caller connects, THE board SHALL show only the new name.
4. THE door menu SHALL list only doors that work.
