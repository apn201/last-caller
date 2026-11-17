"""ANSI/DEMO/*.ANS: the ANSI version of the end demo (slice GE). Telnet callers, and a browser whose card has
no DEMO.JS, get these frames one after the other; STORY/DEMO.INI says which, for how long and what is printed
under each (the [T01]... steps). Each picture is 80 x 14 so the card's lines fit under it.

D_RELEASE   the outbound queue: a packet frame, the antenna, TX lights
D_TITLE     the HALCYON logo over stars and the limb of the Earth
D_LETTER_H  D_LETTER_A  D_LETTER_T  D_LETTER_V   the four crew, one frame each (the letter is printed under it)
D_NEWS      a newspaper page, masthead and columns, for the aftermath headlines
D_RADIO     2026: the ship passing over the club's antenna, a beacon line
D_CREDITS   the frame the credits scroll under
D_AFTER     the reconnect screen: the ship as it is, ELIZA's ring

DRAFT text: every word is a draft. The author makes the final edit.
"""
import random

from _common import *
from _profile import profile
from act1 import portrait

H = 14


def _frame(c, label):
    c.hline(0, 0, 80, "\u2500", DGREY)
    c.text(3, 0, " " + label + " ", CYAN)
    c.hline(0, H - 1, 80, "\u2500", DGREY)
    artist_tag(c, 61, H - 1)
    c.put(70, 0, "\u2591", DGREY)             # a missing block


def release():
    c = Canvas(80, H)
    _frame(c, "HALCYON // OUTBOUND QUEUE OUT-1122")
    c.stars(0, 1, 80, H - 2, seed=21, density=0.03, avoid=lambda x, y: x < 52 and 2 <= y <= 10)
    # the packet: 41 small squares and 3 wide ones
    c.text(4, 3, "MESSAGES", GREY)
    for i in range(41):
        c.put(4 + i, 4, "\u25a0", LCYAN if i % 7 else CYAN)
    c.text(4, 6, "FILES", GREY)
    for i, name in enumerate(("AUDIT.LOG", "ORDERS.CFG", "CLIENTS.LST")):
        x = 4 + i * 16
        c.box(x, 7, 14, 3, "single", AMBER)
        c.text(x + 1, 8, name.ljust(12), YELLOW)
    # the dish, dark, and a beam to the ground
    sp = PixelLayer(22, 8)
    profile(sp, 0, 0, 2)
    sp.flush(c, 56, 3)
    for k in range(6):
        c.put(50 - k * 2, 8 + k % 2, "\u00b7", DGREY)
    c.text(60, 8, "DOWNLINK 28800", LCYAN)
    c.text(60, 9, "GROUND-1", GREY)
    lights(c, 60, 11, "1111A", on=LCYAN)
    return [c.save("ANSI/DEMO/D_RELEASE.ANS")]


def title():
    c = Canvas(80, H)
    c.stars(0, 0, 80, H, seed=33, density=0.05, avoid=lambda x, y: 1 <= y <= 12 and 4 <= x <= 76)
    face = [WHITE, LCYAN, LCYAN, LCYAN, LCYAN, LCYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN]
    logo(c, 5, 0, "HALCYON", face, edge=WHITE, depth=BLUE, gap=2)
    c.text(5, 13, "A REAL-TIME MULTI-USER EXPERIENCE", DGREY)
    c.text(62, 13, "\u00b7halcyon\u00b7crew\u00b798\u00b7", DGREY)
    c.put(33, 5, " ", GREY, BLACK)
    return [c.save("ANSI/DEMO/D_TITLE.ANS")]


LETTERS = {"H": ("MARTA HALVORSEN TO ANDERS", "H"), "A": ("DESMOND ACHTERBERG TO WHOEVER COMES AFTER", "A"),
           "T": ("KEIKO TANABE TO MAE", "T"), "V": ("TY VANCE TO LUMEN", "V")}


def letter(who):
    label, p = LETTERS[who]
    c = Canvas(80, H)
    _frame(c, "LETTER HOME")
    c.stars(0, 1, 80, H - 2, seed=40 + ord(who), density=0.02, avoid=lambda x, y: x < 34)
    small = PixelLayer(12, 16)
    portrait(small, 0, 0, p)
    big = PixelLayer(24, 16)                    # twice as wide: a cell is taller than it is wide
    for y in range(16):
        for x in range(12):
            col = small.get(x, y)
            if col is not None:
                big.set(2 * x, y, col); big.set(2 * x + 1, y, col)
    c.box(4, 2, 28, 10, "single", DGREY)
    big.flush(c, 6, 3)
    c.text(36, 4, label, YELLOW)
    c.hline(36, 5, 40, "\u2500", DGREY)
    c.text(36, 11, "UNSENT 1998. IN THE QUEUE.", DGREY)
    return [c.save("ANSI/DEMO/D_LETTER_%s.ANS" % who)]


