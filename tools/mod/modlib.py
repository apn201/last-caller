"""ProTracker module writer and a few synthesis helpers. Standard library only.

Everything here is made from arithmetic: no recordings, no input files.
The same code always writes the same bytes.

Pattern notation, one token per row:
    C-2/3      note C-2 with sample 3
    -          nothing on this row
    ^          cut the note (effect EC0)
    C-2/3+C20  a token may end with an effect: one hex digit, two hex digits
    -+F7D      an effect with no note
"""
import math
import random
import struct

ROWS = 64
PAL_CLOCK = 3546894.6  # Hz, half the Amiga PAL colour clock

PERIODS = [
    856, 808, 762, 720, 678, 640, 604, 570, 538, 508, 480, 453,  # octave 1
    428, 404, 381, 360, 339, 320, 302, 285, 269, 254, 240, 226,  # octave 2
    214, 202, 190, 180, 170, 160, 151, 143, 135, 127, 120, 113,  # octave 3
]
SEMITONES = {"C-": 0, "C#": 1, "D-": 2, "D#": 3, "E-": 4, "F-": 5,
             "F#": 6, "G-": 7, "G#": 8, "A-": 9, "A#": 10, "B-": 11}
NAMES = ["C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#", "A-", "A#", "B-"]


def note_index(note):
    """'A-2' -> 21 (C-1 is 0)."""
    return (int(note[2]) - 1) * 12 + SEMITONES[note[:2]]


