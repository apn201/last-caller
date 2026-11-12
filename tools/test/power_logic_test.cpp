// Host-side test of src/core/power_logic.h. No board, no Arduino types.
//   zig c++ -std=c++17 -Wall -Wextra -I src/core tools/test/power_logic_test.cpp -o power_logic_test
//   (or g++/clang++ with the same flags), then run ./power_logic_test
#include "power_logic.h"
#include <stdio.h>
#include <stdlib.h>

static int failures = 0;
#define CHECK(cond) do { if (!(cond)) { printf("FAIL line %d: %s\n", __LINE__, #cond); failures++; } } while (0)

static uint32_t lcg = 12345;
static uint32_t rnd() { lcg = lcg * 1664525u + 1013904223u; return lcg >> 8 | lcg << 24; }

int main() {
  // The window is always inside the bounds, for the default and odd settings.
  {
    PowerTiming t;
    uint32_t lo = 0xFFFFFFFF, hi = 0;
    for (int i = 0; i < 200000; ++i) {
      uint32_t w = powerWindowMs(t, rnd());
      CHECK(w >= 60000 && w <= 120000 && w % 1000 == 0);
      if (w < lo) lo = w;
      if (w > hi) hi = w;
    }
    CHECK(lo <= 61000 && hi >= 119000);  // the whole range is used
    t.windowMinSec = 90; t.windowMaxSec = 90;
    CHECK(powerWindowMs(t, 7) == 90000);
    t.windowMinSec = 120; t.windowMaxSec = 60;  // swapped
    for (int i = 0; i < 1000; ++i) { uint32_t w = powerWindowMs(t, rnd()); CHECK(w >= 60000 && w <= 120000); }
    t.windowMinSec = 0; t.windowMaxSec = 0;
    CHECK(powerWindowMs(t, 99) == 1000);
    t.windowMinSec = 5; t.windowMaxSec = 15;
    for (int i = 0; i < 1000; ++i) { uint32_t w = powerWindowMs(t, rnd()); CHECK(w >= 5000 && w <= 15000); }
    t.windowMinSec = 0; t.windowMaxSec = 4294967u;  // the largest span that fits in milliseconds
    for (int i = 0; i < 1000; ++i) CHECK(powerWindowMs(t, rnd()) >= 1000);
  }
  // Brownout never decays or counts.
  {
    PowerMachine m; PowerTiming t;
    CHECK(!m.tick(0, 0, t) && !m.counting && !m.powered);
    CHECK(!m.tick(99999999, 0, t) && !m.counting);
  }
  // Powered with someone connected: no timer. The last caller leaves: 15 minutes.
  {
    PowerMachine m; PowerTiming t;
    m.set(true);
    CHECK(!m.tick(1000, 2, t) && !m.counting);
    CHECK(!m.tick(2000, 1, t) && !m.counting);
    CHECK(!m.tick(3000, 0, t) && m.counting && m.idleSince == 3000);
    CHECK(!m.tick(3000 + 15 * 60000UL - 1, 0, t) && m.powered);
    CHECK(m.tick(3000 + 15 * 60000UL, 0, t) && !m.powered && !m.counting);
    CHECK(!m.tick(3000 + 16 * 60000UL, 0, t));  // already brownout: no repeat
  }
  // A connection during the grace stops the timer; it starts again from zero.
  {
    PowerMachine m; PowerTiming t;
    m.set(true);
    m.tick(0, 0, t);
    CHECK(!m.tick(14 * 60000UL, 0, t) && m.powered);
    CHECK(!m.tick(14 * 60000UL + 1, 1, t) && !m.counting);
    CHECK(!m.tick(20 * 60000UL, 0, t) && m.counting && m.idleSince == 20 * 60000UL);
    CHECK(!m.tick(34 * 60000UL, 0, t) && m.powered);
    CHECK(m.tick(35 * 60000UL, 0, t) && !m.powered);
  }
  // decay_after_idle_min = 0: power never falls back.
  {
    PowerMachine m; PowerTiming t; t.decayAfterIdleMin = 0;
    m.set(true);
    for (uint32_t now = 0; now < 4000000000u; now += 100000000u) CHECK(!m.tick(now, 0, t) && m.powered);
  }
  // A short test setting (1 minute), the one TESTING.md uses.
  {
    PowerMachine m; PowerTiming t; t.decayAfterIdleMin = 1;
    m.set(true);
    m.tick(500, 0, t);
    CHECK(!m.tick(500 + 59999, 0, t));
    CHECK(m.tick(500 + 60000, 0, t));
  }
  // millis() wraps after 49 days: the idle timer survives it.
  {
    PowerMachine m; PowerTiming t;
    m.set(true);
    uint32_t start = 0xFFFFFFFFu - 1000;
    m.tick(start, 0, t);
    CHECK(!m.tick(start + 5 * 60000UL, 0, t));  // wrapped past zero
    CHECK(m.tick(start + 15 * 60000UL, 0, t));
  }
  // A SysOp force: either way at once, and the timer restarts.
  {
    PowerMachine m; PowerTiming t;
    m.set(true); m.tick(0, 0, t);
    m.set(false);
    CHECK(!m.powered && !m.counting);
    m.set(true);
    CHECK(m.powered && !m.counting);
    CHECK(!m.tick(10 * 60000UL, 0, t) && m.idleSince == 10 * 60000UL);
  }
  printf(failures ? "%d FAILED\n" : "all passed\n", failures);
  return failures ? 1 : 0;
}
