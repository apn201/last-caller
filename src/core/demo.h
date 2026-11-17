#pragma once
// GE: the end demo. Releasing the queue (the ENDING scene's SEND or STAY) starts a 1990s demo, then the
// carrier drops. The timeline is on the card, STORY/DEMO.INI, and so are the tune, the pictures and the
// player script (/BBS/DEMO). In the browser the page runs the demo (/demo/DEMO.JS) and the board only says
// "start" (OSC 9;DEMO;END1 or END2) and waits for a key. Telnet, and a browser whose card has no player
// script, get the ANSI version: the [T01]... steps of the same file, no music, run here by demoTick.
// Q skips to NO CARRIER. A key there goes on to the reconnect screen, then the main menu.
// A replay (a second view) is started from the notebook's scene list and ends back in the notebook.
// All caller-visible words are a DRAFT in DEMO.INI. Part of the shared core: included from src/bbs_core.h
// after core/glitch.h. No platform headers.
// GE2: the same machinery plays Ty's text-mode Second Reality, an easter egg after the ending. Its timeline is
// /BBS/DEMO/SR.INI (browser: /demo/SR.JS with the real music, OSC 9;DEMO;SR; Telnet: its [T..] steps). It is
// opened from a .DMO file in a file area (Ty's Gallery, behind NEEDS.BBS END1|END2), has no NO CARRIER and ends
// back in the file list.

static constexpr const char *DEMO_INI = "/BBS/STORY/DEMO.INI";
static constexpr const char *DEMO_JS = "/BBS/DEMO/DEMO.JS";
static constexpr uint32_t DEMO_KEY_GRACE_MS = 1500;     // a key at the NO CARRIER screen counts this long after it appears
static constexpr uint32_t DEMO_BROWSER_LIMIT_MS = 300000;   // a browser that never answers (reloaded, closed): on after this

struct DemoStep {
  String when, art, text, color, bar;   // when: "", END1 or END2
  uint32_t pauseMs = 1500, lineMs = 0;
  bool clear = true;
};
struct DemoTable {
  String scene = "ENDING";
  String noCarrier = "NO CARRIER", lost = "CARRIER LOST. PRESS ANY KEY TO RECONNECT";
  String afterArt, afterText, afterCrew, afterKey = "Press any key for the Main Board: ";
  std::vector<DemoStep> steps;
};
static DemoTable demoTable;
static constexpr const char *EGG_INI = "/BBS/DEMO/SR.INI";
static constexpr const char *EGG_JS = "/BBS/DEMO/SR.JS";
static DemoTable eggTable;
static const DemoTable &demoOf(const Session &s) { return s.demoEgg ? eggTable : demoTable; }
static void listArea(Session &s);       // bbs/files.h (GE2)

static void notebookBack(Session &s);   // bbs/notebook.h
static void mainMenu(Session &s);       // bbs/board.h

static String demoLines(String text) { text.replace("|", "\r\n"); return text; }

// The [T01]... steps of a timeline file: the ANSI version.
static void demoSteps(Ini &ini, DemoTable &t) {
  for (const String &sec : iniSections(ini)) {
    if (sec.length() < 2 || sec[0] != 'T' || !isdigit((unsigned char)sec[1]) || t.steps.size() >= 40) continue;
    DemoStep st;
    st.when = up(iniGet(ini, sec, "when"));
    st.art = iniGet(ini, sec, "art");
    st.text = iniGet(ini, sec, "text");
    st.color = iniGet(ini, sec, "color", "0;37");
    st.bar = iniGet(ini, sec, "bar");
    st.pauseMs = (uint32_t)constrain(iniGet(ini, sec, "pause", "1500").toInt(), 0L, 20000L);
    st.lineMs = (uint32_t)constrain(iniGet(ini, sec, "line_ms", "0").toInt(), 0L, 5000L);
    st.clear = iniGet(ini, sec, "clear", "1").toInt() != 0;
    t.steps.push_back(st);
  }
}
// GE2: Ty's demo. A card without SR.INI has no egg (its .DMO file then says so).
static void loadEgg() {
  Ini ini;
  eggTable = DemoTable();
  if (iniLoad(EGG_INI, ini)) demoSteps(ini, eggTable);
}

// At boot and on RELOAD, from loadSettings(). A card without the file has no demo: the ending goes back to the menu.
static String loadDemo() {
  Ini ini;
  demoTable = DemoTable();
  if (!iniLoad(DEMO_INI, ini)) return "no end demo";
  demoTable.scene = up(iniGet(ini, "DEMO", "scene", "ENDING"));
  demoTable.noCarrier = iniGet(ini, "DEMO", "no_carrier", demoTable.noCarrier);
  demoTable.lost = iniGet(ini, "DEMO", "lost", demoTable.lost);
  demoTable.afterArt = iniGet(ini, "AFTER", "art");
  demoTable.afterText = iniGet(ini, "AFTER", "text");
  demoTable.afterCrew = iniGet(ini, "AFTER", "text_crew");
  demoTable.afterKey = iniGet(ini, "AFTER", "key", demoTable.afterKey);
  demoSteps(ini, demoTable);
  loadEgg();
  return String((unsigned)demoTable.steps.size()) + " demo steps";
}

