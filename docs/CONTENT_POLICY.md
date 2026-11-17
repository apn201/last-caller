# Content policy

Everything on the card is original or licensed. This file lists it with its
source. It replaces the earlier curation policy.

Rules:

- No content is added without a source and a licence in this list.
- Original archive names and `FILE_ID.DIZ`, `DESC.SDI` and `DESCRIPT.ION`
  data are kept as they came.
- `sd/BBS/FILES` and `sd/BBS/MSGBASE` are git-ignored. They are made by the
  scripts in `tools/` and are not part of the repository.
- Caller-visible text uses invented names. No real people, companies, brands,
  games or films (see `.kiro/steering/story-voice.md`).
- Licences and notices for third-party parts are in
  `THIRD_PARTY_NOTICES.md`.

## In the repository (`sd/BBS`)

| Content | Source |
| --- | --- |
| `ANSI/`, `ASCII/`, `BULLETINS/` screens and art | Original to this project, MIT. First pass, to be redrawn by the author |
| `CONFIG/*.INI`, `DOORS/VOLAMUD/` | Original to this project, MIT |
| `ASSETS/DIALUP.MP3` | Source to be confirmed by the author |
| `ASSETS/SFX/*.WAV` (hum, relay, chirp, static, modem) | Original: made by `tools/mod/gen_sfx.py` from arithmetic, MIT |
| `FILES/001_ANSI_GALLERY/SUNSIDE.MOD`, `THERMO.MOD`, `FILES/006_TY_DESKTOP/HARBOUR.MOD` | Original compositions and instruments: made by `tools/mod/gen_mods.py`, no samples, no copied notes, MIT. Sample names are story text (drafts) |
| `STORY/EGGS.INI`, `FILES/002_LIBRARY/BROCHURE.TXT`, `NODELIST.TXT`, `FILES/004_RECORDS/Y2K.MEM`, `TARIFF.TXT`, `FILES/006_TY_DESKTOP/TYDEMO.NFO`, `STORY/SCENES/TY_DEMO.INI`, the POST, SNAKE, NODE_LUMEN and TY_DEMO art | Original to this project (Q11), MIT. The BIOS maker, modem make and boards are invented. Drafts |
| `DEMO/TUNE.MOD` (GE, the end demo's music) | Original composition and instruments: made by `tools/demo/gen_demo.py` with `tools/mod/modlib.py`, from arithmetic. No samples, no copied notes. Dedicated to the public domain (CC0 1.0); also usable under the project's MIT licence. The sample names carry the credits, as in a 1993 module. Drafts |
| `DEMO/FACE_H.PNG`, `FACE_A.PNG`, `FACE_T.PNG`, `FACE_V.PNG`, `SHIP.PNG` (GE) | Original pixel art: drawn by `tools/demo/gen_demo.py` (standard library, no input pictures), MIT |
| `DEMO/DEMO.JS` (GE, the end demo) | Original to this project, MIT: a 320 x 200 demo engine in plain JavaScript. It plays the tune with libopenmpt (BSD-3-Clause, `ASSETS/PLAYER/`, already on the card for the music player, served at `/player/`); nothing comes from a CDN. Pictures of the crew, headlines and every word are invented; the newspaper is "The Harbour Post", not a real paper |
| `ANSI/DEMO/*.ANS` (GE, the ANSI version of the demo) | Original to this project, MIT; made by `tools/ansi/screens/demo.py` |
| `STORY/DEMO.INI` (GE) | The demo's timeline and words: original, drafts |
| `DEMO/MUSIC0.S3M` (GE2, Ty's demo) | Second Reality's start and main music, "UnreaL ][ - The 2ND Reality", composed by Skaven and Purple Motion of Future Crew (1993). From the source and data release of Second Reality by Mika "Trug" Tuomi, github.com/mtuomi/SecondReality, commit 071a82eddb7a7390dbaa491a8a473e0e923a55c2, file `MAIN/MUSIC0.S3M`; its README dedicates the "source code and data" to the public domain (the Unlicense text). The demo kept the pattern data scrambled; `tools/demo/sr_unscramble.py` (original, MIT) undoes the XOR, nothing else is changed. The result is byte for byte the same as the plain copy on The Mod Archive (`2nd_skav.s3m`, module 212082), checked 17 Nov 2026. Credited on screen |
| `DEMO/SR.JS`, `DEMO/SR.INI`, `ANSI/DEMO/SR_*.ANS`, `FILES/001_ANSI_GALLERY/2NDREAL.DMO` (GE2) | Original to this project, MIT: Ty's text-mode tribute, "after Future Crew, Second Reality, 1993". The effects (dots, plasma, glenz, tunnel, scroller) are our own code of well-known demo techniques; no graphics, logos or code of Second Reality are used. The ANSI frames are made by `tools/demo/gen_sr.py`. Words are drafts |
| `ASSETS/PLAYER/` (libopenmpt, jsSID, `player.js`, `stream.js`) | Third party libraries and project glue code; licences in `THIRD_PARTY_NOTICES.md` |

## Made by scripts, not in the repository

| Content | Script | Source |
| --- | --- | --- |
| Message history (`MSGBASE`) | `tools/fidonet-seed.py` | FidoNet traffic archived by Josh Renaud at Break Into Chat, with every name, address and BBS replaced by invented ones |
| Tracker music (`FILES`) | `tools/fetch-modules.py` | The Mod Archive, copied as is; see `THIRD_PARTY_NOTICES.md` |
| SID music (`FILES`) | `tools/fetch-sids.py` | High Voltage SID Collection, copied as is; see `THIRD_PARTY_NOTICES.md` |

## Story content

Story tables in `sd/BBS/STORY/*.INI` and all caller-visible story lines are
original. Drafts are marked as drafts until the author's final edit.

ELIZA's chat brain, `sd/BBS/STORY/ELIZA/*.rive` (G2), is written for this
project in the RiveScript format (rivescript.com, an open format; the
reference interpreter is MIT-licensed). No brain files and no interpreter
code are copied: the reader in `src/core/rive_logic.h` is our own, for a
subset of the format.
