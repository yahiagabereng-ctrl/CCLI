# Reset Raspberry Pi OS password (keeps all data on SD)

Use this if you forgot SSH login password. **Does not erase** your files.

## What you need

- Pi **powered off**
- microSD in a **USB card reader** on your PC (or keyboard + monitor on Pi)

---

## Method A — Edit boot partition on PC (easiest)

1. **Power off Pi**, remove SD, insert into PC.
2. Windows shows a small drive **`boot`** (FAT32). Open it in File Explorer.
3. Open **`cmdline.txt`** in Notepad (one long line).
4. At the **end** of that line (before any newline), add a space and:

   ```
   init=/bin/sh
   ```

   Example end of line:  
   `... rootwait init=/bin/sh`

5. **Save** `cmdline.txt`, eject SD safely, put SD back in **Pi 4**.
6. Connect **HDMI + USB keyboard** to Pi (required for this step).
7. **Power on** — you should get a root shell `#` prompt (may take a minute).

8. Remount root read-write and reset password:

   ```bash
   mount -o remount,rw /
   passwd yahia
   ```

   Enter **new password** twice (for user **`yahia`**).

   If login fails later, list users: `ls /home` and use `passwd` for that name.

9. **Optional:** enable SSH if disabled:

   ```bash
   systemctl enable ssh
   systemctl start ssh
   ```

10. **Power off** Pi (`poweroff` or hold power).

11. Put SD in PC again, open **`boot/cmdline.txt`**, **remove** `init=/bin/sh` (restore original line), save.

12. Boot Pi normally. Connect with PuTTY:

    - Host: `192.168.10.20`
    - User: **`yahia`**
    - Password: **new password**

---

## Method B — Raspberry Pi Imager (if Method A fails)

Imager can set username/password on a **fresh** flash only — **not** for existing SD without wiping.

For existing SD, use **Method A** only.

---

## PuTTY after reset

Run on PC:

```
lab\pi4-openwrt\open-pi-putty.bat
```

Or: Host `192.168.10.20`, user **`yahia`**, port `22`, SSH.

---

## Notes

- Old default `pi` / `raspberry` **no longer works** on new Raspberry Pi OS installs.
- OpenWrt uses user **`root`**, not `pi` — only if you flashed OpenWrt.
- Your Pi at `.20` was **Raspberry Pi OS** (Debian SSH banner).
