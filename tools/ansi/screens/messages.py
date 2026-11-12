"""ANSI/MENUS/MESSAGES.ANS: the crew comms console, "MAIL & MESSAGES".

MENUS.INI [MESSAGES]: row=14 col=4 columns=2 width=30 rows=3.
DRAFT text: channel names and the outbound count are drafts.
"""
import math
from _common import *


def build():
    c = Canvas(80, 22)
    frame(c, "CREW COMMS // MAIL & MESSAGES")
    header(c, "MESSAGES")
    c.text(66, 1, "COMMS DECK", GREY)
    lights(c, 66, 2, "1111A", on=LCYAN)
    c.text(66, 3, "CH 1-3", DGREY)
    sep(c, 4, ticks=True)

    # oscilloscope: a carrier that died away
    c.box(3, 5, 42, 7, "single", DGREY)
    c.text(5, 5, "┤", DGREY)
    c.text(6, 5, " CH 1  EARTH LINK ", CYAN)
    c.text(24, 5, "├", DGREY)
    scope = PixelLayer(40, 10)
    for y in (1, 4, 7):
        for x in range(0, 40, 8):
            scope.set(x, y, DGREY)
    for x in range(40):
        env = math.exp(-x / 16.0)
        y = 4.5 + 4.2 * env * math.sin(x * 0.6)
        yn = 4.5 + 4.2 * math.exp(-(x + 1) / 16.0) * math.sin((x + 1) * 0.6)
        for yy in range(int(min(y, yn)), int(max(y, yn)) + 1):
            scope.set(x, max(0, min(9, yy)), LCYAN if env > 0.3 else CYAN)
    scope.flush(c, 4, 6)

    # channels
    panel(c, 47, 5, 31, 7, "CHANNELS")
    chan = [("1  EARTH LINK", "NO CARRIER", LRED), ("2  CREW", "OPEN", LCYAN), ("3  DECK", "OPEN", LCYAN),
            ("OUTBOUND MAIL", "212 WAITING", YELLOW), ("LAST SENT", "--:--", DGREY)]
    for n, (k, v, col) in enumerate(chan):
        c.text(49, 6 + n, k, GREY)
        c.text(66, 6 + n, v, col)

    sep(c, 12, "COMMANDS")
    slot_area(c, 1, 13, 78, 3)
    sep(c, 16)

    # footer: the mail slot, a ticker, ELIZA listening
    for n, row in enumerate(("╔════════════╗", "║ \\        / ║", "║  \\______/  ║", "╚════════════╝")):
        c.text(4, 17 + n, row, GREY)
    c.text(21, 17, "QUEUE", DGREY)
    c.text(21, 18, "NOTHING DELIVERED", GREY)
    c.text(21, 19, "NOTHING RETURNED", GREY)
    c.text(21, 20, "ALL MAIL IS KEPT", GREY)
    ring = PixelLayer(8, 8)
    eliza_ring(ring, 3.5, 3.5, r_out=3.9, r_in=2.2, segs=8)
    ring.flush(c, 48, 17)
    c.text(58, 18, "ELIZA IS", YELLOW)
    c.text(58, 19, "LISTENING", AMBER)
    c.text(59, 21, "╡", DGREY)
    c.text(60, 21, TAG, DGREY)
    c.text(60 + len(TAG), 21, "╞", DGREY)
    return [c.save("ANSI/MENUS/MESSAGES.ANS")]
