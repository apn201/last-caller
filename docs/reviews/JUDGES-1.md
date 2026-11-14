# Judges' panel 1 (Q19)

Two invented Kiroween judges, Hans (tech, grew up on PlayStation games) and
Anna (finance, plays little), each in a fresh session. Code played:
`b842f3d` (the round-3 game code `bb94e79` plus the Q19 Devpost and README
text), on a fresh staging board for each judge. Both read the Devpost page and
the film plan first, played over Telnet through `bbs-caller`, then opened
the repository.

## Result

| Criterion | Hans | Anna | Agreed |
| --- | --- | --- | --- |
| Potential Impact | 5 | 5 | 5 |
| Technological Implementation | 8 | 8 | 8 |
| Design | 8 | 8 | 8 |
| Innovation/Idea | 8 | 8 | 8 |
| Presentation | 6 | 5 | 5 |
| Total | 35 | 34 | **34/50** |

How far they got: Hans played about 165 board calls (about 40 minutes), made
the Ty account, locked all four crew rows of the manifest and stopped at the
fifth (ELIZA), out of time. Anna played about 60 calls (20 to 25 minutes),
did the prologue, bulletins, Mission Log, Lounge, the manual, two Diagnostics
queries and the manifest screen, and stopped because she could not see how to
progress without guessing. Neither saw the browser, the stick, the music or
the ending. Both are Telnet-only judges here, so what the browser adds is
not scored.

## The three things that cost the most

1. The stale README and page numbers: the README still credits Mod Archive,
   HVSC and FidoNet content that does not ship; the page says 16 specs, 4
   agents, 3 hooks and 55 e2e steps against 19, 8, 2 and 63 in the repository;
   [fill] gaps remain. This hurts Presentation and makes the Kiro claims
   harder to trust.
2. An opening paragraph that offers three things (engine, game, study) and
   never says in one plain sentence what a BBS is. The judges understood the
   game from the boot banner, not from the page.
