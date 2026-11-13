"""The board's short sound effects, made from arithmetic. 8-bit mono WAV, 11025 Hz.

    python tools/mod/gen_sfx.py

Writes sd/BBS/ASSETS/SFX/{HUM,RELAY,CHIRP,STATIC,MODEM}.WAV. The browser terminal
plays them as cues (`ESC ] 9 ; SND ; NAME BEL`). Original: no recordings, no input.
"""
import math
import os
import random
import wave

RATE = 11025
ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
OUT = os.path.join(ROOT, "sd", "BBS", "ASSETS", "SFX")


def render(seconds, fn):
    n = int(RATE * seconds)
    return [fn(i / RATE, i, n) for i in range(n)]


def fade(i, n, edge):
    return min(1.0, i / edge, (n - 1 - i) / edge)


def hum():
    """Power hum: 50 Hz and its harmonics, a slow beat, faded at both ends."""
    def fn(t, i, n):
        v = (math.sin(2 * math.pi * 50 * t) + 0.5 * math.sin(2 * math.pi * 100.7 * t)
             + 0.25 * math.sin(2 * math.pi * 150 * t) + 0.1 * math.sin(2 * math.pi * 300 * t))
        return 0.45 * v / 1.85 * (0.85 + 0.15 * math.sin(2 * math.pi * 3.1 * t)) * fade(i, n, 2200)
    return render(1.6, fn)


def relay():
    """Two clicks: a short burst of noise over a low thump."""
    rng = random.Random(11)
    def click(t, at):
        d = t - at
        if d < 0 or d > 0.05:
            return 0.0
        return (0.8 * (rng.random() * 2 - 1) * math.exp(-d * 400)) + 0.5 * math.sin(2 * math.pi * 140 * d) * math.exp(-d * 90)
    return render(0.32, lambda t, i, n: 0.8 * (click(t, 0.0) + 0.7 * click(t, 0.17)))


def chirp():
    """The reboot chirp: three steps up, the last one longer."""
    steps = [(0.00, 0.07, 660), (0.09, 0.07, 880), (0.18, 0.22, 1320)]
    def fn(t, i, n):
        v = 0.0
        for start, length, f in steps:
            if start <= t < start + length:
                k = (t - start) / length
                v += math.sin(2 * math.pi * f * t) * min(1.0, k * 12, (1 - k) * 6)
        return 0.5 * v
    return render(0.42, fn)


def static():
    """A burst of static that tears: noise gated by a stuttering envelope."""
    rng = random.Random(12)
    gates = [(0.00, 0.18), (0.22, 0.12), (0.38, 0.05), (0.46, 0.22)]
    def fn(t, i, n):
        on = any(a <= t < a + d for a, d in gates)
        return (0.5 * (rng.random() * 2 - 1) if on else 0.03 * (rng.random() * 2 - 1)) * math.exp(-t * 2.2)
    return render(0.72, fn)


def modem():
    """A modem a long way off: two tones trading places, with line noise under them."""
    rng = random.Random(13)
    state = {"phase": 0.0}
    def fn(t, i, n):
        if t < 0.5:
            f = 2100.0
        elif t < 1.0:
            f = 1300.0 + 50 * math.sin(2 * math.pi * 9 * t)
        else:
            f = 1200.0 if int(t * 24) % 2 == 0 else 2200.0     # data: two tones, fast
        state["phase"] += 2 * math.pi * f / RATE
        gain = 0.22 * fade(i, n, 1500)
        return gain * math.sin(state["phase"]) + 0.03 * (rng.random() * 2 - 1)
    return render(2.4, fn)


def write(name, samples):
    os.makedirs(OUT, exist_ok=True)
    path = os.path.join(OUT, name + ".WAV")
    with wave.open(path, "wb") as w:
        w.setnchannels(1)
        w.setsampwidth(1)
        w.setframerate(RATE)
        w.writeframes(bytes(max(0, min(255, int(round(128 + 127 * v)))) for v in samples))
    print("%-10s %6d bytes" % (name + ".WAV", os.path.getsize(path)))


if __name__ == "__main__":
    for nm, fn in (("HUM", hum), ("RELAY", relay), ("CHIRP", chirp), ("STATIC", static), ("MODEM", modem)):
        write(nm, fn())
