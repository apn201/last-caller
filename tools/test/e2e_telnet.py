#!/usr/bin/env python3
"""End-to-end check of a running TinyBBS server over localhost Telnet.

Standard library only. Run it on the host that runs the server, against a
DISPOSABLE card with a short power timer (timer_sec in STORY/POWER.INI, passed
as --timer). Power is per caller: every connection starts in brownout.

    python3 e2e_telnet.py --port 2323 --timer 8 [--crew HANDLE:PASSWORD]

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

def key(c, text):
    """One raw key, no Enter: the reader takes single keys."""
    c.sock.sendall(text.encode("utf-8"))


def advance(c, marker, timeout=120.0):
    """Press Enter now and then (it skips a scene's typing and pauses) until `marker` appears."""
    end = time.time() + timeout
    while True:
        try:
            return c.expect(marker, timeout=1.0)
        except TimeoutError:
            if time.time() > end:
                raise
            c.send("")


def mfill(c, row, picks):
    """Open a manifest row and fill its three blanks with pick-list numbers."""
    c.send(str(row))
    c.expect("MANIFEST>")
    for slot, pick in enumerate(picks, 1):
        c.send(str(slot))
        c.expect("MANIFEST>")
        c.send(str(pick))
        c.expect("MANIFEST>")
    c.send("Q")
    c.expect("MANIFEST>")


def mfile(c):
    """File the manifest. Returns the board's answer: everything up to the count line."""
    start = len(c.all)
    c.send("S")
    c.expect("Rows confirmed:")
    c.expect("MANIFEST>")
    answer = c.all[start:]
    at = answer.index("Rows confirmed:")
    return answer[:answer.index("\n", at)]     # the board's answer only, not the rows listed after it

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--port", type=int, required=True)
    ap.add_argument("--timer", type=int, default=8, help="power timer_sec in seconds")
    ap.add_argument("--shell-open", action="store_true", help="the card's SHELL.INI has needs= empty")
    ap.add_argument("--evidence", action="store_true", help="the card has the E2E_EVIDENCE rule (entering Diagnostics grants the manifest evidence flags)")
    ap.add_argument("--crew", default="", help="HANDLE:PASSWORD of a crew account, if one exists")
    args = ap.parse_args()
    port = args.port
    handle = "e2e%04d" % random.randint(0, 9999)
    password = "e2e-pass-1"
    handle2 = "f2e%04d" % random.randint(0, 9999)
    state = {}

    @step("front page: brownout banner, login only, unknown name refused")
    def banner():
        c = Conn(port)
        c.expect("SOLAR BUS ONLY")
        c.expect("Login:")
        c.send("bob")
        c.expect("Unknown caller")
        c.expect("Login:")
        if "(N)ew" in c.all:
            raise AssertionError("the front page offers New")
        c.close()

    @step("admin/admin starts the power timer; the session drops when it runs out")
    def drop():
        c = Conn(port)
        c.expect("Login:")
        c.send("admin")
        c.expect("Password:")
        c.send("admin")
        c.expect("SHIP>")
        began = time.time()
        c.expect("CARRIER LOST", timeout=args.timer + 10)
        if not c.wait_closed(10):
            raise AssertionError("connection stayed open after CARRIER LOST")
        waited = time.time() - began
        c.close()
        if waited < args.timer - 2:
            raise AssertionError("dropped after %.1f s, before the %d s timer" % (waited, args.timer))
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
        # No account creation before the RTG: not in HELP, not a command.
        c.send("HELP")
        c.expect("COMMANDS")
        c.expect("SHIP>")
        if "Register a crew account" in c.all:
            raise AssertionError("HELP lists CREW before the RTG")
        c.send("CREW")
        c.expect("UNKNOWN COMMAND")
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

    @step("ELIZA's wake-up, and CREW appears in the console")
    def wake():
        c = state["c"]
        c.expect("Good evening", timeout=20)
        c.expect("TYPE CREW", timeout=20)
        c.expect("SHIP>")
        c.send("HELP")
        c.expect("Register a crew account")
        c.expect("SHIP>")

    @step("the timer is stopped: the session stays past it")
    def holds():
        c = state["c"]
        time.sleep(args.timer + 3)
        c._pump()
        if c.eof:
            raise AssertionError("dropped although the RTG is on")

    @step("CREW: cancel, then create the account in the console")
    def crew_account():
        c = state["c"]
        c.send("CREW")
        c.expect("Handle (3-16")
        c.send("")
        c.expect("No account made")
        c.expect("SHIP>")
        c.send("CREW")
        c.expect("Handle (3-16")
        c.send(handle)
        c.expect("Password (6+")
        c.send(password)
        c.expect("Real name:")
        c.send("E2E Tester")
        c.expect("Location:")
        c.send("Localhost")
        c.expect("Log in with it")
        c.expect("Login:")

    @step("log in with the new account and reach the main menu")
    def signup():
        c = state["c"]
        c.send(handle)
        c.expect("Password:")
        c.send(password)
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

    # ---- act 1 (Q7): a fresh caller reads the open evidence, posts, and visits Ty's account ----
    @step("act 1: bulletins; the crew list names the crew and the manifest appears for the first time")
    def act1_bulletins():
        c = state["c"]
        c.send("B")
        c.expect("Bulletin #", timeout=15)
        seg = c.all[-900:]
        for t in ("Welcome from the SysOp", "The Crew", "Status"):
            if t not in seg:
                raise AssertionError("bulletin %r is not listed: %r" % (t, seg[-300:]))
        c.send("3")
        c.expect("Cmdr. Marta Halvorsen", timeout=15)
        c.expect("14 Mar 1974", timeout=15)
        c.expect("Bulletin #")
        c.send("4")
        c.expect("All crew are well", timeout=15)
        c.expect("Bulletin #")
        c.send("")
        c.expect("Crew manifest", timeout=15)
        c.expect("Main Board Command?", timeout=15)

    @step("act 1: the manifest shows the four rows (identity evidence only)")
    def act1_manifest():
        c = state["c"]
        start = len(c.all)
        c.send("R")
        c.expect("MANIFEST>", timeout=15)
        seg = c.all[start:]
        for h in ("CMDR", "CHAOS", "GARDEN", "TYVANCE"):
            if h not in seg:
                raise AssertionError("row %s missing" % h)
        c.send("Q")
        c.expect("Main Board Command?", timeout=15)

    @step("act 1: the user list shows the crew accounts, last calls on the 1998 clock, Ty last")
    def act1_users():
        c = state["c"]
        c.send("U")
        c.expect("to board", "Q board", timeout=15)
        seg = c.all[-1800:]
        for t in ("CMDR", "27 Nov 11:52", "CHAOS", "03 Dec 02:09", "GARDEN", "01 Dec 18:20", "TYVANCE", "08 Dec 03:47",
                  "ELIZA", "MCC", "22 Nov 14:08"):
            if t not in seg:
                raise AssertionError("user list lacks %r: %r" % (t, seg[-500:]))
        if "all decks" not in seg.lower():
            raise AssertionError("TYVANCE is not listed with access to all decks")
        c.send("Q")
        c.expect("Main Board Command?", timeout=15)

    @step("act 1: the Lounge takes a post; Mission Log offers no post or reply key")
    def act1_conferences():
        c = state["c"]
        start = len(c.all)
        c.send("M")
        c.expect("Message command?", timeout=15)
        if "post a message" not in c.all[start:].lower():
            raise AssertionError("no Post option in the Lounge")
        c.send("P")
        c.expect("To (ENTER", timeout=10)
        c.send("")
        c.expect("Subject:")
        c.send("e2e hello")
        c.expect(" 1:", timeout=10)
        c.send("Hello from a new caller.")
        c.expect(" 2:", timeout=10)
        c.send("/S")
        c.expect("posted", timeout=10)
        c.expect("Message command?", timeout=10)
        # Mission Log is a story conference: read-only for everyone.
        c.send("A")
        c.expect("Area #", timeout=10)
        if "Mail: TYVANCE" in c.all[-1500:]:
            raise AssertionError("Ty's mailbox is listed before any visit to his account")
        start = len(c.all)
        c.send("1")
        c.expect("Message command?", timeout=10)
        seg = c.all[start:]
        if "Mission Log" not in seg:
            raise AssertionError("did not enter Mission Log")
        if "post a message" in seg.lower():
            raise AssertionError("Post is offered in a read-only conference")
        c.send("P")
        c.expect("read-only", timeout=10)
        c.expect("Message command?", timeout=10)
        c.send("R")
        c.expect("Read #", timeout=10)
        start = len(c.all)
        c.send("")
        c.expect("Cmdr. M. Halvorsen", timeout=15)
        c.expect("ENTER = next", timeout=10)
        if "eply" in c.all[start:]:
            raise AssertionError("a reply key is offered in a read-only conference")
        c.send("Q")
        c.expect("Message command?", timeout=10)
        c.send("A")
        c.expect("Area #", timeout=10)
        c.send("0")
        c.expect("Message command?", timeout=10)
        c.send("Q")
        c.expect("Main Board Command?", timeout=10)

    @step("act 1: file areas list Ty's Gallery and the Library with 1998 dates; the journal opens")
    def act1_files():
        c = state["c"]
        start = len(c.all)
        c.send("F")
        c.expect("File area?", timeout=15)
        seg = c.all[start:]
        if "Ty's Gallery" not in seg or "Library" not in seg:
            raise AssertionError("file areas missing: %r" % seg[-400:])
        c.send("1")
        c.expect("File command?", timeout=15)
        seg = c.all[start:]
        for t in ("JOURNAL.TXT", "ORBIT_1.ANS", "ORBIT_2.ANS", "11-04-98", "11-13-98", "11-21-98"):
            if t not in seg:
                raise AssertionError("gallery listing lacks %r" % t)
        c.send("1")
        c.expect("SPACE JOURNAL", timeout=15)
        c.expect("N/P file", timeout=10)
        time.sleep(0.4)
        key(c, "q")
        c.expect("File command?", timeout=15)
        c.send("2")
        c.expect("N/P file", timeout=15)
        time.sleep(0.4)
        key(c, "q")
        c.expect("File command?", timeout=15)
        c.send("Q")
        c.expect("File area?", timeout=10)
        c.send("2")
        c.expect("File command?", timeout=15)
        seg = c.all[-1500:]
        for t in ("COMMS.TXT", "README.1ST", "10-28-98"):
            if t not in seg:
                raise AssertionError("library listing lacks %r" % t)
        c.send("1")
        c.expect("QUERY CODES", timeout=15)
        c.expect("N/P file", timeout=10)
        time.sleep(0.4)
        key(c, "q")
        c.expect("File command?", timeout=15)
        c.send("Q")
        c.expect("File area?", timeout=10)
        c.send("Q")
        c.expect("Main Board Command?", timeout=10)

    @step("act 1: Ty's crew account (the birthday opens it): his desk scene, his mailbox, read-only")
    def act1_crew_visit():
        c = state["c"]
        c.send("G")
        if not c.wait_closed(20):
            raise AssertionError("connection stayed open after Goodbye")
        c.close()
        c = Conn(port)
        state["c"] = c
        c.expect("Login:", timeout=10)
        c.send("TYVANCE")
        c.expect("Password:")
        c.send("000000")                   # a wrong guess first: the birthday is not 000000
        c.expect("Bad password")
        c.expect("Login:")
        c.send("TYVANCE")
        c.expect("Password:")
        c.send("140374")
        advance(c, "Main Board Command?", timeout=150)
        if "you are not ty" not in c.all.lower():
            raise AssertionError("Ty's desk scene did not play")
        start = len(c.all)
        c.send("M")
        c.expect("Message command?", timeout=15)
        if "post a message" in c.all[start:].lower():
            raise AssertionError("a read-only crew account is offered Post")
        c.send("A")
        c.expect("Area #", timeout=10)
        if "Mail: TYVANCE" not in c.all[-1500:]:
            raise AssertionError("Ty's mailbox is not listed on his own account")
        c.send("3")
        c.expect("Message command?", timeout=10)
        c.send("R")
        c.expect("Read #", timeout=10)
        c.send("")
        c.expect("you are in SPACE", timeout=15)
        c.expect("ENTER = next", timeout=10)
        c.send("Q")
        c.expect("Message command?", timeout=10)
        c.send("Q")
        c.expect("Main Board Command?", timeout=10)

    @step("act 1: Goodbye from the crew account, sign in as yourself: the visit is carried, the mailbox is open")
    def act1_back():
        c = state["c"]
        c.send("G")
        c.expect("Login:", timeout=20)
        c.send(handle)
        c.expect("Password:")
        c.send(password)
        for _ in range(8):
            hit = c.expect("Press ENTER", "Main Board Command?", timeout=40)
            if hit == "Main Board Command?":
                break
            c.send("")
        start = len(c.all)
        c.send("M")
        c.expect("Message command?", timeout=15)
        c.send("A")
        c.expect("Area #", timeout=10)
        if "Mail: TYVANCE" not in c.all[start:]:
            raise AssertionError("the visit was not carried to the own account: Ty's mailbox is closed")
        c.send("")
        c.expect("Message command?", timeout=10)
        c.send("Q")
        c.expect("Main Board Command?", timeout=10)

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
        c.expect("etc/")
        c.expect("readme")
        c.expect("var/")
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

    # ---- the manifest (S8): needs --evidence (and --shell-open for the endings) ----
    need = "SKIP the card has no E2E_EVIDENCE rule (pass --evidence)"

    @step("main menu: no manifest option before any evidence")
    def manifest_hidden():
        if not args.evidence:
            return need
        c = state["c"]
        start = len(c.all)
        c.send("")
        c.expect("Main Board Command?", timeout=15)
        if "Crew manifest" in c.all[start:]:
            raise AssertionError("the manifest is offered before the caller has any evidence")

    @step("main menu offers the manifest once there is evidence")
    def manifest_menu():
        if not args.evidence:
            return need
        c = state["c"]
        c.send("")
        c.expect("Crew manifest", timeout=15)
        c.expect("Main Board Command?")

    @step("manifest: four crew rows, no ELIZA row, notebook lists what was found")
    def manifest_open():
        if not args.evidence:
            return need
        c = state["c"]
        start = len(c.all)
        c.send("R")
        c.expect("MANIFEST>", timeout=15)
        seg = c.all[start:]
        for h in ("CMDR", "CHAOS", "GARDEN", "TYVANCE"):
            if h not in seg:
                raise AssertionError("row %s missing" % h)
        if "[5]" in seg:
            raise AssertionError("the ELIZA row is shown before the four crew rows are confirmed")
        c.send("N")
        c.expect("NOTEBOOK")
        c.expect("The commander")
        c.expect("MANIFEST>")
        c.send("Q")
        c.expect("MANIFEST>")

    @step("manifest: one right row and one wrong row confirm nothing and name nothing")
    def manifest_wrong():
        if not args.evidence:
            return need
        c = state["c"]
        mfill(c, 1, (1, 1, 1))      # CMDR, right
        mfill(c, 2, (2, 2, 5))      # CHAOS, last blank wrong
        answer = mfile(c)
        if "Rows confirmed: 0 of 4" not in answer:
            raise AssertionError("expected 0 of 4, got %r" % answer[-120:])
        for word in ("CMDR", "CHAOS", "locked", "Confirmed"):
            if word in answer:
                raise AssertionError("the answer mentions %r: %r" % (word, answer[-160:]))

    @step("manifest: two right rows are confirmed together, the wrong one is not named")
    def manifest_pair():
        if not args.evidence:
            return need
        c = state["c"]
        mfill(c, 3, (3, 3, 3))      # GARDEN, right
        answer = mfile(c)
        if "CMDR and GARDEN" not in answer or "Rows confirmed: 2 of 4" not in answer:
            raise AssertionError("expected CMDR and GARDEN, 2 of 4; got %r" % answer[-160:])
        if "CHAOS" in answer:
            raise AssertionError("the wrong row was named: %r" % answer[-160:])
        # a locked row offers no blank
        c.send("1")
        c.expect("LOCKED")
        c.expect("MANIFEST>")
        c.send("1")
        c.expect("Q goes back")
        c.expect("MANIFEST>")
        c.send("Q")
        c.expect("MANIFEST>")

    @step("manifest: the second pair opens the ELIZA row; a wrong ELIZA row says only the count")
    def manifest_eliza():
        if not args.evidence:
            return need
        c = state["c"]
        mfill(c, 2, (2, 2, 2))      # CHAOS, now right
        mfill(c, 4, (4, 4, 4))      # TYVANCE, right
        start = len(c.all)
        answer = mfile(c)
        if "CHAOS and TYVANCE" not in answer or "Rows confirmed: 4 of 5" not in answer:
            raise AssertionError("expected CHAOS and TYVANCE, 4 of 5 (the ELIZA row counts once it is shown); got %r" % answer[-160:])
        if "[5]" not in c.all[start:]:
            raise AssertionError("the ELIZA row did not open")
        mfill(c, 5, (1, 1, 2))      # ELIZA: the last blank is wrong
        answer = mfile(c)
        if "Rows confirmed: 4 of 5" not in answer or "ELIZA" in answer:
            raise AssertionError("a wrong last row must say only the count; got %r" % answer[-160:])

    @step("partial ending: goodbye with an unfinished manifest, ELIZA thanks the caller")
    def manifest_partial():
        if not args.evidence:
            return need
        c = state["c"]
        c.send("Q")
        c.expect("Main Board Command?", timeout=15)
        c.send("G")
        c.expect("Thank you for your interest", timeout=20)
        if not c.wait_closed(20):
            raise AssertionError("connection stayed open after Goodbye")
        c.close()

    @step("sign in again: the picks and the confirmed rows were kept")
    def manifest_again():
        if not args.evidence:
            return need
        c = Conn(port)
        c.expect("Login:", timeout=10)
        c.send(handle)
        c.expect("Password:")
        c.send(password)
        for _ in range(6):
            if c.expect("Press ENTER", "Main Board Command?", timeout=30) == "Main Board Command?":
                break
            c.send("")
        state["c"] = c
        c.send("R")
        c.expect("MANIFEST>", timeout=15)
        if c.all.count("LOCKED") < 4:
            raise AssertionError("the four confirmed rows are not locked after signing in again")
        if "[5]" not in c.all[-1500:]:
            raise AssertionError("the ELIZA row is gone after signing in again")

    @step("full manifest: ELIZA's scene plays and the caller is back at the main menu")
    def manifest_full():
        if not args.evidence:
            return need
        c = state["c"]
        mfill(c, 5, (1, 1, 1))
        start = len(c.all)
        c.send("S")
        c.expect("Rows confirmed: 5 of 5", timeout=15)
        if "Confirmed and locked: ELIZA" not in c.all[start:]:
            raise AssertionError("the last row was not named when confirmed")
        advance(c, "The crew are dead")
        advance(c, "Main Board Command?")

    @step("ending: release outbound, ELIZA argues, leave it queued (join the crew)")
    def ending_crew():
        if not (args.evidence and args.shell_open):
            return need + " and --shell-open"
        c = state["c"]
        c.send("D")
        c.expect("Door command?", timeout=15)
        c.send("N")
        c.expect("guest@nowhere:/$", timeout=15)
        c.send("release outbound")
        c.expect("Released")
        advance(c, "1) Send it.")
        c.send("2")
        advance(c, "You would only have to stay")
        advance(c, "2) Leave it queued.")
        c.send("2")
        advance(c, "added your name to the crew list")
        advance(c, "Main Board Command?")

    @step("a second caller (made through the prologue) sees the first one in WHO")
    def ending_who_crew():
        if not (args.evidence and args.shell_open):
            return need + " and --shell-open"
        # The first caller says goodbye (finished: no partial ending), then a new caller walks in.
        c = state["c"]
        c.send("G")
        if not c.wait_closed(15):
            raise AssertionError("connection stayed open after Goodbye")
        c.close()
        c2 = Conn(port)
        c2.expect("Login:")
        c2.send("admin")
        c2.expect("Password:")
        c2.send("admin")
        c2.expect("SHIP>")
        c2.send("SOURCE RTG")
        c2.expect("[K]", timeout=10)
        c2.send("K")
        c2.expect("TYPE CREW", timeout=40)
        c2.expect("SHIP>")
        c2.send("CREW")
        c2.expect("Handle (3-16")
        c2.send(handle2)
        c2.expect("Password (6+")
        c2.send(password)
        c2.expect("Real name:")
        c2.send("E2E Second")
        c2.expect("Location:")
        c2.send("Localhost")
        c2.expect("Login:")
        c2.send(handle2)
        c2.expect("Password:")
        c2.send(password)
        hit = ""
        for _ in range(6):
            hit = c2.expect("Press ENTER", "Where is the crew", "Main Board Command?", timeout=40)
            if hit != "Press ENTER":
                break
            c2.send("")
        if hit != "Where is the crew":
            c2.expect("Where is the crew", timeout=60)     # the first scene starts at the first calm prompt
        c2.send("1")
        advance(c2, "Main Board Command?", timeout=150)   # the scene plays itself out
        state["c2"] = c2
        start = len(c2.all)
        c2.send("W")
        c2.expect("Press ENTER", timeout=15)
        seg = c2.all[start:]
        if handle not in seg or "On the crew list" not in seg or "28y" not in seg:
            raise AssertionError("no sticky row for %s: %r" % (handle, seg[-300:]))
        c2.send("")
        c2.expect("Main Board Command?", timeout=15)

    @step("ending: the first caller returns and sends the queue; return traffic, families")
    def ending_sent():
        if not (args.evidence and args.shell_open):
            return need + " and --shell-open"
        c = Conn(port)
        c.expect("Login:", timeout=10)
        c.send(handle)
        c.expect("Password:")
        c.send(password)
        for _ in range(6):
            if c.expect("Press ENTER", "Main Board Command?", timeout=30) == "Main Board Command?":
                break
            c.send("")
        state["c"] = c
        c.send("D")
        c.expect("Door command?", timeout=15)
        c.send("N")
        c.expect("guest@nowhere:/$", timeout=15)
        c.send("release outbound")
        advance(c, "1) Send it.")
        c.send("1")
        advance(c, "Station received your burst")
        advance(c, "four families")
        advance(c, "Main Board Command?")

    @step("the sticky row now says the queue was sent")
    def ending_who_sent():
        if not (args.evidence and args.shell_open):
            return need + " and --shell-open"
        state["c"].send("G")
        state["c"].wait_closed(15)
        state["c"].close()
        c2 = state["c2"]
        start = len(c2.all)
        c2.send("W")
        c2.expect("Press ENTER", timeout=15)
        seg = c2.all[start:]
        if handle not in seg or "Downlink open" not in seg:
            raise AssertionError("sticky row not updated: %r" % seg[-300:])
        c2.close()
        # `registered` and the steps after it take a fresh connection each.
        state["c"] = Conn(port)
        state["c"].expect("Login:", timeout=10)
        state["c"].send(handle)
        state["c"].expect("Password:")
        state["c"].send(password)
        for _ in range(6):
            if state["c"].expect("Press ENTER", "Main Board Command?", timeout=30) == "Main Board Command?":
                break
            state["c"].send("")
    @step("a registered caller skips the prologue and has no timer")
    def registered():
        c = Conn(port)
        c.expect("Login:", timeout=10)
        c.send(handle)
        c.expect("Password:")
        c.send(password)   # a second login of the account takes over the first session
        hit = c.expect("Press ENTER", "Main Board Command?", "SHIP>", timeout=20)
        if hit == "SHIP>":
            raise AssertionError("a registered caller was sent to the ship console")
        time.sleep(args.timer + 3)
        c._pump()
        if c.eof:
            raise AssertionError("a registered caller was dropped by the power timer")
        state["c"].close()
        state["c"] = c
        c.send("")
        c.expect("Main Board Command?", timeout=10)

    @step("admin/admin works for a second caller at the same time")
    def second():
        c = Conn(port)
        c.expect("Login:", timeout=10)
        c.send("admin")
        c.expect("Password:")
        c.send("admin")
        c.expect("SHIP>", timeout=20)
        c.close()

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
        c.expect("Login:", timeout=10)
        c.send(who)
        c.expect("Password:")
        c.send(pw)
        c.expect("Press ENTER", "Main Board Command?", timeout=20)
        c.close()

    for fn in (banner, drop, reconnect, console, commands, rtg, wake, holds, crew_account, signup, scene, manifest_hidden, act1_bulletins, act1_manifest, act1_users, act1_conferences, act1_files, act1_crew_visit, act1_back, doors_menu, diag, shell, doors_leave, manifest_menu, manifest_open, manifest_wrong, manifest_pair, manifest_eliza, manifest_partial, manifest_again, manifest_full, ending_crew, ending_who_crew, ending_sent, ending_who_sent, registered, second, bye, crew):
        ok = fn()
        if not ok and fn in (banner, reconnect, console, rtg, wake, crew_account, signup, act1_crew_visit, act1_back, doors_menu, manifest_open, manifest_wrong, manifest_pair, manifest_eliza, manifest_partial, manifest_again, manifest_full):
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
