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

## Round 2 (Ada 66, cut off by a lock-out; Mara not played; round closed)

Round 2 is closed as incomplete rather than replayed after these fixes, so no
score is compared across commits. Round 3 plays the commit with them.

| Change | Answers | Why |
| --- | --- | --- |
| CHAOS password is now CORRIGENDUM, seen only on Orbit 3 played at slow speed. Ty's journal says "one more word", "a very des word"; the Orbit 3 caption says "one more word". LADDER (his paper in the Lounge) is now a wrong guess. `sync=1` brings old cards in line | Ada R2 note 2 (the best mechanic was skippable, LADDER was printed twice in the clear) | The 300-baud trick is now required. STORY CHANGE: Juha please check the word |
| The four Archive strips carry no piece number ("AUTH: NW1", "TORN. SCANNED"; file list "a torn strip"); the order is read from sentences that run on from strip to strip | Ada R1 note 4, R2 note 1 | Achterberg's "the order matters" now asks for a deduction |
| After a scene, the next queued scene waits until the caller has typed a command (`Session::sceneHold`) | Ada R2 note 5 (four death scenes back to back) | Each scene gets room; the caller reads between them |
| Q in a viewer, then Enter, goes back one level, not two (`eatEnterUntil`) | Mara R1 note 5, Ada R2 note 8 | The Enter after the hotkey answered the list's prompt too |
| File cabinets, Diagnostics, manifest and NOWHERE headers draw once per session (`artOnce`), like the menus | Mara R1 note 4, Ada R1 note 7 | Less reprinting |
| Manifest intro: "Right rows are confirmed two at a time." | Ada R2 note 6 | "In pairs" read as a promise that was broken when four rows were right |
| Test tool `tools/mcp/bbs_caller.py`: cursor moves to a new row start a new line, so menu options no longer run into one line | Ada R2 note 7, Mara R1 | The board drew them correctly; the tool hid it. Not a game change |

Not changed: the login lock-out Ada R2 reported after TYDEMO.NFO. I could not
reproduce it on the same board right after (admin and TYVANCE both log in; the
demo scene ends at its [Enter] and returns to the main menu). It looks like the
tool's view fell out of step during the scene's pauses.
