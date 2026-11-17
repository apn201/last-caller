"""The end demo's tune and pictures (slice GE). Written from nothing, the same bytes every run.

    python tools/demo/gen_demo.py

Writes into sd/BBS/DEMO/:
  TUNE.MOD      18 patterns of ProTracker, 125 bpm, 138.24 s, A minor. Original composition and instruments
                (tools/mod/modlib.py makes every sample from arithmetic). Public domain (CC0), see
                docs/CONTENT_POLICY.md. Patterns are 7.68 s long, so the demo's parts cut on the beat.
  FACE_*.PNG    the four crew, 48 x 64 pixel portraits, written here with the standard library
  SHIP.PNG      the ship as it is in 2026, 88 x 32
Sample names carry the credits, as in a 1993 module. All wording is a DRAFT for the author's final edit.
"""
import os
import struct
import sys
import zlib

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, os.path.join(ROOT, "tools", "mod"))
from modlib import (Sample, cycle, saw, square, soft, kick, snare, hat, sample_from_cycle, pattern, write_mod,
                    note_index, note_name)

OUT = os.path.join(ROOT, "sd", "BBS", "DEMO")
BAR = 16


# ------------------------------------------------------------------------------------------ the tune
def tok(row, note, sample, tokens, length=None, eff=""):
    tokens[row] = "%s/%d%s" % (note, sample, "+" + eff if eff else "")
    if length and row + length < 64 and tokens[row + length] == "-":
        tokens[row + length] = "^"


def blank():
    return ["-"] * 64


def drums(kind):
    t = blank()
    for b in range(4):
        o = b * BAR
        if kind == "hat":
            for r in range(0, BAR, 2): tok(o + r, "C-2", 3, t, eff="C1C")
        elif kind == "soft":
            tok(o, "C-2", 1, t, eff="C30"); tok(o + 8, "C-2", 1, t, eff="C24")
            for r in (4, 12): tok(o + r, "C-2", 3, t, eff="C20")
        elif kind in ("basic", "drive"):
            for r in (0, 8) if kind == "basic" else (0, 4, 8, 12): tok(o + r, "C-2", 1, t)
            if kind == "basic": tok(o + 10, "C-2", 1, t, eff="C28")
            for r in (4, 12): tok(o + r, "C-2", 2, t) if kind == "basic" else None
            for r in range(2, BAR, 4): tok(o + r, "C-2", 3, t, eff="C20")
            if kind == "drive":
                for r in (6, 14): tok(o + r, "C-2", 2, t, eff="C30")
        elif kind == "fill" and b == 3:
            for r in (0, 4, 8): tok(o + r, "C-2", 1, t)
            for r in (10, 12, 13, 14, 15): tok(o + r, "C-2", 2, t, eff="C%02X" % (28 + 3 * (r - 10)))
        elif kind == "fill":
            for r in (0, 8): tok(o + r, "C-2", 1, t)
            for r in (4, 12): tok(o + r, "C-2", 2, t)
            for r in range(2, BAR, 4): tok(o + r, "C-2", 3, t, eff="C20")
        elif kind == "end":
            if b < 3:
                for r in (0, 4, 8, 12): tok(o + r, "C-2", 1, t)
                for r in (6, 14): tok(o + r, "C-2", 2, t, eff="C30")
            elif b == 3:
                tok(o, "C-2", 1, t)
                tok(o + 8, "C-2", 2, t)
    return t


ROOTS = [9, 5, 12, 7]   # A F C G, a note index in octave 1 (C-1 = 0)
CHORDS = [("A-2", "C-3", "E-3"), ("F-2", "A-2", "C-3"), ("C-3", "E-3", "G-3"), ("G-2", "B-2", "D-3")]


def bass(kind):
    t = blank()
    for b in range(4):
        o = b * BAR
        root = ROOTS[b]
        n = lambda semis: note_name(root + semis)
        if kind == "long":
            tok(o, n(0), 4, t, 15)
        elif kind == "quarters":
            for r in (0, 4, 8, 12): tok(o + r, n(0), 4, t, 3)
        elif kind == "groove":
            for r, semis, ln in ((0, 0, 3), (3, 0, 1), (6, 12, 2), (8, 0, 3), (11, 7, 2), (14, 0, 1)): tok(o + r, n(semis), 4, t, ln)
        elif kind == "drive":
            for r in range(0, BAR, 2): tok(o + r, n(12 if r % 8 == 6 else 0), 4, t, 1)
    return t


