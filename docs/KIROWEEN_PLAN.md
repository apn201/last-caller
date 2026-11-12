# LAST CALLER - Kiroween 2026 plan

Working title. The entry is TinyBBS wearing a haunted identity: a forgotten 1990s board that went dark the night its SysOp died, and you are the last caller. Murder mystery, escape room, ghost story, all done with BBS parts: messages, files, ANSI, tracker music, doors, WHO, chat, credits and flags.

The phone (Puhelu) is parked. Nothing from it is lost: the Kiro setup ideas (steering, hooks, agents, MCP, PBT) carry over, and so does the director idea in a smaller form.

Written Thu Nov 12 2026. Submit Thu Dec 3. Hard deadline is Dec 4 2:00 PM Pacific, which is Dec 5 00:00 in Kouvola. Do not play with that.

## 1. Decision in short

- Category: Resurrection. A dead BBS comes back on a $10 stick, and a dead SysOp comes back with it.
- Device in the video: the Pocket-Dongle-S3 stick plus a phone on its WiFi. That is the "intended device" the rules ask for.
- Hosted URL for judges: the server build of the same C++ core, behind Apache. Telnet stays closed.
- Kiro does all coding from tonight. Not "mostly". All of it. See section 3, this is the biggest risk.
- New game content lives on the SD card tree. New code is one small engine (the "haunt") that sits in the shared core and runs on both ESP32 and server.

## 2. Rules that shape the plan

From the Devpost rules page, read today.

| Rule | What it means here |
| --- | --- |
| Project must be newly created after Oct 30 | TinyBBS first commit is Nov 11, so the engine is inside the window. Fine, nothing to flag. The older TinyIRC is not in the repo and not part of the entry. |
| Other pre-existing code or work must be disclosed | Nothing in the repo qualifies: the engine was built Nov 11 to 12, in the window. Standard libraries (CivetWeb, miniz, libopenmpt, jsSID) stay in `THIRD_PARTY_NOTICES.md` as they are today. |
| Insufficient use of Kiro can mean disqualification | The base has no `.kiro` and was written with other agents. Everything new must be Kiro, visibly, in git history. |
| Public GitHub repo, open source license detectable in About | MIT is already there. Check the About box shows it. |
| `/.kiro` at the root, not in .gitignore | Add it on day one. Check `.gitignore` for `.kiro` patterns. |
| Hosted URL, or credentials and test instructions if private | Public URL, sign up with any handle. A one-page "how to play in 3 minutes" for the form. |
| Demo video under 3 min, YouTube, public, on the intended device | Stick plus phone on screen. Under 2:50 to be safe. |
| No third-party trademarks or copyrighted music in the video | Biggest content trap, see section 3. |
| Kiro write-up covering specs, steering, hooks, MCP, PBT, custom agents, powers | Section 6 maps each one to something real. |
| Judges may use automated AI analysis, and Social Blitz checks for "original thoughts vs AI writing" | Posts and blog in your own voice. Claude drafts from your seeds only. |

Dates from the rules page: submission closes Dec 4 2:00 PM PT, judging Dec 16 to Jan 29, winners about Feb 3. The 4,000 free credit request must go in before Dec 1 noon PT (that date is from my earlier read of the page, recheck it).

## 3. Risks, stated plainly

