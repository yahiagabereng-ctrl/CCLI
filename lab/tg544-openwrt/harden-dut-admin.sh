#!/bin/sh
# Disable IP-based LuCI / TesPro web UI and northbound mmsd; optional SSH pubkey + password auth off.
# Uploaded by scripts/harden-dut-admin.ps1 — do not run standalone without pubkey when disabling passwords.
set -eu

echo "=== harden-dut-admin: LuCI / uhttpd off ==="
/etc/init.d/uhttpd stop 2>/dev/null || true
/etc/init.d/uhttpd disable 2>/dev/null || true
if uci -q get uhttpd.main >/dev/null 2>&1; then
	uci set uhttpd.main.enabled='0' 2>/dev/null || true
	uci commit uhttpd 2>/dev/null || true
fi

echo "=== harden-dut-admin: TesPro IEC61850 LuCI stack off (ccli owns :3782) ==="
uci set iec61850service.global.enabled='0' 2>/dev/null || true
uci commit iec61850service 2>/dev/null || true
/etc/init.d/iec61850service disable 2>/dev/null || true
/etc/init.d/iec61850service stop 2>/dev/null || true
/etc/init.d/iec61850-mmsd disable 2>/dev/null || true
/etc/init.d/iec61850-mmsd stop 2>/dev/null || true
killall iec61850-mmsd 2>/dev/null || true
killall iec61850d 2>/dev/null || true

if [ -f /tmp/ccli_admin.pub ]; then
	echo "=== harden-dut-admin: install SSH pubkey ==="
	mkdir -p /etc/dropbear
	touch /etc/dropbear/authorized_keys
	chmod 600 /etc/dropbear/authorized_keys
	# One key per line; skip if fingerprint already present
	pub_line=$(tr -d '\r\n' </tmp/ccli_admin.pub)
	if ! grep -qF "$pub_line" /etc/dropbear/authorized_keys 2>/dev/null; then
		echo "$pub_line" >>/etc/dropbear/authorized_keys
	fi
fi

case "${CCLI_DROPBEAR_PASSWORD_AUTH:-keep}" in
off|0|false)
	echo "=== harden-dut-admin: dropbear password auth OFF ==="
	if uci -q get dropbear.@dropbear[0] >/dev/null 2>&1; then
		uci set dropbear.@dropbear[0].PasswordAuth='off'
		uci set dropbear.@dropbear[0].RootPasswordAuth='off'
		uci commit dropbear
		/etc/init.d/dropbear restart
	else
		echo "WARN: no dropbear UCI section — set PasswordAuth manually"
	fi
	;;
*)
	echo "=== harden-dut-admin: dropbear password auth unchanged (set CCLI_DROPBEAR_PASSWORD_AUTH=off to disable) ==="
	;;
esac

echo "=== harden-dut-admin: verify ==="
ps w | grep -E 'uhttpd|iec61850-mmsd' | grep -v grep || echo "OK: no uhttpd/mmsd"
netstat -tlnp 2>/dev/null | grep -E ':80|:443|:3782|:2404' || true
echo "HARDEN_DUT_ADMIN_DONE"
