// Host-side test of src/core/haunt_logic.h. No board, no Arduino types.
//   zig c++ -std=c++17 -Wall -Wextra -I src/core tools/test/haunt_logic_test.cpp -o haunt_logic_test
//   (or g++/clang++ with the same flags), then run ./haunt_logic_test
//   ./haunt_logic_test sd/BBS/STORY/TRIGGERS.INI  also checks the card's own rules load
#include "haunt_logic.h"
#include <stdio.h>
#include <stdlib.h>

using namespace haunt;

static int failures = 0;
#define CHECK(cond) do { if (!(cond)) { printf("FAIL line %d: %s\n", __LINE__, #cond); failures++; } } while (0)

static uint32_t lcg = 987654321;
static uint32_t rnd() { lcg = lcg * 1664525u + 1013904223u; return lcg >> 8 | lcg << 24; }

// A tiny INI reader for the tests, the same shape iniLoad() makes: sections
// upper case, keys lower case.
static std::vector<Kv> ini(const std::string &text) {
  std::vector<Kv> out;
  std::string section, line;
  size_t i = 0;
  while (i <= text.size()) {
    if (i == text.size() || text[i] == '\n') {
      std::string t = trim(line);
      line.clear();
      if (!t.empty() && t[0] != ';') {
        if (t[0] == '[') { size_t c = t.find(']'); section = upper(trim(t.substr(1, c == std::string::npos ? std::string::npos : c - 1))); }
        else { size_t eq = t.find('='); if (eq != std::string::npos && eq > 0) out.push_back({section, lower(trim(t.substr(0, eq))), trim(t.substr(eq + 1))}); }
      }
    } else line += text[i];
    ++i;
  }
  return out;
}
static Table load(const std::string &text) { Table t; parse(ini(text), t); return t; }
static Ctx ctx(Ev ev, const std::string &match = "", const std::string &flags = "", int level = 10) {
  Ctx c; c.ev = ev; c.match = match; c.flags = flags; c.level = level; return c;
}

