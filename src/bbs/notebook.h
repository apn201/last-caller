#pragma once
// The notebook screens (Q21), reached from the crew manifest (N). The notes themselves are
// collected as things are shown (core/notes.h) and kept as flags on the caller's account; this
// lists what the caller holds, in the order seen, six to a page, and shows one again:
//   mode 3  the notes, a page at a time        keys: 1-6 open, N next page, P previous page,
//                                                    S scenes seen, L logins that worked, Q back
//   mode 4  one note: where it came from, where to read it, R shows it again
//   mode 5  scenes seen, newest last           keys: a number replays, N, P, Q
//   mode 6  logins that worked (Addition 6)
// A replay (a scene, a page of ELIZA's, a picture) grants nothing and changes no flag. It keeps what
// was seen and never adds an answer. Part of the shared core: included from src/bbs_core.h after
// bbs/messages.h. No platform headers.

static constexpr uint8_t NB_PAGE = 6;   // notes to a page: three lines at most each

static std::vector<const notes::Note *> notebookHeld(const Session &s) { return notes::held(notesTable, doorFlagsOf(s)); }

// The scenes the caller has seen and that can be played again: the scene's marker flag, in the
// order the flags were saved, and a title on the card. Names only.
static std::vector<String> notebookScenes(const Session &s) {
  std::vector<String> out;
  if (!sdReady) return out;
  for (const std::string &w : haunt::flagWords(doorFlagsOf(s))) {
    if (w.size() < 4 || w.compare(0, 3, "SC_") != 0) continue;
    String name = hauntStr(w.substr(3));
    if (SD_MMC.exists((String(SCENE_DIR) + name + ".INI").c_str())) out.push_back(name);
  }
  return out;
}
static String notebookSceneTitle(const String &name) {
  Ini ini;
  if (!iniLoad((String(SCENE_DIR) + name + ".INI").c_str(), ini)) return "";
  return iniGet(ini, "SCENE", "title", "");
}

// Where a message is: the area and its number. A file is named by the card (where=).
static String notebookWhere(const Session &s, const notes::Note &n) {
  if (n.kind == notes::K_READ) {
    String want = hauntStr(n.match);
    bool bySubject = want.startsWith("~");
    if (bySubject) want = want.substring(1);
    uint32_t number = bySubject ? 0 : (uint32_t)want.toInt();
    String found;
    eachMessage([&](const Message &m) {
      if (m.deleted || !canRead(s, m)) return true;
      bool hit = bySubject ? up(m.subject.substring(0, 39)).startsWith(want) : (number && msgNo(m.id) == number);
      if (!hit) return true;
      found = confName(m.conference) + ", message " + String(msgNo(m.id));
      return false;
    });
    return found;
  }
  return hauntStr(n.where);
}

// Can this note be shown again? Only what the card still has, and for a scene only one the caller has seen.
static bool notebookCanReplay(const Session &s, const notes::Note &n) {
  if (n.replay == notes::R_NONE) return false;
  String name = hauntStr(n.replayName);
  switch (n.replay) {
    case notes::R_SCENE:
      return haunt::flagsHas(doorFlagsOf(s), scene::marker(n.replayName)) && SD_MMC.exists((String(SCENE_DIR) + name + ".INI").c_str());
    case notes::R_PAGE:
      for (const auto &r : hauntRules.rules) if (r.id == n.replayName) return !r.page.empty() || !r.art.empty();
      return false;
    case notes::R_ELIZA:
      for (const auto &l : ghostTable.lines) if (l.id == n.replayName) return !l.say.empty();
      return false;
    case notes::R_GHOST:
      for (const auto &g : ghostTable.ghosts) if (g.id == n.replayName) return !g.page.empty();
      return false;
    case notes::R_ART:
      return SD_MMC.exists((s.ansi ? "/BBS/ANSI/STORY/" + name + ".ANS" : "/BBS/ASCII/STORY/" + name + ".ASC").c_str());
    default: return false;
  }
}

static String notebookHead(const String &title, const String &more = String()) {
  return "\r\n\x1b[1;36m  " + title + "\x1b[0m" + (more.length() ? "  \x1b[0;90m" + more + "\x1b[0m" : String());
}
static void notebookRows(Session &s, const String &text, const char *color, const char *indent) {
  for (const std::string &row : wrap::rows(std::string(text.c_str()), 68)) say(s, String(color) + indent + String(row.c_str()) + "\x1b[0m");
}
static void notebookKeys(Session &s, const String &keys) { say(s, keys); manPrompt(s); }

static void notebookLogins(Session &s) {
  s.manMode = 6; s.digitHot = false;
  std::vector<User> known = crewKnown(s);   // Q20 (Addition 6): logins that worked, with their passwords
  say(s, notebookHead("LOGINS THAT WORKED"));
  for (const User &u : known) {
    String pw = crewKnownPassword(u);
    char row[160];
    snprintf(row, sizeof(row), "  \x1b[1;33m%-9s\x1b[0m %s", u.handle.c_str(),
             pw.length() ? ("Logged in with " + pw + ". L on the main menu opens it.").c_str() : "Logged in once. L on the main menu opens it.");
    say(s, row);
  }
  notebookKeys(s, "  \x1b[1;33m[Q]\x1b[0m Back");
}

