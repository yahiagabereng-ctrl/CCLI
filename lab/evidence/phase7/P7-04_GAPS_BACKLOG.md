# P7-04 — O.14 gap backlog (r28+)

**Date:** 2026-10-09  
**Build:** 0.1.0-r35 (P7-O14-LOGGER)

Tracks Annex O §11 categories not yet fully covered vs what r28 wires into `EventStore`.

---

## Wired in r28 (software)

| O.14 category | Event `type` / `detail` | Notes |
|---------------|-------------------------|-------|
| Power on/off | `system` / `power_on:*`, `power_off:shutdown` | Daemon start/stop |
| Firmware version at boot | `system` / `firmware_boot:*` | From `ccli --version` |
| Plant element comms (Modbus) | `modbus` / `link_up`, `link_down:*`, `link_recovered` | RTU poll success/fail transition |
| Measurement device status | `meter` / `quality_good`, `quality_stale`, `quality_invalid`, `quality_recovered` | `Measurement.quality` transition |
| DSO comms connect/disconnect | `mms` / `client_connect`, `client_disconnect` | libiec61850 connection callback |
| Auth accept/reject | `mms` / `auth_accept_*`, `auth_reject_*`, `rbac_deny_control` | ACSE + RBAC |
| Interface switch (DI) partial | `di` / `permissive_ok`, `permissive_blocked` | Permissive DI only |
| Polygon / electrical params (seed) | `dso` / `polygon_seed smax_kva=* enter_kw=*` | Phase-1 mock at boot |
| Annex M trip relay edge | `annex_m` / `trip_relay_active`, `trip_relay_cleared` | DIO3 monitor |
| Process crash / unclean restart (r32) | `security` / `crash_recovered:*`, `unclean_restart`, `service_monitor:*` | Tombstone + running marker |
| Service hang (r32) | procd `watchdog 90` + ubus ping every 30 s | Restart if main loop stalls |

Also: auto-`mkdir` for `/var/lib/ccli` in `EventStore` + `deploy-ccli-dut.sh`.

---

## Closed in r35 (software)

| Item | Event / API |
|------|-------------|
| Firmware change | `firmware_update:old->new` sidecar |
| GOOSE timeout | `goose link_down:timeout` / `link_up` |
| Syslog RFC 5424 | UDP `event_log.syslog_host:port` |
| User overwrite | `--event-clear` exit 2; 0640 file |
| PF2 persist | `EventStore` not RAM-only ring |
| Eth physical link | `iface lanN:up\|down` |
| Segmentation policy | `net segmentation_deny:EthA->EthB` |
| Priority on comms loss | `priority autonomous_after_eth_a_loss` |

## Still GAP (hardware / product)

| O.14 category | Blocker | Target |
|---------------|---------|--------|
| CCI PSU presence | No PSU monitor driver / ubus | HW + driver |
| Real power-loss cause | PMIC / UPS GPIO | Field wiring |
| Firmware **failed sysupgrade** | opkg/sysupgrade hook | procd postinst |
| Full DG / PI switch map | DI not assigned on TR400 lab | P6-06 IO expansion |
| PG / PI protection trips | Protection relay DI | Field PI |
| Port scan / IDS | Packet IDS | hostapd / suricata |
| Off-hours endpoints | Time policy engine | Annex T |
| Live polygon curve edits | MMS set-point only; no curve object | Annex T data model |

---

## Lab stub (explicit, not field claim)

| Event | Meaning |
|-------|---------|
| `system` / `psu_unmonitored` | Placeholder until PSU HW exists — **not** a PSU OK reading |

---

## Verification

After deploy r28, trigger and dump:

```text
ccli --event-wrap-test
# restart ccli, disconnect Modbus cable → modbus link_down
ccli --config /etc/ccli/lab.yaml --event-dump --count 30
```

Expected new lines: `system power_on`, `modbus link_*`, `meter quality_*`, `mms client_*` (after IEDExplorer connect).
