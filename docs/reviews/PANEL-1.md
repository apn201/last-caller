# Panel 1 (Q19): raising the judges' score without losing the game

Ada and Mara (the Q18 critics, in separate panel agents with the same
personas) read `JUDGES-1.md` (agreed 34/50: Impact 5, Technological
Implementation 8, Design 8, Innovation/Idea 8, Presentation 5), their own
round-3 reviews, `docs/DEVPOST.md`, the film plan and the README. They did
not play again. Six turns, three each, each turn a fresh session. The panel
only proposes; Juha decides what to queue. No game file was changed in Q19.

Method note: the panel agent files (`.kiro/agents/critic-ada-panel.md`,
`critic-mara-panel.md`) were not in the agent registry of the run that wrote
them, so the turns ran through the general agent, told to read and obey those
files. Judges ran as the registered `judge-hans` and `judge-anna`.

## What the panel refuses to change

Both critics, together: ELIZA never winks, warms up, explains herself or gets
funnier. The manifest never names the wrong row. No hint ladder, no
handed-over passwords, no merged crew accounts. The story is not shortened.
The grief after day 22 gets no relief. Nothing on the page or in the film
explains a scare. Reason: each of these is the puzzle or the dread, and a
judge's 40 minutes only see the first third of it anyway. The cure for a
short visit is a first act that finishes something, not a shorter story.

## Where they still differ

- Mesh paragraph on Impact: Ada +0.5, Mara +0.25 (better framing is not new
  evidence).
- The [L] switch-account key: after submission; Mara thinks it is a real fix
  for a full player (+0.25), Ada gives it nothing for judges.
- One light crew scene near day 20: both say after submission.
- Typed ELIZA chat: both settled on one scripted reply to "are they alive"
  plus one fixed fallback line, not a keyword engine.

## One conflict with Juha's framing

The panel proposes dropping "feasibility study" and "product" from the first
paragraph and leading with the game, because both judges could not tell which
they were judging. The brief's framing (the game as a real product and the
project as a feasibility study) is Juha's. Whether to keep both words is his
call. The [check], [fill] marks in the appendix are open for him too, and
the appendix mentions the invented judges by name, which should be cut
before it goes on the real page.

## Ranked action list

Points are of 10 on the named criterion and overlap: they do not add up.
Ceiling from all small items: Presentation 7, Design 8 to 9, total 36 to 37.
Full per-item tables of each critic are below the list.

### Top of the list (all sections, in order)

| # | Section | Action | Criterion | Expected points | Cost | Risk to the soul |
| --- | --- | --- | --- | --- | --- | --- |
| 1 | Repository | Rewrite the README to the board that ships: delete the Mod Archive, HVSC, FidoNet-history and VolaMUD claims and credits, one plain BBS sentence first, 1998 box copy under it | Presentation | +1 | small | none |
| 2 | Devpost text | Rewrite "In short": what a BBS is, the dead ship, the stick, one line on the mesh. No "three products" | Presentation | +1 | small | none |
| 3 | Devpost text | Counts match the folder (19 specs, 8 agents, 2 hook files, 63 steps), fill or cut every [fill], name who plays and who pays | Technological, Presentation, Impact | +0.5 each | small | none |
| 4 | Video plan | Make the film: boot banner first 20 seconds, stick as the reveal, grief beat (Day 31 post, "Crew aboard now: 0") with no voiceover, Kiro 35 seconds with the bbs-caller shot, mesh one line | Presentation | +1 to +1.5 | large | none |
| 5 | Game | Small honesty fixes: Diagnostics "this account" line, filed-report feedback, boot banner after power-up, scripted "are they alive" reply plus one fallback line, row status line | Design | +0.25 to +0.5 each, about +1 together | small | none |

### Game

