# Deploying the hosted board

The hosted HALCYON board is the server build (`server/`, the same C++ core as
the stick) behind Apache. `deploy/` has everything. Spec: `.kiro/specs/deploy/`.
For an agent, `deploy/AGENT_PROMPT.md` is the one prompt to follow.

Target: a Rocky Linux 9 host with root, DNS for a subdomain (for example
`halcyon.helppox.com`) pointing at it, ports 80 and 443 open. Other sites on
the host may stay as they are.

## One command

```sh
git clone <repository> halcyon && cd halcyon
sudo deploy/install.sh --domain halcyon.helppox.com --email you@example.org
```

`sudo deploy/install.sh --domain NAME --check` runs the preflight alone and
changes nothing. The script prints the SysOp and `/sysop` passwords once; they
are also in `/etc/halcyon/` (root only).

Options: `--http-port` (8087), `--telnet-port` (2327), `--max-nodes` (16, up
to 32), `--sysop-password`, `--skip-dns-check` (NAT), `--no-tls`.

## What it does, in order

1. Preflight: Rocky 9, root, the domain resolves to an address of this host,
   both ports free, no Apache file or unit of ours already there, no other
   Apache file naming the domain. Any failure stops it before a change.
2. `dnf install` build tools, `civetweb-devel`, `mbedtls-devel` (EPEL),
   `httpd`, `mod_ssl`, `certbot`. It checks that the Apache modules it needs
   are loaded and does not change the module set.
3. Builds the server and runs `ctest` (logic tests, property tests, story
   lint). A failing test stops the install.
4. Installs the binary, web pages and a pristine card copy under `/opt/halcyon`,
   the live card under `/var/lib/halcyon/BBS` (first run only), the unit
   `halcyon-bbs.service` and `/etc/halcyon/`.
5. Starts the board and creates the `SYSOP` account through the loopback
   Telnet port before the board is public. The first account named SYSOP
   becomes the level-255 SysOp, so this closes the door on anyone else.
6. Writes `/etc/httpd/conf.d/halcyon-<domain>.conf` (HTTP first), runs
   `apachectl configtest`, reloads Apache, gets the certificate with
   `certbot certonly --webroot` (its own webroot `/var/www/halcyon-acme`),
   writes the HTTPS vhost, tests, reloads.
7. Checks `/api/board` through Apache and that `/sysop` asks for a login.

Running it again updates the binary, pages and Apache file. It never touches
the card or the passwords.

## Layout

| Path | What |
| --- | --- |
| `/opt/halcyon/bin/tinybbs-server`, `web/`, `card-src/`, `deploy/` | program, pages, pristine card, reset script |
| `/var/lib/halcyon/BBS` | the live card |
| `/etc/halcyon/` | `halcyon.env`, `sysop.pass`, `sysop-web.pass`, `sysop.htpasswd` |
| `/etc/systemd/system/halcyon-bbs.service` | the unit |
| `/etc/httpd/conf.d/halcyon-<domain>.conf` | the vhost |
| `/var/log/httpd/halcyon-*.log` | Apache logs; the board's own log is `BBS/LOGS/` and `journalctl -u halcyon-bbs` |
| `/var/backups/halcyon/` | reset backups (five kept) |

## Beside the existing board

Everything new is named `halcyon`. The script does not edit, reload-with-changes
or remove any other Apache file, unit, certificate, port or the shared web
root. It does reload Apache (graceful) after its own file passes `configtest`;
if the test fails it removes its file and does not reload. The board listens on
its own `127.0.0.1` port (8087; the existing board uses 8080).

## Hardening

- Uploads off: `upload_max=0` on the installed card, and Apache refuses
  request bodies over 4 KB. The SysOp loads files by `scp` into the card, not
  through the browser.
- `/sysop` (the SysOp file manager) needs Apache basic auth (user `sysop`)
  over TLS, then its own one-use link from the SysOp menu.
- The service runs as user `halcyon`, writes only `/var/lib/halcyon`, has
  `MemoryMax=256M`, `TasksMax=64`, `CPUQuota=50%` and the usual systemd
  sandbox. It listens on `127.0.0.1` only, HTTP and Telnet both.
- Limits: `max_nodes` callers at once (the node pool is the rate limit that
  matters), 4 KB request body, 40 headers, header and body read timeouts, a
  1 KB WebSocket frame, and a 240-character chat line (`CHAT_LINE_MAX` in
  `src/bbs/chat.h`, shared with the stick).
- TLS 1.2 and up, HTTP redirects to HTTPS, no HSTS (the parent domain's other
  sites are not ours to pin).
- Not done: a per-address request limit. Apache has none without `mod_evasive`
  or `mod_qos`, and those settings would reach the other board. `fail2ban` or
  an `nftables` rule on the host is the next step if the board is flooded.
  The script also changes no firewall rule.

## SysOp

- Browser: log in as `SYSOP` at the board's address.
- Terminal: `ssh -L 2327:127.0.0.1:2327 host`, then `telnet 127.0.0.1 2327`.
- File manager: `https://<domain>/sysop?t=...` from the SysOp menu.

## Demo reset

```sh
sudo /opt/halcyon/deploy/reset.sh          # says what it would do
sudo /opt/halcyon/deploy/reset.sh --yes    # does it
```

It stops the board, tars the card to `/var/backups/halcyon/`, puts back a fresh
card with the deploy settings, creates `SYSOP` again and starts the board.
Every caller's account, post and progress is gone. For a few seconds between
start and the SysOp being made the board answers to anyone; reset when nobody
is expected.

## Updating the story or the code

`git pull`, then `sudo deploy/install.sh --domain <same> ...` again. The live
card is not replaced; to take new card content use the reset (it loses callers)
or copy the changed files from `/opt/halcyon/card-src/BBS` by hand, never into
`DATA`, `LOGS` or `TEMP`.

## What was and was not tested

Tested on the staging host (Rocky 9.8): `bash -n` on the scripts, both Apache
templates with `httpd -t` against a throwaway config (not installed), the
SysOp bootstrap against a disposable card (second run reports the handle
taken), the card settings function, the full staging check. `shellcheck` was
not available. Not run: `install.sh` itself, `reset.sh`, the systemd unit under
a real start and certbot; the staging account has no root. The first real run
is on the target host, and its output is the test.
