# Requirements - Deploy package

Queue item Q14 in `docs/briefs/QUEUE.md`. Goal: everything needed to put the
hosted HALCYON board on a fresh Rocky Linux 9 host, at a subdomain of
helppox.com, next to the board that is already there, with one prompt to an
agent. Nothing here is caller-visible text, except the chat line cap.

## Requirements

### R1 - One install script

1. WHEN `deploy/install.sh --domain D` runs as root from a checkout, THE script
   SHALL build the server, install it, install a fresh card, start the service
   and put Apache in front of it, and print a summary.
2. THE script SHALL check before it changes anything: Rocky Linux 9, root, the
   domain resolves to this host (a warning only with `--skip-dns-check`), the
   HTTP and Telnet ports are free, no Apache file or systemd unit of that name
   exists. IF a check fails, THEN it SHALL stop and change nothing.
3. THE script SHALL be safe to run twice: a second run updates the binary,
   the web pages and the Apache file, and SHALL NOT touch the card.

### R2 - Next to the existing board

1. THE script SHALL create only files named `halcyon*` (unit, Apache files,
   user, folders, certificate) and SHALL NOT edit or remove any other Apache
   file, unit, certificate, port or the shared web root.
2. THE script SHALL use its own HTTP port (default 8087) and its own ACME
   webroot, and SHALL abort when the port is taken.
3. THE Apache reload SHALL come after `apachectl configtest`. IF the test
   fails, THEN the script SHALL remove its own files and not reload.

### R3 - Hardening

1. Uploads off: `upload_max=0` on the installed card, and Apache refuses
   request bodies over 4 KB on the board's own paths.
2. The SysOp file manager (`/sysop`) is behind Apache basic auth over TLS, in
   addition to its own one-use link.
3. The service runs as its own user with systemd sandboxing, a memory cap and
   a task cap, and listens on 127.0.0.1 only. Telnet listens on 127.0.0.1
   only; callers reach it by SSH tunnel.
4. Rate limits: the node pool (`max_nodes`), request size, header and body
   read timeouts, a one-line cap on chat input (240 characters), the existing
   1 KB WebSocket frame cap.
5. THE script SHALL create the `SYSOP` account before the board is public,
   so that nobody else can take the handle (the first `SYSOP` becomes the
   SysOp).
6. TLS: a Let's Encrypt certificate for the domain only, HTTP redirects to
   HTTPS, HSTS off by default (the parent domain's other sites are not ours).

### R4 - Demo reset

1. WHEN `deploy/reset.sh --yes` runs, THE script SHALL stop the service, keep a
   tar backup of the card, put a fresh card in its place with the deploy
   settings, create the SysOp account again and start the service.
2. WITHOUT `--yes`, THE script SHALL only say what it would do.

### R5 - Documents

1. `docs/DEPLOY.md` explains the layout, the options, the checks and what is
   not done (no per-address rate limit in Apache without an extra module).
2. `deploy/AGENT_PROMPT.md` is one prompt another agent can follow, with the
   stop conditions and the things it must never touch.

### R6 - Verification

1. THE shell scripts SHALL pass `bash -n` and, where available, `shellcheck`.
2. THE Apache templates SHALL be rendered and syntax-checked on the staging
   host without installing them.
3. `pio run` and the staging check SHALL pass (the chat cap is shared code).
4. A full install on a fresh host is NOT claimed unless it was run.
