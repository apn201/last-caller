#!/usr/bin/env python3
"""End-to-end check of a running TinyBBS server over localhost Telnet.

Standard library only. Run it on the host that runs the server, against a
DISPOSABLE card that starts in brownout (no DATA/STATE/POWER.DAT) and has a
short brownout window (window_max in STORY/POWER.INI, passed as --window-max).

    python3 e2e_telnet.py --port 2323 --window-max 8 [--crew HANDLE:PASSWORD]

Prints one PASS/FAIL/SKIP line per step and a summary; exits 0 only when no
step failed. Caller-visible wording is draft, so the markers matched here are
short and few; change them with the text.
"""
import argparse
import random
import re
import socket
import sys
import time

ANSI = re.compile(r"\x1b\[[0-9;?]*[A-Za-z]")
results = []


class Gone(Exception):
    pass


class Conn:
    def __init__(self, port):
        self.sock = socket.create_connection(("127.0.0.1", port), timeout=5)
        self.sock.settimeout(0.2)
        self.buf = ""      # text not yet matched
        self.all = ""      # everything received on this connection
        self.eof = False
        self.iac = bytearray()

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
        # Strip Telnet negotiation (IAC x y) before decoding.
        raw = bytes(self.iac) + data
        self.iac = bytearray()
        out = bytearray()
        i = 0
        while i < len(raw):
            if raw[i] == 255:
                if i + 1 >= len(raw):
                    self.iac = bytearray(raw[i:])
                    break
                verb = raw[i + 1]
                if verb == 255:
                    out.append(255)
                    i += 2
                elif verb >= 251:
                    if i + 2 >= len(raw):
                        self.iac = bytearray(raw[i:])
                        break
                    i += 3
                else:
                    i += 2
                continue
            out.append(raw[i])
            i += 1
        text = ANSI.sub("", out.decode("utf-8", "replace")).replace("\r", "")
        self.buf += text
        self.all += text

    def expect(self, *needles, timeout=10.0):
        """Wait for any needle (case-insensitive). Returns it; consumes through it."""
        end = time.time() + timeout
        while True:
            low = self.buf.lower()
            for n in needles:
                at = low.find(n.lower())
                if at >= 0:
                    self.buf = self.buf[at + len(n):]
                    return n
            if self.eof:
                raise Gone("connection closed while waiting for %r" % (needles,))
            if time.time() > end:
                raise TimeoutError("timed out waiting for %r; last text: %r" % (needles, self.buf[-300:]))
            self._pump()

    def send(self, text):
        self.sock.sendall(text.encode("utf-8") + b"\r")

    def wait_closed(self, timeout):
        end = time.time() + timeout
        while not self.eof and time.time() < end:
            self._pump()
        return self.eof

    def close(self):
        try:
            self.sock.close()
        except OSError:
            pass


