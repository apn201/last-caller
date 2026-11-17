#pragma once
// Easter eggs that are typed at a prompt: modem commands (AT..., +++) and a few
// hidden main-menu words. They write nothing: no flag, no record, no timer. A
// line is looked at here first, only at the login, main, door menu and "Press
// ENTER" prompts; if it is not an egg the normal commands see it unchanged.
// The text is in STORY/EGGS.INI and the art files; a card without them
// understands OK and NO CARRIER and nothing else.
// Part of the shared core: included in order from src/bbs_core.h, before bbs/commands.h.

static constexpr const char *EGGS_INI = "/BBS/STORY/EGGS.INI";

// "line one|line two" -> two lines.
static String eggLines(String text) { text.replace("|", "\r\n"); return text; }

static String bulletinPrompt(const Session &s);   // bbs/cmd_main.h
static bool eggPromptState(const Session &s) {
  return s.state == LOGIN_CHOICE || s.state == LOGIN_HANDLE || s.state == MAIN || s.state == DOOR_MENU || s.state == BULLETIN;
}
// The prompt the caller was at, drawn again.
static void eggPrompt(Session &s) {
  switch (s.state) {
    case LOGIN_CHOICE: prompt(s, "(N)ew user, (L)ogin, or (G)oodbye? "); break;
    case LOGIN_HANDLE: prompt(s, powerEnabled ? "Login: " : "Handle: "); break;
    case DOOR_MENU: prompt(s, "Door command? "); break;
    case BULLETIN: prompt(s, "Press any key: "); break;
    // Q20 UX: the other calm prompts a page may follow (core/haunt.h hauntShow).
    case MSG_MENU: prompt(s, "Message command? "); break;
    case FILE_MENU: prompt(s, "File area #, then ENTER: "); break;
    case FILE_LIST: prompt(s, "File command? "); break;
    case ANSI_GALLERY: prompt(s, "Gallery command? "); break;
    case BULLETIN_LIST: prompt(s, bulletinPrompt(s)); break;
    case JOIN_CONF: prompt(s, "Area #, then ENTER (Q goes back at once): "); break;
    default: prompt(s, "Main Board Command? "); break;
  }
}
// ART in ANSI/DOORS, or its twin in ASCII/DOORS for a caller without ANSI.
static bool eggArt(Session &s, const String &name) {
  if (!sdReady || !name.length() || name.length() > 16) return false;
  for (size_t i = 0; i < name.length(); ++i) if (!(isalnum((unsigned char)name[i]) || name[i] == '_')) return false;
  String path = (s.ansi ? "/BBS/ANSI/DOORS/" : "/BBS/ASCII/DOORS/") + name + (s.ansi ? ".ANS" : ".ASC");
  if (!SD_MMC.exists(path.c_str())) return false;
  if (s.ansi) art(s, path.c_str()); else plainArt(s, path.c_str());
  return true;
}

// A number the caller dialled: the first [NODE_*] section with that number.
static void eggDial(Session &s, const Ini &ini, const std::string &digits) {
  if (!digits.length()) { say(s, iniGet(ini, "MODEM", "nodialtone", "NO DIALTONE")); return; }
  String shown = String(digits.c_str());
  for (const String &sec : iniSections(ini)) {
    if (!sec.startsWith("NODE_")) continue;
    if (egg::digitsOf(std::string(iniGet(ini, sec, "number").c_str())) != digits) continue;
    event("EGG_DIAL", (s.user.handle.length() ? s.user.handle : String("anonymous")) + " node " + s.node + " dialled " + sec);
    say(s, "DIALING " + shown);
    soundCue(s, "MODEM");
    say(s, "\x1b[0;36m" + iniGet(ini, "MODEM", "connect", "CONNECT 2400") + "\x1b[0m");
    eggArt(s, iniGet(ini, sec, "art"));
    String text = iniGet(ini, sec, "text");
    if (text.length()) say(s, eggLines(text));
    say(s, iniGet(ini, "MODEM", "nocarrier", "NO CARRIER"));
    return;
  }
  say(s, "DIALING " + shown);
  soundCue(s, "MODEM");
  // An odd last digit: the line is busy; an even one: nobody picks up.
  say(s, (digits.back() - '0') % 2 ? iniGet(ini, "MODEM", "busy", "BUSY") : iniGet(ini, "MODEM", "noanswer", "NO ANSWER"));
}

static bool eggModem(Session &s, const String &line) {
  egg::Modem m = egg::parse(std::string(line.c_str()));
  if (m.kind == egg::NONE) return false;
  const bool login = s.state == LOGIN_CHOICE || s.state == LOGIN_HANDLE;
  // A word that only starts with AT may be a caller's handle: at the login prompt
  // only the commands count, and never a name that is on the board.
  if (login) {
    User u;
    if (m.kind == egg::ERR || findUser(up(line), u)) return false;
  }
  Ini ini;
  const bool have = iniLoad(EGGS_INI, ini);
  if (m.kind == egg::HANGUP) {
    say(s, iniGet(ini, "MODEM", "nocarrier", "NO CARRIER"));
    event("EGG_HANGUP", (s.user.handle.length() ? s.user.handle : String("anonymous")) + " node " + s.node + " hung up with ATH");
    s.state = GOODBYE;
    return true;
  }
  if (m.kind == egg::OK) say(s, iniGet(ini, "MODEM", "ok", "OK"));
  else if (!have || m.kind == egg::ERR) say(s, iniGet(ini, "MODEM", "error", "ERROR"));
  else if (m.kind == egg::INFO) {
    String id = iniGet(ini, "MODEM", (String("id") + String((int)m.info)).c_str());
    if (!id.length()) say(s, iniGet(ini, "MODEM", "error", "ERROR"));
    else { say(s, eggLines(id)); say(s, iniGet(ini, "MODEM", "ok", "OK")); }
  }
  else eggDial(s, ini, m.digits);
  eggPrompt(s);
  return true;
}

// Words nobody lists, at the main menu only.
static bool eggHidden(Session &s, const String &cmd) {
  if (s.state != MAIN) return false;
  const bool post = cmd == "POST" || cmd == "REBOOT";
  const bool fortune = cmd == "FORTUNE" || cmd == "/FORTUNE";
  if (!post && !fortune && cmd != "XYZZY") return false;
  Ini ini;
  if (!iniLoad(EGGS_INI, ini)) return false;
  if (cmd == "XYZZY") {
    String text = iniGet(ini, "HIDDEN", "xyzzy");
    if (!text.length()) return false;
    say(s, eggLines(text));
  } else if (fortune) {
    std::vector<String> all;
    for (const auto &e : ini) if (e.section == "FORTUNES" && e.value.length()) all.push_back(e.value);
    if (all.empty()) return false;
    say(s, "\x1b[0;33m" + eggLines(all[bbsRandom() % all.size()]) + "\x1b[0m");
  } else {
    if (!eggArt(s, "POST")) return false;
    event("EGG_POST", s.user.handle + " node " + s.node + " rebooted the 486");
  }
  eggPrompt(s);
  return true;
}

// From handleLine, before the normal commands.
static bool eggCommand(Session &s, const String &line, const String &cmd) {
  if (!eggPromptState(s)) return false;
  return eggModem(s, line) || eggHidden(s, cmd);
}
