# TR400 DIO smoke test — Phase 1 bench

**DUT:** TG544 / Tespro TR500 @ `192.168.1.130`  
**API:** `ubus` object `dido_v2` (GD32 coprocessor — not libgpiod)  
**Status:** **PASS** — 2026-09-15 smoke · **GD32 online** confirmed **2026-09-22** post-reflash (`GD32-DIDO-F v1.0.1`, `set_relay` ch1 OK)  
**Phase 1 runbook:** `lab/PHASE1.md`  
**PF2 limits/actions table:** `lab/PF2_Limits_and_Actions.md`

## Verification record

| Channel | Mode | Bench result |
|---------|------|--------------|
| **DIO-1** | DO | OFF **1.5 V**, ON **0 V** (DIO1↔GND); `set_relay` **channel 1** |
| **DIO-2** | DI | Open → `gd32.read_di` **`di[1]`=0**; key closed / GND bench → **1**; ubus **channel 2** |

## Wiring (Phase 1 production bench)

| Signal | Terminal | Connect to |
|--------|----------|------------|
| Curtailment DO | **DIO-1** / ubus **ch1** | SCH-32F **12 V** coil (−) to DIO1 — see `TR400_HW_IO_CONFIG.md` |
| Permissive DI | **DIO-2** / ubus **ch2** | **+12 V → key (NO) → DIO2**; PSU (−) → **GND** |
| Relay power | — | External **+12 V** + GND (common with TG544) |

Legacy smoke: DIO2 short to **GND** also reads **1** (same yaml polarity).

Leave **DIO-3/4**, **AI/AO** unconnected for this test.

## Software prep (once)

Set DIO-2 to input mode (persists in UCI):

```bash
uci set didoservice_v2.channel_2.mode=di
uci commit didoservice_v2
/etc/init.d/didoservice_v2 restart
```

Verify: `ubus call dido_v2 status` → channel 2 `"mode": "DI"`.

## Run from Windows PC (PuTTY plink)

From repo `lab/` folder (host key from first SSH):

```powershell
& "C:\Program Files\PuTTY\plink.exe" -ssh root@192.168.1.130 -pw YOUR_PASSWORD `
  -hostkey "SHA256:2hiPouwqC1oxP//Q0BpvgIcqD6IG+pqLkzNih1EpNRE" `
  -m tr400-dio-smoke.cmds
```

Or copy `tr400-dio-smoke.sh` to the device and run: `sh tr400-dio-smoke.sh`

## Manual ubus checks

```bash
ubus call dido_v2 set_relay '{"channel":1,"value":1}'   # relay ON
ubus call dido_v2 set_relay '{"channel":1,"value":0}'   # relay OFF (safe)
ubus call dido_v2 read_di '{"channel":2}'               # permissive state
```

## DMM test (no relay module)

### Terminal map (bottom 10-pin block)

| Silkscreen | ubus channel | Lab mode | CCLI role |
|------------|--------------|----------|-----------|
| **DIO1** | 1 | DO | Curtailment |
| **DIO2** | 2 | DI | Permissive |
| **GND** | — | — | DMM black lead |

### DIO-1 — software confirmed 2026-09-15

`ubus call dido_v2 status` toggles channel 1 `"state": 0` ↔ `1`. LuCI **Turn ON/OFF** matches.

**DC V on DIO1↔GND may stay ~0 V in both states** — output is likely **open-drain / sink** (not a push-pull logic pin). A high-impedance DMM alone is not a valid load.

**Better checks for DIO-1:**

| Method | OFF (`value: 0`) | ON (`value: 1`) |
|--------|------------------|-----------------|
| **Continuity** DIO1↔GND | open (no beep) | **beep** (short to GND) |
| **Pull-up test** | 10 kΩ DIO1→+5 V: DMM **~5 V** | DMM **~0 V** |
| **Relay module** | OFF | **click** (intended product wiring) |

**Bench measured (TG544):** OFF **1.5 V**, ON **0 V**; continuity ON **~1 kΩ** (open-drain sink — normal).

### DIO-2 — input — PASS

DMM red on **DIO2**, black on **GND**. Short **DIO2→GND** → LuCI **Input HIGH** / `read_di` state **1**. Open → **LOW** / state **0**. Active-low permissive confirmed.

## Pass criteria

| Test | Expected |
|------|----------|
| `set_relay` ch 1 → 1 | Software `state: 1`; continuity DIO1↔GND or relay click |
| `set_relay` ch 1 → 0 | Software `state: 0`; continuity open |
| DIO-2 open | `read_di` → `"state": 0` / LuCI **LOW** |
| DIO-2 → GND | `read_di` → `"state": 1` / LuCI **HIGH** |

## Next step

Implement `platform_tg500` HAL wrapping these `ubus` calls → deploy `ccli` with `lab_tr400.yaml`.
