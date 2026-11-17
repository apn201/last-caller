#!/usr/bin/env bash
# Bring the HALCYON card's content up to the installed version, KEEPING callers.
# Run as root after install.sh:
#
#   sudo /opt/halcyon/deploy/update-card.sh --yes
#
# Stops the service, keeps a tar backup of the card (last five kept), and replaces
# only the content folders that come from the repository (ANSI, STORY, DEMO, FILES,
# CONFIG, ...) with the ones in /opt/halcyon/card-src. Accounts, posts, progress and
# logs (DATA, LOGS, TEMP, UPLOADS) are not touched. Applies the deploy settings
# (uploads off, node limit) again and starts the service.
# Without --yes it only says what it would do. For a wiped card use reset.sh.
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=lib.sh
. "$HERE/lib.sh"

YES=0
for a in "$@"; do
  case "$a" in
    --yes) YES=1;;
    -h|--help) sed -n '2,13p' "${BASH_SOURCE[0]}"; exit 0;;
    *) die "unknown option $a";;
  esac
done

need_root
load_env
SRC="$HAL_OPT/card-src/BBS"
[ -d "$SRC" ] || die "$SRC is missing: run install.sh again."
[ -d "$HAL_VAR/BBS" ] || die "$HAL_VAR/BBS is missing: run install.sh first."
case "$HAL_VAR" in /var/lib/halcyon) ;; *) die "unexpected card path $HAL_VAR";; esac

# The folders the repository ships. The caller folders must never be among them.
KEEP="DATA LOGS TEMP UPLOADS"
FOLDERS=""
for d in "$SRC"/*/; do
  n="$(basename "$d")"
  case " $KEEP " in *" $n "*) die "card-src has $n, which holds callers' data: refusing.";; esac
  FOLDERS="$FOLDERS $n"
done
[ -n "$FOLDERS" ] || die "no folders in $SRC"

STAMP="$(date +%Y%m%d-%H%M%S)"
if [ "$YES" -ne 1 ]; then
  echo "Would stop halcyon-bbs, back up $HAL_VAR/BBS to $HAL_BACKUPS/card-$STAMP.tar.gz,"
  echo "replace:$FOLDERS"
  echo "keep:   $KEEP (accounts, posts, progress, logs)"
  echo "and start the service. Run again with --yes to do it."
  exit 0
fi

users_before="$(find "$HAL_VAR/BBS/DATA" -type f 2>/dev/null | wc -l)"

say "stop"
systemctl stop halcyon-bbs

say "back up"
install -d -m 0700 "$HAL_BACKUPS"
tar -C "$HAL_VAR" -czf "$HAL_BACKUPS/card-$STAMP.tar.gz" BBS
note "$HAL_BACKUPS/card-$STAMP.tar.gz"
# shellcheck disable=SC2012
ls -1t "$HAL_BACKUPS"/card-*.tar.gz 2>/dev/null | tail -n +6 | while read -r old; do rm -f -- "$old"; done

say "content"
for n in $FOLDERS; do
  rm -rf "${HAL_VAR:?}/BBS/${n:?}"
  cp -r "$SRC/$n" "$HAL_VAR/BBS/$n"
  note "$n"
done
for d in FILES/001_BBS_SOFTWARE UPLOADS/INCOMING DATA/INDEX; do mkdir -p "$HAL_VAR/BBS/$d"; done
apply_card_settings "$HAL_VAR/BBS"
chown -R halcyon:halcyon "$HAL_VAR"
chmod -R u=rwX,g=rX,o= "$HAL_VAR"

users_after="$(find "$HAL_VAR/BBS/DATA" -type f 2>/dev/null | wc -l)"
[ "$users_after" -ge "$users_before" ] || die "DATA lost files ($users_before -> $users_after). Restore $HAL_BACKUPS/card-$STAMP.tar.gz."
note "DATA kept: $users_after files"

say "start"
systemctl start halcyon-bbs
wait_for_board || { journalctl -u halcyon-bbs -n 20 --no-pager; die "the board did not answer after the update"; }

say "done"
echo "Card content updated; accounts and progress kept. Backup: $HAL_BACKUPS/card-$STAMP.tar.gz"
