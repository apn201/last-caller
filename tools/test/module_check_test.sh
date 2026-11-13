#!/usr/bin/env bash
# Runs the install script's Apache module check 50 times against a fake httpd
# that prints a long module list, with the same shell options as install.sh
# (set -euo pipefail). Needs no root. Also checks that a missing module is
# named, and the certbot lock wait. Usage: tools/test/module_check_test.sh
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
LIB="$HERE/../../deploy/lib.sh"
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

# A fake httpd: a long list (far more than a pipe buffer), as the real one prints.
cat > "$TMP/httpd" <<'EOF'
#!/usr/bin/env bash
echo "Loaded Modules:"
for i in $(seq 1 3000); do echo " fake${i}_module (shared)"; done
for m in proxy_module proxy_http_module proxy_wstunnel_module rewrite_module headers_module \
         auth_basic_module authn_file_module authz_user_module reqtimeout_module ssl_module; do
  echo " $m (shared)"
done
for i in $(seq 1 3000); do echo " zz${i}_module (shared)"; done
EOF
chmod +x "$TMP/httpd"

# shellcheck source=../../deploy/lib.sh
. "$LIB"
export HAL_HTTPD="$TMP/httpd"
MODS="proxy_module proxy_http_module proxy_wstunnel_module rewrite_module headers_module auth_basic_module authn_file_module authz_user_module reqtimeout_module ssl_module"

fail=0
for i in $(seq 1 50); do
  # shellcheck disable=SC2086
  if ! out="$(apache_modules_missing $MODS)"; then echo "run $i: false alarm: $out"; fail=$((fail + 1)); fi
done
echo "module check: 50 runs, $fail failures"

if out="$(apache_modules_missing proxy_module nosuch_module)"; then echo "missing module not reported"; fail=$((fail + 1))
elif [ "$out" != "nosuch_module" ]; then echo "wrong name reported: $out"; fail=$((fail + 1))
else echo "missing module named: ok"; fi

# certbot lock: a fake certbot that is locked twice, then works. No sleeping.
cat > "$TMP/certbot" <<EOF
#!/usr/bin/env bash
n=\$(cat "$TMP/n" 2>/dev/null || echo 0); echo \$((n + 1)) > "$TMP/n"
if [ "\$n" -lt 2 ]; then echo "Another instance of Certbot is already running."; exit 1; fi
echo "Successfully received certificate."
EOF
chmod +x "$TMP/certbot"
sleep() { :; }
PATH="$TMP:$PATH"
if run_certbot certonly >/dev/null; then echo "certbot lock wait: ok ($(cat "$TMP/n") calls)"; else echo "certbot lock wait: failed"; fail=$((fail + 1)); fi
rm -f "$TMP/n"
cat > "$TMP/certbot" <<'EOF'
#!/usr/bin/env bash
echo "Some challenge failed."; exit 1
EOF
if run_certbot certonly >/dev/null; then echo "other certbot error passed"; fail=$((fail + 1)); else echo "other certbot error stops: ok"; fi

[ "$fail" -eq 0 ] && echo "PASS" || { echo "FAIL ($fail)"; exit 1; }
