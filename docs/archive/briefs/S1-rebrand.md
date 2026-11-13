> Archived in Q15: done brief. The work is recorded in `.kiro/specs/` and `docs/briefs/QUEUE.md`.

# Brief S1 - Rebrand spec

From: Claude Code (planner). To: Kiro (builder). Run this as a Kiro spec:
write `.kiro/specs/rebrand/requirements.md` (EARS), `design.md` and
`tasks.md` first, commit them, then do the tasks one by one and commit after
each group. Follow `.kiro/steering/`.

## Names (filled in by Juha before this runs)

| Key | Value |
| --- | --- |
| Board name (`name`) | HALCYON BBS |
| Short name (`short_name`) | HALCYON |
| SSID | HALCYON |
| Location line | LOW EARTH ORBIT |
| SysOp handle (the AI) | ELIZA |

The ship is HALCYON. The funder is never named anywhere ("the Sponsor" at
most).

If any value is still TBD, stop and say so. Do not invent names.

## Scope

Card (`sd/BBS/`):
- `CONFIG/SYSTEM.INI`: name, short_name, location, sysop. Keep
  `epoch=1997-03-14 21:00` for now.
- `CONFIG/NETWORK.INI`: ssid.
- `CONFIG/SYSTEM.INI [DOORS]`: VolaMUD stays for now (it becomes the deck
  plan later). Legend (`L`) and Dice (`D`) leave the menus.
- `CONFIG/BULLETINS.INI`: two placeholder bulletins in the story voice, no
  plot yet.
- Art: `ANSI/LOGIN/WELCOME.ANS`, `ASCII/LOGIN/WELCOME.ASC`,
  `ANSI/MENUS/*.ANS`, `ANSI/GOODBYE/GOODBYE.ANS`, `BULLETINS/001_RULES.ANS`.
  First pass only: replace the old name, drain the colours to greys with one
  dark red accent. Keep the menu option areas empty and the row/col layout
  in `MENUS.INI` working. Delete `ANSI/DOORS/KOUVOLA_LEGEND.ANS` and
  `LEGEND.ANS` if nothing reachable uses them after the door change.

Code (small edits only, both builds must still compile):
- `src/doors/doors.h`: remove the Legend and Dice rows from `DOORS`. The
  shared purse in `legend.h` is also used by VolaMUD, so keep what VolaMUD
  needs compiled; only the door entries go.
- Old place names in caller-visible strings in `src/doors/volamud.h`,
  `src/doors/legend.h`, `src/bbs/sysop.h`, `src/bbs/user_editor.h`: neutral
  wording (for example "Black Crown", "the Bank"). No new story content.
- `src/hw/scene_logo.h`: the built-in splash says the old name. Replace it
  with a neutral splash, or make the LCD fall back to the card logo.
- `tools/make-lcd-logo.py`: fix the help text.

Docs:
- `README.md`: rewrite for Last Caller. Engine overview can stay (trim the
  Kouvosto parts), add the haunted-ship premise in two sentences, keep the
  build and card setup sections accurate.
- `docs/CONTENT_POLICY.md`: everything on the card is original or licensed,
  listed with source. Replaces the removed curation policy.
- `docs/ROADMAP.md`, `docs/MENU_AUDIT.md`, `docs/VOLA_MUD.md`,
  `docs/APACHE_PORT.md`, `AGENTS.md`: remove references to the old board and
  its live URL; keep the technical content.
- `THIRD_PARTY_NOTICES.md`: leave as is, except add a line for
  `ASSETS/DIALUP.MP3` saying "source to be confirmed by the author".

## Acceptance (EARS, put these in requirements.md)

- WHEN `git grep -i kouvo` runs, THE result SHALL be empty except
  `THIRD_PARTY_NOTICES.md` and `docs/KIROWEEN_PLAN.md`,
  `docs/KIRO_REPO_STEPS.md`, `docs/SLICES.md` (planning history).
- WHEN `pio run` runs, THE build SHALL succeed.
- WHEN a caller connects, THE board SHALL show only the new name.
- THE door menu SHALL list only doors that work.

## Done when

All tasks ticked in `tasks.md`, the acceptance checks run and their output
pasted into your reply, and commits made. Say plainly if `pio run` was not
run or failed.
