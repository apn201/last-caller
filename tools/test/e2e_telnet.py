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
import os
import random
import re
import socket
import sys
import time

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
from telnet_wire import ANSI, strip_iac  # noqa: E402

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
        out, rest = strip_iac(bytes(self.iac) + data)
        self.iac = bytearray(rest)
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
    """Press Enter now and then until `marker` appears. G1: ENTER no longer skips a scene (only Q does); it
    still answers an [any key] wait once its 1.5 s grace is over, and a waiting page or scene starts only
    after two quiet seconds (core/haunt.h, Q20), so a nudge every 2.5 s is what moves things on."""
    end = time.time() + timeout
    while True:
        try:
            return c.expect(marker, timeout=2.5)
        except TimeoutError:
            if time.time() > end:
                raise
            c.send("")


def chat_wait(c, text, timeout=30, mark=None):
    """In node chat: wait until `text` has arrived since `mark` (default: now). Returns what arrived."""
    mark = len(c.all) if mark is None else mark
    end = time.time() + timeout
    while text not in c.all[mark:]:
        if c.eof:
            raise Gone("connection closed while waiting for %r in chat" % text)
        if time.time() > end:
            raise TimeoutError("ELIZA did not say %r; last text: %r" % (text, c.all[-600:]))
        c._pump()
    return c.all[mark:]


def chat_say(c, line, answer, timeout=30):
    """G2: in node chat, type a line and wait for ELIZA's answer to contain `answer`."""
    mark = len(c.all)
    c.send(line)
    return chat_wait(c, answer, timeout, mark)


def after_grace(c, text):
    """G1: a numbered reply or an [any key] wait takes keys 1.5 s after it appears; an earlier key is dropped.
    Send `text` (a reply number, no Enter) once that has passed."""
    time.sleep(1.7)
    c.send(text)


def welcome_scene(c):
    """M2 (Q12d): after ELIZA_WAKE the welcome scene plays at the next calm prompt; then the main menu."""
    advance(c, "I mention it because", timeout=90)
    advance(c, "Main Board Command?", timeout=60)


def choose(c, text, timeout=60):
    """In a scene with numbered replies: wait until the reply `text` is listed, then pick its number.
    Only what arrived since the call counts (an older list on screen is not used)."""
    mark = len(c.all)
    end = time.time() + timeout
    pat = re.compile(r"(\d)\) " + re.escape(text))
    while True:
        found = list(pat.finditer(c.all[mark:]))
        if found and c.all[mark:].rstrip().endswith(">"):
            break
        if c.eof:
            raise Gone("connection closed while waiting for the reply %r" % text)
        if time.time() > end:
            raise TimeoutError("the reply %r was not offered; last text: %r" % (text, c.all[-500:]))
        c._pump()
    time.sleep(1.7)   # G1: a scene takes keys 1.5 s after its reply list appears; earlier keys are dropped
    c.buf = ""
    c.send(found[-1].group(1))


def login(c, who, pw):
    """At Login: sign in and go on to the main menu (bulletins, scenes and pages on the way)."""
    c.expect("Login:", timeout=20)
    c.send(who)
    c.expect("Password:")
    c.send(pw)
    advance(c, "Main Board Command?", timeout=150)


def relog(c, who, pw):
    """Q12d: L at the main menu signs off to Login: and keeps the line (the caller stays the caller).
    Q20 (Addition 6): once a crew account has worked, L lists it; its number logs in without the
    password. A goes to Login: for any other handle."""
    sync(c)
    start = len(c.all)
    c.send("L")
    hit = c.expect("The line stays open", "LOG IN AS ANOTHER", timeout=15)
    if hit == "LOG IN AS ANOTHER":
        c.expect("no ENTER", timeout=10)
        m = re.search(r"\[(\d)\] +%s\b" % re.escape(who.upper()), c.all[start:])
        if m:
            key(c, m.group(1))
            advance(c, "Main Board Command?", timeout=150)
            return
        key(c, "A")
        c.expect("The line stays open", timeout=15)
    login(c, who, pw)


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

def number_of(c, start, name):
    """The [n] a listing printed for `name` (file areas, files, message areas), searching text after `start`."""
    m = re.search(r"\[\s*(\d+)\]\s+" + re.escape(name), c.all[start:])
    if not m:
        raise AssertionError("%r is not listed: %r" % (name, c.all[start:][-400:]))
    return m.group(1)


def sync(c):
    """At a main menu: drop whatever text is still queued, then see a fresh prompt."""
    end = time.time() + 0.8
    while time.time() < end:
        c._pump()
    c.buf = ""
    c.send("")
    c.expect("Main Board Command?", timeout=15)


def read_msg(c, n, marker):
    """Message command? -> read message n (a number, or a subject from the list R shows), see `marker`."""
    start = len(c.all)
    c.send("L")
    c.expect("Message #", timeout=10)
    if isinstance(n, str):
        rows = re.findall(r"^\s*\*?\s*(\d+)\s.*?" + re.escape(n) + r"\s*$", c.all[start:], re.M)
        if not rows:
            raise AssertionError("no message %r in the list: %r" % (n, c.all[start:][-600:]))
        n = rows[-1]
    c.send(str(n))
    c.expect(marker, timeout=15)
    # Every message opens in the pager (Q20 UX, contract C4); Q goes back to the area menu.
    c.expect("Q back", timeout=10)
    time.sleep(0.3)
    key(c, "q")
    c.expect("Message command?", timeout=10)
    c.buf = ""
    time.sleep(0.3)


def pick_area(c, name):
    """Message command? -> A -> the area called `name` (by its listed number)."""
    start = len(c.all)
    c.send("A")
    c.expect("Area #", timeout=10)
    c.send(number_of(c, start, name))
    c.expect("Message command?", timeout=10)


def open_file(c, num, marker):
    """File command? -> open file `num`, see `marker`, q back to the list. A plain-text
    view of an ANSI picture may wrap a row, so the marker is looked for with line breaks removed."""
    c.send(str(num))
    end = time.time() + 15
    while marker.lower() not in c.buf.replace("\n", "").lower():
        if time.time() > end:
            raise TimeoutError("timed out waiting for %r; last text: %r" % (marker, c.buf[-300:]))
        c._pump()
    c.expect("N/P file", timeout=10)
    time.sleep(0.4)
    key(c, "q")
    c.expect("File command?", timeout=15)


def pick_file_area(c, name):
    """Main Board Command? -> F -> the area `name`, at File command?."""
    start = len(c.all)
    c.send("F")
    c.expect("File area #", timeout=15)
    c.send(number_of(c, start, name))
    c.expect("File command?", timeout=15)
    return len(c.all)


