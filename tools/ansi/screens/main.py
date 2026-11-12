"""ANSI/MENUS/MAIN.ANS: the ship's control panel.

MENUS.INI [MAIN]: row=12 col=4 columns=3 width=25 rows=5 puts the options on
rows 12-16 (1-based), x 4..78: the empty band between the COMMANDS divider
and the lower divider. DRAFT text: the readout lines are drafts.
"""
from _common import *


def build():
    c = Canvas(80, 22)
    frame(c, "HALCYON // DECK 02")

    header(c, "MAIN BOARD")
    c.text(65, 1, "DECK 02", GREY)
    lights(c, 65, 2, "11101", on=LCYAN)
    c.text(65, 3, "SUN 100%", DGREY)
    sep(c, 4, ticks=True)

    # centrepiece: the ship and its readout
    c.stars(1, 5, 28, 5, seed=8, density=0.07)
    sp = PixelLayer(22, 10)
    ship_mini(sp, 0, 0)
    sp.flush(c, 4, 5)
    rows = [
        ("ORBIT", "LOW EARTH", GREY),
        ("POWER", None, None),
        ("BATT", "OPEN CIRCUIT", LRED),
        ("LINK", "28800  CARRIER OK", LCYAN),
        ("CREW", "4 ABOARD  ALL WELL", YELLOW),
    ]
    for n, (k, v, col) in enumerate(rows):
        c.text(32, 5 + n, k, DGREY)
        if v:
            c.text(40, 5 + n, v, col)
    bar(c, 40, 6, 10, 1.0, CYAN, DGREY)
    c.text(51, 6, "SOLAR", GREY)
    grille(c, 64, 5, 13, 5)
    c.put(70, 7, " ")      # a slat that fell out

    sep(c, 10, "COMMANDS")
    slot_area(c, 1, 11, 78, 5)
    sep(c, 16)

    # lower deck: systems, ELIZA on watch, deck label
    for n, (k, pat) in enumerate((("HAB", "1111"), ("COMMS", "1A11"), ("THERMAL", "1101"), ("ATTITUDE", "1111"))):
        c.text(3, 17 + n, k, DGREY)
        lights(c, 13, 17 + n, pat)
    ring = PixelLayer(8, 8)
    eliza_ring(ring, 3.5, 3.5, r_out=3.9, r_in=2.2, segs=8, off=(5,))
    ring.flush(c, 30, 17)
    c.text(39, 18, "ELIZA ON WATCH", YELLOW)
    c.text(39, 19, "ALL STATIONS NOMINAL", AMBER)
    c.text(62, 17, "MAIN BOARD", GREY)
    c.text(62, 18, "CALLER ACCESS", DGREY)
    c.hline(62, 19, 15, "─", DGREY)
    c.text(62, 20, "FRAME 02-A", DGREY)
    c.text(58, 21, "╡", DGREY)
    c.text(59, 21, TAG, DGREY)
    c.text(59 + len(TAG), 21, "╞", DGREY)
    return [c.save("ANSI/MENUS/MAIN.ANS")]
