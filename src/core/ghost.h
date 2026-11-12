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
static uint32_t ghostFirstPage = 0;            // the smallest page_after, 0 = no ghost pages

static void chatSpeak(Session &s, const String &from, const String &line);  // bbs/chat.h

static void ghostLoadFile(const char *path, bool isGhosts) {
  Ini ini;
  if (!iniLoad(path, ini)) return;
  std::vector<haunt::Kv> kv;
  kv.reserve(ini.size());
  for (const auto &e : ini) kv.push_back({hauntStd(e.section), hauntStd(e.key), hauntStd(e.value)});
  if (isGhosts) ghost::parseGhosts(kv, ghostTable); else ghost::parseLines(kv, ghostTable);
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
  c.onlineMin = (now - s.connectedAt) / 60000;
  c.signedSec = s.signedAt ? (now - s.signedAt) / 1000 : 0;
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
static bool ghostPage(Session &s, const std::string &from, const std::string &text) {
  if (s.hauntShows.size() >= 8) return false;
  HauntShow h;
  h.from = hauntStr(from.empty() ? std::string("ELIZA") : from);
  h.text = hauntStr(text);
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
    ghostPage(s, l->from, l->say);
  }
}

// ---------- chat -----------------------------------------------------------------------------
// Queue what ELIZA (or another chat ghost) says, part by part, each after its typing time.
static void ghostSpeak(Session &s, const ghost::Line &l, const char *why) {
  std::vector<ghost::Beat> beats = ghost::beats(l);
  if (beats.empty() || s.ghostSay.size() + beats.size() > GHOST_QUEUE_MAX) return;
  if (!sceneGrant(s, ghost::lineFlags(l))) { event("GHOST_FAIL", hauntStr(l.id) + " " + s.user.handle + ": flags not saved, nothing said"); return; }
  event("GHOST", hauntStr(l.id) + " " + s.user.handle + " node " + s.node + " (" + why + ")");
  uint32_t at = s.ghostSay.empty() ? millis() : s.ghostSay.back().dueAt;
  for (const auto &b : beats) {
    at += b.wait;
    GhostLine g;
    g.dueAt = at; g.from = hauntStr(l.from); g.text = hauntStr(b.text);
    s.ghostSay.push_back(g);
  }
}
// The caller has just entered chat (bbs/chat.h startChat).
static void ghostChatJoined(Session &s) {
  s.ghostSay.clear();
  if (ghostTable.lines.empty() || !s.authenticated) return;
  for (const ghost::Line *l : ghost::joinLines(ghostTable, ghostCtx(s))) ghostSpeak(s, *l, "joined chat");
}
// The caller has said something in chat (bbs/chat.h chatCommand).
static void ghostChatHeard(Session &s, const String &line) {
  if (ghostTable.lines.empty() || !s.authenticated || s.ghostSay.size() >= GHOST_QUEUE_MAX) return;
  const ghost::Line *l = ghost::chatReply(ghostTable, ghostCtx(s), hauntStd(line), s.ghostTurn);
  if (!l) return;
  if (l->kind == ghost::K_ELSE) s.ghostTurn++;
  ghostSpeak(s, *l, l->kind == ghost::K_ELSE ? "chat fallback" : "chat keyword");
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
      }
    }
    if (!pages || !s.authenticated || s.state == GOODBYE || s.state == SCENE_PLAY || hauntMaint(s.user)) continue;
    if (!s.signedAt || (now - s.signedAt) / 1000 < ghostFirstPage) continue;
    ghost::Ctx c = ghostCtx(s);
    for (const auto &g : ghostTable.ghosts) {
      if (!ghost::pageDue(g, c)) continue;
      if (s.hauntShows.size() >= 8) break;
      if (!sceneGrant(s, ghost::pageFlags(g))) { event("GHOST_FAIL", hauntStr(g.id) + " " + s.user.handle + ": flags not saved, nothing shown"); continue; }
      event("GHOST", hauntStr(g.id) + " " + s.user.handle + " node " + s.node + " (page)");
      ghostPage(s, g.pageFrom, g.page);
      c.flags = ghostFlags(s);
    }
  }
}
