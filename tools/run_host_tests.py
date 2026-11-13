#!/usr/bin/env python3
"""Builds and runs the host tests of the shared logic (tools/test/*.cpp) on this PC.

    python tools/run_host_tests.py [name ...]      all tests, or only those named

Uses the first compiler found: zig c++, g++, clang++, c++. The same tests run in
the server build through CTest (server/CMakeLists.txt); this is the quick way on a
machine without the server's libraries (the Windows development PC). It does not
replace the server build: say so when it is the only thing that ran.
Standard library only. Exit 0 = every test built and passed.
"""
import glob
import os
import shutil
import subprocess
import sys
import tempfile

ROOT = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
STORY = os.path.join(ROOT, "sd", "BBS", "STORY")
TESTS = {   # name -> arguments (the card's own files)
    "egg_logic_test": [],
    "power_logic_test": [],
    "modinfo_logic_test": [],
    "haunt_logic_test": [os.path.join(STORY, "TRIGGERS.INI")],
    "manifest_logic_test": [os.path.join(STORY, "MANIFEST.INI")],
    "ghost_logic_test": [os.path.join(STORY, "GHOSTS.INI"), os.path.join(STORY, "SYSOP.INI")],
    "scene_logic_test": sorted(glob.glob(os.path.join(STORY, "SCENES", "*.INI"))),
    "shell_logic_test": [os.path.join(STORY, "SHELL.INI")],
    "story_property_test": [os.path.join(STORY, "TRIGGERS.INI"), os.path.join(STORY, "MANIFEST.INI")],
}


def compiler():
    if shutil.which("zig"):
        return ["zig", "c++"]
    for c in ("g++", "clang++", "c++"):
        if shutil.which(c):
            return [c]
    return None


def main():
    cc = compiler()
    if not cc:
        print("run_host_tests: no C++ compiler (zig, g++, clang++) on this machine; nothing ran")
        return 2
    names = sys.argv[1:] or list(TESTS)
    work = tempfile.mkdtemp(prefix="tbtests")
    failed = []
    try:
        for n in names:
            if n not in TESTS:
                print("unknown test %s" % n)
                return 2
            exe = os.path.join(work, n + (".exe" if os.name == "nt" else ""))
            src = os.path.join(ROOT, "tools", "test", n + ".cpp")
            b = subprocess.run(cc + ["-std=c++17", "-Wall", "-Wextra", "-I", os.path.join(ROOT, "src", "core"), src, "-o", exe],
                               capture_output=True, text=True, cwd=ROOT)
            if b.returncode != 0:
                print("FAIL  %s (build)\n%s" % (n, (b.stdout + b.stderr)[-1500:]))
                failed.append(n)
                continue
            r = subprocess.run([exe] + TESTS[n], capture_output=True, text=True, cwd=ROOT)
            if r.returncode != 0:
                print("FAIL  %s\n%s" % (n, (r.stdout + r.stderr)[-1500:]))
                failed.append(n)
            else:
                print("PASS  %s" % n)
    finally:
        shutil.rmtree(work, ignore_errors=True)
    print("run_host_tests: %s" % ("FAILED: " + ", ".join(failed) if failed else "all %d passed" % len(names)))
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
