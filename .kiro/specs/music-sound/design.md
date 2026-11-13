# Design - Music and sound

## The generator, `tools/mod/`

- `modlib.py`: the module writer (header, 31 samples, patterns, `M.K.` or
  `6CHN`), synthesis helpers (single-cycle oscillators, kick, snare, hat),
  a pattern notation, the PAL period table.
- `gen_mods.py`: the three tunes. Writes into `sd/BBS/FILES/...`.
- `gen_sfx.py`: the five WAVs (8-bit mono, 11025 Hz, under a second except
  the hum). Writes into `sd/BBS/ASSETS/SFX/`.
- `openmpt_check.js`: loads the written modules in the card's own libopenmpt
  build (needs `node`) and prints channels, orders, length and peak level; exits
  1 if one does not load, is under ten seconds or clips. Not part of the staging check.

Pattern notation: one token per row. `C-2/3` plays note C-2 with sample 3,
`-` is an empty row, `^` cuts the note, `+C20` appends an effect (hex).

## Board side

| Piece | File | Notes |
| --- | --- | --- |
| Header parser | `src/core/modinfo_logic.h` | pure logic, no String; host test `tools/test/modinfo_logic_test.cpp` |
| Text view | `src/bbs/viewers.h` | rows in the reader; used on Telnet and when the player is missing |
| Cue | `src/core/sound.h` | `soundCue(Session&, name)`: web sessions, name checked, file must exist |
| `sound=` | `src/core/scene_logic.h`, `scene.h` | `[SCENE] sound=NAME` played at the start |
| Route | `src/net/routes.h`, `esp_web.h` | `/sfx/NAME.WAV`, cached a day |
| Page | `shared/web/terminal.html` | `9;SND;NAME` plays `/sfx/NAME.WAV` at volume 0.4 |
| Player | `sd/BBS/ASSETS/PLAYER/player.js` | reads the sample names from the fetched module and lists them |

## Cues in the story

| Event | Cue |
| --- | --- |
| RTG switched (`stopPowerTimer`) | `CHIRP` |
| Brownout drop (`sunsideDrop`) | `STATIC` |
| `ELIZA_WAKE` scene | `HUM` |
| `MEM_BAY` scene | `RELAY` |

## Tunes

| File | Where | Date | Notes |
| --- | --- | --- | --- |
| `SUNSIDE.MOD` | Ty's Gallery | 1998-11-09 | four channels, A minor, 125 bpm; greets in the sample names |
| `THERMO.MOD` | Ty's Gallery | 1998-11-17 | four channels, D minor, slow; the thermostat argument |
| `HARBOUR.MOD` | Ty's Desktop | 1998-12-05 | six channels, bassline finished, the rest sketched; ends where he stopped |
