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

| Item | Planned path | Status |
|------|--------------|--------|
| CID | `apps/ccli/config/icd/product/product_tr400_eth_a_5ln.cid` | **TODO** — create before Compare sign-off |
| cfg | `apps/ccli/config/icd/product/product_tr400_eth_a_5ln.cfg` | **TODO** — `scripts/gen-mms-model-cfg.ps1` |
| DUT deploy | `/etc/ccli/icd/product_tr400_eth_a_5ln.cfg` | After cfg SHA recorded below |
| Product yaml | `apps/ccli/config/product_tr400.yaml` (or release bundle) | `model_cfg` → deploy path |
| PICS Cover | Declare **5-LN** + firmware semver | [PICS_FILLED_WORKBOOK.md](PICS_FILLED_WORKBOOK.md) |
| MICS | Model matches **same** CID | [MICS_DRAFT.md](MICS_DRAFT.md) — trim from lab draft |

| Field | Value (fill on session) |
|-------|-------------------------|
| CID History rev | *(fill)* |
| `LLN0.NamPlt.configRev` | *(fill)* |
| cfg SHA-256 | *(fill)* |
| `ccli --version` | *(fill)* |
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

**Until CID exists:** run **Phase 0–0b** and **document-only** rows only; do **not** tick **U-T1-02** Compare or submit PICS Cover as final 5-LN.

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

**Keywords:** `5-LN`, `product`, `specimen`, `O.15`, `Compare`, `ccli-product-specimen-5ln`
