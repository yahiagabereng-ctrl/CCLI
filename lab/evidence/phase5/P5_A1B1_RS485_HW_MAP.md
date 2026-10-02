# P5 — RS485 hardware map (A1/B1 bench freeze)

**Document ID:** P5-A1B1-HW-MAP-001  
**Date:** 2026-10-01  
**DUT:** TesPro TG544 / TR500 · TesproOS 2.1.0 r601  
**Status:** **VERIFIED PASS** — Modbus + MMS + TesPro collector  
**Canonical yaml:** `apps/ccli/config/lab_tr400_cleartext_tsp_ttyS1.yaml`  
**DUT live:** `/etc/ccli/lab.yaml` → `modbus.device: /dev/ttyS1`

---

## Requirements

| ID | Requirement |
|----|-------------|
| REQ-LAB-RS485-001 | Modbus RTU POC meter on plant RS485 with stable P/Q read |
| REQ-LAB-RS485-002 | Software `modbus.device` must match **physical screw pair** |
| REQ-LAB-RS485-003 | 9600 8N1 · slave ID **1** · FC03 @ **40001** (float32 BE kW) |

---

## Physical → software map (this unit)

```text
  PC USB-RS485 (COM5)                    TG544 TR500
  ┌─────────────────┐                    ┌──────────────────────────┐
  │  A ─────────────┼────────────────────┤ A1  (RS485-1 screw)     │
  │  B ─────────────┼────────────────────┤ B1                       │
  │  GND ───────────┼── common GND ──────┤ GND                      │
  └─────────────────┘                    │                          │
         │                               │  SoC UART → /dev/ttyS1   │
         │  modbus_rtu_slave.py          │         ↓                │
         │  slave ID 1 · 9600 8N1        │  ccli Modbus master      │
         └───────────────────────────────┤         ↓                │
                                         │  PdCMMXU1 TotW/TotVAr    │
                                         │  MMS :102 · TesPro poll  │
                                         └──────────────────────────┘
```

| Layer | A1/B1 (THIS BENCH) | A2/B2 | LuCI RS485-2 |
|-------|-------------------|-------|--------------|
| **Silkscreen** | **A1 / B1** | A2 / B2 | *(internal FTDI path)* |
| **LuCI Serial Service** | **RS485-1** | TR400 manual: RS485-2 | **RS485-2** |
| **Linux device** | **`/dev/ttyS1`** | `/dev/ttyS2` | `/dev/rs485_2_uart` → `ttyUSB1` |
| **UART driver** | `uart:ST16650V2` **OK** | `uart:unknown` **DEAD** | FTDI USB |
| **`ccli` yaml** | **`/dev/ttyS1`** ✅ | do not use | only if wired to FTDI port |
| **Verified 2026-10-01** | **P=450 Q=45** | `rtu_connect` fail | `read_fail` (no PC frames) |

---

## Wiring (verified)

| TG544 terminal | PC USB-RS485 | Notes |
|----------------|--------------|-------|
| **A1** | **A** (+) | After one A↔B swap — **keep this polarity** |
| **B1** | **B** (−) | |
| **GND** | **GND** | Common reference required |

Do **not** use A2/B2 on this firmware build.

---

## LuCI — Serial Service

**Services → Serial Service**

| Port | Enable | Device path | Baud |
|------|--------|-------------|------|
| RS485-1 | ON | `/dev/ttyS1` | 9600 8N1 |
| RS485-2 | ON | `/dev/rs485_2_uart` | 9600 8N1 *(not used by ccli on this bench)* |

`seriald` runs in **shared** mode — `ccli` opens `/dev/ttyS1` directly (verified coexistence).

---

## Register map (unchanged)

| Modbus | Offset | Type | MMS |
|--------|--------|------|-----|
| **40001** | 0 | float32 BE kW | `PdCMMXU1.TotW.mag.f` |
| **40003** | 2 | float32 BE kvar | `PdCMMXU1.TotVAr.mag.f` |

PC slave: `python -u lab/modbus_rtu_slave.py --port COM5 --power-kw 450 --trace`

---

## Verification

```powershell
.\lab\tg544-openwrt\verify-tespro-points-live.ps1
```

| Pass criteria | Expected |
|---------------|----------|
| PC trace | `SLAVE RX FC03` every ~4 s |
| DUT log | `P=450kW Q=45kvar` |
| MMS read | TotW=450 TotVAr=45 |
| TesPro LuCI | PdC_TotW=450 PdC_TotVAr=45 |

---

## Deploy / restore

```powershell
# Credentials: lab/tg544-openwrt/lab-env.ps1
$Pscp = "C:\Program Files\PuTTY\pscp.exe"
& $Pscp ... apps/ccli/config/lab_tr400_cleartext_tsp_ttyS1.yaml root@192.168.10.1:/etc/ccli/lab.yaml
.\lab\tg544-openwrt\restart-dut-modbus-check.ps1 -UseA1B1
```

---

## Risks

| ID | Risk | Mitigation |
|----|------|------------|
| R-LAB-RS485-01 | Operator wires A2/B2 by silkscreen label | Use **this doc** — A1/B1 only on r601 |
| R-LAB-RS485-02 | yaml/device mismatch after redeploy | Deploy `lab_tr400_cleartext_tsp_ttyS1.yaml` not default cleartext |
| R-LAB-RS485-03 | A/B polarity reversed | Swap once; re-verify FC03 trace |

---

## Traceability

| Doc | Link |
|-----|------|
| Master I/O map | `lab/TR400_HW_IO_CONFIG.md` § Modbus |
| Phase 5 verify | `P5_TESPRO_POINTS_VERIFY_2026-10-01.md` |
| TesPro northbound | `P5_TESPRO_NORTHBOUND.md` |
