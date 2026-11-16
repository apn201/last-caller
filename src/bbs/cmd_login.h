#pragma once
// Login and new-user prompts.
// Part of the single TinyBBS translation unit: included in order from main.cpp.

// The login prompt again after a failed try. With the prologue on there is only Login:.
static void loginRetry(Session &s) {
  if (powerEnabled) { s.state = LOGIN_HANDLE; prompt(s, "Login: "); }
  else { s.state = LOGIN_CHOICE; prompt(s, "(N)ew user, (L)ogin, or (G)oodbye? "); }
}

// G1: a wrong password on a story account is answered by that account (ACCOUNTS.INI badpass=, badpass2=,
// badpass3=), and the third miss in a row is followed by ELIZA's page (page=). The account's own login line is
// shown, never the password. Other accounts get POWER.INI [TEXT] bad_password, _2, _3. No answer twice in a row.
static String loginBadPassword(Session &s) {
  unsigned n = repeatCount(s, "LOGIN:BAD:" + s.user.normalized);
  String sec = s.user.normalized;
  String own = config(ACCOUNTS_INI, sec.c_str(), "badpass", "");
  String out;
  if (own.length()) {
    out = repeatAnswer(n, own, config(ACCOUNTS_INI, sec.c_str(), "badpass2", ""), config(ACCOUNTS_INI, sec.c_str(), "badpass3", ""), "", "");
    if (n == 2) {
      String page = config(ACCOUNTS_INI, sec.c_str(), "page", "");
      if (page.length()) out += "|\x01" + page;
    }
  } else {
    out = repeatAnswer(n, config(POWER_INI, "TEXT", "bad_password", "Bad password."), config(POWER_INI, "TEXT", "bad_password_2", ""),
                       config(POWER_INI, "TEXT", "bad_password_3", ""), "", "");
  }
  String text;
  size_t start = 0;
  while (start <= out.length()) {
    int bar = out.indexOf('|', start);
    String part = out.substring(start, bar < 0 ? out.length() : (size_t)bar);
    if (text.length()) text += "\r\n";
    if (part.length() && part[0] == 1) text += "\x1b[1;33mELIZA\x1b[0;33m> " + part.substring(1) + "\x1b[0m";
    else text += part;
    if (bar < 0) break;
    start = (size_t)bar + 1;
  }
  return text;
}
static String loginUnknown(Session &s) {
  unsigned n = repeatCount(s, "LOGIN:UNKNOWN");
  return repeatAnswer(n, "Unknown caller.", config(POWER_INI, "TEXT", "unknown_caller_2", ""), config(POWER_INI, "TEXT", "unknown_caller_3", ""), "", "");
}

static bool loginCommand(Session &s, String &line, const String &cmd) {
  if (s.state == LOGIN_CHOICE) { if (cmd == "N" || cmd == "NEW") { s.state = NEW_HANDLE; prompt(s, "Handle (3-16 chars, ENTER cancels): "); } else if (cmd == "L" || cmd == "LOGIN") { s.state = LOGIN_HANDLE; prompt(s, "Handle: "); } else if (cmd == "G") { closeSession(s); } else prompt(s, "N, L or G? "); return true; }
  if (s.state == LOGIN_HANDLE) { s.pendingHandle = clean(line, 16, true); User u;
    if (!s.pendingHandle.length() || !findUser(up(s.pendingHandle), u)) { say(s, loginUnknown(s)); if (powerEnabled && powerCfg.unknownHint.length() && s.repCount == 0) say(s, powerCfg.unknownHint); /* the hint once; a second ask gets the variant alone */ loginRetry(s); }
    else if (u.locked) { say(s, "This account is locked. Page the SysOp."); event("AUTH_LOCKED", u.handle); loginRetry(s); } else { s.user = u; s.state = LOGIN_PASSWORD; s.hideInput = true; prompt(s, "Password: "); } return true; }
  // The service label prints ADMIN in capitals; since G1 every password is case-blind (it was only the maintenance one, Q12b).
  if (s.state == LOGIN_PASSWORD) { s.hideInput = false;
    // G1: passwords are case-blind (as on PCBoard, RemoteAccess and Wildcat). A hash made before G1 still opens
    // the account, and a caller's own account is moved to the capitals at this login.
    bool right = passMatchesHash(s.user.salt, s.user.hash, line);
    if (!right) { say(s, "\r\n" + loginBadPassword(s)); event("AUTH_FAIL", s.pendingHandle); loginRetry(s); }
    else {
      if (passHash(s.user.salt, line) != s.user.hash && !hasFlag(s.user, "READONLY")) { s.user.salt = salt(); s.user.hash = passHash(s.user.salt, line); touchUser(s.user); }
      repeatReset(s); claimAccount(s); finishLogin(s);
    }
    return true; }
  // ENTER on an empty line backs out of signing up, at the handle and the password.
  if ((s.state == NEW_HANDLE || s.state == NEW_PASSWORD) && !line.length() && s.makingCrew) { crewAccountCancel(s); return true; }
  if ((s.state == NEW_HANDLE || s.state == NEW_PASSWORD) && !line.length()) { s.hideInput = false; say(s, "\r\nNo account made."); loginPrompt(s); return true; }
  if (s.state == NEW_HANDLE) { s.pendingHandle = clean(line, 16, true); User exists; if (s.pendingHandle.length() < 3 || findUser(up(s.pendingHandle), exists)) { say(s, "Handle unavailable."); prompt(s, "Handle (3-16 chars, ENTER cancels): "); } else { s.state = NEW_PASSWORD; s.hideInput = true; prompt(s, "Password (6+ chars, ENTER cancels): "); } return true; }
  if (s.state == NEW_PASSWORD) { if (line.length() < 6) { say(s, "\r\nToo short."); prompt(s, "Password (6+ chars, ENTER cancels): "); } else { s.hideInput = false; s.pendingPass = line; s.state = NEW_REAL; say(s); prompt(s, "Real name: "); } return true; }
  if (s.state == NEW_REAL) { s.pendingReal = clean(line, 40); s.state = NEW_LOCATION; prompt(s, "Location: "); return true; }
  if (s.state == NEW_LOCATION) {
    trace("NEW_USER_LOCATION", s.pendingHandle + " / " + clean(line, 40));
    s.user = User(); s.user.handle = s.pendingHandle; s.user.normalized = up(s.pendingHandle); s.user.realName = s.pendingReal; s.user.location = clean(line, 40); s.user.salt = salt(); s.user.hash = passHash(s.user.salt, s.pendingPass); s.user.level = (s.user.normalized == "SYSOP" && !anySysop()) ? 255 : board.newUserLevel; newUserDefaults(s.user); if (s.user.level == 255) s.user.flags = "SYSOP";
    if (!appendUser(s.user)) { say(s, "\r\nDatabase write failed: " + (storageFault.length() ? storageFault : "see Serial console") + "."); trace("NEW_USER_FAIL", s.pendingHandle); if (s.makingCrew) logoutToLogin(s); else loginRetry(s); return true; }
    event("NEW_USER", s.user.handle); trace("NEW_USER_OK", s.user.handle);
    if (s.makingCrew) { crewAccountDone(s); return true; }  // the new caller logs in with it
    say(s, "\r\nAccount created. Welcome aboard."); finishLogin(s); return true;
  }
  return false;
}
