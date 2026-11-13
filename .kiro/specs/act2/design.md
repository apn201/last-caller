# Design - Act 2

## Content map

| Item | Where | Grants (TRIGGERS.INI) |
| --- | --- | --- |
| E36 Module 3, E37 Re: Module 3 | Lounge 23 Nov | `KZW` |
| Founder memo FWD: Re: OFFERING | Lounge 22 Nov | `KZO` |
| Notice, Commander Halvorsen, Fourteen, Dr. Tanabe | Lounge | `KHF` `KHC` `KTF` |
| from the seat (E08) | Lounge 7 Dec | `KVC` |
| Day 22 to Day 27 | Mission Log | `KHF` (Day 27) |
| Day 22 to Day 31 | Garden Notes | `KTF` `KTC` (Day 31) |
| Porch light (side story A) | Radio Room 22 Nov | none |
| A small job, The return capsule, Sequence 7 (E25) | Mail: TYVANCE | `KVC` `KAC` |
| the lock (E23) | Mail: CMDR | `KHF` `KHC` |
| sensor (E24) | Mail: GARDEN | `KTF` `KTC` |
| To whoever comes after, The order, Tanabe, Bay 3 (E26) | Archive conference | `KHC` `KZW` `KZO` `KTF` `KAF` `KAC` |
| SUIT1, GARDEN, AUDIT, CAPSULE, RACK, DOWNLINK.CFG, ANTENNA.RPT, MODULE, POWER | Ship Logs (`CREW`) | see TRIGGERS.INI |
| BUDGET.MEM (E28), PROSPECT.TXT (E29) | Records (`CREW`) | `KVC` `KZO`, `KZW` |
| JOURNAL2.TXT | Ty's Gallery | `KAF` `KAC` `KVC` |
| Orbit 3 to 8 (E16, E11 to E15) | Ty's Gallery | `KAI` (300 baud) `KZI` `KHF` `KTF` `KAF` `KVC` |
| Orbit 9 (E35) | Ty's Desktop (`TY_HOME`) | `KVF` `KVC` |
| FRAG_Q, V, K, D (E27) | Archive files (`ACH`) | `ORD1` to `ORD4`, `KZO` |

Conferences 8 (Mail: CMDR, `VISIT_CMDR`), 9 (Mail: GARDEN, `VISIT_GARDEN`),
10 (Archive, `ACH`), all `read_only=1`. File areas `003_LOGS`,
`004_RECORDS` (`CREW`), `005_ARCHIVE` (`ACH`), `006_TY_DESKTOP` (`TY_HOME`).

`TY_HOME` sits in the `TYVANCE` account's own flag field (ACCOUNTS.INI) and
is not set by `TY_VISIT`, so it never travels. The crew accounts also carry
the flag of the areas they open (`CREW` on TYVANCE, `ACH` on CHAOS) so the
areas show while signed in to them.

## Passwords (draft)

`CMDR` = `ANDERS`, `GARDEN` = `HANAMI`, `CHAOS` = `LADDER`. Hints: R3 of
`requirements.md`. Achterberg's Lounge post "Locks" (25 Nov) is the index;
each account has its own two further hints.

## Overstrike in the reader

`renderAnsi` (core/reader.h) records a row's ANSI just before a bare CR
draws over it (`Reader::over`, at most 48 per picture). Still pages show the
final cells. While the picture plays (`pace` in core/stream.h) each covered
row is sent first, then a bare CR, then the final row and `ESC[K`. At the
browser's 300 baud the covered row is on screen for its own length in
characters / 30 seconds. `tools/ansi/screens/act2.py` writes such a file
(`save_overpaint`). Trigger `EV_ORBIT_3_SLOW` uses `baud_max=300`.

`.gitattributes` keeps `*.ANS`/`*.ASC` byte for byte: with `text=auto` a
Linux checkout had bare LFs, which the reader draws as a staircase.

## Viewer

`.CFG`, `.RPT`, `.MEM` join the viewable text extensions.

## Scenes

`MEM_EVA` (suit radio), `MEM_GARDEN` (through the hatch), `MEM_REC`
(Bay 3), `MEM_CAPSULE` (camcorder). Art in `act2.py`, 12 rows, plain twins.
ELIZA answers exactly and never explains.

## Test

The e2e plays act 2 after act 1 (posts by number in the Lounge, by subject
elsewhere, the logs and their scenes, three crew logins with a wrong guess
each, the Archive and the jigsaw code, Ty's Desktop), then the manifest and
endings on the flags the content granted. `staging-remote.sh` no longer
adds `E2E_EVIDENCE`.
