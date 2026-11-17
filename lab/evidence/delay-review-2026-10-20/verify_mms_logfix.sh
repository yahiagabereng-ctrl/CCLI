#!/usr/bin/env bash
set -euo pipefail
repo="$(cd "$(dirname "$0")/../../.." && pwd)"
port=${1:-3782}
label=${2:-live}
python3 "$repo/apps/ccli/tools/mms-client/client.py" read \
    CCI016_01LD_Plant/PdCMMXU1.TotW.mag.f --fc MX --port "$port" \
    --identity-cert "$repo/apps/ccli/config/EJBCA RBAC/dual-certificate/dso-mms.pem" \
    --identity-key "$repo/apps/ccli/config/EJBCA RBAC/dual-certificate/dso-mms.key" \
    --report "$repo/lab/evidence/delay-review-2026-10-20/mms-$label-read.json"
