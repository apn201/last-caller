#!/usr/bin/env bash
# Install the HALCYON board on a Rocky Linux 9 host, behind Apache, next to
# whatever already runs there. Run as root from a checkout of the repository:
#
#   sudo deploy/install.sh --domain halcyon.helppox.com [--email you@example.org]
#
# Options:
#   --domain NAME        required; DNS must already point at this host
#   --email ADDR         for Let's Encrypt (default: register without an email)
#   --http-port N        the board's HTTP port on 127.0.0.1 (default 8087)
#   --telnet-port N      the board's Telnet port on 127.0.0.1 (default 2327)
#   --max-nodes N        callers at once, 1-32 (default 16)
#   --sysop-password P   default: 16 random characters, printed once
#   --skip-dns-check     the domain does not resolve to an address on this host
#                        (NAT, a proxy in front): go on anyway
#   --no-tls             stop after the board runs and the HTTP vhost is in place
#   --check              preflight only: change nothing
#
# Safe to run again: it updates the binary, pages and Apache file, never the card.
# Creates only things named "halcyon". See docs/DEPLOY.md.
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="$(cd "$HERE/.." && pwd)"
# shellcheck source=lib.sh
. "$HERE/lib.sh"

DOMAIN="" EMAIL="" HTTP_PORT=8087 TELNET_PORT=2327 MAX_NODES=16 SYSOP_PW=""
SKIP_DNS=0 NO_TLS=0 CHECK=0
while [ $# -gt 0 ]; do
  case "$1" in
    --domain) DOMAIN="${2:-}"; shift 2;;
    --email) EMAIL="${2:-}"; shift 2;;
    --http-port) HTTP_PORT="${2:-}"; shift 2;;
    --telnet-port) TELNET_PORT="${2:-}"; shift 2;;
    --max-nodes) MAX_NODES="${2:-}"; shift 2;;
    --sysop-password) SYSOP_PW="${2:-}"; shift 2;;
    --skip-dns-check) SKIP_DNS=1; shift;;
    --no-tls) NO_TLS=1; shift;;
    --check) CHECK=1; shift;;
    -h|--help) sed -n '2,22p' "${BASH_SOURCE[0]}"; exit 0;;
    *) die "unknown option $1";;
  esac
done

[[ "$DOMAIN" =~ ^[a-z0-9]([a-z0-9-]*[a-z0-9])?(\.[a-z0-9]([a-z0-9-]*[a-z0-9])?)+$ ]] || die "--domain NAME is required (lower case host name)."
[[ "$HTTP_PORT" =~ ^[0-9]+$ && "$TELNET_PORT" =~ ^[0-9]+$ && "$MAX_NODES" =~ ^[0-9]+$ ]] || die "ports and --max-nodes are numbers."
[ "$MAX_NODES" -ge 1 ] && [ "$MAX_NODES" -le 32 ] || die "--max-nodes is 1 to 32."
[ "$HTTP_PORT" != "$TELNET_PORT" ] || die "the two ports must differ."
[ -z "$SYSOP_PW" ] || [ "${#SYSOP_PW}" -ge 8 ] || die "--sysop-password needs 8 or more characters."
[ "$DOMAIN" != "helppox.com" ] || die "that is the parent domain, not a subdomain for this board."

need_root

# ---------------------------------------------------------------- preflight
say "preflight (nothing is changed yet)"
ERR=0
bad() { printf '   FAIL  %s\n' "$*"; ERR=1; }
ok() { printf '   ok    %s\n' "$*"; }

. /etc/os-release
if [ "${ID:-}" = "rocky" ] && [ "${VERSION_ID%%.*}" = "9" ]; then ok "Rocky Linux $VERSION_ID"; else bad "this is for Rocky Linux 9, found ${PRETTY_NAME:-unknown}"; fi
[ -d "$REPO/server" ] && [ -d "$REPO/sd/BBS" ] && [ -d "$REPO/shared/web" ] && ok "checkout at $REPO" || bad "$REPO is not a full checkout (server/, sd/BBS, shared/web)"
command -v systemctl >/dev/null && ok "systemd" || bad "systemd missing"

