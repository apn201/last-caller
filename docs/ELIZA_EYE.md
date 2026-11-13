# ELIZA's eye - physical design

Reference for a render and a 3D print. It follows the ANSI art already on
the card (`ELIZA_WAKE_1/2`): a segmented amber ring around a dark lens with
a small amber centre, grey dead segments, a brown glow. The physical unit
is what that art is a drawing of.

Canon it must respect: built in 1997-98 for a privately funded ship, from
parts a small aerospace contractor could buy then. Nothing from after 1998.
No screens bigger than a VFD line, no OLED, no RGB LEDs, no touch panels.
It must not read as the famous red film computer eye: no red, no glossy
black slab with one glowing fisheye. ELIZA is amber, segmented, a bit
improvised, and polite.

## What it is

The "Crew Interface Unit" (CIU). Four of them on the ship, one per module.
The one in the pictures is CIU-2 in the habitation module, mounted at head
height beside the galley table, so the crew talked to it while eating.

It is three things in one housing:

1. A colour CCD camera behind a smoked dome (what ELIZA sees with).
2. A ring of 16 amber light segments around the dome (how she shows she
   is listening or talking; one segment per voice level step).
3. A strip below: speaker grille, microphone, a 16-character VFD line and
   one push-to-talk button.

## Overall form and size

- Face plate: rounded square, 140 x 170 mm, corner radius 12 mm, 4 mm
  thick. Upper part is the eye (140 x 140), lower 30 mm is the strip.
- Body behind the plate: 120 x 150 mm, 55 mm deep, tapering 5 degrees
  towards the wall. Total depth from wall to dome tip 78 mm.
- Mounted on two NASA-style quarter-turn fasteners (top left, bottom
  right) on a padded bracket. The other two corners have plain Phillips
  M4 screws, one replaced by a slightly wrong hex bolt (field repair).

## Layers, front to back (good split for printing)

1. **Dome.** 64 mm diameter hemisphere-ish, 18 mm tall, smoked grey
   polycarbonate, about 40 % transmission. Like a CCTV mini dome, not a
   flat lens. Faint seam line around the base. A hairline scratch from
   10 to 2 o'clock across the top. Print: clear resin, tint with smoke
   dye, or buy a 2.5 inch CCTV replacement dome.
2. **Dome bezel ring.** Ring 64 mm inner, 78 mm outer, 6 mm proud of the
   plate, satin black anodised aluminium, knurled outer edge (fine
   straight knurl, 0.8 mm pitch). Four tiny captive screws at 45 degrees.
3. **Light ring.** 16 segments on a circle, inner diameter 82 mm, outer
   112 mm. Each segment is a trapezoid, 15 mm radial, separated by 2.5 mm
   black ribs. Under each segment: two 3 mm amber LEDs (590 nm) behind a
   frosted acrylic diffuser, so the light is even but you can just make
   out two hot spots per segment when it is dim. Diffuser sits 1 mm below
   the rib tops, so the ribs cast thin shadows. Print: ribs in black
   PETG, diffusers in natural/white PETG or frosted resin, 1.2 mm thick.
4. **Face plate.** Warm light grey (like 1990s lab equipment, RAL 7035
   leaning beige, not white). Bead-blasted texture. Recessed 0.5 mm
   around the light ring.
