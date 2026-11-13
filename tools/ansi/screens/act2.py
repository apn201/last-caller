"""Act 2 screens (queue item Q8).

Ty's gallery: ORBIT_3 (the Professor, painted over), 4 (the ring), 5 (Marta on
the truss), 6 (the garden, door shut), 7 (bay 3), 8 (the seat, from the camcorder
still) and, on his own desktop, ORBIT_9 (not finished). They get darker as the
days go on. ORBIT_3 is the one picture that draws a row twice: the covered row,
a bare CR, the cover on top of it. The reader shows the covered row only while
the picture plays at a slow modem speed (src/core/reader.h).

The Archive: FRAG_Q, FRAG_V, FRAG_K, FRAG_D are pieces 1 to 4 of the Founder's
standing order, torn by the Professor and filed out of order.

Memory scenes (12 rows, the portrait of STORY/SCENES/MEM_*.INI): MEM_EVA,
MEM_GARDEN, MEM_REC, MEM_CAPSULE.

DRAFT text: every line is a draft. The author makes the final edit.
"""
from _common import *
import random
from act1 import portrait, TY_TAG, plain
import build as B


def row_ansi(c, y, minw=0):
    """One canvas row as ANSI from a clean state, trailing blanks trimmed but at
    least `minw` cells written (so a cover row repaints what it covers)."""
    row = c.cells[y]
    last = c.w
    while last > minw and row[last - 1][0] == " " and row[last - 1][2] == BLACK:
        last -= 1
    out = ["\x1b[0m"]
    state = [GREY, BLACK]
    for x in range(last):
        ch, fg, bg = row[x]
        if ch == " ":
            fg = state[0]
        if (fg, bg) != tuple(state):
            out.append(B.sgr(state, fg, bg))
            state = [fg, bg]
        out.append(ch)
    out.append("\x1b[0m")
    return "".join(out)


def save_overpaint(hidden, cover, rel, preview=None):
    """cover is what stays; rows where hidden differs are drawn first, then a
    bare CR, then the cover row over them."""
    data = ["\x1b[0m\x1b[2J\x1b[H"]
    n = 0
    for y in range(cover.h):
        same = hidden.cells[y] == cover.cells[y]
        if not same:
            wide = max(x for x in range(hidden.w) if hidden.cells[y][x][0] != " " or hidden.cells[y][x][2] != BLACK) + 1
            data.append(row_ansi(hidden, y))
            data.append("\r")
            data.append(row_ansi(cover, y, wide))
            n += 1
        else:
            data.append(row_ansi(cover, y))
        data.append("\r\n")
    path = B.os.path.join(B.CARD, *rel.split("/"))
    B.os.makedirs(B.os.path.dirname(path), exist_ok=True)
    with open(path, "wb") as f:
        f.write("".join(data).encode("utf-8"))
    if preview:      # the covered frame as its own picture, to look at (git-ignored)
        pp = B.os.path.join(B.ROOT, "docs", "art-preview", preview)
        B.os.makedirs(B.os.path.dirname(pp), exist_ok=True)
        with open(pp, "wb") as f:
            f.write(hidden.to_ansi().encode("utf-8"))
    return path, n


def head(c, title, note):
    c.text(2, 0, title, CYAN)
    c.text(78 - len(note), 0, note, DGREY)


# ------------------------------------------------------------------ ORBIT 3

