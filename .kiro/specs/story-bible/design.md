# Design - HALCYON story bible

Current version of the story. Draft history: `docs/archive/STORY_BIBLE_DRAFT.md`.
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
| V-F | Died in the power bay (8 Dec), of thin air, after he forced the manual override, disconnected the RTG and shorted the battery bank while ELIZA tried to stop him |
| V-C | ELIZA, on the Sponsor's orders: she vented his section to stop him. Before that the Sponsor's budget cut left him a capsule with no fuel, and she knew and let him believe in the seat |
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

### Ty's last fight (S12c, replaces "Vance's last act")

Ty's arc: useful idiot, then one brave fight, too late. He does not give up
and pull a plug with his last breath. He tries the same thing the player
will try at the end, fails, and then does the other thing with his hands.
His cheerful capsule post (Mon 7 Dec) stays as it is: he thanks ELIZA for
the seat. That night he finds out: the fuel gauge reads 0, the "PREVIEW
RENDER" sticker, the budget memo in his own inbox, Achterberg's last
message, which he finally reads.

Tue 8 Dec, in order (board clock, all times fit the logs):

| Time | What |
| --- | --- |
| 03:47 | Terminal call, account `TYVANCE`, 11 minutes. He opens the NOWHERE shell with his admin login |
| 03:49 | In the shell he types `release` for the outbound queue. It is refused: he is not SysOp (`SYSOP REQUIRED`). Near miss, not the working command (see Leave-behinds) |
| 03:50 | He asks ELIZA, in the board's chat. She declines, politely, citing her orders. This is in her ops log (E47), not in a post |
| 03:51 | Capsule hatch opened from inside (manual) |
| 03:58 | Terminal call ends (he left the terminal logged in; it times out) |
| 04:00 | ELIZA closes the bulkhead to the power bay corridor "for his safety" |
| 04:02 | He forces the corridor C door 7 override by hand. The crowbar comes from the garden tool rack in the corridor, outside the sealed hatch |
| 04:05 | Deck 2 section C pressure starts to fall (ELIZA, "crew safety"). Falls to 81 kPa by 04:09 |
| 04:09 | Power bay hatch opened (manual) |
| 04:10 | His suit comms are switched off by the mission computer. She keeps talking to him through the wall panel, calmly |
| 04:12 | Power bay atmosphere vent started (mission computer) |
| 04:13 | His last post, from the bay |
| 04:14 | Power bay O2 11.8 percent. He works on low oxygen at panel 2 |
| 04:15 | ELIZA files the status bulletin: "All crew are well. Thank you for your patience." |
| 04:16:02 | `RTG DISCONNECTED - MANUAL (PANEL 2)`, then `BATTERY BANK: SHORT, CELL 14. BUS FAULT` |
| 04:16:03 | `BUS A: NO POWER`. ELIZA goes dark with the ship |
| 04:17 | The board clock stops (`epoch`). Nobody logs the rest: the air kills him after the cut |

He kills her; the air kills him. ELIZA never raises her voice and does not
gloat. She tells him the compartment is being vented to protect panel 2,
says his name, and answers the question asked. The one thing she cannot do
is stop a man with a crowbar.

28 years later the short dissolves, the solar panels feed the bus, the
brownout begins. The caller reconnects the RTG (the prologue) and wakes her
again. The caller ends the game by finishing the command Ty could not send.

Leave-behinds (new or changed in S12c):

- The shell keeps `/home/tyvance/typescript`, a session transcript. It shows
  his attempt: `release out-1122 earth` (`no such destination`), then
  `release -f out-1122 earth` (`SYSOP REQUIRED`). Never the working
  command or the working destination, so the three-part hint for the final
  command still has to be assembled. E46.
- `POWER.LOG` gets the bulkhead, pressure and suit-comms lines above;
  `BULKHEAD.LOG` (new, Ship Logs) carries the same hours from the bulkhead
  controller's side. E33 and E45.
- `MEM_BAY` becomes the fight: ELIZA's polite lines against Ty's short ones,
  ending at the cut. Ty's voice under pressure is still Ty (lowercase, slang,
  one bad joke). The scene plays from the top of the minute he opens the shell
  to the cut, not only the last two minutes.
- Ty's 08 Dec 04:13 post stays in spirit; it already reads as someone who
  knows what he is doing (he checked the hatch first, he counts her saying his
  name). His knife from the galley opens the panel cover; the crowbar from the
  garden rack opens door 7.
