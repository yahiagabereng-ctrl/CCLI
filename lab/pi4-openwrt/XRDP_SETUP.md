# xRDP — make it work (TestBench / Pi 4)

**Pi IP:** `192.168.10.20` · **User:** `yahia`

---

## Why RDP failed from here

Your PC has lab Ethernet **`192.168.10.10`**, but the **Pi is offline** right now (no ping/SSH to `.20`).

**Before RDP works:**
1. Power on Pi, Ethernet cable to same switch as PC  
2. PuTTY login works → then run setup below  

---

## Fastest fix (copy in PuTTY — one paste)

```bash
curl -sSL https://raw.githubusercontent.com/... 
```

**Use local script** (copy file to Pi or paste):

```bash
bash -s <<'EOF'
set -e
sudo apt-get update
sudo apt-get install -y xrdp dbus-xorg
sudo adduser xrdp ssl-cert 2>/dev/null || true
command -v raspi-config >/dev/null && sudo raspi-config nonint do_wayland W1 2>/dev/null || true
echo 'exec startlxde-pi' > ~/.xsession 2>/dev/null || echo 'exec startlxde' > ~/.xsession
chmod +x ~/.xsession
sudo systemctl enable --now xrdp
sudo systemctl restart xrdp
systemctl is-active xrdp
ss -tlnp | grep 3389 || true
hostname -I
echo "Windows: mstsc -> IP above, user yahia"
EOF
```

Or from repo on PC, when Pi is back:

```bat
lab\pi4-openwrt\deploy-xrdp.bat
```

That uses SSH key `~\.ssh\id_ed25519_testbench` and opens Remote Desktop.

---

## Windows connect

Double-click: **`lab\pi4-openwrt\open-pi-rdp.bat`**

- Computer: `192.168.10.20`  
- User: `yahia`  
- Password: your Pi password  

---

## Black screen fix

```bash
sudo raspi-config
```
Advanced → Wayland → **X11** → Reboot

---

## Verify on Pi

```bash
systemctl status xrdp
ss -tlnp | grep 3389
```

On PC:

```powershell
Test-NetConnection 192.168.10.20 -Port 3389
```
