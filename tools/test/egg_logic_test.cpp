// Host-side test of src/core/egg_logic.h and src/core/snake_logic.h. No board, no Arduino types.
//   zig c++ -std=c++17 -Wall -Wextra -I src/core tools/test/egg_logic_test.cpp -o egg_logic_test
//   (or g++/clang++ with the same flags), then run ./egg_logic_test
#include "egg_logic.h"
#include "snake_logic.h"
#include <stdio.h>

static int failures = 0;
#define CHECK(cond) do { if (!(cond)) { printf("FAIL line %d: %s\n", __LINE__, #cond); failures++; } } while (0)

int main() {
  using namespace egg;
  // Plain words and ordinary commands are not modem commands.
  CHECK(parse("").kind == NONE);
  CHECK(parse("M").kind == NONE);
  CHECK(parse("a").kind == NONE);
  CHECK(parse("hello").kind == NONE);
  CHECK(parse("+").kind == NONE);
  CHECK(parse("++++").kind == NONE);
  // Setup and escape.
  CHECK(parse("AT").kind == OK);
  CHECK(parse("  at  ").kind == OK);
  CHECK(parse("+++").kind == OK);
  CHECK(parse("ATZ").kind == OK);
  CHECK(parse("atz0").kind == OK);
  CHECK(parse("ATE0").kind == OK);
  CHECK(parse("ATM0").kind == OK);
  CHECK(parse("AT&F").kind == OK);
  CHECK(parse("ATS0=1").kind == OK);
  CHECK(parse("ATX3").kind == OK);
  CHECK(parse("ATE0 M0").kind == ERR);        // one command at a time
  // Identity.
  CHECK(parse("ATI").kind == INFO && parse("ATI").info == 0);
  CHECK(parse("ati3").kind == INFO && parse("ati3").info == 3);
  CHECK(parse("ATI9").info == 9);
  CHECK(parse("ATI10").kind == ERR);
  // Hang up.
  CHECK(parse("ATH").kind == HANGUP);
  CHECK(parse("ath0").kind == HANGUP);
  CHECK(parse("ATH1").kind == HANGUP);
  CHECK(parse("ATHENA").kind == ERR);         // a word that starts with AT
  CHECK(parse("ATLAS").kind == ERR);
  // Dial.
  CHECK(parse("ATDT 555-0172").kind == DIAL);
  CHECK(parse("ATDT 555-0172").digits == "5550172");
  CHECK(parse("atdp5550172").digits == "5550172");
  CHECK(parse("ATD(555) 0172").digits == "5550172");
  CHECK(parse("ATDT").kind == DIAL && parse("ATDT").digits.empty());
  CHECK(parse("ATD").digits.empty());
  CHECK(parse("ATDT 55X").kind == ERR);
  CHECK(parse("ATDTT5").kind == ERR);
  CHECK(parse("ATDT 123456789012345678901234").digits.size() == 20);
  CHECK(parse("ATDELTA").kind == ERR);

  // Snake: the start.
  using namespace snake;
  Game g;
  start(g, 12345);
  CHECK(g.body.size() == 3 && g.score == 0 && !g.dead);
  CHECK(!hull(g.food.x, g.food.y) && !onBody(g, g.food.x, g.food.y));
  CHECK(g.food.x > 0 && g.food.x < W - 1 && g.food.y > 0 && g.food.y < H - 1);
  // The same seed gives the same game.
  Game h;
  start(h, 12345);
  CHECK(h.food.x == g.food.x && h.food.y == g.food.y);
  // A step moves the head and keeps the length.
  CHECK(step(g, 'D') == MOVED || step(g, 0) == ATE);
  // A turn back into the neck is ignored.
  start(g, 7);
  g.food = {1, 1};
  step(g, 'A');
  CHECK(g.dir == 'D' && g.body[0].x == 5 && !g.dead);
  step(g, 'w');
  CHECK(g.dir == 'W' && g.body[0].y == 4);
  // A wall ends it, and a dead game stays dead.
  start(g, 7);
  g.food = {1, 1};
  int guard = 0;
  while (!g.dead && guard++ < 100) step(g, 'D');
  CHECK(g.dead && g.body[0].x < W);
  CHECK(step(g, 'S') == DIED);
  // Hull blocks kill: the bulkhead is at y = 3, x 9 to 14.
  start(g, 7);
  g.food = {1, 1};
  step(g, 'W'); step(g, 'W');                  // (4,3)
  CHECK(!g.dead);
  guard = 0;
  while (!g.dead && guard++ < 20) step(g, 'D');
  CHECK(g.dead && g.body[0].x == 8 && g.body[0].y == 3);
  // Eating grows the snake, scores, and puts the food somewhere free.
  start(g, 99);
  g.food = {5, 5};
  CHECK(step(g, 'D') == ATE);
  CHECK(g.score == 1 && g.body.size() == 4);
  CHECK(!hull(g.food.x, g.food.y) && !onBody(g, g.food.x, g.food.y));
  // Food is never on the hull or the body over many meals.
  start(g, 31337);
  for (int i = 0; i < 200 && !g.dead; ++i) {
    CHECK(!hull(g.food.x, g.food.y) && !onBody(g, g.food.x, g.food.y));
    g.food = g.body[0];
    g.food.x += 1;                              // put it in front, whatever the direction, then eat toward it
    if (hull(g.food.x, g.food.y) || onBody(g, g.food.x, g.food.y)) break;
    g.dir = 'D';
    step(g, 'D');
  }
  // Chasing the tail is allowed: the tail cell is free as the head arrives.
  start(g, 5);
  g.food = {1, 1};
  g.body = {{10, 6}, {11, 6}, {11, 7}, {10, 7}};   // head (10,6) heading up; tail at (10,7)
  g.dir = 'A';
  Step st = step(g, 'S');                       // head to (10,7): the tail cell, which moves away
  CHECK(st == MOVED && !g.dead);
  // A real collision.
   start(g, 5);
  g.food = {1, 1};
  g.body = {{10, 6}, {10, 5}, {11, 5}, {11, 6}, {11, 7}, {10, 7}, {9, 7}};
  g.dir = 'A';
  CHECK(step(g, 'S') == DIED);
  // The table: best first, ties go below the older score.
  std::vector<int> t = {41, 29, 17, 3};
  CHECK(place(t, 50) == 0);
  CHECK(place(t, 29) == 2);
  CHECK(place(t, 10) == 3);
  CHECK(place(t, 0) == 4);

  if (failures) { printf("%d failure(s)\n", failures); return 1; }
  printf("egg_logic_test: all passed\n");
  return 0;
}
