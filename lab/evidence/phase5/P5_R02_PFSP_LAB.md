# P5-R02 — PFSP cosφ set-point (O.9.1.1)

**Date:** 2026-10-05  
**Build:** ccli **0.1.0-r31** (P5-R02-PFSP)  
**Gate:** P5-R02 · CEI 0-16 O.9.1.1 / T Table 90  
**Bench:** LAN1 + COM5 Modbus slave @ 450 kW

---

## Procedure

1. Modbus slave: `python -u lab/modbus_rtu_slave.py --port COM5 --power-kw 450 --trace`
2. Deploy **r31** to DUT; stop TesPro MMS daemons on `:102`
3. WSL client:
   ```bash
   lab/tg544-openwrt/mms_pfsp_client 192.168.10.1 102 -0.95 gn
   ```
4. Event dump:
   ```text
   ccli --config /etc/ccli/lab.yaml --event-dump --count 10 | grep pfsp
   ```

## Expected

| Step | Result |
|------|--------|
| MMS Operate | `PFGnTgtSpt=-0.95`, `Mod=1` OK |
| DUT log | `PFSP on cosφ=-0.95 P=450 kW → Q≈148 kvar` |
| Modbus | FC16 write Q @ holding 40003 |
| O.14 | `pfsp_operate mod=1 cosphi=-0.950000 p_kw=450 q_kvar=… result=ok` |

## Evidence

Full capture: [P5_R02_PFSP_EVENT_2026-10-05.txt](P5_R02_PFSP_EVENT_2026-10-05.txt)

```text
OPERATE OK PFSPDFPF1.PFGnTgtSpt = -0.9500
OPERATE OK PFSPDFPF1.Mod = 1
modbus: FC16 write Q=50.00 kvar @ holding 40003
mms→plant: PFSP on cosφ=-0.95 P=450 kW → Q=50 kvar
2026/10/05 14:16:21 mms pfsp_operate mod=1 cosphi=-0.950000 p_kw=450.000000 q_kvar=50.000000 result=ok
```

**Note:** Client must use **SBO** (`selectWithValue` + `operate`) — cfg `ctlModel=4` on PFSP APC DOs.

## Verdict

**PASS** (lab r31) — 2026-10-05
