#pragma once
// G1: the board never gives the same answer twice in a row. A door or the login keeps
// what it answered last (Session.repKey) and how many times in a row (Session.repCount).
// The card writes the other answers: text2= and text3= beside text=, or one "again" line
// that goes in front of the first answer. Part of the shared core: included from
// src/bbs_core.h after core/session.h.

// Count this answer. Another key starts again at 0; the same key as last time counts up.
static unsigned repeatCount(Session &s, const String &key) {
  if (s.repKey == key) { if (s.repCount < 60000) s.repCount++; }
  else { s.repKey = key; s.repCount = 0; }
  return s.repCount;
}
static void repeatReset(Session &s) { s.repKey = ""; s.repCount = 0; }

// The answer for the n-th time in a row. 0 the text; then text2, text3, the text again, ...
// A missing variant is the text with `again2` (or `again3`) as its first line. "|" breaks lines.
static String repeatAnswer(unsigned n, const String &text, const String &text2, const String &text3,
                           const String &again2, const String &again3) {
  unsigned at = n % 3;
  if (at == 0) return text;
  const String &variant = at == 1 ? text2 : text3;
  if (variant.length()) return variant;
  const String &pre = at == 1 ? again2 : again3;
  return pre.length() ? pre + "|" + text : text;
}
