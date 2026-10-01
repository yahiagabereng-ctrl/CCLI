# Regulation mock UI on TesPro TG544

**URL:** `http://192.168.1.130/ccli/mock_console.html`  
**Deploy:** `lab/tg544-openwrt/deploy-regulation-ui.ps1`

## Features

| Panel | Purpose |
|-------|---------|
| **Mock inputs** | Plant, DSO (Wlim/WSd/w110), PF2 timing — builds `/etc/ccli/lab.yaml` |
| **Computed** | R01, R03, R05, L01/L02 preview (same equations as `core/dso/`) |
| **Live plant** | Polls `ccli --bench-status --json` every 2.5 s — **P**, **PF2**, **DIO1 ON/OFF**, permissive |
| **Standards table** | CEI / TR clause → reference value → **expected CCLI action** (DIO, stale, FC03) |
| **Apply** | POST yaml → TG544 → restart **ccli** procd |
| **Presets** | Figura 2 (~42 kW), Legacy 900 kW, O.11 custom |

## Backend (on device)

| Endpoint | Handler |
|----------|---------|
| `GET ?action=status` | `/var/run/ccli-status.json` (daemon writes every ~5 s) |
| `POST ?action=apply` | `ccli --bench-write-config /etc/ccli/lab.yaml` + restart |
| `GET ?action=regulation_live` | Full `--regulation-check --live --json` (brief stop/start for ttyS2) |

CGI script: `lab/tg544-openwrt/cgi-bin/ccli-bench` → `/www/cgi-bin/ccli-bench`

## Requirements

- **`ccli`** with `--bench-status` / `--bench-write-config` (rebuild `ccli-bin`)
- **`/etc/init.d/ccli`** running (Modbus RTU + DIO)
- **uhttpd** with CGI (`cprefix=/cgi-bin`)
- PC **Modbus slave** on A2/B2 for live **P** (see `MODBUS_LIVE_VIEW.md`)

## Document history

| Rev | Date | Change |
|-----|------|--------|
| 1.0 | 2026-09-22 | Mock console + CGI + bench-status |
