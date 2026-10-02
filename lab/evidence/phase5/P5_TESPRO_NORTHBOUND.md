# P5 — TesPro IEC 61850 northbound (MQTT collector)

**Document ID:** P5-TESPRO-NB-001  
**Date:** 2026-10-01  
**Gate:** P5_FULLCIRCLE (parallel track — **not** Annex T DSO path)  
**DUT:** TG544 · TesproOS 2.1.0 r601 · `iec61850d` + `iec61850-mmsd`  
**Southbound target:** `ccli` @ `192.168.10.1:102` cleartext  
**Manual:** [CCI_TesPro_61850_Manual_Extract.md](../../../knowledge-base/08-engineering/CCI_TesPro_61850_Manual_Extract.md)  
**Annex sequence:** [P5_ANNEX_TEST_SEQUENCE.md](P5_ANNEX_TEST_SEQUENCE.md) Part H

---

## 1. Role in lab architecture

```text
Modbus COM5 ──► ccli (MMS server :102) ◄── TesPro iec61850d (MMS client)
                      │                              │
                      │                              ▼
            TSP / IEDExplorer / mms_lab_client   MQTT northbound
               (Annex T lab gate)                   │
                                                    ▼
                                            PC broker 192.168.10.10:1883
                                            (SCADA / analytics / evidence)
```

| Path | Role | Annex |
|------|------|-------|
| **ccli :102 / :3782** | DSO-facing MMS **server** | **T.3.2.1**, **T.3.1.3**, **O.8.3** |
| **TesPro collector** | Plant MMS **client** + optional upload | Supplier feature (P4-00); **cross-check** only |
| **MQTT northbound** | JSON property reports | Out of CEI scope; lab observability |

**Rule:** TesPro does **not** replace TSP for P5 gate sign-off. Northbound proves **coexistence** + **value chain** (Modbus → ccli → TesPro → MQTT).

---

## 2. Preconditions

- [x] P4-00: four `iec61850-*` apk installed; `iec61850d` + `iec61850-mmsd` running  
- [x] P5 cleartext: `ccli` listening **`0.0.0.0:102`** (hairpin to `192.168.10.1`)  
- [x] Modbus slave P=450 kW on COM5 · **A1/B1** → `/dev/ttyS1` ([`P5_A1B1_RS485_HW_MAP.md`](P5_A1B1_RS485_HW_MAP.md))  
- [x] PC MQTT broker on **`0.0.0.0:1883`** (see §5)  
- [x] Windows firewall allows inbound **1883** from DUT `192.168.10.1`

LuCI: **Services → IEC 61850 Protocol** → service **Running** (green).

---

## 3. Device configuration (southbound → ccli)

**Tab:** Device Configuration → device **`CCLI_LAB`**

| Field | Value | Notes |
|-------|-------|-------|
| Device Name | `CCLI_LAB` | Display only |
| Device ID | `cci016-lab-01` | → `${device_id}` in MQTT JSON |
| IED Host | `192.168.10.1` | Same DUT Eth_A |
| MMS Port | `102` | Cleartext lab |
| ICD/CID Path | `/tmp/iec61850_icd/lab_tg544` | Upload `lab_tg544_eth_a.cid` if missing |
| RCB | `LD_Plant/LLN0.RP.urcb_PdC_Mis4sec01` | Matches ccli URCB; IntgPd=4000 |

**Save Device** → green dot on device row when MMS connects.

### 3.1 Data points (leaf paths — mandatory)

Delete/disable auto-generated **533** DO-level points. Keep **9** P5 points:

| Report Key | Object Reference (leaf) | FC | Interval (s) | Pass @ baseline |
|------------|---------------------------|-----|-------------:|-----------------|
| `PdC_TotW` | `CCI016_01LD_Plant/PdCMMXU1.TotW.mag.f` | MX | **4** | **450.0** kW |
| `PdC_TotVAr` | `…/PdCMMXU1.TotVAr.mag.f` | MX | **4** | **45.0** kvar |
| `PdC_PPV_AB` | `…/PdCMMXU1.PPV.phsAB.cVal.mag.f` | MX | **4** | **20.0** kV |
| `Wlim_Mod` | `…/WlimDWMX1.Mod.stVal` | ST | 60 | 5 |
| `Wlim_WMax` | `…/WlimDWMX1.WMaxSptPct.mxVal.f` | MX | 60 | 0 |
| `WSd_Mod` | `…/WSdDAGC1.Mod.stVal` | ST | 60 | 1 (cfg) |
| `WSd_WSpt` | `…/WSdDAGC1.WSptPct.mxVal.f` | MX | 60 | 0 |
| `VArSd_Mod` | `…/VArSdDVAR1.Mod.stVal` | ST | 60 | 5 |
| `VArSd_VArTgt` | `…/VArSdDVAR1.VArTgtSptPct.mxVal.f` | MX | 60 | 0 |

