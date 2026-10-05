# P6-03 — Annex M trip inhibits PF2 curtail (O.11)

**REQ:** O.11 / O.9.3 — CCI must not take conflicting action when defence teletrip is active.

## Implementation

| Layer | Detail |
|-------|--------|
| Config | `io.annex_m_trip_monitor.enabled` + `gpio: 3` in `lab_tr400.yaml` |
| HAL | `cci_gpio_ll_read_relay()` — polls `ubus dido_v2 status` channel state |
| Service | `svc_io_read_annex_m_trip_active()` |
| Main loop | `DIO1 ON ⇔ PF2 commanded ∧ permissive OK ∧ ¬annex_m_trip` |

## Log signatures

```text
svc_io: annex_m_trip_monitor ch3 (O.11 inhibit DIO1) [P6-03]
io: DO curtail_blocked_annex_m
io: curtail_do=OFF permissive=OK annex_m_trip=ACTIVE
annex_m: teledistacco_inhibit
```

## SMS from your mobile (lab)

1. Deploy: `.\lab\tg544-openwrt\deploy-annex-m-sms.ps1`
2. Get modem number: `annex-m-sms-number.sh` on DUT (or `AT+CNUM`)
3. Send **plain SMS** from your phone to that number:

| SMS text | Action |
|----------|--------|
| **`TRIP`** | Telescatto **ON** — DIO3 relay, ccli blocks DIO1 |
| **`CLEAR`** | Telescatto **OFF** — release inhibit |

**Device reply (MO-SMS):** after DO change, DUT sends ack visible in **1NCE Portal → SIM → SMS (MO)**:

| Command | Reply |
|---------|--------|
| `TRIP` | `CCLI OK TRIP DIO3=1` |
| `CLEAR` | `CCLI OK CLEAR DIO3=0` |
| unknown | `CCLI ERR <body> DIO3=<n>` |

| Log / file | Content |
|------------|---------|
| `/tmp/annex-m-sms-last-reply.txt` | **Reliable ack** (always written) |
| `/tmp/annex-m-sms.log` | `sms_ack:` + optional `sms_tx: OK MO-SMS` |
| 1NCE Portal → SIM → SMS (MO) | MO-SMS if modem send succeeds |

Aliases (case-insensitive): `ON`, `SCATTO`, `TELESCATTO`, `CCLI TRIP` → ON; `OFF`, `RESET`, `CCLI CLEAR` → OFF.

Poller: cron every ~30 s → `/usr/sbin/annex-m-sms-poller.sh`  
Log: `/tmp/annex-m-sms.log`

**Note:** This is **cellular SMS**, not IEC 61850 MMS. DSO MMS (`Wlim`, etc.) still uses Ethernet.

## Lab trip command (SSH)

```bash
annex-m-trip.sh on
annex-m-trip.sh off
```

Or direct ubus:

```bash
ubus call dido_v2 set_relay '{"channel":3,"value":1}'
```

## Verification

1. PF2 above threshold + permissive closed → **DIO1 ON** (trip off).
2. `annex-m-trip.sh on` → **DIO1 OFF** even if PF2 still commanded.
3. `annex-m-trip.sh off` → DIO1 may return ON after debounce rules.

**PASS signature (2026-10-02):** ccli **0.1.0-r26** + log line:

```text
io: curtail_do=OFF permissive=OK annex_m_trip=ACTIVE
```

**Deploy:** `deploy-p6-03-config.ps1` + ccli r26 binary (`pscp -scp lab/tg544-openwrt/ccli-bin`)

**Script:** [`lab/tg544-openwrt/run-p6-annex-m-inhibit.ps1`](../../tg544-openwrt/run-p6-annex-m-inhibit.ps1)
