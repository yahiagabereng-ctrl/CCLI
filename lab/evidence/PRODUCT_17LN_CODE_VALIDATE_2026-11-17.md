# Product 17-LN code validation

**Document ID:** CCLI-PROD-17LN-VAL-001  
**Date:** 2026-11-17  
**Tree:** `product/apps/ccli`  
**Specimen:** ERDA Wave-1 `erda_wave1_17ln` (classification schema **1.2**)  
**Ship model:** `product/apps/ccli/config/icd/product/product_tr400_eth_a_17ln.cid` + `.cfg`  
**Ship yaml:** `product/apps/ccli/config/product_tr400.yaml` → `model_cfg: /etc/ccli/icd/product_tr400_eth_a_17ln.cfg`

---

## Requirements

| ID | Requirement | Result |
|----|-------------|--------|
| REQ-17V-001 | Product CID inventory = frozen 17 LN list | **PASS** |
| REQ-17V-002 | Product CID/CFG byte-match generator output from latest main CID | **PASS** |
| REQ-17V-003 | Ship / bench yamls pin the 17-LN `.cfg` | **PASS** |
| REQ-17V-004 | Product `mms_adapter` wires Wlim / WSd / PdC and installs control handlers | **PASS** |
| REQ-17V-005 | Product tree builds; unit tests pass | **PASS** (8/8) |
| REQ-17V-006 | Live writers for St / SGG / IDG XCBR | **PASS** (St/SGG TotW from plant P; Pos from `io.idg_pos`, bad if unwired) |

---

## Method

1. `python scripts/generate_icd.py --profile erda_wave1_17ln --verify --dry-run`
2. `python lab/tmp/validate_product_17ln.py`
3. WSL CMake configure + build `product/apps/ccli` → `build/wsl-product`
4. `ctest` in that build dir

---

## Static checks (`validate_product_17ln.py`)

```
PASS  classification schema 1.2
PASS  CID has 17 LNs
PASS  CID inventory matches erda_wave1_17ln
PASS  CFG has 17 LN(
PASS  product CID == generated latest XML
PASS  product CFG == generated latest cfg
PASS  no Gen*/Q-stack LNs in inventory
PASS  TimeSyncProt sntp=true present
PASS  Wlim/WSd ctlModel = direct-with-enhanced-security
PASS  3 DataSets + 3 URCBs (4 s) present
PASS  product_tr400.yaml pins 17ln cfg
PASS  product_tr400_emt432_17ln.yaml pins 17ln cfg
PASS  lab_tr400_phase4_eth_b_ttyS1_17ln.yaml pins 17ln cfg
PASS  product mms_adapter wires Wlim/WSd/PdC + control handlers
PASS  transmission_mms publish path present
PASS  StMMXU1.TotW live writer
PASS  SGGMMXU1.TotW live writer
PASS  IDGXCBR1.Pos live writer
SUMMARY ok=18 gap=0 fail=0
```

### Frozen 17 LN

`LLN0` · `LPHD1` · `PdC_WiDPCC1` · `PdC_WaDPCC1` · `PdC_QiDPCC1` · `PdC_QcDPCC1` · `PdC_VADPCC1` · `DisFRDECP1` · `DisFRDGEN1` · `DisFRDSTO1` · `PdCMMXU1` · `StMMXU1` · `SGGMMXU1` · `IDGXCBR1` · `SSGGDGEN1` · `WlimDWMX1` · `WSdDAGC1`

---

## Code / test map (booked UCA blocks)

| Block | Needed for pass | Product code | Static | Live DUT |
|-------|-----------------|--------------|--------|----------|
| 1a Directory | All 17 browseable | cfg loader + CID | **PASS** | OPEN |
| 2 DataSets | 3 DS + FCDA members | CID | **PASS** | OPEN |
| 5 URCB 4 s | 3 URCBs IntgPd=4000 | CID + libiec61850 | **PASS** model | OPEN (lab Δt) |
| 5 Report values | PdC live | `transmission_mms` → PdCMMXU1 | **PASS** | OPEN |
| 5 St/SGG values | Storage P updates | `update_tot_w_kw` writes St and SGG from plant P | **PASS** | OPEN |
| 12c Operate | Wlim + WSd enhanced direct | `mms_adapter` handlers | **PASS** | OPEN |
| 13a SNTP | sntp=true + clock | CID + time worker | **PASS** model | OPEN |

---

## Build evidence

| Check | Result |
|-------|--------|
| Configure | `cmake -S product/apps/ccli -B build/wsl-product` exit 0 · entry `program/main.c` |
| Build | `cmake --build build/wsl-product` exit 0 |
| Unit tests | **8/8 passed** (`pf2_fsm`, `dso_phase1`, `setpoint_gate`, `service_supervision`, `event_store`, `io_safe_state`, `program_queue_my`, `mms_acse_auth`) |

---

## Verdict

**Product code + 17-LN model: VALIDATED for structure, control bindings, and the three live writers.**  
`StMMXU1.TotW` and `SGGMMXU1.TotW` follow the plant meter (one storage group). `IDGXCBR1.Pos.stVal` follows `io.idg_pos`; with the contact disabled the published value is Dbpos bad.  
**Not claimed:** DUT TSP pass of booked ERDA blocks. A second storage meter is not in this specimen.

---

## Keywords

`17-LN`, `erda_wave1_17ln`, `product_tr400`, `validate_product_17ln`, `WlimDWMX1`, `WSdDAGC1`, `PdCMMXU1`
