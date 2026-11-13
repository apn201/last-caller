// Host-side test of src/core/modinfo_logic.h and the scene sound= key. No board, no Arduino types.
//   zig c++ -std=c++17 -Wall -Wextra -I src/core tools/test/modinfo_logic_test.cpp -o modinfo_logic_test
//   (or g++/clang++ with the same flags), then run ./modinfo_logic_test
//   ./modinfo_logic_test file.mod ...  also parses those files and lists their sample names.
#include "modinfo_logic.h"
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <iterator>

static int failures = 0;
#define CHECK(cond) do { if (!(cond)) { printf("FAIL line %d: %s\n", __LINE__, #cond); failures++; } } while (0)

// A module header with `tag`, `orders` orders and the given sample names.
static std::vector<uint8_t> make(const char *tag, unsigned orders, std::vector<std::string> names) {
  std::vector<uint8_t> b(1084, 0);
  memcpy(&b[0], "test tune", 9);
  for (size_t i = 0; i < names.size(); ++i) {
    uint8_t *s = &b[20 + i * 30];
    memcpy(s, names[i].data(), names[i].size() > 22 ? 22 : names[i].size());
    s[22] = 0; s[23] = 4;       // 4 words = 8 bytes
    s[25] = 99;                 // volume over 64 is clamped
  }
  b[950] = (uint8_t)orders;
  b[952 + 1] = 6;               // highest pattern number is 6
  memcpy(&b[1080], tag, 4);
  return b;
}

static void tests() {
  modinfo::Info in;
  auto good = make("M.K.", 11, {"first", "  padded name   ", "", "tail"});
  CHECK(modinfo::parse(good.data(), good.size(), in));
  CHECK(in.channels == 4 && in.orders == 11 && in.patterns == 7 && in.title == "test tune");
  CHECK(in.samples.size() == 31);
  CHECK(in.samples[0].name == "first" && in.samples[0].bytes == 8 && in.samples[0].volume == 64);
  CHECK(in.samples[1].name == "  padded name");   // only trailing spaces go
  CHECK(in.samples[2].name.empty() && in.samples[3].name == "tail");

  auto six = make("6CHN", 1, {});
  CHECK(modinfo::parse(six.data(), six.size(), in) && in.channels == 6);
  auto sixteen = make("16CH", 1, {});
  CHECK(modinfo::parse(sixteen.data(), sixteen.size(), in) && in.channels == 16);
  auto eight = make("8CHN", 128, {});
  CHECK(modinfo::parse(eight.data(), eight.size(), in) && in.channels == 8 && in.orders == 128);
  auto fl = make("FLT4", 3, {});
  CHECK(modinfo::parse(fl.data(), fl.size(), in) && in.channels == 4);

  // Not modules, or broken ones.
  auto bad = make("XXXX", 3, {});
  CHECK(!modinfo::parse(bad.data(), bad.size(), in));
  auto zero = make("M.K.", 0, {});
  CHECK(!modinfo::parse(zero.data(), zero.size(), in));
  auto many = make("M.K.", 129, {});
  CHECK(!modinfo::parse(many.data(), many.size(), in));
  auto low = make("1CHN", 3, {});
  CHECK(!modinfo::parse(low.data(), low.size(), in));
  auto thirtythree = make("33CH", 3, {});
  CHECK(!modinfo::parse(thirtythree.data(), thirtythree.size(), in));
  CHECK(!modinfo::parse(good.data(), 1083, in));       // one byte short
  CHECK(!modinfo::parse(nullptr, 5000, in));
  CHECK(!modinfo::parse(good.data(), 0, in));

  // Odd bytes in a name never reach the terminal.
  auto odd = make("M.K.", 2, {"a\x1b[31mred\x07\xff"});
  CHECK(modinfo::parse(odd.data(), odd.size(), in));
  for (char c : in.samples[0].name) CHECK(c >= 32 && c < 127);
  CHECK(in.samples[0].name == "a?[31mred??");
  // A name that fills all 22 bytes has no NUL.
  auto full = make("M.K.", 2, {"0123456789012345678901"});
  CHECK(modinfo::parse(full.data(), full.size(), in) && in.samples[0].name == "0123456789012345678901");
  // A name stops at the first NUL even when text follows it.
  auto gap = make("M.K.", 2, {std::string("ab\0cd", 5)});
  CHECK(modinfo::parse(gap.data(), gap.size(), in) && in.samples[0].name == "ab");
}

int main(int argc, char **argv) {
  tests();
  for (int i = 1; i < argc; ++i) {
    std::ifstream f(argv[i], std::ios::binary);
    std::vector<uint8_t> b((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    modinfo::Info in;
    if (!modinfo::parse(b.data(), b.size(), in)) { printf("FAIL %s: not a module\n", argv[i]); failures++; continue; }
    printf("%s: \"%s\" %s %u channels %u orders %u patterns\n", argv[i], in.title.c_str(), in.tag.c_str(), in.channels, in.orders, in.patterns);
    unsigned named = 0;
    for (size_t k = 0; k < in.samples.size(); ++k)
      if (!in.samples[k].name.empty()) { named++; printf("  %02zu %s\n", k + 1, in.samples[k].name.c_str()); }
    if (!named) { printf("FAIL %s: no sample names\n", argv[i]); failures++; }
  }
  printf(failures ? "%d failed\n" : "all passed\n", failures);
  return failures ? 1 : 0;
}
