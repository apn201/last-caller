#pragma once
// Ghosts and ELIZA's scripted voice: the board side of STORY/GHOSTS.INI and
// STORY/SYSOP.INI. The decisions are in core/ghost_logic.h (tested on the PC);
// this loads the files, builds the context for a caller, adds ghost rows to
// WHO, pages the caller, answers in chat after a typing delay and speaks
// lines keyed by a trigger rule. No platform headers.
// Part of the shared core: included from src/bbs_core.h after core/scene.h.
//
// A ghost is not a session: it takes no node, cannot be disconnected and is
// never in the chat log. A page, and every once-only line, is given to a caller
// at most once: its marker (G_<GHOST>, E_<LINE>) is saved with its flags first,
// and nothing is shown unless that save worked. Flags only grow.

#include "ghost_logic.h"
#include <algorithm>

static constexpr const char *GHOSTS_INI = "/BBS/STORY/GHOSTS.INI";
static constexpr const char *SYSOP_INI = "/BBS/STORY/SYSOP.INI";
static constexpr size_t GHOST_QUEUE_MAX = 6;   // parts waiting to be typed for one caller
static ghost::Table ghostTable;
static constexpr const char *RIVE_DIR = "/BBS/STORY/ELIZA";   // G2: her brain, *.rive
static constexpr size_t RIVE_FILE_MAX = 24 * 1024;
static rive::Brain elizaBrain;
static constexpr const char *RIVE_VARS_FILE = "/BBS/DATA/STATE/ELIZA.DAT";   // G2: what each caller told her, "id|name=..;.."
static uint32_t ghostFirstPage = 0;            // the smallest page_after, 0 = no ghost pages

static void chatSpeak(Session &s, const String &from, const String &line);  // bbs/chat.h
static void leaveChat(Session &s, const char *why);  // bbs/chat.h
static void mainMenu(Session &s);  // bbs/board.h

static void ghostLoadFile(const char *path, bool isGhosts) {
  Ini ini;
  if (!iniLoad(path, ini)) return;
  std::vector<haunt::Kv> kv;
  kv.reserve(ini.size());
  for (const auto &e : ini) kv.push_back({hauntStd(e.section), hauntStd(e.key), hauntStd(e.value)});
  if (isGhosts) ghost::parseGhosts(kv, ghostTable); else ghost::parseLines(kv, ghostTable);
}

// G2: STORY/ELIZA/*.rive, in name order, into one brain. A card without them has the fixed fallback lines of SYSOP.INI only.
static String loadBrain() {
  elizaBrain = rive::Brain();
  if (!sdReady) return "";
  File dir = SD_MMC.open(RIVE_DIR);
  if (!dir || !dir.isDirectory()) { if (dir) dir.close(); return ""; }
  std::vector<String> names;
  File e;
  while ((e = dir.openNextFile())) {
    String n = e.name();
    bool plain = !e.isDirectory();
    e.close();
    if (plain && up(n).endsWith(".RIVE")) names.push_back(n);
  }
  dir.close();
  std::sort(names.begin(), names.end(), [](const String &a, const String &b) { return up(a) < up(b); });
  for (const auto &n : names) {
    String path = String(RIVE_DIR) + "/" + n;
    File f = SD_MMC.open(path.c_str(), FILE_READ);
    if (!f) continue;
    if (f.size() > RIVE_FILE_MAX) { f.close(); event("RIVE_SKIP", n + ": over 24 KB"); continue; }
    String text = f.readString();
    f.close();
    rive::load(elizaBrain, hauntStd(text));
  }
  rive::finish(elizaBrain);
  for (const auto &p : elizaBrain.problems) event("RIVE_SKIP", hauntStr(p));
  return String((unsigned)elizaBrain.trig.size()) + " brain triggers, " + String((unsigned)elizaBrain.replies) + " replies";
}

// At boot and on RELOAD, from loadSettings(). A card without the files has no ghosts and no scripted voice.
static String loadGhosts() {
  ghostTable = ghost::Table();
  ghostFirstPage = 0;
  ghostLoadFile(GHOSTS_INI, true);
  ghostLoadFile(SYSOP_INI, false);
  for (const auto &p : ghostTable.problems) event("GHOST_SKIP", hauntStr(p));
  for (const auto &g : ghostTable.ghosts) if (g.pageAfter && (!ghostFirstPage || g.pageAfter < ghostFirstPage)) ghostFirstPage = g.pageAfter;
  String out = String((unsigned)ghostTable.ghosts.size()) + " ghosts, " + String((unsigned)ghostTable.lines.size()) + " SysOp lines";
  String brain = loadBrain();
  if (brain.length()) out += ", " + brain;
  if (ghostTable.skipped) out += " (" + String(ghostTable.skipped) + " skipped, see the event log)";
  return out;
}

