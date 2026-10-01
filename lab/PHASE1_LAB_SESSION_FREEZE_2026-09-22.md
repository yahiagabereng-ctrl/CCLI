# Phase 1 lab session freeze — 2026-09-22 (end of day)

**Document ID:** CCLI-LAB-FREEZE-20260922  
**DUT:** TG544 @ `192.168.1.130`  
**Resume:** start with § **Tomorrow** below.

---

## Frozen configuration

| Item | Value |
|------|--------|
| CCLI config on DUT | `/etc/ccli/lab.yaml` |
| Repo canonical yaml | `apps/ccli/config/lab_tr400_phase1_regulation.yaml` |
| **Modbus device** | **`/dev/rs485_2_uart`** (LuCI **RS485-2** → `ttyUSB1`) — **not** `/dev/ttyS2` (A2/B2 UART unbound on FW) |
| Modbus | 9600 8N1 · slave **1** · FC03 @ **40001** · `poll_ms: 4000` |
| PF2 (DSO mock) | enter **42 kW** · release **37 kW** · debounce **30 s** · stale **10 s** |
| I/O | DIO1 = DO ch1 curtail · DIO2 = DI ch2 permissive · `permissive_bypass: false` |
| Mock UI | `http://192.168.1.130/ccli/mock_console.html` · CGI `/cgi-bin/ccli-bench` |
| PC slave | `COM5` USB-RS485 · `python -u lab/modbus_rtu_slave.py --port COM5 --trace` |

---

## Verified today (PASS)

- Static regulation check: **19 PASS / 7 N/A / 0 FAIL** (API `action=regulation`).
- Modbus live read after `rs485_2_uart` fix: **P=40–45 kW**, `quality=good`, FC03 trace on PC.
- CCLI service **running**; `--bench-status --json` / status CGI OK.
- GD32 / DIO mapping documented; **ubus** relay smoke path known.

---

## Not completed (resume tomorrow)

| Task | Last known state |
|------|------------------|
| **DIO scenario walk** (A–D in `lab/REGULATION_MOCK_DIO_MAP.md`) | Modbus OK at **40 kW**; **`permissive_ok: false`** (DIO2 key **open**, `di[1]=0`) — **curtail ON test not signed off** |
| Live regulation finalize | `"live": { "valid": true }` with slave + permissive closed + debounce |
| `lab_tr400.yaml` | Still references `/dev/ttyS2` for A2/B2 — align with phase1 yaml when editing |
| Unit test | `dso_phase1_test` negative wspt case (pre-existing fail) |
| Security | Rotate TG544 root password if exposed in chat; use `$env:CCLI_TG544_PW` only locally |

---

## End-of-day snapshot (2026-09-22 ~18:09)

```json
"p_kw": 40, "quality": "good", "pf2_enter_kw": 42,
"permissive_ok": false, "do_curtail_on": false
```

Stop PC slave when leaving bench (Ctrl+C) so COM5 is free tomorrow.

---

## Tomorrow — cold start (≈10 min)

1. **TG544:** `serialservice` + `ccli` restart; confirm `grep device /etc/ccli/lab.yaml` → `rs485_2_uart`.
2. **PC:** one slave: `python -u lab/modbus_rtu_slave.py --port COM5 --power-kw 40 --trace`.
3. **Sanity:** `curl http://192.168.1.130/cgi-bin/ccli-bench?action=status` → `quality: good`.
4. **DIO:** close DIO2 key → `permissive_ok: true` → `--power-kw 45` → wait **30 s** → `do_curtail_on: true`.
5. Run blocked / release / stale scenarios per `lab/REGULATION_MOCK_DIO_MAP.md` §4.

**Docs:** `lab/TR400_HW_IO_CONFIG.md` (RS485-2 vs A2/B2) · `lab/MODBUS_LIVE_VIEW.md` · `lab/PHASE1_REGULATION_CHECK_MATRIX.md`

---

## Document history

| Rev | Date | Change |
|-----|------|--------|
| 1.0 | 2026-09-22 | EOD freeze after Modbus path fix + regulation static PASS |