OURS=0
if [ -r "$HAL_ENV" ]; then
  # shellcheck disable=SC1090
  OLD_DOMAIN="$(. "$HAL_ENV"; echo "${DOMAIN:-}")"
  OLD_HTTP="$(. "$HAL_ENV"; echo "${HTTP_PORT:-}")"
  OLD_TELNET="$(. "$HAL_ENV"; echo "${TELNET_PORT:-}")"
  if [ "$OLD_DOMAIN" = "$DOMAIN" ] && [ "$OLD_HTTP" = "$HTTP_PORT" ] && [ "$OLD_TELNET" = "$TELNET_PORT" ]; then OURS=1; ok "an earlier install of this board: updating"
  else bad "$HAL_ENV belongs to an install with other settings ($OLD_DOMAIN, ports $OLD_HTTP/$OLD_TELNET)"; fi
fi

port_busy() { ss -ltnH "sport = :$1" 2>/dev/null | grep -q .; }
for p in "$HTTP_PORT" "$TELNET_PORT"; do
  if port_busy "$p"; then
    if [ "$OURS" -eq 1 ] && systemctl is-active --quiet halcyon-bbs; then ok "port $p is held by halcyon-bbs"; else bad "port $p is already in use: choose another"; fi
  else ok "port $p is free"; fi
done

if [ "$OURS" -eq 0 ]; then
  [ ! -e /etc/systemd/system/halcyon-bbs.service ] && ok "no halcyon-bbs unit yet" || bad "halcyon-bbs.service exists but $HAL_ENV does not: not touching it"
  [ ! -e "/etc/httpd/conf.d/halcyon-$DOMAIN.conf" ] && ok "no Apache file yet" || bad "/etc/httpd/conf.d/halcyon-$DOMAIN.conf exists: not touching it"