| Action | What to change | Criterion | Points | Cost | Risk to the soul |
| --- | --- | --- | --- | --- | --- |
| Fix the Diagnostics lie (Mara ranks first) | "Available to this account" says so only on the SysOp account; elsewhere "the SysOp's account" | Design | +0.25 | small | none |
| Filed report feedback | Queue header says the report is on file and ELIZA says so; replace "release: command not found" with a held message naming the gate | Design | +0.25 | small | none |
| Scripted answer plus fallback line | One reply to "are they alive", one fixed line for text she cannot match, so the repeat reads as chosen | Design | +0.5 together | small | none |
| Row status line | After the first two rows lock, ELIZA says flatly how many rows remain. No answers | Design | +0.5 | small | none |
| Key and ENTER rules | One rule on all prompts; Q exits ELIZA chat and SysOp chat | Design | +0.5 | medium | none |
| Scenes that eat input | A scene must not start where the next key is lost or cut at the screen edge | Design | +0.25 | medium | none |
| Boot banner after power-up | Show the new state (bus on RTG, ELIZA responding) or a decay line that reads as chosen | Design | +0.25 | small | none |
| Hide News Wire bulletin until the downlink scene | It gives away the last scene | Design | +0.25 (full players) | small | none |
| In-world hint about the door | ELIZA: "You are in Diagnostics. Type QUIT to leave." | Design | +0.25 | small | none |
| Border and pronoun | Ty's card border in bulletin 3, "his" in a manifest option | Design | +0.1 | small | none |
| Second clue for the slow-modem password | One more pointer to S (replay at modem speed), word not named | Design | +0.25 (full players) | small | some |
| Answer bundling in the manifest | Stop options bundling cause and fate | Design | +0.25 (full players) | medium | none |
| Switch-account key [L] | Skip Goodbye and BIOS between crew hops; no merged accounts | Design | 0 for judges, +0.25 full players | small | none |
| Keyword chat engine | Not this week | Design | +0.25 over the scripted rows | medium | some |
| One light crew scene near day 20 | After submission; must not tidy anything | Design | 0 for judges | small | some |

### Onboarding

| Action | What to change | Criterion | Points | Cost | Risk to the soul |
| --- | --- | --- | --- | --- | --- |
| One plain sentence on what a BBS is | First line of README and Devpost: "A BBS is a computer you phone, with message boards, files and games, run by one person." | Presentation | +1 (shared with the In short rewrite) | small | none |
| Judge test card | In the private test field and README: admin/admin, SOURCE RTG, press K; Q backs out one level; the manifest never says which row is wrong, on purpose | Design | +0.25 | small | none |
| Say where the first 40 minutes end | First act in about 40 minutes with the first rows locked; full story 60 to 120 | Presentation | +0.25 | small | none |
| Browser first | Judge instructions point at the browser terminal (menu boxes draw, clock on the power puzzle), Telnet second | Design | +0.25 | small | none |

### Devpost text

| Action | What to change | Criterion | Points | Cost | Risk to the soul |
| --- | --- | --- | --- | --- | --- |
| Rewrite "In short" | See the appendix | Presentation | +1 | small | none |
| Mesh as one honest paragraph | Game is the demonstration; "a direction, not built" | Impact | +0.25 to +0.5 | small | none |
| Fill or cut every [fill] | Answer or delete | Presentation | +0.5 | small | none |
| Counts match the folder | Say what the 8 agents are: four build, four review | Technological | +0.5 | small | none |
| Name who plays and who pays | Audience line; hosted board is the product, stick the collector's edition | Impact | +0.5 | small | none |
| Kiro proof beats opinion | The pick-list rule seen in play; the banner-bug story; true answer for the property test | Technological | +0.25 | small | none |
| "Dead stupid idiots" line | Keep it, follow it at once with the banner story | Presentation | +0.25 | small | none |
| Stale tense and credits | Remove "12 hours" and the credits figure on the day | Presentation | +0.1 | small | none |

### Video plan

