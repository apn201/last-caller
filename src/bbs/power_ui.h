#pragma once
// The caller's side of ship power: the brownout login, the session drop, the
// maintenance account and what the sessions are told when the power changes.
// The state itself is in core/power.h. Same code for Telnet and the browser.
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
  s.user = User(); s.input = ""; s.hideInput = false; s.conference = 0;
  s.pendingHandle = ""; s.pendingPass = ""; s.reader = Reader();
  loginPrompt(s);
}

// The login prompt again after a failed try: brownout offers Login: only.
static void loginRetry(Session &s) {
  if (boardBrownout()) { s.state = LOGIN_HANDLE; prompt(s, "Login: "); }
  else { s.state = LOGIN_CHOICE; prompt(s, "(N)ew user, (L)ogin, or (G)oodbye? "); }
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

// Every tick: arm and enforce the brownout window of each session.
static void powerSessionsTick(uint32_t now) {
  if (!boardBrownout()) return;
  for (auto &s : sessions) {
    if (!s.used || s.state == GOODBYE) continue;
    if (!s.dropArmed) { s.dropArmed = true; s.dropAt = now + powerWindowMs(powerCfg.timing, bbsRandom()); }
    else if ((int32_t)(now - s.dropAt) >= 0) sunsideDrop(s);
  }
}

// The power changed under the sessions (a caller's switch, a SysOp command,
// the decay). `by` is the session that did it.
static void announcePower(bool on, const Session *by) {
  for (auto &n : sessions) {
    if (!n.used || &n == by || n.state == GOODBYE || n.state == DEAD) continue;
    n.dropArmed = false;
    if (on) {
      // Callers waiting at the brownout prompt, or in another ship console, go
      // back to the login prompt, which now offers a new account.
      if (!n.authenticated || isMaintenance(n.user)) {
        say(n, "\r\n\x1b[1;36mRTG ONLINE. THE SHIP HAS POWER.\x1b[0m");
        logoutToLogin(n);
      }
    } else {
      say(n, "\r\n\x1b[1;31mSOLAR BUS ONLY - POWER UNSTABLE\x1b[0m");
    }
  }
}

// SysOp: POWER shows the ship's power, POWER ON and POWER OFF force it (for
// testing and demos). Forcing brownout drops every session, the SysOp's too,
// within the window; the next login is the maintenance account's.
static void sysopPowerCommand(Session &s, const String &cmd) {
  String what = tokenAt(cmd, 1);
  if (!powerEnabled) { say(s, "Ship power is not set up: the card has no STORY/POWER.INI. The board is always powered."); return; }
  if (what == "ON") {
    if (power.powered) say(s, "The ship is already POWERED.");
    else { powerSet(true, "forced by SysOp " + s.user.handle, &s); say(s, "\x1b[1;32mPOWERED. Callers waiting at the brownout prompt are sent back to the login.\x1b[0m"); }
  } else if (what == "OFF") {
    if (!power.powered) say(s, "The ship is already in BROWNOUT.");
    else {
      powerSet(false, "forced by SysOp " + s.user.handle, &s);
      say(s, "\x1b[1;31mBROWNOUT. Every session, yours too, ends within " + String(powerCfg.timing.windowMaxSec) + " s. To power up again, log in as " + powerCfg.maintAccount + ".\x1b[0m");
    }
  } else {
    say(s, String("Ship power: ") + (power.powered ? "POWERED" : "BROWNOUT") + ". " +
           (powerCfg.timing.decayAfterIdleMin ? "Falls back to brownout " + String(powerCfg.timing.decayAfterIdleMin) + " min after the last caller leaves." : String("It never falls back by itself.")));
    say(s, "POWER ON or POWER OFF forces it.");
  }
}