// Can this caller see the demo? The card has the timeline, and the caller holds an ending.
static uint8_t demoKindOf(Session &s) {
  std::string f = doorFlagsOf(s);
  return haunt::flagsHas(f, "END1") ? 1 : haunt::flagsHas(f, "END2") ? 2 : 0;
}
static bool demoAvailable(Session &s) { return sdReady && demoTable.steps.size() && demoKindOf(s); }

// The browser plays it when the card has the player script and the caller is in the browser.
static bool demoInBrowser(const Session &s) { return s.web && sdReady && SD_MMC.exists(DEMO_JS); }
static bool eggAvailable(Session &s) { return sdReady && eggTable.steps.size() && demoKindOf(s); }

static bool demoStart(Session &s, uint8_t kind, bool replay) {
  if (!sdReady || !demoTable.steps.size() || !kind) return false;
  s.demoEgg = false; s.demoKind = kind; s.demoReplay = replay; s.demoStage = 1; s.demoStarted = millis(); s.demoAt = s.demoStarted + 500;
  s.demoStep = 0; s.demoSub = 0; s.demoBrowser = demoInBrowser(s);
  s.input = ""; s.hotKey = 0; s.typeAhead = ""; s.hideInput = false; s.paused = false;
  s.state = DEMO_PLAY; activity(s, "THE END DEMO");
  event("DEMO", s.user.handle + " node " + s.node + (replay ? " replays" : " starts") + " the end demo, ending " + String(kind) + (s.demoBrowser ? " (browser)" : " (ANSI)"));
  if (s.demoBrowser) write(s, String("\x1b]9;DEMO;") + (kind == 2 ? "END2" : "END1") + "\x07");
  return true;
}
// GE2: Ty's demo, from a .DMO file in the file list.
static bool demoEggStart(Session &s) {
  if (!eggAvailable(s)) return false;
  s.demoEgg = true; s.demoKind = demoKindOf(s); s.demoReplay = false; s.demoStage = 1; s.demoStarted = millis(); s.demoAt = s.demoStarted + 500;
  s.demoStep = 0; s.demoSub = 0; s.demoBrowser = s.web && SD_MMC.exists(EGG_JS);
  s.input = ""; s.hotKey = 0; s.typeAhead = ""; s.hideInput = false; s.paused = false;
  s.state = DEMO_PLAY; activity(s, "WATCHING A DEMO");
  event("DEMO", s.user.handle + " node " + s.node + " opens Ty's demo" + (s.demoBrowser ? " (browser)" : " (ANSI)"));
  if (s.demoBrowser) write(s, "\x1b]9;DEMO;SR\x07");
  else write(s, s.ansi ? "\x1b[0m\x1b[2J\x1b[H" : "\r\n\r\n");
  return true;
}

// Step 8: the board again. The aftermath is on the panels already (G2); one screen says what is left to do.
static void demoReconnect(Session &s) {
  s.demoStage = 0; s.demoBrowser = false; s.paused = false;
  if (s.demoEgg) {   // GE2: back to the file list it was opened from
    s.demoEgg = false; s.demoKind = 0; skipStream(s); s.pending = "";
    write(s, s.ansi ? "\x1b[0m\x1b[2J\x1b[H" : "\r\n\r\n");
    listArea(s);
    return;
  }
  if (s.demoReplay) { s.demoReplay = false; notebookBack(s); return; }
  s.menusSeen = "";   // the main menu draws its art again, with the aftermath on it
  write(s, s.ansi ? "\x1b[0m\x1b[2J\x1b[H" : "\r\n\r\n");
  if (demoTable.afterArt.length()) {
    if (s.ansi) art(s, ("/BBS/ANSI/DEMO/" + demoTable.afterArt + ".ANS").c_str());
    else plainArt(s, ("/BBS/ASCII/DEMO/" + demoTable.afterArt + ".ASC").c_str());
  }
  String text = s.readOnly && demoTable.afterCrew.length() ? demoTable.afterCrew : demoTable.afterText;
  if (text.length()) say(s, (s.ansi ? "\x1b[0;37m" : "") + demoLines(text) + (s.ansi ? "\x1b[0m" : ""));
  s.state = BULLETIN;
  prompt(s, demoTable.afterKey);
}

