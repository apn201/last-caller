"""ANSI/STORY/ELIZA_WAKE_*.ANS and ASCII/STORY/ELIZA_WAKE_*.ASC: the portraits
of the demo scene (STORY/SCENES/ELIZA_WAKE.INI).

ELIZA_WAKE_1  the amber ring half awake: three lights, the rest dark
ELIZA_WAKE_2  the ring lit all round, "listening"

Each picture is 12 rows, so the scene's dialogue types below it on an
80x24 screen. The art does not move the cursor: the dialogue simply follows.
DRAFT text: every line is a draft. The author makes the final edit.
"""
from _common import *

ROWS = 12


def common(c, label):
    """The frame: a rule on top with the label, a rule below, rivets, a grille."""
    scene_frame(c, label, ROWS)
    grille(c, 70, 9, 6, 1, DGREY, GREY)
    c.stars(3, 1, 5, 10, seed=12, density=0.12)


def wake1():
    c = Canvas(80, ROWS)
    ring = PixelLayer(20, 20)
    # sixteen segments: three lit, one of them dim, the rest dark
    eliza_ring(ring, 9.5, 9.5, r_out=9.4, r_in=6.0, segs=16,
               off=tuple(i for i in range(16) if i not in (3, 4, 11)), dim=(11,), core=False)
    ring.flush(c, 8, 1)
    c.glow(8 + 10, 1 + 5, 9.0, 11.5, AMBER, inner=9.8)
    common(c, "HALCYON // MISSION AI")   # after the halo, so the frame stays clean
    title5(c, 40, 2, "ELIZA", [YELLOW, YELLOW, AMBER, AMBER, AMBER], DGREY)
    c.text(44, 7, "MISSION AI", GREY)
    c.text(55, 7, "STARTING", YELLOW)
    lights(c, 44, 8, "1A000000", on=YELLOW)
    leader(c, 9, "BUS", "RTG", LCYAN)
    leader(c, 10, "VOICE", "LOW", YELLOW)
    c.put(69, 7, "\u2591", DGREY)          # a missing block: the label is peeling
    return [c.save("ANSI/STORY/ELIZA_WAKE_1.ANS"), ascii_screen("ASCII/STORY/ELIZA_WAKE_1.ASC", [
        "-- HALCYON // MISSION AI " + "-" * 51,
        "",
        "       .-\"\"\"\"\"-.",
        "     /   o       \\            ELIZA",
        "    |  o           |",
        "    |             o |",
        "    |                |         MISSION AI  STARTING",
        "     \\              /          BUS ....... RTG",
        "       '-.....-'               VOICE ..... LOW",
        "",
        "",
        "-" * 78,
    ])]


def wake2():
    c = Canvas(80, ROWS)
    ring = PixelLayer(20, 20)
    eliza_ring(ring, 9.5, 9.5, r_out=9.4, r_in=6.0, segs=16, off=(), dim=(9,), core=True)
    ring.flush(c, 8, 1)
    c.glow(8 + 10, 1 + 5, 10.5, 13.0, AMBER, inner=9.8)
    common(c, "HALCYON // MISSION AI")
    title5(c, 40, 2, "ELIZA", [WHITE, YELLOW, YELLOW, AMBER, AMBER], DGREY)
    c.text(44, 7, "MISSION AI", GREY)
    c.text(55, 7, "LISTENING", YELLOW)
    lights(c, 44, 8, "11111111", on=YELLOW)
    leader(c, 9, "BUS", "RTG", LCYAN)
    leader(c, 10, "VOICE", "NORMAL", LCYAN)
    return [c.save("ANSI/STORY/ELIZA_WAKE_2.ANS"), ascii_screen("ASCII/STORY/ELIZA_WAKE_2.ASC", [
        "-- HALCYON // MISSION AI " + "-" * 51,
        "",
        "       .-\"\"\"\"\"-.",
        "     / o o o o o \\            ELIZA",
        "    | o         o |",
        "    | o    *    o |",
        "    | o         o |          MISSION AI  LISTENING",
        "     \\ o o o o o /           BUS ....... RTG",
        "       '-.....-'               VOICE ..... NORMAL",
        "",
        "",
        "-" * 78,
    ])]


def build():
    return wake1() + wake2()
