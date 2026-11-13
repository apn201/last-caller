# Last Caller - launch kit

Plan for the submission material, 13 Nov 2026. Deadline Fri 4 Dec 2026,
2:00 pm PT (Sat 5 Dec 00:00 Finnish time). Public texts are drafts in
Juha's voice; `[fill: ...]` is a fact only he can give. The long Devpost
story is already in `docs/DEVPOST.md`; this file adds the rest.

## 0. Is the build ready?

Content and code: yes, as far as the queue goes. Q0 to Q14b are ticked.
Last staging check: server build, ctest, smoke, e2e 63 passed, 0 failed.
`halcyon.helppox.com` is live (checked today: boot banner, service label,
`admin`/`admin` prompt).

Not ready yet:

1. **No public GitHub repo.** The only remote is the staging repo. Hard
   requirement. Push the full history (the first commit is the TinyBBS
   copy, everything after is Kiro's).
2. **Board tests.** 14 queue items still say "board test still to do".
   AGENTS.md says a slice is done only after its board test. Needed
   anyway, because the video shows the stick.
3. **Stale docs that a judge will read.** README still describes the old
   board: FidoNet history, "thirty HVSC classics", `fidonet-seed.py` and
   the fetch scripts (both gone from `tools/`), Mod Archive and HVSC
   credits. `CONTENT_POLICY.md` and `THIRD_PARTY_NOTICES.md` the same.
   README says 32 callers, deploy default is 16. `DEVPOST.md` says 16
   specs and 55 e2e steps; the repo has 18 specs and 63 steps.
4. **Story text is still marked draft** in several queue items (Ty's last
   post, MEM_BAY, club posts, news wire, Eero). Your edit pass.

Items 2 to 3 plus press-kit tools go to Kiro as Q16
(`docs/briefs/Q16-launch.md`, after the Q15 refactor). Item 1 and 4 are yours.

## 1. Schedule

| Date | What |
| --- | --- |
| 13 to 16 Nov | Kiro runs Q15 (refactor) and Q16 (launch prep). You do the text edit pass. |
| 17 Nov | Board test on the stick (checklist from Q16). GitHub repo public. |
| 18 to 20 Nov | Big box art, ELIZA eye render, photos of the stick. |
| 21 to 24 Nov | Record screen footage and Kiro footage. Shoot the box and stick. |
| 25 to 27 Nov | Edit video, voiceover, thumbnail. Upload unlisted. |
| 28 Nov | Devpost filled, screenshots up, submit (editable until deadline). |
| 29 Nov to 2 Dec | Blog post and social post. Fix only what is broken. |
| 3 Dec | Final check. Video public. Nothing changes after 4 Dec. |

Freeze rule from Devpost: after the deadline the repo, video and hosted
board stay as they are until winners are announced. Reset the hosted
board once on 3 Dec with `deploy/reset.sh`, then leave it.

## 2. Devpost fields

**Project name:** `The Last Caller` (the repo and README say "Last
Caller"; pick one. The title screen decides.)

**Elevator pitch** (one line, under 200 characters):

> Dial into the BBS of a dead 1998 spaceship and find out what happened to the crew. A real BBS on a $10 ESP32 stick, built with Kiro.

Alternative:

> A 1998 BBS left running on a lost spaceship. Log in as admin/admin, wake the ship's AI, and read your way to what killed the crew.

**About the project:** `docs/DEVPOST.md`, after your edit. Devpost wants
the headings Inspiration / What it does / How I built it / Challenges /
What I learned / What's next. All are in the draft now. Add 2 to 3 gallery images inline (boot
banner, ELIZA wake, the stick).

**Built with** (25 max):

`c++`, `esp32`, `esp32-s3`, `arduino`, `platformio`, `javascript`,
`html5`, `canvas`, `websocket`, `telnet`, `python`, `cmake`, `linux`,
`apache`, `civetweb`, `kiro`, `kiro-cli`, `mcp`, `libopenmpt`, `jssid`,
`ansi-art`, `property-based-testing`, `fat32`, `bbs`, `rocky-linux`

**Try it out links:**
- `https://halcyon.helppox.com` (the board)
- `https://github.com/apn201/last-caller` (once public)
- `https://github.com/apn201/TinyBBS` (the engine)

**Category:** Resurrection. BBS is the dead technology, brought back on
hardware it was never meant for. (Frankenstein would also fit: a 1998 BBS
stitched to WebSocket and a game engine. Resurrection is the cleaner
argument.)

**Demo URL for judges:** `https://halcyon.helppox.com`

**Testing instructions (private):**

> Open https://halcyon.helppox.com in a desktop browser and press DIAL. The first login is the ship's maintenance account: admin / admin (it is printed on the service label on screen). You then have about two minutes of solar power to switch the ship to the RTG; the hint is on the screen. If the carrier drops, dial again, the ship comes back. After that ELIZA lets you create your own account. A first play takes 60 to 120 minutes. If you are short of time, the first 10 minutes show the core: the prologue, ELIZA, the menus, Ty's ANSI art at 300 baud (set SPEED 300 in the top bar) and the music player. Spoilers board: the Spoilers conference has hints from other players. Telnet is not open on the hosted board; the video shows Telnet on the stick.

Add to the end of that text, with the real passwords (they are made on the host, never in the repo):

> We recommend starting from the beginning, the prologue is part of the fun. If you are short of time there are saved games: log in as JUDGE2 (start of act 2) or JUDGE3 (start of act 3), password [from the host]. They are separate accounts, so your own game is not affected.

Decision 13 Nov: start from the beginning is the recommendation; the
JUDGE accounts are savegames for judges in a hurry. Q16 makes the tool.

**How the project meets the Kiro requirement:** paste "How Kiro was used"
from `DEVPOST.md`. Products to name, one line each: Kiro CLI (headless
queue runs), Kiro IDE, specs (18), steering (7), hooks (3), custom agents
(4), MCP (`bbs-caller`, own server), property-based testing (story
property test in ctest). Powers: not used.

**Pre-existing work:** the paragraph in `DEVPOST.md` "Everything was
built in the submission period ... first commit, 12 Nov 2026, MIT", plus
the third-party list.

**Bonus prizes:**
- Blog prize: a post about Kiro, published in the period. Topic that
  writes itself: "Kiro played my game through an MCP Telnet client".
- Social Blitz: post starting with `#KiroweenHackathon26`, favourite Kiro
  thing. Draft below.
- Ghost of Kiroween Past: no (not registered for Kiroween 2025).
- Startup, Student, Kiro Crew: no.

**Other form fields:** submitter type Individual; country Finland; AWS
employee No.

## 3. Film (under 3 minutes)

Judges stop at 3:00. Target 2:50. The rules want: it works, on what it was
built for, and how Kiro was used. Music: only Ty's own modules
(`SUNSIDE.MOD`, `THERMO.MOD`, `HARBOUR.MOD`). No Mod Archive, no HVSC.
Capture the browser in recording mode (Q16) at 1920x1080. Telnet in
SyncTERM or PuTTY with a DOS font.

Voiceover is optional. If you do it, short lines, your own voice, under
the picture. Draft lines are in quotes.

| Time | Picture | Sound / VO |
| --- | --- | --- |
| 0:00 to 0:08 | Black. Then macro shot: the stick going into a USB power bank. Tiny LCD lights up with the HALCYON logo. | Modem handshake (only if the MP3 is cleared). |
| 0:08 to 0:16 | Phone: wifi list shows `HALCYON`, tap, captive portal opens the terminal by itself. | "This is a BBS on a ten dollar USB stick. No internet. It makes its own wifi." |
| 0:16 to 0:20 | Title card in ANSI: LAST CALLER. | Silence. |
| 0:20 to 0:45 | Browser: boot banner, service label, type admin/admin, the two-minute power countdown, switch to RTG. ELIZA wake art lights segment by segment. | "In 1998 a private spaceflight carried a BBS so the crew could write home. In 2026 radio amateurs pick up the signal. You dial in." |
| 0:45 to 1:15 | Main menu art, message conference, a crew post, WHO with ghost callers, ELIZA saying something polite in chat. Ty's gallery: one portrait at 300 baud where the overpaint shows. | `SUNSIDE.MOD` starts. "After that it is a normal 1998 BBS. Messages, files, doors, chat. The crew's posts and logs are the evidence." |
| 1:15 to 1:35 | Puzzle beats, no answers: a crew password prompt, the jigsaw door, the diagnostics door question list, the manifest filling in (blur the names). | "You crack passwords and fill in who died and how. Like Obra Dinn, but you read." |
| 1:35 to 1:50 | Split screen: Telnet client on the left, phone browser on the right, both on the stick. Both in WHO. One pages the other. | "Telnet and browser are the same board. Same nodes, same chat." |
| 1:50 to 2:35 | Kiro. In this order, 7 to 8 s each: (1) `.kiro/specs` folder, story-bible evidence matrix scrolling. (2) steering `writing.md` banned list. (3) save a story file in the IDE, the hook runs story lint, it catches an error. (4) a terminal with `kiro-cli chat --no-interactive` running a queue item; then `git log` scrolling Q1 to Q14. (5) the playtester agent through the `bbs-caller` MCP: its transcript playing the brownout. (6) ctest: property test 300 random orders, e2e 63 passed. | "All of the game is Kiro's. Specs for every slice, even the story. Hooks run the story lint on every save. Kiro CLI ran the queue overnight, one item per run. And Kiro played the game itself, through an MCP server that dials the board over Telnet." |
| 2:35 to 2:50 | The big box. Slow push in, lid off, the Collector's Edition foam with the stick. End card: halcyon.helppox.com, github.com/apn201/last-caller, "Built with Kiro". | `HARBOUR.MOD` ending. "The game is on the card. The collector's edition is the stick." |

Shots to get on the day: stick macro (LCD on, black background, one hard
light), phone in hand joining wifi, laptop and phone on a desk both
connected, the box (three angles), ELIZA's eye render if done. Screen
footage: record each block 2 to 3 times, at 2400 baud for menus and 300
baud for the overpaint.

## 4. YouTube

**Title** (under 70 characters):

- `Last Caller - a haunted BBS on a $10 ESP32 stick (Kiroween 2026)`
- `I dialed into the BBS of a dead spaceship | Kiroween 2026`

**Description:**

> Last Caller is a horror puzzle game that runs on a 1990s style BBS. The BBS runs on a $10 ESP32-S3 USB stick, with its own wifi, Telnet and a browser terminal. There is also a hosted version.
>
> In 1998 a private spaceflight carried a BBS so the crew of four could write home. The mission AI, ELIZA, was the SysOp. In 2026 radio amateurs pick up the signal again. You log in as admin/admin, get the ship on RTG power before the sun goes, and then read the crew's posts, mail, logs and ANSI art to find out what happened to them.
>
> Built for Kiroween 2026. The game, the art and the music are made with Kiro: specs, steering, hooks, custom agents and an MCP server that lets Kiro dial the board and play it.
>
> Play: https://halcyon.helppox.com
> Code: https://github.com/apn201/last-caller
> Engine: https://github.com/apn201/TinyBBS
> Devpost: [fill]
>
> Music: Ty's modules from the game (made for it).
>
> 0:00 The stick
> 0:20 Dialing in
> 0:45 The board
> 1:15 Puzzles
> 1:35 Telnet and browser
> 1:50 How Kiro built it
> 2:35 Collector's edition
>
> #KiroweenHackathon26 #Kiro #BBS #ESP32 #ANSIart

**Tags** (YouTube allows 500 characters):

`BBS, bulletin board system, ANSI art, ESP32, ESP32-S3, Kiro, Kiroween, Kiroween 2026, KiroweenHackathon26, retro computing, dial-up, Telnet, 1990s, horror game, puzzle game, interactive fiction, Obra Dinn, tracker music, MOD music, demoscene, ELIZA, AI horror, spaceship horror, Arduino, PlatformIO, hackathon, Devpost, AWS, MCP, spec driven development`

## 5. Thumbnail (1280x720)

Three to try, pick by looking at them at phone size:

1. **ELIZA's eye.** The amber segmented ring on black, filling the right
   two thirds, one segment dead. Left: `LAST CALLER` in the board's ANSI
   logo font, and below it in small VGA text `admin/admin`. Highest
   contrast, reads at any size.
2. **The stick in the dark.** Macro photo of the stick, the tiny LCD the
   only light, its glow falling on a crumpled crew photo. Text: `THE BBS
   ON THIS STICK IS HAUNTED`.
3. **The boot banner.** The real brownout screen at 4x, cropped to
   `ELIZA MISSION AI ....... NOT RESPONDING`, red and yellow on black,
   with a hand-drawn arrow to the service label. Most honest, least
   pretty.

Rule for all: real screen pixels, nearest-neighbour scaled, no fake glow
filters over the text.

## 6. Screenshots (Devpost gallery)

15 max, 3:2, under 5 MB. Render at 2400x1600 from the real screens (Q16
press-kit tool), not phone photos of a monitor. Order is the gallery
order; the first one is the project card.

1. ELIZA wake screen (the amber ring and logo). Card image.
2. The stick on a desk, phone beside it showing the terminal. Photo.
3. Brownout boot banner with the service label.
4. Power switch screen during the two minutes.
5. Main menu art (the ship in profile).
6. A crew post in a message conference.
7. WHO showing the ghost callers and you.
8. Ty's ANSI gallery piece, 300 baud overpaint mid-draw.
9. Music player with one of Ty's modules and its sample names.
10. Diagnostics door, question list (no answers visible).
11. The manifest with entries blurred or unfilled.
12. Telnet client and browser side by side in chat.
13. Kiro: `.kiro/specs` tree with the story-bible evidence matrix open.
14. Kiro: the playtester agent's MCP transcript next to the same screen.
15. The big box, Collector's Edition open with the stick.

## 7. The big box

A 1990s PC game big box, our own design (no real publisher's look or
logo). Publisher on the box: `APN201`. Box size for print: 23 x 29 x 6 cm
(close to the era's standard). Make it from a single printed dieline on
350 g card, or print sleeves for a cheap gift box of that size.

**Front.** ELIZA's eye in the upper centre, big, amber on black, one
segment dead. Faint ship hull lines in the background, very dark. Title
`LAST CALLER` in a chunky 1990s logo with a chrome-and-amber gradient
(our own lettering, not a copy of any game logo), subtitle `HALCYON`.
Bottom band: `A real-time multi-user science fiction horror experience`.
Corner flash, yellow star: `ANSI GRAPHICS! DIGITAL SOUND!` Bottom left
small badges: `IBM PC & COMPATIBLES`, `MODEM REQUIRED`, `VGA`.

**Back.** The README copy is already in that voice, use it as is:
"A puzzle game like no other, powered by state-of-the-art bulletin board
technology", the five bullets, "Requires a modem and nerves of steel".
Three screenshots in bevelled frames with captions (boot banner, ELIZA,
Ty's gallery). A "System requirements" box:

```
486DX/33 or better, 4 MB RAM
2400 baud Hayes compatible modem (14400 recommended)
VT100 terminal or Netscape Navigator 4
Phone line (long distance charges to low earth orbit may apply)
```

Barcode, a fake catalogue number `APN-1998-HAL`, copyright line
`(C) 1998 HALCYON Holdings. All rights reserved.` and the real small
print `Built for Kiroween 2026. Code MIT.`

**Spines.** Long spine: logo and the eye. Short spine: `LAST CALLER`
and `APN201`.

**Contents, standard edition.** Feelies that help play without spoiling:
- Quick reference card: modem speeds, keys, how to dial.
- The service label as a sticker: `MAINT ACCT ADMIN / PASSWORD ADMIN /
  (CHANGE ON FIRST USE)`.
- HALCYON mission patch (print, or a real embroidered patch later).
- "Letter to the crew families" from HALCYON Holdings, on letterhead,
  polite and empty (sets the tone, gives nothing away).
- Nothing that carries a puzzle answer.

**Deluxe Edition.** Gold sticker on the shrink wrap:
`DELUXE EDITION - CONTAINS THE SHIP'S NODE`. Inside, black foam with a
cutout: the Pocket-Dongle-S3 with the HALCYON card in it, dressed in a
PLA sleeve that makes it a tiny crew interface unit. Its LCD shows
ELIZA's eye, and the eye follows the board: asleep, waking, listening,
speaking (Q17, `docs/briefs/Q17-eliza-lcd.md`). A USB-C cable, and a
small card: "Plug it in. Join HALCYON. It has been waiting." The front
render in `docs/presskit/kling/` still says Collector's Edition; fix the
sticker text in the final art.

**Making the art.** Image tools for the eye and the hull, everything else
from real screens and real photos. Prompt for the front background:

> 1990s PC game box cover painting, airbrushed style, a lone spacecraft
> tumbling slowly in low earth orbit against black space, sun flare on
> its solar panels, a huge amber segmented ring like a mechanical eye
> looming faintly in the dark above it, one segment of the ring dark,
> deep blacks, amber and steel blue palette, painted, no text

## 8. Social and blog drafts

**Social (must start with the hashtag):**

> #KiroweenHackathon26 My favourite thing in Kiro was letting it play my game. I wrote a small MCP server that dials my BBS over Telnet and returns the screen. The playtester agent logged in as admin/admin and got stuck at the first prompt, because my boot banner gave the wrong hint. No unit test would have found that. halcyon.helppox.com

**Blog title ideas:**
- Kiro played my BBS game over Telnet
- Running a game studio from a queue file: 15 nights of Kiro CLI

`[fill: which one, and where it is published]`

## 9. Before you press Submit

- [ ] GitHub repo public, About shows MIT, `.kiro` at root.
- [ ] Repo README is about Last Caller, no stale credits.
- [ ] Video under 3:00, public, shows the stick, the browser, Telnet, Kiro.
- [ ] Judges' test text in the private field, not in the repo.
- [ ] Pre-existing work disclosed (TinyBBS repo, TinyIRC reference,
      Claude and Codex parts, third-party libraries).
- [ ] Hosted board reset on 3 Dec and left alone.
- [ ] Bonus links added (blog, social).
