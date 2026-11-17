# Q25 - TinyTrax: subsongs for modules, arrows never leave the player

From: Claude Code (planner), at Juha's request, 17 Nov 2026. To: Kiro.
A TinyBBS engine fix (Kouvosto gets it too), needed by GE2.

## What Juha found

Second Reality's `MUSIC0.S3M` (public domain, see GE2 in
`docs/briefs/Q23-Q24-SLICES.md`) loads in TinyTrax but plays two seconds of
noise and stops (`Pos 000/078`, `0:02 / 0:02 END`). VLC does the same. The
file is fine: its order list has a `---` (255) terminator early, and the real
tune is the sequence after it; the demo jumped there itself. libopenmpt
treats each such sequence as a subsong, but TinyTrax only switches subsongs
for SID files. Worse: ArrowUp/ArrowDown on a module falls through to the
board and throws the caller back to the menu.

## Fix

1. **Subsongs for every libopenmpt format** (MOD, S3M, XM, IT) in
   `sd/BBS/ASSETS/PLAYER/` (the player script): `get_num_subsongs`,
   `select_subsong`, shown as `Song 2/3` in the header like the SID count;
   ArrowUp/ArrowDown switch it; the position and time readouts follow.
2. **Start on the longest subsong** when a module has more than one (a short
   first sequence is nearly always sync data or a jingle); the caller can
   still step to the others.
3. **Arrow keys never leave the player.** While the player is open, all four
   arrows belong to it; with one subsong, Up/Down do nothing (or blink the
   count). Only Q/ESC close it, as today. Fix in `shared/web/terminal.html`
   (line ~499: the key falls through when `subsong()` returns false).
4. Telnet's text view of a tune (`.MOD` only today) is unchanged.

## Test

A host or browser test with a small module that has two sequences; then the
playtester (or Juha on Kouvosto) plays `MUSIC0.S3M` and `MUSIC1.S3M` and
hears the music; arrows at a one-song module stay in the player. Engine
change: note it on the TinyBBS port list in `docs/PLATFORM_UX.md`.
