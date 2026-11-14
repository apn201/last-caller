#pragma once
// Triggers: the board side of STORY/TRIGGERS.INI. The decisions are in
// core/haunt_logic.h (tested on the PC); this loads the rules, works out who
// the caller is, saves their flags and runs the effects. No platform headers.
// Part of the shared core: included from src/bbs_core.h after bbs/settings.h,
// because it needs the conferences.
//
// The caller is always the caller's own account. On a read-only crew login the
// flags go to the own account signed in earlier on this connection, or wait in
// Session.carriedFlags for the next own login or sign-up (power prologue R8.4).
// Flags only grow. A rule that shows something fires once per caller: its
// marker T_<RULE> is saved with its flags, and nothing is shown unless that
// save worked. Pages and art wait until the caller is at a menu prompt.

#include "haunt_logic.h"

static constexpr const char *TRIGGERS_INI = "/BBS/STORY/TRIGGERS.INI";
static haunt::Table hauntRules;
static std::vector<uint32_t> hauntIdleAt, hauntMinutesAt;  // after= values of the timed rules

static void sceneStart(Session &s, const String &name);  // core/scene.h
static void ghostTrigger(Session &s, const std::string &ruleId, const std::string &flags);  // core/ghost.h

static std::string hauntStd(const String &s) { return std::string(s.c_str(), s.length()); }
static String hauntStr(const std::string &s) { return String(s.c_str()); }

// At boot and on RELOAD, from loadSettings(). A card without the file has no triggers.
static String loadTriggers() {
  Ini ini;
  hauntRules = haunt::Table();
  hauntIdleAt.clear(); hauntMinutesAt.clear();
  if (!iniLoad(TRIGGERS_INI, ini)) return "no triggers";
  std::vector<haunt::Kv> kv;
  kv.reserve(ini.size());
  for (const auto &e : ini) kv.push_back({hauntStd(e.section), hauntStd(e.key), hauntStd(e.value)});
  haunt::parse(kv, hauntRules);
  for (const auto &p : hauntRules.problems) event("TRIGGER_SKIP", hauntStr(p));
  for (const auto &r : hauntRules.rules) {
    if (r.ev == haunt::EV_IDLE) hauntIdleAt.push_back(r.after);
    if (r.ev == haunt::EV_MINUTES) hauntMinutesAt.push_back(r.after);
  }
  String out = String((unsigned)hauntRules.rules.size()) + " triggers";
  if (hauntRules.skipped) out += " (" + String(hauntRules.skipped) + " skipped, see the event log)";
  return out;
}

// The ship's maintenance login plays no part in the story.
static bool hauntMaint(const User &u) { return powerEnabled && u.normalized == up(powerCfg.maintAccount); }

// The caller's own account record, as it is now: the live session's copy if it
// is online (its counters are newer than the card), else the card's.
static bool hauntOwner(const Session &s, User &own) {
  uint32_t id = s.authenticated && !s.readOnly ? s.user.id : s.ownUserId;
  if (!id) return false;
  for (auto &n : sessions) if (n.used && n.authenticated && !n.readOnly && n.user.id == id) { own = n.user; return true; }
  if (!findUserById(id, own)) return false;
  storyLoad(own);   // the story record is not in USERS.DAT
  return true;
}

// The flags a gate (a `Needs:` line, NEEDS.BBS) checks for this session. An own account: its
// flag field and story record. A read-only crew login: its own, what was earned on it and not
// yet carried, and the flags of the own account signed in earlier on this connection (read
// at most once a second and a half: a list checks one message after another).
static std::string gateFlags(const Session &s) {
  std::string f = userFlagList(s.user);
  if (!s.readOnly) return f;
  f = haunt::flagsAdd(f, hauntStd(s.carriedFlags));
  if (s.ownUserId) {
    uint32_t now = millis();
    if (!s.gateAt || now - s.gateAt > 1500) {
      User own;
      s.gateOwn = hauntOwner(s, own) ? hauntStr(userFlagList(own)) : String();
      s.gateAt = now ? now : 1;
    }
    f = haunt::flagsAdd(f, hauntStd(s.gateOwn));
  }
  return f;
}
static bool gatePassFor(const Session &s, const String &needs) {
  return !needs.length() || haunt::gatePass(gateFlags(s), hauntStd(needs));
}

