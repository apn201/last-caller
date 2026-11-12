---
inclusion: always
---

# Tech

- One C++ core. Telnet and browser are two transports into the same BBS core,
  adjacent nodes in the same session pool.
- `src/platform/platform.h` is the only seam between the core and the machine
  (`src/platform/esp32.h` on the stick, `server/platform/` on the server).
- No ESP32 or Linux headers in `src/core`, `src/bbs` or `src/doors`.
- The firmware is one translation unit split into headers. A module may use
  anything included before it in `src/main.cpp`.
- AP-only. The board never joins another network.

## Builds

- ESP32: `pio run` (profile `esp32-s3-devkitc1-n16r8`, build folder
  `.pio-tinybbs`). Flash with `pio run -t upload`.
- Server: `cmake -S server -B server/build`, then
  `cmake --build server/build`. It needs Linux and `civetweb-devel`.
- After a change in `src/core`, `src/bbs`, `src/doors` or `src/platform`,
  both builds must pass.
- This PC is Windows. The server build cannot run here, so say so in the
  report. Never claim it passed.
- Test the server against a disposable copy of the card tree, never the real
  one.

## Web pages

Edit `shared/web/terminal.html` and `shared/web/sysop.html`. Never edit the
generated `src/net/web_terminal.h` or `src/net/sysop_page.h`.
`python tools/embed_web.py` regenerates them. A hook does this on save.

`src/net/routes.h` is shared. `esp_web.h` and `server/net/http.cpp` only
adapt their web-server APIs to it.
