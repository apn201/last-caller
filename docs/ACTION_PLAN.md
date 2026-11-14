# Action plan to submission (3 Dec 2026)

Written 14 Nov 2026 for Q20, part A. Inputs: `docs/reviews/JUDGES-1.md`
(agreed 34/50: Impact 5, Technological 8, Design 8, Innovation 8,
Presentation 5), `docs/reviews/PANEL-1.md`, the Q18 round-3 reviews
(`R3-ada.md`, `R3-mara.md`) and Juha's notes in `docs/briefs/Q20-action-plan-and-ux.md`.

Nothing in the Devpost, README, game or video areas is applied by Q20. Juha
reviews this plan first. Only the platform items (area 1) are built, in Q20
part B.

Points are of 10 on the named criterion, they overlap and do not add up. The
panel's ceiling for all small items together is Presentation 7, Design 8 to 9,
total 36 to 37.

## Decisions for Juha (top of the plan)

### D1. The opening paragraph: "feasibility study" and "product"

**DECIDED by Juha, 14 Nov 2026 (replaces the options below):** frame it
clearly as a game, on the Resurrection theme: "We resurrected BBS technology
to study its feasibility as a game engine." Other uses worth studying (a
LoRa/Meshtastic text interface, FidoNet-style sync over a mesh, and so on)
are named as possible directions that were out of scope for this hackathon.
The first paragraph leads with the game; the mesh is one honest "what's
next" line, not a second product. The wording below is superseded; write
the new "In short" to this framing when the Devpost item runs.

The panel wanted both words out because both judges could not tell which one
they were judging. Juha's framing keeps both: the game is a real product, the
project is a feasibility study for rich text on slow links.

| Option | Effect |
| --- | --- |
| A. Keep both, as now | Judges read it twice. Presentation stays 5. |
| B. Drop both, lead with the game (panel) | Clear, but loses Juha's framing. |
| C. Keep both ideas, one plain sentence each, game first (recommended) | Keeps the framing, says what this is in one sentence. |

