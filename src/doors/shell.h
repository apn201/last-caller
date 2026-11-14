#pragma once
// SHELL: the NOWHERE shell, a fake 1998 Unix-like prompt on the data haven's
// file system. The file tree, the final command and the door's gate are in
// STORY/SHELL.INI (read each time the door opens); the commands are
// core/shell_logic.h, which a host test covers. The door is in the menu only
// for callers who hold the flags SHELL.INI names (needs=), signed in to the account it
// names (account=, if any), and the maintenance
// login never reaches it. Same on Telnet and in the browser; the file tree is
// the card's, no card paths are ever shown.
// Part of the single TinyBBS translation unit: included in order from main.cpp.
#include "../core/shell_logic.h"

static constexpr const char *SHELL_INI = "/BBS/STORY/SHELL.INI";
static nshell::Table shellTable;

static String shellLines(const std::string &text) {
  String out;
  for (char c : text) { if (c == '\n') out += "\r\n"; else out += c; }
  return out;
}
static void shellLoad() {
  Ini ini;
  std::vector<haunt::Kv> kv;
  if (iniLoad(SHELL_INI, ini)) for (const auto &e : ini) kv.push_back({hauntStd(e.section), hauntStd(e.key), hauntStd(e.value)});
  nshell::parse(kv, shellTable);
  for (const auto &p : shellTable.problems) event("SHELL_SKIP", hauntStr(p));
}
static void shellPrompt(Session &s) {
  prompt(s, "\x1b[0;32m" + hauntStr(nshell::promptText(shellTable, hauntStd(s.shellCwd))) + " \x1b[0m");
}

static bool ownsShell(const Session &s) { return s.state == SHELL_PROMPT; }
static bool shellListed(const Session &s) {
  if (isMaintenance(s.user)) return false;
  // account=: the door is only in that account's menu (Q12d: ELIZA's, milestone M8). The flags are the
  // caller's own (a crew login reads the own account behind it, core/haunt.h hauntOwner).
  String only = config(SHELL_INI, "SHELL", "account", "");
  only.trim();
  if (only.length() && up(only) != s.user.normalized) return false;
  return haunt::flagsHasAll(doorFlagsOf(s), haunt::flagsClean(hauntStd(config(SHELL_INI, "SHELL", "needs", ""))));
}

static void enterShell(Session &s) {
  shellLoad();
  s.state = SHELL_PROMPT; s.hideInput = false; s.shellCwd = "/"; activity(s, "NOWHERE");
  event("SHELL", s.user.handle + " node " + s.node + " opened the NOWHERE shell");
  if (s.ansi) artOnce(s, "/BBS/ANSI/DOORS/NOWHERE.ANS", "NOWHERE");
  else plainArt(s, "/BBS/ASCII/DOORS/NOWHERE.ASC", "NOWHERE");
  if (shellTable.motd.length()) { String m = hauntStr(shellTable.motd); m.replace("|", "\r\n"); say(s, "\x1b[0;37m" + m + "\x1b[0m"); }
  shellPrompt(s);
}

static void handleShell(Session &s, String &line, const String &) {
  std::string cwd = hauntStd(s.shellCwd);
  nshell::Result r = nshell::exec(shellTable, cwd, doorFlagsOf(s), hauntStd(line));
  s.shellCwd = hauntStr(cwd);
  if (r.exit) {
    event("SHELL", s.user.handle + " node " + s.node + " left the NOWHERE shell");
    say(s, "\x1b[0;37mlogout\x1b[0m");
    doorMenu(s);
    return;
  }
  if (r.final) {
    // Saved before anything is shown, like every flag grant.
    if (r.grant.length() && !sceneGrant(s, r.grant)) {
      event("SHELL_FAIL", s.user.handle + ": final command: flags not saved");
      say(s, "\x1b[0;37msh: I/O error\x1b[0m");
      shellPrompt(s);
      return;
    }
    event("SHELL", s.user.handle + " node " + s.node + " typed the final command");
  }
  if (r.out.length()) say(s, "\x1b[0;37m" + shellLines(r.out) + "\x1b[0m");
  // The final command may hand the screen to a scene (an ending); it ends at the main menu.
  if (r.final && r.scene.length()) {
    sceneStart(s, hauntStr(r.scene));
    if (s.state == SCENE_PLAY) return;
  }
  shellPrompt(s);
}
