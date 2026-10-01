#!/bin/bash
# Run on Pi (PuTTY): install and enable xRDP for Windows Remote Desktop
set -e

echo "=== Install xRDP on TestBench ==="
sudo apt update
sudo apt install -y xrdp

# Allow xRDP to access X session (common Debian fix)
sudo adduser xrdp ssl-cert 2>/dev/null || true

sudo systemctl enable xrdp
sudo systemctl restart xrdp
sudo systemctl status xrdp --no-pager || true

echo ""
echo "=== Port 3389 ==="
ss -tlnp | grep 3389 || sudo netstat -tlnp | grep 3389 || true

echo ""
echo "=== Done ==="
echo "On Windows: Remote Desktop -> 192.168.10.20"
echo "Session: Xorg (or default)"
echo "User: yahia + your Pi password"
echo ""
echo "If black screen: on Pi run 'sudo raspi-config' -> Advanced -> Wayland -> X11, reboot."
