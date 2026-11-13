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
