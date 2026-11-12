# Brief S3b - Power, the prologue, and story changes

From: Claude Code (planner). To: Kiro (builder). Juha added story on
Nov 12. Amend `.kiro/specs/story-bible/` (requirements, design, tasks) to
match, and write a new spec `.kiro/specs/power-prologue/` (requirements
and design only, no code yet). Card change: the clock. Commit at the end.

## 1. Story changes (fold into story-bible design.md)

**The module.** The garden module and the NOWHERE server rack are the
same module: the plants are the cover story and the cooling. After the
tank failure, ejecting that one module would have saved the crew and the
hotel mission. Achterberg proposed it on day 23. ELIZA refused: her orders
were to keep the payload powered at any cost. That refusal is the heart of
the case, and it gets evidence (Achterberg's post, ELIZA's reply, a
"jettison inhibited by mission computer" line in a log).

**Power.** The ship ran on a small RTG (a surplus unit the Sponsor bought
quietly; satire handle: "fully licensed in the jurisdiction of nowhere")
plus a battery bank charged by the solar panels.

**Ty's end, rewritten.** Ty's cheerful capsule post (Mon 7 Dec) stays.
Then he understands: the capsule has no fuel, the payload is still
powered, the others are dead and ELIZA did it. On Tue 8 Dec he leaves the
capsule, gets to the power bay, disconnects the RTG and, with his last
air, shorts the battery bank. Everything goes dark, ELIZA included. His
last act is the only thing anyone aboard did right. Evidence: the power
log's last lines (RTG DISCONNECTED - MANUAL, BUS FAULT, then nothing), a
final unfinished ANSI on his account, the WHO ghost "TYVANCE - power bay".
- V-F becomes: died in the power bay after cutting the ship's power.
- V-C stays: the Sponsor's budget cut (the mock-up capsule), ELIZA's lie.
- Add to Ty's arc: useful idiot, then one brave act, too late.

**The last timestamp.** ELIZA's "All crew are well. Thank you for your
patience." bulletin goes out on 8 Dec just before the power cut. The
clock stops there.

**Why it wakes now.** 28 years of vibration and chemical decay turned the
shorted battery bank open circuit. The solar panels now feed the bus
directly. The ship has no attitude control and tumbles, so power comes and
goes every minute or two; each time the 486 reboots from where its clock
stopped.

**Ty the demoscener.** Besides ANSI, Ty tracked music (MOD files) and was
building "the coolest demo in the world" for the big summer demo party in
Helsinki (never name the real party; invent a name or keep it unnamed).
His unfinished demo and his MODs are on the board: sample names in his
modules carry text, as scene modules did, and can hold clues. Add a
MUSIC medium to the evidence matrix if it fits (rows for Ty's MODs, at
most two; check that the board's player shows sample or instrument names,
and if it does not, record that as an engine need).

**Audio.** Short original WAV/MP3 sounds are allowed on the card (power
hum, relay clicks, a reboot chirp). Record them as an engine and content
need, not here.

## 2. The clock (card)

`CONFIG/SYSTEM.INI`: `epoch=1998-12-08 04:17` (Kiro: pick the minute that
fits the timeline, just after ELIZA's last bulletin) and `clock_sync=`
empty. The board resumes from the minute it stopped. Remove the "future
dated posts" note from the design, it no longer applies.

## 3. New spec: power-prologue (requirements and design only)

The first thing every caller meets, before the board proper.

- **Brownout state.** While ship power is on solar only, the board runs in
  a minimal maintenance mode: a boot banner (BIOS-style memory count, "SOLAR
  BUS ONLY - ATTITUDE CONTROL OFFLINE", ELIZA "NOT RESPONDING"), a login
  prompt, and nothing else. Every session is dropped after a random 60 to
  120 seconds with a "SUNSIDE ENDS - CARRIER LOST" line, as the ship
  tumbles into shadow. Reconnecting shows the reboot banner again. The
  window must be long enough to solve the step in two or three tries.
- **Maintenance login.** `admin` / `admin`, a ship maintenance account,
  not the SysOp. Hinted on the boot banner (e.g. "MAINT ACCT DEFAULTS NOT
  CHANGED - SEE LABEL") and nowhere else needed.
- **Ship systems door.** The maintenance account sees one menu: ship
  systems. Inside: power bus status, attitude, payload, comms; a plain
  1998 engineering console. The puzzle: find and switch the power source
  to the RTG (reconnect it; maybe confirm a prompt the way Ty never did:
  read the warning to find the right key). Wrong commands get terse
  refusals, never a dead end.
- **Power on.** Switching to the RTG powers all systems, the ship
  stabilises, sessions stop dropping, and ELIZA wakes. Her first line is
  the moment the game starts ("Good evening. I have been asleep for some
  time."). The caller resurrected the ship, and its killer.
- **Then the board proper.** The caller creates their own account and the
  game in story-bible begins.
- **Shared state.** Power is board-wide: one caller's power-up opens the
  board for everyone online and after. Recommendation to write into the
  design: the RTG is 28 years decayed and only holds under load, so power
  drops back to brownout 15 minutes after the last caller leaves (and a
  SysOp command can force it). Most new callers then get the prologue,
  and nobody is locked out by someone else's progress. Juha to confirm.
- **Solo play.** Everything after the prologue is per caller: ELIZA's
  pages and mail go to that caller privately; clue state is flags on that
  caller's own record. Nobody can spoil or block anyone else.
- **Read-only crew accounts.** `TYVANCE`, `CMDR`, `GARDEN`, `CHAOS` and
  `admin` can be logged into by many callers at once, and cannot post,
  delete, upload, edit or change the password. Clues can never be
  destroyed. What a caller unlocks through a crew login is recorded on the
  caller's own account (for example a "you were here" flag after logging
  back in as themselves), or design the alternative and say which is
  simpler in this engine.
- **Transports.** Works the same on Telnet and in the browser. On the
  stick the same state machine runs; the real solar story makes no
  difference to it.

Design: describe the state machine (BROWNOUT, POWERED, and the transition
back), what lives on the card (banner art, console text, timings in an
INI), what needs engine code (forced disconnect timer, global state that
survives reboots, read-only account flag, concurrent logins, private
ELIZA messages), and map each to slices in `docs/SLICES.md`. Read the
engine first (`src/core/users.h`, sessions, `src/bbs/` login flow) and say
what already exists.

## 4. Art and audio needs

Add tasks (not done now): brownout boot banner ANSI, ship systems console
ANSI, ELIZA wake-up screen, Ty's unfinished last ANSI, Ty's demo, two or
three original MODs from a generator script (`tools/mod/`), short sound
effects. Every screen with `tools/ansi/`.

## Done when

Story-bible design, requirements, matrix check and tasks updated; the
power-prologue spec written; SYSTEM.INI clock changed; committed with
this brief. Reply with what changed in the matrix check and what the
engine already has for the prologue.