static void notebookList(Session &s) {
  s.manMode = 3;
  auto held = notebookHeld(s);
  size_t pages = held.empty() ? 1 : (held.size() + NB_PAGE - 1) / NB_PAGE;
  if (s.manPage >= pages) s.manPage = (uint8_t)(pages - 1);
  size_t first = (size_t)s.manPage * NB_PAGE, last = std::min(held.size(), first + NB_PAGE);
  s.digitHot = last > first;
  say(s, notebookHead("NOTEBOOK", held.empty() ? String() : "in the order you saw them, " + String((unsigned)held.size()) + " kept" + (pages > 1 ? ", page " + String((unsigned)s.manPage + 1) + " of " + String((unsigned)pages) : String())));
  if (held.empty()) say(s, "  \x1b[0;90mNothing written down yet. The board writes down what it shows you.\x1b[0m");
  for (size_t i = first; i < last; ++i) {
    const notes::Note &n = *held[i];
    say(s, "  \x1b[1;33m[" + String((unsigned)(i - first + 1)) + "]\x1b[0m \x1b[0;90m" + hauntStr(n.src) + "\x1b[0m");
    notebookRows(s, hauntStr(n.text), "\x1b[0;37m", "      ");
  }
  String keys = "  ";
  if (last > first) keys += "\x1b[1;33m[1-" + String((unsigned)(last - first)) + "]\x1b[0m Open   ";
  if (s.manPage + 1 < pages) keys += "\x1b[1;33m[N]\x1b[0m Next page   ";
  if (s.manPage > 0) keys += "\x1b[1;33m[P]\x1b[0m Previous page   ";
  if (!notebookScenes(s).empty()) keys += "\x1b[1;33m[S]\x1b[0m Scenes seen   ";
  if (!crewKnown(s).empty()) keys += "\x1b[1;33m[L]\x1b[0m Logins   ";
  keys += "\x1b[1;33m[Q]\x1b[0m Back";
  notebookKeys(s, keys);
}

static void notebookOpen(Session &s) { s.manPage = 0; notebookList(s); }

static void notebookNote(Session &s) {
  s.manMode = 4; s.digitHot = false;
  auto held = notebookHeld(s);
  if (s.manNote >= held.size()) { notebookList(s); return; }
  const notes::Note &n = *held[s.manNote];
  say(s, notebookHead("NOTEBOOK", String((unsigned)s.manNote + 1) + " of " + String((unsigned)held.size())));
  say(s, "  \x1b[1;33m" + hauntStr(n.src) + "\x1b[0m");
  notebookRows(s, hauntStr(n.text), "\x1b[1;37m", "  ");
  String where = notebookWhere(s, n);
  if (where.length()) say(s, "  \x1b[0;90mTo read it again: " + where + "\x1b[0m");
  String keys = "  ";
  bool again = notebookCanReplay(s, n);
  if (again) keys += "\x1b[1;33m[R]\x1b[0m Show it again   ";
  if (s.manNote + 1 < held.size()) keys += "\x1b[1;33m[N]\x1b[0m Next   ";
  if (s.manNote > 0) keys += "\x1b[1;33m[P]\x1b[0m Previous   ";
  keys += "\x1b[1;33m[Q]\x1b[0m Back";
  notebookKeys(s, keys);
}

static void notebookSceneList(Session &s) {
  s.manMode = 5;
  std::vector<String> names = notebookScenes(s);
  if (names.empty()) { notebookList(s); return; }
  size_t pages = (names.size() + 8) / 9;
  if (s.manPage >= pages) s.manPage = (uint8_t)(pages - 1);
  size_t first = (size_t)s.manPage * 9, last = std::min(names.size(), first + 9);
  s.digitHot = true;
  say(s, notebookHead("SCENES SEEN", "newest last" + (pages > 1 ? ", page " + String((unsigned)s.manPage + 1) + " of " + String((unsigned)pages) : String())));
  for (size_t i = first; i < last; ++i) {
    String title = notebookSceneTitle(names[i]);
    say(s, "  \x1b[1;33m[" + String((unsigned)(i - first + 1)) + "]\x1b[0m \x1b[0;37m" + (title.length() ? title : names[i]) + "\x1b[0m");
  }
  say(s, "  \x1b[0;90mA replay shows the scene as it was. It changes nothing.\x1b[0m");
  String keys = "  \x1b[1;33m[1-" + String((unsigned)(last - first)) + "]\x1b[0m Replay   ";
  if (s.manPage + 1 < pages) keys += "\x1b[1;33m[N]\x1b[0m Next page   ";
  if (s.manPage > 0) keys += "\x1b[1;33m[P]\x1b[0m Previous page   ";
  keys += "\x1b[1;33m[Q]\x1b[0m Back";
  notebookKeys(s, keys);
}

