#pragma once
// The story's accounts, from STORY/ACCOUNTS.INI: the crew's logins, ELIZA and
// the ground relay. Made by the board at start-up, never by hand in
// USERS.DAT. An account that exists is left alone (callers change nothing on
// them: crew accounts are read-only). A card without the file has none.
//
// One section per handle (3 to 16 characters):
//   name=      real name              location=  shown in the user list
//   password=  empty = nobody can log in (a random one is set)
//   level=     security level, default 20
//   flags=     access flags of the account, e.g. READONLY,VISIT_TY
//   calls=     call count shown in the user list
//   last=      "YYYY-MM-DD HH:MM", the last call; empty = never
//   sync=1     an account that already exists is brought in line with the file: password,
//              level and flags (a board made before the account changed, Q12d). Calls and
//              last call are left as they are. Never for an account with the SYSOP flag.
//
// All caller-visible wording is a DRAFT. The author makes the final edit.
// Part of the shared core: included in order from src/bbs_core.h, after bbs/power_ui.h.

static constexpr const char *ACCOUNTS_INI = "/BBS/STORY/ACCOUNTS.INI";

static void ensureStoryAccounts() {
  if (!sdReady) return;
  Ini ini;
  if (!iniLoad(ACCOUNTS_INI, ini)) return;
  for (const String &sec : iniSections(ini)) {
    String handle = clean(sec, 16, true);
    if (handle.length() < 3) { event("ACCOUNT_SKIP", sec + ": handle must be 3 to 16 characters"); continue; }
    User existing;
    if (findUser(up(handle), existing)) {
      if (iniGet(ini, sec, "sync") != "1" || hasFlag(existing, "SYSOP")) continue;
      String password = iniGet(ini, sec, "password");
      uint8_t level = (uint8_t)constrain(iniGet(ini, sec, "level", "20").toInt(), 0L, 254L);
      String flags = cleanFlags(iniGet(ini, sec, "flags"));
      bool samePass = !password.length() || sha256hex(existing.salt + ":" + password) == existing.hash;
      if (samePass && existing.level == level && up(existing.flags) == up(flags)) continue;
      if (!samePass) { existing.salt = salt(); existing.hash = sha256hex(existing.salt + ":" + password); }
      existing.level = level; existing.flags = flags;
      if (touchUser(existing)) event("ACCOUNT", existing.handle + " brought in line with ACCOUNTS.INI");
      else event("ACCOUNT_FAIL", existing.handle + ": not updated");
      continue;
    }
    User u;
    u.handle = handle; u.normalized = up(handle);
    u.realName = clean(iniGet(ini, sec, "name"), 40);
    u.location = clean(iniGet(ini, sec, "location"), 40);
    String password = iniGet(ini, sec, "password");
    if (!password.length()) password = salt() + salt();   // nobody can log in
    u.salt = salt(); u.hash = sha256hex(u.salt + ":" + password);
    u.level = (uint8_t)constrain(iniGet(ini, sec, "level", "20").toInt(), 0L, 254L);
    u.flags = cleanFlags(iniGet(ini, sec, "flags"));
    u.calls = (uint32_t)max(0L, iniGet(ini, sec, "calls", "0").toInt());
    String last = iniGet(ini, sec, "last");
    u.lastCall = parseClock(last) ? last : String();
    newUserDefaults(u);
    if (appendUser(u)) event("ACCOUNT", u.handle + " created from ACCOUNTS.INI");
    else event("ACCOUNT_FAIL", u.handle + ": not created");
  }
}

// Q20 (Addition 6): the crew accounts this caller has opened. The first crew login that works
// puts KNOWN_<HANDLE> on the caller's own account (or carries it to the next own login on this
// line). L on the own account lists them and logs in without the password; the manifest notebook
// writes the password down. The knowledge is the caller's, never the crew account's.
static String crewKnownFlag(const String &normalized) {
  String f = "KNOWN_";
  for (uint16_t i = 0; i < normalized.length(); ++i) { char c = normalized[i]; f += isalnum((unsigned char)c) ? c : '_'; }
  return f;
}
static void crewKnownGrant(Session &s) {
  if (!s.readOnly || isMaintenance(s.user) || hasFlag(s.user, "SYSOP")) return;
  User own;
  bool have = hauntOwner(s, own);
  if (!hauntGrant(s, have ? &own : nullptr, hauntStd(crewKnownFlag(s.user.normalized))))
    event("TRIGGER_FAIL", "KNOWN " + s.user.handle + ": not saved");
}
// The known crew accounts of the own account signed in on `s`, in ACCOUNTS.INI order. Only
// read-only story accounts that are not locked and below the SysOp; at most nine (one key each).
static std::vector<User> crewKnown(const Session &s) {
  std::vector<User> out;
  if (!s.authenticated || s.readOnly || !sdReady) return out;
  std::string flags = userFlagList(s.user);
  if (flags.find("KNOWN_") == std::string::npos) return out;
  Ini ini;
  if (!iniLoad(ACCOUNTS_INI, ini)) return out;
  for (const String &sec : iniSections(ini)) {
    String n = up(clean(sec, 16, true));
    User u;
    if (!haunt::flagsHas(flags, hauntStd(crewKnownFlag(n)))) continue;
    if (!findUser(n, u) || u.locked || !hasFlag(u, "READONLY") || hasFlag(u, "SYSOP") || u.level >= 255) continue;
    out.push_back(u);
    if (out.size() >= 9) break;
  }
  return out;
}
// The password the notebook writes down: the card's, and only while it still opens the account.
static String crewKnownPassword(const User &u) {
  Ini ini;
  if (!iniLoad(ACCOUNTS_INI, ini)) return "";
  for (const String &sec : iniSections(ini)) {
    if (up(clean(sec, 16, true)) != u.normalized) continue;
    String pw = iniGet(ini, sec, "password");
    return pw.length() && sha256hex(u.salt + ":" + pw) == u.hash ? pw : String();
  }
  return "";
}
