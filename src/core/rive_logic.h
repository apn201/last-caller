#pragma once
// A reader for a subset of RiveScript (rivescript.com, open format, MIT), as pure logic: no String, no
// SD card, no sessions. ELIZA's brain is on the card (STORY/ELIZA/*.rive); core/rive.h loads it and
// answers the caller's chat line. A plain C++ compiler tests it (tools/test/rive_logic_test.cpp).
// Part of the shared core. Written fresh for this board; no RiveScript code is used.
//
// Subset:
//   ! sub a = b  ! person a = b  ! array name = a b c  ! var name = value   (global/local are ignored)
//   > topic NAME [includes X] [inherits Y]  ...  < topic        (> begin and > object are not read)
//   + trigger     words, * (one or more words), # (a number), _ (alphabetic words), (a|b), [a|b] (optional),
//                 @array, (@array), [*]; {weight=N}
//   % previous    the last thing she said must match this pattern
//   - reply       picked at random by weight, never the one she gave last time for this trigger
//   * a == b => reply       conditions (== != < <= > >= eq ne), first true wins, before the replies
//   @ other       redirect to another input
//   ^ more        continues the line before
//   tags: <star> <star1..9> <input> <reply> <get x> <set x=y> <bot x> <person> {topic=x} {random}a|b{/random}
//         {@ text} and \n (a new part: "|" in the card's chat texts)
//   <get flag_NAME> is read-only: the story flag NAME, "true" or "false".
// Limits keep the brain small on the ESP32: MAX_TRIGGERS, MAX_REPLIES, MAX_TEXT, MAX_DEPTH, a step
// limit in the matcher and a cap on the reply length.
#include "haunt_logic.h"
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <map>
#include <string>
#include <vector>

