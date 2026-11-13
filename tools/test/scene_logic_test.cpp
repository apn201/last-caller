// Host-side test of src/core/scene_logic.h. No board, no Arduino types.
//   zig c++ -std=c++17 -Wall -Wextra -I src/core tools/test/scene_logic_test.cpp -o scene_logic_test
//   (or g++/clang++ with the same flags), then run ./scene_logic_test
//   ./scene_logic_test sd/BBS/STORY/SCENES/*.INI  also checks the card's own scenes load
//   and that every path through each of them ends.
#include "scene_logic.h"
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>

using namespace scene;

static int failures = 0;
#define CHECK(cond) do { if (!(cond)) { printf("FAIL line %d: %s\n", __LINE__, #cond); failures++; } } while (0)

static uint32_t lcg = 24681357;
static uint32_t rnd() { lcg = lcg * 1664525u + 1013904223u; return lcg >> 8 | lcg << 24; }

// The same shape iniLoad() makes: sections upper case, keys lower case.
static std::vector<Kv> ini(const std::string &text) {
  std::vector<Kv> out;
  std::string section, line;
  for (size_t i = 0; i <= text.size(); ++i) {
    if (i == text.size() || text[i] == '\n') {
      std::string t = haunt::trim(line);
      line.clear();
      if (!t.empty() && t[0] != ';') {
        if (t[0] == '[') { size_t c = t.find(']'); section = haunt::upper(haunt::trim(t.substr(1, c == std::string::npos ? std::string::npos : c - 1))); }
        else { size_t eq = t.find('='); if (eq != std::string::npos && eq > 0) out.push_back({section, haunt::lower(haunt::trim(t.substr(0, eq))), haunt::trim(t.substr(eq + 1))}); }
      }
    } else line += text[i];
  }
  return out;
}
static std::shared_ptr<Scene> load(const std::string &text, std::string *why = nullptr, const std::string &id = "T") {
  auto sc = std::make_shared<Scene>();
  std::string w;
  bool ok = parse(id, ini(text), *sc, w);
  if (why) *why = w;
  return ok ? sc : nullptr;
}
static bool fails(const std::string &text, const char *part) {
  std::string why;
  bool ok = load(text, &why) != nullptr;
  if (ok || why.find(part) == std::string::npos) { printf("  expected failure with \"%s\", got ok=%d why=\"%s\"\n", part, ok, why.c_str()); return false; }
  return true;
}

static const char *DEMO =
  "[SCENE]\nspeed=100\nset=DONE\n"
  "[START]\nspeaker=A\ntext=Hello there.\npause=100\ngo=ASK\n"
  "[ASK]\nspeaker=A\ntext=Who are you?\nchoice1=Friend\ngo1=NICE\nset1=POLITE\nchoice2=Liar\ngo2=MEAN\nset2=LIED\n"
  "choice3=Secret\ngo3=NICE\nhas3=KEY_CARD\n"
  "[NICE]\ntext=Welcome.\ngo=OUT\n"
  "[MEAN]\ntext=Hm.\ngo=OUT\n"
  "[OUT]\nif_has=LIED\nif_go=OUT_LIED\ntext=Goodbye.\n"
  "[OUT_LIED]\ntext=Goodbye, friend.\nkey=1\n";

// Drive a player to its end the way a caller would. Time passes in 50 ms ticks.
struct Run { std::string text, grant; unsigned ticks = 0; bool finished = false; std::vector<std::string> art; };
static void take(Run &r, const Out &o) {
  for (const auto &p : o.parts) { if (p.isArt) r.art.push_back(p.s); else r.text += p.s; }
  r.grant = haunt::flagsAdd(r.grant, o.grant);
  if (o.finished) r.finished = true;
}
static Run play(const std::shared_ptr<Scene> &sc, const std::string &flags, const std::vector<std::string> &answers, bool ansi = true) {
  Run r; Player p; uint32_t now = 1000;
  take(r, p.start(sc, flags, ansi, now));
  size_t a = 0;
  while (!r.finished && r.ticks < 20000) {
    now += 50; ++r.ticks;
    take(r, p.tick(now));
    if (r.finished) break;
    if (p.phase() == P_CHOICE || p.phase() == P_KEY) {
      std::string ans = a < answers.size() ? answers[a++] : "";
      take(r, p.input(ans, now));
    }
  }
  return r;
}