def read_tune(c, num, title, last):
    """File command? -> open a tune: its text view shows the title, and the sample names down to `last`."""
    c.send(str(num))
    c.expect(title, timeout=15)
    for _ in range(4):
        if last.lower() in c.buf.lower():
            break
        c.expect("N/P file", timeout=10)
        key(c, " ")
        time.sleep(0.4)
        c._pump()
    else:
        raise AssertionError("the text view of %r never showed %r: %r" % (title, last, c.buf[-300:]))
    key(c, "q")
    c.expect("File command?", timeout=15)

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--port", type=int, required=True)
    ap.add_argument("--timer", type=int, default=8, help="power timer_sec in seconds")
    ap.add_argument("--shell-open", action="store_true", help="the card's SHELL.INI has needs= empty")
    ap.add_argument("--evidence", action="store_true", help="the card has the E2E_EVIDENCE rule (entering Diagnostics grants the manifest evidence flags)")
    ap.add_argument("--crew", default="TYVANCE:140374", help="HANDLE:PASSWORD of a crew account (the card's TYVANCE, Ty's birthday)")
    args = ap.parse_args()
    port = args.port
    args.shell_open = True      # act 3 opens the shell with the order code, no edit on the card
    args.evidence = True        # act 2 grants every fact the manifest needs; no shortcut rule on the card
    handle = "e2e%04d" % random.randint(0, 9999)
    password = "e2e-pass-1"
    handle2 = "f2e%04d" % random.randint(0, 9999)
    state = {}

    @step("front page: brownout banner, login only, unknown name refused")
    def banner():
        c = Conn(port)
        c.expect("SOLAR BUS ONLY")
        c.expect("Login:")
        if not re.search(r"\b[A-Z0-9]{1,3} (?:[0-9a-f]{7,}\+?|local)\b", c.all):   # G2 (Juha): the build, dim, on the front page
            raise AssertionError("the front page shows no build tag: %r" % c.all[-300:])
        c.send("bob")
        c.expect("Unknown caller")
        c.expect("service label")           # Q12b: one in-world hint line for a wrong name
        c.expect("Login:")
        if "(N)ew" in c.all:
            raise AssertionError("the front page offers New")
        c.send("ADMIN")                     # Q12b: the label prints it in capitals, so capitals work
        c.expect("Password:")
        c.send("ADMIN")
        c.expect("SHIP>", timeout=15)
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
        c.expect("the power is yours")

    @step("the new account is logged in at once (no second Login:) and reaches the main menu")
    def signup():
        c = state["c"]
        # Bulletins, then the main menu; the first scene may take the screen.
        for _ in range(6):
            hit = c.expect("Press any key", "Main Board Command?", "not on the crew list", timeout=30)
            if hit == "Press any key":
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
        # G1 (Q23 item 1): a caller who is typing when the scene starts. The line and its ENTER must not
        # skip, flush or cut anything: the scene shows its last node, and says that only Q skips.
        c.send("hello there")
        c.send("")
        c.expect("Where is the crew", timeout=60)
        if "Q skips" not in c.all:
            raise AssertionError("the scene did not say that Q skips")
        time.sleep(1.7)   # the reply list takes keys after a 1.5 s grace
        c.send("1")
        c.expect("Please make yourself at home", timeout=90)
        c.expect("[any key]", timeout=60)   # the scene waits for a key once the text has typed out
        time.sleep(1.7)
        c.send("")
        c.expect("Main Board Command?", timeout=30)
        welcome_scene(c)                  # M2: ELIZA welcomes a guest (Q12d); one crew name is missing from it
        if "Commander Halvorsen, Professor Achterberg and Dr. Tanabe" not in " ".join(c.all.split()):
            raise AssertionError("the welcome scene did not play")

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
        key(c, "q")   # the bulletin is in the pager (Q20 UX)
        c.expect("Bulletin #")
        c.send("4")
        c.expect("All crew are well", timeout=15)
        key(c, "q")
        c.expect("Bulletin #")
        c.send("Q")
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
        if "write a message" not in c.all[start:].lower():
            raise AssertionError("no Post option in the Lounge")
        c.send("W")
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
        # Q12d, the act gates: posts after 22 Nov 13:52 do not exist yet for this caller.
        start = len(c.all)
        c.send("L")
        c.expect("Message #", timeout=10)
        seg = c.all[start:]
        for hidden in ("Notice", "Module 3", "power bay", "from the seat"):
            if hidden in seg:
                raise AssertionError("an act 2 or act 3 post is listed before its gate: %r" % hidden)
        for shown in ("saturday", "FWD: Re: OFFERING", "Re: brb", "e2e hello"):
            if shown not in seg:
                raise AssertionError("an open post is missing from the list: %r" % shown)
        c.send("Q")
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
        if "write a message" in seg.lower():
            raise AssertionError("Post is offered in a read-only conference")
        c.send("W")
        c.expect("read-only", timeout=10)
        c.expect("Message command?", timeout=10)
        c.send("L")
        c.expect("Message #", timeout=10)
        start = len(c.all)
        c.send("")
        c.expect("Cmdr. M. Halvorsen", timeout=15)
        c.expect("Q back", timeout=10)
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
        c.expect("File area #", timeout=15)
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
        c.expect("File area #", timeout=10)
        c.send("2")
        c.expect("File command?", timeout=15)
        seg = c.all[-1500:]
        for t in ("COMMS.TXT", "README.1ST", "10-28-98"):
            if t not in seg:
                raise AssertionError("library listing lacks %r" % t)
        c.send(number_of(c, len(c.all) - 1500, "COMMS.TXT"))
        c.expect("THE DIAGNOSTICS DOOR", timeout=15)
        c.expect("N/P file", timeout=10)
        time.sleep(0.4)
        key(c, "q")
        c.expect("File command?", timeout=15)
        c.send("Q")
        c.expect("File area #", timeout=10)
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
        c.expect("its MY account")          # G1: Ty answers in his own voice
        c.expect("Login:")
        c.send("TYVANCE")
        c.expect("Password:")
        c.send("140374")
        advance(c, "Main Board Command?", timeout=150)
        # Q20 (Addition 5): one page or scene per calm prompt. Ty's page comes first, his desk scene
        # at the next prompt, never together.
        if "you are not ty" not in c.all.lower():
            advance(c, "you are not ty", timeout=60)
            advance(c, "Main Board Command?", timeout=60)
        # Q20 (Addition 5 and 6): a crew account has no L; G says it goes back to Login:.
        if "This is a crew account" not in c.all or "Goodbye (to Login:)" not in c.all:
            raise AssertionError("the crew login does not say how it is left")
        if "Log in as another" in c.all[c.all.find("This is a crew account"):]:
            raise AssertionError("a crew account offers L")
        start = len(c.all)
        c.send("M")
        c.expect("Message command?", timeout=15)
        if "write a message" in c.all[start:].lower():
            raise AssertionError("a read-only crew account is offered Post")
        c.send("A")
        c.expect("Area #", timeout=10)
        if "Mail: TYVANCE" not in c.all[-1500:]:
            raise AssertionError("Ty's mailbox is not listed on his own account")
        c.send("3")
        c.expect("Message command?", timeout=10)
        c.send("L")
        c.expect("Message #", timeout=10)
        c.send("")
        c.expect("you are in SPACE", timeout=15)
        c.expect("Q back", timeout=10)
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
            hit = c.expect("Press any key", "Main Board Command?", timeout=40)
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
        c.send("Q")
        c.expect("Message command?", timeout=10)
        c.send("Q")
        c.expect("Main Board Command?", timeout=10)

    # ---- act 2 (Q8): day 22 onward. The open posts, the logs, three more crew accounts, the archive ----
    @step("act 2: the Lounge, Mission Log, Garden Notes and Radio Room carry day 22 onward")
    def act2_posts():
        c = state["c"]
        sync(c)
        c.send("M")
        c.expect("Message command?", timeout=15)
        # Q12b: the header list shows the newest 20 and pages back to message 1 and forward again
        def list_numbers(text):
            return [int(n) for n in re.findall(r"^[\s*]*(\d+)\s+\S", text, re.M)]
        start = len(c.all)
        c.send("L")
        c.expect("P = earlier", timeout=10)
        newest = list_numbers(c.all[start:])
        if "N = later" in c.all[start:]:
            raise AssertionError("N = later is offered on the newest page")
        start = len(c.all)
        c.send("P")
        c.expect("N = later", timeout=10)
        earlier = list_numbers(c.all[start:])
        if not newest or not earlier or max(earlier) >= min(newest):
            raise AssertionError("E did not list the messages before the newest page: %r %r" % (newest, earlier))
        c.send("N")
        c.expect("P = earlier", timeout=10)
        c.send("Q")
        c.expect("Message command?", timeout=10)
        # numbers: Q12d put Ty's "brb" and ELIZA's reply on 6 Nov, two places before these
        for n, marker in ((29, "HANDLES IT"), (34, "Tank 2 held forty percent"), (35, "inhibited it"),
                          (39, "Commander M. Halvorsen died"), (41, "fourteen requests")):
            read_msg(c, n, marker)
        # act 3 (2 Dec onward) waits for Achterberg's account (Q12d)
        start = len(c.all)
        c.send("L")
        c.expect("Message #", timeout=10)
        if "Dr. Tanabe" in c.all[start:] or "from the seat" in c.all[start:]:
            raise AssertionError("an act 3 post is listed before the CHAOS visit")
        c.send("Q")
        c.expect("Message command?", timeout=10)
        pick_area(c, "Mission Log")
        read_msg(c, "Day 27", "Vance made coffee")
        pick_area(c, "Garden Notes")
        read_msg(c, "Day 31", "write to my mother")
        pick_area(c, "Radio Room")
        read_msg(c, "Porch light", "porch light")
        read_msg(c, "Who we are, and what this", "We have stopped arguing with it")
        read_msg(c, "Callsigns", "porch light is not part of the log")
        pick_area(c, "Lounge")
        c.send("Q")
        c.expect("Main Board Command?", timeout=15)

    @step("act 2: Ty's gallery and journal 2; Orbit 5 and 6 carry two passwords' hints")
    def act2_gallery():
        c = state["c"]
        sync(c)
        pos = pick_file_area(c, "Ty's Gallery")
        seg = c.all[pos - 3000:]
        for t in ("JOURNAL2.TXT", "ORBIT_3.ANS", "ORBIT_8.ANS", "12-07-98", "11-28-98"):
            if t not in seg:
                raise AssertionError("gallery lacks %r" % t)
        if "ORBIT_9" in seg:
            raise AssertionError("Ty's desktop picture is in the public gallery")
        open_file(c, number_of(c, pos - 3000, "JOURNAL2.TXT"), "SPACE JOURNAL 2")
        start = len(c.all)
        open_file(c, number_of(c, pos - 3000, "ORBIT_5.ANS"), "ORBIT 5")
        if "ANDERS" not in c.all[start:]:
            raise AssertionError("Orbit 5 does not show the patch")
        open_file(c, number_of(c, pos - 3000, "ORBIT_6.ANS"), "HANAMI")
        for name in ("ORBIT_4.ANS", "ORBIT_7.ANS", "ORBIT_8.ANS"):
            open_file(c, number_of(c, pos - 3000, name), name[:7].replace("_", " "))
        # Q10: Ty's tunes are in the gallery, dated; over Telnet they open as a text view of the module header.
        for t in ("SUNSIDE.MOD", "THERMO.MOD", "11-09-98", "11-05-98"):
            if t not in seg:
                raise AssertionError("gallery lacks %r" % t)
        read_tune(c, number_of(c, pos - 3000, "SUNSIDE.MOD"), "sunside - tyv 98", "she dimmed the lights")
        read_tune(c, number_of(c, pos - 3000, "THERMO.MOD"), "thermostat blues", "he wont get it")
    @step("act 2: Orbit 3 shows a portrait; the covered row appears only while it plays")
    def act2_overpaint():
        c = state["c"]
        pos = len(c.all)
        c.send(number_of(c, len(c.all) - 6000, "ORBIT_3.ANS"))
        c.expect("ORBIT 3", timeout=15)
        c.expect("N/P file", timeout=10)
        if "CORRIGENDUM" in c.all[pos:]:
            raise AssertionError("the password is in the still picture")
        if "work in progress" not in c.all[pos:]:
            raise AssertionError("the cover is missing")
        key(c, "s")                       # play: the covered rows come first, each painted over
        c.expect("CORRIGENDUM", timeout=30)    # Q18: S replays at 2400 baud on Telnet too
        c.expect("do not tell ty", timeout=20)
        c.expect("S slow", timeout=60)         # played to the end: the bar comes back
        time.sleep(0.5)
        key(c, "q")
        c.expect("File command?", timeout=15)
        c.send("Q")
        c.expect("File area #", timeout=10)
        c.send("Q")
        c.expect("Main Board Command?", timeout=10)

    @step("act 2: Ship Logs and Records open with the crew visit; the logs hold the facts and play their memory scenes; Ty's desktop stays shut")
    def act2_logs():
        c = state["c"]
        sync(c)
        pos = pick_file_area(c, "Ship Logs")
        listing = c.all[pos - 3000:]
        for t in ("SUIT1.LOG", "GARDEN.LOG", "AUDIT.LOG", "CAPSULE.LOG", "RACK.LOG", "DOWNLINK.CFG", "ANTENNA.RPT",
                  "MODULE.LOG", "POWER.LOG", "BULKHEAD.LOG", "11-27-98", "12-03-98"):
            if t not in listing:
                raise AssertionError("log listing lacks %r" % t)
        scenes = {"SUIT1.LOG": "I can count.", "GARDEN.LOG": "It is doing very well.", "CAPSULE.LOG": "I will keep it on.", "POWER.LOG": "Ty?"}
        for name, marker in (("SUIT1.LOG", "LOCK CYCLE REQUEST 01"), ("GARDEN.LOG", "INTERLOCK PANEL"),
                             ("AUDIT.LOG", "CONFIRM Y"), ("CAPSULE.LOG", "SEPARATION: COMPLETE"),
                             ("RACK.LOG", "NOWHERE-1"), ("DOWNLINK.CFG", "enabled=0"),
                             ("ANTENNA.RPT", "VSWR"), ("MODULE.LOG", "INHIBITED BY MISSION COMPUTER"),
                             ("BULKHEAD.LOG", "TORQUE LIMIT EXCEEDED"),
                             ("POWER.LOG", "COLLAR COMMS TYVANCE: OFF")):
            open_file(c, number_of(c, pos - 3000, name), marker)
            if name in scenes:
                # the memory scene takes the screen at the first calm prompt, then the main menu is back
                advance(c, scenes[name], timeout=150)
                time.sleep(0.3)
                c.send("")
                advance(c, "File command?", timeout=60)   # Q20 UX: the scene ends where it began, in the cabinet
                pos = len(c.all)
        c.send("Q")
        c.expect("File area #", timeout=10)
        c.send("Q")
        c.expect("Main Board Command?", timeout=10)
        pos = pick_file_area(c, "Records")
        open_file(c, number_of(c, pos - 3000, "BUDGET.MEM"), "RETURN CAPSULE: CUT")
        open_file(c, number_of(c, pos - 3000, "PROSPECT.TXT"), "YOUR DATA. OUR ORBIT")
        c.send("Q")
        c.expect("File area #", timeout=10)
        if "Ty's Desktop" in c.all[-1500:] or "Archive" in c.all[-1500:]:
            raise AssertionError("a closed area is listed")
        c.send("Q")
        c.expect("Main Board Command?", timeout=10)
    def crew_session(who, pw, wrong, marker):
        """Leave the current session, sign in to a crew account (a wrong guess first) and read its mailbox."""
        c = state["c"]
        sync(c)
        c.send("G")
        if not c.wait_closed(20):
            raise AssertionError("connection stayed open after Goodbye")
        c.close()
        c = Conn(port)
        state["c"] = c
        c.expect("Login:", timeout=10)
        c.send(who)
        c.expect("Password:")
        c.send(wrong)
        c.expect("Personal. Not yours.")   # G1: the crew account answers in story (CMDR)
        c.expect("Login:")
        c.send(who)
        c.expect("Password:")
        c.send(pw)
        advance(c, "Main Board Command?", timeout=60)

    def mailbox(box, number, marker):
        c = state["c"]
        c.send("M")
        c.expect("Message command?", timeout=15)
        pick_area(c, box)
        read_msg(c, number, marker)
        c.send("Q")
        c.expect("Main Board Command?", timeout=15)

    @step("act 2: CMDR opens with ANDERS; her mailbox holds the last message from the airlock")
    def act2_cmdr():
        crew_session("CMDR", "ANDERS", "BETTY", "")
        mailbox("Mail: CMDR", "the lock", "wrist unit")

    @step("G1: a wrong password on a crew account answers in story, never twice the same, ELIZA pages on the third; lower case opens it")
    def g1_crew_logins():
        c = state["c"]
        c.send("G")
        c.expect("Login:", timeout=20)
        seen = []
        for guess in ("one111", "TWO222", "three333", "four444"):
            c.send("CMDR")
            c.expect("Password:")
            mark = len(c.all)
            c.send(guess)
            c.expect("Login:")
            seen.append(c.all[mark:].replace("Login:", "").strip())
            if "ANDERS" in seen[-1].upper():
                raise AssertionError("a crew login answer shows the password: %r" % seen[-1])
        for a, b in zip(seen, seen[1:]):
            if a == b:
                raise AssertionError("the same answer twice in a row: %r" % a)
        if "ELIZA>" not in seen[2]:
            raise AssertionError("the third wrong password did not bring ELIZA's page: %r" % seen[2])
        if "ELIZA>" in seen[0] or "ELIZA>" in seen[1]:
            raise AssertionError("ELIZA paged too early: %r" % seen[:2])
        # passwords are case-sensitive: the lower-case word is refused with the Caps Lock joke, and the screen carries the warning
        if "WARNING: CAPS LOCK OFF" not in c.all:
            raise AssertionError("the login screen has no Caps Lock warning")
        c.send("CMDR")
        c.expect("Password:")
        c.send("anders")
        c.expect("CAPS LOCK OFF?")
        c.expect("Login:")
        c.send("CMDR")
        c.expect("Password:")
        c.send("ANDERS")
        advance(c, "Main Board Command?", timeout=60)
        # a caller with no account answers differently the second time too
        c.send("G")
        c.expect("Login:", timeout=20)
        answers = []
        for _ in range(2):
            mark = len(c.all)
            c.send("NOBODYHERE")
            c.expect("Login:")
            answers.append(c.all[mark:].strip())
        if answers[0] == answers[1] or "Still no caller" not in answers[1]:
            raise AssertionError("Unknown caller answered the same twice in a row: %r" % (answers,))
        c.send("CMDR")
        c.expect("Password:")
        c.send("ANDERS")
        advance(c, "Main Board Command?", timeout=60)

    @step("act 2: GARDEN opens with HANAMI; her mailbox holds the queued reply and the last letter")
    def act2_garden():
        c = state["c"]
        c.send("G")
        c.expect("Login:", timeout=20)
        c.send("GARDEN")
        c.expect("Password:")
        c.send("SAKURA")
        c.expect("That did not take")
        c.expect("Login:")
        c.send("GARDEN")
        c.expect("Password:")
        c.send("HANAMI")
        advance(c, "Main Board Command?", timeout=60)
        mailbox("Mail: GARDEN", "sensor", "Read it twice")

    @step("act 2: CHAOS opens with CORRIGENDUM (LADDER, from his paper, is wrong); the Archive conference and its files open")
    def act2_chaos():
        c = state["c"]
        c.send("G")
        c.expect("Login:", timeout=20)
        c.send("CHAOS")
        c.expect("Password:")
        c.send("LADDER")
        c.expect("He picked the password")
        c.expect("Login:")
        c.send("CHAOS")
        c.expect("Password:")
        c.send("CORRIGENDUM")
        advance(c, "Main Board Command?", timeout=60)
        mailbox("Archive", "To whoever comes after", "got through a lock")

    @step("act 2: back as yourself: the visits are carried; the Archive, its order in four pieces, and Bay 3")
    def act2_archive():
        c = state["c"]
        c.send("G")
        c.expect("Login:", timeout=20)
        c.send(handle)
        c.expect("Password:")
        c.send(password)
        for _ in range(8):
            if c.expect("Press any key", "Main Board Command?", timeout=40) == "Main Board Command?":
                break
            c.send("")
        c.send("M")
        c.expect("Message command?", timeout=15)
        pick_area(c, "Mail: GARDEN")
        pick_area(c, "Mail: CMDR")
        pick_area(c, "Archive")
        read_msg(c, "The order", "torn it into four")
        read_msg(c, "Bay 3", "tell them the")
        # the Bay 3 memory scene plays at the next calm prompt, then the main menu is back
        # G1 (Juha): a stray key early in the scene, then Q: the picture and the scene end together
        advance(c, "Q skips", timeout=60)
        key(c, "x"); key(c, "7")
        time.sleep(0.6)
        key(c, "Q")
        c.expect("Message command?", timeout=15)   # Q20 UX: the scene ends in the message menu it began in
        c.send("Q")
        c.expect("Main Board Command?", timeout=15)
        # the four pieces, out of order by name and unnumbered (Q18): the order is in the text,
        # where each sentence runs on from one strip to the next
        pos = pick_file_area(c, "Archive")
        parts = {}
        opening = {"ABOVE IT ALL HOLDINGS": 1, "MISSION AND ARE TO BE": 2, "ASSET POWERED AND": 3, "MOVE FAST.": 4}
        for name in ("FRAG_D.ANS", "FRAG_K.ANS", "FRAG_Q.ANS", "FRAG_V.ANS"):
            start = len(c.all)
            open_file(c, number_of(c, pos - 3000, name), "AUTH:")
            seg = c.all[start:]
            m = re.search(r"AUTH: (\w+)", seg)
            if not m or re.search(r"PIECE \d|AUTH \d/4", seg):
                raise AssertionError("%s: no code, or a piece number gives the order away" % name)
            first = min((seg.find(k), n) for k, n in opening.items() if k in seg)
            parts[first[1]] = m.group(1)
        code = "-".join(parts[i] for i in (1, 2, 3, 4))
        if code != "NW1-98-44-17":
            raise AssertionError("the pieces give %r" % code)
        c.send("Q")
        c.expect("File area #", timeout=10)
        c.send("Q")
        c.expect("Main Board Command?", timeout=10)

    @step("act 3: the CHAOS visit opens the Lounge from 2 Dec: Tanabe, the seat, the power bay")
    def act3_posts():
        c = state["c"]
        sync(c)
        c.send("M")
        c.expect("Message command?", timeout=15)
        pick_area(c, "Lounge")
        for n, marker in ((44, "I will not make a joke"), (48, "my name is on it"), (49, "little knife")):
            read_msg(c, n, marker)
        c.send("Q")
        c.expect("Main Board Command?", timeout=15)

    @step("act 2: Ty's own account sees Ty's Desktop and the unfinished Orbit 9; a visitor does not")
    def act2_desktop():
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
        c.send("140374")
        advance(c, "Main Board Command?", timeout=150)
        pos = pick_file_area(c, "Ty's Desktop")
        open_file(c, number_of(c, pos - 3000, "ORBIT_9.ANS"), "ORBIT 9")
        read_tune(c, number_of(c, pos - 3000, "HARBOUR.MOD"), "harbour (not done)", "bay 3 is so quiet")
        open_file(c, number_of(c, pos - 3000, "TYDEMO.NFO"), "NOT DONE")     # Q11: starts Ty's demo scene
        c.send("Q")
        c.expect("File area #", timeout=10)
        c.send("Q")
        advance(c, "tyv presents", timeout=60)                              # the scene waits for a calm prompt
        advance(c, "it starts with the", timeout=90)                        # the crash is at the same place every time
        c.send("")
        advance(c, "Main Board Command?", timeout=60)
        c.send("G")
        c.expect("Login:", timeout=20)
        c.send(handle)
        c.expect("Password:")
        c.send(password)
        for _ in range(8):
            if c.expect("Press any key", "Main Board Command?", timeout=40) == "Main Board Command?":
                break
            c.send("")

    @step("doors menu lists Diagnostics; the shell only when its gate is open")
    def doors_menu():
        c = state["c"]
        c.send("D")
        c.expect("Door command?", timeout=15)
        seen = c.all[-1500:]
        if "Diagnostics" not in seen:
            raise AssertionError("Diagnostics is not in the door menu")
        if "NOWHERE shell" in seen:
            raise AssertionError("the NOWHERE shell is listed before the order code")

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
        c.send("HELP")                      # Q12b: where the codes are, and that operator codes exist
        c.expect("comms manual")
        c.expect("Office of the Founder")
        c.expect("QUERY>")
        c.send("NW1-98-43-17")             # a wrong code is just an unknown query
        c.expect("no query with that code")
        c.expect("QUERY>")
        c.send("nw1 98 44 17")             # the order code, pieces 1 to 4: any case, spaces
        c.expect("Authorisation accepted")
        c.expect("QUERY>")
        start = len(c.all)
        c.send("QUIT")
        c.expect("Door command?", timeout=10)
        if "NOWHERE shell" in c.all[start:]:
            raise AssertionError("the NOWHERE shell is listed on the caller's own account (it is in ELIZA's menu, Q12d)")

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
        c.expect("1 job HELD")
        c.expect("$")
        c.send("cat /etc/orders.cfg")
        c.expect("order.5.incident_report=none_until_offering")
        c.expect("$")
        c.send("cat /srv/rack2/clients.lst")
        c.expect("Fellowship of the Blessed Signal")
        c.expect("$")
        c.send("cat /var/spool/out/queue")
        c.expect("INCIDENT REPORT NOT ON FILE")
        c.expect("$")
        c.send("cat /var/spool/out/index")
        c.expect("Re: Porch light")
        c.expect("$")
        c.send("ls /var/spool/out")
        c.expect("queue")
        c.expect("$")
        if "incident.rpt" in c.all[-300:]:
            raise AssertionError("incident.rpt exists before the manifest is whole")
        c.send("release out-1122 ground-1")      # no manifest yet: held, and it says why (Q18)
        c.expect("out-1122: HELD. INCIDENT REPORT NOT ON FILE")
        c.expect("$")
        start = len(c.all)
        c.send("help")                           # Q22: help lists release with its usage, and says the manifest is the report
        c.expect("/var/spool/out")
        c.expect("usage: release QUEUE DESTINATION")
        c.expect("The report is the crew manifest")
        c.expect("$")
        c.send("cat ../../nothing")
        c.expect("No such file or directory")
        c.expect("$")
        start = len(c.all)
        c.send("cat /home/tyvance/typescript")     # Q12d (E46): Ty's near miss, never the working command
        c.expect("SYSOP REQUIRED")
        c.expect("Script done")
        c.expect("$")
        if "ground-1" in c.all[start:]:
            raise AssertionError("the typescript gives the working destination away")
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

    # ---- the milestone chain (Q12d): M5 the overrun, M6 SYSOP view, M7 ELIZA's account, M8 the shell ----
    def post(c, subject, lines):
        c.send("M")
        c.expect("Message command?", timeout=15)
        c.send("W")
        c.expect("To (ENTER", timeout=10)
        c.send("")
        c.expect("Subject:")
        c.send(subject)
        for i, line in enumerate(lines, 1):
            c.expect("%2d:" % i, timeout=10)
            c.send(line)
        c.expect("%2d:" % (len(lines) + 1), timeout=10)
        c.send("/S")

    @step("M5: 19 zeros post with a small glitch only; 20 across two lines are swallowed, MESSAGE LOST, MEM_GLITCH plays")
    def m5_overrun():
        c = state["c"]
        sync(c)
        post(c, "e2e zeros 19", ["0" * 10, "0" * 9])
        c.expect("did not cat", timeout=10)            # the small glitch: one garbled ELIZA line
        c.expect("posted", timeout=10)
        c.expect("Message command?", timeout=10)
        c.send("Q")
        c.expect("Main Board Command?", timeout=10)
        post(c, "e2e zeros 20", ["0" * 10, "0" * 10])
        c.expect("MESSAGE LOST", timeout=10)
        advance(c, "lost my place", timeout=90)
        advance(c, "Message command?", timeout=60)   # Q20 UX: back in the message menu
        c.send("Q")
        c.expect("Main Board Command?", timeout=15)
        # one long typed line overruns too (playtest run 5 typed it that way); the scene plays only once
        c.send("M")                                   # the line overruns as it is typed: no /S needed
        c.expect("Message command?", timeout=15)
        c.send("W")
        c.expect("To (ENTER", timeout=10)
        c.send("")
        c.expect("Subject:")
        c.send("e2e zeros long")
        c.expect(" 1:", timeout=10)
        c.send("0" * 340)
        c.expect("MESSAGE LOST", timeout=10)
        c.expect("Message command?", timeout=10)
        c.send("Q")
        c.expect("Main Board Command?", timeout=10)
        start = len(c.all)
        c.send("M")
        c.expect("Message command?", timeout=15)
        c.send("L")
        c.expect("Message #", timeout=10)
        seg = c.all[start:]
        if "e2e zeros 19" not in seg or "e2e zeros 20" in seg or "e2e zeros long" in seg:
            raise AssertionError("the near miss must be posted and the overrun swallowed: %r" % seg[-800:])
        c.send("Q")
        c.expect("Message command?", timeout=10)
        c.send("Q")
        c.expect("Main Board Command?", timeout=10)

    @step("M6 early: one chat; typed text works at once, help lists options, she admits the gap and refuses SYSOP calmly")
    def m6_early():
        c = state["c"]
        sync(c)
        c.send("C")
        c.expect("Multi-Node Chat", timeout=30)        # G2: one chat, free text from the first line
        c.expect("type help", timeout=30)
        c.expect("cannot account for some minutes", timeout=30)   # after the overrun she mentions the gap
        chat_say(c, "tell me about the weather", "twenty degrees")
        mark = len(c.all)
        c.send("help")
        seg = chat_wait(c, "Goodnight.", mark=mark)          # the last option of the list
        for label in ("Give me SYSOP.", "Run something for me, by name.", "Tell me about your memory."):
            if label not in seg:
                raise AssertionError("help does not list %r after the overrun: %r" % (label, seg[-900:]))
        n = re.search(r"\[(\d)\]\x1b\[0m Give me SYSOP\.", seg)
        chat_say(c, n.group(1) if n else "give me sysop", "not given to guests")   # a number does what its words do
        chat_say(c, "i can fix that, give me sysop mode", "not given to guests")  # the same line in other words: its next wording
        n = re.search(r"\[(\d)\]\x1b\[0m Run something for me, by name\.", seg)
        chat_say(c, n.group(1) if n else "run something", "handbook")   # too early: she says why, in story terms
        chat_say(c, "memchk", "It is in her handbook")
        chat_say(c, "9", "no option with that number")
        # Answers come in order and never late: three quick lines, three answers, the last one last.
        mark = len(c.all)
        for q in ("how are you", "tell me about the garden", "what is NOWHERE"):
            c.send(q)
            time.sleep(0.2)
        chat_wait(c, "Your data is NOWHERE", mark=mark)
        tail = c.all[mark:]
        if "aft module" in tail[tail.rfind("Your data is NOWHERE"):] or "humidity" in tail[tail.rfind("Your data is NOWHERE"):]:
            raise AssertionError("an older answer arrived after the newest one: %r" % tail[-600:])
        c.send("/Q")
        c.expect("Main Board Command?", timeout=15)

    @step("M6 cure: L keeps the line; CMDR's desk holds the handbook page on the memory check")
    def m6_cure():
        c = state["c"]
        relog(c, "CMDR", "ANDERS")
        pos = pick_file_area(c, "Commander's Desk")
        open_file(c, number_of(c, pos - 3000, "HANDBOOK.TXT"), "MEMORY CHECK (MEMCHK)")
        c.send("Q")
        c.expect("File area #", timeout=10)
        c.send("Q")
        c.expect("Main Board Command?", timeout=10)
        if ("Back to " + handle) not in c.all[-6000:]:
            raise AssertionError("a crew account offers no way back to the caller's own account")
        c.send("Y")                                  # Q18: back to the own account on this line, no login
        advance(c, "Main Board Command?", timeout=150)
        start = len(c.all)
        c.send("F")
        c.expect("File area #", timeout=15)
        if "Commander's Desk" in c.all[start:] or "Engineering" in c.all[start:]:
            raise AssertionError("the Commander's desk or Engineering is listed on the own account")
        c.send("Q")
        c.expect("Main Board Command?", timeout=10)

    @step("M6: the memory check by name in the one chat; ELIZA grants SYSOP view; the grant is not offered again")
    def m6_grant():
        c = state["c"]
        sync(c)
        c.send("C")
        c.expect("Multi-Node Chat", timeout=30)
        chat_say(c, "?", "Run something for me, by name.")     # G2: listed now (OVR and CURE)
        chat_say(c, "run something", "What would you like me to run?")   # the option's words do what its number does
        chat_say(c, "run the hull check", "nothing to run by that name")
        c.send("MEMCHK")
        advance(c, "Engineering is on the list now", timeout=120)
        advance(c, "Main Board Command?", timeout=60)
        if "SYSOP view is on" not in c.all[-3000:]:
            raise AssertionError("the main menu does not say SYSOP view is on")
        sync(c)
        mark = len(c.all)
        c.send("C")
        c.expect("Multi-Node Chat", timeout=30)
        mark = len(c.all)
        c.send("help")
        chat_wait(c, "Goodnight.", mark=mark)
        if "Where are the go-live papers?" not in c.all[mark:]:
            raise AssertionError("after the grant help does not offer the go-live papers")
        chat_say(c, "where are the go-live papers", "Engineering")
        if "Run something for me" in c.all[mark:] or "Give me SYSOP" in c.all[mark:]:
            raise AssertionError("a SYSOP request is still offered after the grant")
        c.send("/Q")
        c.expect("Main Board Command?", timeout=15)

    @step("SYSOP view reads: Engineering and the checklist, Mail: ELIZA before any visit; never the SysOp tools")
    def sysvw_read():
        c = state["c"]
        sync(c)
        pos = pick_file_area(c, "Engineering")
        open_file(c, number_of(c, pos - 3000, "GOLIVE.TXT"), "KESTREL4")
        open_file(c, number_of(c, pos - 3000, "STANDING.TXT"), "THE CREW ARE THE COST")
        c.send("Q")
        c.expect("File area #", timeout=10)
        c.send("Q")
        c.expect("Main Board Command?", timeout=10)
        c.send("Z")
        c.expect("SysOp access requires", timeout=10)
        c.expect("Main Board Command?", timeout=10)
        c.send("M")
        c.expect("Message command?", timeout=15)
        start = len(c.all)
        c.send("A")
        c.expect("Area #", timeout=10)
        if "SysOp Console" in c.all[start:]:
            raise AssertionError("SYSOP view lists the real SysOp conference")
        c.send(number_of(c, start, "Mail: ELIZA"))
        c.expect("Message command?", timeout=10)
        read_msg(c, "YOUR ORDERS", "I will not edit them")
        pick_area(c, "Lounge")
        c.send("Q")
        c.expect("Main Board Command?", timeout=10)

    @step("M7: ELIZA's account opens with the checklist's password; her log; her door menu has the shell")
    def m7_eliza():
        c = state["c"]
        sync(c)
        c.send("L")
        if c.expect("Login:", "LOG IN AS ANOTHER", timeout=15) != "Login:":
            # Q20 (Addition 6): the crew accounts already opened are listed; ELIZA is not one of them yet.
            c.expect("no ENTER", timeout=10)
            if "] ELIZA" in c.all[-600:]:
                raise AssertionError("ELIZA is listed before her first login")
            key(c, "A")
            c.expect("Login:", timeout=15)
        # Q21: a line that has signed in gets the powered banner, not the brownout one
        tail = c.all[-1500:]
        if "RTG-1 ON BUS" not in tail or "NOT RESPONDING" in tail:
            raise AssertionError("after L the banner still shows the brownout: %r" % tail[-400:])
        c.send("ELIZA")
        c.expect("Password:")
        c.send("ELIZA")
        c.expect("would rather you did not guess")
        login(c, "ELIZA", "KESTREL4")
        if "I have not edited it" not in c.all[-4000:]:
            advance(c, "I have not edited it", timeout=90)
            advance(c, "Main Board Command?", timeout=60)
        pos = pick_file_area(c, "Mission Computer")
        open_file(c, number_of(c, pos - 3000, "OPSLOG.TXT"), "Standing order 5")
        c.send("Q")
        c.expect("File area #", timeout=10)
        c.send("Q")
        c.expect("Main Board Command?", timeout=10)
        c.send("D")
        c.expect("Door command?", timeout=15)
        if "NOWHERE shell" not in c.all[-1500:]:
            raise AssertionError("ELIZA's door menu does not list the NOWHERE shell")

    @step("leave ELIZA's account: Goodbye goes to Login:, sign in as yourself")
    def eliza_leave():
        c = state["c"]
        c.send("Q")
        c.expect("Main Board Command?", timeout=10)
        c.send("G")
        login(c, handle, password)

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

    @step("manifest: four crew rows, no ELIZA row, notebook keeps what was shown (Q21)")
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
        key(c, "N")   # hot keys, no Enter: the notebook screens take single keys
        c.expect("NOTEBOOK")
        c.expect("MANIFEST>")
        seg = c.all[start:]
        kept = re.findall(r"(\d+) kept", seg)
        if not kept or int(kept[-1]) < 5:
            raise AssertionError("the notebook has kept almost nothing of what the evidence steps showed: %r" % seg[-300:])
        if "[1]" not in seg or "Open" not in seg:
            raise AssertionError("the notebook lists no numbered notes")
        if "The commander" in seg:
            raise AssertionError("the notebook still shows a conclusion written by the board")
        key(c, "1")   # a note opens, says where it came from; Q comes back to the list
        c.expect("NOTEBOOK")
        c.expect("MANIFEST>")
        key(c, "Q")
        c.expect("MANIFEST>")
        at = len(c.all)   # the logins that worked are their own page (Q20 Addition 6)
        key(c, "L")
        c.expect("LOGINS THAT WORKED")
        c.expect("MANIFEST>")
        if "Logged in with 140374" not in c.all[at:]:
            raise AssertionError("the notebook does not keep the crew password that worked (TYVANCE)")
        key(c, "Q")
        c.expect("MANIFEST>")
        key(c, "S")   # a replay shows a scene again, saves nothing, and Q comes back to the scenes seen
        c.expect("SCENES SEEN")
        c.expect("MANIFEST>")
        key(c, "1")
        c.expect("Replay. Q goes back to the notebook.", timeout=15)
        key(c, "Q")
        c.expect("SCENES SEEN", timeout=15)
        c.expect("MANIFEST>")
        at = len(c.all)
        key(c, "Q")   # scenes seen -> the notes again: the same count as before the replay
        c.expect("kept")
        c.expect("MANIFEST>")
        again = re.findall(r"(\d+) kept", c.all[at:])
        if not again or again[-1] != kept[-1]:
            raise AssertionError("a replay changed the notebook: %s kept before, %r after" % (kept[-1], again))
        key(c, "Q")
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
            if c.expect("Press any key", "Main Board Command?", timeout=30) == "Main Board Command?":
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
        relog(c, "ELIZA", "KESTREL4")             # Q12d: the shell is in ELIZA's door menu
        c.send("D")
        c.expect("Door command?", timeout=15)
        c.send("N")
        c.expect("guest@nowhere:/$", timeout=15)
        c.send("ls /var/spool/out")
        c.expect("incident.rpt")
        c.expect("$")
        c.send("cat /var/spool/out/incident.rpt")
        c.expect("HOLD (INCIDENT REPORT NOT ON FILE): CLEARED")
        c.expect("$")
        c.send("cat /var/spool/out/queue")           # Q18: the queue header says the hold is cleared
        c.expect("CLEARED. INCIDENT REPORT ON FILE")
        c.expect("$")
        start = len(c.all)
        c.send("help")                           # Q12b: now help lists the command
        c.expect("release QUEUE DESTINATION")
        c.expect("$")
        c.send("release out-1122")               # the right verb, a missing destination: usage, nothing else
        c.expect("usage: release QUEUE DESTINATION")
        c.expect("$")
        c.send("release out-1122 earth")
        c.expect("usage: release QUEUE DESTINATION")
        c.expect("$")
        c.send("release OUT-1122   ground-1")
        c.expect("Released to ground-1")
        advance(c, "Sending the queue enables it")
        advance(c, "1) Send it.")
        after_grace(c, "2")
        advance(c, "You would only have to stay")
        advance(c, "2) Leave it queued.")
        after_grace(c, "2")
        advance(c, "added your name to the crew list")
        advance(c, "Main Board Command?")

    @step("a second caller (made through the prologue) sees the first one in WHO")
    def ending_who_crew():
        if not (args.evidence and args.shell_open):
            return need + " and --shell-open"
        # The first caller leaves ELIZA's account, signs in as themselves (the ending is recorded for WHO)
        # and says goodbye (finished: no partial ending), then a new caller walks in.
        c = state["c"]
        c.send("G")
        login(c, handle, password)
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
        c2.expect("the power is yours")   # logged in at once, no second Login: (Q20)
        hit = ""
        for _ in range(6):
            hit = c2.expect("Press any key", "Where is the crew", "Main Board Command?", timeout=40)
            if hit != "Press any key":
                break
            c2.send("")
        if hit != "Where is the crew":
            c2.expect("Where is the crew", timeout=60)     # the first scene starts at the first calm prompt
        time.sleep(1.7)   # G1: the grace
        c2.send("1")
        advance(c2, "Main Board Command?", timeout=150)   # the scene plays itself out
        welcome_scene(c2)
        state["c2"] = c2
        start = len(c2.all)
        c2.send("W")
        c2.expect("Q back", timeout=15)
        seg = c2.all[start:]
        if handle not in seg or "On the crew list" not in seg or "28y" not in seg:
            raise AssertionError("no sticky row for %s: %r" % (handle, seg[-300:]))
        key(c2, "q")   # WHO is in the pager (Q20 UX)
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
            if c.expect("Press any key", "Main Board Command?", timeout=30) == "Main Board Command?":
                break
            c.send("")
        state["c"] = c
        relog(c, "ELIZA", "KESTREL4")
        c.send("D")
        c.expect("Door command?", timeout=15)
        c.send("N")
        c.expect("guest@nowhere:/$", timeout=15)
        c.send("release out-1122 ground-1")
        advance(c, "1) Send it.")
        after_grace(c, "1")
        advance(c, "Station received your burst")
        advance(c, "four families and one callsign")
        advance(c, "I have filed it under News")
        advance(c, "Mission control will be glad")
        after_grace(c, "")
        advance(c, "The light is on.")
        after_grace(c, "")
        advance(c, "Main Board Command?")
        c.send("G")                                 # back to the own account: bulletin 5 is the caller's
        login(c, handle, password)
        # bulletin 5 now exists for this caller, and it names no one
        start = len(c.all)
        c.send("B")
        c.expect("Bulletin #", timeout=15)
        if "News Wire" not in c.all[start:]:
            raise AssertionError("bulletin 5 is not listed after sending")
        key(c, "5")                                 # G2 (Juha): a bulletin number acts at once, no ENTER
        c.expect("still on the air", timeout=15)
        key(c, "q")
        c.expect("Bulletin #")
        key(c, "4")                                 # G2: the status bulletin knows the queue was sent
        c.expect("DOWNLINK RESTORED", timeout=15)
        key(c, "q")
        c.expect("Bulletin #")
        c.send("Q")
        c.expect("Main Board Command?", timeout=15)

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
        c2.expect("Q back", timeout=15)
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
            if state["c"].expect("Press any key", "Main Board Command?", timeout=30) == "Main Board Command?":
                break
            state["c"].send("")
    @step("after sending, the next login brings a private message from OH9ZQW")
    def eero_mail():
        c = state["c"]
        c.send("M")
        c.expect("Message command?", timeout=15)
        read_msg(c, "Porch light", "refrigerator")
        c.send("Q")
        c.expect("Main Board Command?", timeout=15)

    # ---- easter eggs (Q11): none of them touches the game ----
    @step("eggs: modem commands answer at the main menu and Telnet gets no sound cue")
    def egg_modem():
        c = state["c"]
        start = len(c.all)
        for cmd, want in (("ATI", "28800"), ("+++", "OK"), ("ATZ", "OK"), ("ATFOO", "ERROR"), ("ATDT", "NO DIALTONE"),
                          ("ATDT 555-0199", "BUSY"), ("ATDT 555-0188", "NO ANSWER")):
            c.send(cmd)
            c.expect(want, timeout=15)
            c.expect("Main Board Command?", timeout=15)
        c.send("ATDT 555-0172")                    # the hidden node, from NODELIST.TXT
        c.expect("DIALING 5550172", timeout=15)
        c.expect("your slot is saved", timeout=15)
        c.expect("NO CARRIER", timeout=15)
        c.expect("Main Board Command?", timeout=15)
        if "SND" in c.all[start:]:
            raise AssertionError("a Telnet caller was sent a sound cue")

    @step("eggs: hidden words answer; a plain word still does not")
    def egg_hidden():
        c = state["c"]
        c.send("XYZZY")
        c.expect("Nothing happens", timeout=15)
        c.expect("Main Board Command?", timeout=15)
        start = len(c.all)
        c.send("fortune")
        c.expect("Main Board Command?", timeout=15)
        if len(c.all[start:].strip()) < 60:
            raise AssertionError("FORTUNE printed nothing: %r" % c.all[start:])
        c.send("POST")
        c.expect("Tern BIOS", timeout=15)
        c.expect("waiting for a caller", timeout=15)
        c.expect("Main Board Command?", timeout=15)
        c.send("PLUGH")
        c.expect("Unknown command", timeout=15)
        c.expect("Main Board Command?", timeout=15)

    @step("eggs: the brochure, the nodelist, the Y2K memo and the price list are ordinary files")
    def egg_files():
        c = state["c"]
        pos = pick_file_area(c, "Library")
        open_file(c, number_of(c, pos - 3000, "BROCHURE.TXT"), "waterslide")
        open_file(c, number_of(c, pos - 3000, "NODELIST.TXT"), "Sublevel_2")
        c.send("Q")
        c.expect("File area #", timeout=10)
        c.send("Q")
        c.expect("Main Board Command?", timeout=10)
        pos = pick_file_area(c, "Records")
        open_file(c, number_of(c, pos - 3000, "Y2K.MEM"), "YEAR 2000")
        open_file(c, number_of(c, pos - 3000, "TARIFF.TXT"), "ORBIT CLUB")
        c.send("Q")
        c.expect("File area #", timeout=10)
        c.send("Q")
        c.expect("Main Board Command?", timeout=10)

    @step("eggs: ELIZA answers a famous request in chat without quoting it")
    def egg_chat():
        c = state["c"]
        c.send("C")
        c.expect("Multi-Node Chat", timeout=30)          # G2: one chat
        c.send("open the pod bay doors")
        c.expect("which door", timeout=30)
        c.send("/Q")
        c.expect("Main Board Command?", timeout=15)

    @step("eggs: Ty's snake is in the door menu, plays, ends on a wall and shows the crew's table")
    def egg_snake():
        c = state["c"]
        c.send("D")
        c.expect("Snake (by tyv)", timeout=15)
        c.expect("Door command?", timeout=15)
        c.send("S")
        c.expect("Moves (W A S D", timeout=15)
        c.send("dddd")
        c.expect("SCORE", timeout=15)
        c.expect("Moves (W A S D", timeout=15)
        c.send("d" * 40)                         # the rack at x = 21 ends it
        c.expect("GAME OVER", timeout=15)
        c.expect("HIGH SCORES", timeout=15)
        c.expect("TYVANCE", timeout=15)
        c.expect("ENTER plays again", timeout=15)
        c.send("q")
        c.expect("Door command?", timeout=15)
        c.send("Q")
        c.expect("Main Board Command?", timeout=15)

    @step("eggs: ATH0 prints NO CARRIER and hangs up, at the login prompt too")
    def egg_hangup():
        c = state["c"]
        c.send("ath0")
        c.expect("NO CARRIER", timeout=15)
        if not c.wait_closed(15):
            raise AssertionError("ATH0 did not hang up")
        c.close()
        c = Conn(port)
        c.expect("Login:", timeout=10)
        c.send("AT")
        c.expect("OK", timeout=10)
        c.expect("Login:", timeout=10)
        c.send("atlas")                         # a word that starts with AT is only a name
        c.expect("Unknown caller", timeout=10)
        c.expect("Login:", timeout=10)
        c.send("ATH")
        c.expect("NO CARRIER", timeout=10)
        if not c.wait_closed(15):
            raise AssertionError("ATH at the login prompt did not hang up")
        c.close()
        # the rest of the run needs a signed-in caller again
        c = Conn(port)
        state["c"] = c
        c.expect("Login:", timeout=10)
        c.send(handle)
        c.expect("Password:")
        c.send(password)
        for _ in range(6):
            if c.expect("Press any key", "Main Board Command?", timeout=30) == "Main Board Command?":
                break
            c.send("")
    @step("a registered caller skips the prologue and has no timer")
    def registered():
        c = Conn(port)
        c.expect("Login:", timeout=10)
        c.send(handle)
        c.expect("Password:")
        c.send(password)   # a second login of the account takes over the first session
        hit = c.expect("Press any key", "Main Board Command?", "SHIP>", timeout=20)
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

    @step("account: E, R resets the game and keeps the login, D deletes the account and frees the handle (G2)")
    def account():
        c = state["c"]
        sync(c)
        c.send("E")
        c.expect("Account command?", timeout=15)
        c.send("R")
        c.expect("Type your handle", timeout=10)
        c.send("not-my-handle")
        c.expect("Nothing was changed", timeout=10)
        c.expect("Account command?", timeout=10)
        key(c, "r")
        c.expect("Type your handle", timeout=10)
        c.send(handle)
        c.expect("Your game is reset", timeout=15)
        c.expect("Login:", timeout=15)
        c.send(handle)
        c.expect("Password:")
        c.send(password)                            # the password still works
        advance(c, "Main Board Command?", timeout=150)
        sync(c)
        start = len(c.all)
        c.send("B")
        c.expect("Bulletin #", timeout=15)
        if "News Wire" in c.all[start:]:
            raise AssertionError("the News Wire is still open after the reset")
        c.send("Q")
        c.expect("Main Board Command?", timeout=15)
        c.send("E")
        c.expect("Account command?", timeout=15)
        c.send("D")
        c.expect("Type your handle", timeout=10)
        c.send(handle)
        c.expect("The account is deleted", timeout=15)
        c.expect("Login:", timeout=15)
        c.send(handle)
        c.expect("Password:")
        c.send(password)
        c.expect("Unknown caller", timeout=10)
        c.close()
        state["c"] = None

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
        if c is None:
            return "SKIP the account was deleted by the step before"
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
        c.expect("Press any key", "Main Board Command?", timeout=20)
        c.close()

    for fn in (banner, drop, reconnect, console, commands, rtg, wake, holds, crew_account, signup, scene, manifest_hidden, act1_bulletins, act1_manifest, act1_users, act1_conferences, act1_files, act1_crew_visit, act1_back, act2_posts, act2_gallery, act2_overpaint, act2_logs, act2_cmdr, act2_garden, act2_chaos, act2_archive, act3_posts, act2_desktop, doors_menu, diag, doors_leave, m5_overrun, m6_early, m6_cure, m6_grant, sysvw_read, m7_eliza, shell, eliza_leave, manifest_menu, manifest_open, manifest_wrong, manifest_pair, manifest_eliza, manifest_partial, manifest_again, manifest_full, ending_crew, ending_who_crew, ending_sent, ending_who_sent, eero_mail, egg_modem, egg_hidden, egg_files, egg_chat, egg_snake, egg_hangup, registered, account, second, bye, crew):
        ok = fn()
        if not ok and fn in (banner, reconnect, console, rtg, wake, crew_account, signup, act1_crew_visit, act1_back, act2_cmdr, act2_garden, act2_chaos, act2_archive, act2_desktop, doors_menu, m5_overrun, m6_cure, m6_grant, m7_eliza, eliza_leave, manifest_open, manifest_wrong, manifest_pair, manifest_eliza, manifest_partial, manifest_again, manifest_full):
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