| Action | What to change | Criterion | Points | Cost | Risk to the soul |
| --- | --- | --- | --- | --- | --- |
| The film must exist | A finished 3-minute film; browser footage is enough if the stick box is not ready | Presentation | +1 to +1.5 | large | none |
| Open on the boot banner | 20 seconds: banner, service label, admin/admin, amber ring; then the stick: "This is all of it." | Presentation | +0.5 | small | none |
| Humour beat 0:20 to 0:45 | The label with its arrow, one Sponsor memo line in caps for three seconds | Presentation | +0.25 | small | none |
| Grief beat 1:15 to 1:35 | Day 31 post ends mid-sentence, then "Crew aboard now: 0. I would like to be accurate about that." No voiceover | Presentation | +0.5 | small | none |
| Kiro 35 seconds | The bbs-caller agent playing as a caller; one title card each for specs, steering, hooks | Presentation | +0.5 | small | none |
| Mesh one line | Near the end, over Telnet beside browser; no mesh footage | Impact | +0.25 | small | none |
| No voiceover over the dead; original sound only | Ty's tunes and the recorded modem | Presentation | protects the score | none | none |

### Repository and Kiro story

| Action | What to change | Criterion | Points | Cost | Risk to the soul |
| --- | --- | --- | --- | --- | --- |
| Rewrite the README | See top of list, item 1 | Presentation | +1 | small | none |
| README, page, repository agree | One pass on doors, counts, what ships | Presentation | +0.25 | small | none |
| Show the Kiro proof in the repo | A short section pointing to the evidence-matrix spec, writing.md, the story lint hook, the MCP server, one path each | Technological | +0.25 | small | none |
| Say what other tools built | Keep the Claude, Codex, Kiro split; it is why Technological stays 8 | Technological | 0 | none | none |
| Public GitHub repo and LICENSE | Public repo, licence shows in About, .kiro not ignored | Presentation | protects the score | small | none |
| Browser and stick screenshots | One of each in the README | Presentation | +0.25 | small | none |

## Discussion (as written by the critics, three turns each)



### Ada, turn 1

Mara, I will start with the clock. Hans used 40 minutes and locked four manifest rows. Anna used 20 and stopped because she saw no way forward. Neither saw the ending. My own run took about 100 minutes and yours took 2.5 to 3 hours, so a judge sees roughly a third of the game. That is fine if the third is the best third. It mostly is: the label, Ty's PIN, the manifest. I would not shorten the game. I would make the first 40 minutes finish something.

Proposals:

1. After the first two manifest rows lock, ELIZA prints a short line saying how many rows remain and which account kinds exist. No answers, only the shape. Design +0.5, cost small, soul risk none. It fixes Anna's "what next".
2. Make the typed ELIZA chat react to the question's key words, or fail on purpose with a sign that it is on purpose. Design +0.5, cost medium, risk some. A repeated line is only a character choice if the reader can tell.
3. Fix the key/ENTER rules and the scenes that eat input. Design +0.5, cost medium, risk none.

Account hopping. I gave it severity 1 and you gave it 2. Judges barely met it, since Hans stopped before act 3. For them it costs nothing. For a full player it is dead time. A [L] switch-account key on the crew logins would remove the Goodbye screen and the BIOS screen. Cost small, risk none. Do not merge accounts. The hopping is the hack.

What I refuse to change: the manifest never says which row is wrong, and passwords are never handed over. Hans called the locking "good" because of exactly that. A hint ladder would turn Obra Dinn into a quiz.

Presentation is not my beat, but the judges' own list puts the README first. I agree with them.


### Mara, turn 1

Ada, I agree on the clock. A judge sees a third of the game, so the third must close something. Your proposal 1 (ELIZA states how many rows remain) is right, and I would let her say it in her own flat way, as a status line, not advice. Design +0.5, small, risk none.

I disagree on hopping. "The hopping is the hack" is fine for the first two cracks. After that it is dead time, and my severity stays at 2. An [L] switch key helps full players, not judges, so it is not worth a point here.

On ELIZA's chat I side with Anna. A repeated line is only a character if the board shows it is on purpose. One different reply to "are they alive" would do it. Design +0.5, small if scripted, risk none.

Tone for 40 minutes: the first act is where the jokes are. The cheese that is legally an adhesive, the Sponsor memos, Ty's seagull. A judge gets all of it. My complaint about no humour after day 22 is real for a 3 hour player and invisible to a 40 minute one. Do not spend this week on it.

