// Host-side test of src/core/wrap_logic.h. No board, no Arduino types.
//   zig c++ -std=c++17 -Wall -Wextra -I src/core tools/test/wrap_logic_test.cpp -o wrap_logic_test
#include "wrap_logic.h"
#include <stdio.h>

static int failures = 0;
#define CHECK(cond) do { if (!(cond)) { printf("FAIL line %d: %s\n", __LINE__, #cond); failures++; } } while (0)

static size_t visible(const std::string &s) {
  size_t n = 0;
  for (size_t i = 0; i < s.size(); ++i) {
    if (s[i] == 27) { while (i < s.size() && !isalpha((unsigned char)s[i])) i++; continue; }
    if (((unsigned char)s[i] & 0xc0) != 0x80) n++;
  }
  return n;
}

int main() {
  // A short line, an empty line and an exact fit stay one row.
  CHECK(wrap::rows("").size() == 1);
  CHECK(wrap::rows("hello world").size() == 1);
  CHECK(wrap::rows(std::string(79, 'x')).size() == 1);

  // A word that does not fit moves down whole.
  {
    std::string line = std::string(75, 'a') + " cabin";
    auto r = wrap::rows(line);
    CHECK(r.size() == 2);
    CHECK(r[0] == std::string(75, 'a'));
    CHECK(r[1] == "cabin");
  }
  // A word that ends exactly at column 79 stays; the space after it is dropped.
  {
    std::string line = std::string(73, 'a') + " bcdef next";
    auto r = wrap::rows(line);
    CHECK(r.size() == 2);
    CHECK(r[0] == std::string(73, 'a') + " bcdef");
    CHECK(r[1] == "next");
  }
  // No row is wider than 79 and the words survive, joined, in order.
  {
    std::string line;
    for (int i = 0; i < 60; ++i) line += "word" + std::to_string(i) + " ";
    auto r = wrap::rows(line);
    std::string joined;
    for (size_t i = 0; i < r.size(); ++i) {
      CHECK(visible(r[i]) <= 79);
      joined += r[i];
      if (i + 1 < r.size()) joined += ' ';
    }
    while (!joined.empty() && joined.back() == ' ') joined.pop_back();
    std::string want = line;
    while (!want.empty() && want.back() == ' ') want.pop_back();
    CHECK(joined == want);
  }
  // A word longer than a row is cut at the edge.
  {
    auto r = wrap::rows(std::string(100, 'z'));
    CHECK(r.size() == 2);
    CHECK(r[0].size() == 79 && r[1].size() == 21);
  }
  // Leading spaces stay (indented text) and do not make an empty row.
  {
    auto r = wrap::rows("    " + std::string(80, 'q'));
    CHECK(r.size() == 2);
    CHECK(visible(r[0]) <= 79);
  }
  // Colour codes take no width; UTF-8 characters take one.
  {
    std::string line = "\x1b[1;33m" + std::string(60, 'a') + "\x1b[0m end of line";
    auto r = wrap::rows(line);
    CHECK(r.size() == 1);
    std::string u;
    for (int i = 0; i < 78; ++i) u += "\xc3\xa4";
    u += " ab cd";
    auto w = wrap::rows(u);
    CHECK(w.size() == 2);
    CHECK(visible(w[0]) <= 79);
    CHECK(w[1] == "cd" || w[1] == "ab cd");
  }
  printf(failures ? "%d FAILED\n" : "all passed\n", failures);
  return failures ? 1 : 0;
}
