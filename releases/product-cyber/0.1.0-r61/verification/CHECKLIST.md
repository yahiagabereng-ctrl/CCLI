# Verification — 0.1.0-r61 EMT432 phase-A MMS

| Check | Result |
|-------|--------|
| `/usr/sbin/ccli --version` = `0.1.0-r61` | PASS (lab) |
| ELF SHA-256 `448007a2…` | PASS (staging = DUT after deploy) |
| Yaml SHA `f4d0d29c…` = repo `product_tr400_p3_07_connect_5ln.yaml` | PASS |
| Model cfg includes URCB DE for `A.phsA.cVal.mag.f` | PASS |
| Modbus trace `I1=` non-zero with EMT432 on LAN3 | PASS |
| MMS publish log `A.phsA.cVal.mag.f` tracks I1 | PASS |
| TestPro MX read `PdCMMXU1.A.phsA.cVal.mag.f` | PASS (lab session) |
| P3-07 TLS connect (inherits r57 ISO/ACSE) | PASS (unchanged cyber path) |
