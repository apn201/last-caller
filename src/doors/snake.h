#pragma once
// SNAKE: the little game Ty wrote for the crew. Turn based, so it works over a
// line terminal: the caller types W, A, S, D (several at once play in order),
// the board draws the station after each line. It saves nothing, so it works for
// read-only accounts too. The rules are core/snake_logic.h (tested on the PC);
// the crew's high scores and the door's lines are in STORY/EGGS.INI [SNAKE]; the
// title is ANSI/DOORS/SNAKE.ANS (ASCII/DOORS/SNAKE.ASC).
// Part of the single TinyBBS translation unit: included in order from main.cpp.

static constexpr uint8_t SNAKE_MAX_STEPS = 60;   // moves played from one typed line

static bool snakeArtExists(const Session &s) {
  return sdReady && SD_MMC.exists(s.ansi ? "/BBS/ANSI/DOORS/SNAKE.ANS" : "/BBS/ASCII/DOORS/SNAKE.ASC");
}
// In the door menu for everyone who is not the maintenance login, if the card has the title.
static bool snakeListed(const Session &s) { return !isMaintenance(s.user) && snakeArtExists(s); }
static bool ownsSnake(const Session &s) { return s.state == SNAKE_PLAY; }

// The station. Colour is for ANSI callers; the characters are the same for all.
static void snakeDraw(Session &s) {
  const snake::Game &g = s.snake;
  const bool c = s.ansi;
  String out = "\r\n";
  for (int y = 0; y < snake::H; ++y) {
    String row = "  ";
    for (int x = 0; x < snake::W; ++x) {
      if (g.body[0].x == x && g.body[0].y == y) row += c ? "\x1b[1;36m@\x1b[0m" : "@";
      else if (snake::onBody(g, x, y)) row += c ? "\x1b[0;36mo\x1b[0m" : "o";
      else if (!g.cleared && g.food.x == x && g.food.y == y) row += c ? "\x1b[1;33m*\x1b[0m" : "*";
      else if (snake::hull(x, y)) row += c ? "\x1b[0;37m#\x1b[0m" : "#";
      else row += ' ';
    }
    out += row + "\r\n";
  }
  write(s, out);
  say(s, "  SCORE " + String(g.score) + "   LENGTH " + String((int)g.body.size()));
}

static void snakeNewGame(Session &s) {
  snake::start(s.snake, bbsRandom());
  snakeDraw(s);
  prompt(s, "Moves (W A S D, Q quits)> ");
}

static void enterSnake(Session &s) {
  s.state = SNAKE_PLAY; s.hideInput = false; activity(s, "SNAKE");
  event("SNAKE", s.user.handle + " node " + s.node + " started snake");
  if (s.ansi) art(s, "/BBS/ANSI/DOORS/SNAKE.ANS"); else plainArt(s, "/BBS/ASCII/DOORS/SNAKE.ASC");
  String intro = config(EGGS_INI, "SNAKE", "intro", "");
  if (intro.length()) say(s, "\x1b[0;37m" + eggLines(intro) + "\x1b[0m");
  snakeNewGame(s);
}

// The crew's table with the caller's score placed in it. Nothing is saved.
static void snakeTable(Session &s) {
  Ini ini;
  iniLoad(EGGS_INI, ini);
  struct Row { String name, date; int score; bool you; };
  std::vector<Row> rows;
  for (int i = 1; i <= 9; ++i) {
    String v = iniGet(ini, "SNAKE", (String("hi") + String(i)).c_str());
    if (!v.length()) continue;
    int a = v.indexOf(';'), b = a >= 0 ? v.indexOf(';', a + 1) : -1;
    if (a < 0) continue;
    Row r;
    r.name = v.substring(0, a);
    r.score = (b >= 0 ? v.substring(a + 1, b) : v.substring(a + 1)).toInt();
    r.date = b >= 0 ? v.substring(b + 1) : String();
    r.you = false;
    rows.push_back(r);
  }
  std::vector<int> scores;
  for (const Row &r : rows) scores.push_back(r.score);
  size_t at = snake::place(scores, s.snake.score);
  if (s.snake.score > 0 && at < 9) rows.insert(rows.begin() + at, Row{"YOU", "", s.snake.score, true});
  if (!rows.size()) return;
  say(s, String(s.ansi ? "\x1b[1;36m" : "") + iniGet(ini, "SNAKE", "table", "HIGH SCORES") + (s.ansi ? "\x1b[0m" : ""));
  size_t shown = 0;
  for (const Row &r : rows) {
    if (++shown > 6) break;
    String name = r.name; while (name.length() < 10) name += ' ';
    String score = String(r.score); while (score.length() < 4) score = " " + score;
    String line = String((int)shown) + ". " + name + score + (r.date.length() ? "  " + r.date : String());
    say(s, "  " + (r.you && s.ansi ? "\x1b[1;33m" + line + "\x1b[0m" : line));
  }
}

static void snakeOver(Session &s) {
  say(s);
  say(s, String(s.ansi ? "\x1b[1;31m" : "") + (s.snake.cleared ? "STATION CLEARED" : "GAME OVER") + (s.ansi ? "\x1b[0m" : ""));
  snakeTable(s);
  prompt(s, "ENTER plays again, Q quits> ");
}

static void handleSnake(Session &s, String &, const String &cmd) {
  if (cmd == "Q" || cmd == "QUIT" || cmd == "X") {   // Q goes up to the door menu, X to the main menu (contract C2)
    event("SNAKE", s.user.handle + " node " + s.node + " left snake, score " + String(s.snake.score));
    if (cmd == "X") mainMenu(s); else doorMenu(s);
    return;
  }
  if (s.snake.dead) { snakeNewGame(s); return; }
  uint8_t steps = 0;
  bool moved = false;
  for (size_t i = 0; i < cmd.length() && steps < SNAKE_MAX_STEPS && !s.snake.dead; ++i) {
    if (!snake::isMove(cmd[i])) continue;
    snake::step(s.snake, cmd[i]);
    steps++; moved = true;
  }
  if (!moved) snake::step(s.snake, 0);   // an empty line goes on the way it was going
  snakeDraw(s);
  if (s.snake.dead) snakeOver(s);
  else prompt(s, "Moves (W A S D, Q quits)> ");
}
