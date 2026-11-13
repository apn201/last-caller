"""The easter-egg screens (queue item Q11, spec .kiro/specs/eggs/).

ANSI/DOORS/POST.ANS        the 486's power-on self test (hidden command POST)
ANSI/DOORS/SNAKE.ANS       the title of Ty's snake door
ANSI/DOORS/NODE_LUMEN.ANS  the board a hidden phone number answers
ANSI/STORY/TY_DEMO_1..3    the frames of Ty's unfinished demo (scene TY_DEMO)
and their ASCII twins in ASCII/DOORS and ASCII/STORY.

No cursor moves: the text of the door or the scene simply follows each picture.
DRAFT text: every line is a draft. The author makes the final edit.
"""
import math
from _common import *
from act1 import TY_TAG


def ascii_screen(path, rows):
    c = Canvas(80, len(rows))
    for y, r in enumerate(rows):
        c.text(0, y, r)
    return c.save_plain(path)


# ------------------------------------------------------------------ POST

def post():
    c = Canvas(80, 22)
    c.text(1, 0, "Tern BIOS v4.51PG", WHITE)
    c.text(48, 0, "(C) 1993-97 Tern Systems Inc.", DGREY)
    c.hline(1, 1, 78, "\u2500", DGREY)
    c.text(1, 2, "BIOS date 06/14/94", GREY)
    c.text(1, 4, "CPU          : 486DX2 at 66 MHz", GREY)
    c.text(1, 5, "Coprocessor  : installed", GREY)
    c.text(1, 6, "Memory test  :", GREY)
    c.text(16, 6, "8200K", YELLOW)
    c.text(22, 6, "OK", LCYAN)
    c.text(26, 6, "(8192K installed)", DGREY)
    c.text(1, 7, "Fixed disk 0 : 340MB", GREY)
    c.text(1, 8, "Fixed disk 1 : none", GREY)
    c.text(1, 9, "Floppy A     : 1.44MB, no disk", GREY)
    c.text(1, 10, "COM1 3F8     : GPS relay", GREY)
    c.text(1, 11, "COM2 2F8     : modem, 28800, no line", GREY)
    c.text(1, 12, "LPT1 378     : none", GREY)
    c.text(1, 13, "Keyboard     : not detected", GREY)
    # the Turbo switch: on the front panel, and somebody has taped it down
    c.box(54, 3, 24, 5, "single", DGREY)
    c.text(56, 3, " FRONT PANEL ", GREY)
    c.text(57, 4, "TURBO", GREY)
    c.text(65, 4, "[", DGREY)
    c.text(66, 4, "ON ", LCYAN)
    c.text(69, 4, "]", DGREY)
    c.text(57, 5, "66 MHz", DGREY)
    lights(c, 57, 6, "1110", on=LCYAN)
    c.text(66, 6, "taped", DGREY)
    c.text(1, 15, "Press F1 to continue, DEL to enter setup", GREY)
    c.hline(1, 17, 78, "\u2500", DGREY)
    c.text(1, 18, "Booting from C: ...", GREY)
    c.text(1, 19, "HALCYON BBS 486, node 1 of 1", GREY)
    c.text(1, 20, "ELIZA: waiting for a caller.", YELLOW)
    c.text(62, 21, TAG, DGREY)
    c.put(37, 5, "\u2591", DGREY)        # a worn spot
    return [c.save("ANSI/DOORS/POST.ANS"), ascii_screen("ASCII/DOORS/POST.ASC", [
        " Tern BIOS v4.51PG                                  (C) 1993-97 Tern Systems Inc.",
        " " + "-" * 78,
        " BIOS date 06/14/94",
        "                                                      +-- FRONT PANEL ---------+",
        " CPU          : 486DX2 at 66 MHz                      | TURBO [ON ]            |",
        " Coprocessor  : installed                             | 66 MHz                 |",
        " Memory test  : 8200K OK  (8192K installed)           | * * * *  taped         |",
        " Fixed disk 0 : 340MB                                 +------------------------+",
        " Fixed disk 1 : none",
        " Floppy A     : 1.44MB, no disk",
        " COM1 3F8     : GPS relay",
        " COM2 2F8     : modem, 28800, no line",
        " LPT1 378     : none",
        " Keyboard     : not detected",
        "",
        " Press F1 to continue, DEL to enter setup",
        "",
        " " + "-" * 78,
        " Booting from C: ...",
        " HALCYON BBS 486, node 1 of 1",
        " ELIZA: waiting for a caller.",
        "",
    ])]