def note_name(index):
    return "%s%d" % (NAMES[index % 12], index // 12 + 1)


# ---------- samples -----------------------------------------------------------------------

class Sample:
    """8-bit sample data. loop=(start, length) in bytes, or None for a one-shot."""

    def __init__(self, name="", data=b"", volume=64, loop=None):
        assert len(name) <= 22, name
        assert all(32 <= ord(c) < 127 for c in name), name
        if len(data) % 2:
            data += b"\0"
        self.name, self.data, self.volume, self.loop = name, data, volume, loop


def clip(v):
    return max(-127, min(127, int(round(v))))


def to_bytes(values):
    return struct.pack("%db" % len(values), *[clip(v) for v in values])


def cycle(n, shape, amp=1.0):
    """One period of a waveform, n bytes; shape takes a phase 0..1 and returns -1..1."""
    return to_bytes([127 * amp * shape(i / n) for i in range(n)])


def square(duty=0.5):
    return lambda p: 1.0 if p < duty else -1.0


def saw(p):
    return 2 * p - 1


def tri(p):
    return 4 * p - 1 if p < 0.5 else 3 - 4 * p


def organ(p):
    return (math.sin(2 * math.pi * p) + 0.5 * math.sin(4 * math.pi * p) + 0.25 * math.sin(6 * math.pi * p)) / 1.6


def soft(p):
    return (math.sin(2 * math.pi * p) + 0.3 * math.sin(6 * math.pi * p)) / 1.3


def one_shot(n, fn, rate=8287.0):
    """n bytes from fn(t_seconds, i, n) -> -1..1. The first two bytes are 0, as trackers write them."""
    out = [0, 0] + [127 * fn(i / rate, i, n) for i in range(2, n)]
    return to_bytes(out)


def kick(seed=1):
    def fn(t, i, n):
        f = 45 + 110 * math.exp(-t * 28)
        fn.phase += 2 * math.pi * f / 8287.0
        return math.sin(fn.phase) * math.exp(-t * 11) * min(1.0, (n - i) / 40.0)
    fn.phase = 0.0
    return one_shot(2000, fn)


def snare(seed=2):
    rng = random.Random(seed)
    def fn(t, i, n):
        tone = math.sin(2 * math.pi * 190 * t) * math.exp(-t * 30)
        noise = (rng.random() * 2 - 1) * math.exp(-t * 17)
        return 0.9 * (0.4 * tone + noise) * min(1.0, (n - i) / 40.0)
    return one_shot(1700, fn)


def hat(seed=3):
    rng = random.Random(seed)
    last = [0.0]
    def fn(t, i, n):
        x = rng.random() * 2 - 1
        y = x - last[0]  # crude high-pass
        last[0] = x
        return 0.7 * y * math.exp(-t * 70) * min(1.0, (n - i) / 20.0)
    return one_shot(520, fn)


def gull(seed=4):
    """A cry: pitch up and down with a second harmonic. Not a recording of anything."""
    def fn(t, i, n):
        k = i / n
        f = 760 + 520 * math.sin(math.pi * k) - 220 * k + 40 * math.sin(2 * math.pi * 24 * t)
        fn.phase += 2 * math.pi * f / 8287.0
        env = math.sin(math.pi * k) ** 0.6
        return env * (math.sin(fn.phase) + 0.45 * math.sin(2 * fn.phase + 0.7)) / 1.45 * min(1.0, (n - i) / 40.0)
    fn.phase = 0.0
    return one_shot(4200, fn)


def sample_from_cycle(name, data, volume=48):
    return Sample(name, data, volume, loop=(0, len(data)))


# ---------- patterns ----------------------------------------------------------------------

def parse_token(tok):
    """-> (sample, period, cmd, param)."""
    eff = None
    if "+" in tok:
        tok, eff = tok.split("+", 1)
    sample = period = cmd = param = 0
    if tok == "^":
        cmd, param = 0xE, 0xC0
    elif tok not in ("-", ""):
        note, _, smp = tok.partition("/")
        octave = int(note[2])
        assert 1 <= octave <= 3, tok
        period = PERIODS[note_index(note)]
        sample = int(smp) if smp else 0
    if eff:
        assert len(eff) == 3, tok
        cmd, param = int(eff[0], 16), int(eff[1:], 16)
    return sample, period, cmd, param


def pad(tokens, rows=ROWS):
    t = list(tokens) + ["-"] * (rows - len(tokens))
    assert len(t) == rows, len(t)
    return t


def line(events, rows=ROWS, cut=True):
    """Tokens from (row, note, sample, length_in_rows | None[, effect]). A length cuts the note."""
    t = ["-"] * rows
    for ev in events:
        row, note, smp, length = ev[:4]
        eff = ev[4] if len(ev) > 4 else ""
        t[row] = "%s/%d%s" % (note, smp, "+" + eff if eff else "")
    if cut:
        for ev in events:
            row, length = ev[0], ev[3]
            end = row + length if length else None
            if end is not None and end < rows and t[end] == "-":
                t[end] = "^"
    return t


def pattern(*channels):
    """A pattern from one token list per channel -> list of rows of cells."""
    cols = [[parse_token(x) for x in pad(c)] for c in channels]
    return [[col[r] for col in cols] for r in range(ROWS)]


# ---------- the file ----------------------------------------------------------------------

def write_mod(path, title, samples, orders, patterns, channels=4, restart=127):
    assert len(title) <= 20 and len(samples) <= 31 and 1 <= len(orders) <= 128
    assert max(orders) < len(patterns)
    out = bytearray()
    out += title.encode("ascii").ljust(20, b"\0")
    used = list(samples) + [Sample()] * (31 - len(samples))
    for s in used:
        words = len(s.data) // 2
        ls, ll = (s.loop[0] // 2, s.loop[1] // 2) if s.loop else (0, 1)
        out += s.name.encode("ascii").ljust(22, b"\0")
        out += struct.pack(">HBBHH", words, 0, s.volume if words else 0, ls, ll)
    out += bytes([len(orders), restart])
    out += bytes(orders + [0] * (128 - len(orders)))
    out += b"M.K." if channels == 4 else b"%dCHN" % channels
    for pat in patterns:
        for row in pat:
            assert len(row) == channels
            for smp, per, cmd, par in row:
                out += bytes([(smp & 0xF0) | (per >> 8), per & 0xFF, ((smp & 0x0F) << 4) | cmd, par])
    for s in used:
        out += s.data
    with open(path, "wb") as f:
        f.write(bytes(out))
    return len(out)
