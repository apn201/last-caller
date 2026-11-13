"""ANSI/DOORS/DIAG.ANS, NOWHERE.ANS and the ASCII/DOORS twins: the two shell doors.

DIAG     ELIZA's diagnostics: the amber ring, a row of status lights, the
         line that says the query codes are in the manual (S4b)
NOWHERE  the data haven's login: a cold rack of drives, one light amber
         (S6b)

Each picture is 12 rows, so the door's text types below it on an 80x24
screen. No cursor moves: the text simply follows.
DRAFT text: every line is a draft. The author makes the final edit.
"""
from _common import *

ROWS = 12


def diag():
    c = Canvas(80, ROWS)
    c.hline(2, 0, 76, "\u2500", DGREY)
    c.text(3, 0, " HALCYON // DIAGNOSTICS ", CYAN)
    title5(c, 4, 2, "DIAG", [WHITE, LCYAN, LCYAN, CYAN, CYAN], None, gap=2)
    c.text(4, 8, "ELIZA ANSWERS SYSTEM QUERIES BY NUMBER", GREY)
    c.text(4, 9, "QUERY CODES ARE IN THE COMMS MANUAL", DGREY)
    lights(c, 4, 11, "1111A111")
    c.text(14, 11, "ALL ANSWERS ARE TRUE", DGREY)
    ring = PixelLayer(14, 14)
    eliza_ring(ring, 6.5, 6.5, r_out=6.3, r_in=4.0, segs=12, off=(), dim=(7,), core=True)
    ring.flush(c, 58, 2)
    c.text(56, 10, "ELIZA", AMBER)
    c.text(62, 10, "MISSION AI", DGREY)
    c.text(63, 11, TAG, DGREY)
    c.put(40, 7, "\u2591", DGREY)          # a worn spot
    return [c.save("ANSI/DOORS/DIAG.ANS"), ascii_screen("ASCII/DOORS/DIAG.ASC", [
        "-- HALCYON // DIAGNOSTICS --------------------------------------------------",
        "",
        "  DIAG                                            .-\"\"\"\"\"-.",
        "                                                / o o o o o \\",
        "                                               | o         o |",
        "                                               | o    *    o |",
        "                                                \\ o o   o o /",
        "  ELIZA ANSWERS SYSTEM QUERIES BY NUMBER          '-.....-'",
        "  QUERY CODES ARE IN THE COMMS MANUAL",
        "",
        "                                                  ELIZA  MISSION AI",
        "  ALL ANSWERS ARE TRUE",
    ])]


def nowhere():
    c = Canvas(80, ROWS)
    c.hline(2, 0, 76, "\u2500", DGREY)
    c.text(3, 0, " NOWHERE DATA HAVEN ", CYAN)
    title5(c, 4, 2, "NOWHERE", [WHITE, LCYAN, LCYAN, CYAN, CYAN], None, gap=1)
    c.text(4, 8, "OFF-SITE STORAGE FOR CLIENTS WHO", GREY)
    c.text(4, 9, "PREFER NOT TO BE FOUND", GREY)
    c.text(4, 11, "Your data is NOWHERE.", DGREY)
    # a rack of drives: grilles with one light each, the third amber
    for n in range(4):
        y = 2 + n * 2
        c.box(52, y, 24, 2, "single", DGREY)
        c.text(54, y, "\u2591" * 14, DGREY)
        c.put(71, y, "\u25a0", AMBER if n == 2 else LCYAN)
        c.text(73, y, "%02d" % (n + 1), DGREY)
    c.text(52, 10, "UNIT 03", DGREY)
    c.text(60, 10, "NO RESPONSE", AMBER)
    c.text(63, 11, TAG, DGREY)
    return [c.save("ANSI/DOORS/NOWHERE.ANS"), ascii_screen("ASCII/DOORS/NOWHERE.ASC", [
        "-- NOWHERE DATA HAVEN ------------------------------------------------------",
        "",
        "  N   N  OOO  W   W H   H EEEEE RRRR  EEEEE          +----------------------+",
        "  NN  N O   O W   W H   H E     R   R E              | ############## [] 01 |",
        "  N N N O   O W W W HHHHH EEEE  RRRR  EEEE           +----------------------+",
        "  N  NN O   O WW WW H   H E     R  R  E              | ############## [] 02 |",
        "  N   N  OOO  W   W H   H EEEEE R   R EEEEE          +----------------------+",
        "  OFF-SITE STORAGE FOR CLIENTS WHO                   | ############## [] 03 |",
        "  PREFER NOT TO BE FOUND                              +----------------------+",
        "                                                     UNIT 03  NO RESPONSE",
        "",
        "  Your data is NOWHERE.",
    ])]


def build():
    return diag() + nowhere()
