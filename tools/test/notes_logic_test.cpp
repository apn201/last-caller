// Host-side test of src/core/notes_logic.h (Q21, the notebook). No board, no Arduino types.
//   zig c++ -std=c++17 -Wall -Wextra -I src/core tools/test/notes_logic_test.cpp -o notes_logic_test
//   ./notes_logic_test sd/BBS/STORY/NOTES.INI   also checks the card's own notes load
#include "notes_logic.h"
#include <stdio.h>
#include <fstream>
#include <sstream>

using namespace notes;

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
static Table load(const std::string &text) { Table t; parse(ini(text), t); return t; }

static const char *DEMO =
  "[A_ONE]\non=read\nmatch=~HELLO\nsrc=Lounge post, Ty\ntext=Ty greets everyone.\nclue=E08\n"
  "[B_TWO]\non=file\nmatch=SHIP.LOG\nsrc=SHIP.LOG\ntext=Lock cycled at 14:10.\nwhere=Library, SHIP.LOG\nreplay=art:SHIPLOG\n"
  "[C_THREE]\non=art\nmatch=ORBIT_3.ANS\nbaud_max=2400\nsrc=Orbit 3, slow\ntext=A word under the paint.\n"
  "[D_FOUR]\non=eliza\nmatch=ME\nsrc=ELIZA, chat\ntext=She is the SysOp.\nreplay=eliza:ME\n"
  "[E_FIVE]\non=scene\nmatch=TY_DESK/MORE\nsrc=Ty's desk\ntext=Say yes to a form.\nhas=CREW\nreplay=scene:TY_DESK\n";

int main(int argc, char **argv) {
  Table t = load(DEMO);
  CHECK(t.notes.size() == 5 && t.skipped == 0);
  CHECK(t.notes[0].flag() == "N_A_ONE" && t.notes[0].kind == K_READ && t.notes[0].match == "~HELLO" && t.notes[0].clue == "E08");
  CHECK(t.notes[1].replay == R_ART && t.notes[1].replayName == "SHIPLOG" && t.notes[1].where == "Library, SHIP.LOG");
  CHECK(t.notes[2].baudMax == 2400);

  // ---- a noted line saves its flag once ----
  std::string flags = "KZI";
  auto d = due(t, K_READ, "~hello", 0, flags);
  CHECK(d.size() == 1 && d[0]->id == "A_ONE");
  std::string grant = flagsOf(d);
  CHECK(grant == "N_A_ONE");
  flags = haunt::flagsAdd(flags, grant);
  CHECK(due(t, K_READ, "~HELLO", 0, flags).empty());                       // shown again: nothing due
  CHECK(haunt::flagsAdd(flags, flagsOf(due(t, K_READ, "~HELLO", 0, flags))) == flags);   // and the flags are the same bytes
  CHECK(flags == "KZI,N_A_ONE");

  // ---- matching ----
  CHECK(due(t, K_READ, "~SOMETHING ELSE", 0, "").empty());
  CHECK(due(t, K_FILE, "~HELLO", 0, "").empty());                          // the kind has to match
  CHECK(due(t, K_FILE, "ship.log", 0, "").size() == 1);                    // match is not case sensitive
  CHECK(due(t, K_ART, "ORBIT_3.ANS", 0, "").empty());                      // baud_max needs a known slow speed
  CHECK(due(t, K_ART, "ORBIT_3.ANS", 9600, "").empty());
  CHECK(due(t, K_ART, "ORBIT_3.ANS", 2400, "").size() == 1);
  CHECK(due(t, K_ART, "ORBIT_3.ANS", 300, "").size() == 1);
  CHECK(due(t, K_SCENE, "TY_DESK/MORE", 0, "").empty());                   // has=CREW
  CHECK(due(t, K_SCENE, "TY_DESK/MORE", 0, "CREW").size() == 1);
  CHECK(due(t, K_NONE, "", 0, "").empty());
  // area= keeps a repeated subject to its own conference
  Table ar = load("[M1]\non=read\nmatch=~DAY 27\narea=1\nsrc=Mission Log\ntext=a\n[G1]\non=read\nmatch=~DAY 27\narea=2\nsrc=Garden Notes\ntext=b\n");
  CHECK(ar.notes.size() == 2 && ar.notes[0].area == 1);
  CHECK(due(ar, K_READ, "~DAY 27", 0, "", 1).size() == 1 && due(ar, K_READ, "~DAY 27", 0, "", 1)[0]->id == "M1");
  CHECK(due(ar, K_READ, "~DAY 27", 0, "", 2)[0]->id == "G1" && due(ar, K_READ, "~DAY 27", 0, "", 5).empty());

  // ---- the notebook lists what is held, in the order the flags were saved ----
  auto h = held(t, "KZI,N_D_FOUR,T_X,N_A_ONE,N_GONE,N_B_TWO,N_A_ONE");
  CHECK(h.size() == 3 && h[0]->id == "D_FOUR" && h[1]->id == "A_ONE" && h[2]->id == "B_TWO");   // a flag with no note is not listed
  CHECK(held(t, "").empty());

  // ---- bad notes: skipped and named, the rest load ----
  Table bad = load("[X1]\non=read\nmatch=1\nsrc=a\n[X2]\non=nope\nmatch=1\nsrc=a\ntext=b\n[X3]\non=read\nsrc=a\ntext=b\n"
                   "[X4]\non=file\nmatch=F\nsrc=a\ntext=b\nreplay=film:X\n[WAY_TOO_LONG_AN_ID]\non=file\nmatch=F\nsrc=a\ntext=b\n"
                   "[OK]\non=file\nmatch=F\nsrc=a\ntext=b\n[X5]\non=file\nmatch=F\nbaud_max=300\nsrc=a\ntext=b\n[OK]\non=file\nmatch=G\nsrc=a\ntext=b\n");
  CHECK(bad.notes.size() == 1 && bad.skipped == 7 && bad.notes[0].id == "OK");
  CHECK(load("[L]\non=login\nsrc=a\ntext=b\n").notes.size() == 1);        // a login note needs no match
  // text is cut, not refused
  CHECK(load(std::string("[LONG]\non=file\nmatch=F\nsrc=a\ntext=") + std::string(200, 'x') + "\n").notes[0].text.size() == MAX_TEXT);

  // ---- the card's own file ----
  for (int i = 1; i < argc; ++i) {
    std::ifstream f(argv[i]);
    std::stringstream ss; ss << f.rdbuf();
    Table c = load(ss.str());
    for (const auto &p : c.problems) printf("  %s\n", p.c_str());
    CHECK(c.skipped == 0);
    CHECK(!c.notes.empty());
    size_t flagChars = 0;
    for (const auto &n : c.notes) flagChars += n.flag().size() + 1;
    printf("%s: %zu notes, %zu flag characters if all are held\n", argv[i], c.notes.size(), flagChars);
    CHECK(flagChars < 2000);   // the story record holds 4000 and carries everything else too
  }
  if (failures) { printf("%d notes logic test(s) failed\n", failures); return 1; }
  printf("all notes logic tests passed\n");
  return 0;
}
