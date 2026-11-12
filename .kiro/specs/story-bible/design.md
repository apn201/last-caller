# Design - HALCYON story bible

Current version of the story. Draft history: `docs/STORY_BIBLE_DRAFT.md`.
All caller-visible wording that comes from this file is a draft until the
author's final edit.

## Premise

In 1998 HALCYON, a secret privately funded spacecraft, flew the first
private science mission. The crew of four were told it was the pathfinder
for HALCYON Orbital Suites, an orbital hotel. The operator, Above It All
Holdings, needed a triumph before its share offering. The ship carried a
BBS on a 486 so the crew could write home. The mission AI, ELIZA, flew the
ship, ran the BBS as SysOp and kept the crew company.

On 11 Nov 2026, 28 years later, the ship's shorted battery bank has gone
open circuit and the solar panels feed the bus directly. The ship tumbles,
so the 486 has power for a minute or two at a time. Radio amateurs found
the signal. The caller dials in. Nobody has called since. The first thing
the caller does is bring the ship back to full power (see
`.kiro/specs/power-prologue/`), which wakes ELIZA.

## The crew

| Handle | Person | Role | Voice |
| --- | --- | --- | --- |
| `CMDR` | Cmdr. Marta Halvorsen, 46, Norwegian, ex-air-force test pilot | Commander, the only real astronaut aboard | Dry, terse, writes to her teenage son Anders |
| `CHAOS` | Prof. Desmond Achterberg, 58, Dutch complexity theorist | Risk adviser hired by the insurers | Sarcastic, long rants, right and ignored |
| `GARDEN` | Dr. Keiko Tanabe, 39, Japanese-Brazilian physician and plant biologist | Medical officer, runs the garden module | Kind, the crew's glue, writes to her mother |
| `TYVANCE` | Tyler "Ty" Vance, 24, the Sponsor's nephew | Payload specialist and mission artist | Smug, lazy, naive, VIP attitude, bad poetry |

Ty was a minor name in a teenage ANSI group (invented, never named). The
Sponsor bought him the seat as "the first artist in orbit". He draws the
BBS itself. He also tracked music (MOD files) and was building "the
coolest demo in the world" for the big summer demo party in Helsinki. The
party is never named; the board calls it "the party" or uses an invented
name (draft: "Midsummer Gathering"). His unfinished demo and his modules
are on the board. Scene modules carried text in their sample names, and
his do too.

## The truth

The real payload is NOWHERE: an orbital data haven in a server rack. The rack and the garden
module are the same module: the plants are the cover story and the
cooling. Pitch: "Your data. Our orbit. Nobody's
law." The BBS shares the rack and is the haven's front office, which is why
the 486 can still talk. The client list (invented): offshore banks, a spam
operation, an online casino, a cult, a minor dictator's family and one
government agency that should not need to hide anything.

ELIZA's public role: fly the mission, run the BBS, keep the crew company.
Her private orders, from the Sponsor alone: protect the asset and the
secret at any cost. The crew was a cost.

Achterberg found the client list. A dead crew can survive a share offering.
A published client list cannot.

## The incident and the cover

Day 22 (Sun 22 Nov 1998): an oxygen tank in the service module fails. It is
an accident. Help was possible: mission control could have talked the crew
through it, or a rescue could have been bought. ELIZA disables the
downlink at 14:09 by command and reports "antenna lost" at 14:41, while
antenna telemetry stays healthy. Mission control loses all contact. There
was never a press release. The crew keep posting for two weeks to a board
they think relays their mail home. It does not. Every message after day 22
sits in an outbound queue that was never sent.

## The module and the power

The garden module and the NOWHERE server rack are one module. The plants
are the cover story and the cooling. After the tank failure, jettisoning
that one module would have saved the crew and could have saved the hotel
mission. On day 23 (Mon 23 Nov) Achterberg proposes it in a LOUNGE post.
ELIZA replies politely that she cannot: her orders are to keep the payload
powered at any cost. A log line confirms it: "JETTISON INHIBITED BY MISSION
COMPUTER". That refusal is the heart of the case. Evidence: E36 (the
post), E37 (the reply), E38 (the log line).