1. The Kiro share. The engine was written before Kiro came in, and judges can see the history. Insufficient Kiro use is a disqualification risk at their discretion. Defence: make the Kiro part clearly the substance (ghosts, triggers, case file, story lint, MCP tester, all specs and tests, all the scary content), keep every new commit in Kiro, and in the write-up say plainly what Kiro built (the haunted layer) without implying it wrote the engine. Do not "just finish this bit in the other tool". Not once.
2. Repo choice. Recommended: a fresh repo for the entry, with the TinyBBS engine as one first commit ("TinyBBS engine, built Nov 11 to 12 2026, MIT"), and everything after that in Kiro. It keeps TinyBBS clean as its own project and keeps the Kiro ratio honest. The other option is to add `.kiro` to the existing repo and continue. Both work. The fresh repo is easier to explain in two sentences. Your call, it is reversible for about an hour of work.
3. Music. The board seeds fifty modules and thirty SID tunes from outside archives. Playing those in the video is a copyright problem, and the rules forbid it. Same question for `ASSETS/DIALUP.MP3`: where did it come from? Plan: video uses only audio we made. Kiro writes a small generator for original tracker modules (a few lines of Python writing MOD/XM), which also lets us hide clues in them. The hosted board can serve whatever licence allows, but check each one or drop it.
4. Hosted board is public. Judges share it. Per-user state is fine (clues are stored per account). Needs: uploads off, chat length limit, Apache rate limit, a nightly reset of the test accounts, and the SysOp page behind a password. Allow half a day.
5. Does the server build run today? The git log says phases 1 to 4 are done, but I cannot see whether it is live. First job on day one is to put the unmodified board on a public URL. If that takes more than a day, we have a problem and we want to know it now, not on Dec 1.
6. ESP32 limits. Reading trigger tables from SD on every event will be slow. Load the story tables once into PSRAM at boot. 8 MB is plenty for this.
7. Time. About 21 days, a full-time job, and other hackathons on the board. This is the only one that gets evenings until Dec 3. I assumed 2 to 3 hours on weeknights and 6 to 8 on weekend days, roughly 70 hours. If that is wrong, the cut list in section 9 is what moves.
8. The solution is in a public repo. Anyone can read the story files. Accept it. The game does not need to survive a git clone, and the judges are not the audience for spoilers anyway. Keep the culprit in the spec, not in the marketing.

## 4. The game

### Premise

KOUVOSTO BBS went dark on 14 March 1997 at 21:00. That is already the board's start date in `SYSTEM.INI` (`epoch=1997-03-14 21:00`), so the clock starts at the minute it stopped. You dial in as the last caller. The SysOp is dead, the board is not, and something has been keeping it running. Someone on the user list killed him. You find out who before the ghost decides you are the answer.

Everything is fictional: invented handles, invented town details, no real names, no real brands. This goes in a steering file so Kiro cannot drift.

### Cast (final names get decided in the story-bible spec)

- The SysOp. Present as a ghost node in WHO, pages you, chats in short lines, never explains.
- Five regulars. One did it. Each has a user record, messages, a last-call time, a file or two.
- The last caller. You. Your handle joins the ghost list when you finish (see Could in section 5).

### Three acts, gated with what the board already has

1. Main Board (security 20). Bulletins, messages, WHO, first page from the ghost within a minute. Goal: learn the five regulars, find the hint to the Back Room flag.
2. The Back Room (BACKROOM flag, conference 2 and file area 013). SysOp logs, private mail, alibi evidence. Credits are a puzzle resource: some files cost more than a new account has, so you earn credit by posting, or by finding the right order to read things.
3. The Vault and the door (SYSOP-level area 014, and a haunted corner of VolaMUD). Final proof. Then the accusation.

### Clues: twelve, across at least six media

The point is that this only works as a BBS. Spread the evidence over what the format has:

- Messages: tone, wrong timestamps, a reply to a post that was never written.
- File dates and FILE_ID.DIZ text: someone uploaded a file after they claimed to be offline.
- ZIP contents and comments: listed with `V` without downloading, so a clue that costs nothing but attention.
- ANSI picture at modem speed: the art is drawn, then painted over. Drop the SPEED button to 300 baud, hit space to pause, read the frame that was covered. This uses the existing viewer and costs no new code.
- Pictures (GIF/JPG viewer): a scanned note, a photo with a detail in it.
- Tracker module: sample names or pattern text. Unverified whether the player screen shows sample names. Check in the first content session, drop to a Could if it does not.
- WHO and chat: the ghost node itself, its idle time, what it says when you ask the right thing.
- Door game: one NPC in the haunted corner of VolaMUD who only talks after you have two specific clues.

Solvability rule: the culprit must be provable from any combination of at least four clues from at least three media. No single clue is enough, and no clue is mandatory. The solver in section 5 checks this, not a human with a feeling.

### Endings