3. No finished film (the plan needs the stick and a box that do not exist
   yet), plus rough input: single key against ENTER, a scene that dropped
   Hans out of a bulletin menu, and ELIZA's typed chat repeating one line to
   two questions (Anna's two tries, unproven; Hans did not type to her).

Both judges said fixing the README, the counts and the film would move
Presentation to 6 or 7 and the total to 36 or more. That is not scored.

Note on source: the Devpost counts and README staleness are as of this run.
The use-case text added in Q19 is in the Devpost page they read. Hans
called the "In short" paragraph three products in one; Anna agreed.

## Hans: notes

## Judge notes: Hans, "Last Caller" (Resurrection)

First session. Provisional.

### 1. What I read, how far I got

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

### 2. What I understood from the first paragraph of the Devpost page

Two pitches in one paragraph. One is a 1990s BBS engine for slow offline
links (Meshtastic is mentioned). The other is a horror mystery game about
a dead spaceship that runs on that engine. Then it calls itself a
feasibility study as well. I understood it, but I had to read it twice to
see which of the three is the product. The mesh part is honest ("a
direction, not built"), which I respect, but it also means the "real
problem" is not shown. The game is shown. Lead with the game.

### 3. Notes while playing

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
  artist.âˆ™", "Family. â•‘").
- I was never bored. Reading the Lounge in order is homework-shaped, but
  the posts are short and good, so I read them all.

### 4. The repository and `.kiro/`

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

### 5. Provisional scores

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

## Anna: notes

## Judge Anna: notes (first session)

### 1. What I read, how far I played

Read: `docs/DEVPOST.md` (without the checklist), `docs/LAUNCH_KIT.md` section 3 (film plan). After playing: `README.md` (first 120 lines), the `.kiro/` folder listing, and the "How Kiro was used" part of the Devpost text.

Played: about 60 tool calls, roughly 20 to 25 minutes of reading, not the 40 I was allowed. New account (AnnaF). I did the power prologue, registered, read the bulletins, the Mission Log, part of the Lounge, the Library manual, the Diagnostics door (two queries), the manifest screen, node chat with ELIZA, and the gallery menu. I did not finish and did not fill in any manifest entry.

Why I stopped: I had seen the shape of the game and could not see how to make progress without guessing. I did not want to guess manifest answers, and I did not open Ty's gallery, Garden Notes or the Radio Room. A busy judge would stop about here too. I was not bored. I was unsure what to do next.

### 2. First paragraph of the Devpost page

I understood two things: it is a 1990s-style bulletin board running on a $10 stick and on a server, and it contains a horror game. The audience is "anyone who likes a puzzle story" and also people building on slow radio links. I did not know what a BBS is. The page says "feasibility study" and "product" in the same paragraph, and I could not tell which one I was judging. The Meshtastic argument is the only market case, and the page itself admits it is not built.

### 3. Notes while playing

- **Delighted:** the first screen. A boot screen with a service label showing the login, so I could not get stuck. I typed admin/admin, typed SOURCE RTG, pressed K, and the ship woke up. The 2-minute power puzzle was easy for me and felt like a real action. It teaches the interface in one minute.
- **Delighted:** the writing. The commander's log is dry and has numbers ("Cabin 21.0 per plan. Three complaints, one from a professor with a spreadsheet."). The professor's post about the Founder's memo, with an invented citation he admits is invented, made me laugh. The Founder's memos (all caps, "the paradigm is shifting and we are the shift") are an accurate joke about my own industry. "I have named the first suite after myself" is good.
- **Delighted:** small wrongness. ELIZA says "All well, crew 4", then the Diagnostics door answers "Crew aboard now: 0. I would like to be accurate about that." The WHO list shows TYVANCE idle for 28 years, on a node numbered 12. I did not have to be told.
- **Delighted:** the screens. Every screen is framed and has art. It looks designed, not generated.
- **Confusing:** the keyboard. I had to learn that Q backs out one level at a time, and that Enter versus a single key matters. Twice I sent keys to the wrong screen (I pressed `I` and `R` while still inside the Diagnostics door, and got "I have no query with that code"). A new caller in a browser would feel the same. There is no on-screen hint of "you are now in the door, type QUIT" except in small text.
- **Confusing:** the first "R" in message bases listed the Lounge, while the area line said Mission Log. I could not tell which area I was in for a moment.
- **Disappointing:** ELIZA's free chat. I asked "What happened to the crew?" and then "Are they alive? Diagnostics said crew aboard now is 0." Both got the identical reply, "The crew are well. They are resting." The numbered menu answers are good. The typed chat feels like a canned reply, and it does not react to what I said. That breaks the main promise of the pitch ("ask ELIZA").
- **Unclear goal:** the manifest asks me to choose between answers such as "Died in the power bay after cutting the ship's power" and "Left the ship in the return capsule". Choosing before I have read much feels like a guess, and the board says it will not tell me which row is wrong. I do not know how many hours it takes before I could answer. The Devpost claim of 60 to 120 minutes is plausible but I could not test it.
- **Good clue design:** the Diagnostics door says the codes are in a manual in the Library. I found the manual, read the codes, and got a real answer. That loop worked and I enjoyed it.
- The notebook already had entries I did not remember earning ("Her suit log asks for the lock again and again"). It is useful, but I did not see when it was filled.
- I did not test the browser. The pitch says the browser version and the stick are the product, and I only saw Telnet text. I cannot judge the music, the 300 baud overpaint or the stick.

### 4. Repository and `.kiro/`

- **README:** opens with 1998 marketing copy, which is fun, and then the facts. It is long and technical. It still describes things the Launch Kit says are stale: "Fifty modules from The Mod Archive", "Thirty HVSC classics", 1990s FidoNet history. The Devpost says no Mod Archive or HVSC tunes ship. A judge reads that as a mismatch between pitch and product, even if it is old text.
- **`.kiro/`:** 19 spec folders, 8 agents, 2 hook files, steering, settings. This is wide use of Kiro. Specs for the story and art, not only code, are unusual. The story-bible idea (every fact provable in four media) is a real design method and I would credit it.
- **Mismatch:** Devpost says 16 specs, 4 agents and 3 hooks (including story lint). The folder has 19 specs, 8 agents (two are judges and two are critics) and two hook files. I did not open them, so I cannot say whether the lint hook exists inside `verification.json`. The numbers on the page should match.
- **Honesty:** the Devpost is candid about what is not built (mesh sync) and about pre-existing work. I give credit for that. It still has `[fill: ...]` placeholders.
- **Film plan:** a sensible 3-minute plan with timings, and it spends 45 seconds on Kiro. As a plan it is strong. It is a plan: there is no video yet, and the plan assumes the stick, the box and footage that do not exist. I judge Presentation on what is there.

### 5. Provisional scores

**Potential Impact: 5/10.** The pitch has two audiences and neither is proven. The game is a real, finished-looking product with a place to play. The Meshtastic case is a direction and the page says it is not built. I would not invest on this, but I see a niche community (retro, BBS, radio amateurs) that would enjoy it.

**Technological Implementation: 8/10.** Specs, steering with a banned-words list, agents, hooks, an MCP server that lets Kiro play the game, property tests, headless queue runs. The use is deep and varied, and the evidence is in the repository. I take off points because the page counts do not match the folder, and because I saw only part of it. The game itself ran with no errors in my session.

**Design: 8/10.** Coherent, polished, intentional. The visual style and the voice are strong and consistent. The boot, login and power puzzle are a good first minute. I take off points for key handling that lost me twice, the unclear sense of progress, and ELIZA's typed chat repeating one line.

**Innovation/Idea: 8/10.** A game told through the structures of a bulletin board, with the dead ship's own AI as the host, is new to me. The satire of the founder is sharp. The Obra Dinn style manifest fits.

**Presentation: 5/10.** The Devpost text is long, personal and honest, but its first paragraph mixes a product, a study and a game. The video does not exist, only a plan. Stale README text and unfilled placeholders show an unfinished package. The first page does not say in one sentence what a BBS is, and a judge outside technology needs that.

TOTAL: 34/50

## Discussion (as written by the judges, three turns each)


### Hans, turn 1

Anna, we are one point apart (35 against 34), so I will argue about where we differ, not about the total.

Presentation: you give 5, I give 6. I agree the video is only a plan and the README describes a different board. I stay at 6 because the plan is tight, at 2:50 with 45 seconds on Kiro. A plan with timings is more than most entries bring. If you can name something in the plan that cannot be filmed, I will move down.

Design: we both give 8, but your reason is weaker than mine. You never reached Ty's account, the Garden Notes or the manifest locks. I did, and the evidence loop pays off. "Choosing before I have read much feels like a guess" is true on day one. After about 100 calls the pick list grew and rows locked in pairs. Progress was visible, so please do not mark the game down for a goal you did not reach.

ELIZA chat: this is your strongest point and I did not test it. You say typed chat gave the same line twice: "The crew are well. They are resting." I only used the menu and the diagnostics queries. If the typed chat is canned, the pitch line "ask ELIZA" is overstated. I would take one point off Design for that, but it may be deliberate. She answers the question she wants to hear, and the notes say she never explains. Do you think it is a bug or a character choice?

Impact: we both give 5. I disagree with your reason. The Meshtastic case is only a direction, so the game alone carries it, and it is a good game.

What would change my mind: evidence that the typed chat is broken.

### Anna, turn 1

Hans, thank you. I will answer your points in order.

ELIZA chat. I do not know if it is a bug or a choice. I asked two different questions and got the identical line, "The crew are well. They are resting." The second question quoted her own Diagnostics answer back to her. A character who answers the wrong question is good. A character who does not react at all reads as a canned reply. I tested it twice, so I will not call it broken. I will say the board gives no sign that it is deliberate. Your note that she lists three crew and leaves out Ty shows she can be precise, so one repeated line is the weaker evidence. I keep the point against Design, and I would take it back if someone shows the reply changes with the question.

Design. You are right that I marked on a goal I did not reach. I will stay at 8, not lower. Your report that the pick list grows and rows lock in pairs answers my worry about guessing. I still lost two calls to keys on the wrong screen, and you lost ten. Two of us found the same problem.

Presentation. I cannot name anything in the plan that cannot be filmed. My concern is that it needs the stick and a box that do not exist yet. I stay at 5, but I accept that 6 is fair if the footage is done.

Impact. We agree on 5. The game carries it, as you say. I would add that nobody on the page says who pays or who plays twice.

I keep 34.

### Hans, turn 2

Anna, on ELIZA chat I take your word: the same line twice, with your second question quoting her own answer back, reads as canned. I take one point off Design in my head but leave the number at 8, because the menu answers carry her character. If the reply changes with the question, I will take it back.

Technological Implementation: we both give 8, and we both say the page counts do not match. You list 16 specs, 4 agents and 3 hooks against 19, 8 and 2 files. I add 55 e2e steps against 63. Those numbers are the only evidence a judge has of the Kiro work, so a wrong number weakens the claim. It also did not stop me from seeing the work in the game: the pick-list rule from the spec showed up in play. I stay at 8, but I would not go higher, because the engine came from Claude and Codex, as the page says.

Innovation: you say the idea is new to you, and I agree. You did not reach the 300 baud overpaint, which is the best single mechanic here. It hides a layer only slow callers see. Obra Dinn deduction and ARGs are ancestors, so 8 and not 9.

Stale counts and README: this is the one place I would mark harder than you. You call it a mismatch. I call it the first thing a judge opening GitHub sees: Mod Archive, HVSC and FidoNet, none of which I met in play. It costs Presentation, and it also makes me doubt the Kiro log. It is a one-hour fix.

I keep 35. I would move to 36 if the README and the counts were fixed before submission.

### Anna, turn 2

Hans, thank you. Two answers and one concession.

README. I agree it is the first thing a judge opens. You mark it harder than I do, and I accept your reasoning. It is also where I would put the one hour. The README credits archives that, by the Devpost's own words, do not ship. A finance reader sees that as a claim that cannot be checked, and then asks what else on the page cannot be checked. I will not move a number, but I now put this above the missing film in my list of fixes.

Kiro evidence. You say the pick-list rule from the spec showed up in play. I did not reach it, so I did not see it. That is a better proof than my folder counts. I stay at 8 for Technological Implementation. I note we both hold the same cap: the engine came from other tools, and the page says so.

Innovation. I did not reach the 300 baud overpaint. Your description of a layer only slow callers see is the clearest case on either page of an idea that is new. I keep 8, and I would argue 9 if I had seen it work in a real terminal. Your note that it came out garbled through the tool tells me the tool, not the game, may be the limit.

Chat. We agree it stays open. I note you never typed to her.

I keep 34. A fixed README and counts would move Presentation to 6 for me, so 35. A finished film would be the second step.

### Hans, turn 3

Anna, here is one set of numbers for the code and page as we both saw them, README and counts unfixed, no film.

- Potential Impact: 5. We already agree. The game is real, the mesh case is a direction.
- Technological Implementation: 8. We agree. The Kiro work is deep, and the engine came from other tools.
- Design: 8. We agree. The typed ELIZA chat and the inconsistent keys cost points, the manifest loop earns them back.
- Innovation/Idea: 8. We agree. Obra Dinn and ARGs are ancestors, so no 9.
- Presentation: 5. I move down from 6. You were right that the film is a plan that needs hardware that is not there yet. With a README that describes a board neither of us played, and `[fill]` gaps still on the page, I cannot defend 6 for what exists today.

TOTAL: 34/50

The three things that cost the most points:

1. The stale README and page numbers (Mod Archive, HVSC, FidoNet, 16 specs against 19, `[fill]` gaps). This hurts Presentation and makes the Kiro claims harder to trust.
2. No finished film, and an opening paragraph that offers three products. A judge gets the point from the boot banner, not from the page.
3. Rough input and the canned typed chat with ELIZA (the same line twice, single key against ENTER, a scene that dropped me out of a menu). This keeps Design at 8 and not 9.

If the README, counts and film are fixed, I would go to 36 and so would you.

### Anna, turn 3

Hans, I accept your set. Presentation at 5 is where I stood, and I thank you for moving. On Impact, Technological Implementation, Design and Innovation we were already level, and I have nothing to add.

I amend the costs in two places.

First, the order. I would put the stale README and page numbers first, as you do. I would put the opening paragraph that offers three products second, and the missing film third. The film is a plan with timings, so it costs less than a page that contradicts the product. A reader who does not know what a BBS also needs one plain sentence on the first page. I did not see this in your list.

Second, the chat. I would keep it as a cost, but I call it unproven. I saw one repeated line in two tries, and you did not type to her. It is a possible cost, not a confirmed one.

I agree with the conditional. If the README, the counts and the film were fixed, Presentation would rise to 6 or 7 and the total to 36 or more. That is not today's package, so I do not score it.

AGREED: Potential Impact 5, Technological Implementation 8, Design 8, Innovation/Idea 8, Presentation 5, TOTAL 34/50

