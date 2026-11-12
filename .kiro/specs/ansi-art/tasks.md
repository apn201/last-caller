# Tasks: ANSI art pass (S1b)

- [x] 1. Tools
  - [x] 1.1 `tools/ansi/render.py` (R1.1, R1.4)
  - [x] 1.2 `tools/ansi/build.py` and `screens/_common.py` (R1.2)
- [x] 2. Screens (R2, R3)
  - [x] 2.1 WELCOME.ANS and WELCOME.ASC; `plainArt()` shows the ASC (R2.6)
  - [x] 2.2 MAIN, MESSAGES, GALLERY, DOORS menus and `MENUS.INI` (R3)
  - [x] 2.3 FILES and SYSOP header art
  - [x] 2.4 GOODBYE, 001_RULES, VOLAMUD
  - [x] 2.5 Delete DOORS_V2.ANS and TAVERN.ANS (R2.7)
- [x] 3. Steering `.kiro/steering/ansi-art.md` (R4)
- [x] 4. Door settings that do nothing (R5)
  - [x] 4.1 Remove SysOp door settings
  - [x] 4.2 Remove the forest-fights field, keep the purse
- [x] 5. Acceptance (R6)
  - [x] 5.1 Render all screens, look at every PNG, fix alignment
  - [x] 5.2 Menu option areas checked with `render.py --menu`
  - [x] 5.3 `pio run` passes; `git grep -i kouvo` clean
  - [x] 5.4 Server build not run: Windows PC
  - [x] 5.5 Commit per group

## Pass 2 (S1c)

- [x] 6. Tools: `PixelCanvas`, `ramp()`, `half()`, `Canvas.glow`, new logo font and `logo()` (R7.1-7.3)
- [x] 7. Shared pieces: `planet()`, `earth()` with dither, `ship()`, `ship_mini()`, `eliza_ring()`, `frame()`, `header()` (R7.2, R7.4, R7.5)
- [x] 8. WELCOME.ANS: logo, ship, Earth's limb, ELIZA ring and halo (R7)
- [x] 9. GOODBYE.ANS: ship into the terminator, day side into night (R7)
- [x] 10. SYSOP.ANS: concentric ELIZA core with halo (R7)
- [x] 11. Other screens pick up the new titles and frames (R7.3, R7.5)
- [x] 12. Acceptance: all PNGs viewed (full and phone width), `--menu` passes for 4 menus, sizes under 12 KB, `pio run`
