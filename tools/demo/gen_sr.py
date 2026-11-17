#!/usr/bin/env python3
"""Ty's demo (slice GE2): the ANSI frames of the Telnet version, sd/BBS/ANSI/DEMO/SR_*.ANS.

The browser version (sd/BBS/DEMO/SR.JS) draws the same effects live, with the music. These frames are stills of
them for a caller without the browser: 80 columns, 23 rows, the sixteen text colours, shade and half blocks,
UTF-8 like the board's other art. Background colours stay in 0..7 (plain ANSI, no iCE colour).
Original work, MIT. Run from the repository root:  python tools/demo/gen_sr.py
"""
import math
import os

COLS, ROWS = 80, 23
PW, PH = COLS, ROWS * 2
OUT = os.path.join("sd", "BBS", "ANSI", "DEMO")
LIGHT, MED, DARK, FULL, UP, DOWN = "░", "▒", "▓", "█", "▀", "▄"
DIM = {8: 0, 9: 1, 10: 2, 11: 3, 12: 4, 13: 5, 14: 6, 15: 7}


class Screen:
    def __init__(self):
        self.ch = [[" "] * COLS for _ in range(ROWS)]
        self.fg = [[7] * COLS for _ in range(ROWS)]
        self.bg = [[0] * COLS for _ in range(ROWS)]

    def cell(self, x, y, ch, fg, bg=None):
        if 0 <= x < COLS and 0 <= y < ROWS:
            self.ch[y][x] = ch
            self.fg[y][x] = fg
            if bg is not None:
                self.bg[y][x] = bg

    def text(self, y, s, fg, bg=0):
        x0 = (COLS - len(s)) // 2
        for i, c in enumerate(s):
            self.cell(x0 + i, y, c, fg, bg)

    def pixels(self, px):
        """px[y][x] 0..15, PH x PW: half blocks, with the brighter colour in front."""
        for y in range(ROWS):
            for x in range(COLS):
                top, bot = px[y * 2][x], px[y * 2 + 1][x]
                if top == bot:
                    if top < 8:
                        self.cell(x, y, " ", 7, top)
                    else:
                        self.cell(x, y, FULL, top, 0)
                elif bot < 8:
                    self.cell(x, y, UP, top, bot)
                elif top < 8:
                    self.cell(x, y, DOWN, bot, top)
                else:   # both bright: the lower one dims
                    self.cell(x, y, UP, top, DIM[bot])

    def ansi(self):
        out = ["\x1b[0m\x1b[2J\x1b[H"]
        for y in range(ROWS):
            cur = None
            for x in range(COLS):
                ch, fg, bg = self.ch[y][x], self.fg[y][x], self.bg[y][x]
                if ch == " ":
                    fg = cur[0] if cur else 7   # a space shows no foreground: do not change it for one
                if (fg, bg) != cur:
                    bold = "1" if fg >= 8 else "0"
                    out.append("\x1b[%s;%d;%dm" % (bold, 30 + (fg & 7), 40 + bg))
                    cur = (fg, bg)
                out.append(ch)
            out.append("\x1b[0m\r\n")
        return "".join(out)

    def save(self, name):
        with open(os.path.join(OUT, name + ".ANS"), "w", encoding="utf-8", newline="") as f:
            f.write(self.ansi())


def blank_px(c=0):
    return [[c] * PW for _ in range(PH)]


# a small block font, 5 x 5, for the titles (our own)
FONT = {
    "A": ["01110", "10001", "11111", "10001", "10001"], "B": ["11110", "10001", "11110", "10001", "11110"],
    "C": ["01111", "10000", "10000", "10000", "01111"], "D": ["11110", "10001", "10001", "10001", "11110"],
    "E": ["11111", "10000", "11110", "10000", "11111"], "I": ["11111", "00100", "00100", "00100", "11111"],
    "L": ["10000", "10000", "10000", "10000", "11111"], "N": ["10001", "11001", "10101", "10011", "10001"],
    "O": ["01110", "10001", "10001", "10001", "01110"], "R": ["11110", "10001", "11110", "10010", "10001"],
    "S": ["01111", "10000", "01110", "00001", "11110"], "T": ["11111", "00100", "00100", "00100", "00100"],
    "V": ["10001", "10001", "10001", "01010", "00100"], "Y": ["10001", "01010", "00100", "00100", "00100"],
    " ": ["000", "000", "000", "000", "000"],
}


