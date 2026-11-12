// Host-side test of src/core/manifest_logic.h. No board, no Arduino types.
//   zig c++ -std=c++17 -Wall -Wextra -I src/core tools/test/manifest_logic_test.cpp -o manifest_logic_test
//   (or g++/clang++ with the same flags), then run ./manifest_logic_test
//   ./manifest_logic_test sd/BBS/STORY/MANIFEST.INI  also checks the card's own manifest: it loads cleanly,
//   every row can be filed right with the flags its options need, the count of flags stays small,
//   and filing a wrong row says nothing about it.
#include "manifest_logic.h"
#include <stdio.h>
#include <fstream>
#include <set>
#include <sstream>

using namespace manifest;

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
      if (!t.empty() && t[0] != ';' && t[0] != '#') {
        if (t[0] == '[') { size_t c = t.find(']'); section = haunt::upper(haunt::trim(t.substr(1, c == std::string::npos ? std::string::npos : c - 1))); }
        else { size_t eq = t.find('='); if (eq != std::string::npos && eq > 0) out.push_back({section, haunt::lower(haunt::trim(t.substr(0, eq))), haunt::trim(t.substr(eq + 1))}); }
      }
    } else if (text[i] != '\r') line += text[i];
  }
  return out;
}

static const char *DEMO =
    "[MANIFEST]\ntitle=TEST MANIFEST\nfull_scene=full\ncount=Rows confirmed: {n} of {total}.\nfiled=Locked: {names}.\n"
    "[PARTIAL]\ntext=Thanks.|Bye.\n"
    "[STICKY]\ncrew_activity=Crew\n"
    // options
    "[OPT_A1]\nslot=1\ntext=Alice\nhas=KA1\n[OPT_A2]\nslot=2\ntext=Alice fate\nhas=KA2\n[OPT_A3]\nslot=3\ntext=Alice cause\nhas=KA3\n"
    "[OPT_B1]\nslot=1\ntext=Bob\nhas=KB1\n[OPT_B2]\nslot=2\ntext=Bob fate\nhas=KB2\n[OPT_B3]\nslot=3\ntext=Bob cause\nhas=KB3\n"
    "[OPT_C1]\nslot=1\ntext=Cy\nhas=KC1\n[OPT_C2]\nslot=2\ntext=Cy fate\nhas=KC2\n[OPT_C3]\nslot=3\ntext=Cy cause\nhas=KC3\n"
    "[OPT_D1]\nslot=1\ntext=Di\nhas=KD1\n[OPT_D2]\nslot=2\ntext=Di fate\nhas=KD2\n[OPT_D3]\nslot=3\ntext=Di cause\nhas=KD3\n"
    "[OPT_X2]\nslot=2\ntext=A wrong fate\nhas=KA2\n"
    "[OPT_Z1]\nslot=1\ngroup=last\ntext=What\nhas=KZ1\n[OPT_Z2]\nslot=2\ngroup=last\ntext=Who\nhas=KZ2\n[OPT_Z3]\nslot=3\ngroup=last\ntext=Why\nhas=KZ3\n"
    "[OPT_ZX]\nslot=1\ngroup=last\ntext=Something else\n"
    // rows
    "[ROW_AL]\nhandle=ALICE\ndone=M_A\nneeds_any=KA1,KA2,KA3\na1=A1\na2=A2\na3=A3\n"
    "[ROW_BO]\nhandle=BOB\ndone=M_B\nneeds_any=KB1,KB2,KB3\na1=B1\na2=B2\na3=B3\n"
    "[ROW_CY]\nhandle=CY\ndone=M_C\nneeds_any=KC1,KC2,KC3\na1=C1\na2=C2\na3=C3\n"
    "[ROW_DI]\nhandle=DI\ndone=M_D\nneeds_any=KD1,KD2,KD3\na1=D1\na2=D2\na3=D3\n"
    "[ROW_ZZ]\nhandle=ZED\nkind=last\ndone=M_Z\nneeds=M_A,M_B,M_C,M_D\na1=Z1\na2=Z2\na3=Z3\n"
    // notes
    "[NOTE_N1]\nhas=KA1\nabout=alice\ntext=She wrote home.\n"
    "[NOTE_N2]\nhas=KB1,KB2\nabout=BOB\ntext=Two things.\n"
    // broken things, each skipped
    "[OPT_BAD]\nslot=7\ntext=x\n"
    "[OPT_NOTEXT]\nslot=1\n"
    "[NOTE_BAD]\ntext=no flag\n"
    "[ROW_BAD]\ndone=M_X\na1=NOPE\na2=A2\na3=A3\n";

