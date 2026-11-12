# LAST CALLER - step by step with Kiro

Companion to `KIROWEEN_PLAN.md`. That one has the rules, risks, judging and the schedule. This one is the order of work, starting with the repo duplicate. Where they disagree on the order of the code slices, this file wins.

Names below are placeholders. Change them in step 0.

## Step 0 - Three names (10 minutes)

- Repo: `last-caller`
- Board name shown to callers: `DEADLINE BBS` (a dead line, and the hackathon has one). Short name `DEADLINE`. SSID `DEADLINE`.
- Hostname: a new subdomain, for example `lastcaller.helppox.com`. Kouvosto stays where it is, untouched.

## Step 1 - Duplicate the repo, clean history

Not a fork. A fork drags 70 engine commits along as the visible history. A fresh repo has one first commit with the engine and then only Kiro commits, so the Kiro work is what the history shows.

On the PC, PowerShell, outside Dropbox:

```
git clone --depth 1 https://github.com/apn201/TinyBBS last-caller
cd last-caller
Remove-Item -Recurse -Force .git
git init -b main
git add -A
git commit -m "TinyBBS engine, built Nov 11 to 12 2026, MIT. The haunted layer follows, built in Kiro."
```

Cloning from GitHub, not from the Dropbox folder, keeps out the untracked `APACHE_PORT_PLAN.md`, `reference/` and the `.pio*` build folders.

Then create the public repo on GitHub and push (`gh repo create apn201/last-caller --public --source . --push`, or the web UI). Check that the About box shows "MIT License". If it does not, the LICENSE text was altered, restore the standard text.

Done when: public repo, one commit, license detected.

## Step 2 - Connect Kiro before touching anything

1. Open the `last-caller` folder in Kiro.
2. Let Kiro generate steering from the existing `README.md` and `AGENTS.md`, then edit it down to four files in `.kiro/steering/`:
   - `product.md`: the game, the tone, fiction only, no real names or brands, the five-minute judge.
   - `tech.md`: two builds must pass after every change (`pio run`, and the CMake server build), no Linux or ESP32 headers in `src/core`, `src/bbs`, `src/doors`. Lifted from AGENTS.md.
   - `structure.md`: the folder map.
   - `card-content.md`: content and settings live on the card, not in firmware. A menu never offers something that does not work.
3. Check `.gitignore` has nothing that matches `.kiro`. Today it does not.
4. Commit: "Kiro: steering". This is the first Kiro commit in the history.
5. Use the `pio` command line from Kiro's terminal. Do not count on the PlatformIO extension, it may not be in Kiro's extension store.

First hook, add it now: on save of `shared/web/*.html`, run `python tools/embed_web.py`. It is small and it proves hooks work, so the later ones are routine.

Done when: steering is committed, the hook fires on a save, and `pio run` plus the server build pass inside Kiro's terminal.

## Step 3 - Strip and rebrand, as the first spec

You are right that identity and content live on the card, so this step is almost all file edits under `sd/BBS/`, `art/` and `tools/`, with only a few strings in code (listed at the end of the Change list, and optional). It is still the right first spec, because it is concrete and its result is checkable. Do it as `.kiro/specs/rebrand/` with EARS requirements, then let Kiro run the tasks. Do not do it by hand, the history has to show Kiro doing it.

What the repo actually contains (checked today). Most of the content is already outside git: `sd/BBS/FILES` and `sd/BBS/MSGBASE` are git-ignored, so the duplicate has no archive files and no seeded FidoNet traffic. What is left to deal with:

Delete:
- `tools/`: `Find-ArchiveBbsContent.ps1`, `Get-ArchiveBbsFile.ps1`, `Get-WaybackBbsText.ps1`, `Import-BbsSeed.ps1`, `Import-KouvostoCollection.ps1`, `Seed-KouvostoMessages.ps1`, `fetch-modules.py`, `fetch-sids.py`, `fidonet-seed.py`, `kouvosto-collection.json`, `seed-areas.json`. They fetch third-party archive content. Keep `embed_web.py`, `bbscard.py`, `make-lcd-logo.py`, `prepare-sd.ps1`, `build-platformio.cmd`.
- `docs/CURATION.md` (the Kouvosto archive policy, includes the Metropoli permission). Replace with a short `docs/CONTENT_POLICY.md`: all content is original or licensed, listed with source.
- `art/KOUVOSTO_SPLASH_SOURCE.png`, `art/KOUVOSTO_PREVIEW.png`, `sd/BBS/ASSETS/KOUVOSTO.RGB565`, `sd/BBS/ASSETS/LOGO.RGB565`.
- Conferences 4 to 9 in `CONFIG/CONFERENCES.INI` (the FidoNet echoes). Rewrite 0 to 3 for the story later.
- `docs/APACHE_PORT.md` references to the live board and the Rocky setup prompt: rewrite as a generic deploy note.

