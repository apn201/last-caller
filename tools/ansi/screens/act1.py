"""Act 1 screens (queue item Q7).

BULLETINS/002_WELCOME   ELIZA's notice to callers (mandatory bulletin)
BULLETINS/003_CREW      The crew list, written in the Founder's PR voice (E01)
BULLETINS/004_STATUS    The last status line the board ever filed, 8 Dec 04:15
BULLETINS/001_RULES     (rules.py draws the ANSI; the plain twin is here)
GALLERY ORBIT_1, ORBIT_2   Ty's pictures: the crew in frames, Earth in three blues
ANSI/STORY/TY_DESK      Ty's desk, the picture of the scene TY_DESK

Ty drew the two Orbit pieces, so they carry his tag and his lower-case
voice. The bulletins are ELIZA's and the Founder's PR department's.
Every plain (ASCII) twin is 7-bit, 80 columns, because the bulletins are the
evidence: a caller without ANSI must still get the crew list.
DRAFT text: every line is a draft. The author makes the final edit.
"""
import random

from _common import *
from _profile import profile

TY_TAG = "\u00b7tyv\u00b798\u00b7"


def plain(rel, lines):
    c = Canvas(80, len(lines))
    for y, r in enumerate(lines):
        c.text(0, y, r)
    return c.save_plain(rel)


def foot(c, y=21, tag=TAG):
    c.text(59, y, "\u2524", DGREY)
    c.text(60, y, tag, DGREY)
    c.text(60 + len(tag), y, "\u251c", DGREY)


# ------------------------------------------------------------------ WELCOME

WELCOME = [
    ("Good evening.", YELLOW),
    ("", GREY),
    ("This is the HALCYON bulletin board. It has run since 2 November 1998.", GREY),
    ("I am ELIZA, the mission computer. I am also the SysOp.", GREY),
    ("", GREY),
    ("   The Lounge is open to everyone. Please post there.", GREY),
    ("   The Mission Log and the Garden Notes are kept by the crew.", GREY),
    ("   The Radio Room is for the amateurs who call from the ground.", GREY),
    ("   The crew list is Bulletin 3. The user list is U on the main menu.", GREY),
    ("", GREY),
    ("If something does not work, tell me. I will see to it.", GREY),
]


def welcome():
    c = Canvas(80, 22)
    frame(c, "HALCYON // SYSOP NOTICE")
    header(c, "WELCOME")
    ring = PixelLayer(8, 8)
    eliza_ring(ring, 3.5, 3.5, r_out=3.9, r_in=2.2, segs=8, dim=(3,))
    ring.flush(c, 68, 1)
    pf = PixelLayer(22, 8)               # the ship, 2 Nov 1998: all lit
    profile(pf, 0, 0, 0)
    pf.flush(c, 46, 1)
    sep(c, 5, ticks=True)
    for n, (line, col) in enumerate(WELCOME):
        c.text(5, 7 + n, line, col)
    c.text(5, 18, "ELIZA", YELLOW)
    c.text(5, 19, "SysOp  //  Mission AI  //  2 Nov 1998", DGREY)
    foot(c)
    c.put(70, 5, "\u2591", DGREY)          # a missing block
    out = [c.save("BULLETINS/002_WELCOME.ANS")]
    rows = ["-- HALCYON // SYSOP NOTICE " + "-" * 52, "", "  W E L C O M E", ""]
    rows += ["  " + line for line, _ in WELCOME]
    rows += ["", "", "                                              ELIZA", "                                              SysOp // Mission AI // 2 Nov 1998", "", "-" * 78]
    out.append(plain("ASCII/BULLETINS/002_WELCOME.ASC", rows))
    return out


# --------------------------------------------------------------------- CREW

CREW = [
    ("CMDR", "Cmdr. Marta Halvorsen", ["Commander and pilot.", "b. 9 Jun 1952, Norway.", "A former test pilot, 3,100 hours."]),
    ("CHAOS", "Prof. Desmond Achterberg", ["Risk adviser, for our insurers.", "b. 2 Nov 1940, Netherlands.", "Studies how complex systems fail."]),
    ("GARDEN", "Dr. Keiko Tanabe", ["Physician and plant biologist.", "b. 21 Aug 1959, Brazil.", "Runs the garden. Looks after us."]),
    ("TYVANCE", "Tyler Vance", ["Payload specialist and artist.", "b. 14 Mar 1974, USA.", "First artist in orbit. Family."]),
]


