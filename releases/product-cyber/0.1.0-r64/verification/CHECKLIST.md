# Verification — 0.1.0-r64 EMT432 → PdCMMXU1 LN

| Check | Result |
|-------|--------|
| `/usr/sbin/ccli --version` = `0.1.0-r64` | PASS (lab DUT) |
| ELF SHA-256 `967df276…` | PASS (staging = DUT after deploy) |
| Yaml SHA `366a7d06…` | PASS |
| Model cfg URCB includes `A.phsA` + `PPV.phsAB` **ang.f** | PASS |
| Modbus FC3 @40907 **qty=52** (PF1 @956) | PASS |
| EMT432 probe vs ccli store (P/Q/I/φ) | PASS — `P5_EMT432_LN_VERIFY_2026-10-23_091200.txt` |
| `scripts/verify-emt432-meter-code-freeze.ps1` | PASS |
| P3-07 TLS MMS `192.168.10.1:3782` | PASS (lab) |