Change:
- `CONFIG/SYSTEM.INI`: `name`, `short_name`, `location`, `sysop`. Keep `epoch=1997-03-14 21:00` because the story needs it.
- `CONFIG/NETWORK.INI`: `ssid=DEADLINE`.
- `CONFIG/BULLETINS.INI` and `BULLETINS/001_RULES.ANS`: placeholders for now, real ones in step 7.
- `ANSI/LOGIN/WELCOME.ANS`, `ASCII/LOGIN/WELCOME.ASC`, `ANSI/MENUS/*.ANS`, `ANSI/GOODBYE/GOODBYE.ANS`: new art. A first cheap version is the old art with the name replaced, real art comes in step 7.
- LCD logo: `tools/make-lcd-logo.py` makes `LOGO.RGB565` from a picture. Splash is in `src/hw/scene_logo.h`, which has a Kouvosto ANSI splash baked in. Regenerate or fall back to the card logo.
- The doors say "Legend of Kouvola". Menu names come from the `[DOORS]` section of `SYSTEM.INI`, so renaming those costs nothing. The strings in `src/doors/doors.h`, `src/doors/dice.h`, `src/bbs/sysop.h` and `src/bbs/user_editor.h` are in code. Either rename them in the spec (four small edits) or drop the doors you do not need. Decision for Kiro to propose: keep VolaMUD (the haunted corner later), drop dice and Legend unless they earn their place.
- `README.md`: rewrite. Describes the board engine (built Nov 11 to 12 2026) and the haunted layer, and credits GoMud as a design reference (already in `THIRD_PARTY_NOTICES.md`, keep that file). The Kiro write-up says what Kiro built: the haunted layer, not the engine.

Keep `sd/BBS/ASSETS/DIALUP.MP3`. It is modem handshake noise, functional signal tones. Add one line to `THIRD_PARTY_NOTICES.md` saying where it came from (recorded or generated by you).

Keep as is: `src/`, `server/`, `shared/web/`, `platformio.ini`, the music player assets (the libraries have their licenses in `ASSETS/PLAYER/LICENSES`). No modules ship with them, so nothing to clear.

Acceptance, written as EARS and checked by a hook:
- WHEN `grep -ri kouvo` runs over the repo, THE result SHALL be nothing, apart from the notices file.
- THE two builds SHALL pass.
- WHEN a caller connects, THE board SHALL show `DEADLINE BBS` and no old name.

Done when: acceptance passes, committed as spec tasks complete.

## Step 4 - Second instance on the server (day 1 or 2)

Do this early. It is the risk that bites late.

- New data root, for example `/srv/lastcaller`, a copy of `sd/BBS`. Never share `DATA/` with the Kouvosto instance.
- Its own port and systemd unit. Its own Apache vhost with TLS for the new hostname, proxying HTTP and WebSocket to localhost. Telnet stays closed.
- The server reads the web pages from `shared/web`, so deploy from the new repo clone.
- Hardening before public: uploads off (`upload_max=0`), SysOp page password-protected at Apache, request rate limit, chat line length limit.
- Add a deploy script in the repo, written by Kiro, so redeploying is one command.

Done when: the new URL opens a terminal, you can sign up, post a message and reload to the same node. The old board is unchanged.

## Step 5 - Find out what the card can already do

You said everything works and only content is missing. Mostly true, but be exact about it, because it decides how much code there is. One Kiro task, no spec: read the code and write `docs/CARD_CAPABILITIES.md`.

It answers: what can an INI or a file on the card change? Which flags and levels gate which conferences, bulletins and file areas? Can the card define VolaMUD NPC talk and tasks? What can a caller do that changes their flags? (My read from the config: flags and levels gate content, but nothing grants them to a caller automatically. Today only the SysOp does that in the user editor. That is the gap.)

Done when: the file exists and says in plain words what the card can drive and what it cannot.

## Step 6 - The story bible, as a spec with no code

`.kiro/specs/story-bible/`. Requirements in EARS, design is the document itself, tasks are the content to write.

Contents: premise, the SysOp, five regulars, the timeline of 14 March 1997, an alibi table, twelve clues with the medium each one sits in, the unlock chain, three endings. The rule from the main plan: the culprit is provable from at least four clues across at least three media, and no single clue is mandatory.

Use the `ghostwriter` agent for drafting, with fiction-only enforced by steering. Your job is the edit: it has to sound like a tired SysOp in 1997, not like a model.

Done when: the alibi table is complete and you can solve it on paper.

## Step 7 - Engine, in four small steps, stop where time stops

Each is its own short spec under `.kiro/specs/`. The cut line is after E2. If you only get E0 and E2, you still have a game.

