# P5 — TesPro MQTT northbound finalize (2026-10-01)

**Document ID:** P5-TESPRO-NB-FINAL-001  
**Date:** 2026-10-01  
**Status:** **PASS** (runtime verified 14:28 UTC+2)  
**Manual:** `IEC61850-User-Manual.docx` §3 · [CCI_TesPro_61850_Manual_Extract.md](../../../knowledge-base/08-engineering/CCI_TesPro_61850_Manual_Extract.md)  
**Setup guide:** [P5_TESPRO_NORTHBOUND.md](P5_TESPRO_NORTHBOUND.md)

---

## 1. LuCI northbound — PASS

| Field | Value | Status |
|-------|-------|--------|
| Channel | `channel_1` MQTT | **PASS** |
| Target | `192.168.10.10:1883` | **PASS** |
| Enabled | ON | **PASS** |
| Connection | **Connected** (green) | **PASS** |
| Publish Topic | `device/${gatewayCode}/${device_id}/property/report` | **PASS** |
| Client ID | `tg544-lab-gw` | **PASS** |
| Report Interval | 4 s · JSON · QoS 1 · TLS off | **PASS** |

**Resolved topic:**

```text
device/tg544-lab-gw/cci016-lab-01/property/report
```

---

## 2. Runtime verification — PASS

**Evidence:** [P5_TESPRO_NORTHBOUND_2026-10-01_142810.txt](P5_TESPRO_NORTHBOUND_2026-10-01_142810.txt)

```powershell
.\lab\tg544-openwrt\verify-tespro-northbound.ps1
# exit 0 — REPORT_PROPERTY JSON · PdC_TotW=450 · 3 messages / 12 s (~4 s cadence)
```

**Sample payload (truncated):**

```json
{
  "messageType": "REPORT_PROPERTY",
  "gatewayCode": "tg544-lab-gw",
  "deviceCode": "cci016-lab-01",
  "properties": {
    "PdC_TotW": 450.0000,
    "PdC_TotVAr": 45.0000,
    "PdC_PPV_AB": 20.0000,
    "Wlim_Mod": 5,
    "WSd_Mod": 1,
    "VArSd_Mod": 5
  }
}
```

| Step | REQ-ID | Result |
|------|--------|--------|
| P5-H03 | REQ-NB-002 | LuCI **Connected** |
| P5-H04 | REQ-NB-003 | MQTT JSON **PASS** |
| P5-H05 | REQ-MET-001 | Δt **4000 ms** (7706→7710→7714) |
| P5-H06 | REQ-LN-004 | All 9 keys in one message **PASS** |

---

## 3. PC broker

```powershell
.\lab\tg544-openwrt\install-mqtt-lab-broker.ps1   # Admin, once — 0.0.0.0:1883
.\lab\tg544-openwrt\start-mqtt-broker.ps1         # daily check
```

Config: `lab/mosquitto-lab.conf`

---

## 4. Known issue — Connected but Sent = 0

**Symptom:** LuCI northbound **Connected**, **Sent** stays 0, no MQTT on PC.

**Cause:** TesPro southbound MMS to `192.168.10.1:102` failed (`IedClientError=5` / `connect -3`) — stale `:102` sessions or duplicate `ccli` processes.

**Fix:**

```powershell
.\lab\tg544-openwrt\restart-tespro-northbound.ps1 -Verify
```

Then refresh LuCI — **Sent** should increase every ~4 s.

---

## 5. End-to-end chain (closed)

```text
Modbus COM5 → ccli (:102 MMS server) → iec61850d (MMS client)
                                              ↓
                                    MQTT → PC 192.168.10.10:1883
                                    topic device/tg544-lab-gw/cci016-lab-01/property/report
```

**Gate:** P5-H03–H06 **PASS** · parallel to TSP Annex T path (not normative for CEI sign-off).
