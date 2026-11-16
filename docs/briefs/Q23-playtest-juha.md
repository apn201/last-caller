# Brief Q23 - Juha's playtest: scenes, ELIZA's login, her chat, hints

From: Claude Code (planner), from Juha's own playtest on 16 Nov 2026. To: Kiro.

Juha played the deployed board (333bf19 + 8ab7ce8). The Q22 bugs are gone and
the password in the ANSI picture works. Case-sensitivity friction is accepted.
He found the manifest easy to guess but knows the story; ignore that for now.
Four items, in this order. Follow `docs/PUZZLE_CHAIN.md` and the memory rule
in the steering: **fix friction, never name an answer outside the evidence.**

## 1. Scenes killed by the caller's own typing

Juha: most scenes play, but "sometimes they start playing just when you start
typing something and that kills them." A trigger fires (a menu key, a login,
reading a post), the scene starts, and the keys already typed or the ENTER that
follows skip the text (`scene::Player::input`: ENTER flushes typing, any key
ends a pause or `[any key]`). Q22 dropped type-ahead at the *end* of a scene;
this is the *start*.

- Juha's rule: **only Q skips a scene.** ENTER and other keys no longer flush
  the typing or end a pause. Q jumps to the scene's end and still grants its
  flags, then-target and notebook lines, as if it had played. Each scene opens
  with one dim line, `Q skips`, and the help and contract C7 say so.
- `[any key]` waits and numbered choices still take keys, but only after a
  1.5 s grace from when they appear; earlier keys are dropped.
- At scene start, drop the caller's half-typed line and any buffered keys.
- If practical, defer a triggered scene until the caller is at a calm prompt
  with nothing typed (pages already wait for this: `ghostTick`).
- The notebook replay stays as it is (it is how Juha recovered).
- Engine item: add to the TinyBBS port list in `docs/PLATFORM_UX.md`.
- e2e: a scripted caller types a line with ENTER within 200 ms of a trigger;
  the scene's last node must still appear on screen.

## 2. M7: no way to learn ELIZA's login

Juha ran the grant scene (`ELZ_GRANT`) and then had no idea how to log in
"to sysop". He tried `ANDERS`. Nothing pointed him at a *login named ELIZA* or
at the go-live checklist. The only hint he saw was "Engineering is on the list
now", which reads as a permission note, not a pointer.

Q24 plans to replace this gate with an argument with ELIZA. Until then, keep
it working and do the small fix only. Keep the answer only in `GOLIVE.TXT`
item 14. Make the way there findable:

- `ELZ_GRANT` DONE line: say that the go-live papers from Ground Ops are filed
  in Engineering (where to look; not the password, not the item number).
- `SYSOP.INI`: a keyword line for `password, login, log in, sysop account,
  your account, log on` (after M6 only: `has=SYSVW`): she says Ground Ops
  issued every password on paper at go-live and the papers are in Engineering;
  she would prefer the caller not use hers. Before M6 a different line: her
  account is not for guests.
- Check that the Archive 3 Dec post ("the operators never changed it") is
  reachable in SYSOP view and says *whose* password was never changed.
- Notes: the new lines get `clue=M7` entries; `story_lint.py` stays green.

## 3. ELIZA's free chat repeats herself

Juha: "the elisa chat is stupid. just repeating the same. Like Larry 1. It
must be more conservative." Today every unmatched line gets one fixed `ELSE1`
reply. The SysOp is called ELIZA; she should talk like one.

Juha and the planner chose (16 Nov) a **RiveScript** reader, not a 1966
ELIZA and not an LLM. RiveScript is an open, MIT-licensed text format
(rivescript.com, `aichaos/rivescript-js` as the reference). There is no C++
port: write a fresh interpreter for a subset in `src/core` (shared, no
platform headers). Her brain is on the card: `sd/BBS/STORY/ELIZA/*.rive`.

- Subset: `+` triggers with `*`, `#`, `_`, optional `[words]` and
  alternatives `(a|b)`; `-` replies picked at random without repeating the
  last one; weights; `@` redirects; `%` previous (she remembers her last
  line); `>topic` with `{topic=}` (one topic per act and for her damaged
  memory); `*` conditions on user variables, where the story flags are
  readable variables (`<get flag_SYSVW>`, read-only); `<set>`/`<get>` for
  what the caller told her (name, "my ..."), saved on the account; `<star>`;
  `! sub` substitutions and `! person` pronoun swaps; `! array`.
  No `> object` macros (no code on the card). Caps on recursion, reply
  length and brain size so it fits the ESP32; state RAM and flash in the tick.
