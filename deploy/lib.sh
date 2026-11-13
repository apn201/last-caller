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