static std::string rightPicks(const Table &t) {
  std::string p;
  for (const auto &r : t.rows) for (int i = 0; i < SLOTS; ++i) setPick(p, r.id, i, r.answer[i]);
  return p;
}
static std::string allFlags(const Table &t, bool withDone = false) {
  std::string f;
  for (const auto &o : t.options) f = haunt::flagsAdd(f, o.has);
  for (const auto &r : t.rows) { f = haunt::flagsAdd(f, r.needsAny); if (!r.last) f = haunt::flagsAdd(f, r.needs); if (withDone) f = haunt::flagsAdd(f, r.done); }
  return f;
}

int main(int argc, char **argv) {
  Table t;
  parse(ini(DEMO), t);
  CHECK(t.title == "TEST MANIFEST");
  CHECK(t.rows.size() == 5);
  CHECK(t.skipped == 4);                    // OPT_BAD, OPT_NOTEXT, NOTE_BAD, ROW_BAD
  CHECK(t.rows.back().last && t.rows.back().handle == "ZED");
  CHECK(t.fullScene == "FULL");
  CHECK(t.crewActivity == "Crew" && t.sentActivity == "Downlink open");
  CHECK(t.notes.size() == 2 && t.notes[0].about == "ALICE");
  CHECK(t.slot[0] == "Who they were" && t.lastSlot[2] == "Why she is still running");

  // a card with a last row before a crew row, or two last rows, keeps the first
  Table order;
  parse(ini(std::string(DEMO) + "[ROW_LATE]\nhandle=LATE\ndone=M_L\na1=A1\na2=A2\na3=A3\n"), order);
  CHECK(order.rows.size() == 5);
  // a row that answers with an option of the wrong blank or list is skipped
  Table wrong;
  parse(ini("[OPT_P]\nslot=1\ntext=p\n[OPT_Q]\nslot=2\ntext=q\n[OPT_R]\nslot=3\ntext=r\n[ROW_X]\ndone=M_X\na1=Q\na2=Q\na3=R\n[ROW_Y]\ndone=M_Y\na1=P\na2=Q\na3=R\n"), wrong);
  CHECK(wrong.rows.size() == 1 && wrong.rows[0].id == "Y" && wrong.skipped == 1);

  // nothing to show without evidence
  CHECK(!available(t, ""));
  CHECK(!available(t, "CREW,T_X"));
  CHECK(available(t, "KB2"));
  auto shown = shownRows(t, "KB2,KD3");
  CHECK(shown.size() == 2 && shown[0]->handle == "BOB" && shown[1]->handle == "DI");
  CHECK(shownRows(t, allFlags(t)).size() == 4);                          // the last row waits for M_A..M_D
  CHECK(shownRows(t, allFlags(t) + ",M_A,M_B,M_C,M_D").size() == 5);

  // pick lists hold only what the caller may see, in file order, per list
  const Row &alice = t.rows[0];
  CHECK(choices(t, alice, 1, "KA1").empty());
  auto fates = choices(t, alice, 1, "KA2");
  CHECK(fates.size() == 2 && fates[0]->id == "A2" && fates[1]->id == "X2");   // the true one and a look-alike
  CHECK(choices(t, alice, 0, "KA1,KB1").size() == 2);                       // identity lists are shared by the crew rows
  const Row &zed = t.rows[4];
  CHECK(choices(t, zed, 0, "").size() == 1 && choices(t, zed, 0, "").front()->id == "ZX");
  CHECK(choices(t, zed, 0, "KZ1").size() == 2);

  // picks as text
  std::string p;
  CHECK(setPick(p, "AL", 0, "a1") && p == "AL=A1,,");
  CHECK(setPick(p, "AL", 2, "A3") && p == "AL=A1,,A3");
  CHECK(setPick(p, "BO", 1, "B2") && p == "AL=A1,,A3;BO=,B2,");
  CHECK(setPick(p, "AL", 0, "") && p == "AL=,,A3;BO=,B2,");
  CHECK(picksOf(p, "bo")[1] == "B2" && picksOf(p, "CY")[0].empty());
  CHECK(setPick(p, "AL", 2, "") && setPick(p, "BO", 1, "") && p.empty());   // empty rows are dropped
  CHECK(!setPick(p, "AL", 3, "A1"));
  CHECK(pickedCount(t, alice, "AL=A1,NOPE,A3") == 2);                       // an unknown id is not a pick
  std::string longPicks;
  bool refused = false;
  for (int i = 0; i < 80 && !refused; ++i) refused = !setPick(longPicks, "R" + std::to_string(i), 0, "A1");
  CHECK(refused && longPicks.size() <= MAX_PICKS);

  // filing: one right row alone is never confirmed
  std::string flags = allFlags(t);
  p = "";
  for (int i = 0; i < SLOTS; ++i) setPick(p, "AL", i, alice.answer[i]);
  Filed f = file(t, flags, p);
  CHECK(f.names.empty() && f.grant.empty() && f.confirmed == 0 && f.total == 4 && !f.full);
  CHECK(filedText(t, f) == "Rows confirmed: 0 of 4.");

  // one right and one wrong: still nothing, and the wrong row is not named
  setPick(p, "BO", 0, "B1"); setPick(p, "BO", 1, "B2"); setPick(p, "BO", 2, "X2");   // wrong cause (a fate in the cause blank)
  f = file(t, flags, p);
  CHECK(f.names.empty());
  std::string text = filedText(t, f);
  CHECK(text == "Rows confirmed: 0 of 4." && text.find("BOB") == std::string::npos && text.find("ALICE") == std::string::npos);

  // two right, one wrong: the pair is confirmed, the wrong row is still not named
  for (int i = 0; i < SLOTS; ++i) setPick(p, "CY", i, t.rows[2].answer[i]);
  f = file(t, flags, p);
  CHECK(f.names.size() == 2 && f.names[0] == "ALICE" && f.names[1] == "CY" && f.confirmed == 2 && f.total == 4);
  CHECK(haunt::flagsHas(f.grant, "M_A") && haunt::flagsHas(f.grant, "M_C") && !haunt::flagsHas(f.grant, "M_B"));
  text = filedText(t, f);
  CHECK(text == "Locked: ALICE and CY.\nRows confirmed: 2 of 4." && text.find("BOB") == std::string::npos);

  // after saving the flags, filing again changes nothing (idempotent) and the wrong row stays silent
  std::string after = haunt::flagsAdd(flags, f.grant);
  Filed again = file(t, after, p);
  CHECK(again.names.empty() && again.grant.empty() && again.confirmed == 2);

  // three right: the first two in file order are confirmed, the third waits for a partner
  for (int i = 0; i < SLOTS; ++i) setPick(p, "BO", i, t.rows[1].answer[i]);
  for (int i = 0; i < SLOTS; ++i) setPick(p, "DI", i, t.rows[3].answer[i]);
  std::string q;
  for (int r = 0; r < 3; ++r) for (int i = 0; i < SLOTS; ++i) setPick(q, t.rows[(size_t)r + 1].id, i, t.rows[(size_t)r + 1].answer[i]);   // BO CY DI
  f = file(t, flags, q);
  CHECK(f.names.size() == 2 && f.names[0] == "BOB" && f.names[1] == "CY" && f.confirmed == 2);
  Filed third = file(t, haunt::flagsAdd(flags, f.grant), q);
  CHECK(third.names.empty() && third.confirmed == 2);                  // DI is right but alone
  // the fourth, later, is paired with it
  std::string withAl = q;
  for (int i = 0; i < SLOTS; ++i) setPick(withAl, "AL", i, alice.answer[i]);
  third = file(t, haunt::flagsAdd(flags, f.grant), withAl);
  CHECK(third.names.size() == 2 && third.names[0] == "ALICE" && third.names[1] == "DI" && third.confirmed == 4 && third.total == 5);

  // all four right at once: two pairs, and the last row is shown as soon as they are
  std::string all = rightPicks(t);
  f = file(t, flags, all);
  CHECK(f.names.size() == 5 || f.names.size() == 4);                    // the last row has picks in `all` too
  CHECK(f.confirmed == (unsigned)f.names.size());
  Table noLast = t;
  noLast.rows.pop_back();
  f = file(noLast, flags, rightPicks(noLast));
  CHECK(f.names.size() == 4 && f.names[0] == "ALICE" && f.names[3] == "DI" && f.confirmed == 4 && f.total == 4 && !f.full);

  // the last row: not before the four, confirmed alone when right, never before the pairs are saved
  std::string crewDone = haunt::flagsAdd(flags, "M_A,M_B,M_C,M_D");
  std::string zp;
  for (int i = 0; i < SLOTS; ++i) setPick(zp, "ZZ", i, zed.answer[i]);
  f = file(t, flags, zp);
  CHECK(f.names.empty() && !f.full && f.total == 4);                   // the row is not shown yet
  f = file(t, crewDone, zp);
  CHECK(f.names.size() == 1 && f.names[0] == "ZED" && f.full && f.confirmed == 5 && f.total == 5);
  CHECK(filedText(t, f) == "Locked: ZED.\nRows confirmed: 5 of 5.");
  setPick(zp, "ZZ", 1, "ZX");                                           // wrong (a look-alike in the wrong blank)
  f = file(t, crewDone, zp);
  CHECK(f.names.empty() && !f.full && f.confirmed == 4 && f.total == 5 && filedText(t, f) == "Rows confirmed: 4 of 5.");

  // a row without evidence cannot be confirmed even with picks written to the file
  f = file(t, "KA1", rightPicks(t));
  CHECK(f.names.empty() && f.confirmed == 0);

  // notebook
  CHECK(notebook(t, "").empty());
  CHECK(notebook(t, "KA1").size() == 1 && notebook(t, "KA1")[0]->text == "She wrote home.");
  CHECK(notebook(t, "KB1").empty() && notebook(t, "KB1,KB2").size() == 1);

  // endings
  CHECK(!finished("M_Z,SC_MANFULL") && finished("END1") && finished("A,END2"));

  // the card's own manifest
  for (int i = 1; i < argc; ++i) {
    std::ifstream in(argv[i], std::ios::binary);
    if (!in) { printf("FAIL cannot open %s\n", argv[i]); failures++; continue; }
    std::stringstream ss; ss << in.rdbuf();
    Table c;
    parse(ini(ss.str()), c);
    for (auto &pr : c.problems) { printf("FAIL %s: %s\n", argv[i], pr.c_str()); failures++; }
    printf("%s: %zu rows, %zu options, %zu notes\n", argv[i], c.rows.size(), c.options.size(), c.notes.size());
    CHECK(c.rows.size() == 5 && c.rows.back().last);
    CHECK(!c.fullScene.empty());
    CHECK(!c.partial.empty());
    // option text must fit the pick list line: "[n] " + text + " (picked)" in 80 columns
    for (auto &o : c.options) if (o.text.size() > 64) { printf("FAIL %s: option %s is over 64 characters\n", argv[i], o.id.c_str()); failures++; }
    // the flags the story needs: evidence (what the options and rows ask for), confirmations, endings
    std::string ev = allFlags(c);
    std::string conf = allFlags(c, true);
    printf("evidence flags: %s (%zu characters); with confirmations %zu\n", ev.c_str(), ev.size(), conf.size());
    CHECK(haunt::flagWords(ev).size() <= 20);
    CHECK(conf.size() + std::string(",END1,END2,SC_MANFULL,SC_ENDING").size() <= 120);   // leaves room in the account's 255
    // no look-alike option may be the right answer of a row
    std::set<std::string> answers;
    for (auto &r : c.rows) for (int k = 0; k < SLOTS; ++k) answers.insert(r.answer[k]);
    unsigned decoys = 0;
    for (auto &o : c.options) if (!answers.count(o.id)) decoys++;
    printf("%u right answers, %u look-alikes\n", (unsigned)answers.size(), decoys);
    CHECK(decoys >= 4);
    // with all the evidence: every row is there, every blank has a pick list, and the right answers are in it
    std::string ev2 = ev;
    for (auto &r : c.rows) {
      std::string fl = r.last ? haunt::flagsAdd(ev2, r.needs) : ev2;
      CHECK(rowShown(r, fl));
      for (int k = 0; k < SLOTS; ++k) {
        bool inList = false;
        for (auto *o : choices(c, r, k, fl)) inList |= o->id == r.answer[k];
        CHECK(inList);
        CHECK(choices(c, r, k, fl).size() >= 2);                   // a list of one is not a choice
      }
    }
    // filing the right picks with all the evidence confirms all five rows, in two pairs and the last
    Filed whole = file(c, ev2, rightPicks(c));
    CHECK(whole.names.size() == 5 && whole.confirmed == 5 && whole.total == 5 && whole.full);
    // filing with nothing right says only the count
    Filed none = file(c, ev2, "");
    CHECK(none.names.empty() && filedText(c, none).find("0 of 4") != std::string::npos);
    // no flag in the card is lower case or empty
    for (auto &n : c.notes) CHECK(!n.has.empty());
  }

  if (failures) { printf("%d FAILED\n", failures); return 1; }
  printf("all manifest logic tests passed\n");
  return 0;
}
