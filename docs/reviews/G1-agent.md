# G1 - agent playtest (16 Nov 2026)

Run by the `playtester` agent on staging (deploy 8a02528, before the two fixes below; f52ebd6 has them). The agent hit its turn limit and did not write this file; Kiro wrote it from the agent's report. About 230 tool calls, 45 minutes.

## Result

Not finished. The agent stood in the NOWHERE shell as ELIZA (KESTREL4) with all five manifest rows filed and confirmed on the first try. It never typed `release out-1122 ground-1`, so no ending.

## G1 checks

1. **Wrong passwords.** TYVANCE answered in three different lines in turn, never the same twice in a row (ELIZA's page came on the third). `hanami` at GARDEN gave `ACCESS DENIED. CAPS LOCK OFF?`; `HANAMI` opened it. At ELIZA, `kestrel4` then `Kestrel4` got the same caps line twice: fixed in f52ebd6 (`caps_denied_2`, `_3`). Lower-case user names work.
2. **The warning line.** Read as flavour on first sight, understood only after the denial. By rule 3 it is never true or false, so it stays; the agent's suggestion to say "passwords are upper case" is not taken.
3. **Repeats.** Chat option 3 three times: three different replies. Diagnostics `200` three times: plain, "I have answered that", "You have asked that three times". The authorisation code twice: clear refusal. Shell not tested.
4. **Type-ahead.** `zzz` + ENTER after a scene: one key ended the scene and the rest reached the menu ("Unknown command"). Fixed in f52ebd6. A scene at MEMCHK dropped typed keys cleanly with the notice.

## Frustration log

| Seen | Calls | What helped |
| --- | --- | --- |
| "TYVANCE has access to all decks", no way in | about 12 | Lounge #23 "Six digits" |
| Achterberg's "inside a picture" (Lounge #38) | about 10 | ORBIT_3 slow mode (`S`): KEY CORRIGENDUM |
| Halvorsen's account, no clue | 3 | A guess, ANDERS (the agent found no stated clue) |
| ELIZA's refusal (Archive #111 "read the handbook") | about 25 | Handbook 9.4, COMMS.TXT line limit, 20 zeros, chat option 1 |
| ELIZA's password | about 5 | She said "Engineering"; GOLIVE.TXT |
| Ty's manifest row | 1 | A guess |

The longest stretch was about 25 calls, at the memory check. That is G3 and G4 material.

## Bugs

- Fixed here: scene key leak into the menu; same caps answer twice.
- G2: typed text refused at the first chat prompt ("Choose 1 to 3, or Q") although it says "type a reply"; chat replies arrive a turn late and out of order; the same typed question gets generic answers.
- Later: a menu letter that starts a scene is eaten; "0 new" and `N` in read-only areas; pager footer percentage (92% at "1-23/25"); ORBIT_3 slow mode looked garbled in the text capture (probably the overstrike, not checked in a terminal); a Q during the Bay 3 recording did not stop it (unclear if sent too early).

## Suggestions

- A stated clue for ANDERS (G3 ladder).
- Tell a wrong manifest caller which row is wrong (not needed on this run).