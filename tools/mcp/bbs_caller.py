#!/usr/bin/env python3
"""bbs-caller: a caller's keyboard and screen for the board, over Telnet.

Standard library only. Three front ends over one `Caller` class:

  MCP server (stdio, JSON-RPC 2.0), for Kiro:   python tools/mcp/bbs_caller.py
  Command line, for a shell-only agent or by hand:
      python tools/mcp/bbs_caller.py cli connect
      python tools/mcp/bbs_caller.py cli send "admin"
      python tools/mcp/bbs_caller.py cli keys "K"        (no ENTER)
      python tools/mcp/bbs_caller.py cli read
      python tools/mcp/bbs_caller.py cli disconnect
    The first `cli` call starts a small background process that holds the
    connection (127.0.0.1 only) so the session lives between commands.
    `cli stop` ends it.

Where it connects: BBS_HOST (default 127.0.0.1) and BBS_PORT (default 12323: the
local end of the SSH tunnel to the staging board, see tools/staging-playtest.ps1).
It never connects anywhere else unless told so in the environment.

What it returns: the text the board sent since the last call, with ANSI colour
codes and Telnet negotiation removed, as a caller would read it. It waits until
the board has been quiet for `quiet_ms` (scenes type with pauses, so the default
is generous) or `max_ms` has passed, whichever comes first.
"""
import codecs
import json
import os
import re
import socket
import subprocess
import sys
import threading
import time

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
from telnet_wire import ANSI, strip_iac  # noqa: E402

HOST = os.environ.get("BBS_HOST", "127.0.0.1")
PORT = int(os.environ.get("BBS_PORT", "12323"))
CTL_PORT = int(os.environ.get("BBS_CTL_PORT", "12399"))
MAX_RETURN = 12000
POS = re.compile(r"\x1b\[(\d+);(\d+)H")


def place(raw):
    """Menus write their options into the art with cursor moves (ESC[row;colH).
    A terminal shows them in rows; with the moves removed they ran into one line.
    Without a screen model, each move to a new row starts a new line here, and a
    move along the same row keeps the options side by side."""
    state = {"row": None}

    def move(m):
        row = int(m.group(1))
        if row == state["row"]:
            return "  "
        state["row"] = row
        return "\n"
    return POS.sub(move, raw)


