# Requirements: ANSI art pass (S1b)

Source: `docs/briefs/S1b-ansi-art.md`.

## Introduction

The S1 art is a grey-drained leftover of the old board. Every caller-visible
screen should look like a 1990s art group made it for HALCYON BBS, a dead
ship still powered by sunlight, with ELIZA in amber. Art is a large part of
the Design score.

## Requirements

### 1. Tools

**User story:** As the builder, I want to compose and review screens in
code, so that art can be regenerated, tweaked and checked.

1. THE repository SHALL have `tools/ansi/render.py` that renders an `.ANS`
   file to PNG in `docs/art-preview/`, which is git-ignored.
2. THE repository SHALL have `tools/ansi/build.py`, a library with a cell
   canvas, text, boxes, gradients, a block-letter logo font, and an
   optimised ANSI writer (SGR only when the colour changes).
3. EACH screen SHALL be a script in `tools/ansi/screens/` that regenerates
   it.
4. WHERE a menu name is given, THE renderer SHALL draw the options from
   `MENUS.INI` and report any option that lands on a non-empty cell.

### 2. Screens

**User story:** As a caller, I want every screen to look made for this board.

1. THE card SHALL have new art for `ANSI/LOGIN/WELCOME.ANS`,
   `ASCII/LOGIN/WELCOME.ASC`, the menus `MAIN`, `MESSAGES`, `FILES`,
   `GALLERY`, `DOORS`, `SYSOP`, `ANSI/GOODBYE/GOODBYE.ANS`,
   `BULLETINS/001_RULES.ANS` and `ANSI/DOORS/VOLAMUD.ANS`.
2. THE art SHALL follow the palette and motif in `ansi-art.md`: black,
   greys, one cold accent, amber for ELIZA, red only when it means
   something.
3. EACH full screen SHALL be 80 columns and at most 23 rows, and a screen
   followed by text from the board SHALL leave room for it.
4. THE files SHALL be UTF-8 with SGR escapes, `ESC[0m ESC[2J ESC[H` first,
   CRLF, no SAUCE, and backgrounds 0-7.
5. THE art SHALL carry small decay touches and the tag `·halcyon·crew·98·`,
   and SHALL NOT copy any existing art.
6. THE ASCII welcome SHALL be 7-bit ASCII, and callers without ANSI SHALL
   see it at login.
7. WHEN no reachable code uses `ANSI/MENUS/DOORS_V2.ANS` or
   `ANSI/DOORS/TAVERN.ANS`, THE files SHALL be deleted.

### 3. Menu option areas

1. FOR each menu in `MENUS.INI`, THE options SHALL land inside the frame, on
   empty cells, with the prompt below the art.
2. `MENUS.INI` SHALL be adjusted to the new layouts.

### 4. Steering

1. THE repository SHALL have `.kiro/steering/ansi-art.md` with
   `inclusion: always`: the art direction and the rule that every new
   caller-visible screen ships with its own art, rendered and checked as PNG.

### 5. Door settings that do nothing

1. THE SysOp console SHALL NOT offer door settings, and the user editor
   SHALL NOT show forest fights, because the Legend door is gone.
2. THE user editor SHALL keep the purse field (VolaMUD uses it).

### 6. Acceptance

1. EVERY screen regenerates from `tools/ansi/screens/` and renders without
   error.
2. EACH PNG has been looked at and alignment problems fixed.
3. `git grep -i kouvo` is clean and `pio run` passes.
4. `docs/SLICES.md` and the brief are committed with the spec.
5. THE server build cannot run on this Windows PC and SHALL be reported as
   not run.

## Pass 2 (S1c): scene-style technique

Source: `docs/briefs/S1c-ansi-polish.md`. Same screens, files and tools.

### 7. Technique

1. `build.py` SHALL have a `PixelCanvas` that draws on a canvas twice as tall
   as the cell grid and renders `▀`/`▄` with fg and bg colours, so a cell
   carries two pixels.
2. SHADING SHALL use `░ ▒ ▓ █` ramps between two colours (`ramp()`); Earth
   SHALL be half-block on its edge, shaded inside, with a thin cyan
   atmosphere and amber city lights on the night side.
3. THE HALCYON logo SHALL have shaped letters, a bright top and left
   highlight, a mid-colour body, a blue drop shadow one cell right and down,
   a `▓▒` fade on the lower rows and sparkles. Menu titles SHALL get a
   highlight, body and drop shadow in the small font.
4. THE ship SHALL have framed solar panels with cell lines, struts, a banded
   hull, windows, a dish and an engine bell. ELIZA's ring SHALL have a halo
   (`Canvas.glow`); the SysOp core SHALL be concentric rings.
5. FRAMES SHALL be lit from the top left (light top and left edges, shadowed
   bottom and right) with rivets set into the edges. Stars SHALL use
   `. · + *` in three brightnesses.
6. EACH screen SHALL keep two or three planned decay touches.
7. EACH screen SHALL stay under 12 KB; 80 columns; option areas unchanged;
   `render.py --menu` passes for all four menus.