def step(name):
    def wrap(fn):
        def run(*a, **k):
            try:
                note = fn(*a, **k)
                if note and note.startswith("SKIP"):
                    results.append(("SKIP", name, note[5:]))
                else:
                    results.append(("PASS", name, note or ""))
                return True
            except Exception as e:  # noqa: BLE001 - report every kind of failure
                results.append(("FAIL", name, "%s: %s" % (type(e).__name__, e)))
                return False
        return run
    return wrap


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--port", type=int, required=True)
    ap.add_argument("--window-max", type=int, default=8, help="brownout window_max in seconds")
    ap.add_argument("--shell-open", action="store_true", help="the card's SHELL.INI has needs= empty")
    ap.add_argument("--crew", default="", help="HANDLE:PASSWORD of a crew account, if one exists")
    args = ap.parse_args()
    port = args.port
    handle = "e2e%04d" % random.randint(0, 9999)
    password = "e2e-pass-1"
    state = {}

    @step("brownout banner and wrong-name refusal")
    def banner():
        c = Conn(port)
        c.expect("SOLAR BUS ONLY")
        c.expect("Login:")
        c.send("bob")
        c.expect("MAINTENANCE ONLY")
        c.expect("Login:")
        state["first"] = c
        state["first_at"] = time.time()

    @step("brownout drop after the window")
    def drop():
        c = state["first"]
        c.expect("CARRIER LOST", timeout=args.window_max + 10)
        if not c.wait_closed(10):
            raise AssertionError("connection stayed open after CARRIER LOST")
        waited = time.time() - state["first_at"]
        c.close()
        return "dropped after %.0f s" % waited

    @step("reconnect shows the banner again")
    def reconnect():
        c = Conn(port)
        c.expect("Login:")
        state["c"] = c

    @step("admin/admin opens the ship console")
    def console():
        c = state["c"]
        c.send("admin")
        c.expect("Password:")
        c.send("admin")
        c.expect("SHIP>")

    @step("ship console commands answer, nonsense is not a dead end")
    def commands():
        c = state["c"]
        for cmd, marker in (("HELP", "COMMANDS"), ("STATUS", "SHIP STATUS"), ("POWER", "POWER BUS"),
                            ("ATTITUDE", "ATTITUDE"), ("FOO", "UNKNOWN COMMAND")):
            c.send(cmd)
            c.expect(marker)
            c.expect("SHIP>")

    @step("SOURCE RTG: abort, then confirm")
    def rtg():
        c = state["c"]
        c.send("SOURCE RTG")
        c.expect("[K]", timeout=10)
        c.send("X")
        c.expect("SWITCH ABORTED")
        c.expect("SHIP>")
        c.send("SOURCE RTG")
        c.expect("[K]")
        c.send("K")
        c.expect("RTG ONLINE")
        c.expect("BUS STABLE")

    @step("ELIZA's wake-up, then the login with New")
    def wake():
        c = state["c"]
        c.expect("Good evening", timeout=20)
        c.expect("Press ENTER", timeout=20)
        c.send("")
        c.expect("(N)ew user", timeout=10)

    @step("power holds past the old drop window")
    def holds():
        c = state["c"]
        time.sleep(args.window_max + 3)
        c._pump()
        if c.eof:
            raise AssertionError("dropped although the ship is on RTG")

    @step("create a caller account and reach the main menu")
    def signup():
        c = state["c"]
        c.send("N")
        c.expect("Handle (3-16")
        c.send(handle)
        c.expect("Password (6+")
        c.send(password)
        c.expect("Real name:")
        c.send("E2E Tester")
        c.expect("Location:")
        c.send("Localhost")
        # Bulletins, then the main menu; the first scene may take the screen.
        for _ in range(6):
            hit = c.expect("Press ENTER", "Main Board Command?", "not on the crew list", timeout=30)
            if hit == "Press ENTER":
                c.send("")
            else:
                state["pending_hit"] = hit
                return
        raise AssertionError("no main menu after sign-up")

    @step("ELIZA_WAKE scene plays and returns to the main menu")
    def scene():
        c = state["c"]
        # The scene starts at a calm prompt: either the text is already here or
        # it follows the first main-menu prompt.
        if state.get("pending_hit") != "not on the crew list":
            c.expect("not on the crew list", timeout=30)
        c.expect("Where is the crew", timeout=60)
        c.send("1")
        c.expect("Please make yourself at home", timeout=90)
        c.expect("[Enter]", timeout=60)   # the scene waits for a key once the text has typed out
        c.send("")
        c.expect("Main Board Command?", timeout=30)

    @step("doors menu lists Diagnostics; the shell only when its gate is open")
    def doors_menu():
        c = state["c"]
        c.send("D")
        c.expect("Door command?", timeout=15)
        seen = c.all[-1500:]
        if "Diagnostics" not in seen:
            raise AssertionError("Diagnostics is not in the door menu")
        has_shell = "NOWHERE shell" in seen
        if has_shell != args.shell_open:
            raise AssertionError("NOWHERE shell listed=%s, expected %s" % (has_shell, args.shell_open))

    @step("Diagnostics answers a query code, refuses nonsense, QUIT returns")
    def diag():
        c = state["c"]
        c.send("I")
        c.expect("QUERY>", timeout=15)
        c.send("200")
        c.expect("Crew aboard now")
        c.expect("QUERY>")
        c.send(" 2-01 ")             # spaces and dashes do not matter
        c.expect("Oxygen")
        c.expect("QUERY>")
        c.send("99999")
        c.expect("no query with that code")
        c.expect("QUERY>")
        c.send("QUIT")
        c.expect("Door command?", timeout=10)

    @step("NOWHERE shell: ls, cat, cd, pwd, unknown command, exit")
    def shell():
        if not args.shell_open:
            return "SKIP shell gate is closed on this card (pass --shell-open)"
        c = state["c"]
        c.send("N")
        c.expect("guest@nowhere:/$", timeout=15)
        c.send("ls")
        c.expect("README")
        c.expect("etc/")
        c.expect("guest@nowhere:/$")
        c.send("cat etc/motd")
        c.expect("Your data is NOWHERE.")
        c.expect("guest@nowhere:/$")
        c.send("cd var/log")
        c.expect("guest@nowhere:/var/log$")
        c.send("cat outbound")
        c.expect("No jobs queued")
        c.expect("$")
        c.send("cat ../../nothing")
        c.expect("No such file or directory")
        c.expect("$")
        c.send("rm -rf /")
        c.expect("command not found")
        c.expect("$")
        c.send("exit")
        c.expect("Door command?", timeout=10)

    @step("leave the doors menu")
    def doors_leave():
        c = state["c"]
        c.send("Q")
        c.expect("Main Board Command?", timeout=10)
    @step("a second signed-in session does not break the first")
    def second():
        c = Conn(port)
        c.expect("(N)ew user", timeout=10)
        c.send("L")
        c.expect("Handle:")
        c.send("admin")
        c.expect("Password:")
        c.send("admin")
        c.expect("Press ENTER", "Main Board Command?", "SHIP>", timeout=20)
        c.close()
        state["c"].send("")
        state["c"].expect("Main Board Command?", timeout=10)

    @step("goodbye closes the session")
    def bye():
        c = state["c"]
        c.send("G")
        c.expect("Goodbye", "Sign off", "NO CARRIER", "Log off", timeout=10)
        if not c.wait_closed(15):
            raise AssertionError("connection stayed open after Goodbye")
        c.close()

    @step("crew account login")
    def crew():
        if not args.crew:
            return "SKIP no crew account on the card yet (pass --crew HANDLE:PASSWORD once one exists)"
        who, pw = args.crew.split(":", 1)
        c = Conn(port)
        c.expect("(N)ew user", timeout=10)
        c.send("L")
        c.expect("Handle:")
        c.send(who)
        c.expect("Password:")
        c.send(pw)
        c.expect("Press ENTER", "Main Board Command?", timeout=20)
        c.close()

    for fn in (banner, drop, reconnect, console, commands, rtg, wake, holds, signup, scene, doors_menu, diag, shell, doors_leave, second, bye, crew):
        ok = fn()
        if not ok and fn in (banner, reconnect, console, rtg, wake, signup, doors_menu):
            results.append(("SKIP", "remaining steps", "stopped after a step they depend on failed"))
            break

    width = max(len(n) for _, n, _ in results)
    for status, name, note in results:
        print("%-4s  %-*s  %s" % (status, width, name, note))
    failed = sum(1 for s, _, _ in results if s == "FAIL")
    passed = sum(1 for s, _, _ in results if s == "PASS")
    skipped = sum(1 for s, _, _ in results if s == "SKIP")
    print("E2E %s: %d passed, %d failed, %d skipped" % ("FAIL" if failed else "PASS", passed, failed, skipped))
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