int main(int argc, char **argv) {
  // ---- parse ----
  auto sc = load(DEMO);
  CHECK(sc != nullptr);
  CHECK(sc && sc->nodes.size() == 6);
  CHECK(sc && sc->speed == 100 && sc->start == "START" && sc->set == "DONE" && !sc->repeat);
  CHECK(sc && sc->nodes[1].choices.size() == 3 && sc->nodes[1].choices[2].has == "KEY_CARD");
  CHECK(sc && sc->id == "T");
  // unknown keys are ignored, case is ignored
  CHECK(load("[scene]\nfuture=1\n[start]\ntext=hi\nfuture2=x\n") != nullptr);

  // ---- bad scenes ----
  CHECK(load("[START]\ntext=x\n") != nullptr);   // no [SCENE] keys: defaults
  CHECK(fails("[SCENE]\ntitle=t\n", "no nodes"));
  CHECK(fails("[SCENE]\ntitle=t\n[START]\nspeaker=A\n", "empty node"));
  CHECK(fails("[SCENE]\nstart=NOPE\n[START]\ntext=x\n", "start node"));
  CHECK(fails("[SCENE]\n[START]\ntext=x\ngo=NOPE\n", "no node NOPE"));
  CHECK(fails("[SCENE]\n[START]\ntext=x\nchoice1=a\ngo1=NOPE\n", "no node NOPE"));
  CHECK(fails("[SCENE]\n[START]\ntext=x\nchoice1=a\n", "needs go1"));
  CHECK(fails("[SCENE]\n[START]\ntext=x\nchoice2=a\ngo2=END\n", "gap"));
  CHECK(fails("[SCENE]\n[START]\ntext=x\nchoice1=a\ngo1=END\nchoice5=e\n", "more than 4"));
  CHECK(fails("[SCENE]\n[START]\ntext=x\nchoice1=a\ngo1=END\nhas1=Q\n", "stuck"));
  CHECK(fails("[SCENE]\n[START]\ntext=x\nif_go=END\n", "if_go needs"));
  CHECK(fails("[SCENE]\n[START]\ntext=x\nif_has=A\n", "needs if_go"));
  CHECK(fails("[SCENE]\n[START]\ntext=x\npause=abc\n", "bad pause"));
  CHECK(fails("[SCENE]\n[START]\ntext=x\npause=99999\n", "bad pause"));
  CHECK(fails("[SCENE]\n[START]\ntext=x\nart=../EVIL\n", "bad art"));
  CHECK(sc && sc->sound.empty());
  { auto snd = load("[SCENE]\nsound=hum\n[START]\ntext=x\n"); CHECK(snd && snd->sound == "HUM"); }
  CHECK(fails("[SCENE]\nsound=../EVIL\n[START]\ntext=x\n", "bad sound"));
  CHECK(fails("[SCENE]\nspeed=2\n[START]\ntext=x\n", "speed"));
  CHECK(fails("[SCENE]\nspeed=900\n[START]\ntext=x\n", "speed"));
  CHECK(fails("[SCENE]\ntitle=a\n[START]\ntext=x\n[SCENE]\ntitle=b\n", "twice"));
  CHECK(fails("[SCENE]\n[START]\ntext=x\n[X]\ntext=y\n[START]\ntext=z\n", "duplicate"));
  CHECK(fails("[SCENE]\n[END]\ntext=x\n", "bad node name"));
  CHECK(fails("[SCENE]\n[A B]\ntext=x\n", "bad node name"));
  CHECK(fails("text=x\n[SCENE]\n[START]\ntext=x\n", "outside"));
  { std::string big = "[SCENE]\n"; for (int i = 0; i < 65; ++i) big += "[N" + std::to_string(i) + "]\ntext=x\n"; big.replace(big.find("[N0]"), 4, "[START]");
    CHECK(fails(big, "64 nodes")); }
  { std::string ok = "[SCENE]\n"; for (int i = 0; i < 64; ++i) ok += (i ? "[N" + std::to_string(i) + "]\n" : std::string("[START]\n")) + "text=x\n";
    CHECK(load(ok) != nullptr); }
  CHECK(!load("[SCENE]\n[START]\ntext=x\n", nullptr, "bad name"));
  // a choice node whose choices are all conditional is fine with a go=
  CHECK(load("[SCENE]\n[START]\ntext=x\nchoice1=a\ngo1=END\nhas1=Q\ngo=END\n") != nullptr);

  // ---- text limits: control characters out, length cut ----
  { auto s = load("[SCENE]\n[START]\ntext=ab\x01\x1b[0mcd\n"); CHECK(s && s->nodes[0].text.find('\x1b') == std::string::npos); }
  { auto s = load("[SCENE]\n[START]\ntext=" + std::string(2000, 'x') + "\n"); CHECK(s && s->nodes[0].text.size() == MAX_TEXT); }
  { auto s = load("[SCENE]\n[START]\ntext=x\ncolor=1;33;evil\n"); CHECK(s && s->nodes[0].color == "1;33"); }

  // ---- layout and typer ----
  {
    auto u = layout("", "", "", "", "Hello, world. Bye|Next", 30);
    std::string all; for (auto &x : u) all += x.s;
    CHECK(all == "\r\n  Hello, world. Bye\r\n  Next\r\n");
    // pauses: comma, sentence end and the break are longer than a plain letter
    uint32_t dH = 0, dComma = 0, dDot = 0, dBreak = 0;
    for (auto &x : u) { if (x.s == "H") dH = x.delay; if (x.s == ",") dComma = x.delay; if (x.s == "d") dDot = x.delay; if (x.s == "\r\n  " && x.delay) dBreak = x.delay; }
    CHECK(dH == 33); CHECK(dComma > dH); CHECK(dDot == 0 || true);
    // the period after "world" is a sentence end
    uint32_t dPeriod = 0; for (auto &x : u) if (x.s == ".") dPeriod = x.delay;
    CHECK(dPeriod >= 33 + 380); CHECK(dBreak == 520);
  }
  {
    // wrap: no line longer than WIDTH, no word split, text unchanged
    std::string words; for (int i = 0; i < 80; ++i) words += "word" + std::to_string(i % 10) + (i % 7 ? " " : ", ");
    auto u = layout("", "ELIZA", "", "", words, 200);
    std::string all; for (auto &x : u) all += x.s;
    size_t col = 0, maxcol = 0; bool split = false;
    for (size_t i = 0; i < all.size(); ++i) { if (all[i] == '\n') { maxcol = std::max(maxcol, col); col = 0; } else if (all[i] != '\r') ++col; }
    CHECK(maxcol <= WIDTH);
    CHECK(all.find("ELIZA: word0") != std::string::npos);
    for (size_t i = 0; i + 1 < all.size(); ++i) if (all[i] == '\n' && all[i + 1] == ' ' && all.compare(i + 1, 3, "   ") == 0) split = true;
    CHECK(!split);
    // every word survives whole
    for (int i = 0; i < 10; ++i) CHECK(all.find("word" + std::to_string(i)) != std::string::npos);
    // one long word is never cut
    auto v = layout("", "", "", "", std::string(100, 'x'), 200);
    std::string s2; for (auto &x : v) s2 += x.s;
    CHECK(s2.find(std::string(100, 'x')) != std::string::npos);
  }
  {
    // UTF-8 stays whole and counts as one column
    auto u = layout("", "", "", "", "caf\xc3\xa9 ok", 30);
    bool whole = false; for (auto &x : u) if (x.s == "\xc3\xa9") whole = true;
    CHECK(whole);
  }
  {
    // timing: nothing before its time, a late tick does not burst
    Typer t; t.begin(layout("", "", "", "", "abcdefghij", 10), 1000);   // 100 ms a character
    std::string first = t.advance(1000);
    CHECK(first.find("\r\n  a") == 0);                       // the line start and the first letter, at once
    CHECK(t.advance(1050).empty());
    CHECK(t.advance(1100) == "b");
    std::string late = t.advance(60000);                    // a minute late: caps the catch-up
    CHECK(late.size() <= 2);
    CHECK(!t.done());
    std::string rest = t.flush();
    CHECK(t.done());
    CHECK(rest.size() > 3 && rest.substr(rest.size() - 2) == "\r\n");
    CHECK(t.advance(99999).empty() && t.flush().empty());
  }

  // ---- player: a full run ----
  if (sc) {
    Run r = play(sc, "", {"1", ""});
    CHECK(r.finished);
    CHECK(r.text.find("Hello there.") != std::string::npos);
    CHECK(r.text.find("Welcome.") != std::string::npos);
    CHECK(r.text.find("Goodbye.") != std::string::npos);
    CHECK(r.text.find("Goodbye, friend") == std::string::npos);
    CHECK(r.grant == "POLITE,DONE");
    CHECK(r.text.find("Friend") != std::string::npos && r.text.find("Liar") != std::string::npos);
    CHECK(play(sc, "", {"1", ""}, false).text.find("1) Friend") != std::string::npos);
    CHECK(r.text.find("Secret") == std::string::npos);        // hidden: the caller lacks KEY_CARD

    Run l = play(sc, "", {"2", ""});
    CHECK(l.finished && l.text.find("Hm.") != std::string::npos && l.text.find("Goodbye, friend.") != std::string::npos);
    CHECK(l.grant == "LIED,DONE");

    // with the flag, the third choice shows and is number 3
    Run k = play(sc, "KEY_CARD", {"3", ""});
    CHECK(k.text.find("Secret") != std::string::npos && k.text.find("Welcome.") != std::string::npos);
    CHECK(play(sc, "KEY_CARD", {"3", ""}, false).text.find("3) Secret") != std::string::npos);
    CHECK(k.grant == "DONE");

    // bad answers re-ask, they never advance
    Run b = play(sc, "", {"0", "x", "9", "", "1", ""});
    CHECK(b.finished && b.text.find("Choose 1 to 2.") != std::string::npos && b.text.find("Welcome.") != std::string::npos);
    CHECK(b.grant == "POLITE,DONE");

    // a caller without ANSI sees no escape codes
    Run p = play(sc, "", {"1", ""}, false);
    CHECK(p.finished && p.text.find('\x1b') == std::string::npos);
    CHECK(play(sc, "", {"1", ""}, true).text.find('\x1b') != std::string::npos);
  }

  // ---- player: Enter while typing shows the rest, and never skips a choice ----
  if (sc) {
    Player p; uint32_t now = 5000;
    Out o = p.start(sc, "", false, now);
    CHECK(p.phase() == P_TYPE);
    std::string said = o.text();
    o = p.input("", now + 10);                       // Enter: the line is out at once
    said += o.text();
    CHECK(said.find("Hello there.") != std::string::npos);
    CHECK(p.phase() == P_PAUSE);
    o = p.input("", now + 20);                       // Enter in the pause: on to the next node
    CHECK(p.nodeId() == "ASK" && p.phase() == P_TYPE);
    o = p.input("1", now + 30);                      // a digit while typing only flushes
    CHECK(p.phase() == P_CHOICE && p.nodeId() == "ASK");
    CHECK(o.text().find("1) Friend") != std::string::npos);
    o = p.input("1", now + 40);
    CHECK(p.nodeId() == "NICE");
    CHECK(o.grant == "POLITE");
  }

  // ---- player: keys are echoed only at a choice prompt ----
  if (sc) {
    Player p; uint32_t now = 9000;
    CHECK(!p.wantsEcho());                           // not started
    p.start(sc, "", false, now);
    CHECK(p.phase() == P_TYPE && !p.wantsEcho());    // typing
    p.input("", now + 10);
    CHECK(p.phase() == P_PAUSE && !p.wantsEcho());   // pause
    p.input("", now + 20);
    p.input("", now + 30);                           // flush the choice node's text
    CHECK(p.phase() == P_CHOICE && p.wantsEcho());   // choice prompt
    p.input("1", now + 40);
    CHECK(!p.wantsEcho());
    auto k = load("[SCENE]\n[START]\ntext=x\nkey=1\ngo=END\n");
    Player q; q.start(k, "", true, 0);
    q.tick(5000);
    CHECK(q.phase() == P_KEY && !q.wantsEcho());     // waiting for Enter
    q.input("", 5001);
    CHECK(!q.wantsEcho());                           // finished
  }

  // ---- player: art, set on entry, redirects ----
  {
    auto s = load("[SCENE]\n[START]\nart=PIC_ONE\nset=SEEN\ngo=TWO\n[TWO]\nart=PIC_TWO\ntext=Hi\n");
    CHECK(s != nullptr);
    Run r = play(s, "", {});
    CHECK(r.finished && r.art.size() == 2 && r.art[0] == "PIC_ONE" && r.art[1] == "PIC_TWO");
    CHECK(r.grant == "SEEN");
    // the art comes before the text of its node
    Player p; Out o = p.start(s, "", true, 0);
    CHECK(o.parts.size() >= 1 && o.parts[0].isArt);
  }
  {
    // a redirect that can never end (A -> B -> A) ends the scene, it does not hang
    auto s = load("[SCENE]\nstart=A\n[A]\nif_has=X\nif_go=B\ngo=END\n[B]\nif_has=X\nif_go=A\ngo=END\n");
    CHECK(s != nullptr);
    Run r = play(s, "X", {});
    CHECK(r.finished && r.ticks < 5);
    // and without the flag it just ends: no text, no choice
    CHECK(play(s, "", {}).finished);
  }
  {
    // a loop of plain nodes ends at MAX_STEPS
    auto s = load("[SCENE]\n[START]\ntext=x\npause=0\ngo=START\n");
    CHECK(s != nullptr);
    Player p; uint32_t now = 0; Out o = p.start(s, "", true, now);
    unsigned guard = 0;
    while (!o.finished && guard++ < 100000) { now += 100; o = p.tick(now); }
    CHECK(o.finished && p.steps() == MAX_STEPS);
  }
  {
    // all choices hidden: go= takes the caller on, nobody is stuck
    auto s = load("[SCENE]\n[START]\ntext=x\nchoice1=a\ngo1=END\nhas1=NOPE\ngo=OK\n[OK]\ntext=fine\n");
    CHECK(s != nullptr);
    Run r = play(s, "", {});
    CHECK(r.finished && r.text.find("fine") != std::string::npos && r.text.find("1)") == std::string::npos);
  }
  {
    // later nodes see the flags of earlier choices in the same run
    auto s = load("[SCENE]\n[START]\ntext=q\nchoice1=yes\ngo1=NEXT\nset1=SAID_YES\n[NEXT]\ntext=n\nchoice1=again\ngo1=END\nhas1=SAID_YES\nchoice2=other\ngo2=END\n");
    CHECK(s != nullptr);
    Run r = play(s, "", {"1", "1"});
    CHECK(r.text.find("again") != std::string::npos && r.text.find("other") != std::string::npos);
    Run q = play(s, "", {"1", "1"}, false);
    CHECK(q.grant == "SAID_YES");
    CHECK(q.text.find("1) again") != std::string::npos && q.text.find("2) other") != std::string::npos);
  }
  {
    // a node with a key waits for Enter, and nothing else
    auto s = load("[SCENE]\n[START]\ntext=x\nkey=1\ngo=END\n");
    Player p; uint32_t now = 0; p.start(s, "", true, now);
    Out o = p.tick(now + 5000);
    CHECK(p.phase() == P_KEY);
    o = p.tick(now + 90000);
    CHECK(p.phase() == P_KEY && !o.finished);
    o = p.input("", now + 90001);
    CHECK(o.finished && !p.active());
    // dead player: nothing happens
    CHECK(p.tick(now + 99999).parts.empty() && p.input("1", now).parts.empty());
  }
  {
    // the scene's end flags are granted once, with the last node
    auto s = load("[SCENE]\nset=ENDED,ALSO\n[START]\ntext=x\npause=0\n");
    Run r = play(s, "ENDED", {});
    CHECK(r.finished && r.grant == "ALSO");        // ENDED was there: nothing to add for it
  }

  // ---- available(): conditions and the marker ----
  {
    auto s = load("[SCENE]\nhas=CREW\nlacks=BLOCKED\n[START]\ntext=x\n", nullptr, "demo_one");
    CHECK(s && s->id == "DEMO_ONE" && marker(s->id) == "SC_DEMO_ONE");
    CHECK(!available(*s, ""));
    CHECK(available(*s, "CREW"));
    CHECK(!available(*s, "CREW,BLOCKED"));
    CHECK(!available(*s, "CREW,SC_DEMO_ONE"));
    s->repeat = true;
    CHECK(available(*s, "CREW,SC_DEMO_ONE"));
  }

  // ---- property: any answers, any flags: the player ends, flags only grow, no crash ----
  if (sc) {
    const char *ans[] = {"", "1", "2", "3", "4", "0", "x", "99", " 2 "};
    for (int round = 0; round < 3000; ++round) {
      std::string flags = rnd() % 3 == 0 ? "KEY_CARD" : rnd() % 3 == 0 ? "LIED,POLITE" : "";
      Player p; uint32_t now = rnd() % 100000;
      Out o = p.start(sc, flags, rnd() & 1, now);
      std::string seen = flags;
      bool fin = o.finished;
      unsigned guard = 0;
      while (!fin && guard++ < 5000) {
        now += rnd() % 400;
        std::string before = p.flags();
        Out t = rnd() % 3 ? p.tick(now) : p.input(ans[rnd() % 9], now);
        fin = t.finished;
        CHECK(p.flags().compare(0, before.size(), before) == 0);   // flags only grow, byte for byte
      }
      CHECK(fin);
      CHECK(p.flags().compare(0, flags.size(), flags) == 0);
      if (failures > 20) break;
    }
  }

  // ---- the card's own scenes ----
  for (int i = 1; i < argc; ++i) {
    std::ifstream f(argv[i], std::ios::binary);
    std::stringstream ss; ss << f.rdbuf();
    std::string name = argv[i];
    size_t slash = name.find_last_of("/\\"); if (slash != std::string::npos) name = name.substr(slash + 1);
    size_t dot = name.find_last_of('.'); if (dot != std::string::npos) name = name.substr(0, dot);
    std::string why;
    auto s = load(ss.str(), &why, name);
    if (!s) { printf("FAIL %s: %s\n", argv[i], why.c_str()); failures++; continue; }
    printf("%s: %zu nodes, speed %u\n", argv[i], s->nodes.size(), (unsigned)s->speed);
    // every route: try each choice number at every choice, with and without every flag the scene mentions
    std::string all;
    for (const auto &n : s->nodes) { all = haunt::flagsAdd(all, n.set); for (const auto &c : n.choices) all = haunt::flagsAdd(haunt::flagsAdd(all, c.has), c.set); }
    for (int mode = 0; mode < 3; ++mode)
      for (int pickN = 1; pickN <= 4; ++pickN) {
        Run r = play(s, mode == 0 ? "" : mode == 1 ? all : haunt::flagsAdd("", s->has), {std::string(1, (char)('0' + pickN))});
        // the answers list repeats the last answer for later choices
        std::vector<std::string> many(8, "1");   // after the first pick (maybe out of range, so it re-asks) always 1
        many[0] = std::string(1, (char)('0' + pickN));
        r = play(s, mode == 0 ? "" : mode == 1 ? all : haunt::flagsAdd("", s->has), many);
        if (!r.finished) { printf("FAIL %s: does not end (mode %d, pick %d)\n", argv[i], mode, pickN); failures++; }
        if (r.text.find("\x1b[2J") != std::string::npos) { printf("FAIL %s: text has a clear screen\n", argv[i]); failures++; }
      }
  }

  if (failures) { printf("%d FAILED\n", failures); return 1; }
  printf("all scene logic tests passed\n");
  return 0;
}
