# Requirements - Easter eggs and nerd stuff

Queue item Q11 in `docs/briefs/QUEUE.md`. Source: `docs/STORY_EXTRAS.md`
("Easter eggs and nerd stuff"). Voice: `.kiro/steering/writing.md`. Every
caller-visible line is a draft for the author's final edit and lives in card
files, not in code.

## Glossary

- Egg: something the board does that no menu offers and no gate needs.
- Modem command: a line a caller types that a 1998 modem would understand
  (`AT...`, `+++`).
- Hidden command: a main-menu word that is in no menu and no help text.
- Node: a number the caller can dial with `ATDT`.

## Requirements

### R1 - Nothing in the main game depends on an egg

1. NO egg SHALL set a flag, start a trigger, grant evidence, open a gate,
   stop or extend the power timer, or change the manifest. The only flag an
   egg may leave is the once-only marker of a scene (`SC_TY_DEMO`) and the
   trigger's own `T_` flag, and no rule, scene, gate or ending SHALL test
   either.
2. THE end-to-end test SHALL still finish the whole game with and without
   the eggs being used.
3. A caller who never types an egg SHALL see no difference except one more
   door in the door menu.

### R2 - Modem commands

1. WHILE the caller is at the login prompt, the main menu prompt, the door
   menu prompt or a "Press ENTER" prompt, THE board SHALL understand:
   `AT` and the setup commands of a modem (`ATZ`, `ATE0`, `ATM0`, `AT&F`, ...)
   answer `OK`; `+++` answers `OK`; `ATI` and `ATI0` to `ATI9` print the
   modem's identity; `ATH`, `ATH0` print `NO CARRIER` and hang up; `ATD`
   with a number dials; any other `AT...` answers `ERROR`.
2. A line that does not start with `AT` or is not `+++` SHALL be handled as
   before. Case is ignored.
3. WHEN a caller dials a number listed on the card, THE board SHALL print
   `DIALING`, `CONNECT 2400`, the node's screen and then `NO CARRIER`, and
   leave the caller at the prompt where they were. A number that is not
   listed answers `BUSY` or `NO ANSWER`; no digits answers `NO DIALTONE`.
4. WHEN a browser caller dials, THE board SHALL send the cue `MODEM`
   (`MODEM.WAV`, the distant modem) once. Telnet callers get no cue.
5. THE numbers, the identity strings and the node screens SHALL be in
   `STORY/EGGS.INI` and the art files. A card without the file understands
   `OK`, `NO CARRIER` and nothing else.
6. THE number of the hidden node SHALL be written in a file a caller can
   open (`NODELIST.TXT`), not only in `EGGS.INI`.

### R3 - Hidden commands

1. AT the main menu prompt, `XYZZY` SHALL answer with a short line,
   `FORTUNE` and `/FORTUNE` with one of the ship's fortunes, and `POST`
   (and `REBOOT`) with the 486's power-on self test screen.
2. THE POST screen SHALL show a BIOS date, a Turbo switch state and a
   memory test that counts a wrong amount, and then return to the prompt.
3. THESE words SHALL NOT appear in any menu or in the `?` help text.
4. THE fortunes SHALL be in `STORY/EGGS.INI`, chosen at random, and SHALL
   NOT say anything about the crew's fates.
5. WHEN a caller types a famous request to a famous film computer into node
   chat, ELIZA SHALL answer with a dry original line (`STORY/SYSOP.INI`).
   The board never quotes the film back.

### R4 - Files

1. THE card SHALL hold, as ordinary files in ordinary file areas: a Y2K
   compliance memo (Records), the hotel brochure (Library), the NOWHERE
   price list (Records), a FidoNet-style nodelist fragment (Library).
2. EACH file SHALL be in its writer's voice: the Sponsor's memos in capitals
   with 1998 buzzwords, the brochure in the same register, the nodelist dry.
3. NO file SHALL be the only evidence for a fact the manifest asks for.

### R5 - Ty's demo

1. THE card SHALL hold `TYDEMO.NFO` in Ty's Desktop. WHEN a caller opens it,
   THE scene engine SHALL play `TY_DEMO`: ANSI frames, a scroller with greets
   to invented groups, a music line, and a crash at the same point every time.
2. THE demo SHALL be skippable like any scene (Enter) and SHALL end at the
   main menu.
3. THE demo is unfinished: it SHALL NOT resolve and SHALL NOT explain itself.

### R6 - Ty's snake door

1. THE door menu SHALL list a snake game for any caller who has power and is
   not the maintenance login. It saves nothing and works for read-only
   accounts.
2. THE game SHALL be turn based so it works over a line terminal: the caller
   types `W`, `A`, `S`, `D` (several at once are played in order) and the
   board draws the station after each line. `Q` leaves.
3. THE station SHALL have walls and a few hull blocks. Hitting a wall, a
   block or the snake ends the game.
4. THE game over screen SHALL show a high score table of the crew, from
   `STORY/EGGS.INI`, with the caller's score placed in it. The caller's score
   is not saved.
5. THE game logic SHALL be a pure header with a host test.

### R7 - Art

1. EVERY new screen (POST, snake, the demo frames, the hidden node) SHALL
   have ANSI art made with `tools/ansi/`, rendered to PNG and looked at, and
   an ASCII twin for callers without ANSI.
2. THE door menu art SHALL keep the new option in its empty slots.

### R8 - Checks

1. HOST tests SHALL cover the modem command parser and the snake rules.
2. THE end-to-end test SHALL check at least: `ATI`, `+++`, an unknown `AT`
   command, a dial to the listed node and to an unlisted number, `XYZZY`,
   `FORTUNE`, `POST`, ELIZA's answer in chat, a file from R4, the demo
   scene, a game of snake, and `ATH0` hanging up.
3. `pio run` and the staging check SHALL pass.
