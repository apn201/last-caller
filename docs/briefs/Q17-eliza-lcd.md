# Q17 - ELIZA's eye on the stick, and its sleeve (brief for Kiro)

Written by Claude Code, 13 Nov 2026, from Juha's idea. The Deluxe Edition
box ships the Pocket-Dongle-S3 itself. Its LCD shows ELIZA's eye, and a
3D printed PLA sleeve covers the rest of the stick, so the stick looks
like a tiny crew interface unit. The look is set in `docs/ELIZA_EYE.md`
and in the ANSI art `ELIZA_WAKE_1/2` (amber segmented ring, dark lens,
small amber pupil, one dead segment). Reference render:
`docs/presskit/kling/eliza-eye-pla.png` (git-ignored, ask Juha if it is
missing).

Same rules as every queue item (top of `docs/briefs/QUEUE.md`): spec in
`.kiro/specs/eliza-lcd/`, spec commit first, small commits, `pio run` and
the staging check pass, tick with the commit range, stop. Run it after
Q16.

## A. The eye on the LCD

The panel is 160x80 landscape (`src/hw/lcd.h`, rotation 1). Layout:

- Left 80x80: the eye. 16 trapezoid segments on a ring (outer radius about
  37 px, inner about 25 px, 3 px black gaps), a dark lens inside with a
  thin grey bezel, a 3x3 amber pupil in the middle. Segment 11, counting
  clockwise from the top and 1 based, is dead: dark grey (`#2A2A2A`),
  never lit, in every state. Colours: segment full `#FFB000`, dim
  `#8A4A00`, pupil `#FFC233`, converted to RGB565 at compile time.
- Right 80x80: a "VFD" strip, one or two short words in blue-green
  `#4FF2C8` on black, 5x7 style, centred: `ELIZA` on the top line and the
  state below (`LISTENING`, `SPEAKING`, `BOOT`, `ASLEEP`, `PAGE N3`).
  Under it, small and dim, the node count (`2/8`). This keeps the
  status the SysOp needs without the old text panel.

Drawn with Arduino GFX primitives (`fillArc` or triangles), no bitmaps
needed. Redraw only segments that changed, so it does not flicker and
the SPI time stays small next to the Telnet and web tasks. 20 frames a
second is enough; measure the time per frame and write it in the tick
line.

## B. States, from what the board is doing

Read state the way `drawLcd()` already does (the session pool), plus
one small hook in the shared core: a `uint32_t elizaSpokeAt` set from
the platform clock wherever an ELIZA line (chat, scene, ghost, door) is
sent to a caller. No ESP32 header in the core, so the server build keeps
compiling (it just never reads it).

| State | When | Ring | Pupil | VFD |
| --- | --- | --- | --- | --- |
| Boot | from power on until the card is mounted | segments light one by one clockwise from the top, skipping 11 | on last | `BOOT` |
| Asleep | no caller for 10 minutes | all off except a slow dim pulse on segment 1 | off | `ASLEEP` |
| Brownout | any caller is still in the power prologue | segment 1 flickers irregularly, rest off | off | `SOLAR` |
| Listening | callers on, ELIZA quiet | all dim, one bright segment sweeps (4 s a turn) | on | `LISTENING` |
| Speaking | within 1.5 s of `elizaSpokeAt` | lit from the top both ways, count follows a fake voice level (random walk), like a VU meter folded into a ring | on | `SPEAKING` |
| Page | a caller paged the SysOp | ring flashes full twice a second | on | `PAGE N<n>` |
| Fault | SD not mounted | ring off, pupil steady | on | `SD ERR` |

## C. Switches

Juha's stick has its BOOT button broken off (13 Nov), so there is no
button on the device. Do not read GPIO 0.

- Card setting in `SYSTEM.INI`: `lcd_mode=eye` (default for HALCYON) or
  `lcd_mode=status` (the old panel). Content on the card, not in code.
- The SysOp tools get one line to switch the mode until the next reboot,
  shown only on the stick (the server has no LCD), following the menu
  rule in `docs/MENU_AUDIT.md`.
- The card logo (`lcd_logo=`) still shows during the first second of boot.

## D. The sleeve (OpenSCAD)

`hardware/sleeve/eliza_sleeve.scad`, parametric, printable in PLA without
supports. Every board measurement is a variable at the top with a
comment `// MEASURE: ...`, default values left as `0` so the file refuses
to render (an `assert`) until Juha fills them from calipers:

- board length, width, thickness at the PCB, height over the LCD;
- LCD active area: offset from the USB end and from one long edge, width
  and height (the panel is 0.96 inch, about 21.7 x 10.9 mm active, but
  measure it);
- USB plug: its length out of the board end;
- microSD slot: position, so the card can still be swapped;
- the stump of the broken BOOT button: position and height, so the
  sleeve clears it;
- any reset button, LED or antenna area that must stay uncovered (the
  antenna end gets no thick plastic over it).

Shape, from `docs/ELIZA_EYE.md` scaled down: a rounded box that slides
over the stick from the back and leaves the USB plug out. Front face in
light grey. A round window over the left half of the LCD for the eye,
with a raised printed bezel ring (separate black part, press fit), and a
small rectangular window over the right half for the VFD text, with a
black frame. A printed yellow `CALL` key on the front as decoration only
(glued, it presses nothing; there is no button under it). Engraved `CIU-2 HAB` on the side, recessed 0.4 mm so it
can be filled with paint. Two parts plus the bezel and the key, each
flat on one face for the print bed. 0.2 mm layers, 1.2 mm walls, 0.25 mm
clearance (a variable). The window can take a 1 mm clear or smoked
acrylic cut-out, or stay open.

Export STLs only after the measurements are in. Write in
`hardware/sleeve/README.md` how to measure, print settings, and which
parts get which colour of PLA.

## Done when

- `pio run` passes, and the server build still passes (the hook is the
  only core change).
- On the board: boot animation, asleep after 10 minutes with no caller
  (a test build may shorten it), brownout while a caller sits in the
  prologue, listening, speaking when ELIZA writes, page, `lcd_mode=status`
  on the card and the SysOp switch both bring back the old panel. Juha checks this on the stick; write the
  steps into the board test in `docs/TESTING.md`.
- `eliza_sleeve.scad` renders once the measurements are filled; with the
  zeros it stops with a clear message.
- A photo or PNG of the eye on the real LCD is the art check
  (`tools/ansi` is not used here). If you cannot get one, say so.
