# Test Suite Pro — PC1 (Masomeh) start guide

**Role:** **PC-B** in [TSP_TWO_PC_BENCH_RUNBOOK.md](../../lab/conformance/TSP_TWO_PC_BENCH_RUNBOOK.md) — **LAN1 only** · Test Suite Pro · **no** plant/104 on this PC.

**Date:** 2026-10-14  
**Contact:** Yahia (project channel)

---

## 1. Get the field pack (pick one)

### A — Yahia pushed to this PC (fastest)

Folder should exist:

```text
D:\CCLI_TSP_FIELD_PACK\
```

(or `C:\CCLI_TSP_FIELD_PACK\` — same contents)

### B — GitHub (recommended): pull repo on PC1, then stage pack

```powershell
# First time on PC1:
git clone https://github.com/yahiagabereng-ctrl/CCLI.git
cd CCLI

# Every session (get Yahia's latest matrices + scripts):
git pull origin main

# Build USB-free field pack locally:
powershell -File scripts\stage-tsp-field-pack.ps1 -Dest D:\CCLI_TSP_FIELD_PACK
```

TLS is included under `D:\CCLI_TSP_FIELD_PACK\tls\` after stage. No separate USB copy needed if git pull succeeds.

---

## 2. TLS for Test Suite Pro

Point TSP **Connection / TLS** at PEMs in the pack:

```text
D:\CCLI_TSP_FIELD_PACK\tls\
```

Optional copy to TSP default browse folder:

```powershell
powershell -File scripts\stage-tsp-tls.ps1 -ProductDest "D:\CCLI_TSP_FIELD_PACK\tls"
# In TSP UI: trust + client cert from that folder (DSO = client.pem)
```

---

## 3. LAN1 (before Connect)

Run **as Administrator** (from pack):

```text
D:\CCLI_TSP_FIELD_PACK\bench_scripts\set_pc_lan1_dso.cmd
```

| Item | Value |
|------|--------|
| This PC (PC1) | `192.168.10.10` |
| DUT MMS TLS | `192.168.10.1:3782` |
| IED name | `CCI016_01` |

```powershell
ping 192.168.10.1
```

**Do not** run Test Suite Pro on the other PC (plant/logger). **Do not** bridge DSO traffic with operator LAN casually.

---

## 4. Open these files (master checklist)

| Order | File in pack |
|-------|----------------|
| 1 | `00_UNIFIED_CERT_TEST_MATRIX.md` |
| 2 | `00b_UNIFIED_CERT_ROADMAP.md` |
| 3 | `00c_PRODUCT_SPECIMEN_5LN.md` |
| 4 | `00e_P5_FULLCIRCLE_5LN_SCOPE.md` |
| 5 | `02_TSP_TEST_IDENTIFICATION.md` (Tier 1 / 3 detail) |
| 6 | **`00f_PC1_HW_CERT_PREFLIGHT.md`** — TLS, CID, LAN1, O.15 docs + PC-A plant coordination |

Excel: `00_UNIFIED_CERT_TEST_MATRIX.csv`

---

## 5. Preflight (hardware + certificates)

Open **`00f_PC1_HW_CERT_PREFLIGHT.md`** in the pack and complete section **A** on PC1 before **Connect**.

- TLS: **`tls\`** PEMs (mutual TLS to `192.168.10.1:3782`)
- CID: **`cid\product_tr400_eth_a_5ln.cid`** when present; run **`scripts\validate-5ln-cid.ps1`** from pack or repo
- Bench map: **`08_TSP_PC_FULL_BENCH_HW.md`** · plant/meter/Sun details in **`00_TWO_PC_BENCH_RUNBOOK.md`**
- Plant tools (for PC-A, copied in pack **`tools\`**): `modbus_emt432_tcp_probe.py`, `start-bench-plant.ps1`

---

## 6. TSP project setup

1. Import CID from `cid\` — **cert run:** use `product_tr400_eth_a_5ln.cid` when present; until then read `00d_PRODUCT_SPECIMEN_5LN_VALIDATION.md` (Compare blocked for 5-LN until product CID exists).  
2. **Lab interim only:** `cid\lab_tg544_eth_a.cid` (31-LN — **not** for O.15 cert Compare).  
3. Connect: `192.168.10.1:3782` · mutual TLS · DSO client cert.

---

## 7. Evidence (every T1 / T3 step)

From repo clone (or copy `scripts\` from pack):

```powershell
cd D:\CCLI\CCLI   # if full repo on PC1
powershell -File scripts\tsp-evidence-logger.ps1 -Action Init -SessionDate 2026-10-14
powershell -File scripts\tsp-evidence-logger.ps1 -Action Start -TestId T1-01 -Operator Masomeh
# ... run one TSP step ...
powershell -File scripts\tsp-evidence-logger.ps1 -Action Stop -PasteClipboard
```

Save logs under a folder e.g. `D:\CCLI_TSP_EVIDENCE\inbox\` — USB or git to Yahia → `lab/evidence/testsuite-pro/inbox/`.

Pack includes: `scripts\tsp-evidence-logger.ps1` if staged from repo.

---

## 8. Phase order (PC1 only)

1. Phase 0 — LAN1 + TLS + CID import (`00b` roadmap)  
2. Phase 1 — **T1-01 … T1-10** (matrix rows U-T1-*)  
3. Phase 2 — **P5 5-LN scope** (`00e`)  
4. Phase 3 — **T3-01 … T3-05** (cyber)

Yahia / other PC runs plant (COM5, meter, 104, SSH) in parallel when needed.

---

## 9. RAG (on dev PC — optional)

Yahia can query: `ccli-lab-unified-cert-test-matrix` · `ccli-tsp-two-pc-bench-runbook`

---

**Keywords:** Masomeh, PC1, TSP, field-pack, unified matrix
