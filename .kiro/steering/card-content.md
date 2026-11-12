---
inclusion: always
---

# Card content

- Content and settings live on the SD card, not in firmware. Prefer
  discovering things from the card over hard-coding them.
- The board's identity (name, short name, logos, door names, art) is in
  `SYSTEM.INI` and the art files. The pages read it from `/api/board`.
- Never show a menu option that does not work for that caller on that
  transport.
- Never expose SD paths to a caller. Do not format an SD card.
- Do not hand-edit `/BBS/DATA`, `/BBS/LOGS` or `/BBS/TEMP`. The board writes
  them.
- `sd/BBS/FILES` and `sd/BBS/MSGBASE` are git-ignored and stay out of the
  repository.
- Source content is in `sd/BBS/`. `tools/prepare-sd.ps1` copies it to a card.
- Story tables go in `sd/BBS/STORY/*.INI`. Clue and progress state are flags
  on the user record. Flags only grow.
- Keep the core transport-independent: a feature must work the same over
  Telnet and the browser, or hide itself where it cannot.
