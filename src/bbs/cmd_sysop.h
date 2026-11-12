#pragma once
// SysOp console. The user editor is in bbs/user_editor.h.
// Part of the single TinyBBS translation unit: included in order from main.cpp.

static bool sysopCommand(Session &s, String &line, const String &cmd) {
  if (s.state == SYSOP_MENU) {
    if (cmd == "Q" || !cmd.length()) { mainMenu(s); return true; }
    if (cmd == "U") { sysopUsers(s); return true; }
    if (cmd == "W") { nodeMonitor(s, SYSOP_MENU); return true; }
    if (cmd == "I") { uploadQueue(s); return true; }
    if (tokenAt(cmd, 0) == "L") { sysopLogs(s, afterTokens(line, 1)); return true; }
    if (tokenAt(cmd, 0) == "POWER") { sysopPowerCommand(s, cmd); prompt(s, "SysOp command? "); return true; }
    if (cmd == "R" || cmd == "RELOAD") {
      say(s, "Settings reloaded: " + loadSettings() + ".");
      say(s, "Network settings take effect after a restart.");
    }
    else if (cmd == "F" && s.web) {
      write(s, "\x1b]9;LINK;/sysop?t=" + issueAdminToken(s.user) + ";OPEN FILE MANAGER\x07");
      say(s, "\x1b[1;36mFile manager armed. Tap OPEN FILE MANAGER below the terminal.\x1b[0m");
      say(s, "The link stays valid for 30 minutes of use.");
    }
    else if (cmd == "S") {
      say(s, "Calls: " + String(callers) + "  Users next #: " + String(nextUserId) + "  Messages: " + String(nextMessageId - 1));
      say(s, "Nodes: " + String(board.maxNodes) + "  SD: " + (sdReady ? "online" : "offline"));
    } else say(s, s.web ? "Use U, W, I, L, S, R, F, or Q." : "Use U, W, I, L, S, R, or Q.");
    prompt(s, "SysOp command? "); return true;
  }
  return false;
}
