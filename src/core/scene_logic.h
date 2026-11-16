#pragma once
// The scene engine's decisions, as pure logic: the scene file parser (from
// plain key/value triples), the typewriter (text with pauses and word wrap)
// and the player (which node is on, what to write, which flags to grant).
// No String, no SD card, no sessions: the board wraps this in core/scene.h.
// A plain C++ compiler tests it (tools/test/scene_logic_test.cpp).
// Part of the shared core: included by core/session.h.
#include "haunt_logic.h"
#include <memory>

namespace scene {

using haunt::Kv;

static constexpr size_t MAX_NODES = 64, MAX_CHOICES = 4, MAX_TEXT = 600, MAX_CHOICE_TEXT = 70, MAX_SPEAKER = 16;
static constexpr unsigned MAX_STEPS = 200;   // nodes entered in one run
static constexpr unsigned MAX_JUMPS = 16;    // redirects followed in a row
static constexpr unsigned WIDTH = 76;        // columns, with the 2-column indent
static constexpr const char *END = "END";    // reserved target: the scene ends

struct Choice { std::string text, go, set, has, lacks; };
// typed1..4= at a choice node: words the caller may type instead of a number (comma separated,
// whole line, any case), not listed. typedgo1..4= the node; typedhas/typedlacks1..4= conditions (Q18).
struct Typed { std::vector<std::string> words; std::string go, has, lacks; };

struct Node {
  std::string id, speaker, text, art, color, set, go, ifHas, ifLacks, ifGo;
  std::vector<std::string> alt;   // G1: text2=, text3=: what the node says the second and the third time in a row (and after)
  std::string typedElse;   // Q21: typed_else= the node a typed reply that matches no word goes to (else: "Choose 1 to n")
  std::string then;   // follow-up when the scene ends by this node: CHAT (node chat) or another scene's name
  uint32_t pauseMs = 700;
  bool key = false;
  std::vector<Choice> choices;
  std::vector<Typed> typed;
};

struct Scene {
  std::string id;               // upper case, from the file name
  std::string title;            // for the event log only
  std::string start = "START";
  std::string has, lacks, set;
  std::string sound;            // a cue (ASSETS/SFX/<NAME>.WAV) the browser plays when the scene starts
  uint32_t speed = 30;          // characters per second
  bool repeat = false;
  std::vector<Node> nodes;
  int find(const std::string &nodeId) const {
    for (size_t i = 0; i < nodes.size(); ++i) if (nodes[i].id == nodeId) return (int)i;
    return -1;
  }
};

inline std::string marker(const std::string &sceneId) { return "SC_" + haunt::upper(sceneId); }

// May this caller start the scene now? (Conditions and the once-only marker.)
inline bool available(const Scene &sc, const std::string &flags) {
  if (!sc.has.empty() && !haunt::flagsHasAll(flags, sc.has)) return false;
  if (!sc.lacks.empty() && haunt::flagsHasAny(flags, sc.lacks)) return false;
  if (!sc.repeat && haunt::flagsHas(flags, marker(sc.id))) return false;
  return true;
}

// ---------- parsing ------------------------------------------------------------------------
inline bool nodeIdOk(const std::string &id) { return haunt::idOk(id) && haunt::upper(id) != END; }

inline std::string getKey(const std::vector<Kv> &kv, const std::string &key, bool *found = nullptr) {
  for (const auto &e : kv) if (haunt::lower(haunt::trim(e.key)) == key) { if (found) *found = true; return e.value; }
  if (found) *found = false;
  return "";
}
inline std::string colorOk(const std::string &s) {  // SGR digits and ; only
  std::string out;
  for (char c : haunt::trim(s)) { if ((c >= '0' && c <= '9') || c == ';') out += c; if (out.size() >= 12) break; }
  while (!out.empty() && out.back() == ';') out.pop_back();
  return out;
}
inline std::string idText(const std::string &s) { return haunt::upper(haunt::trim(s)); }

inline bool buildNode(const std::string &id, const std::vector<Kv> &kv, Node &n, std::string &why) {
  n = Node();
  if (!nodeIdOk(id)) { why = "bad node name"; return false; }
  n.id = haunt::upper(id);
  n.speaker = haunt::capText(getKey(kv, "speaker"), MAX_SPEAKER);
  n.text = haunt::capText(getKey(kv, "text"), MAX_TEXT);
  for (const char *k : {"text2", "text3"}) { bool has = false; std::string v = haunt::capText(getKey(kv, k, &has), MAX_TEXT); if (has && !v.empty()) n.alt.push_back(v); }
  n.art = idText(getKey(kv, "art"));
  n.color = colorOk(getKey(kv, "color"));
  n.set = haunt::flagsClean(getKey(kv, "set"));
  n.go = idText(getKey(kv, "go"));
  n.ifHas = haunt::flagsClean(getKey(kv, "if_has"));
  n.ifLacks = haunt::flagsClean(getKey(kv, "if_lacks"));
  n.ifGo = idText(getKey(kv, "if_go"));
  n.then = idText(getKey(kv, "then"));
  n.typedElse = idText(getKey(kv, "typed_else"));
  if (!n.typedElse.empty() && !haunt::idOk(n.typedElse)) { why = "bad typed_else"; return false; }
  if (!n.then.empty() && !haunt::idOk(n.then)) { why = "bad then"; return false; }
  bool f = false;
  std::string pause = haunt::trim(getKey(kv, "pause", &f)), key = haunt::trim(getKey(kv, "key"));
  if (f) { uint32_t v = 0; if (!haunt::parseUint(pause, 10000, v)) { why = "bad pause"; return false; } n.pauseMs = v; }
  n.key = key == "1";
  if (!n.art.empty() && !haunt::nameOk(n.art)) { why = "bad art name"; return false; }
  if (!n.go.empty() && !haunt::idOk(n.go)) { why = "bad go"; return false; }
  if (!n.ifGo.empty() && !haunt::idOk(n.ifGo)) { why = "bad if_go"; return false; }
  if (!n.ifGo.empty() && n.ifHas.empty() && n.ifLacks.empty()) { why = "if_go needs if_has or if_lacks"; return false; }
  if (n.ifGo.empty() && (!n.ifHas.empty() || !n.ifLacks.empty())) { why = "if_has or if_lacks needs if_go"; return false; }
  getKey(kv, "choice5", &f);
  if (f) { why = "more than 4 choices"; return false; }
  bool gap = false, anyOpen = false;
  for (size_t i = 1; i <= MAX_CHOICES; ++i) {
    std::string num = std::to_string(i), t = haunt::capText(getKey(kv, "choice" + num, &f), MAX_CHOICE_TEXT);
    if (!f || t.empty()) { gap = true; continue; }
    if (gap) { why = "choices must be numbered 1, 2, 3 without a gap"; return false; }
    Choice c;
    c.text = t;
    c.go = idText(getKey(kv, "go" + num));
    c.set = haunt::flagsClean(getKey(kv, "set" + num));
    c.has = haunt::flagsClean(getKey(kv, "has" + num));
    c.lacks = haunt::flagsClean(getKey(kv, "lacks" + num));
    if (c.go.empty() || !haunt::idOk(c.go)) { why = "choice " + num + " needs go" + num; return false; }
    if (c.has.empty() && c.lacks.empty()) anyOpen = true;
    n.choices.push_back(c);
  }
  for (size_t i = 1; i <= MAX_CHOICES; ++i) {
    std::string num = std::to_string(i), w = getKey(kv, "typed" + num, &f);
    if (!f || haunt::trim(w).empty()) continue;
    Typed ty;
    std::string word;
    for (size_t k = 0; k <= w.size(); ++k) {
      if (k == w.size() || w[k] == ',') { word = haunt::lower(haunt::trim(word)); if (!word.empty()) ty.words.push_back(word); word.clear(); }
      else word += w[k];
    }
    ty.go = idText(getKey(kv, "typedgo" + num));
    ty.has = haunt::flagsClean(getKey(kv, "typedhas" + num));
    ty.lacks = haunt::flagsClean(getKey(kv, "typedlacks" + num));
    if (ty.go.empty() || !haunt::idOk(ty.go)) { why = "typed" + num + " needs typedgo" + num; return false; }
    if (n.choices.empty()) { why = "typed" + num + " needs choices on the node"; return false; }
    n.typed.push_back(ty);
  }
  // Every choice may be hidden by flags: then go= is the way on, else the caller is stuck.
  if (!n.choices.empty() && !anyOpen && n.go.empty()) { why = "every choice has a condition and the node has no go (a caller could be stuck)"; return false; }
  if (n.text.empty() && n.art.empty() && n.set.empty() && n.ifGo.empty() && n.choices.empty() && n.go.empty()) { why = "empty node"; return false; }
  if (n.text.empty() && !n.choices.empty()) { why = "choices need text"; return false; }
  return true;
}

// The whole file, from its entries. `sceneId` is the file name without the
// extension. Returns false with `why` set; nothing is kept in that case.
inline bool parse(const std::string &sceneId, const std::vector<Kv> &entries, Scene &sc, std::string &why) {
  sc = Scene();
  if (!haunt::idOk(sceneId)) { why = "bad scene name"; return false; }
  sc.id = haunt::upper(sceneId);
  bool haveHeader = false;   // a scene with no [SCENE] keys uses the defaults
  size_t i = 0;
  while (i < entries.size()) {
    std::string name = haunt::trim(entries[i].section);
    std::vector<Kv> run;
    while (i < entries.size() && haunt::trim(entries[i].section) == name) run.push_back(entries[i++]);
    if (name.empty()) { why = "lines outside a section"; return false; }
    if (haunt::upper(name) == "SCENE") {
      if (haveHeader) { why = "[SCENE] twice"; return false; }
      haveHeader = true;
      sc.title = haunt::capText(getKey(run, "title"), 60);
      bool f = false;
      std::string start = idText(getKey(run, "start", &f));
      if (f) sc.start = start;
      std::string speed = getKey(run, "speed", &f);
      if (f) { uint32_t v = 0; if (!haunt::parseUint(speed, 200, v) || v < 5) { why = "speed must be 5 to 200"; return false; } sc.speed = v; }
      sc.has = haunt::flagsClean(getKey(run, "has"));
      sc.lacks = haunt::flagsClean(getKey(run, "lacks"));
      sc.set = haunt::flagsClean(getKey(run, "set"));
      sc.repeat = haunt::trim(getKey(run, "repeat")) == "1";
      sc.sound = idText(getKey(run, "sound"));
      if (!sc.sound.empty() && !haunt::idOk(sc.sound)) { why = "bad sound name"; return false; }
      continue;
    }
    if (sc.nodes.size() >= MAX_NODES) { why = "more than 64 nodes"; return false; }
    Node n; std::string w;
    if (!buildNode(name, run, n, w)) { why = "[" + name + "]: " + w; return false; }
    if (sc.find(n.id) >= 0) { why = "[" + name + "]: duplicate node"; return false; }
    sc.nodes.push_back(n);
  }
  if (sc.nodes.empty()) { why = "no nodes"; return false; }
  if (!haunt::idOk(sc.start) || sc.find(sc.start) < 0) { why = "start node " + sc.start + " does not exist"; return false; }
  auto target = [&](const std::string &t, const std::string &where) {
    if (t.empty() || t == END || sc.find(t) >= 0) return true;
    why = where + ": no node " + t;
    return false;
  };
  for (const auto &n : sc.nodes) {
    if (!target(n.go, "[" + n.id + "] go") || !target(n.ifGo, "[" + n.id + "] if_go")) return false;
    for (size_t k = 0; k < n.choices.size(); ++k)
      if (!target(n.choices[k].go, "[" + n.id + "] go" + std::to_string(k + 1))) return false;
    for (size_t k = 0; k < n.typed.size(); ++k)
      if (!target(n.typed[k].go, "[" + n.id + "] typedgo" + std::to_string(k + 1))) return false;
    if (!target(n.typedElse, "[" + n.id + "] typed_else")) return false;
  }
  return true;
}

// ---------- the typewriter -----------------------------------------------------------------
// One unit is what is written at one moment (a character, a line break with
// its indent, or an escape sequence) and the wait before the next one.
struct Unit { std::string s; uint32_t delay = 0; };

inline bool isBreakPunct(char c) { return c == '.' || c == '!' || c == '?'; }

// Lay out `text` for `speed` characters per second: word wrap at WIDTH, "|"
// is a line break. `head` and `tail` are written before and after at once
// (colour codes), `speaker` is written in front of the text.
inline std::vector<Unit> layout(const std::string &head, const std::string &speaker, const std::string &body,
                                const std::string &tail, const std::string &text, uint32_t speed) {
  std::vector<Unit> u;
  if (speed < 5) speed = 5;
  const uint32_t step = 1000 / speed;
  u.push_back({"\r\n  " + head, 0});
  unsigned col = 2;
  if (!speaker.empty()) { u.push_back({speaker + ": ", 0}); col += (unsigned)speaker.size() + 2; }
  u.push_back({body, 0});
  for (size_t i = 0; i < text.size(); ++i) {
    char c = text[i];
    if ((c & 0xC0) == 0x80 && !u.empty()) { u.back().s += c; continue; }   // rest of a UTF-8 character
    if (c == '|') { u.push_back({"\r\n  ", 520}); col = 2; continue; }
    if (c == ' ') {
      size_t j = i + 1, len = 0;
      while (j < text.size() && text[j] != ' ' && text[j] != '|') { if ((text[j] & 0xC0) != 0x80) ++len; ++j; }
      if (col + 1 + len > WIDTH && col > 2) { u.push_back({"\r\n  ", step}); col = 2; }
      else if (col > 2) { u.push_back({" ", step}); ++col; }
      continue;
    }
    uint32_t d = step;
    bool endOfWord = i + 1 >= text.size() || text[i + 1] == ' ' || text[i + 1] == '|';
    if (endOfWord && isBreakPunct(c)) d += 380;
    else if (endOfWord && (c == ',' || c == ';' || c == ':')) d += 120;
    u.push_back({std::string(1, c), d});
    ++col;
  }
  u.push_back({tail + "\r\n", 0});
  return u;
}

class Typer {
 public:
  void begin(std::vector<Unit> units, uint32_t now) { u_ = std::move(units); pos_ = 0; due_ = now; }
  bool done() const { return pos_ >= u_.size(); }
  // Everything whose time has come. A tick that is late does not make up the
  // lost time in one burst.
  std::string advance(uint32_t now) {
    std::string out;
    if ((int32_t)(now - due_) > 250) due_ = now;
    while (pos_ < u_.size() && (int32_t)(now - due_) >= 0) { out += u_[pos_].s; due_ += u_[pos_].delay; ++pos_; }
    return out;
  }
  std::string flush() {
    std::string out;
    while (pos_ < u_.size()) out += u_[pos_++].s;
    return out;
  }
 private:
  std::vector<Unit> u_;
  size_t pos_ = 0;
  uint32_t due_ = 0;
};

// ---------- the player ---------------------------------------------------------------------
// G1: the last node that had a text2=, and how many times in a row it has been shown; kept by the board per
// session, so a refusal that comes back through the hub is not word for word the same.
struct Memory { std::string node; unsigned count = 0; };
struct Part { bool isArt = false; std::string s; };   // text to write, or the name of a picture to show
struct Out {
  std::vector<Part> parts;
  std::string grant;      // flags to save to the caller now
  std::string then;       // with finished: where the caller goes next (CHAT, or a scene name); empty = the main menu
  bool finished = false;  // the scene is over
  std::vector<std::string> nodes;   // nodes entered by this call, in order (Q21: the notebook notes lines from them)
  std::string text() const { std::string t; for (const auto &p : parts) if (!p.isArt) t += p.s; return t; }
  void addText(const std::string &s) { if (!s.empty()) parts.push_back({false, s}); }
};

static constexpr uint32_t GRACE_MS = 1500;   // G1: a [any key] wait or a choice takes keys only this long after it appears

enum Phase : uint8_t { P_IDLE, P_TYPE, P_PAUSE, P_KEY, P_CHOICE, P_END };

class Player {
 public:
  bool active() const { return ph_ != P_IDLE && ph_ != P_END; }
  Phase phase() const { return ph_; }
  const std::string &flags() const { return flags_; }
  const std::string &nodeId() const { return node_; }
  unsigned steps() const { return steps_; }
  // Should the board echo the caller's keys now? Only at a choice prompt. While
  // the text types, pauses or waits for Enter a key would land in the middle
  // of a sentence, so it is dropped (Enter still skips: see input()).
  bool wantsEcho() const { return ph_ == P_CHOICE && armed_; }
  // Numbered replies of nine or fewer, and Q: single keys (contract C1), no ENTER.
  // G1: and only once the 1.5 s grace is over (armed_, set by tick()); before that keys are dropped.
  bool choiceKeys() const { return armed_ && ((ph_ == P_CHOICE && shown_.size() <= 9) || ph_ == P_KEY); }   // a [any key] wait takes any key
  bool armed() const { return armed_; }