Recommendation C. Proposed wording (draft, for Juha's edit):

> Last Caller is a horror mystery played on a 1990s bulletin board (BBS), a
> computer you phone, with message boards, files and games, run by one
> person. The ship is dead, the BBS is still on, and the SysOp is the ship's
> AI. The game is a finished product you can play today, and it is also a
> feasibility study: the same small engine runs on a $10 USB stick and on a
> server, and tells a whole story in a few kilobytes, which is what a slow
> radio mesh can carry.

The first sentence says what a BBS is and what this is. "Product" and
"feasibility study" each get a clause, in that order, and neither is in a
list of three things. The mesh is one honest clause: a direction, not built.

### D2. Kouvosto is missing, and the FidoNet credits

The Q19 brief asked for `kouvosto.helppox.com` as the reference board: the
same TinyBBS engine as an ordinary BBS, seeded with about 1000 real FidoNet
messages. Neither `docs/DEVPOST.md` nor `README.md` names it. The panel then
proposes deleting the FidoNet credits as content that "does not ship". Both
are partly right.

Resolution, so every claim is accurate (which content is on which board):

| Content | HALCYON (`halcyon.helppox.com`, the stick) | Kouvosto (`kouvosto.helppox.com`) |
| --- | --- | --- |
| Story: crew posts, files, doors, ELIZA | yes | no |
| FidoNet history seed (about 1000 messages from `tools/fidonet-seed.py`) | no | yes |
| Mod Archive modules, HVSC tunes | no | [check: Juha to say what is on the live board] |
| Legend, dice, VolaMUD and other TinyBBS doors | no | yes (they exist in `../TinyBBS`) |

Options:

| Option | Effect |
| --- | --- |
| A. Delete all FidoNet credits (panel) | Accurate for HALCYON, but loses the proof that the engine is a real BBS. |
| B. Keep credits as they are | False: the README says HALCYON ships FidoNet history. |
| C. Move FidoNet credits to a "Kouvosto" section and say it is the reference board, not part of the game (recommended) | Accurate, and it is the evidence for "real BBS as engine". |

Recommendation C. The README says in one place which board has what. The
Devpost says: "Kouvosto runs the same engine as an ordinary BBS with about
1000 real FidoNet messages. HALCYON ships none of it." Mod Archive and HVSC
credits stay only if Kouvosto really carries them (D2 needs Juha's answer).
The fetch scripts stay in the repository and are described as Kouvosto tools.

### D3. The invented judges

`PANEL-1.md`'s appendix names the invented judges (Hans, Anna, Ada, Mara).
None of those names goes on the real Devpost page or in the film. The
repository files may keep them (they are agents in `.kiro/agents`), but the
page says only "simulated callers and critics". Recommendation: cut, no
options. Owner Kiro, when the Devpost item runs.

### D4. Other open decisions

| # | Decision | Options | Recommendation |
| --- | --- | --- | --- |
| D4a | Finish the film with browser footage or wait for the stick box | Browser only now; stick shots later | Browser footage now, stick for the reveal if the box exists by 26 Nov. A finished film beats a perfect one. |
| D4b | Scripted "are they alive" reply plus one fallback line, or a keyword engine | Scripted; keyword engine | Scripted (both critics agree). The engine waits. |
| D4c | The `[L]` switch-account key | Now; after submission | After submission (both critics, no points for judges). |
| D4d | Who plays and who pays | Audience line only; pricing line | Audience line: hosted board is the product, the stick is the collector's edition. |
| D4e | "AI agents are dead stupid idiots" quote | Keep; cut | Keep, and follow at once with the banner-bug story. |
| D4f | Hide News Wire until the downlink scene | Hide; leave | Hide (small, gives away the last scene). |
| D4g | Keys that move in Q20 part B | See `docs/PLATFORM_UX.md` | Juha reads the "keys moved" table before the TinyBBS port. |

## Schedule

| Date | Work | Owner |
| --- | --- | --- |
| 14 Nov | Plan committed. Q20 part B: audit, contract. | Kiro |
| 15 to 18 Nov | Q20 part B build and usability rounds; Part C port notes. Juha answers D1 to D3. | Kiro, Juha |
| 19 Nov | README rewrite (area 3, item 1), one plain BBS sentence first, Kouvosto section | Kiro |
| 20 Nov | Devpost "In short", counts, `[fill]` gaps, audience line, mesh paragraph, remove the invented judges' names | Kiro, Juha edits |
| 21 to 22 Nov | Game honesty fixes (area 2): Diagnostics line, filed-report feedback, boot banner after power-up, scripted ELIZA reply, row status line, News Wire | Kiro |
| 23 Nov | Replay the game on a fresh board: e2e, story lint, host tests; one judge-style session | Kiro |
| 24 to 26 Nov | Stick box and photos; browser and stick screenshots for the README | Juha |
| 27 Nov | Film plan locked (boot banner first, grief beat, Kiro 35 seconds); everything the film shows must be final | Juha, Kiro |
| 28 Nov to 1 Dec | Film: record, cut, sound. Nothing in the game changes after 27 Nov except bug fixes. | Juha |
| 2 Dec | Public repository, LICENSE, `.kiro` not ignored, test card in the private field, final counts, Devpost final edit | Juha, Kiro |
| 3 Dec | Submit. | Juha |

Before filming (27 Nov): every game fix, the README, the counts on the page,
the UX contract in the browser terminal (the film shows the keys).

After submission: `[L]` switch-account key, keyword chat engine, one light
crew scene near day 20, answer bundling in the manifest, the TinyBBS port (it
is a separate item inside that repository and does not touch Last Caller).

## 1. Platform (TinyBBS engine)

These go to Q20 part B and then to both repositories (Part C).

| What | Why (review, criterion) | Expected effect | Cost | Risk to the soul | Owner | Queue |
| --- | --- | --- | --- | --- | --- | --- |
| One key rule: single key or ENTER, said on the screen where it matters | Hans lost about ten calls; Anna twice (judges, Design). Panel: "Key and ENTER rules" | Design +0.5 | medium | none | Kiro | Q20 B |
| Same letter, same action in every menu; move the odd one out | Juha's notes, confirmed by both judges | Design, fewer lost keys | medium | none, story clues that name a key move with it | Kiro | Q20 B |
| Pager always, with the same footer and keys whatever the length | Juha's notes; Hans: `N` works on paged messages, not on short ones | Design, one experience | medium | none | Kiro | Q20 B |
| Global keys: back, help, quit, next, previous, main menu | Anna: "Q backs out one level at a time" | Design | medium | none | Kiro | Q20 B |
| Scenes and timed events never swallow a key or cut a line | Hans: a scene dropped him out of a bulletin menu (Design). Panel: "Scenes that eat input" | Design +0.25 | medium | none | Kiro | Q20 B |
| Every chat and door is left the same way, and says how | Anna sent `I` and `R` into Diagnostics. Panel: Q exits ELIZA chat and SysOp chat; in-world door hint | Design +0.25 | small | none | Kiro | Q20 B |
| Menu option area draws, box not empty under Telnet | Hans: options printed under the frame while "COMMANDS" stayed empty (Design) | Design, polish | small | none | Kiro | Q20 B, check first whether it is the tool |
| Border break on Ty's card (bulletin 3) | Hans (Design +0.1) | polish | small | none | Kiro | Q20 B, art item in game list if story text |
| Area line says which conference you are in after `R` | Anna: Lounge listed while the line said Mission Log | Design | small | none | Kiro | Q20 B |
| Browser terminal adds buttons, never other rules | Brief rule | consistency | small | none | Kiro | Q20 B |

## 2. Game (Last Caller only)

| What | Why | Expected effect | Cost | Risk to the soul | Owner | Queue |
| --- | --- | --- | --- | --- | --- | --- |
| Diagnostics: "available to this account" only on the SysOp account, elsewhere "the SysOp's account" | Mara ranks it first (Design) | +0.25 | small | none | Kiro | after Q20 (new item) |
| Filed report feedback: queue header and ELIZA say it is on file; replace "release: command not found" with a held message naming the gate | Panel (Design) | +0.25 | small | none | Kiro | new item |
| Scripted reply to "are they alive" plus one fixed fallback line | Anna's repeat; both critics settled on it (Design) | +0.5 | small | none | Kiro | new item |
| Row status line after the first two rows lock | Anna "what next"; both critics (Design) | +0.5 | small | none | Kiro | new item |
| Boot banner after power-up shows the new state | Hans, Anna notes (Design) | +0.25 | small | none | Kiro | new item |
| Hide News Wire until the downlink scene | Gives away the last scene | +0.25 for full players | small | none | Kiro | new item |
| In-world hint "You are in Diagnostics. Type QUIT to leave." | Anna (Design) | +0.25 | small | none; covered by Q20 B if the platform prints it | Kiro | Q20 B |
| Ty's card border and "his" in a manifest option | Hans, Mara (Design) | +0.1 | small | none | Kiro | new item |
| Second clue to `S` (replay at modem speed) | Hans never found where the word is used | +0.25 for full players | small | some: must not hand over the word | Kiro | new item |
| Answer bundling in the manifest | Panel | +0.25 for full players | medium | none | Kiro | after submission |
| Keyword chat engine | Panel, not this week | +0.25 | medium | some | Kiro | after submission |
| `[L]` switch account, one light crew scene near day 20 | Panel, after submission | 0 for judges | small | some for the scene | Kiro | after submission |
| Refused changes: ELIZA never winks or explains; the manifest never names the wrong row; no hint ladder, no handed-over passwords, no merged accounts; the story is not shortened | Both critics | protects the score | none | none | Kiro, Juha | standing rule |

## 3. Devpost text and README

| What | Why | Expected effect | Cost | Risk to the soul | Owner | Queue |
| --- | --- | --- | --- | --- | --- | --- |
| README: delete the claims that do not ship on HALCYON (Mod Archive, HVSC, FidoNet history, VolaMUD as a HALCYON door), one plain BBS sentence first, 1998 box copy under it | Both judges, Presentation (cost 1 point) | +1 | small | none | Kiro | new item (README) |
| README: Kouvosto section, FidoNet credits moved there (D2) | Q19 brief, D2 | accuracy | small | none | Kiro | README item |
| README, Devpost and repository say the same on doors, counts and what ships | Panel | +0.25 | small | none | Kiro | README item |
| Rewrite "In short" per D1 | Both judges, panel (Presentation) | +1 | small | none | Kiro drafts, Juha edits | Devpost item |
| Counts match the folder: 19 specs, 8 agents (four build, four review), 2 hook files, 7 steering files, 63 e2e steps, 60 to 120 minutes. Re-count on the day, this plan adds the interaction steering file. | Hans, Anna (Technological, Presentation) | +0.5 each | small | none | Kiro | Devpost item |
| Fill or cut every `[fill]` and `[check]` | Anna, Hans (Presentation) | +0.5 | small | none | Kiro, Juha for facts | Devpost item |
| Name who plays and who pays | Anna (Impact) | +0.5 | small | none | Juha | Devpost item |
| Mesh as one honest paragraph: the game is the demonstration, "a direction, not built" | Both critics (Impact) | +0.25 to +0.5 | small | none | Juha edits | Devpost item |
| Kiro proof beats opinion: the pick-list rule seen in play, the banner-bug story, a true answer for the property test | Hans (Technological) | +0.25 | small | none | Kiro | Devpost item |
| Remove "12 hours" and the credits figure on the day; cut the invented judges' names (D3) | Panel | +0.1 | small | none | Kiro | Devpost item |
| Judge test card: admin/admin, `SOURCE RTG`, press K; Q backs out one level (update to the new contract); the manifest never says which row is wrong, on purpose; "first act about 40 minutes, full story 60 to 120" | Anna stopped for lack of a next step | Design +0.25, Presentation +0.25 | small | none | Kiro | Devpost item, after Q20 B so the keys are right |
| Browser first in judge instructions, Telnet second | Panel | +0.25 | small | none | Kiro | Devpost item |

## 4. Video plan

| What | Why | Expected effect | Cost | Risk to the soul | Owner | Queue |
| --- | --- | --- | --- | --- | --- | --- |
| The film exists: 3 minutes, browser footage is enough if the box is not ready | Both judges: it is a plan (Presentation) | +1 to +1.5 | large | none | Juha | Q16 |
| Open on the boot banner, 20 seconds: banner, service label, admin/admin, amber ring; then the stick: "This is all of it." | Panel (Presentation) | +0.5 | small | none | Juha | Q16 |
| Humour beat 0:20 to 0:45: the label with its arrow, one Sponsor memo line in caps for three seconds | Panel | +0.25 | small | none | Juha | Q16 |
| Grief beat 1:15 to 1:35: Day 31 post ends mid-sentence, then "Crew aboard now: 0. I would like to be accurate about that." No voiceover | Panel | +0.5 | small | none | Juha | Q16 |
| Kiro 35 seconds: the `bbs-caller` agent playing as a caller, one title card each for specs, steering, hooks | Panel (Technological, Presentation) | +0.5 | small | none | Juha | Q16 |
| Mesh one line near the end, Telnet beside the browser, no mesh footage | Panel (Impact) | +0.25 | small | none | Juha | Q16 |
| No voiceover over the dead, original sound only | Panel | protects the score | none | none | Juha | Q16 |
| The film shows the new key contract: do not film before Q20 B is done | This plan | avoids a re-shoot | none | none | Juha | Q16 |

## 5. Repository and Kiro story

| What | Why | Expected effect | Cost | Risk to the soul | Owner | Queue |
| --- | --- | --- | --- | --- | --- | --- |
| A short section pointing to the evidence-matrix spec, `writing.md`, the story lint hook, the MCP server, one path each | Hans (Technological) | +0.25 | small | none | Kiro | README item |
| Say what other tools built (Claude, Codex, Kiro): keep it | It is why Technological stays 8 | 0 | none | none | Juha | standing |
| Public GitHub repository, LICENSE visible in About, `.kiro` not ignored | Panel (Presentation) | protects the score | small | none | Juha | final day |
| Browser and stick screenshots in the README | Panel | +0.25 | small | none | Juha provides stick photo, Kiro browser shot | README item |
| `docs/PLATFORM_UX.md` and `.kiro/steering/interaction.md` as Kiro-story evidence of a usability loop with simulated new users | This item | Technological, a real result with SUS scores | none | none | Kiro | Q20 B |
| Port notes for TinyBBS (Part C), no edits to `../TinyBBS` | Brief | Kouvosto keeps the engine in step | small | none | Kiro | Q20 C, port is a separate item in that repository |
