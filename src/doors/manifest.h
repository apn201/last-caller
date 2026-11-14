#pragma once
// MANIFEST: the crew manifest, its notebook and the endings. Rows, pick lists,
// notes and the board's wording are in STORY/MANIFEST.INI (read at boot and on
// RELOAD); the decisions (which rows a caller sees, filing in pairs, the count)
// are core/manifest_logic.h, which a host test covers. The caller opens it
// with R from the main menu, only when it works for them: an account that can
// save, with evidence about at least one crew member. Picks are kept per
// account in DATA/STATE/MAN.DAT; confirmed rows are flags (M_H M_A M_T M_V
// M_Z) and flags only grow. A finished caller (END1 sent, END2 stayed) is
// listed in WHO afterwards from DATA/STATE/DONE.DAT. Same on Telnet and in the
// browser; no card path is ever shown.
// Part of the single TinyBBS translation unit: included in order from main.cpp.
#include "../core/manifest_logic.h"
#include "../core/wrap_logic.h"

static constexpr const char *MANIFEST_INI = "/BBS/STORY/MANIFEST.INI";
static constexpr const char *MAN_DAT = "/BBS/DATA/STATE/MAN.DAT";    // id|picks
static constexpr const char *DONE_DAT = "/BBS/DATA/STATE/DONE.DAT";  // id|handle|1 or 2
static constexpr int STICKY_SHOWN = 6, STICKY_NODE = 60;
static manifest::Table manTable;

// "|" breaks a line in the card's texts; "\n" does too (the logic joins lines with it).
static String manLines(String text, const char *sep = "\r\n") { text.replace("\n", "|"); text.replace("|", sep); return text; }

// At boot and on RELOAD, from loadSettings(). A card without the file has no manifest.
static String loadManifest() {
  Ini ini;
  std::vector<haunt::Kv> kv;
  if (iniLoad(MANIFEST_INI, ini)) for (const auto &e : ini) kv.push_back({hauntStd(e.section), hauntStd(e.key), hauntStd(e.value)});
  manifest::parse(kv, manTable);
  for (const auto &p : manTable.problems) event("MANIFEST_SKIP", hauntStr(p));
  String out = String((unsigned)manTable.rows.size()) + " manifest rows";
  if (manTable.skipped) out += " (" + String(manTable.skipped) + " skipped, see the event log)";
  return out;
}

// ---------- picks and the sticky list on the card --------------------------------------------------
static String manPicksLoad(uint32_t id) {
  if (!sdReady || !id) return "";
  File f = SD_MMC.open(MAN_DAT, FILE_READ);
  if (!f) return "";
  String found;
  while (f.available()) {
    String l = f.readStringUntil('\n');
    while (l.endsWith("\r")) l.remove(l.length() - 1);
    if (strtoul(l.c_str(), nullptr, 10) == id) { found = field(l, 1); break; }
  }
  f.close();
  return found;
}
static bool manPicksSave(const Session &s, const String &picks) {
  return putRecord(MAN_DAT, s.user.id, String(s.user.id) + "|" + picks);
}

// A finished caller is kept in WHO: called when a scene ends and at sign-in, so a
// lost connection after the ending flag cannot lose the record.
static void endingRecord(Session &s) {
  if (!s.authenticated || !canWrite(s) || !s.user.id) return;
  std::string flags = doorFlagsOf(s);
  int kind = haunt::flagsHas(flags, "END1") ? 1 : haunt::flagsHas(flags, "END2") ? 2 : 0;
  if (!kind) return;
  String handle = s.user.handle;
  handle.replace("|", "");
  if (!putRecord(DONE_DAT, s.user.id, String(s.user.id) + "|" + handle + "|" + kind)) {
    event("MANIFEST_FAIL", s.user.handle + ": the ending was not recorded for WHO");
    return;
  }
  event("ENDING", s.user.handle + " node " + s.node + " ending " + String(kind));
}

