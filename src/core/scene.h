#pragma once
// Scenes: the board side of STORY/SCENES/*.INI. The decisions are in
// core/scene_logic.h (tested on the PC); this reads the file, starts a run for
// a caller, writes what the player says, saves the flags it grants and puts
// the caller back at the main menu when the scene ends. No platform headers.
// Part of the shared core: included from src/bbs_core.h after core/haunt.h,
// whose rules start scenes (scene=NAME).
//
// A scene starts only when the caller is at a calm prompt (haunt.h decides)
// and only once per caller unless the scene says repeat=1: its marker flag
// SC_<NAME> is saved first, and nothing is shown unless that save worked.

static constexpr const char *SCENE_DIR = "/BBS/STORY/SCENES/";
static constexpr size_t SCENE_MAX_BYTES = 24 * 1024;

static void mainMenu(Session &s);  // bbs/board.h
static void endingRecord(Session &s);  // doors/manifest.h

// "ELIZA_WAKE" -> a checked, upper-case name; "" if it is not a plain name.
static String sceneName(const String &raw) {
  String n = raw; n.trim(); n.toUpperCase();
  if (!n.length() || n.length() > 24) return "";
  return haunt::nameOk(hauntStd(n)) ? n : String();
}

// Read and check one scene. Null with `why` set if it cannot be played.
static std::shared_ptr<scene::Scene> sceneLoad(const String &name, String &why) {
  if (!sdReady) { why = "no card"; return nullptr; }
  String path = String(SCENE_DIR) + name + ".INI";
  File f = SD_MMC.open(path.c_str(), FILE_READ);
  if (!f || f.isDirectory()) { if (f) f.close(); why = "no such scene"; return nullptr; }
  size_t size = f.size();
  f.close();
  if (size > SCENE_MAX_BYTES) { why = "file over 24 KB"; return nullptr; }
  Ini ini;
  if (!iniLoad(path.c_str(), ini)) { why = "cannot read"; return nullptr; }
  std::vector<haunt::Kv> kv;
  kv.reserve(ini.size());
  for (const auto &e : ini) kv.push_back({hauntStd(e.section), hauntStd(e.key), hauntStd(e.value)});
  auto sc = std::make_shared<scene::Scene>();
  std::string problem;
  if (!scene::parse(hauntStd(name), kv, *sc, problem)) { why = hauntStr(problem); return nullptr; }
  return sc;
}

// The caller's flags as the triggers see them: the own account, or the flags a crew login carries.
static std::string sceneFlagsOf(Session &s) {
  User own;
  return hauntOwner(s, own) ? userFlagList(own) : hauntStd(s.carriedFlags);
}
static bool sceneGrant(Session &s, const std::string &add) {
  User own;
  bool have = hauntOwner(s, own);
  return hauntGrant(s, have ? &own : nullptr, add);
}

static void sceneEnd(Session &s) {
  s.sceneRun = scene::Player();
  s.hideInput = false;
  endingRecord(s);   // a finished caller stays in WHO
  mainMenu(s);
}

// What the player says goes to the caller; what it grants goes to the account.
static void sceneWrite(Session &s, const scene::Out &o) {
  for (const auto &p : o.parts) {
    if (!p.isArt) { write(s, hauntStr(p.s)); continue; }
    String file = hauntStr(p.s);
    if (s.ansi) art(s, ("/BBS/ANSI/STORY/" + file + ".ANS").c_str());
    else plainArt(s, ("/BBS/ASCII/STORY/" + file + ".ASC").c_str());
  }
  if (o.grant.size() && !sceneGrant(s, o.grant)) event("SCENE_FAIL", s.user.handle + ": flags not saved: " + hauntStr(o.grant));
  if (o.finished) { event("SCENE", s.user.handle + " node " + s.node + " finished a scene"); sceneEnd(s); }
}

static void sceneStart(Session &s, const String &raw) {
  if (!s.authenticated || hauntMaint(s.user) || s.state == SCENE_PLAY) return;
  String name = sceneName(raw), why;
  if (!name.length()) { event("SCENE_SKIP", raw + ": not a scene name"); return; }
  auto sc = sceneLoad(name, why);
  if (!sc) { event("SCENE_SKIP", name + ": " + why); return; }
  if (!scene::available(*sc, sceneFlagsOf(s))) return;  // seen already, or not for this caller yet
  const std::string mark = scene::marker(sc->id);
  if (!sceneGrant(s, mark) || !haunt::flagsHas(sceneFlagsOf(s), mark)) {
    event("SCENE_SKIP", name + " " + s.user.handle + ": marker not saved, nothing shown");
    return;
  }
  event("SCENE", s.user.handle + " node " + s.node + " starts " + name);
  s.hideInput = false;
  s.state = SCENE_PLAY;
  activity(s, "IN A SCENE");
  soundCue(s, hauntStr(sc->sound));
  sceneWrite(s, s.sceneRun.start(sc, sceneFlagsOf(s), s.ansi, millis()));
}

// From bbsTick, after hauntTick.
static void sceneTick(uint32_t now) {
  for (auto &s : sessions) {
    if (!s.used || s.state != SCENE_PLAY) continue;
    sceneWrite(s, s.sceneRun.tick(now));
  }
}

// The door-table side (doors/doors.h): a scene is entered by a trigger, never from a menu.
static bool ownsScene(const Session &s) { return s.state == SCENE_PLAY; }
static void sceneEnter(Session &) {}               // not reachable: listed nowhere
static bool sceneListed(const Session &) { return false; }
static void handleScene(Session &s, String &line, const String &) {
  sceneWrite(s, s.sceneRun.input(hauntStd(line), millis()));
}
