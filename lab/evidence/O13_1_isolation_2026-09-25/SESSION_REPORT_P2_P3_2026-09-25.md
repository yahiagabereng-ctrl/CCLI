# Session report — Phase 2 isolation close-out → Phase 3 Steps 1–3

**Date:** 2026-09-25  
**DUT:** TesPro TG544 / TR500 (`platform_tg500`) · OpenWrt  
**Programme:** HiTEKS CCLI · CEI 0-16 Allegato O / T · IEC 62443 zones  
**Audience:** reuse for lab handoff, checklist evidence, and next MMS work

---

## 1. Verdict (one page)

| Gate | Result | Evidence |
|------|--------|----------|
| **P2-01** role PC↔PC isolation | **PASS** | `lab/evidence/O13_1_isolation_2026-09-25/P2_01_CROSS_PING_MATRIX.txt` |
| **P2-02** separate L3 + no role forward | **PASS** | same + firewall verify log |
| **P2-03** LTE/WWAN ↛ DSO Eth_A | **PASS** | `P2_03_LTE_ISOLATION.txt` |
| **P2 hard gates** | **CLOSED** | P2-04 / P2-06 remain PART/OPEN (non-blocking) |
| **P3 Step 1** LAN1 / Eth_A access | **PASS** | PC `192.168.10.10` → LuCI `http://192.168.10.1` |
| **P3 Step 2** lab CID IP | **PART (P3-02)** | `lab_tg544_eth_a.cid` @ `192.168.10.1/24` |
| **P3 Step 3** object map freeze | **PART (P3-12)** | `apps/ccli/config/icd/signal_map.yaml` |
| **P3 Step 5** client + 4 s TotW | **PASS (P3-03)** | avg 3961 ms · `P3_03_MMS_CLIENT_STEP5.txt` |

Phase 3 **exit** (P3-01, P3-04, P3-06) = **PASS** as of 2026-09-25. Soft items (P3-07+) remain.

---

## 2. As-built network (TG544)

| Port | Role | UCI IF | IP | Zone |
|------|------|--------|-----|------|
| LAN1 | DSO Eth_A | `lan1` | **192.168.10.1/24** | `lan1_sec` |
| LAN2 | Operator Eth_B | `lan2` | 192.168.1.130/24 | `lan_sec` |
| LAN3 | Plant | `lan3` | 192.168.30.1/24 | `lan2_sec` |
| ENG | Local config | `br-lan` | 192.168.0.1/24 | `lan` (no lan4 PHY on TR500) |
| WAN | LTE backup | `usb0` (EC200U) | e.g. 10.43.46.209 | `wan` |

**Diagrams / docs:**  
`Architecture/Zones_62443_TG544_PortMap.drawio` · `knowledge-base/08-engineering/CCI_62443_Zones.md` · `lab/evidence/.../P2_ANNEX_PORT_MAP.txt`

**Lab PC helpers:**  
`lab/set_pc_lan1_oa.cmd` · `lab/set_pc_lan2_oa.cmd` (static role IPs; UAC may require manual set)

**Isolation rule (important):** P2-01 pass = **role PC cannot ping other-role PC**. Ping of TG gateway IPs on other LANs may still answer INPUT — that is **N/A**, not fail.

---

## 3. Phase 2 work done this session

### 3.1 Isolation matrix
- Confirmed firewall / zone peel (`lan1_sec` / `lan_sec` / `lan2_sec`).
- Operator matrix: DSO PC, OA PC, Plant PC — cross-role **FAIL**, own path **PASS**.
- Hardened notes: lan1 **/24** (was /32), Wi-Fi off, role masq=0.

### 3.2 LTE backup (P2-03)
- **2G SIM failed** for data path.
- **1NCE 4G** SIM + APN **`iot.1nce.net`**.
- LuCI alone left APN stuck on `internet.it`; **SSH** fix: TesPro `sim_manager` uses **`apn_user`** (not only `apn` / Current).
- Evidence: `P2_LTE_APN_SSH_FIX.txt`, then `P2_03_LTE_ISOLATION.txt`:
  - `ping 8.8.8.8` PASS (LTE up)
  - `ping -I usb0 192.168.10.1` / `.10` **100% loss** PASS (no LTE→DSO)

### 3.3 Still open (Phase 2 soft)
| ID | Status | Note |
|----|--------|------|
| P2-04 | PART | LuCI still reachable on OA `.1.130`; eng-only not enforced |
| P2-05 | PART | Annex map done; ZCR-7 asset-owner sign-off missing |
| P2-06 | OPEN | Eth link-status logging |
| — | optional | Rename UCI `lan2_sec` → `plant`; 1NCE VPN |

---

## 4. Phase 3 Steps 1–3 (DSO MMS)