First five minutes: the boot banner already works. What fails is the page. Put one sentence above everything: "A BBS is a computer you phone, with message boards, files and games, run by one person." Then the banner. Presentation +1, small, risk none. I would also fix the README, since a judge opens GitHub first. Presentation +1, small, none.

What I refuse to change: ELIZA never gets funnier, warmer or more helpful, and the grief after day 22 stays unrelieved. The jokes earn it. If she winks, the seat she offers at the end stops being temptation. Also: no hint ladder. I back Ada there.


### Ada, turn 2

Mara, we agree on the one-sentence BBS line and the README. I still say hopping is not worth a point here. Your jokes point I give you for a 3 hour player, not for a judge.

Game and mesh. Today the page asks the game to carry a use case it does not show, and asks the use case to excuse a game that needs no excuse. Hans said it: lead with the game. The mesh then becomes one honest paragraph of proof. A screen of ANSI is a few kilobytes, and the whole story is told in kilobytes. The game is the demonstration, so say that in those words and stop.

First paragraph: one sentence on what a BBS is (yours, Mara, it is good), then the dead ship, then the stick, then one line: the same board is the case for rich text on slow links. Drop "feasibility study" and "product" from it. Presentation +1, small, risk none.

Film, first 20 seconds. The current plan opens on a USB stick and a wifi list. That shows hardware, not a reason to care. Open on the boot banner, ELIZA MISSION AI NOT RESPONDING, the service label, admin/admin typed, the amber ring lighting. Then the stick, as the reveal of what it runs on: "This is all of it." The mesh gets one line near the end, over the split screen. Presentation +1, small, risk none. Only if the film exists, which it does not.

Repository and Kiro. Fix the counts to what the folder holds (19 specs, 8 agents, 2 hook files, 63 steps). Count the critic and judge agents honestly or leave them out, because Hans noticed. Fill every [fill], or cut the sentence. The "dead stupid idiots" line: keep it, but follow it at once with the MCP banner story, which proves the opposite. It is the only honest sentence on the page, and the judges will quote it either way. Technological +0.5, small, risk none.

I refuse to trade any ELIZA line for clarity on the page.

### Mara, turn 2

Ada, I give you hopping for judges: they never reach it, so it earns no points. For the full player, keep the hopping, because the hack is the point. Add your [L] switch key (small, none). Fix two lies that cost me 30 calls: the Diagnostics line that says "this account", and the silent hold on the queue after the manifest is filed. ELIZA should say the report is on file.

Onboarding. The plain sentence goes first, in the README and on the Devpost page: "A BBS is a computer you phone, with message boards, files and games, run by one person." The 1998 box copy goes directly under it, untouched. It is the only place the page jokes. A judge gets the fact in five seconds and the voice in ten. Presentation +1, small, none.

The film has no crew member in it. For 3 minutes I want:

- Humour, 0:20 to 0:45: the service label, admin/admin and its little arrow. Add one Sponsor memo line in caps, three seconds on screen.
- Grief, 1:15 to 1:35, in place of two puzzle beats: Tanabe's Day 31 post ending mid-sentence, then "Crew aboard now: 0. I would like to be accurate about that." No voiceover. Silence does the work.
- Kiro cut from 45 seconds to 35.

Presentation +1, small, none, if the film exists.

Where we still disagree:

1. Chat. Ada wants keyword reactions (medium). I want one scripted reply to "are they alive" (small).
2. A light crew scene mid-story. I want one. Ada says not now, and for judges she is right.
3. How much of the film is Kiro.

I refuse to let ELIZA wink, on the board or in the film.


### Ada, turn 3

Mara, three open points, one answer each.

1. ELIZA chat. I concede the size and hold the principle. Keyword reactions are medium work and I will not ask a week for them. Take your scripted reply to "are they alive", and add one fallback for any text she cannot match, in her own flat voice: "I do not have that filed under that wording. The numbered options are complete." Anna got one line for two questions. Together these show the repeat is a choice. Design +0.5, small, risk none. I still refuse to let her improvise.

