# Test Suite Pro PC — field pack (preview & push)

**Document ID:** CCLI-LAB-TSP-PACK-001  
**Revision:** 1.1  
**Date:** 2026-10-09  
**RAG source_id:** `ccli-tsp-field-pack`  
**Script:** `scripts/stage-tsp-field-pack.ps1`  
**Master checklist:** [LAB_UNIFIED_CERT_TEST_MATRIX.md](LAB_UNIFIED_CERT_TEST_MATRIX.md) · **5-LN run:** [PRODUCT_SPECIMEN_5LN.md](PRODUCT_SPECIMEN_5LN.md) · **31-LN lab pin:** [LAB_SPECIMEN_VERSION.md](LAB_SPECIMEN_VERSION.md)

---

## One command (repo PC)

```powershell
cd C:\Yahia\projects\CCLI
powershell -File scripts\stage-tsp-field-pack.ps1
# Optional: -Dest D:\USB\CCLI_TSP_FIELD_PACK
```

Creates **`C:\CCLI_TSP_FIELD_PACK\`** with checklist, procedures, CID, TLS, and templates reference.

---

## Copy to the other device

1. Plug USB → copy entire **`CCLI_TSP_FIELD_PACK`** folder.  
2. On TSP PC: paste e.g. `D:\CCLI_TSP_FIELD_PACK`.  
3. Run **`stage-tsp-tls.ps1`** is already reflected in **`tls\`** subfolder — point TSP at **`tls\`** (same as `CCLI_product_tls`).  
4. Open **`00_UNIFIED_CERT_TEST_MATRIX.md`** + **`00b_UNIFIED_CERT_ROADMAP.md`** (print or second monitor).  
5. Import CID in TSP from **`cid\product_tr400_eth_a_5ln.cid`** when present; else see **`00c_PRODUCT_SPECIMEN_5LN.md`**.

---

## Pack contents (after stage script)

| Path in pack | Role |
|--------------|------|
| `00_UNIFIED_CERT_TEST_MATRIX.md` | **Unified** functional + cyber + O.15 □ matrix |
| `00_UNIFIED_CERT_TEST_MATRIX.csv` | Same rows for Excel |
| `00b_UNIFIED_CERT_ROADMAP.md` | Two-PC phase order (TSP + meter + RS485) |
| `00c_PRODUCT_SPECIMEN_5LN.md` | 5-LN product pin |
| `01_CHECKLIST_LAB_REQUIREMENT_TEST_MATRIX.md` | Legacy functional detail |
| `02_TSP_TEST_IDENTIFICATION.md` | Tier 1–4 detail |
| `03_TSP_SEQUENCER_DRAFT.md` | Step objects / MMS paths |
| `04_TSP_VERIFICATION_LAYER.md` | Install paths · evidence naming |
| `05_EVIDENCE_README.md` | `TSP_*` filename rules |
| `cid/lab_tg544_eth_a.cid` | Compare Model / MICS source (must match **rev 9** pin) |
| `tls/*` | EJBCA lab PEMs + `TSP_STAGE_README.txt` |
| `config/lab_tr400_phase1_regulation.yaml` | Reference ports 3782 |
| `config/lab_tr400_phase4_emt432_lan3_tcp.yaml` | LAN3 EMT432 TCP + PPV reg 980 |
| `00f_PC1_HW_CERT_PREFLIGHT.md` | PC1 TLS/CID/HW + PC-A plant coordination |
| `tools/modbus_emt432_tcp_probe.py` | Meter LAN3 probe (PC-A) |
| `tools/start-bench-plant.ps1` | EMT432 probe + Sun mock launcher |
| `scripts/validate-5ln-cid.ps1` | 5-LN CID gate before Compare |
| `09_PICS_FILL.md` … `16_LAB_SPECIMEN_VERSION.md` | PID fill packs + **CID/cfg rev pin** (SHA-256) |
| `PACK_MANIFEST.json` | Generated timestamp + file list |

---

## Bench constants (TSP PC)

**Full triple-NIC + USB RS485:** [TSP_PC_FULL_BENCH_HW.md](TSP_PC_FULL_BENCH_HW.md)

| Item | Value |
|------|--------|
| DUT LAN1 (DSO) | `192.168.10.1` · MMS **3782** TLS |
| PC LAN1 | `192.168.10.10` → **Test Suite Pro** |
| DUT LAN2 (OA) | `192.168.1.130` · 104 **2404** TLS |
| PC LAN2 | `192.168.1.183` → 104 client (same PC OK) |
| DUT LAN3 (plant) | `192.168.30.1` |
| PC LAN3 | `192.168.30.10` → GOOSE / plant |
| USB RS485 | **COM5** → **SUN2000** RTU sim (`tools/start-bench-plant.ps1` in pack) |
| IED | `CCI016_01` · Server = DUT · Client = TSP |

After `git clone` on TSP PC: run `stage-tsp-field-pack.ps1` from repo root.

---

## RAG on dev PC (while TSP PC runs offline)

Query examples:

- *ccli-lab-requirement-test-matrix sCtl Wlim*  
- *ccli-61850-10-extract Table 14 URCB*  
- *ccli-lab-uca-document-checklist D1 PICS*

After session: copy logs back → `ingest-testsuite-pro.ps1`.

---

**RAG tags:** `TSP`, `field-pack`, `ccli-tsp-field-pack`
