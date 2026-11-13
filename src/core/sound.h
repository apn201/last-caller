#pragma once
// Cues: short sounds the browser terminal plays on the board's request. The
// files are on the card (/BBS/ASSETS/SFX/<NAME>.WAV, made by tools/mod/gen_sfx.py)
// and the browser fetches them from /sfx/<NAME>.WAV. Telnet callers get nothing.
// A cue is sent at the event, never on a timer, and only if the file exists, so
// a card without the sounds is silent and nothing else changes.
// Part of the shared core: included from src/bbs_core.h after core/modinfo_logic.h.

static constexpr const char *SFX_DIR = "/BBS/ASSETS/SFX/";

// "chirp" -> "CHIRP"; "" if it is not a plain name (letters, digits, _; at most 16).
static String sfxName(const String &raw) {
  String n = raw; n.trim(); n.toUpperCase();
  if (!n.length() || n.length() > 16) return "";
  for (size_t i = 0; i < n.length(); ++i) {
    char c = n[i];
    if (!((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_')) return "";
  }
  return n;
}
static bool sfxExists(const String &name) {
  return sdReady && SD_MMC.exists((String(SFX_DIR) + name + ".WAV").c_str());
}
static void soundCue(Session &s, const String &raw) {
  if (!s.web) return;
  String name = sfxName(raw);
  if (!name.length() || !sfxExists(name)) return;
  write(s, "\x1b]9;SND;" + name + "\x07");
}
