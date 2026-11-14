#pragma once
// The NOWHERE shell as pure logic: the parser for STORY/SHELL.INI (from plain
// key/value triples), path handling and the commands (ls, cat, cd, pwd, help,
// whoami, uname, exit and the one final command). No String, no SD card, no
// sessions: the board wraps this in doors/shell.h. A plain C++ compiler tests
// it (tools/test/shell_logic_test.cpp).
// Part of the shared core: included by doors/shell.h.
#include "haunt_logic.h"
#include <algorithm>

namespace nshell {

using haunt::Kv;

static constexpr size_t MAX_NODES = 120, MAX_TEXT = 1200, MAX_PATH = 64, MAX_DEPTH = 8, MAX_OUT = 4000;

struct Node {
  std::string path;     // absolute, lower case, no trailing "/"
  bool dir = false;
  std::string text;     // files: "|" breaks a line
  std::string has;      // flags the caller needs to see it (empty = everyone)
  std::string altHas, altText;   // alt_has= alt_text=: what the file reads once the caller holds those flags
  std::string date;     // ls -l
  std::string owner;
};

struct Table {
  // [SHELL]
  std::string host = "nowhere", user = "guest", prompt = "{user}@{host}:{cwd}$";
  std::string needs;        // flags that put the door in the menu (empty = everyone)
  std::string motd;
  std::string unknown = "{cmd}: command not found";
  std::string help = "ls [-la] [dir]  cat file  cd [dir]  pwd  whoami  uname  exit";   // "|" breaks a line
  std::string helpFinal;    // added to help for a caller who may use the final command
  std::string date = "Oct 31  1998", owner = "root";
  std::string finalCmd, finalHas, finalSet, finalText, finalScene, finalUsage;
  std::string finalHeld;    // the final verb typed by a caller without final_has: why it does not run yet ({cmd})   // finalScene: a scene (STORY/SCENES) the door starts after the final text
  std::vector<Node> nodes;
  unsigned skipped = 0;
  std::vector<std::string> problems;
  void problem(const std::string &id, const std::string &why) {
    skipped++;
    if (problems.size() < 20) problems.push_back((id.empty() ? std::string("(none)") : id) + ": " + why);
  }
};

// Split on spaces and tabs. No quoting: the paths on the haven have no spaces.
inline std::vector<std::string> words(const std::string &line) {
  std::vector<std::string> out;
  std::string cur;
  for (char c : line) {
    if (c == ' ' || c == '\t') { if (!cur.empty()) { out.push_back(cur); cur.clear(); } }
    else cur += c;
  }
  if (!cur.empty()) out.push_back(cur);
  return out;
}

// Lower-case, single spaces: how the final command is compared.
inline std::string squash(const std::string &line) {
  std::string out;
  for (const auto &w : words(line)) { if (!out.empty()) out += ' '; out += haunt::lower(w); }
  return out;
}

// "/a/b" from a cwd and what was typed: "." ".." "~" "" and absolute forms work.
// False if the result is too long or too deep, or a part holds a character a
// file name does not.
inline bool resolve(const std::string &cwd, const std::string &typed, std::string &out) {
  std::string t = typed;
  std::vector<std::string> parts;
  std::string full;
  if (t.empty() || t == "~") full = t.empty() ? cwd : "/";
  else if (t[0] == '/') full = t;
  else if (t.compare(0, 2, "~/") == 0) full = "/" + t.substr(2);
  else full = cwd + "/" + t;
  std::string cur;
  for (size_t i = 0; i <= full.size(); ++i) {
    if (i == full.size() || full[i] == '/') {
      if (cur == "..") { if (!parts.empty()) parts.pop_back(); }
      else if (!cur.empty() && cur != ".") parts.push_back(cur);
      cur.clear();
    } else cur += full[i];
  }
  if (parts.size() > MAX_DEPTH) return false;
  out.clear();
  for (auto &p : parts) {
    for (char c : p) {
      bool ok = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' || c == '.' || c == '-';
      if (!ok) return false;
    }
    out += "/" + haunt::lower(p);
  }
  if (out.empty()) out = "/";
  return out.size() <= MAX_PATH;
}

inline std::string joinPath(const std::string &dir, const std::string &name) { return dir == "/" ? "/" + name : dir + "/" + name; }

inline bool visible(const Node &n, const std::string &flags) { return n.has.empty() || haunt::flagsHasAll(flags, n.has); }
inline const std::string &textFor(const Node &n, const std::string &flags) {
  return !n.altHas.empty() && haunt::flagsHasAll(flags, n.altHas) ? n.altText : n.text;
}

inline const Node *findNode(const Table &t, const std::string &path) {
  for (const auto &n : t.nodes) if (n.path == path) return &n;
  return nullptr;
}

// A directory exists if a section says so or something visible lives below it.
inline bool isDir(const Table &t, const std::string &path, const std::string &flags) {
  if (path == "/") return true;
  if (const Node *n = findNode(t, path)) return n->dir && visible(*n, flags);
  std::string pre = path + "/";
  for (const auto &n : t.nodes) if (n.path.compare(0, pre.size(), pre) == 0 && visible(n, flags)) return true;
  return false;
}
inline const Node *fileAt(const Table &t, const std::string &path, const std::string &flags) {
  const Node *n = findNode(t, path);
  return n && !n->dir && visible(*n, flags) ? n : nullptr;
}

struct Entry { std::string name; bool dir; size_t size; std::string date, owner; };

inline std::string expand(std::string text) {
  for (auto &c : text) if (c == '|') c = '\n';
  return text;
}

// The visible entries of one directory, sorted by name. Subdirectories that
// exist only because a file lives below them are listed once.
inline std::vector<Entry> list(const Table &t, const std::string &dir, const std::string &flags, bool all) {
  std::vector<Entry> out;
  std::string pre = dir == "/" ? "/" : dir + "/";
  for (const auto &n : t.nodes) {
    if (n.path.size() <= pre.size() || n.path.compare(0, pre.size(), pre) != 0 || !visible(n, flags)) continue;
    std::string rest = n.path.substr(pre.size());
    size_t slash = rest.find('/');
    std::string name = slash == std::string::npos ? rest : rest.substr(0, slash);
    bool isD = slash != std::string::npos || n.dir;
    if (name.empty() || (name[0] == '.' && !all)) continue;
    bool seen = false;
    for (auto &e : out) if (e.name == name) { seen = true; if (isD) e.dir = true; break; }
    if (seen) continue;
    const Node *self = findNode(t, joinPath(dir, name));
    out.push_back({name, isD, isD ? 512 : expand(textFor(n, flags)).size(),
                   self && !self->date.empty() ? self->date : t.date, self && !self->owner.empty() ? self->owner : t.owner});
  }
  std::sort(out.begin(), out.end(), [](const Entry &a, const Entry &b) { return a.name < b.name; });
  return out;
}

inline std::string subst(std::string tmpl, const std::string &key, const std::string &value) {
  for (size_t at = tmpl.find(key); at != std::string::npos; at = tmpl.find(key, at + value.size())) tmpl.replace(at, key.size(), value);
  return tmpl;
}

// The prompt text (the board adds the space after it): {user} {host} {cwd}.
inline std::string promptText(const Table &t, const std::string &cwd) {
  return subst(subst(subst(t.prompt, "{user}", t.user), "{host}", t.host), "{cwd}", cwd);
}

// ---- parsing ---------------------------------------------------------------

inline std::string get(const std::vector<Kv> &kv, const std::string &sec, const std::string &key, const std::string &fb = "") {
  for (const auto &e : kv) if (e.section == sec && e.key == key) return e.value;
  return fb;
}

// Section names are upper case after iniLoad; paths are case-insensitive here.
inline void parse(const std::vector<Kv> &kv, Table &t) {
  t = Table();
  auto set = [&](const char *key, std::string &dst) { std::string v = get(kv, "SHELL", key); if (!v.empty()) dst = haunt::capText(v, 160); };
  set("host", t.host); set("user", t.user); set("prompt", t.prompt); set("unknown", t.unknown);
  set("date", t.date); set("owner", t.owner); set("help", t.help); set("help_final", t.helpFinal);
  t.needs = haunt::flagsClean(get(kv, "SHELL", "needs"));
  t.motd = haunt::capText(get(kv, "SHELL", "motd"), MAX_TEXT);
  t.finalCmd = squash(get(kv, "SHELL", "final_cmd"));
  t.finalHas = haunt::flagsClean(get(kv, "SHELL", "final_has"));
  t.finalSet = haunt::flagsClean(get(kv, "SHELL", "final_set"));
  t.finalText = haunt::capText(get(kv, "SHELL", "final_text"), MAX_TEXT);
  t.finalScene = haunt::upper(haunt::trim(get(kv, "SHELL", "final_scene")));
  t.finalUsage = haunt::capText(get(kv, "SHELL", "final_usage"), MAX_TEXT);
  t.finalHeld = haunt::capText(get(kv, "SHELL", "final_held"), MAX_TEXT);
  std::vector<std::string> seen;
  for (const auto &e : kv) {
    if (e.section.empty() || e.section[0] != '/') continue;
    if (std::find(seen.begin(), seen.end(), e.section) != seen.end()) continue;
    seen.push_back(e.section);
    std::string path;
    if (e.section == "/" || !resolve("/", e.section, path)) { t.problem(e.section, "not a usable path"); continue; }
    if (t.nodes.size() >= MAX_NODES) { t.problem(e.section, "over 120 entries"); continue; }
    if (findNode(t, path)) { t.problem(e.section, "same path twice"); continue; }
    Node n;
    n.path = path;
    n.dir = get(kv, e.section, "dir") == "1";
    n.text = haunt::capText(get(kv, e.section, "text"), MAX_TEXT);
    n.has = haunt::flagsClean(get(kv, e.section, "has"));
    n.altHas = haunt::flagsClean(get(kv, e.section, "alt_has"));
    n.altText = haunt::capText(get(kv, e.section, "alt_text"), MAX_TEXT);
    if (n.altText.empty()) n.altHas.clear();
    n.date = haunt::capText(get(kv, e.section, "date"), 20);
    n.owner = haunt::capText(get(kv, e.section, "owner"), 12);
    if (!n.dir && n.text.empty()) { t.problem(e.section, "a file needs text= (or dir=1)"); continue; }
    t.nodes.push_back(n);
  }
}

// ---- commands ----------------------------------------------------------------

struct Result {
  std::string out;      // lines separated by "\n"; the board turns them into CRLF
  bool exit = false;
  std::string grant;    // flags to add to the caller (the final command)
  bool final = false;
  std::string scene;    // the final command: the scene to start after the text
};

inline std::string pad(const std::string &s, size_t w, bool right = false) {
  if (s.size() >= w) return s;
  return right ? std::string(w - s.size(), ' ') + s : s + std::string(w - s.size(), ' ');
}

inline Result exec(const Table &t, std::string &cwd, const std::string &flags, const std::string &line) {
  Result r;
  auto w = words(line);
  if (w.empty()) return r;
  std::string cmd = haunt::lower(w[0]);
  if (!t.finalCmd.empty() && squash(line) == t.finalCmd && (t.finalHas.empty() || haunt::flagsHasAll(flags, t.finalHas))) {
    r.out = t.finalText; r.grant = t.finalSet; r.final = true; r.scene = t.finalScene;
    return r;
  }
  // The final command's verb with other arguments: a usage line, only for a caller who may use it.
  if (!t.finalUsage.empty() && !t.finalCmd.empty() && cmd == t.finalCmd.substr(0, t.finalCmd.find(' ')) && (t.finalHas.empty() || haunt::flagsHasAll(flags, t.finalHas))) {
    r.out = t.finalUsage;
    return r;
  }
  // The final verb before the caller may use it: why it is held, when the card says so.
  if (!t.finalHeld.empty() && !t.finalCmd.empty() && cmd == t.finalCmd.substr(0, t.finalCmd.find(' '))) {
    r.out = subst(t.finalHeld, "{cmd}", w[0]);
    return r;
  }
  if (cmd == "exit" || cmd == "logout" || cmd == "quit" || cmd == "bye" || cmd == "q") { r.exit = true; return r; }
  if (cmd == "pwd") { r.out = cwd; return r; }
  if (cmd == "whoami") { r.out = t.user; return r; }
  if (cmd == "hostname") { r.out = t.host; return r; }
  if (cmd == "uname") { r.out = "NOWHERE 4.1.3 #1 " + t.date; return r; }
  if (cmd == "help" || cmd == "?") {
    r.out = expand(t.help);
    // The final command is listed only for a caller it works for (it is "not found" for the rest).
    if (!t.helpFinal.empty() && !t.finalCmd.empty() && (t.finalHas.empty() || haunt::flagsHasAll(flags, t.finalHas))) r.out += "\n" + expand(t.helpFinal);
    return r;
  }
  if (cmd == "cd") {
    std::string to;
    if (w.size() > 2) { r.out = "cd: too many arguments"; return r; }
    if (!resolve(cwd, w.size() > 1 ? w[1] : "~", to) || !isDir(t, to, flags)) {
      r.out = "cd: " + (w.size() > 1 ? w[1] : std::string("~")) + ": No such file or directory";
      return r;
    }
    cwd = to;
    return r;
  }
  if (cmd == "cat") {
    if (w.size() < 2) { r.out = "usage: cat file"; return r; }
    for (size_t i = 1; i < w.size(); ++i) {
      std::string p;
      std::string one;
      if (!resolve(cwd, w[i], p)) one = "cat: " + w[i] + ": No such file or directory";
      else if (const Node *n = fileAt(t, p, flags)) one = expand(textFor(*n, flags));
      else if (isDir(t, p, flags)) one = "cat: " + w[i] + ": Is a directory";
      else one = "cat: " + w[i] + ": No such file or directory";
      if (!r.out.empty()) r.out += "\n";
      r.out += one;
    }
    if (r.out.size() > MAX_OUT) r.out.resize(MAX_OUT);
    return r;
  }
  if (cmd == "ls") {
    bool longFmt = false, all = false;
    std::string target;
    for (size_t i = 1; i < w.size(); ++i) {
      if (w[i][0] == '-' && w[i].size() > 1) {
        for (size_t k = 1; k < w[i].size(); ++k) {
          if (w[i][k] == 'l') longFmt = true;
          else if (w[i][k] == 'a') all = true;
          else { r.out = "ls: illegal option -- " + std::string(1, w[i][k]); return r; }
        }
      } else if (target.empty()) target = w[i];
      else { r.out = "ls: too many arguments"; return r; }
    }
    std::string p;
    if (!resolve(cwd, target, p)) { r.out = "ls: " + target + ": No such file or directory"; return r; }
    std::vector<Entry> es;
    if (const Node *f = fileAt(t, p, flags)) {
      std::string name = p.substr(p.rfind('/') + 1);
      es.push_back({name, false, expand(textFor(*f, flags)).size(), f->date.empty() ? t.date : f->date, f->owner.empty() ? t.owner : f->owner});
    } else if (isDir(t, p, flags)) es = list(t, p, flags, all);
    else { r.out = "ls: " + target + ": No such file or directory"; return r; }
    for (auto &e : es) {
      if (longFmt) {
        r.out += std::string(e.dir ? "drwxr-xr-x" : "-rw-r--r--") + " 1 " + pad(e.owner, 8) + " " + pad(std::to_string(e.size), 6, true) + " " + e.date + " " + e.name + (e.dir ? "/" : "") + "\n";
      } else r.out += e.name + (e.dir ? "/" : "") + "\n";
    }
    if (!r.out.empty()) r.out.pop_back();
    if (r.out.size() > MAX_OUT) r.out.resize(MAX_OUT);
    return r;
  }
  r.out = subst(t.unknown, "{cmd}", w[0]);
  return r;
}

}  // namespace nshell