namespace rive {

static constexpr size_t MAX_TRIGGERS = 300, MAX_REPLIES = 1500, MAX_TEXT = 500, MAX_PATTERN = 120, MAX_REPLY_LEN = 600;
static constexpr int MAX_DEPTH = 6, MAX_CAPS = 9, MAX_PROG = 96, MAX_STEPS = 6000, MAX_TOPIC_CHAIN = 4;
static constexpr size_t MAX_VARS = 24, MAX_VALUE = 60;

// ---------- words -----------------------------------------------------------------------------
inline std::string lowerOf(std::string s) { for (auto &c : s) if (c >= 'A' && c <= 'Z') c = (char)(c - 'A' + 'a'); return s; }
inline std::string trimOf(const std::string &s) {
  size_t a = 0, b = s.size();
  while (a < b && (s[a] == ' ' || s[a] == '\t' || s[a] == '\r' || s[a] == '\n')) ++a;
  while (b > a && (s[b - 1] == ' ' || s[b - 1] == '\t' || s[b - 1] == '\r' || s[b - 1] == '\n')) --b;
  return s.substr(a, b - a);
}
inline bool alnum(char c) { return (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || (unsigned char)c >= 0x80; }
// "Don't, really?" -> "dont really": letters and digits only, one space between words.
inline std::vector<std::string> words(const std::string &text) {
  std::vector<std::string> out;
  std::string w;
  for (char raw : text) {
    char c = (raw >= 'A' && raw <= 'Z') ? (char)(raw - 'A' + 'a') : raw;
    if (c == '\'') continue;
    if (alnum(c)) w += c;
    else if (!w.empty()) { out.push_back(w); w.clear(); }
  }
  if (!w.empty()) out.push_back(w);
  return out;
}
inline std::string joinWords(const std::vector<std::string> &w, size_t from = 0, size_t to = (size_t)-1) {
  std::string out;
  for (size_t i = from; i < w.size() && i < to; ++i) { if (!out.empty()) out += ' '; out += w[i]; }
  return out;
}
inline bool isNumber(const std::string &s) {
  if (s.empty()) return false;
  for (char c : s) if (c < '0' || c > '9') return false;
  return true;
}
inline bool isAlpha(const std::string &s) {
  if (s.empty()) return false;
  for (char c : s) if (c >= '0' && c <= '9') return false;
  return true;
}

// ---------- patterns: a word-level backtracking program ---------------------------------------
struct Inst { enum Op : uint8_t { WORD, ANY, NUM, ALPHA, SPLIT, JMP, SAVE, MATCH } op = MATCH; uint16_t a = 0, b = 0; };
struct Pat {
  std::vector<Inst> prog;
  std::vector<std::string> lits;
  int caps = 0;
  int literal = 0;      // words that must be there
  bool wild = false;    // any wildcard
  bool onlyStar = false;
  bool ok = false;
};

struct Arrays { std::map<std::string, std::vector<std::string>> m; };

struct Comp {
  const std::string &s; size_t i = 0; Pat &p; const Arrays &arr; int depth = 0; bool fail = false;
  Comp(const std::string &src, Pat &pat, const Arrays &a) : s(src), p(pat), arr(a) {}
};
inline void emit(Comp &c, Inst::Op op, uint16_t a = 0, uint16_t b = 0) {
  if ((int)c.p.prog.size() >= MAX_PROG) { c.fail = true; return; }
  Inst in; in.op = op; in.a = a; in.b = b;
  c.p.prog.push_back(in);
}
inline void emitWord(Comp &c, const std::string &w) {
  if (w.empty()) return;
  c.p.lits.push_back(w);
  emit(c, Inst::WORD, (uint16_t)(c.p.lits.size() - 1));
  c.p.literal++;
}
inline void parseAlts(Comp &c, char close, bool optional, bool capture);
// One element or word sequence up to '|' or a closing bracket.
inline void parseSeq(Comp &c) {
  while (c.i < c.s.size() && !c.fail) {
    char ch = c.s[c.i];
    if (ch == ' ' || ch == '\t') { ++c.i; continue; }
    if (ch == '|' || ch == ')' || ch == ']') return;
    if (ch == '(') { ++c.i; if (++c.depth > 4) { c.fail = true; return; } parseAlts(c, ')', false, true); --c.depth; continue; }
    if (ch == '[') { ++c.i; if (++c.depth > 4) { c.fail = true; return; } parseAlts(c, ']', true, false); --c.depth; continue; }
    std::string tok;
    while (c.i < c.s.size() && !std::strchr(" \t|()[]", c.s[c.i])) tok += c.s[c.i++];
    if (tok == "*" || tok == "_") {
      if (c.p.caps >= MAX_CAPS) { c.fail = true; return; }
      int k = c.p.caps++;
      c.p.wild = true;
      emit(c, Inst::SAVE, (uint16_t)(2 * k));
      uint16_t loop = (uint16_t)c.p.prog.size();
      emit(c, tok == "*" ? Inst::ANY : Inst::ALPHA);
      emit(c, Inst::SPLIT, (uint16_t)(loop + 2), loop);
      emit(c, Inst::SAVE, (uint16_t)(2 * k + 1));
    } else if (tok == "#") {
      if (c.p.caps >= MAX_CAPS) { c.fail = true; return; }
      int k = c.p.caps++;
      c.p.wild = true;
      emit(c, Inst::SAVE, (uint16_t)(2 * k));
      emit(c, Inst::NUM);
      emit(c, Inst::SAVE, (uint16_t)(2 * k + 1));
    } else if (!tok.empty() && tok[0] == '@') {
      auto it = c.arr.m.find(lowerOf(tok.substr(1)));
      if (it == c.arr.m.end()) { c.fail = true; return; }
      // Rewrite as (a|b|c) in a temporary pattern string.
      std::string alt = "(";
      for (size_t k = 0; k < it->second.size(); ++k) { if (k) alt += '|'; alt += it->second[k]; }
      alt += ")";
      Comp sub(alt, c.p, c.arr); sub.depth = c.depth + 1;
      ++sub.i;
      parseAlts(sub, ')', false, true);
      if (sub.fail) c.fail = true;
    } else {
      for (const auto &w : words(tok)) emitWord(c, w);
    }
  }
}
inline void parseAlts(Comp &c, char close, bool optional, bool capture) {
  int k = -1;
  if (capture) {
    if (c.p.caps >= MAX_CAPS) { c.fail = true; return; }
    k = c.p.caps++;
    c.p.wild = true;
    emit(c, Inst::SAVE, (uint16_t)(2 * k));
  }
  std::vector<size_t> jumps, splits;
  size_t lastSplit = 0;
  while (!c.fail) {
    lastSplit = c.p.prog.size();
    emit(c, Inst::SPLIT);
    if (c.fail) return;
    c.p.prog[lastSplit].a = (uint16_t)(lastSplit + 1);
    parseSeq(c);
    if (c.i < c.s.size() && c.s[c.i] == '|') {
      ++c.i;
      jumps.push_back(c.p.prog.size());
      emit(c, Inst::JMP);
      if (c.fail) return;
      c.p.prog[lastSplit].b = (uint16_t)c.p.prog.size();
      continue;
    }
    break;
  }
  if (c.fail) return;
  if (c.i < c.s.size() && c.s[c.i] == close) ++c.i;
  else if (close) { c.fail = true; return; }
  size_t end = c.p.prog.size();
  if (optional) c.p.prog[lastSplit].b = (uint16_t)end;     // none of the branches: skip them all
  else { c.p.prog[lastSplit].op = Inst::JMP; c.p.prog[lastSplit].a = (uint16_t)(lastSplit + 1); }
  for (size_t j : jumps) c.p.prog[j].a = (uint16_t)end;
  if (capture) emit(c, Inst::SAVE, (uint16_t)(2 * k + 1));
}
inline bool compile(const std::string &src, const Arrays &arr, Pat &p) {
  p = Pat();
  if (src.size() > MAX_PATTERN) return false;
  Comp c(src, p, arr);
  p.onlyStar = (trimOf(src) == "*");
  parseSeq(c);
  while (!c.fail && c.i < c.s.size()) { ++c.i; parseSeq(c); }   // a stray bracket is skipped
  emit(c, Inst::MATCH);
  p.ok = !c.fail;
  return p.ok;
}

struct Frame { uint16_t pc, wi; int16_t caps[2 * MAX_CAPS]; };
// Whole-input match. `stars` gets one entry per wildcard or group, in the order written.
inline bool matches(const Pat &p, const std::vector<std::string> &in, std::vector<std::string> *stars) {
  if (!p.ok) return false;
  std::vector<Frame> stack;
  Frame cur; cur.pc = 0; cur.wi = 0;
  for (auto &v : cur.caps) v = -1;
  int steps = 0;
  for (;;) {
    if (++steps > MAX_STEPS) return false;
    if (cur.pc >= p.prog.size()) return false;
    const Inst &x = p.prog[cur.pc];
    bool fail = false;
    switch (x.op) {
      case Inst::WORD: if (cur.wi < in.size() && in[cur.wi] == p.lits[x.a]) { cur.wi++; cur.pc++; } else fail = true; break;
      case Inst::ANY: if (cur.wi < in.size()) { cur.wi++; cur.pc++; } else fail = true; break;
      case Inst::NUM: if (cur.wi < in.size() && isNumber(in[cur.wi])) { cur.wi++; cur.pc++; } else fail = true; break;
      case Inst::ALPHA: if (cur.wi < in.size() && isAlpha(in[cur.wi])) { cur.wi++; cur.pc++; } else fail = true; break;
      case Inst::JMP: cur.pc = x.a; break;
      case Inst::SAVE: if (x.a < 2 * MAX_CAPS) cur.caps[x.a] = (int16_t)cur.wi; cur.pc++; break;
      case Inst::SPLIT: {
        if (stack.size() >= 160) return false;
        Frame alt = cur; alt.pc = x.b;
        stack.push_back(alt);
        cur.pc = x.a;
        break;
      }
      case Inst::MATCH:
        if (cur.wi == in.size()) {
          if (stars) {
            stars->clear();
            for (int k = 0; k < p.caps; ++k) {
              int a = cur.caps[2 * k], b = cur.caps[2 * k + 1];
              stars->push_back(a >= 0 && b >= a ? joinWords(in, (size_t)a, (size_t)b) : std::string());
            }
          }
          return true;
        }
        fail = true;
        break;
    }
    if (fail) {
      if (stack.empty()) return false;
      cur = stack.back();
      stack.pop_back();
    }
  }
}

// ---------- the brain -------------------------------------------------------------------------
struct Reply { std::string text; int weight = 1; };
struct Cond { std::string left, op, right, text; };
struct Trigger {
  Pat pat, prev;
  bool hasPrev = false;
  int weight = 0;
  std::string topic = "random", redirect;
  std::vector<Cond> conds;
  std::vector<Reply> replies;
};
struct Topic { std::vector<std::string> includes; };

struct Brain {
  std::vector<Trigger> trig;
  std::vector<std::pair<std::string, std::string>> subs, persons;   // longest key first after finish()
  Arrays arrays;
  std::map<std::string, std::string> bot;
  std::map<std::string, Topic> topics;
  std::vector<size_t> order;          // trigger indexes, most specific first
  std::vector<std::string> problems;  // one short line each, for the event log
  size_t replies = 0;
  bool empty() const { return trig.empty(); }
  void problem(const std::string &p) { if (problems.size() < 20) problems.push_back(p); }
};

inline std::string stripWeight(std::string &text) {
  size_t a = text.find("{weight=");
  if (a == std::string::npos) return "1";
  size_t b = text.find('}', a);
  if (b == std::string::npos) return "1";
  std::string w = text.substr(a + 8, b - a - 8);
  text.erase(a, b - a + 1);
  text = trimOf(text);
  return w;
}
inline int toInt(const std::string &s, int dflt = 0) {
  if (s.empty() || s.size() > 6) return dflt;
  int v = 0; bool neg = false; size_t i = 0;
  if (s[0] == '-') { neg = true; i = 1; }
  if (i >= s.size()) return dflt;
  for (; i < s.size(); ++i) { if (s[i] < '0' || s[i] > '9') return dflt; v = v * 10 + (s[i] - '0'); }
  return neg ? -v : v;
}

// Adds one file's text. May be called for several files; call finish() when all are in.
inline void load(Brain &b, const std::string &text) {
  // Lines, with ^ continuations joined and // and /* */ comments dropped.
  std::vector<std::string> lines;
  std::string cur;
  bool block = false;
  for (size_t i = 0; i <= text.size(); ++i) {
    if (i < text.size() && text[i] != '\n') { cur += text[i]; continue; }
    std::string t = trimOf(cur);
    cur.clear();
    if (block) { if (t.find("*/") != std::string::npos) block = false; continue; }
    if (t.empty() || t.compare(0, 2, "//") == 0) continue;
    if (t.compare(0, 2, "/*") == 0) { if (t.find("*/") == std::string::npos) block = true; continue; }
    if (t[0] == '^') { if (!lines.empty()) lines.back() += trimOf(t.substr(1)); continue; }
    lines.push_back(t);
  }
  std::string topic = "random";
  Trigger *t = nullptr;
  auto stray = [&](const std::string &l) { b.problem("no trigger for: " + l.substr(0, 40)); };
  for (const std::string &l : lines) {
    char cmd = l[0];
    std::string rest = trimOf(l.substr(1));
    if (cmd == '!') {
      size_t sp = rest.find(' ');
      std::string kind = sp == std::string::npos ? rest : rest.substr(0, sp);
      std::string body = sp == std::string::npos ? "" : trimOf(rest.substr(sp + 1));
      size_t eq = body.find('=');
      if (eq == std::string::npos) continue;
      std::string key = trimOf(body.substr(0, eq)), val = trimOf(body.substr(eq + 1));
      if (kind == "sub" || kind == "person") {
        std::string k = joinWords(words(key));
        if (!k.empty()) (kind == "sub" ? b.subs : b.persons).push_back({k, joinWords(words(val))});
      } else if (kind == "array") {
        std::vector<std::string> items;
        if (val.find('|') != std::string::npos) { std::string it; for (char ch : val + "|") { if (ch == '|') { it = trimOf(it); if (!it.empty()) items.push_back(it); it.clear(); } else it += ch; } }
        else { std::string it; for (char ch : val + " ") { if (ch == ' ') { if (!it.empty()) items.push_back(it); it.clear(); } else it += ch; } }
        b.arrays.m[lowerOf(key)] = items;
      } else if (kind == "var") {
        b.bot[lowerOf(key)] = val;
      }
      continue;
    }
    if (cmd == '>') {
      std::vector<std::string> w;
      { std::string it; for (char ch : rest + " ") { if (ch == ' ') { if (!it.empty()) w.push_back(it); it.clear(); } else it += ch; } }
      if (w.size() >= 2 && w[0] == "topic") {
        topic = lowerOf(w[1]);
        Topic &tp = b.topics[topic];
        for (size_t k = 2; k + 1 < w.size(); ++k)
          if (w[k] == "includes" || w[k] == "inherits") { size_t j = k + 1; while (j < w.size() && w[j] != "includes" && w[j] != "inherits") tp.includes.push_back(lowerOf(w[j++])); }
      }
      t = nullptr;
      continue;
    }
    if (cmd == '<') { topic = "random"; t = nullptr; continue; }
    if (cmd == '+') {
      t = nullptr;
      if (b.trig.size() >= MAX_TRIGGERS) { b.problem("too many triggers"); continue; }
      Trigger n;
      std::string wt = stripWeight(rest);
      n.weight = toInt(wt, 1) - 1; if (n.weight < 0) n.weight = 0;
      n.topic = topic;
      if (!compile(rest, b.arrays, n.pat)) { b.problem("bad trigger: " + rest.substr(0, 40)); continue; }
      b.trig.push_back(n);
      t = &b.trig.back();
      continue;
    }
    if (!t) { stray(l); continue; }
    if (cmd == '%') { if (!compile(rest, b.arrays, t->prev)) b.problem("bad previous: " + rest.substr(0, 40)); else t->hasPrev = true; continue; }
    if (cmd == '@') { t->redirect = rest; continue; }
    if (cmd == '-') {
      if (b.replies >= MAX_REPLIES || rest.size() > MAX_TEXT) { b.problem("reply skipped"); continue; }
      Reply r; r.weight = std::max(1, toInt(stripWeight(rest), 1)); r.text = rest;
      t->replies.push_back(r); b.replies++;
      continue;
    }
    if (cmd == '*') {
      size_t arrow = rest.find("=>");
      if (arrow == std::string::npos) { b.problem("condition without =>: " + rest.substr(0, 30)); continue; }
      std::string lhs = trimOf(rest.substr(0, arrow));
      Cond c; c.text = trimOf(rest.substr(arrow + 2));
      static const char *ops[] = {"==", "!=", "<>", "<=", ">=", "<", ">", " eq ", " ne "};
      bool found = false;
      for (const char *op : ops) {
        const size_t n = std::strlen(op);
        size_t at = std::string::npos;
        int depth = 0;
        for (size_t k = 0; k + n <= lhs.size(); ++k) {   // outside of <tags>: "<get x> > 3"
          const char ch = lhs[k];
          if (ch == '<' && k + 1 < lhs.size() && ((lhs[k + 1] >= 'a' && lhs[k + 1] <= 'z') || (lhs[k + 1] >= 'A' && lhs[k + 1] <= 'Z'))) { depth++; continue; }
          if (ch == '>' && depth > 0) { depth--; continue; }
          if (depth == 0 && lhs.compare(k, n, op) == 0) { at = k; break; }
        }
        if (at == std::string::npos) continue;
        c.left = trimOf(lhs.substr(0, at)); c.op = trimOf(op); c.right = trimOf(lhs.substr(at + n));
        found = true; break;
      }
      if (!found) { b.problem("condition without a comparison: " + rest.substr(0, 30)); continue; }
      t->conds.push_back(c);
      continue;
    }
  }
}
inline std::string spaced(const std::string &s) { return " " + s + " "; }
// A stable insertion sort: small lists, and no std::stable_sort (its template code does not link into the
// ESP32's one big translation unit: "l32r literal target out of range").
template <typename T, typename Less> inline void sortStable(std::vector<T> &v, Less less) {
  for (size_t i = 1; i < v.size(); ++i) {
    T x = v[i];
    size_t j = i;
    while (j > 0 && less(x, v[j - 1])) { v[j] = v[j - 1]; --j; }
    v[j] = x;
  }
}
// Sort the triggers and the substitutions. Call once after every file is loaded.
inline void finish(Brain &b) {
  auto longer = [](const std::pair<std::string, std::string> &x, const std::pair<std::string, std::string> &y) { return x.first.size() > y.first.size(); };
  sortStable(b.subs, longer);
  sortStable(b.persons, longer);
  b.order.clear();
  for (size_t i = 0; i < b.trig.size(); ++i) if (b.trig[i].pat.ok) b.order.push_back(i);
  auto cls = [&](const Trigger &t) { return t.pat.onlyStar ? 3 : (t.pat.wild ? 1 : 0); };
  sortStable(b.order, [&](size_t x, size_t y) {
    const Trigger &a = b.trig[x], &c = b.trig[y];
    if (a.weight != c.weight) return a.weight > c.weight;
    if (a.hasPrev != c.hasPrev) return a.hasPrev;
    if (cls(a) != cls(c)) return cls(a) < cls(c);
    return a.pat.literal > c.pat.literal;
  });
}

// ---------- one caller's conversation ----------------------------------------------------------
struct State {
  std::string topic = "random";
  std::map<std::string, std::string> vars;     // <set> and <get>; "name", "my ..." and the like
  std::map<uint16_t, uint16_t> lastPick;       // trigger -> the reply index given last time
  std::string lastInput, lastReply;
  uint32_t rng = 12345;
};
struct Result {
  bool matched = false;
  bool repeated = false;     // the only answer there was is the one she gave last
  std::string text;
};

inline uint32_t nextRand(State &st) { st.rng = st.rng * 1664525u + 1013904223u; return st.rng >> 8; }

// Substitutions on a word list: whole words only, longest key first.
inline std::vector<std::string> substitute(const std::vector<std::pair<std::string, std::string>> &subs, const std::vector<std::string> &in) {
  if (subs.empty()) return in;
  std::string s = spaced(joinWords(in));
  for (const auto &kv : subs) {
    const std::string key = spaced(kv.first), val = spaced(kv.second);
    size_t at = 0;
    int guard = 0;
    while ((at = s.find(key, at)) != std::string::npos && ++guard < 40) {
      s.replace(at, key.size(), val);
      at += val.size() > 1 ? val.size() - 1 : 1;
    }
  }
  return words(s);
}

struct Ctx { const Brain &b; State &st; const std::string &flags; std::vector<std::string> stars; int depth; };

inline std::string respond(const Brain &b, State &st, const std::string &flags, const std::vector<std::string> &in, int depth, bool &matched, bool &repeated);

inline std::string getVar(const Ctx &c, const std::string &name) {
  if (name.compare(0, 5, "flag_") == 0) return haunt::flagsHas(c.flags, name.substr(5)) ? "true" : "false";
  auto it = c.st.vars.find(name);
  return it == c.st.vars.end() ? "undefined" : it->second;
}
inline std::string expand(Ctx &c, const std::string &text);
inline std::string expand(Ctx &c, const std::string &text) {
  std::string out;
  for (size_t i = 0; i < text.size() && out.size() < MAX_REPLY_LEN; ++i) {
    char ch = text[i];
    if (ch == '\\' && i + 1 < text.size() && text[i + 1] == 'n') { out += '|'; ++i; continue; }
    if (ch == '<') {
      size_t e = i + 1;   // the matching '>' (tags nest: <set name=<formal>>)
      for (int d = 1; e < text.size(); ++e) { if (text[e] == '<') d++; else if (text[e] == '>' && --d == 0) break; }
      if (e < text.size()) {
        std::string tag = text.substr(i + 1, e - i - 1);
        std::string name = lowerOf(tag.substr(0, tag.find_first_of(" =")));
        std::string arg = tag.find(' ') == std::string::npos ? "" : trimOf(tag.substr(tag.find(' ') + 1));
        if (arg.find('<') != std::string::npos) arg = expand(c, arg);
        bool done = true;
        if (name.compare(0, 4, "star") == 0 && (name.size() == 4 || isNumber(name.substr(4)))) {
          int n = name.size() == 4 ? 1 : toInt(name.substr(4), 1);
          if (n >= 1 && (size_t)n <= c.stars.size()) out += c.stars[n - 1];
        } else if (name == "input") out += c.st.lastInput;
        else if (name == "reply") out += c.st.lastReply;
        else if (name == "get") out += getVar(c, lowerOf(arg));
        else if (name == "bot") { auto it = c.b.bot.find(lowerOf(arg)); out += it == c.b.bot.end() ? "undefined" : it->second; }
        else if (name == "set") {
          size_t eq = arg.find('=');
          if (eq != std::string::npos) {
            std::string k = lowerOf(trimOf(arg.substr(0, eq))), v = trimOf(arg.substr(eq + 1));
            if (k.compare(0, 5, "flag_") != 0 && !k.empty() && v.size() <= MAX_VALUE && (c.st.vars.count(k) || c.st.vars.size() < MAX_VARS)) c.st.vars[k] = v;
          }
        } else if (name == "formal") {
          std::string f = c.stars.empty() ? std::string() : c.stars[0];
          bool up = true;
          for (auto &x : f) { if (x == ' ') up = true; else { if (up && x >= 'a' && x <= 'z') x = (char)(x - 'a' + 'A'); up = false; } }
          out += f;
        } else if (name == "person") {
          std::vector<std::string> w = words(c.stars.empty() ? std::string() : c.stars[0]);
          out += joinWords(substitute(c.b.persons, w));
        } else done = false;
        if (done) { i = e; continue; }
      }
    }
    if (ch == '{') {
      size_t e = text.find('}', i);
      if (e != std::string::npos) {
        std::string tag = text.substr(i + 1, e - i - 1);
        if (tag.compare(0, 6, "topic=") == 0) { c.st.topic = lowerOf(trimOf(tag.substr(6))); i = e; continue; }
        if (tag == "random") {
          size_t close = text.find("{/random}", e);
          if (close != std::string::npos) {
            std::string body = text.substr(e + 1, close - e - 1);
            std::vector<std::string> opts; std::string it;
            const char sep = body.find('|') != std::string::npos ? '|' : ' ';
            for (char x : body + sep) { if (x == sep) { opts.push_back(it); it.clear(); } else it += x; }
            if (!opts.empty()) out += expand(c, opts[nextRand(c.st) % opts.size()]);
            i = close + 8; continue;
          }
        }
        if (!tag.empty() && tag[0] == '@' && c.depth < MAX_DEPTH) {
          bool m = false, r = false;
          std::string target = expand(c, trimOf(tag.substr(1)));
          out += respond(c.b, c.st, c.flags, substitute(c.b.subs, words(target)), c.depth + 1, m, r);
          i = e; continue;
        }
      }
    }
    out += ch;
  }
  if (out.size() > MAX_REPLY_LEN) out.resize(MAX_REPLY_LEN);
  return out;
}
inline bool compare(const std::string &l, const std::string &op, const std::string &r) {
  if (op == "==" || op == "eq") return l == r;
  if (op == "!=" || op == "<>" || op == "ne") return l != r;
  int a = toInt(l, 0), b = toInt(r, 0);
  if (op == "<") return a < b;
  if (op == "<=") return a <= b;
  if (op == ">") return a > b;
  if (op == ">=") return a >= b;
  return false;
}
inline bool topicChain(const Brain &b, const std::string &topic, std::vector<std::string> &chain, int depth = 0) {
  if (depth > MAX_TOPIC_CHAIN) return false;
  for (const auto &t : chain) if (t == topic) return true;
  chain.push_back(topic);
  auto it = b.topics.find(topic);
  if (it != b.topics.end()) for (const auto &inc : it->second.includes) topicChain(b, inc, chain, depth + 1);
  return true;
}

inline std::string respond(const Brain &b, State &st, const std::string &flags, const std::vector<std::string> &in, int depth, bool &matched, bool &repeated) {
  matched = false; repeated = false;
  if (depth > MAX_DEPTH) return "";
  std::vector<std::string> chain;
  topicChain(b, st.topic, chain);
  const std::vector<std::string> prevWords = words(st.lastReply);
  for (const std::string &tp : chain) {
    for (size_t idx : b.order) {
      const Trigger &t = b.trig[idx];
      if (t.topic != tp) continue;
      if (t.hasPrev && !matches(t.prev, prevWords, nullptr)) continue;
      std::vector<std::string> stars;
      if (!matches(t.pat, in, &stars)) continue;
      Ctx c{b, st, flags, stars, depth};
      // A redirect, then conditions, then a reply.
      if (!t.redirect.empty()) {
        std::string target = expand(c, t.redirect);
        bool m = false, r = false;
        std::string out = respond(b, st, flags, substitute(b.subs, words(target)), depth + 1, m, r);
        if (m) { matched = true; repeated = r; return out; }
        continue;
      }
      for (const Cond &cd : t.conds) {
        if (compare(expand(c, cd.left), cd.op, expand(c, cd.right))) {
          matched = true;
          return expand(c, cd.text);
        }
      }
      if (t.replies.empty()) continue;
      // Weighted pick, not the one given last time for this trigger when there is a choice.
      auto last = st.lastPick.find((uint16_t)idx);
      int total = 0;
      for (size_t k = 0; k < t.replies.size(); ++k) if (t.replies.size() == 1 || last == st.lastPick.end() || last->second != k) total += t.replies[k].weight;
      int roll = total ? (int)(nextRand(st) % (uint32_t)total) : 0;
      size_t pick = 0;
      for (size_t k = 0; k < t.replies.size(); ++k) {
        if (!(t.replies.size() == 1 || last == st.lastPick.end() || last->second != k)) continue;
        if (roll < t.replies[k].weight) { pick = k; break; }
        roll -= t.replies[k].weight;
      }
      st.lastPick[(uint16_t)idx] = (uint16_t)pick;
      matched = true;
      std::string out = expand(c, t.replies[pick].text);
      repeated = (t.replies.size() == 1 && out == st.lastReply);
      return out;
    }
  }
  return "";
}

// What she was told, as one line for the account's record: "name=Anna Lee;job=welder". Keys and values keep
// only letters, digits, spaces, _ and . (so the line never holds a | ; = or a control character).
inline std::string cleanVar(const std::string &s, size_t cap) {
  std::string out;
  for (char c : s) {
    bool ok = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == ' ' || c == '_' || c == '.' || c == '-';
    if (ok && out.size() < cap) out += c;
  }
  return trimOf(out);
}
inline std::string encodeVars(const std::map<std::string, std::string> &vars) {
  std::string out;
  for (const auto &kv : vars) {
    std::string k = cleanVar(kv.first, 24), v = cleanVar(kv.second, MAX_VALUE);
    if (k.empty() || v.empty()) continue;
    if (!out.empty()) out += ';';
    out += k + "=" + v;
  }
  return out;
}
inline std::map<std::string, std::string> decodeVars(const std::string &line) {
  std::map<std::string, std::string> out;
  std::string part;
  for (size_t i = 0; i <= line.size(); ++i) {
    if (i < line.size() && line[i] != ';') { part += line[i]; continue; }
    size_t eq = part.find('=');
    if (eq != std::string::npos && out.size() < MAX_VARS) {
      std::string k = lowerOf(cleanVar(part.substr(0, eq), 24)), v = cleanVar(part.substr(eq + 1), MAX_VALUE);
      if (!k.empty() && !v.empty() && k.compare(0, 5, "flag_") != 0) out[k] = v;
    }
    part.clear();
  }
  return out;
}

// The brain's answer to what the caller typed. matched=false: no trigger fits (and no `+ *` is written).
inline Result reply(const Brain &b, State &st, const std::string &flags, const std::string &input) {
  Result r;
  if (b.empty()) return r;
  std::vector<std::string> in = substitute(b.subs, words(input));
  if (in.empty()) return r;
  bool m = false, rep = false;
  std::string out = respond(b, st, flags, in, 0, m, rep);
  st.lastInput = joinWords(in);
  if (!m) return r;
  r.matched = true; r.text = trimOf(out); r.repeated = rep || (r.text == st.lastReply && !r.text.empty());
  if (r.text.empty()) r.matched = false;
  else st.lastReply = r.text;
  return r;
}

}  // namespace rive
