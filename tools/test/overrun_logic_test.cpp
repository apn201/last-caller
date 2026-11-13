// Host-side test of src/core/overrun_logic.h. No board, no Arduino types.
//   zig c++ -std=c++17 -Wall -Wextra -I src/core tools/test/overrun_logic_test.cpp -o overrun_logic_test
#include "overrun_logic.h"
#include <stdio.h>

static int failures = 0;
#define CHECK(cond) do { if (!(cond)) { printf("FAIL line %d: %s\n", __LINE__, #cond); failures++; } } while (0)

int main() {
  CHECK(!overrun::inBody(""));
  CHECK(!overrun::inBody("hello"));
  CHECK(!overrun::inBody(std::string(254, '0')));            // the near miss
  CHECK(overrun::inBody(std::string(255, '0')));
  CHECK(overrun::inBody(std::string(1000, '0')));
  CHECK(overrun::inBody("hi\n" + std::string(255, '0') + "\nbye"));
  // Line breaks do not end a run: three lines of zeros join.
  CHECK(overrun::inBody(std::string(100, '0') + "\n" + std::string(100, '0') + "\n" + std::string(55, '0')));
  CHECK(overrun::inBody(std::string(100, '0') + "\r\n" + std::string(155, '0')));
  CHECK(!overrun::inBody(std::string(100, '0') + "\n" + std::string(100, '0') + "\n" + std::string(54, '0')));
  // One different character in the middle breaks the run: a letter O, a space, a one.
  CHECK(!overrun::inBody(std::string(127, '0') + "O" + std::string(127, '0')));
  CHECK(!overrun::inBody(std::string(127, '0') + " " + std::string(127, '0')));
  CHECK(!overrun::inBody(std::string(127, '0') + "1" + std::string(200, '0')));
  // Two short runs add up to nothing.
  CHECK(!overrun::inBody(std::string(200, '0') + "x" + std::string(200, '0')));
  if (failures) { printf("%d failed\n", failures); return 1; }
  printf("all passed\n");
  return 0;
}
