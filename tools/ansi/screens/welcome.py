"""ANSI/LOGIN/WELCOME.ANS and ASCII/LOGIN/WELCOME.ASC: the first thing a caller sees.

DRAFT text: the greeting and the telemetry line are drafts. The author makes
the final edit of every caller-visible line.
"""
from _common import *

W, H = 80, 22


def build():
    c = Canvas(W, H)

    def keep_clear(x, y):
        return (0 <= y <= 8) or (9 <= y <= 15 and (3 <= x <= 58))
    c.stars(0, 0, W, H, seed=11, density=0.05, avoid=lambda x, y: keep_clear(x, y) and y != 0)
    c.stars(58, 9, 22, 7, seed=5, density=0.06)

    # --- logo: highlight on top, cyan body, blue drop shadow, fade at the foot
    face = [WHITE, LCYAN, LCYAN, LCYAN, LCYAN, LCYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN]
    logo(c, 5, 1, "HALCYON", face, edge=WHITE, depth=BLUE, gap=2)

    # --- tagline
    c.text_center(8, "B U L L E T I N   B O A R D   S Y S T E M   ·   N O D E   1", DGREY)
    c.text(2, 8, "══╡", DGREY)
    c.text(74, 8, "╞══", DGREY)

    # --- Earth's limb: half blocks on the edge, shade characters inside
    planet(c, 0, 12, W, 9, 30, 60, 52, sun=(-0.62, -0.22, 0.75), seed=2.0)

    # --- the ship
    sp = PixelLayer(26, 22)
    ship(sp, 0, 0)
    sp.flush(c, 52, 9)
    c.put(50, 12, "+", WHITE)

    # --- ELIZA: an amber ring with a halo, and the greeting
    ring = PixelLayer(14, 14)
    eliza_ring(ring, 6.5, 6.5, off=(3, 4, 13), dim=(5,))
    ring.flush(c, 3, 9)
    c.glow(9.5, 12.5, 7.0, 10.0, AMBER, inner=7.0)
    c.text(19, 10, "Good evening, caller.", YELLOW)
    c.text(19, 11, "I am ELIZA. I look after this board.", AMBER)
    c.text(19, 12, "The crew are resting.", AMBER)
    c.text(19, 13, "Please keep your voice down.", AMBER)
    c.text(19, 15, "ELIZA", YELLOW)
    c.text(25, 15, "·", DGREY)
    c.text(27, 15, "SysOp", GREY)

    # --- telemetry strip
    c.fill(0, 21, W, 1, " ", GREY, BLACK)
    c.text(1, 21, "LINK ACQUIRED", LCYAN)
    c.text(15, 21, "│", DGREY)
    c.text(17, 21, "CARRIER 28800", GREY)
    c.text(31, 21, "│", DGREY)
    c.text(33, 21, "DOPPLER -3.1kHz", GREY)
    c.text(49, 21, "│", DGREY)
    c.text(51, 21, "SUN 100%", GREY)
    c.text(60, 21, "│", DGREY)
    c.text(62, 21, "BATT", GREY)
    c.text(67, 21, "OPEN", LRED)
    artist_tag(c, 62, 0)
    # decay: a missing block in the logo, a block gone soft, a separator that is not quite one
    c.put(60, 2, " ", GREY, BLACK)
    c.put(26, 4, "▒", LCYAN, BLUE)
    c.put(31, 21, ":", DGREY)

    out = [c.save("ANSI/LOGIN/WELCOME.ANS")]
    return out