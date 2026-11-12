#pragma once
// The door table. A new door is one header plus one line here.
// Part of the single TinyBBS translation unit: included in order from main.cpp.

// keys: menu keys that open the door (first one is shown in prompts).
// owns: true while the caller is inside the door.
// handle: one typed line while the caller is inside the door.
struct Door {
  const char *keys;
  const char *name;
  void (*enter)(Session &);
  bool (*owns)(const Session &);
  void (*handle)(Session &, String &, const String &);
  bool saves;                       // keeps per-user state: hidden from read-only accounts
  bool (*listed)(const Session &);  // nullptr = in the door menu for everyone; else who sees it
};

static const Door DOORS[] = {
  {"V", "VolaMUD", enterMud, playingMud, handleMud, true, nullptr},
  // The ship console: entered at login by the maintenance account, never from the menu.
  {"", "Ship systems", enterShip, ownsShip, handleShip, false, shipListed},
};

// Is this door in the menu for this caller? Nothing is shown that would not work.
static bool doorOpen(const Door &d, const Session &s) {
  if (d.saves && !canWrite(s)) return false;
  return !d.listed || d.listed(s);
}
static bool anyDoorOpen(const Session &s) {
  for (const Door &d : DOORS) if (doorOpen(d, s)) return true;
  return false;
}

static const Door *doorFor(const Session &s) {
  for (const Door &d : DOORS) if (d.owns(s)) return &d;
  return nullptr;
}
