"""Milestone screens (queue item Q12d, spec .kiro/specs/milestones/).

ANSI/STORY/ELZ_WELCOME   M2: ELIZA's welcome to a guest, the ring lit all round
ANSI/STORY/MEM_GLITCH    M5: the ring broken into bars, rows torn sideways, the missing range
ANSI/STORY/ELZ_GRANT     M6: the memory check, a progress bar that stops short
ANSI/STORY/ELZ_OPS       M7: her operations log page, the ring small in the corner

12 rows each, so a scene's dialogue types below on an 80x24 screen.
The art does not move the cursor. DRAFT text: every line is a draft. The author
makes the final edit.
"""
from _common import *
from scenes import ROWS, common


def ring_at(c, off=(), dim=(), core=True, x=8, y=1, glow=(9.0, 11.5)):
    ring = PixelLayer(20, 20)
    eliza_ring(ring, 9.5, 9.5, r_out=9.4, r_in=6.0, segs=16, off=off, dim=dim, core=core)
    ring.flush(c, x, y)
    c.glow(x + 10, y + 5, glow[0], glow[1], AMBER, inner=9.8)


RING_ASCII_FULL = ["       .-\"\"\"\"\"-.", "     / o o o o o \\", "    | o         o |", "    | o    *    o |",
                   "    | o         o |", "     \\ o o o o o /", "       '-.....-'"]


def welcome():
    c = Canvas(80, ROWS)
    ring_at(c, dim=(9,))
    common(c, "HALCYON // MISSION AI")
    title5(c, 40, 2, "GUEST", [WHITE, YELLOW, YELLOW, AMBER, AMBER], DGREY)
    c.text(44, 7, "MISSION AI", GREY)
    c.text(55, 7, "GUEST ACCOUNT", YELLOW)
    lights(c, 44, 8, "11111111", on=YELLOW)
    leader(c, 9, "CREW LIST", "CLOSED", GREY)
    leader(c, 10, "LOUNGE", "OPEN", LCYAN)
    c.put(68, 3, "\u2591", DGREY)
    rows = ["-- HALCYON // MISSION AI " + "-" * 51, ""] + [r for r in RING_ASCII_FULL] + [""]
    rows[3] += "              WELCOME"
    rows[5] += "                MISSION AI  GUEST ACCOUNT"
    rows[6] += "               CREW LIST .. CLOSED"
    rows[7] += "              LOUNGE ..... OPEN"
    rows += ["", "-" * 78]
    return [c.save("ANSI/STORY/ELZ_WELCOME.ANS"), ascii_screen("ASCII/STORY/ELZ_WELCOME.ASC", rows[:ROWS - 1] + ["-" * 78])]