- E0 - Keyword terminal. One new door in `src/doors/` (a door is one header plus one line in the door table). The caller types a code or phrase found somewhere on the board, and the door sets a flag, raises the level, or sends a message. This is the missing "grant" and it unlocks the Back Room by play, not by the SysOp. About 100 lines. Clues are stored as flags on the user record, so no new storage.
- E1 - Triggers. `sd/BBS/STORY/TRIGGERS.INI`: when a caller reads a given message, views a given file, or opens art at a given baud, set a flag or give credit. Loaded once at boot into PSRAM on the stick.
- E2 - Ghosts. Scripted callers in WHO that page you and speak in chat, from `STORY/GHOSTS.INI`.
- E3 - Notebook and accusation. `N` for clues found, `A` to accuse, three endings.

Properties and the story lint (from the main plan section 5, H5) grow alongside E1 and E3. Write them as the specs are written, not after.

Done when, per step: a scripted account does the thing on both the stick and the server.

## Step 8 - The rest of the Kiro kit, in this order

1. Hooks: on save in `sd/BBS/STORY/` run `story-lint`; on save in `src/` run the server build and the property tests; before commit scan for real names and brands; on spec task done, append to `docs/devlog.md`.
2. Custom agents: `ghostwriter`, `puzzle-smith`, `firmware-guard`, `playtester`.
3. MCP `bbs-caller`: connects to the test instance, sends input, reads the screen, lists WHO, dumps a test account's flags. Point it at the new URL, not Kouvosto.
4. Property tests with RapidCheck in the server CMake build.

Check the current Kiro format for hooks, agents and MCP config before generating them. Formats move, and Kiro itself can show the current shape.

## Step 9 - Content, act by act

Order matters: each act playable start to finish before the next one starts.

- Act 1, Main Board: bulletins, a dozen messages, the user list, the first ghost page. Ends with the hint toward the Back Room code.
- Act 2, Back Room: SysOp logs, private mail, the alibi evidence, the overpainted ANSI. Credits are part of the puzzle.
- Act 3, Vault and the door: the last proof and the accusation.

Sourcing, so nothing in the video is a problem:
- Text: drafted by `ghostwriter`, rewritten by you.
- Pictures: you are a filmmaker. Photograph real things in your own house at night, then reduce to 16 colours and dither. It looks period and it is yours.
- ANSI art: a Kiro-written script converts your pictures. Hand-draw only the title screen.
- Sound: your own recordings, a few seconds each. The voice script from Puhelu can supply the idea of one human voice on the line. Generated tracker modules if you want music.
- No archive modules or SID tunes in anything the video shows.

## Step 10 - Scare design

A text board has no jump scares. What works is wrongness and timing. Five of these are must-haves, the rest are free if the engine allows:

1. The board pages you by your own handle within 30 seconds of login. (E2)
2. WHO shows a caller who has been on since 14 March 1997. Idle time absurd. (E2)
3. The "last call" line on your login shows a 1997 date that is not yours. (a login bulletin, plus E1)
4. A message you already read has an extra line when you read it again. (E1)
5. The ANSI picture that draws slowly and stops, and then something types a line that was not in the file.
6. Goodbye does not return you immediately. Short pause, one more line, then the disconnect.
7. The board remembers you hung up halfway and says so on the next login.
8. Chat lines arrive with pauses, like someone typing and deleting.
9. Sound cue in the browser: one static burst, once, at the right moment. Own audio only.
10. Earlier callers appear in WHO as ghosts for the next judge. (Could, best one if there is time.)

Rule for all of them: the board never explains. Scary is a plain sentence with the wrong date on it.

## Step 11 - Verify like a stranger

- The `playtester` agent finishes the game through the MCP.
- A person who has never seen it plays on a phone, on mobile data, from the public URL, with no help, and gets stuck or does not.
- The stick, flashed with the final card, runs an hour with two callers.
- Lint and properties are green.

## The stick stays an option

The firmware and the card are the same project, so flashing is always available. Keep it alive the whole way:

- After steps 3 and 7, flash the stick with the rebranded firmware and copy the card (`tools/prepare-sd.ps1`). The LCD, the `DEADLINE` WiFi network and the captive portal then work as a physical object.
- The hosted URL is what judges use. The stick is what the video shows. Both run the same core, which is the point of the write-up.
- Flashing is a manual step, not a spec. Test on the stick at the end of each engine step (E0 to E3), because the ESP32 is where it breaks first (memory, SD speed).
- If time runs out, the stick is the thing to drop, not the hosted URL. The URL is required, the stick is the gimmick.

## Step 12 - Ship

Follow sections 8 to 10 of `KIROWEEN_PLAN.md`: video, write-up, blog, social posts, credits request before Dec 1 noon PT, submit Dec 3.

## Order for the first two days

| When | Steps |
| --- | --- |
| Thu Nov 12 evening | 0, 1, 2 |
| Fri Nov 13 | 3, then 4 in parallel, then 5 |
| Sat Nov 14 | 6 |
| Sun Nov 15 | 7 E0 and the first Act 1 content, so something can be played |
| After that | the schedule in the main plan, with E1 to E3 and acts interleaved |
