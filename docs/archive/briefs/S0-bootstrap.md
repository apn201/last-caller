> Archived in Q15: done brief. The work is recorded in `.kiro/specs/` and `docs/briefs/QUEUE.md`.

# Brief S0 - Kiro bootstrap

From: Claude Code (planner). To: Kiro (builder). Kiro does the work and
commits it. Do not touch firmware code in this step.

## Read first

`AGENTS.md`, `docs/SLICES.md` (the scope and story decisions),
`docs/KIROWEEN_PLAN.md` sections 2 and 6, `docs/ROADMAP.md` ground rules.
For format, look at the sibling project's `.kiro/` in
`../Puhelu/.kiro/` (steering with `inclusion: always` front matter, hooks as
JSON with `version`, `hooks`, `trigger`, `matcher`, `action`). Copy the
format, not the content.

## Do

1. `.kiro/steering/`, five short files (each under ~60 lines):
   - `product.md` - Last Caller: a BBS aboard a ship or station whose crew
     is long dead; the SysOp is the ship's AI that took over; the caller
     works out each crew member's fate from messages, files, logs and
     ghosts in WHO, and fills in a crew manifest. Two front ends, one core:
     the ESP32-S3 stick (offline WiFi AP) and the hosted server build.
     Built for a judge who has five minutes.
     Premise: in 1998 a secret, privately funded spacecraft flew the first
     private science mission, crew of four, preparing for a space hotel and
     tourist flights. The ship carried a BBS on a 486 so the crew could
     message friends and family on Earth, and a mission AI (working name
     ELIZA) that ran the mission, ran the BBS as SysOp, and kept the crew
     company. Mission control lost all contact; it never reached the news.
     28 years later the ship's shorted batteries have gone open circuit, so
     the solar panels power the 486 whenever the ship is in sunlight (a
     "zombie satellite"). Radio amateurs found its signal on 11 Nov 2026.
     The caller is dialling into that BBS. The funder is unnamed and never
     resembles a real person.
   - `tech.md` - from AGENTS.md: one C++ core, `src/platform/platform.h` is
     the only seam, no ESP32 or Linux headers in `src/core`, `src/bbs`,
     `src/doors`. Both builds must pass after a code change (`pio run`;
     the CMake server build in `server/`, which needs Linux and
     civetweb, so on this Windows PC say so instead of claiming it passed).
     Edit `shared/web/*.html`, never the generated `src/net/*.h` pages.
   - `structure.md` - the folder map from AGENTS.md, plus `sd/BBS/STORY/`
     (planned story tables) and `.kiro/`.
   - `card-content.md` - content and settings live on the card, not in
     firmware; never show a menu option that does not work for that caller
     on that transport; never expose SD paths; do not hand-edit
     `/BBS/DATA`, `/BBS/LOGS`, `/BBS/TEMP`.
   - `story-voice.md` - fiction only: invented names, ship, places; no real
     people, companies, brands, games or films named in caller-visible
     text. The AI is calm, polite, helpful and wrong. The board never
     explains a scare. Short plain sentences, 1990s BBS register. All art,
     audio and text original.
2. `.kiro/hooks/embed-web.json`: on save of `shared/web/*.html`, run
   `python tools/embed_web.py`.
3. `.kiro/hooks/README.md`: one line per hook, and the planned hooks from
   `docs/SLICES.md` S11 listed as "planned".
4. Check `.gitignore` has nothing that ignores `.kiro`.
5. Run `python tools/embed_web.py` once to prove the hook command works.
6. Commit with git: `.kiro/`, `docs/SLICES.md`, `docs/briefs/`. Message:
   `Kiro: steering and first hook (S0)`.

## Done when

`git log` shows the S0 commit, `.kiro/steering/` has five files, and the
embed command ran without errors. Reply with a short list of files made and
anything you were unsure about.
