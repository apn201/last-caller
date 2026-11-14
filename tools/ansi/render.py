#!/usr/bin/env python3
"""Render an ANSI art file (.ANS) to a PNG, the way the board's browser
terminal draws it: 80 columns, VGA palette, 8x16 cells (drawn at 2x).

    python tools/ansi/render.py sd/BBS/ANSI/LOGIN/WELCOME.ANS
    python tools/ansi/render.py --all
    python tools/ansi/render.py sd/BBS/ANSI/MENUS/MAIN.ANS --menu MAIN

Output goes to docs/art-preview/ (git-ignored).

Block elements and box-drawing characters are drawn from geometry here, so
they look right on any machine. Letters and symbols come from the first
monospaced TrueType font found (only used for previews, never shipped).
--menu NAME reads sd/BBS/CONFIG/MENUS.INI and draws the option text the board
would write into the art, then checks that the option area was empty.
"""
import argparse
import os
import re
import sys

from PIL import Image, ImageDraw, ImageFont

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
OUT = os.path.join(ROOT, "docs", "art-preview")
CARD = os.path.join(ROOT, "sd", "BBS")

# Same colours as shared/web/terminal.html.
PALETTE = [
    (0x00, 0x00, 0x00), (0xaa, 0x00, 0x00), (0x00, 0xaa, 0x00), (0xaa, 0x55, 0x00),
    (0x00, 0x00, 0xaa), (0xaa, 0x00, 0xaa), (0x00, 0xaa, 0xaa), (0xaa, 0xaa, 0xaa),
    (0x55, 0x55, 0x55), (0xff, 0x55, 0x55), (0x55, 0xff, 0x55), (0xff, 0xff, 0x55),
    (0x55, 0x55, 0xff), (0xff, 0x55, 0xff), (0x55, 0xff, 0xff), (0xff, 0xff, 0xff),
]
COLS = 80
SCALE = 2
CW, CH = 8 * SCALE, 16 * SCALE
SCREEN_ROWS = 25

FONT_CANDIDATES = [
    r"C:\Windows\Fonts\consola.ttf", r"C:\Windows\Fonts\CascadiaMono.ttf",
    r"C:\Windows\Fonts\lucon.ttf", r"C:\Windows\Fonts\cour.ttf",
    "/usr/share/fonts/dejavu/DejaVuSansMono.ttf",
    "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf",
]
SYMBOL_FONTS = [r"C:\Windows\Fonts\seguisym.ttf", r"C:\Windows\Fonts\CascadiaMono.ttf"]

# ---------------------------------------------------------------- parsing


class Cell:
    __slots__ = ("ch", "fg", "bg")

    def __init__(self, ch=" ", fg=7, bg=0):
        self.ch, self.fg, self.bg = ch, fg, bg


