# Requirements - Ghosts and ELIZA's voice

Slices S6 and S9 in `docs/SLICES.md`, queue item Q4 in `docs/briefs/QUEUE.md`.
Story: `.kiro/specs/story-bible/` (E03, E04, E34). Triggers:
`.kiro/specs/triggers/`. Scenes: `.kiro/specs/scenes/`. All caller-visible
wording in `GHOSTS.INI` and `SYSOP.INI` is a draft until the author's final
edit.

## Glossary

- Ghost: a caller who is listed in WHO and node chat but is not a session.
  One section in `STORY/GHOSTS.INI`.
- ELIZA's line: one section in `STORY/SYSOP.INI`: something the SysOp says,
  keyed by a trigger rule, a chat keyword, the caller entering chat, or
  nothing else (a fallback).
- Caller flags: the flags on the caller's own account, or the flags a crew
  login carries (as in the triggers).
- Marker: `G_<GHOST>` or `E_<LINE>`, the flag that records a ghost's page or
  a once-only line has been given to this caller.

## Requirements

### R1 - Ghosts in WHO

1. THE board SHALL list each ghost that is visible to the caller in WHO and in
   the SysOp node monitor, with its node number, handle, idle time and
   activity text, all from `GHOSTS.INI`.
2. WHO SHALL have an idle column for every row. A real caller's idle time is
   the time since the last key. A ghost's idle time is the text on the card
   (it may be impossible, such as `28y`).
3. A ghost SHALL be visible only when the caller meets its conditions:
   `has=`, `lacks=`, `online_min=` (minutes since connect) and `powered=1`
   (the board is not in brownout).
4. A ghost SHALL NOT be listed on a node number a real session uses. The rows
   SHALL be in node number order.
5. A ghost SHALL NOT be a session: it SHALL NOT take a node from the pool, be
   disconnectable, or show in any caller's chat log.

### R2 - Ghost pages

1. WHERE a ghost gives `page_after=` and `page=`, THE board SHALL page the
   caller from that ghost (`page_from=`, default the ghost's handle) once the
   caller has been signed in that many seconds, at the next calm menu prompt,
   as the triggers' pages are shown.
2. A ghost page SHALL be given at most once per caller. THE marker SHALL be
   saved first, with the ghost's `set=` flags, and nothing SHALL be shown
   unless that save worked.
3. THE SysOp ghost SHALL be able to page a caller within 30 seconds of login
   on both transports.

### R3 - Ghosts in chat

1. A ghost with `chat=1` that is visible to the caller SHALL be named in the
   chat bar, in `/W` and in the "Here" list of that caller, and SHALL NOT be
   named for a caller who cannot see it.

### R4 - ELIZA's voice

1. THE board SHALL read ELIZA's lines from `STORY/SYSOP.INI`. No line of her
   speech SHALL be in firmware.
2. A line SHALL have exactly one key: `trigger=` (a trigger rule id), `keys=`
   (chat words), `chat=join` (when the caller enters chat) or `chat=else` (a
   fallback for chat lines no keyword matched).
3. WHEN a trigger rule fires for a caller, THE lines keyed by it SHALL be
   shown as a page from ELIZA at the next calm prompt. Conditions SHALL see
   the flags the rule just set.
4. WHEN the caller sends a line in chat, THE board SHALL answer with the first
   line whose keys occur in it as whole words or phrases, compared without
   regard to case and punctuation. IF none matches, THEN it SHALL use a
   fallback line, taking them in turn.
5. A line SHALL give `has=`, `lacks=` and `set=`; `set=` flags SHALL be added
   to the caller as the line is spoken. A line SHALL give `once=1` to be
   spoken once per caller. Trigger and join lines are always once-only.
6. ELIZA SHALL speak in chat only if a ghost with `chat=1` and her handle
   (the line's `from=`, default ELIZA) is visible to that caller: nothing is
   said by someone who is not shown to be there.

### R5 - Typing delays

1. A chat answer SHALL appear after a thinking delay (`delay=`, default 1500
   ms) plus a typing time that grows with the length of the text. A line with
   several `|` parts SHALL appear part by part, each with its own typing
   time.
2. An answer SHALL be printed the way other callers' chat lines are, so it
   does not break a half-typed line. IF the caller has left chat before it is
   due, THEN it SHALL be dropped.
3. At most three answers SHALL wait for one caller.

### R6 - Scenes do not echo keys

1. WHILE a scene types its text, pauses or waits for Enter, THE board SHALL
   NOT echo keys the caller types, and SHALL NOT keep them as a pending line.
   Enter SHALL still skip the typing or the pause as it does today.
2. WHILE a scene waits for a choice, THE board SHALL echo as it does for any
   line prompt.

### R7 - Safety

1. A malformed ghost or line SHALL be skipped, counted and logged; it SHALL
   NOT stop the board starting or the other entries loading.
2. THE tables SHALL hold at most 24 ghosts and 200 lines, and every text
   field SHALL be capped.
3. A missing file SHALL mean no ghosts and no scripted voice.
4. Flags only grow. A ghost or line SHALL NOT remove or rewrite a flag.

### R8 - Transports and platform

1. THE features SHALL work the same over Telnet and the browser, and SHALL
   stay in the shared core with no ESP32 or Linux headers.
2. THE parsers and the decision logic SHALL be free of platform types so that
   a plain C++ compiler can test them on the development PC.
3. THE tables SHALL be on the card, loaded at boot and on RELOAD.
