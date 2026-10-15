#!/bin/sh
# LuCI IEC 61850: normalize ICD path (no .cid suffix) and start iec61850-mmsd.
set -eu

ICD_BASE="${1:-product_tr400_eth_a_5ln}"

mkdir -p /tmp/iec61850_icd
if [ -f "/tmp/iec61850_icd/${ICD_BASE}.cid" ] && [ ! -f "/tmp/iec61850_icd/${ICD_BASE}" ]; then
	cp -f "/tmp/iec61850_icd/${ICD_BASE}.cid" "/tmp/iec61850_icd/${ICD_BASE}"
fi
chmod 644 "/tmp/iec61850_icd/${ICD_BASE}" 2>/dev/null || true
chmod 644 "/tmp/iec61850_icd/${ICD_BASE}.cid" 2>/dev/null || true

/etc/init.d/iec61850-mmsd enable 2>/dev/null || true
/etc/init.d/iec61850-mmsd start 2>/dev/null || /etc/init.d/iec61850-mmsd restart
sleep 2

echo "=== mmsd ==="
/etc/init.d/iec61850-mmsd status || true
ps w | grep iec61850-mmsd | grep -v grep || echo "WARN: mmsd not in ps"

echo "=== icd dir ==="
ls -la /tmp/iec61850_icd/

echo "=== parseICD ==="
if ubus list | grep -q iec61850.mms; then
	ubus call iec61850.mms parseICD "{\"path\":\"/tmp/iec61850_icd/${ICD_BASE}\"}" | head -c 400
	echo
else
	echo "FAIL: iec61850.mms not on ubus (start mmsd)"
fi

echo "=== LuCI path ==="
echo "/tmp/iec61850_icd/${ICD_BASE}"