// A replay of a scene is over (Q or the end): the screen it was started from is drawn again.
static void notebookBack(Session &s) {
  s.state = MANIFEST_PROMPT; s.hideInput = false;
  activity(s, "CREW MANIFEST");
  write(s, "\r\n");
  if (s.manMode == 5) notebookSceneList(s);
  else if (s.manMode == 4) notebookNote(s);
  else notebookList(s);
}

// ELIZA's page, a rule's page or a ghost's, shown as it was.
static void notebookPage(Session &s, const String &from, const String &text) {
  String t = text; t.replace("|", "\r\n  ");
  say(s, "\r\n\x1b[1;33m" + from + ":\x1b[0;33m " + t + "\x1b[0m");
}
static bool notebookReplay(Session &s, const notes::Note &n) {
  if (!notebookCanReplay(s, n)) return false;
  String name = hauntStr(n.replayName);
  switch (n.replay) {
    case notes::R_SCENE: return sceneReplayStart(s, name);   // ends in notebookBack
    case notes::R_PAGE:
      for (const auto &r : hauntRules.rules) {
        if (r.id != n.replayName) continue;
        if (!r.page.empty()) notebookPage(s, hauntStr(r.pageFrom.empty() ? std::string("ELIZA") : r.pageFrom), hauntStr(r.page));
        if (!r.art.empty()) { if (s.ansi) art(s, ("/BBS/ANSI/STORY/" + hauntStr(r.art) + ".ANS").c_str()); else plainArt(s, ("/BBS/ASCII/STORY/" + hauntStr(r.art) + ".ASC").c_str()); }
      }
      break;
    case notes::R_ELIZA:
      for (const auto &l : ghostTable.lines) if (l.id == n.replayName) notebookPage(s, hauntStr(l.from), hauntStr(l.say));
      break;
    case notes::R_GHOST:
      for (const auto &g : ghostTable.ghosts) if (g.id == n.replayName) notebookPage(s, hauntStr(g.pageFrom.empty() ? g.handle : g.pageFrom), hauntStr(g.page));
      break;
    case notes::R_ART:
      if (s.ansi) art(s, ("/BBS/ANSI/STORY/" + name + ".ANS").c_str()); else plainArt(s, ("/BBS/ASCII/STORY/" + name + ".ASC").c_str());
      break;
    default: return false;
  }
  notebookNote(s);
  return true;
}

// The keys of the notebook screens (modes 3 to 6 of the manifest door).
static void notebookKey(Session &s, const String &cmd) {
  if (s.manMode == 6) { notebookList(s); return; }   // any key goes back
  if (cmd == "X") { mainMenu(s); return; }
  long n = cmd.toInt();
  bool number = n >= 1 && n <= 9 && String(n) == cmd;
  if (s.manMode == 3) {
    auto held = notebookHeld(s);
    if (cmd == "Q") { manOverview(s); return; }
    if (!cmd.length() || cmd == "?") { notebookList(s); return; }
    if (cmd == "N" && (size_t)(s.manPage + 1) * NB_PAGE < held.size()) { s.manPage++; notebookList(s); return; }
    if (cmd == "P" && s.manPage > 0) { s.manPage--; notebookList(s); return; }
    if (cmd == "S" && !notebookScenes(s).empty()) { s.manPage = 0; notebookSceneList(s); return; }
    if (cmd == "L" && !crewKnown(s).empty()) { notebookLogins(s); return; }
    size_t at = (size_t)s.manPage * NB_PAGE + (size_t)n - 1;
    if (number && n <= NB_PAGE && at < held.size()) { s.manNote = (uint16_t)at; notebookNote(s); return; }
    manSay(s, "Keys: a number to open, N, P, S, L, Q back.");
    manPrompt(s);
    return;
  }
  if (s.manMode == 4) {
    auto held = notebookHeld(s);
    if (cmd == "Q") { notebookList(s); return; }
    if (!cmd.length() || cmd == "?") { notebookNote(s); return; }
    if (cmd == "N" && (size_t)s.manNote + 1 < held.size()) { s.manNote++; notebookNote(s); return; }
    if (cmd == "P" && s.manNote > 0) { s.manNote--; notebookNote(s); return; }
    if (cmd == "R" && s.manNote < held.size() && notebookReplay(s, *held[s.manNote])) return;
    manSay(s, "Keys: R shows it again where it can, N, P, Q back.");
    manPrompt(s);
    return;
  }
  // mode 5: scenes seen
  std::vector<String> names = notebookScenes(s);
  if (cmd == "Q") { s.manPage = 0; notebookList(s); return; }
  if (!cmd.length() || cmd == "?") { notebookSceneList(s); return; }
  size_t pages = (names.size() + 8) / 9;
  if (cmd == "N" && (size_t)s.manPage + 1 < pages) { s.manPage++; notebookSceneList(s); return; }
  if (cmd == "P" && s.manPage > 0) { s.manPage--; notebookSceneList(s); return; }
  size_t at = (size_t)s.manPage * 9 + (size_t)n - 1;
  if (number && at < names.size() && sceneReplayStart(s, names[at])) return;
  manSay(s, "Keys: a number to replay, N, P, Q back.");
  manPrompt(s);
}