// The last few finished callers, as extra WHO rows on nodes from 60 up (a handle that is
// online right now is shown by its own row instead).
static void manifestStickyRows(Session &viewer, std::vector<WhoRow> &rows) {
  (void)viewer;
  if (!sdReady) return;
  File f = SD_MMC.open(DONE_DAT, FILE_READ);
  if (!f) return;
  std::vector<std::pair<String, String>> recs;
  while (f.available()) {
    String l = f.readStringUntil('\n');
    while (l.endsWith("\r")) l.remove(l.length() - 1);
    if (!l.length() || !strtoul(l.c_str(), nullptr, 10)) continue;
    recs.push_back({field(l, 1), field(l, 2)});
  }
  f.close();
  size_t first = recs.size() > (size_t)STICKY_SHOWN ? recs.size() - STICKY_SHOWN : 0;
  int node = STICKY_NODE;
  for (size_t i = first; i < recs.size(); ++i) {
    bool online = false;
    for (const auto &r : rows) if (up(r.handle) == up(recs[i].first)) { online = true; break; }
    if (online || !recs[i].first.length()) continue;
    for (bool taken = true; taken && node < 99;) {
      taken = false;
      for (const auto &r : rows) if (r.node == node) { taken = true; node++; break; }
    }
    bool sent = recs[i].second == "1";
    rows.push_back({(uint8_t)node++, recs[i].first, hauntStr(sent ? manTable.sentIdle : manTable.crewIdle),
                    hauntStr(sent ? manTable.sentActivity : manTable.crewActivity)});
  }
}

// ---------- who may use it ------------------------------------------------------------------------
static bool manifestListed(const Session &s) {
  if (!s.authenticated || isMaintenance(s.user) || !canWrite(s)) return false;
  return manifest::available(manTable, doorFlagsOf(s));
}
static bool manifestNever(const Session &) { return false; }   // reached by R, never from the door menu

// ---------- screens ------------------------------------------------------------------------------
static std::vector<const manifest::Row *> manRows(const Session &s) { return manifest::shownRows(manTable, doorFlagsOf(s)); }
static String manOptionText(const manifest::Row &r, int slot, const String &picks) {
  manifest::Picks3 p = manifest::picksOf(hauntStd(picks), r.id);
  const manifest::Option *o = p[slot].empty() ? nullptr : manifest::findOption(manTable, p[slot]);
  return o ? hauntStr(manifest::optionText(*o, r)) : String();
}
static void manPrompt(Session &s) { prompt(s, "\x1b[1;36mMANIFEST> \x1b[0m"); }
static void manSay(Session &s, const String &text) { say(s, "\x1b[0;37m" + text + "\x1b[0m"); }

static void manOverview(Session &s) {
  s.manMode = 0; s.digitHot = true;
  std::string flags = doorFlagsOf(s);
  auto rows = manRows(s);
  say(s, "");
  unsigned done = 0;
  for (size_t i = 0; i < rows.size(); ++i) {
    const manifest::Row &r = *rows[i];
    bool locked = manifest::rowDone(r, flags);
    if (locked) done++;
    unsigned n = manifest::pickedCount(manTable, r, hauntStd(s.manPicks));
    String state = locked ? String("\x1b[1;32mLOCKED\x1b[0m") : n ? String(n) + " of 3 filled" : String("\x1b[0;90mblank\x1b[0m");
    char row[120];
    snprintf(row, sizeof(row), "  \x1b[1;33m[%u]\x1b[0m %-10s %s", (unsigned)(i + 1), r.handle.c_str(), state.c_str());
    say(s, row);
  }
  say(s, "");
  say(s, "  \x1b[1;33m[S]\x1b[0m File the manifest   \x1b[1;33m[N]\x1b[0m Notebook   \x1b[1;33m[Q]\x1b[0m Back");
  manPrompt(s);
}

