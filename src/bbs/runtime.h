#pragma once
// Starting the board and keeping it running. Both builds call these: the
// ESP32 firmware from setup() and loop(), the server from its own main loop.
// Transports (web, Telnet) and hardware (LCD, WiFi) are started by the
// caller, outside the core.
// Part of the shared core: included last from src/bbs_core.h.

// Card, user and message counters, settings and message history. Call once,
// after the card is mounted (sdReady says whether it is).
static String bbsBegin() {
  bbsMutex.init();
  bootAt = millis();
  if (sdReady && prepareStorage()) { userCounters(); messageCounters(); }
  else { if (!storageFault.length()) storageFault = "storage setup failed"; sdReady = false; trace("SD_OFFLINE", storageFault); }
  String summary = loadSettings();
  ensureMaintAccount();  // after the settings: POWER.INI names the account
  return summary;
}
// Paced output, the board clock and idle timeouts. Call often (every loop),
// holding the BBS lock. `transports` runs in between: it may open sessions,
// so the time is read again after it.
template <typename F> static void bbsTick(F transports) {
  uint32_t now = millis();
  for (auto &s : sessions) if (s.used) pace(s, now);
  transports();
  tickClock();
  now = millis();
  powerTick(now);
  powerSessionsTick(now);
  for (auto &s : sessions) {
    if (!s.used) continue;
    uint32_t idleLimit = s.authenticated ? 30UL * 60UL * 1000UL : LOGIN_TIMEOUT_MS;
    // In brownout only the window ends a session (power.h); idle time does not count.
    bool idle = !boardBrownout() && now - s.lastAt >= idleLimit;
    if (idle || (s.web && now - s.lastPoll >= WEB_CARRIER_TIMEOUT_MS))
      closeSession(s);
  }
}
