#pragma once
// Message reading, menus, login and node allocation.
// Part of the single TinyBBS translation unit: included in order from main.cpp.

static void keysHelp(Session &s);  // bbs/help.h: ? on any key screen
static bool sysopOnline(const Session &s) {
  for (auto &n : sessions) if (n.used && &n != &s && isSysop(n)) return true;
  return false;
}
// Only what works for this caller: no file cabinets without an open area, no
// bulletins without one to read, no page when no SysOp is on.
static void mainMenu(Session &s) {
  s.state = MAIN; activity(s, "MAIN MENU");
  bool files = false, bulletins = false;
  for (size_t i = 0; i < fileAreas.size(); ++i) files |= listedArea(s, i);
  for (const auto &b : board.bulletins) bulletins |= bulletinOpen(s, b);
  std::vector<MenuItem> items;
  items.push_back({"M", "Message bases"});
  items.push_back({"N", "New scan"});
  if (files) items.push_back({"F", "File cabinets"});
  items.push_back({"A", "ANSI gallery"});
  if (anyDoorOpen(s)) items.push_back({"D", "Doors & games"});
  if (manifestListed(s)) items.push_back({"R", "Crew manifest"});
  items.push_back({"C", isSysop(s) ? "Node chat" : "Chat with the SysOp"});   // a caller meets ELIZA first (cmd_main.h)
  if (bulletins) items.push_back({"B", "Bulletins"});
  if (severalAreas(s.user)) items.push_back({"J", "Conferences"});
  items.push_back({"W", "Who is online"});
  if (sysopOnline(s)) items.push_back({"O", "Page SysOp"});
  items.push_back({"U", "User list"});
  items.push_back({"S", "System info"});
  items.push_back({"?", "Help"});
  // L (Q12d): an own account signs off to the login prompt and keeps the line, so a crew login
  // after it reads this caller's own flags (the shell in ELIZA's menu needs that, core/haunt.h).
  // Q20 (Addition 6): L lists the crew accounts already opened. A crew account has no L: it is
  // shared by every caller, so its way back is G (to Login:) or Y (to the own account on this line).
  if (s.user.id && !isMaintenance(s.user) && !s.readOnly) items.push_back({"L", "Log in as another"});
  // Q18: a crew account goes back to the caller's own account on this line without the login again.
  if (s.readOnly && s.ownUserId && s.ownHandle.length()) items.push_back({"Y", "Back to " + s.ownHandle});
  items.push_back({"G", s.readOnly ? "Goodbye (to Login:)" : "Goodbye"});
  if (isSysop(s)) items.push_back({"Z", "SysOp console", "\x1b[1;35m"});
  showMenu(s, "MAIN", items, board.name + " // MAIN BOARD");
  // SYSOP view (milestone M6) opens reading and nothing else: one line tells the caller what changed.
  if (hasFlag(s.user, "SYSVW")) say(s, s.ansi ? "\x1b[0;33mSYSOP view is on: every area is open to read.\x1b[0m" : "SYSOP view is on: every area is open to read.");
  // Q20 UX: the one rule a new caller needs, once per call, under the first main menu.
  if (s.menusSeen.indexOf("|KEYHINT|") < 0) {
    s.menusSeen += "|KEYHINT|";
    say(s, s.ansi ? "\x1b[0;36mPress a letter. No ENTER needed. ? shows the keys, X comes back here.\x1b[0m" : "Press a letter. No ENTER needed. ? shows the keys, X comes back here.");
  }
  prompt(s, "\x1b[1;33mMain Board Command? \x1b[0m");
}
// The gallery's G browses the first file area whose folder name has ANSI in it.
static int galleryArea(const Session &s) {
  for (size_t i = 0; i < fileAreas.size(); ++i) if (listedArea(s, i) && up(fileAreas[i].folder).indexOf("ANSI") >= 0) return i;
  return -1;
}
static void ansiGallery(Session &s) {
  s.state = ANSI_GALLERY;
  activity(s, "ANSI GALLERY");
  std::vector<MenuItem> items;
  if (s.ansi) {  // pictures: not for an ASCII caller
    items.push_back({"A", "Login splash"});
    items.push_back({"F", "File cabinet screen"});
  }
  if (galleryArea(s) >= 0) items.push_back({"G", "Browse ANSI / ASCII files"});
  items.push_back({"Q", "Main Board"});
  showMenu(s, "GALLERY", items, "ANSI GALLERY");
  prompt(s, "Gallery command? ");
}
// FILES.ANS is the header art; the areas below it come from the card.
static void fileMenu(Session &s) {
  s.state = FILE_MENU; activity(s, "FILE AREAS");
  artOnce(s, "/BBS/ANSI/MENUS/FILES.ANS", "\x1b[1;36mFILE CABINETS\x1b[0m");
  uint32_t n = 0;
  for (size_t i = 0; i < fileAreas.size(); ++i) {
    if (!listedArea(s, i)) continue;
    const FileArea &a = fileAreas[i];
    char row[100];
    snprintf(row, sizeof(row), "  \x1b[1;33m[%2lu]\x1b[0m %s%-32.32s\x1b[0m \x1b[0;36m%5u files\x1b[0m", (unsigned long)++n,
             a.flag.length() ? "\x1b[1;35m" : "\x1b[1;37m", a.name.c_str(), a.files);
    say(s, row);
  }
  if (!n) say(s, "  No file areas are open to you tonight.");
  say(s, "  \x1b[1;33m[Q]\x1b[0m Main Board");
  prompt(s, "File area #, then ENTER: ");
}
// The door table (doors/doors.h) is the list: a new door shows up by itself.
static void doorMenu(Session &s) {
  s.state = DOOR_MENU; activity(s, "DOORS & GAMES");
  std::vector<MenuItem> items;
  for (const Door &d : DOORS) if (doorOpen(d, s)) items.push_back({String(d.keys[0]), doorName(d.keys[0], d.name)});
  items.push_back({"Q", "Main Board"});
  showMenu(s, "DOORS", items, "DOORS & GAMES");
  prompt(s, "Door command? ");
}
static void finishLogin(Session &s) {
  s.authenticated = true;
  s.signedAt = millis();  // ghost pages count from here (core/ghost.h)
  s.readOnly = hasFlag(s.user, "READONLY");
  // Older cards could already contain a SYSOP-named account created before
  // the level/flag bootstrap rule was added.  SYSOP is the reserved primary
  // operator identity, so normalize that record on its next successful login
  // instead of leaving the caller trapped behind the console gate forever.
  bool repairedSysop = false;
  if (s.user.normalized == "SYSOP") {
    String flags = up(s.user.flags);
    bool flagged = ("," + flags + ",").indexOf(",SYSOP,") >= 0;
    if (s.user.level != 255 || !flagged || s.user.locked) {
      s.user.level = 255;
      s.user.locked = false;
      if (!flagged) {
        if (flags.length()) flags += ",";
        flags += "SYSOP";
        s.user.flags = flags;
      }
      repairedSysop = true;
      trace("SYSOP_REPAIR", "normalized primary SYSOP account");
    }
  }
  // The ship's maintenance account goes straight to the ship console: no
  // counters, no bulletins, no menu.
  if (isMaintenance(s.user)) {
    startPowerTimer(s);
    event("LOGIN", s.user.handle + " node " + s.node);
    trace("LOGIN_OK", s.user.handle + " node " + s.node);
    enterShip(s);
    return;
  }
  s.powered = true;  // every other login has power: the prologue is the maintenance account's alone
  // A read-only account never changes its record: no call count, no touch.
  // An own account becomes the caller here, and takes what crew visits earned
  // (core/haunt.h), in the same save.
  if (!s.readOnly) {
    storyOpen(s.user);   // the story record; story flags still in the flag field move into it
    hauntOwnLogin(s);
    endingRecord(s);   // a finished caller stays in WHO (doors/manifest.h)
    s.user.calls++;
    s.user.lastCall = nowText();
    if (!touchUser(s.user)) say(s, "\r\nWarning: user statistics were not saved. See Serial console.");
  }
  if (repairedSysop) say(s, "\r\nPrimary SYSOP account normalized: level 255, SYSOP flag.");
  callers++; loadPointer(s); event("LOGIN", s.user.handle + " node " + s.node); trace("LOGIN_OK", s.user.handle + " node " + s.node); activity(s, "BULLETINS");
  say(s, "\r\nCaller #" + String(callers) + "  Security: " + String(s.user.level) + "  Conference: " + confName(s.conference));
  // Mandatory bulletins from BULLETINS.INI are shown at a caller's first login only (a crew login or a return visit skips them); B lists them after that.
  bool shown = false;
  for (const auto &b : board.bulletins)
    if (b.mandatory && (!s.readOnly && s.user.calls <= 1) && bulletinOpen(s, b)) { showBulletin(s, b); shown = true; }
  if (!shown) say(s, "Welcome to " + board.name + ".");
  // Q20 (Addition 5 and 6): a crew login says once how it is left and where the manifest is.
  if (s.readOnly && !isMaintenance(s.user)) {
    crewKnownGrant(s);
    String note = "This is a crew account, read-only. G goes back to Login: for your own account.";
    if (s.ownUserId && s.ownHandle.length()) note += "\r\nY goes straight back to " + s.ownHandle + ".";
    if (manTable.rows.size()) note += "\r\nYour crew manifest is on your own account.";
    say(s, s.ansi ? "\x1b[0;36m" + note + "\x1b[0m" : note);
  }
  // Triggers: an own login, or a visit to a crew account. Pages wait for the prompt below.
  if (s.readOnly) hauntEvent(s, haunt::EV_CREW_LOGIN, s.user.handle);
  else hauntEvent(s, haunt::EV_LOGIN, "");
  s.state = BULLETIN; prompt(s, "Press any key for the Main Board: ");
}
static void loginPrompt(Session &s) {
  if (powerEnabled) { activity(s, "AWAITING LOGIN"); brownoutLogin(s); return; }  // front page: the banner and Login:
  s.state = LOGIN_CHOICE;
  activity(s, "AWAITING LOGIN");
  if (s.ansi) art(s, board.welcome.c_str(), board.name);
  else plainArt(s, "/BBS/ASCII/LOGIN/WELCOME.ASC", board.name);
  prompt(s, "(N)ew user, (L)ogin, or (G)oodbye? ");
}
static void newSession(Session &s) {
  s.connectedAt = s.lastAt = s.lastPoll = millis();
  loginPrompt(s);
}
static void leaveChat(Session &s, const char *why);  // bbs/chat.h
static void closeSession(Session &s) {
  if (!s.used) return;
  if (s.state == CHAT) leaveChat(s, "lost carrier.");
  if (playingMud(s)) mudBroadcastRoom(s, s.user.handle + " loses carrier.");
  trace("SESSION_CLOSE", String("node ") + s.node + " " + (s.user.handle.length() ? s.user.handle : "anonymous"));
  event("LOGOFF", (s.user.handle.length() ? s.user.handle : String("anonymous")) + " node " + s.node + ", " + String((millis() - s.connectedAt) / MS_PER_MINUTE) + " min");
  if (s.socket) s.socket.stop();
  if (s.wsClient) wsHangups.push_back(s.wsClient);
  s = Session();
}
static Session *allocate(bool web, NetClient incoming = NetClient()) {
  for (uint8_t i = 0; i < board.maxNodes; ++i) if (!sessions[i].used) { Session &s = sessions[i]; s = Session(); s.used = true; s.web = web; s.socket = incoming; if (!web) s.cps = 0; /* Telnet: full speed */ s.node = i + 1; s.token = salt(); newSession(s); event("CONNECT", String("node ") + s.node + (web ? " web" : " telnet")); trace("SESSION_OPEN", String("node ") + s.node + (web ? " web" : " telnet")); return &s; } trace("SESSION_REJECT", "all nodes busy"); return nullptr;
}
static Session *webSession(uint8_t node, const String &token) { if (!node || node > MAX_NODES) return nullptr; Session &s = sessions[node - 1]; return s.used && s.web && s.token == token ? &s : nullptr; }