  // replay (Q21, the notebook): the scene as it was. It grants nothing and changes no flag; a choice is shown and
  // followed but sets nothing, and a `then` is not followed.
  Out start(std::shared_ptr<const Scene> sc, const std::string &flags, bool ansi, uint32_t now, bool replay = false, Memory *mem = nullptr) {
    mem_ = mem;
    sc_ = std::move(sc); flags_ = flags; ansi_ = ansi; replay_ = replay; steps_ = 0; ph_ = P_IDLE; node_.clear(); then_.clear();
    skipping_ = false; armed_ = false; shownAt_ = now;
    Out out;
    if (!sc_) { out.finished = true; return out; }
    enter(sc_->start, now, out);
    pump(now, out);
    return out;
  }
  Out tick(uint32_t now) {
    Out out;
    if (!active()) return out;
    if (!armed_ && (ph_ == P_CHOICE || ph_ == P_KEY) && (int32_t)(now - shownAt_) >= (int32_t)GRACE_MS) armed_ = true;
    if (ph_ == P_PAUSE && (int32_t)(now - until_) >= 0) next(now, out);
    pump(now, out);
    return out;
  }
  // One typed line (the caller pressed Enter).
  Out input(const std::string &line, uint32_t now) {
    Out out;
    if (!active()) return out;
    std::string t = haunt::trim(line);
    // G1: only Q skips a scene. ENTER or any other key does not flush the typing or end a pause.
    // A choice or an [any key] wait takes keys once the grace is over; earlier keys are dropped.
    if (ph_ == P_CHOICE) { if (armed_) pick(t, now, out); }
    else if (t == "q" || t == "Q") skip(now, out);
    else if (ph_ == P_KEY && armed_) next(now, out);
    pump(now, out);
    return out;
  }

