#pragma once
// Modem commands, as pure logic: what a line typed by a caller means to a 1998
// modem. No String, no SD card, no sessions: the board wraps this in
// bbs/eggs.h. A plain C++ compiler tests it (tools/test/egg_logic_test.cpp).
// Part of the shared core: included by core/session.h.
#include <stdint.h>
#include <string>

namespace egg {

enum Kind : uint8_t {
  NONE,      // not a modem command: handle the line as before
  OK,        // AT, +++, ATZ, ATE0, AT&F ...
  ERR,       // any other AT...
  INFO,      // ATI, ATI0 .. ATI9: `info` says which
  HANGUP,    // ATH, ATH0, ATH1
  DIAL       // ATD, ATDT, ATDP: `digits` is the number, maybe empty
};

struct Modem {
  Kind kind = NONE;
  int info = 0;
  std::string digits;
};

inline std::string upperTrim(const std::string &raw) {
  size_t a = 0, b = raw.size();
  while (a < b && (raw[a] == ' ' || raw[a] == '\t')) a++;
  while (b > a && (raw[b - 1] == ' ' || raw[b - 1] == '\t' || raw[b - 1] == '\r' || raw[b - 1] == '\n')) b--;
  std::string out = raw.substr(a, b - a);
  for (char &c : out) if (c >= 'a' && c <= 'z') c = char(c - 'a' + 'A');
  return out;
}

// The digits of a phone number: separators and dial modifiers dropped. At most 20.
inline std::string digitsOf(const std::string &s) {
  std::string out;
  for (char c : s) if (c >= '0' && c <= '9' && out.size() < 20) out += c;
  return out;
}

inline Modem parse(const std::string &raw) {
  Modem m;
  const std::string s = upperTrim(raw);
  if (s == "+++") { m.kind = OK; return m; }
  if (s.size() < 2 || s[0] != 'A' || s[1] != 'T') return m;
  const std::string r = s.substr(2);
  if (r.empty()) { m.kind = OK; return m; }
  const char c = r[0];
  const bool lone = r.size() == 1 || (r.size() == 2 && r[1] >= '0' && r[1] <= '9');
  if (c == 'H' && lone) { m.kind = HANGUP; return m; }
  if (c == 'I' && lone) { m.kind = INFO; m.info = r.size() == 2 ? r[1] - '0' : 0; return m; }
  if (c == 'D') {
    // ATD, ATDT, ATDP, then a number; spaces, dashes, commas and brackets are allowed.
    for (size_t i = 1; i < r.size(); ++i) {
      char d = r[i];
      bool ok = (d >= '0' && d <= '9') || d == ' ' || d == '-' || d == ',' || d == '(' || d == ')' ||
                (i == 1 && (d == 'T' || d == 'P'));
      if (!ok) { m.kind = ERR; return m; }
    }
    m.kind = DIAL;
    m.digits = digitsOf(r.substr(1));
    return m;
  }
  // Setup commands: reset, echo, verbose, speaker, volume, result codes (a digit or two),
  // S-registers (S0=1), and & commands (&F).
  auto digits = [&](size_t from) { for (size_t i = from; i < r.size(); ++i) if (r[i] < '0' || r[i] > '9') return false; return r.size() - from <= 2; };
  if (std::string("ZEVMLXQF").find(c) != std::string::npos && digits(1)) { m.kind = OK; return m; }
  if (c == 'S' && r.size() > 1) {
    for (size_t i = 1; i < r.size(); ++i) if (!((r[i] >= '0' && r[i] <= '9') || r[i] == '=' || r[i] == '?')) { m.kind = ERR; return m; }
    m.kind = OK; return m;
  }
  if (c == '&' && r.size() >= 2 && r[1] >= 'A' && r[1] <= 'Z' && digits(2)) { m.kind = OK; return m; }
  m.kind = ERR;
  return m;
}

}  // namespace egg
