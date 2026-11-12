# Tasks - Manifest and endings (queue item Q6)

Requirements: `requirements.md`. Design: `design.md`.

- [x] 1. Spec (EARS requirements, design)
- [x] 2. `src/core/manifest_logic.h` and `tools/test/manifest_logic_test.cpp`, run
  with `zig c++` (R1, R2, R3)
- [x] 3. `src/doors/manifest.h`: screens, picks, filing, the sticky list, the goodbye
  text; hooks in session, door table, main menu, scene end, WHO, settings (R1, R2, R4)
- [x] 4. Shell `final_scene=` (R4.1)
- [x] 5. Content: `STORY/MANIFEST.INI`, scenes `MANFULL` and `ENDING`, the draft final
  command in `SHELL.INI` (drafts)
- [x] 6. Art: `tools/ansi/screens/manifest.py`, five screens plus ASCII twins,
  rendered and looked at (R5.4)
- [x] 7. `pio run` passes
- [x] 8. `e2e_telnet.py` covers the manifest (a pair confirmed, wrong rows never named,
  the ELIZA row last), the partial ending, both ending choices and the sticky WHO
  row; `staging-remote.sh` grants the evidence on the disposable card and checks the
  event log
- [ ] 9. Board test: `docs/TESTING.md`, "Manifest and endings", on the stick and in the browser
- [ ] 10. Evidence triggers that grant `KHI` ... `KZW`, the real final command and the 2026
  bulletin (Q7 to Q9)
