---
inclusion: always
---

# Interaction contract

Full text and the audit behind it: `docs/PLATFORM_UX.md`. Telnet and the
browser follow the same rules. The 1990s look, the art and the voice stay;
the friction goes.

- **Keys and lines.** At a menu, list, pager or yes/no, a letter acts at once.
  At a text or number prompt, ENTER ends the line and the prompt says so.
  Digits start a number; ENTER ends it.
- **Global keys** (never in text entry): Q back one level, X main menu,
  ? the keys of this screen, N next, P previous, SPACE next page, B back a
  page, ESC = Q. ENTER is the forward step or a redraw; it never leaves a
  level.
- **Same action, same key.** L list, W write, R reply, N next or new,
  P previous, O page the operator, K kill, E edit, H history, T thread,
  D download, V view, U upload. A new screen takes its letters from this
  list. If a letter would need a second meaning, move the odd action and
  record it in `docs/PLATFORM_UX.md` section 3.
- **One pager** for every text a caller reads, whatever its length: same
  footer, same keys. Menus and option lists are not documents.
- **Yes or no is one key.** "Continue" is any key, not "Press ENTER".
- **Scenes and timed events never swallow a key or cut a line.** Keys typed
  during a scene are kept and played back after it, in order.
- **Chats and doors say how to leave** when entered and take Q (or `/Q` where
  the line is text), QUIT and EXIT.
- **The browser adds buttons, never rules.** A button sends the key a
  keyboard would.
- Never show a key that does not work for that caller on that transport.
- A change to a key or a prompt updates `docs/MENU_AUDIT.md`, the in-world
  help, `tools/test/e2e_telnet.py`, the `bbs-caller` tool and any story text
  that names the key, and `tools/story_lint.py` checks it.