def news():
    c = Canvas(80, H)
    c.fill(2, 0, 76, H, " ", GREY, BLACK)
    c.box(2, 0, 76, H, "double", GREY)
    c.text_center(1, "T H E   H A R B O U R   P O S T", WHITE)
    c.hline(3, 2, 74, "\u2550", GREY)
    rnd = random.Random(7)
    for col in range(3):
        x = 5 + col * 24
        for y in range(4, 12):
            n = rnd.randint(14, 21)
            c.text(x, y, "\u2592" * n if y == 4 else "\u2591" * n, DGREY)
    c.put(70, 12, "\u2591", DGREY)
    return [c.save("ANSI/DEMO/D_NEWS.ANS")]


def radio():
    c = Canvas(80, H)
    c.stars(0, 0, 80, 7, seed=55, density=0.05)
    planet(c, 0, 6, 80, 8, 40, 60, 52, sun=(-0.62, -0.22, 0.75), seed=2.0, lights=True)
    sp = PixelLayer(22, 8)
    profile(sp, 0, 0, 2)
    sp.flush(c, 30, 1)
    c.text(2, 1, "2026  PASS 3 OF 3", GREY)
    c.text(2, 2, "SIGNAL 145.800", LCYAN)
    c.text(2, 3, "\u2588 ... .-.. -.-. --- -.", AMBER)
    c.put(62, 11, "\u25cf", YELLOW)             # a light on the night side
    return [c.save("ANSI/DEMO/D_RADIO.ANS")]


def inworld():
    """D_INWORLD: Ty's production screen - TYVANCE presents, in-world credits."""
    c = Canvas(80, H)
    # stars in the background
    c.stars(0, 0, 80, H, seed=88, density=0.03, avoid=lambda x, y: 10 <= x <= 69 and 1 <= y <= 12)
    # outer double box
    c.box(2, 0, 76, H, "double", DGREY)
    # a bright top bar inside the box
    for x in range(3, 78):
        c.put(x, 1, "\u2591", AMBER)
    # the production title in the bar
    label = "TYVANCE  presents"
    tx = (80 - len(label)) // 2
    c.text(tx, 1, label, WHITE)
    # inner horizontal rule
    c.hline(4, 3, 72, "\u2500", DGREY)
    # the demo title
    c.text_center(4, "TY'S DEMO  (the release build)", LCYAN)
    c.hline(4, 5, 72, "\u2500", DGREY)
    # credits
    c.text(8, 6,  "code:", GREY);     c.text(20, 6,  "ty vance", WHITE)
    c.text(8, 7,  "graphics:", GREY); c.text(20, 7,  "ty vance  (marta lent me the camera)", WHITE)
    c.text(8, 8,  "music:", GREY);    c.text(20, 8,  "borrowed from sunside.mod, sorry", DGREY)
    c.text(8, 9,  "platform:", GREY); c.text(20, 9,  "the 486 behind the nav seat", GREY)
    # copyright and tag
    c.hline(4, 11, 72, "\u2500", DGREY)
    c.text(8, 12, "(c) tyv 98", DGREY)
    artist_tag(c, 54, 12)
    return [c.save("ANSI/DEMO/D_INWORLD.ANS")]


def credits():
    c = Canvas(80, H)
    _frame(c, "CREDITS")
    c.stars(0, 1, 80, H - 2, seed=61, density=0.04)
    ring = PixelLayer(14, 14)
    eliza_ring(ring, 6.5, 6.5, off=(3, 4, 13), dim=(5,))
    ring.flush(c, 33, 3)
    c.glow(39.5, 6.5, 7.0, 10.0, AMBER, inner=7.0)
    return [c.save("ANSI/DEMO/D_CREDITS.ANS")]


def after():
    c = Canvas(80, H)
    _frame(c, "HALCYON // LINK RESTORED")
    c.stars(0, 1, 80, H - 2, seed=71, density=0.03, avoid=lambda x, y: 4 <= x <= 75 and 2 <= y <= 11)
    sp = PixelLayer(22, 8)
    profile(sp, 0, 0, 2)
    sp.flush(c, 4, 3)
    ring = PixelLayer(8, 8)
    eliza_ring(ring, 3.5, 3.5, r_out=3.9, r_in=2.2, segs=8, off=(5,))
    ring.flush(c, 66, 3)
    c.text(30, 4, "LINK", DGREY); c.text(38, 4, "28800  CARRIER", GREY)
    c.text(30, 5, "QUEUE", DGREY); c.text(38, 5, "SENT. 41 AND 3.", LCYAN)
    c.text(30, 6, "BOARD", DGREY); c.text(38, 6, "OPEN", LCYAN)
    return [c.save("ANSI/DEMO/D_AFTER.ANS")]


def build():
    out = []
    for fn in (release, title, news, radio, inworld, credits, after):
        out += fn()
    for who in LETTERS:
        out += letter(who)
    return out