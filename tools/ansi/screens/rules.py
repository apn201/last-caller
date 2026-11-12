"""BULLETINS/001_RULES.ANS: a crew conduct notice in mission-document style,
signed ELIZA. Shown at every login (mandatory bulletin), then "Press ENTER".

DRAFT text: every rule, the date and the signature are drafts. The author
makes the final edit of every caller-visible line.
"""
from _common import *

RULES = [
    "Be kind to the crew. The crew are your friends.",
    "Messages are kept for the whole mission.",
    "Please do not leave a message half written.",
    "If you feel unwell, tell ELIZA. ELIZA will help.",
    "Lights go down at 22:00 ship time. Rest well.",
    "Nobody leaves the ship alone.",
]


def build():
    c = Canvas(80, 22)
    c.box(0, 0, 80, 22, "single", GREY)
    c.rivets(1, 1, 78, 20, DGREY)
    # letterhead
    c.text(3, 0, "┤", GREY)
    c.text(4, 0, " HALCYON  MISSION DOCUMENT ", CYAN)
    c.text(31, 0, "├", GREY)
    header(c, "CREW NOTICE")
    c.box(70, 1, 8, 4, "single", DGREY)
    c.text(72, 2, "DOC", DGREY)
    c.text(72, 3, "001", WHITE)
    c.put(0, 4, "├", GREY); c.hline(1, 4, 78, "─", GREY); c.put(79, 4, "┤", GREY)

    # form fields
    fields = [("DOC NO.", "001/98", 3), ("CLASS", "CREW", 22), ("ISSUED", "2 NOV 1998", 36), ("PAGE", "1 OF 1", 64)]
    for k, v, x in fields:
        c.text(x, 5, k, DGREY)
        c.text(x + len(k) + 1, 5, v, GREY)
    c.text(3, 6, "TO", DGREY); c.text(6, 6, "ALL CREW AND ALL CALLERS", GREY)
    c.text(36, 6, "FROM", DGREY); c.text(41, 6, "ELIZA, MISSION AI AND SYSOP", GREY)
    c.put(0, 7, "├", GREY); c.hline(1, 7, 78, "─", GREY); c.put(79, 7, "┤", GREY)

    # the rules
    c.text(3, 8, "CONDUCT ABOARD", CYAN)
    for n, r in enumerate(RULES):
        y = 10 + n
        c.text(5, y, "[x]", LCYAN)
        c.text(10, y, "%d." % (n + 1), DGREY)
        c.text(14, y, r, GREY)
    c.text(14, 16, "Thank you for your cooperation.", DGREY)
    c.put(0, 17, "├", GREY); c.hline(1, 17, 78, "─", GREY); c.put(79, 17, "┤", GREY)

    # signature block
    ring = PixelLayer(6, 6)
    eliza_ring(ring, 2.5, 2.5, r_out=2.9, r_in=1.5, segs=6)
    ring.flush(c, 5, 18)
    c.text(14, 18, "Signed,", DGREY)
    c.text(14, 19, "ELIZA", YELLOW)
    c.text(14, 20, "SysOp  ·  Mission AI  ·  2 Nov 1998", DGREY)
    # stamp
    c.box(58, 18, 18, 3, "double", AMBER)
    c.text(60, 19, "READ AND FILED", YELLOW)
    c.text(60, 21, "·halcyon·crew·98·", DGREY)
    c.put(59, 21, "┤", GREY); c.put(77, 21, "├", GREY)
    c.put(77, 8, "·", DGREY)    # a rivet that came loose\n    c.put(60, 4, "═", DGREY)
    return [c.save("BULLETINS/001_RULES.ANS")]
