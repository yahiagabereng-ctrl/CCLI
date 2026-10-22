# EMT432 register report — bench load (2026-10-22)

**Audience:** Alireza (@alirezavzand) · PC-A / logger  
**DUT:** TG544 @ 192.168.1.130 · meter **192.168.178.10:502** unit 1  
**Yaml:** `apps/ccli/config/lab_tr400_emt432_lan3_tcp_102.yaml` · commissioning **rev 5**

---

## Evidence files (git)

| File | Content |
|------|---------|
| `lab/evidence/phase5/EMT432_DUT_FULL_REGISTER_LOG_LATEST.txt` | Full log + live snapshot (reference) |
| `lab/evidence/phase5/EMT432_DUT_FULL_REGISTER_LOG_2026-10-22_load.txt` | With bench load (1st capture) |
| `lab/evidence/phase5/EMT432_DUT_FULL_REGISTER_LOG_2026-10-22_load2.txt` | With bench load (2nd capture) |

Each file: **500 lines DUT `logread modbus:`** (FC3 @40907 qty=36 + hex raw) → **Lua live holding dump** → **post-restart commissioning lines**.

---

## Load test snapshot (load2, live Modbus)

| PLC | Signal | Value |
|-----|--------|-------|
| 40907 | V_L1_N | **~224.7 V** |
| 40923 | I_L1 | **~8.31 A** |
| 40933 | P1 | **~1.87 kW** |
| 40941 | Q1 | **0** |
| 40245 | minimum_current_ripple | **0** (commissioning PASS) |
| 40247 | minimum_power_ripple | **0** (commissioning PASS) |
| 40235 | TA sensitivity | **11764 V/A** (wrong vs bench **0.000085** — not written yet) |

**DUT trace:** `modbus: FC3 … P≈1.87kW` aligns with snapshot P1.

**Energy:** offsets **715/728/764/844** READ_FAIL on ITA Rev0 map (use **40721** for KWh1 if needed).

---

## Regenerate on PC-A

```powershell
. .\lab\tg544-openwrt\lab-env.ps1
powershell -File lab\emt432_dump_registers_one_ssh.ps1 -FullLogs -Open
```

Save a copy for matrix evidence: `modbus_emt432_2026-10-22_load.txt`.

See also: [EMT432_MODBUS_COMMISSIONING_ALIREZA.md](EMT432_MODBUS_COMMISSIONING_ALIREZA.md)
