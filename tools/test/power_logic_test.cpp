// Host-side test of src/core/power_logic.h. No board, no Arduino types.
//   zig c++ -std=c++17 -Wall -Wextra -I src/core tools/test/power_logic_test.cpp -o power_logic_test
//   (or g++/clang++ with the same flags), then run ./power_logic_test
#include "power_logic.h"
#include <stdio.h>

static int failures = 0;
#define CHECK(cond) do { if (!(cond)) { printf("FAIL line %d: %s\n", __LINE__, #cond); failures++; } } while (0)

int main() {
  // The default is two minutes; odd settings are put right.
  {
    PowerTiming t;
    CHECK(powerTimerMs(t) == 120000);
    t.timerSec = 0;
    CHECK(powerTimerMs(t) == 1000);
    t.timerSec = 5;
    CHECK(powerTimerMs(t) == 5000);
    t.timerSec = 4000000000u;
    CHECK(powerTimerMs(t) == 3600000);
  }
  // The deadline: not due before, due at and after it.
  {
    uint32_t at = 1000 + 120000;
    CHECK(!powerTimerDue(1000, at));
    CHECK(!powerTimerDue(at - 1, at));
    CHECK(powerTimerDue(at, at));
    CHECK(powerTimerDue(at + 5000, at));
  }
  // millis() wraps after 49 days: a deadline set before the wrap still works.
  {
    uint32_t start = 0xFFFFFFFFu - 1000;
    PowerTiming t;
    uint32_t at = start + powerTimerMs(t);  // wraps past zero
    CHECK(!powerTimerDue(start + 60000UL, at));
    CHECK(powerTimerDue(at, at));
    CHECK(powerTimerDue(at + 1, at));
  }
  printf(failures ? "%d FAILED\n" : "all passed\n", failures);
  return failures ? 1 : 0;
}
