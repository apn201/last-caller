#pragma once
// SHIP: the ship systems console. Only the maintenance account reaches it, and
// it is entered at login, not from the door menu. Commands, answers and the
// wake-up lines are in STORY/SHIP.INI; the screens are ANSI/POWER/*.ANS.
// The one command that matters is the power source: SOURCE RTG, then the key
// the warning names. That switches this caller's session to the RTG, stops its
// power timer and adds one option: CREW, to make a crew account.
// Part of the single TinyBBS translation unit: included in order from main.cpp.

static constexpr const char *SHIP_INI = "/BBS/STORY/SHIP.INI";
static Ini shipIni;  // read each time the console opens

static String shipNorm(String t) {
  t.trim(); t.toUpperCase();
  String out; bool gap = false;
  for (size_t i = 0; i < t.length(); ++i) {
    char c = t[i];
    if (c == ' ' || c == '\t') gap = true;
    else { if (gap && out.length()) out += ' '; gap = false; out += c; }
  }
  return out;
}
// The INI section that answers this typed command: its name or an alias.
static bool shipFind(const String &typed, String &section) {
  String want = shipNorm(typed);
  if (!want.length()) return false;
  for (const String &sec : iniSections(shipIni)) {
    if (sec == "SHIP") continue;
    if (shipNorm(sec) == want) { section = sec; return true; }
    String aliases = iniGet(shipIni, sec, "aliases");
    for (int start = 0; start <= (int)aliases.length();) {
      int comma = aliases.indexOf(',', start); if (comma < 0) comma = aliases.length();
      if (shipNorm(aliases.substring(start, comma)) == want) { section = sec; return true; }
      start = comma + 1;
    }
  }
  return false;
}
static String shipLines(String text) { text.replace("|", "\r\n"); return text; }
static void shipSay(Session &s, const String &text) { if (text.length()) say(s, "\x1b[0;37m" + shipLines(text) + "\x1b[0m"); }
static void shipPrompt(Session &s) { prompt(s, "\x1b[1;36m" + iniGet(shipIni, "SHIP", "prompt", "SHIP>") + " \x1b[0m"); }

static void enterShip(Session &s) {
  iniLoad(SHIP_INI, shipIni);
  s.state = SHIP_CONSOLE; s.hideInput = false; activity(s, "SHIP SYSTEMS");
  event("SHIP", s.user.handle + " node " + s.node + " opened the ship console");
  powerScreen(s, "CONSOLE", iniGet(shipIni, "SHIP", "title", "SHIP SYSTEMS"));
  shipPrompt(s);
}
static bool ownsShip(const Session &s) { return s.state == SHIP_CONSOLE || s.state == SHIP_CONFIRM; }
static bool shipListed(const Session &) { return false; }  // not in the door menu

static void shipLeave(Session &s) {
  event("SHIP", s.user.handle + " node " + s.node + " left the ship console");
  say(s);
  logoutToLogin(s);
}
static void shipHelp(Session &s) {
  say(s, "\x1b[0;37m" + iniGet(shipIni, "SHIP", "help_title", "COMMANDS") + "\x1b[0m");
  for (const String &sec : iniSections(shipIni)) {
    String help = iniGet(shipIni, sec, "help");
    if (sec == "SHIP" || !help.length()) continue;
    if (up(iniGet(shipIni, sec, "effect")) == "ACCOUNT" && !s.powered) continue;  // only once the RTG is on
    char row[100];
    snprintf(row, sizeof(row), "  \x1b[1;36m%-14s\x1b[0;37m%s\x1b[0m", sec.c_str(), help.c_str());
    say(s, row);
  }
}

// The key is right: this session takes the RTG. The timer stops, ELIZA wakes for
// this caller, and the console gains CREW.
static void shipPowerOn(Session &s) {
  stopPowerTimer(s);
  shipSay(s, iniGet(shipIni, "SHIP", "rtg_online", "RTG ONLINE|BUS STABLE|ATTITUDE HOLD"));
  powerScreen(s, "WAKE");
  say(s, "\x1b[0;33m" + iniGet(shipIni, "SHIP", "wake_line", "Good evening. I have been asleep for some time.") + "\x1b[0m");
  shipSay(s, iniGet(shipIni, "SHIP", "make_account", "CREW REGISTRATION ENABLED|TYPE CREW TO REGISTER"));
  s.state = SHIP_CONSOLE;
  shipPrompt(s);
}

static void handleShip(Session &s, String &line, const String &cmd) {
  if (s.state == SHIP_CONFIRM) {
    if (shipNorm(line) == powerCfg.confirmKey) { shipPowerOn(s); return; }
    shipSay(s, iniGet(shipIni, "SHIP", "aborted", "SWITCH ABORTED. NO CHANGE."));
    s.state = SHIP_CONSOLE; shipPrompt(s);
    return;
  }
  if (!cmd.length()) { shipPrompt(s); return; }
  String sec;
  if (!shipFind(line, sec)) { shipSay(s, iniGet(shipIni, "SHIP", "unknown", "UNKNOWN COMMAND - TYPE HELP")); shipPrompt(s); return; }
  String effect = up(iniGet(shipIni, sec, "effect"));
  if (effect == "EXIT") { shipLeave(s); return; }
  if (effect == "HELP") { shipHelp(s); shipPrompt(s); return; }
  if (effect == "RTG") {
    if (s.powered) { shipSay(s, iniGet(shipIni, sec, "text_powered", "RTG ALREADY ON THE BUS")); shipPrompt(s); return; }
    // The warning names the confirming key, so a caller who reads it can find it.
    powerScreen(s, "WARNING");
    String ask = iniGet(shipIni, sec, "confirm_text", "REARM REQUIRES OPERATOR ACK.|TYPE {KEY} AND PRESS ENTER TO ACKNOWLEDGE.|ANY OTHER ENTRY ABORTS.");
    ask.replace("{KEY}", "[" + powerCfg.confirmKey + "]");
    shipSay(s, ask);
    s.state = SHIP_CONFIRM;
    prompt(s, "\x1b[1;33mACK> \x1b[0m");
    return;
  }
  if (effect == "ACCOUNT") {
    if (!s.powered) { shipSay(s, iniGet(shipIni, "SHIP", "unknown", "UNKNOWN COMMAND - TYPE HELP")); shipPrompt(s); return; }
    crewAccountStart(s);
    return;
  }
  String text = s.powered ? iniGet(shipIni, sec, "text_powered", iniGet(shipIni, sec, "text")) : iniGet(shipIni, sec, "text");
  shipSay(s, text);
  shipPrompt(s);
}