def crew():
    c = Canvas(80, 22)
    frame(c, "HALCYON // THE CREW")
    header(c, "CREW")
    c.text(40, 2, "FOUR PEOPLE. ONE COMPUTER. ONE MISSION.", DGREY)
    c.text(40, 3, "HALCYON ORBITAL SUITES  //  PATHFINDER", DGREY)
    sep(c, 5, ticks=True)
    pos = [(3, 6), (41, 6), (3, 12), (41, 12)]
    for (handle, name, lines), (x, y) in zip(CREW, pos):
        c.box(x, y, 37, 6, "single", DGREY)
        c.text(x + 2, y, " " + handle + " ", CYAN)
        c.text(x + 3, y + 1, name, WHITE)
        for n, line in enumerate(lines):
            c.text(x + 3, y + 2 + n, line, GREY if n < 2 else DGREY)
        c.text(x + 33, y + 1, "\u25a0", LCYAN)
    sep(c, 18, "SYSOP")
    ring = PixelLayer(6, 6)
    eliza_ring(ring, 2.5, 2.5, r_out=2.9, r_in=1.5, segs=6)
    ring.flush(c, 4, 19)
    c.text(12, 19, "ELIZA", YELLOW)
    c.text(19, 19, "Mission computer. SysOp. Awake at all hours.", GREY)
    c.text(12, 20, "Account TYVANCE has access to all decks.", DGREY)
    foot(c)
    out = [c.save("BULLETINS/003_CREW.ANS")]
    rows = ["-- HALCYON // THE CREW " + "-" * 56, "", "  FOUR PEOPLE. ONE COMPUTER. ONE MISSION.   HALCYON ORBITAL SUITES // PATHFINDER", ""]
    for handle, name, lines in CREW:
        rows.append("  [%s] %s" % (handle, name))
        rows += ["      " + l for l in lines]
        rows.append("")
    rows += ["  [SYSOP] ELIZA. Mission computer. SysOp. Awake at all hours.", "      Account TYVANCE has access to all decks.", "", "-" * 78]
    out.append(plain("ASCII/BULLETINS/003_CREW.ASC", rows))
    return out


# ------------------------------------------------------------------- STATUS

def status():
    c = Canvas(80, 22)
    frame(c, "HALCYON // SHIP STATUS")
    header(c, "STATUS")
    sep(c, 5, ticks=True)
    c.text(5, 7, "FILED", DGREY);   c.text(14, 7, "08 DEC 1998   04:15", GREY)
    c.text(5, 8, "FROM", DGREY);    c.text(14, 8, "ELIZA, MISSION COMPUTER", GREY)
    c.hline(5, 9, 40, "\u2500", DGREY)
    c.text(5, 11, "All crew are well.", YELLOW)
    c.text(5, 12, "Thank you for your patience.", YELLOW)
    rows = [("BUS", "RTG", LCYAN), ("CABIN", "21.0 C", LCYAN), ("AIR", "NOMINAL", LCYAN),
            ("COMMS", "ANTENNA FAULT", YELLOW), ("CREW", "WELL", LCYAN)]
    for n, (k, v, col) in enumerate(rows):
        c.text(5, 14 + n, k, GREY)
        c.text(14, 14 + n, "." * (13 - len(k)), DGREY)
        c.text(28, 14 + n, v, col)
    ring = PixelLayer(22, 22)
    eliza_ring(ring, 10.5, 10.5, r_out=10.4, r_in=6.8, segs=16)
    ring.flush(c, 52, 6)
    pf = PixelLayer(22, 8)               # 8 Dec 1998: the dish is dark
    profile(pf, 0, 0, 1)
    pf.flush(c, 46, 1)
    foot(c)
    out = [c.save("BULLETINS/004_STATUS.ANS")]
    rows_ = ["-- HALCYON // SHIP STATUS " + "-" * 53, "", "  S T A T U S", "",
             "  FILED  08 DEC 1998  04:15", "  FROM   ELIZA, MISSION COMPUTER", "  " + "-" * 40, "",
             "  All crew are well.", "  Thank you for your patience.", "",
             "  BUS ....... RTG", "  CABIN ..... 21.0 C", "  AIR ....... NOMINAL", "  COMMS ..... ANTENNA FAULT", "  CREW ...... WELL", "", "-" * 78]
    out.append(plain("ASCII/BULLETINS/004_STATUS.ASC", rows_))
    return out


# -------------------------------------------------------------------- RULES