// The caller's flags: the live session's copy for an own account, else what the trigger engine sees.
static std::string ghostFlags(Session &s) {
  if (s.authenticated && !s.readOnly) return userFlagList(s.user);
  return sceneFlagsOf(s);
}
static ghost::Ctx ghostCtx(Session &s, const std::string &flags) {
  uint32_t now = millis();
  ghost::Ctx c;
  c.flags = flags;
  c.onlineMin = msSince(now, s.connectedAt) / MS_PER_MINUTE;
  c.signedSec = s.signedAt ? msSince(now, s.signedAt) / 1000 : 0;
  c.powered = true;  // ghosts only reach callers who have power (the maintenance login is skipped)
  return c;
}
static ghost::Ctx ghostCtx(Session &s) { return ghostCtx(s, ghostFlags(s)); }

// ---------- WHO and chat ---------------------------------------------------------------------
struct WhoRow { uint8_t node = 0; String handle, idle, activity; };
static void manifestStickyRows(Session &viewer, std::vector<WhoRow> &rows);  // doors/manifest.h: callers who finished the game

// Adds the ghosts this caller can see. A ghost never takes a node a real
// session is on. The rows are left in node order.
static void ghostWhoRows(Session &viewer, std::vector<WhoRow> &rows) {
  if (!ghostTable.ghosts.empty()) {
    ghost::Ctx c = ghostCtx(viewer);
    for (const auto &g : ghostTable.ghosts) {
      if (!ghost::visible(g, c)) continue;
      bool taken = false;
      for (const auto &r : rows) if (r.node == g.node) { taken = true; break; }
      if (taken) continue;
      rows.push_back({(uint8_t)g.node, hauntStr(g.handle), hauntStr(g.idle.empty() ? std::string("0:00") : g.idle), hauntStr(g.activity)});
      notesOn(viewer, notes::K_GHOST, hauntStr("WHO_" + g.id));   // Q21: a ghost in WHO is in the notebook
    }
  }
  manifestStickyRows(viewer, rows);   // finished callers stay (they take a free node from 60 up)
  std::stable_sort(rows.begin(), rows.end(), [](const WhoRow &a, const WhoRow &b) { return a.node < b.node; });
}
// Handles shown in node chat to this caller besides the real ones.
static std::vector<String> ghostsInChat(Session &viewer) {
  std::vector<String> out;
  if (ghostTable.ghosts.empty()) return out;
  for (const auto &h : ghost::inChat(ghostTable, ghostCtx(viewer))) out.push_back(hauntStr(h));
  return out;
}

// ---------- pages ----------------------------------------------------------------------------
// Queue a page or line for the next calm prompt. False if the queue is full.
static bool ghostPage(Session &s, const std::string &from, const std::string &text, notes::Kind noteKind = notes::K_NONE, const std::string &noteId = std::string()) {
  if (s.hauntShows.size() >= 8) return false;
  HauntShow h;
  h.from = hauntStr(from.empty() ? std::string("ELIZA") : from);
  h.text = hauntStr(text);
  h.noteKind = noteKind; h.noteId = hauntStr(noteId);
  s.hauntShows.push_back(h);
  return true;
}

// Lines keyed by a trigger rule that just fired. `flags` are the caller's flags after the rule.
static void ghostTrigger(Session &s, const std::string &ruleId, const std::string &flags) {
  if (ghostTable.lines.empty()) return;
  ghost::Ctx c = ghostCtx(s, flags);
  for (const ghost::Line *l : ghost::triggerLines(ghostTable, c, ruleId)) {
    if (s.hauntShows.size() >= 8) break;
    if (!sceneGrant(s, ghost::lineFlags(*l))) { event("GHOST_FAIL", hauntStr(l->id) + " " + s.user.handle + ": flags not saved, nothing said"); continue; }
    event("GHOST", hauntStr(l->id) + " " + s.user.handle + " node " + s.node + " (trigger " + hauntStr(ruleId) + ")");
    ghostPage(s, l->from, l->say, notes::K_ELIZA, l->id);
  }
}

