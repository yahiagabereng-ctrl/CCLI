# TesPro `dido_v2` ubus API — TG544 bench notes

**DUT:** TG544 / Tespro TR500 · TesproOS 32-2.1.0  
**Status:** Reverse-engineered on device — **not** published on [wiki.tespro.com](https://wiki.tespro.com/) (2026-09)

## Internet / vendor documentation

| Source | Finding |
|--------|---------|
| [wiki.tespro.com](https://wiki.tespro.com/) | Product wiki exists; **no TR500/TR400 `dido_v2` page** |
| [tespro.com TR-400](https://www.tespro.com/tr-400-industrial-vpn-router/) | Router/VPN focus — **no DIO ubus API** |
| TG-324/325 gateway pages | **2×DI + 2×DO** on 8-pin — different product line |
| **Authoritative for TG544** | On-device `ubus -v list dido_v2` + bench smoke |

## Firmware capability (TR400 / TR500 / TG544)

From LuCI `dido-service-v2.lua` on TesproOS (on-device, not on public web):

> GD32 coprocessor models (TR400/TG400/TR500/TG500): **all 4 DIO channels support full DI+DO switching** — not fixed 2×DI + 2×DO.

`didod_v2` log lines decode as:

| Log | Meaning |
|-----|---------|
| `GD32 south: set_mode mask=0x1 mode=0 OK` | Channel **1** → **DO** |
| `GD32 south: set_mode mask=0x2 mode=1 OK` | Channel **2** → **DI** |
| `mode=0` | DO |
| `mode=1` | DI |
| `mask` | Bit **(channel−1)** set |

**Lab default (Phase 1):**

| Channel | Silkscreen | UCI `mode` | CCLI role |
|---------|------------|------------|-----------|
| 1 | DIO1 | **do** | Curtailment output |
| 2 | DIO2 | **di** | Permissive input |
| 3 | DIO3 | do | Spare |
| 4 | DIO4 | do | Spare |

**Failure mode seen 2026-09-18:** both `channel_1` and `channel_2` set to **`di`** in UCI → DIO1 no longer DO; GD32 mask `0x3 mode=1`. Restore with `lab/tr400-dio-factory.cmds`.

`report_key: relay_2` on a DI channel is **cosmetic naming only** — not “stuck in output mode”.

---

## ubus object `dido_v2`

```bash
ubus -v list dido_v2
```

| Method | Args | Role |
|--------|------|------|
| `status` | — | Service + per-channel mode/state |
| `set_relay` | `channel`, `value` | DO drive (DIO1 curtail) |
| `read_di` | `channel` | Northbound DI read (may be stale) |
| `set_mode` | `channel`, `mode` | `"DO"` / `"DI"` — service layer |
| `gd32.read_di` | — | **Raw GD32 DI array** — use for debug |
| `gd32.set_mode` | `channel`, `mode` | **Coprocessor mode** — required with UCI |
| `gd32.set_do` | `channel`, `value` | Low-level DO (diagnostics only) |

## Channel 2 permissive — known quirks

1. **UCI alone is insufficient** for reliable reads:
   ```bash
   uci set didoservice_v2.channel_2.mode=di
   uci commit didoservice_v2
   /etc/init.d/didoservice_v2 restart
   ubus call dido_v2 set_mode '{"channel":2,"mode":"DI"}'
   ubus call dido_v2 gd32.set_mode '{"channel":2,"mode":"DI"}'
   ```

2. **`report_key: "relay_2"`** in `read_di` output is normal even when `"mode": "DI"` in `status`.

3. **`last_poll: 0`** on a DI channel → northbound poll may not have run; prefer **`gd32.read_di`**.

4. **`gd32.read_di` response:**
   ```json
   { "success": true, "di_mask": 0, "di": [0, 0, 0, 0] }
   ```
   - Channel **N** (1-based) → `di[N-1]` or bit **(N-1)** of `di_mask`
   - Bench: open → **0**; DIO2→GND → **1**

5. **ccli HAL** (`platform_tg500/hal_gpio_ll.c`):
   - Calls `set_mode` + `gd32.set_mode` on `config_input`
   - Reads **`gd32.read_di` first**, then `read_di` fallback

## Quick diagnostic (switch open vs closed)

```bash
ubus call dido_v2 gd32.read_di '{}'
ubus call dido_v2 read_di '{"channel":2}'
ubus call dido_v2 status
```

Pass: `di[1]` or `read_di` **state** toggles **0 ↔ 1** when DIO2 is open vs shorted to **GND** on the 10-pin block.

## References

- `lab/TR400_DIO_SMOKE_TEST.md`
- `knowledge-base/08-engineering/CCI_TR500_IO_Peripheral.md`
- TesPro support: [sales@tespro.com](mailto:sales@tespro.com) — request TR500 DIDO V2 SDK / API doc
