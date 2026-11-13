#pragma once
// The overrun (milestone M5): a message body that holds a run of 255 zero
// characters. Ty found it in week one by holding a key down; it is the one
// place ELIZA cannot watch. Pure logic: no String, no card, no sessions. The
// board's post path (bbs/messages.h) calls it. A plain C++ compiler tests it
// (tools/test/overrun_logic_test.cpp).
// Part of the shared core: included by bbs/messages.h.
#include <string>

namespace overrun {

static constexpr size_t RUN = 255;   // the number is printed in the board's manual and in two posts

// Line breaks are ignored, so zeros typed on several lines join up. Any other
// character (a letter O, a space, a 1) ends the run.
inline bool inBody(const std::string &body) {
  size_t run = 0;
  for (char c : body) {
    if (c == '\r' || c == '\n') continue;
    if (c == '0') { if (++run >= RUN) return true; }
    else run = 0;
  }
  return false;
}

}  // namespace overrun
