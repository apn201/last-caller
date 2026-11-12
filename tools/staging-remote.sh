#!/usr/bin/env bash
# Runs ON the staging host, from tools/staging-check.ps1 (or by hand).
# Builds the server from the pushed `main`, checks it against a disposable
# card and prints a PASS/FAIL summary. Everything stays under $HOME; the
# server binds 127.0.0.1 only (HTTP 8086, Telnet 2323 by default).
set -uo pipefail

HTTP_PORT="${HTTP_PORT:-8086}"
TELNET_PORT="${TELNET_PORT:-2323}"
WINDOW_MIN=4
WINDOW_MAX=8
STAGE="$HOME/lastcaller-staging"
REPO="$HOME/last-caller.git"
SRC="$STAGE/src"
CARD="$STAGE/card"
LOG="$STAGE/server.log"

declare -a SUMMARY
FAILED=0
note() { SUMMARY+=("$1  $2"); [ "$1" = "FAIL" ] && FAILED=1; }
say() { printf '\n== %s\n' "$*"; }

PLACEHOLDER_RESTORE=""
SERVER_PID=""
cleanup() {
  if [ -n "$SERVER_PID" ] && kill -0 "$SERVER_PID" 2>/dev/null; then
    kill "$SERVER_PID" 2>/dev/null
    for _ in $(seq 1 50); do kill -0 "$SERVER_PID" 2>/dev/null || break; sleep 0.1; done
    kill -9 "$SERVER_PID" 2>/dev/null || true
  fi
  if [ -n "$PLACEHOLDER_RESTORE" ]; then
    # Put back the placeholder that held the port before the run.
    (cd "$STAGE" && setsid -f python3 -m http.server "$HTTP_PORT" --bind 127.0.0.1 >/dev/null 2>&1 </dev/null)
  fi
}
trap cleanup EXIT

finish() {
  say "SUMMARY"
  printf '%s\n' "${SUMMARY[@]}"
  if [ "$FAILED" -eq 0 ]; then echo "STAGING CHECK: PASS"; exit 0; fi
  echo "STAGING CHECK: FAIL"; exit 1
}

# 1. Source: the pushed main.
say "checkout"
mkdir -p "$SRC" || exit 2
cd "$SRC" || exit 2
[ -d .git ] || git init -q
if git fetch -q "$REPO" main && git checkout -q -f FETCH_HEAD; then
  note PASS "checkout $(git rev-parse --short HEAD)"
else
  note FAIL "checkout from $REPO"; finish
fi

# 2. Build.
say "build"
if cmake -S server -B server/build >/dev/null && cmake --build server/build -j2 2>&1 | tail -25; then
  if [ -x server/build/tinybbs-server ] && [ -x server/build/tinybbs-phase1-smoke ]; then note PASS "server build (cmake, g++)"; else note FAIL "server build produced no binaries"; finish; fi
else
  note FAIL "server build"; finish
fi

# 3. Fresh disposable card.
say "card"
case "$CARD" in "$HOME"/*) ;; *) echo "card path outside home"; exit 2;; esac
rm -rf "$CARD"; mkdir -p "$CARD"
cp -r "$SRC/sd/BBS" "$CARD/BBS"
for d in FILES/001_BBS_SOFTWARE UPLOADS/INCOMING DATA/INDEX; do mkdir -p "$CARD/BBS/$d"; done
rm -rf "$CARD/BBS/DATA/STATE"
sed -i "s/^window_min=.*/window_min=$WINDOW_MIN/; s/^window_max=.*/window_max=$WINDOW_MAX/" "$CARD/BBS/STORY/POWER.INI"
sed -i "s/^decay_after_idle_min=.*/decay_after_idle_min=0/" "$CARD/BBS/STORY/POWER.INI"

