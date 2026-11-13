"""Telnet and ANSI text handling shared by the caller tools.

Standard library only. Used by tools/test/e2e_telnet.py and
tools/mcp/bbs_caller.py (both run from a checkout of the repository).
"""
import re

ANSI = re.compile(r"\x1b\[[0-9;?]*[A-Za-z]")


def strip_iac(raw, on_option=None):
    """Remove Telnet negotiation from received bytes.

    Returns (clean, rest): `clean` is the data bytes, `rest` is an incomplete
    command cut off at the end of `raw`, to be put in front of the next chunk.
    `on_option(verb, option)` is called for each WILL/WONT/DO/DONT (251-254).
    """
    out = bytearray()
    i = 0
    while i < len(raw):
        if raw[i] != 255:
            out.append(raw[i])
            i += 1
            continue
        if i + 1 >= len(raw):
            return bytes(out), bytes(raw[i:])
        verb = raw[i + 1]
        if verb == 255:
            out.append(255)
            i += 2
        elif verb >= 251:
            if i + 2 >= len(raw):
                return bytes(out), bytes(raw[i:])
            if on_option:
                on_option(verb, raw[i + 2])
            i += 3
        else:
            i += 2
    return bytes(out), b""
