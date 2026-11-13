"""Ty's three tunes, written as ProTracker modules from nothing.

    python tools/mod/gen_mods.py

SUNSIDE.MOD and THERMO.MOD go to Ty's Gallery, HARBOUR.MOD to Ty's Desktop.
Original compositions, original instruments (see modlib.py). The sample names
are Ty's: lowercase, typos, greets, bad poetry. All wording is a DRAFT for the
author's final edit.
"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from modlib import (Sample, cycle, saw, square, tri, organ, soft, kick, snare, hat, gull,
                    sample_from_cycle, line, pattern, write_mod, note_index, note_name)

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
GALLERY = os.path.join(ROOT, "sd", "BBS", "FILES", "001_ANSI_GALLERY")
DESKTOP = os.path.join(ROOT, "sd", "BBS", "FILES", "006_TY_DESKTOP")

BAR = 16
K, S, H = "C-2/1", "C-2/2", "C-2/3"   # drum tokens (the pitch of a drum does not matter)


def text(*names):
    """Samples that hold only a message."""
    return [Sample(n) for n in names]


def shift(events, semis):
    return [(r, note_name(note_index(n) + semis), s, d) + tuple(e) for r, n, s, d, *e in events]


def at(events, bar):
    return [(r + bar * BAR, n, s, d) + tuple(e) for r, n, s, d, *e in events]


def merge(*lists):
    out = ["-"] * 64
    for t in lists:
        for i, x in enumerate(t):
            if x != "-":
                out[i] = x
    return out


def bass_bar(root, bar, walk=None):
    """The bassline of one bar. root is a note index (C-1 is 0)."""
    n = lambda semis: note_name(root + semis)
    ev = walk or [(0, n(0), 4, 3), (3, n(0), 4, 1), (6, n(12), 4, 2), (8, n(0), 4, 3), (11, n(7), 4, 2), (14, n(0), 4, 1)]
    return at(ev, bar)


# =============================================================================================
# SUNSIDE: A minor, 125 bpm. Four channels: drums, hats or plucks, bass, lead.
# =============================================================================================
def sunside(path):
    samples = [
        Sample("sunside / tyv 98", kick(), 64),
        Sample("snare = the mess tray", snare(), 60),
        Sample("hat, bay 2 hiss", hat(), 34),
        sample_from_cycle("bass 64 bytes ok", cycle(64, lambda p: 0.7 * saw(p) + 0.3 * square(0.5)(p)), 56),
        sample_from_cycle("lead, saw, crunchy", cycle(32, saw), 34),
        sample_from_cycle("pluck (arp)", cycle(32, square(0.25)), 40),
    ] + text(
        "--------------------",
        "made 9 nov, cupola",
        "16 sunsides a day",
        "i counted",
        "--------------------",
        "stops at 0100. deal.",
        "(marta's rule)",
        "--------------------",
        "gretz: skit, lumen",
        "and the one note guy",
        "no greets 2 the office",
        "--------------------",
        "roses are red",
        "earth is blue",
        "the bass is too",
        "des: that doesnt rhyme",
        "--------------------",
        "eliza says nice tune",
        "she dimmed the lights",
    )
    roots = [9, 5, 12, 7]                                      # A F C G, in octave 1
    chords = [(21, 24, 28), (17, 21, 24), (24, 28, 31), (19, 23, 26)]   # A C E / F A C / C E G / G B D

    def drums(kicks=(0, 8, 10), snares=(4, 12)):
        ev = []
        for b in range(4):
            ev += [(b * BAR + r, "C-2", 1, None) for r in kicks] + [(b * BAR + r, "C-2", 2, None) for r in snares]
        return line(ev)

    def hats():
        t = ["-"] * 64
        for r in range(64):
            if r % 2 == 0:
                t[r] = "C-2/3+C%s" % ("28" if r % 4 == 0 else "18")
            elif r % 16 == 15:
                t[r] = "C-2/3+C10"
        return t

    def arp(sample=6):
        ev = []
        for b in range(4):
            c = chords[b]
            for r in range(BAR):
                ev.append((b * BAR + r, note_name(c[[0, 1, 2, 1][r % 4]]), sample, None, "A03"))
        return line(ev, cut=False)

    def bass():
        ev = []
        for b in range(4):
            ev += bass_bar(roots[b], b)
        return line(ev)

    lead_a = [(0, "E-3", 5, 3), (3, "D-3", 5, 1), (4, "C-3", 5, 2), (6, "A-2", 5, 2), (8, "C-3", 5, 4), (12, "E-3", 5, 4),
              (16, "F-3", 5, 3), (19, "E-3", 5, 1), (20, "C-3", 5, 2), (22, "A-2", 5, 2), (24, "C-3", 5, 6), (30, "A-2", 5, 2),
              (32, "G-3", 5, 3), (35, "E-3", 5, 1), (36, "C-3", 5, 2), (38, "E-3", 5, 2), (40, "G-3", 5, 4), (44, "E-3", 5, 2), (46, "D-3", 5, 2),
              (48, "D-3", 5, 3), (51, "B-2", 5, 1), (52, "G-2", 5, 2), (54, "B-2", 5, 2), (56, "D-3", 5, 4), (60, "B-2", 5, 2), (62, "A-2", 5, 2)]
    lead_hold = [(0, "E-3", 5, 16, "482"), (16, "F-3", 5, 16, "482"), (32, "G-3", 5, 16, "482"), (48, "D-3", 5, 12, "482")]

    # A: the groove (drums, hats, bass, plucks). B: plucks, bass, lead over drums.
    # C: the break (hats, plucks, bass, held lead). D: the end.
    pat_a = pattern(drums(), hats(), bass(), arp())
    pat_b = pattern(drums(), arp(), bass(), line(lead_a))
    pat_b2 = pattern(drums(kicks=(0, 6, 8, 10), snares=(4, 12, 15)), arp(), bass(), line(shift(lead_a, -12)))
    pat_c = pattern(hats(), arp(), bass(), line(lead_hold, cut=False))
    pat_d = pattern(
        ["C-2/1", "-", "-", "-", "C-2/2"] + ["-"] * 11 + ["C-2/1"],
        ["-"] * 64,
        ["A-1/4+C40"] + ["-+A01"] * 40,
        ["A-3/5+C30"] + ["-+A02"] * 24,
    )
    patterns = [pat_a, pat_b, pat_b2, pat_c, pat_d]
    # A A B B C B B2 B2 C B2 D
    orders = [0, 0, 1, 1, 3, 1, 2, 2, 3, 2, 4]
    # 125 bpm is the default; make speed 6 explicit on the first row.
    pat_a[0][0] = pat_a[0][0][:2] + (0xF, 6)
    return write_mod(path, "sunside - tyv 98", samples, orders, patterns)


# =============================================================================================
# THERMO: a twelve-bar blues in A, 100 bpm. Drums, bass, keys, lead.
# =============================================================================================
def thermo(path):
    samples = [
        Sample("thermostat blues", kick(), 56),
        Sample("snare (22. final)", snare(), 46),
        Sample("brush, 21.0 c", hat(7), 30),
        sample_from_cycle("bass, period 41 min", cycle(64, tri), 60),
        sample_from_cycle("keys, dead band 0.5", cycle(32, organ), 30),
        sample_from_cycle("lead (des whistles)", cycle(32, square(0.3)), 28),
    ] + text(
        "--------------------",
        "written 5 nov",
        "wore two jackets",
        "--------------------",
        "keiko is right",
        "for the wrong reason",
        "(des said that)",
        "--------------------",
        "marta says 22",
        "she will log it",
        "eliza: both within",
        "spec. thanks eliza",
        "--------------------",
        "i just want 24",
        "this one is 4 des",
        "he wont get it",
    )
    chords = [9, 9, 9, 9, 2, 2, 9, 9, 4, 2, 9, 4]                  # A A A A D D A A E D A E

    def walk(root, bar):
        steps = [0, 4, 7, 9] if bar % 2 == 0 else [0, 4, 7, 4]
        return at([(4 * i, note_name(root + st), 4, 3) for i, st in enumerate(steps)], bar)

    def drums():
        ev = []
        for b in range(4):
            ev += [(b * BAR + r, "C-2", 1, None) for r in (0, 10)] + [(b * BAR + r, "C-2", 2, None) for r in (4, 12)]
            ev += [(b * BAR + r, "C-2", 3, None, "C14") for r in (2, 6, 8, 14)]
        return line(ev)
    v = "482"
    lead = [
        [(0, "E-3", 6, 3), (4, "G-3", 6, 2), (6, "E-3", 6, 2), (8, "D-3", 6, 3), (12, "C-3", 6, 2), (14, "A-2", 6, 2),
         (20, "E-3", 6, 3), (24, "D-3", 6, 2), (26, "C-3", 6, 2), (28, "A-2", 6, 4, v),
         (48, "A-2", 6, 6), (54, "C-3", 6, 2), (56, "D-3", 6, 4), (60, "D#3", 6, 2), (62, "E-3", 6, 2)],
        [(0, "A-2", 6, 3), (4, "C-3", 6, 2), (6, "D-3", 6, 2), (8, "D#3", 6, 2), (10, "E-3", 6, 6, v),
         (16, "D-3", 6, 3), (20, "C-3", 6, 2), (22, "A-2", 6, 2), (24, "C-3", 6, 4), (28, "A-2", 6, 4),
         (32, "E-3", 6, 2), (34, "G-3", 6, 2), (36, "E-3", 6, 2), (38, "D-3", 6, 2), (40, "C-3", 6, 8, v),
         (48, "D-3", 6, 2), (50, "C-3", 6, 2), (52, "A-2", 6, 8, v)],
        [(0, "E-3", 6, 6, v), (8, "G-3", 6, 2), (10, "E-3", 6, 2), (12, "D-3", 6, 4),
         (16, "C-3", 6, 3), (20, "D-3", 6, 3), (24, "C-3", 6, 2), (26, "A-2", 6, 6, v),
         (32, "A-2", 6, 2), (34, "C-3", 6, 2), (36, "E-3", 6, 2), (38, "G-3", 6, 2), (40, "E-3", 6, 8, v),
         (48, "D-3", 6, 2), (50, "D#3", 6, 2), (52, "E-3", 6, 12, v)],
    ]
    def block_pattern(k):
        roots = chords[4 * k:4 * k + 4]
        b_ev, k_ev = [], []
        for i, root in enumerate(roots):
            b_ev += walk(root, i)
            r = root + 12 if root > 4 else root + 24
            k_ev += at([(2, note_name(r), 5, 2, "047"), (10, note_name(r), 5, 2, "047")], i)
        return pattern(drums(), line(lead[k]), line(b_ev), line(k_ev))
    patterns = [block_pattern(0), block_pattern(1), block_pattern(2)]
    # coda: one hit and out
    patterns.append(pattern(["C-2/1", "-", "-", "-", "C-2/2"], ["-"] * 64,
                            ["A-1/4+C40", "-+A02", "-+A02", "-+A02", "-+A02", "-+D00"],
                            ["A-2/5+047", "-", "-", "-", "-+EC0"]))
    patterns[0][0][0] = patterns[0][0][0][:2] + (0xF, 100)       # 100 bpm
    orders = [0, 1, 2, 0, 1, 2, 3]
    return write_mod(path, "thermostat blues", samples, orders, patterns)


# =============================================================================================
# HARBOUR: six channels (FastTracker's 6CHN tag), F major, 96 bpm. Not finished.
# The bassline is done. A pad, a few hats, a sketch of a kick, four bars of lead,
# a gull. It stops where he stopped.
# =============================================================================================
def harbour(path):
    samples = [
        Sample("harbour. not done.", kick(5), 64),
        Sample("hat (todo)", hat(9), 40),
        sample_from_cycle("bass done. rest todo", cycle(64, lambda p: 0.8 * tri(p) + 0.2 * saw(p)), 64),
        sample_from_cycle("pad (todo)", cycle(32, soft), 40),
        sample_from_cycle("lead (todo)", cycle(32, lambda p: 0.6 * tri(p) + 0.4 * square(0.5)(p)), 48),
        Sample("gull. its a seagull", gull(), 56),
    ] + text(
        "--------------------",
        "for lumen. june.",
        "save my slot skit",
        "--------------------",
        "formality she said",
        "0205 it asked. i hit y",
        "the seat is mine. shh",
        "--------------------",
        "going home soon",
        "bassline done tonight",
        "bay 3 is so quiet",
    )
    # F Am Dm C: bass roots as note indexes (octave 1); the pad plays the chord
    # on one note with the arpeggio effect (major 047, minor 037).
    roots = [5, 9, 2, 0]
    pad_notes = ["F-2", "A-2", "D-2", "C-2"]
    pad_fx = ["047", "037", "037", "047"]
    def bass_line(bars):
        ev = []
        for i in range(bars):
            root = roots[i % 4]
            n = lambda semis: note_name(root + semis)
            third = 3 if root in (9, 2) else 4
            ev += at([(0, n(0), 3, 3), (3, n(0), 3, 1), (6, n(7), 3, 2), (8, n(12), 3, 3),
                      (11, n(7), 3, 2), (14, n(third), 3, 2)], i)
        return ev

    def pad_line(bars):
        return line([(i * BAR, pad_notes[i % 4], 4, None, pad_fx[i % 4]) for i in range(bars)], cut=False)
    def hat_line():
        t = ["-"] * 64
        for r in range(0, 64, 4):
            t[r + 2] = "C-2/2+C18"
        return t

    def kick_line():
        ev = [(b * BAR + r, "C-2", 1, None) for b in range(4) for r in (0, 8)]
        return line(ev)

    lead_ev = [(0, "C-3", 5, 4), (4, "A-2", 5, 2), (6, "C-3", 5, 2), (8, "D-3", 5, 6), (14, "C-3", 5, 2),
               (16, "C-3", 5, 4), (20, "E-3", 5, 2), (22, "C-3", 5, 2), (24, "A-2", 5, 8),
               (32, "D-3", 5, 4), (36, "F-3", 5, 4)]       # and it stops: no cut, no more

    silent = ["-"] * 64
    gull_a = line([(8, "C-2", 6, None)])
    gull_b = line([(40, "D-2", 6, None)])
    p0 = pattern(silent, silent, line(bass_line(4)), silent, silent, gull_a)
    p1 = pattern(silent, hat_line(), line(bass_line(4)), pad_line(4), silent, silent)
    p2 = pattern(kick_line(), hat_line(), line(bass_line(4)), pad_line(4), silent, gull_b)
    p3 = pattern(silent, silent, line(bass_line(4)), pad_line(4), line(lead_ev, cut=False), silent)
    end_bass = line(bass_line(4))
    end_bass[41] = "-+D00"
    end_lead = ["-"] * 64
    end_lead[4] = "^"
    p4 = pattern(silent, silent, end_bass, silent, end_lead, line([(30, "E-2", 6, None)]))
    patterns = [p0, p1, p2, p3, p4]
    patterns[0][0][0] = patterns[0][0][0][:2] + (0xF, 96)
    orders = [0, 1, 1, 2, 3, 4]
    return write_mod(path, "harbour (not done)", samples, orders, patterns, channels=6)


def main():
    os.makedirs(GALLERY, exist_ok=True)
    os.makedirs(DESKTOP, exist_ok=True)
    for fn, folder, name in ((sunside, GALLERY, "SUNSIDE.MOD"), (thermo, GALLERY, "THERMO.MOD"), (harbour, DESKTOP, "HARBOUR.MOD")):
        size = fn(os.path.join(folder, name))
        print("%-14s %6d bytes" % (name, size))


if __name__ == "__main__":
    main()
