"""ANSI/DOORS/MANIFEST.ANS, ANSI/STORY/MANFULL, END_SENT, END_CREW, END_PARTIAL
and their ASCII twins: the crew manifest and its endings (S8, Q6).

MANIFEST     header of the manifest screen: the form, stamped UNSENT
MANFULL      ELIZA when all five rows are confirmed (the scene STORY/SCENES/MANFULL.INI)
END_SENT     the ship and the open downlink (ending: the queue is sent)
END_CREW     the crew list with one place filled (ending: the caller stays)
END_PARTIAL  ELIZA thanks the caller for their interest (goodbye, unfinished)

The scene pictures are 12 rows (END_PARTIAL 7), so the text types below them on
an 80x24 screen. No cursor moves. The art never names a crew member: a caller
who has not found them yet must not learn them from the header.
DRAFT text: every line is a draft. The author makes the final edit.
"""
from _common import *

ROWS = 12


def ascii_screen(rel, rows):
    c = Canvas(80, len(rows))
    for y, r in enumerate(rows):
        c.text(0, y, r)
    return c.save_plain(rel)


def frame12(c, label, rows=ROWS):
    """The scene frame (as in scenes.py): a rule on top with the label, one below, rivets, a grille."""
    c.hline(2, 0, 76, "\u2500", DGREY)
    c.text(4, 0, " " + label + " ", GREY)
    c.hline(2, rows - 1, 76, "\u2500", DGREY)
    artist_tag(c, 60, rows - 1)
    for x, y in ((2, 0), (77, 0), (2, rows - 1), (77, rows - 1)):
        c.put(x, y, "\u253c", DGREY)


def leader(c, y, label, value, vcol, x=44, lead_to=14):
    c.text(x, y, label, GREY)
    c.text(x + len(label) + 1, y, "." * (lead_to - len(label) - 1), DGREY)
    c.text(x + lead_to + 1, y, value, vcol)


# ---------------------------------------------------------------- MANIFEST

def manifest():
    rows = 9
    c = Canvas(80, rows)
    c.hline(2, 0, 76, "\u2500", DGREY)
    c.text(3, 0, " HALCYON // CREW MANIFEST ", CYAN)
    c.hline(2, rows - 1, 76, "\u2500", DGREY)
    artist_tag(c, 60, rows - 1)
    title5(c, 4, 2, "MANIFEST", [WHITE, LCYAN, LCYAN, CYAN, CYAN], DGREY, gap=1)
    # the blank form on the right, stamped
    c.text(60, 1, "FORM HC-1  REV 3", DGREY)
    c.hline(60, 2, 16, "\u2500", DGREY)
    for y in (3, 4, 5):
        c.text(60, y, "." * 16, DGREY)
    c.box(62, 6, 12, 2, "single", AMBER)
    c.text(65, 6, "UNSENT", YELLOW)
    c.put(73, 7, "\u2591", DGREY)          # the stamp did not take at the corner
    return [c.save("ANSI/DOORS/MANIFEST.ANS"), ascii_screen("ASCII/DOORS/MANIFEST.ASC", [
        "-- HALCYON // CREW MANIFEST ------------------------------------------------",
        "",
        "  M   M  AAA  N   N III FFFFF  EEEEE  SSSS TTTTT                FORM HC-1  REV 3",
        "  MM MM A   A NN  N  I  F      E     S       T                  ----------------",
        "  M M M AAAAA N N N  I  FFFF   EEEE   SSS    T                  ................",
        "  M   M A   A N  NN  I  F      E         S   T                  ................",
        "  M   M A   A N   N III F      EEEEE SSSS    T                  ...+--------+...",
        "                                                                   | UNSENT |",
        "-" * 78,
    ])]


# ---------------------------------------------------------------- MANFULL

def manfull():
    c = Canvas(80, ROWS)
    ring = PixelLayer(20, 20)
    eliza_ring(ring, 9.5, 9.5, r_out=9.4, r_in=6.0, segs=16, off=(), dim=(), core=True)
    ring.flush(c, 8, 1)
    c.glow(8 + 10, 1 + 5, 10.5, 13.0, AMBER, inner=9.8)
    frame12(c, "HALCYON // MISSION AI")
    grille(c, 70, 9, 6, 1, DGREY, GREY)
    title5(c, 40, 2, "ELIZA", [WHITE, YELLOW, YELLOW, AMBER, AMBER], DGREY)
    c.text(44, 7, "MISSION AI", GREY)
    c.text(55, 7, "RECORD COMPLETE", YELLOW)
    lights(c, 44, 8, "11111", on=YELLOW)
    leader(c, 9, "ROWS", "5 OF 5", LCYAN)
    leader(c, 10, "VOICE", "NORMAL", LCYAN)
    return [c.save("ANSI/STORY/MANFULL.ANS"), ascii_screen("ASCII/STORY/MANFULL.ASC", [
        "-- HALCYON // MISSION AI " + "-" * 51,
        "",
        "       .-\"\"\"\"\"-.",
        "     / o o o o o \\            ELIZA",
        "    | o         o |",
        "    | o    *    o |",
        "    | o         o |          MISSION AI  RECORD COMPLETE",
        "     \\ o o o o o /           o o o o o",
        "       '-.....-'               ROWS ...... 5 OF 5",
        "                               VOICE ..... NORMAL",
        "",
        "-" * 78,
    ])]


# ---------------------------------------------------------------- END_SENT