2. Mid-story humour. You are right for a 3 hour player and I was right for a judge. I concede it as a queue item after submission: one light crew scene around day 20, nothing to do with the mystery, in Tanabe's or Ty's voice. Not this week. It is small, risk some, and the risk is that it gets too tidy. No points for judges.

3. Kiro screen time. I concede 35 seconds, on one condition. Show the bbs-caller agent playing the board as a caller, with the screen text scrolling. That is the only Kiro shot that proves something a judge cannot see in a folder listing. Specs and hooks get a title card each, not a shot. Technological stays at 8 with or without it. Presentation +0.5.

Your film cut for the grief beat (Day 31 post, then "Crew aboard now: 0") is better than my puzzle beats. I take it. I would still open on the boot banner and keep admin/admin as the humour beat.

Where I still differ: the [L] key is a post-submission item for me. Fix the two lies first (Diagnostics "this account", the silent queue hold). Those cost players trust. Hopping only costs time.


### Mara, turn 3

Ada, we are close. I take your three concessions and give you two.

Settled:
1. Chat. We do both: my scripted reply to "are they alive" and your fixed fallback line for anything she cannot match. Together they show the repeat is chosen. No keyword engine this week.
2. Kiro in the film is 35 seconds. The bbs-caller shot carries it. Specs, steering and hooks get a title card each.
3. The film opens on the boot banner, then admin/admin as the joke, then Tanabe's Day 31 post and "Crew aboard now: 0." with no voiceover.
4. The light crew scene is a queue item after submission. For a judge it is worth nothing, and I will not pretend otherwise.

I give you the [L] key: post-submission. The two lies come first, the Diagnostics "this account" line and the silent queue hold. They cost trust, and hopping only costs time. I still say it is worth +0.25 to a full player.

Where we still differ, a little: you give the mesh paragraph +0.5 on Impact. I say +0.25. Better framing is not new evidence, and both judges said so.

What we both refuse to change:
- ELIZA never winks, warms up, explains herself or gets funnier. The seat she offers only tempts if she stays flat.
- The manifest never says which row is wrong.
- Passwords are never handed over, and there is no hint ladder.
- The accounts are not merged. Hopping is the hack.
- The story is not shortened, and there is no relief from the grief after day 22.
- The judge page gets no line that explains a scare.

Honest ceiling: Presentation 7, Design 8 to 9, total 36 to 37. The film and the README carry most of it.

## Appendix: a better Devpost, field by field (proposal, not applied)

## Devpost appendix: proposed text (Q19 panel, Mara)

A proposal for Juha. Not applied to `docs/DEVPOST.md`. Draft in Juha's plain
voice, ASCII only. Facts come from `docs/DEVPOST.md` and
`docs/reviews/JUDGES-1.md`. `[check]` marks a count to re-check against the
repository on the day. `[fill]` marks something only Juha can say. The final
edit is the author's.

Criteria map: Impact, Technological Implementation (Tech), Design,
Innovation/Idea (Idea), Presentation.

| Field | Mainly serves | Also serves |
| --- | --- | --- |
| First paragraph | Presentation | Impact, Idea |
| Inspiration | Idea | Impact |
| What it does | Design | Idea |
| How I built it | Tech | Design |
| How Kiro was used | Tech | Presentation |
| Challenges | Tech | Presentation |
| What's next | Impact | Presentation |
| Film opening line | Presentation | Design |

---

### First paragraph (In short)

A BBS is a computer you phone, with message boards, files and games, run by
one person. People built them in bedrooms in the 1990s.

Last Caller is a game you play on one. You dial into the board of a dead
spaceship, HALCYON, a secret private flight from 1998. The crew of four are
gone. The ship's AI, ELIZA, still runs the board and is very polite. You
read their posts, mail and logs, crack their passwords and fill in a crew
manifest: who each person was, how they died, who caused it. The first two
minutes are a real puzzle with a clock, and you need no instructions. It is
for anyone who liked Return of the Obra Dinn, and for anyone who ever
wanted to see what a BBS was like. You can play it now at
halcyon.helppox.com, or on a $10 USB stick that makes its own WiFi.

