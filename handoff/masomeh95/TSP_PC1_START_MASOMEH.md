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

**Full steps:** [`PC1_GITHUB_PULL_RUNBOOK.md`](PC1_GITHUB_PULL_RUNBOOK.md)

```powershell
cd D:\CCLI\CCLI\CCLI
git pull origin main
powershell -File scripts\ensure-lab-env.ps1
powershell -File scripts\source-lab-env.ps1

# P3-07 Connect session (matches DUT p3_07_connect, crl=off):
powershell -File scripts\setup-pc1-p3-07-connect.ps1 -Dest D:\CCLI_TSP_FIELD_PACK
powershell -File scripts\verify-lab-access-freeze.ps1 -TlsDir D:\CCLI_TSP_FIELD_PACK\tls
```

Read **`00t_PC1_FIX_WRONG_TRANSPORT_CERT.md`** then **`00r_PC1_P3_07_CONNECT_PROFILE_ACTIVE.md`** in the pack.

**Normative book (clause → code Lines):** `lab/conformance/Full Validate.md` after `git pull` · pack copy **`00_FULL_VALIDATE.md`**.

**Unified 5-LN / EJBCA matrix** (different TLS tree): use **`setup-pc1-testpro-from-git.ps1`** and **`00k_PC1_EJBCA_TLS_TESTPRO.md`** instead.

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
| 6 | **`00i_PC1_TSP_IMPORT_CHECKLIST.md`** — **CID + TLS paths in Test Suite Pro** |
| 7 | **`00f_PC1_HW_CERT_PREFLIGHT.md`** — TLS, CID, LAN1, O.15 docs + PC-A plant coordination |
| 8 | **`00h_PC1_LOG_CAPTURE_REQUIREMENTS.md`** — **mandatory** Wireshark + 4 artefacts per row |
| 9 | **`00g_TSP_5LN_EXECUTION_AND_LOGS.md`** — TestPro step order + logger `-TestId` map |

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

## 7. Evidence (every T1 / T3 / P5-01 step)

**Full rules:** open **`00h_PC1_LOG_CAPTURE_REQUIREMENTS.md`** in the pack (REQ-PC1-LOG-001 … 007).

**Install on PC1:** Wireshark (tshark) · optional PuTTY plink/pscp for DUT SSH logs.

Run logger from **field pack root** or **git clone** (same relative paths):

```powershell
cd D:\CCLI_TSP_FIELD_PACK   # or D:\CCLI\CCLI after git clone

$dt = Get-Date -Format yyyy-MM-dd
powershell -File scripts\tsp-evidence-logger.ps1 -Action Init -SessionDate $dt -Operator Masomeh -TspVersion "<TSP build>"

powershell -File scripts\tsp-evidence-logger.ps1 -Action Start -TestId T1-01 -Operator Masomeh
# ... exactly one TSP step; copy Output ...
powershell -File scripts\tsp-evidence-logger.ps1 -Action Stop -PasteClipboard
```

Outputs:

```text
lab\evidence\testsuite-pro\inbox\   ← TSP_*.txt, *_DUT_LOG, *_WIRE
lab\evidence\testsuite-pro\pcap\      ← TSP_*.pcapng
```

Return to Yahia: **U-CLO-01** — robocopy `inbox` + `pcap` → repo `lab/evidence/testsuite-pro/` (USB OK).

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
