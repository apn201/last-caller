#pragma once
// The header of a tracker module (ProTracker and its look-alikes), as pure
// logic: title, format tag, channels, order list, the 31 sample names. The
// board shows this as a text view when a tune cannot be played (Telnet, or a
// browser with no player on the card). No String, no SD card, no sessions.
// A plain C++ compiler tests it (tools/test/modinfo_logic_test.cpp).
// Part of the shared core: included from src/bbs_core.h.
#include <stdint.h>
#include <stddef.h>
#include <string>
#include <vector>

namespace modinfo {

static constexpr size_t HEADER_BYTES = 1084;   // title, 31 samples, orders, tag
static constexpr unsigned SAMPLES = 31;

struct Sample {
  std::string name;      // printable text only, trailing spaces trimmed
  uint32_t bytes = 0;    // length of the sample data
  uint8_t volume = 0;
};

struct Info {
  std::string title, tag;
  unsigned channels = 0, orders = 0, patterns = 0;
  std::vector<Sample> samples;   // always 31, in slot order
};

// Text up to the first NUL, at most `max` bytes. Anything that is not a
// printable ASCII character becomes '?': a module's bytes never reach a terminal as they are.
inline std::string printable(const uint8_t *p, size_t max) {
  std::string s;
  for (size_t i = 0; i < max && p[i]; ++i) s += (p[i] >= 32 && p[i] < 127) ? (char)p[i] : '?';
  while (!s.empty() && s.back() == ' ') s.pop_back();
  return s;
}

// The channel count a format tag stands for, 0 if the tag is not one we know.
inline unsigned channelsFor(const uint8_t *tag) {
  auto is = [&](const char *t) { return tag[0] == (uint8_t)t[0] && tag[1] == (uint8_t)t[1] && tag[2] == (uint8_t)t[2] && tag[3] == (uint8_t)t[3]; };
  if (is("M.K.") || is("M!K!") || is("FLT4") || is("4CHN")) return 4;
  if (is("6CHN")) return 6;
  if (is("8CHN") || is("FLT8") || is("OCTA") || is("CD81")) return 8;
  if (tag[0] >= '1' && tag[0] <= '3' && tag[1] >= '0' && tag[1] <= '9' && tag[2] == 'C' && tag[3] == 'H') {
    unsigned n = (tag[0] - '0') * 10 + (tag[1] - '0');   // 10CH to 32CH
    return n >= 10 && n <= 32 ? n : 0;
  }
  if (tag[0] >= '2' && tag[0] <= '9' && tag[1] == 'C' && tag[2] == 'H' && tag[3] == 'N') return tag[0] - '0';
  return 0;
}

// False if the bytes are not a module of a known tag.
inline bool parse(const uint8_t *b, size_t n, Info &out) {
  out = Info();
  if (!b || n < HEADER_BYTES) return false;
  unsigned channels = channelsFor(b + 1080);
  unsigned orders = b[950];
  if (!channels || orders < 1 || orders > 128) return false;
  out.channels = channels;
  out.orders = orders;
  out.tag.assign((const char *)b + 1080, 4);
  out.title = printable(b, 20);
  unsigned highest = 0;
  for (unsigned i = 0; i < 128; ++i) if (b[952 + i] > highest) highest = b[952 + i];
  out.patterns = highest + 1;
  for (unsigned i = 0; i < SAMPLES; ++i) {
    const uint8_t *s = b + 20 + i * 30;
    Sample sm;
    sm.name = printable(s, 22);
    sm.bytes = ((uint32_t)s[22] << 8 | s[23]) * 2;
    sm.volume = s[25] > 64 ? 64 : s[25];
    out.samples.push_back(sm);
  }
  return true;
}

}  // namespace modinfo
