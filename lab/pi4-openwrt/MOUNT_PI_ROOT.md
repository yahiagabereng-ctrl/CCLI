# Access Pi SD card partition 2 (Linux root) on Windows

Windows only shows **D: bootfs** (FAT32, ~512 MB). Your data is on **partition 2** (ext4, ~57 GB).

## Method A — WSL mount (recommended)

1. Insert SD card (D: boot may appear — that is OK).
2. **Right-click** `mount-pi-root-partition.ps1` → **Run with PowerShell** → allow **Admin (UAC)**.
3. Explorer opens the Linux root. Copy from:
   ```
   \\wsl$\Ubuntu\mnt\wsl\PHYSICALDRIVE1p2\home\yahia
   ```
   to `C:\Yahia\projects\The PI4 972026\`

**Unmount when done** (Admin PowerShell):
```powershell
wsl --unmount \\.\PHYSICALDRIVE1
```

## Method B — Linux Reader (GUI, no WSL)

1. Download [DiskInternals Linux Reader](https://www.diskinternals.com/linux-reader/) (free, read-only).
2. Open app → select **Mass Storage Device** (~57 GB) → partition **Ext4**.
3. Browse `/home/yahia` → right-click → **Save** to PC.

## Method C — Full card image (backup before flash)

Raspberry Pi Imager → **Read** → save `.img` to `C:\Yahia\projects\The PI4 972026\`.

## Disk layout (this PC)

| Disk | Partition | Letter | Size | Content |
|------|-----------|--------|------|---------|
| Disk 1 | 1 | D: | 512 MB | boot (FAT32) |
| Disk 1 | 2 | — | ~57 GB | root (ext4) — **your data** |
