#pragma once
// Login and new-user prompts.
// Part of the single TinyBBS translation unit: included in order from main.cpp.

// The login prompt again after a failed try. With the prologue on there is only Login:.
static void loginRetry(Session &s) {
  if (powerEnabled) { s.state = LOGIN_HANDLE; prompt(s, "Login: "); }
  else { s.state = LOGIN_CHOICE; prompt(s, "(N)ew user, (L)ogin, or (G)oodbye? "); }
}

static bool loginCommand(Session &s, String &line, const String &cmd) {
  if (s.state == LOGIN_CHOICE) { if (cmd == "N" || cmd == "NEW") { s.state = NEW_HANDLE; prompt(s, "Handle (3-16 chars, ENTER cancels): "); } else if (cmd == "L" || cmd == "LOGIN") { s.state = LOGIN_HANDLE; prompt(s, "Handle: "); } else if (cmd == "G") { closeSession(s); } else prompt(s, "N, L or G? "); return true; }
  if (s.state == LOGIN_HANDLE) { s.pendingHandle = clean(line, 16, true); User u;
    if (!s.pendingHandle.length() || !findUser(up(s.pendingHandle), u)) { say(s, "Unknown caller."); if (powerEnabled && powerCfg.unknownHint.length()) say(s, powerCfg.unknownHint); loginRetry(s); }
    else if (u.locked) { say(s, "This account is locked. Page the SysOp."); event("AUTH_LOCKED", u.handle); loginRetry(s); } else { s.user = u; s.state = LOGIN_PASSWORD; s.hideInput = true; prompt(s, "Password: "); } return true; }
  // The service label prints ADMIN in capitals: the maintenance password also works in lower case (Q12b).
  if (s.state == LOGIN_PASSWORD) { s.hideInput = false;
    String lowered = line; lowered.toLowerCase();
    bool right = sha256hex(s.user.salt + ":" + line) == s.user.hash || (isMaintenance(s.user) && sha256hex(s.user.salt + ":" + lowered) == s.user.hash);
    if (!right) { say(s, "\r\nBad password."); event("AUTH_FAIL", s.pendingHandle); loginRetry(s); } else { claimAccount(s); finishLogin(s); } return true; }
  // ENTER on an empty line backs out of signing up, at the handle and the password.
  if ((s.state == NEW_HANDLE || s.state == NEW_PASSWORD) && !line.length() && s.makingCrew) { crewAccountCancel(s); return true; }
  if ((s.state == NEW_HANDLE || s.state == NEW_PASSWORD) && !line.length()) { s.hideInput = false; say(s, "\r\nNo account made."); loginPrompt(s); return true; }
  if (s.state == NEW_HANDLE) { s.pendingHandle = clean(line, 16, true); User exists; if (s.pendingHandle.length() < 3 || findUser(up(s.pendingHandle), exists)) { say(s, "Handle unavailable."); prompt(s, "Handle (3-16 chars, ENTER cancels): "); } else { s.state = NEW_PASSWORD; s.hideInput = true; prompt(s, "Password (6+ chars, ENTER cancels): "); } return true; }
  if (s.state == NEW_PASSWORD) { if (line.length() < 6) { say(s, "\r\nToo short."); prompt(s, "Password (6+ chars, ENTER cancels): "); } else { s.hideInput = false; s.pendingPass = line; s.state = NEW_REAL; say(s); prompt(s, "Real name: "); } return true; }
  if (s.state == NEW_REAL) { s.pendingReal = clean(line, 40); s.state = NEW_LOCATION; prompt(s, "Location: "); return true; }
  if (s.state == NEW_LOCATION) {
    trace("NEW_USER_LOCATION", s.pendingHandle + " / " + clean(line, 40));
    s.user = User(); s.user.handle = s.pendingHandle; s.user.normalized = up(s.pendingHandle); s.user.realName = s.pendingReal; s.user.location = clean(line, 40); s.user.salt = salt(); s.user.hash = sha256hex(s.user.salt + ":" + s.pendingPass); s.user.level = (s.user.normalized == "SYSOP" && !anySysop()) ? 255 : board.newUserLevel; newUserDefaults(s.user); if (s.user.level == 255) s.user.flags = "SYSOP";
    if (!appendUser(s.user)) { say(s, "\r\nDatabase write failed: " + (storageFault.length() ? storageFault : "see Serial console") + "."); trace("NEW_USER_FAIL", s.pendingHandle); if (s.makingCrew) logoutToLogin(s); else loginRetry(s); return true; }
    event("NEW_USER", s.user.handle); trace("NEW_USER_OK", s.user.handle);
    if (s.makingCrew) { crewAccountDone(s); return true; }  // the new caller logs in with it
    say(s, "\r\nAccount created. Welcome aboard."); finishLogin(s); return true;
  }
  return false;
}
