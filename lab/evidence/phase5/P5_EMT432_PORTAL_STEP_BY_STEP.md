# EMT432 INSTALLER portal — step-by-step configure (Rogowski bench)

**Document ID:** CCLI-EMT432-PORTAL-SOP-001  
**Date:** 2026-10-21  
**RAG source_id:** `ccli-emt432-portal-step-by-step`  
**Parents:** `ccli-chronos-emt432-ui` · `ccli-chronos-rogowski-bench`  
**Device:** EMT432.V.FULL · SN 261001085352 · FW 008  
**Screenshot pack:** `lab/evidence/phase5/emt432_ui_2026-10-21/` (10 PNG, synced for evidence)

**Portal URL:** `https://<meter-ip>/` · user **`admin`** · password **`PW Web`** on nameplate (not stored in git).

---

## Figure index (UI evidence for RAG)

| Fig | File | Menu path | What it shows |
|-----|------|-----------|---------------|
| 01 | `01_measurements_energy.png` | Measurements → Energy | kWh/kVARh/kVAh L1–L3 + Sum (zeros before load) |
| 02 | `02_measurements_energy_apparent.png` | Measurements → Energy (scroll) | Reactive capacitive + apparent energy rows |
| 03 | `03_sidebar_navigation.png` | Sidebar | Measurements, Communications, Electrical Settings, Settings, Logging, WaveForms, MQTT, Power Quality |
| 04 | `04_power_quality_sag_events.png` | Power Quality | SAG RAM + Flash event tables |
| 05 | `05_power_quality_interruption_events.png` | Power Quality | Interruption RAM + Flash |
| 06 | `06_mqtt_logging_registers.png` | MQTT | Registers 1–20 (P/Q/V/I/KWh), period 20 s, broker Chronos |
| 07 | `07_waveforms_waves.png` | WaveForms → Waves | ~20 ms V/I waveforms L1–L3 |
| 08 | `08_logging_flash_sd.png` | Logging | Flash regs 1–10, SD/USB 11–20, start/stop |
| 09 | `09_electrical_settings_inputs.png` | Electrical Settings | **Connessione** 3P, **Integratore** OFF, TA/TV ratio, PQ thresholds |
| 10 | `10_communications_ethernet_wifi_modbus.png` | Communications | Ethernet .10, STA .102, AP 4.1, Modbus 502 unit 1 |

Deep capability matrix: `knowledge-base/08-engineering/CCI_Chronos_EMT432_Web_UI_Capabilities.md`.

---

## Prerequisites

1. PC plant NIC: **192.168.178.10/24**, gateway **empty** (`lab/set_pc_lan3_plant_178.cmd`).
2. Reach portal: **AP** `https://192.168.4.1` or **LAN** `https://<meter-ip>`.
3. FRC-350 on **I1+ / I1−**; **L1+N** 1P; aux **Pow+/Pow−** only.

---

## Step 1 — Communications (Fig 10)

**Path:** Sidebar → **Communications** → review all panels → **Submit**.

| Panel | Action | Bench value |
|-------|--------|-------------|
| **Ethernet** | Static, DHCP **off** | IP **192.168.178.250** (factory yaml) **or** **192.168.178.102** (Rogowski yaml). **Not** same as PC **.10**. GW **0.0.0.0** (direct PC) or **192.168.178.1** (DUT LAN3). **Never** GW = meter IP. |
| **Modbus TCP** | Port | **502** |
| **Modbus RTU** | Address | **1** (same unit ID for TCP) |
| **Wi‑Fi STA** | Optional | Use when meter on router; target **192.168.178.102**, GW **.1** |
| **Wi‑Fi AP** | Leave on for setup | **192.168.4.1** — can disable later on plant |

**Verify:** `python lab/modbus_emt432_tcp_probe.py --host <meter-ip>` → connect OK.

---

## Step 2 — Date and time (Fig 09, top-left panel)

**Path:** **Electrical Settings** → **Data e ora**.