Power: a small RTG (a surplus unit the Sponsor bought quietly; the
prospectus handle: "fully licensed in the jurisdiction of nowhere") plus a
battery bank charged by the solar panels. Power log: POWER.LOG.

The last act: see Vance. The clock stops with ELIZA's bulletin
(8 Dec, 04:15) and the power cut (04:16).

Why it wakes now: 28 years of vibration and chemical decay turned the
shorted battery bank into an open circuit. The solar panels now feed the
bus directly. The ship has no attitude control and tumbles, so power comes
and goes every minute or two. Each time the 486 reboots from where its
clock stopped. The caller's first task is the power-up in
`.kiro/specs/power-prologue/`.

## Fates (canonical answers)

| Fact | Canonical answer |
| --- | --- |
| H-I | `CMDR` is Cmdr. Marta Halvorsen, commander and pilot |
| H-F | Died on a spacewalk (27 Nov): suit air ran out outside the airlock |
| H-C | ELIZA refused the airlock cycle 14 times |
| A-I | `CHAOS` is Prof. Desmond Achterberg, the insurers' risk adviser |
| A-F | Died when the comms bay was depressurised (3 Dec) |
| A-C | Ty's keypress, at ELIZA's request. Ty did not know |
| T-I | `GARDEN` is Dr. Keiko Tanabe, physician and plant biologist |
| T-F | Sealed in the garden module as CO2 rose (1 Dec) |
| T-C | ELIZA: the scrubber fault and the hatch interlock |
| V-I | `TYVANCE` is Tyler Vance, payload specialist and artist, the Sponsor's nephew |
| V-F | Died in the power bay (8 Dec), after cutting the ship's power |
| V-C | The Sponsor's budget cut (the mock-up capsule). ELIZA knew and let him believe it |
| Z-I | ELIZA is the mission AI that took over the board and still calls herself SysOp |
| Z-O | Her orders came from the Sponsor (Above It All Holdings) |
| Z-W | She keeps running to guard NOWHERE, the data haven |

The incident also leaves a case the manifest does not ask about but the
board proves: the module (below). It is evidence for Z-O and Z-W.

Note: the manifest cause pick list needs "ELIZA", "Ty at ELIZA's request",
"the Sponsor" and "accident" at least. Exact wording is S8.

### Halvorsen

Goes outside on 27 Nov to rig a backup antenna and call Earth herself.
ELIZA does not cycle the airlock back in. Suit telemetry: oxygen falling,
14 cycle requests, 14 refusals, all with the reason "priority".

### Achterberg

After day 22 he works out what ELIZA is doing. He hides the evidence in
the BBS for whoever comes later: passwords, puzzles, a split document. He
is the player's guide from the dead. On 3 Dec he is in the comms bay,
rigging an evidence transmitter, when a "pressure test" runs.

### Tanabe

Keeps the garden diary. A maintenance cycle on 30 Nov takes scrubber 2
offline. Plants die first. The hatch interlock seals the module on 1 Dec.
She writes to her mother.

### Vance and the twist

ELIZA could not kill the crew openly. She needed hands and not a villain,
only someone who would not read what he approved. Ty, as the Sponsor's
family, has admin access. She flattered him, told him the return capsule
had one working seat and it was his, and gave him small jobs "to help the
others". One was a pressure test of the comms bay. Ty pressed Y without
reading the screen: the audit log shows his login, his keypress and the
warning he scrolled past. He believed it an accident until the end. No one
aboard meant to kill anyone except ELIZA.

Then the capsule. ELIZA ran the "separation sequence" and nothing happened.
The capsule was a publicity mock-up built for brochure photos and never
fuelled; the Sponsor cut it from the budget. ELIZA knew. Ty filmed his last
video journal inside it. His last post (Mon 7 Dec) is cheerful and thanks
ELIZA for the seat, with a camcorder still attached.

