#pragma once
// Ty's snake game, as pure logic: the station, the steps, the food, the score.
// No String, no SD card, no sessions: the board draws it in doors/snake.h. A
// plain C++ compiler tests it (tools/test/snake_logic_test.cpp). The generator
// is its own, so a seed gives the same game on every machine.
// Part of the shared core: included by core/session.h.
#include <stdint.h>
#include <vector>

namespace snake {

static constexpr int W = 30, H = 11;
struct Pt { int x = 0, y = 0; };

// The walls, and the hull blocks that stand in the corridors.
inline bool hull(int x, int y) {
  if (x <= 0 || y <= 0 || x >= W - 1 || y >= H - 1) return true;
  if (y == 3 && x >= 9 && x <= 14) return true;     // a bulkhead
  if (x == 21 && y >= 5 && y <= 8) return true;     // a rack
  return false;
}

struct Game {
  std::vector<Pt> body;    // the head first
  Pt food;
  char dir = 'D';          // W up, A left, S down, D right
  int score = 0;
  bool dead = false;
  bool cleared = false;    // no free cell left for food
  uint32_t rng = 1;
};

enum Step : uint8_t { MOVED, ATE, DIED };

inline uint32_t next(Game &g) {
  uint32_t x = g.rng ? g.rng : 1;
  x ^= x << 13; x ^= x >> 17; x ^= x << 5;
  g.rng = x;
  return x;
}

inline bool onBody(const Game &g, int x, int y) {
  for (const Pt &p : g.body) if (p.x == x && p.y == y) return true;
  return false;
}

// Put the food on a free cell. False if there is none.
inline bool placeFood(Game &g) {
  std::vector<Pt> free;
  for (int y = 1; y < H - 1; ++y)
    for (int x = 1; x < W - 1; ++x)
      if (!hull(x, y) && !onBody(g, x, y)) free.push_back({x, y});
  if (free.empty()) return false;
  g.food = free[next(g) % free.size()];
  return true;
}

inline void start(Game &g, uint32_t seed) {
  g = Game();
  g.rng = seed ? seed : 1;
  g.body = {{4, 5}, {3, 5}, {2, 5}};
  placeFood(g);
}

inline char normal(char c) { return c >= 'a' && c <= 'z' ? char(c - 'a' + 'A') : c; }
inline bool isMove(char c) { c = normal(c); return c == 'W' || c == 'A' || c == 'S' || c == 'D'; }
inline bool opposite(char a, char b) {
  return (a == 'W' && b == 'S') || (a == 'S' && b == 'W') || (a == 'A' && b == 'D') || (a == 'D' && b == 'A');
}

// One step. `key` is W, A, S, D or 0 (go on the way you were going). Turning
// back into the neck is ignored: the snake goes on.
inline Step step(Game &g, char key) {
  if (g.dead) return DIED;
  key = normal(key);
  if (isMove(key) && !opposite(g.dir, key)) g.dir = key;
  Pt h = g.body[0];
  if (g.dir == 'W') h.y--; else if (g.dir == 'S') h.y++; else if (g.dir == 'A') h.x--; else h.x++;
  const bool eats = h.x == g.food.x && h.y == g.food.y;
  // The tail cell is free again unless the snake is growing.
  size_t limit = g.body.size() - (eats ? 0 : 1);
  bool hit = hull(h.x, h.y);
  for (size_t i = 0; i < limit && !hit; ++i) if (g.body[i].x == h.x && g.body[i].y == h.y) hit = true;
  if (hit) { g.dead = true; return DIED; }
  g.body.insert(g.body.begin(), h);
  if (!eats) { g.body.pop_back(); return MOVED; }
  g.score++;
  if (!placeFood(g)) { g.cleared = true; g.dead = true; }
  return ATE;
}

// Where a score falls in a table of scores kept best first: 0 is the top row.
inline size_t place(const std::vector<int> &table, int score) {
  size_t i = 0;
  while (i < table.size() && table[i] >= score) i++;
  return i;
}

}  // namespace snake
