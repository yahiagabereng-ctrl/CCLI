# Chronos EMT432 — web UI capabilities (INSTALLER portal)

**Document ID:** CCLI-CHRONOS-EMT432-UI-001  
**Date:** 2026-10-21  
**RAG source_id:** `ccli-chronos-emt432-ui`  
**Parent:** `ccli-chronos-emt432` · `ccli-chronos-rogowski-bench`  
**DUT:** EMT432.V.FULL · SN **261001085352** · HW **04** · FW **008**  
**Evidence:** lab UI screenshots 2026-10-21 (Nicola bench)

**Access:** `https://<meter-ip>/` (plain HTTP often disabled) · user **`admin`** · password **`PW Web`** on nameplate (not in git).

---

## Navigation map (sidebar)

| Menu | Sub-areas (tabs / pages) | CCLI relevance |
|------|---------------------------|----------------|
| **Measurements** | Energy, Status, RMS Values, Peaks, RMS Max/Min/Avg, Max Demand, Avg Power, Inputs | Verify P/Q/V/I vs Modbus **932/940/905** |
| **Communications** | Wi‑Fi, Wi‑Fi STA, Wi‑Fi AP, Modbus RTU/TCP, Security, Ethernet | Plant IP, **502**, unit **1** |
| **Electrical Settings** | Date/time, Inputs, Power, Machine, Output, Power Quality | **1P Rogowski + integrator** (critical) |
| **Settings** | (general device) | Secondary |
| **Logging** | Flash + SD/USB registers 1–20, start/stop | **GAP** — not in ccli today |
| **WaveForms** | Waves, Harmonics, Interharmonics, Phasor, THD | PQ lab / future MMS |
| **MQTT** | Logging registers 1–20, broker, period | **GAP** — optional cloud path |
| **Power Quality** | SAG / SWELL / Interruption events (RAM + Flash) | **GAP** — event export not wired |

Role badge: **INSTALLER** (full config).

---

## Communications (confirmed on bench unit)

| Interface | Active | IP / notes |
|-----------|--------|------------|
| **Ethernet** | Connected, DHCP **off** | **192.168.178.10**, GW **0.0.0.0**, /24 |
| **Wi‑Fi STA** | On, DHCP **on** | Target/join **FRITZ!Box**; assigned **192.168.178.102**, GW **192.168.178.1** (when associated) |
| **Wi‑Fi AP** | On | **192.168.4.1/24**, SSID **EMT432_&lt;serial&gt;** |
| **Modbus RTU** | — | Address **1**, 9600 8N1 |
| **Modbus TCP** | — | Port **502**, write energy/peaks **enabled** |

### Bench IP plan (before DUT deploy — **confirm**)

| Role | Recommended IP | yaml |
|------|----------------|------|
| PC plant NIC | **192.168.178.10** | `lab/set_pc_lan3_plant_178.cmd` |
| Meter Ethernet (factory-style) | **192.168.178.250** | `lab_tr400_phase4_emt432_lan3_tcp.yaml` |
| Meter on LAN3 (STA or static) | **192.168.178.102** | `lab_tr400_emt432_lan3_tcp_102.yaml` |
| DUT LAN3 gateway | **192.168.178.1** | TG-544 UCI |

**Conflict rule:** PC and meter must **not** share the same host address. Current snapshot had meter Ethernet **.10** while PC also used **.10** — resolve before production bench.

---

## Electrical settings — Rogowski 1P + FRC-350 (must confirm)

Snapshot **2026-10-21** (needs change for CCLI plant reads):

| UI field (IT / EN) | Snapshot value | Bench target |
|--------------------|----------------|--------------|
| **Connessione** | Trifase 4 fili, 3 TA (con neutro) | **Monofase** / single phase |
| **Integratore** | Disabilitato | **Abilitato** (Rogowski **V** meter) |
| **Rapporto Trasf. TA** | 1 | FRC-350 via catalog or **40235** ≈ **0.0001 V/A** (100 mV/kA) |
| **Tensione stellata nominale** | 230 V | OK |
| **Data/ora** | Year **2000** | Set NTP or manual (PQ events use timestamps) |

Modbus cross-check (EMT430_MR):

