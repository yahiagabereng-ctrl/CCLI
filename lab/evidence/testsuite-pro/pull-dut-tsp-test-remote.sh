#!/bin/sh
# Remote bundle for tsp-evidence-logger.ps1 (runs on DUT via plink).
# Arg1: TSP test id e.g. T1-08
# Arg2: phase PRE|POST

TEST_ID="${1:-T1-xx}"
PHASE="${2:-POST}"

echo "=== DUT evidence ${PHASE} test_id=${TEST_ID} ==="
date -Iseconds 2>/dev/null || date
/usr/sbin/ccli --version 2>/dev/null || ccli --version 2>/dev/null || echo "ccli_version: unknown"
echo "=== process / listen ==="
pgrep -a ccli || echo NO_CCLI
netstat -lntp 2>/dev/null | grep 3782 || echo NO_3782
echo "=== dido_v2 (actuation) ==="
ubus call dido_v2 status 2>/dev/null || echo "ubus dido_v2: unavailable"

case "$TEST_ID" in
  T1-01|T1-10)
    PAT='CONNECT|DISCONNECT|TLS|ACSE|AARE|auth|handshake|Initiate|session|listening|FAILED|closed'
    ;;
  T1-07)
    PAT='Wlim|WMax|ctl|Operate|control|RBAC|dido|curtail|permissive'
    ;;
  T1-08)
    PAT='WSd|WSpt|ctl|Operate|control|RBAC|dido|curtail|permissive'
    ;;
  T1-09)
    PAT='fallback|comms_loss|Release|DISCONNECT|Wlim|WSd|session|closed'
    ;;
  T1-05|T1-06)
    PAT='urcb|report|integrity|URCB|Rpt|dataset|Mis4sec'
    ;;
  *)
    PAT='mms:|TLS|ACSE|ctl|Operate|report|session|error|WARN'
    ;;
esac

echo "=== logread ccli (filter: ${PAT}) ==="
logread -e ccli 2>/dev/null | grep -iE "$PAT" | tail -80 || echo "(no logread match)"

echo "=== /tmp/ccli.log tail ==="
tail -60 /tmp/ccli.log 2>/dev/null || echo "(no /tmp/ccli.log)"
