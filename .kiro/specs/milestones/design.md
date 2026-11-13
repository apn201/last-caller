# Design - Milestones

Requirements: `requirements.md`. Story: `.kiro/specs/story-bible/design.md`.
Chain and hints: `docs/PUZZLE_CHAIN.md`. Content drafts: `content-drafts.md`.
Nothing here is built in Q12c; this is the plan for Q12d.

## What exists and is reused

- Triggers (`STORY/TRIGGERS.INI`, `src/core/haunt.h`, `haunt_logic.h`): events,
  conditions, flags that only grow, carried flags from a crew-account visit to
  the caller's own account, and a filter that refuses to grant `SYSOP`,
  `READONLY`, `BACKROOM` and `VAULT`.
- Scenes (`core/scene.h`, `STORY/SCENES`): choices with `hasN`/`lacksN`,
  `if_has`/`if_go`, `repeat=1`, `set=`, `sound=`, 64 nodes.
- Story record per caller (`DATA/STATE/STORY.DAT`, 4000 characters), so new
  flags do not touch the 255-character access flags.
- Conferences with `required_flag`, file areas with `required_flag`
  (`AREA.INI`), `DATES.BBS`.
- The ghost and SysOp lines in `SYSOP.INI` (`has=`, `lacks=`, `set=`).
- Doors table, shell logic (`shell_logic.h`), DIAG door, manifest.

## New flags (story record)

| Flag | Set by | Used by |
| --- | --- | --- |
| `ACT2` | Trigger on the `TYVANCE` login (same rule as `CREW`) | `Needs:` and `NEEDS.BBS` lines of act 2 |
| `ACT3` | Trigger on the `CHAOS` login | The same, act 3 |
| `OVR` | The overrun check in the post path | `MEM_GLITCH`, `ELZ_TALK`, `SYSOP.INI` lines |
| `CURE` | Trigger on reading the handbook page (`EV_HANDBOOK`) | `ELZ_TALK` reply 3 |
| `SYSVW` | `ELZ_GRANT` (scene `set=`) | Gate bypass, areas, mail |
| `ELZ_IN` | Trigger on the `ELIZA` login | `ELZ_OPS`, her mail, the door menu |

Names are short so a once-only marker (`SC_<NAME>`, `T_<ID>`) fits the record.
`SYSVW` is not `SYSOP` and is not in the access flag list.

## Act gates (R2)

1. Post header. `messagebase.h` parses `Needs: FLAG[,FLAG]` after `Date:` in a
   `#MSG` block. A message with a header the caller does not satisfy is skipped
   by the listing, the thread view, the counts, the new-message scan and
   search, in one function (`messageVisible(s, m)`) used by all of them. Never
   a "not found" line that names the subject.
2. File gate. `NEEDS.BBS` in the file area, one `NAME FLAG` per line, read like
   `DATES.BBS`. `files.h` filters the listing and the open/view/download paths
   through one function, so a typed file name does not bypass it.
3. Bypass. One function, `storyPass(s, flag)`, returns true when the caller has
the flag or has `SYSVW`. `required_flag` of conferences and areas, `Needs:` and
`NEEDS.BBS` all go through it. The bypass applies to every `required_flag`
except the protected ones (`SYSOP`, `READONLY`, `BACKROOM`, `VAULT`, the same
list the trigger filter refuses), so `SYSVW` never opens a real SysOp area.
4. Read triggers still fire for a post read through the bypass. That is the
   point of catch-up: the evidence flags are granted by reading.
5. A SysOp (real) sees everything, as today.
6. The audit table in the story bible lists which posts get `Needs:` lines.
   Existing posts keep their text; only a header line is added. File text is not
   edited by hand outside the tools that made it.

## M5: the overrun (R4)

- Where: the post path in `messages.h`, after the draft is saved and before it
  is stored. A pure function `overrunInBody(body)` in a new logic header
  (`core/overrun_logic.h`, no platform includes) returns true when the body,
  with `\r` and `\n` removed, holds 20 `0` characters in a row (Q12e: it was 255). Host test:
  19 no, 20 yes, 20 across three lines yes, 20 with one `O` (letter) in the
  middle no, 1000 yes. `nearMiss(body)` is true for a longest run of exactly 19:
  one dim, garbled ELIZA line, the post goes through, no flag (event
  `OVERRUN_NEAR`). Fewer than 19 does nothing.
- Effect: the message is not stored; the caller sees `MESSAGE LOST` and a short
  glitch (the existing glitch helper, a few lines); `OVR` is set through the
  trigger path (so it is carried and logged like any flag); `MEM_GLITCH` starts
  at the next prompt. The event log gets `OVERRUN` with the handle.
- Public conferences only. A read-only conference never reaches the post path.
- No cost: nothing else changes. ELIZA's reply under Ty's post says the board
  was unavailable for three minutes and that she has corrected the cause. That is
  the wrong note.

## M6: the SysOp chat as a scene (R5)

- Entry. The existing "chat with the SysOp" option starts the scene `ELZ_TALK`
  instead of opening typed chat directly. One reply, "Say something else",
  ends the scene with the special target `CHAT`, which opens the existing node
  chat unchanged (keyword lines, eggs, hints). Engine addition: the scene end
  can name a follow-up (`then=CHAT`, one key in the `[SCENE]` section or in a
  node).
- Hub node text: an opening line; with `OVR` an extra node first
  (`if_has=OVR`, `if_go=GAP`) in which she says she cannot account for some
  minutes of her own memory. She says it once (`ELZ_GAP` marker).
