#!/usr/bin/env python3
"""Self-test of tools/mcp/bbs_caller.py against a fake board on localhost.

    python tools/test/bbs_caller_selftest.py

Starts a tiny Telnet-like server (IAC negotiation, ANSI colour, a login prompt),
drives the MCP server over stdio JSON-RPC (initialize, tools/list, tools/call)
and checks the replies. Needs no board and no network. Exit 0 = pass.
"""
import json
import os
import socket
import subprocess
import sys
import threading
import time

HERE = os.path.dirname(os.path.abspath(__file__))
SERVER = os.path.join(HERE, "..", "mcp", "bbs_caller.py")


def fake_board(listener):
    conn, _ = listener.accept()
    with conn:
        conn.sendall(bytes([255, 253, 24]) + b"\x1b[1;33mHALCYON\x1b[0m\r\nLogin: ")
        got = b""
        while b"\r" not in got:
            d = conn.recv(100)
            if not d:
                return
            got += d.replace(bytes([255, 252, 24]), b"")
        conn.sendall(b"\r\n\x1b[3")          # a colour code cut in half by the network
        time.sleep(0.4)
        conn.sendall(b"2mWelcome " + got.strip() + b"\x1b[0m\r\nMAIN> ")
        conn.recv(10)
        conn.sendall(b"\r\nNO CARRIER\r\n")


def main():
    listener = socket.socket()
    listener.bind(("127.0.0.1", 0))
    listener.listen(1)
    port = listener.getsockname()[1]
    threading.Thread(target=fake_board, args=(listener,), daemon=True).start()
    env = dict(os.environ, BBS_HOST="127.0.0.1", BBS_PORT=str(port))
    p = subprocess.Popen([sys.executable, SERVER], stdin=subprocess.PIPE, stdout=subprocess.PIPE, env=env, text=True)
    failures = []

    def rpc(i, method, params=None):
        p.stdin.write(json.dumps({"jsonrpc": "2.0", "id": i, "method": method, "params": params or {}}) + "\n")
        p.stdin.flush()
        return json.loads(p.stdout.readline())

    def check(cond, what):
        print(("PASS  " if cond else "FAIL  ") + what)
        if not cond:
            failures.append(what)

    r = rpc(1, "initialize", {"protocolVersion": "2024-11-05", "capabilities": {}, "clientInfo": {"name": "t", "version": "1"}})
    check(r["result"]["serverInfo"]["name"] == "bbs-caller", "initialize names the server")
    p.stdin.write(json.dumps({"jsonrpc": "2.0", "method": "notifications/initialized"}) + "\n")
    p.stdin.flush()
    r = rpc(2, "tools/list")
    names = sorted(t["name"] for t in r["result"]["tools"])
    check(names == ["bbs_connect", "bbs_disconnect", "bbs_read", "bbs_send"], "tools/list: %s" % names)
    r = rpc(3, "tools/call", {"name": "bbs_connect", "arguments": {"quiet_ms": 300}})
    text = r["result"]["content"][0]["text"]
    check("HALCYON" in text and "\x1b" not in text and "Login:" in text, "connect: colours and Telnet bytes removed")
    r = rpc(4, "tools/call", {"name": "bbs_send", "arguments": {"text": "ty", "quiet_ms": 900}})
    text = r["result"]["content"][0]["text"]
    check("Welcome ty" in text and "\x1b" not in text, "send: reply to a typed line")
    r = rpc(5, "tools/call", {"name": "bbs_send", "arguments": {"text": "Q", "enter": False, "quiet_ms": 300}})
    text = r["result"]["content"][0]["text"]
    check("NO CARRIER" in text and "connection closed" in text, "send: closed connection is reported")
    r = rpc(6, "tools/call", {"name": "bbs_nothing", "arguments": {}})
    check(r["result"]["isError"] is True, "unknown tool is an error, server stays up")
    r = rpc(7, "tools/call", {"name": "bbs_send", "arguments": {"text": "x"}})
    check("not connected" in r["result"]["content"][0]["text"] or "closed" in r["result"]["content"][0]["text"], "send after close is refused")
    p.stdin.close()
    p.wait(timeout=5)
    print("bbs_caller_selftest: %d failed" % len(failures) if failures else "bbs_caller_selftest: PASS")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
