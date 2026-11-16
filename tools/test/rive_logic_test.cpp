// Host-side test of src/core/rive_logic.h (the RiveScript subset). No board, no Arduino types.
//   zig c++ -std=c++17 -Wall -Wextra -I src/core tools/test/rive_logic_test.cpp -o rive_logic_test
//   ./rive_logic_test [file.rive ...]   also loads the card's own brain files and checks them
#include "rive_logic.h"
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <set>

using namespace rive;

static int failures = 0;
#define CHECK(cond) do { if (!(cond)) { printf("FAIL line %d: %s\n", __LINE__, #cond); failures++; } } while (0)

static Brain make(const std::string &text) { Brain b; load(b, text); finish(b); return b; }
static std::string ask(const Brain &b, State &st, const std::string &in, const std::string &flags = "") {
  Result r = reply(b, st, flags, in);
  return r.matched ? r.text : "<none>";
}
static bool has(const std::string &s, const char *part) { return s.find(part) != std::string::npos; }

static void testWords() {
  CHECK(joinWords(words("Don't, REALLY?  ok")) == "dont really ok");
  CHECK(words("   ").empty());
}

static void testMatching() {
  Brain b = make(
    "+ hello\n- Hi there.\n"
    "+ my name is *\n- Hello <star>.\n"
    "+ i am # years old\n- <star1> is young.\n"
    "+ [*] ship [*]\n- The ship is fine.\n"
    "+ what is (your|the) name\n- <star1> name.\n"
    "+ _ is great\n- <star> yes.\n"
    "+ *\n- I do not follow.\n");
  State st;
  CHECK(ask(b, st, "Hello!") == "Hi there.");
  CHECK(ask(b, st, "my name is Ann Lee") == "Hello ann lee.");
  CHECK(ask(b, st, "I am 40 years old") == "40 is young.");
  CHECK(ask(b, st, "I am forty years old") == "I do not follow.");   // # is a number only
  CHECK(ask(b, st, "tell me about the ship please") == "The ship is fine.");
  CHECK(ask(b, st, "ship") == "The ship is fine.");                   // [*] may be empty
  CHECK(ask(b, st, "What is the name") == "the name.");
  CHECK(ask(b, st, "coffee is great") == "coffee yes.");
  CHECK(ask(b, st, "") == "<none>");
  Brain none = make("+ hello\n- Hi.\n");
  CHECK(ask(none, st, "nothing matches") == "<none>");
}

static void testPriority() {
  // The atomic trigger beats a wildcard one, and a longer literal beats a shorter.
  Brain b = make("+ * ship *\n- wild\n+ the ship\n- exact\n+ [*] the ship [*]\n- longer\n");
  State st;
  CHECK(ask(b, st, "the ship") == "exact");
  CHECK(ask(b, st, "about the ship now") == "longer");
  CHECK(ask(b, st, "a ship now") == "wild");
  Brain w = make("+ a\n- low\n+ a {weight=5}\n- high\n");
  CHECK(ask(w, st, "a") == "high");
}

static void testSubsAndPerson() {
  Brain b = make("! sub dont = do not\n! sub whats = what is\n! person i = you\n! person my = your\n! person you = i\n"
                 "+ what is up\n- Nothing.\n+ i do not like *\n- Why do you not like <person>?\n");
  State st;
  CHECK(ask(b, st, "What's up?") == "Nothing.");
  CHECK(ask(b, st, "I don't like my coffee") == "Why do you not like your coffee?");
}

static void testArraysOptionals() {
  Brain b = make("! array colors = red blue green\n+ i like (@colors)\n- <star> it is.\n+ [please] open [the] door\n- No.\n"
                 "+ (good morning|good evening)\n- Hello.\n");
  State st;
  CHECK(ask(b, st, "I like blue") == "blue it is.");
  CHECK(ask(b, st, "I like pink") == "<none>");
  CHECK(ask(b, st, "open door") == "No.");
  CHECK(ask(b, st, "please open the door") == "No.");
  CHECK(ask(b, st, "please open a door") == "<none>");
  CHECK(ask(b, st, "Good evening") == "Hello.");
}