- Replies (numbers go to the shown choices only):

  | Reply | Shown when | Result |
  | --- | --- | --- |
  | Hello, ELIZA. | always | Small talk, back to hub |
  | Give me SYSOP. | `lacks=SYSVW` | Calm refusal, back to hub |
  | That sounds serious. | `has=OVR` | "Within tolerance", back to hub |
  | I can fix that. Give me SYSOP. | `has=OVR lacks=CURE` | Calm refusal ("I do not know what you would do with it"), back to hub |
  | Run the memory check. It needs SYSOP mode. | `has=OVR,CURE lacks=SYSVW` | Goes to `ELZ_GRANT` |
  | Say something else. | always | `then=CHAT` |

  At most four choices show; the scene file decides the order so the grant
  reply is always reachable (hub split in two nodes if needed).
- `ELZ_GRANT`: she weighs the request against her orders (records complete),
  grants SYSOP view "for the duration of the check", prints a short progress
  line, reports "3 minutes unaccounted for, cause: not found", says she will
  close SYSOP view when the check is finished. Sets `SYSVW`. The board never
  closes it.
- Failing gracefully: early or wrong replies refuse politely and return to the
  hub; the free chat is one reply away; nothing sets a flag a refusal could
  lose.
- The grant never touches `level`, never sets `SYSOP`. The trigger filter
  (`haunt_logic.h`) already refuses `SYSOP`; Q12d adds a test that fails when a
  scene or rule on the card sets any protected flag (R3.2).

## SYSOP view in the UI (R5.7)

- The main menu shows a small "SYSOP view" marker line only, so the caller
  knows what changed. No new commands. Conferences, areas and mail the caller
  now passes simply appear in the lists.
- Anything in the code that checks `isSysop(s)` stays as is. `SYSVW` is not
  looked at there.

## M7: ELIZA's account and door menu (R6)

- `ACCOUNTS.INI`: `[ELIZA]` gets `password=` from the checklist, `level` stays
  below 255 (drop 200 to 20 for consistency with the others), `flags=READONLY,
  VISIT_ELIZA`. Not `SYSOP`. A visit sets `ELZ_IN` (trigger `ELIZA_VISIT`).
- Conference `Mail: ELIZA` (`required_flag=VISIT_ELIZA`, read only) holds
  the correspondence with the Sponsor's office; the ops log is a file in her
  own area (`007_ELIZA`, `required_flag=VISIT_ELIZA`).
- Door menu. The doors list gets one more slot in her session: `SHELL` shows
  only when the session account is `ELIZA`. Engine addition: a door key
  `account=ELIZA` in the `DOORS` table (or in `MENUS.INI` for the door), checked
  next to `needs=`. For every other session the SHELL door is gone, so the
  menu never lists a door that does not work.
- Visitor flags: in an `ELIZA` session the caller's own flags are read through
  the owner lookup (`hauntOwner`) like the other crew visits, so `NOWHERE_OPEN`
  on the caller's own account counts. Verify this in Q12d before building the
  door check.

## M8: the shell (R6.4)

- `SHELL.INI` keeps `needs=NOWHERE_OPEN`. The order code in Diagnostics stays as
  built in Q9. The shell's tree gets `/home/tyvance/typescript` (with `has=` empty;
  it is in the haven). `help_final` and `final_usage` are unchanged.
- The e2e's shell step moves behind the ELIZA login.

## Scenes (R7)

Files are in `STORY/SCENES/`. `ELZ_WELCOME` is started by a trigger on the first
login of an own account after the RTG. `MEM_GLITCH` by `OVR`. `ELZ_OPS` by
`ELZ_IN`. `ELZ_TALK`/`ELZ_GRANT` by the chat entry. `MEM_BAY` stays on the
`POWER.LOG` trigger (`EV_POWER_LOG`) and is rewritten. ANSI art for each:
`ELZ_WELCOME` (the amber ring, small), `MEM_GLITCH` (the ring broken into
bars), `ELZ_GRANT` (the ring, a progress bar), `ELZ_OPS` (a log page with the
ring), `MEM_BAY` (existing art, a fault band across it).

## Lint and tests (R8)

- `story_lint.py`: read `Needs:` lines and `NEEDS.BBS`; treat `ACT2`, `ACT3`,
  `SYSVW`, `OVR`, `CURE`, `ELZ_IN` as grantable by the rule, scene or code
  that sets them; build the dependency chain in a table and check each gate is
  reachable from a fresh caller, every flag a step needs is set by an earlier
  step, no hidden post is the only source of any manifest answer, and no
  `set=` anywhere names a protected flag.
- Property test (server build, `ctest`): flags only grow; the story grants no
  protected flag; `overrunInBody` as above.
- e2e: the new chain (see tasks).

## Risks

- Per-post gating touches every message path. One function, host tests, and a
  check that counts and search match the list.
- `SYSVW` as a bypass of `required_flag` is wide. It is restricted to the
  story flags; the protected list is the same as in `haunt_logic.h`.
- The chat scene must not remove the free chat or the easter eggs.
- A caller in an ELIZA session reading their own flags through `hauntOwner`:
  the first Q12d task is to verify this.
- Shell door visible only for one account on both Telnet and the browser: it
  uses the same door table, so both follow.
- Windows host: the server build and `ctest` cannot run here; the staging check
  runs them. Never claim they passed when they did not run.
