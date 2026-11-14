# Last Caller - Devpost text (draft)

Draft in Juha's voice from his notes of 13 Nov 2026. `[fill: ...]` marks a
fact only he can give.

## In short

Last Caller is a working 1990s bulletin board system (BBS) for today's
low-bandwidth, offline text links, and a game that runs on it. The BBS
engine, TinyBBS, runs on a $10 ESP32 USB stick and on a Linux server. The
game is a horror mystery you play by calling the BBS of a dead spaceship.
It is for anyone who likes a good puzzle story, and for people who build on
slow radio links such as Meshtastic and want to see how rich and friendly a
text-only system can be. The game is a real product you can play now, on the
hosted board or on the stick. It is also a feasibility study: does BBS
technology still work on today's low-data links? The game is the proof that
it can.
## Inspiration

I decided to resurrect BBS technology to see how relevant it is now, 30
years after the heyday.

BBS was simple and robust technology. It was optimized for very slow
connections and offline operation, because it had to be: processing power
was low, phone lines were slow and expensive, and the board was usually one
PC in someone's bedroom. It works in text mode, so it does not need a heavy
graphical layer. Still it could be artistic, aesthetically pleasing and
easy to use. ANSI art did a lot with 16 colours and 80 columns.

Almost all of this has been replaced by the web browser, and BBSes are seen
as an obsolete relic. But many of the technologies still survive. Small
microcontrollers, embedded systems and slow networks (LoRa, packet radio
etc.) have the same limits the BBS was built for. In emergency
communications, like Meshtastic, BBS technology would allow a graphically
rich experience on very simple devices. A screen of ANSI is a few kilobytes.

So this is a study of how BBS techniques work today.

There is a practical problem behind it. Text-mode communication over slow,
high-latency or offline links is hard to use and joyless. Meshtastic and
other LoRa mesh networks today offer little more than a public chat line.
The BBS solved exactly this problem 30 years ago. A BBS could hold message
boards that were copied slowly from machine to machine, the way FidoNet did,
and it had chat between the people connected to one node. TinyBBS could run
on small ESP-based Meshtastic devices and work the same way: message boards
that sync slowly between devices over the mesh, using private channels, and
node chat like the public chat of Meshtastic today. So the retro has a
function. It is a way to learn from old systems how to make text-based
systems intuitive and fun. The demo game is entertaining, which is worth
something in itself, and it shows what a few kilobytes of ANSI can carry.

What exists and what does not: the mesh sync is a direction, not built. It
does not run on Meshtastic today. What exists is the part that makes it
possible. The BBS core does not know how bytes reach it. Telnet and the
browser are two transports into the same core, and a mesh link would be a
third.

Why a game and not a utility? One reason was Kiro. A utility is mostly
tested by checking that its functions work. A game has to work on every
level at once: the story, the gameplay, the puzzles and the technical
implementation. If one of them fails, the whole game fails. So I thought a
game would test Kiro's abilities better than a utility, and it did force
testing on all of those levels: playtester and critic agents for the story
and puzzles, usability testers for the interface, and property tests and an
end to end script for the code.

## What it does

The core of the project is TinyBBS (fully built within the hackathon
window). It was first written natively for an ESP32-S3 USB dongle, with its
own internal Telnet and web servers, and only after that ported to Linux.
All the data and most of the C++ stayed exactly the same. C++ felt period
correct, and it is still relevant on microcontrollers. All the features in
the Apache version also work on the stick.

It works on Telnet. For easier testing and some convenience features
(listening to music etc.) it also runs in a browser terminal window. Both
are just transports into the same BBS, so a Telnet caller and a browser
caller see each other in WHO and can chat.

For the demo I decided to use it as a game engine, for a horror
puzzle adventure in the style of Return of the Obra Dinn. The player dials
into the BBS of a dead spaceship and has to hack it to find out what
happened to the crew. Could had been anything, but a dead board with a dead
crew felt like the right story for a resurrected technology.
The unusual part is that the game engine is a real, working BBS. The story
is not a game engine dressed up as a BBS. It is told through the things a BBS
has: message bases, file areas, door games, node chat and logins.

The story: HALCYON was a secret private spaceflight in 1998, crew of four.
The ship had a BBS on a 486 so the crew could write home, and the mission
AI, ELIZA, ran it as SysOp. Mission control lost contact and nobody ever
announced it. In November 2026 the shorted battery bank has gone open
circuit, the solar panels feed the bus again, and radio amateurs pick up the
signal. The ship tumbles, so the BBS has power for a minute or two at a
time. You log in as the maintenance account, `admin`/`admin`, and have two
minutes to switch the ship to the RTG before the sun goes away and the
carrier drops. Then ELIZA wakes up, very politely.

After that it is a normal 1998 BBS. Message conferences, file areas, door
games, chat, WHO, bulletins, ANSI art on every screen. The crew's posts,
mail, logs, Ty's ANSI art and his tracker modules are the evidence. You
crack crew passwords, put together a jigsaw of ANSI pieces, read a portrait
that is overpainted at 300 baud, ask ELIZA questions by number in the
diagnostics door, and fill in the crew manifest: who each person was, how
they died and who caused it. Facts confirm in pairs, like in Obra Dinn. The
ending is sending ELIZA's held outbound queue to the world.

