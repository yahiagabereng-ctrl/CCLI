# Phase 4 kickoff — Operator 104 (Eth_B)

**Date:** 2026-09-30  
**Prior phase:** Phase 3 **CLOSED** (lab) 2026-09-25  
**Canonical:** `CCI_Phase_Regulation_Checklists.md` · `CCLI_PHASE_LAB_MASTER_LOG.md`

---

## Phase 3 — last documented install & tests

### Official lab close (2026-09-25)

| Item | Value |
|------|--------|
| Package | **`ccli-0.1.0-r15`** (progression r5→r6→r9→r15) |
| DUT | TG544 · Eth_A **`192.168.10.1:3782`** TLS |
| Config | `lab_tr400_phase1_regulation.yaml` |
| Close report | `lab/evidence/O13_1_isolation_2026-09-25/SESSION_REPORT_P3_LAB_CLOSE_2026-09-25.md` |

### Gate board (official close)

| ID | Status | Evidence |
|----|--------|----------|
| P3-01 MMS Eth_A | **PASS** | `P3_01_MMS_LISTEN_ETH_A.txt` |
| P3-03 TotW ~4 s | **PASS** | `P3_03_MMS_CLIENT_STEP5.txt` |
| P3-04 Wlim→DIO1 | **PASS** | `P3_04_WLIM_ACTUATOR.txt` |
| P3-05 Comms fallback | **PASS** | `P3_05_COMMS_LOSS_FALLBACK.txt` |
| P3-06 TLS :3782 | **PASS** | `P3_06_MMS_TLS_ETH_A.txt` |
| P3-07 Security assoc | **PASS** (lab) | `P3_07_SECURITY_ASSOC.txt` |
| P3-08 RBAC | **PASS** | `P3_08_SECURITY_RBAC.txt` |
| P3-09 CA+CRL | **PASS** (lab) | `P3_08_09_RBAC_PKI.txt` |
| P3-11 TimeQuality | **PASS** | `P3_11_CHRONY_TRACKING.txt` |
| P3-02/12/13 | **PART** | lab CID + signal_map MVP |
| P3-10/14 | **WAIVED** | product / corpus |

**Exit gates P3-01 + P3-04 + P3-06:** all **PASS** → Phase 3 lab **CLOSED**.

### Post-close engineering (2026-09-29) — TSP P3-07 deep test

Not required for Phase 3 gate re-open; documented for TMW ticket / WAIVE.

| Item | Value |
|------|--------|
| Deploy | `deploy-ccli-session-fix.ps1` + **Solution C** yaml (`server_combined.pem`) |
| DUT binary | `/usr/sbin/ccli` ~7.5 MB (session LI + ACSE fixes; not necessarily r16 opkg) |
| TSP | Still **FAIL** `Unable to verify signature (+)` |
| Offline Step 7 | **PASS** `time_tlv_81` on live TX_ACCEPT hex |
| Wire | `pcap/TSP_P3_07_mms_2026-09-29_222559.pcapng` — AARE → Alert **2.6 ms** |
| Evidence | `lab/evidence/testsuite-pro/inbox/TSP_P3_07_WIRE_EVIDENCE_2026-09-29_222559.md` |

**Programme stance:** Phase 3 remains **CLOSED**; TSP A-profile interop = **residual / TMW** (WAIVE path available).

---

## Phase 4 scope

| ID | Requirement | Target |
|----|-------------|--------|
| **P4-00** | TesPro supplier **IEC 61850** add-on | 4× apk installed; mmsd + service **running**; optional plant poll LAN3 |
| **P4-01** | 104 on **Eth_B only** | No listener on Eth_A |
| **P4-02** | 60870-5-104 session | STARTDT, GI, reconnect |
| **P4-03** | 62351-3/5 secure 104 | TLS (+ 62351-5 when scoped) |
| P4-04 | Eth_B comms-loss fallback | Timer + restore |
| P4-05 | Log operator commands | O.14 |

**CEI core stays Eth_A MMS** — 104 does not replace Annex T.

---

## As-built Eth_B (from Phase 2)

| Port | Role | DUT IP | PC (lab) |
|------|------|--------|----------|
| **LAN1** | DSO Eth_A | `192.168.10.1` | `192.168.10.10` — **MMS/TSP** |
| **LAN2** | Operator Eth_B | **`192.168.1.130`** | `192.168.1.x` (DHCP .150–.199) |
| LAN3 | Plant | `192.168.30.1` | — |

