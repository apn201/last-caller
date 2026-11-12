# Design: Rebrand (S1)

## Approach

Mostly card content and text. Code edits are small and local. No new
features, no new story.

## Card

- `SYSTEM.INI`, `NETWORK.INI`: set the values from the brief. `[DOORS]`
  keeps `V=VolaMUD` only.
- `BULLETINS.INI`: two short bulletins, plain 1990s register, ELIZA calm and
  polite, no explanation of anything. Marked as drafts in a comment.
- Art (`.ANS`/`.ASC`): a one-off script reads each file, replaces the old
  name with HALCYON (keeping column widths by padding), and remaps SGR
  colour codes to greys (30/37/90/1;30/1;37 family) with one dark red
  (31) accent. Menu option areas are left alone, so `MENUS.INI` row/col
  stays valid. The script is run once and not committed.
- The two Legend door art files in `ANSI/DOORS` are deleted after `git grep` shows
  only `legend.h` reaches them. Because `legend.h` stays compiled for the
  purse, its `art()` call is reachable only through the removed door row;
  the call is kept harmless or removed if the build needs it.

## Code

- `src/doors/doors.h`: remove the `LK` and `D` rows. `legend.h` and
  `dice.h` stay included so shared purse functions compile. Check that
  nothing else refers to the removed rows (state enum, sysop door
  settings).
- Strings: Old place names in `volamud.h`, `legend.h`, `sysop.h`,
  `user_editor.h` become neutral ("Black Crown", "the Bank", "the Hall").
  `WORLD.TXT` on the card gets the same treatment.
- `scene_logo.h`: replace the pixel data with a neutral splash by making
  the LCD fall back to the card logo when present, and a plain built-in
  mark otherwise; the comment no longer names the old board.
- `make-lcd-logo.py`: help text only.

## Docs

README rewritten for Last Caller; `CONTENT_POLICY.md` new;
`ROADMAP`, `MENU_AUDIT`, `VOLA_MUD`, `APACHE_PORT`, `AGENTS.md` scrubbed;
`THIRD_PARTY_NOTICES.md` gets the `DIALUP.MP3` line.

## Verification

- `git grep -i kouvo` against the allowed list.
- `pio run` (ESP32). The server build cannot run on Windows; say so.
- Menu check by reading `MENUS.INI` against the art.