static void manRowView(Session &s) {
  s.manMode = 1; s.digitHot = true;
  std::string flags = doorFlagsOf(s);
  auto rows = manRows(s);
  if (s.manRow >= rows.size()) { manOverview(s); return; }
  const manifest::Row &r = *rows[s.manRow];
  bool locked = manifest::rowDone(r, flags);
  say(s, "");
  say(s, "\x1b[1;36m  " + hauntStr(r.handle) + "\x1b[0m" + (locked ? "  \x1b[1;32mLOCKED\x1b[0m" : ""));
  for (int i = 0; i < manifest::SLOTS; ++i) {
    String pick = manOptionText(r, i, s.manPicks);
    String label = hauntStr(manifest::slotLabel(manTable, r, i));
    say(s, locked ? "   " + label : "  \x1b[1;33m[" + String(i + 1) + "]\x1b[0m " + label);
    say(s, "      " + (pick.length() ? "\x1b[0;37m" + pick + "\x1b[0m" : String("\x1b[0;90m(blank)\x1b[0m")));
  }
  say(s, "  \x1b[1;33m[Q]\x1b[0m Back");
  manPrompt(s);
}

static void manPickView(Session &s) {
  s.manMode = 2; s.digitHot = false;
  auto rows = manRows(s);
  if (s.manRow >= rows.size()) { manOverview(s); return; }
  const manifest::Row &r = *rows[s.manRow];
  auto opts = manifest::choices(manTable, r, s.manSlot, doorFlagsOf(s));
  s.digitHot = opts.size() <= 9;   // a one-digit number is a key; ten or more are typed, then ENTER
  manifest::Picks3 p = manifest::picksOf(hauntStd(s.manPicks), r.id);
  say(s, "");
  say(s, "\x1b[1;36m  " + hauntStr(r.handle) + ": " + hauntStr(manifest::slotLabel(manTable, r, s.manSlot)) + "\x1b[0m");
  for (size_t i = 0; i < opts.size(); ++i) {
    bool mine = p[s.manSlot] == opts[i]->id;
    say(s, "  \x1b[1;33m[" + String((unsigned)(i + 1)) + "]\x1b[0m " + (mine ? "\x1b[1;37m" : "\x1b[0;37m") + hauntStr(manifest::optionText(*opts[i], r)) + (mine ? " (picked)" : "") + "\x1b[0m");
  }
  if (!opts.size()) {
    say(s, "  Nothing to choose from yet.");
    const std::string &hint = manifest::emptyHint(manTable, r, s.manSlot);
    if (!hint.empty()) {
      bool first = true;
      for (const std::string &row : wrap::rows(hint, 66)) {
        say(s, first ? "  \x1b[0;33mNotebook:\x1b[0;37m " + String(row.c_str()) + "\x1b[0m" : "            \x1b[0;37m" + String(row.c_str()) + "\x1b[0m");
        first = false;
      }
    }
  }
  say(s, "  \x1b[1;33m[0]\x1b[0m Clear this blank   \x1b[1;33m[Q]\x1b[0m Back");
  manPrompt(s);
}

// The notebook (Q21): what the board has shown the caller, in the order seen. Screens in bbs/notebook.h.
static void notebookOpen(Session &s);
static void notebookKey(Session &s, const String &cmd);

// ---------- the door ---------------------------------------------------------------------------------
static bool ownsManifest(const Session &s) { return s.state == MANIFEST_PROMPT; }

static void enterManifest(Session &s) {
  s.state = MANIFEST_PROMPT; s.hideInput = false; s.manRow = 0; s.manSlot = 0;
  s.manPicks = manPicksLoad(s.user.id);
  activity(s, "CREW MANIFEST");
  event("MANIFEST", s.user.handle + " node " + s.node + " opened the manifest");
  if (s.ansi) artOnce(s, "/BBS/ANSI/DOORS/MANIFEST.ANS", hauntStr(manTable.title));
  else plainArt(s, "/BBS/ASCII/DOORS/MANIFEST.ASC", hauntStr(manTable.title));
  if (manTable.intro.length()) manSay(s, manLines(hauntStr(manTable.intro)));
  manOverview(s);
}