def parse(data):
    """ANSI bytes -> (rows of Cell, cursor_row, cursor_col). Handles SGR,
    cursor moves, 2J, CR/LF and the wrap at column 80 like the browser."""
    text = data.decode("utf-8", errors="replace")
    grid = [[Cell() for _ in range(COLS)] for _ in range(SCREEN_ROWS)]
    x = y = 0
    fg, bg, bright = 7, 0, False
    wrap = False
    saved = (0, 0)
    i = 0
    n = len(text)

    def need(row):
        while len(grid) <= row:
            grid.append([Cell() for _ in range(COLS)])

    while i < n:
        c = text[i]
        if c == "\x1a":
            break
        if c == "\x1b" and i + 1 < n and text[i + 1] == "[":
            j = i + 2
            while j < n and not (0x40 <= ord(text[j]) <= 0x7e):
                j += 1
            params, cmd = text[i + 2:j], text[j] if j < n else ""
            args = [int(a) if a.isdigit() else 0 for a in params.split(";")] if params else []
            k = args[0] if args and args[0] else 1
            if cmd == "m":
                for a in args or [0]:
                    if a == 0:
                        fg, bg, bright = 7, 0, False
                    elif a == 1:
                        bright = True
                    elif a in (2, 22):
                        bright = False
                    elif 30 <= a <= 37:
                        fg = a - 30
                    elif 90 <= a <= 97:
                        fg = a - 82
                    elif 40 <= a <= 47:
                        bg = a - 40
                    elif 100 <= a <= 107:
                        bg = a - 92
                    elif a == 39:
                        fg = 7
                    elif a == 49:
                        bg = 0
            elif cmd in "Hf":
                y = max(0, (args[0] if args and args[0] else 1) - 1)
                x = min(COLS - 1, max(0, (args[1] if len(args) > 1 and args[1] else 1) - 1))
                wrap = False
                need(y)
            elif cmd == "A":
                y = max(0, y - k); wrap = False
            elif cmd == "B":
                y += k; wrap = False; need(y)
            elif cmd == "C":
                x = min(COLS - 1, x + k); wrap = False
            elif cmd == "D":
                x = max(0, x - k); wrap = False
            elif cmd == "G":
                x = min(COLS - 1, k - 1); wrap = False
            elif cmd == "s":
                saved = (x, y)
            elif cmd == "u":
                x, y = saved
            elif cmd == "J" and args and args[0] == 2:
                grid = [[Cell() for _ in range(COLS)] for _ in range(SCREEN_ROWS)]
                x = y = 0
            i = j + 1
            continue
        if c == "\r":
            x = 0; wrap = False
        elif c == "\n":
            y += 1; wrap = False; need(y)
        elif c in "\x00\x07\x08":
            pass
        else:
            if wrap:
                x = 0; y += 1; wrap = False; need(y)
            ink = fg | (8 if bright else 0)
            cell = grid[y][x]
            cell.ch, cell.fg, cell.bg = c, ink, bg
            if x == COLS - 1:
                wrap = True
            else:
                x += 1
        i += 1
    return grid, y, x


# ---------------------------------------------------------------- drawing

_font = None
_symbol = None
_cache = {}
_missing = set()


def load_fonts():
    global _font, _symbol
    for path in FONT_CANDIDATES:
        if os.path.exists(path):
            _font = ImageFont.truetype(path, 26)
            break
    if _font is None:
        _font = ImageFont.load_default()
    for path in SYMBOL_FONTS:
        if os.path.exists(path):
            _symbol = ImageFont.truetype(path, 26)
            break


def has_glyph(font, ch):
    try:
        mask = font.getmask(ch)
        return mask.getbbox() is not None
    except Exception:
        return False


