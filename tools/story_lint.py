#!/usr/bin/env python3
"""story-lint: checks the story on the card (sd/BBS) without a board.

  python tools/story_lint.py [--card sd/BBS] [--strict] [--quiet]

Exit 0 = no errors (warnings are listed; --strict makes them errors too).
Standard library only. Spec: .kiro/specs/verification/.

What it checks
  files     every story INI loads; no duplicate section; limits the engine has
  targets   every trigger names something the card has (a message subject, a
            file, an art file, a bulletin, a crew account), every scene,
            art file and scene section that is named exists
  flags     every flag that anything asks for is granted by something
  reach     a play-through simulated on the flags: starting with none, what
            can a caller get? Every evidence option of the manifest, every row,
            both endings must be reached (no unreachable fact, no ending lost)
  softlock  the same, with the worst order: a flag that a "lacks" rule would
            refuse once the caller already holds another flag. Whatever the
            optimistic run reaches, the pessimistic run must reach too
  redundancy the story bible says a fate is provable from several pieces of
            evidence: an evidence flag with one source is a warning
  scenes    every section is reachable from START, every path ends, targets exist
  voice     (warnings) characters and words the writing rules ban in text
"""
import argparse
import os
import re
import sys

# ---------------------------------------------------------------- INI

class Ini:
    """Sections in file order; keys lower case; the first value of a key wins (as iniGet())."""

    def __init__(self, path):
        self.path = path
        self.sections = []          # [(name_upper, {key: value}, line)]
        self.dupes = []
        self.bad = []
        seen = set()
        cur = None
        try:
            with open(path, encoding="utf-8", errors="strict", newline="") as f:
                lines = f.read().splitlines()
        except (OSError, UnicodeDecodeError) as e:
            self.bad.append("cannot read: %s" % e)
            return
        for n, raw in enumerate(lines, 1):
            t = raw.strip()
            if not t or t.startswith(";") or t.startswith("#"):
                continue
            if t.startswith("["):
                end = t.find("]")
                if end < 0:
                    self.bad.append("line %d: '[' without ']'" % n)
                    continue
                name = t[1:end].strip().upper()
                cur = {}
                if name in seen:
                    self.dupes.append((name, n))
                seen.add(name)
                self.sections.append((name, cur, n))
            elif "=" in t and cur is not None:
                k, v = t.split("=", 1)
                k = k.strip().lower()
                if k and k not in cur:
                    cur[k] = v.strip()
            elif cur is None:
                self.bad.append("line %d: text before the first section" % n)

    def get(self, sec, key, default=""):
        for name, kv, _ in self.sections:
            if name == sec.upper():
                return kv.get(key, default)
        return default

    def named(self):
        return {name: kv for name, kv, _ in self.sections}


def flags(text):
    return [w.upper() for w in re.split(r"[,\s]+", text or "") if w]


def norm_name(s):
    return re.sub(r"[^A-Z0-9]", "", (s or "").upper())


# ---------------------------------------------------------------- the lint

