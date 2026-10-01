#!/bin/sh
# Modbus A2/B2 bench check on TG544 (no CCLI required for basic checks)
echo "=== /dev/ttyS2 (A2/B2) ==="
ls -l /dev/ttyS2 || exit 1
echo ""
echo "=== modbus section in /etc/ccli/lab.yaml ==="
grep -A10 '^modbus:' /etc/ccli/lab.yaml 2>/dev/null || echo "Deploy lab_tr400.yaml to /etc/ccli/lab.yaml first"
echo ""
echo "=== NOT the A2/B2 path (for comparison) ==="
readlink -f /dev/rs485_2_uart 2>/dev/null || echo "rs485_2_uart: N/A"
echo ""
echo "Next: start PC slave (MODBUS_RTU_LAB.md), deploy ccli, run:"
echo "  /usr/sbin/ccli --config /etc/ccli/lab.yaml"
