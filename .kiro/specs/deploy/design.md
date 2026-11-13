# Design - Deploy package

## Layout on the host

| Path | What |
| --- | --- |
| `/opt/halcyon/bin/tinybbs-server` | the binary |
| `/opt/halcyon/web/` | `shared/web/*` |
| `/opt/halcyon/card-src/BBS/` | pristine card, for reset |
| `/var/lib/halcyon/BBS/` | the live card (`--root /var/lib/halcyon`) |
| `/etc/halcyon/halcyon.env` | domain and ports, read by the scripts |
| `/etc/halcyon/sysop.pass` | the SysOp password, mode 0600, root only |
| `/etc/halcyon/sysop.htpasswd` | basic auth for `/sysop`, group `apache` 0640 |
| `/etc/systemd/system/halcyon-bbs.service` | the unit, user `halcyon` |
| `/etc/httpd/conf.d/halcyon-<domain>.conf` | the vhost (80 first, 80+443 after the certificate) |
| `/var/www/halcyon-acme/` | ACME webroot of this vhost only |
| `/var/backups/halcyon/` | reset backups, last five kept |

All names start `halcyon`. Nothing else on the host is edited.

## Flow of `install.sh`

1. Preflight (R1.2). Nothing is written until all pass.
2. `dnf install` the build and run packages (EPEL for `civetweb-devel`).
3. Build in `deploy/build/` of the checkout, run `ctest`.
4. User, folders, binary, web pages, card (first run only), card settings
   (`upload_max=0`, `max_nodes`), unit.
5. Start the service; wait for `/api/board`.
6. `bootstrap-sysop.py`: log in as `admin`, `SOURCE RTG`, `CREW`, create
   `SYSOP`, log in once. Needs Telnet on 127.0.0.1 (the unit has it).
7. Apache: render the HTTP vhost, configtest, reload, `certbot certonly
   --webroot`, render the TLS vhost, configtest, reload.
8. Smoke: `curl` `/api/board` through Apache over HTTPS.

## Why these choices

- Telnet stays on the loopback: the SysOp can use a real terminal over an
  SSH tunnel, and the bootstrap needs it.
- `certonly --webroot` with our own webroot, not `--apache`: certbot's
  Apache plugin edits vhosts; we write ours ourselves and nobody else's.
- No per-address rate limit in Apache: it needs `mod_evasive` or `mod_qos`,
  and mod_evasive settings would also apply to the other board. The limits
  that exist are per board (nodes, systemd caps) and per request.
- The chat cap is in the shared core (`CHAT_LINE_MAX` in `src/bbs/chat.h`),
  so the stick gets it too.
