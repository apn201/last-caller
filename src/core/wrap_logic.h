#pragma once
// Word wrap for message text, as pure logic: one line in, screen rows out.
// A row breaks at the last space that fits, so a word is never cut in two; a
// word longer than the row is cut at the edge. Columns count UTF-8 characters,
// and an ANSI colour sequence (ESC [ ... letter) takes none. No String, no SD
// card: the board's bodyRows() wraps this. A plain C++ compiler tests it
// (tools/test/wrap_logic_test.cpp).
// Part of the shared core: included by bbs/messages.h.
#include <string>
#include <vector>

namespace wrap {

inline std::vector<std::string> rows(const std::string &line, size_t width = 79) {
  std::vector<std::string> out;
  std::string cur;
  size_t cols = 0, colsAtSp = 0;
  long lastSp = -1;                 // byte index in cur of the last space that follows a word
  bool afterWord = false;
  for (size_t i = 0; i < line.size();) {
    unsigned char c = (unsigned char)line[i];
    if (c == 27 && i + 1 < line.size() && line[i + 1] == '[') {   // a colour code: no width
      size_t j = i + 2;
      while (j < line.size() && !((line[j] >= 'A' && line[j] <= 'Z') || (line[j] >= 'a' && line[j] <= 'z'))) j++;
      if (j < line.size()) j++;
      cur.append(line, i, j - i);
      i = j;
      continue;
    }
    size_t len = 1;
    if (c >= 0xf0) len = 4; else if (c >= 0xe0) len = 3; else if (c >= 0xc0) len = 2;
    if (i + len > line.size()) len = line.size() - i;
    if (c == ' ') {
      if (cols >= width) { out.push_back(cur); cur.clear(); cols = 0; lastSp = -1; afterWord = false; i++; continue; }
      if (afterWord) { lastSp = (long)cur.size(); colsAtSp = cols; }
      cur += ' ';
      cols++;
      afterWord = false;
      i++;
      continue;
    }
    if (cols >= width) {
      if (lastSp >= 0) {
        out.push_back(cur.substr(0, (size_t)lastSp));
        cur = cur.substr((size_t)lastSp + 1);
        cols = cols - colsAtSp - 1;
      } else {
        out.push_back(cur); cur.clear(); cols = 0;
      }
      lastSp = -1;
    }
    cur.append(line, i, len);
    cols++;
    afterWord = true;
    i += len;
  }
  out.push_back(cur);
  return out;
}

}  // namespace wrap
