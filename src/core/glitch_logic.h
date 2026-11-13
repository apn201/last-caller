#pragma once
// Glitches (S7): rare, timed slips in the board's own screen, driven by the card
// (STORY/GLITCH.INI). Pure logic: no String, no card, no sessions. The board side
// is core/glitch.h; a plain C++ compiler tests this (tools/test/glitch_logic_test.cpp).
//
// Rule of the file: rare and timed. A caller gets nothing in the first first_sec
// seconds after signing in, and then at most one glitch every gap_sec seconds.
// A glitch is one line, shown at a calm menu prompt. It never sets a flag.
// Part of the shared core: included from core/glitch.h.
#include "haunt_logic.h"

namespace glitch {

static constexpr size_t MAX_ITEMS = 40;
static constexpr uint32_t MIN_GAP = 60;   // seconds: the card cannot ask for more than one a minute

struct Item {
  std::string id, text, has, lacks, sound;
  bool garble = false;       // one letter of the text is wrong each time it is shown
  uint32_t afterMin = 0;     // minutes since sign-in
};
struct Table {
  uint32_t gapSec = 240, firstSec = 180;
  std::vector<Item> items;
  std::vector<std::string> problems;
};

inline void parse(const std::vector<haunt::Kv> &kv, Table &t) {
  t = Table();
  std::vector<std::string> order;
  for (const auto &e : kv) {
    std::string sec = haunt::upper(e.section);
    if (sec.empty()) continue;
    if (sec == "SETTINGS") {
      uint32_t v = 0;
      if (e.key == "gap_sec" && haunt::parseUint(e.value, 86400, v)) t.gapSec = v < MIN_GAP ? MIN_GAP : v;
      if (e.key == "first_sec" && haunt::parseUint(e.value, 86400, v)) t.firstSec = v;
      continue;
    }
    Item *it = nullptr;
    for (auto &x : t.items) if (x.id == sec) it = &x;
    if (!it) {
      if (t.items.size() >= MAX_ITEMS) { t.problems.push_back(sec + ": too many glitches"); continue; }
      if (!haunt::idOk(sec)) { t.problems.push_back(sec + ": bad id"); continue; }
      t.items.push_back(Item());
      it = &t.items.back();
      it->id = sec;
    }
    uint32_t v = 0;
    if (e.key == "text") it->text = haunt::capText(e.value, haunt::MAX_LINE);
    else if (e.key == "has") it->has = e.value;
    else if (e.key == "lacks") it->lacks = e.value;
    else if (e.key == "sound") it->sound = e.value;
    else if (e.key == "garble") it->garble = e.value == "1";
    else if (e.key == "after_min" && haunt::parseUint(e.value, 100000, v)) it->afterMin = v;
  }
  for (size_t i = t.items.size(); i-- > 0;)
    if (t.items[i].text.empty()) { t.problems.push_back(t.items[i].id + ": no text"); t.items.erase(t.items.begin() + i); }
}

// One letter of the longest-enough word becomes its neighbour in the alphabet
// (e -> f, Z -> Y). Text with no word of four letters is left as it is. The seed
// picks the word and the letter, so a test can repeat it.
inline std::string garble(const std::string &s, uint32_t seed) {
  std::vector<std::pair<size_t, size_t>> words;   // start, length
  size_t i = 0;
  while (i < s.size()) {
    if (!((s[i] >= 'a' && s[i] <= 'z') || (s[i] >= 'A' && s[i] <= 'Z'))) { ++i; continue; }
    size_t j = i;
    while (j < s.size() && ((s[j] >= 'a' && s[j] <= 'z') || (s[j] >= 'A' && s[j] <= 'Z'))) ++j;
    if (j - i >= 4) words.push_back({i, j - i});
    i = j;
  }
  if (words.empty()) return s;
  auto w = words[seed % words.size()];
  size_t at = w.first + 1 + (seed / 7) % (w.second - 1);   // never the first letter
  std::string out = s;
  char c = out[at];
  bool up = c >= 'A' && c <= 'Z';
  char base = up ? 'A' : 'a';
  int n = (c - base + ((seed & 1) ? 1 : 25)) % 26;
  out[at] = (char)(base + n);
  return out;
}

// Is a glitch allowed now? `online` is seconds since sign-in, `sinceLast` seconds since the
// last one (ignored when `any` is false).
inline bool allowed(const Table &t, uint32_t online, bool any, uint32_t sinceLast) {
  if (t.items.empty() || online < t.firstSec) return false;
  return !any || sinceLast >= t.gapSec;
}

// The next glitch that fits this caller, starting at `cursor` and going round once. Returns
// its index or -1. The caller keeps the cursor (index + 1) so the same slip does not repeat
// until the others have been shown.
inline int pick(const Table &t, const std::string &flags, uint32_t minutesOnline, size_t cursor) {
  size_t n = t.items.size();
  for (size_t k = 0; k < n; ++k) {
    size_t i = (cursor + k) % n;
    const Item &it = t.items[i];
    if (minutesOnline < it.afterMin) continue;
    if (!it.has.empty() && !haunt::flagsHasAll(flags, it.has)) continue;
    if (!it.lacks.empty() && haunt::flagsHasAny(flags, it.lacks)) continue;
    return (int)i;
  }
  return -1;
}

}  // namespace glitch
