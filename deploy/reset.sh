#!/usr/bin/env bash
# Put the HALCYON board back to a fresh card, for a demo. Run as root:
#
#   sudo /opt/halcyon/deploy/reset.sh --yes
#
# Stops the service, keeps a tar backup of the card (last five kept), puts a
# fresh card in its place with the deploy settings (uploads off, node limit),
# creates the SYSOP account again with the password in /etc/halcyon/sysop.pass,
# and starts the service. Without --yes it only says what it would do.
# Callers' accounts, posts and progress are gone after a reset.
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=lib.sh
. "$HERE/lib.sh"

YES=0
for a in "$@"; do
  case "$a" in
    --yes) YES=1;;
    -h|--help) sed -n '2,11p' "${BASH_SOURCE[0]}"; exit 0;;
    *) die "unknown option $a";;
  esac
done

need_root
load_env
[ -d "$HAL_OPT/card-src/BBS" ] || die "$HAL_OPT/card-src/BBS is missing: run install.sh again."
[ -d "$HAL_VAR/BBS" ] || die "$HAL_VAR/BBS is missing: run install.sh first."
case "$HAL_VAR" in /var/lib/halcyon) ;; *) die "unexpected card path $HAL_VAR";; esac

STAMP="$(date +%Y%m%d-%H%M%S)"
if [ "$YES" -ne 1 ]; then
  echo "Would stop halcyon-bbs, back up $HAL_VAR/BBS to $HAL_BACKUPS/card-$STAMP.tar.gz,"
  echo "replace the card with a fresh one, create SYSOP again and start the service."
  echo "Run again with --yes to do it."
  exit 0
fi

say "stop"
systemctl stop halcyon-bbs

say "back up"
install -d -m 0700 "$HAL_BACKUPS"
tar -C "$HAL_VAR" -czf "$HAL_BACKUPS/card-$STAMP.tar.gz" BBS
note "$HAL_BACKUPS/card-$STAMP.tar.gz"
# Keep the five newest.
# shellcheck disable=SC2012
ls -1t "$HAL_BACKUPS"/card-*.tar.gz 2>/dev/null | tail -n +6 | while read -r old; do rm -f -- "$old"; done

say "fresh card"
rm -rf "${HAL_VAR:?}/BBS"
make_card "$HAL_VAR"
rm -f "$HAL_SYSOP_DONE"

say "start"
systemctl start halcyon-bbs
wait_for_board || { journalctl -u halcyon-bbs -n 20 --no-pager; die "the board did not answer after the reset"; }
bootstrap_sysop "$HERE"

say "done"
echo "The board is fresh. SysOp login: SYSOP / (see $HAL_SYSOP_PASS)"