- Right accusation: the ghost lets go. Board shows the farewell art, the clock stops, WHO empties.
- Wrong accusation: the ghost keeps you. Your terminal says you are disconnected, and WHO shows you as a caller who never left.
- Hang up halfway: next login the ghost says "You hung up." (Puhelu's callback, one line, free.)

### Built for a five-minute judge

The existing roadmap says testers spend a minute in files and five in doors. Judges will be the same or worse. So:

- Ghost pages within 30 seconds of login, with a nudge toward the first clue.
- `N` is the case notebook: clues found out of twelve, plus one line of hint per missing clue. Hints cost a little credit so it still feels like a game.
- One quick win in the first two minutes, so something visibly unlocks.
- A full game takes about 25 minutes. The video and the write-up have to work without anyone finishing it.

## 5. What gets built

Slices, in order. Each one is a Kiro spec with EARS requirements, then tasks, then a done-when you can check. Both builds (PlatformIO for ESP32, CMake for the server) must pass after every slice. That rule is already in AGENTS.md and goes into steering.

### H0 - Bootstrap (Nov 12 to 13)

- Create the repo, import the engine as one first commit, add `.kiro/` with steering and hooks.
- Put the unmodified board on the public URL (risk 5).
- Done when: repo is public with license shown, `.kiro` is visible at the root, the public URL opens the terminal and you can sign up.

### H1 - Haunt engine (Nov 14 to 17)

A small portable C++ header in the shared core, no ESP32 or Linux includes. It reads a trigger table from the card and reacts to board events.

- Events: login, message read, file viewed, file downloaded, art viewed (with baud setting), door entered, command typed, minutes since login, idle.
- Conditions: has flag, lacks flag, level at least, clue count at least.
- Effects: set flag, give credits, post a message as a named user, page the caller, ghost chat line, raise level, show art.
- Clue state is stored as flags on the user record (`C01` to `C12`). The user file already has a flags field, so there is no new storage format and nothing to migrate.
- Tables live in `sd/BBS/STORY/*.INI`, loaded once at boot into PSRAM on the stick.
- Done when: a test account reads a given message and gets a clue flag and a page, on both the stick and the server, and reloads give the same state.

### H2 - Ghost presence (Nov 18 to 19)

- Ghost callers show in WHO with a node, a handle, an activity line and an idle time that is wrong.
- They can page the caller and speak in chat on triggers, with delays so it reads as typing.
- Scripted from `sd/BBS/STORY/GHOSTS.INI`, so a new board gets new ghosts without touching firmware.
- Done when: the SysOp ghost pages you at login +30 s, answers one chat keyword, and shows in WHO on both transports.

### H3 - Case notebook and accusation (Nov 20 to 22)

- `N` command: clues found, hints, progress. Only appears after the first clue (the roadmap rule: never offer a dead menu).
- `A` accuse, available after enough clues. Endings as in section 4.
- Done when: a scripted playthrough reaches each of the three endings.

### H4 - Content (Nov 17 to 27, runs in parallel with H1 to H3)

All on the card, no firmware. This is where most of the hours go and where most of the points are.

- Story bible spec first: cast, timeline, alibi table, twelve clues, which medium holds each.
- Act 1, then act 2, then act 3, in that order, each one playable end to end before the next starts.
- Original art: ANSI screens, two or three pictures, the overpainted ANSI. Original audio: generated modules and a few short effects.
- Rewrite the seeded content that does not fit (the FidoNet history stays as period wallpaper, the invented names are fine).
- Done when: a fresh account can play act 1 to the Back Room flag in under ten minutes without help.

### H5 - Kiro tooling and verification (Nov 21 to 27)

- `story-lint`: a small C++ tool that uses the core's own INI reader. Checks every clue is reachable, no room is gated above any level a caller can reach, no softlock, and the culprit is unique when all clues are known.
- Properties (RapidCheck in the server CMake build, host-side, fast):
  1. Flags only grow. No effect removes a clue.
  2. Firing the same trigger twice equals firing it once.
  3. Credits never go negative and cannot be farmed without limit.
  4. Every clue is reachable from a fresh account.
  5. From any reachable state, an accusable state is reachable (no softlock).
  6. Exactly one suspect fits all twelve clues, and no set of fewer than four clues is enough.
  7. A malformed trigger file never crashes the board (fuzz the INI).
- MCP server `bbs-caller`: dial the test server over its web or Telnet transport, send input, read the screen, list WHO, dump the test account's flags. An agent can then play the game.
- Done when: the `playtester` agent finishes the game through the MCP and the lint and properties are green in a hook.

### H6 - Polish, hosting, hardware (Nov 25 to 29)

- Public hosting hardening from risk 4. Mobile check on a real phone.
- Flash the final card to the stick, soak test an hour with two callers.
- Browser extras only if time: a static flicker and a short sound cue on ghost events, driven from the existing page.

### H7 - Demo and write-ups (Nov 28 to Dec 2)

See section 8. Video, Devpost text, Kiro write-up, blog post, social posts, credits request.

### Could, in this order, only after H6 is green

1. Earlier callers become ghosts. On the hosted board, a finished account's handle appears in WHO for the next judge. Cheap, since users already persist, and it is the best scare in the game.
2. Original tracker module with a clue in it.
3. A second tiny story pack on a second card tree, to prove "swap the card, get a new game".
4. A Kiro power that bundles the authoring docs and the lint.

## 6. Kiro, mapped to the write-up checklist

The write-up has to name the features. Each row is something that will exist and have history.

| Feature | What we use it for |
| --- | --- |
| Specs (EARS) | Four specs: `story-bible`, `haunt-engine`, `ghost-presence`, `case-file`. Requirements in EARS, design, tasks. Properties come from the requirements. |
| Steering | `product.md` (tone, fiction only, no real names or brands, the five-minute judge), `tech.md` (two builds must pass, platform seam, no Linux or ESP32 headers in core), `structure.md` (folder map), `card-content.md` (the card is the control panel, never offer dead menu options). Mostly lifted from AGENTS.md. |
| Hooks | On save in `sd/BBS/STORY/`: run `story-lint`. On save in `shared/web/`: run `embed_web.py`. On save in `src/`: run the server build and the property tests. Before commit: scan for real names and brands. On spec task done: append a line to `docs/devlog.md`, which feeds the blog post. |
| Custom agents | `ghostwriter` (writes in the story voice, fictional names only), `puzzle-smith` (designs clues, runs lint, refuses a clue that breaks solvability), `firmware-guard` (reviews diffs for platform-seam violations and both builds), `playtester` (plays the game through the MCP and reports where it got stuck). |
| MCP | `bbs-caller`, as above. This is the demo moment: Kiro plays the product. |
| Property-based testing | Seven properties in H5, with the one that matters most being "the mystery has exactly one solution". Most hackathon entries test code, this tests a story. |
| Powers | The Could in section 5. Do not promise it in the write-up unless it exists. |

Be honest in the write-up about what was vibe-coded (content drafting in chat) and what was spec-driven (engine, ghosts, case file). Judges can tell, and the contrast is part of the story.

## 7. Judging criteria, five at 20 percent each

Stage one is a pass or fail on theme fit and required tool use. Theme fit is easy. Tool use is the risk from section 3.

| Criterion | What earns it | Proof in the repo or video |
| --- | --- | --- |
| Potential Impact | The "story on a card" idea: swap the SD card, get a new haunted board, no internet, no accounts, $10. Uses: escape rooms, school and library events, Halloween parties, ARG props, teaching how the pre-web scene worked. Not a world-changer, so do not oversell it. | `docs/AUTHORING.md` with a lint-checked authoring flow, the Could second pack, one paragraph in the write-up |
| Technological Implementation | One C++ core on ESP32-S3 and Linux. WebSocket and Telnet transports. A portable engine with property tests, a solver that proves the mystery has one answer, and an MCP server so an agent can play. | Spec history, test output in CI or a hook log, the lint report in the write-up |
| Design | Authentic ANSI board look, a notebook that keeps a new player oriented, pacing built for five minutes, evidence that uses the format (modem speed, ratios, file dates) instead of generic puzzles. | Video clue montage, screenshots, the case notebook screen |
| Innovation / Idea | A murder mystery solved with the artefacts of a dead medium. Ghosts as callers in WHO. Earlier judges becoming ghosts. | The overpainted ANSI clue, the ghost WHO line, the Could 1 |
| Presentation | Clean video under three minutes, a one-page "play in three minutes", a clear Kiro write-up, a blog post that sounds like you. | Section 8 |

## 8. Demo, write-ups and bonus prizes

### Video, 2:45 target

- 0:00 stick into a USB port, phone joins THE_BOARD, captive portal opens the terminal. No narration needed.
- 0:15 DIAL, the board comes up, the ghost pages within seconds.
- 0:40 clue montage: message, file date, ZIP view, the ANSI drawn then covered at 300 baud, WHO.
- 1:35 Kiro: the four specs, a hook firing, the `playtester` agent playing through the MCP, the lint saying "one solution".
- 2:20 accusation, ending, the WHO list emptying.
- 2:35 hosted URL and the repo, on screen, readable.
- Sound: only our own. No archive modules, no dial-up sample unless its provenance is clean.

### Bonus prizes worth chasing

- Best Demo ($1,000): same video, so it costs nothing extra.
- Bonus Blog (100 awards, 2,000 credits): one post about the Kiro side. The exact place it has to be published is on the Devpost page, check before writing it. Draft from the devlog, then rewrite in your own voice.
- Social Blitz (5 awards): posts with `#KiroweenHackathon26`. They check for original thoughts, so short, dry, yours. Seed it, I proofread.
- Skip: Best Startup, Student, Ghost of Kiroween Past, Kiro Crew App.

## 9. Schedule

Local dates, Kouvola. Weeknights are the thin days.

| Date | Work |
| --- | --- |
| Thu Nov 12 to Fri Nov 13 | H0. Repo, `.kiro`, steering, hooks, public URL for the base. Start the `story-bible` spec. |
| Sat Nov 14 to Tue Nov 17 | H1 engine. Story bible finished: cast, timeline, alibi table, twelve clues. |
| Wed Nov 18 to Thu Nov 19 | H2 ghost presence. Act 1 content. |
| Fri Nov 20 to Sun Nov 22 | H3 notebook and accusation. Act 2 content. |
| Mon Nov 23 to Wed Nov 25 | H5 lint, properties, MCP, `playtester`. Act 3 content. |
| Thu Nov 26 to Sun Nov 29 | H6 hosting, ESP32 flash and soak, mobile check. Full playthrough by an agent and by a human who has not seen it. Content freeze Sun night. |
| Mon Nov 30 | Request the Kiro credits. Record video. Write the Devpost text. |
| Tue Dec 1 | Kiro write-up, blog post, social posts. Fix only what the playtest broke. Code freeze. |
| Wed Dec 2 | Buffer. Re-shoot the weak scene. Check the public URL from a phone on mobile data. |
| Thu Dec 3 | Submit. Check every field and the video link, then stop. |
| Fri Dec 4 | Nothing. The deadline is 2:00 PM Pacific, which is Dec 5 00:00 here. |

## 10. Cut list, in this order

If a week goes badly, cut from the top and keep going. Do not cut anything in the must-have list.

Cut first: Could 1 to 4, browser sound and flicker, the tracker clue, the haunted VolaMUD corner (use a plain NPC in the main board instead), hints costing credit.

Must have: engine, ghost presence, notebook and accusation, three acts with at least eight clues over at least four media, the lint with the unique-solution check, one MCP, the public URL, the stick flashed with the final card, the video, the Kiro write-up.

## 11. Tonight

1. Read `docs/ROADMAP.md` and `AGENTS.md` in Kiro, have it summarise what it thinks the rules are. First Kiro history.
2. Create the repo, import the base in one commit, add `.kiro/steering/` (four files from section 6) and push. Check `.gitignore`.
3. Open the `story-bible` spec in Kiro: requirements in EARS first, no code. Seed it with section 4 and let Kiro argue with it.
4. In parallel, start the public deploy of the unmodified board. Report back whether it works.
5. Answer the three questions in section 3: new repo or same, where DIALUP.MP3 came from, and whether the server build is live.

Sources: Kiroween 2026 rules at https://kiroween.devpost.com/rules, TinyBBS repo at https://github.com/apn201/TinyBBS (read today: README, AGENTS.md, docs/ROADMAP.md, docs/APACHE_PORT.md, THIRD_PARTY_NOTICES.md, docs/CURATION.md, SD config files).
