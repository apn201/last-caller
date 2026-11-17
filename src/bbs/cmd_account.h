#pragma once
// G2 (Juha): the caller's own account. E on the main menu. R resets the game (the handle and password stay;
// the story flags, the notebook, the private story mail, ELIZA's memory of the caller and the read pointers
// go), D deletes the account and frees the handle. Both ask for the handle typed in full. Never for a crew
// account, the SysOp or the maintenance login. Same on Telnet and in the browser.
// Part of the shared core: included in order from src/bbs_core.h, after bbs/cmd_main.h.

static bool accountEligible(const Session &s) {
  return s.authenticated && s.user.id && !s.readOnly && !isMaintenance(s.user) && !hasFlag(s.user, "SYSOP") && s.user.level < 255;
}
static void accountMenu(Session &s) {
  s.state = ACCT_MENU; activity(s, "YOUR ACCOUNT");
  std::vector<MenuItem> items;
  items.push_back({"R", "Reset my game"});
  items.push_back({"D", "Delete my account"});
  items.push_back({"Q", "Main Board"});
  showMenu(s, "ACCOUNT", items, "YOUR ACCOUNT: " + s.user.handle);
  say(s, s.ansi ? "\x1b[0;37mR keeps your handle and password and starts the story again.\r\nD removes the account and frees the handle.\x1b[0m" : "R keeps your handle and password and starts the story again.\r\nD removes the account and frees the handle.");
  prompt(s, "Account command? ");
}

// The private mail the story wrote to this caller (a crew or ELIZA sender, or a name that is no account):
// marked killed, as a reader's K would. What the caller wrote, and mail from other callers, stays.
static void accountWipeMail(const User &u) {
  if (!sdReady) return;
  std::vector<Message> mine;
  File f = SD_MMC.open(MSG_FILE, FILE_READ);
  while (f && f.available() && mine.size() < 64) {
    Message m;
    if (!parseMessage(f.readStringUntil('\n'), m) || !m.priv || m.deleted || up(m.to) != u.normalized || up(m.from) == u.normalized) continue;
    mine.push_back(m);
  }
  if (f) f.close();
  for (Message &m : mine) {
    User sender;
    if (findUser(up(m.from), sender) && !hasFlag(sender, "READONLY")) continue;   // another caller's own letter
    m.deleted = true; m.editedAt = nowText(); m.editedBy = "reset";
    appendRevision(m);
  }
}
// Everything the board keeps about this caller's story, apart from the account record itself.
static void accountWipeState(const User &u) {
  putRecord(STORY_FILE, u.id, "", false);
  putRecord(MAN_DAT, u.id, "", false);
  putRecord(DONE_DAT, u.id, "", false);
  putRecord(RIVE_VARS_FILE, u.id, "", false);
  if (sdReady) SD_MMC.remove((String("/BBS/DATA/STATE/PTR_") + u.id + ".DAT").c_str());
  accountWipeMail(u);
}
// The live sessions of the account forget too, so nothing is written back from memory.
static void accountForget(const User &u) {
  for (auto &n : sessions) {
    if (!n.used || n.user.id != u.id) continue;
    n.user.story = ""; n.user.flags = ""; n.carriedFlags = ""; n.manPicks = ""; n.menusSeen = "";
    n.hauntShows.clear(); n.ghostSay.clear(); n.chatOpts.clear(); n.rive = rive::State();
    n.readPtrs.clear(); n.lastRead = 0; n.sceneHold = false; n.typeAhead = "";
    n.gateAt = 0; n.gateOwn = "";
  }
}

static bool accountCommand(Session &s, String &line, const String &cmd) {
  if (s.state == ACCT_MENU) {
    if (!accountEligible(s)) { mainMenu(s); return true; }
    if (cmd == "Q" || cmd == "X") { mainMenu(s); return true; }
    if (cmd == "?") { keysHelp(s); return true; }
    if (!cmd.length()) { accountMenu(s); return true; }
    if (cmd == "R" || cmd == "D") {
      s.state = cmd == "R" ? ACCT_RESET : ACCT_DELETE;
      say(s, cmd == "R" ? "\x1b[1;33mThis wipes your story: clues, notebook, mail from the ship and ELIZA's memory of you.\x1b[0m"
                        : "\x1b[1;31mThis deletes the account " + s.user.handle + ". The handle is free for anyone afterwards.\x1b[0m");
      prompt(s, "Type your handle to confirm, ENTER cancels: ");
      return true;
    }
    say(s, "R, D or Q."); prompt(s, "Account command? ");
    return true;
  }
  if (s.state == ACCT_RESET || s.state == ACCT_DELETE) {
    const bool reset = s.state == ACCT_RESET;
    if (!accountEligible(s) || !line.length() || up(line) != s.user.normalized) {
      say(s, line.length() ? "That is not your handle. Nothing was changed." : "Nothing was changed.");
      if (accountEligible(s)) accountMenu(s); else mainMenu(s);
      return true;
    }
    User u = s.user;
    accountWipeState(u);
    accountForget(u);
    if (reset) {
      u.flags = ""; u.story = "";
      if (!touchUser(u)) { say(s, "The account record was not saved. Try again."); accountMenu(s); return true; }
      event("RESET", u.handle + " node " + s.node + " started the game again");
      s.user = u; s.ownUserId = 0; s.ownHandle = ""; s.carriedFlags = "";
      say(s, "\r\n\x1b[1;32mYour game is reset.\x1b[0m Handle and password are as they were. Sign in to begin again.");
      logOffKeepLine(s);
    } else {
      if (!removeUser(u)) { say(s, "The account was not removed. Try again."); accountMenu(s); return true; }
      event("DELETE_SELF", u.handle + " node " + s.node + " deleted the own account");
      s.user = User(); s.ownUserId = 0; s.ownHandle = ""; s.carriedFlags = "";
      say(s, "\r\n\x1b[1;32mThe account is deleted.\x1b[0m The handle is free.");
      logoutToLogin(s);
    }
    return true;
  }
  return false;
}