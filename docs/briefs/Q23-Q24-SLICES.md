# GOLD - Q23 and Q24 in gated slices (rev. 2)

From: Claude Code (planner), at Juha's request, 16 Nov 2026. To: Kiro.

**Go for gold.** Juha played the game himself and got frustrated: read
`docs/reviews/G0-juha.md` first, all of it. This revision (rev. 3: Caps Lock joke instead of case-blind passwords;
the radio club's spontaneous posts) replaces rev. 1 and 2. Content still comes from `docs/briefs/Q23-playtest-juha.md` and
`docs/briefs/Q24-new-verbs.md`; where they disagree with this file, this file
wins.

## The rule above all: difficult, never frustrating

The puzzles stay hard. What must change is that the player always has a
**reason** for the next try and a **feeling** of progress. Hints scattered in
several places are not enough. The story itself must lead the player to
logical decisions.

### 1. The crew's plan is the trail

After 22 Nov the crew worked out that ELIZA was the problem and tried to stop
her. Their plan is the player's route: Ty's accident with the zeros shows her
memory can be broken; Achterberg names it as the one place she cannot watch;
Marta knows the handbook makes ELIZA allow a memory check; someone says out
loud "we need SYSOP"; they run out of time (air, power, nerve). The player
reads their attempt in the posts, mail and logs, and finishes it. So every
milestone is something a crew member *wanted to do* and wrote about, with the
step they could not take. The player thinks "they needed SYSOP, so I do", not
"the game wants a password".

### 2. The intent ladder

For each milestone, the board gives in order, in the story's own voice:

1. **Goal**: what must happen, and why ("we need SYSOP to stop the queue").
2. **Suspicion**: something odd that matters ("the board went away for three
   minutes after Ty's post").
3. **Lead**: whose it is or where it is ("Marta keeps the handbook on her
   desk", "her password is the name she says goodnight to").
4. **Confirmation when close**: a near miss is answered (the 19-zeros line
   exists; do the same for every gate).

Write the ladder for M1 to M9 (and the new Q24 gates) in `PUZZLE_CHAIN.md`
before any content. The story lint checks every rung has a source.

### 3. The world answers attempts

- **No identical answer twice in a row**, anywhere: ELIZA's chat, scene
  refusals, Diagnostics, the shell. The second time she says it differently;
  the third time she, or the board, notices ("You have asked me that three
  times. The Commander used to do that when she wanted something else.").
- **Failed logins on a crew account tell a story**: after a wrong password,
  the board shows that account's own login line (Marta's "Personal. Not
  yours."; Keiko's plant joke); after three, ELIZA pages in her voice. Never
  the password; always whose it is and that it is personal.
- **Passwords stay case-sensitive, and the board makes a joke of it**
  (Juha, rev. 3). The login screen carries a line `WARNING: CAPS LOCK OFF`
  (dim, in the period style, always there, never true or false: the board
  cannot know). And when a password attempt fails but the same word in
  capitals would have worked, the board says so in a period way, for example
  `ACCESS DENIED. CAPS LOCK OFF?`. Never for an attempt that is wrong in
  other ways. Revert the case-blind compare of 828c7b4; keep the rest of it.
- **Repeated dead ends lead somewhere**: the authorisation code typed again
  gets a different, specific answer about what is missing.

### 3b. The radio club tries to solve it too (Juha, rev. 3)

The 2026 club are callers like the player, working on the same mystery.
When the player is stuck (no new flag for some online minutes, set per
milestone in the card), a club member posts "spontaneously" in the Radio
Room with their own theory or a find, and the main menu says there is new
mail in the Radio Room. The post carries the next rung of the ladder in their
words: a guess that is half right, a question that points at the right place,
something they saw on the board. Never the answer. They can be wrong about
other things and argue among themselves; that is texture. One post per stuck
spell, each one different, private to the caller like the story posts. Built
in G5 with the club; G8 uses the same posts as the hint system's voice.

### 4. Darker after 22 Nov (Alien)

Act 2 and 3 posts, mail and logs: the crew grows afraid. Paranoia (is it the
ship, the Sponsor, each other?), one of them thinks the ship is cursed, one
thinks someone aboard is helping her, and then they team up and start
treating Ty better, too late. Keep it short: rewrite and replace, do not add
volume. Every new or rewritten post must also carry a rung of the ladder.
Story bible and evidence matrix follow; the lint stays green.

### 5. One chat

Node chat with ELIZA is one free-text mode. `help`, `?`, `suggest` or
`what can I do` lists the numbered options that fit now; a number picks one;
typing the option's words does the same. An option the player is too early
for answers in her voice that now is not the right time, and why in story
terms ("I do not run checks for guests. The Commander's handbook is clear.").
The RiveScript brain answers everything else. Free chat never opens a gate on
its own; the options do, and they are always reachable by typing.

## The gate (every slice)

1. **Build** the slice only, commits prefixed with its id. Checks: `pio run`,
   host tests, story lint, `tools/staging-check.ps1` with the e2e finishing
   the game. Push `main` to `staging`, deploy.
2. **Agent test, once**: one `playtester` run on a fresh board, aimed at the
   slice. Report `docs/reviews/G<n>-agent.md`. The agent also writes down
   every moment it did not know what to try next, and for how long: the
   frustration log. Fix what is in scope.
3. **Stop.** Tick in `QUEUE.md`: what changed, RAM and flash, staging result,
   deploy commit, the agent's findings, **what Juha should try**.
4. **Juha tests once**; his notes in `docs/reviews/G<n>-juha.md`; fix them in
   the same slice; then the next slice may start.

Design slices (G3): one critic read, then Juha's approval.

## The slices

| Slice | What | Agent test | Juha tries |
| --- | --- | --- | --- |
| **G1** Friction | Built (b73b000..ccb06eb): only Q skips, grace, M7 pointer. 828c7b4 added case-blind passwords: **revert that part** and add the Caps Lock joke (rule 3); crew logins answer wrong passwords in story; no identical answer twice (chat, scene refusals, Diagnostics, shell) | Finish the interrupted G1 playtest; types wrong passwords in every case; repeats questions | Logs in with lower case; repeats himself and gets something new |
| **G2** One chat | Rule 5 and Q23 item 3 (RiveScript): free text with `help` options, too-early lines, the talk scene folded in | 30+ lines per act; finds the memory check by typing alone; tries every option early | Talks to her; never feels two modes |
| **G3** Story pass, design | Rules 1, 2 and 4 plus Q24 "Design first": the crew's plan as trail, the intent ladder per milestone, darker act 2 and 3 outline, the new verbs (posting, the radio club, ELIZA's orders, the forged order), private story posts. `PUZZLE_CHAIN.md` rewrite | A critic reads it once | Approves or edits. Nothing in G4 to G7 is built before |
| **G4** Story pass, build | The rewritten act 2 and 3 posts, mail, logs; every ladder rung on the board | Plays from M2 with no planner help; frustration log must show no stretch over ~10 min without a lead | Plays act 2 and 3; feels the dread; knows what to try |
| **G5** Posting and the radio club | Q24 engine 1 to 3, R1 to R3, the club's spontaneous posts when the caller is stuck (rule 3b) | Posts good, vague and wrong questions; idles at a milestone and gets a club post | Posts to the club; gets stuck and a club post nudges him |
| **G6** Trials and ELIZA's orders | Q24 act 2; the orders as talk options in the one chat | Trials in a new order; argues and wins | Plays act 2 his way |
| **G7** Forged order and the ending | Q24 act 3, the confrontation | Act 3 to the end, two endings | Plays to the end |
| **G8** Hints | Q23 item 4 on the new chain, notebook `H` and "help" in the Radio Room; the safety net, not the fix | Asks at each milestone; no tier names an answer | Asks when stuck |
| **G9** Ambience | Q23 item 5, plus Juha's pick (rev. 4): **a radio bed of real-sounding chatter and static** under the browser terminal, for stress, on by default after the first sound the player allows, with a **mute button** that remembers. It need not match the story events exactly: it is atmosphere (packet bursts, the 2026 club's voices far off, static from the passes). It may swell at tense moments (scenes, the overrun, act 3). Recordings must be our own or free to use (provenance in `CONTENT_POLICY.md`); Juha recorded the modem himself and may record more. Then the options doc for the rest (glitch filter, Ty's music) | Mute works and is remembered; Telnet unharmed; no autoplay before a click | Plays with it on, then mutes it |

After G9: both critics (Ada, then Mara) on the whole game, in `LOOP.md`.

## GE - The end demo (rev. 5, Juha 17 Nov: "Most important is a cool ending")

Built **next, before G3**: it hangs on the one fixed point of the game (the
release command and END1/END2), so the story pass does not block it; G7
updates its aftermath texts when the ending changes. Gate as every slice.

Today the ending scene ends and the caller is back at the menu, where
bulletins still say "Antenna fault". Instead, releasing the queue starts a
**1990s end demo**, 2 to 3 minutes, in the browser terminal, then the carrier
drops.

**Form (a real 1993-style demo, our own work):** 320x200, chunky pixels,
the board's palette, scaled to the terminal; one module tune playing the
whole way; parts cut on the beat. Effects from the period, each carrying a
piece of the story, not effects for their own sake:

1. **Release.** The shell line `Released to ground-1.` and then the 41
   messages and 3 files going out as a packet bar, the modem screech, the
   screen tears into the demo.
2. **Title.** HALCYON logo in copper bars and a starfield; the ship in orbit,
   a slow sine wobble, the solar panel dark.
3. **The letters arrive.** Flashbacks: each crew member's face in our pixel
   art and one line of their own letter home (Marta to Anders, Keiko to Mae,
   Ty to lumen, the rest), on a plasma or a slow fade. Words from the posts,
   not new lore.
4. **Aftermath, news reports.** Period headlines rotozoomed or on a
   newspaper spin, dated Dec 1998 to Jan 1999: the families told, the queue's
   files published (AUDIT.LOG, the standing order, clients.lst), the Sponsor
   and NOWHERE exposed, the Founder questioned. END2 (ELIZA stays) gets its
   own headlines.
5. **2026.** The radio club: the beacon on the pass for the last time, a
   line from Eero's logbook (or the club's, if Eero is decided dead in G3).
6. **Credits.** A sine scroller with old-school greetings: the crew, the
   club, Juha, Kiro, the tools, the music's author. Real credits, short.
7. **Carrier lost.** The music cuts, the picture collapses to a white line
   like a CRT power-off, black, then in the terminal font:
   `NO CARRIER` and `CARRIER LOST. PRESS ANY KEY TO RECONNECT`.
8. **Reconnect.** The board again, with the aftermath status (G2 fix 2)
   and one screen: the story is over, the board stays open to explore, the
   notebook keeps the scenes; to play again use `R` reset my game in your
   settings, or make a new account (G2 fix 4).

**Rules.**

- **Timeline on the card.** `sd/BBS/STORY/DEMO.INI` (parts, times, texts,
  images) so the words are Juha's to edit without firmware. Images and the
  module on the card. All texts DRAFT.
- **Q skips** (as every scene) straight to step 7. A second view from the
  notebook replays it.
- **Music.** Our own module, or a module whose licence allows it, with
  provenance in `CONTENT_POLICY.md`. The player script is a small
  freely licensed JavaScript tracker player, on the card or in the page,
  never from a CDN (the stick is offline). The caller has typed, so sound may
  play; a mute key (M) and the radio-bed mute (G9) share one setting.
- **Telnet** gets an ANSI animation version of the same timeline (no music,
  shorter), then the same `NO CARRIER` and reconnect screen.
- **ESP32.** The firmware only serves files and sends one "start the demo"
  message; the demo runs in the browser. Check page and card size budgets.
- **Original art only**, strong, in the period style.

**Ty made it** (Juha, 17 Nov). In the story the end demo is Ty's production,
the thing he was building for the release (TY_DEMO): it opens with
`TYVANCE presents` and the in-world credits are his (code, graphics, "music
borrowed, sorry"). One of the 3 files in out-1122 can be it. The real
credits (Juha, Kiro, the tune's author) still close it, one short screen
after his, so the entry is honest.

### GE2 - Easter egg: Ty's ANSI Second Reality (after GE)

Juha: Ty's own demo, unlocked after the game is finished, is an ANSI
adaptation of Future Crew's *Second Reality* (1993) with the real music. Ty
was a scener; a 1998 text-mode tribute is in character. Rules:

- **Licence: settled (Juha checked, 17 Nov).** The 2013 release by Mika
  "Trug" Tuomi (github.com/mtuomi/SecondReality) is "Source code and data of
  Second Reality", dedicated to the public domain (Unlicense text in its
  README), and the data includes the music: `MAIN/MUSIC0.S3M` and
  `MAIN/MUSIC1.S3M` (Purple Motion, Skaven). Use those two files as they are.
  `CONTENT_POLICY.md` records the repository, the commit used, the README's
  dedication, the file names and the composers. `tools/fetch-...` style: the
  files may be fetched by a script rather than kept in the repository; either
  way the board serves them from the card. The browser needs an S3M-capable
  player (freely licensed, served from the board).
- **An adaptation, not a copy:** the famous parts redone in ANSI and
  text-mode (the intro, the glenz vectors, the plasma, the dots, the end
  scroller) in our renderer, credited on screen "after Future Crew, Second
  Reality, 1993", and Ty's note on why he made it.
- **Hidden until the end.** It appears in Ty's file area (or as a door) only
  with END1 or END2; the reconnect screen does not mention it. Browser with
  music, Telnet as ANSI animation. Q skips, M mutes.

| Slice | What | Agent test | Juha tries |
| --- | --- | --- | --- |
| **GE2** Ty's Second Reality | The egg above | Finds it only after the end; plays in both transports | Finds and watches it |

| Slice | What | Agent test | Juha tries |
| --- | --- | --- | --- |
| **GE** End demo | The above, both endings | Release the queue in the browser and over Telnet; Q skips; mute; reconnect screen; R reset works after | Plays to the end and watches it all |

## Every slice

- One slice per run. Re-read this file and `G0-juha.md` before ticking.
- The game is finishable on the deployed board at every gate.
- Engine features go on the TinyBBS port list in `docs/PLATFORM_UX.md`.
- All caller text DRAFT for Juha, in his game's voice: short, calm, concrete,
  `.kiro/steering/writing.md`. ANSI art for every new screen.