**Annex alignment:** POC measurements **4 s** per **T.3.1.3** / **O.8.3** — set **Interval = 4** on TotW/TotVAr/PPV (not 60).

Optional: enable **Report on Change** for control points only.

### 3.1.1 Bulk import (recommended — 8 remaining points)

**File:** [`CCLI_LAB_P5_points_import.csv`](CCLI_LAB_P5_points_import.csv)  
**Scope:** adds **8** points; keeps your existing **`PdC_TotW`** row unchanged.

**LuCI steps:**

1. **Services → IEC 61850 Protocol → Device Configuration**
2. Select device **`CCLI_LAB`**
3. Click **Import Points** (top-right)
4. **File:** choose `lab/evidence/phase5/CCLI_LAB_P5_points_import.csv` from your PC
5. **Conflict mode:** **Append — skip existing (key=Name)**
6. **Parse & Preview** → confirm **8 rows** → **Import**
7. **Save Device**
8. Refresh the Data Points table — expect **9 rows** total

**Pass criteria after import:**

| Name | Expected Realtime |
|------|-------------------|
| `PdC_TotW` | **450.0** (already OK) |
| `PdC_TotVAr` | **45.0** |
| `PdC_PPV_AB` | **20.0** |
| `Wlim_Mod` | **5** |
| `WSd_Mod` | **5** (lab yaml; CID default **1**) |
| `VArSd_Mod` | **5** |
| `Wlim_WMax`, `WSd_WSpt`, `VArSd_VArTgt` | **0.0** @ baseline |

If a POC point shows **0** or **—**, check **ObjectRef** uses leaf paths (`.mag.f`) — not whole DOs.  
If import rejects a row, use **Model Tree** → expand `LD_Plant` → `PdCMMXU1` → tick **TotVAr.mag.f** (etc.) → confirm.

### 3.2 Report format

**Report Format** → keep **Platform Standard Format (REPORT_PROPERTY)** unless your platform requires custom JSON.

Key placeholders: `${device_id}`, `${timestamp}`, `${properties}` with inner `${value}`, `${point_name}`, `${report_key}`, `${point_timestamp}`.

---

## 4. Northbound channel — `channel_1` (from UI)

**Tab:** Northbound Configuration → **Edit** `channel_1`

**Frozen config (2026-10-01 screenshot):** [P5_TESPRO_NORTHBOUND_2026-10-01.md](P5_TESPRO_NORTHBOUND_2026-10-01.md)

### 4.1 Connection + upload (single dialog)

| Field | Lab value | Status 2026-10-01 |
|-------|-----------|-------------------|
| Name | `channel_1` | **PASS** |
| Type | **MQTT** | **PASS** |
| **Enabled** | **ON** | **PASS** |
| Host | **`192.168.10.10`** | **PASS** |
| Port | **`1883`** | **PASS** |
| Publish Topic | `device/${gatewayCode}/${device_id}/property/report` | **PASS** (type explicitly) |
| Subscribe Topic | *(blank)* | **Clear** default `function/invoke` |
| Reply Topic | *(blank)* | **Clear** default `function/reply` |
| Client ID | `tg544-lab-gw` | **PASS** |
| QoS | **1** | **PASS** |
| Username / Password | empty | **PASS** |
| TLS Enabled | **OFF** | **PASS** |
| PassThrough | OFF | **PASS** |
| **Report Interval (s)** | **4** | **PASS** |
| Data Format | **JSON** | **PASS** |

**Resolved publish topic:**

```text
device/tg544-lab-gw/cci016-lab-01/property/report
```

**Save Channel** → Status **Connected** once broker listens on **LAN** (not localhost only).

If **Not Connected**: run §5 Admin install, clear Subscribe/Reply, Save, restart IEC 61850 service if needed.

---

## 5. PC MQTT broker (lab)

PC Eth_A: **`192.168.10.10`**. DUT connects inbound to port **1883**.

**Install Mosquitto (once):**

```powershell
winget install EclipseFoundation.Mosquitto
```

**LAN bind (Administrator — once):** Windows service defaults to **127.0.0.1 only**; TG544 cannot connect until fixed.

```powershell
.\lab\tg544-openwrt\install-mqtt-lab-broker.ps1
```

