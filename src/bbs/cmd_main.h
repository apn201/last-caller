#pragma once
// Main board, bulletins, user list, conferences and node chat.
// Part of the single TinyBBS translation unit: included in order from main.cpp.

// SysOp page: the SysOp answers with /A from any prompt, which pulls both
// callers into node chat, or refuses with /R.
static void enterChat(Session &s, const String &banner) {
  if (playingMud(s)) mudBroadcastRoom(s, s.user.handle + " is called away by the SysOp.");
  startChat(s, banner);
}
static void answerPage(Session &op, bool accept) {
  uint8_t node = op.pageNode; uint32_t uid = op.pageUser;
  for (auto &n : sessions) if (n.pageNode == node) { n.pageNode = 0; n.pageUser = 0; n.paged = false; }
  Session *caller = sessionByNode(node);
  if (!caller || !caller->used || caller->user.id != uid) { say(op, "\r\nThe caller already hung up."); return; }
  if (!accept) {
    say(*caller, "\r\n\x1b[1;31mThe SysOp is busy right now. Try again later.\x1b[0m");
    say(op, "\r\nPage refused.");
    return;
  }
  event("SYSOP_CHAT", op.user.handle + " answered " + caller->user.handle);
  enterChat(*caller, "\r\n\x1b[1;33m*** The SysOp breaks in: you are in chat with " + op.user.handle + ". /Q returns to the board. ***\x1b[0m");
  enterChat(op, "\r\n\x1b[1;33m*** Chat with " + caller->user.handle + " (node " + caller->node + "). /Q returns to the board. ***\x1b[0m");
}

// Bulletins from BULLETINS.INI that this caller may read, numbered from 1.
static const Bulletin *visibleBulletin(const Session &s, uint32_t wanted) {
  uint32_t n = 0;
  for (const auto &b : board.bulletins) if (canSee(s.user, b.level, b.flag) && ++n == wanted) return &b;
  return nullptr;
}
static uint32_t bulletinCount(const Session &s) {
  uint32_t n = 0;
  for (const auto &b : board.bulletins) if (canSee(s.user, b.level, b.flag)) n++;
  return n;
}
static void bulletinMenu(Session &s);
static void bulletinBack(Session &s) { if (bulletinCount(s) > 1) bulletinMenu(s); else mainMenu(s); }
// Contract C4: a bulletin is read in the pager, like every text. An art file is drawn into the pager's
// screen; a card that has no file for it (or a caller without ANSI and no .ASC twin) gets the text.
static void openBulletin(Session &s, const Bulletin &b) {
  Reader &r = s.reader;
  r = Reader();
  r.title = clean(b.title, 24); r.onQuit = bulletinBack; r.utf8 = true;
  String path = b.path;
  bool ansiFile = path.length() && s.ansi;
  if (path.length() && !s.ansi) {
    path.replace("/BBS/", "/BBS/ASCII/");
    if (up(path).endsWith(".ANS")) path = path.substring(0, path.length() - 4) + ".ASC";
  }
  File f;
  if (path.length() && sdReady) f = SD_MMC.open(path, FILE_READ);
  if (f && !f.isDirectory() && f.size()) {
    r.path = path; r.start = 0; r.end = f.size();
    f.close();
    if (ansiFile) { r.kind = READER_ANSI; renderAnsi(r); }
    else { r.kind = READER_TEXT; r.crop = true; indexText(r); }
  } else {
    if (f) f.close();
    r.kind = READER_ROWS;
    r.rows.push_back("");
    r.rows.push_back("\x1b[1;36m" + b.title + "\x1b[0m");
    String text = b.text; text.replace("|", "\n");
    bodyRows(r.rows, text);
  }
  activity(s, "BULLETINS");
  openReader(s, false);
  hauntScreen(s, "BULLETIN " + up(b.title));
}
static void bulletinMenu(Session &s) {
  activity(s, "BULLETINS");
  uint32_t count = bulletinCount(s);
  if (!count) { say(s, "No bulletins tonight."); s.state = BULLETIN; prompt(s, "Press any key: "); return; }
  if (count == 1) { openBulletin(s, *visibleBulletin(s, 1)); return; }
  say(s, "\r\n\x1b[1;36mBULLETINS\x1b[0m");
  for (uint32_t i = 1; i <= count; ++i) say(s, "\x1b[1;33m[" + String(i) + "]\x1b[0m " + visibleBulletin(s, i)->title);
  s.state = BULLETIN_LIST;
  prompt(s, "Bulletin #, then ENTER (Q = back): ");
}
// C: node chat. A caller first meets ELIZA in the scene ELZ_TALK (numbered replies, STORY/SCENES);
// its reply "Say something else" opens the typed chat (then=CHAT). A card without the scene, a
// SysOp, or a scene that will not start (no power for this caller) opens the chat directly.
static void chatEntry(Session &s) {
  const char *banner = "\x1b[1;33mMulti-Node Chat. Type and press ENTER to talk. /Q leaves.\x1b[0m";
  if (!isSysop(s) && SD_MMC.exists("/BBS/STORY/SCENES/ELZ_TALK.INI")) {
    sceneStart(s, "ELZ_TALK");
    if (s.state == SCENE_PLAY) return;
  }
  startChat(s, banner);
}