class Caller:
    def __init__(self):
        self.sock = None
        self.buf = ""
        self.eof = False
        self.iac = bytearray()
        self.raw = ""
        self.dec = codecs.getincrementaldecoder("utf-8")("replace")
        self.lock = threading.Lock()

    # ---- wire
    def _refuse(self, verb, option):
        # refuse every option politely: DO -> WONT, WILL -> DONT
        if verb in (253, 251):
            try:
                self.sock.sendall(bytes([255, 252 if verb == 253 else 254, option]))
            except OSError:
                pass

    def _pump(self, wait):
        if self.sock is None:
            return False
        self.sock.settimeout(wait)
        try:
            data = self.sock.recv(4096)
        except socket.timeout:
            return False
        except OSError:
            self.eof = True
            return False
        if not data:
            self.eof = True
            return False
        out, rest = strip_iac(bytes(self.iac) + data, self._refuse)
        self.iac = bytearray(rest)
        # Decode across chunk edges (a multi-byte character may be split) and keep the raw text:
        # colour codes are removed in _collect, once a code cannot be cut in half any more.
        self.raw += self.dec.decode(out).replace("\r", "")
        return True

    def _collect(self, quiet_ms, max_ms):
        end = time.time() + max_ms / 1000.0
        last = time.time()
        while time.time() < end and not self.eof:
            if self._pump(0.1):
                last = time.time()
            elif time.time() - last >= quiet_ms / 1000.0:
                break
        # Strip complete colour codes; hold back a code that is still arriving.
        tail = re.search(r"\x1b(\[[0-9;?]*)?$", self.raw)
        keep = ""
        if tail and not self.eof:
            keep = self.raw[tail.start():]
            self.raw = self.raw[:tail.start()]
        text = ANSI.sub("", place(self.raw))
        self.raw = keep
        self.buf = ""
        text = re.sub(r"\n{4,}", "\n\n\n", text)
        if len(text) > MAX_RETURN:
            text = "[... %d characters cut ...]\n" % (len(text) - MAX_RETURN) + text[-MAX_RETURN:]
        if self.eof:
            text += "\n[connection closed by the board]"
        return text

    # ---- tools
    def connect(self, host=None, port=None, quiet_ms=2500, max_ms=15000):
        with self.lock:
            self.disconnect()
            self.eof = False
            self.buf = ""
            self.raw = ""
            try:
                self.sock = socket.create_connection((host or HOST, int(port or PORT)), timeout=8)
            except OSError as e:
                self.sock = None
                return "[cannot connect to %s:%s: %s]" % (host or HOST, port or PORT, e)
            return self._collect(quiet_ms, max_ms)

    def send(self, text, enter=True, quiet_ms=1500, max_ms=20000):
        with self.lock:
            if self.sock is None or self.eof:
                return "[not connected]"
            data = text.replace("\\r", "\r").replace("\\n", "\r").encode("utf-8", "replace")
            if enter:
                data += b"\r"
            data = data.replace(b"\xff", b"\xff\xff")
            try:
                self.sock.sendall(data)
            except OSError as e:
                self.eof = True
                return "[send failed: %s]" % e
            return self._collect(quiet_ms, max_ms)

    def read(self, quiet_ms=1500, max_ms=20000):
        with self.lock:
            if self.sock is None:
                return "[not connected]"
            return self._collect(quiet_ms, max_ms)

    def disconnect(self):
        if self.sock is not None:
            try:
                self.sock.close()
            except OSError:
                pass
        self.sock = None
        return "[disconnected]"


# ---------------------------------------------------------------- MCP (stdio)

TOOLS = [
    {"name": "bbs_connect",
     "description": "Dial the board (new Telnet connection; any earlier one is dropped). Returns the first screen.",
     "inputSchema": {"type": "object", "properties": {
         "quiet_ms": {"type": "integer", "description": "Return after this much silence from the board (default 2500)"}}}},
    {"name": "bbs_send",
     "description": "Type a line and press ENTER (set enter=false for a single key such as a menu letter). "
                    "Returns what the board printed in reply, ANSI colours removed.",
     "inputSchema": {"type": "object", "required": ["text"], "properties": {
         "text": {"type": "string"},
         "enter": {"type": "boolean", "description": "press ENTER after the text (default true)"},
         "quiet_ms": {"type": "integer", "description": "wait for this much silence (default 1500); raise it for scenes"},
         "max_ms": {"type": "integer", "description": "never wait longer than this (default 20000)"}}}},
    {"name": "bbs_read",
     "description": "Wait and return whatever the board prints next (scenes and pages arrive on their own).",
     "inputSchema": {"type": "object", "properties": {
         "quiet_ms": {"type": "integer"}, "max_ms": {"type": "integer"}}}},
    {"name": "bbs_disconnect", "description": "Hang up.", "inputSchema": {"type": "object", "properties": {}}},
]


def call_tool(c, name, a):
    ints = {k: int(v) for k, v in a.items() if k in ("quiet_ms", "max_ms") and v is not None}
    if name == "bbs_connect":
        return c.connect(**ints)
    if name == "bbs_send":
        return c.send(str(a.get("text", "")), a.get("enter", True) is not False, **ints)
    if name == "bbs_read":
        return c.read(**ints)
    if name == "bbs_disconnect":
        return c.disconnect()
    raise KeyError(name)


