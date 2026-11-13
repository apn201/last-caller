"""HALCYON in profile, three times, as the story goes on (Q13 art pass 3)."""
from _common import *


def profile(layer, ox, oy, stage=0):
    """22 x 8 pixels. stage 0: 2 Nov 1998, everything lit. stage 1: 8 Dec 1998,
    the dish is dark (ANTENNA FAULT) and one wing cell has gone. stage 2: 2026,
    the dish and half the lower wing are gone, the windows are dark but one,
    and a piece floats off the stern."""
    def rect(x, y, w, h, col):
        layer.rect(ox + x, oy + y, w, h, col)

    def px(x, y, col):
        layer.set(ox + x, oy + y, col)

    # wings, seen from the side: a lit upper wing, a shaded lower wing
    rect(5, 0, 9, 2, BLUE)
    rect(5, 0, 9, 1, LCYAN if stage == 0 else CYAN)
    for gx in (7, 9, 11, 13):
        px(gx, 1, CYAN)
    rect(5, 7, 9 if stage < 2 else 4, 1, CYAN if stage < 2 else BLUE)
    px(8, 2, GREY)
    px(12, 2, GREY)
    px(8, 6, DGREY)
    if stage < 2:
        px(12, 6, DGREY)
    if stage >= 1:
        px(10, 1, BLACK)
    if stage >= 2:
        px(9, 0, BLACK)
        px(11, 1, BLACK)
    # hab cylinder
    rect(4, 3, 10, 1, WHITE if stage < 2 else GREY)
    rect(4, 4, 10, 1, GREY if stage < 2 else DGREY)
    rect(4, 5, 10, 1, DGREY)
    px(4, 3, GREY)
    px(13, 3, GREY)
    for wx in (6, 8, 10, 12):
        px(wx, 4, BLACK)
    if stage < 2:
        px(8, 4, YELLOW)
    else:
        px(10, 4, YELLOW)
    # docking node
    rect(1, 3, 3, 1, GREY)
    rect(1, 4, 3, 1, DGREY)
    px(0, 3, DGREY)
    px(1, 3, LCYAN if stage < 2 else GREY)
    # service module, engine bell
    rect(14, 3, 4, 1, GREY)
    rect(14, 4, 4, 1, GREY)
    rect(14, 5, 4, 1, DGREY)
    px(15, 4, CYAN if stage < 2 else DGREY)
    rect(18, 3, 2, 3, DGREY)
    px(20, 2, DGREY)
    px(20, 6, DGREY)
    # mast and dish
    px(15, 2, GREY)
    if stage < 2:
        px(14, 1, GREY)
        px(15, 1, WHITE if stage == 0 else DGREY)
        if stage == 0:
            px(16, 1, GREY)
            px(15, 0, LCYAN)
    if stage >= 2:
        px(20, 7, DGREY)       # a piece that is no longer attached