 private:
  std::shared_ptr<const Scene> sc_;
  std::string flags_, node_, then_;
  bool ansi_ = true, replay_ = false;
  unsigned steps_ = 0;
  Phase ph_ = P_IDLE;
  uint32_t until_ = 0, shownAt_ = 0;
  bool skipping_ = false, armed_ = false;
  Typer typer_;
  Memory *mem_ = nullptr;
  std::vector<size_t> shown_;   // indexes of the choices on screen

  const Node &cur() const { return sc_->nodes[(size_t)sc_->find(node_)]; }

  void grant(const std::string &add, Out &out) {
    if (add.empty() || replay_) return;
    for (const auto &w : haunt::flagWords(add)) {   // only what the caller does not have yet
      if (haunt::flagsHas(flags_, w)) continue;
      flags_ = haunt::flagsAdd(flags_, w);
      out.grant = haunt::flagsAdd(out.grant, w);
    }
  }
  void finish(Out &out) {
    if (ph_ == P_END) return;
    grant(sc_->set, out);
    ph_ = P_END; out.finished = true; out.then = replay_ ? std::string() : then_;
  }
  // Typing output and the step to the next phase when the text is out.
  void pump(uint32_t now, Out &out) {
    if (ph_ != P_TYPE) return;
    out.addText(typer_.advance(now));
    if (typer_.done()) afterText(now, out);
  }
  static bool conditionHolds(const std::string &flags, const std::string &has, const std::string &lacks) {
    return (has.empty() || haunt::flagsHasAll(flags, has)) && (lacks.empty() || !haunt::flagsHasAny(flags, lacks));
  }
  // G1: Q. The rest of the scene runs without its text: every node is entered (flags, notebook lines,
  // the then= target), until a numbered choice, which the caller still has to make.
  void skip(uint32_t now, Out &out) {
    if (ph_ == P_TYPE) {
      if (cur().choices.empty()) { typer_.flush(); skipping_ = true; next(now, out); }
      else { out.addText(typer_.flush()); afterText(now, out); }   // a choice keeps its question
    } else if (ph_ == P_PAUSE || ph_ == P_KEY) { skipping_ = true; next(now, out); }
    skipping_ = false;
  }
  void enter(std::string id, uint32_t now, Out &out) {
   for (;;) {   // G1: while skipping, a node without choices is passed through in this loop
    for (unsigned jumps = 0;; ++jumps) {
      if (id.empty() || id == END || sc_->find(id) < 0 || jumps > MAX_JUMPS || steps_ >= MAX_STEPS) { finish(out); return; }
      const Node &n = sc_->nodes[(size_t)sc_->find(id)];
      if (!n.ifGo.empty() && conditionHolds(flags_, n.ifHas, n.ifLacks)) { id = n.ifGo; continue; }
      break;
    }
    ++steps_;
    node_ = id;
    out.nodes.push_back(id);
    const Node &n = cur();
    grant(n.set, out);
    if (!n.then.empty()) then_ = n.then;
    if (!n.art.empty()) out.parts.push_back({true, n.art});
    // G1: one dim line tells the caller the only key a scene takes (after the first picture, which may clear the screen).
    if (steps_ == 1 && !replay_) out.addText(std::string(ansi_ ? "\x1b[0;90m" : "") + "  Q skips" + (ansi_ ? "\x1b[0m" : "") + "\r\n");
    if (skipping_ && n.choices.empty()) {
      if (n.go.empty()) { finish(out); return; }
      id = n.go; continue;
    }
    if (n.text.empty()) { afterText(now, out); return; }
    std::string text = n.text;
    if (mem_ && !replay_ && !n.alt.empty()) {   // G1: the same node twice in a row says it differently
      const std::string key = sc_->id + ":" + n.id;
      if (mem_->node == key) mem_->count++; else { mem_->node = key; mem_->count = 0; }
      const size_t at = mem_->count % (n.alt.size() + 1);   // text, text2, text3, text, ...: never the same twice in a row
      if (at > 0) text = n.alt[at - 1];
    }
    std::string head, body, tail, speaker = n.speaker;
    if (ansi_) {
      head = "\x1b[1;37m";
      body = "\x1b[" + (n.color.empty() ? std::string("0;37") : n.color) + "m";
      tail = "\x1b[0m";
      if (!speaker.empty()) head = "\x1b[" + (n.color.empty() ? std::string("1;37") : n.color) + "m";
    }
    typer_.begin(layout(head, speaker, body, tail, text, sc_->speed), now);
    ph_ = P_TYPE;
    if (skipping_) { out.addText(typer_.flush()); skipping_ = false; afterText(now, out); }   // a choice: shown at once
    return;
   }
  }
  void next(uint32_t now, Out &out) {
    const std::string go = cur().go;
    if (go.empty()) { finish(out); return; }
    enter(go, now, out);
  }
  void afterText(uint32_t now, Out &out) {
    const Node &n = cur();
    shown_.clear();
    for (size_t i = 0; i < n.choices.size(); ++i)
      if (conditionHolds(flags_, n.choices[i].has, n.choices[i].lacks)) shown_.push_back(i);
    if (!n.choices.empty()) {
      if (shown_.empty()) { next(now, out); return; }   // go= is the way on (checked when the scene loads)
      out.addText("\r\n");
      for (size_t k = 0; k < shown_.size(); ++k)
        out.addText(std::string(ansi_ ? "  \x1b[1;36m" : "  ") + std::to_string(k + 1) + (ansi_ ? "\x1b[0;37m) " : ") ") +
                    n.choices[shown_[k]].text + (ansi_ ? "\x1b[0m" : "") + "\r\n");
      bool q = false, words = false;
      for (const auto &ty : n.typed) for (const auto &w : ty.words) { if (w == "q") q = true; else words = true; }
      // contract C8: say how to answer and how to leave (Q20 UX round 3: "no ENTER" on the screen).
      // Critic round 5: where the node also takes typed replies, say so (never which words).
      out.addText(std::string(ansi_ ? "\x1b[0;90m" : "") + "  Press a number, no ENTER." + (words ? " Or type a reply, then ENTER." : "") + (q ? " Q leaves." : "") + (ansi_ ? "\x1b[0m" : "") + "\r\n");
      out.addText(ansi_ ? "\r\n\x1b[1;36m> \x1b[0m" : "\r\n> ");
      ph_ = P_CHOICE; armed_ = false; shownAt_ = now;
    } else if (n.key) {
      out.addText(ansi_ ? "\x1b[0;90m  [any key]\x1b[0m" : "  [any key]");
      ph_ = P_KEY; armed_ = false; shownAt_ = now;
    } else if (n.go.empty()) {
      finish(out);
    } else {
      until_ = now + n.pauseMs; ph_ = P_PAUSE;
    }
  }
  void pick(const std::string &t, uint32_t now, Out &out) {
    const std::string said = haunt::lower(haunt::trim(t));
    for (const auto &ty : cur().typed) {
      if (!conditionHolds(flags_, ty.has, ty.lacks)) continue;
      for (const auto &w : ty.words) if (w == said) { enter(ty.go, now, out); return; }
    }
    uint32_t v = 0;
    // Q21: a node that invites a typed reply answers one it does not know in its own words (typed_else=)
    if (!cur().typedElse.empty() && !said.empty() && !haunt::parseUint(t, 99, v)) { enter(cur().typedElse, now, out); return; }
    if (!haunt::parseUint(t, 99, v) || v < 1 || v > shown_.size()) {
      bool q = false;
      for (const auto &ty : cur().typed) for (const auto &w : ty.words) if (w == "q") q = true;
      out.addText(std::string(ansi_ ? "\x1b[0;33m" : "") + "  Choose 1 to " + std::to_string(shown_.size()) + (q ? ", or Q to leave" : "") + "." + (ansi_ ? "\x1b[0m" : "") + "\r\n" +
                  (ansi_ ? "\x1b[1;36m> \x1b[0m" : "> "));
      return;
    }
    const Choice c = cur().choices[shown_[v - 1]];
    grant(c.set, out);
    enter(c.go, now, out);
  }
};

}  // namespace scene
