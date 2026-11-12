#pragma once
// The prologue's power timer, as pure logic: no String, no SD card, no sessions.
// The board wraps it in core/power.h. Keeping it free of platform types lets a
// plain C++ compiler test it (tools/test/power_logic_test.cpp).
// Part of the shared core: included in order from src/bbs_core.h.
#include <stdint.h>

struct PowerTiming {
  uint32_t timerSec = 120;  // how long a maintenance login has to switch to the RTG
};

// The timer length in milliseconds. Never zero, and short enough to fit.
static inline uint32_t powerTimerMs(const PowerTiming &t) {
  uint32_t sec = t.timerSec < 1 ? 1 : (t.timerSec > 3600 ? 3600 : t.timerSec);
  return sec * 1000UL;
}

// True once `now` has reached `deadline`; the subtraction is wrap-safe.
static inline bool powerTimerDue(uint32_t now, uint32_t deadline) {
  return (int32_t)(now - deadline) >= 0;
}