### Step 1 — Reach Eth_A
- PC on LAN1: **192.168.10.10**
- TG: **192.168.10.1** — LuCI OK

### Step 2 — Lab CID
| Item | Value |
|------|--------|
| Reference (unchanged) | `apps/ccli/config/icd/cei-tr-57-126-example.cid` (was 192.168.8.167) |
| Lab CID | `apps/ccli/config/icd/lab_tg544_eth_a.cid` |
| IED | `CCI016_01` |
| IP / mask / GW | **192.168.10.1** / 255.255.255.0 / 192.168.10.1 |
| Evidence | `P3_02_LAB_CID_ETH_A.txt` |

### Step 3 — Frozen object map
File: **`apps/ccli/config/icd/signal_map.yaml`**  
Evidence: `P3_03_SIGNAL_MAP.txt`

| Function | Object | Clause |
|----------|--------|--------|
| Limit P | `Wlim` DWMX · `WMaxSptPct` | T.3.1.4 · O.9.2.2 |
| Enable | `Wlim` · `Mod` (1=Active, 5=Inactive) | T.3.1.4 |
| PdC P | `PdC` MMXU · `TotW` | T.3.1.3 |
| 4 s report | `urcb_PdC_Mis4sec` · `intgPd=4000` | T.3.2.1 |

**Runtime intent:** MMS `Wlim` → same `DsoActivePowerCommands` / PF2 → **DIO1** path as Phase 1 mock.

**Deferred in freeze:** WSd/VAr* MMS, TLS/62351, plant DSO workbook, live server.

### Supplier note (do not confuse)
Vendor IEC 61850 zip analysed earlier = **MMS client / gateway APK**, **not** Allegato T server for CCLI. Lab path remains **libiec61850 server** + TR 57-126 CID.

---

## 5. Checklist snapshot (2026-09-25)

**Source of truth:** `knowledge-base/08-engineering/CCI_Phase_Regulation_Checklists.md`

| Check | Status |
|-------|--------|
| P2-01, P2-02, P2-03 | **PASS** |
| P2-04 | **PART** |
| P2-05 | **PART** |
| P2-06 | **OPEN** |
| P3-01 MMS listen Eth_A | **PASS** |
| P3-02 lab CID | **PART** |
| P3-03 4 s TotW on wire | **PASS** |
| P3-04 Wlim → actuator | **PASS** — `SESSION_REPORT_P3_STEP7_P304_2026-09-25.md` |
| P3-06…09 TLS/PKI/RBAC | **OPEN** |
| P3-12 signal_map.yaml | **PART** (file present; DSO workbook MISSING) |

---

## 6. Artifact index (copy/paste)

```
lab/evidence/O13_1_isolation_2026-09-25/
  P2_01_CROSS_PING_MATRIX.txt
  P2_03_LTE_ISOLATION.txt
  P2_LTE_APN_SSH_FIX.txt
  P2_ANNEX_PORT_MAP.txt
  P3_02_LAB_CID_ETH_A.txt
  P3_03_SIGNAL_MAP.txt
  SESSION_REPORT_P2_P3_2026-09-25.md   ← this file

apps/ccli/config/icd/
  cei-tr-57-126-example.cid            # reference — do not edit
  lab_tg544_eth_a.cid                  # lab Eth_A IP
  signal_map.yaml                      # Step 3 freeze
  README.md

Architecture/Zones_62443_TG544_PortMap.drawio
knowledge-base/08-engineering/CCI_Phase_Regulation_Checklists.md
knowledge-base/08-engineering/CCI_62443_Zones.md
```

Pointers:  
`lab/evidence/P2_01_CROSS_PING_MATRIX_LATEST.txt` · `lab/evidence/P2_03_LTE_ISOLATION_LATEST.txt`

---

## 7. Recommended next steps (in order)

1. **Step 7 — Wlim write path** — SBO `Mod` + `WMaxSptPct` → PF2 → DIO1 (P3-04).
2. Optional: non-zero TotW from Modbus for live P in reports.
3. **Security track** — TLS 1.2+ / 62351 (P3-06…09).
4. Soft P2 — eng-only LuCI (P2-04), link logging (P2-06).
5. Optional: Java genconfig → load full `lab_tg544_eth_a.cid` (replace dynamic MVP).

---

## 8. Corpus status (HAVE / MISSING)

| Item | Status |
|------|--------|
| Allegato T / TR 57-126 extracts | HAVE |
| TR 57-126 example CID | HAVE |
| Lab CID + signal_map (MVP) | HAVE (lab) |
| Plant-specific DSO ICD workbook | **MISSING** |
| 61850-8-1:2011 Ed.2 PDF | MISSING (2004 PART) |
| Live MMS server on TG544 | **HAVE (lab)** — 2026-09-25 P3-01 PASS · dynamic MVP |

---

*End of report — suitable for email / lab book / PR description.*
