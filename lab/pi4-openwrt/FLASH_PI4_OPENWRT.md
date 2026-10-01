# ARCHIVED — Pi 4 out of scope. See `ARCHIVED.md` and `../tg544-openwrt/`.

# Flash OpenWrt 24.10.5 on Raspberry Pi 4 (historical)

**Target:** `bcm27xx/bcm2711` · **Lab IP after setup:** `192.168.10.20`

## Image download (official)

**Direct URL (Pi 4 factory, OpenWrt 24.10.5):**

https://downloads.openwrt.org/releases/24.10.5/targets/bcm27xx/bcm2711/openwrt-24.10.5-bcm27xx-bcm2711-rpi-4-squashfs-factory.img.gz

**SHA256:**
```
0e89d9a5389023b0f1bc10467ade0cecce6bc6da9ea37b19eb7414af04054d07
```

**Size:** ~14.9 MB compressed · **Do not** decompress for Pi Imager — select the `.img.gz` directly.

Upstream index: https://downloads.openwrt.org/releases/24.10.5/targets/bcm27xx/bcm2711/

---

## Image on disk (verified)

Download the file above into e.g. `Downloads\` or `lab\pi4-openwrt\`.

```
openwrt-24.10.5-bcm27xx-bcm2711-rpi-4-squashfs-factory.img.gz
```

---

## Step 1 — Flash SD card (Windows)

1. Insert a **microSD** (≥ 4 GB) into your PC.
2. Open **Raspberry Pi Imager** (install from https://www.raspberrypi.com/software/ if needed).
3. Choose **Use custom image** → select the `.img.gz` file above (Imager accepts gz).
4. Choose the SD card → **Write** → wait until finished.
5. Eject SD, insert into **Pi 4**, connect **Ethernet**, power on.

> **Warning:** This erases the SD card. Pi will no longer boot Raspberry Pi OS from this card.

---

## Step 2 — First login

Default after factory flash:

- IP: **192.168.1.1** on `eth0` (unless your DHCP gives something else)
- User: **root**
- Password: *(none)* — set a password when prompted

From your PC (lab PC is `192.168.10.10`):

```powershell
# If direct cable / Pi on 192.168.1.x temporarily:
ssh root@192.168.1.1
```

If the Pi gets DHCP on your lab LAN, try:

```powershell
ssh root@192.168.10.20
```

---

## Step 3 — Set lab static IP

On the Pi (SSH):

```bash
uci set network.lan.ipaddr='192.168.10.20'
uci set network.lan.netmask='255.255.255.0'
uci set network.lan.gateway='192.168.10.10'
uci commit network
/etc/init.d/network restart
```

Reconnect:

```bash
ssh root@192.168.10.20
```

---

## Step 4 — GPIO tools (relay test)

```bash
opkg update
opkg install libgpiod gpiod-tools
gpiodetect
gpioinfo gpiochip0 | grep -E 'line   5|line  27'
```

CCLI pin map (`config/lab_pi.yaml`):

| Signal | BCM line |
|--------|----------|
| DO curtail (relay) | **5** (active-**low** — typical opto relay board) |
| DI permissive | **27** (active-low) |

Relay test (stop ccli first: `/etc/init.d/ccli stop`):

```bash
gpioset gpiochip0 5=0    # ON  (active-low module)
gpioset gpiochip0 5=1    # OFF (safe)
gpioget gpiochip0 27      # read permissive
```

**Wiring:** BCM **GPIO 5** = **physical pin 29** on the 40-pin header (not physical pin 5).
Relay module needs **5 V/VCC** and **GND** to the Pi; **IN** to GPIO 5.

---

## Step 5 — Optional: set hostname

```bash
uci set system.@system[0].hostname='ccli-pi4'
uci commit system
/etc/init.d/system restart
```

---

## Troubleshooting

| Issue | Fix |
|-------|-----|
| No SSH on 192.168.1.1 | Connect HDMI+keyboard; run `ip addr` |
| Wrong image (Pi 3) | Must use **bcm2711** / **rpi-4-** image only |
| `gpiodetect` empty | Reboot; check `dmesg \| grep gpio` |
| Relay inverted | Module may be active-low — try `5=0` for ON |

---

## After OpenWrt works

Next engineering steps (repo):

1. Implement libgpiod in `apps/ccli/platform/platform_pi/hal_gpio_ll.c`
2. Cross-build `ccli` `.ipk` — see **`lab/pi4-openwrt/DEPLOY_CCLI.md`** and **`package/ccli/`**
3. procd init + UCI config — **package ships** `/etc/init.d/ccli` + `/etc/config/ccli` (see `CCI_OpenWrt_Procedural_Extract.md`)
