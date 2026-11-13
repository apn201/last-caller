# Requirements - Verification

Queue item Q12 in `docs/briefs/QUEUE.md`. The game is built; this item proves
it can be finished and keeps it so. Formats checked against the Kiro docs
(hooks v1, custom agents, MCP) on 13 Nov 2026.

## Glossary

- Lint: `tools/story_lint.py`, a check of the card's story files with no board.
- Property test: `tools/test/story_property_test.cpp`, random orders of play over
  the card's own tables, built from the shared logic headers.
- bbs-caller: the MCP server in `tools/mcp/bbs_caller.py` that gives an agent a
  caller's keyboard and screen over Telnet.
- Playtester: the custom agent that plays the board as a first-time caller.

## Requirements

### R1 - Story lint

1. WHEN the lint runs on the card, THE lint SHALL report an error for every
   flag that something asks for and nothing grants.
2. THE lint SHALL simulate play on the flags and report an error for every
   manifest option, row, or ending that no play can reach.
3. THE lint SHALL repeat the simulation with the worst order for rules that
   carry `lacks=` and report an error for every needed flag the worst order
   loses (a softlock).
4. THE lint SHALL report an error for a trigger that names a message subject, a
   file, an art file, a bulletin, a scene or a crew account that the card does
   not have.
5. THE lint SHALL check every scene: a START section, targets that exist, every
   section reachable, every section able to reach END.
6. WHEN an evidence flag that a right answer needs has one source only, THE lint
   SHALL warn (a fate is provable from several pieces of evidence).
7. THE lint SHALL warn on characters and words the writing rules ban.
8. THE lint SHALL use the standard library only and exit non-zero on any error.

### R2 - Property tests in the server build

1. THE server build SHALL build every host test in `tools/test/` and register it
   with CTest, each given the card's own files where it takes any.
2. THE story property test SHALL show, over many random orders of play, that
   flags only grow, a repeated event changes nothing, the end flags do not depend
   on the order, every manifest need is reached, the right picks confirm every
   row (crew rows in pairs, ELIZA's last) and wrong picks never do.
3. THE staging check SHALL run CTest (host tests, property test, lint) and fail
   if it fails.
4. WHERE no server build is possible (the Windows PC), `tools/run_host_tests.py`
   SHALL run the same tests with the compiler found; it SHALL not be reported as
   the server build.

### R3 - bbs-caller

1. THE tool SHALL expose `bbs_connect`, `bbs_send`, `bbs_read`, `bbs_disconnect`
   over MCP stdio, return the text a caller would read (no colour codes, no Telnet
   negotiation), and wait for the board to be quiet before it returns.
2. THE tool SHALL connect to 127.0.0.1 only unless the environment says otherwise;
   the staging board is reached through an SSH tunnel
   (`tools/staging-playtest.ps1`).
3. THE tool SHALL also run from a command line, with the session held by a small
   background process, so an agent without MCP can play.
4. A self-test SHALL drive the MCP server against a fake board with no network.

### R4 - Agents and hooks

1. THE workspace SHALL define the agents `ghostwriter`, `puzzle-smith`,
   `firmware-guard` and `playtester` in `.kiro/agents/`.
2. THE playtester SHALL NOT read the story files; it plays through bbs-caller only.
3. THE workspace SHALL run the lint when a card story file is saved, and the host
   tests when the shared logic or a test is saved.

### R5 - The playtest

1. THE playtester SHALL play the staging board, with the card's own timers and a
   fresh card, to the end of a chosen ending.
2. `docs/PLAYTEST.md` SHALL record the run: finished or not, the route, where the
   player was stuck, what looked broken, and which tool front end played.
3. WHERE the playtest finds a fault in the content or the core, THE fault SHALL be
   listed in `docs/PLAYTEST.md`; it SHALL be fixed only if small and safe.