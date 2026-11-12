"""ANSI/POWER/*.ANS and ASCII/POWER/*.ASC: the power prologue.

BOOT     brownout boot banner, a BIOS count with the ship's faults (first connection)
REBOOT   the same, short (every later connection)
SUNSIDE  "SUNSIDE ENDS - CARRIER LOST", shown as a session is dropped
CONSOLE  ship systems console, solar bus only
CONSOLE_ON  the same once the RTG is on the bus
WARNING  the RTG switch warning (the board prints the confirming key under it)
WAKE     ELIZA wakes: the amber ring, her status

This is engineering equipment, not Ty's work, so there is no artist tag.
DRAFT text: every line is a draft. The author makes the final edit.
Red is used once, for the dead battery bank: a fault.
"""
from _common import *

FULL, DARK, MID, LIGHT = "\u2588", "\u2593", "\u2592", "\u2591"
UPPER, LOWER = "\u2580", "\u2584"


def ascii_screen(name, rows):
    """The plain twin for callers without ANSI: 7-bit text, 80 columns."""
    c = Canvas(80, len(rows))
    for y, r in enumerate(rows):
        c.text(0, y, r)
    return c.save_plain("ASCII/POWER/%s.ASC" % name)


def leader(c, y, label, value, vcol, x=3, lead_to=24, lcol=GREY):
    c.text(x, y, label, lcol)
    c.text(x + len(label) + 1, y, "." * (lead_to - len(label) - 1), DGREY)
    c.text(x + lead_to + 1, y, value, vcol)


# ---------------------------------------------------------------- BOOT

def boot():
    c = Canvas(80, 22)
    c.text(2, 0, "HALCYON-486 FLIGHT BIOS  REV 2.04  14 MAR 1997", WHITE)
    c.text(56, 0, "ABOVE IT ALL HOLDINGS", DGREY)
    c.hline(2, 1, 76, "\u2500", DGREY)
    c.text(2, 2, "CPU 486DX2-66   NPU PRESENT   CACHE 8K   BUS 33MHZ", GREY)
    # the memory count
    c.text(2, 4, "MEMORY TEST", GREY)
    c.text(14, 4, "16384K OK", WHITE)
    c.text(24, 4, FULL * 26, CYAN)
    c.text(50, 4, MID, DGREY)               # the count did not quite finish
    c.text(51, 4, LIGHT * 4, DGREY)
    c.text(2, 5, "EXTENDED 15360K   BASE 640K   SHADOW RAM ENABLED", DGREY)

    leader(c, 7, "SOLAR BUS", "DIRECT", LCYAN)
    leader(c, 8, "BATTERY BANK", "OPEN CIRCUIT", LRED)
    leader(c, 9, "RTG-1", "DISCONNECTED - MANUAL", YELLOW)
    leader(c, 10, "ATTITUDE CONTROL", "OFFLINE", YELLOW)
    leader(c, 11, "PAYLOAD BUS", "LOW DRAW", GREY)
    leader(c, 12, "MODEM", "28800 READY", GREY)
    leader(c, 13, "ELIZA MISSION AI", "NOT RESPONDING", AMBER)
    c.put(2, 12, " ")

    # a dark ELIZA: the ring is there, nothing is lit but one light going out
    ring = PixelLayer(14, 14)
    eliza_ring(ring, 6.5, 6.5, r_out=6.3, r_in=4.0, segs=12, off=tuple(range(12)), dim=(3,), core=False)
    ring.flush(c, 62, 7)

    c.text(2, 15, "/////", YELLOW)
    c.text(8, 15, "SOLAR BUS ONLY - ATTITUDE CONTROL OFFLINE", YELLOW)
    c.text(53, 15, "/////", YELLOW)

    # the service label stuck to the case, a little crooked
    c.box(3, 17, 40, 5, "single", GREY)
    c.text(5, 17, " SERVICE LABEL ", WHITE)
    c.text(5, 18, "MAINT ACCT   ADMIN", GREY)
    c.text(5, 19, "PASSWORD     DEFAULT", GREY)
    c.text(5, 20, "(CHANGE ON FIRST USE)", DGREY)
    c.put(41, 21, "\u2591", DGREY)        # the corner is peeling
    c.text(46, 19, "MAINT ACCT DEFAULTS", LCYAN)
    c.text(46, 20, "NOT CHANGED - SEE LABEL", LCYAN)
    c.put(44, 19, "<", LCYAN)
    return [c.save("ANSI/POWER/BOOT.ANS"), ascii_screen("BOOT", [
        "HALCYON-486 FLIGHT BIOS  REV 2.04  14 MAR 1997        ABOVE IT ALL HOLDINGS",
        "-" * 76,
        "CPU 486DX2-66   NPU PRESENT   CACHE 8K   BUS 33MHZ",
        "",
        "MEMORY TEST  16384K OK  [##########################]",
        "EXTENDED 15360K   BASE 640K   SHADOW RAM ENABLED",
        "",
        "SOLAR BUS ........... DIRECT",
        "BATTERY BANK ........ OPEN CIRCUIT",
        "RTG-1 ............... DISCONNECTED - MANUAL",
        "ATTITUDE CONTROL .... OFFLINE",
        "PAYLOAD BUS ......... LOW DRAW",
        "MODEM ............... 28800 READY",
        "ELIZA MISSION AI .... NOT RESPONDING",
        "",
        "///// SOLAR BUS ONLY - ATTITUDE CONTROL OFFLINE /////",
        "",
        "+--- SERVICE LABEL ------------------+",
        "| MAINT ACCT   ADMIN                 |   MAINT ACCT DEFAULTS",
        "| PASSWORD     DEFAULT               |   NOT CHANGED - SEE LABEL",
        "+------------------------------------+",
    ])]


