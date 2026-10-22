# EMT432 Modbus — handoff for Alireza (@alirezavzand)

**Date:** 2026-10-22  
**DUT path:** TG544 LAN3 → meter **192.168.178.10:502** unit **1**  
**Lab yaml:** `apps/ccli/config/lab_tr400_emt432_lan3_tcp_102.yaml`  
**Commissioning doc:** `lab/conformance/EMT432_CCLI_COMMISSIONING_MAP.md` (**rev 5.0**)

---

## What landed in git

| Area | Detail |
|------|--------|
| **CCLI commissioning** | `modbus_emt432_commissioning.cpp` — verify/write bench defaults on startup (40232, 40234–40243, **40245/40247** ripple, 40256, energy log reads) |
| **Revision pin** | `commissioning.revision: 5` in lab yaml ↔ `kRevision = 5` in `modbus_emt432_commissioning_revision.hpp` |
| **MMS poll** | FC3 block **906 × 36** — **V_L1_N 906**, **P1 932**, **Q1 940** (1P Rogowski; not P_SUM / L-L peak) |
| **Lab tools** | `lab/emt432_rogowski_config.py`, `lab/modbus_emt432_tcp_probe.py` |
| **One-SSH live dump** | `lab/emt432_dump_registers_one_ssh.ps1` + `lab/emt432_dut_ccli_register_dump.lua` → `lab/evidence/phase5/EMT432_DUT_SSH_DUMP_LATEST.txt` |
| **Deploy** | `lab/tg544-openwrt/deploy-emt432-80mv.ps1` (yaml + optional `--ApplyCertPin`) |

Product tree mirrored under `product/apps/ccli/` (same commissioning sources).

---

## Deploy on DUT (PC-A)

```powershell
cd C:\path\to\CCLI
. .\lab\tg544-openwrt\lab-env.ps1
powershell -File lab\tg544-openwrt\deploy-emt432-80mv.ps1
# optional: write meter 40232/40235 when bench agrees
powershell -File lab\tg544-openwrt\deploy-emt432-80mv.ps1 -ApplyCertPin -SensitivityMvKa 80
```

After restart, confirm commissioning in DUT log:

```text
logread -e modbus | tail -30
```

Expect **40245=0**, **40247=0**, **40243=0**, **40232** with integrator bit 13 set.

---

## Live register snapshot (one SSH from PC)

```powershell
. .\lab\tg544-openwrt\lab-env.ps1
powershell -File lab\emt432_dump_registers_one_ssh.ps1
```

Script stops `ccli` briefly on the DUT, reads all CCLI holding registers over LAN3, restarts `ccli`.  
Save evidence as **`modbus_emt432_<date>.txt`** (copy of dump) for matrix **U-P0-03/04**.

PC-direct (no DUT): `python lab/emt432_rogowski_config.py --host 192.168.178.10 --dump-ccli-registers --full-rms`

---

## Known bench gaps (2026-10-22)

| Item | Status |
|------|--------|
| **40235** Rogowski V/A | Meter may still show **~11764** until cert pin / `write_rogowski_sensitivity: true` — then expect **0.00008** (80 mV/kA) or **0.0001** (100 mV/kA) |
| **40716 KWh1** (Rev2 pin) | **READ_FAIL** on ITA Rev0 map; use **40721** for energy |
| **I / P / Q** | Stay ~0 until **40235** is correct |

---

## References

- Specimen: `lab/conformance/METER_SPECIMEN_EMT432_V_FULL.md`
- Cert pin: `lab/conformance/EMT432_CERT_REGISTER_PIN.md`
- RMS block: `lab/conformance/EMT432_RMS_REALTIME_HOLDING_SPEC.md`
- Portal steps: `lab/evidence/phase5/P5_EMT432_PORTAL_STEP_BY_STEP.md`

**Matrix:** `lab/conformance/LAB_UNIFIED_CERT_TEST_MATRIX.md` — Modbus / LAN3 preflight rows.