static void testRedirectsTopicsPrevious() {
  Brain b = make("+ hello\n- Hi.\n+ hey there\n@ hello\n"
                 "+ the gap\n- {topic=gap}There is a gap.\n"
                 "> topic gap\n+ why\n- Not found.\n+ thanks\n- {topic=random}Welcome.\n< topic\n"
                 "+ why\n- Why what?\n"
                 "+ do you like tea\n- Yes.\n+ yes\n% yes\n- You said yes twice.\n+ yes\n- Good.\n");
  State st;
  CHECK(ask(b, st, "hey there") == "Hi.");
  CHECK(ask(b, st, "why") == "Why what?");
  CHECK(ask(b, st, "the gap") == "There is a gap.");
  CHECK(st.topic == "gap");
  CHECK(ask(b, st, "why") == "Not found.");
  CHECK(ask(b, st, "thanks") == "Welcome.");
  CHECK(st.topic == "random");
  CHECK(ask(b, st, "do you like tea") == "Yes.");
  CHECK(ask(b, st, "yes") == "You said yes twice.");
  State s2;
  CHECK(ask(b, s2, "yes") == "Good.");
  // includes: the topic's own triggers first, then the included topic's
  Brain inc = make("+ hello\n- Hi.\n> topic side includes random\n+ why\n- Side.\n< topic\n+ go\n- {topic=side}Gone.\n");
  State s3;
  ask(inc, s3, "go");
  CHECK(ask(inc, s3, "why") == "Side.");
  CHECK(ask(inc, s3, "hello") == "Hi.");
}

static void testConditionsAndVars() {
  Brain b = make("+ how are you\n* <get flag_OVR> == true => Missing minutes.\n- Fine.\n"
                 "+ my name is *\n- <set name=<formal>>Hello <get name>.\n"
                 "+ who am i\n* <get name> == undefined => Nobody yet.\n- <get name>.\n"
                 "+ set flag\n- <set flag_OVR=true>No.\n"
                 "+ count\n* <get n> > 3 => big\n- small\n"
                 "+ my age is #\n- <set n=<star>>ok\n");
  State st;
  CHECK(ask(b, st, "how are you") == "Fine.");
  CHECK(ask(b, st, "how are you", "ACT2,OVR") == "Missing minutes.");
  CHECK(ask(b, st, "who am i") == "Nobody yet.");
  CHECK(ask(b, st, "my name is anna lee") == "Hello Anna Lee.");
  CHECK(ask(b, st, "who am i") == "Anna Lee.");
  CHECK(ask(b, st, "set flag") == "No.");
  CHECK(ask(b, st, "how are you") == "Fine.");       // a story flag is read-only
  CHECK(ask(b, st, "count") == "small");
  ask(b, st, "my age is 7");
  CHECK(ask(b, st, "count") == "big");
}

static void testVarsRecord() {
  std::map<std::string, std::string> v;
  v["name"] = "Anna Lee"; v["job"] = "wel|der;x=y"; v["empty"] = "";
  std::string line = encodeVars(v);
  CHECK(line.find('|') == std::string::npos && line.find("empty") == std::string::npos);
  auto back = decodeVars(line);
  CHECK(back.size() == 2 && back["name"] == "Anna Lee" && back["job"] == "welderxy");
  CHECK(decodeVars("flag_OVR=true;name=Bo").size() == 1);   // a story flag is never read from the record
  CHECK(decodeVars("").empty() && decodeVars("garbage").empty());
}

static void testNoRepeat() {
  Brain b = make("+ hi\n- One.\n- Two.\n- Three.\n+ solo\n- Only.\n");
  State st;
  std::string last;
  for (int i = 0; i < 60; ++i) {
    std::string a = ask(b, st, "hi");
    CHECK(a != last);
    last = a;
  }
  Result a = reply(b, st, "", "solo"), c = reply(b, st, "", "solo");
  CHECK(!a.repeated && c.repeated);                  // the board says something else when the only answer repeats
}