# ---------------------------------------------------------------- REBOOT

def reboot():
    c = Canvas(80, 9)
    c.text(2, 0, "HALCYON-486 FLIGHT BIOS  REV 2.04", WHITE)
    c.text(2, 1, "MEMORY TEST  16384K OK", GREY)
    c.text(2, 3, "RESTART AFTER POWER LOSS", DGREY)
    c.text(2, 4, "SOLAR BUS ONLY - ATTITUDE CONTROL OFFLINE", YELLOW)
    leader(c, 6, "ELIZA MISSION AI", "NOT RESPONDING", AMBER, x=2)
    c.text(2, 7, "MAINT ACCT DEFAULTS NOT CHANGED - SEE LABEL", LCYAN)
    return [c.save("ANSI/POWER/REBOOT.ANS"), ascii_screen("REBOOT", [
        "HALCYON-486 FLIGHT BIOS  REV 2.04",
        "MEMORY TEST  16384K OK",
        "",
        "RESTART AFTER POWER LOSS",
        "SOLAR BUS ONLY - ATTITUDE CONTROL OFFLINE",
        "",
        "ELIZA MISSION AI .... NOT RESPONDING",
        "MAINT ACCT DEFAULTS NOT CHANGED - SEE LABEL",
    ])]


# ---------------------------------------------------------------- SUNSIDE

def sunside():
    c = Canvas(80, 8)
    # the day side slides away: a gradient band that goes dark from left to right
    for x in range(80):
        t = x / 79.0
        ch = FULL if t < 0.18 else DARK if t < 0.34 else MID if t < 0.5 else LIGHT if t < 0.66 else ""
        if ch and (x % 7 != 3 or t < 0.3):
            c.put(x, 1, ch, YELLOW if t < 0.34 else AMBER if t < 0.5 else DGREY)
    c.put(61, 1, ".", DGREY)
    c.text_center(3, "SUNSIDE ENDS - CARRIER LOST", LCYAN)
    c.text(30, 4, "LINK", DGREY)
    c.text(35, 4, "\u2592\u2591\u2591       0%", DGREY)
    c.text(1, 6, "NO CARRIER", DGREY)
    c.put(58, 6, "\u2591", DGREY)
    return [c.save("ANSI/POWER/SUNSIDE.ANS"), ascii_screen("SUNSIDE", [
        "####====----....",
        "",
        "                    SUNSIDE ENDS - CARRIER LOST",
        "                              LINK ...  0%",
        "",
        "NO CARRIER",
    ])]