// The carrier drops. The ANSI version prints the lines; the browser has drawn them in its own picture.
static void demoLost(Session &s) {
  if (s.demoEgg) { demoReconnect(s); return; }   // GE2: Ty's demo has no carrier to lose
  s.demoStage = 2; s.demoAt = millis() + DEMO_KEY_GRACE_MS;
  skipStream(s);
  if (s.demoBrowser) return;
  s.pending = "";
  String blank; for (int i = 0; i < 9; ++i) blank += "\r\n";
  write(s, (s.ansi ? "\x1b[0m\x1b[2J\x1b[H" : "\r\n\r\n") + blank);
  String pad1, pad2;
  for (int i = 0; i < (80 - (int)demoTable.noCarrier.length()) / 2; ++i) pad1 += ' ';
  for (int i = 0; i < (80 - (int)demoTable.lost.length()) / 2; ++i) pad2 += ' ';
  say(s, (s.ansi ? "\x1b[1;37m" : "") + pad1 + demoTable.noCarrier + (s.ansi ? "\x1b[0m" : ""));
  say(s);
  say(s, (s.ansi ? "\x1b[0;90m" : "") + pad2 + demoTable.lost + (s.ansi ? "\x1b[0m" : ""));
}

// A key. Playing: Q skips (Telnet and the ANSI version); in the browser the page keeps the keys until it ends
// and then sends one, so any key after the first seconds (a reloaded page) or the page's own end mark goes on.
// Lost: any key, after its grace.
static void demoInput(Session &s, const String &bytes) {
  const uint32_t now = millis();
  for (uint16_t i = 0; i < bytes.length(); ++i) {
    char c = bytes[i];
    if (c == 0) continue;
    if (c == '\n' && s.ignoreNextLf) { s.ignoreNextLf = false; continue; }
    if (c == '\r') s.ignoreNextLf = true;
    if (s.demoStage == 1) {
      if (s.demoBrowser) {
        if (c == 0x1d || msSince(now, s.demoStarted) > 5000) { demoReconnect(s); return; }
        continue;
      }
      if (c == 'q' || c == 'Q') { demoLost(s); return; }
      continue;
    }
    if (s.demoStage == 2) {
      if ((int32_t)(s.demoAt - now) > 0 && c != 0x1d) continue;   // before the grace: dropped
      demoReconnect(s);
      return;
    }
  }
}

// From bbsTick, after sceneTick: the ANSI version, one step at a time.
static void demoTick(uint32_t now) {
  for (auto &s : sessions) {
    if (!s.used || s.state != DEMO_PLAY || s.demoStage != 1) continue;
    if (s.demoBrowser) {   // the page plays it; a page that never answers is left behind
      if (msSince(now, s.demoStarted) > DEMO_BROWSER_LIMIT_MS) demoReconnect(s);
      continue;
    }
    if ((int32_t)(s.demoAt - now) > 0 || streaming(s) || !s.jobs.empty() || s.pending.length() > 400) continue;
    const char *want = s.demoKind == 2 ? "END2" : "END1";
    const DemoTable &table = demoOf(s);
    while (s.demoStep < table.steps.size() && table.steps[s.demoStep].when.length() && table.steps[s.demoStep].when != want) { s.demoStep++; s.demoSub = 0; }
    if (s.demoStep >= table.steps.size()) { demoLost(s); continue; }
    const DemoStep &st = table.steps[s.demoStep];
    const String on = s.ansi ? "\x1b[" + st.color + "m" : String(), off = s.ansi ? "\x1b[0m" : String();
    if (st.bar.length()) {   // a progress bar over `pause` ms, drawn in place with CR
      const uint16_t parts = 20;
      String fill, rest;
      for (uint16_t i = 0; i < parts; ++i) (i < s.demoSub ? fill : rest) += i < s.demoSub ? (s.ansi ? "\xe2\x96\x88" : "#") : (s.ansi ? "\xe2\x96\x91" : ".");
      write(s, "\r" + on + "  " + st.bar + "  [" + fill + rest + "] " + String(s.demoSub * 100 / parts) + "%" + off);
      if (s.demoSub >= parts) { write(s, "\r\n"); s.demoStep++; s.demoSub = 0; s.demoAt = now + 400; }
      else { s.demoSub++; s.demoAt = now + st.pauseMs / parts; }
      continue;
    }
    if (s.demoSub == 0) {
      if (st.clear) write(s, s.ansi ? "\x1b[0m\x1b[2J\x1b[H" : "\r\n\r\n");
      if (st.art.length() && s.ansi) art(s, ("/BBS/ANSI/DEMO/" + st.art + ".ANS").c_str());
    }
    std::vector<String> lines;
    { String t = st.text; int at = 0; while (t.length() && at <= (int)t.length()) { int e = t.indexOf('|', at); if (e < 0) e = t.length(); lines.push_back(t.substring(at, e)); at = e + 1; } }
    if (!st.lineMs) {
      for (const String &l : lines) say(s, on + l + off);
      s.demoStep++; s.demoSub = 0; s.demoAt = now + st.pauseMs;
    } else {
      if (s.demoSub < lines.size()) { say(s, on + lines[s.demoSub] + off); s.demoSub++; s.demoAt = now + st.lineMs; }
      if (s.demoSub >= lines.size()) { s.demoStep++; s.demoSub = 0; s.demoAt = now + st.pauseMs; }
    }
  }
}