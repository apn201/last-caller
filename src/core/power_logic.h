#pragma once
// The ship's power, as pure logic: no String, no SD card, no sessions. The
// board wraps it in core/power.h. Keeping it free of platform types lets a
// plain C++ compiler test it (tools/test/power_logic_test.cpp).
// Part of the shared core: included in order from src/bbs_core.h.
#include <stdint.h>

struct PowerTiming {
  uint32_t windowMinSec = 60, windowMaxSec = 120;  // brownout: how long a session lasts
  uint32_t decayAfterIdleMin = 15;                 // powered: minutes after the last caller leaves; 0 = never
};

// A random session window in milliseconds, always between the INI bounds
// (swapped bounds are put right, a zero window is raised to one second).
static inline uint32_t powerWindowMs(const PowerTiming &t, uint32_t random) {
  uint32_t lo = t.windowMinSec < t.windowMaxSec ? t.windowMinSec : t.windowMaxSec;
  uint32_t hi = t.windowMinSec < t.windowMaxSec ? t.windowMaxSec : t.windowMinSec;
  if (hi < 1) hi = 1;
  if (lo < 1) lo = 1;
  return (lo + random % (hi - lo + 1)) * 1000UL;
}

// BROWNOUT or POWERED, and the timer that takes POWERED back to BROWNOUT.
// `now` is millis(); the subtraction is wrap-safe.
struct PowerMachine {
  bool powered = false;
  bool counting = false;   // the idle timer is running (powered, nobody connected)
  uint32_t idleSince = 0;

  void set(bool on) { powered = on; counting = false; }
  // Call often. `sessions` = connected sessions of any kind. Returns true on
  // the tick the board falls back to brownout.
  bool tick(uint32_t now, uint32_t sessions, const PowerTiming &t) {
    if (!powered || sessions) { counting = false; return false; }
    if (!counting) { counting = true; idleSince = now; }
    if (!t.decayAfterIdleMin) return false;
    if (now - idleSince < t.decayAfterIdleMin * 60000UL) return false;
    powered = false; counting = false;
    return true;
  }
};
