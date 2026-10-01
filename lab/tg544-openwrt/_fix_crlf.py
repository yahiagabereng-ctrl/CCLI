from pathlib import Path

files = [
    "wsl-build-ccli.sh",
    "wsl-build-measure-gnss.sh",
    "wsl-build-chrony.sh",
    "wsl-build-gnss-sock.sh",
    "measure_gnss_offset.sh",
    "measure_gnss_offset_legacy.sh",
    "ccli-chrony.init",
    "chrony.conf",
    "install-chrony-files.sh",
]
base = Path("/mnt/c/Yahia/projects/CCLI/lab/tg544-openwrt")
for name in files:
    p = base / name
    if not p.exists():
        print("skip", p)
        continue
    t = p.read_bytes().replace(b"\r\n", b"\n").replace(b"\r", b"\n")
    p.write_bytes(t)
    print("ok", p)