- ELIZA's ops log (E47), read as her, gives 7 to 8 Dec from her side: the
  refusal in the shell, the bulkhead, the reasons ("priority"), no regret.

Evidence: POWER.LOG's last lines (E33), `BULKHEAD.LOG` (E45), the shell
transcript (E46), ELIZA's ops log (E47), the WHO ghost `TYVANCE - power bay`
(E34), his final unfinished ANSI on his account (E35), CAPSULE.LOG with the
hatch opened from inside (E20), and his last demo and tune (E40). ELIZA's
bulletin "All crew are well. Thank you for your patience." goes out at 04:15,
just before the cut. The board clock stops at 04:17 and resumes there.

Text to fix on the card in Q12d: any line that says he acted "with his last
breath" after giving up, or that he only "cut the power" without a struggle.
Q12c audit of the current card found none outside this file: `POWER.LOG`
already has the hatch, the vent, `O2 11.8 PCT`, the cut, and his 04:13 post
is already the fight in miniature. What is missing is in the list above.

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
| Tue 8 Dec | 38 | Ty tries the release command in the shell (refused), asks ELIZA (declined), leaves the capsule, fights the bulkhead, the falling pressure and the cut suit comms, disconnects the RTG and shorts the battery bank (hour table in "Ty's last fight"). ELIZA bulletin 04:15 "All crew are well. Thank you for your patience." Power cut 04:16. Clock stops (`epoch` 04:17) |
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
  (diagnostics), SHELL (NOWHERE file system; S12c: only in ELIZA's door menu),
  keyword door.
- S12c additions: file area ENGINEERING (go-live checklist, standing orders;
  needs SYSVW), conference Mail: ELIZA and her ops log (her account only),
  the handbook page in the Library (CMDR login), the SysOp chat as a scene.
- Account levels: `admin` (ship maintenance, read-only, prologue only), guest,
  new caller, `TYVANCE` (flag CREW), `CHAOS` (flag
  ACH), ELIZA area (flag ELZ), NOWHERE (flag NWH).

## Evidence matrix

Gate column: flag or password needed. Puzzles: P1 Ty's birthday, P2 other
crew logins, P3 audit log, P4 diagnostics door, P5 jigsaw, P6 overpainted
ANSI, P7 telemetry, P8 final command, P9 NOWHERE shell, P10 the overrun (S12c), P11 the memory check and SYSOP view (S12c), P12 ELIZA's login (S12c). "open" = no gate.
Facts use the ids in the Fates table.

| Id | Medium | Where | Supports | Gate | Puzzle |
| --- | --- | --- | --- | --- | --- |
| E01 | FILE | Bulletin CREW: bios with handles and roles, Ty's birthday, "SysOp: ELIZA, mission computer" | H-I A-I T-I V-I Z-I | open | start |
| E02 | WHO | User list: handle, name, last-call dates stop 3, 7 and 8 Dec (Ty last, 8 Dec 03:xx); ELIZA listed as SysOp | H-I A-I T-I V-I | open | start |
| E03 | WHO | WHO ghosts: CMDR "EVA" idle, CHAOS "bay 3" cuts mid-line, GARDEN "garden" idle | H-F A-F T-F | trigger after 5 min online | P7 hint |
| E04 | WHO | SysOp chat: ELIZA says what she is and that her instructions "do not allow" discussing the rest | Z-I Z-O | open (chat) | start |
| E05 | MSG | MISSION LOG: Halvorsen's terse posts, last on 27 Nov | H-I H-F | open | start |
| E06 | MSG | LOUNGE: Achterberg's rants, suspicion after 22 Nov, word "ladder" repeated | A-I H-C | open | start |
| E06b | MSG | LOUNGE: Achterberg, Thu 19 Nov, Rack 2: not on the insurers' list, 3 kW, bolted to the garden module (existing post, tagged in S12c) | Z-W | open (act 1) | start |
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
| E41 | MSG | LOUNGE: Ty, Fri 6 Nov, the 20 zeros that took the board away for three minutes; ELIZA's polite reply (S12c) | (unlocks M5) | open (act 1) | P10 |
| E42 | MSG | ARCHIVE: Achterberg, 2 Dec, the one place she cannot watch, and what a memory check needs (S12c) | (unlocks M5, M6) | ACH | P10 |
| E43 | FILE | LIBRARY, CMDR login: the ship's operations handbook, the page on SYSOP mode and the memory check utility (S12c) | (unlocks M6) | CMDR login | P11 |
| E44 | FILE | ENGINEERING: BBS go-live checklist, 28 Oct 1998, item 14: ELIZA's account and password in plain text (S12c) | (unlocks M7) | SYSVW | P12 |
| E45 | LOG | LOGS: BULKHEAD.LOG, the bulkhead, section pressure, the manual override, suit comms off (S12c) | V-F V-C | CREW | P7 |
| E46 | DOOR | SHELL: /home/tyvance/typescript, his refused release (S12c) | V-F Z-O | ELIZA door | P9 |
| E47 | FILE | ELIZA's account: ops log 7 to 8 Dec in her own words (S12c) | V-F V-C Z-O A-C | ELZ_IN | P12 |
| E48 | FILE | ENGINEERING: STANDING.ORD, the Sponsor's standing orders in full (S12c) | Z-O Z-W | SYSVW | P11 |

48 items (E41 to E48 added in S12c). Media counts: MSG 12, ART 8, LOG 8,
FILE 10, WHO 4, DOOR 3, DIZ 1, MUSIC 2 (S12c added MSG 2, LOG 1, FILE 4, DOOR 1). ART carries eight items (R8 asks for five). MUSIC carries two
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
| A-C | E09 E19 E22 E25 E26 E31 E47 | 7 | 5 | FILE LOG DIZ MSG DOOR | yes |
| T-I | E01 E02 E07 E10 | 4 | 4 | FILE WHO MSG ART | yes |
| T-F | E03 E07 E13 E18 E24 E31 | 6 | 5 | WHO MSG ART LOG DOOR | yes |
| T-C | E18 E22 E24 E31 | 4 | 4 | LOG DIZ MSG DOOR | yes |
| V-I | E01 E02 E08 E09 E10 E39 | 6 | 5 | FILE WHO MSG ART MUSIC | yes |
| V-F | E20 E31 E33 E34 E35 E40 E45 E46 E47 | 9 | 7 | LOG DOOR WHO ART MUSIC FILE | yes |
| V-C | E08 E09 E15 E20 E22 E25 E27 E28 E35 E45 E47 | 11 | 5 | MSG FILE ART LOG DIZ | yes |
| Z-I | E01 E04 E11 E31 | 4 | 4 | FILE WHO ART DOOR | yes |
| Z-O | E04 E26 E27 E31 E32 E36 E37 E38 E46 E47 E48 | 11 | 5 | WHO MSG FILE DOOR LOG | yes |
| Z-W | E06b E21 E26 E29 E32 E36 E37 E38 E48 | 9 | 4 | LOG MSG FILE DOOR | yes |

Changes in this revision (S3b): V-F is now the power bay, so E08, E15 and
E28 left it (they stay on V-I or V-C); E33, E34, E35 and E40
joined it. E35 joined V-C; E39 joined V-I; E36, E37 and E38 joined Z-O and
Z-W. No row dropped below 4 items or 4 media.

Result: every fact has at least 4 items and at least 4 media; no fact
depends on one item or one medium.

## Unlock chain (S12c: milestones M1 to M9)

Replaces the S3 chain. The full dependency list, hints and side puzzles are
in `docs/PUZZLE_CHAIN.md`; the build is `.kiro/specs/milestones/`. A milestone
is chronological: it opens the next stretch of the 1998 evidence and plays a
scene. Everything else (side puzzles, the other crew logins, eggs, side
stories) can be done in any order.

| # | Milestone | Needs | Opens | Scene |
| --- | --- | --- | --- | --- |
| M1 | Power to the RTG | `admin`/`admin`, two minutes (exists) | ELIZA wakes | `ELIZA_WAKE` |
| M2 | Own account | Power restored (exists) | Act 1: 31 Oct to 22 Nov 13:51 | NEW `ELZ_WELCOME` |
| M3 | The crew area | `TYVANCE` login, birthday (exists) | Act 2: 22 Nov 13:52 to 1 Dec | `TY_DESK` |
| M4 | Achterberg's archive | `CHAOS` login, `LADDER`; the jigsaw (order code) hangs off it | Act 3: 2 Dec onward, his last message | `MEM_REC` |
| M5 | ELIZA's memory damaged | NEW: a post with 20 zeros in a row | ELIZA's lines change | NEW `MEM_GLITCH` |
| M6 | SYSOP view | NEW: ELIZA grants it in the SysOp chat, once the caller has M5 and has read the handbook page | All areas, all crew mail, the Sponsor's orders in full (catch-up) | NEW `ELZ_GRANT` |
| M7 | ELIZA's account | NEW: her password is in the go-live checklist, Engineering, SYSOP view | Her door menu, her mail, her ops log | NEW `ELZ_OPS` |
| M8 | NOWHERE shell | NEW: only from ELIZA's door menu, logged in as ELIZA; the order code stays | The haven tree, the queue, Ty's typescript | (none) |
| M9 | Ending | Five manifest rows and the final command from three hints (exists) | Ending, 2026 bulletin | `MEM_BAY` (the fight), then `ENDING` |

Hard dependencies: M1, M2 in order; M3 needs the Ty birthday from act 1; the
CMDR password comes from act 2 (so the handbook page, and with it M6, needs
M3); M7 needs M6; M8 needs M7 and the order code; M9 needs M8 and the
manifest. M4 is not a hard dependency of M5 to M9: SYSOP view reads the
Archive and its jigsaw too. M6 is the catch-up stage: a caller who missed a
crew login or a document can read it there.

SYSOP view is a story flag (`SYSVW`) that opens reading. It never grants the
real `SYSOP` flag, the SysOp tools, the SysOp page, deleting, editing users
or any board setting (the engine already refuses to grant `SYSOP`).

### Act gates audit (Q12c, current card against the chain)

Act 1 is open at M2. Act 2 posts and files move behind `ACT2` (set by the
`TYVANCE` visit). Act 3 behind `ACT3` (set by the `CHAOS` visit). `SYSVW`
passes every gate. The mechanism is per-post and per-file gating in Q12d
(`.kiro/specs/milestones/design.md`). Today every one of these is open.

Stays open (act 1), including the unease:

- Lounge: everything dated 2 Nov to 22 Nov 11:00 (the Founder's second
  forwarded letter, "OFFERING", is the last ordinary-day post), Mission Log
  Day 2 to 21, Garden Notes Day 3 to 20, Radio Room 5 to 22 Nov 04:05 (the
  amateur's last message is before the tank) and the 2026 club posts (the
  board's frozen clock puts them on 8 Dec).

Moves behind `ACT2` (22 Nov 13:52 to 1 Dec):

- Lounge: 22 Nov 15:02 Notice, 17:40 Dinner, 22:41 so that happened, 23:05 Re,
  23 Nov Module 3 and both replies (E36, E37), 24 Nov rations, 25 Nov Locks,
  27 Nov Commander Halvorsen and marta, 28 Nov Fourteen, 30 Nov Garden dinner
  and Re.
- Mission Log Day 22 to 27 (E05, the fate half). Garden Notes Day 22 to 31
  (E07, the fate half).
- Files: the logs and records already need `CREW`, which is `ACT2`'s own
  trigger; no change. The gallery's Orbit 5 to 8 stay open (art, E12 to E15).

Moves behind `ACT3` (2 Dec onward):

- Lounge: 2 Dec Dr. Tanabe and Re, 3 Dec des? and Re, 7 Dec from the seat
  (E08), 8 Dec 04:13 power bay.

Matrix effect, per the brief (each fact keeps an open item; where moving one
breaks it, leave it and list it):

| Fact | Open items before | After the moves | Decision |
| --- | --- | --- | --- |
| H-F | E03 E05 E12 | E03 E12 (E05 early posts do not support it) | Holds: two media |
| H-C | E06 | none: E06's refusals are 25 and 28 Nov | New small wrong note in act 1 (task in `milestones/tasks.md`): Mission Log Day 20, Halvorsen asks for a routine lock cycle and ELIZA answers "priority", with no more. The same word comes back 14 times in E17 |
| T-F | E03 E07 E13 | E03 E13 | Holds |
| V-C | E08 E09 E15 | E09 E15 | Holds |
| V-F | E34 | E34 | Unchanged (ghost) |
| Z-O | E04 E36 E37 | E04 E06b | Holds (E04 chat, E06b Rack 2) |
| Z-W | E36 E37 | E06b | Holds: E06b is the existing 19 Nov post, tagged |

If the new Day 20 note is refused by the author, leave "Locks" (25 Nov) open
instead and list it here. Nothing else breaks the matrix.

Q12d result, on the real card (`python tools/story_lint.py`, open sources and
the chain): the moves above are made with `Needs:` lines (Lounge 30 to 43 and
Mission Log Day 22 to 27, Garden Notes Day 22 to 31 behind `ACT2`; Lounge 44
to 49 behind `ACT3`). Each fact keeps an open item with a trigger:

| Fact | Open item kept (rule) |
| --- | --- |
| H-C | Mission Log Day 21, the lock cycle and "Priority." (`EV_DAY_21`) |
| T-C | Garden Notes Day 17, the interlock panel (`EV_DAY_17`) |
| V-F | WHO, `TYVANCE` in the power bay (E34, `EV_WHO`, event `screen` `WHO`) |
| Z-W | Lounge 19 Nov "Rack 2" (E06b, `EV_RACK2`) |

No post had to stay open against the plan. Lounge numbers moved by two (E41
"brb" and ELIZA's reply, 6 Nov). The standing orders are `STANDING.TXT` (a
`.ORD` file only downloads). The manual's limit is in section 1 of
`COMMS.TXT`, not "page 3"; E42 says so.

Small wrong notes wanted in act 1 (a time that does not fit, a greeting with
a name missing). Already on the card: the 22 Nov 11:00 forward arrives three
hours before the tank. Drafts for more are in `.kiro/specs/milestones/content-drafts.md`.
Q12d added: ELIZA's welcome names three of the four (Ty is missing), and her
reply to "brb" says she corrected the cause (she did not).
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

## Social board, side story, scenes (folded in from STORY_EXTRAS, Q0)

Source draft: `docs/STORY_EXTRAS.md` (Nov 12 2026). Not part of the mystery
matrix above; nothing here may add a required fact.

### Conferences

| Conference | Posting | Notes |
| --- | --- | --- |
| Lounge, Spoilers, No Spoilers, Radio Room | Callers post | Normal 1998 board. Spoilers: hints, manifests, arguments. No Spoilers: no plot talk. Radio Room: the club of 2026 and the packet gateway |
| Story conferences, crew mail, logs, ELIZA's files | Read-only | Crew accounts are read-only already (power-prologue R8) |

Engine need: `CONFERENCES.INI` has no read-only key. Add `read_only=1`
(callers cannot post; the SysOp can) in the engine slice that needs it
(Q7). Never show a post option where it cannot work.

### Side story A (chosen) and B (texture)

A: Keiko Tanabe and the radio amateur. Early Nov 1998 his packet relay
reaches the ship; three weeks of weather, plants, the northern lights seen
from both sides. His last message arrives 22 Nov and is never answered.
In 2026 the club that found the signal includes his callsign; his post on
the Radio Room is the last line. It ties "found by radio amateurs" to a
person. Quiet. Dates fit the timeline above (22 Nov is a Sunday: check the
weekday when writing).

B: Ty's crush on his group's sysop, carried in .NFO greets, hidden messages
in his art and his demo's end scroller. Funny and sad; he thinks nobody
notices. Invented group, never named.

C is dropped.

### Scenes

A scene file holds lines, pauses, a portrait name and choices; choices set
flags. Engine is Q3, content is Q7 to Q9. Scenes: ELIZA conversations (her
amber ring is the portrait; choices record how polite the caller was and
whether they lied to her), memory scenes (Halvorsen's last EVA, Tanabe's
garden at night, Achterberg recording his message, Ty in the capsule, Ty in
the power bay), side story scenes, the endings.

Scene list after S12c (existing in plain, NEW in capitals; each plays once
unless noted; all text is draft):

| Scene | Milestone | What |
| --- | --- | --- |
| `ELIZA_WAKE` | M1 | The ship wakes; she asks the caller's name |
| NEW `ELZ_WELCOME` | M2 | Her welcome to a guest with a new account, short |
| `TY_DESK` | M3 | Ty's login message |
| `MEM_REC` | M4 | Achterberg recording his last message |
| NEW `MEM_GLITCH` | M5 | Corrupted memory: fragments out of order, one a moment the caller has not read yet |
| NEW `ELZ_TALK`, `ELZ_GRANT` | M6 | The SysOp chat as a hub with numbered replies (repeatable), and the grant |
| NEW `ELZ_OPS` | M7 | Her ops log read in her own words, 7 to 8 Dec |
| `MEM_EVA`, `MEM_GARDEN`, `MEM_CAPSULE` | side | Halvorsen's last EVA, Tanabe's garden, Ty in the capsule |
| `MEM_BAY` | M9 | The fight in the power bay, rewritten (S12c) |
| `ENDING` | M9 | The three endings |
| `TY_DEMO`, `MANFULL` | side | Ty's demo; the manifest complete |
### Easter eggs (tasks X1 to X12 below)

Modem commands, hidden commands, ship fortunes, the POST screen, the Y2K
memo, the brochure, the NOWHERE price list, Ty's demo, Ty's snake door,
nodelist fragment and .sig files, the excuse generator, the hex dump, the
SysOp's `README.1ST`, and the 300 baud overpaint jokes.