# Masomeh — EMT432 (Chronos) + TestPro vendor requirements

**Date:** 2026-10-22  
**Role:** PC-B / TSP · LAN1 **192.168.10.10** · logger evidence  
**PC-A (Yahia):** LAN3 plant **192.168.178.10** · EMT432 **.102** · DUT **.178.1** / LAN2 **192.168.1.130**

You do **not** own vendor certificate closure — use this pack to **know scope**, **avoid wrong PASS**, and **capture evidence** when bench runs EMT432 + full HW yaml.

---

## Read order

| # | Document | Purpose |
|---|----------|---------|
| 1 | **[MASOMEH_EMT432_TESPRO_REQUIREMENTS.md](MASOMEH_EMT432_TESPRO_REQUIREMENTS.md)** | This index |
| 2 | **[CHRONOS_EMT432_VENDOR_REQUIREMENTS.md](CHRONOS_EMT432_VENDOR_REQUIREMENTS.md)** | All Chronos cert asks (O.15 **#2**) + Modbus pin |
| 3 | **[TESPRO_VENDOR_REQUIREMENTS_HANDOFF.md](TESPRO_VENDOR_REQUIREMENTS_HANDOFF.md)** | TesPro 9-row reply vs O.15 **#2/#3/#8** |
| 4 | [METER_SPECIMEN_EMT432_V_FULL.md](../../lab/conformance/METER_SPECIMEN_EMT432_V_FULL.md) | Bench wiring · 905 · 80 mV/kA deploy |
| 5 | [61557-12_TESPRO_VENDOR_MAP.md](../../lab/conformance/61557-12_TESPRO_VENDOR_MAP.md) | Full TesPro vs 61557 map |
| 6 | [TSP_TWO_PC_BENCH_RUNBOOK.md](../../lab/conformance/TSP_TWO_PC_BENCH_RUNBOOK.md) | Two-PC roles |

Field pack copies (after `stage-tsp-field-pack.ps1`): **`00q_MASOMEH_EMT432_TESPRO_REQUIREMENTS.md`**, **`00q1_CHRONOS_EMT432_VENDOR_REQUIREMENTS.md`**, **`00s_TESPRO_VENDOR_REQUIREMENTS.md`**, **`00u_METER_SPECIMEN_EMT432.md`**, **`00v_61557-12_TESPRO_VENDOR_MAP.md`**.

---

## EMT432 — what matters for TSP / unified matrix

| Topic | Fact |
|-------|------|
| **Who measures P/Q/V** | **Chronos EMT432.V.FULL** on LAN3 — **not** the TG544 |
| **DUT yaml (full bench)** | `apps/ccli/config/lab_tr400_emt432_lan3_tcp_102.yaml` |
| **Your MMS checks** | **TotW / TotVAr / PPV** on **5-LN** when PC-A deploys EMT432 yaml — compare vs meter display if asked |
| **61557 on meter** | **OPEN** until Chronos sends **61557-12** report — **62053-22 ≠ 61557** |
| **Do not mark** | O.15 **#2 PASS** on datasheet alone |

**PC-A deploy (not PC-B):**

```powershell
. .\lab\tg544-openwrt\lab-env.ps1
powershell -File lab\tg544-openwrt\deploy-emt432-80mv.ps1
python lab\modbus_emt432_tcp_probe.py --host 192.168.178.102
```

---

## TestPro — what matters for TSP

| Row | Ask | HiTEKS minimal status |
|-----|-----|------------------------|
| **1** | FIPS **L3** TPM | **PARTIAL** — L2 policy only; need CMVP L3 or **no-claim** |
| **2** | CE + DoC TG544 | **PARTIAL** — CNAS/A2LA ≠ product DoC |
| **6** | 61557 / 61010 on gateway | **ALIGNED** — TesPro **comms-only**; **61557 N/A** on TG544; **HiTEKS** owns meter chain |
| **3–9** | ISO9001, Secure Boot, tamper, image, 61850, IDevID | See **TESPRO_VENDOR_REQUIREMENTS_HANDOFF.md** |

TesPro pack does **not** replace **Chronos** certs for **#2** metrology.

---

## Evidence you may be asked to capture (EMT432 session)

When unified matrix runs with **EMT432 TCP** yaml:

| U-ID / test | Capture |
|-------------|---------|
| **P5-04** / Compare | MMS **TotW / TotVAr / PPV** vs meter (photo or Modbus probe log from PC-A) |
| **U-T1-05/06** | 4 s reporting — note **PART** if not block-aligned |
| Logger | Four artefacts per [PC1_LOG_CAPTURE_REQUIREMENTS.md](PC1_LOG_CAPTURE_REQUIREMENTS.md) |

---

## Git pull

```powershell
cd C:\Yahia\projects\CCLI   # or your clone
git pull origin main
powershell -File scripts\stage-tsp-field-pack.ps1 -Dest D:\CCLI_TSP_FIELD_PACK
```

Questions → Yahia / project channel.
