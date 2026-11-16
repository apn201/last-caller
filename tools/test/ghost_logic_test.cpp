// Host-side test of src/core/ghost_logic.h. No board, no Arduino types.
//   zig c++ -std=c++17 -Wall -Wextra -I src/core tools/test/ghost_logic_test.cpp -o ghost_logic_test
//   (or g++/clang++ with the same flags), then run ./ghost_logic_test
//   ./ghost_logic_test sd/BBS/STORY/GHOSTS.INI sd/BBS/STORY/SYSOP.INI  also checks the card's own files load
//   with no skipped entry.
#include "ghost_logic.h"
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>

using namespace ghost;

static int failures = 0;
#define CHECK(cond) do { if (!(cond)) { printf("FAIL line %d: %s\n", __LINE__, #cond); failures++; } } while (0)

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
static Table ghosts(const std::string &text) { Table t; parseGhosts(ini(text), t); return t; }
static Table lines(const std::string &text) { Table t; parseLines(ini(text), t); return t; }
static bool problemHas(const Table &t, const char *part) {
  for (const auto &p : t.problems) if (p.find(part) != std::string::npos) return true;
  printf("  expected a problem with \"%s\"; got %zu problems:\n", part, t.problems.size());
  for (const auto &p : t.problems) printf("    %s\n", p.c_str());
  return false;
}
static std::string slurp(const char *path) { std::ifstream f(path); std::stringstream ss; ss << f.rdbuf(); return ss.str(); }

