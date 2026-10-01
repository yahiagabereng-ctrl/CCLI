#!/usr/bin/env python3
from pathlib import Path


def read_tlv(data: bytes, pos: int):
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


def main() -> None:
    hx = Path("lab/tmp_tx.hex").read_text().strip().replace("\n", " ")
    b = bytes.fromhex(hx)
    pos = b.find(bytes([0x61]))
    _, aare, _ = read_tlv(b, pos)
    names = {
        0xA1: "ctx",
        0xA2: "result",
        0xA3: "diag",
        0xA4: "respAP",
        0xA5: "respAE",
        0x88: "req",
        0x89: "mech",
        0xAA: "auth",
        0xBE: "userinfo",
    }
    print("AARE fields in order:")
    p = 0
    while p < len(aare):
        tag, val, p = read_tlv(aare, p)
        print(f"  0x{tag:02x} {names.get(tag, '?')} len={len(val)}")
        if tag in (0xA4, 0xA6) and val[:1] == bytes([0x06]):
            _, oid, _ = read_tlv(val, 0)
            print(f"    oid={oid.hex()}")
        if tag in (0xA5, 0xA7) and val[:1] == bytes([0x02]):
            print(f"    ae={int.from_bytes(val[2:], 'big')}")


if __name__ == "__main__":
    main()