| Field | Action |
|-------|--------|
| Year/month/day/time | Set **real** clock (snapshot had year **2000** — breaks PQ event timestamps) |

**Submit** (or Submit once after Step 3).

---

## Step 3 — Electrical inputs / Rogowski (Fig 09) — **required for CCLI**

**Path:** **Electrical Settings** → **Impostazioni Ingressi**.

| Field (IT) | Was (screenshot) | Set to |
|------------|------------------|--------|
| **Connessione** | Trifase 4 fili, 3 TA (con neutro) | **Monofase** / single phase |
| **Integratore** | Integratore Disabilitato | **Integratore Abilitato** (EMT432.**V** Rogowski) |
| **Misura Filtrata** | Filtro Disabilitato | OK or per manual |
| **Rapporto Trasf. TA** | 1 | Keep **1** if FRC-350 profile applied elsewhere; else set via Modbus **40235** |

**Alternative / cross-check (Modbus):**

```powershell
python lab/emt432_rogowski_config.py --host <meter-ip> --apply-1p-rogowski --set-sensitivity-mvka 100
python lab/emt432_rogowski_config.py --host <meter-ip> --read
```

Pass: **40232** single phase + integrator ON; **40235** ≈ **0.0001** V/A (100 mV/kA).

**Submit.**

---

## Step 4 — Power settings (Fig 09)

**Path:** **Impostazioni Potenze** / **Power Quality** on same page.

| Field | Bench |
|-------|--------|
| **Tensione stellata nominale** | **230** V |
| TA/TV ratio | **1** / **1** unless using external VT |
| Sag/Swell/Interruption | Defaults OK for lab |

**Submit** if changed.

---

## Step 5 — Measurements sanity (Fig 01–02)

**Path:** **Measurements** → **Energy** / **RMS Values** / **Inputs**.

| Check | Pass |
|-------|------|
| L1-N voltage | ~230 V scale |
| L1 current | **> 1 A** with heater load (not ~0.001 A) |
| P1 | ~**kW** with load (e.g. ~1.6 kW for 1600 W) |
| L2/L3 | Idle on **1P** bench |

---

## Step 6 — MQTT / Logging (Fig 06, 08) — **skip for CCLI v1**

**Path:** **MQTT**, **Logging**.

| Item | Action |
|------|--------|
| MQTT logging registers 1–20 | **No change** required for Modbus plant path |
| SD/Flash logging | Optional; not ingested by ccli today |

Document only — see UI capabilities doc **GAP** matrix.

---

## Step 7 — WaveForms / Power Quality (Fig 04–05, 07)

**Path:** **WaveForms**, **Power Quality**.

Use for wiring/noise debug. **Not** required before yaml deploy.

---

## Step 8 — Confirm Modbus + deploy CCLI yaml

```powershell
python lab/modbus_emt432_tcp_probe.py --host <meter-ip>
python lab/emt432_rogowski_config.py --host <meter-ip> --read
```

| Meter IP | Deploy yaml |
|----------|-------------|
| **192.168.178.250** | `apps/ccli/config/lab_tr400_phase4_emt432_lan3_tcp.yaml` |
| **192.168.178.102** | `apps/ccli/config/lab_tr400_emt432_lan3_tcp_102.yaml` |

```powershell
powershell -File lab/tg544-openwrt/push-lab-yaml-to-dut.ps1 -LabYaml apps/ccli/config/<chosen>.yaml
```

---

## Step 9 — Bench script (optional)

```powershell
powershell -File lab/validate-bench-hw-configuration.ps1 `
  -MeterHost <meter-ip> `
  -ConfigYaml apps/ccli/config/lab_tr400_emt432_lan3_tcp_102.yaml `
  -ProbeMeter
```

---

## Document history

| Rev | Date | Change |
|-----|------|--------|
| 0.1 | 2026-10-21 | Initial SOP + screenshot pack under `emt432_ui_2026-10-21/` |