def big(px, word, y0, colour):
    width = sum(len(FONT[c][0]) + 1 for c in word) - 1
    x = (PW - width) // 2
    for c in word:
        g = FONT[c]
        for j, row in enumerate(g):
            for i, bit in enumerate(row):
                if bit == "1" and 0 <= x + i < PW and 0 <= y0 + j < PH:
                    px[y0 + j][x + i] = colour(i, j)
        x += len(g[0]) + 1


def title():
    px = blank_px()
    for y in range(PH):
        if math.sin(y * 0.5) > 0.93:
            for x in range(PW):
                px[y][x] = 1
    big(px, "TYVANCE", 4, lambda i, j: [15, 14, 14, 6, 6][j])
    big(px, "SECOND REALITY", 20, lambda i, j: [15, 11, 11, 3, 9][j])
    s = Screen()
    s.pixels(px)
    s.text(17, "a text-mode thing", 7)
    s.text(19, "after Future Crew, Second Reality, 1993", 8)
    s.save("SR_TITLE")


def dots():
    px = blank_px()
    for y in range(36, PH):
        for x in range(PW):
            px[y][x] = 1 if ((x >> 2) ^ (y >> 1)) & 1 else 0
    n, cx, cy = 140, 40, 18
    pts = []
    for i in range(n):
        y = 1 - (i + 0.5) / n * 2
        r = math.sqrt(1 - y * y)
        a = i * 2.39996 + 0.6
        x, z = math.cos(a) * r, math.sin(a) * r
        y2, z2 = y * math.cos(0.5) - z * math.sin(0.5), y * math.sin(0.5) + z * math.cos(0.5)
        pts.append((cx + x * 20, cy + y2 * 13, z2))
    for a in range(0, 628, 4):
        for r in (0.3, 0.6, 1.0):
            xx, yy = int(cx + math.cos(a / 100) * 14 * r), int(39 + math.sin(a / 100) * 2 * r)
            if 0 <= yy < PH and 0 <= xx < PW:
                px[yy][xx] = 0
    for x, y, z in sorted(pts, key=lambda p: -p[2]):
        xi, yi = int(x), int(y)
        if 0 <= xi < PW and 0 <= yi < PH:
            px[yi][xi] = 8 if z > 0.4 else 7 if z > -0.3 else 15
    s = Screen()
    s.pixels(px)
    s.save("SR_DOTS")


RAMP_BLUE = [(0, 1, LIGHT), (0, 1, MED), (0, 1, DARK), (1, 9, LIGHT), (1, 9, MED), (1, 9, DARK), (1, 11, LIGHT),
             (1, 11, MED), (3, 11, DARK), (3, 15, LIGHT), (3, 15, MED), (7, 15, DARK)]
RAMP_VIOLET = [(0, 5, LIGHT), (0, 5, MED), (5, 13, LIGHT), (5, 13, MED), (5, 13, DARK), (5, 15, LIGHT),
               (5, 15, MED), (5, 11, MED), (3, 11, DARK), (3, 1, MED), (1, 0, MED), (0, 1, LIGHT)]


def plasma(name, t, ramp):
    s = Screen()
    for y in range(ROWS):
        for x in range(COLS):
            v = (math.sin(x * 0.09 + t) + math.sin(y * 0.22 + t * 0.7) + math.sin(x * 0.05 + y * 0.12 + t * 1.3)
                 + math.sin(math.hypot(x - 40 - math.sin(t) * 20, (y - 11) * 2.2) * 0.12 - t))
            k = ((v + 4) / 8 + t * 0.05) % 1
            bg, fg, ch = ramp[min(len(ramp) - 1, int(k * len(ramp)))]
            s.cell(x, y, ch, fg, bg)
    s.save(name)


OCTA_V = [(1, 0, 0), (-1, 0, 0), (0, 1, 0), (0, -1, 0), (0, 0, 1), (0, 0, -1)]
OCTA_F = [(0, 2, 4), (2, 1, 4), (1, 3, 4), (3, 0, 4), (2, 0, 5), (1, 2, 5), (3, 1, 5), (0, 3, 5)]


