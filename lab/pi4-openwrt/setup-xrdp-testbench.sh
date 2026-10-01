#!/bin/bash
# One-shot xRDP setup for TestBench (Raspberry Pi OS)
# Run on Pi: bash setup-xrdp-testbench.sh
# Or from PC when Pi is reachable:
#   plink -ssh yahia@192.168.10.20 -i %USERPROFILE%\.ssh\id_ed25519_testbench -m setup-xrdp-testbench.sh

set -euo pipefail

echo "=== xRDP setup for $(hostname) ==="

sudo apt-get update
sudo apt-get install -y xrdp dbus-xorg

# Debian/Ubuntu: xrdp user in ssl-cert
sudo adduser xrdp ssl-cert 2>/dev/null || true

# Prefer X11 for xRDP (Wayland often gives black screen)
if command -v raspi-config >/dev/null 2>&1; then
  echo "Switching to X11 session (raspi-config)..."
  sudo raspi-config nonint do_wayland W1 2>/dev/null || \
    sudo raspi-config nonint do_wayland 1 2>/dev/null || true
fi

# Desktop session for user yahia
USER_HOME="${HOME:-/home/yahia}"
if [ -d "$USER_HOME" ]; then
  if [ -f /usr/bin/startlxde-pi ]; then
    echo "exec /usr/bin/startlxde-pi" > "$USER_HOME/.xsession"
  elif [ -f /usr/bin/lxsession ]; then
    echo "exec startlxde" > "$USER_HOME/.xsession"
  else
    echo "exec dbus-launch --exit-with-session openbox" > "$USER_HOME/.xsession"
  fi
  chmod +x "$USER_HOME/.xsession"
  chown yahia:yahia "$USER_HOME/.xsession" 2>/dev/null || true
fi

# Polkit / colord (optional noise fix)
sudo tee /etc/polkit-1/localauthority/50-local.d/45-allow-colord.pkla >/dev/null <<'EOF' || true
[Allow Colord all Users]
Identity=unix-user:*
Action=org.freedesktop.color-manager.create-device;org.freedesktop.color-manager.create-profile;org.freedesktop.color-manager.delete-device;org.freedesktop.color-manager.delete-profile;org.freedesktop.color-manager.modify-device;org.freedesktop.color-manager.modify-profile
ResultAny=no
ResultInactive=no
ResultActive=yes
EOF

sudo systemctl enable xrdp
sudo systemctl restart xrdp

# Firewall if ufw present
if command -v ufw >/dev/null 2>&1 && sudo ufw status | grep -q active; then
  sudo ufw allow 3389/tcp
fi

echo ""
echo "=== Status ==="
systemctl is-active xrdp
ss -tlnp | grep 3389 || sudo netstat -tlnp | grep 3389

IP=$(hostname -I | awk '{print $1}')
echo ""
echo "=== Connect from Windows ==="
echo "  mstsc /v:${IP}"
echo "  User: yahia"
echo ""
echo "If still black screen after login, reboot once: sudo reboot"
