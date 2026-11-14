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
  {"V", "VolaMUD", enterMud, playingMud, handleMud, true, mudListed},
  // The ship console: entered at login by the maintenance account, never from the menu.
  {"", "Ship systems", enterShip, ownsShip, handleShip, false, shipListed},
  // ELIZA's diagnostics: query codes from the comms manual; open while the ship has power.
  {"I", "Diagnostics", enterDiag, ownsDiag, handleDiag, false, diagListed},
  // The NOWHERE shell: in the menu only for callers who hold SHELL.INI's needs= flags.
  {"N", "NOWHERE shell", enterShell, ownsShell, handleShell, false, shellListed},
  // The crew manifest: reached by R from the main menu (when it works for the caller), never from this menu.
  {"", "Crew manifest", enterManifest, ownsManifest, handleManifest, false, manifestNever},
  // Ty's snake (doors/snake.h): turn based, saves nothing, so every caller with power may play.
  {"S", "Snake (by tyv)", enterSnake, ownsSnake, handleSnake, false, snakeListed},
  // Scenes (core/scene.h): started by a trigger rule (scene=NAME), never from the menu.
  {"", "Scene", sceneEnter, ownsScene, handleScene, false, sceneListed},
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