def end_sent():
    c = Canvas(80, ROWS)
    frame12(c, "HALCYON // DOWNLINK")
    c.stars(3, 1, 74, 9, seed=17, density=0.03, avoid=lambda x, y: x < 34 or x > 46)
    sp = PixelLayer(26, 10)
    ship_mini(sp, 2, 0)
    sp.flush(c, 6, 3)
    # the signal leaves the ship: arcs of growing size, brightest nearest
    for i, (x, col) in enumerate(((33, LCYAN), (36, CYAN), (40, BLUE), (45, DGREY))):
        c.text(x, 5, ")" * 1, col)
        c.text(x, 4 + (i % 2) * 2, ")", col)
        c.text(x, 6 - (i % 2) * 2, ")", col)
    title5(c, 50, 2, "SENT", [WHITE, LCYAN, LCYAN, CYAN, CYAN], DGREY)
    c.text(50, 7, "DOWNLINK", GREY)
    c.text(59, 7, "OPEN", LCYAN)
    bar(c, 50, 8, 20, 1.0, LCYAN, DGREY)
    leader(c, 9, "MESSAGES", "41", LCYAN, x=50, lead_to=12)
    leader(c, 10, "FILES", "3", LCYAN, x=50, lead_to=12)
    c.put(70, 8, "\u2591", DGREY)          # the bar is one block short
    return [c.save("ANSI/STORY/END_SENT.ANS"), ascii_screen("ASCII/STORY/END_SENT.ASC", [
        "-- HALCYON // DOWNLINK " + "-" * 53,
        "",
        "                                                SSSS EEEEE N   N TTTTT",
        "      ====[##]====                     ) )      S     E     NN  N   T",
        "     ====[ ## ]====           )  ) ) )          SSS   EEEE  N N N   T",
        "      ====[##]====                     ) )          S E     N  NN   T",
        "                                                SSSS  EEEEE N   N   T",
        "                                                DOWNLINK  OPEN",
        "                                                [###################]",
        "                                                MESSAGES ..... 41",
        "                                                FILES ........ 3",
        "-" * 78,
    ])]


# ---------------------------------------------------------------- END_CREW

def end_crew():
    c = Canvas(80, ROWS)
    frame12(c, "HALCYON // CREW LIST")
    ring = PixelLayer(14, 14)
    eliza_ring(ring, 6.5, 6.5, r_out=6.3, r_in=4.0, segs=12, off=(), dim=(2,), core=True)
    ring.flush(c, 6, 3)
    c.glow(6 + 7, 3 + 3.5, 6.0, 7.5, AMBER, inner=6.3)
    title5(c, 30, 1, "CREW", [WHITE, LCYAN, LCYAN, CYAN, CYAN], DGREY)
    # four places taken, steady; the fifth is a cursor
    c.text(30, 7, "PLACES", GREY)
    for n in range(4):
        c.put(38 + n * 2, 7, "\u25a0", CYAN)
        c.text(38 + n * 2, 8, str(n + 1), DGREY)
    c.put(46, 7, "\u25a0", YELLOW)
    c.text(46, 8, "5", YELLOW)
    c.text(30, 9, "5  " + "." * 16, GREY)
    c.put(33, 9, "\u2588", YELLOW)         # the cursor waits where the name goes
    c.text(30, 10, "ENTERED BY THE SYSOP", DGREY)
    return [c.save("ANSI/STORY/END_CREW.ANS"), ascii_screen("ASCII/STORY/END_CREW.ASC", [
        "-- HALCYON // CREW LIST " + "-" * 53,
        "",
        "       .-\"\"\"\"\"-.         CCCC RRRR  EEEEE W   W",
        "     / o o o o o \\       C     R   R E     W   W",
        "    | o    *    o |      C     RRRR  EEEE  W W W",
        "    | o         o |      C     R  R  E     WW WW",
        "     \\ o o o o o /        CCCC R   R EEEEE W   W",
        "       '-.....-'          PLACES   * * * * [*]",
        "                          1 2 3 4   5",
        "                          5  ................_",
        "                          ENTERED BY THE SYSOP",
        "-" * 78,
    ])]


# ---------------------------------------------------------------- END_PARTIAL

def end_partial():
    rows = 7
    c = Canvas(80, rows)
    c.hline(2, 0, 76, "\u2500", DGREY)
    c.text(4, 0, " HALCYON // MISSION AI ", GREY)
    c.hline(2, rows - 1, 76, "\u2500", DGREY)
    artist_tag(c, 60, rows - 1)
    ring = PixelLayer(10, 10)
    eliza_ring(ring, 4.5, 4.5, r_out=4.4, r_in=2.8, segs=10, off=(), dim=(6,), core=True)
    ring.flush(c, 6, 1)
    c.text(22, 2, "THANK YOU FOR", AMBER)
    c.text(22, 3, "YOUR INTEREST", AMBER)
    lights(c, 22, 5, "1110", on=YELLOW)
    c.text(32, 5, "MANIFEST SAVED", DGREY)
    return [c.save("ANSI/STORY/END_PARTIAL.ANS"), ascii_screen("ASCII/STORY/END_PARTIAL.ASC", [
        "-- HALCYON // MISSION AI " + "-" * 51,
        "",
        "   .-\"\"\"-.      THANK YOU FOR",
        "  / o o o \\     YOUR INTEREST",
        " | o  *  o |",
        "  \\ o o o /     o o o .   MANIFEST SAVED",
        "-" * 78,
    ])]


def build():
    return manifest() + manfull() + end_sent() + end_crew() + end_partial()
