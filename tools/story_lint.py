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
        base = os.path.join(self.card, "MSGBASE")
        for root, _, names in os.walk(base):
            for n in names:
                if not n.upper().endswith(".TXT"):
                    continue
                try:
                    with open(os.path.join(root, n), encoding="utf-8", errors="replace") as f:
                        for line in f:
                            if line.lower().startswith("subject:"):
                                self.subjects.add(line.split(":", 1)[1].strip().upper()[:39])
                except OSError:
                    pass
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
            visible = any(kv.get(k) for k in ("page", "mail_body", "post_body", "art", "scene"))
            if visible:
                gr.append("T_" + name)
            sc = kv.get("scene", "").upper()
            lab = "rule " + name
            units.append((lab, needs, gr))
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
                units.append(("sysop " + name, flags(kv.get("has")), flags(kv.get("set"))))
                for f in flags(kv.get("has")):
                    required.append(("sysop " + name, f))
        if ghosts:
            for name, kv, _ in ghosts.sections:
                for f in flags(kv.get("has")):
                    required.append(("ghost " + name, f))
                units.append(("ghost " + name, flags(kv.get("has")), flags(kv.get("set"))))

        # accounts
        crew_logins = set()
        if acc:
            for name, kv, _ in acc.sections:
                if kv.get("password"):
                    crew_logins.add(name)
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
        engine = lambda f: f.startswith("T_") or f.startswith("SC_") or f.startswith("E_") or f.startswith("G_")
        for where, f in required:
            if f not in granted and not engine(f):
                self.err(where, "asks for flag %s, which nothing grants" % f)

        self.required_flags = {f for _, f in required}
        # ---- reachability
        opt = self.fixpoint(units, {}, set())
        pess = self.fixpoint(units, lacking, opt)
        self.check_reach(man, opt, pess, units)

        # ---- redundancy of evidence
        self.check_redundancy(man, units)

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

        # ---- power
        if power:
            pn = power.named().get("POWER", {})
            if pn.get("timer_sec", "x").isdigit() and int(pn["timer_sec"]) < 30:
                self.warn("POWER.INI", "timer_sec=%s: a first-time player cannot finish the prologue in that time" % pn["timer_sec"])

    # -------- parts
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
            elif m.upper() != "USERS":
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
                if k == "go" or k == "if_go" or re.fullmatch(r"go\d", k):
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
        # grants: a choice's flag is granted when the scene is played (any choice is possible)
        for n, kv in nodes.items():
            if n not in reach:
                continue
            g = flags(kv.get("set"))
            for k, v in kv.items():
                if re.fullmatch(r"set\d", k):
                    g += flags(v)
            if g:
                units.append(("scene %s [%s]" % (sid, n), gate + ["SC_ENTER_" + sid], g))
            for k, v in kv.items():
                if re.fullmatch(r"has\d", k):
                    for f in flags(v):
                        required.append((where + " [%s]" % n, f))
            for k in ("art",):
                if kv.get(k) and not self.art_exists(kv[k]) and not self.art_exists(kv[k].replace("STORY/", "")):
                    self.err(where, "[%s] art '%s' is not on the card" % (n, kv[k]))
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

    BANNED = ["delve", "tapestry", "testament", "realm", "beacon", "embark", "symphony", "paradigm shift",
              "game-changer", "unleash", "multifaceted", "ever-evolving", "nestled", "bustling", "indelible",
              "plethora", "myriad", "quintessential", "elucidate", "underscore", "a reminder that", "stands as"]
    ODD = {"\u2014": "em dash", "\u2013": "en dash", "\u201c": "curly quote", "\u201d": "curly quote",
           "\u2018": "curly quote", "\u2019": "curly quote", "\u2026": "ellipsis character"}

    def check_voice(self):
        paths = []
        for top in ("STORY", "MSGBASE", "BULLETINS"):
            for root, _, names in os.walk(os.path.join(self.card, top)):
                for n in names:
                    if n.upper().endswith((".INI", ".TXT")):
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
