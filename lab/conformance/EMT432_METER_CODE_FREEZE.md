# EMT432 meter → MMS code freeze (poll + PdCMMXU1 CMV)

**Document ID:** CCLI-LAB-EMT432-METER-CODE-FREEZE-001  
**Revision:** 1.0  
**Effective:** 2026-10-23  
**RAG source_id:** `ccli-emt432-meter-code-freeze`  
**Machine pin:** `lab/emt432-meter-code.freeze.json`  
**Parents:** [EMT432_CERT_REGISTER_PIN.md](EMT432_CERT_REGISTER_PIN.md) · [EMT432_CCLI_COMMISSIONING_MAP.md](EMT432_CCLI_COMMISSIONING_MAP.md) · `ccli-product-sw-freeze`  
**Evidence:** [P5_EMT432_LN_VERIFY_2026-10-23_091200.txt](../evidence/phase5/P5_EMT432_LN_VERIFY_2026-10-23_091200.txt)

This freeze pins **how ccli reads the EMT432** and maps **`PdCMMXU1`** (TotW, TotVAr, PPV, A, angles, 4 s URCB). It is **independent** of commissioning **write** revision (`kRevision = 5` in `modbus_emt432_commissioning_revision.hpp`).

---

## Requirements

| ID | Requirement |
|----|-------------|
| REQ-EMT-FRZ-001 | **ccli ≥ 0.1.0-r64** with codename **P5-EMT432-CMV-ANG-URCB-PF1** on bench DUT |
| REQ-EMT-FRZ-002 | Product yaml **`product_tr400_p3_07_connect_5ln.yaml`** Modbus section unchanged without bumping this freeze rev |
| REQ-EMT-FRZ-003 | FC3 block **40907**, **qty=52** (offsets **906–957**), `float_word_swap: true` |
| REQ-EMT-FRZ-004 | **`poll_pdc_3ph: true`** with auto offsets **916–944**, **922** I1, **956** PF1 |
| REQ-EMT-FRZ-005 | Angles: **atan2(Q1,P1)** deg; if **\|Q1\| < 5 var** use **PF1**; **1P:** `PPV.phsAB.ang` = `A.phsA.ang` |
| REQ-EMT-FRZ-006 | ICD **`product_tr400_eth_a_5ln.cfg`**: `DS_R_PdC_Mis4sec` includes **A.phsA** and **PPV.phsAB** **mag.f + ang.f** |
| REQ-EMT-FRZ-007 | Re-verify after any change to files in § Frozen sources — log under `lab/evidence/phase5/P5_EMT432_LN_VERIFY_*` |

---

## Frozen pair (lab bench @ 192.168.178.10)

| Item | Value |
|------|--------|
| **ccli release** | **0.1.0-r64** · ELF SHA-256 `967df2764819c3b616e5467a16a3f9a923b4002ad1ca4a73d156d16e91ab2df7` |
| **Yaml** | `apps/ccli/config/product_tr400_p3_07_connect_5ln.yaml` SHA-256 `366a7d06100d13cff0d1f1c1c2ecdc171aa8e548c4dcccbf9e9f3860b04713a4` |
| **ICD cfg** | `apps/ccli/config/icd/product/product_tr400_eth_a_5ln.cfg` SHA-256 `68fc1de01c67037ca502945f0c7a94628b0847f7da7f342f5f4f7d102a0f5741` |
| **MMS map code rev** | **1** (`modbus_emt432_mms_map_revision.hpp`) |
| **Meter TCP** | **192.168.178.10:502** unit **1** |
| **Deploy TLS** | `apps/ccli/config/tls/p3_07_connect` |

---

## Realtime map (read → LN)

| Offset | PLC | Signal | MMS |
|--------|-----|--------|-----|
| 932 | 40933 | P1 W | `TotW.mag.f` kW (×0.001) |
| 940 | 40941 | Q1 var | `TotVAr.mag.f` kvar (×0.001) |
| 906 | 40907 | V_L1_N | `PPV.phsAB.cVal.mag.f` kV (×0.001) |
| 922 | 40923 | I1 A | `A.phsA.cVal.mag.f` |
| 932+940 / 956 | — | φ | `A.phsA.cVal.ang.f`, `PPV.phsAB.cVal.ang.f` |

**Sources (do not split without rev bump):**

- `apps/ccli/adapters/modbus_master/modbus_pdc_decode.cpp`
- `apps/ccli/core/measurement/pdc_mmxu_phasors.hpp`
- `apps/ccli/adapters/modbus_master/modbus_adapter.cpp` (`read_block_*`, qty 52)
- `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp` (`update_pdc_phasors`)
- `apps/ccli/program/ccli_bridge.cpp` (measurement → MMS)

---

## Verification

```powershell
$env:CCLI_TG544_PW = '...'
powershell -File scripts/verify-emt432-meter-code-freeze.ps1
```

**Pass:** probe + DUT modbus trace within tolerances in freeze JSON; **Fail:** exit non-zero.

---

## Maintenance

| Rev | Date | Change |
|-----|------|--------|
| 1.0 | 2026-10-23 | Initial freeze @ r64; LN verify log 2026-10-23_091200 |