| Register | Meaning | Target |
|----------|---------|--------|
| **40011** | mounted_version | **1** = Rogowski |
| **40232** | Measurement config | Single phase + **bit 13** integrator ON |
| **40235** | Rogowski sensitivity [V/A] | **0.0001** for FRC-350-100mV |

Script: `lab/emt432_rogowski_config.py --read` / `--apply-1p-rogowski --set-sensitivity-mvka 100`.

---

## MQTT logging registers (UI capability — not ccli)

**MQTT Attivo** can publish up to **20** mapped quantities every **N** seconds (snapshot: **20 s**, broker **Chronos**).

Default-style mapping observed:

| Reg | Quantity |
|-----|----------|
| 1–3 | KWh1_Plus … KWh3_Plus |
| 4–6 | KWh1_Neg … KWh3_Neg |
| 7 | Status_1 |
| 8–10 | V_L1_N, V_L2_N, V_L3_N |
| 11–13 | I_L1, I_L2, I_L3 |
| 14–16 | P1, P2, P3 |
| 17–19 | Q1, Q2, Q3 |
| 20 | Frequency |

**CCLI status:** **MISSING** — plant path is **Modbus TCP** only (`chronos_emt432_map.yaml`). MQTT would be a separate adapter (REQ-TBD).

---

## Internal / SD logging (UI capability — not ccli)

- **Logging registers 1–10:** internal flash (period e.g. 5 s).
- **Logging registers 11–20:** mass storage (**SD** or **USB**).
- Start/stop, download flash log by date range.

**CCLI status:** **MISSING** — no register ingest in `apps/ccli` today.

---

## Measurements → Energy

Grid: **Line 1–3 + Sum** for active/reactive/apparent energy (kWh, kVARh, kVAh), positive/negative splits.

For **1P bench**, only **Line 1** columns matter after electrical settings fixed; L2/L3 should idle.

---

## WaveForms & power quality

- **Waves:** ~20 ms windows, V L1-N…L3-N, I L1–L3, I N.
- **PQ events:** SAG, SWELL, INTERRUPTION — RAM (volatile) + Flash (historical); erase per class.

Nominal PQ thresholds (Electrical → Power Quality): Sag **0.9**, Swell **1.2**, Interruption **0.1** (pu), Vnom **230 V**.

---

## CCLI integration matrix

| Capability | Protocol / surface | ccli today | Register / config |
|------------|-------------------|------------|-------------------|
| Active/reactive power | Modbus TCP | **HAVE** | P1 **932**, Q1 **940**, scale 0.001 |
| Line voltage 1P | Modbus TCP | **HAVE** | V_L1_N **905** → PPV (906 was first-adapt off-by-one) |
| Frequency | Modbus TCP | **HAVE** | MR map |
| Rogowski config | Modbus write / UI | **LAB SCRIPT** | 40232, 40235 |
| Energy totals | Modbus / UI | Partial | Totals in MR; yaml uses P/Q instantaneous |
| MQTT cloud | MQTT | **MISSING** | UI registers 1–20 |
| Waveforms / THD | UI / MR? | **MISSING** | PQ research |
| PQ events | UI flash | **MISSING** | — |
| IEC 61850 MMS | ccli server | **HAVE** | Maps from Modbus poll |

---

## Confirmation checklist (before `push-lab-yaml-to-dut`)

1. [ ] Ethernet IP = chosen bench IP (**`.250`** or **`.102`**) — not PC **`.10`**
2. [ ] Gateway = **`.1`** (via DUT) or **0.0.0.0** (direct PC only)
3. [ ] Modbus TCP **502**, unit **1**
4. [ ] Electrical: **monofase**, **integrator ON**, FRC-350 / sensitivity OK
5. [ ] Display or probe: **P1 ~ kW** with heater load; **I1** ~ load current
6. [ ] `python lab/modbus_emt432_tcp_probe.py --host <ip>` → PASS
7. [ ] Deploy matching yaml: **phase4 (.250)** or **lan3_tcp_102 (.102)**

---

## Document history

| Rev | Date | Change |
|-----|------|--------|
| 0.1 | 2026-10-21 | Initial from INSTALLER UI screenshots + Modbus probe |