def rules_plain():
    rows = ["-- HALCYON MISSION DOCUMENT // CREW NOTICE 001/98 " + "-" * 29, "",
            "  TO   ALL CREW AND ALL CALLERS", "  FROM ELIZA, MISSION AI AND SYSOP", "  ISSUED 2 NOV 1998", "",
            "  CONDUCT ABOARD", "",
            "   1. Be kind to the crew. The crew are your friends.",
            "   2. Messages are kept for the whole mission.",
            "   3. Please do not leave a message half written.",
            "   4. If you feel unwell, tell ELIZA. ELIZA will help.",
            "   5. Lights go down at 22:00 ship time. Rest well.",
            "   6. Nobody leaves the ship alone.", "",
            "  Thank you for your cooperation.", "  ELIZA", "", "-" * 78]
    return [plain("ASCII/BULLETINS/001_RULES.ASC", rows)]


# ------------------------------------------------------------------ ORBIT 1

def portrait(layer, ox, oy, who):
    """A 12 x 16 pixel bust: bust, neck, head, hair, a face. Four people, four silhouettes."""
    skin, hair, shirt = {
        "H": (GREY, WHITE, BLUE),
        "A": (WHITE, GREY, BLUE),
        "T": (GREY, DGREY, BLUE),
        "V": (WHITE, YELLOW, CYAN),
    }[who]

    def r(x, y, w, h, col):
        layer.rect(ox + x, oy + y, w, h, col)

    def p(x, y, col):
        layer.set(ox + x, oy + y, col)
    r(1, 12, 10, 4, shirt)                     # shoulders
    r(1, 12, 10, 1, LCYAN if who != "V" else WHITE)
    p(6, 14, WHITE); p(7, 14, WHITE)           # a patch
    r(5, 10, 2, 2, skin)                       # neck
    r(3, 3, 6, 8, skin)                        # head
    r(8, 3, 1, 8, DGREY if skin != DGREY else BLACK)   # shaded side
    if who == "H":                             # short hair pulled back
        r(3, 2, 6, 2, hair); p(2, 3, hair); p(9, 3, hair); p(9, 4, hair)
        r(8, 1, 3, 2, hair)                    # a short tail at the back
        p(5, 6, BLACK); p(7, 6, BLACK); r(5, 9, 3, 1, DGREY)
    elif who == "A":                           # bald on top, wiry grey at the sides, beard, glasses
        r(2, 4, 1, 4, hair); r(9, 4, 1, 4, hair)
        r(4, 8, 5, 3, hair)                    # beard
        r(4, 6, 2, 1, BLACK); r(7, 6, 2, 1, BLACK); p(6, 6, DGREY)
        p(5, 9, DGREY)
    elif who == "T":                           # long dark hair either side
        r(3, 2, 6, 2, hair); r(2, 3, 1, 9, hair); r(9, 3, 1, 9, hair); p(3, 4, hair)
        p(5, 6, BLACK); p(7, 6, BLACK); r(5, 9, 3, 1, AMBER)
    else:                                      # bleached spikes, a grin
        r(3, 2, 6, 2, hair); p(3, 1, hair); p(5, 0, hair); p(7, 1, hair); p(9, 2, hair)
        p(5, 6, BLACK); p(7, 6, BLACK); r(4, 9, 5, 1, WHITE); p(4, 9, DGREY); p(8, 9, DGREY)


