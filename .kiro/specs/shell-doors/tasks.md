# Tasks - Diagnostics and shell doors (queue item Q5)

Requirements: `requirements.md`. Design: `design.md`.

- [x] 1. Staging fixes: no port-8086 placeholder handling; `$LASTEXITCODE` in
  `staging-check.ps1` (R4)
- [x] 2. `src/core/shell_logic.h` and `tools/test/shell_logic_test.cpp`, run with
  `zig c++` (R2)
- [x] 3. `src/doors/diag.h`, `src/doors/shell.h`, door table lines, session
  states (R1, R2, R3)
- [x] 4. Content: `STORY/DIAG.INI`, `STORY/SHELL.INI`, door names in
  `SYSTEM.INI` (drafts)
- [x] 5. Art: `tools/ansi/screens/diag.py`, rendered and looked at (R3)
- [x] 6. `pio run` passes
- [x] 7. `e2e_telnet.py` covers both doors; `staging-remote.sh` opens the shell
  gate on the disposable card
- [ ] 8. Board test: connect to the stick, sign up, open Doors: Diagnostics is
  listed, the shell is not. Query `200`, `q 2-01`, nonsense, QUIT. Give the
  account the flag `NOWHERE_OPEN` (SysOp user editor): `N` appears; try `ls`,
  `cd etc`, `cat motd`, `exit`. Repeat in the browser.
- [ ] 9. Act 1 and act 3 content (Q7, Q9): the comms manual with the query codes,
  the haven tree, the final command, and whatever grants `NOWHERE_OPEN`
