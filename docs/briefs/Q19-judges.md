# Brief Q19 - Judges' panel, then the critics' plan

From: Claude Code (planner), at Juha's request, 14 Nov 2026. To: Kiro.

Q18 is stopped by Juha after round 3 (Ada 85, Mara 80, average 82.5). The
round-3 fixes are in `bb94e79` and pushed, but their staging check was cut
off. Q18 stays open; do not run critic rounds in this item.

This item asks a different question: not "is it a good adventure game"
but "how would Kiroween judges score it, and how do we raise that score
without destroying the soul of the game".

## 0. Before anything

Run the staging check (`tools/staging-check.ps1`) on `bb94e79`. If it fails,
fix it first (prefix `Q19:`), so the judges play working code. If the
staging host still has a board running from the stopped Q18 run, stop it
with `ssh lastcaller@helppox.com "touch ~/lastcaller-staging/stop"`.

## 1. The use case (Juha's words, to add to the Devpost text)

Add this to `docs/DEVPOST.md` (Inspiration, What it does or What's next,
where it fits best; draft for Juha's edit, in his plain voice, following
`.kiro/steering/writing.md`), and one short paragraph to `README.md`:

- The problem: low-bandwidth, high-latency and offline text-mode
  communication systems are hard to use and joyless. Meshtastic and other
  LoRa mesh networks today offer little more than a public chat line.
- The BBS solved exactly this 30 years ago. TinyBBS could run on small
  ESP-based Meshtastic devices and work like FidoNet: message boards that
  sync slowly between devices over the mesh using private channels, plus
  node chat working like Meshtastic's public chat today.
- So the retro has a function: learning from past systems how to make
  text-based systems intuitive and fun. The demo game is also entertaining,
  a value in itself, and it shows what a few kilobytes of ANSI can carry.
- Be honest about what exists: the mesh sync is a direction, not built.
  Write it as what the design allows and what is next (the core is already
  transport-independent: Telnet and browser are two transports into one
  BBS core; a mesh would be a third). Do not claim it runs on Meshtastic
  today.

Juha's framing for the judges (14 Nov 2026), to shape the text:

- The game counts as a real product someone can use: people play it, on
  the hosted board or on the stick. It is not a mock-up of a product.
- At the same time the project is a feasibility study: does BBS technology
  still work for today's low-data links such as Meshtastic? The game is the
  proof that a text-mode system can be rich, intuitive and fun.
- Using a real, working BBS as the game engine is the unusual part (the
  Innovation criterion): the story is told through message bases, file
  areas, doors, node chat and logins, not a game engine dressed as a BBS.
- Public resources: say honestly what the project already uses (check
  `docs/CONTENT_POLICY.md` and the content scripts in `tools/`). Do not
  bolt on a dataset (NASA or other) just to tick the box; it would feel
  artificial.
- Proof that the engine is a real, usable BBS and not only a game:
  `kouvosto.helppox.com` runs the same TinyBBS engine as an ordinary board,
  seeded with about 1000 real FidoNet messages (the `tools/fidonet-seed.py`
  history). Name it in the Devpost text and README as the reference board,
  next to `halcyon.helppox.com` (the game). The FidoNet archive is also the
  honest answer to "public resources": real BBS history, reused. Check its
  source and licence in `docs/CONTENT_POLICY.md` before describing it. The
  judges may look at the Kouvosto board's front page as a judge would
  (read only, no posting).
- The Devpost form answers carry most of the weight: a judge must
  understand in the first paragraph what this is, who it is for and why
  it matters.

Commit this before the judges play, so they read it.

## 2. The judges

Write two Kiro agents in `.kiro/agents/` (check the current agent format;
do not guess): `judge-hans.md` and `judge-anna.md`. Invented people.

- **Hans** has never used a BBS. He played PlayStation games when he was
  younger (he remembers Metal Gear Solid and Final Fantasy VII), works in
  tech, and is curious but impatient with anything that feels like homework.
- **Anna** is a finance professional. She has never used a BBS or a
  terminal, plays little, and judges like an investor: who is this for,
  what problem does it solve, would anyone use it, is it finished.

Both are hackathon judges with many entries to get through. Each one:

1. Reads `docs/DEVPOST.md` as the Devpost page (not the checklist at the
   end), and the film plan in `docs/LAUNCH_KIT.md` section 3 as a stand-in
   for the video, since the video is not made yet.