It is a solo game, but the board is social. Story areas and crew accounts
are read only. The Lounge, Spoilers, No Spoilers and Radio Room work like
any BBS, so players can trade hints, or stay away from them. A first time
player should finish in about 60 to 120 minutes, with no grinding.

## How I built it

The hardware is a Pocket-Dongle-S3: ESP32-S3 with 16 MB flash and 8 MB
PSRAM, a tiny 160x80 LCD, a microSD slot and a BOOT button. About $10.
Nothing attached, no modem, no keyboard. It makes its own WiFi network with
a captive portal, so a phone opens the terminal by itself. It never joins
another network.

- One translation unit split into headers. The core reaches the machine
  only through `src/platform/platform.h`: `esp32.h` on the stick, a POSIX
  version for the Linux server (civetweb for HTTP and WebSocket). No ESP32
  or Linux headers in the core.
- Eight nodes in a fixed pool of session structs, each a state machine with
  an input line and a capped output buffer. A transport only pushes bytes in
  and drains bytes out.
- Telnet has a small IAC state machine that refuses everything except ECHO
  and SUPPRESS-GO-AHEAD. CR, LF and CRLF all end a line, so PuTTY and raw
  clients behave the same.
- The browser terminal is one page compiled into flash: 80x25 on a canvas,
  8x16 cells, the 16 colour DOS palette and an ANSI parser in JavaScript.
  The carrier is a WebSocket with one letter frames, with HTTP polling as a
  fallback. A page reload rejoins the same node.
- ANSI files stream from the card at the caller's chosen modem speed, and a
  chunk never ends in the middle of an escape sequence. That is also how the
  300 baud overpaint puzzle works: a line is drawn, then a bare CR, then a
  second line over it. At 300 baud you can read the first one.
- Everything is plain text on a FAT32 card: users, messages, config, story,
  art. Record rewrites go to a `.NEW` file first and are swapped in with a
  `.BAK`. Passwords are salted SHA-256 with the salt from the hardware RNG.
- There is no real time clock and no NTP, so the board invents one. For
  HALCYON it is frozen at 8 Dec 1998 04:17, the minute the power was cut,
  and resumes from there.
- The game is data on the card, not firmware: a per caller story record,
  a trigger engine (`TRIGGERS.INI`, flags only grow), a scene engine for
  visual novel style scenes, ghosts in WHO, ELIZA's lines, the diagnostics
  door, the NOWHERE shell and the manifest are all INI files. Change the
  story without reflashing.
- Tracker music (MOD, S3M, XM, IT) plays in the browser with libopenmpt, C64
  SIDs with jsSID. Ty's three tunes are original modules generated by a
  script, and the sample names carry his messages, like scene modules did.
  Telnet gets a text view of the sample names.
- ANSI art is generated by Python scripts and rendered to PNG to check it
  before it goes on the card. CP437 is converted to UTF-8 so it survives
  modern Telnet clients.

Rough split between the LLMs: Claude built the ESP32 version. Codex built
the Linux port (and an OpenClaw agent implemented the server side). Kiro
built the story, the game mechanics and the art, with Claude supervising.
Each one got the role it was best at, and they checked each other's work:
Claude wrote the briefs and reviewed what Kiro committed, Kiro had to prove
every item on a server it did not build.

Everything was built in the submission period. The TinyBBS engine has its
own repo, https://github.com/apn201/TinyBBS, and its commit history shows
it was built inside the hackathon window. It makes sense to keep the pure
BBS platform separate from the game logic. If we go to the very nitty
gritty details: TinyBBS used my earlier TinyIRC sketch as a reference, to
learn the hardware setup of the ESP card and the comms protocols. Nothing
more, and it is part of that repo. We deal with special hardware here, and
often earlier code is a better reference than datasheets (the display
offsets of this board, for example). This repo starts from a copy of it
(the first commit, 12 Nov 2026, MIT), and everything after that commit is
Kiro's.

Pre-existing work and third-party parts (disclosed as the rules ask):
libopenmpt and jsSID for music in the browser, civetweb for the server,
Arduino GFX Library, ESPAsyncWebServer, AsyncTCP and the ESP32 Arduino core
on the stick. The dial-up sound is recorded from a real modem. No Mod
Archive or HVSC tunes ship with the game. Story, art, music for Ty, crew
posts and code are original.
Public resources, honestly: the game uses no dataset. Its story, art, music
and posts are original. The third-party parts are the libraries listed
below. The engine's early slices had optional scripts that filled the board
with archived FidoNet messages and tracker tunes. They are not part of the
game, because the story needs its own voices. I did not want to bolt on a
dataset only to have one.

## How Kiro was used

