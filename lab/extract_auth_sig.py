#!/usr/bin/env python3
from pathlib import Path
import sys

def read_tlv(data, pos):
    tag = data[pos]
    pos += 1
    lb = data[pos]
    pos += 1
    if lb & 0x80:
        n = lb & 0x7F
        length = int.from_bytes(data[pos : pos + n], "big")
        pos += n
    else:
        length = lb
    return tag, data[pos : pos + length], pos + length

hexdata = bytes.fromhex(Path(sys.argv[1]).read_text().strip())
off = int(sys.argv[2], 16) if len(sys.argv) > 2 else 0x8D
_, inner, _ = read_tlv(hexdata, off)
p = 0
_, cert, p = read_tlv(inner, p)
_, gt, p = read_tlv(inner, p)
_, sig, p = read_tlv(inner, p)
time_tlv = bytes([0x81, len(gt)]) + gt
Path("/tmp/wgt.bin").write_bytes(time_tlv)
Path("/tmp/wsig.bin").write_bytes(sig)
print(f"gt={gt.decode()!r}")
print(f"wire_sig_prefix={sig[:8].hex()}")