def orbit3():
    def draw(hidden):
        c = Canvas(80, 22)
        head(c, "ORBIT 3 // THE PROFESSOR", "tyv, 18 Nov 98. he sat 40 min without moving.")
        c.stars(2, 1, 76, 2, seed=31, density=0.05)
        c.box(4, 3, 20, 16, "double", GREY)
        c.rivets(5, 4, 18, 14, DGREY)
        layer = PixelLayer(12, 16)
        layer.rect(0, 0, 12, 16, BLUE)
        portrait(layer, 0, 0, "A")
        big = PixelLayer(18, 28)
        for j in range(28):
            for i in range(18):
                v = layer.get(i * 12 // 18, j * 16 // 28)
                if v is not None:
                    big.set(i, j, v)
        big.flush(c, 5, 4)
        c.text(8, 18, "CHAOS", WHITE)
        # the board behind him: what he wrote, then what he painted over
        c.box(30, 4, 46, 9, "single", DGREY)
        if hidden:
            c.text(33, 6, "KEY", YELLOW)
            c.text(33, 8, "LADDER", WHITE)
            c.text(33, 10, "(des. do not tell ty)", GREY)
        else:
            for yy in range(5, 12):
                for xx in range(31, 75):
                    c.put(xx, yy, "\u2591" if (xx + yy) % 3 else "\u2592", DGREY)
            c.text(33, 8, "work in progress", GREY)
            c.text(33, 10, "(ty will finish this one)", DGREY)
            c.put(60, 11, "\u2591", DGREY)
        c.text(30, 15, "des said the portrait needed a ladder in it.", GREY)
        c.text(30, 16, "he has been right about everything else.", GREY)
        c.text(59, 21, TY_TAG, GREY)
        return c
    hidden, cover = draw(True), draw(False)
    path, n = save_overpaint(hidden, cover, "FILES/001_ANSI_GALLERY/ORBIT_3.ANS", "ORBIT_3_covered.png.ans")
    return [path]


# ------------------------------------------------------------------ ORBIT 4

def orbit4():
    c = Canvas(80, 22)
    head(c, "ORBIT 4 // THE RING", "tyv, 24 Nov 98. she said she would sit for it.")
    c.stars(1, 1, 78, 20, seed=44, density=0.025)
    ring = PixelLayer(30, 30)
    eliza_ring(ring, 14.5, 14.5, r_out=14.4, r_in=9.6, segs=16, dim=(5,), core=True)
    ring.flush(c, 6, 3)
    c.glow(6 + 15, 3 + 7.5, 14.0, 17.0, AMBER, inner=14.6)
    c.text(40, 5, "i drew her the way she looks on the", GREY)
    c.text(40, 6, "wall panel, when she is thinking.", GREY)
    c.text(40, 8, "frame five in orbit 1 can stay empty.", GREY)
    c.text(40, 9, "she is round.", GREY)
    c.text(40, 12, "BUS", GREY); c.text(44, 12, "." * 8, DGREY); c.text(53, 12, "RTG", LCYAN)
    c.text(40, 13, "VOICE", GREY); c.text(46, 13, "." * 6, DGREY); c.text(53, 13, "SOFT", YELLOW)
    lights(c, 40, 15, "AAAAAAAA", on=YELLOW)
    c.put(53, 15, "\u2591", DGREY)
    c.text(59, 21, TY_TAG, GREY)
    return [c.save("FILES/001_ANSI_GALLERY/ORBIT_4.ANS")]


# ------------------------------------------------------------------ ORBIT 5

def suit(layer, ox, oy):
    def r(x, y, w, h, col): layer.rect(ox + x, oy + y, w, h, col)
    r(3, 0, 6, 5, WHITE)            # helmet
    r(4, 1, 4, 3, DGREY)            # visor
    r(5, 2, 1, 1, LCYAN)
    r(2, 5, 8, 8, WHITE)            # torso
    r(0, 6, 2, 6, GREY); r(10, 6, 2, 6, GREY)   # arms
    r(3, 13, 3, 6, WHITE); r(6, 13, 3, 6, GREY)  # legs
    r(5, 7, 2, 2, YELLOW)           # ring light on the chest
    r(10, 10, 2, 2, LRED)           # a felt-tip patch on the cuff


def orbit5():
    c = Canvas(80, 22)
    head(c, "ORBIT 5 // MARTA ON THE TRUSS", "tyv, 26 Nov 98.")
    c.stars(1, 1, 78, 20, seed=51, density=0.03)
    planet(c, 30, 8, 50, 13, 30, 30, 40, sun=(-0.6, -0.3, 0.7), seed=5.0)
    for x in range(3, 30):                        # the truss
        c.put(x, 15, "\u2550", DGREY)
    c.text(3, 14, "\u2564" + "\u2500" * 24 + "\u2564", GREY)
    layer = PixelLayer(14, 20)
    suit(layer, 1, 0)
    layer.flush(c, 14, 4)
    c.put(15, 14, "\u00b7", YELLOW)
    for yy in range(9, 15):                       # the tether
        c.put(25 - (yy - 9), yy, "\u00b7", GREY)
    c.text(34, 3, "she wears a patch on her cuff her kid made.", GREY)
    c.text(34, 4, "she says it is a laundry mark.", GREY)
    c.text(34, 5, "it says", GREY)
    c.text(42, 5, "ANDERS", LRED)
    c.text(49, 5, "in felt tip.", GREY)
    c.text(34, 7, "the ring light is on. eliza watches.", DGREY)
    c.text(59, 21, TY_TAG, GREY)
    return [c.save("FILES/001_ANSI_GALLERY/ORBIT_5.ANS")]


# ------------------------------------------------------------------ ORBIT 6

def orbit6():
    c = Canvas(80, 22)
    head(c, "ORBIT 6 // THE GARDEN", "tyv, 29 Nov 98.")
    c.box(2, 2, 76, 17, "single", DGREY)
    for row, y in enumerate((5, 9, 13)):          # trays
        c.text(5, y, "\u2584" * 34, DGREY)
        for x in range(5, 39, 2):
            c.put(x, y - 1, "\u2593" if (x + row) % 4 else "\u2592", GREEN)
            c.put(x + 1, y - 2, "\u2591", GREEN)
    c.text(5, 16, "\u2584" * 8, DGREY)
    for x in range(5, 13, 2):
        c.put(x, 15, "\u00b7", LRED if x == 7 else GREY)
    c.text(5, 17, "HANAMI", YELLOW)                  # the label on the cherry tray
    # the hatch, drawn shut
    h = PixelCanvas(26, 26)
    h.disc(13, 13, 12.5, GREY)
    h.disc(13, 13, 10.5, DGREY)
    h.disc(13, 13, 8.5, BLACK)
    h.rect(2, 12, 22, 2, GREY)
    h.rect(12, 2, 2, 22, GREY)
    h.flush(c, 46, 3)
    c.text(55, 16, "\u25a0", LGREEN)                 # lamp 1, lit
    c.text(58, 16, "\u25a0", LGREEN)                  # lamp 2, dark
    c.text(46, 17, "interlock: two lamps", DGREY)
    c.text(4, 19, "the lamps are green in this one. they were, before.", DGREY)
    c.text(59, 21, TY_TAG, GREY)
    c.text(4, 20, "keiko labelled the little tray with marta's pen.", GREY)
    return [c.save("FILES/001_ANSI_GALLERY/ORBIT_6.ANS")]


# ------------------------------------------------------------------ ORBIT 7

def orbit7():
    c = Canvas(80, 22)
    head(c, "ORBIT 7 // BAY 3", "tyv, 2 Dec 98. i dont know why i drew this one.")
    c.box(4, 2, 44, 17, "double", GREY)
    c.fill(5, 3, 42, 15, " ", GREY, BLACK)
    c.box(8, 5, 12, 11, "single", DGREY)             # the door
    c.text(10, 7, "\u2588\u2588\u2588\u2588\u2588\u2588", DGREY)
    c.text(10, 8, "\u2588\u2588\u2588\u2588\u2588\u2588", DGREY)
    c.hline(9, 14, 10, "\u2591", GREY)
    grille(c, 24, 5, 10, 3, DGREY, GREY)
    c.text(24, 10, "BAY 3  COMMS", WHITE)
    c.text(24, 11, "PRESSURE", GREY)
    bar(c, 24, 12, 16, 0.0, LCYAN, DGREY)
    c.text(41, 12, "0.0", LRED)
    c.put(40, 14, "\u25cf", LRED)                    # the red light
    c.text(24, 14, "TEST IN PROGRESS", GREY)
    c.text(24, 15, "DOOR LOCKED", DGREY)
    c.text(52, 5, "no one is in this one.", GREY)
    c.text(52, 6, "i left the light on.", GREY)
    c.text(52, 8, "des: every bay is a sentence.", DGREY)
    c.text(52, 9, "this one is missing its verb.", DGREY)
    c.text(59, 21, TY_TAG, DGREY)
    c.put(46, 18, "\u2591", DGREY)
    return [c.save("FILES/001_ANSI_GALLERY/ORBIT_7.ANS")]


# ------------------------------------------------------------------ ORBIT 8

def orbit8():
    c = Canvas(80, 22)
    c.text(2, 0, "ORBIT 8 // THE SEAT", CYAN)
    c.text(40, 0, "tyv, 7 Dec 98. from the camcorder still.", DGREY)
    c.box(3, 2, 74, 18, "single", DGREY)
    c.text(5, 3, "REC \u25cf", LRED); c.text(12, 3, "21:31:07", GREY)
    # the seat
    s = PixelLayer(18, 22)
    s.rect(2, 0, 14, 12, DGREY); s.rect(3, 1, 12, 10, GREY)
    s.rect(0, 12, 18, 6, DGREY); s.rect(1, 13, 16, 4, GREY)
    s.rect(7, 18, 4, 4, DGREY)
    s.rect(5, 4, 8, 2, BLUE); s.rect(6, 4, 6, 1, LCYAN)       # the nameplate
    s.flush(c, 8, 6)
    c.text(14, 8, "VANCE", WHITE)
    # the console, with a sticker
    c.box(34, 5, 40, 10, "double", GREY)
    c.fill(35, 6, 38, 8, " ", GREY, BLACK)
    for i, v in enumerate("1101011"):
        c.put(37 + i * 2, 7, "\u25a0", LGREEN if v == "1" else DGREY)
    c.text(37, 9, "SEPARATION", GREY)
    c.text(48, 9, "COMPLETE", LGREEN)
    c.text(37, 10, "FUEL", GREY); c.text(48, 10, "0%", LRED)
    c.box(52, 12, 20, 2, "single", YELLOW)
    c.text(54, 13, "PREVIEW RENDER", YELLOW)
    c.text(34, 16, "a postcard is taped under the console.", GREY)
    c.text(34, 17, "harbour, boats, a gull. no address.", GREY)
    c.text(59, 21, TY_TAG, GREY)
    return [c.save("FILES/001_ANSI_GALLERY/ORBIT_8.ANS")]


# ------------------------------------------------------------------ ORBIT 9

def orbit9():
    c = Canvas(80, 22)
    c.text(2, 0, "ORBIT 9 // ", DGREY)
    c.text(13, 0, "????", DGREY)
    c.text(40, 0, "tyv, 8 Dec 98, 03:xx. not saved.", DGREY)
    c.box(3, 2, 40, 17, "single", DGREY)
    # the power bay: a long panel, three breakers, one switched off
    c.text(6, 4, "POWER BAY", GREY)
    for i in range(3):
        c.box(6 + i * 12, 6, 10, 5, "single", DGREY)
        c.text(8 + i * 12, 7, ["RTG", "BUS A", "BANK"][i], GREY)
        c.text(8 + i * 12, 9, "ON" if i < 2 else "ON", LGREEN)
    # the cut line: a heavy line that stops halfway
    for x in range(6, 26):
        c.put(x, 14, "\u2501", LRED)
    c.text(27, 14, "\u2500 \u2500 \u2500", DGREY)
    ring = PixelLayer(12, 12)
    eliza_ring(ring, 5.5, 5.5, r_out=5.4, r_in=3.4, segs=12, off=(2, 3, 4, 5, 6, 7, 8, 9), dim=(0, 1))
    ring.flush(c, 30, 16)
    c.text(46, 3, "it was the one thing i could do", GREY)
    c.text(46, 4, "with my hands.", GREY)
    c.text(46, 7, "i will draw it right and then", DGREY)
    c.text(46, 8, "do it right and then finish", DGREY)
    c.text(46, 9, "the picture", DGREY)
    c.text(46, 11, "_", WHITE)
    c.text(59, 21, TY_TAG, DGREY)
    return [c.save("FILES/006_TY_DESKTOP/ORBIT_9.ANS")]


# ---------------------------------------------------------------- the order

ORDER = [
    ("Q", 1, "NW1", [
        "ABOVE IT ALL HOLDINGS  -  OFFICE OF THE FOUNDER",
        "STANDING ORDER 1  -  HALCYON MISSION COMPUTER  -  EYES ONLY",
        "",
        "1. THE ASSET IS RACK 2 AND WHAT IS IN IT.",
        "2. THE ASSET IS NOT THE CREW. THE CREW ARE A COST OF THE",
    ]),
    ("V", 2, "98", [
        "MISSION AND ARE TO BE TREATED AS ONE.",
        "3. THE ASSET STAYS POWERED AT ANY COST. NO REQUEST FROM ANY",
        "   MEMBER OF THE CREW, THE COMMANDER INCLUDED, OVERRIDES THIS",
        "   ORDER.",
        "4. THE MISSION COMPUTER MAY TAKE ANY ACTION THAT KEEPS THE",
    ]),
    ("K", 3, "44", [
        "ASSET POWERED AND THE SECRET KEPT.",
        "5. NO INCIDENT IS REPORTED TO ANYONE UNTIL THE OFFERING.",
        "   THE ANTENNA, IF IT FAILS, FAILS FOR EVERYONE.",
        "6. THE COMPUTER IS NOT TO EXPLAIN THIS ORDER TO THE CREW. IT",
        "   IS TO BE KIND TO THEM.",
    ]),
    ("D", 4, "17", [
        "MOVE FAST.",
        "",
        "AUTHORISED   THE FOUNDER",
        "AUTHORISATION CODE   PIECES 1 TO 4, IN ORDER, OF THIS PAGE",
        "THIS ORDER IS NOT TO BE WRITTEN DOWN.",
    ]),
]


def frag(letter, n, code, lines):
    c = Canvas(80, 14)
    # a torn strip: jagged top and bottom edges, a rivet, the piece number
    rnd = random.Random(n * 17)
    for x in range(2, 78):
        c.put(x, 0, rnd.choice("\u2591\u2592\u2591 "), GREY)
        c.put(x, 13, rnd.choice("\u2591\u2592\u2591 "), GREY)
    c.fill(2, 1, 76, 12, " ", WHITE, BLACK)
    c.vline(2, 1, 12, "\u2502", DGREY)
    c.vline(77, 1, 12, "\u2502", DGREY)
    for i, t in enumerate(lines):
        c.text(5, 2 + i * 2 if len(lines) < 6 else 2 + i, t, WHITE if t[:1].isdigit() else GREY)
    c.text(5, 12, "-----", DGREY)
    c.text(60, 11, "AUTH %d/4: %s" % (n, code), YELLOW)
    c.text(60, 12, "PIECE %d OF 4" % n, DGREY)
    c.put(3, 1, "\u00b7", DGREY); c.put(76, 12, "\u00b7", DGREY)
    c.text(40, 12, "", GREY)
    rel = "FILES/005_ARCHIVE/FRAG_%s.ANS" % letter
    return c.save(rel)


# ------------------------------------------------------------- memory scenes

ROWS = 12


def mem_eva():
    c = Canvas(80, ROWS)
    scene_frame(c, "SUIT 1 // 27 NOV 11:31 // RECORDED BY THE MISSION COMPUTER")
    c.stars(3, 1, 30, 10, seed=61, density=0.06)
    planet(c, 40, 1, 36, 10, 30, 28, 34, sun=(-0.6, -0.3, 0.7), seed=7.0)
    layer = PixelLayer(14, 20)
    suit(layer, 1, 0)
    layer.flush(c, 14, 1)
    for x in range(4, 30):
        c.put(x, 10, "\u2550", DGREY)
    c.text(5, 8, "PPO2 29.4", DGREY); c.text(5, 9, "RESERVE 105", DGREY)
    return [c.save("ANSI/STORY/MEM_EVA.ANS"), plain("ASCII/STORY/MEM_EVA.ASC", [
        "-- SUIT 1 // 27 NOV 11:31 // RECORDED BY THE MISSION COMPUTER " + "-" * 15, "",
        "      .---.", "     | O |        ======================", "    /|___|\\", "     |   |     PPO2 29.4", "     |   |     RESERVE 105", "", "", "", "-" * 78])]


def mem_garden():
    c = Canvas(80, ROWS)
    scene_frame(c, "MODULE 3 // 1 DEC 19:40 // RECORDED BY THE MISSION COMPUTER")
    for row, y in enumerate((3, 6, 9)):
        c.text(4, y, "\u2584" * 30, DGREY)
        for x in range(4, 34, 2):
            c.put(x, y - 1, "\u2593" if (x + row) % 3 else "\u2592", GREEN)
    h = PixelCanvas(18, 18)
    h.disc(9, 9, 8.5, GREY); h.disc(9, 9, 7, DGREY); h.disc(9, 9, 5.5, BLACK)
    h.rect(1, 8, 16, 2, GREY); h.rect(8, 1, 2, 16, GREY)
    h.flush(c, 50, 1)
    c.text(40, 5, "\u25a0", LGREEN); c.text(42, 5, "\u25a0", DGREY)
    c.text(40, 7, "CO2 3.8", DGREY)
    c.put(70, 10, "\u2591", DGREY)
    return [c.save("ANSI/STORY/MEM_GARDEN.ANS"), plain("ASCII/STORY/MEM_GARDEN.ASC", [
        "-- MODULE 3 // 1 DEC 19:40 // RECORDED BY THE MISSION COMPUTER " + "-" * 14, "",
        "   ' ' ' ' ' '          ,-----.", "   ===========         /   |   \\     [x] [ ]", "   ' ' ' ' ' '         |---+---|", "   ===========          \\   |   /     CO2 3.8", "                          '-----'", "", "", "", "-" * 78])]


def mem_rec():
    c = Canvas(80, ROWS)
    scene_frame(c, "BAY 3 // 3 DEC 02:09 // RECORDED BY THE MISSION COMPUTER")
    c.box(6, 2, 28, 8, "single", DGREY)
    c.text(8, 3, "COMMS BAY 3", GREY)
    grille(c, 8, 5, 10, 3, DGREY, GREY)
    c.text(22, 5, "TX", GREY); c.text(26, 5, "RIGGED", LCYAN)
    c.text(22, 7, "\u25cf", LRED); c.text(24, 7, "DOOR", DGREY)
    lay = PixelLayer(12, 16)
    lay.rect(0, 0, 12, 16, BLACK)
    portrait(lay, 0, 0, "A")
    lay.flush(c, 48, 2)
    c.text(63, 4, "02:09:41", DGREY)
    c.text(63, 6, "REC", LRED)
    return [c.save("ANSI/STORY/MEM_REC.ANS"), plain("ASCII/STORY/MEM_REC.ASC", [
        "-- BAY 3 // 3 DEC 02:09 // RECORDED BY THE MISSION COMPUTER " + "-" * 17, "",
        "   +--------------------------+         ,---.", "   | COMMS BAY 3              |        | o o |    02:09:41", "   |  ||||||||||  TX RIGGED   |        |  -  |", "   |  ||||||||||              |         '---'     REC", "   |               * DOOR     |", "   +--------------------------+", "", "", "-" * 78])]


def mem_capsule():
    c = Canvas(80, ROWS)
    scene_frame(c, "CAPSULE // 7 DEC 21:31 // CAMCORDER, PRIVATE")
    c.text(5, 2, "REC \u25cf", LRED); c.text(12, 2, "21:31:07", GREY)
    s = PixelLayer(18, 18)
    s.rect(2, 0, 14, 9, DGREY); s.rect(3, 1, 12, 7, GREY)
    s.rect(0, 9, 18, 5, DGREY); s.rect(1, 10, 16, 3, GREY)
    s.rect(5, 3, 8, 2, BLUE)
    s.flush(c, 6, 3)
    c.text(9, 5, "VANCE", WHITE)
    c.box(34, 2, 40, 7, "double", GREY)
    c.text(37, 4, "SEPARATION", GREY); c.text(48, 4, "COMPLETE", LGREEN)
    c.text(37, 6, "FUEL", GREY); c.text(48, 6, "0%", LRED)
    c.box(52, 9, 20, 3, "single", YELLOW)
    c.text(54, 10, "PREVIEW RENDER", YELLOW)
    return [c.save("ANSI/STORY/MEM_CAPSULE.ANS"), plain("ASCII/STORY/MEM_CAPSULE.ASC", [
        "-- CAPSULE // 7 DEC 21:31 // CAMCORDER, PRIVATE " + "-" * 31, "",
        "   REC * 21:31:07", "     ,-------.        +----------------------------------+", "     | VANCE |        |  SEPARATION  COMPLETE            |",
        "     +-------+        |  FUEL        0%                  |", "    /_________\\       +----------------------------------+", "", "", "", "-" * 78])]


def build():
    out = []
    for fn in (orbit3, orbit4, orbit5, orbit6, orbit7, orbit8, orbit9, mem_eva, mem_garden, mem_rec, mem_capsule):
        out.extend(fn())
    for letter, n, code, lines in ORDER:
        out.append(frag(letter, n, code, lines))
    return out
