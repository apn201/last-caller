"""ANSI/GOODBYE/GOODBYE.ANS: the ship drifts into Earth's shadow, "LOSS OF SIGNAL".

The board prints "Time online: N minutes" on the line after this art, so the
screen is 21 rows. DRAFT text: ELIZA's farewell and the readout are drafts.
"""
from _common import *


def build():
    c = Canvas(80, 21)
    c.stars(0, 0, 80, 21, seed=31, density=0.05,
            avoid=lambda x, y: (y <= 8 and x < 42) or (y <= 6 and x >= 44) or (9 <= y <= 14 and x < 48) or y >= 14)

    # headline, two lines of the 5x5 font: highlight, body, blue drop shadow
    top = [WHITE, LCYAN, LCYAN, CYAN, CYAN]
    title5(c, 3, 1, "LOSS OF", top, DGREY)
    title5(c, 3, 5, "SIGNAL", top, DGREY)
    c.put(33, 1, "+", WHITE)
    c.put(41, 7, "·", DGREY)

    # readout, top right
    rows = [("CARRIER", "DROPPED", LCYAN), ("LINK", "░░░░░░░░░░ 0%", DGREY),
            ("SUN", "SETTING", YELLOW), ("ECLIPSE", "ENTERING", GREY), ("NEXT PASS", "SUNRISE", GREY)]
    for n, (k, v, col) in enumerate(rows):
        c.text(48, 1 + n, k, DGREY)
        c.text(59, 1 + n, v, col)

    # Earth: day side on the left shading through the terminator into night
    planet(c, 0, 10, 80, 10, 28, 66, 60, sun=(-0.95, -0.2, 0.3), seed=3.0)

    # the ship slides into shadow: its right end is already dark
    sp = PixelLayer(26, 10)
    ship_mini(sp, 2, 0)
    fade(sp, 12, 3)
    sp.flush(c, 46, 6)
    for i, x in enumerate(range(30, 45, 3)):       # the way it came
        c.put(x, 9, "·", (DGREY, GREY)[i % 2])

    # farewell, with ELIZA's ring and its halo
    c.text(3, 9, "Goodbye, caller.", YELLOW)
    c.text(3, 10, "Thank you for calling.", AMBER)
    c.text(3, 11, "The crew send their regards.", AMBER)
    ring = PixelLayer(8, 8)
    eliza_ring(ring, 3.5, 3.5, r_out=3.9, r_in=2.2, segs=8, dim=(1,))
    ring.flush(c, 40, 8)
    c.glow(43.5, 10, 4.2, 6.5, AMBER, inner=4.2)

    # bottom strip
    c.fill(0, 20, 80, 1, " ", GREY, BLACK)
    c.text(1, 20, "LOSS OF SIGNAL", LCYAN)
    c.text(16, 20, "│", DGREY)
    c.text(18, 20, "CARRIER DROPPED", GREY)
    c.text(34, 20, "│", DGREY)
    c.text(36, 20, "LAST SUNLIGHT 21:47", GREY)
    c.text(56, 20, "│", DGREY)
    c.text(58, 20, "·halcyon·crew·98·", DGREY)
    c.put(34, 20, ":")
    return [c.save("ANSI/GOODBYE/GOODBYE.ANS")]
