# OpenClaw prompt: deploy HALCYON to halcyon.helppox.com

From Claude Code, 13 Nov 2026. This is `deploy/AGENT_PROMPT.md` with the values
filled in. There is no GitHub repository yet, so the source is the staging repo
that already lives on helppox (`lastcaller@helppox.com:last-caller.git`).

Before pasting:
- Wait until Kiro has finished Q12e and pushed it to `staging` (Q12e's staging
  check pushes). Today `staging/main` is `9b468ee`, which has the deploy
  scripts but not `docs/DEPLOY.md`. Check with `git ls-remote staging`.
- Add an A record `halcyon.helppox.com` pointing at the helppox host, if it
  is not there yet.
- No email is given to Let's Encrypt (the script registers without one). Add
  `--email ...` in steps 3 and 4 yourself if you want expiry mails.

Paste everything below the line.

---

You are deploying the HALCYON BBS on the Rocky Linux 9 host helppox.com, at
`halcyon.helppox.com`. You have root or sudo on the host. Another board
(Kouvosto) already runs there, and a staging copy of HALCYON runs as user
`lastcaller` on 127.0.0.1:8086. Do not touch either.

The source is the bare git repository on this host:
`/home/lastcaller/last-caller.git` (branch `main`). Do not push to it, do not
edit anything under `/home/lastcaller`.

Rules, in order of importance:

1. Never edit, stop, restart, remove or reconfigure anything that is not named
   `halcyon`: no other Apache file, systemd unit, certificate, port, user,
   firewall rule or folder under `/var/www` other than `/var/www/halcyon-acme`.
   Ports 8080 and 8086 belong to others. Do not run `certbot --apache`,
   `certbot renew` or `dnf update`. Leave Kouvosto's files, service, vhost and
   data alone.
2. If a step fails, stop and report it with the output. Do not work around a
   failed preflight, a failed `configtest` or a failed test by editing other
   files, skipping tests or using `--skip-dns-check`.
3. Do not print the passwords anywhere except in your final report to me,
   once. Do not put them in a repository or a log.

Steps:

1. Clone as root into root's home:
   `sudo git -c safe.directory=/home/lastcaller/last-caller.git clone /home/lastcaller/last-caller.git /root/halcyon`
   then `cd /root/halcyon && git log --oneline -1` and quote the commit.
   If root cannot read the repository, stop and tell me; do not change its
   permissions.
2. Read `docs/DEPLOY.md` and `deploy/install.sh` before running anything.
3. Check DNS: `getent hosts halcyon.helppox.com` must give an address of this
   host (`ip -4 addr`). If it does not, stop and tell me which record to add.
4. Record the other board's state for later:
   `curl -s -o /dev/null -w '%{http_code}' http://127.0.0.1:8080/api/board`
   and `sudo apachectl -S 2>&1 | head -40`.
5. `sudo deploy/install.sh --domain halcyon.helppox.com --check`.
   It must say `PREFLIGHT OK`. If port 8087 or 2327 is taken, pick free ones
   with `--http-port` and `--telnet-port` (check with `ss -ltn`) and run the
   check again.
6. Run the same command without `--check` (same options). It takes a few
   minutes: it builds the server and runs the tests.
7. Verify, and quote each result:
   - `systemctl is-active halcyon-bbs` gives `active`.
   - `curl -s https://halcyon.helppox.com/api/board` contains `HALCYON`.
   - `curl -s -o /dev/null -w '%{http_code}' https://halcyon.helppox.com/sysop` gives `401`.
   - `curl -sI http://halcyon.helppox.com/` redirects to https.
   - The other board answers as in step 4 (same code from 8080, its vhost
     still listed in `apachectl -S`).
   - The staging copy still answers: `curl -s -o /dev/null -w '%{http_code}' http://127.0.0.1:8086/api/board`.
   - `sudo apachectl configtest` says `Syntax OK`.
   - `sudo ss -ltn | grep -E '8087|2327'` shows `127.0.0.1` only.
8. Open `https://halcyon.helppox.com/` (or Telnet over an SSH tunnel): the
   front page is a brownout banner and a `Login:` prompt. Do not create
   accounts and do not change the card.
9. Report: the URL, the commit, what you ran, each check with its result, the
   SysOp and `/sysop` passwords (once), what you did not do (no firewall
   change, no per-address rate limit), and anything odd in the output.

Later updates: `cd /root/halcyon && sudo git -c safe.directory=/home/lastcaller/last-caller.git pull`,
then the same `install.sh` command again. It keeps the live card and the
passwords. A demo reset is `sudo /opt/halcyon/deploy/reset.sh --yes` (deletes
every caller's account and progress, keeps a backup); only when I ask.