# ------------------------------------------------------------------ SNAKE

def snake():
    c = Canvas(80, 6)
    title5(c, 4, 0, "SNAKE", [WHITE, LCYAN, LCYAN, CYAN, CYAN], DGREY, gap=2)
    c.text(4, 4, "A GAME BY TYV, FOR THE CREW", GREY)
    c.text(4, 5, "HIGH SCORES ARE KEPT", DGREY)
    # a snake in shade blocks, winding to a star
    body = [(46, 1), (47, 1), (48, 1), (49, 1), (50, 1), (50, 2), (50, 3), (51, 3), (52, 3), (53, 3), (54, 3), (55, 3), (56, 3), (56, 2), (56, 1)]
    for n, (x, y) in enumerate(body):
        c.put(x, y, "\u2593" if n % 3 else "\u2592", CYAN if n > 2 else LCYAN)
    c.put(56, 1, "@", WHITE)
    c.put(60, 1, "*", YELLOW)
    c.text(62, 5, TY_TAG, DGREY)
    c.put(70, 2, "\u2591", DGREY)
    return [c.save("ANSI/DOORS/SNAKE.ANS"), ascii_screen("ASCII/DOORS/SNAKE.ASC", [
        "  SSSS  N   N  AAA  K  K EEEEE                   oooo@",
        "  S     NN  N A   A K K  E                        o   *",
        "  SSSS  N N N AAAAA KK   EEEE                 ooooo",
        "     S  N  NN A   A K K  E",
        "  SSSS  N   N A   A K  K EEEEE",
        "  A GAME BY TYV, FOR THE CREW. HIGH SCORES ARE KEPT",
    ])]


# ------------------------------------------------------------------ the hidden node

def node_lumen():
    c = Canvas(80, 10)
    c.hline(2, 0, 76, "\u2500", DGREY)
    c.text(3, 0, " NODE 1 OF 3 ", CYAN)
    title5(c, 4, 2, "SUBLEVEL 2", [WHITE, LCYAN, LCYAN, CYAN, CYAN], DGREY, gap=1)
    c.text(4, 7, "33.6K  ANSI  MOD  NFO  NO LAMERS", GREY)
    c.text(4, 8, "SYSOP LUMEN  CO-SYSOP SKIT", DGREY)
    # stairs going down, one step lit
    layer = PixelLayer(12, 14)
    for n in range(6):
        layer.rect(n * 2, 2 + n * 2, 12 - n * 2, 2, GREY if n != 3 else WHITE)
        layer.rect(n * 2, 4 + n * 2, 12 - n * 2, 1, DGREY)
    layer.flush(c, 66, 1)
    c.text(62, 9, TAG, DGREY)
    return [c.save("ANSI/DOORS/NODE_LUMEN.ANS"), ascii_screen("ASCII/DOORS/NODE_LUMEN.ASC", [
        "-- NODE 1 OF 3 " + "-" * 62,
        "",
        "  SSSS U   U BBBB  L     EEEEE V   V EEEEE L     2222       ####",
        "  S    U   U B   B L     E     V   V E     L        2         ####",
        "  SSSS U   U BBBB  L     EEEE  V   V EEEE  L      22            ####",
        "     S U   U B   B L     E      V V  E     L     2                ####",
        "  SSSS  UUU  BBBB  LLLLL EEEEE   V   EEEEE LLLLL 22222",
        "  33.6K  ANSI  MOD  NFO  NO LAMERS",
        "  SYSOP LUMEN  CO-SYSOP SKIT",
        "",
    ])]


# ------------------------------------------------------------------ Ty's demo

