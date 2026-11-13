#!/usr/bin/env python3
"""Create the SYSOP account on a fresh HALCYON board, over the loopback Telnet port.

On a new card the first account named SYSOP becomes the level-255 SysOp, and
accounts are made in the ship console (log in as the maintenance account,
switch to the RTG, CREW). This walks that path once, then logs in with the new
account to prove the password works. Run it before the board is public.

    SYSOP_PASSWORD=... python3 bootstrap-sysop.py --port 2327

Standard library only. Exit 0 = the account exists and logs in.
"""
import argparse
import os
import re
import socket
import sys
import time

ANSI = re.compile(r"\x1b\[[0-9;?]*[A-Za-z]")


class Conn:
    def __init__(self, port):
        self.sock = socket.create_connection(("127.0.0.1", port), timeout=5)
        self.sock.settimeout(0.2)
        self.buf = ""
        self.eof = False

    def _pump(self):
        try:
            data = self.sock.recv(4096)
        except socket.timeout:
            return
        except OSError:
            self.eof = True
            return
        if not data:
            self.eof = True
            return
        out = bytearray()
        i = 0
        while i < len(data):  # drop Telnet negotiation (IAC verb option)
            if data[i] == 255 and i + 1 < len(data):
                i += 3 if data[i + 1] >= 251 else 2
                continue
            out.append(data[i])
            i += 1
        self.buf += ANSI.sub("", out.decode("utf-8", "replace")).replace("\r", "")

    def expect(self, *needles, timeout=20.0):
        end = time.time() + timeout
        while True:
            low = self.buf.lower()
            for n in needles:
                at = low.find(n.lower())
                if at >= 0:
                    self.buf = self.buf[at + len(n):]
                    return n
            if self.eof:
                raise RuntimeError("connection closed while waiting for %r" % (needles,))
            if time.time() > end:
                raise RuntimeError("timed out waiting for %r; last text: %r" % (needles, self.buf[-300:]))
            self._pump()

    def send(self, text):
        self.sock.sendall(text.encode("utf-8") + b"\r")

    def close(self):
        try:
            self.sock.close()
        except OSError:
            pass


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--port", type=int, required=True, help="the board's loopback Telnet port")
    ap.add_argument("--handle", default="SYSOP")
    args = ap.parse_args()
    password = os.environ.get("SYSOP_PASSWORD", "")
    if len(password) < 6:
        print("SYSOP_PASSWORD must be set and at least 6 characters", file=sys.stderr)
        return 2

    c = Conn(args.port)
    try:
        c.expect("Login:")
        c.send("admin")
        c.expect("Password:")
        c.send("admin")
        c.expect("SHIP>")
        c.send("SOURCE RTG")
        c.expect("[K]")
        c.send("K")
        c.expect("TYPE CREW", timeout=30)
        c.expect("SHIP>")
        c.send("CREW")
        c.expect("Handle (3-16")
        c.send(args.handle)
        hit = c.expect("Password (6+", "Handle unavailable")
        if hit != "Password (6+":
            print("the handle %s is unavailable: the board is not fresh" % args.handle, file=sys.stderr)
            return 3
        c.send(password)
        c.expect("Real name:")
        c.send("SysOp")
        c.expect("Location:")
        c.send("Low Earth Orbit")
        c.expect("Log in with it")
        c.expect("Login:")
        c.send(args.handle)
        c.expect("Password:")
        c.send(password)
        hit = c.expect("Press ENTER", "Main Board Command?", "Bad password", "Unknown caller", timeout=40)
        if hit in ("Bad password", "Unknown caller"):
            print("the new account did not log in", file=sys.stderr)
            return 4
    except RuntimeError as e:
        print("bootstrap failed: %s" % e, file=sys.stderr)
        return 1
    finally:
        c.close()
    print("SysOp account %s created and logged in" % args.handle)
    return 0


if __name__ == "__main__":
    sys.exit(main())