class Lint:
    def __init__(self, card, strict):
        self.card = card
        self.strict = strict
        self.errors = []
        self.warnings = []
        self.story = os.path.join(card, "STORY")
        self.scene_thens = []

    def err(self, where, msg):
        self.errors.append("%s: %s" % (where, msg))

    def warn(self, where, msg):
        (self.errors if self.strict else self.warnings).append("%s: %s" % (where, msg))

    def load(self, rel, required=True):
        path = os.path.join(self.story, rel)
        if not os.path.isfile(path):
            if required:
                self.err(rel, "missing")
            return None
        ini = Ini(path)
        for b in ini.bad:
            self.err(rel, b)
        for name, n in ini.dupes:
            self.err(rel, "section [%s] twice (line %d): the second is ignored or breaks the file" % (name, n))
        return ini

    # -------- card facts
    def card_facts(self):
        self.files = set()      # upper case file names in FILES, ANSI, ASCII, ASSETS, BULLETINS
        for top in ("FILES", "ANSI", "ASCII", "ASSETS", "BULLETINS", "DOORS"):
            for root, _, names in os.walk(os.path.join(self.card, top)):
                for n in names:
                    self.files.add(n.upper())
        self.subjects = set()   # message subjects as the board matches them: upper case, 39 characters
        self.subject_conf = {}  # subject -> conference ids that hold it
        base = os.path.join(self.card, "MSGBASE")
        for root, _, names in os.walk(base):
            for n in names:
                if not n.upper().endswith(".TXT"):
                    continue
                try:
                    with open(os.path.join(root, n), encoding="utf-8", errors="replace") as f:
                        for line in f:
                            if line.lower().startswith("subject:"):
                                subj = line.split(":", 1)[1].strip().upper()[:39]
                                self.subjects.add(subj)
                                self.subject_conf.setdefault(subj, set()).add(os.path.basename(root))
                except OSError:
                    pass
        # Gates (Q12d): every story post and file, with what a caller needs to see it.
        self.access = {"SYSOP", "READONLY", "BACKROOM", "VAULT"}
        self.conf_flag = {}
        cp = os.path.join(self.card, "CONFIG", "CONFERENCES.INI")
        if os.path.isfile(cp):
            for name, kv, _ in Ini(cp).sections:
                self.conf_flag[name] = flags(kv.get("required_flag"))
        self.subject_gates = {}     # subject -> [gate flags of each post with it]
        self.hidden_subjects = set()  # subjects of posts behind a Needs: line
        for root, _, names in os.walk(base):
            conf = os.path.basename(root)
            for n in names:
                if not n.upper().endswith(".TXT"):
                    continue
                with open(os.path.join(root, n), encoding="utf-8", errors="replace") as f:
                    text = f.read().replace("\r\n", "\n")
                for part in text.split("#MSG\n")[1:]:
                    hdr = part.split("\n\n", 1)[0]
                    subj, need = "", []
                    for line in hdr.split("\n"):
                        if line.lower().startswith("subject:"):
                            subj = line.split(":", 1)[1].strip().upper()[:39]
                        elif line.lower().startswith("needs:"):
                            need = flags(line.split(":", 1)[1])
                    if need:
                        self.hidden_subjects.add(subj)
                    self.subject_gates.setdefault(subj, []).append(self.conf_flag.get(conf, []) + need)
        self.file_gates = {}        # upper case file name -> [gate flags of each copy]
        fbase = os.path.join(self.card, "FILES")
        if os.path.isdir(fbase):
            for area in os.listdir(fbase):
                adir = os.path.join(fbase, area)
                ini = os.path.join(adir, "AREA.INI")
                area_flag = flags(Ini(ini).get("AREA", "required_flag")) if os.path.isfile(ini) else []
                for root, _, names in os.walk(adir):
                    extra = {}
                    np = os.path.join(root, "NEEDS.BBS")
                    if os.path.isfile(np):
                        for line in open(np, encoding="utf-8", errors="replace"):
                            bits = line.strip().rsplit(" ", 1)
                            if len(bits) == 2 and not line.startswith(";"):
                                extra[bits[0].strip().upper()] = flags(bits[1])
                    for n in names:
                        self.file_gates.setdefault(n.upper(), []).append(area_flag + extra.get(n.upper(), []))
        self.bulletins = set()
        b = Ini(os.path.join(self.card, "CONFIG", "BULLETINS.INI")) if os.path.isfile(os.path.join(self.card, "CONFIG", "BULLETINS.INI")) else None
        if b:
            for _, kv, _ in b.sections:
                if kv.get("title"):
                    self.bulletins.add(kv["title"].strip().upper())

    # -------- main
    def run(self):
        self.card_facts()
        trig = self.load("TRIGGERS.INI")
        man = self.load("MANIFEST.INI")
        diag = self.load("DIAG.INI", False)
        shell = self.load("SHELL.INI", False)
        sysop = self.load("SYSOP.INI", False)
        ghosts = self.load("GHOSTS.INI", False)
        acc = self.load("ACCOUNTS.INI", False)
        power = self.load("POWER.INI", False)
        scenes = {}
        sdir = os.path.join(self.story, "SCENES")
        if os.path.isdir(sdir):
            for n in sorted(os.listdir(sdir)):
                if n.upper().endswith(".INI"):
                    ini = self.load("SCENES/" + n)
                    if ini:
                        scenes[n[:-4].upper()] = ini
        if not trig or not man:
            return

        # Grants: unit = (label, needs [flags], grants [flags]).
        units = []
        required = []           # (where, flag)
        lacking = {}            # unit label -> lacks flags (softlock run)

        # triggers
        rules = []
        if len(trig.sections) > 300:
            self.err("TRIGGERS.INI", "%d rules, the board loads 300" % len(trig.sections))
        for name, kv, line in trig.sections:
            ev = kv.get("event", "").lower()
            if ev not in ("login", "crew_login", "read", "file", "art", "door", "idle", "minutes", "screen"):
                self.err("TRIGGERS.INI [%s]" % name, "unknown event '%s'" % ev)
                continue
            if len(name) > 24:
                self.err("TRIGGERS.INI [%s]" % name, "id longer than 24")
            rules.append((name, kv))
            self.check_trigger_target("TRIGGERS.INI [%s]" % name, ev, kv, acc, scenes)
            needs = flags(kv.get("has"))
            gr = flags(kv.get("set"))
            self.check_protected("TRIGGERS.INI [%s]" % name, gr)
            visible = any(kv.get(k) for k in ("page", "mail_body", "post_body", "art", "scene"))
            if visible:
                gr.append("T_" + name)
            sc = kv.get("scene", "").upper()
            lab = "rule " + name
            for gate in self.gates_of(ev, kv.get("match", "")):
                units.append((lab, needs + gate, gr))
            if kv.get("lacks"):
                lacking[lab] = flags(kv["lacks"])
            for f in needs:
                required.append((lab, f))
            if sc:
                units.append(("scene " + sc + " entered", needs, ["SC_ENTER_" + sc]))
            if kv.get("art") and not self.art_exists(kv["art"]):
                self.err(lab, "art '%s' is not on the card" % kv["art"])

        # scenes
        for sid, ini in scenes.items():
            self.check_scene(sid, ini, units, required, lacking)

        # diag
        if diag:
            for name, kv, _ in diag.sections:
                if name == "DIAG":
                    for f in flags(kv.get("needs")):
                        required.append(("DIAG door", f))
                        units.append(("DIAG door open", [], []))
                    continue
                lab = "diag " + name
                self.check_protected("DIAG.INI [%s]" % name, flags(kv.get("set")))
                units.append((lab, flags(kv.get("has")), flags(kv.get("set"))))
                for f in flags(kv.get("has")):
                    required.append((lab, f))
            dn = diag.named().get("DIAG", {})
            units.append(("DIAG door", flags(dn.get("needs")), ["DOOR_DIAG"]))

        # shell
        final_has = []
        if shell:
            sh = shell.named().get("SHELL", {})
            door_needs = flags(sh.get("needs"))
            if sh.get("account"):
                door_needs = door_needs + ["LOGIN_" + sh["account"].strip().upper()]
                if acc and sh["account"].strip().upper() not in {n for n, _, _ in acc.sections}:
                    self.err("SHELL.INI", "account=%s is not in ACCOUNTS.INI" % sh["account"])
            self.check_protected("SHELL.INI final_set", flags(sh.get("final_set")))
            for f in door_needs:
                required.append(("SHELL door", f))
            units.append(("SHELL door", door_needs, ["DOOR_SHELL"]))
            final_has = flags(sh.get("final_has"))
            for f in final_has:
                required.append(("SHELL final command", f))
            fscene = sh.get("final_scene", "").upper()
            if fscene and fscene not in scenes:
                self.err("SHELL.INI", "final_scene '%s' has no file in SCENES" % fscene)
            if sh.get("final_cmd"):
                units.append(("SHELL final", ["DOOR_SHELL"] + final_has,
                              flags(sh.get("final_set")) + (["SC_ENTER_" + fscene] if fscene else [])))
            for name, kv, _ in shell.sections:
                if name.startswith("/") and kv.get("has"):
                    for f in flags(kv["has"]):
                        required.append(("SHELL " + name, f))

        # sysop / ghosts
        if sysop:
            for name, kv, _ in sysop.sections:
                if kv.get("trigger") and kv["trigger"].upper() not in {n for n, _ in rules}:
                    self.err("SYSOP.INI [%s]" % name, "trigger=%s is not a rule in TRIGGERS.INI" % kv["trigger"])
                self.check_protected("SYSOP.INI [%s]" % name, flags(kv.get("set")))
                units.append(("sysop " + name, flags(kv.get("has")), flags(kv.get("set"))))
                for f in flags(kv.get("has")):
                    required.append(("sysop " + name, f))
                # G2: a chat option or gate may start a scene (then=); a listed option needs words to type.
                if kv.get("then"):
                    nxt = kv["then"].strip().upper()
                    self.scene_thens.append(("SYSOP.INI [%s]" % name, nxt))
                    units.append(("sysop " + name + " then", flags(kv.get("has")), ["SC_ENTER_" + nxt]))
                if kv.get("option") and not kv.get("keys"):
                    self.err("SYSOP.INI [%s]" % name, "option= needs keys= (the words that do the same as its number)")
        if ghosts:
            for name, kv, _ in ghosts.sections:
                for f in flags(kv.get("has")):
                    required.append(("ghost " + name, f))
                self.check_protected("GHOSTS.INI [%s]" % name, flags(kv.get("set")))
                units.append(("ghost " + name, flags(kv.get("has")), flags(kv.get("set"))))

        # accounts
        crew_logins = set()
        if acc:
            for name, kv, _ in acc.sections:
                if kv.get("password"):
                    crew_logins.add(name)
                    # learned_by= (lint only): the flag a caller holds once the password can be found.
                    own = [f for f in flags(kv.get("flags")) if f not in self.access]
                    units.append(("login " + name, flags(kv.get("learned_by")), ["LOGIN_" + name] + own))
                    for f in flags(kv.get("learned_by")):
                        required.append(("ACCOUNTS.INI [%s] learned_by" % name, f))
        # The board's own grants: the overrun in the post path (M5) sets OVR for any own account.
        units.append(("post path overrun", [], ["OVR"]))
        if "MEM_GLITCH" in scenes:
            units.append(("post path overrun scene", ["OVR"], ["SC_ENTER_MEM_GLITCH"]))
        # G2: node chat (C) is one free-text chat; its options and gates are lines of SYSOP.INI (see above).
        required.extend(self.check_rive())
        self.check_demo(scenes)   # GE: the end demo
        for name, kv in rules:
            if kv.get("event", "").lower() == "crew_login":
                who = kv.get("match", "").upper()
                if who not in crew_logins:
                    self.err("TRIGGERS.INI [%s]" % name, "crew_login '%s' has no account with a password in ACCOUNTS.INI" % who)

        # manifest
        self.check_manifest(man, scenes, units, required, lacking)

        # ---- flag provenance
        granted = set()
        for _, _, g in units:
            granted.update(g)
        for where, nxt in self.scene_thens:
            if nxt not in scenes:
                self.err(where, "then=%s has no file in STORY/SCENES" % nxt)
        engine = lambda f: f.startswith("T_") or f.startswith("SC_") or f.startswith("E_") or f.startswith("G_")
        for where, f in required:
            if f not in granted and not engine(f):
                self.err(where, "asks for flag %s, which nothing grants" % f)

        self.required_flags = {f for _, f in required}
        # ---- reachability
        opt = self.fixpoint(units, {}, set())
        pess = self.fixpoint(units, lacking, opt)
        self.check_reach(man, opt, pess, units)
        self.check_chain(units, opt, pess, lacking)

        # ---- redundancy of evidence
        self.check_redundancy(man, units)
        self.check_open_sources(rules, units)

        # ---- flag budget: the story record holds 4000 characters
        names = {g for _, _, gr in units for g in gr if not g.startswith(("SC_ENTER_", "ROWSHOWN_", "ROWRIGHT_", "DOOR_"))}
        budget = sum(len(f) + 1 for f in names)
        if budget > 4000:
            self.err("flags", "%d distinct story flags use %d characters; the record holds 4000" % (len(names), budget))
        elif budget > 3000:
            self.warn("flags", "story flags use %d of 4000 characters" % budget)
        self.stats = (len(rules), len(scenes), len(names), budget)

        # ---- voice
        self.check_voice()
        self.check_keys()

        # ---- the notebook (Q21)
        self.check_notes(self.load("NOTES.INI", False), trig, scenes, diag, shell, sysop, ghosts)

        # ---- power
        if power:
            pn = power.named().get("POWER", {})
            if pn.get("timer_sec", "x").isdigit() and int(pn["timer_sec"]) < 30:
                self.warn("POWER.INI", "timer_sec=%s: a first-time player cannot finish the prologue in that time" % pn["timer_sec"])

    # -------- parts
    def check_protected(self, where, granted):
        """R3.2: the story never grants an access flag (the real SYSOP above all)."""
        for f in granted:
            if f in self.access:
                self.err(where, "grants the access flag %s; the story may never set it" % f)

    def gate_alternatives(self, gate):
        """A gate as the board checks it: held, or SYSOP view for every flag that is not an access flag."""
        if not gate:
            return [[]]
        out = [gate]
        if not any(f in self.access for f in gate):
            out.append(["SYSVW"])
        return out

    def gates_of(self, ev, match):
        """The flag sets that let a caller see what a rule matches (any one of them is enough)."""
        if ev == "read" and match.startswith("~"):
            gates = self.subject_gates.get(match[1:].strip().upper()[:39], [[]])
        elif ev in ("file", "art"):
            gates = self.file_gates.get(match.upper(), [[]])
        elif ev == "crew_login":
            gates = [["LOGIN_" + match.upper()]]
        else:
            gates = [[]]
        if any(not g for g in gates):
            return [[]]
        out = []
        for g in gates:
            for alt in self.gate_alternatives(g):
                if alt not in out:
                    out.append(alt)
        return out

    def art_exists(self, name):
        n = name.upper()
        return any((n + ext) in self.files for ext in ("", ".ANS", ".ASC"))

    def check_trigger_target(self, where, ev, kv, acc, scenes):
        m = kv.get("match", "")
        if ev == "read" and m.startswith("~"):
            if m[1:].strip().upper()[:39] not in self.subjects:
                self.err(where, "no message has the subject '%s' (matched whole, 39 characters, any case)" % m[1:])
        elif ev in ("file", "art"):
            if m.upper() not in self.files:
                self.err(where, "no file '%s' on the card" % m)
        elif ev == "screen":
            if m.upper().startswith("BULLETIN "):
                if m.upper()[9:].strip() not in self.bulletins:
                    self.err(where, "no bulletin titled '%s'" % m[9:])
            elif m.upper() not in ("USERS", "WHO"):
                self.err(where, "unknown screen '%s'" % m)
        elif ev == "crew_login" and acc and m.upper() not in {n for n, _, _ in acc.sections}:
            self.err(where, "no account '%s' in ACCOUNTS.INI" % m)
        sc = kv.get("scene", "").upper()
        if sc and sc not in scenes:
            self.err(where, "scene '%s' has no file in STORY/SCENES" % sc)
        if kv.get("page") and not kv.get("page_from"):
            self.warn(where, "page= without page_from=")

    def check_scene(self, sid, ini, units, required, lacking):
        where = "SCENES/%s.INI" % sid
        nodes = {n: kv for n, kv, _ in ini.sections if n != "SCENE"}
        head = ini.named().get("SCENE")
        if head is None:
            self.err(where, "no [SCENE] section")
            return
        if "START" not in nodes:
            self.err(where, "no [START] section")
            return
        gate = flags(head.get("has"))
        for f in gate:
            required.append((where, f))
        units.append(("scene " + sid + " play", gate + ["SC_ENTER_" + sid], ["SC_" + sid]))
        # edges
        edges = {}
        for n, kv in nodes.items():
            out = []
            for k in kv:
                if k in ("go", "if_go", "typed_else") or re.fullmatch(r"go\d", k) or re.fullmatch(r"typedgo\d", k):
                    out.append(kv[k].upper())
            edges[n] = out
            for t in out:
                if t != "END" and t not in nodes:
                    self.err(where, "[%s] goes to [%s], which does not exist" % (n, t))
            has = flags(kv.get("if_has"))
            for f in has:
                required.append((where + " [%s]" % n, f))
            choices = sorted(k for k in kv if re.fullmatch(r"choice\d", k))
            for c in choices:
                i = c[-1]
                if "go" + i not in kv:
                    self.err(where, "[%s] %s has no go%s" % (n, c, i))
            if not choices and "go" not in kv and "if_go" not in kv and not any(k.startswith("go") for k in kv):
                self.err(where, "[%s] has no go: the scene would stop dead there" % n)
            # grants by this node (the scene must be entered and the node reached; the node
            # check below is by graph reachability, so grant at scene level)
        reach, stack = set(), ["START"]
        while stack:
            n = stack.pop()
            if n in reach or n not in nodes:
                continue
            reach.add(n)
            stack.extend(edges.get(n, []))
        for n in nodes:
            if n not in reach:
                self.err(where, "[%s] can never be reached from START" % n)
        # every reachable node can reach END
        can_end = set()
        changed = True
        while changed:
            changed = False
            for n in reach:
                if n in can_end:
                    continue
                if "END" in edges.get(n, []):
                    can_end.add(n)
                    changed = True
                elif any(t in can_end for t in edges.get(n, [])):
                    can_end.add(n)
                    changed = True
        for n in reach:
            if n not in can_end:
                self.err(where, "[%s] cannot reach END: the caller would be stuck in the scene" % n)
        # The flags a caller needs to reach each node: a choice with hasN= on the way adds them.
        # (if_has jumps and lacks are taken as possible; a few alternative sets per node.)
        conds = {"START": [frozenset()]}
        queue = ["START"]
        while queue:
            n = queue.pop()
            kv = nodes.get(n)
            if kv is None:
                continue
            for cset in list(conds.get(n, [])):
                outs = []
                for k in ("go", "if_go"):
                    if kv.get(k):
                        outs.append((kv[k].upper(), cset | (frozenset(flags(kv.get("if_has"))) if k == "if_go" else frozenset())))
                if kv.get("typed_else"):   # Q21
                    outs.append((kv["typed_else"].upper(), cset))
                for k, v in kv.items():
                    m = re.fullmatch(r"go(\d)", k)
                    if m:
                        outs.append((v.upper(), cset | frozenset(flags(kv.get("has" + m.group(1))))))
                    m = re.fullmatch(r"typedgo(\d)", k)   # Q18: a typed word at a choice node
                    if m:
                        outs.append((v.upper(), cset | frozenset(flags(kv.get("typedhas" + m.group(1))))))
                for t, s in outs:
                    if t == "END" or t not in nodes:
                        continue
                    have = conds.setdefault(t, [])
                    if any(h <= s for h in have) or len(have) >= 6:
                        continue
                    have[:] = [h for h in have if not s <= h] + [s]
                    queue.append(t)
        # grants: a node's flags (and its choices') are granted when the node can be reached
        for n, kv in nodes.items():
            if n not in reach:
                continue
            g = flags(kv.get("set"))
            for k, v in kv.items():
                if re.fullmatch(r"set\d", k):
                    g += flags(v)
            self.check_protected(where + " [%s]" % n, g)
            nxt = kv.get("then", "").upper()   # then= (Q12d): hand over to another scene at the end
            if nxt and nxt != "CHAT":
                self.scene_thens.append((where + " [%s]" % n, nxt))
            for cset in conds.get(n, [frozenset()]):
                if g:
                    units.append(("scene %s [%s]" % (sid, n), gate + ["SC_ENTER_" + sid] + sorted(cset), g))
                if nxt and nxt != "CHAT":
                    units.append(("scene %s [%s] then" % (sid, n), gate + ["SC_ENTER_" + sid] + sorted(cset), ["SC_ENTER_" + nxt]))
            for k, v in kv.items():
                if re.fullmatch(r"has\d", k) or re.fullmatch(r"typedhas\d", k):
                    for f in flags(v):
                        required.append((where + " [%s]" % n, f))
            for k in ("art",):
                if kv.get(k) and not self.art_exists(kv[k]) and not self.art_exists(kv[k].replace("STORY/", "")):
                    self.err(where, "[%s] art '%s' is not on the card" % (n, kv[k]))
        self.check_protected(where + " [SCENE]", flags(head.get("set")))
        for f in flags(head.get("set")):
            units.append(("scene %s [SCENE] set" % sid, gate + ["SC_ENTER_" + sid], [f]))

    def check_manifest(self, man, scenes, units, required, lacking):
        named = man.named()
        mn = named.get("MANIFEST", {})
        fs = mn.get("full_scene", "").upper()
        if fs and fs not in scenes:
            self.err("MANIFEST.INI", "full_scene '%s' has no file in SCENES" % fs)
        opts = {}
        rows = []
        for name, kv, _ in man.sections:
            if name.startswith("OPT_"):
                opts[name[4:]] = kv
                if len(kv.get("text", "")) > 64:
                    self.err("MANIFEST.INI [%s]" % name, "text longer than 64")
                if kv.get("slot") not in ("1", "2", "3"):
                    self.err("MANIFEST.INI [%s]" % name, "slot must be 1, 2 or 3")
            elif name.startswith("ROW_"):
                rows.append((name[4:], kv))
            elif name.startswith("NOTE_"):
                if len(kv.get("text", "")) > 90:
                    self.err("MANIFEST.INI [%s]" % name, "text longer than 90")
                for f in flags(kv.get("has")):
                    required.append(("MANIFEST.INI [%s]" % name, f))
        self.manifest_rows = rows
        self.manifest_opts = opts
        if len(rows) > 8:
            self.err("MANIFEST.INI", "%d rows, the board loads 8" % len(rows))
        if rows and rows[-1][1].get("kind") != "last":
            self.err("MANIFEST.INI", "the last row in the file must be kind=last")
        for rid, kv in rows:
            where = "MANIFEST.INI [ROW_%s]" % rid
            last = kv.get("kind") == "last"
            need_all = flags(kv.get("needs"))
            need_any = flags(kv.get("needs_any"))
            ans = []
            for i in (1, 2, 3):
                a = kv.get("a%d" % i, "").upper()
                if a not in opts:
                    self.err(where, "a%d '%s' is not an option" % (i, a))
                    continue
                o = opts[a]
                if o.get("slot") != str(i):
                    self.err(where, "a%d '%s' sits in slot %s" % (i, a, o.get("slot")))
                if (o.get("group", "crew") == "last") != last:
                    self.err(where, "a%d '%s' is in the wrong group" % (i, a))
                ans.append(o)
            if not kv.get("done"):
                self.err(where, "no done= flag")
            for f in need_all + need_any:
                required.append((where, f))
            for o in ans:
                for f in flags(o.get("has")):
                    required.append((where + " answer", f))
            # a row can be confirmed once shown and every right option can be seen. Crew rows
            # confirm two at a time, so a lone row waits for a second: that is a unit with the
            # others, modelled as the pair (any two crew rows) in fixpoint(); here we state the
            # row's own needs.
            show = need_all
            units.append(("row " + rid, show, [("ROWSHOWN_" + rid)]))
            units.append(("row " + rid + " answers", [f for o in ans for f in flags(o.get("has"))] + [("ROWSHOWN_" + rid)], [("ROWRIGHT_" + rid)]))
            if need_any:
                for f in need_any:
                    units.append(("row " + rid + " shown by " + f, [f], ["ROWSHOWN_" + rid]))
            kv["_last"] = last
        crew = [rid for rid, kv in rows if not kv["_last"]]
        done_of = {rid: kv.get("done") for rid, kv in rows}
        # crew rows: confirmed in pairs, so every crew row needs one other crew row also right
        for rid in crew:
            others = [o for o in crew if o != rid]
            for o in others:
                units.append(("row %s confirmed with %s" % (rid, o), ["ROWRIGHT_" + rid, "ROWRIGHT_" + o], [done_of[rid]]))
        for rid, kv in rows:
            if kv["_last"]:
                units.append(("row " + rid + " confirmed", ["ROWRIGHT_" + rid] + flags(kv.get("needs")), [done_of[rid]]))
        if fs:
            last_done = [done_of[rid] for rid, kv in rows if kv["_last"]]
            units.append(("manifest full scene", last_done, ["SC_ENTER_" + fs]))
        # unique done flags
        dones = [d for d in done_of.values() if d]
        if len(dones) != len(set(dones)):
            self.err("MANIFEST.INI", "two rows leave the same done= flag")

    def fixpoint(self, units, lacking, optimistic):
        """Flags a caller can hold. With `lacking`, a unit whose lacks= flag is in `optimistic`
        is taken as refused (the worst order of play)."""
        have = set()
        moved = True
        while moved:
            moved = False
            for lab, needs, grants in units:
                if lab in lacking and any(f in optimistic for f in lacking[lab]):
                    continue
                if all(n in have for n in needs):
                    for g in grants:
                        if g not in have:
                            have.add(g)
                            moved = True
        return have

    def check_reach(self, man, opt, pess, units):
        for rid, kv in self.manifest_rows:
            where = "MANIFEST.INI [ROW_%s]" % rid
            done = kv.get("done", "")
            if done and done not in opt:
                self.err(where, "can never be confirmed (flag %s is not reached)" % done)
            for i in (1, 2, 3):
                o = self.manifest_opts.get(kv.get("a%d" % i, "").upper())
                if o:
                    for f in flags(o.get("has")):
                        if f not in opt:
                            self.err(where, "right answer %d needs %s, which no play reaches" % (i, f))
        for name, o in self.manifest_opts.items():
            for f in flags(o.get("has")):
                if f not in opt:
                    self.err("MANIFEST.INI [OPT_%s]" % name, "option can never appear (%s is not reached)" % f)
        # endings: any scene or unit that grants END1 / END2
        for end in ("END1", "END2"):
            if end not in opt:
                self.err("endings", "%s is never granted by a reachable path" % end)
        needed = self.required_flags | set(ends for ends in ("END1", "END2"))
        for f in sorted((opt - pess) & needed):
            self.err("softlock", "flag %s is lost to a caller who already holds a flag a 'lacks' rule refuses" % f)

    # The milestone chain (docs/PUZZLE_CHAIN.md): the flag each milestone leaves, and the hard
    # dependencies the design names. Every milestone is reached in the best and the worst order,
    # and every named dependency is a real gate (without it, the later milestone is not reached).
    CHAIN = [("M1 power", "MET_ELIZA"), ("M3 crew area", "ACT2"), ("M4 archive", "ACT3"), ("M5 overrun", "OVR"),
             ("M6 cure", "CURE"), ("M6 SYSOP view", "SYSVW"), ("M7 ELIZA", "ELZ_IN"), ("M8 shell", "DOOR_SHELL"),
             ("M9 ending", "END1")]
    DEPENDS = [("SYSVW", "OVR"), ("SYSVW", "CURE"), ("ELZ_IN", "SYSVW"), ("DOOR_SHELL", "SYSVW"),
               ("DOOR_SHELL", "NOWHERE_OPEN"), ("END1", "DOOR_SHELL"), ("END1", "M_Z"), ("CURE", "CREW"), ("ACT3", "CREW")]

    def check_chain(self, units, opt, pess, lacking):
        for name, f in self.CHAIN:
            if f not in opt:
                self.err("chain", "%s (%s) is never reached" % (name, f))
            elif f not in pess:
                self.err("chain", "%s (%s) is lost in the worst order of play" % (name, f))
        for later, earlier in self.DEPENDS:
            cut = [(lab, needs, [g for g in grants if g != earlier]) for lab, needs, grants in units]
            if later in self.fixpoint(cut, {}, set()):
                self.err("chain", "%s is reached without %s: the gate does not hold" % (later, earlier))

    def check_redundancy(self, man, units):
        sources = {}
        for lab, needs, grants in units:
            if lab.startswith("row ") or lab.startswith("manifest"):
                continue
            for g in grants:
                sources.setdefault(g, set()).add(lab)
        for rid, kv in self.manifest_rows:
            for i in (1, 2, 3):
                o = self.manifest_opts.get(kv.get("a%d" % i, "").upper())
                if not o:
                    continue
                for f in flags(o.get("has")):
                    n = len(sources.get(f, ()))
                    if n == 1:
                        self.warn("evidence", "%s (answer %d of %s) has one source only: %s" % (f, i, rid, next(iter(sources[f]))))

    def gated_conferences(self):
        """Conference ids that need a flag (a crew visit opens them)."""
        p = os.path.join(self.card, "CONFIG", "CONFERENCES.INI")
        out = set()
        if os.path.isfile(p):
            for name, kv, _ in Ini(p).sections:
                if kv.get("required_flag"):
                    out.add(name)
        return out

    def gated_files(self):
        """Upper case file names in a file area that needs a flag."""
        out = set()
        base = os.path.join(self.card, "FILES")
        if not os.path.isdir(base):
            return out
        for area in os.listdir(base):
            ini = os.path.join(base, area, "AREA.INI")
            if os.path.isfile(ini) and any(kv.get("required_flag") for _, kv, _ in Ini(ini).sections):
                for root, _, names in os.walk(os.path.join(base, area)):
                    out.update(n.upper() for n in names)
        return out

    def check_open_sources(self, rules, units):
        """A caller can lose what a crew visit gave: carried flags live in the connection until an own
        account signs in. So every fact a right answer needs must also have a source that needs no
        crew visit (an open conference, an open file area, a bulletin, a scene, the SysOp)."""
        gated_conf = self.gated_conferences()
        gated_file = self.gated_files()
        open_flags = set()
        for name, kv in rules:
            ev = kv.get("event", "").lower()
            m = kv.get("match", "")
            is_open = True
            if ev == "crew_login":
                is_open = False
            elif ev == "read" and m.startswith("~"):
                subj = m[1:].strip().upper()[:39]
                is_open = any(not g for g in self.subject_gates.get(subj, []))
            elif ev in ("file", "art"):
                is_open = m.upper() not in gated_file
            if is_open:
                open_flags.update(flags(kv.get("set")))
        for lab, _, grants in units:
            if not lab.startswith("rule ") and not lab.startswith("row ") and not lab.startswith("manifest"):
                open_flags.update(grants)
        for rid, kv in self.manifest_rows:
            for i in (1, 2, 3):
                o = self.manifest_opts.get(kv.get("a%d" % i, "").upper())
                if not o:
                    continue
                for f in flags(o.get("has")):
                    if f not in open_flags:
                        self.err("open sources", "%s (answer %d of %s) can only be earned on a crew visit; a caller who loses the visit is stuck" % (f, i, rid))

    BANNED = ["delve", "tapestry", "testament", "realm", "beacon", "embark", "symphony", "paradigm shift",
              "game-changer", "unleash", "multifaceted", "ever-evolving", "nestled", "bustling", "indelible",
              "plethora", "myriad", "quintessential", "elucidate", "underscore", "a reminder that", "stands as"]
    ODD = {"\u2014": "em dash", "\u2013": "en dash", "\u201c": "curly quote", "\u201d": "curly quote",
           "\u2018": "curly quote", "\u2019": "curly quote", "\u2026": "ellipsis character"}

    # Q20 UX: caller text that names a key (docs/PLATFORM_UX.md section 3). A key that moved or went away
    # must not still be named. MOVED maps the old wording to the key that replaced it.
    MOVED_KEYS = [(r"\b(press|type|hit|use|key)\s+\[?P\]?\b[^.\n]{0,20}\b(page|sysop)", "O pages the SysOp now"),
                  (r"\b(press|type|hit|use|key)\s+\[?P\]?\b[^.\n]{0,20}\bpost", "W writes a post now"),
                  (r"\b(press|type|hit|use|key)\s+\[?R\]?\b[^.\n]{0,20}\b(read|list)", "L lists messages now"),
                  (r"press\s+enter\s+(to\s+continue|for\b|:)", "continue is any key now (Press any key)")]
    # Keys the board gives for what a caller may be told by name in a story text.
    def check_keys(self):
        for top in ("STORY", "MSGBASE", "BULLETINS", "FILES"):
            for root, _, names in os.walk(os.path.join(self.card, top)):
                for n in names:
                    if not n.upper().endswith((".INI", ".TXT", ".LOG", ".MEM", ".RPT", ".CFG", ".ASC")):
                        continue
                    p = os.path.join(root, n)
                    rel = os.path.relpath(p, self.card).replace("\\", "/")
                    try:
                        low = open(p, encoding="utf-8").read()
                    except (UnicodeDecodeError, OSError):
                        continue
                    for pat, now in self.MOVED_KEYS:
                        if re.search(pat, low, re.I):
                            self.err(rel, "names a key that moved (%s)" % now)

    NOTE_KINDS = ("login", "crew_login", "read", "file", "art", "door", "screen", "scene", "page", "eliza", "ghost", "diag", "shell")

    def chain_clues(self):
        """The clue ids the notebook has to cover: E01.. in the story bible's evidence matrix, M1..M9 and the
        side puzzles P1.. in docs/PUZZLE_CHAIN.md. (None, None) when the docs are not next to the card."""
        root = os.path.normpath(os.path.join(self.card, "..", ".."))
        bible = os.path.join(root, ".kiro", "specs", "story-bible", "design.md")
        chain = os.path.join(root, "docs", "PUZZLE_CHAIN.md")
        if not (os.path.isfile(bible) and os.path.isfile(chain)):
            return None
        want = {}
        with open(bible, encoding="utf-8") as f:
            for line in f:
                m = re.match(r"\| (E\d+b?) \|", line)
                if m:
                    want[m.group(1).upper()] = "story bible evidence matrix"
        with open(chain, encoding="utf-8") as f:
            for line in f:
                m = re.match(r"\| (M\d) \|", line)
                if m:
                    want[m.group(1).upper()] = "docs/PUZZLE_CHAIN.md chain"
                m = re.match(r"\| (?:NEW )?(P\d+)\b", line)
                if m:
                    want[m.group(1).upper()] = "docs/PUZZLE_CHAIN.md side puzzles"
        return want

    def check_notes(self, notes, trig, scenes, diag, shell, sysop, ghosts):
        """NOTES.INI: every note points at something on the card, fits, and every clue of the chain and the
        story bible has a note (a clue the notebook cannot keep is a clue a caller can lose)."""
        if not notes:
            return
        covered = set()
        rules = {name: kv for name, kv, _ in trig.sections} if trig else {}
        lines = {}
        for ini in (sysop, ghosts):
            if ini:
                for name, kv, _ in ini.sections:
                    lines[name] = kv
        ghost_ids = {name for name, kv, _ in ghosts.sections if kv.get("handle")} if ghosts else set()
        diag_codes = {name for name, _, _ in diag.sections} if diag else set()
        shell_paths = {name.upper() for name, _, _ in shell.sections if name.startswith("/")} if shell else set()
        for name, kv, _ in notes.sections:
            where = "NOTES.INI [%s]" % name
            if not re.fullmatch(r"[A-Z0-9_]{1,14}", name):
                self.err(where, "the id must be letters, digits and _, 14 characters at most (the flag is N_<ID>)")
            on = kv.get("on", "").lower()
            match = kv.get("match", "").upper()
            if on not in self.NOTE_KINDS:
                self.err(where, "on=%s is not a kind of note" % on)
                continue
            if not kv.get("text"):
                self.err(where, "no text")
            if len(kv.get("text", "")) > 110:
                self.err(where, "text longer than 110")
            if not kv.get("src"):
                self.err(where, "no src")
            if len(kv.get("src", "")) > 30:
                self.err(where, "src longer than 30")
            if len(kv.get("where", "")) > 40:
                self.err(where, "where longer than 40")
            if on != "login" and not match:
                self.err(where, "no match=")
            if on == "read" and match.startswith("~"):
                subj = match[1:]
                if subj not in self.subjects:
                    self.err(where, "no message with the subject '%s'" % subj)
                elif kv.get("area", "") and kv["area"] not in self.subject_conf.get(subj, set()):
                    self.err(where, "no message '%s' in conference %s" % (subj, kv["area"]))
            elif on == "read" and not match.isdigit():
                self.err(where, "read needs ~SUBJECT or a message number")
            elif on in ("file", "art") and match not in self.files:
                self.err(where, "%s is not on the card" % match)
            elif on == "screen" and match not in ("USERS", "WHO") and not (match.startswith("BULLETIN ") and match[9:] in self.bulletins):
                self.err(where, "screen '%s' is not USERS, WHO or a bulletin title" % match)
            elif on == "scene":
                sid, _, node = match.partition("/")
                sc = scenes.get(sid)
                if not sc:
                    self.err(where, "scene %s has no file" % sid)
                elif node and node not in {n for n, _, _ in sc.sections}:
                    self.err(where, "scene %s has no node %s" % (sid, node))
            elif on == "page" and not (rules.get(match.lower()) or rules.get(match)):
                self.err(where, "no trigger rule %s" % match)
            elif on == "eliza" and match not in lines:
                self.err(where, "no line %s in SYSOP.INI" % match)
            elif on == "ghost" and match.replace("WHO_", "", 1) not in ghost_ids and match not in ghost_ids:
                self.err(where, "no ghost %s in GHOSTS.INI" % match)
            elif on == "diag" and match != "HELP" and match not in diag_codes:
                self.err(where, "no query %s in DIAG.INI" % match)
            elif on == "shell" and match not in shell_paths:
                self.err(where, "no path %s in SHELL.INI" % match)
            rp = kv.get("replay", "")
            if rp:
                kind, _, target = rp.partition(":")
                target = target.strip().upper()
                kind = kind.strip().lower()
                ok = ((kind == "scene" and target in scenes) or (kind == "page" and target in rules) or (kind == "eliza" and target in lines)
                      or (kind == "ghost" and target in ghost_ids) or (kind == "art" and (target + ".ANS") in self.files))
                if not ok:
                    self.err(where, "replay=%s points at nothing on the card" % rp)
            for c in re.split(r"[,\s]+", kv.get("clue", "")):
                if c:
                    covered.add(c.upper())
        want = self.chain_clues()
        if want is None:
            self.warn("NOTES.INI", "docs/PUZZLE_CHAIN.md or the story bible is not next to the card: clue coverage not checked")
            return
        missing = sorted(c for c in want if c not in covered)
        for c in missing:
            self.err("NOTES.INI", "clue %s (%s) has no note: add clue=%s to the note that keeps it" % (c, want[c], c))
        self.note_count = len(notes.sections)

    def check_rive(self):
        """G2: ELIZA's brain (STORY/ELIZA/*.rive): every file parses, topics exist, flags it reads are real,
        nothing needs code, and the brain stays small (it is loaded into the ESP32's memory)."""
        need = []
        d = os.path.join(self.story, "ELIZA")
        if not os.path.isdir(d):
            return need
        triggers = replies = 0
        topics = {"random"}
        sets = []
        texts = {}
        for n in sorted(os.listdir(d)):
            if not n.lower().endswith(".rive"):
                continue
            rel = "STORY/ELIZA/" + n
            raw = open(os.path.join(d, n), "rb").read()
            if len(raw) > 24 * 1024:
                self.err(rel, "over 24 KB")
            try:
                text = raw.decode("ascii")
            except UnicodeDecodeError:
                self.err(rel, "plain ASCII only")
                continue
            texts[rel] = text
            has_reply = None
            in_block = False
            for i, line in enumerate(text.splitlines(), 1):
                t = line.strip()
                if in_block:
                    in_block = "*/" not in t
                    continue
                if not t or t.startswith("//"):
                    continue
                if t.startswith("/*"):
                    in_block = "*/" not in t
                    continue
                where = "%s:%d" % (rel, i)
                c = t[0]
                if c not in "!+-%@*^><":
                    self.err(where, "a RiveScript line starts with one of ! + - %% @ * ^ > <, not %r" % c)
                    continue
                if c == "+":
                    if has_reply is False:
                        self.err(where, "the trigger before has no reply")
                    triggers += 1
                    has_reply = False
                elif c in "-@":
                    replies += 1 if c == "-" else 0
                    has_reply = True
                elif c == "*":
                    has_reply = True
                    if "=>" not in t:
                        self.err(where, "a condition needs =>")
                elif c == ">":
                    parts = t[1:].split()
                    if parts[:1] == ["topic"] and len(parts) > 1:
                        topics.add(parts[1].lower())
                    else:
                        self.err(where, "only > topic blocks are read (no begin, no object)")
                if re.search(r"<call|<env|<bot\s+\w+=", t):
                    self.err(where, "no code, no macros on the card")
                for m in re.finditer(r"<get flag_([A-Za-z0-9_]+)>", t):
                    need.append((where, m.group(1).upper()))
                for m in re.finditer(r"<set flag_", t):
                    self.err(where, "story flags are read-only here")
                for m in re.finditer(r"\{topic=([^}]+)\}", t):
                    sets.append((where, m.group(1).strip().lower()))
            if has_reply is False:
                self.err(rel, "the last trigger has no reply")
        for where, tp in sets:
            if tp not in topics:
                self.err(where, "topic %s is never defined" % tp)
        if triggers > 300 or replies > 1500:
            self.err("STORY/ELIZA", "brain too big for the board (%d triggers, %d replies)" % (triggers, replies))
        self.rive_stats = (triggers, replies)
        # The flags the brain reads must exist (the lint's own list of story flags is the check in `required`).
        return [(w, f) for w, f in need]

    def check_demo(self, scenes):
        """GE: STORY/DEMO.INI. The scene exists, every browser part has a known kind and sits on the tune, the
        files it names are on the card, every ANSI step has its art and ends in a way the board can run."""
        ini = self.load("DEMO.INI", required=False)
        if ini is None:
            return
        by = {name: kv for name, kv, _ in ini.sections}
        demo = by.get("DEMO", {})
        if demo.get("scene", "ENDING").upper() not in scenes:
            self.err("DEMO.INI", "scene=%s has no file in STORY/SCENES" % demo.get("scene"))
        folder = os.path.join(self.card, "DEMO")
        for need in ("DEMO.JS", demo.get("tune", "TUNE.MOD"), "SHIP.PNG"):
            if not os.path.isfile(os.path.join(folder, need)):
                self.err("DEMO.INI", "BBS/DEMO/%s is not on the card" % need)
        kinds = {"release", "title", "letter", "news", "radio", "inworldcredits", "credits"}
        last_end = None
        parts = [(n, kv) for n, kv, _ in ini.sections if re.match(r"P\d", n)]
        for name, kv in parts:
            where = "DEMO.INI [%s]" % name
            if kv.get("kind") not in kinds:
                self.err(where, "unknown kind %r" % kv.get("kind"))
            try:
                s, e = float(kv.get("start", "x")), float(kv.get("end", "x"))
            except ValueError:
                self.err(where, "start= and end= are seconds")
                continue
            if e <= s:
                self.err(where, "ends before it starts")
            if last_end is not None and abs(s - last_end) > 0.01:
                self.err(where, "starts at %s but the part before ends at %s (a gap or an overlap)" % (s, last_end))
            last_end = e
            if kv.get("face") and not os.path.isfile(os.path.join(folder, kv["face"])):
                self.err(where, "face %s is not in BBS/DEMO" % kv["face"])
            if kv.get("kind") == "news" and not (kv.get("head") and kv.get("head_end2")):
                self.err(where, "a news part needs head= and head_END2=")
        if not parts:
            self.err("DEMO.INI", "no [P01]... parts")
        elif last_end is not None and not (120 <= last_end <= 180):
            self.err("DEMO.INI", "the demo runs %.0f s; the plan says 2 to 3 minutes" % last_end)
        steps = [(n, kv) for n, kv, _ in ini.sections if re.match(r"T\d", n)]
        for name, kv in steps:
            where = "DEMO.INI [%s]" % name
            art = kv.get("art")
            if art and not os.path.isfile(os.path.join(self.card, "ANSI", "DEMO", art + ".ANS")):
                self.err(where, "art %s is not in ANSI/DEMO" % art)
            if not (art or kv.get("text") or kv.get("bar")):
                self.err(where, "a step with no art, text or bar")
            if kv.get("when", "").upper() not in ("", "END1", "END2"):
                self.err(where, "when= is END1 or END2")
        if len(steps) > 40:
            self.err("DEMO.INI", "%d steps, the board loads 40" % len(steps))
        after = by.get("AFTER", {})
        if not after.get("text"):
            self.err("DEMO.INI [AFTER]", "no text: the reconnect screen says nothing")
        if after.get("art") and not os.path.isfile(os.path.join(self.card, "ANSI", "DEMO", after["art"] + ".ANS")):
            self.err("DEMO.INI [AFTER]", "art %s is not in ANSI/DEMO" % after["art"])
        # the keys it names must exist: E (Your account) and R (reset my game)
        if "E" not in after.get("text", "") or "reset" not in after.get("text", ""):
            self.err("DEMO.INI [AFTER]", "the reconnect text must name E (Your account) and the reset")

    def check_voice(self):
        paths = []
        for top in ("STORY", "MSGBASE", "BULLETINS"):
            for root, _, names in os.walk(os.path.join(self.card, top)):
                for n in names:
                    if n.upper().endswith((".INI", ".TXT", ".RIVE")):
                        paths.append(os.path.join(root, n))
        for p in sorted(paths):
            rel = os.path.relpath(p, self.card).replace("\\", "/")
            try:
                text = open(p, encoding="utf-8").read()
            except UnicodeDecodeError:
                self.err(rel, "not valid UTF-8")
                continue
            except OSError:
                continue
            for ch, what in self.ODD.items():
                if ch in text:
                    self.warn(rel, "contains %s (plain keyboard characters only)" % what)
            low = text.lower()
            for w in self.BANNED:
                if re.search(r"\b" + re.escape(w) + r"\b", low):
                    self.warn(rel, "banned word '%s' (writing rules)" % w)


def main():
    here = os.path.dirname(os.path.abspath(__file__))
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--card", default=os.path.join(here, "..", "sd", "BBS"))
    ap.add_argument("--strict", action="store_true", help="warnings fail the run")
    ap.add_argument("--quiet", action="store_true", help="print only problems")
    a = ap.parse_args()
    lint = Lint(os.path.normpath(a.card), a.strict)
    lint.run()
    for w in lint.warnings:
        print("warn   " + w)
    for e in lint.errors:
        print("ERROR  " + e)
    if not a.quiet and hasattr(lint, "stats"):
        r, s, f, b = lint.stats
        print("story-lint: %d rules, %d scenes, %d story flags (%d of 4000 characters)" % (r, s, f, b))
    print("story-lint: %d error(s), %d warning(s)" % (len(lint.errors), len(lint.warnings)))
    return 1 if lint.errors else 0


if __name__ == "__main__":
    sys.exit(main())
