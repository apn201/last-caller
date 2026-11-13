# Agent prompt: deploy HALCYON

Copy everything below the line into the agent. Fill the three values in angle
brackets first.

---

You are deploying the HALCYON BBS (repository `<REPOSITORY URL>`) on a Rocky
Linux 9 host at `<SUBDOMAIN>.helppox.com`. You have root or sudo on the host.
Another board already runs there. Do not touch it.

Read `docs/DEPLOY.md` and `deploy/install.sh` before running anything.

Rules, in order of importance:

1. Never edit, stop, restart, remove or reconfigure anything that is not named
   `halcyon`: no other Apache file, systemd unit, certificate, port, user,
   firewall rule or folder under `/var/www` other than `/var/www/halcyon-acme`.
   Ports 8080 and 8086 belong to others. Do not run `certbot --apache`,
   `certbot renew` or `dnf update`.
2. If a step fails, stop and report it with the output. Do not work around a
   failed preflight, a failed `configtest` or a failed test by editing other
   files, skipping tests or using `--skip-dns-check` unless the host is
   behind NAT and you say so.
3. Do not print the passwords anywhere except in your final report to me,
   once. Do not put them in a repository or a log.

Steps:

1. `git clone <REPOSITORY URL> halcyon && cd halcyon`
2. Check DNS: `getent hosts <SUBDOMAIN>.helppox.com` must give an address of
   this host (`ip -4 addr`). If it does not, stop and tell me which record to add.
3. `sudo deploy/install.sh --domain <SUBDOMAIN>.helppox.com --email <EMAIL> --check`.
   It must say `PREFLIGHT OK`. If a port is taken, pick free ones with
   `--http-port` and `--telnet-port` (check with `ss -ltn`) and run the check again.
4. Run the same command without `--check` (same options). It takes a few
   minutes: it builds the server and runs the tests.
5. Verify yourself, with these commands, and quote the results:
   - `systemctl is-active halcyon-bbs` gives `active`.
   - `curl -s https://<SUBDOMAIN>.helppox.com/api/board` contains `HALCYON`.
   - `curl -s -o /dev/null -w '%{http_code}' https://<SUBDOMAIN>.helppox.com/sysop` gives `401`.
   - `curl -sI http://<SUBDOMAIN>.helppox.com/` redirects to https.
   - The other board still answers as before: compare
     `curl -s -o /dev/null -w '%{http_code}' http://127.0.0.1:8080/api/board`
     before step 4 and after it.
   - `sudo apachectl configtest` says `Syntax OK`.
   - `sudo ss -ltn | grep -E '8087|2327'` shows `127.0.0.1` only.
6. Play the first screen through the browser address or Telnet over an SSH
   tunnel: the front page is a brownout banner and a `Login:` prompt. Do not
   create accounts and do not change the card.
7. Report: the URL, what you ran, each check with its result, the SysOp and
   `/sysop` passwords (once), what you did not do (no firewall change, no
   per-address rate limit), and anything odd in the output.

To reset the board for a demo later: `sudo /opt/halcyon/deploy/reset.sh --yes`
(deletes all callers' accounts and progress; keeps a backup).
