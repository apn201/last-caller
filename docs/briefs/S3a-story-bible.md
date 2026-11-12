# Brief S3a - Story bible spec (no code)

From: Claude Code (planner). To: Kiro (builder). Turn
`docs/STORY_BIBLE_DRAFT.md` (Juha's story, draft 2) into the spec
`.kiro/specs/story-bible/`. This spec has no code. Its design document IS
the story bible; its tasks are the content and engine work that follow.

## Do

1. `requirements.md` (EARS). At least:
   - Every crew member (Halvorsen, Achterberg, Tanabe, Vance) has three
     manifest facts: identity (handle to person and role), fate (how they
     died), cause (who or what is responsible). ELIZA has a fifth row.
   - Each fact SHALL be provable from at least two pieces of evidence in at
     least two different media (message, file, file date or DIZ, ANSI art,
     telemetry or log, WHO or chat, door). No fact SHALL be provable from a
     single piece.
   - No single piece of evidence SHALL be mandatory for finishing.
   - The manifest SHALL confirm rows two at a time, never naming a wrong
     row. ELIZA's row SHALL open only after all four crew rows are
     confirmed.
   - The transmit ending SHALL require a complete manifest and the final
     command; the other endings as in the draft.
   - Caller-visible text SHALL follow the satire rules (archetypes, no
     real names, no quoted film lines, real accidents never played for
     laughs) and the `story-voice.md` steering.
   - A first-time caller SHALL get a clear lead within two minutes and a
     visible unlock within ten.
2. `design.md`: the bible, cleaned up from the draft (premise, crew, the
   truth, the twist, the secret, ELIZA, timeline, endings, tone). Then two
   tables that the draft does not have yet:
   - **Evidence matrix**: one row per evidence item: id (E01...), medium,
     where on the board (conference, file area, bulletin, door, WHO),
     which facts it supports, what gates it (flag, level, password), and
     which puzzle reveals it. Aim for 24 to 32 items.
   - **Unlock chain**: guest -> new caller -> `TYVANCE` login -> crew area
     -> Achterberg's trail -> SysOp/ELIZA area -> NOWHERE file system ->
     transmit. Each step names the puzzle and the evidence that hints it.
   Check the matrix against the requirements and write the check as a
   short table (fact, evidence ids, media count).
3. `tasks.md`: two groups.
   - Content (card only, for slice S3): accounts and user list, message
     bases and posts per conference, bulletins, file areas with files and
     DIZ, art per screen (with `tools/ansi/`), the board epoch.
   - Engine needs (slices S4 to S8 in `docs/SLICES.md`): what the story
     needs that the engine does not do yet (keyword door, triggers,
     ghosts, manifest, endings, diagnostics door, shell door). One line
     each, mapped to a slice. Do not design the engine here.
   Ty is the ANSI artist (see "Ty the ANSI artist" in the draft): his
   art is a whole evidence medium. Give it several matrix rows, and add a
   content task to change the art tag on every screen from
   `·halcyon·crew·98·` to Ty's tag, and to plan his gallery series.
4. Steering: update `.kiro/steering/story-voice.md` with the satire rules
   from the draft and the names (HALCYON, ELIZA, Above It All Holdings,
   NOWHERE; the Sponsor stays unnamed).
5. Card: set `epoch=1998-11-02 08:00` in `CONFIG/SYSTEM.INI` (BBS online
   date). Keep `clock_sync=browser`.
6. Move `docs/STORY_BIBLE_DRAFT.md` into the spec history: keep the file,
   add a line at the top pointing to `.kiro/specs/story-bible/design.md`
   as the current version.
7. Commit: spec, steering, SYSTEM.INI, the draft and this brief.

## Done when

The evidence matrix covers every fact with at least two items in at least
two media, the unlock chain has no step without a hint, and it is
committed. Reply with the matrix check table and anything in the story
that did not fit the rules.
