// Property test of the card's own story: STORY/TRIGGERS.INI and STORY/MANIFEST.INI run through
// the shared logic headers (haunt_logic.h, manifest_logic.h). No board, no Arduino types, no card
// writes. Run by CTest in the server build (server/CMakeLists.txt) and by hand:
//   zig c++ -std=c++17 -Wall -Wextra -I src/core tools/test/story_property_test.cpp -o story_property_test
//   ./story_property_test sd/BBS/STORY/TRIGGERS.INI sd/BBS/STORY/MANIFEST.INI
//
// Properties, over many random orders in which a caller could trigger every event the rules name:
//   P1  flags only grow (byte for byte, step by step)
//   P2  delivering the same event again changes nothing (idempotent)
//   P3  the flags a caller ends with do not depend on the order of events (T_ markers aside)
//   P4  the flags every evidence option and row of the manifest asks for are all reached
//   P5  filing the right picks confirms every row, two crew rows at a time, ELIZA's row last
//   P6  random picks never confirm a row that is wrong, and confirmed rows stay confirmed
//   P7  no rule that only sets flags depends on a flag only it can set (no deadlock)
#include "haunt_logic.h"
#include "manifest_logic.h"
#include <algorithm>
#include <fstream>
#include <set>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

static int failures = 0;
#define CHECK(cond) do { if (!(cond)) { printf("FAIL line %d: %s\n", __LINE__, #cond); failures++; } } while (0)
#define CHECKF(cond, ...) do { if (!(cond)) { printf("FAIL line %d: ", __LINE__); printf(__VA_ARGS__); printf("\n"); failures++; } } while (0)

static uint32_t lcg = 20261113;
static uint32_t rnd() { lcg = lcg * 1664525u + 1013904223u; return lcg >> 8 | lcg << 24; }

static std::vector<haunt::Kv> iniText(const std::string &text) {
  std::vector<haunt::Kv> out;
  std::string section, line;
  for (size_t i = 0; i <= text.size(); ++i) {
    if (i == text.size() || text[i] == '\n') {
      std::string t = haunt::trim(line);
      line.clear();
      if (t.empty() || t[0] == ';') continue;
      if (t[0] == '[') { size_t c = t.find(']'); section = haunt::upper(haunt::trim(t.substr(1, c == std::string::npos ? std::string::npos : c - 1))); }
      else { size_t eq = t.find('='); if (eq != std::string::npos && eq > 0) out.push_back({section, haunt::lower(haunt::trim(t.substr(0, eq))), haunt::trim(t.substr(eq + 1))}); }
    } else if (text[i] != '\r') line += text[i];
  }
  return out;
}
static bool slurp(const char *path, std::string &text) {
  std::ifstream in(path, std::ios::binary);
  if (!in) return false;
  std::stringstream ss; ss << in.rdbuf(); text = ss.str();
  return true;
}

// One event as a caller can cause it, taken from a rule.
static haunt::Ctx eventOf(const haunt::Rule &r) {
  haunt::Ctx c;
  c.ev = r.ev; c.match = r.match; c.level = 20; c.amount = r.after; c.baud = r.baudMax ? r.baudMax : 0;
  return c;
}
// Deliver one event: the rules due on the flags as they were, then their grants.
static std::string deliver(const haunt::Table &t, haunt::Ctx c, std::string flags) {
  c.flags = flags;
  std::string grant;
  for (const haunt::Rule *r : haunt::due(t, c)) grant = haunt::flagsAdd(grant, haunt::flagsOf(*r));
  return grant.empty() ? flags : haunt::flagsAdd(flags, grant);
}
static std::string withoutMarkers(const std::string &flags) {
  std::set<std::string> keep;
  for (const auto &w : haunt::flagWords(flags)) if (w.compare(0, 2, "T_") != 0) keep.insert(w);
  std::string out;
  for (const auto &w : keep) out += (out.empty() ? "" : ",") + w;
  return out;
}
static bool grows(const std::string &before, const std::string &after) {
  for (const auto &w : haunt::flagWords(before)) if (!haunt::flagsHas(after, w)) return false;
  return true;
}