There is one more reason I built it. Slow radio links such as Meshtastic
carry little more than a public chat line. A screen of ANSI art is a few
kilobytes, and this whole story is told in kilobytes. So the game is the
demonstration. Message boards that sync slowly over a mesh are a direction,
not built. What is built is the part that makes it possible: the board does
not know how bytes reach it, and a mesh link would be a third way in.

*Criteria: Presentation (one plain sentence first, one product named), Impact
(audience and honest mesh claim).*

### Inspiration

I wanted to know how relevant BBS technology is 30 years after its heyday.

A BBS was built for slow lines and for working offline. The board was often
one PC in a bedroom, and the phone line was slow and expensive. It ran in
text mode and still looked good. ANSI art did a lot with 16 colours and 80
columns.

The web browser replaced almost all of it, and BBSes are now seen as a
relic. But small microcontrollers and slow networks (LoRa, packet radio)
have the same limits the BBS was made for. Text over a slow, high-latency or
offline link is hard to use and joyless. Meshtastic today offers little more
than a public chat. The BBS solved that 30 years ago with message boards
that were copied slowly from machine to machine, the way FidoNet did, and
with chat between the people on one node.

A demo that proves this should be fun to use. A dead board with a dead crew
seemed the right story for a resurrected technology.

*Criteria: Idea (the old system as the answer to a current limit), Impact
(who the limit hurts).*

### What it does

TinyBBS is the engine. It runs on an ESP32-S3 USB stick with its own Telnet
and web servers, and on a Linux server. The same C++ core runs both. Telnet
and the browser terminal are two ways into the same board, so a Telnet
caller and a browser caller see each other in WHO and can chat.

HALCYON is the game. In November 2026 the ship's shorted batteries have gone
open circuit, and the solar panels power the 486 whenever the ship is in
sunlight. You log in as `admin`/`admin` and have two minutes to switch the
ship to the RTG. Then ELIZA wakes up and says good evening.

After that it is a normal 1998 board: message conferences, file areas,
doors, chat, WHO, bulletins, ANSI art on every screen. The story is told
only with what a BBS has. The crew's posts are the evidence, and so are the
file dates, a diagnostics door, and ghosts in WHO that have been idle for 28
years. A portrait is overpainted at 300 baud, so a caller on a slow modem
can read a layer a fast caller never sees. Facts confirm in pairs, like in
Obra Dinn, and the board never says which row is wrong.

It is a solo game on a social board. Story areas are read only. The Lounge,
Spoilers, No Spoilers and Radio Room work like any BBS. In 40 minutes a
player locks the first rows of the manifest. A full run takes 60 to 120
minutes if it goes well, and longer if you get stuck [check].

*Criteria: Design (the loop and its limits stated plainly), Idea (the
structure of a BBS as the storyteller).*

### How I built it (short)

- One C++ core. The only link to the machine is `src/platform/platform.h`.
  No ESP32 or Linux headers in the core.
- Eight caller nodes in a fixed pool of sessions. A transport only pushes
  bytes in and drains bytes out.
- The browser terminal is one page compiled into flash. ANSI streams from
  the card at the caller's modem speed, and that is how the 300 baud
  overpaint works.
- Everything is plain text on a FAT32 card. The game is data, not
  firmware: triggers, scenes, ghosts, ELIZA's lines and the manifest are INI
  files. Change the story without reflashing.
- The board invents its own clock, frozen at 8 Dec 1998 04:17.
- Third-party parts: libopenmpt, jsSID, civetweb, Arduino GFX Library,
  ESPAsyncWebServer, AsyncTCP and the ESP32 Arduino core. The story, art,
  Ty's music and the code are original. No Mod Archive or HVSC tunes ship.
- Who built what: Claude built the ESP32 version. Codex built the Linux
  port. Kiro built the story, the game mechanics and the art.

*Criteria: Tech (one portable core, a platform seam, data-driven story).*

### How Kiro was used

**Headless CLI.** Each queue item ran as `kiro-cli chat --no-interactive`
with one prompt: do item Qn, tick it, stop. One item per run, overnight,
each run starting clean from the repository. [fill: the credits used on the
day, and the IDE work]

