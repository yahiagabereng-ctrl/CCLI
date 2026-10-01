#!/usr/bin/env python3
from pathlib import Path
src = Path("/mnt/c/Yahia/projects/CCLI/lab/tg544-openwrt/_build_mms_step4.sh")
dst = Path("/tmp/_build_mms_step4.sh")
data = src.read_bytes().replace(b"\r\n", b"\n").replace(b"\r", b"\n")
dst.write_bytes(data)
dst.chmod(0o755)
print("wrote", dst, "bytes", len(data))
