#pragma once
// The trigger engine's decisions, as pure logic: the rule table, the parser
// for STORY/TRIGGERS.INI (from plain key/value triples) and "which rules are
// due now". No String, no SD card, no sessions: the board wraps this in
// core/haunt.h. A plain C++ compiler tests it (tools/test/haunt_logic_test.cpp).
// Part of the shared core: included in order from src/bbs_core.h.
#include <stdint.h>
#include <string>
#include <vector>

namespace haunt {

enum Ev : uint8_t { EV_NONE, EV_LOGIN, EV_CREW_LOGIN, EV_READ, EV_FILE, EV_ART, EV_DOOR, EV_IDLE, EV_MINUTES, EV_SCREEN };

static constexpr size_t MAX_RULES = 300;   // memory on the stick
static constexpr size_t MAX_FLAGS = 255;         // characters in the account's flag field (access flags)
static constexpr size_t MAX_STORY_FLAGS = 4000;  // characters in a caller's story record (core/story.h)
static constexpr size_t MAX_ID = 24;       // characters in a rule id
static constexpr size_t MAX_LINE = 200, MAX_BODY = 1200, MAX_NAME = 16, MAX_SUBJECT = 60, MAX_MATCH = 40;

struct Kv { std::string section, key, value; };  // one INI line

struct Rule {
  std::string id;               // upper case, letters digits _
  Ev ev = EV_NONE;
  std::string match;            // upper case; empty = any
  uint32_t baudMax = 0;         // art: 0 = no limit asked for
  uint32_t after = 0;           // idle: seconds; minutes: minutes
  std::string has, lacks;       // flag lists
  int levelMin = 0, levelMax = 255;
  std::string set;              // flags to add
  std::string pageFrom, page;
  std::string mailFrom, mailConf, mailSubject, mailBody;
  std::string postFrom, postConf, postSubject, postBody;
  std::string art;              // card art name
  std::string scene;            // a scene to play (STORY/SCENES/NAME.INI)
  std::string add;              // read: a line added while the message is shown
  // A rule that shows something to the caller fires once per caller.
  bool visible() const { return !page.empty() || !mailBody.empty() || !postBody.empty() || !art.empty() || !scene.empty(); }
};

struct Table {
  std::vector<Rule> rules;
  unsigned skipped = 0;                 // malformed rules
  std::vector<std::string> problems;    // one short line each, for the event log
};

// What happened. `flags` are the caller's flags before the event.
struct Ctx {
  Ev ev = EV_NONE;
  std::string match;     // message number, file name, door key or crew handle
  uint32_t baud = 0;     // art: the caller's modem speed in bits per second, 0 = no limit
  uint32_t amount = 0;   // idle: seconds since the last key; minutes: minutes since connect
  std::string flags;
  int level = 0;
};

// ---------- flags --------------------------------------------------------------------------
inline char upperChar(char c) { return c >= 'a' && c <= 'z' ? (char)(c - 'a' + 'A') : c; }
inline bool flagChar(char c) { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_'; }
inline std::string upper(std::string s) { for (auto &c : s) c = upperChar(c); return s; }

// Words of a comma list, upper case, only letters digits and _, in order, no repeats.
inline std::vector<std::string> flagWords(const std::string &list) {
  std::vector<std::string> out;
  std::string word;
  auto take = [&]() {
    if (!word.empty()) {
      bool seen = false;
      for (const auto &w : out) if (w == word) { seen = true; break; }
      if (!seen) out.push_back(word);
    }
    word.clear();
  };
  for (char c : list) { if (flagChar(c)) word += upperChar(c); else if (c == ',') take(); }
  take();
  return out;
}
inline std::string flagsClean(const std::string &list) {
  std::string out;
  for (const auto &w : flagWords(list)) { if (!out.empty()) out += ','; out += w; }
  return out;
}
inline bool flagsHas(const std::string &flags, const std::string &flag) {
  std::string want = upper(flag);
  if (want.empty()) return false;
  for (const auto &w : flagWords(flags)) if (w == want) return true;
  return false;
}
// Flags only grow: the result always starts with `flags`, byte for byte. Words
// already there change nothing. A word that does not fit in `cap` is not
// added and is named in `dropped` (comma list) so the caller can log it.
inline std::string flagsAdd(const std::string &flags, const std::string &add, std::string *dropped = nullptr, size_t cap = MAX_STORY_FLAGS) {
  std::string out = flags;
  for (const auto &w : flagWords(add)) {
    if (flagsHas(out, w)) continue;
    size_t need = w.size() + (out.empty() ? 0 : 1);
    if (out.size() + need > cap) { if (dropped) { if (!dropped->empty()) *dropped += ','; *dropped += w; } continue; }
    if (!out.empty()) out += ',';
    out += w;
  }
  return out;
}
inline bool flagsHasAll(const std::string &flags, const std::string &list) {
  for (const auto &w : flagWords(list)) if (!flagsHas(flags, w)) return false;
  return true;
}
inline bool flagsHasAny(const std::string &flags, const std::string &list) {
  for (const auto &w : flagWords(list)) if (flagsHas(flags, w)) return true;
  return false;
}
// Flags that open something by themselves stay in the account's flag field;
// every other flag is story progress and lives in the story record.
inline bool accessFlag(const std::string &word) {
  const std::string w = upper(word);
  return w == "SYSOP" || w == "READONLY" || w == "BACKROOM" || w == "VAULT";
}
struct Split { std::string access, story; };
inline Split splitFlags(const std::string &list) {
  Split s;
  for (const auto &w : flagWords(list)) {
    std::string &to = accessFlag(w) ? s.access : s.story;
    if (!to.empty()) to += ',';
    to += w;
  }
  return s;
}
// A gate (a `Needs:` line, NEEDS.BBS, a required_flag): every word must be held.
// SYSOP view (the story flag SYSVW, milestone M6) passes every gate except the
// access flags, so it never opens a real SysOp area (SYSOP, READONLY, BACKROOM, VAULT).
inline bool gatePass(const std::string &have, const std::string &needs) {
  const bool view = flagsHas(have, "SYSVW");
  for (const auto &w : flagWords(needs)) {
    if (flagsHas(have, w)) continue;
    if (view && !accessFlag(w)) continue;
    return false;
  }
  return true;
}
inline std::string markerFor(const std::string &ruleId) { return "T_" + upper(ruleId); }

// ---------- parsing ------------------------------------------------------------------------
inline std::string trim(const std::string &s) {
  size_t a = 0, b = s.size();
  while (a < b && (unsigned char)s[a] <= ' ') ++a;
  while (b > a && (unsigned char)s[b - 1] <= ' ') --b;
  return s.substr(a, b - a);
}
inline std::string lower(std::string s) { for (auto &c : s) if (c >= 'A' && c <= 'Z') c = (char)(c - 'A' + 'a'); return s; }
// Text for a caller: no control characters (the board adds its own line
// breaks, "|" in the file), cut to `max` characters.
inline std::string capText(const std::string &s, size_t max) {
  std::string out;
  for (char c : trim(s)) { if (out.size() >= max) break; if ((unsigned char)c >= 32 && c != 127) out += c; }
  return out;
}
inline bool parseUint(const std::string &s, uint32_t max, uint32_t &out) {
  std::string t = trim(s);
  if (t.empty() || t.size() > 9) return false;
  uint32_t v = 0;
  for (char c : t) { if (c < '0' || c > '9') return false; v = v * 10 + (uint32_t)(c - '0'); }
  if (v > max) return false;
  out = v;
  return true;
}
inline Ev parseEvent(const std::string &s) {
  std::string e = lower(trim(s));
  if (e == "login") return EV_LOGIN;
  if (e == "crew_login") return EV_CREW_LOGIN;
  if (e == "read") return EV_READ;
  if (e == "file") return EV_FILE;
  if (e == "art") return EV_ART;
  if (e == "door") return EV_DOOR;
  if (e == "idle") return EV_IDLE;
  if (e == "minutes") return EV_MINUTES;
  if (e == "screen") return EV_SCREEN;
  return EV_NONE;
}
inline bool idOk(const std::string &id) {
  if (id.empty() || id.size() > MAX_ID) return false;
  for (char c : id) if (!flagChar(c)) return false;
  return true;
}
inline bool nameOk(const std::string &s) {  // art names: no paths
  for (char c : s) if (!flagChar(c)) return false;
  return true;
}

// Build one rule from the key/value pairs of one section. Returns false with
// `why` set for a malformed rule.
inline bool buildRule(const std::string &id, const std::vector<Kv> &kv, Rule &r, std::string &why) {
  r = Rule();
  if (!idOk(id)) { why = "bad rule name"; return false; }
  r.id = upper(id);
  std::string levelMin, levelMax, after, baud;
  std::vector<std::string> seen;  // the first value of a key wins, as in iniGet()
  for (const auto &e : kv) {
    const std::string k = lower(trim(e.key)), &v = e.value;
    bool again = false;
    for (const auto &s : seen) if (s == k) { again = true; break; }
    if (again) continue;
    seen.push_back(k);
    if (k == "event") r.ev = parseEvent(v);
    else if (k == "match") r.match = upper(capText(v, MAX_MATCH));
    else if (k == "baud_max") baud = trim(v);
    else if (k == "after") after = trim(v);
    else if (k == "has") r.has = flagsClean(v);
    else if (k == "lacks") r.lacks = flagsClean(v);
    else if (k == "level_min") levelMin = trim(v);
    else if (k == "level_max") levelMax = trim(v);
    else if (k == "set") r.set = flagsClean(v);
    else if (k == "page_from") r.pageFrom = capText(v, MAX_NAME);
    else if (k == "page") r.page = capText(v, MAX_LINE);
    else if (k == "mail_from") r.mailFrom = capText(v, MAX_NAME);
    else if (k == "mail_conf") r.mailConf = capText(v, MAX_NAME * 2);
    else if (k == "mail_subject") r.mailSubject = capText(v, MAX_SUBJECT);
    else if (k == "mail_body") r.mailBody = capText(v, MAX_BODY);
    else if (k == "post_from") r.postFrom = capText(v, MAX_NAME);
    else if (k == "post_conf") r.postConf = capText(v, MAX_NAME * 2);
    else if (k == "post_subject") r.postSubject = capText(v, MAX_SUBJECT);
    else if (k == "post_body") r.postBody = capText(v, MAX_BODY);
    else if (k == "art") r.art = upper(trim(v));
    else if (k == "scene") r.scene = upper(trim(v));
    else if (k == "add") r.add = capText(v, MAX_LINE);
    // other keys: ignored, so a newer card still loads on older firmware
  }
  if (r.ev == EV_NONE) { why = "missing or unknown event"; return false; }
  uint32_t n = 0;
  if (!levelMin.empty()) { if (!parseUint(levelMin, 255, n)) { why = "bad level_min"; return false; } r.levelMin = (int)n; }
  if (!levelMax.empty()) { if (!parseUint(levelMax, 255, n)) { why = "bad level_max"; return false; } r.levelMax = (int)n; }
  if (r.levelMin > r.levelMax) { why = "level_min above level_max"; return false; }
  if (!baud.empty()) {
    if (r.ev != EV_ART) { why = "baud_max needs event=art"; return false; }
    if (!parseUint(baud, 10000000, r.baudMax) || !r.baudMax) { why = "bad baud_max"; return false; }
  }
  if (r.ev == EV_IDLE || r.ev == EV_MINUTES) {
    if (!parseUint(after, 1000000, r.after) || !r.after) { why = "idle and minutes need after= above 0"; return false; }
  } else if (!after.empty()) { why = "after needs event=idle or minutes"; return false; }
  if (r.ev == EV_CREW_LOGIN && r.match.empty()) { why = "crew_login needs match= (the crew handle)"; return false; }
  if (!r.art.empty() && !nameOk(r.art)) { why = "bad art name"; return false; }
  if (!r.scene.empty() && (!nameOk(r.scene) || r.scene.size() > MAX_ID)) { why = "bad scene name"; return false; }
  if (!r.mailBody.empty() && (r.mailFrom.empty() || r.mailSubject.empty())) { why = "mail needs mail_from, mail_subject and mail_body"; return false; }
  if (r.mailBody.empty() && (!r.mailFrom.empty() || !r.mailSubject.empty() || !r.mailConf.empty())) { why = "mail without a body"; return false; }
  if (!r.postBody.empty() && (r.postFrom.empty() || r.postConf.empty() || r.postSubject.empty())) { why = "post needs post_from, post_conf, post_subject and post_body"; return false; }
  if (r.postBody.empty() && (!r.postFrom.empty() || !r.postSubject.empty() || !r.postConf.empty())) { why = "post without a body"; return false; }
  if (!r.add.empty()) {
    if (r.ev != EV_READ) { why = "add needs event=read"; return false; }
    if (r.visible() || !r.set.empty()) { why = "add cannot share a rule with set, page, mail, post or art"; return false; }
  }
  if (r.set.empty() && !r.visible() && r.add.empty()) { why = "rule has no effect"; return false; }
  return true;
}

// The whole file. Entries of one section must be together (as in an INI file);
// a section name that comes back later is a duplicate and is skipped.
inline void parse(const std::vector<Kv> &entries, Table &t) {
  t = Table();
  auto fail = [&](const std::string &id, const std::string &why) {
    t.skipped++;
    if (t.problems.size() < 20) t.problems.push_back((id.empty() ? std::string("(none)") : id) + ": " + why);
  };
  size_t i = 0;
  while (i < entries.size()) {
    std::string name = trim(entries[i].section);
    std::vector<Kv> run;
    while (i < entries.size() && trim(entries[i].section) == name) run.push_back(entries[i++]);
    if (name.empty()) { fail("", "lines outside a [RULE] section"); continue; }
    if (t.rules.size() >= MAX_RULES) { fail(name, "too many rules"); continue; }
    bool dup = false;
    for (const auto &r : t.rules) if (r.id == upper(name)) { dup = true; break; }
    if (dup) { fail(name, "duplicate rule name"); continue; }
    Rule r; std::string why;
    if (buildRule(name, run, r, why)) t.rules.push_back(r); else fail(name, why);
  }
}

// ---------- deciding -----------------------------------------------------------------------
inline bool matches(const Rule &r, const Ctx &c) {
  if (r.ev != c.ev) return false;
  if (!r.match.empty() && r.match != upper(trim(c.match))) return false;
  if (r.baudMax && (!c.baud || c.baud > r.baudMax)) return false;
  if ((r.ev == EV_IDLE || r.ev == EV_MINUTES) && c.amount < r.after) return false;
  if (c.level < r.levelMin || c.level > r.levelMax) return false;
  if (!r.has.empty() && !flagsHasAll(c.flags, r.has)) return false;
  if (!r.lacks.empty() && flagsHasAny(c.flags, r.lacks)) return false;
  return true;
}
// Nothing left for this rule to do for this caller?
inline bool spent(const Rule &r, const std::string &flags) {
  if (!r.add.empty()) return false;                                   // shows on every re-read
  if (r.visible()) return flagsHas(flags, markerFor(r.id));           // once per caller
  return flagsHasAll(flags, r.set);                                   // set-only: nothing to change
}
// Rules to run for this event, in file order. All are judged on the flags before
// the event, so one event never chains rules.
inline std::vector<const Rule *> due(const Table &t, const Ctx &c) {
  std::vector<const Rule *> out;
  for (const auto &r : t.rules) if (matches(r, c) && !spent(r, c.flags)) out.push_back(&r);
  return out;
}
// The flags a due rule adds: its set= list plus, for a visible rule, its marker.
inline std::string flagsOf(const Rule &r) {
  return r.visible() ? flagsClean(r.set.empty() ? markerFor(r.id) : r.set + "," + markerFor(r.id)) : r.set;
}

}  // namespace haunt
