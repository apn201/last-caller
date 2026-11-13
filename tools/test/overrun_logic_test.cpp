// Host-side test of src/core/overrun_logic.h. No board, no Arduino types.
//   zig c++ -std=c++17 -Wall -Wextra -I src/core tools/test/overrun_logic_test.cpp -o overrun_logic_test
#include "overrun_logic.h"
#include <stdio.h>

static int failures = 0;
#define CHECK(cond) do { if (!(cond)) { printf("FAIL line %d: %s\n", __LINE__, #cond); failures++; } } while (0)

int main() {
  CHECK(overrun::RUN == 20);
  CHECK(!overrun::inBody(""));
  CHECK(!overrun::inBody("hello"));
  CHECK(!overrun::inBody(std::string(19, '0')));             // the near miss
  CHECK(overrun::inBody(std::string(20, '0')));
  CHECK(overrun::inBody(std::string(1000, '0')));
  CHECK(overrun::inBody("hi\n" + std::string(20, '0') + "\nbye"));
  // The near miss is exactly one short: 19 glitches, fewer does nothing, 20 is the overrun.
  CHECK(overrun::nearMiss(std::string(19, '0')));
  CHECK(overrun::nearMiss("a" + std::string(19, '0') + "b"));
  CHECK(!overrun::nearMiss(std::string(18, '0')));
  CHECK(!overrun::nearMiss(std::string(5, '0')));
  CHECK(!overrun::nearMiss(""));
  CHECK(!overrun::nearMiss(std::string(20, '0')));
  CHECK(!overrun::nearMiss(std::string(300, '0')));
  CHECK(overrun::nearMiss(std::string(19, '0') + "x" + std::string(5, '0')));
  CHECK(!overrun::nearMiss(std::string(19, '0') + "x" + std::string(20, '0')));   // the overrun wins
  // Line breaks do not end a run: three lines of zeros join.
  CHECK(overrun::inBody(std::string(8, '0') + "\n" + std::string(8, '0') + "\n" + std::string(4, '0')));
  CHECK(overrun::inBody(std::string(10, '0') + "\r\n" + std::string(10, '0')));
  CHECK(!overrun::inBody(std::string(8, '0') + "\n" + std::string(8, '0') + "\n" + std::string(3, '0')));
  CHECK(overrun::nearMiss(std::string(8, '0') + "\n" + std::string(8, '0') + "\n" + std::string(3, '0')));
  // One different character in the middle breaks the run: a letter O, a space, a one.
  CHECK(!overrun::inBody(std::string(10, '0') + "O" + std::string(10, '0')));
  CHECK(!overrun::inBody(std::string(10, '0') + " " + std::string(10, '0')));
  CHECK(!overrun::inBody(std::string(10, '0') + "1" + std::string(19, '0') + "1"));
  // Two short runs add up to nothing.
  CHECK(!overrun::inBody(std::string(15, '0') + "x" + std::string(15, '0')));
  CHECK(!overrun::nearMiss(std::string(15, '0') + "x" + std::string(15, '0')));
  if (failures) { printf("%d failed\n", failures); return 1; }
  printf("all passed\n");
  return 0;
}
