# P7-04 — O.14 mandatory event category coverage matrix

**Gate:** P7-04  
**Date:** 2026-10-02  
**Build:** 0.1.0-r35  
**Source:** [CCI_Annex_O_Extract.md](../../../knowledge-base/08-engineering/CCI_Annex_O_Extract.md) §11 (O.14)  
**Backlog:** [P7-04_GAPS_BACKLOG.md](P7-04_GAPS_BACKLOG.md)

Legend: **HAVE** = logged today · **PART** = partial / lab mock · **GAP** = not implemented · **N/A** = product claim deferred

---

## Matrix

| O.14 category (Annex O) | Event type / detail (ccli r28) | Status | Notes |
|-------------------------|--------------------------------|--------|-------|
| CCI power supply presence | `system` / `psu_unmonitored` | **PART** | Lab stub — no HW monitor |
| Power on/off and cause | `system` / `power_on:*`, `power_off:shutdown` | **PART** | Daemon start/stop; no PMIC loss cause |
| Firmware update (+ version) / failed | `system` / `firmware_boot:*`, `firmware_update:old->new` | **HAVE** (r35) | Sidecar compare; opkg hook still N/A |
| DG / interface switch (DI) status | `di` / `permissive_*`; `do` / `curtail_*` | **PART** | Permissive + curtail DO; no DG/PI map |
| DSO comms presence | `mms` / `comms_loss_fallback`, `client_connect`, `client_disconnect` | **HAVE** | Eth_A MMS |
| External operator comms | `iec104` / `comms_loss_fallback`, `operator_asdu_*` | **HAVE** | Eth_B 104 + O.14 category |
| Plant element comms | `modbus` / `link_up`, `link_down`, `link_recovered` | **HAVE** | Poll transition |
| Plant GOOSE comms | `goose` / `link_up`, `link_down:timeout`, RX meta | **HAVE** (r35) | `goose.timeout_s` (default 10) |
| CCI functionality status | `pf2` / reason strings | **HAVE** (r35) | Persisted to jsonl (was RAM-only) |
| Measurement device status | `meter` / `quality_*` | **HAVE** | Quality transitions |
| Process crash / service restart | `security` / `crash_recovered:*`, `unclean_restart`, `service_monitor:*` | **HAVE** | Tombstone + procd; SIEM = syslog UDP |
| Irregular network connect/disconnect | `iface` / `lanN:up\|down`; `net` / `segmentation_deny:*` | **HAVE** (r35) | operstate watch + NetPolicy; not IDS |
| Abnormal endpoints / off-hours | `net` / `segmentation_deny:EthA->EthB` | **PART** | Policy logged at boot; no time-of-day engine |
| Port scan / malformed / auth errors | `mms` / `auth_reject_*`, `rbac_deny_control` | **PART** | ACSE/RBAC; no packet IDS |
| Authentication attempts | `mms` / `auth_accept_*`, `auth_reject_*` | **HAVE** | + syslog warning PRI |
| Bootloader lockout auth | — | **N/A** | Secure boot path TBD |
| Control function / plant commands | `do`, `pf2`, `mms` (`wlim_*`, `wsd_*`, `varsd_*`) | **HAVE** | PF2 now on disk logger |
| DSO / operator commands + parameters | `mms` / `wlim_*`, `wsd_*`, `varsd_operate…`; `iec104` / `operator_asdu_*` | **HAVE** | |
| PG / PI tripping | — | **GAP** | Field protection inputs |
| Annex M teletripping | `annex_m` / `teledistacco_inhibit`, `trip_relay_*` | **HAVE** | P6-03 + relay edges |
| Over/under-frequency regulation trip | — | **N/A** | PF3 not in scope |
| Control priority changes | `priority` / `autonomous_after_eth_a_loss` | **HAVE** (r35) | P3-05 fallback |
| Polygonal curve / electrical param changes | `dso` / `polygon_seed …` | **PART** | Boot seed; live curve edit GAP |

---

## Storage compliance (P7-01 / P7-02)

| Requirement | Implementation | Status |
|-------------|----------------|--------|
| ≥2048 rolling events | `EventRing::kMaxEvents` + `EventStore` | **HAVE** |
| User cannot overwrite | 0640 file; `--event-clear` rejected; wrap drops oldest only | **HAVE** (software) | FIPS tamper HW is O.15 |
| Timestamp format | `format_o14_timestamp()` UTC in dump | **HAVE** | |
| Remote read (syslog) | RFC 5424 UDP (`event_log.syslog_*`) | **HAVE** (r35) | Point `syslog_host` at SIEM; default 127.0.0.1:514 |

---

## Verdict

| Verdict | Meaning |
|---------|---------|
| **HAVE (software r35)** | Logger implements O.14 store + categories + syslog client. Remaining GAP = PSU/PG-PI HW. |

**Pass criteria for Phase 7 software exit:** wrap test PASS · dump UTC · `--event-clear` denied · syslog line on append. HW categories stay PART/GAP.