**Kiro CLI and IDE.** I used the IDE to look at specs, hooks and the MCP
setup [fill: anything else you did in the IDE]. The real work ran in Kiro
CLI, headless: `kiro-cli chat --no-interactive` with one short prompt, "do
item Qn of the queue, tick it and stop". So it could run overnight without
me, one item per run, and every run started clean from the files in the
repo instead of a long chat history. Kiro has been running this for about
12 hours now, mostly unattended. Huge effort. I'm hoping 4000 credits are
enough. By 13 Nov it had used 1390 of the 4000 bonus credits.

**Spec-driven development.** 16 specs in `.kiro/specs`: rebrand, ANSI art,
story bible, power prologue, triggers, scenes, ghosts, three acts, shell
and doors, manifest, music and sound, eggs, verification, playtest fixes,
milestones. Each has EARS requirements, a design and a task list. The story
bible is a spec too: an evidence matrix where every fact about the crew
must be provable by at least four items in four different media (posts,
logs, art, WHO, doors, music), so the game survives if a player misses any
one item. Vibe coding a mystery does not work, because a puzzle that has no
way in is invisible until someone gets stuck. The spec made that checkable.

**Steering.** 7 files: tech rules (portable core, no ESP32 or Linux headers
in shared code), structure, product, card content, ANSI art, story voice
and writing rules. The writing rules made the biggest difference: one voice
per crew member, a list of banned words and shapes that make text sound
machine made, no narrator explaining the scare. [fill: your own opinion on
what worked]

**Hooks.** Saving a web page regenerates the firmware headers. Saving any
story file runs the story lint (unreachable facts, softlocks, lost endings,
broken scenes). Saving the shared game logic builds and runs the host tests
and the property test.

**Custom agents.** ghostwriter (crew posts in each voice), puzzle-smith
(hints and gates), firmware-guard (keeps ESP32 and Linux headers out of the
core) and playtester. Later two adventure-game critics who score the story
and puzzles, and two usability testers who never used a BBS and score the
interface on the System Usability Scale. Each starts with an empty memory
and a new caller account.

**MCP.** I wrote a small MCP server, `bbs-caller`, that dials the board
over Telnet and returns the screen as text. Through it the playtester agent
plays the game on the staging server like a real caller, from the brownout
login to the ending, and writes down where it got stuck. Without it Kiro
could only read its own code and guess. The first run died at the login
prompt because the boot banner gave the wrong hint, a bug no unit test
would find. Run 4 finished from a brand-new account, about 90 minutes of
reading by the agent's estimate.

**Property-based testing.** The story property test plays the card's own
triggers and manifest in 300 random orders and checks that flags only grow,
every ending stays reachable and nothing softlocks. It runs in CTest on the
server build and from the hook. [fill: did it catch something]

**Verification.** Code reading is not verification. Every queue item had
to pass `pio run` for the stick and a staging check on a Linux server: the
server build, CTest, a smoke test and an end to end Telnet script that
plays the whole game as a new caller (55 steps). Only then was it ticked.

## What I learned

I learned that AI agents are dead stupid idiots, but if you carefully
explain what you want, they can be useful. I also learned what Kiro is. I
had no former experience of it.

## Challenges

One early slice put the brownout login gate on a line that was commented
out, and it looked fine until the end to end test ran on the real server.
That is where the staging rule came from.

Git turned CRLF into LF and all ANSI art drew as staircases on Linux. Fixed
with `.gitattributes`.

The user flag field is 255 characters and story progress would not fit, so
it moved to its own record file.

The first playtester runs got stuck at the login, because the boot banner
gave the wrong hint. Humans would have got stuck too.

[fill: anything else you want to mention, e.g. the ESP32 memory limits]

## What's next

The first step is the mesh. I want to run TinyBBS on a small ESP-based
Meshtastic device, with message boards that sync slowly between devices over
private channels, like FidoNet, and node chat next to them. The core is
already transport independent, so this is a new transport and a sync rule,
not a rewrite. [fill: what you have tried so far, if anything]

I'm planning to port it to the Pimoroni Tufty 2350 Badgeware as a handheld
console. [fill: one line on what the Tufty port needs]

The hosted board runs at halcyon.helppox.com. The stick is the
collector's edition.

---

## Submission checklist (from the rules, not for the Devpost page)

- Deadline 4 Dec 2026, 2:00 pm Pacific. No extensions.
- Category: Resurrection.
- Public GitHub repo. Today the only remote is the staging repo on
  helppox, so a GitHub repo is still needed. `LICENSE` exists; check that
  GitHub shows it in the About box.
- `/.kiro` at the root and not in `.gitignore` (checked: it is not).
- URL of the hosted board, plus test instructions for judges in the
  private field (new callers start with `admin`/`admin` and the two-minute
  power puzzle).
- Video under 3 minutes on YouTube, public, showing it working on the
  device it was built for (browser, Telnet, and the stick if possible). No
  third-party music: use Ty's own tunes, not Mod Archive or HVSC files.
- Text description of features (this file) and the Kiro write-up (the
  "How Kiro was used" section answers the rules' questions one by one).
- Disclose pre-existing work and third-party parts (in "How I built it").
- Optional bonuses: a blog post that mentions Kiroween 2026 (2000 credits
  for the top 100), and a social post starting with #KiroweenHackathon26
  about your favourite thing in Kiro.
