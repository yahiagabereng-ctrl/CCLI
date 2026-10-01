# IEDScout trial — P3 multi-tool (Phase D)

**Date:** 2026-09-29  
**DUT:** `192.168.10.1:3782` · **CCI016_01** · `lab_tg544_eth_a.cid`  
**Companion:** `lab/CCI_MMS_MultiTool_Verification.md`

---

## Purpose

Independent **MMS client** check (TLS + browse). **Does not** replace Test Suite Pro for 62351-4 §11.2.2 Step 7.

---

## Setup

1. Install [IEDScout trial](https://www.omicronenergy.com/en/products/iedscout/) (30 days).
2. PC on **LAN1** — `192.168.10.x`.
3. Import **`apps/ccli/config/icd/lab_tg544_eth_a.cid`**.
4. TLS: `C:\CCLI_tls\` — `root_CA.pem`, `client.pem`, client key (same as TSP).

---

## Tests

| # | Action | Record |
|---|--------|--------|
| 1 | Connect `192.168.10.1:3782` | PASS / FAIL + error text |
| 2 | Read `LLN0.Mod.stVal` or `PdCMMXU1.TotW` | Value |
| 3 | Note security UI | TLS only vs MMS Security / 62351-4 options |
| 4 | Optional write `WlimDWMX1.WMaxSptPct` | If enabled in lab |

---

## Interpret vs TSP

| IEDScout | TSP | Meaning |
|----------|-----|---------|
| PASS | FAIL sig | DUT OK; TSP A-profile delta |
| FAIL TLS | FAIL | Fix transport/certs first |
| PASS | PASS | Full interop |

---

## Evidence file

Save log as: `TSP_P3_06_IEDSCOUT_CLIENT_YYYY-MM-DD.txt`