def mcp_main():
    c = Caller()
    for line in sys.stdin:
        line = line.strip()
        if not line:
            continue
        try:
            msg = json.loads(line)
        except ValueError:
            continue
        mid, method = msg.get("id"), msg.get("method", "")
        if mid is None:        # a notification (initialized, cancelled ...): no answer
            continue
        if method == "initialize":
            res = {"protocolVersion": (msg.get("params") or {}).get("protocolVersion", "2024-11-05"),
                   "capabilities": {"tools": {}},
                   "serverInfo": {"name": "bbs-caller", "version": "1.0.0"}}
        elif method == "tools/list":
            res = {"tools": TOOLS}
        elif method == "tools/call":
            p = msg.get("params") or {}
            try:
                text = call_tool(c, p.get("name", ""), p.get("arguments") or {})
                res = {"content": [{"type": "text", "text": text}], "isError": False}
            except KeyError:
                res = {"content": [{"type": "text", "text": "unknown tool"}], "isError": True}
            except Exception as e:  # keep the server alive
                res = {"content": [{"type": "text", "text": "error: %s" % e}], "isError": True}
        elif method == "ping":
            res = {}
        else:
            sys.stdout.write(json.dumps({"jsonrpc": "2.0", "id": mid, "error": {"code": -32601, "message": "no such method"}}) + "\n")
            sys.stdout.flush()
            continue
        sys.stdout.write(json.dumps({"jsonrpc": "2.0", "id": mid, "result": res}) + "\n")
        sys.stdout.flush()


# ---------------------------------------------------------------- CLI (daemon holds the session)

def daemon_main():
    c = Caller()
    srv = socket.socket()
    srv.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    srv.bind(("127.0.0.1", CTL_PORT))
    srv.listen(1)
    while True:
        conn, _ = srv.accept()
        with conn:
            data = b""
            while not data.endswith(b"\n"):
                chunk = conn.recv(65536)
                if not chunk:
                    break
                data += chunk
            try:
                req = json.loads(data.decode("utf-8"))
                if req.get("tool") == "stop":
                    conn.sendall(b'{"text": "[stopped]"}\n')
                    c.disconnect()
                    return
                out = call_tool(c, req["tool"], req.get("args", {}))
            except Exception as e:
                out = "error: %s" % e
            conn.sendall((json.dumps({"text": out}) + "\n").encode("utf-8"))


def cli_request(tool, args):
    for attempt in range(2):
        try:
            with socket.create_connection(("127.0.0.1", CTL_PORT), timeout=120) as s:
                s.sendall((json.dumps({"tool": tool, "args": args}) + "\n").encode("utf-8"))
                buf = b""
                while not buf.endswith(b"\n"):
                    chunk = s.recv(65536)
                    if not chunk:
                        break
                    buf += chunk
                return json.loads(buf.decode("utf-8"))["text"]
        except OSError:
            if tool == "stop":
                return "[not running]"
            if attempt == 0:
                kw = {"creationflags": 0x00000008 | 0x00000200} if os.name == "nt" else {"start_new_session": True}
                subprocess.Popen([sys.executable, os.path.abspath(__file__), "daemon"],
                                 stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, **kw)
                time.sleep(1.0)
    return "[cannot reach the session process]"


def cli_main(argv):
    if not argv:
        print(__doc__)
        return 2
    cmd, rest = argv[0], argv[1:]
    if cmd == "connect":
        out = cli_request("bbs_connect", {})
    elif cmd == "send":
        out = cli_request("bbs_send", {"text": rest[0] if rest else "", "enter": True,
                                       **({"quiet_ms": int(rest[1])} if len(rest) > 1 else {})})
    elif cmd == "keys":
        out = cli_request("bbs_send", {"text": rest[0] if rest else "", "enter": False})
    elif cmd == "read":
        out = cli_request("bbs_read", {})
    elif cmd == "disconnect":
        out = cli_request("bbs_disconnect", {})
    elif cmd == "stop":
        out = cli_request("stop", {})
    else:
        print(__doc__)
        return 2
    sys.stdout.buffer.write((out + "\n").encode("utf-8", "replace"))
    return 0


if __name__ == "__main__":
    if len(sys.argv) > 1 and sys.argv[1] == "cli":
        sys.exit(cli_main(sys.argv[2:]))
    if len(sys.argv) > 1 and sys.argv[1] == "daemon":
        daemon_main()
    else:
        mcp_main()
