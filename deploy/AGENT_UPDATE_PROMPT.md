# Agent prompt: update the public HALCYON board

Copy everything below the line into the agent (OpenClaw / ApnBot, root on
the helppox.com host). For a first install use `AGENT_PROMPT.md` instead.

---

Update the HALCYON BBS at `halcyon.helppox.com` to the newest commit and give
it a fresh card. The board already runs there (`halcyon-bbs` service, files
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

Steps:

1. Fresh clone of the staging repository on this host:
   ```
   cd /root && rm -rf halcyon-new
   git config --global --add safe.directory /home/lastcaller/last-caller.git
   git clone /home/lastcaller/last-caller.git halcyon-new && cd halcyon-new
   git log --oneline -1
   ```
   Quote the commit. It must be `221a42f` or newer.
2. Find the options of the first install (the ports in
   `/etc/systemd/system/halcyon-bbs.service` and the Apache file for
   halcyon) and use the same ones. Preflight:
   `deploy/install.sh --domain halcyon.helppox.com <same port options> --check`
   must say `PREFLIGHT OK`.
3. Run the same command without `--check`. It builds the server and runs the
   tests (all must pass), then updates the binary and pages. It never touches
   the card.
4. Fresh card (the game changed; this wipes callers' progress and keeps a
   backup): `/opt/halcyon/deploy/reset.sh --yes`. Quote the backup file name
   it prints.
5. When everything is done, move the new checkout into place:
   `cd /root && rm -rf halcyon-old && mv halcyon halcyon-old 2>/dev/null; mv halcyon-new halcyon`.
6. Verify, and quote each result:
   - `systemctl is-active halcyon-bbs` gives `active`, and
     `systemctl show halcyon-bbs -p ActiveEnterTimestamp` is from today.
   - `curl -s https://halcyon.helppox.com/api/board` contains `HALCYON` and
     the build tag with the commit from step 1.
   - `curl -sI https://halcyon.helppox.com/demo/DEMO.JS` gives `200`.
   - `grep -c caps_warning /opt/halcyon/card/BBS/STORY/POWER.INI` (or where
     the card lives) is 1 or more, and `ls /opt/halcyon/card/BBS/STORY/DEMO.INI`
     exists: the card is the new one.
   - The old card is in the backup from step 4.

Report: the commit, the test summary line (`N tests passed`), the backup file,
and the verify results.
