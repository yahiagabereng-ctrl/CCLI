# Product specimen pin — 5-LN (first unified cert test run)

**Document ID:** CCLI-PROD-SPEC-5LN-001  
**Revision:** 1.1  
**Date:** 2026-10-14  
**Validation:** [PRODUCT_SPECIMEN_5LN_VALIDATION.md](PRODUCT_SPECIMEN_5LN_VALIDATION.md) · `scripts/validate-5ln-cid.ps1`
**RAG source_id:** `ccli-product-specimen-5ln`  
**Parent matrix:** [LAB_UNIFIED_CERT_TEST_MATRIX.md](LAB_UNIFIED_CERT_TEST_MATRIX.md) (`ccli-lab-unified-cert-test-matrix`)  
**Bench runbook:** [TSP_TWO_PC_BENCH_RUNBOOK.md](TSP_TWO_PC_BENCH_RUNBOOK.md) (`ccli-tsp-two-pc-bench-runbook`)

**Distinction:** [LAB_SPECIMEN_VERSION.md](LAB_SPECIMEN_VERSION.md) pins **31-LN lab CID rev 9** (historical TSP). This document pins the **O.15 / product certificate** MMS model for the **first** execution of the unified matrix.

---

## Model scope (certificate intent)

| LN | Purpose | Annex T / O |
|----|---------|-------------|
| `LLN0` | Datasets, URCB, NamPlt | T.3.2 · reporting |
| `LPHD1` | Physical device | Table 3 |
| `PdCMMXU1` | POC P/Q/V measures | T.3.1.3 · O.8 |
| `WlimDWMX1` | Active power limit control | O.9.2.2 |
| `WSdDAGC1` | Setpoint control | O.9.2.3 |

**IED name (unchanged):** `CCI016_01` · **MMS:** `192.168.10.1:3782` TLS · **AccessPoint:** `accessPoint1`

---

## File pin (update on first commit of product CID)

| Item | Path | Status |
|------|------|--------|
| CID | `apps/ccli/config/icd/product/product_tr400_eth_a_5ln.cid` | **DONE 2026-10-14** — `validate-5ln-cid.ps1` **PASS** |
| cfg | `apps/ccli/config/icd/product/product_tr400_eth_a_5ln.cfg` | **DONE** — `gen-mms-model-cfg.ps1` (5 LNs, URCB options 159) |
| DUT deploy | `/etc/ccli/icd/product/product_tr400_eth_a_5ln.cfg` | **DEPLOYED 2026-10-15** — use `scripts/push-5ln-product-cfg-to-dut.ps1` (deploy script path fixed r55) |
| Cert-run yaml | `apps/ccli/config/lab_tr400_phase4_eth_b_ttyS1_5ln.yaml` | `model_cfg` → product cfg; lab 31-LN stays in `..._ttyS1.yaml` |
| PICS Cover | Declare **5-LN** + firmware semver | [PICS_FILLED_WORKBOOK.md](PICS_FILLED_WORKBOOK.md) |
| MICS | Model matches **same** CID | [MICS_DRAFT.md](MICS_DRAFT.md) — trim from lab draft |

| Field | Value (session 2026-10-22) |
|-------|-----------------------------|
| CID History rev | **4** (`when=2026-10-22`, ENS DOType dedupe + APC/ENC timeout defaults) |
| CID SHA-256 | `431047507CDC16B117EA1740A37805C4235F9CDF6A22D6C2F724F6CCEAA8BB4B` |
| `LLN0.NamPlt.configRev` | **20261022** |
| cfg SHA-256 | `A4F8845382C4D09FC14B3F10EDB03AE3FAB51FB73FD8D1322B2391EDD4AE568E` (5946 bytes) |
| `ccli --version` | **0.1.0-r55** (`P5-5LN-CERT-MODEL`) — reactive LNs optional in MMS adapter |
| DUT boot log | `mms: full CID model loaded from /etc/ccli/icd/product_tr400_eth_a_5ln.cfg` · VArSd/PFSP **inactive** notices |
| LN count (Compare) | **5** |

### Exact MMS browse list (U-T1-03)

Under `LD_Plant`, GetNameList / browse must show **only**:

