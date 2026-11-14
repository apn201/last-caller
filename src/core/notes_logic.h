#pragma once
// The notebook's decisions, as pure logic (Q21): the notes table, the parser for
// STORY/NOTES.INI (from plain key/value triples) and "which notes are due now".
// A note is one short line the game has shown to the caller: a clue, a code, a
// name, a number, a direction. Showing it saves the flag N_<ID> on the caller's
// own account, once. The notebook lists the notes the caller holds, in the order
// the flags were saved. It keeps what was shown and never adds an answer.
// No String, no SD card, no sessions: the board wraps this in core/notes.h. A
// plain C++ compiler tests it (tools/test/notes_logic_test.cpp).
// Part of the shared core: included in order from src/bbs_core.h.
#include "haunt_logic.h"

namespace notes {

using haunt::Kv;

// Where a note is collected. READ to CREW_LOGIN are the trigger events (core/haunt.h) and match
// the same way; the rest are named by the module that shows the thing.
enum Kind : uint8_t { K_NONE, K_LOGIN, K_CREW_LOGIN, K_READ, K_FILE, K_ART, K_DOOR, K_SCREEN,
                      K_SCENE,    // match=NAME when the scene starts, NAME/NODE when a node is shown
                      K_PAGE,     // match=RULE: a page a trigger rule sends (TRIGGERS.INI page=)
                      K_ELIZA,    // match=LINE: a line of hers (SYSOP.INI) in chat or after a rule
                      K_GHOST,    // match=GHOST: a ghost's page (GHOSTS.INI)
                      K_DIAG,     // match=CODE: an answer of the diagnostics door
                      K_SHELL };  // match=PATH: a file or listing in the NOWHERE shell

static constexpr size_t MAX_NOTES = 200;      // memory on the stick
static constexpr size_t MAX_ID = 14;          // "N_" + id is the flag; 16 characters in the story record
static constexpr size_t MAX_TEXT = 90, MAX_SRC = 30, MAX_WHERE = 40, MAX_MATCH = 40;

// Replay: how a note can be shown again as it was (or "" for none).
enum Replay : uint8_t { R_NONE, R_SCENE, R_PAGE, R_ELIZA, R_GHOST, R_ART };

struct Note {
  std::string id;               // upper case, letters digits _
  Kind kind = K_NONE;
  std::string match;            // upper case; empty = any
  uint32_t baudMax = 0;         // art: only at that speed or slower (0 = any)
  std::string has, lacks;       // flags the caller needs / must not have for the note to be taken
  std::string src;              // where it came from: "Mail from CMDR", "SHIP.LOG"
  std::string text;             // the line kept
  std::string where;            // where to read it again (area and file), for files; messages are looked up
  std::string clue;             // story bible and puzzle chain ids this note covers (E16, M3): for the lint only
  Replay replay = R_NONE;
  std::string replayName;       // scene, rule, line, ghost or art name
  std::string flag() const { return "N_" + id; }
};

struct Table {
  std::vector<Note> notes;
  unsigned skipped = 0;
  std::vector<std::string> problems;
};

inline Kind parseKind(const std::string &s) {
  std::string e = haunt::lower(haunt::trim(s));
  if (e == "login") return K_LOGIN;
  if (e == "crew_login") return K_CREW_LOGIN;
  if (e == "read") return K_READ;
  if (e == "file") return K_FILE;
  if (e == "art") return K_ART;
  if (e == "door") return K_DOOR;
  if (e == "screen") return K_SCREEN;
  if (e == "scene") return K_SCENE;
  if (e == "page") return K_PAGE;
  if (e == "eliza") return K_ELIZA;
  if (e == "ghost") return K_GHOST;
  if (e == "diag") return K_DIAG;
  if (e == "shell") return K_SHELL;
  return K_NONE;
}
// The trigger events, for core/haunt.h to hand over.
inline Kind kindOf(haunt::Ev ev) {
  switch (ev) {
    case haunt::EV_LOGIN: return K_LOGIN;
    case haunt::EV_CREW_LOGIN: return K_CREW_LOGIN;
    case haunt::EV_READ: return K_READ;
    case haunt::EV_FILE: return K_FILE;
    case haunt::EV_ART: return K_ART;
    case haunt::EV_DOOR: return K_DOOR;
    case haunt::EV_SCREEN: return K_SCREEN;
    default: return K_NONE;
  }
}

// replay= is "scene:NAME", "page:RULE", "eliza:LINE", "ghost:ID" or "art:NAME".
inline bool parseReplay(const std::string &v, Replay &r, std::string &name) {
  std::string t = haunt::trim(v);
  size_t c = t.find(':');
  if (c == std::string::npos) return false;
  std::string k = haunt::lower(haunt::trim(t.substr(0, c)));
  name = haunt::upper(haunt::trim(t.substr(c + 1)));
  if (!haunt::idOk(name)) return false;
  if (k == "scene") r = R_SCENE;
  else if (k == "page") r = R_PAGE;
  else if (k == "eliza") r = R_ELIZA;
  else if (k == "ghost") r = R_GHOST;
  else if (k == "art") r = R_ART;
  else return false;
  return true;
}

inline bool buildNote(const std::string &id, const std::vector<Kv> &kv, Note &n, std::string &why) {
  n = Note();
  if (!haunt::idOk(id) || id.size() > MAX_ID) { why = "bad note name"; return false; }
  n.id = haunt::upper(id);
  std::string baud;
  std::vector<std::string> seen;
  for (const auto &e : kv) {
    const std::string k = haunt::lower(haunt::trim(e.key)), &v = e.value;
    bool again = false;
    for (const auto &s : seen) if (s == k) { again = true; break; }
    if (again) continue;
    seen.push_back(k);
    if (k == "on") n.kind = parseKind(v);
    else if (k == "match") n.match = haunt::upper(haunt::capText(v, MAX_MATCH));
    else if (k == "baud_max") baud = haunt::trim(v);
    else if (k == "has") n.has = haunt::flagsClean(v);
    else if (k == "lacks") n.lacks = haunt::flagsClean(v);
    else if (k == "src") n.src = haunt::capText(v, MAX_SRC);
    else if (k == "text") n.text = haunt::capText(v, MAX_TEXT);
    else if (k == "where") n.where = haunt::capText(v, MAX_WHERE);
    else if (k == "clue") n.clue = haunt::flagsClean(v);
    else if (k == "replay") { if (!parseReplay(v, n.replay, n.replayName)) { why = "bad replay"; return false; } }
    // other keys: ignored, so a newer card still loads on older firmware
  }
  if (n.kind == K_NONE) { why = "missing or unknown on="; return false; }
  if (n.text.empty()) { why = "no text"; return false; }
  if (n.src.empty()) { why = "no src"; return false; }
  if (n.match.empty() && n.kind != K_LOGIN) { why = "needs match="; return false; }
  if (!baud.empty()) {
    if (n.kind != K_ART) { why = "baud_max needs on=art"; return false; }
    if (!haunt::parseUint(baud, 10000000, n.baudMax) || !n.baudMax) { why = "bad baud_max"; return false; }
  }
  return true;
}

// The whole file. Entries of one section must be together; a section that comes back
// later is a duplicate and is skipped.
inline void parse(const std::vector<Kv> &entries, Table &t) {
  t = Table();
  auto fail = [&](const std::string &id, const std::string &why) {
    t.skipped++;
    if (t.problems.size() < 20) t.problems.push_back((id.empty() ? std::string("(none)") : id) + ": " + why);
  };
  size_t i = 0;
  while (i < entries.size()) {
    std::string name = haunt::trim(entries[i].section);
    std::vector<Kv> run;
    while (i < entries.size() && haunt::trim(entries[i].section) == name) run.push_back(entries[i++]);
    if (name.empty()) { fail("", "lines outside a [NOTE] section"); continue; }
    if (t.notes.size() >= MAX_NOTES) { fail(name, "too many notes"); continue; }
    bool dup = false;
    for (const auto &n : t.notes) if (n.id == haunt::upper(name)) { dup = true; break; }
    if (dup) { fail(name, "duplicate note name"); continue; }
    Note n; std::string why;
    if (buildNote(name, run, n, why)) t.notes.push_back(n); else fail(name, why);
  }
}

// ---------- deciding -----------------------------------------------------------------------
// The notes this thing being shown puts in the notebook now: the ones that match and that the
// caller does not hold yet. `flags` are the caller's flags before. A held note is never due again,
// so showing a line twice saves its flag once.
inline std::vector<const Note *> due(const Table &t, Kind kind, const std::string &match, uint32_t baud, const std::string &flags) {
  std::vector<const Note *> out;
  if (kind == K_NONE) return out;
  const std::string m = haunt::upper(haunt::trim(match));
  for (const auto &n : t.notes) {
    if (n.kind != kind) continue;
    if (!n.match.empty() && n.match != m) continue;
    if (n.baudMax && (!baud || baud > n.baudMax)) continue;
    if (!n.has.empty() && !haunt::flagsHasAll(flags, n.has)) continue;
    if (!n.lacks.empty() && haunt::flagsHasAny(flags, n.lacks)) continue;
    if (haunt::flagsHas(flags, n.flag())) continue;
    out.push_back(&n);
  }
  return out;
}
// The flags to save for a set of due notes.
inline std::string flagsOf(const std::vector<const Note *> &due) {
  std::string out;
  for (const Note *n : due) { if (!out.empty()) out += ','; out += n->flag(); }
  return out;
}
inline const Note *find(const Table &t, const std::string &id) {
  const std::string u = haunt::upper(id);
  for (const auto &n : t.notes) if (n.id == u) return &n;
  return nullptr;
}
// What the caller holds, in the order the flags were saved ("in the order seen").
inline std::vector<const Note *> held(const Table &t, const std::string &flags) {
  std::vector<const Note *> out;
  for (const auto &w : haunt::flagWords(flags)) {
    if (w.size() < 3 || w.compare(0, 2, "N_") != 0) continue;
    if (const Note *n = find(t, w.substr(2))) out.push_back(n);
  }
  return out;
}

}  // namespace notes
