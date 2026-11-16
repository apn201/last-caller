# Brief Q24 - More than passwords: posting, the radio club, ELIZA's orders

From: Claude Code (planner), with Juha, 16 Nov 2026. To: Kiro. After Q23.

## Why

Juha: "The whole game can not just be 'guess 3 passwords, guess 4 manifest
records, type a command'." Today six of the nine milestones end in typing a
string found somewhere (TYVANCE, CHAOS, ELIZA passwords, the order code, the
manifest picks, `release`). The verb is always *find a word, type it*.

Juha's direction: keep the BBS as the engine, no big changes. Gated events
change the world the way adventure games do: new areas (files and posts that
appear), new dialogue options, new answers. The unused verbs are **posting a
message** and **uploading a file**. The radio amateurs can **answer posts**.
A crafted post can damage ELIZA's memory. Inspiration: *The Secret of Monkey
Island* (read for structure, copy nothing):

- Several open goals at once, then a bottleneck, then the next open hub.
- Three trials in any order, each a different kind of puzzle.
- Knowledge as inventory: insults learned in fights become options in the
  final duel. Here: ELIZA's standing orders, learned one by one, become
  options in the chat where you argue with her.
- The world changes after events: the shop restocks, people say new things.

## Design first, for Juha's review

Do **not** build until Juha approves the new chain. Step 1 is a rewrite of
`docs/PUZZLE_CHAIN.md` (keep the old one as `docs/archive/PUZZLE_CHAIN_Q22.md`)
plus the story-bible changes, committed as `Q24: chain draft`. Then stop and
tick the step. The proposal below is the planner's draft; improve it.

### The draft chain

**Act 1, open:** M1 power and M2 account as now. Then three threads at once:
Ty's birthday (TYVANCE login, kept: one classic password is fair for 1998);
the Radio Room (below); the Lounge.

**The radio club (new verb: post, and get an answer).** The 2026 club posts
in the Radio Room through the gateway (they exist in act 3 today; bring them
in from M2). They are the caller's hands outside the ship: they have antennas,
callbooks, 1998 newspapers, and Eero's old logbook. A caller who posts the
right question with the right facts gets an answer **on the next pass**: the
reply appears a few minutes later, threaded under the caller's post, with a
line from ELIZA ("Messages come in on the passes"). A vague post gets a kind,
vague reply ("Which frequency? We need the pass time too."). They answer only
from what they could know in 2026, and they reveal facts the board cannot:

- R1: post the gateway frequency and a pass time (comms manual, Radio Room
  1998) -> they record the next pass and post the beacon text: ELIZA's own
  telemetry, with the bulkhead closure time. Evidence for the timeline.
- R2: post Eero's callsign -> they find his old logbook (he died in 2019 / or
  is alive, Juha decides): the 22 Nov entry. Feeds the love story and a fate.
- R3: post the Sponsor's company name -> a 1999 newspaper clipping about
  NOWHERE. Gives the Founder's memo *format* (letterhead, reference style).

They are also the hint voice of Q23 item 4 (not Ty's notes): a stuck caller
can post "help" in the Radio Room and get the next tier, in their words.

**Act 2, three trials in any order** (the Monkey Island shape), each a
different verb:

- T1 Look: the CHAOS password on Orbit 3 at 300 baud (kept as is).
- T2 Listen: the radio club's beacon (R1). Opens the ship logs of 22 Nov.
- T3 Break: damage her memory with a post. Today it is 20 zeros. Better:
  Ty's own broken post in the Lounge (6 Nov) is the line she could not read;
  quoting it back to her in a reply (or the 254-character line limit from the
  checklist, item 11, overrun on purpose) does it. Keep it one action.

**Bottleneck: ELIZA's orders (new verb: argue).** She obeys her standing
orders. Each order is a piece of knowledge found in a different place: the
Founder's memo (Office), the CMDR handbook, her own go-live note, the beacon,
the Sponsor's prospectus, her chat (she quotes one when refused). Learning an
order adds it to the notebook *and* as an option in her chat (`ELZ_TALK`,
`has=` per option). To get SYSOP view the caller sets two orders against each
other (for example: "records must be complete" plus the memory damage plus
"the Commander's handbook governs maintenance"). Wrong pairs get calm,
specific refusals that teach the rule. This **replaces M6 reply 3 and M7's
password**. The KESTREL4 line on the checklist stays, ticked: she changed it,
as the checklist told her to ("I changed it. Item 14 said to.").

**Act 3: forge an order (new verb: write).** In SYSOP view the Founder's
Office conference opens. A post there in the Founder's format (from R3 and the
memos: reference number, the standing-order wording, signed by the Founder)
with the right instruction makes her act: she opens the NOWHERE shell to the
caller. Replaces the jigsaw code (keep the jigsaw as the source of the
reference number, so its work is not lost). Browser callers may instead upload
the memo as a text file to the Office file area; posting is the one way that
works on every transport, so it is the required one.

**Ending:** the manifest as now (option C), then `release` turns into a short
confrontation in the shell: she closes bulkheads, the caller answers with an
order she must obey, as Ty did. Three or four steps, no timer.

Count: passwords TYVANCE and CHAOS only. Posting three or four times, two
dialogues with options that grow, one written forgery, one deduction.

## Engine (small, after approval)

1. **`event=post`** in `TRIGGERS.INI`: `conf=`, `match=` words in the body or
   subject (all of `match_all=`, any of `match_any=`, case and punctuation
   ignored), `min_len=`/`max_len=`, and the usual has/lacks/set/page/mail/post/
   scene. Move the overrun to it if it fits; keep the near-miss line.
2. **Delayed, threaded replies**: `reply_from=`, `reply_body=`,
   `reply_after=` minutes of the caller's online time (or `next_login=1`),
   posted as a reply to the caller's own post. Pending replies are saved on the
   account so a logoff does not lose them. Public posts are visible to all:
   decide (and document) whether a reply is public or private mail; private is
   safer on the shared board.
3. **`event=upload`** (browser only, never required): area, file name and
   text words as for posts.
4. **Notebook "orders" kind** and scene options gated by notebook entries
   (scene `has=` already works with flags; an order learned sets a flag).
5. Tests: host test for the matcher; e2e plays the new chain to the end.
6. TinyBBS port list: `event=post`, delayed replies and `event=upload` are
   engine features worth porting.

## Rules kept

- No grind, no waiting longer than a "pass" (at most 3 minutes, and the
  caller can read meanwhile).
- Every post a caller must write has at least two hints saying what goes in
  it, and a wrong post gets a reply that says what is missing, never the
  value.
- Posts written by callers are public on a shared board: a solved post would
  spoil it for the next caller. Story posts go to a private copy (the caller's
  own thread, hidden from others) or the matching conference is per-caller.
  Solve this in the design.
- All caller text DRAFT for Juha; ANSI art for every new screen.

Commits `Q24:`. Step 1 only (the chain draft) in the first run.