int main(int argc, char **argv) {
  if (argc < 3) { printf("usage: story_property_test TRIGGERS.INI MANIFEST.INI\n"); return 2; }
  std::string trText, maText;
  if (!slurp(argv[1], trText) || !slurp(argv[2], maText)) { printf("FAIL cannot read the card files\n"); return 1; }
  haunt::Table tr; haunt::parse(iniText(trText), tr);
  manifest::Table ma; manifest::parse(iniText(maText), ma);
  for (const auto &p : tr.problems) printf("triggers: %s\n", p.c_str());
  for (const auto &p : ma.problems) printf("manifest: %s\n", p.c_str());
  CHECK(tr.skipped == 0 && !tr.rules.empty());
  CHECK(ma.skipped == 0 && !ma.rows.empty());
  printf("%zu rules, %zu manifest rows, %zu options\n", tr.rules.size(), ma.rows.size(), ma.options.size());

  // Every distinct event the rules name (the rules' own match values).
  std::vector<haunt::Ctx> events;
  {
    std::set<std::string> seen;
    for (const auto &r : tr.rules) {
      haunt::Ctx c = eventOf(r);
      std::string key = std::to_string((int)c.ev) + "|" + c.match + "|" + std::to_string(c.amount) + "|" + std::to_string(c.baud);
      if (seen.insert(key).second) events.push_back(c);
    }
  }
  CHECK(!events.empty());

  // P1, P2, P3: many random orders, every event delivered until nothing changes.
  std::string firstEnd;
  const int ORDERS = 300;
  for (int n = 0; n < ORDERS; ++n) {
    std::vector<haunt::Ctx> order = events;
    for (size_t i = order.size(); i > 1; --i) std::swap(order[i - 1], order[rnd() % i]);
    std::string flags;
    // The login-class events need flags that other events give; passes in this order, until stable.
    for (int pass = 0; pass < 12; ++pass) {
      std::string passStart = flags;
      for (const auto &ev : order) {
        std::string next = deliver(tr, ev, flags);
        if (!grows(flags, next)) { CHECKF(false, "P1 flags shrank on event %d %s", (int)ev.ev, ev.match.c_str()); }
        std::string again = deliver(tr, ev, next);
        if (again != next) { CHECKF(false, "P2 event %d %s is not idempotent", (int)ev.ev, ev.match.c_str()); }
        flags = next;
      }
      if (flags == passStart) break;
    }
    std::string end = withoutMarkers(flags);
    if (n == 0) firstEnd = end;
    else if (end != firstEnd) {
      CHECKF(false, "P3 order %d ends with other flags:\n  first: %s\n  this:  %s", n, firstEnd.c_str(), end.c_str());
      break;
    }
  }

  // P4: the closure holds everything the manifest needs.
  const std::string &all = firstEnd;
  for (const auto &o : ma.options) CHECKF(o.has.empty() || haunt::flagsHasAll(all, o.has), "P4 option %s needs %s: not reachable", o.id.c_str(), o.has.c_str());
  for (const auto &r : ma.rows) {
    if (r.last) continue;
    CHECKF(r.needsAny.empty() || haunt::flagsHasAny(all, r.needsAny), "P4 row %s is never shown (%s)", r.id.c_str(), r.needsAny.c_str());
  }

  // P5: all right picks, filed again and again, confirm every row; crew rows in pairs, ELIZA last.
  std::string picks;
  for (const auto &r : ma.rows) for (int s = 0; s < manifest::SLOTS; ++s) manifest::setPick(picks, r.id, s, r.answer[s]);
  {
    std::string flags = all;
    unsigned lastCount = 0;
    for (int round = 0; round < 8; ++round) {
      manifest::Filed f = manifest::file(ma, flags, picks);
      if (f.grant.empty()) break;
      CHECK(f.names.size() <= 2 || f.full);
      if (f.full) CHECK(f.names.size() >= 1 && f.names.back() == "ELIZA");
      flags = haunt::flagsAdd(flags, f.grant);
      lastCount = f.confirmed;
    }
    for (const auto &r : ma.rows) CHECKF(haunt::flagsHas(flags, r.done), "P5 row %s never confirmed", r.id.c_str());
    CHECK(lastCount == ma.rows.size());
  }

  // P6: random picks, many rounds. A confirmed row is a right row, and confirmations stay.
  for (int n = 0; n < 400; ++n) {
    std::string flags = all, rp;
    std::string seenDone;
    for (int round = 0; round < 6; ++round) {
      for (const auto &r : ma.rows)
        for (int s = 0; s < manifest::SLOTS; ++s) {
          std::vector<const manifest::Option *> pool;
          for (const auto &o : ma.options) if (o.slot == s + 1 && o.last == r.last) pool.push_back(&o);
          if (!pool.empty() && rnd() % 3) manifest::setPick(rp, r.id, s, pool[rnd() % pool.size()]->id);
        }
      manifest::Filed f = manifest::file(ma, flags, rp);
      for (const auto &name : f.names)
        for (const auto &r : ma.rows) if (r.handle == name) CHECKF(manifest::rowRight(r, rp), "P6 row %s confirmed on wrong picks", r.id.c_str());
      std::string next = haunt::flagsAdd(flags, f.grant);
      CHECK(grows(flags, next));
      flags = next;
      CHECK(grows(seenDone, flags));
      seenDone = flags;
    }
  }

  // P7: a flag-only rule must not need a flag that only it (or rules that need it) can give.
  {
    std::set<std::string> reach;                 // flags a caller can get, rule by rule, from nothing
    bool moved = true;
    while (moved) {
      moved = false;
      for (const auto &r : tr.rules) {
        bool ok = true;
        for (const auto &w : haunt::flagWords(r.has)) if (!reach.count(w)) ok = false;
        if (!ok) continue;
        for (const auto &w : haunt::flagWords(haunt::flagsOf(r))) if (reach.insert(w).second) moved = true;
      }
    }
    for (const auto &r : tr.rules)
      for (const auto &w : haunt::flagWords(r.has))
        CHECKF(reach.count(w) || w == "END1" || w == "END2" || w.compare(0, 2, "M_") == 0 || w.compare(0, 3, "SC_") == 0 || w.compare(0, 2, "T_") == 0,
               "P7 rule %s needs %s which no rule can give", r.id.c_str(), w.c_str());
  }

  if (failures) { printf("story_property_test: %d FAILED\n", failures); return 1; }
  printf("story_property_test: all properties hold (%d orders)\n", ORDERS);
  return 0;
}
