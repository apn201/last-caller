> Archived in Q15: done brief. The work is recorded in `.kiro/specs/` and `docs/briefs/QUEUE.md`.

# Brief S1b - ANSI art pass

From: Claude Code (planner). To: Kiro (builder). Run as a Kiro spec
(`.kiro/specs/ansi-art/`): requirements, design, tasks, then do them. This
replaces the placeholder art from S1. The art is a large part of the
Design score: every caller-visible screen should look like a top 1990s art
group made it for this board.

## Art direction

- Subject: HALCYON, a 1998 private spacecraft, dead for 28 years, still
  powered by sunlight. Cold, clean, abandoned. A ship's corridor at night,
  not a horror poster. No blood, no skulls.
- Palette: black ground, greys and dim white for structure, one cold accent
  (cyan or steel blue) for the ship, one warm accent (amber) for ELIZA.
  Red is rare and means something (alarms, a fault, a dead crew member).
- ELIZA's motif: an original one. A soft amber ring or a row of status
  lights. NOT a red camera lens or anything that resembles a famous film
  computer.
- Technique: full ANSI block art (`█ ▓ ▒ ░ ▀ ▄ ▌ ▐`), shading gradients,
  box-drawing frames, a big HALCYON logo in block letters with shading and
  a highlight edge. Mix in starfield dots, panel rivets, grilles, telemetry
  numbers. 80 columns, at most 23 rows per screen unless the screen is
  meant to scroll.
- Small "damage" touches: a missing block, a line offset by one column, a
  stray character. Subtle; the art should look decayed, not broken.
- Original only. No copied art, no group tags of real art groups. An
  artist tag of our own is fine, e.g. `·halcyon·crew·98·`.

## Format

- Same encoding as the existing files: UTF-8 text with ANSI SGR escapes
  (`ESC[...m`), `ESC[2J ESC[H` at the start of full screens, CRLF line ends.
  Check `src/bbs/viewers.h` and the existing `.ANS` files for what the
  viewer accepts; do not add a SAUCE record unless the viewer skips it.
- Menu screens: keep the option area empty and in the place `MENUS.INI`
  expects (`row`, `col`, `columns`, `width`, `rows`). Adjust `MENUS.INI` if
  the new layout needs it, and test that the options land inside the frame.
- Telnet sees the same bytes. Keep each screen readable at 80x24.

## Tools (write these first, in `tools/`)

1. `tools/ansi/render.py`: renders an `.ANS` file to PNG (Pillow is
   installed, a bitmap 8x16 VGA-style font is fine; draw it from a small
   embedded glyph table or a free font, nothing copyrighted). Used to
   review every screen before commit. Output to `docs/art-preview/`
   (git-ignored).
2. `tools/ansi/build.py`: a small library for composing screens in code:
   canvas of cells (char, fg, bg), draw text, boxes, gradients, a block-
   letter font for logos, then write optimised ANSI (only emit SGR when the
   colour changes). Screens are then `tools/ansi/screens/<name>.py` so they
   can be regenerated and tweaked.
3. Optional: `tools/ansi/img2ans.py` converts a photo to half-block ANSI
   (`▀` with fg/bg), for later pictures Juha shoots himself.

## Screens in this slice

| File | What it shows |
| --- | --- |
| `ANSI/LOGIN/WELCOME.ANS` | The big one. HALCYON logo, the ship in silhouette against the Earth's limb, telemetry line "LINK ACQUIRED", ELIZA's amber ring greeting. |
| `ASCII/LOGIN/WELCOME.ASC` | Plain ASCII version for callers without ANSI. |
| `ANSI/MENUS/MAIN.ANS` | Ship control panel frame, deck label "MAIN BOARD", option area. |
| `ANSI/MENUS/MESSAGES.ANS` | Crew comms console, "MAIL & MESSAGES". |
| `ANSI/MENUS/FILES.ANS` | Data storage bay, tape racks. |
| `ANSI/MENUS/GALLERY.ANS` | Viewport frame. |
| `ANSI/MENUS/DOORS.ANS` | Hatches, "AIRLOCKS". |
| `ANSI/MENUS/SYSOP.ANS` | ELIZA's core, amber. |
| `ANSI/GOODBYE/GOODBYE.ANS` | The ship drifting into Earth's shadow, "LOSS OF SIGNAL". |
| `BULLETINS/001_RULES.ANS` | Crew conduct notice in mission-document style, signed ELIZA. |
| `ANSI/DOORS/VOLAMUD.ANS` | Deck plan header (VolaMUD becomes the ship walk later). |

Delete `ANSI/MENUS/DOORS_V2.ANS` and `ANSI/DOORS/TAVERN.ANS` if nothing
reachable uses them.

## Steering

Add a short `.kiro/steering/ansi-art.md` (inclusion: always) with the art
direction above and the rule: every new caller-visible screen ships with
its own ANSI art made with `tools/ansi/`, rendered and checked as PNG
before commit. No placeholder text screens.

## Also (small, from the S1 review)

- `src/bbs/sysop.h` `sysopDoors`, and the forest-fights field in
  `src/bbs/user_editor.h`: both still offer Legend door settings, but the
  door is gone. Hide them (or remove the SysOp menu entry when no door has
  settings), so no menu shows an option that does nothing. `pio run` after.
- Commit `docs/SLICES.md` and this brief with the spec files.

## Done when

- Every screen above regenerates from `tools/ansi/screens/` and renders to
  PNG without errors.
- Kiro has looked at each PNG and fixed alignment problems (it can read
  images).
- `MENUS.INI` options land inside their frames (check the render with the
  options drawn in, or reason it out from row/col).
- `git grep -i kouvo` still clean, `pio run` passes (art is on the card,
  so it should).
- Committed per group. Reply with the list of screens and the path to the
  previews.
