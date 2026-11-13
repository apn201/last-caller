# Design - Verification

## Lint model

Every grant and gate is a unit `(label, needs[], grants[])`: a trigger rule, a
diagnostics query, the shell's final command, a scene (entered, then its choice
flags), a manifest row (shown, right, confirmed; crew rows in pairs), the full
scene. Engine flags (`T_<RULE>`, `SC_<SCENE>`) are produced by units too, so
nothing special is needed except `SC_ENTER_<SCENE>`, a lint-only flag meaning
"something started this scene". A fixpoint over the units gives the flags a
caller can hold. The worst-order run drops every unit whose `lacks=` flags are in
the first run's result and runs again; the difference, restricted to flags that
something asks for, is the softlock list.

Targets are matched the way the board matches them: a read trigger `~SUBJECT`
is the whole subject, upper case, first 39 characters; file and art triggers are
a file name anywhere under FILES, ANSI, ASCII, ASSETS, BULLETINS or DOORS.

## Property test

C++ on the shared headers `haunt_logic.h` and `manifest_logic.h`. Every distinct
event in the rule table is delivered in 300 random orders, repeating passes until
nothing changes. Flags are compared without `T_` markers (a marker may depend on a
`lacks` rule). The manifest part files right picks and random picks against the
closure.

## bbs-caller

One `Caller` class (socket, Telnet option refusal, ANSI strip, quiet-based read).
Front ends: MCP stdio (JSON-RPC 2.0, newline framed), and `cli` commands that talk
to a `daemon` process on 127.0.0.1:12399 holding the connection. The board is
reached at 127.0.0.1:12323, the local end of `ssh -L` to the staging host's
127.0.0.1:2323.

## Staging

`tools/staging-remote.sh` runs `ctest` after the build. With `SERVE=1` it keeps the
card's own timer, skips the end-to-end run and leaves the server up until
`~/lastcaller-staging/stop` exists. `tools/staging-playtest.ps1` starts it with the
tunnel.

## Kiro formats (checked 13 Nov 2026)

- Hooks: `.kiro/hooks/*.json`, `version: v1`, `trigger`, `matcher`, `action`.
- Agents: `.kiro/agents/*.md` with front matter (`name`, `description`, `tools`,
  `includeMcpJson`, `resources`) and the prompt as the body.
- MCP: `.kiro/settings/mcp.json`, `mcpServers` with `command`, `args`, `env`,
  `disabled`, `autoApprove`.