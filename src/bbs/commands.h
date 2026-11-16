#pragma once
// Line input and the top-level command dispatch.
// Part of the single TinyBBS translation unit: included in order from main.cpp.

static void handleLine(Session &s, String line) {
  s.lastAt = millis(); line.trim(); String cmd = up(line);
  if (s.state != SCENE_PLAY) s.sceneHold = false;  // a command after a scene: the next scene may come (core/haunt.h)
  if (s.user.locked) { closeSession(s); return; }
  if (s.pageNode && isSysop(s) && (cmd == "/A" || cmd == "/R")) { answerPage(s, cmd == "/A"); return; }
  bool sysopState = s.state == SYSOP_MENU || s.state == SYSOP_USERS || s.state == SYSOP_DOORS || s.state == SYSOP_ADD_PASSWORD ||
                    s.state == SYSOP_DELETE_CONFIRM || s.state == SYSOP_USER_EDIT || s.state == SYSOP_USER_FIELD || s.state == SYSOP_ADD_HANDLE || s.state == SYSOP_NODES;
  if (sysopState && !isSysop(s)) {
    s.hideInput = false;
    s.editId = 0;
    mainMenu(s);
    return;
  }
  if (s.authenticated && !canJoinConference(s.user, s.conference)) {
    s.conference = 0;
    mainMenu(s);
    return;
  }
  if (eggCommand(s, line, cmd)) return;   // modem commands and hidden words: bbs/eggs.h
  loginCommand(s, line, cmd) || mainCommand(s, line, cmd) || sysopCommand(s, line, cmd) || userEditorCommand(s, line, cmd) ||
    uploadCommand(s, line, cmd) || fileCommand(s, line, cmd) || messageCommand(s, line, cmd) || doorCommand(s, line, cmd);
}
static void input(Session &s, const String &bytes) {
  if (bytes.length()) s.lastAt = millis();   // Q21: any key keeps the line (the idle warning says so)
  // While a picture or file is streaming, keys belong to the stream: space
  // pauses and resumes, anything else skips to the end.
  if (s.state == READER && !streaming(s)) { readerInput(s, bytes); return; }
  if (s.state == FILE_IMAGE && !streaming(s)) { imageKey(s, bytes); return; }
  if (s.state == FILE_MUSIC && !streaming(s)) { musicKey(s, bytes); return; }
  if (streaming(s)) {
    bool space = false, other = false;
    for (size_t i = 0; i < bytes.length(); ++i) {
      char c = bytes[i];
      // Telnet sends Enter as CR NUL or CR LF. The CR started the picture;
      // its NUL or LF can arrive a moment later and must not skip it.
      if (c == 0) continue;
      if (c == '\n' && s.ignoreNextLf) { s.ignoreNextLf = false; continue; }
      if (c == ' ') space = true;
      else other = true;
    }
    if (!space && !other) return;
    // Q22: at "Press any key" (a login banner or a bulletin still drawing) SPACE is a key, not a pause: it skips to the end.
    if (space && !other && s.state != BULLETIN) s.paused = !s.paused;
    else { if (bytes.indexOf('\r') >= 0) s.ignoreNextLf = true; skipStream(s); }
    return;
  }
  for (uint16_t i = 0; i < bytes.length() && s.used && s.state != GOODBYE; ++i) { char c = bytes[i];
    // The Enter after a hotkey that closed a viewer belongs to that key, not to the next prompt.
    if (s.eatEnterUntil) {
      if ((c == '\r' || c == '\n' || c == 0) && s.input.length() == 0 && (int32_t)(s.eatEnterUntil - millis()) > 0) { if (c == '\r') s.ignoreNextLf = true; else if (c == '\n') s.ignoreNextLf = false; if (c != 0) s.eatEnterUntil = 0; continue; }
      s.eatEnterUntil = 0;
    }
    // A scene types its own text: a key typed meanwhile is neither echoed nor kept (it would land in the
    // middle of a sentence). G1: only Q skips a scene; ENTER and other keys do nothing, except a key at
    // an [any key] wait once its grace is over. A lone Q waits HOTKEY_MS like a menu hotkey, so a word
    // typed ahead ("quit") does not skip the scene.
    const bool quiet = s.state == SCENE_PLAY && !s.sceneRun.wantsEcho();
    if (quiet) {
      const bool keyWait = s.sceneRun.phase() == scene::P_KEY && s.sceneRun.armed();
      if (s.sceneReplay && (c == 'q' || c == 'Q')) { s.input = ""; handleLine(s, "Q"); continue; }   // Q21: a replay from the notebook ends at once
      if (c == '\r' || c == '\n') {
        if (c == '\n' && s.ignoreNextLf) { s.ignoreNextLf = false; continue; }
        if (c == '\r') s.ignoreNextLf = true;
        s.input = "";
        if (s.hotKey) { s.hotKey = 0; handleLine(s, "Q"); }   // "q" then ENTER, a line-mode client: a lone Q
        else if (keyWait) { handleLine(s, ""); if (s.state != SCENE_PLAY) { if (i + 1 < bytes.length()) s.typeAhead = "x"; break; } }   // G1: the rest of this burst was typed ahead of the scene's end
        continue;
      }
      if ((uint8_t)c < 32) continue;
      if (s.hotKey) { if (s.typeAhead.length() < 24) s.typeAhead += s.hotKey; s.hotKey = 0; }   // another key right behind it: a word, not a Q
      if ((c == 'q' || c == 'Q') && !s.typeAhead.length()) { s.hotKey = c; s.hotAt = millis(); continue; }
      if (keyWait) { s.input = ""; handleLine(s, ""); if (s.state != SCENE_PLAY) { if (i + 1 < bytes.length()) s.typeAhead = "x"; break; } }   // [any key]: any key goes on; the rest of the burst was typed ahead (G1)
      else if (s.typeAhead.length() < 24) s.typeAhead += c;   // Q22: not echoed, and dropped when the scene ends (contract C7): a key meant for what the caller could not see must not land on the next prompt
      continue;
    }
    // Q20 UX, contract C1: a letter at a menu acts at once. It waits hotkeyTick's moment first: a
    // second key right behind it (a pasted word, a tool, Telnet in line mode) makes a typed word.
    if (s.hotKey) {
      char h = s.hotKey; s.hotKey = 0;
      if (!s.hideInput) write(s, String(h));
      s.input += h;
    } else if (!s.input.length() && hotkeyState(s)) {
      if (c == ' ' && s.state != BULLETIN && s.state != MSG_VIEW) continue;   // a tap or a stray space is not the start of a word
      if (s.state == SCENE_PLAY) { if ((c >= '1' && c <= '9') || c == 'q' || c == 'Q') { s.hotKey = c; s.hotAt = millis(); continue; } }
      else if (s.state == CHAT) { if (c == 'q' || c == 'Q') { s.hotKey = c; s.hotAt = millis(); continue; } }   // a lone Q leaves; anything after it makes a line
      else if (isalpha((unsigned char)c) || c == '?' || (s.state == MANIFEST_PROMPT && s.digitHot && isdigit((unsigned char)c)) || (s.state == CREW_PICK && isdigit((unsigned char)c)) || (s.state == BULLETIN && c >= ' ') || (s.state == MSG_VIEW && c > ' ')) { s.hotKey = c; s.hotAt = millis(); continue; }
    }
    // CRLF is one Enter key.  Without this guard the LF becomes a phantom
    // handle/command in Telnet clients such as PuTTY.
    if (c == '\r') { if (!s.hideInput) say(s); String line = s.input; s.input = ""; s.ignoreNextLf = true; handleLine(s, line); }
    else if (c == '\n') { if (s.ignoreNextLf) { s.ignoreNextLf = false; continue; } if (!s.hideInput) say(s); String line = s.input; s.input = ""; handleLine(s, line); }
    else if (c == 8 || c == 127) { if (s.input.length()) { s.input.remove(s.input.length() - 1); write(s, "\b \b"); } }
    else if ((uint8_t)c >= 32 && s.input.length() < 900) { s.input += c; write(s, s.hideInput ? String("*") : String(c)); }   // Q20 UX: a password shows a star per key
  }
}
// bbsTick: a letter that nobody followed within HOTKEY_MS is a hotkey. It is echoed and run as the
// one-letter line it stands for, and the Enter that a line-mode client sends after it is eaten.
static constexpr uint32_t HOTKEY_MS = 120;
static void hotkeyTick(uint32_t now) {
  for (auto &s : sessions) {
    // Q22, contract C7: keys typed during a scene were meant for what the caller could not see. When the
    // scene has ended they are dropped, not played into whatever prompt comes next.
    // The caller is told once, under the prompt, so a key that did nothing is not a mystery.
    if (s.used && s.typeAhead.length() && s.state != SCENE_PLAY) {
      if (s.state == GOODBYE || !s.authenticated) s.typeAhead = "";
      else if (!streaming(s) && s.pending.length() == 0 && s.jobs.empty()) {
        s.typeAhead = "";
        write(s, "\r\n\x1b[0;90m(Keys typed during the scene were not used.)\x1b[0m\r\n" + s.lastPrompt);
        if (s.input.length()) write(s, s.hideInput ? String("*") : s.input);
      }
    }
    if (!s.used || !s.hotKey || now - s.hotAt < HOTKEY_MS) continue;
    char c = s.hotKey; s.hotKey = 0;
    if (s.used && s.state == SCENE_PLAY && !s.sceneRun.wantsEcho()) { if (c == 'q' || c == 'Q') handleLine(s, "Q"); continue; }   // G1: a lone Q skips the scene
    if (!hotkeyState(s) || s.input.length()) continue;
    s.eatEnterUntil = now + 700;
    if (!s.hideInput) { write(s, String(c)); say(s); }
    handleLine(s, String(c));
  }
}