2. Plays the game from a brand-new caller on a fresh staging board
   (`tools/staging-playtest.ps1`) through `bbs-caller`, as a busy judge
   would: about 40 minutes of real reading, roughly 200 tool calls at
   most. Finishing is not required; note honestly where and why they stop
   or get lost.
3. Looks at the repository the way a judge opens the GitHub link:
   `README.md`, `.kiro/` (specs, steering, hooks, agents), `docs/DEVPOST.md`
   "How Kiro was used". Only after playing. No reading `sd/` or `src/` to
   solve the game.
4. Writes notes and a provisional score per criterion, 1 to 10, against the
   Kiroween judging criteria below, with reasons.

Then the discussion: the two judges compare notes and argue it out (at
least three exchanges each, each judge in her or his own fresh session
that reads only her or his own notes and the other judge's notes), ending
with one agreed score per criterion and a total. Write it as
`docs/reviews/JUDGES-1.md`: each judge's notes, the discussion transcript
(condensed, but in their voices), the agreed scores and the three things
that cost the most.

### Judging criteria (verbatim from Kiroween)

- **Potential Impact.** Does the project make a credible case for solving a
  real problem for a real audience? Does the solution address that problem
  based on what's demonstrated? How useful, intuitive, and accessible is it?
  Would someone actually use this?
- **Technological Implementation.** How thoroughly and skillfully does the
  project use Kiro? Was there a thoughtful strategy to AI development? Was a
  variety of Kiro tools used impactfully? Does the project show a deep
  understanding of Kiro features? Was there experimentation?
- **Design.** Does the project deliver a complete, coherent product
  experience, not just a technical proof of concept? Is the design
  intuitive, polished, and intentional?
- **Innovation/Idea.** How creative and novel is the concept, and does the
  project differ from existing concepts? Are any unusual technologies or
  public resources (like datasets) leveraged or combined in a unique way?
- **Presentation.** Does the video clearly demonstrate the project working
  end-to-end? Does the pitch communicate what problem is solved, who it's
  for, and why it matters? Is the overall presentation easy to follow? Is
  the presentation creative and memorable?

## 3. Ada and Mara take the result

Ada (Obra Dinn fan, puzzles) and Mara (Leisure Suit Larry fan, story and
humour), the Q18 critics, read `docs/reviews/JUDGES-1.md`, their own
round-3 reviews (`docs/reviews/R3-ada.md`, `R3-mara.md`) and
`docs/DEVPOST.md`. The Q18 agent files stay frozen; make separate panel
agents (`critic-ada-panel.md`, `critic-mara-panel.md`) with the same
personas and these reading rights.

They discuss, in the same way as the judges, how to raise the judges'
score without destroying the soul of the game: the haunted ship, ELIZA,
the crew, the 1998 board, the slow reveal, the grief and the jokes. They
must say what they would refuse to change and why. Typical tensions to
settle: a judge's 40 minutes against a 2-hour story; first-five-minutes
onboarding for people who never saw a BBS; the account hopping; how the
game and the mesh use case support each other in the pitch and the video.

Because the Devpost answers matter most, the panel also drafts a better
version of each Devpost field, mapped to the five criteria, as an appendix
of `PANEL-1.md` (a proposal for Juha, not applied to `DEVPOST.md`).

Write `docs/reviews/PANEL-1.md`: the discussion (condensed), then a ranked
action list, each item with: what to change, which criterion and how many
points they expect it to win, the cost (small, medium, large), and the
risk to the soul (none, some, real). Split it into: game, onboarding,
Devpost text, video plan, repository and Kiro story.

## Rules

- Do not change the game in this item. The only edits are the Devpost
  text and README paragraph (section 1), the new agent files, the review
  files, a staging fix if section 0 needs one, and the tick. Juha decides
  what of the action list to queue.
- Never tell the judges what the critics found, or the critics' scores.
- Small commits prefixed `Q19:`. Never claim a check passed if it did not
  run.

## Done when

The staging check on the round-3 code has run (result in the tick),
`DEVPOST.md` and `README.md` have the use case, `JUDGES-1.md` and
`PANEL-1.md` are committed, and the tick in `docs/briefs/QUEUE.md` gives the
agreed judge scores per criterion and total, how far each judge got, and
the top five actions from the panel.
