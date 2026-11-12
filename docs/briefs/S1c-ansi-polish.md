# Brief S1c - ANSI art, second pass

From: Claude Code (planner). To: Kiro (builder). Extend the existing spec
`.kiro/specs/ansi-art/` with a "Pass 2" requirements section and tasks,
then do them. Same screens, same files, same tools.

## Review of pass 1 (rendered previews, Nov 12)

Good: composition, the palette, the copy ("The crew are resting. Please
keep your voice down."), the status readouts ("4 ABOARD ALL WELL", "BATT
OPEN CIRCUIT"), menu layouts, ELIZA's ring. Keep all of that.

Not yet: it reads as large flat pixels, not as 1990s scene ANSI. Every
shape is full blocks in one or two flat colours. The logo letters are
square stamps. The Earth is two blues. Nothing uses the character set.

## Pass 2: technique

1. Half-block resolution. Draw pictures (ship, Earth, ELIZA's ring) on a
   pixel canvas twice as tall as the cell grid and emit `▀`/`▄` with fg/bg
   colours, so each cell carries two pixels. Add this to `build.py` as a
   `PixelCanvas` that renders into the cell canvas. Curves and the Earth's
   limb get smooth.
2. Shading. Gradients through `░ ▒ ▓ █` between two colours (dark grey on
   black, blue to cyan, brown to yellow). The Earth's atmosphere is a thin
   cyan-to-blue `▓▒░` band; night side fades to black with `░`. Hull panels
   get a light edge and a shadow edge.
3. The logo. A real ANSI-style logo for HALCYON: letters with shaped
   strokes (half-block curves on C, O, Y), a bright top highlight line, a
   mid colour body, a dark drop shadow one cell down and right, a `░▒`
   fade on the lower part, small `·` sparkles. Same treatment, smaller, for
   the menu titles (MAIN BOARD, LOSS OF SIGNAL, ELIZA). The letters must
   still read at a glance on a phone.
4. Detail. Rivets (`·` and `∙`) along panel frames, grilles (`▌▐` rows),
   antenna struts on the ship, solar panels with cell lines, a few stars of
   different brightness (`.` `·` `+` `*` in dark grey, grey, white).
5. Decay. Two or three subtle defects per screen: a panel block missing, a
   line shifted one column, one wrong-coloured cell, a `▒` where a `█`
   should be. Planned, not random.

Keep: 80 columns, at most 23 rows, the option areas `MENUS.INI` expects,
red only where it means something, ELIZA amber, ship cyan/steel.

## Screens that need the most work

1. `ANSI/LOGIN/WELCOME.ANS` - the money shot. Logo, ship with solar
   panels, Earth's limb with atmosphere glow, ELIZA ring.
2. `ANSI/GOODBYE/GOODBYE.ANS` - ship sliding into the terminator line,
   Earth's day side shading into night.
3. `ANSI/MENUS/SYSOP.ANS` - ELIZA's ring with concentric shading, glow.
4. The rest: titles and frames upgraded with the new logo style, frames
   with rivets and shading.

## Check

- Render every screen with `tools/ansi/render.py`; look at each PNG at full
  size and at phone width; compare against the pass 1 PNG and say what got
  better. Keep pass 1 renders for the comparison.
- `render.py --menu` still passes for all four menus.
- File size: each screen under ~12 KB so it streams fast at 28800 and on
  the ESP32 (the stick has to read it from SD).
- `pio run` passes.

## Done when

Committed. Reply with the screen list, file sizes, and the preview paths
for welcome, goodbye and SysOp.
