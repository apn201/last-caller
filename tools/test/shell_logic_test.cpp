// Host-side test of src/core/shell_logic.h. No board, no Arduino types.
//   zig c++ -std=c++17 -Wall -Wextra -I src/core tools/test/shell_logic_test.cpp -o shell_logic_test
//   (or g++/clang++ with the same flags), then run ./shell_logic_test
//   ./shell_logic_test sd/BBS/STORY/SHELL.INI  also checks the card's own shell loads cleanly,
//   and (with sd/BBS/STORY/DIAG.INI as a second argument) that the diagnostics table is sound.
#include "shell_logic.h"
#include <stdio.h>
#include <fstream>
#include <set>
#include <sstream>

using namespace nshell;

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
    "[SHELL]\nhost=haven\nuser=guest\nfinal_cmd=Shut Down  Now\nfinal_has=SEEN_ALL\nfinal_set=ENDED\nfinal_text=Done.\n"
    "[/etc/motd]\ntext=Your data is NOWHERE.|Second line.\n"
    "[/orders]\ndir=1\n"
    "[/orders/001]\ntext=order one\n"
    "[/orders/002]\ntext=order two\nhas=KEY\n"
    "[/.hidden]\ntext=dot\n"
    "[/bad path]\ntext=x\n"
    "[/empty]\nnote=x\n";

