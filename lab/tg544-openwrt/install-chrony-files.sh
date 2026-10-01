#!/usr/bin/env bash
# Deploy chrony + GNSS SOCK feeder + lab conf to TG544 (sideload; apk repos locked).
# Usage (from Windows via plink/pscp) or WSL with sshpass — see deploy-chrony.ps1
set -eu
ROOT="${1:-/}"
install -d "$ROOT/usr/sbin" "$ROOT/usr/bin" "$ROOT/etc/chrony" "$ROOT/etc/init.d" \
  "$ROOT/var/lib/chrony" "$ROOT/var/run/chrony" "$ROOT/var/log/chrony"