// Add flags to the caller's story record (core/story.h). Saved before anything
// is shown; false = not saved.
static bool hauntGrant(Session &s, User *own, const std::string &add) {
  std::string dropped;
  if (!own) {
    s.carriedFlags = hauntStr(haunt::flagsAdd(hauntStd(s.carriedFlags), add, &dropped));
  } else {
    std::string before = hauntStd(own->story), after = haunt::flagsAdd(before, add, &dropped);
    if (after != before) {
      String saved = own->story;
      own->story = hauntStr(after);
      if (!storySave(*own)) { own->story = saved; return false; }
      // Every online session of that account gets the new flags, so it sees them at once.
      for (auto &n : sessions) if (n.used && n.authenticated && n.user.id == own->id) n.user.story = own->story;
    }
  }
  if (dropped.length()) event("TRIGGER_FULL", (own ? own->handle : String("(carried)")) + ": no room for " + hauntStr(dropped));
  return true;
}

// A conference by number or name; the caller's own one if the card names none that exists.
static uint8_t hauntConference(const std::string &want, uint8_t fallback) {
  String w = hauntStr(want); w.trim();
  if (!w.length()) return fallback;
  for (const auto &c : board.conferences) if (up(c.name) == up(w) || String(c.id) == w) return c.id;
  event("TRIGGER_CONF", "no conference " + w);
  return fallback;
}
static String hauntBody(const std::string &text) { String b = hauntStr(text); b.replace("|", "\n"); return b; }

static bool hauntMessage(const std::string &from, const String &to, const std::string &conf, const std::string &subject,
                         const std::string &body, bool priv, uint8_t fallback) {
  Message m;
  m.conference = hauntConference(conf, fallback);
  m.from = hauntStr(from); m.to = to; m.priv = priv;
  m.subject = hauntStr(subject); m.body = hauntBody(body); m.when = nowText();
  return appendMessage(m);
}

// Something happened to this caller. `adds` collects the lines read rules add
// to the message being shown. `amount` is idle seconds or minutes online.
static void hauntEvent(Session &s, haunt::Ev ev, const String &match, uint32_t amount = 0, std::vector<String> *adds = nullptr) {
  if (hauntRules.rules.empty() || !s.authenticated || hauntMaint(s.user)) return;
  User own;
  bool haveOwn = hauntOwner(s, own);
  haunt::Ctx c;
  c.ev = ev; c.match = hauntStd(match); c.amount = amount;
  c.baud = s.cps ? s.cps * 10 : 0;
  c.flags = haveOwn ? userFlagList(own) : hauntStd(s.carriedFlags);
  c.level = haveOwn ? own.level : s.user.level;
  for (const haunt::Rule *r : haunt::due(hauntRules, c)) {
    if (!r->add.empty()) { if (adds) adds->push_back(hauntStr(r->add)); continue; }
    // Mail and posts need somewhere to go: a crew visit with no own account waits.
    if ((!r->mailBody.empty() || !r->postBody.empty()) && !haveOwn) continue;
    if (!hauntGrant(s, haveOwn ? &own : nullptr, haunt::flagsOf(*r))) {
      event("TRIGGER_FAIL", hauntStr(r->id) + " " + s.user.handle + ": flags not saved, nothing shown");
      continue;
    }
    event("TRIGGER", hauntStr(r->id) + " " + (haveOwn ? own.handle : s.user.handle + " (carried)") + " node " + s.node);
    if (!r->mailBody.empty() && !hauntMessage(r->mailFrom, own.handle, r->mailConf, r->mailSubject, r->mailBody, true, s.conference))
      event("TRIGGER_FAIL", hauntStr(r->id) + ": mail not written");
    if (!r->postBody.empty() && !hauntMessage(r->postFrom, "", r->postConf, r->postSubject, r->postBody, false, s.conference))
      event("TRIGGER_FAIL", hauntStr(r->id) + ": post not written");
    if (!r->page.empty() || !r->art.empty() || !r->scene.empty()) {
      HauntShow h;
      h.from = hauntStr(r->pageFrom.empty() ? std::string("ELIZA") : r->pageFrom);
      h.text = hauntStr(r->page); h.art = hauntStr(r->art); h.scene = hauntStr(r->scene);
      if (s.hauntShows.size() < 8) s.hauntShows.push_back(h);
    }
    // ELIZA's lines keyed by this rule (STORY/SYSOP.INI). They save flags to the
    // account, so `own` is read again before the next rule touches it.
    ghostTrigger(s, r->id, haunt::flagsAdd(c.flags, haunt::flagsOf(*r)));
    if (haveOwn) haveOwn = hauntOwner(s, own);
  }
}