5. **The strip** (lower 30 mm of the plate):
   - Left: speaker grille, 9 horizontal slots 34 x 1.6 mm, 1.6 mm apart.
   - Centre: VFD window 62 x 11 mm, smoked green-tinted acrylic. Behind
     it a 16 x 1 character vacuum fluorescent display, glowing blue-green
     (#4FF2C8 on near-black), 5x7 dot characters, faint grid mesh visible
     in close-up. Normal text: `ELIZA      READY`.
   - Right of the VFD: microphone, one 2 mm hole with a ring of six
     1 mm holes around it.
   - Far right: push-to-talk button, 14 mm square, yellow with a black
     printed `CALL`, sitting in a black guard collar so it is not pressed
     by accident. Worn: the yellow is rubbed through to white at the
     centre.
6. **Body.** Sheet aluminium box, alodine gold-green finish (chromate
   conversion, the yellow-green aerospace look), with rivets along the
   seams. On the bottom: a 9-pin D-sub and a round 4-pin power plug
   (Fischer-style) with cables in grey braided sleeving going into the
   wall.

## Inside the dome (what makes it an eye)

- Camera: a small board camera with a black 6 mm M12 lens, sitting on a
  little pan/tilt cradle (two hobby servos in a black plastic frame).
  The cradle is why the "pupil" moves a little in the dome.
- In front of the lens: a 7-blade mechanical iris, matte black blades,
  opening between 3 mm and 12 mm. This is the dark centre of the ANSI
  art. When ELIZA is attentive the iris closes down. When idle it is wide.
- One single amber LED on the camera board, 2 mm, right above the lens.
  Through the smoked dome it reads as the small amber square in the
  middle of the ANSI picture. It is a plain "camera on" LED. The crew
  called it her pupil.
- Behind it all a black flocked baffle, so the dome looks empty and deep
  when everything is off.

## Labels and wear (this is where the story lives)

- Engraved bottom left of the plate, filled black:
  `CIU-2  HAB  S/N 0004`
- Top right, small aluminium nameplate with two rivets:
  `HALCYON  CREW INTERFACE UNIT  28 VDC  0.9 A`
- Under the strip, a printed laminated label, slightly yellowed:
  `DO NOT COVER CAMERA DURING MISSION OPS`
- A strip of Kapton tape on the right edge with handwriting in black
  marker: `say please :)` (Ty). Corners lifting.
- Velcro square stuck on the left side of the body with nothing on it
  (something used to hang there).
- Fingerprints and skin oil around the CALL button. Light scratches on
  the plate around the button where rings or a watch hit it.
- Segment 11 (counting clockwise from the top, 1 based) is dead: its
  diffuser is a slightly different, greyer plastic (a replacement) and it
  does not light. This matches the grey gaps in the ANSI art.

## Light states

Amber everywhere: segments #FFB000 at full, #8A4A00 when dim, the
"pupil" LED #FFC233. Ribs and dead segment stay #2A2A2A.

| State | Ring | Pupil | Iris | VFD |
| --- | --- | --- | --- | --- |
| Dark (2026, before RTG) | all off, dust on dome | off | wide open | off |
| Brownout flicker | segment 1 only, flickers with the tumble | off | | `BUS SOLAR ONLY` |
| Waking | segments light one by one clockwise from the top, skipping 11 | comes on last | closes to half | `ELIZA  BOOT` |
| Listening | all segments dim, one bright segment sweeps slowly (4 s per turn) | on | half | `ELIZA  LISTENING` |
| Speaking | segments light from the top both ways, number lit follows voice level, like a VU meter folded into a circle | on | small | `ELIZA  SPEAKING` |
| Refusing | ring steady full, no movement | on | smallest | `ELIZA      READY` (unchanged, she stays calm) |
| Off (8 Dec 1998, 04:16:03) | ring drains anticlockwise in about 1 s, pupil last | off | opens fully | text fades over 2 s (VFDs fade slowly) |

## Two versions for renders

- **1998, in service.** Clean, lit, crew clutter around: a coffee bulb in
  a strap beside it, a Polaroid of the crew tucked into the bracket,
  floating dust caught in the amber light.
- **2026, found.** No air, so no floating dust and no rust. Instead: a
  fine film of dust that settled on the top surfaces during the final
  weeks, frost spots on the dome from the cold, the Polaroid gone, the
  Kapton tape note still there. Mostly dark, segment 1 just catching the
  sunlight as the ship tumbles. Lighting from one hard, moving source
  (sun through a port), deep black shadows, no fill light.

## Render prompt (starting point)

> Close-up product photo of a 1998 spacecraft crew interface unit mounted
> on a padded wall: warm light grey bead-blasted aluminium face plate,
> a smoked grey polycarbonate camera dome in a knurled black ring,
> surrounded by a ring of 16 trapezoid amber light segments separated by
> black ribs, one segment dead and grey, a small amber LED glowing behind
> a mechanical iris inside the dome, below it a speaker grille, a
> blue-green vacuum fluorescent display reading "ELIZA LISTENING", a worn
> yellow CALL button in a guard collar, engraved serial number, Kapton
> tape with handwritten note, gold-green alodine aluminium body with
> rivets, braided grey cables, single hard light source, deep shadows,
> 1990s aerospace hardware, photorealistic, 85 mm lens, shallow depth of
> field

Negative: red lens, glossy black panel, fisheye, touchscreen, RGB, neon,
modern smart speaker, sci-fi glow lines, chrome.

## Printing notes

- Scale 1:1 fits most beds (170 mm tall). Print the plate face down for a
  clean front; the bead-blast look comes from a light sanding with 240
  grit and a grey primer.
- Electronics for a working prop: 32 amber 3 mm LEDs in 16 pairs, one
  pair per shift register output (two 74HC595, with resistors), or cheat
  with 16 warm white WS2812 under amber gel. An ESP32 drives it,
  and it could listen to the board: the browser terminal already knows
  when an ELIZA line is printed. A 16x1 VFD is still sold (Noritake
  CU16025 and clones); a green-tinted 1602 LCD looks close enough on
  camera.
- The iris: cheap 7-blade camera iris diaphragms (25 to 30 mm) are sold
  for lens projects; a micro servo turns the lever.