// ---------- chat -----------------------------------------------------------------------------
// Queue what ELIZA (or another chat ghost) says, part by part, each after its typing time.
static void ghostSpeak(Session &s, const ghost::Line &l, const char *why) {
  std::vector<ghost::Beat> beats = ghost::beats(l);
  if (beats.empty()) return;
  if (!sceneGrant(s, ghost::lineFlags(l))) { event("GHOST_FAIL", hauntStr(l.id) + " " + s.user.handle + ": flags not saved, nothing said"); return; }
  event("GHOST", hauntStr(l.id) + " " + s.user.handle + " node " + s.node + " (" + why + ")");
  uint32_t at = s.ghostSay.empty() ? millis() : s.ghostSay.back().dueAt;
  for (const auto &b : beats) {
    at += b.wait;
    GhostLine g;
    g.dueAt = at; g.from = hauntStr(l.from); g.text = hauntStr(b.text); g.id = hauntStr(l.id);
    if (&b == &beats.back()) g.then = hauntStr(l.then);   // G2: an option that starts a scene does it after the last part
    s.ghostSay.push_back(g);
  }
}
// G2: what the brain says (or a list), queued like a line of the card but with no flags and no id.
static void ghostQueueText(Session &s, const std::string &text, const std::string &from = "ELIZA") {
  ghost::Line l;
  l.from = from; l.say = text;
  std::vector<ghost::Beat> beats = ghost::beats(l);
  uint32_t at = s.ghostSay.empty() ? millis() : s.ghostSay.back().dueAt;
  for (const auto &b : beats) {
    at += b.wait;
    GhostLine g;
    g.dueAt = at; g.from = hauntStr(from); g.text = hauntStr(b.text);
    s.ghostSay.push_back(g);
  }
}
// G2: what an own account told her (its name and the like) is kept on the board, one line per account.
// Crew accounts and the maintenance login keep nothing.
static uint32_t riveOwner(Session &s) { return s.authenticated && !s.readOnly ? s.user.id : 0; }
static void riveVarsLoad(Session &s) {
  s.rive.vars.clear();
  uint32_t id = riveOwner(s);
  if (!sdReady || !id) return;
  File f = SD_MMC.open(RIVE_VARS_FILE, FILE_READ);
  while (f && f.available()) {
    String line = f.readStringUntil('\n');
    line.trim();
    if (strtoul(line.c_str(), nullptr, 10) != id) continue;
    int bar = line.indexOf('|');
    if (bar >= 0) s.rive.vars = rive::decodeVars(hauntStd(line.substring(bar + 1)));
    break;
  }
  if (f) f.close();
}
static void riveVarsSave(Session &s, const std::string &before) {
  uint32_t id = riveOwner(s);
  if (!id) return;
  std::string now = rive::encodeVars(s.rive.vars);
  if (now == before) return;
  if (!putRecord(RIVE_VARS_FILE, id, now.empty() ? String() : String(id) + "|" + hauntStr(now))) event("RIVE_FAIL", s.user.handle + ": what she was told is not saved");
}