int main(int argc, char **argv) {
  // With a path: the card's own TRIGGERS.INI must load with nothing skipped.
  if (argc > 1) {
    FILE *f = fopen(argv[1], "rb");
    CHECK(f != nullptr);
    if (f) {
      std::string text; char buf[4096]; size_t n;
      while ((n = fread(buf, 1, sizeof(buf), f)) > 0) text.append(buf, n);
      fclose(f);
      for (auto &ch : text) if (ch == '\r') ch = ' ';
      Table card = load(text);
      for (const auto &p : card.problems) printf("card: %s\n", p.c_str());
      CHECK(card.skipped == 0 && !card.rules.empty());
      printf("card: %u rules\n", (unsigned)card.rules.size());
    }
  }
  // ---- flags ----
  {
    CHECK(flagsClean(" sysop, backroom ,SYSOP,a-b") == "SYSOP,BACKROOM,AB");
    CHECK(flagsHas("SYSOP,CREW", "crew") && !flagsHas("SYSOP,CREW", "CRE") && !flagsHas("", "X") && !flagsHas("A", ""));
    CHECK(flagsAdd("A,B", "b,c") == "A,B,C");
    CHECK(flagsAdd("", "x") == "X");
    CHECK(flagsAdd("A", "") == "A");
    // The old list is kept byte for byte, even when it is not tidy.
    CHECK(flagsAdd("b,a", "c") == "b,a,C");
    // The cap: a flag that does not fit is named, never silently lost, and nothing is rewritten.
    std::string full;
    for (int i = 0; full.size() < MAX_STORY_FLAGS - 4 && i < 4000; ++i) full = flagsAdd(full, "F" + std::to_string(i));
    CHECK(full.size() <= MAX_STORY_FLAGS);
    std::string dropped, more = flagsAdd(full, "ZZZZZZZZZZ,Y", &dropped);
    CHECK(more.size() <= MAX_STORY_FLAGS && more.compare(0, full.size(), full) == 0);
    CHECK(dropped.find("ZZZZZZZZZZ") != std::string::npos);
    CHECK(flagsAdd(more, "ZZZZZZZZZZ") == more);
  }
  // ---- property: flags only grow, adding twice changes nothing ----
  {
    std::string flags;
    for (int i = 0; i < 1500; ++i) {
      std::string add;
      int n = 1 + rnd() % 3;
      for (int k = 0; k < n; ++k) { if (k) add += ','; add += "F" + std::to_string(rnd() % 90); }
      std::string next = flagsAdd(flags, add);
      CHECK(next.size() >= flags.size() && next.compare(0, flags.size(), flags) == 0);
      CHECK(next.size() <= MAX_STORY_FLAGS);
      CHECK(flagsAdd(next, add) == next);
      for (const auto &w : flagWords(flags)) CHECK(flagsHas(next, w));
      flags = next;
    }
  }
  // ---- the story record (Q7): the flag field keeps access flags, the rest moves ----
  {
    Split sp = splitFlags("sysop,CREW,KHI,readonly,T_X,BACKROOM,SC_ELIZA_WAKE,vault");
    CHECK(sp.access == "SYSOP,READONLY,BACKROOM,VAULT");
    CHECK(sp.story == "CREW,KHI,T_X,SC_ELIZA_WAKE");
    CHECK(splitFlags("").access.empty() && splitFlags("").story.empty());
    // The old 255-character field overflowed with 40 evidence items, trigger and scene markers.
    // The story record takes a full game: 300 flags of up to 12 characters.
    std::string story;
    std::string dropped;
    for (int i = 0; i < 300; ++i) story = flagsAdd(story, "T_RULE_" + std::to_string(i), &dropped);
    CHECK(dropped.empty() && story.size() > 255 && story.size() <= MAX_STORY_FLAGS);
    // Property: through any mix of migration (split + merge), grants and carries, every flag ever held is still held.
    std::string field, record, everything;
    for (int i = 0; i < 3000; ++i) {
      std::string add;
      int n = 1 + rnd() % 3;
      for (int k = 0; k < n; ++k) {
        if (k) add += ',';
        switch (rnd() % 5) {
          case 0: add += "SYSOP"; break;
          case 1: add += "READONLY"; break;
          case 2: add += "K" + std::string(1, (char)('A' + rnd() % 26)) + std::string(1, (char)('A' + rnd() % 3)); break;
          case 3: add += "T_R" + std::to_string(rnd() % 120); break;
          default: add += "SC_S" + std::to_string(rnd() % 40); break;
        }
      }
      everything = flagsAdd(everything, add);
      int how = rnd() % 3;
      if (how == 0) {          // a trigger grants: always to the record
        std::string before = record;
        record = flagsAdd(record, add);
        CHECK(record.compare(0, before.size(), before) == 0);
      } else if (how == 1) {   // an old account: the field gets story flags, then the next sign-in moves them
        field = flagsAdd(field, add, nullptr, MAX_FLAGS);
        Split s2 = splitFlags(field);
        std::string before = record, d2;
        std::string merged = flagsAdd(record, s2.story, &d2);
        CHECK(d2.empty());
        CHECK(merged.compare(0, before.size(), before) == 0);
        record = merged;
        field = s2.access;
      } else {                 // a crew visit carried flags into the record
        record = flagsAdd(record, add);
      }
      CHECK(field.size() <= MAX_FLAGS && record.size() <= MAX_STORY_FLAGS);
    }
    // What the account holds is the union, and it holds all of what was granted, except the access flags
    // that were only ever asked for the record (those are fine in the record too: hasFlag looks at both).
    std::string held = flagsClean(field + "," + record);
    for (const auto &w : flagWords(everything)) {
      bool ok = flagsHas(held, w);
      if (!ok) { printf("lost flag %s\n", w.c_str()); }
      CHECK(ok);
    }
    for (const auto &w : flagWords(field)) CHECK(accessFlag(w));
  }
  // ---- events added in Q7: screen, and a message named by its subject ----
  {
    Table t = load("[USERS]\nevent=screen\nmatch=users\nset=KHI\n[CREWB]\nevent=screen\nmatch=bulletin the crew\nset=KAI\n[SUBJ]\nevent=read\nmatch=~on the thermostat\nset=KAI\n");
    CHECK(t.skipped == 0 && t.rules.size() == 3);
    CHECK(due(t, ctx(EV_SCREEN, "USERS")).size() == 1);
    CHECK(due(t, ctx(EV_SCREEN, "BULLETIN THE CREW")).size() == 1);
    CHECK(due(t, ctx(EV_SCREEN, "BULLETIN RULES")).empty());
    CHECK(due(t, ctx(EV_READ, "~ON THE THERMOSTAT")).size() == 1);
    CHECK(due(t, ctx(EV_READ, "12")).empty());
    CHECK(due(t, ctx(EV_SCREEN, "USERS", "KHI")).empty());   // set-only rule: spent once held
  }
  // ---- parsing: a good file ----
  const char *good =
    "; comment\n"
    "[VISIT_TY]\nevent=crew_login\nmatch=tyvance\nset=CREW,TY_VISIT\n"
    "[WAKE_MAIL]\nevent=login\nhas=TY_VISIT\nlevel_min=10\nset=KNOWS_TY\n"
    "mail_from=ELIZA\nmail_conf=CREW MAIL\nmail_subject=Welcome back\nmail_body=Line one.|Line two.\n"
    "[PAGE_IDLE]\nevent=idle\nafter=90\nlacks=PAGED\nset=paged\npage_from=ELIZA\npage=Are you still there?\n"
    "[HUM]\nevent=minutes\nafter=5\nart=hum\n"
    "[DOOR_V]\nevent=door\nmatch=v\nset=SAW_DOOR\n"
    "[SLOW_ART]\nevent=art\nmatch=orbit3.ans\nbaud_max=2400\nset=SAW_UNDER\n"
    "[FILE_LOG]\nevent=file\nmatch=capsule.log\nset=READ_CAPSULE\npost_from=GARDEN\npost_conf=LOUNGE\npost_subject=Re: hatch\npost_body=Noted.\n"
    "[ADDED]\nevent=read\nmatch=12\nhas=READ_12\nadd=A line that was not there before.\n";
  Table t = load(good);
  CHECK(t.skipped == 0);
  CHECK(t.rules.size() == 8);
  if (t.rules.size() == 8) {
    CHECK(t.rules[0].id == "VISIT_TY" && t.rules[0].ev == EV_CREW_LOGIN && t.rules[0].match == "TYVANCE" && t.rules[0].set == "CREW,TY_VISIT");
    CHECK(t.rules[1].mailBody == "Line one.|Line two." && t.rules[1].visible());
    CHECK(t.rules[2].after == 90 && t.rules[2].set == "PAGED" && t.rules[2].visible());
    CHECK(t.rules[5].baudMax == 2400);
  }

  // ---- events and match ----
  {
    CHECK(due(t, ctx(EV_LOGIN, "", "")).empty());                    // WAKE_MAIL needs TY_VISIT
    auto d = due(t, ctx(EV_LOGIN, "", "ty_visit"));                  // lower case flags still count
    CHECK(d.size() == 1 && d[0]->id == "WAKE_MAIL");
    CHECK(due(t, ctx(EV_CREW_LOGIN, "tyvance")).size() == 1);        // case-insensitive match
    CHECK(due(t, ctx(EV_CREW_LOGIN, "CMDR")).empty());
    CHECK(due(t, ctx(EV_DOOR, "V")).size() == 1 && due(t, ctx(EV_DOOR, "D")).empty());
    CHECK(due(t, ctx(EV_FILE, "CAPSULE.LOG")).size() == 1 && due(t, ctx(EV_FILE, "OTHER.LOG")).empty());
    CHECK(due(t, ctx(EV_READ, "12", "READ_12")).size() == 1 && due(t, ctx(EV_READ, "12", "")).empty() && due(t, ctx(EV_READ, "13", "READ_12")).empty());
    CHECK(due(t, ctx(EV_FILE, "orbit3.ans")).empty());               // an art rule is not a file rule
  }
  // ---- baud ----
  {
    Ctx c = ctx(EV_ART, "ORBIT3.ANS");
    c.baud = 2400; CHECK(due(t, c).size() == 1);
    c.baud = 1200; CHECK(due(t, c).size() == 1);
    c.baud = 9600; CHECK(due(t, c).empty());
    c.baud = 0;    CHECK(due(t, c).empty());                         // no limit is not slow
  }
  // ---- idle and minutes thresholds ----
  {
    Ctx c = ctx(EV_IDLE);
    c.amount = 89; CHECK(due(t, c).empty());
    c.amount = 90; CHECK(due(t, c).size() == 1);
    c.amount = 4000; CHECK(due(t, c).size() == 1);
    c.flags = "PAGED,T_PAGE_IDLE"; CHECK(due(t, c).empty());
    Ctx m = ctx(EV_MINUTES);
    m.amount = 4; CHECK(due(t, m).empty());
    m.amount = 5; CHECK(due(t, m).size() == 1);
    m.flags = "T_HUM"; CHECK(due(t, m).empty());
  }
  // ---- level ----
  {
    Table l = load("[A]\nevent=login\nlevel_min=20\nlevel_max=30\nset=X\n");
    CHECK(due(l, ctx(EV_LOGIN, "", "", 19)).empty());
    CHECK(due(l, ctx(EV_LOGIN, "", "", 20)).size() == 1);
    CHECK(due(l, ctx(EV_LOGIN, "", "", 30)).size() == 1);
    CHECK(due(l, ctx(EV_LOGIN, "", "", 31)).empty());
  }
  // ---- lacks ----
  {
    Table l = load("[A]\nevent=login\nlacks=X,Y\nset=Z\n");
    CHECK(due(l, ctx(EV_LOGIN, "", "")).size() == 1);
    CHECK(due(l, ctx(EV_LOGIN, "", "Y")).empty());
    CHECK(due(l, ctx(EV_LOGIN, "", "X,Y")).empty());
  }
  // ---- idempotent: a rule applied twice does nothing the second time ----
  {
    std::string flags = "TY_VISIT";
    Ctx c = ctx(EV_LOGIN, "", flags);
    auto d = due(t, c);
    CHECK(d.size() == 1);
    flags = flagsAdd(flags, flagsOf(*d[0]));
    CHECK(flagsHas(flags, "T_WAKE_MAIL") && flagsHas(flags, "KNOWS_TY"));
    c.flags = flags;
    CHECK(due(t, c).empty());                                       // visible rule: marker spends it
    // set-only rule: due until its flags are there, then spent
    Ctx v = ctx(EV_CREW_LOGIN, "TYVANCE", "");
    CHECK(due(t, v).size() == 1);
    v.flags = flagsAdd("", flagsOf(t.rules[0]));
    CHECK(due(t, v).empty());
    v.flags = "CREW";                                               // only one of the two flags
    CHECK(due(t, v).size() == 1);
  }
  // ---- no chaining within one event ----
  {
    Table l = load("[ONE]\nevent=login\nset=B\n[TWO]\nevent=login\nhas=B\nset=C\n");
    Ctx c = ctx(EV_LOGIN, "", "");
    auto d = due(l, c);
    CHECK(d.size() == 1 && d[0]->id == "ONE");                      // TWO waits for the next event
    c.flags = flagsAdd(c.flags, flagsOf(*d[0]));
    d = due(l, c);
    CHECK(d.size() == 1 && d[0]->id == "TWO");
  }
  // ---- add= rules show on every re-read and are never spent ----
  {
    Ctx c = ctx(EV_READ, "12", "READ_12,T_ADDED");
    CHECK(due(t, c).size() == 1);
    CHECK(due(t, c).size() == 1);
  }
  // ---- flagsOf ----
  {
    CHECK(flagsOf(t.rules[2]) == "PAGED,T_PAGE_IDLE");
    CHECK(flagsOf(t.rules[3]) == "T_HUM");                          // no set=: marker only
    CHECK(flagsOf(t.rules[4]) == "SAW_DOOR");                       // set-only: no marker
  }
  // ---- malformed files never throw and never load a bad rule ----
  {
    struct Bad { const char *text; const char *why; } bad[] = {
      {"[A]\nset=X\n", "no event"},
      {"[A]\nevent=sneeze\nset=X\n", "unknown event"},
      {"[A]\nevent=login\n", "no effect"},
      {"[A]\nevent=idle\nset=X\n", "idle without after"},
      {"[A]\nevent=idle\nafter=0\nset=X\n", "after zero"},
      {"[A]\nevent=idle\nafter=ten\nset=X\n", "after not a number"},
      {"[A]\nevent=login\nafter=5\nset=X\n", "after on login"},
      {"[A]\nevent=login\nbaud_max=2400\nset=X\n", "baud on login"},
      {"[A]\nevent=art\nbaud_max=0\nset=X\n", "baud zero"},
      {"[A]\nevent=login\nlevel_min=300\nset=X\n", "level out of range"},
      {"[A]\nevent=login\nlevel_min=50\nlevel_max=10\nset=X\n", "levels reversed"},
      {"[A]\nevent=login\nlevel_min=-1\nset=X\n", "negative level"},
      {"[A]\nevent=crew_login\nset=X\n", "crew_login without match"},
      {"[A]\nevent=login\nmail_body=hi\n", "mail without from and subject"},
      {"[A]\nevent=login\nmail_from=E\n", "mail without a body"},
      {"[A]\nevent=login\npost_body=hi\npost_from=E\n", "post without conf"},
      {"[A B]\nevent=login\nset=X\n", "space in rule name"},
      {"[AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA]\nevent=login\nset=X\n", "rule name too long"},
      {"[]\nevent=login\nset=X\n", "empty section name"},
      {"event=login\nset=X\n", "lines before any section"},
      {"[A]\nevent=login\nart=../../etc\n", "path in art name"},
      {"[A]\nevent=login\nadd=hi\n", "add on login"},
      {"[A]\nevent=read\nadd=hi\npage=x\n", "add with page"},
      {"[A]\nevent=read\nadd=hi\nset=X\n", "add with set"},
    };
    for (const auto &b : bad) {
      Table x = load(b.text);
      if (!(x.rules.empty() && x.skipped >= 1)) { printf("not rejected: %s\n", b.why); failures++; }
    }
    // One bad rule does not stop the good ones around it.
    Table mix = load("[A]\nevent=login\nset=ONE\n[B]\nevent=nope\nset=X\n[C]\nevent=login\nset=TWO\n");
    CHECK(mix.rules.size() == 2 && mix.skipped == 1 && mix.problems.size() == 1);
    // A duplicate name later in the file is skipped; the first one stays.
    Table dup = load("[A]\nevent=login\nset=ONE\n[B]\nevent=login\nset=X\n[A]\nevent=login\nset=TWO\n");
    CHECK(dup.rules.size() == 2 && dup.rules[0].set == "ONE" && dup.skipped == 1);
    // A key given twice in one rule: the first value wins, as iniGet() does.
    Table twice = load("[A]\nevent=login\nset=ONE\nset=TWO\n");
    CHECK(twice.rules.size() == 1 && twice.rules[0].set == "ONE");
    // A missing file is an empty file.
    Table none = load("");
    CHECK(none.rules.empty() && none.skipped == 0 && due(none, ctx(EV_LOGIN)).empty());
  }
  // ---- limits ----
  {
    std::string big;
    for (int i = 0; i < 350; ++i) big += "[R" + std::to_string(i) + "]\nevent=login\nset=F" + std::to_string(i) + "\n";
    Table x = load(big);
    CHECK(x.rules.size() == MAX_RULES && x.skipped == 50);
    CHECK(x.problems.size() <= 20);
    // Long text is cut, control characters are removed.
    std::string longText(5000, 'x');
    Table y = load("[A]\nevent=login\npage=" + longText + "\n[B]\nevent=login\npage=a\x01" "b\x7f" "c\n");
    CHECK(y.rules.size() == 2 && y.rules[0].page.size() == MAX_LINE && y.rules[1].page == "abc");
  }
  // ---- fuzz: random garbage never crashes the parser, and every loaded rule is sane ----
  {
    const char *words[] = {"[", "]", "=", "\n", "event", "login", "read", "idle", "set", "after", "match", "has", "lacks",
                           "level_min", "level_max", "page", "mail_body", "art", "A", "B_1", "12", "-3", "99999999999", "|", ";", " ", "\t", "\x01", "\xff"};
    for (int round = 0; round < 3000; ++round) {
      std::string text;
      int n = rnd() % 60;
      for (int k = 0; k < n; ++k) text += words[rnd() % (sizeof(words) / sizeof(*words))];
      Table x = load(text);
      CHECK(x.rules.size() <= MAX_RULES);
      for (const auto &r : x.rules) {
        CHECK(idOk(r.id) && r.ev != EV_NONE && r.levelMin <= r.levelMax);
        CHECK(r.page.size() <= MAX_LINE && r.mailBody.size() <= MAX_BODY && r.set.size() <= text.size() + 1);
        CHECK(!r.set.empty() || r.visible() || !r.add.empty());
        Ctx c = ctx(r.ev, r.match, r.set);
        c.amount = r.after; c.baud = r.baudMax;
        (void)due(x, c);                                             // must not crash
      }
    }
  }
  // Gates (Q12d): a held flag passes; SYSOP view passes every gate but the access flags.
  CHECK(gatePass("", ""));
  CHECK(gatePass("ACT2", "ACT2"));
  CHECK(!gatePass("", "ACT2"));
  CHECK(!gatePass("ACT3", "ACT2"));
  CHECK(gatePass("ACT2,ACT3", "ACT2,ACT3"));
  CHECK(!gatePass("ACT2", "ACT2,ACT3"));
  CHECK(gatePass("SYSVW", "ACT2,ACT3,VISIT_CMDR"));
  CHECK(gatePass("sysvw", "act3"));
  for (const char *a : {"SYSOP", "READONLY", "BACKROOM", "VAULT"}) {
    CHECK(!gatePass("SYSVW", a));
    CHECK(!gatePass("SYSVW,ACT2", std::string("ACT2,") + a));
    CHECK(gatePass(std::string("SYSVW,") + a, a));
  }

  printf(failures ? "%d FAILED\n" : "all passed\n", failures);
  return failures ? 1 : 0;
}
