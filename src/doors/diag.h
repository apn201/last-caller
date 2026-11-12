#pragma once
// DIAG: ELIZA's diagnostics door. The caller types a query code from the comms
// manual and she answers it, truthfully, in her voice. Codes, answers, the
// flags a query needs or grants and the door's own lines are in
// STORY/DIAG.INI (read each time the door opens); the screen is
// ANSI/DOORS/DIAG.ANS. The door works only while the ship has power (she is
// awake), and the same on Telnet and in the browser.
// Part of the single TinyBBS translation unit: included in order from main.cpp.

static constexpr const char *DIAG_INI = "/BBS/STORY/DIAG.INI";
static Ini diagIni;

// The caller's flags for a door: the own account's, or what a crew login carries.
// (A const twin of scene.h's sceneFlagsOf, for the door table's `listed`.)
static std::string doorFlagsOf(const Session &s) {
  User own;
  return hauntOwner(s, own) ? hauntStd(own.flags) : hauntStd(s.carriedFlags);
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
static String diagLines(String text) { text.replace("|", "\r\n"); return text; }
static void diagSay(Session &s, const String &text) { if (text.length()) say(s, "\x1b[0;33m" + diagLines(text) + "\x1b[0m"); }
static void diagPrompt(Session &s) { prompt(s, "\x1b[1;36m" + iniGet(diagIni, "DIAG", "prompt", "QUERY>") + " \x1b[0m"); }

static bool ownsDiag(const Session &s) { return s.state == DIAG_PROMPT; }
// In the door menu only while the ship has power, never for the maintenance login.
static bool diagListed(const Session &s) {
  if (isMaintenance(s.user)) return false;
  return haunt::flagsHasAll(doorFlagsOf(s), haunt::flagsClean(hauntStd(config(DIAG_INI, "DIAG", "needs", ""))));
}

static void enterDiag(Session &s) {
  iniLoad(DIAG_INI, diagIni);
  s.state = DIAG_PROMPT; s.hideInput = false; activity(s, "DIAGNOSTICS");
  event("DIAG", s.user.handle + " node " + s.node + " opened diagnostics");
  if (s.ansi) art(s, "/BBS/ANSI/DOORS/DIAG.ANS", iniGet(diagIni, "DIAG", "title", "DIAGNOSTICS"));
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
  String code = diagCode(line);
  if (!code.length()) { diagSay(s, iniGet(diagIni, "DIAG", "nocode", "Please enter a query code.")); diagPrompt(s); return; }
  bool found = code != "DIAG";
  if (found) { found = false; for (const String &sec : iniSections(diagIni)) if (sec == code) { found = true; break; } }
  if (!found) { diagSay(s, iniGet(diagIni, "DIAG", "unknown", "No such query code.")); diagPrompt(s); return; }
  std::string need = haunt::flagsClean(hauntStd(iniGet(diagIni, code, "has")));
  if (!haunt::flagsHasAll(doorFlagsOf(s), need)) { diagSay(s, iniGet(diagIni, "DIAG", "denied", "That query is not open to you.")); diagPrompt(s); return; }
  // Flags are saved before the answer is shown; if the save fails, nothing is given.
  std::string give = haunt::flagsClean(hauntStd(iniGet(diagIni, code, "set")));
  if (give.length() && !sceneGrant(s, give)) {
    event("DIAG_FAIL", s.user.handle + " query " + code + ": flags not saved");
    diagSay(s, "I cannot answer that right now.");
    diagPrompt(s);
    return;
  }
  event("DIAG", s.user.handle + " node " + s.node + " query " + code);
  diagSay(s, iniGet(diagIni, code, "text"));
  diagPrompt(s);
}