def arp(kind):
    t = blank()
    for b in range(4):
        o = b * BAR
        c = CHORDS[b]
        if kind == "pad":
            tok(o, c[0], 7, t, 15)
        elif kind == "slow":
            for r in range(0, BAR, 4): tok(o + r, c[(r // 4) % 3], 6, t)
        elif kind == "fast":
            for r in range(BAR): tok(o + r, c[[0, 1, 2, 1][r % 4]], 6, t)
    return t


def lead(events, kind="saw"):
    t = blank()
    s = 5 if kind == "saw" else 8
    for row, note, ln in events:
        tok(row, note, s, t, ln)
    return t


MEL_A = [(0, "E-3", 3), (3, "D-3", 1), (4, "C-3", 2), (6, "D-3", 2), (8, "E-3", 6), (14, "C-3", 2),
         (16, "F-3", 3), (19, "E-3", 1), (20, "C-3", 2), (22, "E-3", 2), (24, "F-3", 6), (30, "A-3", 2),
         (32, "G-3", 3), (35, "E-3", 1), (36, "C-3", 2), (38, "E-3", 2), (40, "G-3", 4), (44, "E-3", 2), (46, "D-3", 2),
         (48, "D-3", 3), (51, "B-2", 1), (52, "G-2", 2), (54, "B-2", 2), (56, "D-3", 6), (62, "E-3", 2)]
MEL_B = [(0, "A-3", 4), (4, "G-3", 2), (6, "E-3", 2), (8, "G-3", 4), (12, "E-3", 4),
         (16, "A-3", 6), (22, "G-3", 2), (24, "F-3", 4), (28, "E-3", 4),
         (32, "G-3", 4), (36, "E-3", 4), (40, "C-3", 4), (44, "E-3", 4),
         (48, "D-3", 4), (52, "G-3", 4), (56, "B-3", 4), (60, "A-3", 4)]
MEL_HOLD = [(0, "E-3", 15), (16, "F-3", 15), (32, "G-3", 15), (48, "D-3", 15)]
BEACON = [(r + 16 * b, "E-3", 1) for b in range(4) for r in (0, 4, 6)]      # three blips a bar
MORSE = [(0, "A-3", 1), (2, "A-3", 1), (4, "A-3", 1), (8, "A-3", 3), (16, "A-3", 3), (20, "A-3", 1), (24, "A-3", 3),
         (32, "A-3", 1), (34, "A-3", 1), (36, "A-3", 1), (40, "A-3", 3), (48, "A-3", 3), (52, "A-3", 1), (56, "A-3", 1)]
MEL_END = MEL_A[:-6] + [(48, "A-3", 8), (56, "E-3", 4)]


def tune(path):
    sine = lambda p: soft(p)
    samples = [
        Sample("halcyon end demo", kick(), 64),
        Sample("snare, bay 2 tray", snare(), 58),
        Sample("hat (quiet please)", hat(), 40),
        sample_from_cycle("bass, solar bus", cycle(64, lambda p: 0.7 * saw(p) + 0.3 * square(0.5)(p)), 56),
        sample_from_cycle("lead, the long call", cycle(32, saw), 32),
        sample_from_cycle("pluck, letters home", cycle(32, square(0.25)), 36),
        sample_from_cycle("pad, cabin 21.0 c", cycle(64, soft), 30),
        sample_from_cycle("signal, 145.800", cycle(32, sine), 42),
        Sample("--------------------"),
        Sample("music written in code"),
        Sample("for the halcyon board"),
        Sample("no samples, no loans"),
        Sample("public domain, cc0"),
        Sample("--------------------"),
        Sample("greets to the crew"),
        Sample("marta desmond keiko ty"),
        Sample("and the club"),
        Sample("--------------------"),
    ]
    P = lambda d, b, a, l: pattern(d, b, a, l)
    patterns = [
        P(blank(), blank(), arp("pad"), lead(BEACON, "beacon")),                              # 0 intro
        P(drums("hat"), bass("quarters"), arp("pad"), lead(BEACON, "beacon")),               # 1 build
        P(drums("basic"), bass("groove"), arp("fast"), blank()),                              # 2 title
        P(drums("drive"), bass("groove"), arp("fast"), lead(MEL_A)),                          # 3 title, the lead comes in
        P(drums("soft"), bass("long"), arp("slow"), lead(MEL_A)),                             # 4 letters
        P(drums("soft"), bass("long"), arp("slow"), lead(MEL_B)),                             # 5
        P(drums("basic"), bass("quarters"), arp("slow"), lead(MEL_A)),                        # 6
        P(drums("fill"), bass("groove"), arp("fast"), lead(MEL_B)),                           # 7
        P(drums("basic"), bass("groove"), arp("fast"), lead(MEL_HOLD)),                       # 8 news
        P(drums("drive"), bass("drive"), arp("fast"), lead(MEL_A)),                           # 9
        P(drums("drive"), bass("drive"), arp("fast"), lead(MEL_B)),                           # 10
        P(drums("fill"), bass("groove"), arp("fast"), lead(MEL_A)),                           # 11
        P(blank(), bass("long"), arp("pad"), lead(MORSE, "beacon")),                          # 12 2026
        P(blank(), bass("long"), arp("pad"), lead(MORSE, "beacon")),                          # 13
        P(drums("drive"), bass("drive"), arp("fast"), lead(MEL_A)),                           # 14 credits
        P(drums("drive"), bass("drive"), arp("fast"), lead(MEL_B)),                           # 15
        P(drums("fill"), bass("groove"), arp("fast"), lead(MEL_A)),                           # 16
        P(drums("end"), bass("groove"), arp("slow"), lead(MEL_END)),                          # 17 the last pattern
    ]
    # speed 6 and 125 bpm are the defaults; say so on the first row
    patterns[0][0][0] = patterns[0][0][0][:2] + (0xF, 6)
    patterns[0][0][1] = patterns[0][0][1][:2] + (0xF, 0x7D)
    return write_mod(path, "halcyon end demo", samples, list(range(len(patterns))), patterns)


# ------------------------------------------------------------------------------------------ pictures
def png(path, width, height, rows, palette):
    """An 8-bit indexed PNG; rows are lists of palette indexes, 0 is transparent."""
    raw = b"".join(b"\0" + bytes(r) for r in rows)
    def chunk(kind, data):
        body = kind + data
        return struct.pack(">I", len(data)) + body + struct.pack(">I", zlib.crc32(body) & 0xFFFFFFFF)
    plte = b"".join(bytes(c) for c in palette)
    trns = bytes([0])
    data = (b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", width, height, 8, 3, 0, 0, 0)) +
            chunk(b"PLTE", plte) + chunk(b"tRNS", trns) + chunk(b"IDAT", zlib.compress(raw, 9)) + chunk(b"IEND", b""))
    with open(path, "wb") as f:
        f.write(data)
    return len(data)


class Img:
    def __init__(self, w, h):
        self.w, self.h = w, h
        self.px = [[0] * w for _ in range(h)]

    def set(self, x, y, c):
        if 0 <= x < self.w and 0 <= y < self.h and c is not None:
            self.px[y][x] = c

    def rect(self, x, y, w, h, c):
        for j in range(h):
            for i in range(w):
                self.set(x + i, y + j, c)

    def ellipse(self, cx, cy, rx, ry, c, shade=None):
        for y in range(int(cy - ry) - 1, int(cy + ry) + 2):
            for x in range(int(cx - rx) - 1, int(cx + rx) + 2):
                d = ((x + 0.5 - cx) / rx) ** 2 + ((y + 0.5 - cy) / ry) ** 2
                if d <= 1.0:
                    col = c
                    if shade:
                        light = ((x + 0.5 - cx) / rx) * -0.7 + ((y + 0.5 - cy) / ry) * -0.4   # light from the upper left
                        bayer = ((x & 1) * 2 + (y & 1) * 3) % 4 / 4.0
                        col = shade[0] if light + d * 0.5 > 0.35 + bayer * 0.25 else shade[1] if light + d * 0.5 < -0.35 + bayer * 0.25 else c
                    self.set(x, y, col)

    def save(self, name, palette):
        return png(os.path.join(OUT, name), self.w, self.h, self.px, palette)


# the board's palette (VGA, as the terminal draws it) plus a few skin tones. Index 0 is transparent.
PAL = [(0, 0, 0), (0, 0, 0), (170, 170, 170), (85, 85, 85), (255, 255, 255), (0, 170, 170), (85, 255, 255), (0, 0, 170),
       (255, 255, 85), (170, 85, 0), (255, 85, 85), (233, 190, 150), (196, 148, 110), (150, 104, 74), (120, 120, 130),
       (60, 60, 70), (200, 200, 205), (90, 50, 20), (230, 200, 120), (30, 30, 40), (60, 160, 90), (110, 70, 40)]
BLACK, GREY, DGREY, WHITE, CYAN, LCYAN, BLUE, YELLOW, AMBER, RED = 1, 2, 3, 4, 5, 6, 7, 8, 9, 10
SKIN, SKIN_D, SKIN_DD, SILVER, DARK, GREYHAIR, BEARD, BROWN, BLONDE, INK, GREEN, TAN = 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 17
SKINS = {"H": (11, 12, 13), "A": (11, 12, 13), "T": (12, 13, 17), "V": (11, 12, 13)}
IDX = dict(BLACK=1, GREY=2, DGREY=3, WHITE=4, CYAN=5, LCYAN=6, BLUE=7, YELLOW=8, AMBER=9, RED=10, SKIN=11, SKIN_D=12,
           SKIN_DD=13, SILVER=14, DARK=15, GREYHAIR=16, BEARD=17, BROWN=18, BLONDE=19, INK=20, GREEN=21)


def face(who):
    im = Img(48, 64)
    skin, skin_d, skin_dd = {"H": (11, 12, 13), "A": (11, 12, 13), "T": (12, 13, 13), "V": (11, 12, 13)}[who]
    # shoulders and the collar with a patch
    shirt = {"H": BLUE, "A": BLUE, "T": CYAN, "V": 21}[who]
    im.ellipse(24, 74, 30, 20, shirt)
    im.rect(0, 54, 48, 10, shirt)
    im.rect(8, 54, 32, 2, LCYAN if who != "V" else WHITE)
    im.rect(30, 58, 6, 4, WHITE)
    # neck and head
    im.rect(19, 42, 10, 12, skin_d)
    im.ellipse(24, 28, 15, 19, skin, shade=(skin, skin_d))
    # hair, per person
    if who == "H":      # grey-brown, pulled back, a short tail
        im.ellipse(24, 15, 16, 9, 16)
        im.rect(8, 15, 5, 14, 16); im.rect(35, 15, 5, 14, 16)
        im.rect(38, 22, 6, 12, 16); im.set(41, 35, 16)
    elif who == "A":    # bald on top, wiry grey sides, beard, glasses
        im.rect(8, 20, 4, 14, 14); im.rect(36, 20, 4, 14, 14)
        im.rect(14, 12, 20, 2, 16)
        im.ellipse(24, 40, 11, 8, 14)             # beard
        im.rect(16, 36, 16, 3, 14)
        for x in (15, 29): im.rect(x, 24, 8, 6, 0); im.rect(x, 24, 8, 1, DARK); im.rect(x, 29, 8, 1, DARK); im.rect(x, 24, 1, 6, DARK); im.rect(x + 7, 24, 1, 6, DARK)
        im.rect(23, 26, 2, 1, DARK)
    elif who == "T":    # long black hair and a flower
        im.ellipse(24, 14, 17, 10, 19)
        im.rect(7, 14, 6, 38, 19); im.rect(35, 14, 6, 38, 19)
        im.rect(30, 10, 5, 5, WHITE); im.rect(31, 11, 3, 3, YELLOW)
    else:               # yellow hair, a beanie of the crew's colour, headphones
        im.ellipse(24, 12, 16, 9, 18)
        im.rect(8, 10, 32, 5, RED)
        im.rect(6, 22, 4, 14, DARK); im.rect(38, 22, 4, 14, DARK)
        im.rect(8, 8, 32, 1, 0)
    # eyes, brows, nose, mouth
    eye_y = 27
    for x in (16, 30):
        im.rect(x, eye_y, 4, 3, WHITE); im.rect(x + 1, eye_y, 2, 3, 20 if who != "H" else DARK)
        im.rect(x, eye_y - 3, 5, 1, DARK if who != "A" else GREY)
    im.rect(23, 31, 2, 6, skin_d); im.rect(22, 37, 4, 1, skin_dd)
    if who == "V":
        im.rect(18, 42, 12, 2, WHITE); im.rect(18, 42, 12, 1, 4)
    elif who == "T":
        im.rect(19, 42, 10, 2, 10 if False else RED)
    else:
        im.rect(19, 42, 10, 1, skin_dd)
    return im


def ship_png():
    im = Img(88, 32)
    im.rect(8, 12, 56, 8, GREY); im.rect(8, 16, 56, 4, DGREY)           # hull
    im.rect(64, 13, 10, 6, DGREY); im.rect(74, 15, 4, 2, DGREY)         # nose
    im.rect(0, 14, 8, 4, DGREY)                                         # stern
    im.rect(14, 4, 26, 6, CYAN); im.rect(14, 4, 26, 1, LCYAN)           # a solar wing
    im.rect(40, 22, 14, 6, BLUE)                                        # the lower wing is half gone
    im.rect(20, 10, 2, 2, GREY); im.rect(30, 10, 2, 2, GREY)
    im.rect(24, 14, 2, 2, YELLOW)                                       # the one window that is lit
    for x in range(30, 60, 6): im.rect(x, 14, 2, 2, BLACK)
    im.set(3, 3, WHITE); im.set(80, 26, GREY)
    return im


def main():
    os.makedirs(OUT, exist_ok=True)
    print("TUNE.MOD", tune(os.path.join(OUT, "TUNE.MOD")), "bytes")
    pal = PAL
    for who in "HATV":
        print("FACE_%s.PNG" % who, face(who).save("FACE_%s.PNG" % who, pal), "bytes")
    print("SHIP.PNG", ship_png().save("SHIP.PNG", pal), "bytes")


if __name__ == "__main__":
    main()