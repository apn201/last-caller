#pragma once
// The caller's side of the prologue: the front page, the maintenance account,
// each session's own power timer and the session drop. The settings are in core/power.h. Same code for Telnet and the browser.
// Part of the shared core: included in order from src/bbs_core.h.

static void loginPrompt(Session &s);   // bbs/board.h
static void finishLogin(Session &s);   // bbs/board.h

static bool isMaintenance(const User &u) { return powerEnabled && u.id && u.normalized == up(powerCfg.maintAccount); }

// The ship's maintenance login exists from the first boot. It is made by the
// board (not by hand in USERS.DAT), read-only, and is not the SysOp.
static void ensureMaintAccount() {
  if (!powerEnabled || !sdReady) return;
  User u;
  if (findUser(up(powerCfg.maintAccount), u)) {
    if (!hasFlag(u, "READONLY")) { u.flags = cleanFlags(u.flags.length() ? u.flags + ",READONLY" : String("READONLY")); touchUser(u); }
    return;
  }
  u = User();
  u.handle = powerCfg.maintAccount; u.normalized = up(powerCfg.maintAccount);
  u.realName = "Ship maintenance"; u.location = "HALCYON";
  u.salt = salt(); u.hash = sha256hex(u.salt + ":" + powerCfg.maintPassword);
  u.level = 10; u.flags = "READONLY";
  newUserDefaults(u);
  appendUser(u);
}

// Back to the login prompt, signed out, without dropping the connection.
static void logoutToLogin(Session &s) {
  s.authenticated = false; s.readOnly = false;
  s.powered = false; s.powerTimerOn = false; s.makingCrew = false;
  s.user = User(); s.input = ""; s.hideInput = false; s.conference = 0;
  s.pendingHandle = ""; s.pendingPass = ""; s.reader = Reader();
  loginPrompt(s);
}

// A card screen for ANSI callers, its ASCII twin for the others.
static void powerScreen(Session &s, const char *name, const String &fallback = "") {
  if (s.ansi) art(s, (String("/BBS/ANSI/POWER/") + name + ".ANS").c_str(), fallback);
  else plainArt(s, (String("/BBS/ASCII/POWER/") + name + ".ASC").c_str(), fallback);
}

// The first connection since start-up gets the full BIOS banner, every later
// one the short reboot banner. The caller then sees "Login:".
static void brownoutLogin(Session &s) {
  powerScreen(s, powerBannerPlayed ? "REBOOT" : "BOOT");
  powerBannerPlayed = true;
  s.state = LOGIN_HANDLE;
  prompt(s, "Login: ");
}

// An account may be online once, unless it is read-only. A second login of the
// same account takes over: the first session is told and hung up. (Two live
// sessions would both write that account's files.)
static void claimAccount(Session &s) {
  if (hasFlag(s.user, "READONLY")) return;
  for (auto &n : sessions) {
    if (!n.used || &n == &s || !n.authenticated || n.state == GOODBYE || n.user.id != s.user.id) continue;
    n.hideInput = false;
    say(n, "\r\n\x1b[1;31mThis account has logged in on another node. NO CARRIER\x1b[0m");
    n.state = GOODBYE;
    event("LOGIN_TAKEOVER", s.user.handle + " node " + s.node + " replaced node " + n.node);
  }
}

// Brownout: the ship tumbles into shadow. The picture goes out after what is
// already on its way, then the connection closes once it is sent.
static void sunsideDrop(Session &s) {
  if (s.state == GOODBYE) return;
  skipStream(s);
  s.hideInput = false;
  powerScreen(s, "SUNSIDE", "SUNSIDE ENDS - CARRIER LOST");
  say(s);
  event("POWER", String("sunside drop, node ") + s.node + (s.user.handle.length() ? " " + s.user.handle : String()));
  s.state = GOODBYE;
}

// The maintenance login: the ship is dark for this caller until the RTG is
// switched. Two minutes (POWER.INI) from now. Other logins have power.
static void startPowerTimer(Session &s) {
  s.powered = false;
  s.powerTimerOn = true;
  s.powerAt = millis() + powerTimerMs(powerCfg.timing);
}
// The caller switched this session to the RTG: the timer stops.
static void stopPowerTimer(Session &s) {
  s.powered = true;
  s.powerTimerOn = false;
  event("POWER", String("RTG connected, node ") + s.node);
}

// Every tick: a maintenance session whose time has run out ends in the power glitch.
static void powerSessionsTick(uint32_t now) {
  if (!powerEnabled) return;
  for (auto &s : sessions)
    if (s.used && s.powerTimerOn && s.state != GOODBYE && powerTimerDue(now, s.powerAt)) sunsideDrop(s);
}

// Crew account: the new-account prompts, opened from the ship console after the
// RTG. This is the only place an account is made when the prologue is on.
static void shipPrompt(Session &s);  // doors/ship.h
static void crewAccountStart(Session &s) {
  s.makingCrew = true;
  s.state = NEW_HANDLE;
  prompt(s, "Handle (3-16 chars, ENTER cancels): ");
}
static void crewAccountCancel(Session &s) {
  s.makingCrew = false; s.hideInput = false; s.pendingHandle = ""; s.pendingPass = "";
  s.state = SHIP_CONSOLE;
  say(s, "\r\nNo account made.");
  shipPrompt(s);
}
static void crewAccountDone(Session &s) {
  say(s, "\r\nAccount created. Log in with it.");
  logoutToLogin(s);
}