1. `LLN0`  
2. `LPHD1`  
3. `PdCMMXU1`  
4. `WlimDWMX1`  
5. `WSdDAGC1`  

Pre-Compare gate:

```powershell
powershell -File scripts\validate-5ln-cid.ps1
```

### CID revision discipline (any model edit)

After **any** change to `product_tr400_eth_a_5ln.cid`, append History and bump identifiers **before** Compare or deploy:

```powershell
powershell -File scripts\bump-5ln-cid-revision.ps1 `
  -What "One-line summary of SCL change" `
  -Why "U-T1-02 Compare / waiver / annex alignment" `
  -RegenCfg -SyncProduct
```

The script updates:

| Field | Rule |
|-------|------|
| `<Header revision="N">` | `N` = previous + 1 |
| `<History><Hitem … revision="N" when="YYYY-MM-DD" what="…" /></History>` | New row per edit (lab CID pattern) |
| `LLN0.NamPlt.configRev` | `YYYYMMDD` (default: today) |

Then update this document (**History rev**, **configRev**, **CID/cfg SHA-256**), `scripts/validate-5ln-stack.ps1` pin constants, DUT cfg push, TSP re-import CID, LuCI ICD path refresh (`enable-dut-luci-iec61850.ps1`).

**CID exists (2026-10-14):** U-T1-02 Compare and U-T1-03 browse are **unblocked** — TSP must import `cid\product_tr400_eth_a_5ln.cid` and DUT must run `lab_tr400_phase4_eth_b_ttyS1_5ln.yaml` (check session header `ccli --version` ≥ r54).

---

## PICS / Compare expectations (5-LN vs 31-LN lab)

| Check | 31-LN lab (rev 9) | 5-LN product |
|-------|-------------------|--------------|
| Browse depth (T1-03) | ~31 LNs | **5 LNs** |
| Compare Value waivers | ~60 PdC PPV/A (TSP-SCL-01) | Target **0 Value** on cert CID |
| GOOSE / BRCB / extra LNs | In CID Services; PICS **N/A** first quote | **Absent** from cert CID |
| Cyber (3782, RBAC, CRL) | Same transport | **Same rows** — LN count irrelevant |

Corpus: `ccli-dnv-product-readiness` · `ccli-lab-cid-pics-alignment`

---

## DUT yaml (two-PC bench — same day as TSP + 104 + plant)

Recommended for your bench (**2× PC · TSP · EMT432 · USB RS485 mock**):

| Goal | Yaml on DUT | Modbus P source |
|------|-------------|-----------------|
| **TSP + 104 + RS485 inverter mock** | `lab_tr400_phase4_eth_b_ttyS1.yaml` | RTU **COM5** slave on A1/B1 |
| **TSP + 104 + EMT432 TCP** | `lab_tr400_phase4_emt432_lan3_tcp.yaml` | TCP meter (confirm LAN3 subnet first) |

See runbook § **DUT deploy** and **LAN3 IP alignment** (`.30.0/24` vs `.178.0/24` Chronos bench).

---

## Related RAG source_ids

| source_id | Use |
|-----------|-----|
| `ccli-lab-unified-cert-test-matrix` | Master □ table |
| `ccli-lab-unified-cert-test-roadmap` | Phase order + HW |
| `ccli-product-specimen-5ln` | This pin |
| `ccli-lab-specimen-version` | Lab 31-LN reference only |
| `ccli-tsp-two-pc-bench-runbook` | PC-A / PC-B roles |

**Meter (plant P/Q/V):** [METER_SPECIMEN_EMT432_V_FULL.md](METER_SPECIMEN_EMT432_V_FULL.md) — bench pin · [CHRONOS_EMT432_VENDOR_REQUIREMENTS.md](CHRONOS_EMT432_VENDOR_REQUIREMENTS.md) — **all Chronos cert asks**.

**Annex O/T (software):** [ANNEX_OT_SW_GAP_MATRIX.md](ANNEX_OT_SW_GAP_MATRIX.md) — 55×6 clause → SW → 5-LN → Phase; index [ANNEX_O_SOFTWARE_MATRIX.md](ANNEX_O_SOFTWARE_MATRIX.md).

**Keywords:** `5-LN`, `product`, `specimen`, `O.15`, `Compare`, `ccli-product-specimen-5ln`
