"""ANSI/MENUS/FILES.ANS: the data storage bay, tape racks.

Header art only: the board prints the file areas on the lines below it
(src/bbs/board.h fileMenu), so this screen is 12 rows and ends in a closed frame.
DRAFT text: the rack readout is a draft.
"""
from _common import *


def tape(c, x, y, label, body=GREY):
    c.text(x, y, "▄▄▄▄▄", body)
    c.put(x, y + 1, "█", body)
    c.put(x + 4, y + 1, "█", body)
    for i in range(1, 4):
        c.put(x + i, y + 1, "▓", label, BLACK)


def build():
    c = Canvas(80, 12)
    frame(c, "HALCYON // DATA STORAGE BAY")
    header(c, "DATA BAY")
    c.text(65, 1, "BAY 3", GREY)
    lights(c, 65, 2, "1111", on=LCYAN)
    c.text(65, 3, "ARCHIVE", DGREY)
    sep(c, 4, ticks=True)

    labels = [LCYAN, CYAN, LCYAN, GREY, CYAN, LCYAN, YELLOW, CYAN, LCYAN, GREY]
    for shelf, y in enumerate((5, 8)):
        for n in range(10):
            if shelf == 1 and n == 6:
                continue                       # an empty slot
            drop = 1 if (shelf == 0 and n == 3) else 0   # one tape pulled out
            if shelf == 0 and n == 3:
                c.text(4 + n * 6, y, "     ", GREY)
            lab = labels[(n + shelf * 3) % len(labels)]
            tape(c, 4 + n * 6, y + drop, lab)
        c.hline(2, y + 2, 63, "═", DGREY)
        for n in range(11):
            c.put(3 + n * 6, y + 2, "╤", DGREY)
    c.text(67, 5, "SLOTS", DGREY)
    c.text(67, 6, "19 / 20", GREY)
    c.text(67, 8, "MOUNTED", DGREY)
    c.text(67, 9, "ALL", LCYAN)
    c.text(59, 11, "╡", DGREY)
    c.text(60, 11, TAG, DGREY)
    c.text(60 + len(TAG), 11, "╞", DGREY)
    return [c.save("ANSI/MENUS/FILES.ANS")]
