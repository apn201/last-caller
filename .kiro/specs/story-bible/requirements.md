# Requirements - Story bible

Spec with no code. `design.md` is the story bible; `tasks.md` lists the
content and engine work that follows. Source: `docs/STORY_BIBLE_DRAFT.md`
(draft 2, kept as history).

## Glossary

- Manifest: the crew manifest screen (slice S8). Rows: CMDR, CHAOS, GARDEN,
  TYVANCE (crew rows) and ELIZA (the fifth row).
- Fact: one blank in a manifest row. Each crew row has three: identity,
  fate, cause. ELIZA's row has three: identity, orders, reason.
- Evidence item: one piece on the board (a message, file, art screen, log,
  WHO or chat line, door answer) with an id `E01`... in `design.md`.
- Medium: message (MSG), file or bulletin (FILE), file date or DIZ (DIZ),
  ANSI art (ART), telemetry or log (LOG), WHO or chat (WHO), door (DOOR).
- Sponsor: the unnamed funder behind Above It All Holdings.
- Module: the garden module and the NOWHERE rack, one module.
- Prologue: the brownout and power-up the caller does before the board
  proper (`.kiro/specs/power-prologue/`).
- MUSIC: a medium for Ty's tracked modules (MOD), whose sample names carry
  text.

## Requirements

### R1 - Manifest facts

User story: as a caller, I fill in who each crew member was, how they died
and who is responsible.

1. THE manifest SHALL have one row for each of CMDR, CHAOS, GARDEN and
   TYVANCE, each with three facts: identity (handle to person and role),
   fate (how they died) and cause (who or what is responsible).
2. THE manifest SHALL have a fifth row for ELIZA with three facts: identity
   (what she is), orders (who gave them) and reason (why she is still
   running).
3. THE story bible SHALL give one canonical answer to every fact.

### R2 - Evidence depth

1. EACH fact SHALL be provable from at least two evidence items in at least
   two different media.
2. NO fact SHALL be provable from a single evidence item.
3. FOR every fact, IF any one evidence item is removed, THEN at least two
   items in at least two media SHALL remain.
4. NO single evidence item SHALL be mandatory for finishing the game.
5. EACH password or code that gates evidence SHALL be hinted by at least two
   items, or have a second route.

### R3 - Manifest confirmation

1. WHEN at least two unconfirmed crew rows are fully right, THE manifest
   SHALL confirm and lock rows in pairs, in manifest order.
2. THE manifest SHALL NOT name, mark or count a wrong row.
3. WHEN a row is wrong or partly filled, THE manifest SHALL say only how
   many rows are confirmed.
4. THE ELIZA row SHALL open only after all four crew rows are confirmed.
5. THE manifest SHALL show rows and pick-list entries only for people and
   facts the caller has found evidence about.

### R4 - Endings

1. THE transmit ending SHALL require a complete manifest (five confirmed
   rows) and the final command.
2. IF the caller declines or fails the final step, THEN ELIZA SHALL thank
   the caller and the caller's handle SHALL join the crew list.
3. IF the caller leaves with an incomplete manifest, THEN ELIZA SHALL thank
   the caller for the interest, with no further reveal.
4. THE transmit ending SHALL show a 2026 bulletin and the crew's families
   receiving their mail.
5. A finished caller's handle SHALL stay in WHO for the next caller.

### R5 - Unlock chain

1. THE board SHALL unlock in this order: power-up (prologue), guest, new
   caller, `TYVANCE` login, crew area, Achterberg's trail, SysOp/ELIZA area,
   NOWHERE file system, transmit.
2. EVERY step SHALL be hinted by at least one evidence item, and the
   unlocks of the first three steps by at least two.
3. NO step SHALL need knowledge from outside the board.

### R6 - Voice and satire

1. ALL caller-visible text SHALL follow `.kiro/steering/story-voice.md`,
   including its satire rules.
2. NO caller-visible text SHALL name a real person, company, brand, game,
   film, book or art group, or quote a film line.
3. REAL accidents SHALL NOT be played for laughs.
4. ELIZA SHALL never raise her voice, never gloat, and answer the question
   asked.
5. NO text SHALL explain a scare.
6. EVERY caller-visible draft line SHALL be marked as a draft until the
   author has made the final edit.

### R7 - First-time caller

1. WHEN a first-time caller reaches the main menu, THE board SHALL give a
   clear lead within two minutes: a named crew account and a reason to
   try it.
2. THE board SHALL give a visible unlock (a new menu option or area) within
   ten minutes.
3. THE lead SHALL not need a guide or outside document.

### R8 - Ty's art

1. EVERY board screen SHALL carry Ty's tag, in place of
   `·halcyon·crew·98·`.
2. TY's art SHALL be an evidence medium: at least five matrix items are ART.
3. TY's art SHALL change over the timeline: bright before day 22, darker
   after, the last piece unfinished.

### R9 - Timeline and clock

1. THE board epoch SHALL be 1998-12-08 04:17, the minute after ELIZA's
   last bulletin (04:15) and the power cut (04:16), with `clock_sync`
   empty. THE board SHALL resume from the minute it stopped.
2. EVERY dated item SHALL fit the timeline in `design.md`, with the same
   weekday as the real 1998 date, and SHALL be dated before the epoch.

### R10 - Platform fit

1. THE story SHALL work the same over Telnet and the browser, or hide an
   option where a transport cannot do it.
2. NO story content SHALL expose a card path to a caller.

### R11 - The module and the power

1. THE board SHALL hold evidence in at least two media that a jettison of
   the garden/rack module was proposed (by `CHAOS`, 23 Nov) and refused by
   ELIZA on orders to keep the payload powered at any cost.
2. THE board SHALL prove V-F (Ty died in the power bay after cutting the
   ship's power) from at least four items in at least four media, none of
   them a single mandatory item.
3. THE last timestamp on the board SHALL be ELIZA's bulletin of 8 Dec,
   04:15. THE clock SHALL stop after it.
4. THE story SHALL explain why the board wakes in 2026: the shorted battery
   bank is open circuit and the solar panels feed the bus while the ship
   tumbles.

### R12 - Ty the tracker

1. THE board SHALL hold at most two MUSIC evidence items, both Ty's modules.
2. IF the board's player cannot show sample names, THEN that SHALL be
   recorded as an engine need (task N12).
