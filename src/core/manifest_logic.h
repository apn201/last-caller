#pragma once
// The crew manifest as pure logic: the parser for STORY/MANIFEST.INI (from
// plain key/value triples), which rows and options a caller may see, the
// picks as a short text, and filing (rows are confirmed in pairs, the last row
// alone, wrong rows are never named). No String, no SD card, no sessions: the
// board wraps this in doors/manifest.h. A plain C++ compiler tests it
// (tools/test/manifest_logic_test.cpp).
// Part of the shared core: included by doors/manifest.h.
#include "haunt_logic.h"
#include <algorithm>

namespace manifest {

using haunt::Kv;

static constexpr size_t MAX_ROWS = 8, MAX_OPTIONS = 80, MAX_NOTES = 40, MAX_PICKS = 400;
static constexpr int SLOTS = 3;

struct Option {
  std::string id, text, has;   // has: flags the caller needs to see it
  int slot = 1;                // 1..3
  bool last = false;           // belongs to the last row's pick lists
};

struct Row {
  std::string id, handle, needs, needsAny, done;   // done: the flag a confirmed row leaves
  std::string his = "their";                       // what {his} in an option reads as on this row
  bool last = false;                               // the ELIZA row: confirmed alone, shown last
  std::string answer[SLOTS];                       // option ids
};

struct Note { std::string has, about, text; };

struct Table {
  std::string title = "CREW MANIFEST";
  std::string slot[SLOTS] = {"Who they were", "What happened to them", "Who or what is responsible"};
  std::string lastSlot[SLOTS] = {"What she is", "Who gave her orders", "Why she is still running"};
  std::string hint[SLOTS], lastHint[SLOTS];   // shown under an empty pick list, in the caller's own voice
  std::string intro, fullScene;
  std::string count = "Rows confirmed: {n} of {total}.";
  std::string status, statusLast;      // Q21: under the rows once two crew rows are locked ({n} {total}), and once all are
  std::string filed = "Confirmed and locked: {names}.";
  std::string partial;                 // goodbye without finishing; "|" breaks a line
  std::string sentActivity = "Downlink open", sentIdle = "0:00", crewActivity = "On the crew list", crewIdle = "28y";
  std::vector<Row> rows;
  std::vector<Option> options;
  std::vector<Note> notes;
  unsigned skipped = 0;
  std::vector<std::string> problems;
  void problem(const std::string &id, const std::string &why) {
    skipped++;
    if (problems.size() < 20) problems.push_back((id.empty() ? std::string("(none)") : id) + ": " + why);
  }
};

inline std::string kget(const std::vector<Kv> &kv, const std::string &sec, const std::string &key, const std::string &fb = "") {
  for (const auto &e : kv) if (e.section == sec && e.key == key) return e.value;
  return fb;
}

inline std::string subst(std::string tmpl, const std::string &key, const std::string &value) {
  for (size_t at = tmpl.find(key); at != std::string::npos; at = tmpl.find(key, at + value.size())) tmpl.replace(at, key.size(), value);
  return tmpl;
}

inline bool startsWith(const std::string &s, const char *p) { return s.compare(0, std::string(p).size(), p) == 0; }

inline const Option *findOption(const Table &t, const std::string &id) {
  for (const auto &o : t.options) if (o.id == id) return &o;
  return nullptr;
}

// ---- parsing ---------------------------------------------------------------

inline void parse(const std::vector<Kv> &kv, Table &t) {
  t = Table();
  auto set = [&](const char *key, std::string &dst, size_t max) { std::string v = kget(kv, "MANIFEST", key); if (!v.empty()) dst = haunt::capText(v, max); };
  set("title", t.title, 40); set("intro", t.intro, 400); set("count", t.count, 120); set("filed", t.filed, 120); set("status", t.status, 160); set("status_last", t.statusLast, 160);
  for (int i = 0; i < SLOTS; ++i) {
    set(("slot" + std::to_string(i + 1)).c_str(), t.slot[i], 40);
    set(("last_slot" + std::to_string(i + 1)).c_str(), t.lastSlot[i], 40);
  }
  for (int i = 0; i < SLOTS; ++i) {
    set(("hint" + std::to_string(i + 1)).c_str(), t.hint[i], 200);
    set(("last_hint" + std::to_string(i + 1)).c_str(), t.lastHint[i], 200);
  }
  t.fullScene = haunt::upper(haunt::trim(kget(kv, "MANIFEST", "full_scene")));
  t.partial = haunt::capText(kget(kv, "PARTIAL", "text"), 600);
  auto sticky = [&](const char *key, std::string &dst) { std::string v = kget(kv, "STICKY", key); if (!v.empty()) dst = haunt::capText(v, 24); };
  sticky("sent_activity", t.sentActivity); sticky("sent_idle", t.sentIdle);
  sticky("crew_activity", t.crewActivity); sticky("crew_idle", t.crewIdle);

  std::vector<std::string> seen;
  for (const auto &e : kv) {
    if (std::find(seen.begin(), seen.end(), e.section) != seen.end()) continue;
    seen.push_back(e.section);
    const std::string &sec = e.section;
    if (startsWith(sec, "OPT_")) {
      std::string id = sec.substr(4);
      if (!haunt::idOk(id)) { t.problem(sec, "bad option id"); continue; }
      if (t.options.size() >= MAX_OPTIONS) { t.problem(sec, "over 80 options"); continue; }
      if (findOption(t, id)) { t.problem(sec, "same option twice"); continue; }
      Option o;
      o.id = id;
      o.text = haunt::capText(kget(kv, sec, "text"), 70);
      o.has = haunt::flagsClean(kget(kv, sec, "has"));
      uint32_t slot = 0;
      if (!haunt::parseUint(kget(kv, sec, "slot"), 3, slot) || slot < 1) { t.problem(sec, "slot must be 1 to 3"); continue; }
      o.slot = (int)slot;
      std::string group = haunt::lower(haunt::trim(kget(kv, sec, "group", "crew")));
      if (group != "crew" && group != "last") { t.problem(sec, "group must be crew or last"); continue; }
      o.last = group == "last";
      if (o.text.empty()) { t.problem(sec, "an option needs text="); continue; }
      t.options.push_back(o);
    } else if (startsWith(sec, "NOTE_")) {
      if (!haunt::idOk(sec.substr(5))) { t.problem(sec, "bad note id"); continue; }
      if (t.notes.size() >= MAX_NOTES) { t.problem(sec, "over 40 notes"); continue; }
      Note n;
      n.has = haunt::flagsClean(kget(kv, sec, "has"));
      n.about = haunt::upper(haunt::capText(kget(kv, sec, "about"), 16));
      n.text = haunt::capText(kget(kv, sec, "text"), 90);
      if (n.has.empty() || n.text.empty()) { t.problem(sec, "a note needs has= and text="); continue; }
      t.notes.push_back(n);
    }
  }
  // Rows after the options, so the answers can be checked against them.
  seen.clear();
  for (const auto &e : kv) {
    if (std::find(seen.begin(), seen.end(), e.section) != seen.end()) continue;
    seen.push_back(e.section);
    const std::string &sec = e.section;
    if (!startsWith(sec, "ROW_")) continue;
    std::string id = sec.substr(4);
    if (!haunt::idOk(id)) { t.problem(sec, "bad row id"); continue; }
    if (t.rows.size() >= MAX_ROWS) { t.problem(sec, "over 8 rows"); continue; }
    Row r;
    r.id = id;
    r.handle = haunt::upper(haunt::capText(kget(kv, sec, "handle", id), 16));
    std::string kind = haunt::lower(haunt::trim(kget(kv, sec, "kind", "crew")));
    if (kind != "crew" && kind != "last") { t.problem(sec, "kind must be crew or last"); continue; }
    r.last = kind == "last";
    r.his = haunt::lower(haunt::capText(kget(kv, sec, "his", "their"), 8));
    if (r.his.empty()) r.his = "their";
    r.needs = haunt::flagsClean(kget(kv, sec, "needs"));
    r.needsAny = haunt::flagsClean(kget(kv, sec, "needs_any"));
    r.done = haunt::flagsClean(kget(kv, sec, "done"));
    if (r.done.empty() || haunt::flagWords(r.done).size() != 1) { t.problem(sec, "done= must be one flag"); continue; }
    bool ok = true;
    for (int i = 0; i < SLOTS && ok; ++i) {
      r.answer[i] = haunt::upper(haunt::trim(kget(kv, sec, "a" + std::to_string(i + 1))));
      const Option *o = findOption(t, r.answer[i]);
      if (!o) { t.problem(sec, "a" + std::to_string(i + 1) + " is not an option"); ok = false; }
      else if (o->slot != i + 1 || o->last != r.last) { t.problem(sec, "a" + std::to_string(i + 1) + " is in the wrong list"); ok = false; }
    }
    if (!ok) continue;
    bool dup = false;
    for (const auto &x : t.rows) if (x.id == r.id || x.done == r.done) dup = true;
    if (dup) { t.problem(sec, "same row id or done flag twice"); continue; }
    if (!t.rows.empty() && t.rows.back().last) { t.problem(sec, "the last row must be the last in the file"); continue; }
    if (r.last) {
      bool second = false;
      for (const auto &x : t.rows) if (x.last) second = true;
      if (second) { t.problem(sec, "only one last row"); continue; }
    }
    t.rows.push_back(r);
  }
}

// ---- visibility ----------------------------------------------------------------

inline bool rowShown(const Row &r, const std::string &flags) {
  return (r.needs.empty() || haunt::flagsHasAll(flags, r.needs)) && (r.needsAny.empty() || haunt::flagsHasAny(flags, r.needsAny));
}
inline bool rowDone(const Row &r, const std::string &flags) { return haunt::flagsHas(flags, r.done); }
inline bool optionShown(const Option &o, const std::string &flags) { return o.has.empty() || haunt::flagsHasAll(flags, o.has); }

inline std::vector<const Row *> shownRows(const Table &t, const std::string &flags) {
  std::vector<const Row *> out;
  for (const auto &r : t.rows) if (rowShown(r, flags)) out.push_back(&r);
  return out;
}

// The pick list for a blank (slot 0..2) of a row, in file order, only what the caller may see.
inline std::vector<const Option *> choices(const Table &t, const Row &r, int slot, const std::string &flags) {
  std::vector<const Option *> out;
  for (const auto &o : t.options) if (o.slot == slot + 1 && o.last == r.last && optionShown(o, flags)) out.push_back(&o);
  return out;
}

// An option as one row shows it: {his} reads as the row's own word (her, his, their).
inline std::string optionText(const Option &o, const Row &r) { return subst(o.text, "{his}", r.his); }

// What the board says under a pick list with nothing in it (empty = nothing).
inline const std::string &emptyHint(const Table &t, const Row &r, int slot) { return r.last ? t.lastHint[slot] : t.hint[slot]; }

inline const std::string &slotLabel(const Table &t, const Row &r, int slot) { return r.last ? t.lastSlot[slot] : t.slot[slot]; }

// Does the manifest have anything to show this caller?
inline bool available(const Table &t, const std::string &flags) {
  for (const auto &r : t.rows) if (rowShown(r, flags)) return true;
  return false;
}

// ---- picks -----------------------------------------------------------------------
// "CMDR=IH,FH,CH;CHAOS=IA,,": one entry per row, three option ids (empty = blank).

using Picks3 = std::vector<std::string>;   // always three

inline std::vector<std::pair<std::string, Picks3>> picksParse(const std::string &text) {
  std::vector<std::pair<std::string, Picks3>> out;
  size_t at = 0;
  while (at <= text.size()) {
    size_t semi = text.find(';', at);
    std::string part = text.substr(at, semi == std::string::npos ? std::string::npos : semi - at);
    at = semi == std::string::npos ? text.size() + 1 : semi + 1;
    size_t eq = part.find('=');
    if (eq == std::string::npos || eq == 0) continue;
    Picks3 p(SLOTS);
    std::string rest = part.substr(eq + 1), cur;
    int i = 0;
    for (size_t k = 0; k <= rest.size(); ++k) {
      if (k == rest.size() || rest[k] == ',') { if (i < SLOTS) p[i] = haunt::upper(cur); ++i; cur.clear(); }
      else cur += rest[k];
    }
    out.push_back({haunt::upper(part.substr(0, eq)), p});
  }
  return out;
}
inline std::string picksText(const std::vector<std::pair<std::string, Picks3>> &v) {
  std::string out;
  for (const auto &e : v) {
    if (e.second[0].empty() && e.second[1].empty() && e.second[2].empty()) continue;
    if (!out.empty()) out += ';';
    out += e.first + "=" + e.second[0] + "," + e.second[1] + "," + e.second[2];
  }
  return out;
}
inline Picks3 picksOf(const std::string &text, const std::string &rowId) {
  for (const auto &e : picksParse(text)) if (e.first == haunt::upper(rowId)) return e.second;
  return Picks3(SLOTS);
}
// Set one blank (slot 0..2) of a row; "" clears it. False if the result would be too long.
inline bool setPick(std::string &text, const std::string &rowId, int slot, const std::string &optionId) {
  if (slot < 0 || slot >= SLOTS) return false;
  auto v = picksParse(text);
  bool found = false;
  for (auto &e : v) if (e.first == haunt::upper(rowId)) { e.second[slot] = haunt::upper(optionId); found = true; }
  if (!found) { Picks3 p(SLOTS); p[slot] = haunt::upper(optionId); v.push_back({haunt::upper(rowId), p}); }
  std::string next = picksText(v);
  if (next.size() > MAX_PICKS) return false;
  text = next;
  return true;
}

// How many blanks of the row hold a pick that is a real option.
inline unsigned pickedCount(const Table &t, const Row &r, const std::string &picks) {
  Picks3 p = picksOf(picks, r.id);
  unsigned n = 0;
  for (int i = 0; i < SLOTS; ++i) if (!p[i].empty() && findOption(t, p[i])) n++;
  return n;
}
inline bool rowRight(const Row &r, const std::string &picks) {
  Picks3 p = picksOf(picks, r.id);
  for (int i = 0; i < SLOTS; ++i) if (p[i] != r.answer[i]) return false;
  return true;
}

// ---- filing --------------------------------------------------------------------------

struct Filed {
  std::string grant;                  // flags to add, comma list
  std::vector<std::string> names;     // handles confirmed just now, in order
  unsigned confirmed = 0, total = 0;  // after filing
  bool full = false;                  // the last row was confirmed just now
};

inline Filed file(const Table &t, const std::string &flags, const std::string &picks) {
  Filed f;
  std::string local = flags;
  auto confirm = [&](const Row &r) {
    local = haunt::flagsAdd(local, r.done);
    f.grant = haunt::flagsAdd(f.grant, r.done);
    f.names.push_back(r.handle);
  };
  std::vector<const Row *> ready;
  for (const auto &r : t.rows)
    if (!r.last && rowShown(r, local) && !rowDone(r, local) && rowRight(r, picks)) ready.push_back(&r);
  size_t at = 0;
  while (ready.size() - at >= 2) { confirm(*ready[at]); confirm(*ready[at + 1]); at += 2; }
  for (const auto &r : t.rows) {
    if (!r.last || !rowShown(r, local) || rowDone(r, local) || !rowRight(r, picks)) continue;
    confirm(r);
    f.full = true;
  }
  unsigned crew = 0;
  for (const auto &r : t.rows) {
    if (!r.last) { crew++; if (rowDone(r, local)) f.confirmed++; }
    else if (rowShown(r, local)) { f.total++; if (rowDone(r, local)) f.confirmed++; }
  }
  f.total += crew;
  return f;
}

inline std::string joinNames(const std::vector<std::string> &names) {
  std::string out;
  for (size_t i = 0; i < names.size(); ++i) {
    if (i) out += (i + 1 == names.size()) ? " and " : ", ";
    out += names[i];
  }
  return out;
}

// What the board says after filing: the names just confirmed (if any), then
// the count. Nothing else, whatever is wrong.
inline std::string filedText(const Table &t, const Filed &f) {
  std::string out;
  if (!f.names.empty()) out = subst(t.filed, "{names}", joinNames(f.names)) + "\n";
  out += subst(subst(t.count, "{n}", std::to_string(f.confirmed)), "{total}", std::to_string(f.total));
  return out;
}

// ---- notebook ------------------------------------------------------------------------

inline std::vector<const Note *> notebook(const Table &t, const std::string &flags) {
  std::vector<const Note *> out;
  for (const auto &n : t.notes) if (haunt::flagsHasAll(flags, n.has)) out.push_back(&n);
  return out;
}

// ---- endings ---------------------------------------------------------------------------

inline bool finished(const std::string &flags) { return haunt::flagsHas(flags, "END1") || haunt::flagsHas(flags, "END2"); }

}  // namespace manifest
