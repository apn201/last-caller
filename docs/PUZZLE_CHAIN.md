# Puzzle chain - HALCYON

Q12c, from `docs/briefs/S12c-milestones.md`. Canonical story:
`.kiro/specs/story-bible/design.md`. Build: `.kiro/specs/milestones/`. Every
caller-visible line named here is a draft for the author's final edit.

Two timelines. The callers live in Nov 2026, together on a social board. The
evidence relives Sat 31 Oct to Tue 8 Dec 1998, mostly in order. A milestone
opens the next stretch of the evidence and plays a scene. Everything else can
be done in any order.

## The chain, traced back from the ending

```
M1 power ---> M2 account ---> M3 crew area ---> M4 Achterberg ---+
                  |               |                              |
                  |               +--> CMDR handbook (cure)       |  jigsaw
                  |                         |                    |  (order code)
                  +--> M5 overrun ----------+--> M6 SYSOP view    |
                                                  |              |
                                                  v              |
                                             M7 ELIZA's login    |
                                                  |              |
                                                  v              v
                                             M8 NOWHERE shell <--+
                                                  |   (needs five manifest rows)
                                                  v
                                             M9 release out-1122 ground-1
```

Hard dependencies only. M4 is not needed for M5 to M9: SYSOP view opens the
Archive, so a caller who skipped Achterberg can still read him and take the
jigsaw there.

| # | Milestone | Needs | Flag (story record) | Opens | Scene |
| --- | --- | --- | --- | --- | --- |
| M1 | Power to the RTG | `admin`/`admin`, switch the source within two minutes | `MET_ELIZA` (exists) | ELIZA wakes | `ELIZA_WAKE` |
| M2 | Own account | M1; create the account in the console | (account) | Act 1 board, 31 Oct to 22 Nov 13:51 | NEW `ELZ_WELCOME` |
| M3 | Crew area | M2; `TYVANCE` login, password = his birthday | `CREW`, NEW `ACT2` | Act 2, 22 Nov 13:52 to 1 Dec: logs, records, the other crew passwords | `TY_DESK` |
| M4 | Achterberg's archive | M3; `CHAOS` login, password `LADDER` | `ACH`, NEW `ACT3` | Act 3, 2 Dec onward; his last message; the jigsaw | `MEM_REC` |
| M5 | ELIZA's memory damaged | M2; post a body with 255 zeros in a row | NEW `OVR` | ELIZA's lines change | NEW `MEM_GLITCH` |
| M6 | SYSOP view | M5 and the handbook page (CMDR login, so M3); reply 3 in the SysOp chat | NEW `CURE`, NEW `SYSVW` | Everything, every crew mailbox, the Sponsor's orders in full | NEW `ELZ_GRANT` |
| M7 | ELIZA's account | M6; her password from the go-live checklist | NEW `ELZ_IN` | Her door menu, her mail, her ops log | NEW `ELZ_OPS` |
| M8 | NOWHERE shell | M7; the order code (jigsaw) typed in Diagnostics | `NOWHERE_OPEN` (exists) | The haven tree, the queue, Ty's typescript | none |
| M9 | Ending | M8; five manifest rows (`M_Z` is the report on file); the command | `M_Z`, `END1` (exist) | Ending, 2026 bulletin | `MEM_BAY`, `ENDING` |

SYSOP view is a story flag that opens reading. It never grants the real
`SYSOP` flag, the SysOp console, the SysOp page, deleting or editing anything,
or any board setting. The story never sets `SYSOP`; the flag filter in
`src/core/haunt_logic.h` already refuses it, and Q12d adds a test for the new
flags.

## Hints, per milestone (at least two, in two places)

| Step | Hint 1 | Hint 2 | Hint 3 |
| --- | --- | --- | --- |
| M1 | Boot banner: maintenance account, see the label | Console warning names the key | Unknown login gets one in-world line (exists) |
| M2 | WELCOME bulletin: guests read, ELIZA offers an account | The console offers it after the RTG | |
| M3 | CREW bulletin: Ty's bio with his birthday | Ty's journal: "I use my birthday for everything" | User list: `TYVANCE` "all decks"; his tune's sample names (E39) |
| M4 | Orbit 3 shown at 300 baud: the covered frame | Lounge "Locks", 25 Nov, and Achterberg's repeated "ladder" | AUDIT.LOG shows his archive made 28 Nov |
| M5 | Lounge, 6 Nov, Ty: "255 zeros", the board went away for three minutes | Archive, 2 Dec, Achterberg: the one place she cannot watch | ELIZA's reply under Ty's post: "unavailable for three minutes" |
| M6 cure | CMDR's handbook page: SYSOP mode has a memory check | Archive, 2 Dec: she must allow the check, for its duration | ELIZA, once the caller has M5: she cannot account for some minutes |
| M7 | The checklist itself, by name and date, in Engineering | ELIZA's grant line: Engineering is listed now | Archive, 3 Dec: "the operators never changed it" |
| M8 | The Founder's order pieces in the jigsaw give the code | Comms manual: queries and the code form | DIAG help: codes from the Founder's office are entered as queries (exists) |
| M9 | Achterberg's last message: the verb | Comms manual: the syntax | The queue header in the haven: the destination |