Uses `lab/mosquitto-lab.conf` (`listener 1883 0.0.0.0`, `allow_anonymous true`) + firewall rule for `192.168.10.0/24`.

**Check / start:**

```powershell
.\lab\tg544-openwrt\start-mqtt-broker.ps1
Get-NetTCPConnection -LocalPort 1883 -State Listen   # expect 0.0.0.0:1883
```

**Verify uploads:**

```powershell
.\lab\tg544-openwrt\verify-tespro-northbound.ps1
.\lab\mqtt-northbound-listen.ps1
```

---

## 6. Verification (Part H — annex cross-check)

| Step | REQ-ID | Procedure | Pass criteria |
|------|--------|-----------|---------------|
| P5-H01 | REQ-NB-001 | LuCI device tab | Green dot; Realtime **PdC_TotW ≈ 450** |
| P5-H02 | REQ-NB-002 | Channel Status | **Connected**; Sent counter increases |
| P5-H03 | REQ-NB-003 | `mosquitto_sub` 60 s | JSON received; `PdC_TotW` ≈ Modbus P |
| P5-H04 | REQ-MET-001 | Compare MQTT `dataTime` / point ts Δt | ~4000 ms between POC uploads (±200 ms lab) |
| P5-H05 | REQ-LN-004 | Same JSON message | TotW + TotVAr + PPV timestamps coherent |
| P5-H06 | — | Service Logs → Southbound | No `IedClientError=5`; connect OK |
| P5-H07 | — | DUT + ccli coexistence | `ccli` :102 + TSP still OK while collector runs |

**Evidence:** screenshot Connected status · `mosquitto_sub` capture · LuCI **View Logs** export →  
`lab/evidence/phase5/P5_TESPRO_NORTHBOUND_<date>.txt`

### Known failure modes

| Symptom | Cause | Fix |
|---------|-------|-----|
| `IedClientError=5` | ccli bound `192.168.10.1` only | `bind_address: 0.0.0.0` in lab yaml |
| TotW = 0 | Wrong ObjectRef (DO not leaf) | Use `.mag.f` leaf paths §3.1 |
| Not Connected | Broker down / wrong Host | §5; Host=`192.168.10.10` |
| **Connected, Sent=0** | Southbound MMS to :102 failed | `restart-tespro-northbound.ps1 -Verify` |
| `northbound_count: 0` | Channel disabled | Enabled=ON; Save |
| 60 s MQTT cadence | Channel interval 60 | Set channel + point interval **4** |

---

## 7. Annex traceability (collector vs DSO)

| Annex clause | DSO path (ccli + TSP) | TesPro northbound |
|--------------|----------------------|-------------------|
| **T.3.1.3** PdC TotW/TotVAr/PPV | TSP Read / URCB | MQTT `${value}` cross-check |
| **T.3.2.1** 4 s Type 3 | URCB IntgPd=4000 | Point interval 4 s; not normative |
| **O.8.3** :00/:04 grid | URCB report `t` | **GAP** both if scheduler open |
| **T.3.3.4.5** UTC ±100 ms | chrony + MMS q | `${point_timestamp}` informational |
| **O.9.2.2** Wlim | TSP Operate | MQTT `Wlim_*` keys after Operate |

Northbound **does not close** REQ-MET-002 or REQ-TIM-002 — it **validates supplier stack** + **end-to-end value chain**.

---

## 8. Coexistence notes

- **Port 102:** single listener — ccli **server**; TesPro **client** (no conflict).  
- **Port 3782:** product TLS — revert after lab ([P5_TSP_CLEARTEXT_README.md](P5_TSP_CLEARTEXT_README.md)).  
- **CPU/RAM:** both `ccli` + `iec61850d` on TG544 — monitor if reports stall.  
- **Do not** enable TesPro as MMS server on Eth_A — DSO path is **`apps/ccli`**.

---

## 9. Checklist (printable)

```text
[x] ccli 0.0.0.0:102 · Modbus P=450 · A1/B1 /dev/ttyS1
[x] Device CCLI_LAB · cci016-lab-01 · 9 points · POC interval 4 s
[x] channel_1 Enabled · Host 192.168.10.10:1883 · Client ID tg544-lab-gw · Report interval 4 s
[x] LuCI Connected · Target 192.168.10.10:1883
[x] Broker 0.0.0.0:1883 · verify-tespro-northbound.ps1 PASS
[x] PdC_TotW=450 in MQTT JSON · ~4 s cadence
[ ] Optional: clear Subscribe/Reply topics (lab read-only)
```