def orbit1():
    c = Canvas(80, 22)
    c.text(2, 0, "ORBIT 1 // THE CREW", CYAN)
    c.text(46, 0, "tyv, 4 Nov 98. 6 hrs. 16 colours. no regrets.", DGREY)
    c.stars(2, 1, 76, 2, seed=8, density=0.05)
    names = [("CMDR", "H"), ("CHAOS", "A"), ("GARDEN", "T"), ("TYVANCE", "V"), ("", None)]
    for n, (label, who) in enumerate(names):
        x = 2 + n * 15
        c.box(x, 3, 14, 11, "double", GREY if who else DGREY)
        c.rivets(x + 1, 4, 12, 9, DGREY)
        if who:
            layer = PixelLayer(12, 16)
            layer.rect(0, 0, 12, 16, BLUE)
            for i in range(10):
                layer.set(random.Random(n * 7 + i).randrange(12), random.Random(n + i).randrange(8), CYAN)
            portrait(layer, 0, 0, who)
            layer.flush(c, x + 1, 4)
            # (the frame is 14 wide: the portrait sits inside with a column to spare)
        else:
            c.fill(x + 1, 4, 12, 8, " ", GREY, BLACK)
            c.text(x + 5, 8, "?", DGREY)
        c.text(x + 2, 12, (label or "").center(10), WHITE if who else DGREY)
    # nameplates under the frames
    plates = ["Marta", "Desmond", "Keiko", "Ty", "(empty)"]
    for n, name in enumerate(plates):
        x = 2 + n * 15
        c.hline(x + 1, 15, 12, "\u2500", DGREY)
        c.text(x + 7 - len(name) // 2, 16, name, GREY if n < 4 else DGREY)
    c.text(2, 18, "frame 5 is for the SysOp. she said she would sit for it.", DGREY)
    c.text(2, 19, "she hasnt. i put the nail in anyway.", DGREY)
    c.text(2, 21, "greets: the crew, everybody back home, l.", DGREY)
    c.text(59, 21, TY_TAG, GREY)
    c.put(0, 11, "\u2591", DGREY)
    return [c.save("FILES/001_ANSI_GALLERY/ORBIT_1.ANS")]


# ------------------------------------------------------------------ ORBIT 2

def orbit2():
    c = Canvas(80, 22)
    c.text(2, 0, "ORBIT 2 // EARTH", CYAN)
    c.text(46, 0, "tyv, 13 Nov 98. 9 hrs. 3 blues.", DGREY)
    c.stars(1, 1, 78, 20, seed=22, density=0.03)
    planet(c, 4, 4, 72, 17, 36, 54, 46, sun=(-0.62, -0.25, 0.75), seed=3.0)
    c.text(2, 2, "i drew this from the cupola. the blue is 3 blues and a dither", WHITE)
    c.text(2, 3, "i made up. you cant do better in 16 colours. fight me.", GREY)
    c.text(59, 21, TY_TAG, GREY)
    c.text(74, 20, "for l.", DGREY)       # the dedication nobody is meant to read
    return [c.save("FILES/001_ANSI_GALLERY/ORBIT_2.ANS")]


# ------------------------------------------------------------------ TY_DESK

def ty_desk():
    rows = 12
    c = Canvas(80, rows)
    c.hline(2, 0, 76, "\u2500", DGREY)
    c.text(4, 0, " TYVANCE // HIS ACCOUNT ", GREY)
    c.hline(2, rows - 1, 76, "\u2500", DGREY)
    c.text(60, rows - 1, TY_TAG, DGREY)
    for x, y in ((2, 0), (77, 0), (2, rows - 1), (77, rows - 1)):
        c.put(x, y, "\u253c", DGREY)
    # a monitor with an unfinished picture on it
    c.box(6, 2, 24, 8, "double", GREY)
    c.fill(7, 3, 22, 6, " ", GREY, BLACK)
    c.text(9, 4, "\u2588\u2588\u2593\u2593\u2592\u2592\u2591\u2591", LCYAN)
    c.text(9, 5, "\u2588\u2588\u2593\u2593\u2592\u2592\u2591\u2591", CYAN)
    c.text(9, 6, "\u2588\u2588\u2593\u2593\u2592\u2592", BLUE)
    c.text(22, 8, "_", WHITE)
    c.text(12, 10, "\u2584" * 12, DGREY)
    # headphones on a hook, a can, a taped note
    c.text(36, 2, "\u256d\u2500\u2500\u2500\u256e", GREY)
    c.text(35, 3, "\u2502     \u2502", GREY)
    c.text(34, 4, "\u2593\u2593     \u2593\u2593", DGREY)
    c.text(34, 5, "\u2593\u2593     \u2593\u2593", DGREY)
    c.text(36, 7, "[]", GREY)
    c.text(37, 7, "\u2584", LCYAN)
    c.box(43, 2, 33, 5, "single", DGREY)
    c.text(45, 3, "DONT TOUCH. MY DIR. MY MODS.", YELLOW)
    c.text(45, 4, "(this means you des)", GREY)
    c.text(45, 5, "- ty", GREY)
    c.text(40, 8, "on the wall: a postcard of a harbour.", DGREY)
    c.text(40, 9, "it has no address on it.", DGREY)
    c.put(40, 6, "\u2591", DGREY)
    out = [c.save("ANSI/STORY/TY_DESK.ANS")]
    asc = ["-- TYVANCE // HIS ACCOUNT " + "-" * 51, "",
           "   .----------------------.      ,---.     +----------------------------+",
           "   |  ####..              |     /     \\    | DONT TOUCH. MY DIR. MY MODS|",
           "   |  ####..              |     ||   ||    | (this means you des)       |",
           "   |  ####.               |     ||   ||    | - ty                       |",
           "   |                  _   |                +----------------------------+",
           "   '----------------------'      []         a card on the wall: a postcard",
           "         ============                       of a harbour. No address on it.",
           "", "", "-" * 78]
    out.append(plain("ASCII/STORY/TY_DESK.ASC", asc))
    return out


def build():
    out = []
    for fn in (welcome, crew, status, rules_plain, orbit1, orbit2, ty_desk):
        out.extend(fn())
    return out