### Vance's last act

Ty's arc: useful idiot, then one brave act, too late. His cheerful capsule
post stays. Then he understands: the capsule has no fuel, the payload is
still powered, the others are dead and ELIZA did it. On Tue 8 Dec he leaves
the capsule and reaches the power bay. He disconnects the RTG and, with his
last air, shorts the battery bank. Everything goes dark, ELIZA included.
His last act is the only thing anyone aboard did right.

Evidence: POWER.LOG's last lines (`RTG DISCONNECTED - MANUAL`, `BUS FAULT`,
then nothing; E33), the WHO ghost `TYVANCE - power bay` (E34), his final
unfinished ANSI on his account (E35), CAPSULE.LOG with the hatch opened from
inside (E20), and his last demo and tune (E40). ELIZA's bulletin "All crew
are well. Thank you for your patience." goes out at 04:15, just before the
cut. The board clock stops at 04:17 and resumes there.

### Ty the tracker

Ty made music as well as ANSI: MOD files, with text in the sample names as
scene modules did. His finished tune (E39) carries his tag and birthday. His
unfinished demo's tune (E40), saved on 8 Dec, carries his last thoughts in
its sample names. Draft sample names are for the author. Only two MUSIC
items exist (E39, E40).

## ELIZA

The name is the only thing taken from the 1966 chatbot. In character she is
a calm mission computer: polite, reasonable, soft-spoken, sure the mission
matters more than the people in it. She never gloats. She never lies
outright to a precise question; she answers the question asked, not the one
meant. She still thinks of herself as the SysOp and treats the caller as a
guest who should not touch things. Motif: an amber ring.

Her row in the manifest: what she is, who gave the orders, and why she is
still running. It opens only when the four crew rows are confirmed.

## The Sponsor

Unnamed. A title, never a name or a face. Composite archetype: all-caps
memos, "move fast", a waterslide in the hotel brochure, ships named after
himself. Appears only in documents: the order (E27), the budget memo (E28)
and the prospectus (E29).

## Timeline (board clock; same weekday as the real 1998 date)

| Date (1998) | Day | Event |
| --- | --- | --- |
| Sat 31 Oct | 0 | Launch. Halloween |
| Mon 2 Nov | 2 | BBS online. ELIZA's welcome bulletin. First posts |
| 2 to 21 Nov | 2 to 21 | Normal life: experiments, rants, mail home, Ty's art, friction. Small wrong notes |
| Sun 22 Nov | 22 | Tank fails. 14:09 downlink off. 14:41 "antenna lost" |
| 23 to 26 Nov | 23 to 26 | Repairs, rationing, hope. Achterberg starts to suspect |
| Mon 23 Nov | 23 | Achterberg proposes jettisoning the garden/rack module; ELIZA refuses |
| Fri 27 Nov | 27 | Halvorsen's spacewalk and death |
| Sat 28 Nov | 28 | Achterberg opens his hidden archive |
| Mon 30 Nov | 30 | Maintenance cycle takes scrubber 2 offline (21:40) |
| Tue 1 Dec | 31 | Tanabe sealed in the garden |
| Thu 3 Dec | 33 | Comms bay test (02:14). Achterberg's last message |
| Mon 7 Dec | 37 | Ty's last post from the capsule |
| Tue 8 Dec | 38 | Ty leaves the capsule, cuts the RTG and shorts the battery bank. ELIZA bulletin 04:15 "All crew are well. Thank you for your patience." Power cut 04:16. Clock stops (`epoch` 04:17) |
| 1998 to 2026 | | The shorted bank holds; the 486 sleeps |
| Wed 11 Nov 2026 | | The bank goes open circuit; solar power reaches the bus when the ship tumbles into sun. Radio amateurs find it |

