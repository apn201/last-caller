---
inclusion: always
---

# ANSI art

Every new caller-visible screen ships with its own ANSI art made with
`tools/ansi/`, rendered to PNG and looked at before commit. No placeholder
text screens.

## Direction

- Subject: HALCYON, a 1998 private spacecraft, dead for 28 years, still
  powered by sunlight. Cold, clean, abandoned. A ship's corridor at night,
  not a horror poster. No blood, no skulls.
- Palette: black ground, greys and dim white for structure, one cold accent
  (cyan or steel blue) for the ship, one warm accent (amber) for ELIZA. Red
  is rare and means something (alarm, fault, dead crew member).
- ELIZA's motif is an amber ring or a row of status lights. Never a red
  camera lens or anything that resembles a famous film computer.
- Technique: block art (`█ ▓ ▒ ░ ▀ ▄ ▌ ▐`), shading gradients, box frames,
  starfield dots, rivets, grilles, telemetry. 80 columns, at most 23 rows
  unless the screen is meant to scroll. Small decay touches (a missing
  block, an offset line, a stray character), never broken.
- Original only. No copied art. Our own artist tag: `·halcyon·crew·98·`.

## Format

UTF-8 with SGR escapes, `ESC[0m ESC[2J ESC[H` first, CRLF, no SAUCE, no
cursor moves, backgrounds 0-7 only. Menu screens keep the option area empty
where `sd/BBS/CONFIG/MENUS.INI` (`row`, `col`, `columns`, `width`, `rows`)
says.

## Tools

- `tools/ansi/build.py`: canvas, pixel layers, 5x5 title font, logo font;
  `python tools/ansi/build.py [names]` regenerates `tools/ansi/screens/*.py`
  into `sd/BBS`.
- `tools/ansi/render.py`: `.ANS` to PNG in `docs/art-preview/` (git-ignored).
  `--menu NAME` draws the options and checks they land on empty cells.
  `--suffix _2` gives a new file name (the image viewer caches by name).
- Edit the screen scripts, never the generated `.ANS` files by hand.
- Caller-visible wording in the scripts is a draft. The author makes the
  final edit.
