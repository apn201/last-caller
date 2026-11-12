# Requirements - Triggers

Slice S5 in `docs/SLICES.md`, queue item Q2 in `docs/briefs/QUEUE.md`.
Story: `.kiro/specs/story-bible/`. Prologue: `.kiro/specs/power-prologue/`
(R8.4 is built here). All caller-visible wording in trigger files is a draft
until the author's final edit.

## Glossary

- Event: something a caller does that the board reports to the trigger
  engine (login, a message read, a file viewed, an art file viewed, a door
  entered, idle time, time online, a crew-account login).
- Rule: one section in `STORY/TRIGGERS.INI`: an event, conditions and effects.
- Caller flags: the flags on the caller's own account (`User.flags`).
- Own account: the caller's non-read-only account. A crew login (read-only)
  is not an own account.
- Marker: the flag `T_<RULE>` that records a rule with a visible effect has
  fired for that caller.

## Requirements

### R1 - Events

1. THE board SHALL report these events to the engine: `login` (own account
   signed in), `crew_login` (a read-only account signed in, matched by its
   handle), `read` (a message is shown, matched by message number), `file`
   (a file is opened in the viewer, matched by file name), `art` (an ANSI or
   ASCII art file is opened in the viewer, matched by file name, with the
   caller's modem speed), `door` (a door is entered, matched by its first
   key), `idle` (seconds since the caller's last key) and `minutes` (minutes
   since connect).
2. WHERE a rule gives `match=`, THE rule SHALL fire only for that value,
   compared without regard to case.
3. WHERE a rule gives `baud_max=`, THE `art` rule SHALL fire only when the
   caller's modem speed (characters per second times ten) is at or below
   it. A caller with no speed limit SHALL NOT match.
4. THE `idle` and `minutes` rules SHALL fire when their `after=` value is
   reached or passed.

### R2 - Conditions

1. WHERE a rule gives `has=`, THE rule SHALL fire only if the caller has
   every listed flag.
2. WHERE a rule gives `lacks=`, THE rule SHALL fire only if the caller has
   none of the listed flags.
3. WHERE a rule gives `level_min=` or `level_max=`, THE rule SHALL fire only
   for a caller whose level is inside the range.
4. THE conditions SHALL be tested against the caller's flags as they were
   before the event. One event SHALL NOT chain rules: a flag set by one rule
   is seen by the next event, not by the same one.

### R3 - Effects

1. THE `set=` effect SHALL add flags to the caller's own account.
2. THE `page=` effect SHALL show the caller one line from a named sender
   (`page_from=`, default ELIZA) on the caller's own session only.
3. THE `mail=` effect SHALL post a private message to the caller from a named
   sender (`mail_from=`, `mail_subject=`, `mail_body=`) in a named conference
   (`mail_conf=`). No other caller SHALL be able to read it.
4. THE `post=` effect SHALL post a public message as a named user
   (`post_from=`, `post_conf=`, `post_subject=`, `post_body=`), once per
   caller's trigger. IT SHALL be visible to every caller who can read that
   conference.
5. THE `art=` effect SHALL show a card art screen to the caller.
6. THE `add=` effect, on `read` rules, SHALL add one line to the message
   while it is shown to this caller, every time it is shown, and SHALL NOT
   change the stored message.
7. A rule SHALL run its effects in this order: set, page, mail, post, art,
   add.

### R4 - Idempotent, flags only grow

1. A rule with a page, mail, post or art effect SHALL fire at most once per
   caller. THE engine SHALL record the marker `T_<RULE>` in the same save as
   the rule's `set=` flags, and SHALL run the visible effects only after that
   save succeeded. IF the save fails, THEN the effects SHALL NOT run and the
   rule SHALL be due again at the next event.
2. A rule with only `set=` effects SHALL be due only while it would change
   something.
3. THE engine SHALL NOT remove or rewrite any flag. Adding a flag that is
   already there SHALL change nothing.
4. THE flag field SHALL hold enough flags for a story: its limit SHALL be
   255 characters, and a flag that does not fit SHALL be reported to the
   event log, never dropped without a trace.
5. A flag change SHALL also reach any other online session of the same
   account, so that session's next save does not undo it.

### R5 - Crew visits

1. WHEN a caller signs in to a read-only crew account, THE engine SHALL run
   the `crew_login` rules for that handle.
2. THE `set=` effect of such a rule SHALL go to the caller's own account: at
   once, if this connection has signed in to an own account before;
   otherwise the connection SHALL remember the flags and give them to the
   next own account that signs in or signs up on that connection (R8.4 of
   the power prologue).
3. A crew account's record SHALL NOT change.
4. A `mail=` or `post=` effect SHALL need an own account. WHEN the
   connection has none, THE rule SHALL NOT fire and SHALL NOT be marked. The
   author who wants a reply to a crew visit SHALL write a `login` rule that
   has the visit flag.

### R6 - Safety

1. A malformed rule (no section name, no or unknown event, a number out of
   range, text too long) SHALL be skipped, counted and logged. IT SHALL NOT
   stop the board starting or the other rules loading.
2. THE rule table SHALL hold at most 300 rules and cap every text field.
3. A missing `TRIGGERS.INI` SHALL mean no triggers.
4. Timed rules SHALL be tested at most once a second.

### R7 - Transports and platform

1. THE engine SHALL work the same over Telnet and the browser, and SHALL
   stay in the shared core with no ESP32 or Linux headers.
2. THE rule parser and the decision logic SHALL be free of platform types so
   that a plain C++ compiler can test them on the development PC.
3. THE rules SHALL be on the card in `STORY/TRIGGERS.INI`, loaded at boot and
   on RELOAD.
