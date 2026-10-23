# Full hardware bench — button, meter, RS485, relay

**Document ID:** CCLI-LAB-HW-FULL-2026-10-15  
**DUT:** TG544 @ `192.168.1.130` · **ccli `0.1.0-r55`**  
**Active yaml:** `apps/ccli/config/product_tr400.yaml` → `/etc/ccli/lab.yaml`

## Installed stack

| Function | Physical | Software |
|----------|----------|----------|
| **POC meter** | **LAN3** RJ45 (EMT432) | **Modbus TCP** `192.168.178.250:502` — **ccli** TotW/TotVAr/PPV |
| **Inverter mock (Sun)** | **PC USB RS485** → DUT **A1/B1** | `start-bench-plant.ps1 -SunOnly -Port COM5` — **not** ccli Modbus source |
| Curtail relay | **DIO1** | `io.do_curtail.gpio: 1` |
| Permissive button | **DIO2** | `io.di_permissive.gpio: 2` |
| MMS / 104 cyber | LAN1 / LAN2 | `product_tr400.yaml` |

```text
  PC-A COM5 (USB-RS485) ----A1/B1----> ttyS1   SUN2000 RTU sim (parallel)
  EMT432 RJ45 -------- LAN3 --------> TCP :502  ccli plant read (primary)
```

Wiring: [CURTAIL_BUTTON_AND_RELAY_SETUP.md](../../CURTAIL_BUTTON_AND_RELAY_SETUP.md) · [P5_A1B1_RS485_HW_MAP.md](../phase5/P5_A1B1_RS485_HW_MAP.md) · [TSP_TWO_PC_BENCH_RUNBOOK.md](../../conformance/TSP_TWO_PC_BENCH_RUNBOOK.md)

## Verify (PC)

```powershell
. .\lab\tg544-openwrt\lab-env.ps1
.\lab\tr400-io-verify-remote.ps1 -HostAddr 192.168.1.130
# Modbus + PF2: logread on DUT — expect P>0 kW when meter live, not read_fail
```

## Sign-off

| Item | Pass |
|------|------|
| GD32 online | **YES** (2026-10-16 SSH) |
| DIO1 relay click OFF→ON→OFF | **YES** (ubus set_relay remote verify) |
| DIO2 di[1] 0 open / 1 closed | **wired** — verify manually with key |
| Modbus P read (no ERR=read_fail) | **NO** — DUT on **RTU ttyS1** + read_fail; need **TCP yaml** + meter **or** COM5 Sun |
| permissive_bypass false | **YES** on DUT yaml |
| Wlim operate → DIO1 only with key closed | **pending** TSP U-T1-07 after deploy r55 + product yaml |

**Validation artefact:** [BENCH_HW_VALIDATION_2026-10-16.txt](BENCH_HW_VALIDATION_2026-10-16.txt) (`scripts/validate-bench-hw-configuration.ps1 -RemoteDut -WriteEvidence`).
