# Last Caller

> **HALCYON.** A real-time multi-user science fiction horror experience.
> A puzzle game like no other, powered by state-of-the-art bulletin board
> technology.
>
> Dial in. Wake the ship. Find out what happened to the crew of the first
> private spaceflight.
>
> - Up to 32 simultaneous callers (eight on the stick)
> - ANSI graphics
> - Digital sound
> - Over a dozen interactive screens, one very polite computer
> - Requires a modem and nerves of steel
>
> *Works on any VT100 compatible terminal or Netscape Navigator 4. Best
> viewed at 800x600. Some features may not be available in all regions of
> cyberspace.*

Below the copy, the facts.

Last Caller is a horror puzzle game you play by calling the BBS of a dead
spaceship. The game engine is a real, working 1990s style bulletin board
system, TinyBBS. It runs on a $10 ESP32-S3 USB stick, the
[Pocket-Dongle-S3](https://www.aliexpress.com/item/1005008896509314.html), and
the same code runs on a Linux server. Play it at
[halcyon.helppox.com](https://halcyon.helppox.com) in a browser, or with
Telnet on the stick.

This is an entry to Kiroween 2026, category Resurrection. I resurrected BBS
technology to see if it still works as a game engine. The first puzzle of the
game is a resurrection too: the ship is dead and you have to bring its power
back before you can do anything else.

## The game

HALCYON was a secret private spaceflight in 1998, crew of four. The ship had a
BBS on a 486 so the crew could write home, and the mission AI, ELIZA, ran it as
SysOp. Mission control lost contact and nobody ever announced it. In November
2026 the shorted battery bank has gone open circuit, the solar panels feed the
bus again, and radio amateurs pick up the signal.

The ship tumbles, so the BBS has power for a minute or two at a time. You log
in as the maintenance account, `admin` / `admin`, and have two minutes to
switch the ship to the RTG before the sun goes away and the carrier drops. The
login screen says it is a puzzle game and that the first job is the power and
your own account. Then ELIZA wakes up, very politely.

After that it is a normal 1998 BBS: message conferences, file areas, doors,
chat, WHO, bulletins, ANSI art on every screen. The crew's posts, mail, logs,
Ty's ANSI art and his tracker tunes are the evidence. You crack crew
passwords, put together a jigsaw of ANSI pieces, read a portrait that is
overpainted at 300 baud, ask ELIZA questions in the diagnostics door, poke
around the NOWHERE shell and fill in the crew manifest: who each person was,
how they died and who caused it. Facts confirm in pairs, like in Return of the
Obra Dinn. The ending is sending ELIZA's held outbound queue to the world.

Could had been any story, but a dead board with a dead crew felt right for a
resurrected technology. The story is told only with things a BBS has: message
bases, file areas, door games, node chat and logins.

It is a solo game on a social board. Story areas and crew accounts are read
only. The Lounge, Spoilers, No Spoilers and Radio Room work like on any BBS, so
players can trade hints or stay away from them. A first time player should
finish in about 60 to 120 minutes. The puzzles are meant to be hard. The
interface is not: the same key does the same thing everywhere (Q back, X main
menu, ? help), letters act at once, and the notebook (K) collects every hint
and password you have seen, and replays the scenes.

Easter eggs for people who remember 1998: modem commands (`ATI`, `+++`,
`ATH0`, `ATDT`), a few unlisted words at the main menu, a snake game by Ty in
the doors menu. None of them is needed to finish the game.

## Why a BBS

BBS was simple and robust technology, made for slow phone lines, low
processing power and a PC in someone's bedroom. It works in text, still it
could look good. ANSI art did a lot with 16 colours and 80 columns, and a
screen of it is a few kilobytes.

Small microcontrollers and slow radio links (LoRa, Meshtastic, packet radio)
have the same limits today. Text over them is mostly a public chat line. A BBS
had message boards that synced slowly between machines, like FidoNet, and chat
on the node. TinyBBS could run on a Meshtastic device like that some day. That
is a direction and was out of scope here. Nothing in this repo runs on
Meshtastic. What exists is a core that does not care how bytes reach it:
Telnet and the browser are two transports into one BBS, and a mesh link would
be a third.

The engine has its own repository,
[github.com/apn201/TinyBBS](https://github.com/apn201/TinyBBS), and a plain
board with about 1000 archived FidoNet messages runs on it at
[kouvosto.helppox.com](https://kouvosto.helppox.com). That one is the proof it
is a real BBS and not a game pretending to be one. The interface work done for
this game (keys, pager, notebook engine) was ported back to it.

## How it was built

Everything was built in the hackathon window. Claude built the ESP32 version
of TinyBBS. Codex built the Linux port, and an OpenClaw agent set up the
server side. Kiro built the game on top: the story, the game mechanics, the
art and the tests, with Claude writing the briefs and reviewing. This repo
starts from a copy of TinyBBS (the first commit, 12 Nov 2026), and everything
after that commit is Kiro's.

Kiro ran headless, `kiro-cli chat --no-interactive`, one queue item per run
from [`docs/briefs/QUEUE.md`](docs/briefs/QUEUE.md). Every run started clean
from the files in the repo instead of a long chat history, so it could run
overnight without me. What is in [`.kiro`](.kiro):

- 19 specs (rebrand, ANSI art, story bible, power prologue, triggers, scenes,
  ghosts, acts, shell and doors, manifest, music, eggs, verification and so
  on). The story bible is a spec too: every fact about the crew must be
  provable from several items in different media, so the game survives a
  missed clue.
- 8 steering files: tech rules (no ESP32 or Linux headers in shared code),
  structure, product, card content, ANSI art, story voice, writing rules and
  the interaction contract.
- Hooks: saving a web page regenerates the firmware headers, saving a story
  file runs the story lint, saving shared game logic runs the host tests.
- Agents: ghostwriter, puzzle-smith, firmware-guard and playtester, then two
  adventure game critics who score the story and puzzles, two usability
  testers who never used a BBS, and two judges.
- An MCP server, `bbs-caller` ([`tools/mcp`](tools/mcp)), that dials the board
  over Telnet and returns the screen as text. With it the agents play the
  game on the staging server like a real caller.

One reason I made a game and not a utility was to test Kiro. A utility is
mostly tested by checking that its functions work. A game has to work on
every level at once, story, gameplay, puzzles and code, and it forced testing
on all of them.

No queue item was ticked on code reading. Each had to pass `pio run` for the
stick, the host tests, the story lint and a staging check on a Linux server:
the server build, CTest, a smoke test and an end to end Telnet script that
plays the whole game as a new caller (63 steps). How to run them is in
[`docs/TESTING.md`](docs/TESTING.md).

## Technical details

### Hardware

| Part | Detail |
| --- | --- |
| Board | [Pocket-Dongle-S3](https://www.aliexpress.com/item/1005008896509314.html), USB stick form factor, get the variant with the SD slot |
| MCU | ESP32-S3 N16R8, 16 MB flash, 8 MB PSRAM, PlatformIO profile `esp32-s3-devkitc1-n16r8` |
| Display | On-board 0.96 inch ST7735, 80x160 native, used as 160x80 landscape |
| Storage | On-board microSD slot, SD_MMC 1-bit mode, FAT32 |
| Input | BOOT button only |
| Network | 2.4 GHz soft AP, channel 1 by default |
| Power | USB, from a computer or a phone charger |

Nothing attached, no modem, no keyboard. The stick makes its own WiFi network,
`HALCYON`, with a captive portal, so a phone opens the terminal by itself. It
never joins another network. Pins and display offsets are in
[`docs/HARDWARE.md`](docs/HARDWARE.md).

### Firmware layout

Arduino on ESP32, about 11000 lines of C++ plus the two web pages.
`src/main.cpp` has `setup()` and `loop()` and includes the modules in order, as
one translation unit.

| Folder | Contents |
| --- | --- |
| `src/core` | Globals, helpers, board clock, SD storage and INI reader, users, message store, sessions, paced output, the reader, and the game logic: story record, triggers, scenes, ghosts, glitches, notebook |
| `src/bbs` | Settings, menus, file areas, ZIP reading, viewers, uploads, messages, SysOp tools, login, one `cmd_*.h` per command area |
| `src/doors` | Ship systems, diagnostics, NOWHERE shell, crew manifest, snake, scenes, the door table |
| `src/net` | HTTP and WebSocket routes, Telnet, browser pages (generated) |
| `src/platform` | `platform.h`, the contract between the core and the machine; `esp32.h` |
| `src/hw` | Status LCD and the splash logo |
| `server/` | The Linux build of the same core, with civetweb for HTTP and WebSocket |

The core reaches the machine only through `src/platform/platform.h`, so the
same code builds for the stick and for Linux. The browser pages are plain
files in `shared/web/` and a pre-build step embeds them in the firmware.

### The game is data

The story is on the SD card, not in the firmware. `sd/BBS/STORY` holds a per
caller story record, the trigger table (`TRIGGERS.INI`, flags only grow), the
scenes, ghosts in WHO, glitches, ELIZA's diagnostics, the shell, the
manifest, the crew accounts and the notebook (`NOTES.INI`, 124 notes). Change
the story without reflashing. `python tools/story_lint.py` checks it for
unreachable facts, softlocks and lost endings, and a property test plays the
triggers and manifest in random orders.

### One BBS, two transports

The core does not know how a caller connected. A fixed pool of session structs
(eight on the stick), each a state machine with an input line and a capped
output buffer. A transport only pushes bytes in and drains bytes out.

- **Telnet** on port 23. A small IAC state machine refuses every option except
  ECHO and SUPPRESS-GO-AHEAD. CR, LF and CRLF all end a line, so PuTTY and raw
  clients behave the same.
- **Browser**: one page, 80x25 on a canvas, 8x16 cells, the 16 colour DOS
  palette and an ANSI parser in JavaScript. The carrier is a WebSocket with
  one letter frames, with HTTP polling as a fallback. A page reload rejoins
  the same node. Tracker modules play with libopenmpt and sound effects play
  in the browser.

A Telnet caller and a browser caller share the pool, so they see each other in
WHO and can chat.

ANSI files stream from the card at the caller's chosen modem speed, and a
chunk never ends inside an escape sequence. That is also how the 300 baud
puzzle works: a line is drawn, then a bare CR, then another line over it. At
300 baud you can read the first one. Archive ANSI is CP437 and the server
converts it to UTF-8, so box drawing survives modern Telnet clients.

### Storage

Everything is plain text on a FAT32 card under `/BBS`. Users and messages are
pipe-delimited lines. Record rewrites go to a `.NEW` file first and are
swapped in with a `.BAK`. Passwords are salted SHA-256, the salt from the
hardware RNG. There is no real-time clock and no NTP, so the board invents
one: for HALCYON it is frozen at 8 Dec 1998 04:17, the minute the power was
cut, and runs only while the board has power.

## Getting started

### Hosted

Open [halcyon.helppox.com](https://halcyon.helppox.com) in a browser and press
DIAL. A new caller starts with `admin` / `admin`. The hosted Telnet port
listens on the server only (reach it with an SSH tunnel, see the deploy doc).
Installing your own server copy is in
[`docs/DEPLOY.md`](docs/DEPLOY.md) (`deploy/install.sh`).

### On the stick

1. Install PlatformIO with the Espressif32 platform.
2. Build and flash: `pio run -t upload`. On Windows,
   `tools\build-platformio.cmd` does the same. See **Flash and recovery**
   below if the upload stalls.
3. Format a microSD card as FAT32 and copy `sd/BBS` to it, or run
   `.\tools\prepare-sd.ps1 -CardRoot X:\`. The script also creates the
   folders the board needs. It does not format the card.
4. Insert the card and power the stick from USB.
5. Join `HALCYON`. Press DIAL in the browser, or Telnet to `192.168.4.1:23`.

Settings live in `sd/BBS/CONFIG`, each file explained in its comments:
`SYSTEM.INI` (board identity, nodes, modem speed, clock), `CONFERENCES.INI`,
`BULLETINS.INI`, `NETWORK.INI` (SSID, channel, Telnet port) and `MENUS.INI`.
Menu options are generated: only what exists and what the caller may use is
listed.

## Flash and recovery

The verified board is the Pocket-Dongle-S3 with an N16R8 module. Use the
profile `esp32-s3-devkitc1-n16r8`. The runtime image uses QIO/OPI, but the
serial write must use **DIO**. Forcing QIO in a direct `esptool` command can
make the ROM reset before the bootloader loads (repeating USB chimes).

If a PlatformIO upload stalls, build without uploading and write the image
directly. Replace `COM10` with the real port. It rewrites only firmware
regions and does not touch the SD card.

```powershell
pio run
$env:PYTHONIOENCODING = 'utf-8'
& "$env:USERPROFILE\.platformio\penv\Scripts\esptool.exe" --chip esp32s3 --port COM10 --baud 921600 `
  write-flash --flash-mode dio --flash-freq 80m --flash-size 16MB `
  0x0 .pio-tinybbs\tinybbs_s3\bootloader.bin `
  0x8000 .pio-tinybbs\tinybbs_s3\partitions.bin `
  0xe000 "$env:USERPROFILE\.platformio\packages\framework-arduinoespressif32\tools\partitions\boot_app0.bin" `
  0x10000 .pio-tinybbs\tinybbs_s3\firmware.bin
```

Expect `Hash of data verified` for every segment, then check that `HALCYON`
shows up as a 2.4 GHz network. The ROM loader stays reachable over USB after a
bad flash, so don't erase the chip as a first response.

## SD card layout

| Path | Purpose |
| --- | --- |
| `/BBS/ANSI`, `/BBS/ASCII`, `/BBS/BULLETINS`, `/BBS/ASSETS` | Screens, art, music player, sounds |
| `/BBS/CONFIG` | Board, conference, bulletin, menu and network config |
| `/BBS/STORY` | The game: triggers, scenes, ghosts, shell, manifest, notebook |
| `/BBS/FILES` | File areas, one folder each |
| `/BBS/MSGBASE` | The crew's posts and mail, a folder per conference |
| `/BBS/DATA`, `/BBS/LOGS`, `/BBS/TEMP` | Runtime: users, messages, story records, logs. Do not edit by hand |

Callers never see SD paths. Content rules are in
[`docs/CONTENT_POLICY.md`](docs/CONTENT_POLICY.md).

## SysOp

A level-255 account with the `SYSOP` flag gets `Z`, the SysOp console: user
editor, node monitor, page answering, upload queue, file manager (browser
only), the event log and `RELOAD`. The story never gives a caller the real
SysOp flag.

## Limits

- Telnet has no file transfers (no ZMODEM) and there is no QWK mail.
- Eight nodes on the stick. It has not been tested with eight real callers.
- The stick is AP only and never joins another network.
- Tested on the server and in agent playtests far more than on the stick.

## Repository map

| Path | Contents |
| --- | --- |
| `src/` | Firmware and the shared BBS core |
| `server/` | Linux build of the same core |
| `shared/web/` | Browser pages, embedded into the firmware |
| `sd/BBS/` | SD card content: the game, art, config, music player |
| `tools/` | SD preparation, story lint, tests, ANSI art and module scripts, the MCP server |
| `deploy/` | Install and reset scripts for the hosted board |
| `docs/` | Hardware, testing, deploy, puzzle chain, interaction rules, briefs, reviews, Devpost text |
| `.kiro/` | Specs, steering, hooks and agents |

## Credits

Story, art, Ty's music, crew posts and code are original to this project. The
dial-up sound is recorded from a real modem.

Libraries: [libopenmpt](https://lib.openmpt.org/) and
[jsSID](https://github.com/jhohertz/jsSID) in the browser, civetweb and miniz
on the server, Arduino GFX Library, ESPAsyncWebServer, AsyncTCP and the ESP32
Arduino core on the stick. TinyBBS used my earlier TinyIRC sketch as a
reference for this board's hardware setup. Licences are in
[`THIRD_PARTY_NOTICES.md`](THIRD_PARTY_NOTICES.md).

## Licence

MIT, see [`LICENSE`](LICENSE). That covers the code, the ANSI screens and the
art. The player libraries on the card keep their own licences (libopenmpt BSD,
jsSID GPL v2).
