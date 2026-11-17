#!/usr/bin/env bash
set -euo pipefail
repo="$(cd "$(dirname "$0")/../../../.." && pwd)"
ctest --test-dir "$repo/apps/ccli/build-event-log-fix" --output-on-failure -R 'event_store|aggregator_power|tls_allowlist'
openssl pkey -pubin -in "$repo/apps/firmware-updater/keys/manufacturer/ccli-release.pub.pem" -outform DER | sha256sum