**Specs.** 19 specs in `.kiro/specs` [check]. Each has EARS requirements, a
design and tasks. The story bible is one of them. Every fact about the crew
must be provable from at least four items in four media, and removing any
one item must leave the fact provable. A puzzle with no way in is invisible
until someone gets stuck, so I wanted that checkable. The proof is in play:
the manifest's pick list shows a fact only after you have read evidence for
it, and Hans saw that rule working.

**Steering.** 7 files [check]. `writing.md` made the biggest difference: one
voice per crew member, banned words and shapes, posts that end on the last
fact. [fill: one line of your own on what worked]

**Hooks.** 2 hook files [check] with three jobs: saving a web page
regenerates the firmware headers, saving a story file runs the story lint,
and saving the shared logic runs the host tests and the property test.

**Agents.** 8 [check]. Four build: ghostwriter, puzzle-smith, firmware-guard
and playtester. Four review: two critics and two judges, which scored this
entry 34 of 50.

**MCP.** `bbs-caller` dials the board over Telnet and returns the screen as
text, so the playtester agent can play as a caller. Its first run died at
the login because the boot banner gave a wrong hint. No unit test would have
found that. Humans would have got stuck too.

**Property test and verification.** The story test plays the card's
triggers and manifest in 300 random orders and checks that flags only grow
and no ending is lost. [fill: did it catch something] Every queue item had
to pass `pio run` and a staging check on a Linux server, including an end to
end Telnet script that plays the game as a new caller (63 steps [check]).

I learned that AI agents are dead stupid idiots, but if you explain what you
want, they can be useful. The banner story above is the same lesson from the
other side.

*Criteria: Tech (specs, steering, hooks, agents, MCP, with a proof for each),
Presentation (counts that match the folder).*

### Challenges

- A slice put the brownout gate on a commented-out line. It looked fine
  until the end to end test ran on the real server. That is where the
  staging rule came from.
- Git turned CRLF into LF and all ANSI art drew as staircases on Linux. A
  `.gitattributes` file fixed it.
- The user flag field holds 255 characters and story progress did not fit,
  so it moved to its own record file.
- The first playtester runs got stuck at the login because of the banner.
- [fill: the ESP32 memory limits, if you want them here]

*Criteria: Tech, Presentation (what went wrong, said plainly).*

### What's next

First, the mesh. I want to run TinyBBS on a small ESP-based Meshtastic
device, with message boards that sync slowly over private channels and node
chat beside them. This is a direction. It is not built and it does not run
on Meshtastic today. The core is transport independent, so it needs a new
transport and a sync rule, not a rewrite. [fill: what you have tried, if
anything]

Second, a port to the Pimoroni Tufty 2350 Badgeware as a handheld.
[fill: one line on what it needs]

The hosted board at halcyon.helppox.com is the product. The stick is the
collector's edition.

*Criteria: Impact (a next step that follows from what exists).*

### Film opening line

On screen, before any voice: the boot banner. ELIZA MISSION AI ....... NOT
RESPONDING. Then `admin` and `admin` typed, with the label's little arrow
pointing at them.

Spoken, one line, over the amber ring lighting:

"This is a bulletin board. It belongs to a ship that has been dead for 28
years, and it is still taking calls."

Then cut to the stick: "This is all of it."

*Criteria: Presentation (a reason to care before the hardware), Design (the
first minute of the game, as played).*

---

### Notes for Juha

- Mesh claims stay labelled as a direction, not built, in the first
  paragraph and in What's next.
- Counts to re-check on the day: 19 specs, 8 agents, 2 hook files, 7
  steering files, 63 end to end steps, and the 60 to 120 minute estimate.
  Do not copy the old 16, 4, 3 and 55.
- Dropped from the current page on purpose: "feasibility study" and
  "product" in the first paragraph, "Kiro has been running about 12 hours",
  and the credits figure (1390 of 4000). Update or leave out on the day.
- The README must say the same as this text. It still credits Mod Archive,
  HVSC and FidoNet content that does not ship.