# ---------------------------------------------------------------- CONSOLE

def console(powered):
    c = Canvas(80, 20)
    frame(c, "HALCYON // SHIP SYSTEMS")
    title5(c, 4, 1, "SHIP", [WHITE, LCYAN, LCYAN, CYAN, CYAN], None, gap=2)
    c.text(30, 2, "ENGINEERING CONSOLE", GREY)
    c.text(30, 3, "MAINTENANCE ACCOUNT", DGREY)
    c.hline(30, 4, 46, "\u2500", DGREY)

    sp = PixelLayer(24, 12)
    ship_mini(sp, 1, 1)
    sp.flush(c, 4, 6)
    # the bus feed under the ship
    c.text(4, 12, "SOLAR INPUT", DGREY)
    bar(c, 4, 13, 22, 1.0 if powered else 0.38, fg=LCYAN if powered else CYAN)

    p = panel(c, 30, 5, 46, 12, "SUBSYSTEMS", accent=LCYAN)
    rows = [
        ("POWER BUS", "RTG" if powered else "SOLAR DIRECT", "1" if powered else "A"),
        ("ATTITUDE", "HOLD" if powered else "TUMBLING", "1" if powered else "R"),
        ("PAYLOAD", "POWERED" if powered else "LOW DRAW", "1"),
        ("COMMS", "DOWNLINK DISABLED", "0"),
    ]
    for n, (name, value, light) in enumerate(rows):
        y = 7 + n * 2
        c.text(33, y, name, GREY)
        lights(c, 44, y, light)
        c.text(47, y, value, WHITE if light in "1" else YELLOW if light == "A" else LRED if light == "R" else DGREY)
    c.text(33, 15, "RTG-1", DGREY)
    c.text(44, 15, "CONNECTED" if powered else "DISCONNECTED - MANUAL", LCYAN if powered else YELLOW)
    c.hline(31, 14, 44, "\u2500", DGREY)
    grille(c, 66, 6, 8, 1, DGREY, GREY)

    c.text(4, 18, "TYPE HELP FOR COMMANDS", GREY)
    c.text(60, 18, "BUS %s" % ("RTG" if powered else "SOLAR"), DGREY)
    name = "CONSOLE_ON" if powered else "CONSOLE"
    rows_ascii = [
        "+-- HALCYON // SHIP SYSTEMS -------------------------------------------------+",
        "",
        "  SHIP                   ENGINEERING CONSOLE",
        "                         MAINTENANCE ACCOUNT",
        "  ----------------------------------------------------------------------------",
        "",
        "  SUBSYSTEMS",
        "  POWER BUS ...... %s" % ("RTG" if powered else "SOLAR DIRECT"),
        "  ATTITUDE ....... %s" % ("HOLD" if powered else "TUMBLING"),
        "  PAYLOAD ........ %s" % ("POWERED" if powered else "LOW DRAW"),
        "  COMMS .......... DOWNLINK DISABLED",
        "  RTG-1 .......... %s" % ("CONNECTED" if powered else "DISCONNECTED - MANUAL"),
        "",
        "  TYPE HELP FOR COMMANDS",
    ]
    return [c.save("ANSI/POWER/%s.ANS" % name), ascii_screen(name, rows_ascii)]


# ---------------------------------------------------------------- WARNING

