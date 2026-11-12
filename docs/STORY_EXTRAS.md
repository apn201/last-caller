# HALCYON - extras, DRAFT 1

Draft by Claude from Juha's seeds, Nov 12 2026. Everything here is on top
of the mystery in `.kiro/specs/story-bible/`. None of it is needed to
finish the game; all of it is why people stay.

## How the board plays (decided Nov 12)

- One-off power-up. The first caller who switches the ship to the RTG
  powers it for everyone, for good. After that new callers make an
  account and walk in. (The SysOp can force brownout again for a demo.)
- Solo game, social board. The story areas (crew conferences, logs,
  crew mail, ELIZA's files) are read-only. Everything else is a normal
  1998 BBS: a public message base, node chat, WHO, pages, a user list.
  Callers trade hints there, argue, brag, post their manifests, or stay
  away if they want no spoilers. A "SPOILERS" conference and a "NO
  SPOILERS" lounge make the choice explicit.
- Your progress is yours. ELIZA talks to each caller privately.

## Side story: the unrelated love story

Draft, pick one or combine. None of it touches the mystery.

**A. The radio amateur.** In early November 1998 Keiko Tanabe's posts
reach a ham radio operator in a small Finnish town who has been listening
to HALCYON's packet relay. They write to each other through the BBS
gateway for three weeks: weather, plants, the northern lights seen from
both sides. His last message to her arrives on 22 Nov and is never
answered. In 2026 the radio amateurs who find the signal are a club; one
of their callsigns belongs to an old man. His 2026 post on the public
board is the last line of the side story. (Ties the "found by radio
amateurs" premise to a person. Very quiet, very sad.)

**B. Ty and the sysop of his art group's board.** Ty's long-distance
crush on the sysop of his group's home board, carried on in .NFO greets
and hidden messages in his art. He thinks nobody notices. His unfinished
demo's end scroller is for her. (Fits the demoscene thread, funnier and
sadder at once.)

**C. Halvorsen and the flight surgeon.** A slow, professional, never
spoken thing between the commander and the flight surgeon at mission
control, visible only in the formality of their messages and one that is
not formal at all, sent the night before launch. (The adult one.)

My vote: A as the main side story, B as Ty's texture.

## Visual novel scenes

A "scene" is a screen of Ty's portrait ANSI on top, a dialogue box below,
lines that type out with pauses, and sometimes a choice (1, 2, 3). Scenes
are card files (`STORY/SCENES/*.INI` plus art), so content is cheap once
the engine reads them. Uses:

- ELIZA conversations: her ring as the portrait, her lines, the caller's
  choices. Choices set flags (how polite you were, whether you lied to
  her) that change later lines and the ending text.
- Memory scenes: unlocked by evidence, a crew member's moment in their own
  words: Halvorsen's last EVA, Tanabe's garden at night, Achterberg
  recording his message, Ty in the capsule, Ty in the power bay.
- Side story scenes for the love story.
- The ending as a scene sequence.

## Easter eggs and nerd stuff

The board should reward people who know 1998. Rough list, Kiro picks
and adds:

- Modem: `ATH0` at any prompt hangs you up with a NO CARRIER, `+++`
  gets an OK, `ATDT` with the right number from a file dials a hidden
  node. `ATI` prints the modem's identity string.
- Commands nobody documents: `xyzzy`, the cheat code with arrow keys, a
  hidden `/fortune` with ship fortunes, ROT13 posts in the lounge.
- Typing a famous request to a famous film computer into chat gets an
  original, dry reply from ELIZA. The caller types the quote; ELIZA never
  quotes back.
- The 486's POST screen with a Turbo button state, a BIOS date, and a
  memory test that counts the wrong amount.
- A Y2K compliance memo from the Sponsor's IT ("not our problem, we will
  be in orbit").
- The hotel brochure: waterslide, zero-g bar, "rooms with a view of every
  country's tax authority, none of which can reach you".
- NOWHERE price list per megabyte per orbit, with a loyalty programme.
- Ty's demo: an ANSI animation with his MOD, unfinished, a scroller with
  greets to invented groups, crashes at the same frame every time.
- Ty's door game: a tiny ANSI game he wrote for the crew (snake in a
  space station), with a high score table of the dead crew.
- A FidoNet-style nodelist fragment, .sig files, a BOFH-style excuse
  generator in the ship's IT files (original text), a hex dump with a
  message in it, a GIF of the crew patch.
- The SysOp's own `README.1ST` from ELIZA, perfectly polite.
- Hidden ANSI by Ty only visible at 300 baud (the overpaint mechanic,
  used for jokes as well as clues).

## Marketing voice (1998)

For the README, the Devpost page and the board's own bulletins. Late
1990s games-company and WAP-portal copy, with a straight face:

> HALCYON. A real-time multi-user science fiction horror experience.
> A puzzle game like no other, powered by state-of-the-art bulletin board
> technology. Dial in. Wake the ship. Find out what happened to the crew
> of the first private spaceflight. Up to 32 simultaneous callers.
> ANSI graphics. Digital sound. Requires a modem and nerves of steel.

Words to use: real-time, multi-user, interactive, cutting-edge,
next-generation, cyberspace, online community, digital, experience.
Footnote energy: "Works on any VT100 compatible terminal or Netscape
Navigator 4."

The stick is secondary: judges play the hosted board. The stick is the
"collector's edition".
