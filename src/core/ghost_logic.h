#pragma once
// Ghosts and ELIZA's scripted voice, as pure logic: the parsers for
// STORY/GHOSTS.INI and STORY/SYSOP.INI (from plain key/value triples), who is
// visible to a caller, which line answers a chat sentence, idle text and typing
// times. No String, no SD card, no sessions: the board wraps this in
// core/ghost.h. A plain C++ compiler tests it (tools/test/ghost_logic_test.cpp).
// Part of the shared core: included by core/ghost.h.
#include "haunt_logic.h"

namespace ghost {

using haunt::Kv;

static constexpr size_t MAX_OPTION = 60;
static constexpr size_t MAX_GHOSTS = 24, MAX_LINES = 200, MAX_KEYS = 12;
static constexpr size_t MAX_HANDLE = 16, MAX_ACTIVITY = 24, MAX_IDLE = 10, MAX_PAGE = 200, MAX_SAY = 600, MAX_KEY = 24;
static constexpr uint32_t MAX_NODE = 99, MAX_DELAY = 10000, DEFAULT_DELAY = 300;   // G2: was 1500; answers come right after the question

struct Ghost {
  std::string id;                 // upper case, letters digits _
  std::string handle, activity, idle;
  uint32_t node = 0;
  std::string has, lacks;
  uint32_t onlineMin = 0;         // minutes the caller has been connected
  bool powered = false;           // only while the board has power
  bool chat = false;              // named in node chat
  uint32_t pageAfter = 0;         // seconds after sign-in; 0 = never pages
  std::string pageFrom, page, set;
};

// G2: K_HELP says the options are coming (when there are some), K_NOHELP when there are none, K_NONUM answers a number that
// names nothing. K_OPTION is one numbered option of the help list: a label and the words its number types.
enum Kind : uint8_t { K_NONE, K_TRIGGER, K_KEYS, K_JOIN, K_ELSE, K_AGAIN, K_HELP, K_NOHELP, K_NONUM, K_OPTION };

struct Line {
  std::string id;
  std::string from = "ELIZA";
  std::string say;
  Kind kind = K_NONE;
  std::string trigger;            // rule id (upper case)
  std::vector<std::string> keys;  // normalised, padded with one space on each side
  std::string has, lacks, set;
  bool once = false;
  uint32_t delay = DEFAULT_DELAY; // ms before she starts typing
  std::string option;             // G2: chat=option lines: the label `help` lists; its number types the first of keys=
  std::string then;               // G2: keys= lines only: a scene that starts when she has said it (upper case)
  std::vector<std::string> says;  // G2: say=, say2=, say3=: a keys line asked again in other words answers with the next
};

struct Table {
  std::vector<Ghost> ghosts;
  std::vector<Line> lines;
  unsigned skipped = 0;
  std::vector<std::string> problems;  // one short line each, for the event log
  void problem(const std::string &id, const std::string &why) {
    skipped++;
    if (problems.size() < 20) problems.push_back((id.empty() ? std::string("(none)") : id) + ": " + why);
  }
};

// What the board knows about the caller. `flags` are the caller's flags now.
struct Ctx {
  std::string flags;
  uint32_t onlineMin = 0;   // minutes since connect
  uint32_t signedSec = 0;   // seconds since sign-in
  bool powered = true;
};

inline std::string markerGhost(const std::string &id) { return "G_" + haunt::upper(id); }
inline std::string markerLine(const std::string &id) { return "E_" + haunt::upper(id); }

// ---------- parsing ------------------------------------------------------------------------
inline std::string val(const std::vector<Kv> &kv, const std::string &key, bool *found = nullptr) {
  for (const auto &e : kv) if (haunt::lower(haunt::trim(e.key)) == key) { if (found) *found = true; return e.value; }
  if (found) *found = false;
  return "";
}

// Lower case, anything but a letter or a digit is a space, runs collapse, one
// space at each end: "Who are you?" -> " who are you ".
inline std::string norm(const std::string &s) {
  std::string out = " ";
  for (char c : s) {
    char l = (c >= 'A' && c <= 'Z') ? (char)(c - 'A' + 'a') : c;
    bool word = (l >= 'a' && l <= 'z') || (l >= '0' && l <= '9') || ((unsigned char)l >= 0x80);
    if (word) out += l;
    else if (out.back() != ' ') out += ' ';
  }
  if (out.back() != ' ') out += ' ';
  return out;
}

template <typename F> inline void eachSection(const std::vector<Kv> &entries, F fn) {
  size_t i = 0;
  while (i < entries.size()) {
    std::string name = haunt::trim(entries[i].section);
    std::vector<Kv> run;
    while (i < entries.size() && haunt::trim(entries[i].section) == name) run.push_back(entries[i++]);
    fn(name, run);
  }
}

inline bool buildGhost(const std::string &id, const std::vector<Kv> &kv, Ghost &g, std::string &why) {
  g = Ghost();
  if (!haunt::idOk(id)) { why = "bad ghost name"; return false; }
  g.id = haunt::upper(id);
  g.handle = haunt::capText(val(kv, "handle"), MAX_HANDLE);
  g.activity = haunt::capText(val(kv, "activity"), MAX_ACTIVITY);
  g.idle = haunt::capText(val(kv, "idle"), MAX_IDLE);
  g.has = haunt::flagsClean(val(kv, "has"));
  g.lacks = haunt::flagsClean(val(kv, "lacks"));
  g.set = haunt::flagsClean(val(kv, "set"));
  g.pageFrom = haunt::capText(val(kv, "page_from"), MAX_HANDLE);
  g.page = haunt::capText(val(kv, "page"), MAX_PAGE);
  g.powered = haunt::trim(val(kv, "powered")) == "1";
  g.chat = haunt::trim(val(kv, "chat")) == "1";
  if (g.handle.empty()) { why = "missing handle"; return false; }
  bool f = false;
  std::string node = val(kv, "node", &f);
  if (!f || !haunt::parseUint(node, MAX_NODE, g.node)) { why = "node must be 0 to 99"; return false; }
  std::string online = val(kv, "online_min", &f);
  if (f && !haunt::parseUint(online, 100000, g.onlineMin)) { why = "bad online_min"; return false; }
  std::string after = val(kv, "page_after", &f);
  if (f && (!haunt::parseUint(after, 86400, g.pageAfter) || !g.pageAfter)) { why = "page_after must be 1 to 86400"; return false; }
  if (g.pageAfter && g.page.empty()) { why = "page_after needs page"; return false; }
  if (!g.page.empty() && !g.pageAfter) { why = "page needs page_after"; return false; }
  if (g.pageFrom.empty()) g.pageFrom = g.handle;
  if (g.page.empty() && (!g.set.empty())) { why = "set is for the page (a ghost has no other effect)"; return false; }
  return true;
}

inline bool buildLine(const std::string &id, const std::vector<Kv> &kv, Line &l, std::string &why) {
  l = Line();
  if (!haunt::idOk(id)) { why = "bad line name"; return false; }
  l.id = haunt::upper(id);
  bool f = false;
  std::string from = haunt::capText(val(kv, "from", &f), MAX_HANDLE);
  if (f && !from.empty()) l.from = from;
  l.say = haunt::capText(val(kv, "say"), MAX_SAY);
  if (!l.say.empty()) l.says.push_back(l.say);
  for (const char *k : {"say2", "say3"}) { std::string v = haunt::capText(val(kv, k), MAX_SAY); if (!v.empty()) l.says.push_back(v); }
  l.has = haunt::flagsClean(val(kv, "has"));
  l.lacks = haunt::flagsClean(val(kv, "lacks"));
  l.set = haunt::flagsClean(val(kv, "set"));
  l.once = haunt::trim(val(kv, "once")) == "1";
  std::string delay = val(kv, "delay", &f);
  if (f && !haunt::parseUint(delay, MAX_DELAY, l.delay)) { why = "delay must be 0 to 10000"; return false; }
  l.option = haunt::capText(val(kv, "option"), MAX_OPTION);
  std::string then = haunt::upper(haunt::trim(val(kv, "then")));
  if (!then.empty()) { if (!haunt::idOk(then)) { why = "bad then"; return false; } l.then = then; }
  std::string trigger = haunt::upper(haunt::trim(val(kv, "trigger"))), keys = val(kv, "keys"), chat = haunt::lower(haunt::trim(val(kv, "chat")));
  const bool isOption = chat == "option";
  if (l.say.empty() && !isOption) { why = "missing say"; return false; }
  int ways = (!trigger.empty()) + (!haunt::trim(keys).empty() && !isOption) + (!chat.empty());
  if (ways != 1) { why = "exactly one of trigger, keys and chat"; return false; }
  if (!trigger.empty()) {
    if (!haunt::idOk(trigger)) { why = "bad trigger"; return false; }
    l.kind = K_TRIGGER; l.trigger = trigger; l.once = true;
  } else if (!chat.empty()) {
    if (chat == "join") { l.kind = K_JOIN; l.once = true; }
    else if (chat == "else") l.kind = K_ELSE;
    else if (chat == "again") l.kind = K_AGAIN;   // G1: said when the same answer would come a second time in a row
    else if (chat == "help") l.kind = K_HELP;
    else if (chat == "nohelp") l.kind = K_NOHELP;
    else if (chat == "nonum") l.kind = K_NONUM;
    else if (isOption) l.kind = K_OPTION;
    else { why = "chat must be join, else, again, help, nohelp, nonum or option"; return false; }
    if (!l.then.empty()) { why = "then is for keys lines"; return false; }
    if (isOption && (l.option.empty() || !l.say.empty())) { why = "an option has option= and keys=, no say"; return false; }
    if (!isOption && !l.option.empty()) { why = "option= is for chat=option lines"; return false; }
  }
  if (l.kind == K_OPTION || l.kind == K_NONE) {
    if (l.kind == K_NONE) l.kind = K_KEYS;
    std::string word;
    auto take = [&]() {
      std::string n = norm(word);
      word.clear();
      if (n == " ") return;
      if (n.size() - 2 > MAX_KEY) n = norm(n.substr(1, MAX_KEY));
      for (const auto &k : l.keys) if (k == n) return;
      if (l.keys.size() < MAX_KEYS) l.keys.push_back(n);
    };
    for (char c : keys) { if (c == ',') take(); else word += c; }
    take();
    if (l.keys.empty()) { why = "keys has no words"; return false; }
  }
  if (l.kind != K_KEYS && !l.then.empty()) { why = "then is for keys lines"; return false; }
  if (l.kind != K_OPTION && !l.option.empty()) { why = "option= is for chat=option lines"; return false; }
  return true;
}

inline void parseGhosts(const std::vector<Kv> &entries, Table &t) {
  t.ghosts.clear();
  eachSection(entries, [&](const std::string &name, const std::vector<Kv> &run) {
    if (name.empty()) { t.problem("", "lines outside a [GHOST] section"); return; }
    if (t.ghosts.size() >= MAX_GHOSTS) { t.problem(name, "too many ghosts"); return; }
    for (const auto &g : t.ghosts) if (g.id == haunt::upper(name)) { t.problem(name, "duplicate ghost name"); return; }
    Ghost g; std::string why;
    if (!buildGhost(name, run, g, why)) { t.problem(name, why); return; }
    for (const auto &o : t.ghosts) if (o.node == g.node) { t.problem(name, "node " + std::to_string(g.node) + " is taken by " + o.id); return; }
    t.ghosts.push_back(g);
  });
}

inline void parseLines(const std::vector<Kv> &entries, Table &t) {
  t.lines.clear();
  eachSection(entries, [&](const std::string &name, const std::vector<Kv> &run) {
    if (name.empty()) { t.problem("", "lines outside a [LINE] section"); return; }
    if (t.lines.size() >= MAX_LINES) { t.problem(name, "too many lines"); return; }
    for (const auto &l : t.lines) if (l.id == haunt::upper(name)) { t.problem(name, "duplicate line name"); return; }
    Line l; std::string why;
    if (!buildLine(name, run, l, why)) { t.problem(name, why); return; }
    t.lines.push_back(l);
  });
}

// ---------- deciding -----------------------------------------------------------------------
inline bool visible(const Ghost &g, const Ctx &c) {
  if (!g.has.empty() && !haunt::flagsHasAll(c.flags, g.has)) return false;
  if (!g.lacks.empty() && haunt::flagsHasAny(c.flags, g.lacks)) return false;
  if (c.onlineMin < g.onlineMin) return false;
  if (g.powered && !c.powered) return false;
  return true;
}
// Is this ghost's page due now? (Visible, time reached, not given before.)
inline bool pageDue(const Ghost &g, const Ctx &c) {
  return !g.page.empty() && visible(g, c) && c.signedSec >= g.pageAfter && !haunt::flagsHas(c.flags, markerGhost(g.id));
}
inline std::string pageFlags(const Ghost &g) { return haunt::flagsClean(g.set.empty() ? markerGhost(g.id) : g.set + "," + markerGhost(g.id)); }

// Flags a spoken line adds: its set= plus, for a once-only line, its marker.
inline std::string lineFlags(const Line &l) {
  return l.once ? haunt::flagsClean(l.set.empty() ? markerLine(l.id) : l.set + "," + markerLine(l.id)) : l.set;
}
inline bool lineDue(const Line &l, const Ctx &c) {
  if (!l.has.empty() && !haunt::flagsHasAll(c.flags, l.has)) return false;
  if (!l.lacks.empty() && haunt::flagsHasAny(c.flags, l.lacks)) return false;
  if (l.once && haunt::flagsHas(c.flags, markerLine(l.id))) return false;
  return true;
}
// Does the caller's sentence contain one of the line's keys as whole words?
inline bool hears(const Line &l, const std::string &text) {
  const std::string t = norm(text);
  for (const auto &k : l.keys) if (t.find(k) != std::string::npos) return true;
  return false;
}
// May this speaker be heard in chat? Only if a visible ghost with chat=1 has her handle.
inline bool present(const Table &t, const Ctx &c, const std::string &handle) {
  for (const auto &g : t.ghosts) if (g.chat && visible(g, c) && haunt::upper(g.handle) == haunt::upper(handle)) return true;
  return false;
}
// Handles of the ghosts shown in chat to this caller, in file order.
inline std::vector<std::string> inChat(const Table &t, const Ctx &c) {
  std::vector<std::string> out;
  for (const auto &g : t.ghosts) if (g.chat && visible(g, c)) out.push_back(g.handle);
  return out;
}
// The line that answers `text` in chat, or null. Keyword lines first, in file
// order; then the fallbacks in turn (`turn` counts fallbacks used so far).
// G1: never the same answer twice in a row. `lastId` is the line she said last; `repeats` how many
// "again" lines came since. When the only answer left is the one just said, the next chat=again
// line speaks instead (second time, third time, then round again); with none on the card
// the old behaviour stands.
inline const Line *againLine(const Table &t, const Ctx &c, uint32_t repeats) {
  std::vector<const Line *> again;
  for (const auto &l : t.lines) if (l.kind == K_AGAIN && lineDue(l, c) && present(t, c, l.from)) again.push_back(&l);
  return again.empty() ? nullptr : again[repeats % again.size()];
}
// G2: the keyword lines alone (the story's own lines win over her brain).
inline const Line *keysReply(const Table &t, const Ctx &c, const std::string &text,
                             const std::string &lastId = std::string(), uint32_t repeats = 0) {
  const Line *same = nullptr;
  for (const auto &l : t.lines)
    if (l.kind == K_KEYS && lineDue(l, c) && present(t, c, l.from) && hears(l, text)) {
      if (l.id != lastId) return &l;
      if (!same) same = &l;
    }
  if (same) { const Line *a = againLine(t, c, repeats); return a ? a : same; }
  return nullptr;
}
// The fixed fallback lines, in turn.
inline const Line *elseReply(const Table &t, const Ctx &c, uint32_t turn,
                             const std::string &lastId = std::string(), uint32_t repeats = 0) {
  std::vector<const Line *> fallback;
  for (const auto &l : t.lines) if (l.kind == K_ELSE && lineDue(l, c) && present(t, c, l.from)) fallback.push_back(&l);
  if (fallback.empty()) return nullptr;
  const Line *pick = fallback[turn % fallback.size()];
  if (pick->id == lastId) {
    for (size_t i = 1; i < fallback.size() && pick->id == lastId; ++i) pick = fallback[(turn + i) % fallback.size()];
    if (pick->id == lastId) { const Line *a = againLine(t, c, repeats); if (a) return a; }
  }
  return pick;
}
inline const Line *chatReply(const Table &t, const Ctx &c, const std::string &text, uint32_t turn,
                             const std::string &lastId = std::string(), uint32_t repeats = 0) {
  const Line *k = keysReply(t, c, text, lastId, repeats);
  return k ? k : elseReply(t, c, turn, lastId, repeats);
}
// G2: the options `help` lists: chat=option lines that are due now, in file order.
inline std::vector<const Line *> options(const Table &t, const Ctx &c) {
  std::vector<const Line *> out;
  for (const auto &l : t.lines) if (l.kind == K_OPTION && lineDue(l, c) && present(t, c, l.from)) out.push_back(&l);
  return out;
}
// The words an option's number stands for: its first key, as typed.
inline std::string optionWords(const Line &l) { return l.keys.empty() ? std::string() : haunt::trim(l.keys[0]); }
// G2: the first due keyword line that hears `text`, with no repeat logic (the board decides about repeats).
inline const Line *keysMatch(const Table &t, const Ctx &c, const std::string &text) {
  for (const auto &l : t.lines) if (l.kind == K_KEYS && lineDue(l, c) && present(t, c, l.from) && hears(l, text)) return &l;
  return nullptr;
}
// First due line of a kind (help, nohelp, nonum).
inline const Line *firstOf(const Table &t, const Ctx &c, Kind k) {
  for (const auto &l : t.lines) if (l.kind == k && lineDue(l, c) && present(t, c, l.from)) return &l;
  return nullptr;
}
inline const Line *byId(const Table &t, const std::string &id) {
  for (const auto &l : t.lines) if (l.id == id) return &l;
  return nullptr;
}
// Does the caller ask what she can do? "?", "help", "suggest", "what can I do" ...
inline bool wantsHelp(const std::string &text) {
  const std::string n = norm(text);
  if (n == " " && haunt::trim(text) == "?") return true;
  static const char *keys[] = {" help ", " suggest ", " suggestions ", " options ", " what can i do ", " what can you do ", " what do i do ", " what now ", " what next ", " i am stuck ", " im stuck "};
  for (const char *k : keys) if (n.find(k) != std::string::npos) return true;
  return false;
}
// A whole line that is one digit 1 to 9: the number of an option. 0 if it is not.
inline int pickNumber(const std::string &text) {
  std::string t = haunt::trim(text);
  if (t.size() == 2 && t[1] == '.') t.pop_back();
  return (t.size() == 1 && t[0] >= '1' && t[0] <= '9') ? t[0] - '0' : 0;
}
// Lines said when the caller enters chat (at most one per speaker is not enforced: the card decides).
inline std::vector<const Line *> joinLines(const Table &t, const Ctx &c) {
  std::vector<const Line *> out;
  for (const auto &l : t.lines) if (l.kind == K_JOIN && lineDue(l, c) && present(t, c, l.from)) out.push_back(&l);
  return out;
}
// Lines shown as a page after trigger rule `ruleId` fired. `c.flags` must be the flags after the rule.
inline std::vector<const Line *> triggerLines(const Table &t, const Ctx &c, const std::string &ruleId) {
  std::vector<const Line *> out;
  const std::string id = haunt::upper(ruleId);
  for (const auto &l : t.lines) if (l.kind == K_TRIGGER && l.trigger == id && lineDue(l, c)) out.push_back(&l);
  return out;
}

// ---------- typing -------------------------------------------------------------------------
// How long a part of `chars` characters takes to type.
inline uint32_t typingMs(size_t chars) {
  // G2: was 55 ms a character, up to 5 s. A caller (or a tool) that waits for a quiet line read the silence
  // as "no answer" and saw it a turn late. Now no gap is over about 1.4 s.
  uint32_t ms = (uint32_t)chars * 20;
  return ms < 300 ? 300 : (ms > 1100 ? 1100 : ms);
}
struct Beat { std::string text; uint32_t wait = 0; };   // `wait` ms after the previous beat (or after the caller's line)
// A line as the parts the caller sees one after the other. "|" separates parts.
inline std::vector<Beat> beats(const Line &l) {
  std::vector<Beat> out;
  std::string part;
  auto take = [&]() {
    std::string t = haunt::trim(part);
    part.clear();
    if (t.empty()) return;
    Beat b; b.text = t;
    b.wait = typingMs(t.size()) + (out.empty() ? l.delay : 250);
    out.push_back(b);
  };
  for (char c : l.say) { if (c == '|') take(); else part += c; }
  take();
  return out;
}

// ---------- WHO ----------------------------------------------------------------------------
// Idle time for WHO: "0:42", "12:05", "3h07m", "28d".
inline std::string idleText(uint32_t seconds) {
  auto two = [](uint32_t v) { return std::string(v < 10 ? "0" : "") + std::to_string(v); };
  if (seconds < 3600) return std::to_string(seconds / 60) + ":" + two(seconds % 60);
  if (seconds < 86400) return std::to_string(seconds / 3600) + "h" + two((seconds % 3600) / 60) + "m";
  return std::to_string(seconds / 86400) + "d";
}

}  // namespace ghost
