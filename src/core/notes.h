#pragma once
// The notebook, board side (Q21): STORY/NOTES.INI and "this was just shown to the
// caller". The decisions are in core/notes_logic.h (tested on the PC). Showing a
// noted thing saves the flag N_<ID> on the caller's own account before anything
// else happens, once. A read-only crew login saves to the own account signed in
// earlier on this connection, or the flag waits in Session.carriedFlags like any
// other. A replay from the notebook saves nothing. No platform headers.
// Part of the shared core: included from src/bbs_core.h after core/haunt.h.
// The notebook screens are in bbs/notebook.h.

static constexpr const char *NOTES_INI = "/BBS/STORY/NOTES.INI";
static notes::Table notesTable;

// At boot and on RELOAD, from loadSettings(). A card without the file has no notebook.
static String loadNotes() {
  Ini ini;
  notesTable = notes::Table();
  if (!iniLoad(NOTES_INI, ini)) return "no notebook";
  std::vector<haunt::Kv> kv;
  kv.reserve(ini.size());
  for (const auto &e : ini) kv.push_back({hauntStd(e.section), hauntStd(e.key), hauntStd(e.value)});
  notes::parse(kv, notesTable);
  for (const auto &p : notesTable.problems) event("NOTE_SKIP", hauntStr(p));
  String out = String((unsigned)notesTable.notes.size()) + " notes";
  if (notesTable.skipped) out += " (" + String(notesTable.skipped) + " skipped, see the event log)";
  return out;
}

// Something was shown to this caller: a message, a file, a picture, a scene line, a page, a
// line of ELIZA's, an answer. Whatever notes it carries go into the notebook, in this order,
// and not again. The flags are saved first; a failed save is logged and the show goes on.
static void notesOn(Session &s, notes::Kind kind, const String &match) {
  if (notesTable.notes.empty() || kind == notes::K_NONE || !s.authenticated || s.sceneReplay || hauntMaint(s.user)) return;
  std::vector<const notes::Note *> due = notes::due(notesTable, kind, hauntStd(match), s.cps ? s.cps * 10 : 0, gateFlags(s), (int)s.conference);
  if (due.empty()) return;
  User own;
  bool haveOwn = hauntOwner(s, own);
  std::string add = notes::flagsOf(due);
  if (!hauntGrant(s, haveOwn ? &own : nullptr, add)) { event("NOTE_FAIL", s.user.handle + ": notes not saved: " + hauntStr(add)); return; }
  event("NOTE", hauntStr(add) + " " + (haveOwn ? own.handle : s.user.handle + " (carried)") + " node " + s.node);
}