// Filing: pairs of right rows are confirmed, then the last row. Flags are saved
// before anything is said; the answer holds only the names just confirmed and the count.
static void manFile(Session &s) {
  manifest::Filed f = manifest::file(manTable, doorFlagsOf(s), hauntStd(s.manPicks));
  if (f.grant.length() && !sceneGrant(s, f.grant)) {
    event("MANIFEST_FAIL", s.user.handle + ": rows not confirmed, flags not saved");
    manSay(s, "The manifest could not be filed just now.");
    manOverview(s);
    return;
  }
  event("MANIFEST", s.user.handle + " node " + s.node + " filed: " + String((unsigned)f.names.size()) + " new, " + String(f.confirmed) + " of " + String(f.total));
  say(s, "");
  manSay(s, manLines(hauntStr(manifest::filedText(manTable, f))));
  if (f.full && manTable.fullScene.length()) {
    sceneStart(s, hauntStr(manTable.fullScene));   // takes the screen; its end is the main menu
    if (s.state == SCENE_PLAY) return;
  }
  manOverview(s);
}

static void handleManifest(Session &s, String &, const String &cmd) {
  auto rows = manRows(s);
  switch (s.manMode) {
    case 0: {
      if (cmd == "Q" || cmd == "X" || cmd == "QUIT" || cmd == "EXIT") { event("MANIFEST", s.user.handle + " node " + s.node + " left the manifest"); mainMenu(s); return; }
      if (!cmd.length() || cmd == "?") { manOverview(s); return; }
      if (cmd == "S") { manFile(s); return; }
      if (cmd == "N") { notebookOpen(s); return; }
      long n = cmd.toInt();
      if (n >= 1 && (size_t)n <= rows.size() && String(n) == cmd) { s.manRow = (uint8_t)(n - 1); manRowView(s); return; }
      manSay(s, "Keys: a row number, S file, N notebook, Q back.");
      manPrompt(s);
      return;
    }
    case 1: {
      if (cmd == "Q") { manOverview(s); return; }
      if (cmd == "X") { mainMenu(s); return; }
      if (!cmd.length() || cmd == "?") { manRowView(s); return; }
      bool locked = s.manRow < rows.size() && manifest::rowDone(*rows[s.manRow], doorFlagsOf(s));
      long n = cmd.toInt();
      if (!locked && n >= 1 && n <= manifest::SLOTS && String(n) == cmd) { s.manSlot = (uint8_t)(n - 1); manPickView(s); return; }
      manSay(s, locked ? "Q goes back." : "Choose 1 to 3, or Q.");
      manPrompt(s);
      return;
    }
    case 2: {
      if (cmd == "Q") { manRowView(s); return; }
      if (cmd == "X") { mainMenu(s); return; }
      if (!cmd.length() || cmd == "?") { manPickView(s); return; }
      if (s.manRow >= rows.size()) { manOverview(s); return; }
      const manifest::Row &r = *rows[s.manRow];
      auto opts = manifest::choices(manTable, r, s.manSlot, doorFlagsOf(s));
      long n = cmd.toInt();
      std::string optionId;
      bool valid = (cmd == "0") || (n >= 1 && (size_t)n <= opts.size() && String(n) == cmd);
      if (!valid) { manSay(s, "Choose a number from the list, 0 to clear, or Q."); manPrompt(s); return; }
      if (n >= 1) optionId = opts[(size_t)n - 1]->id;
      std::string next = hauntStd(s.manPicks);
      if (!manifest::setPick(next, r.id, s.manSlot, optionId) || !manPicksSave(s, hauntStr(next))) {
        manSay(s, "The manifest could not be saved. The blank is unchanged.");
        manPrompt(s);
        return;
      }
      s.manPicks = hauntStr(next);
      manRowView(s);
      return;
    }
    default:   // modes 3 to 6: the notebook (bbs/notebook.h)
      notebookKey(s, cmd);
      return;
  }
}

// Goodbye without finishing: ELIZA thanks the caller for their interest. Nothing more.
static void manifestPartial(Session &s) {
  if (!manifestListed(s) || manifest::finished(doorFlagsOf(s)) || manTable.partial.empty()) return;
  event("ENDING", s.user.handle + " node " + s.node + " left with an incomplete manifest");
  if (s.ansi) art(s, "/BBS/ANSI/STORY/END_PARTIAL.ANS");
  else plainArt(s, "/BBS/ASCII/STORY/END_PARTIAL.ASC");
  say(s, "\x1b[1;33mELIZA:\x1b[0;33m " + manLines(hauntStr(manTable.partial), "\r\n       ") + "\x1b[0m");
}
