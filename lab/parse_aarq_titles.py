#!/usr/bin/env python3
"""Parse ACSE AARQ AP/AE title fields from hex capture."""
from __future__ import annotations

import sys
from pathlib import Path


def read_hex(path: Path) -> bytes:
    text = path.read_text().strip().replace("\n", " ")
    return bytes.fromhex(text)


def read_tlv(data: bytes, pos: int) -> tuple[int, bytes, int]:
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


def decode_oid(raw: bytes) -> str:
    if not raw:
        return ""
    first = raw[0]
    parts = [str(first // 40), str(first % 40)]
    val = 0
    for b in raw[1:]:
        val = (val << 7) | (b & 0x7F)
        if not (b & 0x80):
            parts.append(str(val))
            val = 0
    return ".".join(parts)


def parse_acse(data: bytes) -> None:
    pos = data.find(bytes([0x60]))
    if pos < 0:
        print("no AARQ (0x60)")
        return
    _, aarq, _ = read_tlv(data, pos)
    p = 0
    names = {
        0xA1: "application-context-name",
        0xA2: "called-AP-title",
        0xA3: "called-AE-qualifier",
        0xA6: "calling-AP-title",
        0xA7: "calling-AE-qualifier",
        0x88: "sender-acse-requirements",
        0x89: "mechanism-name",
        0xAC: "calling-authentication-value",
    }
    while p < len(aarq):
        tag, val, p = read_tlv(aarq, p)
        name = names.get(tag, f"tag-0x{tag:02x}")
        if tag in (0xA2, 0xA6) and val and val[0] == 0x06:
            _, oid_raw, _ = read_tlv(val, 0)
            print(f"{name}: OID {decode_oid(oid_raw)} raw={oid_raw.hex()}")
        elif tag in (0xA3, 0xA7) and val and val[0] == 0x02:
            ae = int.from_bytes(val[2:], "big", signed=False)
            print(f"{name}: {ae}")
        else:
            print(f"{name}: len={len(val)}")


if __name__ == "__main__":
    path = Path(sys.argv[1] if len(sys.argv) > 1 else "lab/tmp_rx.hex")
    parse_acse(read_hex(path))