**Phase 4 lab:** move operator PC cable to **LAN2** for 104 tests; keep MMS work on LAN1.

---

## P4-00 — TesPro supplier IEC 61850 (2026-09-30)

| Item | Status |
|------|--------|
| apk install (LuCI offline upload) | **PASS** — all 4 packages |
| `iec61850-mmsd` + `iec61850d` | **PASS** — running |
| LuCI service **Running** | **OPEN** — confirm in browser |
| Plant device + northbound | **OPEN** |
| `ccli` coexistence (V-005) | **OPEN** — ccli not running at SSH check |

Evidence: `P4_00_TESPRO_61850_SUPPLIER_SW.md` · `P4_00_TESPRO_61850_SSH_VERIFY.txt`  
Script: `lab/tg544-openwrt/check-tespro-61850-remote.ps1`

**Not CEI Eth_B gate** — supplier MMS **client/collector** only; DSO MMS stays **`ccli` :3782**.

---

## Code / corpus status (Phase 4 start)

| Item | Status |
|------|--------|
| `iec104_adapter.cpp` | **STUB** (`start()` returns false) |
| `lib60870` in CMake | **Not linked** yet |
| `ccli_config` Iec104 section | **Missing** |
| `CCI_60870-5-104_Extract.md` | **HAVE** |
| `62351-3/5` extracts | **HAVE** |

---

## Phase 4 — step plan (order)

### Step −1 — TesPro IEC 61850 supplier SW (P4-00) — **DONE (install)**

1. LuCI upload 4× apk in dependency order (see `P4_00_TESPRO_61850_SUPPLIER_SW.md`).
2. SSH verify: `check-tespro-61850-remote.ps1` → **PASS** 2026-09-30.
3. **Remaining:** LuCI **Start** confirm · optional LAN3 plant device · coexistence with `ccli`.

### Step 0 — Lab network (P4-01 prep)

```powershell
# PC on LAN2 after cable move
ipconfig
ping 192.168.1.130
# Confirm NO 104 on Eth_A (after implementation):
# ssh: netstat -tlnp | grep 2404  → must not show 192.168.10.1:2404
```

### Step 1 — Implement minimal CS104 slave (P4-02)

- Add `Iec104Config` to yaml (`bind: 192.168.1.130`, port **2404**)
- Wire `Iec104Adapter` + lib60870 CS104 slave in `ccli_main`
- Publish at least one monitor point (e.g. TotW analogue)

### Step 2 — Client test (P4-02)

- lib60870 `cs104_client` example or QTester from PC on LAN2
- PASS: STARTDT act/con, periodic data, reconnect after TCP drop

### Step 3 — Isolation (P4-01)

- Re-run cross-ping: Eth_A ↔ Eth_B blocked (reuse P2 matrix)
- Confirm `:2404` LISTEN only on `192.168.1.130`

### Step 4 — TLS (P4-03) — after cleartext PASS

- TLS wrapper per 62351-3 on 104 port (lab PEMs)
- 62351-5 STAS deferred unless required for gate

---

## Immediate actions (today)

1. **Read** this kickoff + `knowledge-base/08-engineering/CCI_60870-5-104_Extract.md` Batch A–E  
2. **Cable** PC to **LAN2** when starting 104 (not needed for MMS on LAN1)  
3. **Confirm DUT** still on Solution C for MMS (`192.168.10.1:3782`) — independent of 104 work  
4. **Implement** Phase 4 Step 1 in `apps/ccli` (next coding session)  
5. **Optional parallel:** TMW ticket + IEDScout trial (P3 residual) — does not block P4

---

## Evidence folder

Save Phase 4 artifacts under: **`lab/evidence/phase4/`**

| File (planned) | Gate |
|----------------|------|
| `P4_00_TESPRO_61850_SSH_VERIFY.txt` | P4-00 |
| `P4_00_TESPRO_61850_SUPPLIER_SW.md` | P4-00 runbook |
| `P4_01_ETH_B_ONLY.txt` | P4-01 |
| `P4_02_104_STARTDT.txt` | P4-02 |
| `P4_03_104_TLS.txt` | P4-03 |
