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
  ensureStoryAccounts();  // STORY/ACCOUNTS.INI: the crew, ELIZA, the ground relay
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
  hotkeyTick(now);
  powerSessionsTick(now);
  hauntTick(now);
  sceneTick(now);
  ghostTick(now);
  glitchTick(now);
  for (auto &s : sessions) {
    if (!s.used) continue;
    // Q21: a caller back at Login: on a line that has signed in (a crew visit, L, G) has ten minutes,
    // a new line two. Thirty seconds before the line closes it says so once, on its own line, and the
    // prompt and what was typed are drawn again under it.
    uint32_t idleLimit = s.authenticated ? SIGNED_IN_TIMEOUT_MS : (s.ownUserId ? CALLER_LOGIN_TIMEOUT_MS : LOGIN_TIMEOUT_MS);
    // Q22: a key run in this tick (hotkeyTick, a scene) sets lastAt after `now`; unsigned, now - lastAt would wrap
    // to about 49 days and close the line at once. That was the "connection closed" both critics saw.
    uint32_t quiet = msSince(now, s.lastAt);
    if (quiet + IDLE_WARN_MS < idleLimit) s.idleWarned = false;
    else if (!s.idleWarned && s.state != GOODBYE && s.state != SCENE_PLAY && !streaming(s)) {
      s.idleWarned = true;
      write(s, "\r\n\x1b[0;33mStill there? The line closes in 30 seconds. Press any key to stay.\x1b[0m\r\n" + s.lastPrompt);
      if (s.input.length()) { String shown = s.input; if (s.hideInput) for (size_t i = 0; i < shown.length(); ++i) shown[i] = '*'; write(s, shown); }
    }
    bool idle = quiet >= idleLimit;
    if (idle || (s.web && msSince(now, s.lastPoll) >= WEB_CARRIER_TIMEOUT_MS))
      closeSession(s);
  }
}
