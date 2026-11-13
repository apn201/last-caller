#!/usr/bin/env bash
# Shared by install.sh and reset.sh. Sourced, not run.
# Everything this package creates is named "halcyon". Nothing else on the host is edited.

HAL_ETC=/etc/halcyon
HAL_OPT=/opt/halcyon
HAL_VAR=/var/lib/halcyon            # --root of the server; the card is $HAL_VAR/BBS
HAL_ACME=/var/www/halcyon-acme
HAL_BACKUPS=/var/backups/halcyon
HAL_ENV=$HAL_ETC/halcyon.env
HAL_SYSOP_PASS=$HAL_ETC/sysop.pass
HAL_WEB_PASS=$HAL_ETC/sysop-web.pass
HAL_HTPASSWD=$HAL_ETC/sysop.htpasswd
HAL_SYSOP_DONE=$HAL_ETC/.sysop-created

say() { printf '\n== %s\n' "$*"; }
note() { printf '   %s\n' "$*"; }
die() { printf '\nSTOP: %s\n' "$*" >&2; exit 1; }

need_root() { [ "$(id -u)" -eq 0 ] || die "run as root (sudo)."; }

# Reads $HAL_ENV (DOMAIN, HTTP_PORT, TELNET_PORT, MAX_NODES).
load_env() {
  [ -r "$HAL_ENV" ] || die "$HAL_ENV not found: run install.sh first."
  # shellcheck disable=SC1090
  . "$HAL_ENV"
}

# A fresh card from the pristine copy, with the deploy settings applied.
# $1 = target folder that will hold BBS/
make_card() {
  local target="$1"
  mkdir -p "$target"
  cp -r "$HAL_OPT/card-src/BBS" "$target/BBS"
  local d
  for d in FILES/001_BBS_SOFTWARE UPLOADS/INCOMING DATA/INDEX; do mkdir -p "$target/BBS/$d"; done
  rm -rf "$target/BBS/DATA/STATE"
  apply_card_settings "$target/BBS"
  chown -R halcyon:halcyon "$target"
  chmod -R u=rwX,g=rX,o= "$target"
}

# Deploy settings on the card's SYSTEM.INI: uploads off, node limit.
apply_card_settings() {
  local ini="$1/CONFIG/SYSTEM.INI"
  [ -f "$ini" ] || die "no $ini"
  sed -i -e 's/^upload_max=.*/upload_max=0/' -e "s/^max_nodes=.*/max_nodes=${MAX_NODES:-16}/" "$ini"
  grep -q '^upload_max=0$' "$ini" || die "could not set upload_max=0 in $ini"
}

# Apache modules the vhosts need. $HAL_HTTPD names the binary (tests put a fake
# one there). The list is captured once and matched in the shell: a pipe into
# "grep -q" makes httpd die of SIGPIPE under pipefail when grep exits early.
# Prints the missing module names, one per line; returns 1 if any is missing.
apache_modules_missing() {
  local httpd="${HAL_HTTPD:-httpd}" list m miss=0
  list="$("$httpd" -M 2>&1)" || { echo "(httpd -M failed)"; return 1; }
  for m in "$@"; do
    case "$list" in *" $m "*) ;; *) echo "$m"; miss=1;; esac
  done
  return "$miss"
}

# certbot certonly with the host's existing lock respected: if another certbot
# holds it, wait and try again; never touch the lock file. Gives up after
# about five minutes with a clear message (the install can simply be run again).
# $@ = arguments for certbot.
run_certbot() {
  local out rc tries=0 max=20
  while :; do
    rc=0
    out="$(certbot "$@" 2>&1)" || rc=$?
    printf '%s\n' "$out"
    [ "$rc" -eq 0 ] && return 0
    case "$out" in
      *"Another instance of Certbot is already running"*|*".certbot.lock"*)
        tries=$((tries + 1))
        if [ "$tries" -ge "$max" ]; then
          echo "certbot is still locked by another process. The lock was not touched."
          return 2
        fi
        echo "certbot is locked by another process: waiting 15 s ($tries/$max)"
        sleep 15;;
      *) return "$rc";;
    esac
  done
}

wait_for_board() {
  local i
  for i in $(seq 1 60); do
    if curl -s -o /dev/null -m 2 "http://127.0.0.1:$HTTP_PORT/api/board"; then return 0; fi
    sleep 0.5
  done
  return 1
}

# Creates the SYSOP account through the loopback Telnet port, before the board is public.
bootstrap_sysop() {
  local here="$1"
  [ -r "$HAL_SYSOP_PASS" ] || die "$HAL_SYSOP_PASS missing"
  SYSOP_PASSWORD="$(cat "$HAL_SYSOP_PASS")" python3 "$here/bootstrap-sysop.py" --port "$TELNET_PORT" \
    || die "could not create the SYSOP account (see above). The board is running but NOT ready to be public."
  : > "$HAL_SYSOP_DONE"
}
