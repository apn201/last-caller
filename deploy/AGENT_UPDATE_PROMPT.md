# Agent prompt: update the public HALCYON board

Copy everything below the line into the agent (OpenClaw / ApnBot, root on
the helppox.com host). For a first install use `AGENT_PROMPT.md` instead.

---

Update the HALCYON BBS at `halcyon.helppox.com` to the newest commit and bring
its card content up to date, **keeping every caller's account, posts and
progress**. The board already runs there (`halcyon-bbs` service, files
in `/opt/halcyon`). Other boards run on this host, Kouvosto among them: do
not touch them.

Rules, in order of importance:

1. Touch nothing that is not named `halcyon`. Do not run `dnf update`,
   `certbot --apache` or `certbot renew`. Kouvosto (`kouvosto.helppox.com`)
   is not part of this update.
2. Build from a **fresh clone**, not an old checkout (the old `/root/halcyon`
   checkout failed the tests last time; a fresh clone passes).
3. If a step fails, stop and report it with the output. Do not skip tests or
   edit files to get past a failure.
4. Do not print passwords except once in your final report.
5. **Never run `reset.sh`** and never delete or edit anything in
   `/var/lib/halcyon/BBS/DATA`, `LOGS`, `TEMP` or `UPLOADS`. Those hold the
   callers' accounts and progress, and the owner's own account is among them.
   The only card step is `update-card.sh` below, which keeps them.

Steps:

1. Fresh clone of the staging repository on this host:
   ```
   cd /root && rm -rf halcyon-new
   git config --global --add safe.directory /home/lastcaller/last-caller.git
   git clone /home/lastcaller/last-caller.git halcyon-new && cd halcyon-new
   git log --oneline -1
   ```
   Quote the commit. It must be `80c20fe` or newer (and include
   `deploy/update-card.sh`).
2. Find the options of the first install (the ports in
   `/etc/systemd/system/halcyon-bbs.service` and the Apache file for
   halcyon) and use the same ones. Preflight:
   `deploy/install.sh --domain halcyon.helppox.com <same port options> --check`
   must say `PREFLIGHT OK`.
3. Run the same command without `--check`. It builds the server and runs the
   tests (all must pass), then updates the binary, pages, the card source in
   `/opt/halcyon/card-src` and the scripts in `/opt/halcyon/deploy`. It never
   touches the live card.
4. Card content, accounts kept. First count the accounts:
   `find /var/lib/halcyon/BBS/DATA -type f | wc -l` (quote it). Then
   `/opt/halcyon/deploy/update-card.sh` (no `--yes`): it lists what it would
   replace and what it keeps; `DATA` must be on the keep line. Then
   `/opt/halcyon/deploy/update-card.sh --yes`. It backs up the card first and
   stops by itself if `DATA` lost files. Quote the backup file name and the
   `DATA kept: N files` line; N must be at least the count above.
5. When everything is done, move the new checkout into place:
   `cd /root && rm -rf halcyon-old && mv halcyon halcyon-old 2>/dev/null; mv halcyon-new halcyon`.
6. Verify, and quote each result:
   - `systemctl is-active halcyon-bbs` gives `active`, and
     `systemctl show halcyon-bbs -p ActiveEnterTimestamp` is from today.
   - `curl -s https://halcyon.helppox.com/api/board` contains `HALCYON` and
     the build tag with the commit from step 1.
   - `curl -sI https://halcyon.helppox.com/demo/DEMO.JS` gives `200`.
   - `curl -sI https://halcyon.helppox.com/demo/SR.JS` gives `200` (the
     easter egg's player is on the card).
   - `ls /var/lib/halcyon/BBS/DATA` still lists the same folders as before,
     and the file count is the same or higher: no account was lost.
   - The card as it was is in the backup from step 4.

Report: the commit, the test summary line (`N tests passed`), the backup file,
the account file counts before and after, and the verify results.
