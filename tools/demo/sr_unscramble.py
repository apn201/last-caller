#!/usr/bin/env python3
"""GE2: Second Reality's music as plain S3M files for the board.

The 2013 public-domain release of Second Reality (github.com/mtuomi/SecondReality, MAIN/MUSIC0.S3M and
MAIN/MUSIC1.S3M, Purple Motion and Skaven) keeps the pattern data scrambled, as the demo shipped it: only the
demo's own player unscrambled it, so every ordinary player plays noise. Each packed pattern is XORed byte by byte
with (i ^ (i << 2)) & 0xFF, i counted from the start of the pattern (its 2-byte length included). Headers,
order list, instruments and samples are untouched. This undoes it; the result plays in libopenmpt and TinyTrax.

    python tools/demo/sr_unscramble.py MUSIC0.S3M sd/BBS/DEMO/MUSIC0.S3M

Checked against independent plain copies of the same two modules: byte for byte identical.
Original work, MIT.
"""
import struct
import sys


def unscramble(data: bytes) -> bytes:
    if data[0x2C:0x30] != b"SCRM":
        raise ValueError("not an S3M file")
    out = bytearray(data)
    orders, instruments, patterns = struct.unpack_from("<HHH", data, 0x20)
    table = 0x60 + orders + instruments * 2
    for n in range(patterns):
        start = struct.unpack_from("<H", data, table + n * 2)[0] * 16
        if not start:
            continue
        length = struct.unpack_from("<H", data, start)[0]
        for i in range(2, length):
            out[start + i] ^= (i ^ (i << 2)) & 0xFF
    return bytes(out)


def main():
    if len(sys.argv) != 3:
        sys.exit(__doc__)
    with open(sys.argv[1], "rb") as f:
        data = unscramble(f.read())
    with open(sys.argv[2], "wb") as f:
        f.write(data)
    print("wrote", sys.argv[2], len(data), "bytes")


if __name__ == "__main__":
    main()