static bool mainCommand(Session &s, String &line, const String &cmd) {
  if (s.state == BULLETIN) { mainMenu(s); return true; }
  if (s.state == MAIN) {
    if (cmd == "B") { bulletinMenu(s); }
    else if (cmd == "W" && isSysop(s)) { nodeMonitor(s, MAIN); }
    else if (cmd == "W") { std::vector<String> rows; rows.push_back(""); whoRows(s, rows); rows.push_back(""); openRows(s, rows, "Who is online", "", mainMenu); }
    else if (cmd == "U") { s.userPage = 0; userList(s); }
    else if (cmd == "M") { messageArea(s); }
    else if (cmd == "N") { readNew(s); }
    else if (cmd == "C") { chatEntry(s); }
    else if (cmd == "J") { areaList(s); }
    else if (cmd == "O") {
      bool sent = false;
      for (auto &n : sessions) {
        if (!n.used || &n == &s || !isSysop(n)) continue;
        say(n, "\r\n\x1b[5;31m*** SYSOP PAGE from " + s.user.handle + " (node " + s.node + ") ***\x1b[0m");
        say(n, "\x1b[1;33mType /A to answer and chat, or /R to refuse.\x1b[0m");
        n.paged = true; n.pageNode = s.node; n.pageUser = s.user.id; sent = true;
      }
      say(s, sent ? "Paging the SysOp... stay on the line." : "The SysOp is not online.");
      s.state = BULLETIN; prompt(s, "Press any key: ");
    }
    else if (cmd == "S") {
      std::vector<String> rows;
      rows.push_back("");
      rows.push_back("System: " + board.name + (board.location.length() ? " // " + board.location : String()) + " // " + clockText(boardNow(), true));
      rows.push_back("SysOp: " + board.sysop + "  Nodes: " + String(board.maxNodes) + "  Messages: " + String((unsigned long)historyCount + nextMessageId - 1));
      rows.push_back("");
      rows.push_back("\x1b[1;36mYour stats\x1b[0m  Calls " + String(s.user.calls) + "  Messages " + s.user.messages +
             "  Uploaded " + sizeText(s.user.uploadBytes) + "  Downloaded " + sizeText(s.user.downloadBytes) + " (" + s.user.downloads + " files)");
      rows.push_back("Download credit: " + creditText(s.user) + (board.creditsPerMessage ? "  Each message posted earns " + sizeText(board.creditsPerMessage) + "." : String()));
      rows.push_back("");
      openRows(s, rows, "System info", "", mainMenu);
    }
    else if (cmd == "A") { ansiGallery(s); }
    else if (cmd == "F") { s.galleryCabinet = false; fileMenu(s); }
    else if (cmd == "D" && anyDoorOpen(s)) { doorMenu(s); }
    else if (cmd == "R" && manifestListed(s)) { enterManifest(s); }
    else if (cmd == "?") { keysHelp(s); }
    else if (cmd == "X") { mainMenu(s); }
    else if (cmd == "Q") { say(s, "This is the main menu. G logs off."); prompt(s, "Main Board Command? "); }
    else if (cmd == "Z" && isSysop(s)) { sysopMenu(s); }
    else if (cmd == "RELOAD" && isSysop(s)) { say(s, "Settings reloaded: " + loadSettings() + "."); say(s, "Network settings take effect after a restart."); prompt(s, "Main Board Command? "); }
    else if (cmd == "USER" && isSysop(s)) { sysopUsers(s); }
    else if (cmd == "Z" || cmd == "USER") { say(s, "SysOp access requires level 255 and the SYSOP flag."); prompt(s, "Main Board Command? "); }
    else if (cmd == "Y" && s.readOnly && s.ownUserId) {
      // Q18: back to the own account signed in earlier on this line (Session.ownUserId), no password again.
      User own;
      if (!findUserById(s.ownUserId, own) || own.locked || hasFlag(own, "READONLY")) { say(s, "That account is not available."); prompt(s, "Main Board Command? "); return true; }
      event("LOGOFF", s.user.handle + " node " + s.node + ", back to " + own.handle);
      s.authenticated = false; s.readOnly = false; s.input = ""; s.hideInput = false; s.conference = 0; s.reader = Reader();
      s.user = own; s.pendingHandle = own.handle;
      claimAccount(s); finishLogin(s);
    }
    else if (cmd == "L" && s.user.id) {
      // Sign off to Login: on the same line. The caller stays the caller (Session.ownUserId), so a
      // crew account signed in next carries its finds here and reads this account's flags.
      event("LOGOFF", s.user.handle + " node " + s.node + ", to the login prompt");
      say(s, "\r\n\x1b[0;37mSigned off. The line stays open.\x1b[0m");
      logoutToLogin(s);
    }
    else if (cmd == "G") { s.state = GOODBYE; activity(s, "LOGOFF"); uint32_t mins = (millis() - s.connectedAt) / MS_PER_MINUTE; art(s, board.goodbye.c_str()); say(s, "Time online: " + String(mins) + " minutes");
      manifestPartial(s);   // an unfinished manifest: ELIZA thanks the caller for their interest
      // A crew (read-only) login logs off to the login prompt, line kept: the
      // caller can sign in as themselves and keep what the visit found (core/haunt.h).
      if (s.readOnly) logoutToLogin(s); }
    else if (!cmd.length()) mainMenu(s);
    else { say(s, "Unknown command. Type ? for help."); prompt(s, "Main Board Command? "); }
    return true;
  }
  if (s.state == USER_LIST) {
    if (cmd == "Q") { mainMenu(s); return true; }
    if (cmd == "X") { mainMenu(s); return true; }
    if (cmd == "?") { keysHelp(s); return true; }
    if (cmd == "Z" && isSysop(s)) { sysopUsers(s); return true; }
    uint16_t pages = userListPages();
    if (cmd == "P" && s.userPage) s.userPage--;
    else if ((cmd == "N" || !cmd.length()) && s.userPage + 1 < pages) s.userPage++;
    userList(s); return true;
  }
  if (s.state == BULLETIN_LIST) {
    uint32_t n;
    if (cmd == "Q") { mainMenu(s); return true; }
    if (cmd == "X") { mainMenu(s); return true; }
    if (cmd == "?") { keysHelp(s); return true; }
    if (!line.length()) { bulletinMenu(s); return true; }
    const Bulletin *b = readNumber(line, n) ? visibleBulletin(s, n) : nullptr;
    if (!b) { say(s, "No such bulletin."); prompt(s, "Bulletin #, then ENTER (Q = back): "); }
    else openBulletin(s, *b);
    return true;
  }
  if (s.state == CHAT) { chatCommand(s, line, cmd); return true; }
  return false;
}
