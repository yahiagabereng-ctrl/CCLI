# Rogowski FRC-350 + EMT430/432 LAN3 TCP bench

**Date:** 2026-10-21  
**Meter IP:** `192.168.178.102` · GW `192.168.178.1` (DUT LAN3)  
**Corpus:** `ccli-chronos-rogowski-bench` → [CCI_Chronos_Rogowski_Bench_Extract.md](../../../knowledge-base/08-engineering/CCI_Chronos_Rogowski_Bench_Extract.md)  
**Freeze:** [METER_SPECIMEN_EMT432_V_FULL.md](../../conformance/METER_SPECIMEN_EMT432_V_FULL.md) — **100 mV/kA**, not 85 mV.

---

## Deploy yaml

| File | DUT path |
|------|----------|
| `apps/ccli/config/lab_tr400_emt432_lan3_tcp_102.yaml` | `/etc/ccli/lab.yaml` |

```powershell
powershell -File lab/tg544-openwrt/push-lab-yaml-to-dut.ps1 `
  -LabYaml apps/ccli/config/lab_tr400_emt432_lan3_tcp_102.yaml
```

---

## Physical

1. Meter **V** variant (Rogowski / 333 mV integrator inside meter).
2. **FRC-350** on L1 conductor → **I1+ / I1−** (= manual **I1 S1 / S2**): **blue** and **black** (swap if **P** sign wrong).
3. **L1/N** voltage · **24 V** on Pow+/Pow− · **L2/L3** open for 1P.
4. RJ45 to LAN3 switch with DUT + PC-A.

**Local validation (2026-10-21):** [P5_ROGOWSKI_FRC350_VALIDATE_2026-10-21.md](P5_ROGOWSKI_FRC350_VALIDATE_2026-10-21.md) — display **PASS** for 1P + I1 Rogowski; Modbus/DUT **OPEN**.

---

## PC direct Ethernet + portal / registers

See [EMT432_DIRECT_ETHERNET_SETUP.md](../../EMT432_DIRECT_ETHERNET_SETUP.md) — NIC **178.10**, web **PW Web**, Modbus **`emt432_rogowski_config.py`**.

## PC-A checks (before TSP)

```powershell
lab\set_pc_lan3_plant_178.cmd
python lab\modbus_emt432_tcp_probe.py --host 192.168.178.102
python lab\emt432_rogowski_config.py --host 192.168.178.102 --read
. .\lab\tg544-openwrt\lab-env.ps1
powershell -File scripts\validate-bench-hw-configuration.ps1 `
  -MeterHost 192.168.178.102 `
  -ConfigYaml lab_tr400_emt432_lan3_tcp_102.yaml `
  -RemoteDut -ProbeMeter -WriteEvidence
```

Probe must show `mounted_version` → **333mV/Rog** and plausible **P1** under load.

---

## Two-PC roles

Unchanged: [TSP_TWO_PC_BENCH_RUNBOOK.md](../../conformance/TSP_TWO_PC_BENCH_RUNBOOK.md) — PC-B LAN1 TSP, PC-A LAN2/LAN3/COM5.