def letter_mask(ch):
    """A 16x32 'L' mask of a text glyph, or None."""
    if ch in _cache:
        return _cache[ch]
    mask = None
    for font in (_font, _symbol):
        if font is not None and has_glyph(font, ch):
            img = Image.new("L", (CW, CH), 0)
            d = ImageDraw.Draw(img)
            d.text((CW // 2, CH - 7), ch, fill=255, font=font, anchor="ms")
            mask = img
            break
    if mask is None:
        _missing.add(ch)
    _cache[ch] = mask
    return mask


# arms: (up, down, left, right), 0 none / 1 single / 2 double
BOX = {}
for _chars, _arms in [
    ("─│┌┐└┘├┤┬┴┼", [(0, 0, 1, 1), (1, 1, 0, 0), (0, 1, 0, 1), (0, 1, 1, 0), (1, 0, 0, 1), (1, 0, 1, 0),
                      (1, 1, 0, 1), (1, 1, 1, 0), (0, 1, 1, 1), (1, 0, 1, 1), (1, 1, 1, 1)]),
    ("═║╔╗╚╝╠╣╦╩╬", [(0, 0, 2, 2), (2, 2, 0, 0), (0, 2, 0, 2), (0, 2, 2, 0), (2, 0, 0, 2), (2, 0, 2, 0),
                      (2, 2, 0, 2), (2, 2, 2, 0), (0, 2, 2, 2), (2, 0, 2, 2), (2, 2, 2, 2)]),
    ("╒╓╕╖╘╙╛╜╞╟╡╢╤╥╧╨╪╫", [(0, 1, 0, 2), (0, 2, 0, 1), (0, 1, 2, 0), (0, 2, 1, 0), (1, 0, 0, 2), (2, 0, 0, 1),
                           (1, 0, 2, 0), (2, 0, 1, 0), (1, 1, 0, 2), (2, 2, 0, 1), (1, 1, 2, 0), (2, 2, 1, 0),
                           (0, 1, 2, 2), (0, 2, 1, 1), (1, 0, 2, 2), (2, 0, 1, 1), (1, 1, 2, 2), (2, 2, 1, 1)]),
]:
    for _c, _a in zip(_chars, _arms):
        BOX[_c] = _a

V_SLOTS = {1: [(7, 8, "C")], 2: [(4, 5, "L"), (10, 11, "R")]}
H_SLOTS = {1: [(15, 16, "C")], 2: [(12, 13, "U"), (18, 19, "D")]}


def box_rects(arms):
    """Rectangles (x0, y0, x1, y1) in the 16x32 cell for a box character."""
    up, down, left, right = arms
    rects = []
    harms = {d: w for d, w in (("left", left), ("right", right)) if w}
    varms = {d: w for d, w in (("up", up), ("down", down)) if w}
    hslots = [s for w in harms.values() for s in H_SLOTS[w]]
    vslots = [s for w in varms.values() for s in V_SLOTS[w]]
    for d, w in varms.items():
        for x0, x1, side in V_SLOTS[w]:
            if not harms:
                far = near = 16 if d == "up" else 15
            elif d == "up":
                near, far = min(s[1] for s in hslots), max(s[1] for s in hslots)
            else:
                near, far = max(s[0] for s in hslots), min(s[0] for s in hslots)
            if side == "C":
                edge = far
            elif len(harms) == 2:
                edge = near
            elif harms:
                hd = next(iter(harms))
                edge = near if (side == "L") == (hd == "left") else far
            else:
                edge = near
            rects.append((x0, 0, x1, edge) if d == "up" else (x0, edge, x1, CH - 1))
    for d, w in harms.items():
        for y0, y1, side in H_SLOTS[w]:
            if not varms:
                far = near = 16 if d == "left" else 15
            elif d == "right":
                near, far = max(s[0] for s in vslots), min(s[0] for s in vslots)
            else:
                near, far = min(s[1] for s in vslots), max(s[1] for s in vslots)
            if side == "C":
                edge = far
            elif len(varms) == 2:
                edge = near
            elif varms:
                vd = next(iter(varms))
                edge = near if (side == "U") == (vd == "up") else far
            else:
                edge = near
            rects.append((edge, y0, CW - 1, y1) if d == "right" else (0, y0, edge, y1))
    return rects


def shade_mask(level):
    """Dither masks on the 8x16 base grid for the three shade characters."""
    img = Image.new("L", (CW, CH), 0)
    px = img.load()
    for by in range(16):
        for bx in range(8):
            if level == 1:
                on = (bx % 4 == 0 and by % 2 == 0) or (bx % 4 == 2 and by % 2 == 1)
            elif level == 2:
                on = (bx + by) % 2 == 0
            else:
                on = not ((bx % 4 == 0 and by % 2 == 1) or (bx % 4 == 2 and by % 2 == 0))
            if on:
                for dy in range(SCALE):
                    for dx in range(SCALE):
                        px[bx * SCALE + dx, by * SCALE + dy] = 255
    return img


def rect_mask(rects):
    img = Image.new("L", (CW, CH), 0)
    d = ImageDraw.Draw(img)
    for x0, y0, x1, y1 in rects:
        d.rectangle([x0, y0, x1, y1], fill=255)
    return img


def special_mask(ch):
    """Masks for characters drawn from geometry, or None."""
    key = ("special", ch)
    if key in _cache:
        return _cache[key]
    w, h = CW, CH
    m = None
    if ch == "█":
        m = rect_mask([(0, 0, w - 1, h - 1)])
    elif ch == "▀":
        m = rect_mask([(0, 0, w - 1, h // 2 - 1)])
    elif ch == "▄":
        m = rect_mask([(0, h // 2, w - 1, h - 1)])
    elif ch == "▌":
        m = rect_mask([(0, 0, w // 2 - 1, h - 1)])
    elif ch == "▐":
        m = rect_mask([(w // 2, 0, w - 1, h - 1)])
    elif ch == "░":
        m = shade_mask(1)
    elif ch == "▒":
        m = shade_mask(2)
    elif ch == "▓":
        m = shade_mask(3)
    elif ch == "■":
        m = rect_mask([(3 * SCALE, 5 * SCALE, 5 * SCALE + 1, 10 * SCALE + 1)])
        m = rect_mask([(2 * SCALE, 4 * SCALE, 6 * SCALE - 1, 12 * SCALE - 1)])
    elif ch == "▪":
        m = rect_mask([(3 * SCALE, 6 * SCALE, 5 * SCALE - 1, 10 * SCALE - 1)])
    elif ch == "·":
        m = rect_mask([(7, 15, 8, 16)])
    elif ch == "∙":
        m = rect_mask([(6, 14, 9, 17)])
    elif ch in BOX:
        m = rect_mask(box_rects(BOX[ch]))
    _cache[key] = m
    return m


def draw_cell(img, x, y, cell):
    ox, oy = x * CW, y * CH
    bg = PALETTE[cell.bg & 7] if cell.bg < 8 else PALETTE[cell.bg]
    fg = PALETTE[cell.fg]
    img.paste(bg, (ox, oy, ox + CW, oy + CH))
    ch = cell.ch
    if ch == " ":
        return
    mask = special_mask(ch)
    if mask is None:
        mask = letter_mask(ch)
    if mask is not None:
        img.paste(fg, (ox, oy), mask)


def render(grid, rows):
    load_fonts() if _font is None else None
    img = Image.new("RGB", (COLS * CW, rows * CH), (0, 0, 0))
    for y in range(rows):
        if y >= len(grid):
            break
        for x in range(COLS):
            draw_cell(img, x, y, grid[y][x])
    return img


# ------------------------------------------------------------ menu overlay

# What the board writes (src/bbs/board.h, messages.h): [K] Label, key in
# bright yellow. A SysOp sees the longest lists, so these are the full ones.
SAMPLE_ITEMS = {
    "MAIN": [("M", "Message bases"), ("N", "New scan"), ("F", "File cabinets"), ("A", "ANSI gallery"),
             ("D", "Doors & games"), ("C", "Chat with the SysOp"), ("B", "Bulletins"), ("J", "Conferences"),
             ("W", "Who is online"), ("O", "Page SysOp"), ("U", "User list"), ("S", "System info"),
             ("?", "Help"), ("G", "Goodbye"), ("Z", "SysOp console")],
    "MESSAGES": [("R", "Read / list"), ("N", "Read new"), ("P", "Post a message"), ("A", "Change area"),
                 ("Q", "Main Board")],
    "GALLERY": [("A", "Login splash"), ("F", "File cabinet screen"), ("G", "Browse ANSI / ASCII files"),
                ("Q", "Main Board")],
    "DOORS": [("V", "VolaMUD"), ("Q", "Main Board")],
}


def read_menus():
    path = os.path.join(CARD, "CONFIG", "MENUS.INI")
    menus, cur = {}, None
    with open(path, encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith(";"):
                continue
            m = re.match(r"\[(.+)\]", line)
            if m:
                cur = menus.setdefault(m.group(1).upper(), {})
            elif "=" in line and cur is not None:
                k, v = line.split("=", 1)
                cur[k.strip().lower()] = v.strip()
    return menus


def overlay_menu(grid, name, art_height):
    """Draw the options like menus.h does and report any clash with the art."""
    menus = read_menus()
    m = menus.get(name.upper())
    if not m:
        print("  no [%s] in MENUS.INI" % name)
        return art_height + 1, 0, []
    row, col = int(m.get("row", 0)), int(m.get("col", 3))
    columns, width, rows = int(m.get("columns", 1)), int(m.get("width", 26)), int(m.get("rows", 0))
    items = SAMPLE_ITEMS.get(name.upper(), [])
    problems = []
    cap = rows * columns
    placed = 0
    for i, (key, label) in enumerate(items[:cap] if rows else []):
        r, c = row + i // columns, col + (i % columns) * width
        text = "[%s] %s" % (key, label)
        room = width - len("[%s]" % key) - 2
        label = label[:room]
        text = "[%s] %s" % (key, label)
        for k, ch in enumerate(text):
            x, y = c - 1 + k, r - 1
            if y >= len(grid) or x >= COLS:
                problems.append("option %s falls off the screen" % key)
                break
            cell = grid[y][x]
            if cell.ch != " " or cell.bg != 0:
                problems.append("option %s at row %d col %d lands on %r (bg %d)" % (key, r, c + k, cell.ch, cell.bg))
                break
        # also the padding that menuText writes (spaces, default colours)
        for k in range(len(text), width - 1):
            x, y = c - 1 + k, r - 1
            if x < COLS and y < len(grid) and (grid[y][x].ch != " " or grid[y][x].bg != 0):
                problems.append("option %s padding at row %d col %d lands on %r" % (key, r, c + k, grid[y][x].ch))
                break
        for k, ch in enumerate(text):
            x, y = c - 1 + k, r - 1
            if x < COLS and y < len(grid):
                cell = grid[y][x]
                cell.ch, cell.bg = ch, 0
                cell.fg = 11 if k < len("[%s]" % key) else 7
        placed += 1
    return art_height + 1, placed, problems


def art_lines(data):
    """Rows of the art as menus.h counts them: the number of LF bytes."""
    return min(24, data.count(b"\n"))


def prompt_line(grid, row, text):
    for k, ch in enumerate(text):
        if k < COLS and row < len(grid):
            cell = grid[row][k]
            cell.ch, cell.fg, cell.bg = ch, 7, 0


def render_file(path, menu=None, prompt=None, suffix=""):
    with open(path, "rb") as f:
        data = f.read()
    grid, cy, cx = parse(data)
    problems = []
    lines = art_lines(data)
    height = lines
    if menu:
        prompt_row, placed, problems = overlay_menu(grid, menu, lines)
        prompt = prompt or "Main Board Command? "
        prompt_line(grid, prompt_row - 1, prompt)
    elif prompt:
        prompt_line(grid, cy + (1 if cx else 0), prompt)
    rows = max(SCREEN_ROWS, len(grid))
    # Screen bottom: a row of 25 is the browser terminal's height.
    img = render(grid, rows)
    os.makedirs(OUT, exist_ok=True)
    name = os.path.relpath(path, CARD) if path.startswith(CARD) else os.path.basename(path)
    out = os.path.join(OUT, re.sub(r"[\\/]", "_", os.path.splitext(name)[0]) + suffix + ".png")
    img.save(out)
    notes = []
    if lines > 23:
        notes.append("%d rows (a screen holds 23 plus the prompt)" % lines)
    for line in data.split(b"\n"):
        pass
    return out, problems, notes, height


def main():
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("files", nargs="*")
    ap.add_argument("--menu", help="MENUS.INI section: draw sample options and check the option area")
    ap.add_argument("--prompt", help="text drawn on the line after the art")
    ap.add_argument("--suffix", default="", help="added to the PNG name, e.g. _2 (the viewer caches by name)")
    ap.add_argument("--all", action="store_true", help="every .ANS under sd/BBS")
    a = ap.parse_args()
    files = list(a.files)
    if a.all:
        for dirpath, _, names in os.walk(CARD):
            for n in sorted(names):
                if n.upper().endswith(".ANS"):
                    files.append(os.path.join(dirpath, n))
    if not files:
        ap.error("no files")
    bad = False
    for f in files:
        out, problems, notes, height = render_file(os.path.abspath(f), a.menu, a.prompt, a.suffix)
        print("%s -> %s" % (os.path.relpath(f, ROOT), os.path.relpath(out, ROOT)))
        for p in problems + notes:
            print("  !", p)
            bad = True
    if _missing:
        print("  ! no glyph for:", " ".join(sorted(_missing)))
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
