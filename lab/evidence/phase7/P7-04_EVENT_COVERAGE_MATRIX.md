# P7-04 — O.14 mandatory event category coverage matrix

**Gate:** P7-04  
**Date:** 2026-10-02  
**Build:** 0.1.0-r28  
**Source:** [CCI_Annex_O_Extract.md](../../../knowledge-base/08-engineering/CCI_Annex_O_Extract.md) §11 (O.14)  
**Backlog:** [P7-04_GAPS_BACKLOG.md](P7-04_GAPS_BACKLOG.md)

Legend: **HAVE** = logged today · **PART** = partial / lab mock · **GAP** = not implemented · **N/A** = product claim deferred

---

## Matrix

| O.14 category (Annex O) | Event type / detail (ccli r28) | Status | Notes |
|-------------------------|--------------------------------|--------|-------|
| CCI power supply presence | `system` / `psu_unmonitored` | **PART** | Lab stub — no HW monitor |
| Power on/off and cause | `system` / `power_on:*`, `power_off:shutdown` | **PART** | No PMIC loss cause yet |
| Firmware update (+ version) / failed | `system` / `firmware_boot:*` | **PART** | Boot version only; opkg/sysupgrade GAP |
| DG / interface switch (DI) status | `di` / `permissive_*`; `do` / `curtail_*` | **PART** | Permissive + curtail DO; no DG/PI map |
| DSO comms presence | `mms` / `comms_loss_fallback`, `client_connect`, `client_disconnect` | **HAVE** | Eth_A MMS |
| External operator comms | `iec104` / `comms_loss_fallback`, `operator_asdu_*` | **PART** | Eth_B 104 |
| Plant element comms | `modbus` / `link_up`, `link_down`, `link_recovered` | **HAVE** | Poll transition |
| Plant GOOSE comms | `goose` / `appId=… stNum=…` | **PART** | RX when `goose.enabled`; timeout GAP |
| CCI functionality status | `pf2` / reason strings | **PART** | On state change only (r28) |
| Measurement device status | `meter` / `quality_*` | **HAVE** | Quality transitions |
| Irregular network connect/disconnect | — | **GAP** | netlink audit |
| Abnormal endpoints / off-hours | — | **GAP** | net_policy not wired |
| Port scan / malformed / auth errors | `mms` / `auth_reject_*`, `rbac_deny_control` | **PART** | ACSE/RBAC; no IDS |
| Authentication attempts | `mms` / `auth_accept_*`, `auth_reject_*` | **PART** | TLS layer partial |
| Bootloader lockout auth | — | **N/A** | Secure boot path TBD |
| Control function / plant commands | `do`, `pf2`, `mms` (`wlim_*`, `wsd_*`, `varsd_*`) | **PART** | Wlim/WSd/VArSd + curtail |
| DSO / operator commands + parameters | `mms` / `wlim_on`, `wlim_off`, `wsd_update`, `varsd_update` | **HAVE** | |
| PG / PI tripping | — | **GAP** | Field protection inputs |
| Annex M teletripping | `annex_m` / `teledistacco_inhibit`, `trip_relay_*` | **HAVE** | P6-03 + relay edges |
| Over/under-frequency regulation trip | — | **N/A** | PF3 not in scope |
| Control priority changes | — | **GAP** | |
| Polygonal curve / electrical param changes | `dso` / `polygon_seed …` | **PART** | Boot seed; live curve edit GAP |

---

## Storage compliance (P7-01 / P7-02)

| Requirement | Implementation | Status |
|-------------|----------------|--------|
| ≥2048 rolling events | `EventRing::kMaxEvents` + `EventStore` | **HAVE** |
| User cannot overwrite | root-owned append-only file; no erase API | **PART** | Tamper-resistant HW (O.15) separate |
| Timestamp format | `format_o14_timestamp()` UTC in dump | **HAVE** | Local TZ policy TBD for field |
| Remote read (syslog) | — | **GAP** | P7-03 |

---

## Verdict

| Verdict | Meaning |
|---------|---------|
| **PART (lab)** | r28 closes major **software** gaps; HW/syslog/cert items tracked in backlog |

**Pass criteria for Phase 7 exit:** matrix **published** + top gaps tracked — not 100% HAVE.