def glenz(name, p):
    front = [[0] * PW for _ in range(PH)]
    back = [[0] * PW for _ in range(PH)]
    size, cx, cy = 10, 40, 23
    ax, ay, az = p * 0.9, p * 1.3, p * 0.4

    def rot(v):
        x, y, z = v
        y, z = y * math.cos(ax) - z * math.sin(ax), y * math.sin(ax) + z * math.cos(ax)
        x, z = x * math.cos(ay) + z * math.sin(ay), -x * math.sin(ay) + z * math.cos(ay)
        x, y = x * math.cos(az) - y * math.sin(az), x * math.sin(az) + y * math.cos(az)
        k = 3.2 / (3.2 + z)
        return (cx + x * size * 2 * k, cy + y * size * k, z)

    def tri(a, b, c, buf, bit):
        ys = [a[1], b[1], c[1]]
        xs = [a[0], b[0], c[0]]
        for y in range(max(0, int(min(ys))), min(PH - 1, int(max(ys)) + 1) + 1):
            for x in range(max(0, int(min(xs))), min(PW - 1, int(max(xs)) + 1) + 1):
                px_, py_ = x + 0.5, y + 0.5
                w0 = (b[0] - a[0]) * (py_ - a[1]) - (b[1] - a[1]) * (px_ - a[0])
                w1 = (c[0] - b[0]) * (py_ - b[1]) - (c[1] - b[1]) * (px_ - b[0])
                w2 = (a[0] - c[0]) * (py_ - c[1]) - (a[1] - c[1]) * (px_ - c[0])
                if (w0 >= 0 and w1 >= 0 and w2 >= 0) or (w0 <= 0 and w1 <= 0 and w2 <= 0):
                    if y < PH and x < PW:
                        buf[y][x] |= bit

    verts = [rot(v) for v in OCTA_V]
    for n, f in enumerate(OCTA_F):
        m = [sum(OCTA_V[f[k]][i] for k in range(3)) for i in range(3)]
        ln = math.sqrt(sum(c * c for c in m))
        tip = rot(tuple(c / ln * 1.3 for c in m))
        for e in range(3):
            a, b = verts[f[e]], verts[f[(e + 1) % 3]]
            cross = (b[0] - a[0]) * (tip[1] - a[1]) - (b[1] - a[1]) * (tip[0] - a[0])
            tri(a, b, tip, front if cross < 0 else back, 2 if (n * 3 + e) & 1 else 1)
    px = blank_px()
    for y in range(PH):
        for x in range(PW):
            f, b = front[y][x], back[y][x]
            grid = 8 if (x % 8 == 0 or y % 6 == 0) else 0
            px[y][x] = (11 if b else 15) if f & 1 else (1 if b else 9) if f else (3 if b & 1 else 1) if b else grid
    s = Screen()
    s.pixels(px)
    s.save(name)


def tunnel():
    px = blank_px()
    t = 2.0
    for y in range(PH):
        for x in range(PW):
            dx, dy = (x - 40) / 2, y - 23
            dist = math.hypot(dx, dy) + 0.001
            a = math.atan2(dy, dx)
            u, v = int(math.floor(a / math.pi * 8 + t * 2)), int(math.floor(48 / dist + t * 6))
            light = min(1, dist / 22)
            on = (u ^ v) & 1
            px[y][x] = 0 if light < 0.15 else (14 if light > 0.6 else 6 if light > 0.35 else 4) if on else (12 if light > 0.6 else 4 if light > 0.35 else 0)
    s = Screen()
    s.pixels(px)
    s.save("SR_TUNNEL")


def end():
    px = blank_px()
    seed = 7
    for _ in range(90):
        seed = (seed * 1103515245 + 12345) & 0x7FFFFFFF
        x = seed % PW
        seed = (seed * 1103515245 + 12345) & 0x7FFFFFFF
        y = seed % PH
        px[y][x] = [8, 7, 15][seed % 3]
    big(px, "TYVANCE", 14, lambda i, j: [15, 14, 14, 6, 6][j])
    s = Screen()
    s.pixels(px)
    s.text(13, "finished. told you.", 7)
    s.text(16, "code and ansi: tyv        music: Purple Motion and Skaven", 8)
    s.text(17, "greets: skit  lumen  rastar  fuselage  the cellar", 8)
    s.save("SR_END")


def main():
    os.makedirs(OUT, exist_ok=True)
    title()
    dots()
    plasma("SR_PLASMA1", 0.5, RAMP_BLUE)
    plasma("SR_PLASMA2", 1.3, RAMP_BLUE)
    plasma("SR_PLASMA3", 2.4, RAMP_VIOLET)
    glenz("SR_GLENZ1", 0.6)
    glenz("SR_GLENZ2", 1.4)
    tunnel()
    end()
    print("wrote SR_*.ANS to", OUT)


if __name__ == "__main__":
    main()
