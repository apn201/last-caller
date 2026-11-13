// Host-side test of src/core/glitch_logic.h. No board, no Arduino types.
//   zig c++ -std=c++17 -Wall -Wextra -I src/core tools/test/glitch_logic_test.cpp -o glitch_logic_test
//   ./glitch_logic_test sd/BBS/STORY/GLITCH.INI   also checks the card's own file loads with no problem.
#include "glitch_logic.h"
#include <stdio.h>
#include <fstream>
#include <sstream>

static int failures = 0;
#define CHECK(cond) do { if (!(cond)) { printf("FAIL line %d: %s\n", __LINE__, #cond); failures++; } } while (0)

static std::vector<haunt::Kv> ini(const std::string &text) {
  std::vector<haunt::Kv> out;
  std::string section, line;
  for (size_t i = 0; i <= text.size(); ++i) {
    if (i == text.size() || text[i] == '\n') {
      std::string t = haunt::trim(line);
      line.clear();
      if (!t.empty() && t[0] != ';') {
        if (t[0] == '[') { size_t c = t.find(']'); section = haunt::upper(haunt::trim(t.substr(1, c == std::string::npos ? std::string::npos : c - 1))); }
        else { size_t eq = t.find('='); if (eq != std::string::npos && eq > 0) out.push_back({section, haunt::lower(haunt::trim(t.substr(0, eq))), haunt::trim(t.substr(eq + 1))}); }
      }
    } else line += text[i];
  }
  return out;
}
static glitch::Table table(const std::string &text) { glitch::Table t; glitch::parse(ini(text), t); return t; }

static int diffs(const std::string &a, const std::string &b) {
  int n = 0;
  if (a.size() != b.size()) return 99;
  for (size_t i = 0; i < a.size(); ++i) if (a[i] != b[i]) ++n;
  return n;
}

int main(int argc, char **argv) {
  using namespace glitch;
  // parsing
  Table t = table("[SETTINGS]\nfirst_sec=100\ngap_sec=10\n[A]\ntext=hello there\n[B]\ngarble=1\n[C]\ntext=x\nhas=ACT2\nafter_min=5\nsound=HUM\n");
  CHECK(t.firstSec == 100);
  CHECK(t.gapSec == MIN_GAP);                 // never more often than once a minute
  CHECK(t.items.size() == 2);                 // B has no text
  CHECK(t.problems.size() == 1);
  CHECK(t.items[1].id == "C" && t.items[1].afterMin == 5 && t.items[1].sound == "HUM");

  // timing: quiet at first, then spaced
  CHECK(!allowed(t, 99, false, 0));
  CHECK(allowed(t, 100, false, 0));
  CHECK(!allowed(t, 500, true, MIN_GAP - 1));
  CHECK(allowed(t, 500, true, MIN_GAP));
  CHECK(!allowed(Table(), 9999, false, 0));   // no glitches on the card

  // picking: flags and minutes decide, the cursor rotates, flags are never needed
  CHECK(pick(t, "", 0, 0) == 0);
  CHECK(pick(t, "", 10, 1) == 0);             // C needs ACT2: wraps round to A
  CHECK(pick(t, "ACT2", 3, 1) == 0);          // C needs 5 minutes
  CHECK(pick(t, "ACT2", 5, 1) == 1);
  CHECK(pick(t, "", 0, 7) == 0);              // a cursor past the end still works
  Table lone = table("[X]\ntext=only\nhas=ACT3\n");
  CHECK(pick(lone, "ACT2", 99, 0) == -1);
  Table lack = table("[X]\ntext=only\nlacks=ACT3\n");
  CHECK(pick(lack, "ACT3", 99, 0) == -1 && pick(lack, "ACT2", 99, 0) == 0);

  // garble: one letter, never the first of a word, same size, no change for short words
  const std::string s = "ELIZA: Cabin temperature is 21.0 C.";
  for (uint32_t seed = 0; seed < 200; ++seed) {
    std::string g = garble(s, seed);
    CHECK(diffs(s, g) == 1);
    CHECK(g.find("21.0 C.") != std::string::npos);   // digits and symbols are never touched
  }
  CHECK(garble("Go on, you.", 3) == "Go on, you.");
  CHECK(garble("", 3) == "");
  CHECK(garble(s, 5) == garble(s, 5));

  // the card's own file
  for (int i = 1; i < argc; ++i) {
    std::ifstream f(argv[i]);
    std::stringstream ss; ss << f.rdbuf();
    Table c = table(ss.str());
    if (ss.str().empty()) { printf("FAIL cannot read %s\n", argv[i]); failures++; continue; }
    CHECK(c.problems.empty());
    CHECK(!c.items.empty());
    CHECK(c.gapSec >= MIN_GAP && c.firstSec >= 120);
    for (const auto &it : c.items) {
      CHECK(it.text.size() <= 79);
      for (char ch : it.text) CHECK(ch >= 32 && ch < 127);
      CHECK(it.sound.empty() || it.sound.size() <= 16);
    }
  }
  if (failures) { printf("%d failed\n", failures); return 1; }
  printf("all passed\n");
  return 0;
}
