"""ANSI/DOORS/VOLAMUD.ANS: the deck plan header of the VolaMUD door.

Shown by mudMenu() when a caller enters the door; the room description and
the command list follow on the lines below, so this is 16 rows with the
door's original intro text kept at the bottom. VolaMUD becomes the ship walk
later. DRAFT text: room names are drafts.
"""
from _common import *

INTRO = [
    "",
    "You arrive at Vola City, where the fixed streets meet an endless shared map.",
    "The city has its temple, tinker, academy, armory, tavern, archive and bank.",
    "Outside the lamps, one seed makes the same wilderness for every traveller.",
    "",
    "Type LOOK. TALK to locals. TASK takes work. QUIT returns to the board.",
]


def build():
    c = Canvas(80, 16)
    c.box(0, 0, 80, 10, "single", DGREY)
    c.rivets(1, 1, 78, 8, DGREY)
    c.text(3, 0, "┤", DGREY)
    c.text(4, 0, " DECK PLAN // HALCYON ONLINE WORLD ", CYAN)
    c.text(39, 0, "├", DGREY)
    header(c, "VOLAMUD")
    c.text(49, 2, "FRAME 04-C", DGREY)
    c.text(49, 3, "PLAN NOT TO SCALE", DGREY)

    # the plan: one long deck, five compartments
    xs = [3, 17, 33, 47, 62, 76]
    names = ["BRIDGE", "HAB", "LABORATORY", "GALLEY", "STORES"]
    top, bot = 5, 8
    c.hline(xs[0], top, xs[-1] - xs[0] + 1, "═", GREY)
    c.hline(xs[0], bot, xs[-1] - xs[0] + 1, "═", GREY)
    c.put(xs[0], bot, "╚", GREY); c.put(xs[-1], bot, "╝", GREY)
    for x in xs[1:-1]:
        c.put(x, bot, "╩", GREY)
    c.put(xs[0], top, "╔", GREY); c.put(xs[-1], top, "╗", GREY)
    for i in range(1, len(xs) - 1):
        c.put(xs[i], top, "╦", GREY)
    for i, x in enumerate(xs):
        c.vline(x, top + 1, 2, "║", GREY)
    for i, n in enumerate(names):
        mid = (xs[i] + xs[i + 1]) // 2
        c.text(mid - len(n) // 2 + 1, top + 1, n, GREY if n != "HAB" else LCYAN)
    # doorways between compartments: gaps in the walls
    for x in xs[1:-1]:
        c.put(x, top + 2, " ")
    # you are here
    c.text(xs[1] + 3, top + 2, "● YOU", YELLOW)
    c.text(xs[3] + 4, top + 2, "░░░░", DGREY)
    c.text(xs[4] + 3, top + 2, "· · ·", DGREY)

    for n, line in enumerate(INTRO):
        c.text(0, 10 + n, line, GREY)
    c.put(61, 1, " ")
    return [c.save("ANSI/DOORS/VOLAMUD.ANS")]
