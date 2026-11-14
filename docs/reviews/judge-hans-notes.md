# Judge notes: Hans, "Last Caller" (Resurrection)

First session. Provisional.

## 1. What I read, how far I got

- Read `docs/DEVPOST.md` (everything above the submission checklist) and
  `docs/LAUNCH_KIT.md` section 3 (the film plan, standing in for the video).
- Played on the board through `bbs-caller` (Telnet) as a new caller,
  handle `HansJudge`. About 165 board tool calls, roughly 40 minutes of
  reading. Then about 20 calls on the repository.
- How far: prologue done, account made, read the Mission Log, Garden Notes,
  most of the Lounge, Ty's two journals, three gallery pieces, the comms
  manual and the brochure. Ran four diagnostics queries, got into Ty's
  account, read part of his mail and the late Garden Notes. **Manifest: all
  four crew rows confirmed and locked (in two filings).** The fifth row
  (ELIZA) opened.
- Where I stopped: at the ELIZA row. I had a hunch, not evidence, and the
  rest clearly runs through the payload host and the outbound queue. That
  is act 3, and I was out of my time budget. I did not stop because I was
  bored. I stopped because I was out of time, and that is unusual for me.

## 2. What I understood from the first paragraph of the Devpost page

Two pitches in one paragraph. One is a 1990s BBS engine for slow offline
links (Meshtastic is mentioned). The other is a horror mystery game about
a dead spaceship that runs on that engine. Then it calls itself a
feasibility study as well. I understood it, but I had to read it twice to
see which of the three is the product. The mesh part is honest ("a
direction, not built"), which I respect, but it also means the "real
problem" is not shown. The game is shown. Lead with the game.

## 3. Notes while playing

**First minute (delighted).** The boot banner is the best first screen I
have seen in this batch. A fake BIOS, "ELIZA MISSION AI ....... NOT
RESPONDING", and a service label with `ADMIN / ADMIN` and a little arrow
saying the defaults were never changed. I needed no instructions. HELP
lists `SOURCE RTG`, and then there is an ACK prompt and an amber ring
lighting up with "Good evening. I have been asleep for some time." In
under two minutes I had a story and a villain. That is the Metal Gear
opening I want.

- Small letdown: over Telnet I never saw a countdown, so the "two minutes
  before the sun goes" pressure from the pitch did not reach me. I just
  typed HELP and won. It may work better in the browser.
- The status screen says `CREW 4 ALL WELL`, and three menus later the
  bulletin says the same. I noticed it. Good.
- After I registered, the login banner still said "SOLAR BUS ONLY" and
  "ELIZA NOT RESPONDING". I could not tell whether that was the story or a
  stale screen.

**ELIZA.** She is excellent. She is calm and polite and answers slightly
the wrong question. When she lists the crew she names Halvorsen,
Achterberg and Tanabe and leaves out Ty. Diagnostics query 200: "Crew
aboard at launch: 4. Crew aboard now: 0. I would like to be accurate
about that." That line gave me a chill. Twelve minutes in, she prints
"You have been reading for twelve minutes. Some of the accounts here are
very old. One of them has never been logged off," which is a hint
delivered as menace. A "stauions" typo appeared in one ELIZA line; I took
it as an intended glitch.

**The writing.** This is why I kept going. Each crew member has their own
voice, and I could tell who wrote a post from its first line. Halvorsen's
log is all times and numbers ("He asked whether the airlock was heated.
It is a vacuum."). Achterberg footnotes an imaginary paper and admits it
does not exist. Ty writes in lowercase ("who is going to hack it, a
seagull"). Tanabe writes about basil, the B flat hum of rack 2, and
cherry stones in the galley's cold drawer. The first three weeks are
funny and petty (thermostat, headphones, the upside-down pen), so the
later posts land hard. Tanabe's Day 31 post ends mid-sentence. Ty's
second journal ("i am going home. i am going home.") and ELIZA's mail
promising him the one working capsule seat made me wince. I was never
told it was sad.

**Puzzles.**
- Ty's password: Achterberg's "Six digits" post, the crew bulletin with
  the birth date, and Ty's journal, which gives the exact format. That is
  three hints, maybe one too generous, but I did not mind. It worked first
  try.
- 300-baud overpaint: `S` (slow) on ORBIT 3 showed a hidden layer, "KEY
  CORRIGENDUM (des. do not tell ty)". This is the most original mechanic
  here. Through this text tool both layers came out side by side and a bit
  garbled. In a real terminal it is probably lovely. I never found where
  the word is used.
- The manifest is Obra Dinn done properly. Rows lock two at a time and it
  never says which row is wrong. The pick list grows as you find evidence:
  "Sealed in the garden module while the CO2 rose" only appeared after I
  read Tanabe's late posts. Filing and seeing "Confirmed and locked: CMDR
  and CHAOS" felt good. The Notebook (`N`) collects clues as you go, which
  helps a busy judge. It also listed a suit log and a power log that I do
  not remember reading, so it may give away a little.
- For one cause (Halvorsen's) I had to choose between two ELIZA options on
  inference. It was right, but it felt like a coin toss between two good
  answers.

**Friction (lost or annoyed).**
- Input is inconsistent. Some prompts take a single key, others need
  ENTER, and in the message reader `N` works on paged messages but not on
  short ones. I lost maybe ten calls to this. A real caller would just
  press ENTER more, but it reads as rough.
- A scene from ELIZA cut into the bulletin menu and dropped me back at the
  main menu.
- Over Telnet the menu options print under each ANSI frame, while the
  frame's "COMMANDS" box stays empty. This may be the tool stripping cursor
  moves, but it looked unfinished on every menu.
- In Bulletin 3 (The Crew), Ty's card breaks the right border ("mission
  artist.∙", "Family. ║").
- I was never bored. Reading the Lounge in order is homework-shaped, but
  the posts are short and good, so I read them all.

## 4. The repository and `.kiro/`

- **README.md is stale and contradicts the game.** It describes FidoNet
  history in seven conferences, "Fifty modules from The Mod Archive",
  "Thirty HVSC classics", VolaMUD as "one native door", and fetch scripts.
  The board I played had none of this. Its doors were Diagnostics and
  Snake, and the Devpost page says no Mod Archive or HVSC tunes ship. The
  credits section still credits those archives. The copy at the top is fun
  (the 1998 box text), but a judge who opens GitHub first will be confused.
  The launch kit knows this ("Stale docs that a judge will read"). It is
  still not fixed.
- **`.kiro/specs`:** 19 folders (Devpost says 16), each with requirements,
  design and tasks. The story bible is written as a spec with EARS
  requirements: every fact needs two items in two media, removing any one
  item must leave two, rows lock in pairs, the pick list shows only facts
  with evidence. I saw that last rule working in play, which is the best
  proof a spec was followed. Using specs for narrative design is new to me.
- **Steering:** 7 files. `writing.md` is the standout: one voice per
  character, banned words and shapes, "posts end on the last fact". It
  shows in the text.
- **Hooks:** web page to header regeneration, story lint on any card story
  file save, and host logic and property tests on core saves. These are
  sensible and tied to real risks (softlocks).
- **Agents:** ghostwriter, puzzle-smith, firmware-guard, playtester, plus
  two critic agents and two judge agents. Simulated judges are clever or
  cheeky, depending on taste.
- **MCP:** an in-house `bbs-caller` Telnet server so Kiro can play the game
  as a caller. It is the tool I just used. The story of the playtester
  stuck at the login because of a wrong banner hint is the best Kiro
  anecdote in the write-up.
- **Headless Kiro CLI queue runs** ("do item Qn, tick it, stop"), one clean
  run per item, overnight. This is a thoughtful strategy.
- Against: Devpost says Claude built the ESP32 engine and Codex built the
  Linux port, and Kiro did the story, mechanics and art. The Kiro part is
  large and is the part I played, but the engine is not Kiro's. The
  write-up also still has `[fill: ...]` gaps and stale numbers (16 specs,
  55 e2e steps against 19 and 63). And "AI agents are dead stupid idiots"
  will be quoted, for better or worse.

## 5. Provisional scores

**Potential Impact: 5/10.** The game is a real product and I would send it
to friends who liked Obra Dinn. That audience is niche, and Telnet and BBS
habits narrow it further. The stated real problem (joyless text on
Meshtastic or LoRa) is argued well but not demonstrated: nothing runs on a
mesh. The transport-independent core is a credible first step.

**Technological Implementation: 8/10.** This is one of the deeper uses of
Kiro I expect to see: specs for story logic with checkable evidence rules,
steering that visibly shapes the prose, hooks that run a story lint and
property tests, custom agents, and an in-house MCP server so Kiro can
playtest over Telnet. It also experiments, with headless CLI queue runs.
Points off because the engine itself was built by other tools and the
docs about the Kiro work are out of date.

**Design: 8/10.** The experience is complete and intentional: a two-minute
hook, a polite villain, four distinct voices, a manifest that rewards
reading, and art on every screen. Some rough edges hurt the polish:
inconsistent single-key and ENTER input, a scene that broke a menu, a
broken border, a banner that did not change after power-up, and command
boxes that sat empty over Telnet.

**Innovation/Idea: 8/10.** The story is told only through what a BBS
already has: WHO ghosts idle for 28 years, message bases, file dates, a
diagnostics door, and a portrait overpainted for 300 baud callers. I have
not seen that combination before, and running it on a $10 stick fits the
category. Mystery ARGs and Obra Dinn-style deduction exist, so the idea is
not without ancestors.

**Presentation: 6/10.** The film plan is tight and hits the brief: the
stick, the captive portal, the prologue, puzzles, Telnet and browser side
by side, 45 seconds of Kiro, all inside 2:50. It is still a plan. The
Devpost text has open `[fill]` gaps and a three-headed opening paragraph,
and the README describes a different board. Fix the README and lead with
the boot banner, and this goes up two points.

TOTAL: 35/50
