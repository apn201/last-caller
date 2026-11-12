---
inclusion: always
---

# Structure

| Folder | Contents |
| --- | --- |
| `src/main.cpp` | `setup()`, `loop()`; includes the modules in order |
| `src/core` | Globals, helpers, SD storage, INI lookup, clock, users, message store, sessions, output pacing, the reader |
| `src/bbs` | Settings, menus, file areas, ZIP reading, viewers, uploads, messages, SysOp tools, one `cmd_*.h` per command area; `runtime.h` has `bbsBegin()` and `bbsTick()` |
| `src/doors` | One header per door plus `doors.h`, the door table |
| `src/net` | Shared HTTP routes (`routes.h`), ESP32 web binding (`esp_web.h`), file manager, Telnet; generated page headers |
| `src/platform` | `platform.h` (the contract), `esp32.h` |
| `src/hw` | LCD and splash logo |
| `server/` | Linux build: CMake project, `server/platform/`, `server/net/http.cpp` |
| `shared/web/` | Browser pages, the source of the generated headers |
| `tools/` | `embed_web.py`, `prepare-sd.ps1`, content scripts |
| `sd/BBS/` | Card source content; `FILES` and `MSGBASE` are git-ignored |
| `sd/BBS/STORY/` | Planned story tables (`*.INI`: triggers, ghosts, SysOp lines), loaded once at boot |
| `docs/` | `ROADMAP.md`, `SLICES.md`, `KIROWEEN_PLAN.md`, `HARDWARE.md`, `briefs/` |
| `.kiro/` | Steering, hooks, and later specs, agents and MCP |

Adding a door: write `src/doors/<name>.h` with `enter`, `owns` and `handle`,
include it in `main.cpp` before `doors/doors.h`, add one line to the `DOORS`
table.

The planned haunt engine is one portable header, `src/core/haunt.h`.