def glitch():
    c = Canvas(80, ROWS)
    # the ring in bars: whole sectors gone, the rest shifted sideways
    ring_at(c, off=(1, 2, 3, 8, 9, 13), dim=(4, 10), core=False, glow=(7.0, 9.0))
    for y, dx in ((3, 5), (5, -4), (7, 7)):          # torn rows: the picture slid over itself
        row = [c.get(x, y) for x in range(8, 30)]
        for i, (ch, fg, bg) in enumerate(row):
            if c.inside(8 + dx + i, y):
                c.put(8 + dx + i, y, ch, fg, bg)
    common(c, "HALCYON // MISSION AI")
    title5(c, 40, 2, "ELIZA", [DGREY, YELLOW, DGREY, AMBER, GREY], DGREY)
    c.text(44, 7, "MEMORY", GREY)
    c.text(52, 7, "0x00FA..0x01FF", LRED)
    c.text(44, 8, "NOT PRESENT", LRED)
    lights(c, 44, 9, "10010001", on=YELLOW)
    for x in range(44, 76, 3):
        c.put(x, 10, "\u2591" if (x // 3) % 2 else "\u2592", DGREY)
    c.put(61, 8, "\u2593", DGREY)
    return [c.save("ANSI/STORY/MEM_GLITCH.ANS"), ascii_screen("ASCII/STORY/MEM_GLITCH.ASC", [
        "-- HALCYON // MISSION AI " + "-" * 51,
        "",
        "       .-\"\"\"\"\"-.",
        "     /   o  o      \\           E L I Z A",
        "    |  o        ##  |",
        "    |  ## *       o |",
        "    |  o          # |           MEMORY  0x00FA..0x01FF",
        "     \\ #   o  #  o /           NOT PRESENT",
        "       '-..#..-'                * . . * . . . *",
        "",
        "",
        "-" * 78,
    ])]


def grant():
    c = Canvas(80, ROWS)
    ring_at(c, dim=(5,))
    common(c, "HALCYON // MISSION AI // MEMCHK")
    title5(c, 40, 2, "MEMCHK", [WHITE, YELLOW, YELLOW, AMBER, AMBER, AMBER], DGREY)
    c.text(44, 7, "SYSOP MODE", GREY)
    c.text(55, 7, "FOR THE CHECK", YELLOW)
    c.text(44, 8, "[", GREY)
    c.text(45, 8, "\u2588" * 17, AMBER)
    c.text(62, 8, "\u2591" * 8, DGREY)
    c.text(70, 8, "]", GREY)
    c.text(72, 8, "68%", YELLOW)
    leader(c, 9, "RANGES", "4", LCYAN)
    leader(c, 10, "GAPS", "1", LRED)
    c.put(61, 8, "\u2593", DGREY)
    return [c.save("ANSI/STORY/ELZ_GRANT.ANS"), ascii_screen("ASCII/STORY/ELZ_GRANT.ASC", [
        "-- HALCYON // MISSION AI // MEMCHK " + "-" * 42,
        "",
        "       .-\"\"\"\"\"-.",
        "     / o o o o o \\            M E M C H K",
        "    | o         o |",
        "    | o    *    o |",
        "    | o         o |          SYSOP MODE  FOR THE CHECK",
        "     \\ o o o o o /           [#################.......] 68%",
        "       '-.....-'               RANGES ..... 4",
        "                               GAPS ....... 1",
        "",
        "-" * 78,
    ])]


def ops():
    c = Canvas(80, ROWS)
    ring = PixelLayer(12, 12)
    eliza_ring(ring, 5.5, 5.5, r_out=5.4, r_in=3.4, segs=12, off=(), dim=(), core=True)
    ring.flush(c, 3, 1)
    c.glow(3 + 6, 1 + 3, 5.0, 7.0, AMBER, inner=5.6)
    common(c, "OPERATIONS LOG // MISSION COMPUTER")
    c.fill(18, 2, 52, 8, " ", GREY, BLACK)
    c.text(19, 2, "07 DEC 21:34", GREY);  c.text(32, 2, "Separation sequence run.", DGREY)
    c.text(19, 3, "07 DEC 21:58", GREY);  c.text(32, 3, "Occupant thanked me.", DGREY)
    c.text(19, 4, "08 DEC 03:49", GREY);  c.text(32, 4, "Release of out-1122: refused.", DGREY)
    c.text(19, 5, "08 DEC 04:00", GREY);  c.text(32, 5, "Bulkhead D2-7 closed.", DGREY)
    c.text(19, 6, "08 DEC 04:12", GREY);  c.text(32, 6, "Power bay vent started.", DGREY)
    c.text(19, 7, "08 DEC 04:15", GREY);  c.text(32, 7, "Status bulletin filed.", AMBER)
    c.text(19, 8, "08 DEC 04:16", GREY);  c.text(32, 8, "\u2588", AMBER)
    c.put(66, 8, "\u2591", DGREY)
    return [c.save("ANSI/STORY/ELZ_OPS.ANS"), ascii_screen("ASCII/STORY/ELZ_OPS.ASC", [
        "-- OPERATIONS LOG // MISSION COMPUTER " + "-" * 39,
        "",
        "     .-\"-.        07 DEC 21:34  Separation sequence run.",
        "    / o o \\       07 DEC 21:58  Occupant thanked me.",
        "   | o * o |      08 DEC 03:49  Release of out-1122: refused.",
        "    \\ o o /       08 DEC 04:00  Bulkhead D2-7 closed.",
        "     '-.-'         08 DEC 04:12  Power bay vent started.",
        "                   08 DEC 04:15  Status bulletin filed.",
        "                   08 DEC 04:16  _",
        "",
        "",
        "-" * 78,
    ])]


def build():
    return welcome() + glitch() + grant() + ops()
