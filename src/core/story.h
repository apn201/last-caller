#pragma once
// The story record: a caller's story progress (evidence, markers, confirmed
// manifest rows, endings) in DATA/STATE/STORY.DAT, one line per account:
// "id|FLAG,FLAG". The account's own flag field in USERS.DAT keeps only the
// flags that open something by themselves (SYSOP, READONLY, BACKROOM, VAULT;
// haunt::accessFlag). The record is written with putRecord(), so a failed
// write leaves the old one. Flags only grow. No platform headers.
// Part of the shared core: included in order from src/bbs_core.h, after
// core/users.h.

#include "haunt_logic.h"

static constexpr const char *STORY_FILE = "/BBS/DATA/STATE/STORY.DAT";

static std::string storyStd(const String &s) { return std::string(s.c_str(), s.length()); }
static String storyStr(const std::string &s) { return String(s.c_str()); }

// The story flags saved for an account ("" if none).
static String storyRead(uint32_t id) {
  if (!sdReady || !id) return "";
  File f = SD_MMC.open(STORY_FILE, FILE_READ);
  String out;
  while (f && f.available()) {
    String line = f.readStringUntil('\n');
    line.trim();
    if (strtoul(line.c_str(), nullptr, 10) != id) continue;
    int bar = line.indexOf('|');
    out = bar >= 0 ? line.substring(bar + 1) : String();
    break;
  }
  if (f) f.close();
  return out;
}
static void storyLoad(User &u) { u.story = storyRead(u.id); }
static bool storySave(const User &u) {
  if (!u.id) return false;
  return putRecord(STORY_FILE, u.id, u.story.length() ? String(u.id) + "|" + u.story : String());
}

// Every flag the account holds, as one list: its flag field and its story record.
static std::string userFlagList(const User &u) {
  return haunt::flagsClean(storyStd(u.flags) + "," + storyStd(u.story));
}

// On sign-in: load the story record, then move story flags out of the flag
// field (accounts from before the record existed). Nothing is lost: the
// flag field is only cleared once the record is saved.
static void storyOpen(User &u) {
  storyLoad(u);
  haunt::Split sp = haunt::splitFlags(storyStd(u.flags));
  if (sp.story.empty()) return;
  std::string dropped;
  std::string merged = haunt::flagsAdd(storyStd(u.story), sp.story, &dropped);
  if (!dropped.empty()) { event("STORY_FULL", u.handle + ": no room for " + storyStr(dropped)); return; }
  String before = u.story;
  u.story = storyStr(merged);
  if (!storySave(u)) { u.story = before; event("STORY_FAIL", u.handle + ": flags not moved"); return; }
  u.flags = cleanFlags(storyStr(sp.access));
  event("STORY_MOVED", u.handle + ": story flags moved to the story record");
}
