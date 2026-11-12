"""ANSI/GOODBYE/GOODBYE.ANS: the ship drifts into Earth's shadow, "LOSS OF SIGNAL".

The board prints "Time online: N minutes" on the line after this art, so the
screen is 21 rows. DRAFT text: ELIZA's farewell and the readout are drafts.
"""
from _common import *


def build():
    c = Canvas(80, 21)
    c.stars(0, 0, 80, 21, seed=31, density=0.04,
            avoid=lambda x, y: (y <= 6 and x < 30) or (y <= 6 and x >= 44) or (7 <= y <= 13 and x < 48))

    # headline, two lines of the 5x5 font
    top = [WHITE, WHITE, LCYAN, LCYAN, CYAN]
    title5(c, 3, 1, "LOSS OF", top, None)
    title5(c, 3, 4, "SIGNAL", top, None)

    # readout, top right
    rows = [("CARRIER", "DROPPED", LCYAN), ("LINK", "░░░░░░░░░░ 0%", DGREY),
            ("SUN", "SETTING", YELLOW), ("ECLIPSE", "ENTERING", GREY), ("NEXT PASS", "SUNRISE", GREY)]
    for n, (k, v, col) in enumerate(rows):
        c.text(48, 1 + n, k, DGREY)
        c.text(59, 1 + n, v, col)

    # Earth with the terminator under the ship; the ship slides into shadow
    ground = PixelLayer(80, 22)
    earth(ground, 38, 72, 66, sun=(-1.0, -0.15), seed=3.0, bias=0.2)
    ground.flush(c, 0, 10)
    sp = PixelLayer(26, 10)
    ship_mini(sp, 2, 0)
    fade(sp, 12, 3)
    sp.flush(c, 46, 6)
    for i, x in enumerate(range(30, 45, 3)):       # the way it came
        c.put(x, 10, "·", (DGREY, GREY)[i % 2])

    # farewell
    c.text(3, 8, "Goodbye, caller.", YELLOW)
    c.text(3, 9, "Thank you for calling.", AMBER)
    c.text(3, 10, "The crew send their regards.", AMBER)
    ring = PixelLayer(8, 8)
    eliza_ring(ring, 3.5, 3.5, r_out=3.9, r_in=2.2, segs=8, dim=(1,))
    ring.flush(c, 36, 7)

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