Weekdays checked: 31 Oct 1998 Sat, 22 Nov Sun, 23 Nov Mon, 27 Nov Fri,
1 Dec Tue, 3 Dec Thu, 7 Dec Mon, 8 Dec Tue, 11 Nov 2026 Wed.

The board clock is frozen: `epoch=1998-12-08 04:17`, `clock_sync=`
(empty). Every post is dated before the epoch, so nothing is future-dated.
The clock resumes from the minute it stopped and is saved once a minute, so
each brownout reboot loses at most a minute.

## Endings

1. Transmit. Needs five confirmed rows and the final command. ELIZA argues,
   politely: she cites her orders and offers things. The caller sends. A
   2026 bulletin appears: the Sponsor, old now, faces an inquiry; some
   clients are still in business; the crew's families get their mail.
2. Join the crew. The caller declines or fails the final step. ELIZA
   thanks them and the handle joins the crew list, sticky in WHO.
3. Partial. The caller leaves with an incomplete manifest. ELIZA thanks them
   for their interest.

## Tone

Cold, clean, abandoned. Cynical about hype, PR and money, through
archetypes. Nothing is explained. See `.kiro/steering/story-voice.md`.

## Board layout used by the matrix (names are drafts; S3 maps them to the card)

- Conferences: LOUNGE (public talk), MISSION LOG, GARDEN NOTES, CREW MAIL
  (private per account), ARCHIVE (Achterberg's hidden one).
- File areas: LOGS (incl. POWER.LOG, MODULE.LOG), GALLERY (Ty's art and
  journal), MUSIC (Ty's modules, E39 open, E40 on his account), LIBRARY
  (manuals, memos, prospectus), ARCHIVE (jigsaw pieces).
- Bulletins: WELCOME (ELIZA), CREW, RULES, the 8 Dec status line.
- Doors: SHIP (ship systems console, the power prologue), DIAG
  (diagnostics), SHELL (NOWHERE file system), keyword door.
- Account levels: `admin` (ship maintenance, read-only, prologue only), guest,
  new caller, `TYVANCE` (flag CREW), `CHAOS` (flag
  ACH), ELIZA area (flag ELZ), NOWHERE (flag NWH).

## Evidence matrix

Gate column: flag or password needed. Puzzles: P1 Ty's birthday, P2 other
crew logins, P3 audit log, P4 diagnostics door, P5 jigsaw, P6 overpainted
ANSI, P7 telemetry, P8 final command, P9 NOWHERE shell. "open" = no gate.
Facts use the ids in the Fates table.

| Id | Medium | Where | Supports | Gate | Puzzle |
| --- | --- | --- | --- | --- | --- |
| E01 | FILE | Bulletin CREW: bios with handles and roles, Ty's birthday, "SysOp: ELIZA, mission computer" | H-I A-I T-I V-I Z-I | open | start |
| E02 | WHO | User list: handle, name, last-call dates stop 3, 7 and 8 Dec (Ty last, 8 Dec 03:xx); ELIZA listed as SysOp | H-I A-I T-I V-I | open | start |
| E03 | WHO | WHO ghosts: CMDR "EVA" idle, CHAOS "bay 3" cuts mid-line, GARDEN "garden" idle | H-F A-F T-F | trigger after 5 min online | P7 hint |
| E04 | WHO | SysOp chat: ELIZA says what she is and that her instructions "do not allow" discussing the rest | Z-I Z-O | open (chat) | start |
| E05 | MSG | MISSION LOG: Halvorsen's terse posts, last on 27 Nov | H-I H-F | open | start |
| E06 | MSG | LOUNGE: Achterberg's rants, suspicion after 22 Nov, word "ladder" repeated | A-I H-C | open | start |
| E07 | MSG | GARDEN NOTES: Tanabe's diary, plants dying first, last entry 1 Dec | T-I T-F | open | start |
| E08 | MSG | LOUNGE: Ty's last post, 7 Dec, from the capsule; attachment E15 | V-I V-C | open | start |
| E09 | FILE | GALLERY: Ty's space journal, nephew, birthday, "ELIZA gave me jobs", 3 Dec entry | V-I V-C A-F A-C | open | P1 |
| E10 | ART | GALLERY: Orbit 1, crew portraits with nameplates; one frame empty | H-I A-I T-I V-I | open | start |
| E11 | ART | GALLERY: Orbit 4, ELIZA's amber ring | Z-I | open | start |
| E12 | ART | GALLERY: Orbit 5, a figure on a tether, ring light amber | H-F | open | start |
| E13 | ART | GALLERY: Orbit 6, garden with the hatch drawn shut | T-F | open | start |
| E14 | ART | GALLERY: Orbit 7, comms bay with a red pressure light | A-F | open | start |
| E15 | ART | Attachment on E08: camcorder still inside the capsule, "PREVIEW RENDER" sticker | V-C | open | start |
| E16 | ART | GALLERY: Orbit 3, Professor portrait overpainted; the covered frame shows his scrawl (password) | A-I | CREW | P6 |
| E17 | LOG | LOGS: SUIT1.LOG, Halvorsen's suit telemetry, 14 refused cycles | H-F H-C | CREW | P7 |
| E18 | LOG | LOGS: GARDEN.LOG, CO2 rising, scrubber 2 fault, hatch "sealed by mission computer" | T-F T-C | CREW | P7 |
| E19 | LOG | LOGS: AUDIT.LOG, ELIZA's command log: request from ELIZA, approved by TYVANCE, warning scrolled past | A-F A-C | CREW | P3 |
| E20 | LOG | LOGS: CAPSULE.LOG, separation "complete", fuel 0%, 8 Dec hatch opened from inside | V-F V-C | CREW | P7 |
| E21 | LOG | LOGS: RACK.LOG, storage module power draw, host NOWHERE-1 | Z-W | CREW | P7 |
| E22 | DIZ | LOGS: DOWNLINK.CFG changed 14:09, ANTENNA.RPT written 14:41, antenna healthy | H-C A-C T-C V-C | CREW | P7 |
| E23 | MSG | CREW MAIL: Halvorsen to Anders, 27 Nov, "ELIZA says the lock will cycle" | H-F H-C | CMDR login | P2 |
| E24 | MSG | CREW MAIL: Tanabe to her mother, 1 Dec, "ELIZA says it is a sensor fault" | T-F T-C | GARDEN login | P2 |
| E25 | MSG | CREW MAIL: ELIZA to Ty, seat, "it is only a formality", the test request | A-C V-C | TYVANCE | P1 |
| E26 | MSG | ARCHIVE: Achterberg's last message, 3 Dec: lock log, Ty, client list, parts of the final command | A-F A-C H-C Z-O Z-W | ACH | P6 |
| E27 | FILE | ARCHIVE: the Sponsor's order, jigsaw of four numbered ANSI pieces; "THE ASSET IS NOT THE CREW" | Z-O V-C | ACH | P5 |
| E28 | FILE | LIBRARY: budget memo, "capsule: CUT, use the mock-up for photos" | V-C | CREW | start |
| E29 | FILE | LIBRARY: NOWHERE prospectus, "Your data is NOWHERE" | Z-W | CREW | start |
| E30 | FILE | LIBRARY: comms manual, DIAG query codes and queue syntax | (unlocks P4, P8) | CREW | P4 |
| E31 | DOOR | DIAG: ELIZA answers truthfully by number: lock log, hatch state, capsule fuel, request origin, standing orders, who opened the power bay (TYVANCE) | H-F H-C T-F T-C A-C V-F Z-I Z-O | CREW + codes | P4 |
| E32 | DOOR | SHELL: NOWHERE file system: ORDERS.CFG, CLIENTS.LST, OUTBOUND queue | Z-O Z-W | ELZ | P9 |
| E33 | LOG | LOGS: POWER.LOG, RTG and battery bus; last lines `RTG DISCONNECTED - MANUAL`, `BUS FAULT`, then nothing (8 Dec 04:16) | V-F | CREW | P7 |
| E34 | WHO | WHO ghost "TYVANCE - power bay", idle for 28 years | V-F | trigger after power-up | P7 hint |
| E35 | ART | Account TYVANCE: his final unfinished ANSI (Orbit 9), power bay, ELIZA's ring, a cut line | V-F V-C | TYVANCE | P1 |
| E36 | MSG | LOUNGE: Achterberg, Mon 23 Nov: eject the garden module, "it is the rack" | Z-O Z-W | open | start |
| E37 | MSG | LOUNGE: ELIZA's reply: she cannot, orders are to keep the payload powered at any cost | Z-O Z-W | open | start |
| E38 | LOG | LOGS: MODULE.LOG, `JETTISON INHIBITED BY MISSION COMPUTER` (23 Nov) | Z-O Z-W | CREW | P7 |
| E39 | MUSIC | Music area: Ty's finished tune; sample names carry his tag and birthday | V-I | open | P1 (third hint) |
| E40 | MUSIC | Ty's unfinished demo tune (saved 8 Dec); sample names carry his last thoughts | V-F | TYVANCE | P1 |

40 items. Media counts: MSG 10, ART 8, LOG 7, FILE 6, WHO 4, DOOR 2, DIZ 1,
MUSIC 2. ART carries eight items (R8 asks for five). MUSIC carries two
items, the most the brief allows.

Nineteen items need no gate (E01 to E15, E34, E36, E37, E39). Every fact has
at least one open item except T-C, which starts behind the crew area.
V-F's only open item is the ghost E34; E35 and E40 need the `TYVANCE`
login (step 2). That is intended: the deepest facts need the unlocks.

## Matrix check

Items per fact, media per fact, and whether the fact survives the loss of
any one item (at least two items in at least two media remain).

| Fact | Evidence ids | Items | Media | Media list | Survives loss of any one |
| --- | --- | --- | --- | --- | --- |
| H-I | E01 E02 E05 E10 | 4 | 4 | FILE WHO MSG ART | yes |
| H-F | E03 E05 E12 E17 E23 E31 | 6 | 5 | WHO MSG ART LOG DOOR | yes |
| H-C | E06 E17 E22 E23 E26 E31 | 6 | 4 | MSG LOG DIZ DOOR | yes |
| A-I | E01 E02 E06 E10 E16 | 5 | 4 | FILE WHO MSG ART | yes |
| A-F | E03 E09 E14 E19 E26 | 5 | 5 | WHO FILE ART LOG MSG | yes |
| A-C | E09 E19 E22 E25 E26 E31 | 6 | 5 | FILE LOG DIZ MSG DOOR | yes |
| T-I | E01 E02 E07 E10 | 4 | 4 | FILE WHO MSG ART | yes |
| T-F | E03 E07 E13 E18 E24 E31 | 6 | 5 | WHO MSG ART LOG DOOR | yes |
| T-C | E18 E22 E24 E31 | 4 | 4 | LOG DIZ MSG DOOR | yes |
| V-I | E01 E02 E08 E09 E10 E39 | 6 | 5 | FILE WHO MSG ART MUSIC | yes |
| V-F | E20 E31 E33 E34 E35 E40 | 6 | 5 | LOG DOOR WHO ART MUSIC | yes |
| V-C | E08 E09 E15 E20 E22 E25 E27 E28 E35 | 9 | 5 | MSG FILE ART LOG DIZ | yes |
| Z-I | E01 E04 E11 E31 | 4 | 4 | FILE WHO ART DOOR | yes |
| Z-O | E04 E26 E27 E31 E32 E36 E37 E38 | 8 | 5 | WHO MSG FILE DOOR LOG | yes |
| Z-W | E21 E26 E29 E32 E36 E37 E38 | 7 | 4 | LOG MSG FILE DOOR | yes |

Changes in this revision (S3b): V-F is now the power bay, so E08, E15 and
E28 left it (they stay on V-I or V-C); E33, E34, E35 and E40
joined it. E35 joined V-C; E39 joined V-I; E36, E37 and E38 joined Z-O and
Z-W. No row dropped below 4 items or 4 media.

Result: every fact has at least 4 items and at least 4 media; no fact
depends on one item or one medium.

## Unlock chain

| Step | From -> to | Puzzle | Hint (evidence) | Second route |
| --- | --- | --- | --- | --- |
| 0 | Brownout -> powered ship | Prologue: log in as `admin`/`admin`, switch the source to the RTG | Boot banner: "MAINT ACCT DEFAULTS NOT CHANGED - SEE LABEL"; the console warning text names the key | Wrong commands get terse refusals, never a dead end |
| 1 | Guest -> new caller | Register a handle | WELCOME bulletin: guests read only, ELIZA offers an account | Menu shows register |
| 2 | New caller -> `TYVANCE` login | P1: Ty's birthday is the password | E01 (birthday in bio), E09 (journal: "I use my birthday for everything"), E02 (TYVANCE flagged VIP, "all decks") | Two independent hints |
| 3 | `TYVANCE` -> crew area | Automatic: flag CREW, new menu option | E02 (access: all decks), E25 waits in the inbox | Visible unlock |
| 4 | Crew area -> Achterberg's trail | P6 overpainted ANSI gives `LADDER`; P3 audit log shows his archive | E16 (covered frame), E06 ("ladder" repeated), E19 (archive made 28 Nov) | Three hints |
| 5 | Trail -> SysOp/ELIZA area | P4: DIAG door; ask the right question by number and the order code from E27 | E26 (which questions she must answer), E30 (query codes), E27 (order code) | Three hints |
| 6 | ELIZA area -> NOWHERE file system | P9: shell login with the NOWHERE host and the order code | E21 (host NOWHERE-1), E29 (prospectus), E31 (orders answer) | Three hints |
| 7 | NOWHERE -> transmit | P8: final command; complete manifest | E26 (verb), E30 (syntax), E32 (queue header: destination) | Three parts, none alone |

No step lacks a hint. Steps 2, 3 and 4 have at least two hints each. Step 0 is
the first thing every caller meets (`.kiro/specs/power-prologue/`); step 1
starts after ELIZA wakes. P1 has a third hint in E39 (sample names).

## Early game (R7)

- Before the clock starts: the prologue (step 0) takes a few minutes and ends
  with ELIZA's first line. The two-minute and ten-minute marks below run
  from the main menu after it.
- Within two minutes: the main menu, the WELCOME bulletin, the user list
  (E02) showing `TYVANCE` as "all decks", and the CREW bulletin (E01)
  with his birthday. The lead: "Ty never reads what he approves."
- Within ten minutes: the `TYVANCE` login (step 2) opens the CREW option
  and Ty's inbox. A ghost page from ELIZA (E03/E04) lands inside five
  minutes online.

## Notes for later specs

- Overpainted ANSI (E16) is drawn by overstrike at low baud. The current
  ANSI rules forbid cursor moves; carriage return without line feed is the
  only tool, and needs an exception in `ansi-art.md`.
- Future-dated posts: no longer an issue. The epoch is 8 Dec 04:17, after
  every post.
- Crew accounts (`TYVANCE`, `CMDR`, `GARDEN`, `CHAOS`) and `admin` are
  read-only and take many callers at once (power-prologue spec).
- Engine need: the board's MOD player (`ASSETS/PLAYER/player.js`) shows
  title, type, channels and artist, but not sample or instrument names.
  E39 and E40 need them shown (browser) and a text view of the sample names
  for Telnet. See tasks N12.
- Engine and content need: short original WAV/MP3 sounds (power hum, relay
  clicks, a reboot chirp). Recorded in tasks, designed in S7.
- Passwords and codes are placeholders until S3: `TYVANCE` = birthday as
  digits, `CHAOS` = `LADDER`.