int main(int argc, char **argv) {
  // ---- ghosts: parse ----
  {
    Table t = ghosts(
      "[CMDR]\nhandle=CMDR\nnode=9\nactivity=Reading mail\nidle=28y\nonline_min=5\nhas=A\nlacks=B\npowered=1\nchat=1\n"
      "page_after=20\npage_from=Eva\npage=Hello|there\nset=SEEN_EVA\n"
      "[TWO]\nhandle=TWO\nnode=0\nfuture_key=1\n");
    CHECK(t.skipped == 0 && t.ghosts.size() == 2);
    const Ghost &g = t.ghosts[0];
    CHECK(g.id == "CMDR" && g.handle == "CMDR" && g.node == 9 && g.activity == "Reading mail" && g.idle == "28y");
    CHECK(g.onlineMin == 5 && g.has == "A" && g.lacks == "B" && g.powered && g.chat);
    CHECK(g.pageAfter == 20 && g.pageFrom == "Eva" && g.page == "Hello|there" && g.set == "SEEN_EVA");
    CHECK(t.ghosts[1].node == 0 && t.ghosts[1].pageFrom == "TWO" && !t.ghosts[1].chat && !t.ghosts[1].powered);
    // the first value of a key wins
    Table f = ghosts("[X]\nhandle=First\nhandle=Second\nnode=1\n");
    CHECK(f.ghosts.size() == 1 && f.ghosts[0].handle == "First");
    // text is capped, control characters dropped
    Table c = ghosts("[X]\nhandle=ABCDEFGHIJKLMNOPQRSTUVWXYZ\nnode=1\nactivity=" + std::string(60, 'a') + "\n");
    CHECK(c.ghosts[0].handle.size() == MAX_HANDLE && c.ghosts[0].activity.size() == MAX_ACTIVITY);
  }
  // ---- ghosts: bad entries are skipped and named, the rest load ----
  {
    Table t = ghosts(
      "[OK1]\nhandle=A\nnode=1\n"
      "[NOHANDLE]\nnode=2\n"
      "[NONODE]\nhandle=B\n"
      "[BIGNODE]\nhandle=B\nnode=100\n"
      "[TEXTNODE]\nhandle=B\nnode=x\n"
      "[SAMENODE]\nhandle=C\nnode=1\n"
      "[PAGENOTIME]\nhandle=D\nnode=3\npage=hi\n"
      "[TIMENOPAGE]\nhandle=D\nnode=3\npage_after=5\n"
      "[ZEROTIME]\nhandle=D\nnode=3\npage_after=0\npage=x\n"
      "[SETONLY]\nhandle=D\nnode=3\nset=Q\n"
      "[BAD NAME]\nhandle=D\nnode=3\n"
      "[OK1]\nhandle=Dup\nnode=4\n"
      "[OK2]\nhandle=E\nnode=5\n");
    CHECK(t.ghosts.size() == 2 && t.ghosts[0].id == "OK1" && t.ghosts[1].id == "OK2");
    CHECK(t.skipped == 11);
    CHECK(problemHas(t, "NOHANDLE: missing handle") && problemHas(t, "NONODE: node must be") && problemHas(t, "BIGNODE: node must be"));
    CHECK(problemHas(t, "TEXTNODE: node must be") && problemHas(t, "SAMENODE: node 1 is taken by OK1"));
    CHECK(problemHas(t, "PAGENOTIME: page needs page_after") && problemHas(t, "TIMENOPAGE: page_after needs page"));
    CHECK(problemHas(t, "ZEROTIME: page_after must be") && problemHas(t, "SETONLY: set is for the page"));
    CHECK(problemHas(t, "BAD NAME: bad ghost name") && problemHas(t, "OK1: duplicate ghost name"));
    // lines outside a section
    Table o = ghosts("handle=x\nnode=1\n");
    CHECK(o.ghosts.empty() && o.skipped == 1 && problemHas(o, "outside"));
    // too many
    std::string many;
    for (int i = 0; i < 30; ++i) many += "[G" + std::to_string(i) + "]\nhandle=h\nnode=" + std::to_string(i) + "\n";
    Table m = ghosts(many);
    CHECK(m.ghosts.size() == MAX_GHOSTS && m.skipped == 6 && problemHas(m, "too many ghosts"));
  }
  // ---- ghosts: visibility and pages ----
  {
    Table t = ghosts(
      "[A]\nhandle=A\nnode=1\n"
      "[B]\nhandle=B\nnode=2\nhas=X,Y\nlacks=Z\nonline_min=5\npowered=1\npage_after=10\npage=hi\nset=GOT\n");
    const Ghost &a = t.ghosts[0], &b = t.ghosts[1];
    Ctx c; c.flags = "x,y"; c.onlineMin = 5; c.signedSec = 9;
    CHECK(visible(a, c) && visible(b, c));
    CHECK(!pageDue(b, c) && !pageDue(a, c));                  // too early; A has no page
    c.signedSec = 10;
    CHECK(pageDue(b, c));
    CHECK(pageFlags(b) == "GOT,G_B" && pageFlags(a) == "G_A");
    c.flags = "X,Y,G_B";                                      // given before: never again
    CHECK(visible(b, c) && !pageDue(b, c));
    c.flags = "X";                                            // missing a flag
    CHECK(!visible(b, c) && visible(a, c));
    c.flags = "X,Y,Z";                                        // a forbidden flag
    CHECK(!visible(b, c));
    c.flags = "X,Y"; c.onlineMin = 4;                         // not online long enough
    CHECK(!visible(b, c));
    c.onlineMin = 5; c.powered = false;                       // brownout
    CHECK(!visible(b, c) && visible(a, c));
  }
  // ---- lines: parse ----
  {
    Table t = lines(
      "[T]\ntrigger=ty_visit\nsay=Hi\nfrom=Eva\n"
      "[K]\nkeys=Who are you?, ELIZA ,  ,who are you\nsay=I am the SysOp.|I help.\ndelay=0\nhas=A\nlacks=B\nset=ASKED\n"
      "[J]\nchat=JOIN\nsay=Hello\n"
      "[E]\nchat=else\nsay=Hm.\n");
    CHECK(t.skipped == 0 && t.lines.size() == 4);
    CHECK(t.lines[0].kind == K_TRIGGER && t.lines[0].trigger == "TY_VISIT" && t.lines[0].once && t.lines[0].from == "Eva");
    const Line &k = t.lines[1];
    CHECK(k.kind == K_KEYS && k.keys.size() == 2 && k.keys[0] == " who are you " && k.keys[1] == " eliza ");   // empty key dropped, duplicate dropped
    CHECK(k.delay == 0 && k.has == "A" && k.lacks == "B" && k.set == "ASKED" && !k.once && k.from == "ELIZA");
    CHECK(t.lines[2].kind == K_JOIN && t.lines[2].once && t.lines[3].kind == K_ELSE && !t.lines[3].once);
    CHECK(lines("[K]\nkeys=a\nsay=x\nonce=1\n").lines[0].once);
  }
  // ---- lines: bad entries ----
  {
    Table t = lines(
      "[OK]\nkeys=a\nsay=x\n"
      "[NOSAY]\nkeys=a\n"
      "[NOKEY]\nsay=x\n"
      "[TWOKEYS]\nkeys=a\ntrigger=T\nsay=x\n"
      "[BADCHAT]\nchat=now\nsay=x\n"
      "[BADTRIG]\ntrigger=a b\nsay=x\n"
      "[BADDELAY]\nkeys=a\nsay=x\ndelay=99999\n"
      "[PUNCT]\nkeys=!!,??\nsay=x\n"
      "[OK]\nkeys=b\nsay=y\n");
    CHECK(t.lines.size() == 1 && t.skipped == 8);
    CHECK(problemHas(t, "NOSAY: missing say") && problemHas(t, "NOKEY: exactly one") && problemHas(t, "TWOKEYS: exactly one"));
    CHECK(problemHas(t, "BADCHAT: chat must be") && problemHas(t, "BADTRIG: bad trigger") && problemHas(t, "BADDELAY: delay must be"));
    CHECK(problemHas(t, "PUNCT: keys has no words") && problemHas(t, "OK: duplicate line name"));
    // a long say is cut, a long key is cut
    Table l = lines("[L]\nkeys=" + std::string(80, 'k') + "\nsay=" + std::string(900, 's') + "\n");
    CHECK(l.lines.size() == 1 && l.lines[0].say.size() == MAX_SAY && l.lines[0].keys[0].size() == MAX_KEY + 2);
    std::string many;
    for (int i = 0; i < 210; ++i) many += "[L" + std::to_string(i) + "]\nkeys=a\nsay=x\n";
    CHECK(lines(many).lines.size() == MAX_LINES);
    // at most 12 keys
    std::string keys;
    for (int i = 0; i < 20; ++i) keys += "w" + std::string(1, (char)('a' + i)) + ",";
    CHECK(lines("[L]\nkeys=" + keys + "\nsay=x\n").lines[0].keys.size() == MAX_KEYS);
  }
  // ---- matching ----
  {
    Table t = lines("[K]\nkeys=who are you,you\nsay=x\n");
    const Line &k = t.lines[0];
    CHECK(hears(k, "Who are YOU?") && hears(k, "hey, you!") && hears(k, "you"));
    CHECK(!hears(k, "yo") && !hears(k, "young") && !hears(k, "your") && !hears(k, "") && !hears(k, "who are"));
    CHECK(hears(k, "who   are\tyou") && hears(k, "...you..."));
    CHECK(norm("A,b!") == " a b " && norm("") == " " && norm("   ") == " ");
  }
  // ---- chat: who speaks, replies, fallbacks, once ----
  {
    Table t = ghosts("[ELIZA]\nhandle=ELIZA\nnode=0\nchat=1\n[TY]\nhandle=TY\nnode=5\nchat=1\nhas=SEEN\n[QUIET]\nhandle=QUIET\nnode=6\n");
    Table l = lines(
      "[R1]\nkeys=name\nsay=I am ELIZA.\n"
      "[R2]\nkeys=ship,halcyon\nsay=The ship is well.\nlacks=KNOWS\nset=KNOWS\n"
      "[R3]\nkeys=ship\nsay=Still well.\n"
      "[TYSAYS]\nkeys=lazy\nsay=Hey.\nfrom=TY\n"
      "[NOBODY]\nkeys=ghost\nsay=Boo.\nfrom=QUIET\n"
      "[HELLO]\nchat=join\nsay=Welcome.|Good evening.\n"
      "[F1]\nchat=else\nsay=Go on.\n"
      "[F2]\nchat=else\nsay=I see.\n"
      "[F3]\nchat=else\nsay=Hm.\nhas=NEVER\n"
      "[AFTER]\ntrigger=Rule_One\nsay=Noted.\nhas=GOT\n");
    t.lines = l.lines;
    Ctx c;
    CHECK(inChat(t, c).size() == 1 && inChat(t, c)[0] == "ELIZA");
    c.flags = "SEEN";
    CHECK(inChat(t, c).size() == 2 && inChat(t, c)[1] == "TY");
    c.flags = "";
    CHECK(present(t, c, "eliza") && !present(t, c, "TY") && !present(t, c, "QUIET") && !present(t, c, "nobody"));
    // keyword lines in file order; set flags are given by the board, so the same flags answer again until KNOWS is there
    const Line *r = chatReply(t, c, "what is your name", 0);
    CHECK(r && r->id == "R1");
    r = chatReply(t, c, "how is the ship?", 0);
    CHECK(r && r->id == "R2" && lineFlags(*r) == "KNOWS");
    c.flags = "KNOWS";
    r = chatReply(t, c, "how is the ship?", 0);                 // R2 lacks KNOWS: R3 answers
    CHECK(r && r->id == "R3" && lineFlags(*r).empty());
    // a line from someone who is not shown to be there is not said
    c.flags = "";
    r = chatReply(t, c, "you are lazy", 0);
    CHECK(r && r->id == "F1");                                  // TYSAYS is skipped, TY is not here: fallback
    c.flags = "SEEN";
    r = chatReply(t, c, "you are lazy", 0);
    CHECK(r && r->id == "TYSAYS");
    c.flags = "";
    r = chatReply(t, c, "boo ghost", 0);
    CHECK(r && r->id == "F1");
    // fallbacks in turn, and only the eligible ones
    CHECK(chatReply(t, c, "zzz", 0)->id == "F1" && chatReply(t, c, "zzz", 1)->id == "F2" && chatReply(t, c, "zzz", 2)->id == "F1");
    c.flags = "NEVER";
    CHECK(chatReply(t, c, "zzz", 2)->id == "F3");
    // G1: never the same answer twice in a row. A repeated question gets the "again" lines in turn, then round again.
    {
      Table g = t;
      g.lines = lines("[R1]\nkeys=name\nsay=I am ELIZA.\n[R9]\nkeys=name\nsay=Eliza, still.\n[R2]\nkeys=ship\nsay=Well.\n"
                      "[F1]\nchat=else\nsay=Go on.\n[A1]\nchat=again\nsay=Said.\n[A2]\nchat=again\nsay=Three times.\n").lines;
      Ctx k;
      CHECK(chatReply(g, k, "name", 0, "", 0)->id == "R1");
      CHECK(chatReply(g, k, "name", 0, "R1", 0)->id == "R9");              // another line with the same keyword answers
      CHECK(chatReply(g, k, "the ship", 0, "R2", 0)->id == "A1");          // only the line just said: the second time
      CHECK(chatReply(g, k, "the ship", 0, "R2", 1)->id == "A2");          // the third time
      CHECK(chatReply(g, k, "the ship", 0, "R2", 2)->id == "A1");          // and round again, never the same twice
      CHECK(chatReply(g, k, "zzz", 0, "F1", 0)->id == "A1");               // one fallback only, just said
      CHECK(chatReply(g, k, "zzz", 0, "R2", 0)->id == "F1");
    }
    // no fallback at all: silence
    Table none = t; none.lines = lines("[R1]\nkeys=name\nsay=x\n").lines;
    CHECK(chatReply(none, Ctx(), "zzz", 0) == nullptr);
    // join lines are once per caller
    c.flags = "";
    auto j = joinLines(t, c);
    CHECK(j.size() == 1 && j[0]->id == "HELLO" && lineFlags(*j[0]) == "E_HELLO");
    c.flags = "E_HELLO";
    CHECK(joinLines(t, c).empty());
    // trigger lines see the flags after the rule; match is case-insensitive
    c.flags = "";
    CHECK(triggerLines(t, c, "rule_one").empty());
    c.flags = "GOT";
    auto a = triggerLines(t, c, "RULE_ONE");
    CHECK(a.size() == 1 && a[0]->id == "AFTER" && lineFlags(*a[0]) == "E_AFTER");
    CHECK(triggerLines(t, c, "OTHER").empty());
    c.flags = "GOT,E_AFTER";
    CHECK(triggerLines(t, c, "RULE_ONE").empty());
    // once=1 on a keyword line
    Table o = t; o.lines = lines("[R]\nkeys=name\nsay=x\nonce=1\n").lines;
    c.flags = "";
    CHECK(chatReply(o, c, "name", 0) != nullptr);
    c.flags = "E_R";
    CHECK(chatReply(o, c, "name", 0) == nullptr);
  }
  // ---- typing ----
  {
    CHECK(typingMs(0) == 400 && typingMs(3) == 400 && typingMs(20) == 1100 && typingMs(1000) == 5000);
    Table t = lines("[L]\nchat=else\nsay=Hello.|  |Second part, longer.|\ndelay=2000\n");
    auto b = beats(t.lines[0]);
    CHECK(b.size() == 2 && b[0].text == "Hello." && b[1].text == "Second part, longer.");
    CHECK(b[0].wait == 2000 + 400 && b[1].wait == 500 + typingMs(20));
    Table z = lines("[L]\nchat=else\nsay=A\ndelay=0\n");
    CHECK(beats(z.lines[0]).size() == 1 && beats(z.lines[0])[0].wait == 400);
  }
  // ---- idle text ----
  {
    CHECK(idleText(0) == "0:00" && idleText(7) == "0:07" && idleText(65) == "1:05" && idleText(3599) == "59:59");
    CHECK(idleText(3600) == "1h00m" && idleText(3600 * 3 + 7 * 60 + 5) == "3h07m" && idleText(86399) == "23h59m");
    CHECK(idleText(86400) == "1d" && idleText(86400 * 400) == "400d");
  }

  // ---- the card's own files ----
  Table card;
  for (int i = 1; i < argc; ++i) {
    std::string path = argv[i], text = slurp(argv[i]);
    if (text.empty()) { printf("FAIL: cannot read %s\n", argv[i]); failures++; continue; }
    Table t;
    bool isGhosts = path.find("GHOSTS") != std::string::npos;
    if (isGhosts) { parseGhosts(ini(text), t); card.ghosts = t.ghosts; } else { parseLines(ini(text), t); card.lines = t.lines; }
    printf("%s: %zu entries, %u skipped\n", argv[i], isGhosts ? t.ghosts.size() : t.lines.size(), t.skipped);
    for (const auto &p : t.problems) printf("  %s\n", p.c_str());
    if (t.skipped) failures++;
  }
  if (!card.ghosts.empty() && !card.lines.empty()) {
    // Slice S6: the SysOp ghost pages a new caller within 30 s, and she is in chat with no conditions.
    Ctx fresh; fresh.signedSec = 30; fresh.onlineMin = 0;
    bool pages = false;
    for (const auto &g : card.ghosts) if (g.handle == "ELIZA" && pageDue(g, fresh)) pages = true;
    CHECK(pages);
    CHECK(present(card, Ctx(), "ELIZA"));
    // every chat line is spoken by someone who can be shown in chat to a fresh caller or to a caller with the line's own flags
    for (const auto &l : card.lines) {
      if (l.kind == K_TRIGGER) continue;
      Ctx c; c.flags = l.has; c.onlineMin = 1000;
      if (!present(card, c, l.from)) { printf("FAIL: line %s is by %s, who is not in chat\n", l.id.c_str(), l.from.c_str()); failures++; }
    }
  }

  if (failures) { printf("%d check(s) failed\n", failures); return 1; }
  printf("all ghost logic tests passed\n");
  return 0;
}
