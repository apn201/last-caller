#pragma once
// Ship power: the board-wide BROWNOUT / POWERED state, its settings from
// STORY/POWER.INI and its persistence in DATA/STATE/POWER.DAT. The logic is in
// core/power_logic.h; this is the card and session side. No platform headers.
// Part of the shared core: included in order from src/bbs_core.h.
//
// A card without STORY/POWER.INI has no prologue: the board is simply powered.

#include "power_logic.h"

static constexpr const char *POWER_INI = "/BBS/STORY/POWER.INI";
static constexpr const char *POWER_FILE = "/BBS/DATA/STATE/POWER.DAT";

struct PowerConfig {
  PowerTiming timing;
  String maintAccount = "admin", maintPassword = "admin";  // the ship's maintenance login
  String confirmKey = "K";                                  // the key the RTG warning asks for
  bool startPowered = false;                                // state when POWER.DAT does not exist
};
static PowerConfig powerCfg;
static PowerMachine power;
static bool powerEnabled = false;   // POWER.INI exists
static bool powerLoaded = false;    // the state has been read once
static bool powerBannerPlayed = false;  // the full boot banner has been shown since start-up

static bool boardPowered() { return !powerEnabled || power.powered; }
static bool boardBrownout() { return powerEnabled && !power.powered; }

static void announcePower(bool on, const Session *by);  // bbs/power_ui.h: tells the sessions

static void powerSave() {
  putRecord(POWER_FILE, 1, String("1|") + (power.powered ? "POWERED" : "BROWNOUT") + "|" + nowText());
}

// POWER.INI is read at boot and again on RELOAD. The state itself is read from
// POWER.DAT once, at boot: a RELOAD does not change the power.
static void loadPower() {
  Ini ini;
  powerEnabled = iniLoad(POWER_INI, ini);
  PowerConfig c;
  if (powerEnabled) {
    c.timing.windowMinSec = (uint32_t)constrain(iniGet(ini, "POWER", "window_min", "60").toInt(), 1L, 3600L);
    c.timing.windowMaxSec = (uint32_t)constrain(iniGet(ini, "POWER", "window_max", "120").toInt(), 1L, 3600L);
    c.timing.decayAfterIdleMin = (uint32_t)constrain(iniGet(ini, "POWER", "decay_after_idle_min", "0").toInt(), 0L, 1440L);
    String account = clean(iniGet(ini, "POWER", "maint_account", "admin"), 16, true);
    if (account.length() >= 3) c.maintAccount = account;
    c.maintPassword = iniGet(ini, "POWER", "maint_password", "admin");
    String key = iniGet(ini, "POWER", "confirm_key", "K");
    if (key.length()) c.confirmKey = key.substring(0, 1);
    c.confirmKey.toUpperCase();
    c.startPowered = up(iniGet(ini, "POWER", "start", "brownout")) == "POWERED";
  }
  powerCfg = c;
  if (powerLoaded) return;
  powerLoaded = true;
  bool on = c.startPowered;
  if (sdReady && SD_MMC.exists(POWER_FILE)) {
    File f = SD_MMC.open(POWER_FILE, FILE_READ);
    if (f) { String x = f.readStringUntil('\n'); f.close(); x.trim(); String st = up(field(x, 1)); if (st == "POWERED") on = true; else if (st == "BROWNOUT") on = false; }
  }
  power.set(on);
  // A stick that lost power while POWERED comes back POWERED; the idle timer
  // starts by itself on the first tick with nobody connected.
  if (powerEnabled) event("POWER", String("boot ") + (on ? "POWERED" : "BROWNOUT"));
}

// Switch the whole board. `why` goes to the event log; `by` is the session that
// did it (it is not told twice).
static void powerSet(bool on, const String &why, const Session *by = nullptr) {
  if (!powerEnabled || power.powered == on) return;
  power.set(on);
  for (auto &s : sessions) s.dropArmed = false;  // browned-out sessions get a fresh window on the next tick
  powerSave();
  event("POWER", String(on ? "POWERED" : "BROWNOUT") + ": " + why);
  announcePower(on, by);
}

static uint32_t connectedSessions() {
  uint32_t n = 0;
  for (auto &s : sessions) if (s.used) n++;
  return n;
}
// Called every tick: the powered board falls back to brownout once the last
// caller has been gone for decay_after_idle_min.
static void powerTick(uint32_t now) {
  if (!powerEnabled) return;
  if (power.tick(now, connectedSessions(), powerCfg.timing)) {
    powerSave();
    event("POWER", "BROWNOUT: no callers for " + String(powerCfg.timing.decayAfterIdleMin) + " min, the RTG decayed");
    announcePower(false, nullptr);
  }
}