- Order: the story lines in `SYSOP.INI` win (keys, has/lacks, once). Only an
  unmatched line goes to the brain. **Free chat never advances the game.**
  Every gate stays a numbered option in a talk scene (`ELZ_TALK`); the brain
  is flavour and nudges.
- She is aware of her role: write her brain as the ship's AI, per act, with
  her orders, her calm lies, her view of each crew member, how the memory
  damage shows. When she does not understand she says so in character and
  names what she can talk about now (the list follows the act).
- Note RiveScript (format, MIT) in `docs/CONTENT_POLICY.md`; copy no brain
  files from anywhere.
- Voice: hers, not a therapist's. Calm, polite, short, slightly wrong
  ("Why do you say the hatch is locked? It is locked on purpose."). Game words
  (crew names, ship parts, Ty, the module, the garden, NOWHERE) map to her
  story lines; never a hint that is not already in the hint table.
- Host tests: wildcards, optional words, redirects, topics, conditions on
  flags, no repeat, memory, and that a `SYSOP.INI` key still wins. A lint
  check in `story_lint.py` parses every `.rive` file.
- Engine item: the RiveScript reader goes on the TinyBBS port list.

## 4. A hint system for a stuck caller

Juha: "some hint system (when user is stuck) would be needed to keep spirits
up." The rule from the memory still holds: hints say **where to look**, never
the answer, and being stuck for a while is the game working.

- Data on the card, `sd/BBS/STORY/HINTS.INI`: per milestone (by `has=` and
  `lacks=` flags, the next unmet step in `PUZZLE_CHAIN.md`), three tiers taken
  from the hint table there: 1 a nudge (which area, which person), 2 the place
  (conference, file, date), 3 the near-answer (what to do with it, still not
  the value).
- Asked for, never pushed: a key in the notebook (`H hint`), listed only when a
  hint exists for this caller. Tier 2 opens 5 minutes after tier 1 was shown,
  tier 3 5 minutes after tier 2, counted in online time; until then the key
  repeats the last tier and says when the next one is ready.
- In-world voice: the 2026 radio club in the Radio Room (Q24 makes them the
  caller's helpers outside the ship). Mark it DRAFT for Juha. Not ELIZA (she
  lies).
- Every shown hint goes in the notebook. The e2e opens tier 1 at each
  milestone. Story lint checks every milestone M1 to M9 has three tiers.

## 5. Ambience

Juha: "we need to improve the ambience somehow... glitches on the terminal
filter? Background music (Ty's music?) as option?" Propose first, then build
what Juha picks. Write `docs/briefs/Q23-ambience-options.md` with 3 to 5 ideas,
each with cost, RAM/flash, and a short clip or PNG. At least:

- **Terminal glitches** in `shared/web/terminal.html`: a CRT layer (scanlines,
  slight bloom, a rare roll or tear), with glitches that grow with the story:
  none before the RTG, a few after M5 (her memory damaged), more near the
  ending. The board sends a mood level (story flags), the page draws it. The
  board already has `GLITCH.INI`; tie it in. Off switch in the header, kept
  per viewer; `prefers-reduced-motion` turns it off. Telnet gets nothing new.
- **Background music, opt-in**: a `MUSIC` button next to `MODEM AUDIO`, off by
  default, playing Ty's tunes (`HARBOUR.MOD` and the others on the card) with
  the MOD player in `sd/BBS/ASSETS/PLAYER`, quiet, looping. The track could
  follow the act. Browser only; never autoplay.
- **Ship sound bed**: low hum and relay clicks from `ASSETS/SFX`, opt-in like
  the music.

Juha picks; build only that, after items 1 to 4.

## Then

One critic round (Ada, then Mara, fresh boards) to confirm the fun survived
items 2 and 4; record it in `LOOP.md`. Checks as always: `pio run`, host tests,
story lint, `tools/staging-check.ps1` with the e2e finishing the game; push
`main` to `staging`. Commits prefixed `Q23:`. Tick the item in `QUEUE.md` with
what changed, RAM and flash, the staging result and the deploy commit.
