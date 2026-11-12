# Design - Ghosts and ELIZA's voice

Requirements: `requirements.md`. Same split as triggers and scenes: pure
logic in a header a plain C++ compiler tests, a thin board side.

## Files

| File | Role |
| --- | --- |
| `src/core/ghost_logic.h` | Pure. Ghost and line tables, INI parse from plain triples, visibility, keyword matching, idle text, typing times. `std::string` only. Test: `tools/test/ghost_logic_test.cpp` |
| `src/core/ghost.h` | Board side: loads the two files, builds the context for a session, WHO rows, pages, the chat queue. Included after `core/scene.h` |
| `sd/BBS/STORY/GHOSTS.INI`, `SYSOP.INI` | The ghosts and ELIZA's lines |

`Session` gets `signedAt` (millis at sign-in), `ghostTurn` (fallback counter)
and `ghostSay` (answers waiting for chat). `scene_logic.h` gets
`Player::wantsEcho()`.

## GHOSTS.INI

```
[ID]                   ; letters digits _, at most 24
handle=CMDR            ; max 16, shown in WHO
node=9                 ; 0 to 99, shown in WHO
activity=Reading mail  ; max 24
idle=28y               ; text, max 10; empty = 0:00
has= lacks=            ; flags of the caller
online_min=5           ; minutes the caller has been connected
powered=1              ; only while the board has power
chat=1                 ; named in node chat
page_after=20          ; seconds after sign-in
page_from= page=       ; one line to the caller; "|" breaks a line
set=FLAG,FLAG          ; added when the page is given
```

## SYSOP.INI

```
[ID]
from=ELIZA             ; default ELIZA, max 16
say=Line one.|Line two ; max 600
trigger=RULE_ID        ; a page after that trigger rule fires
keys=word,two words    ; chat: whole words or phrases
chat=join|else         ; entering chat | fallback for chat lines with no match
has= lacks= set=
once=1                 ; keys and else lines; trigger and join are always once
delay=1500             ; ms before she starts typing, 0 to 10000
```

Exactly one of `trigger`, `keys`, `chat` per line.

## Decisions

- Ghost visible: `visible(g, ctx)`: `has`, `lacks`, `online_min`, `powered`.
  Context is the caller's flags (own account or carried), minutes connected,
  seconds signed in, and whether the board has power.
- Matching: text and key are lower-cased, anything but a letter or digit
  becomes a space, runs of spaces collapse, both are padded with one space
  and the key is searched as a substring. "Who are you?" hears `who are you`
  and `you`, not `yo`.
- Fallback: eligible `chat=else` lines, picked by `ghostTurn % count`;
  `ghostTurn` goes up on every use.
- ELIZA speaks only when a visible `chat=1` ghost has her handle (R4.6).
- Page and trigger lines are `HauntShow`s: they wait for `hauntCalm`, so they
  never break the reader or a half-typed line.
- Chat answers are held in `Session.ghostSay` with a due time. `ghostTick`
  (from `bbsTick`) prints one when due: `\r ESC[2K`, the line as
  `[HANDLE] text`, the prompt and the half-typed input, like `broadcastChat`.
  Typing time is 55 ms per character, 400 ms at least, 5000 ms at most, per
  part. Not logged in `CHAT.LOG`: it is per caller.
- Markers (`G_<ID>`, `E_<ID>`) go in with the line's `set=` flags in one
  `hauntGrant`; nothing is said if that fails. Keep ids short: the flag field
  holds 255 characters.
- Trigger lines: `hauntEvent` calls `ghostTrigger` after a rule fires, with
  the flags as they are after the rule's own.
- WHO rows are collected into one list, sorted by node. Real rows first
  claim their nodes; a ghost on a used node is skipped.

## Scene echo

`input()` in `bbs/commands.h` asks `s.sceneRun.wantsEcho()` for each key while
`state == SCENE_PLAY`. False in every phase except the choice prompt: the
byte is dropped (not echoed, not kept), and Enter calls `handleLine` with an
empty line without echoing a newline, so it still flushes or skips.

## Not in this item

The real SysOp's node-monitor handling of ghosts (a ghost node is "no caller
on that node"), ghost-to-ghost talk, ghosts that move between activities
(Q13 glitches), the sticky ghost handle (Q6), the content of the first real
conversations (Q7 and later). Page SysOp (`P`) is unchanged: it pages a real
SysOp.
