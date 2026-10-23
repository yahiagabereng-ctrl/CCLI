# EMT432 → PdCMMXU1 (LN) — r64 handoff for Alireza

**Date:** 2026-10-23  
**Audience:** @alirezavzand (PC-A / logger)  
**DUT reference:** TG544 `192.168.1.130` SSH · MMS `192.168.10.1:3782` · meter `192.168.178.10:502`

---

## What changed (pull `main` after push)

| Item | Value |
|------|--------|
| **Firmware** | **ccli 0.1.0-r64** `P5-EMT432-CMV-ANG-URCB-PF1` |
| **Bundle** | `releases/product-cyber/0.1.0-r64/` (APK + ELF + yaml/cfg/cid) |
| **Lab pin** | `lab/tg544-openwrt/ccli-bin.r64.p5-emt432-cmv-ang-urcb-pf1` |
| **ELF SHA-256** | `967df2764819c3b616e5467a16a3f9a923b4002ad1ca4a73d156d16e91ab2df7` |
| **Yaml** | `apps/ccli/config/product_tr400_p3_07_connect_5ln.yaml` |
| **ICD cfg** | `apps/ccli/config/icd/product/product_tr400_eth_a_5ln.cfg` (URCB includes **ang.f**) |
| **Meter map freeze** | `lab/conformance/EMT432_METER_CODE_FREEZE.md` · `lab/emt432-meter-code.freeze.json` |

---

## EMT432 vs LN verification (PASS 2026-10-23)

Evidence log:

`lab/evidence/phase5/P5_EMT432_LN_VERIFY_2026-10-23_091200.txt`

Paired probe + DUT modbus (example):

| EMT432 | MMS / ccli |
|--------|------------|
| P1 1.873 kW @932 | TotW |
| Q1 −0.031 kvar @940 | TotVAr |
| I1 8.294 A @922 | A.phsA mag |
| V_L1_N 0.226 kV @906 | PPV phsAB mag |
| φ ≈ −0.94° | A.phsA ang · PPV phsAB ang |

---

## Commands

```powershell
git pull origin main
$env:CCLI_TG544_PW = '...'
powershell -File scripts/verify-emt432-meter-code-freeze.ps1
powershell -File lab\tg544-openwrt\verify-emt432-lan3.ps1 -MeterHost178 192.168.178.10 -UseSshTunnel
```

Deploy (LAN2):

```powershell
powershell -File lab\tg544-openwrt\deploy-ccli-session-fix.ps1 -HostAddr 192.168.1.130 `
  -LabYaml apps\ccli\config\product_tr400_p3_07_connect_5ln.yaml `
  -TlsDir apps\ccli\config\tls\p3_07_connect `
  -Changes @("r64 EMT432 meter LN map")
```

**Commissioning writes** (40232/40235): still rev **5** — see [EMT432_MODBUS_COMMISSIONING_ALIREZA.md](EMT432_MODBUS_COMMISSIONING_ALIREZA.md).

TestPro cert points: `lab/evidence/phase5/CCLI_LAB_5LN_cert_points.csv` (includes **A.phsA ang**).
