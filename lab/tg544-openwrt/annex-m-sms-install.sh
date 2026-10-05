#!/bin/sh
# Install Annex M SMS poller on TG544 (OpenWrt). Run once on DUT as root.
set -u

BINDIR="/usr/sbin"
CRON_MARK="# ccli-annex-m-sms"
SRC_DIR="$(cd "$(dirname "$0")" && pwd)"

mkdir -p "$BINDIR"

cp "$SRC_DIR/annex-m-trip.sh" "$BINDIR/annex-m-trip.sh"
cp "$SRC_DIR/annex-m-sms-poller.sh" "$BINDIR/annex-m-sms-poller.sh"
cp "$SRC_DIR/annex-m-sms-number.sh" "$BINDIR/annex-m-sms-number.sh"
cp "$SRC_DIR/annex-m-sms-send.sh" "$BINDIR/annex-m-sms-send.sh"
cp "$SRC_DIR/annex-m-sms-reply.sh" "$BINDIR/annex-m-sms-reply.sh"
chmod 755 "$BINDIR/annex-m-trip.sh" "$BINDIR/annex-m-sms-poller.sh" \
	"$BINDIR/annex-m-sms-number.sh" "$BINDIR/annex-m-sms-send.sh" \
	"$BINDIR/annex-m-sms-reply.sh"

CRON_FILE="/etc/crontabs/root"
touch "$CRON_FILE"
grep -v "$CRON_MARK" "$CRON_FILE" >"${CRON_FILE}.tmp" 2>/dev/null || true
mv "${CRON_FILE}.tmp" "$CRON_FILE"
{
	echo "* * * * * /usr/sbin/annex-m-sms-poller.sh $CRON_MARK"
	echo "* * * * * sleep 30; /usr/sbin/annex-m-sms-poller.sh $CRON_MARK"
} >>"$CRON_FILE"

/etc/init.d/cron enable 2>/dev/null || true
/etc/init.d/cron restart 2>/dev/null || /etc/init.d/cron start 2>/dev/null || true

touch /tmp/annex-m-sms.log
chmod 644 /tmp/annex-m-sms.log

echo "Installed:"
echo "  $BINDIR/annex-m-trip.sh"
echo "  $BINDIR/annex-m-sms-poller.sh"
echo "  crontab: $CRON_FILE"
echo ""
echo "SMS commands (see annex-m-sms-number.sh for MSISDN):"
echo "  TRIP   → telescatto ON"
echo "  CLEAR  → telescatto OFF"
echo ""
"$BINDIR/annex-m-sms-number.sh"
