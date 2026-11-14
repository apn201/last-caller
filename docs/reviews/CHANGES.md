# Q18 changes (for Juha to review)

## Round 1 (scores: Ada 84, Mara 69, average 76.5)

| Change | Answers | Why |
| --- | --- | --- |
| Menu art is drawn once per menu per session; later visits show the title and options only (`src/bbs/menus.h`, `Session::menusSeen`) | Both critics: screens reprint on every return (Ada 7, Mara 4); Mara ran out of context because of it | The full frame on every return was the largest cost in pacing and interface |
| Mandatory bulletins (rules, SysOp welcome) show at a caller's first login only; crew logins and return visits skip them. B still lists them (`src/bbs/board.h`) | Ada 7 | Fifty lines before every menu, ten times per game |
| Ty's last manifest option now reads "ELIZA vented the bay. The Sponsor's capsule was a mock-up" (`MANIFEST.INI`, `OPT_CV`) | Ada 1 (the one unfair deduction) | The row asked who was responsible and the right option did not say it |
| ELIZA's lead to Ty's account comes after twelve minutes, not two, and no longer says "every deck" (`TRIGGERS.INI`, `ELIZA_NUDGE`) | Ada 3, Mara 8 | The aha was given away before the caller could look |
| The talk menu offers "About those minutes you cannot account for." instead of "Run the memory check. It needs SYSOP mode." (`ELZ_TALK.INI`; e2e updated) | Ada 2 | The menu gave away the handbook's "ask by name" step; the caller still needs the overrun and the handbook page to see it |
| VolaMUD is out of the Doors menu (`VOLAMUD.INI listed=0`, `mudListed`) | Ada 5 | A fantasy city does not belong on the ship |

Not changed this round: the stale "ELIZA NOT RESPONDING" line on the reboot
banner (kept as ELIZA's lie, noted by both), fragment numbering, digits in the
reader, Q depth, the unlisted P key.
