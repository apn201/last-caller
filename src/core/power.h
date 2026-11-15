#pragma once
// Ship power, per caller: the settings from STORY/POWER.INI. There is no
// board-wide power state. Every connection starts in brownout; the session
// that logs in as the maintenance account has a timer (core/session.h
// powerAt) and switches its own session to the RTG from the ship console. The
// logic is in core/power_logic.h; the session side is bbs/power_ui.h.
// No platform headers. Part of the shared core: included in order from src/bbs_core.h.
//
// A card without STORY/POWER.INI has no prologue: callers log in or sign up as usual.

#include "power_logic.h"

static constexpr const char *POWER_INI = "/BBS/STORY/POWER.INI";

struct PowerConfig {
  PowerTiming timing;
  String maintAccount = "admin", maintPassword = "admin";  // the ship's maintenance login
  String confirmKey = "K";                                  // the key the RTG warning asks for
  String unknownHint;                                       // one line after "Unknown caller." (Q12b)
  String frontHint, accountDone;                            // Q20: under the front page (what this is, the first task); after CREW
};
static PowerConfig powerCfg;
static bool powerEnabled = false;       // POWER.INI exists
static bool powerBannerPlayed = false;  // the full boot banner has been shown since start-up
static bool rtgSwitched = false;        // Q22: some caller has switched the ship to the RTG since start-up: a line that redials sees the powered banner

// POWER.INI is read at boot and again on RELOAD.
static void loadPower() {
  Ini ini;
  powerEnabled = iniLoad(POWER_INI, ini);
  PowerConfig c;
  if (powerEnabled) {
    c.timing.timerSec = (uint32_t)constrain(iniGet(ini, "POWER", "timer_sec", "120").toInt(), 1L, 3600L);
    String account = clean(iniGet(ini, "POWER", "maint_account", "admin"), 16, true);
    if (account.length() >= 3) c.maintAccount = account;
    c.maintPassword = iniGet(ini, "POWER", "maint_password", "admin");
    String key = iniGet(ini, "POWER", "confirm_key", "K");
    if (key.length()) c.confirmKey = key.substring(0, 1);
    c.confirmKey.toUpperCase();
    c.unknownHint = clean(iniGet(ini, "TEXT", "unknown_hint", ""), 78);
    c.frontHint = iniGet(ini, "TEXT", "front_hint", "");
    c.accountDone = iniGet(ini, "TEXT", "account_done", "");
  }
  powerCfg = c;
}