def warning():
    c = Canvas(80, 11)
    hazard(c, 0, 0, 80, 1)
    c.text(3, 2, "RTG-1  BUS TIE", WHITE)
    c.text(3, 3, "STATE", DGREY)
    c.text(9, 3, "OPEN", YELLOW)
    c.text(3, 4, "REARM", DGREY)
    c.text(9, 4, "MANUAL", YELLOW)

    # the RTG: a finned canister, a cable, the bus box
    lay = PixelLayer(54, 14)
    lay.rect(2, 3, 22, 8, GREY)
    lay.rect(2, 3, 22, 1, WHITE)
    lay.rect(2, 10, 22, 1, DGREY)
    for fx in range(4, 23, 3):
        lay.rect(fx, 1, 1, 12, DGREY)
        lay.rect(fx, 1, 1, 2, GREY)
        lay.rect(fx, 11, 1, 2, DGREY)
    lay.rect(24, 5, 8, 4, DGREY)
    lay.rect(24, 5, 8, 1, GREY)
    lay.line(32, 7, 40, 7, YELLOW)         # the cable, one gap in it: the bus tie is open
    lay.set(36, 7, None)
    lay.set(37, 7, None)
    lay.rect(41, 3, 11, 8, BLUE)
    lay.rect(41, 3, 11, 1, CYAN)
    lay.rect(43, 5, 7, 4, DGREY)
    lay.set(44, 6, YELLOW)
    lay.flush(c, 22, 1)
    c.text(63, 7, "BUS - NO LOAD", DGREY)
    c.text(3, 8, "OPERATOR ACK", YELLOW)
    c.text(3, 9, "REQUIRED", YELLOW)
    hazard(c, 0, 10, 80, 1)
    return [c.save("ANSI/POWER/WARNING.ANS"), ascii_screen("WARNING", [
        "//////////////////////////////////////////////////////////////////////////////",
        "",
        "  RTG-1  BUS TIE      [ RTG ]=====[ gap ]=====[ BUS ]",
        "  STATE  OPEN",
        "  REARM  MANUAL",
        "",
        "",
        "  OPERATOR ACK",
        "  REQUIRED",
        "//////////////////////////////////////////////////////////////////////////////",
    ])]


# ---------------------------------------------------------------- WAKE

def wake():
    c = Canvas(80, 15)
    ring = PixelLayer(26, 26)
    eliza_ring(ring, 12.5, 12.5, r_out=12.4, r_in=8.0, segs=16, off=(5,), dim=(6, 12))
    ring.flush(c, 6, 1)
    c.glow(6 + 13, 1 + 13, 13.2, 16.0, AMBER, inner=13.2)

    top = [YELLOW, YELLOW, YELLOW, AMBER, AMBER]
    title5(c, 40, 2, "ELIZA", top, DGREY)
    c.text(40, 6, "MISSION AI", GREY)
    c.text(52, 6, "ONLINE", YELLOW)
    lights(c, 40, 8, "11111A11")
    leader(c, 10, "BUS", "RTG", LCYAN, x=40, lead_to=14)
    leader(c, 11, "ATTITUDE", "HOLD", LCYAN, x=40, lead_to=14)
    leader(c, 12, "CREW", "4  ALL WELL", YELLOW, x=40, lead_to=14)
    c.put(70, 12, "\u2591", DGREY)
    return [c.save("ANSI/POWER/WAKE.ANS"), ascii_screen("WAKE", [
        "",
        "          .-\"\"\"\"\"-.",
        "        / o o o o o \\          ELIZA",
        "       | o         o |         MISSION AI  ONLINE",
        "       | o    *    o |",
        "       | o         o |         BUS ......... RTG",
        "        \\ o o   o o /          ATTITUDE .... HOLD",
        "          '-.....-'            CREW ........ 4  ALL WELL",
    ])]


def build():
    out = []
    for f in (boot, reboot, sunside, warning, wake):
        out.extend(f())
    out.extend(console(False))
    out.extend(console(True))
    return out
