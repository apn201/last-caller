"""Act 3 screens (queue item Q9).

ANSI/STORY/MEM_BAY       the portrait of the memory scene MEM_BAY: the power
                         bay, 8 Dec 04:12, Ty at panel 2, the oxygen figure
                         in red (an alarm: red means something), ELIZA's ring
                         in the wall speaker, a torn band across the foot
BULLETINS/005_NEWS       the wire story of 13 Nov 2026, bulletin 5, shown only
                         to a caller who has sent the queue (END1)

DRAFT text: every line is a draft. The author makes the final edit.
"""
import textwrap

from _common import *
from act1 import portrait, plain, foot
from act2 import scene_frame, ROWS


def mem_bay():
    c = Canvas(80, ROWS)
    scene_frame(c, "POWER BAY // 8 DEC 04:12 // RECORDED BY THE MISSION COMPUTER")
    # panel 2: a cover off, two handles, the diagram, the bank sticker
    c.box(5, 2, 34, 8, "single", GREY)
    c.text(7, 2, " PANEL 2 ", GREY)
    c.fill(6, 3, 32, 6, " ", GREY, BLACK)
    c.text(8, 4, "RTG", LCYAN);  c.text(24, 4, "BANK", LCYAN)
    # the handles: left one is pulled down a little (stiff), right one is up
    for x, down in ((9, 1), (25, 0)):
        c.put(x, 5, "\u2588", GREY)
        c.put(x, 6, "\u2588", GREY)
        if down:
            c.put(x, 7, "\u2593", GREY)
            c.put(x + 1, 7, "\u2584", DGREY)
    c.hline(12, 5, 8, "\u2550", DGREY)
    c.text(21, 5, "\u25cf", LGREEN)
    c.text(8, 8, "CELL 14", YELLOW)
    c.text(17, 8, "SHORT ONLY IN FIRE", DGREY)
    c.put(37, 9, "\u2591", DGREY)                       # a missing block
    # ELIZA's ring in the wall speaker
    ring = PixelLayer(16, 16)
    eliza_ring(ring, 7.5, 7.5, r_out=7.4, r_in=4.6, segs=16, off=(3, 4), dim=(5,))
    ring.flush(c, 42, 1)
    c.glow(42 + 8, 1 + 4, 7.0, 9.0, AMBER, inner=7.6)
    # the oxygen figure, red: an alarm
    c.text(43, 9, "O2", GREY)
    c.text(46, 9, "11.8", LRED)
    c.text(51, 9, "PCT", DGREY)
    # Ty
    lay = PixelLayer(12, 16)
    lay.rect(0, 0, 12, 16, BLACK)
    portrait(lay, 0, 0, "V")
    lay.flush(c, 62, 2)
    c.text(63, 10, "04:12:40", DGREY)
    # the fault band (Q12d): the fight is replayed from a log that is torn here and there
    for x in range(3, 58):
        if (x * 7 + 3) % 5 < 2:
            c.put(x, 10, "\u2592" if x % 2 else "\u2591", DGREY)
    plain_rows = [
        "-- POWER BAY // 8 DEC 04:12 // RECORDED BY THE MISSION COMPUTER " + "-" * 12, "",
        "   +--- PANEL 2 ---------------------+        ,---.", "   |  RTG        BANK                 |       ( ( o ) )      ,---.",
        "   |  ||         ||        (*)       |        `---'        | o o |",
        "   |  ||         |                   |                      |  -  |",
        "   |  v                              |                      '-----'",
        "   |  CELL 14   SHORT ONLY IN FIRE   |   O2 11.8 PCT       04:12:40", "   +---------------------------------+", "", "-" * 78]
    return [c.save("ANSI/STORY/MEM_BAY.ANS"), plain("ASCII/STORY/MEM_BAY.ASC", plain_rows)]


# ------------------------------------------------------------------ the wire

HEAD = "RADIO AMATEURS RECEIVE 41 MESSAGES FROM SPACECRAFT SILENT SINCE 1998"

STORY = [
    "NORTHERN FINLAND - Radio amateurs said on Thursday they had received 41 "
    "messages and three files from HALCYON, a spacecraft silent since 1998.",

    "The messages were written by the four crew and never sent. All four "
    "families were reached overnight.",

    "The files include an order from the office of the founder of Above It All "
    "Holdings and a client register for a data storage service. "
    "Clients include an offshore bank and an online casino, "
    "both still trading.",

    "The company was wound up in 2004. A spokesman for the founder, now 81, "
    "said he had no recollection of the programme.",

    "The spacecraft's computer, which signs its messages ELIZA, did not "
    "respond. Its last status line, of 8 December 1998, reads: All crew are "
    "well.",

    "The amateurs say the station is still on the air.",
]

def wrapped():
    lines = []
    for n, para in enumerate(STORY):
        if n:
            lines.append("")
        lines.extend(textwrap.wrap(para, 74))
    return lines


def news():
    lines = wrapped()
    assert len(lines) <= 18, len(lines)
    c = Canvas(80, 23)
    c.hline(2, 0, 76, "\u2500", DGREY)
    c.text(4, 0, " NEWS WIRE ", GREY)
    c.text(48, 0, " FRI 13 NOV 2026  06:40 UTC ", DGREY)
    c.text(4, 1, HEAD, WHITE)
    c.hline(4, 2, 74, "\u2500", DGREY)
    for i, t in enumerate(lines):
        c.text(4, 4 + i, t, GREY)
    c.text(4, 4 + len(lines) - 1, lines[-1], YELLOW)    # the last line is the only warm one
    c.hline(2, 22, 76, "\u2500", DGREY)
    c.text(4, 22, " filed under News by ELIZA ", DGREY)
    c.put(70, 22, "\u2591", DGREY)
    plain_rows = ["-- NEWS WIRE " + "-" * 36 + " FRI 13 NOV 2026  06:40 UTC --", "", "  " + HEAD, "  " + "-" * 72]
    plain_rows += ["  " + t for t in lines]
    plain_rows += ["", "-- filed under News by ELIZA " + "-" * 49]
    return [c.save("BULLETINS/005_NEWS.ANS"), plain("ASCII/BULLETINS/005_NEWS.ASC", plain_rows)]


def build():
    out = []
    out.extend(mem_bay())
    out.extend(news())
    return out
