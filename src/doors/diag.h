#pragma once
// DIAG: ELIZA's diagnostics door. The caller types a query code from the comms
// manual and she answers it, truthfully, in her voice. Codes, answers, the
// flags a query needs or grants and the door's own lines are in
// STORY/DIAG.INI (read each time the door opens); the screen is
// ANSI/DOORS/DIAG.ANS. The door works only while the ship has power (she is
// awake), and the same on Telnet and in the browser.
// Part of the single TinyBBS translation unit: included in order from main.cpp.
#include "../core/wrap_logic.h"

static constexpr const char *DIAG_INI = "/BBS/STORY/DIAG.INI";
static Ini diagIni;

// The caller's flags for a door: the own account's, or what a crew login carries.
// (A const twin of scene.h's sceneFlagsOf, for the door table's `listed`.)
static std::string doorFlagsOf(const Session &s) {
  User own;
  return hauntOwner(s, own) ? userFlagList(own) : hauntStd(s.carriedFlags);
}

// "q 2-00" -> "Q200": upper case, no spaces or dashes.
static String diagCode(const String &typed) {
  String out;
  for (size_t i = 0; i < typed.length(); ++i) {
    char c = typed[i];
    if (c == ' ' || c == '\t' || c == '-') continue;
    out += (char)toupper((unsigned char)c);
  }
  return out;
}
// "|" breaks a line; a line longer than a screen wraps on a word (core/wrap_logic.h).
static String diagLines(const String &text) {
  String out;
  size_t start = 0;
  while (start <= text.length()) {
    int bar = text.indexOf('|', start);
    size_t end = bar < 0 ? text.length() : (size_t)bar;
    for (const std::string &row : wrap::rows(std::string(text.substring(start, end).c_str()), 79)) {
      if (out.length()) out += "\r\n";
      out += String(row.c_str());
    }
    start = end + 1;
  }
  return out;
}
static void diagSay(Session &s, const String &text) { if (text.length()) say(s, "\x1b[0;33m" + diagLines(text) + "\x1b[0m"); }
// G1: the n-th answer in a row to the same thing differs (core/repeat.h). `k` is the key in the section
// (text, unknown, denied, nocode, help); k2 and k3 beside it are the variants, DIAG again2/again3 the fallback.
static String diagAnswer(Session &s, const String &key, const String &sec, const String &k, const String &def = String()) {
  unsigned n = repeatCount(s, "DIAG:" + key);
  return repeatAnswer(n, iniGet(diagIni, sec, k.c_str(), def), iniGet(diagIni, sec, (k + "2").c_str()), iniGet(diagIni, sec, (k + "3").c_str()),
                      iniGet(diagIni, "DIAG", "again2"), iniGet(diagIni, "DIAG", "again3"));
}
static void diagPrompt(Session &s) { prompt(s, "\x1b[1;36m" + iniGet(diagIni, "DIAG", "prompt", "QUERY>") + " \x1b[0m"); }

static bool ownsDiag(const Session &s) { return s.state == DIAG_PROMPT; }
// In the door menu only while the ship has power, never for the maintenance login.
static bool diagListed(const Session &s) {
  if (isMaintenance(s.user)) return false;
  return haunt::flagsHasAll(doorFlagsOf(s), haunt::flagsClean(hauntStd(config(DIAG_INI, "DIAG", "needs", ""))));
}

static void enterDiag(Session &s) {
  iniLoad(DIAG_INI, diagIni);
  s.state = DIAG_PROMPT; s.hideInput = false; activity(s, "DIAGNOSTICS"); repeatReset(s);
  event("DIAG", s.user.handle + " node " + s.node + " opened diagnostics");
  if (s.ansi) artOnce(s, "/BBS/ANSI/DOORS/DIAG.ANS", iniGet(diagIni, "DIAG", "title", "DIAGNOSTICS"));
  else plainArt(s, "/BBS/ASCII/DOORS/DIAG.ASC", iniGet(diagIni, "DIAG", "title", "DIAGNOSTICS"));
  diagSay(s, iniGet(diagIni, "DIAG", "intro", "Enter a query code, or QUIT."));
  diagPrompt(s);
}

static void handleDiag(Session &s, String &line, const String &cmd) {
  if (cmd == "QUIT" || cmd == "Q" || cmd == "EXIT" || cmd == "BYE") {
    event("DIAG", s.user.handle + " node " + s.node + " left diagnostics");
    doorMenu(s);
    return;
  }
  if (cmd == "X") {   // Q20 UX: X is the main menu from every screen, a line door too
    event("DIAG", s.user.handle + " node " + s.node + " left diagnostics");
    mainMenu(s);
    return;
  }
  if (cmd == "HELP" || cmd == "?") {
    if (iniGet(diagIni, "DIAG", "help", "").length()) { diagSay(s, diagAnswer(s, "HELP", "DIAG", "help")); notesOn(s, notes::K_DIAG, "HELP"); diagPrompt(s); return; }
  }
  String code = diagCode(line);
  if (!code.length()) { diagSay(s, diagAnswer(s, "NOCODE", "DIAG", "nocode", "Please enter a query code.")); diagPrompt(s); return; }
  bool found = code != "DIAG";
  if (found) { found = false; for (const String &sec : iniSections(diagIni)) if (sec == code) { found = true; break; } }
  if (!found) { diagSay(s, diagAnswer(s, "UNKNOWN", "DIAG", "unknown", "No such query code.")); diagPrompt(s); return; }
  std::string need = haunt::flagsClean(hauntStd(iniGet(diagIni, code, "has")));
  if (!haunt::flagsHasAll(doorFlagsOf(s), need)) { diagSay(s, diagAnswer(s, "DENIED", "DIAG", "denied", "That query is not open to you.")); diagPrompt(s); return; }
  // Flags are saved before the answer is shown; if the save fails, nothing is given.
  std::string give = haunt::flagsClean(hauntStd(iniGet(diagIni, code, "set")));
  if (give.length() && !sceneGrant(s, give)) {
    event("DIAG_FAIL", s.user.handle + " query " + code + ": flags not saved");
    diagSay(s, "I cannot answer that right now.");
    diagPrompt(s);
    return;
  }
  event("DIAG", s.user.handle + " node " + s.node + " query " + code);
  diagSay(s, diagAnswer(s, code, code, "text"));
  notesOn(s, notes::K_DIAG, code);   // Q21: the answer is in the notebook
  diagPrompt(s);
}