// The caller has just entered chat (bbs/chat.h startChat).
static void ghostChatJoined(Session &s) {
  s.ghostSay.clear();
  s.chatOpts.clear();
  s.rive.topic = "random";   // G2: a new visit starts in the main topic; what she was told stays
  if (!s.rive.rng || s.rive.rng == 12345) s.rive.rng = millis() | 1;
  riveVarsLoad(s);
  if (ghostTable.lines.empty() || !s.authenticated) return;
  for (const ghost::Line *l : ghost::joinLines(ghostTable, ghostCtx(s))) ghostSpeak(s, *l, "joined chat");
}
// G2: `help`, `?` or `suggest`: the options that fit now, numbered, in her voice. A number or the words picks one.
static void ghostHelp(Session &s, const ghost::Ctx &c) {
  std::vector<const ghost::Line *> opts = ghost::options(ghostTable, c);
  if (opts.size() > 9) opts.resize(9);
  s.chatOpts.clear();
  const ghost::Line *intro = ghost::firstOf(ghostTable, c, opts.empty() ? ghost::K_NOHELP : ghost::K_HELP);
  if (intro) { ghostSpeak(s, *intro, "chat help"); }
  uint32_t at = s.ghostSay.empty() ? millis() : s.ghostSay.back().dueAt;
  for (size_t i = 0; i < opts.size(); ++i) {
    at += 250;
    GhostLine g;
    g.dueAt = at; g.from = hauntStr(opts[i]->from);
    g.text = "\x1b[1;33m[" + String((unsigned)(i + 1)) + "]\x1b[0m " + hauntStr(opts[i]->option);
    s.ghostSay.push_back(g);
    s.chatOpts.push_back(hauntStr(opts[i]->id));
  }
}
// The caller has said something in chat (bbs/chat.h chatCommand). G2: one free-text chat. Her answer to the newest line
// is the next thing on the screen: anything still queued from an older line is dropped, so answers never arrive late
// or out of order. Order of answering: help, a number, the story's own lines (SYSOP.INI), her brain (STORY/ELIZA),
// then the fixed fallbacks. Free chat sets no flag of its own; only a line's own set= and an option's then= do.
// What she had not finished saying is shown now, in order. True if it started a scene (the chat is gone).
static bool ghostStartThen(Session &s, const GhostLine &g);
static bool ghostFlush(Session &s) {
  while (!s.ghostSay.empty()) {
    GhostLine g = s.ghostSay.front();
    s.ghostSay.erase(s.ghostSay.begin());
    chatSpeak(s, g.from, g.text);
    if (g.id.length()) notesOn(s, notes::K_ELIZA, g.id);
    if (ghostStartThen(s, g)) return true;
  }
  return false;
}
static void ghostChatHeard(Session &s, const String &line) {
  if (ghostTable.lines.empty() || !s.authenticated) return;
  if (ghostFlush(s)) return;
  const std::string text = haunt::trim(hauntStd(line));
  if (text.empty()) return;
  const ghost::Ctx c = ghostCtx(s);
  const std::string lastId = hauntStd(s.ghostLastId);
  if (ghost::wantsHelp(text)) { ghostHelp(s, c); s.ghostRepeat = 0; s.ghostLastId = "~"; return; }
  const ghost::Line *l = nullptr;
  const int n = ghost::pickNumber(text);
  if (n) {
    if ((size_t)n <= s.chatOpts.size()) {
      l = ghost::byId(ghostTable, hauntStd(s.chatOpts[n - 1]));
      if (l && !ghost::lineDue(*l, c)) l = nullptr;
      if (l && l->id == lastId) { const ghost::Line *a = ghost::againLine(ghostTable, c, s.ghostRepeat); if (a) l = a; }
    }
    if (!l) l = ghost::firstOf(ghostTable, c, ghost::K_NONUM);
  } else {
    l = ghost::keysReply(ghostTable, c, text, lastId, s.ghostRepeat);
    if (!l && !elizaBrain.empty() && ghost::present(ghostTable, c, "ELIZA")) {
      const std::string before = rive::encodeVars(s.rive.vars);
      rive::Result r = rive::reply(elizaBrain, s.rive, c.flags, text);
      riveVarsSave(s, before);
      if (r.matched) {
        if (r.repeated) l = ghost::againLine(ghostTable, c, s.ghostRepeat);
        if (!l) {
          s.ghostRepeat = 0; s.ghostLastId = "~";
          event("GHOST", "BRAIN " + s.user.handle + " node " + s.node + " (brain)");
          ghostQueueText(s, r.text);
          return;
        }
      }
    }
    if (!l) l = ghost::elseReply(ghostTable, c, s.ghostTurn, lastId, s.ghostRepeat);
  }
  if (!l) return;
  if (l->kind == ghost::K_ELSE) s.ghostTurn++;
  if (l->kind == ghost::K_AGAIN) s.ghostRepeat++; else { s.ghostRepeat = 0; s.ghostLastId = hauntStr(l->id); }
  ghostSpeak(s, *l, l->kind == ghost::K_ELSE ? "chat fallback" : "chat keyword");
}

// G2: a line with then= (the memory check): once its last part is said, the chat closes and the scene plays.
static bool ghostStartThen(Session &s, const GhostLine &g) {
  if (!g.then.length()) return false;
  s.ghostSay.clear();
  leaveChat(s, "left the chat.");
  sceneStart(s, g.then);
  if (s.state != SCENE_PLAY) mainMenu(s);
  return true;
}

// From bbsTick: print an answer when its typing time is over; once a second, give pages.
static void ghostTick(uint32_t now) {
  static uint32_t lastPage = 0;
  bool pages = ghostFirstPage && now - lastPage >= 1000;
  if (pages) lastPage = now;
  for (auto &s : sessions) {
    if (!s.used) continue;
    if (!s.ghostSay.empty()) {
      if (s.state != CHAT) s.ghostSay.clear();   // left chat: the answer is dropped
      else if ((int32_t)(now - s.ghostSay.front().dueAt) >= 0) {
        GhostLine g = s.ghostSay.front();
        s.ghostSay.erase(s.ghostSay.begin());
        chatSpeak(s, g.from, g.text);
        if (g.id.length()) notesOn(s, notes::K_ELIZA, g.id);   // Q21: said, so noted
        if (ghostStartThen(s, g)) continue;
      }
    }
    if (!pages || !s.authenticated || s.state == GOODBYE || s.state == SCENE_PLAY || hauntMaint(s.user)) continue;
    if (!s.signedAt || msSince(now, s.signedAt) / 1000 < ghostFirstPage) continue;
    ghost::Ctx c = ghostCtx(s);
    for (const auto &g : ghostTable.ghosts) {
      if (!ghost::pageDue(g, c)) continue;
      if (s.hauntShows.size() >= 8) break;
      if (!sceneGrant(s, ghost::pageFlags(g))) { event("GHOST_FAIL", hauntStr(g.id) + " " + s.user.handle + ": flags not saved, nothing shown"); continue; }
      event("GHOST", hauntStr(g.id) + " " + s.user.handle + " node " + s.node + " (page)");
      ghostPage(s, g.pageFrom, g.page, notes::K_GHOST, g.id);
      c.flags = ghostFlags(s);
    }
  }
}