fi
# Another vhost must not already answer for this name.
OTHER="$(grep -rlsE "^[[:space:]]*Server(Name|Alias)[[:space:]].*\b${DOMAIN//./\\.}\b" /etc/httpd/conf.d /etc/httpd/conf 2>/dev/null | grep -v "/halcyon-" || true)"
[ -z "$OTHER" ] && ok "no other Apache file names $DOMAIN" || bad "another Apache file already names $DOMAIN: $OTHER"

# DNS: the name must point at an address of this host, or certbot cannot succeed.
if [ "$SKIP_DNS" -eq 1 ]; then ok "DNS check skipped by request"
else
  RESOLVED="$(getent ahostsv4 "$DOMAIN" 2>/dev/null | awk '{print $1}' | sort -u | tr '\n' ' ')"
  LOCAL="$(ip -4 -o addr show scope global 2>/dev/null | awk '{print $4}' | cut -d/ -f1 | tr '\n' ' ')"
  if [ -z "$RESOLVED" ]; then bad "$DOMAIN does not resolve yet: add the DNS record first"
  else
    HIT=0; for a in $RESOLVED; do for l in $LOCAL; do [ "$a" = "$l" ] && HIT=1; done; done
    [ "$HIT" -eq 1 ] && ok "$DOMAIN resolves to this host ($RESOLVED)" || bad "$DOMAIN resolves to $RESOLVED, this host has $LOCAL (use --skip-dns-check behind NAT)"
  fi
fi

if [ "$ERR" -ne 0 ]; then die "preflight failed. Nothing was changed."; fi
if [ "$CHECK" -eq 1 ]; then echo; echo "PREFLIGHT OK. Nothing was changed."; exit 0; fi

# ---------------------------------------------------------------- packages
say "packages"
dnf -y -q install epel-release
dnf -y -q install gcc-c++ cmake make git python3 curl openssl civetweb-devel mbedtls-devel \
  httpd mod_ssl httpd-tools certbot policycoreutils
for m in proxy_module proxy_http_module proxy_wstunnel_module rewrite_module headers_module auth_basic_module \
         authn_file_module authz_user_module reqtimeout_module ssl_module; do
  httpd -M 2>/dev/null | grep -q "${m} " || die "Apache module $m is not loaded (see /etc/httpd/conf.modules.d). Not changing the host's module set."
done
note "build tools and Apache modules present"

# ---------------------------------------------------------------- build
say "build"
BUILD=/var/tmp/halcyon-build
rm -rf "$BUILD"
cmake -S "$REPO/server" -B "$BUILD" >/dev/null
cmake --build "$BUILD" -j2 2>&1 | tail -3
[ -x "$BUILD/tinybbs-server" ] || die "the build made no tinybbs-server"
if ctest --test-dir "$BUILD" --output-on-failure 2>&1 | tail -6; [ "${PIPESTATUS[0]}" -eq 0 ]; then note "ctest passed"; else die "ctest failed: not installing a build that fails its tests."; fi

# ---------------------------------------------------------------- files
say "install"
id halcyon >/dev/null 2>&1 || useradd --system --home-dir "$HAL_VAR" --shell /sbin/nologin --comment "HALCYON BBS" halcyon
install -d -m 0755 "$HAL_OPT" "$HAL_OPT/bin" "$HAL_OPT/web"
install -d -m 0750 -o root -g root "$HAL_ETC"
install -m 0755 "$BUILD/tinybbs-server" "$HAL_OPT/bin/tinybbs-server"
rm -rf "$HAL_OPT/web"; install -d -m 0755 "$HAL_OPT/web"; cp -r "$REPO/shared/web/." "$HAL_OPT/web/"
rm -rf "$HAL_OPT/card-src"; install -d -m 0755 "$HAL_OPT/card-src"; cp -r "$REPO/sd/BBS" "$HAL_OPT/card-src/BBS"
install -d -m 0755 "$HAL_OPT/deploy"; cp "$HERE/lib.sh" "$HERE/reset.sh" "$HERE/bootstrap-sysop.py" "$HAL_OPT/deploy/"
chmod 0755 "$HAL_OPT/deploy/reset.sh" "$HAL_OPT/deploy/bootstrap-sysop.py"
rm -rf "$BUILD"

cat > "$HAL_ENV" <<EOF
DOMAIN=$DOMAIN
HTTP_PORT=$HTTP_PORT
TELNET_PORT=$TELNET_PORT
MAX_NODES=$MAX_NODES
EOF
chmod 0640 "$HAL_ENV"

NEW_CARD=0
if [ ! -d "$HAL_VAR/BBS" ]; then
  NEW_CARD=1
  install -d -m 0750 -o halcyon -g halcyon "$HAL_VAR"
  make_card "$HAL_VAR"
  note "fresh card at $HAL_VAR/BBS (uploads off, $MAX_NODES nodes)"
else
  note "card kept as it is"
fi

if [ ! -s "$HAL_SYSOP_PASS" ]; then
  [ -n "$SYSOP_PW" ] || SYSOP_PW="$(openssl rand -hex 8)"
  ( umask 077; printf '%s' "$SYSOP_PW" > "$HAL_SYSOP_PASS" )
  PRINT_SYSOP=1
else PRINT_SYSOP=0; fi
if [ ! -s "$HAL_WEB_PASS" ]; then ( umask 077; openssl rand -hex 8 | tr -d '\n' > "$HAL_WEB_PASS" ); PRINT_WEB=1; else PRINT_WEB=0; fi
htpasswd -B -b -c "$HAL_HTPASSWD" sysop "$(cat "$HAL_WEB_PASS")" >/dev/null 2>&1
chown root:apache "$HAL_HTPASSWD"; chmod 0640 "$HAL_HTPASSWD"

render() { sed -e "s|@DOMAIN@|$DOMAIN|g" -e "s|@HTTP_PORT@|$HTTP_PORT|g" -e "s|@TELNET_PORT@|$TELNET_PORT|g" "$1"; }
render "$HERE/halcyon-bbs.service.in" > /etc/systemd/system/halcyon-bbs.service
systemctl daemon-reload

# ---------------------------------------------------------------- start, SysOp
say "start"
systemctl enable halcyon-bbs >/dev/null 2>&1
systemctl restart halcyon-bbs
wait_for_board || { journalctl -u halcyon-bbs -n 20 --no-pager; die "the board did not answer on 127.0.0.1:$HTTP_PORT"; }
note "board answers on 127.0.0.1:$HTTP_PORT"
if [ ! -e "$HAL_SYSOP_DONE" ]; then
  [ "$NEW_CARD" -eq 1 ] || die "the card exists but no SysOp was ever created by this script. Create SYSOP by hand over the loopback Telnet port $TELNET_PORT, then 'touch $HAL_SYSOP_DONE'."
  bootstrap_sysop "$HERE"
fi

# ---------------------------------------------------------------- Apache
say "apache"
install -d -m 0755 "$HAL_ACME/.well-known/acme-challenge"
command -v restorecon >/dev/null && restorecon -R "$HAL_ACME" || true
CONF="/etc/httpd/conf.d/halcyon-$DOMAIN.conf"
CERT="/etc/letsencrypt/live/$DOMAIN/fullchain.pem"
BACKUP=""
[ -e "$CONF" ] && { BACKUP="$CONF.prev"; cp -p "$CONF" "$BACKUP"; }
put_conf() {  # $1 template; keeps the old file if the syntax test fails
  render "$1" > "$CONF"
  if apachectl configtest >/dev/null 2>&1; then rm -f "$BACKUP"; return 0; fi
  apachectl configtest 2>&1 | tail -5
  if [ -n "$BACKUP" ] && [ -e "$BACKUP" ]; then mv -f "$BACKUP" "$CONF"; else rm -f "$CONF"; fi
  die "Apache refused the new file, so it was removed. Nothing was reloaded."
}
if [ -r "$CERT" ] && [ "$NO_TLS" -eq 0 ]; then
  put_conf "$HERE/apache-tls.conf.in"; systemctl reload httpd; note "vhost with TLS (certificate already there)"
else
  put_conf "$HERE/apache-http.conf.in"; systemctl reload httpd; note "HTTP vhost in place"
  if [ "$NO_TLS" -eq 0 ]; then
    say "certificate"
    MAIL=(--register-unsafely-without-email); [ -z "$EMAIL" ] || MAIL=(--email "$EMAIL")
    certbot certonly --webroot -w "$HAL_ACME" -d "$DOMAIN" --non-interactive --agree-tos "${MAIL[@]}" \
      --deploy-hook "systemctl reload httpd" || die "certbot failed. The board runs on HTTP only (redirects to HTTPS will fail). Fix DNS or ports and run this script again."
    put_conf "$HERE/apache-tls.conf.in"; systemctl reload httpd; note "vhost with TLS"
  fi
fi

# ---------------------------------------------------------------- smoke
say "check"
SCHEME=https; [ -r "$CERT" ] || SCHEME=http
CODE="$(curl -s -m 10 -o /tmp/halcyon-board.json -w '%{http_code}' --resolve "$DOMAIN:443:127.0.0.1" --resolve "$DOMAIN:80:127.0.0.1" "$SCHEME://$DOMAIN/api/board" || true)"
if [ "$CODE" = "200" ] && grep -q HALCYON /tmp/halcyon-board.json; then note "$SCHEME://$DOMAIN/api/board: HTTP 200, HALCYON"; else rm -f /tmp/halcyon-board.json; die "$SCHEME://$DOMAIN/api/board answered HTTP $CODE"; fi
rm -f /tmp/halcyon-board.json
SYS="$(curl -s -m 10 -o /dev/null -w '%{http_code}' --resolve "$DOMAIN:443:127.0.0.1" --resolve "$DOMAIN:80:127.0.0.1" "$SCHEME://$DOMAIN/sysop" || true)"
[ "$SYS" = "401" ] || [ "$SCHEME" = http ] || die "/sysop answered HTTP $SYS without a login (expected 401)"
[ "$SCHEME" = http ] || note "/sysop asks for a login (HTTP 401)"

say "done"
echo "Board:        $SCHEME://$DOMAIN/"
echo "Service:      systemctl status halcyon-bbs   (logs: journalctl -u halcyon-bbs)"
echo "SysOp tunnel: ssh -L $TELNET_PORT:127.0.0.1:$TELNET_PORT <this host>, then telnet 127.0.0.1 $TELNET_PORT"
echo "Reset:        sudo $HAL_OPT/deploy/reset.sh --yes"
[ "$PRINT_SYSOP" -eq 0 ] || echo "SysOp login:  SYSOP / $(cat "$HAL_SYSOP_PASS")   (also in $HAL_SYSOP_PASS, root only)"
[ "$PRINT_WEB" -eq 0 ] || echo "/sysop auth:  sysop / $(cat "$HAL_WEB_PASS")   (also in $HAL_WEB_PASS, root only)"
echo "Not done: no firewall change (80 and 443 are assumed open), no per-address rate limit."
