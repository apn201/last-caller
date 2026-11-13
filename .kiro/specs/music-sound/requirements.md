# Requirements - Music and sound

Queue item Q10 in `docs/briefs/QUEUE.md`. Story: `docs/archive/SLICES.md` S7,
`docs/archive/briefs/S3b-power-prologue.md` (Ty the demoscener). Voice:
`.kiro/steering/writing.md`. Every caller-visible line is a draft for the
author's final edit.

## Glossary

- Tune: a ProTracker-style module (`.MOD`) in a file area.
- Sample names: the 31 text fields of 22 characters in a module's header. Trackers
  show them as a list, and scene musicians wrote messages in them.
- Cue: a short sound the browser terminal plays on the board's request.
- SFX: the cue sound files, `sd/BBS/ASSETS/SFX/*.WAV`.

## Requirements

### R1 - Original tunes from a script

1. THE repository SHALL hold a generator, `tools/mod/`, that writes the
   modules and the SFX from nothing but its own code (standard library only,
   no input files, same bytes on every run).
2. THE tunes SHALL be original compositions. No sampled or copied material.
3. THERE SHALL be three tunes by Ty: two finished (`SUNSIDE.MOD`,
   `THERMO.MOD`), one he did not finish (`HARBOUR.MOD`, six channels, the
   bassline done, the rest sketched).
4. EACH tune SHALL carry text in its sample names, in Ty's voice (lowercase,
   typos, greets, bad poetry), and fit the timeline of the posts.
5. THE unfinished tune MAY corroborate the night of 2 Dec 1998 (sequence 7)
   but SHALL NOT be the only evidence for any fact, and no flag or gate
   depends on opening it.

### R2 - Where the tunes are

1. SUNSIDE and THERMO SHALL be in Ty's Gallery, listed with 1998 dates.
2. HARBOUR SHALL be in Ty's Desktop only (`TY_HOME`).
3. THE card files `FILES.BBS` and `DATES.BBS` SHALL describe them; nothing
   about tunes is in firmware.

### R3 - Seeing a tune

1. WHEN a browser caller opens a tune and the player is on the card, THE
   player SHALL show the sample names that hold text, under the pattern view.
2. WHEN a Telnet caller opens a tune, THE board SHALL show a text view: title,
   format, channels, length in orders, patterns, and every sample name that
   holds text, in the reader, with the usual keys (N and P step through the
   folder, Q goes back).
3. WHEN a browser caller opens a tune and the player is not on the card, THE
   board SHALL show the same text view instead of an error.
4. THE text view SHALL show only printable characters; the module's bytes
   never reach the terminal raw.
5. A file that is not a module of a known tag SHALL keep today's behaviour.

### R4 - Sound effects and cues

1. THE SFX SHALL be short original WAV files from the generator: power hum,
   relay click, reboot chirp, static burst, distant modem.
2. THE board SHALL serve them at `/sfx/<NAME>.WAV` (name letters, digits and
   `_` only; a missing file is a 404).
3. THE board SHALL send a cue to browser callers only, as an OSC message
   `ESC ] 9 ; SND ; NAME BEL`. Telnet callers get nothing.
4. THE cues SHALL be: the reboot chirp when the caller switches to the RTG,
   static when the session drops in the brownout, and whatever a scene names
   in a `sound=` key of its `[SCENE]` section when it starts.
5. THE terminal page SHALL play a cue once, at low volume, and ignore it if
   the browser blocks audio or the file is missing.
6. A cue SHALL NOT repeat: the board sends it at the event, not on a timer.

### R5 - Checks

1. A host test SHALL cover the module header parser (known tags, bad tags,
   short files, odd bytes in names) and the scene `sound=` key.
2. THE end-to-end test SHALL open a tune over Telnet and see its sample names,
   and fetch an SFX file over HTTP on the staging host.
3. `pio run` and the staging check SHALL pass.
