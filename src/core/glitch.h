#pragma once
// Glitches (S7): the board side of STORY/GLITCH.INI. The decisions are in
// core/glitch_logic.h (tested on the PC). Once a second, a caller on their own
// account who is at a calm menu prompt may get one slipped line, if the card's
// quiet time and gap allow. Telnet gets a bell, the browser the item's cue. No
// flags, no saves: a glitch leaves nothing behind. No platform headers.
// Part of the shared core: included from src/bbs_core.h after core/ghost.h.

#include "glitch_logic.h"

static constexpr const char *GLITCH_INI = "/BBS/STORY/GLITCH.INI";
static glitch::Table glitchTable;

static void loadGlitches() {
  Ini ini;
  glitchTable = glitch::Table();
  if (!iniLoad(GLITCH_INI, ini)) return;
  std::vector<haunt::Kv> kv;
  kv.reserve(ini.size());
  for (const auto &e : ini) kv.push_back({hauntStd(e.section), hauntStd(e.key), hauntStd(e.value)});
  glitch::parse(kv, glitchTable);
  for (const auto &p : glitchTable.problems) event("GLITCH_SKIP", hauntStr(p));
}

// From bbsTick, after ghostTick. Cheap when the card has no glitches.
static void glitchTick(uint32_t now) {
  static uint32_t last = 0;
  if (glitchTable.items.empty() || now - last < 1000) return;
  last = now;
  for (auto &s : sessions) {
    if (!s.used || !s.authenticated || s.readOnly || s.state == GOODBYE || hauntMaint(s.user)) continue;
    uint32_t signed_ = s.signedAt ? s.signedAt : s.connectedAt;
    uint32_t online = msSince(now, signed_) / 1000;
    if (!glitch::allowed(glitchTable, online, s.glitchAt != 0, s.glitchAt ? msSince(now, s.glitchAt) / 1000 : 0)) continue;
    if (!s.hauntShows.empty() || !hauntCalm(s)) continue;   // a page or a scene goes first
    int i = glitch::pick(glitchTable, gateFlags(s), online / 60, s.glitchNext);
    if (i < 0) continue;
    const glitch::Item &it = glitchTable.items[i];
    std::string text = it.garble ? glitch::garble(it.text, now / 1000 + s.node) : it.text;
    s.glitchAt = now ? now : 1;
    s.glitchNext = (size_t)i + 1;
    event("GLITCH", hauntStr(it.id) + " " + s.user.handle + " node " + s.node);
    if (s.web) soundCue(s, hauntStr(it.sound));
    else write(s, "\a");
    say(s, "\r\n\x1b[1;30m" + hauntStr(text) + "\x1b[0m");
  }
}
