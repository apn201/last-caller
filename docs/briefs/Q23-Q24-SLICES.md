# Q23 and Q24 in gated slices

From: Claude Code (planner), at Juha's request, 16 Nov 2026. To: Kiro.

Juha: "We have so many changes. Split it in gated slices, to be tested both by
me and agents (1 time is enough) before advancing."

The content is in `docs/briefs/Q23-playtest-juha.md` and
`docs/briefs/Q24-new-verbs.md`. This file sets the **order and the gates**.
Where it disagrees with those two briefs, this file wins.

## The gate (every slice)

1. **Build.** Kiro does the slice only, in small commits prefixed with the
   slice id (`G1:` ...). Checks: `pio run`, host tests, story lint,
   `tools/staging-check.ps1` with the e2e finishing the game. Push `main` to
   `staging` and deploy.
2. **Agent test, once.** One `playtester` run (`docs/PLAYTEST.md`,
   `tools/staging-playtest.ps1`) on a fresh board, aimed at the slice's test
   below. Report in `docs/reviews/G<n>-agent.md`. Fix what it finds that is
   in the slice's scope; anything else goes to the next slice's notes.
3. **Stop.** Tick the slice in `QUEUE.md` with: what changed, RAM and flash,
   the staging result, the deploy commit, the agent's findings and fixes, and
   **what Juha should try**. Do not start the next slice.
4. **Juha tests, once.** He plays the deployed board. His notes go to
   `docs/reviews/G<n>-juha.md` (the planner writes them down). Fixes from
   his notes are done in the same slice, then the next slice may start.

Exception: G4 is a design slice. Its "test" is a critic agent reading the
draft once, then Juha's approval.

## The slices

| Slice | What (source) | Agent test | Juha tries |
| --- | --- | --- | --- |
| **G1** Scenes and the M7 pointer | Q23 items 1 and 2: only Q skips a scene, the `Q skips` line, the 1.5 s grace, type-ahead dropped at start; the grant line and the password/login keyword point at Engineering | Types during every scene trigger it can reach; each scene shows its last node. Finds ELIZA's login from the hints alone | Plays to M7 typing as he likes; no scene is lost; finds the login |
| **G2** ELIZA's chat | Q23 item 3: RiveScript reader, her brain on the card, per act | 30+ chat lines per act it has reached; counts repeats; tries to get a gate from free chat (must fail) | Talks to her in each act he reaches; she sounds alive, never stuck on one line |
| **G3** Hints | Q23 item 4: `HINTS.INI`, three tiers per milestone, `H` in the notebook, the 5-minute steps, the radio club's voice | Asks for a hint at each milestone it reaches; checks no tier names an answer | Asks for hints when stuck; they help without giving it away |
| **G4** New chain, design only | Q24 "Design first": `PUZZLE_CHAIN.md` rewrite, story bible, private story posts solved on paper | A critic (Ada or Mara) reads the draft once and reports | Approves or edits the chain. Nothing is built before this |
| **G5** Posting and the radio club | Q24 engine 1 to 3 and R1 to R3: `event=post`, delayed private threaded replies, `event=upload` (browser, optional), the club from M2, "help" in the Radio Room | Posts good, vague and wrong questions; replies arrive on the pass, under its post, private | Posts to the club; the reply feels like a person on the next pass |
| **G6** Three trials and ELIZA's orders | Q24 act 2: T1 to T3, the orders as notebook entries and talk options, the argument replaces M6 reply 3 and M7's password | Plays the three trials in a different order from the e2e; argues with wrong pairs, then wins | Plays act 2 in his own order; the argument is fair and fun |
| **G7** Forged order and the ending | Q24 act 3 and the ending confrontation | Plays from act 3 to the end, two endings | Plays to the end |
| **G8** Ambience | Q23 item 5: options doc first, Juha picks, build only that | Checks the picked effects can be turned off and do not break Telnet | Picks, then listens and watches |

After G7: one critic round (Ada, then Mara, fresh boards) on the whole game,
recorded in `LOOP.md`, as the fun check required by the memory rule.

## Rules for every slice

- One slice per Kiro run. Re-read this file before ticking.
- The e2e and the story lint stay green after every slice; the game is
  finishable on the deployed board at every gate.
- G1 to G3 work on today's chain. G5 to G7 change it only as G4 approved.
- Engine features go on the TinyBBS port list in `docs/PLATFORM_UX.md`.
- All caller text DRAFT for Juha; ANSI art for every new screen.