// event=screen: the user list, a bulletin ("BULLETIN <TITLE>") was shown.
static void hauntScreen(Session &s, const String &what) { hauntEvent(s, haunt::EV_SCREEN, what); }

// An own account signed in or signed up on this connection: it is now the
// caller, and gets what was earned on crew accounts before it. Call before the
// login's own save, so both go in one write.
static void hauntOwnLogin(Session &s) {
  s.ownUserId = s.user.id; s.ownHandle = s.user.handle;
  if (!s.carriedFlags.length()) return;
  std::string dropped;
  String before = s.user.story;
  s.user.story = hauntStr(haunt::flagsAdd(hauntStd(s.user.story), hauntStd(s.carriedFlags), &dropped));
  if (!storySave(s.user)) {   // the visit stays carried until a save works
    s.user.story = before;
    event("TRIGGER_FAIL", s.user.handle + ": carried flags not saved");
    return;
  }
  if (dropped.length()) event("TRIGGER_FULL", s.user.handle + ": no room for " + hauntStr(dropped));
  event("TRIGGER_CARRY", s.user.handle + " gets " + s.carriedFlags);
  s.carriedFlags = "";
}

// A menu prompt with nothing streaming and nothing typed: a page will not
// break a picture, the reader or a half-typed line.
static bool hauntCalm(const Session &s) {
  switch (s.state) {
    case MAIN: case MSG_MENU: case FILE_MENU: case FILE_LIST: case DOOR_MENU: case ANSI_GALLERY:
    case BULLETIN: case BULLETIN_LIST: case JOIN_CONF: break;
    default: return false;
  }
  return !streaming(s) && !s.input.length() && !s.hideInput;
}
static void hauntShow(Session &s) {
  // A scene takes the whole screen: whatever is queued behind it waits for the next calm prompt,
  // and, after a scene, for the caller's next command, so two scenes never play back to back.
  if (s.sceneHold) return;
  std::vector<HauntShow> shows;
  shows.swap(s.hauntShows);
  for (size_t i = 0; i < shows.size(); ++i) {
    const HauntShow &h = shows[i];
    if (h.text.length()) {
      String text = h.text; text.replace("|", "\r\n  ");
      say(s, "\r\n\x1b[1;33m" + h.from + ":\x1b[0;33m " + text + "\x1b[0m");
    }
    if (h.art.length()) {
      if (s.ansi) art(s, ("/BBS/ANSI/STORY/" + h.art + ".ANS").c_str());
      else plainArt(s, ("/BBS/ASCII/STORY/" + h.art + ".ASC").c_str());
    }
    if (h.scene.length()) {
      sceneStart(s, h.scene);
      if (s.state == SCENE_PLAY) {
        s.sceneHold = true;
        for (size_t k = i + 1; k < shows.size(); ++k) s.hauntShows.push_back(shows[k]);
        return;
      }
    }
  }
}

// Did the count go past one of the thresholds since the last look?
static bool hauntCrossed(const std::vector<uint32_t> &at, uint32_t before, uint32_t now) {
  for (uint32_t a : at) if (a > before && a <= now) return true;
  return false;
}

// From bbsTick: idle and minutes rules, then the waiting shows. Once a second.
static void hauntTick(uint32_t now) {
  static uint32_t last = 0;
  if (now - last < 1000) return;
  last = now;
  for (auto &s : sessions) {
    if (!s.used || !s.authenticated || s.state == GOODBYE || hauntMaint(s.user)) continue;
    uint32_t idle = (now - s.lastAt) / 1000, minutes = (now - (s.signedAt ? s.signedAt : s.connectedAt)) / 60000;   // minutes since sign-in
    if (idle < s.hauntIdleSec) s.hauntIdleSec = 0;  // a key was pressed: a new idle stretch
    if (!hauntIdleAt.empty() && hauntCrossed(hauntIdleAt, s.hauntIdleSec, idle)) hauntEvent(s, haunt::EV_IDLE, "", idle);
    s.hauntIdleSec = idle;
    if (!hauntMinutesAt.empty() && hauntCrossed(hauntMinutesAt, s.hauntMinute, minutes)) hauntEvent(s, haunt::EV_MINUTES, "", minutes);
    s.hauntMinute = minutes;
    if (!s.hauntShows.empty() && hauntCalm(s)) hauntShow(s);
  }
}