def demo1():
    """A tunnel: nested frames shrinking to a point, the first thing Ty finished."""
    c = Canvas(80, 12)
    cx, cy = 40, 5
    for n in range(0, 9):
        w, h = 76 - n * 8, 12 - n * 1
        x0, y0 = cx - w // 2, cy - h // 2 + 1
        colour = [DGREY, BLUE, BLUE, CYAN, CYAN, LCYAN, LCYAN, WHITE, WHITE][n]
        if w < 2 or h < 2:
            break
        c.box(x0, y0, w, h, "single", colour)
    c.text_center(5, " TY'S DEMO ", WHITE, x0=0, x1=80)
    c.text(2, 11, "NOT FINISHED", DGREY)
    c.text(62, 11, TY_TAG, DGREY)
    return [c.save("ANSI/STORY/TY_DEMO_1.ANS"), ascii_screen("ASCII/STORY/TY_DEMO_1.ASC", [
        "+" + "-" * 76 + "+",
        "|   +" + "-" * 66 + "+   |",
        "|   |   +" + "-" * 56 + "+   |   |",
        "|   |   |   +" + "-" * 46 + "+   |   |   |",
        "|   |   |   |   +" + "-" * 36 + "+   |   |   |   |",
        "|   |   |   |   |          TY'S DEMO          |   |   |   |   |",
        "|   |   |   |   +" + "-" * 36 + "+   |   |   |   |",
        "|   |   |   +" + "-" * 46 + "+   |   |   |",
        "|   |   +" + "-" * 56 + "+   |   |",
        "|   +" + "-" * 66 + "+   |",
        "+" + "-" * 76 + "+",
        "  NOT FINISHED",
    ])]


def demo2():
    """Plasma bars: sine waves in the cyan ramp."""
    c = Canvas(80, 12)
    ramp_cols = [BLACK, BLUE, CYAN, LCYAN, WHITE]
    for y in range(11):
        for x in range(80):
            v = (math.sin(x / 7.0 + y / 2.3) + math.sin(x / 11.0 - y / 1.7) + 2) / 4.0
            ch, fg, bg = ramp(ramp_cols[:4], v)
            c.put(x, y, ch, fg, bg)
    c.text(2, 11, "FRAME 2   SUNSIDE.MOD", GREY)
    c.text(62, 11, TY_TAG, DGREY)
    return [c.save("ANSI/STORY/TY_DEMO_2.ANS"), ascii_screen("ASCII/STORY/TY_DEMO_2.ASC", [
        ".:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*",
        ":+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#",
        "+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*",
        "*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+",
        "#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:",
        "*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.",
        "+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:. ",
        ":.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.  ",
        ".   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   ",
        "   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .",
        "  .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:",
        "  FRAME 2   SUNSIDE.MOD",
    ])]


def demo3():
    """The same plasma, torn at one row: where the demo stops."""
    c = Canvas(80, 12)
    ramp_cols = [BLACK, BLUE, CYAN, LCYAN]
    for y in range(11):
        shift = 0 if y < 6 else (9 if y == 6 else 0)
        for x in range(80):
            if y > 6:
                c.put(x, y, " ")
                continue
            v = (math.sin((x + shift) / 7.0 + y / 2.3) + math.sin(x / 11.0 - y / 1.7) + 2) / 4.0
            ch, fg, bg = ramp(ramp_cols, v)
            c.put(x, y, ch, fg, bg)
    c.text(2, 8, "FAULT 0D AT 0042:1F3C", LRED)
    c.text(2, 9, "RETURN ADDRESS 0042:1F31", DGREY)
    c.text(2, 11, "FRAME 3", GREY)
    c.text(62, 11, TY_TAG, DGREY)
    return [c.save("ANSI/STORY/TY_DEMO_3.ANS"), ascii_screen("ASCII/STORY/TY_DEMO_3.ASC", [
        ".:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*",
        ":+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#",
        "+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*",
        "*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+",
        "#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:",
        "*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.",
        "   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .:+*#*+:.   .",
        "",
        "  FAULT 0D AT 0042:1F3C",
        "  RETURN ADDRESS 0042:1F31",
        "",
        "  FRAME 3",
    ])]


def build():
    return post() + snake() + node_lumen() + demo1() + demo2() + demo3()