int main(int argc, char **argv) {
  std::string cwd = "/";
  std::string r;

  // resolve
  CHECK(resolve("/", "", r) && r == "/");
  CHECK(resolve("/a/b", "..", r) && r == "/a");
  CHECK(resolve("/a/b", "../../..", r) && r == "/");
  CHECK(resolve("/a", "./B//c", r) && r == "/a/b/c");
  CHECK(resolve("/a", "~", r) && r == "/");
  CHECK(resolve("/a", "~/x", r) && r == "/x");
  CHECK(resolve("/", "/x/../y", r) && r == "/y");
  CHECK(!resolve("/", "a b", r));            // words are split before; a space is not a name character
  CHECK(!resolve("/", "a;b", r));
  CHECK(!resolve("/", "/1/2/3/4/5/6/7/8/9", r));
  CHECK(!resolve("/", std::string(70, 'a'), r));

  Table t;
  parse(ini(DEMO), t);
  CHECK(t.host == "haven");
  CHECK(t.skipped == 2);   // "/bad path" and "/empty" (no text, no dir)
  CHECK(t.finalCmd == "shut down now");

  // ls
  Result o = exec(t, cwd, "", "ls");
  CHECK(o.out == "etc/\norders/");
  o = exec(t, cwd, "", "ls -a");
  CHECK(o.out == ".hidden\netc/\norders/");
  o = exec(t, cwd, "", "ls orders");
  CHECK(o.out == "001");                        // 002 needs a flag
  o = exec(t, cwd, "KEY", "ls /orders");
  CHECK(o.out == "001\n002");
  o = exec(t, cwd, "", "ls -l etc");
  CHECK(o.out.find("-rw-r--r--") == 0 && o.out.find(" motd") != std::string::npos);
  o = exec(t, cwd, "", "ls -z");
  CHECK(o.out.find("illegal option") != std::string::npos);
  o = exec(t, cwd, "", "ls nope");
  CHECK(o.out == "ls: nope: No such file or directory");

  // cat
  o = exec(t, cwd, "", "cat etc/motd");
  CHECK(o.out == "Your data is NOWHERE.\nSecond line.");
  o = exec(t, cwd, "", "cat orders/002");
  CHECK(o.out == "cat: orders/002: No such file or directory");   // hidden is not found
  o = exec(t, cwd, "KEY", "cat orders/002");
  CHECK(o.out == "order two");
  o = exec(t, cwd, "", "cat orders");
  CHECK(o.out == "cat: orders: Is a directory");
  o = exec(t, cwd, "", "cat");
  CHECK(o.out == "usage: cat file");

  // cd and pwd
  o = exec(t, cwd, "", "cd orders");
  CHECK(cwd == "/orders" && o.out.empty());
  o = exec(t, cwd, "", "cat 001");
  CHECK(o.out == "order one");
  o = exec(t, cwd, "", "pwd");
  CHECK(o.out == "/orders");
  o = exec(t, cwd, "", "cd ..");
  CHECK(cwd == "/");
  o = exec(t, cwd, "", "cd /nowhere");
  CHECK(cwd == "/" && o.out.find("No such file") != std::string::npos);
  o = exec(t, cwd, "", "cd etc/motd");
  CHECK(cwd == "/");                            // a file is not a directory
  exec(t, cwd, "", "cd orders"); exec(t, cwd, "", "cd");
  CHECK(cwd == "/");
  CHECK(promptText(t, "/orders") == "guest@haven:/orders$");

  // a directory that exists only for a hidden file is not there
  Table h;
  parse(ini("[SHELL]\n[/secret/file]\ntext=x\nhas=KEY\n"), h);
  cwd = "/";
  o = exec(h, cwd, "", "cd secret");
  CHECK(cwd == "/");
  o = exec(h, cwd, "", "ls");
  CHECK(o.out.empty());
  o = exec(h, cwd, "KEY", "cd secret");
  CHECK(cwd == "/secret");

  // unknown, exit, the final command
  cwd = "/";
  o = exec(t, cwd, "", "frobnicate now");
  CHECK(o.out == "frobnicate: command not found");
  o = exec(t, cwd, "", "EXIT");
  CHECK(o.exit);
  o = exec(t, cwd, "", "shut down now");
  CHECK(!o.final && o.out == "shut: command not found");      // lacks the flag: it does not exist
  o = exec(t, cwd, "SEEN_ALL", "SHUT   down NOW");
  CHECK(o.final && o.grant == "ENDED" && o.out == "Done.");
  o = exec(t, cwd, "", "");
  CHECK(o.out.empty() && !o.exit);

  // limits
  std::string many = "[SHELL]\n";
  for (int i = 0; i < 130; ++i) many += "[/f" + std::to_string(i) + "]\ntext=x\n";
  Table big;
  parse(ini(many), big);
  CHECK(big.nodes.size() == MAX_NODES && big.skipped == 10);

  // the card's own files
  for (int i = 1; i < argc; ++i) {
    std::ifstream f(argv[i], std::ios::binary);
    if (!f) { printf("FAIL cannot open %s\n", argv[i]); failures++; continue; }
    std::stringstream ss; ss << f.rdbuf();
    auto kv = ini(ss.str());
    std::string name = argv[i];
    if (name.find("SHELL") != std::string::npos) {
      Table c;
      parse(kv, c);
      for (auto &p : c.problems) { printf("FAIL %s: %s\n", argv[i], p.c_str()); failures++; }
      printf("%s: %zu entries\n", argv[i], c.nodes.size());
      CHECK(!c.nodes.empty());
      // with every flag, every file can be read and every listed directory entered
      std::string all;
      for (auto &n : c.nodes) all = haunt::flagsAdd(all, n.has);
      std::string d = "/";
      for (auto &n : c.nodes) {
        std::string cwd2 = "/";
        if (n.dir) { exec(c, cwd2, all, "cd " + n.path); CHECK(cwd2 == n.path); }
        else { Result cr = exec(c, cwd2, all, "cat " + n.path); CHECK(cr.out == expand(n.text)); }
      }
      if (!c.finalCmd.empty()) { std::string cw = "/"; Result fr = exec(c, cw, c.finalHas, c.finalCmd); CHECK(fr.final); }
    } else {
      // DIAG.INI: a section per query code, upper case digits/letters only, each with text=
      std::set<std::string> codes;
      for (auto &e : kv) {
        if (e.section == "DIAG" || e.section.empty() || codes.count(e.section)) continue;
        codes.insert(e.section);
        bool ok = e.section.size() >= 2 && e.section.size() <= 12;
        for (char c : e.section) ok = ok && ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z'));
        if (!ok) { printf("FAIL %s: [%s] is not a query code\n", argv[i], e.section.c_str()); failures++; }
        if (get(kv, e.section, "text").empty()) { printf("FAIL %s: [%s] has no text\n", argv[i], e.section.c_str()); failures++; }
      }
      printf("%s: %zu queries\n", argv[i], codes.size());
      CHECK(!codes.empty());
    }
  }

  if (failures) { printf("%d FAILED\n", failures); return 1; }
  printf("all shell logic tests passed\n");
  return 0;
}
