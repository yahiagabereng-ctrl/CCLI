#!/bin/sh
# Args: duration_seconds (default 90)
set -eu
DUR="${1:-90}"

echo "=== PRE snapshot ==="
date -Iseconds 2>/dev/null || date
/usr/sbin/ccli --version 2>/dev/null | head -1 || true
pgrep -a ccli || echo NO_CCLI
netstat -tlnp 2>/dev/null | grep -E '3782|2404' || echo NO_LISTEN

echo "=== LIVE logread -e ccli (${DUR}s) ==="
logread -e ccli -f 2>/dev/null &
LPID=$!
sleep "$DUR"
kill "$LPID" 2>/dev/null || true

echo "=== POST logread (TLS|ACSE|AARE|handshake|auth|3782|Initiate|session|FAILED|closed|mbedtls) ==="
logread -e ccli 2>/dev/null | grep -iE 'TLS|ACSE|AARE|handshake|auth|3782|Initiate|session|FAILED|closed|mbedtls|iso session|EXTERNAL' | tail -120 || true

echo "=== /tmp/ccli.log tail ==="
tail -120 /tmp/ccli.log 2>/dev/null || echo '(no /tmp/ccli.log)'

echo "=== END capture ==="