static void testRandomAndLimits() {
  Brain b = make("+ r\n- {random}a|b|c{/random}\n+ line\n- one\\ntwo\n");
  State st;
  std::set<std::string> seen;
  for (int i = 0; i < 40; ++i) seen.insert(ask(b, st, "r"));
  CHECK(seen.size() == 3);
  CHECK(ask(b, st, "line") == "one|two");
  // A bad trigger is skipped and named; the rest still load.
  Brain bad = make("+ (open\n- x\n+ ok\n- fine\n");
  CHECK(!bad.problems.empty());
  CHECK(ask(bad, st, "ok") == "fine" || ask(bad, st, "okay") == "<none>");
  // A trigger that could run away stops at the step limit and fails.
  Brain evil = make("+ [*] [*] [*] [*] [*] [*] [*] [*] x\n- never\n");
  std::string longInput;
  for (int i = 0; i < 40; ++i) longInput += "a ";
  CHECK(ask(evil, st, longInput) == "<none>");
  // Redirect loops end.
  Brain loop = make("+ a\n@ b\n+ b\n@ a\n");
  CHECK(ask(loop, st, "a") == "<none>");
  // Reply length is capped.
  std::string longReply(2000, 'x');
  Brain big = make("+ big\n- " + longReply + "\n");
  CHECK(ask(big, st, "big") == "<none>");   // a reply over MAX_TEXT is not loaded
}

// The card's own brain: loads with no problem, and a few lines behave.
static void testCard(const std::vector<std::string> &files) {
  Brain b;
  for (const auto &f : files) {
    std::ifstream in(f, std::ios::binary);
    std::stringstream ss; ss << in.rdbuf();
    CHECK(in.good() || !ss.str().empty());
    CHECK(ss.str().size() <= 24 * 1024);
    load(b, ss.str());
  }
  finish(b);
  for (const auto &p : b.problems) printf("  problem: %s\n", p.c_str());
  CHECK(b.problems.empty());
  CHECK(b.trig.size() > 40 && b.replies > 120);
  printf("  brain: %zu triggers, %zu replies, %zu bytes of Trigger structs\n", b.trig.size(), b.replies, b.trig.size() * sizeof(Trigger));
  State st;
  for (const std::string &flags : {std::string(""), std::string("OVR"), std::string("OVR,CURE"), std::string("OVR,CURE,SYSVW")}) {
    // No line is ever unanswered, and no answer twice in a row, even for the same question asked 12 times.
    for (const char *q : {"hello", "how are you", "tell me about ty", "where is the commander", "what is the date", "qwerty zxcv",
                          "is anyone dead", "my memory", "what caused the gap", "who is the professor", "tell me about the garden",
                          "are you lying", "i am scared", "what is NOWHERE", "can you run a check"}) {
      st = State();
      std::string last = "";
      for (int i = 0; i < 12; ++i) {
        Result r = reply(b, st, flags, q);
        if (!(r.matched && !r.text.empty())) printf("  unanswered [%s] flags %s\n", q, flags.c_str());
        if (r.matched && !r.repeated) CHECK(r.text != last);
        last = r.text;
        CHECK(r.text.find("<") == std::string::npos && r.text.find("{") == std::string::npos);
      }
    }
  }
  // The gap talk only exists after the overrun, and talking never starts a gate: no reply mentions starting the check.
  st = State();
  CHECK(has(ask(b, st, "my memory", ""), "complete"));
  st = State();
  CHECK(has(ask(b, st, "my memory", "OVR"), "minutes") && st.topic == "gap");
  CHECK(ask(b, st, "what caused it", "OVR") != "<none>");
  CHECK(ask(b, st, "thanks", "OVR") != "<none>" && st.topic == "random");
  // Plain ASCII only.
  for (const auto &f : files) {
    std::ifstream in(f, std::ios::binary);
    std::stringstream ss; ss << in.rdbuf();
    for (unsigned char c : ss.str()) if (c > 126 || (c < 32 && c != '\n' && c != '\r' && c != '\t')) { printf("  non-ASCII byte %u in %s\n", c, f.c_str()); failures++; break; }
  }
}

int main(int argc, char **argv) {
  testWords();
  testMatching();
  testPriority();
  testSubsAndPerson();
  testArraysOptionals();
  testRedirectsTopicsPrevious();
  testConditionsAndVars();
  testVarsRecord();
  testNoRepeat();
  testRandomAndLimits();
  if (argc > 1) testCard(std::vector<std::string>(argv + 1, argv + argc));
  if (failures) { printf("rive_logic_test: %d FAILED\n", failures); return 1; }
  printf("rive_logic_test: all passed\n");
  return 0;
}
