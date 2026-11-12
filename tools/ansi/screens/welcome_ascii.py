"""ASCII/LOGIN/WELCOME.ASC: the plain version of the welcome screen for
callers without ANSI. 7-bit ASCII only, 80 columns, CRLF.

DRAFT text: same greeting and telemetry as WELCOME.ANS.
"""
import random
from _common import *


def build():
    c = Canvas(80, 22)
    rnd = random.Random(11)
    for _ in range(40):
        y = rnd.choice([0, 0, 9, 10, 11, 12, 13])
        x = rnd.randrange(80) if y < 9 or y == 9 else rnd.randrange(62, 78)
        c.put(x, y, rnd.choice(".....+*"))

    # logo: the same letters as the ANSI screen, two pixel rows to one text row
    glyphs = [big_glyph(ch) for ch in "HALCYON"]
    for r in range(6):
        line = ""
        for n, g in enumerate(glyphs):
            row = "".join("#" if (g[2 * r][i] == "#" or g[2 * r + 1][i] == "#") else " " for i in range(8))
            line += row + "  "
        c.text(5, 1 + r, line.rstrip())
    c.hline(5, 7, 70, "=")
    c.text_center(8, "B U L L E T I N   B O A R D   S Y S T E M   -   N O D E   1")

    ring = ["  .-\"\"\"-.  ", " / o o o \\ ", "| o     o |", "| o     o |", " \\ o o o / ", "  '-...-'  "]
    for n, r in enumerate(ring):
        c.text(4, 10 + n, r)
    c.text(19, 10, "Good evening, caller.")
    c.text(19, 11, "I am ELIZA. I look after this board.")
    c.text(19, 12, "The crew are resting.")
    c.text(19, 13, "Please keep your voice down.")
    c.text(19, 15, "ELIZA - SysOp")

    earth = [
        "                 _.-''''''''''''''''''-._",
        "            _.-'   .   .      .    .      '-._",
        "        _.-'    .      .    .        .       '-._",
        "    _.-'  .        .         .    .       .      '-._",
    ]
    for n, e in enumerate(earth):
        c.text(9, 16 + n, e)
    c.text(0, 21, "LINK ACQUIRED | CARRIER 28800 | DOPPLER -3.1kHz | SUN 100% | BATT OPEN")
    return [c.save_plain("ASCII/LOGIN/WELCOME.ASC")]