Each hint reveals where to look, never the answer. Hard cases listed in
`.kiro/specs/milestones/design.md` ("Failing gracefully").

## Side puzzles, hung on the milestone that opens them

Optional unless marked. Any order after their parent.

| Side puzzle | Parent | What the caller does | Reward |
| --- | --- | --- | --- |
| P1 Ty's birthday | M2 | Finds the date in the bio, journal and tune | M3 |
| P6 Overpainted Orbit 3 | M3 | Slows the picture to 300 baud and reads the covered frame | `LADDER` for M4 |
| P2 CMDR login (`ANDERS`) | M3 | Reads the Lounge "Locks" and Orbit 5 | Her mail (E23); NEW handbook page (E43), needed for M6 |
| P2 GARDEN login (`HANAMI`) | M3 | Reads Garden Notes 24 Nov and Orbit 6 | Her mail (E24) |
| P3 Audit log | M3 | Reads AUDIT.LOG and the warning Ty scrolled past | A-C, A-F |
| P7 Logs | M3 | Reads SUIT1, GARDEN, CAPSULE, RACK, POWER, MODULE logs and NEW BULKHEAD.LOG | H-F, T-F, V-F, Z-W |
| P4 Diagnostics | M3 | Queries by number from the comms manual | Truthful answers; the order-code query |
| P5 Jigsaw | M4 | Orders the four Archive pieces | Order code for M8 |
| NEW P10 Overrun | M2 | See M5 | M5 |
| NEW P11 Memory check | M3, M5 | See M6 | M6 |
| NEW P12 ELIZA's login | M6 | See M7 | M7 |
| Manifest | M2 (rows fill as facts are found) | Pairs of rows confirmed | `M_Z` for M9 |
| Eggs, snake, Ty's demo, modem commands | M2 | Type something odd | Colour; no flags that gate anything |
| Tanabe and Eero | M2 | Reads the Radio Room, her mail, Eero's last message (22 Nov) | The love story; his answer at the end |
| Ty's crush | M2 | Greets in .NFO files, the demo scroller | Texture |
| Halvorsen and Anders | M2 | Her log and mail | Texture, H-F |
| Achterberg and the insurers | M3 | His Lounge posts and Archive | Texture, A-I |
| The Sponsor in memos | M3 | Budget memo, Y2K memo, prospectus, standing orders in full at M6 | Z-O, Z-W |
| The 2026 radio club | M2 | Radio Room | Frame; tells the caller who they are |

## Walk-through (design target, minutes of a first-time caller)

An estimate for the design, not a measurement. Run 5 records the real figure.

| Minute | Where |
| --- | --- |
| 0 to 3 | Brownout, M1, ELIZA wakes |
| 3 to 6 | M2, welcome |
| 6 to 25 | Act 1: bulletins, user list, Lounge, Mission Log, Garden Notes, Ty's journal, Gallery; the Radio Room; the overrun post is in the Lounge (M5 can be done here) |
| 25 | M3 |
| 25 to 55 | Act 2: logs, records, Orbit 3 slow, the Lounge thread on the module, DIAG; the CMDR and GARDEN passwords |
| 55 | M4 |
| 55 to 75 | Act 3: the Archive, his last message, the jigsaw; M5 if not yet done; the handbook page (CMDR) |
| 75 | M6 (the chat), M7 (the checklist) |
| 75 to 90 | ELIZA's mail and ops log, catch-up in SYSOP view for anything missed |
| 90 to 105 | M8; the manifest; M9 |

Inside the 60 to 120 minute goal. Nothing in the chain is a wait or a
repeat. The only typed repetition is the 255 zeros of M5, and it can be pasted.

## Rules this chain keeps

- No grind: every gate opens by noticing or thinking. The zeros are a single
  action the hints explain.
- No dead end: a wrong or early reply in the chat gets a calm refusal. A caller
  who loses a crew login finds the same evidence through SYSOP view.
- Flags only grow. No story step grants `SYSOP`, a level above the caller's
  own, or a setting.
- Each fact keeps an open item and at least four items in four media (matrix in
  the story bible, with E41 to E48 added in S12c).
- Posts after 22 Nov 13:52 are behind the act gates; the audit and the one new
  small note for H-C are in the story bible, "Act gates audit".