# 4. Phase-1 smoke check (own throwaway copy: it creates runtime files).
say "phase-1 smoke"
SMOKE="$STAGE/card-smoke"
rm -rf "$SMOKE"; cp -r "$CARD" "$SMOKE"
if "$SRC/server/build/tinybbs-phase1-smoke" "$SMOKE" admin admin 2>&1 | tail -4; [ "${PIPESTATUS[0]}" -eq 0 ]; then
  note PASS "phase-1 smoke check (admin login)"
else
  note FAIL "phase-1 smoke check"
fi
rm -rf "$SMOKE"

# 5. Port 8086: only our own placeholder may be moved aside.
say "start server"
if ss -ltn "sport = :$HTTP_PORT" | grep -q LISTEN; then
  PID="$(ss -ltnp "sport = :$HTTP_PORT" | sed -n 's/.*pid=\([0-9]*\).*/\1/p' | head -1)"
  CMD="$(tr '\0' ' ' < "/proc/$PID/cmdline" 2>/dev/null)"
  case "$CMD" in
    "python3 -m http.server $HTTP_PORT --bind 127.0.0.1 "*) kill "$PID"; PLACEHOLDER_RESTORE=1; sleep 1;;
    "$STAGE"/src/server/build/tinybbs-server*) kill "$PID"; sleep 1;;
    *) note FAIL "port $HTTP_PORT is held by something else ($CMD)"; finish;;
  esac
fi
: > "$LOG"
nohup "$SRC/server/build/tinybbs-server" --root "$CARD" --web "$SRC/shared/web" \
  --http "127.0.0.1:$HTTP_PORT" --telnet "127.0.0.1:$TELNET_PORT" >> "$LOG" 2>&1 &
SERVER_PID=$!
UP=0
for _ in $(seq 1 50); do
  if curl -s -o /dev/null -m 1 "http://127.0.0.1:$HTTP_PORT/api/board"; then UP=1; break; fi
  kill -0 "$SERVER_PID" 2>/dev/null || break
  sleep 0.2
done
if [ "$UP" -eq 1 ]; then note PASS "server started on 127.0.0.1:$HTTP_PORT (Telnet $TELNET_PORT)"; else note FAIL "server did not start"; tail -20 "$LOG"; finish; fi

# 6. HTTP.
say "http"
BOARD="$(curl -s -m 5 "http://127.0.0.1:$HTTP_PORT/api/board")"
echo "$BOARD" | head -c 300; echo
if echo "$BOARD" | grep -q 'HALCYON'; then note PASS "/api/board names the board"; else note FAIL "/api/board"; fi
CODE="$(curl -s -m 5 -o "$STAGE/page.html" -w '%{http_code}' "http://127.0.0.1:$HTTP_PORT/")"
if [ "$CODE" = "200" ] && grep -qi '<html' "$STAGE/page.html"; then note PASS "terminal page (HTTP 200)"; else note FAIL "terminal page (HTTP $CODE)"; fi
rm -f "$STAGE/page.html"

# 7. End-to-end over Telnet.
say "end-to-end"
python3 "$SRC/tools/test/e2e_telnet.py" --port "$TELNET_PORT" --window-max "$WINDOW_MAX" ${CREW:+--crew "$CREW"} | tee "$STAGE/e2e.out"
if [ "${PIPESTATUS[0]}" -eq 0 ]; then note PASS "end-to-end Telnet test ($(tail -1 "$STAGE/e2e.out"))"; else note FAIL "end-to-end Telnet test ($(tail -1 "$STAGE/e2e.out"))"; fi

# 8. Server still alive and quiet.
if kill -0 "$SERVER_PID" 2>/dev/null; then note PASS "server survived the run"; else note FAIL "server died during the run"; tail -20 "$LOG"; fi

say "stop server"
kill "$SERVER_PID" 2>/dev/null
for _ in $(seq 1 50); do kill -0 "$SERVER_PID" 2>/dev/null || break; sleep 0.1; done
if kill -0 "$SERVER_PID" 2>/dev/null; then note FAIL "server did not stop on SIGTERM"; else note PASS "server stopped on SIGTERM"; SERVER_PID=""; fi
finish
