# Plant CSV mapping — commissioning guide

**Status:** HAVE (2026-10-04) · schema v1 · CID `lab_tg544_eth_a.cid`  
**Normative context:** CEI 0-16 Annex T (MMS/DSO) · plant bus (Modbus RTU / GOOSE L2)  
**Related:** `signal_map.yaml` (31-LN doc plane) · `lab/GOOSE_ADAPTATION.md`

This folder is the **single CSV control plane** for commissioning: Berlin-style GOOSE data map, field↔MMS assignment (Huawei + PdC), and SCADA/TestSuite Pro MMS points.

---

## Quick start

1. Open CSVs in Excel (UTF-8 BOM — save as CSV UTF-8).
2. Edit rows; keep `map_id` / `plane_id` unique.
3. Validate: `python scripts/plant-map-validate.py`
4. Regenerate YAML: `python scripts/plant-map-generate.py`
5. Wide GOOSE view (Berlin matrix): `python scripts/goose-map-to-wide.py`

Refresh templates from code: `python scripts/plant-map-init.py --force` (overwrites CSVs).

---

## File map

| File | Rows (v1) | Role |
|------|-----------|------|
| `PLANT_MAP_INDEX.csv` | 4 | Manifest — lists all plane files |
| `GOOSE_DATA_MAP.csv` | 89 | Berlin GOOSE matrix (long format) + CCLI publish |
| `PLANT_ASSIGNMENT_PLANE.csv` | 61 | Modbus/Huawei, PdC, DSO, 31 LN refs |
| `MMS_POINT_MAP.csv` | **82** | SCADA / TestSuite Pro — schema **v4** (O.11 priority + classes + q/t + RBAC) |
| `generated/` | — | Auto YAML — **do not edit by hand** |

---

## 1. GOOSE_DATA_MAP.csv

### What it replaces

The Berlin Excel **GOOSE data map**: Multicast Address · Dataset · Description · Sending Variable · subscriber columns with `VB###` slots.

### Long format rule

**One row per non-empty cell** in the Berlin matrix:

```text
(dataset member) × (subscriber IED) → subscriber_local (VBnnn)
```

Empty Excel cells = no CSV row.

### Columns

| Column | Berlin sheet | Example |
|--------|--------------|---------|
| `map_id` | — (unique key) | `Dset_BF.BreakerFailure→1-1TA/B` |
| `multicast_address` | Multicast Address | `01-0C-CD-01-00-14` |
| `dataset_name` | Dataset Name | `Dset_BF` |
| `member_index` | row order in block | `0`=Quality, `1`=Test Mode, `2`=BF/TT |
| `member_desc` | Description | `Breaker Failure` |
| `sending_variable` | Sending Variable | `LT09`, `N/A` |
| `subscriber_ied` | column header | `1-1TA/B`, `CCI016_01` |
| `subscriber_local` | cell value | `VB002` |
| `sending_relay` | Sending Relay | `I-1AMB1`, `CCI016_01` |
| `cb_name` | Message Name | `GooseDset_BF`, `gcb_PdC_Mis4sec` |
| `cb_go_id` | Message ID | `I-1AMB1_Dset_BF`, `PdCMis4` |
| `vlan_dec` / `vlan_hex` | VLAN | `20` / `14` |
| `cos` | CoS | `7` |
| `app_id_hex` | AppID | `0x0002`, `0x1000` |
| `mms_path` | CCLI target | `LD_Plant/PdCMMXU1.TotW` |
| `bind_kind` | Runtime bind | `goose_sub`, `mms_int` |
| `enabled` | Commissioning | `Y` / `N` |
| `runtime_status` | Code status | `IMPL`, `STUB`, `CID_GAP` |
| `notes` | Free text | |

### Dataset blocks in v1

| Block | MAC | Dataset | Publisher | CCLI |
|-------|-----|---------|-----------|------|
| Breaker failure | `01-0C-CD-01-00-14` | `Dset_BF` | `I-1AMB1` | STUB template |
| Transfer trip | `01-0C-CD-01-00-01` | `Dset_TT` | `I-1AMB1` | STUB template |
| PdC 4 s | `01-0C-CD-01-00-01` | `DS_R_PdC_Mis4sec` | `CCI016_01` | **IMPL** |
| Alarms/status | `01-0C-CD-01-00-02` | `DS_R_Stato_Allarmi_Segnali` | `CCI016_01` | **IMPL** |

### Subscriber IEDs (Berlin columns U–AH + CCI)

`1-1TA/B`, `1-IGA/B`, `2-3A1/2B1`, `G-25A LGC`, `2-R10`, `2-1T1`,  
`1-10MA/B`, `1-20MA/B`, `1-30MA/B`, `1-40MA/B`,  
`7-T10A/B`, `7-T20A/B`, `7-T30A/B`, `7-T40A/B`, **`CCI016_01`**

Berlin bay rows: `enabled=N`, `runtime_status=STUB` until SCD bay IEDs exist.

### Wide pivot (Excel view)

```powershell
python scripts/goose-map-to-wide.py -o generated/GOOSE_DATA_MAP_wide.csv
```

---

## 2. PLANT_ASSIGNMENT_PLANE.csv

Field bus (Modbus RTU) ↔ MMS logical nodes.

### Record types

| `record_type` | Meaning |
|---------------|---------|
| `META` | RS485 bus parameters |
| `MEASURE` | Read path (analyzer, inverter P) |
| `STATUS` | Device status word |
| `CONTROL` | DSO write fan-out to inverters |
| `AGGREGATE` | Computed MMS value (e.g. GenPV sum) |
| `LN_REF` | 31-LN traceability (from `signal_map.yaml`) |

### Key rows (v1)

| `plane_id` | Bus | Register | MMS |
|------------|-----|----------|-----|
| `PdC.TotW` | Modbus lab | `40001` float32 | `PdCMMXU1.TotW` |
| `PdC.TotVAr` | Modbus lab | `40003` float32 | `PdCMMXU1.TotVAr` |
| `INV01..10.TotW` | Huawei direct | `32080` I32 ÷1000 | `SGGMMXU1..10.TotW` |
| `INV01..10.Status` | Huawei | `32089` U16 | `SGGMMXU*.Beh` |
| `GenPV.TotW` | computed | sum INV01..10 | `GenPVMMXU1.TotW` |
| `DSO.WSd.PctWrite` | Huawei FC06 | `40125` ×10 | `WSdDAGC1.WSptPct` |
| `DSO.Wlim.PctWrite` | Huawei FC06 | `40125` ×10 | `WlimDWMX1.WMaxSptPct` |
| `DSO.VArSd.QWrite` | Huawei FC06 | `40123` ×1000 | `VArSdDVAR1.VArTgtSptPct` |
| `COMM.42014` | Huawei FC06 | `42014` | remote scheduling enable |
| `COMM.42000` | Huawei FC06 | `42000`=70 | CEI0-16 Italy grid code |

### Addressing rules

- **PdC analyzer:** lab map `40001` / `40003` (not Huawei space)
- **Huawei SUN2000:** direct addresses ≥ `30000` (`32080`, `40125`, …)
- **INV03–10:** `runtime_status=CID_GAP` — MMS LN not in CID yet

`modbus_slave_id=ALL` on DSO rows = fan-out to slaves 1–10 after MMS accept (O.7.3.3 spacing).

---

## 3. MMS_POINT_MAP.csv

TestSuite Pro / operator monitor import (same schema as `lab/evidence/phase5/CCLI_LAB_P5_points_import.csv`).

**Catalog source:** `scripts/plant_mms_catalog.py` parses all four CID report datasets from `lab_tg544_eth_a.cid`:

| Report | Dataset | Points |
|--------|---------|--------|
| `urcb_PdC_Mis4sec` | `DS_R_PdC_Mis4sec` | PdC TotW, TotVAr, PPV, A |
| `urcb_GenAcc_Mis4sec` | `DS_R_GenAcc_Mis4sec` | GenPV, GenTer, GenIdr, St TotW |
| `urcb_SingGen_Mis4sec` | `DS_R_SingGen_Mis4sec` | SGG1/2 TotW, GnGrId |
| `brcb_Stato_Allarmi` | `DS_R_Stato_Allarmi_Segnali` | 19 alarm/status members |

Plus manual rows: nameplate DPCC (Eq 1), DSO Mod/setpoints, LPHD health.

**Description** column uses plain operator text (e.g. `POC — active power export/import (kW)`), defined in `scripts/plant_mms_catalog.py` → `OPERATOR_DESCRIPTIONS`.

Regenerate after CID edit:

```powershell
python scripts/plant-mms-sync.py
python scripts/plant-map-validate.py
```

Lab cleartext TSP: import the same file but set client **Port = 102** and disable TLS (points unchanged).

| Column | Purpose |
|--------|---------|
| `Device` / `DeviceID` | `CCLI_LAB` / `cci016-lab-01` |
| `Host` / `Port` | `192.168.10.1` / `3782` (lab cleartext TSP: set client Port **102**, TLS off) |
| `Name` | Short SCADA tag |
| `ObjectRef` | Full MMS path (`CCI016_01LD_Plant/...`) |
| `FC` | `MX`, `ST`, `CO`, `SP`, `DC`, … |
| `Description` | Plain operator meaning (every row) |
| `Interval` | Poll seconds (4 for PdC, 60 for DSO) |
| `Enabled` | `Y` / `N` |
| `AccessRole` | `ANY` (read) or `DSO_OPERATOR` (control) |
| `AccessRight` | `READ` or `CONTROL` |
| `AnnexScope` | `OBSERVABILITY`, `T98_DSO`, or `T99_AGG` |
| `OperateObjectRef` | Control DO path for Operate tests (CO rows) |
| `AppGroup` | Figura 2 plane **A0–A8** (same as `signal_map.yaml`) |
| `FunctionClass` | Stable function key (e.g. `active_ctrl_wlim`) |
| `FunctionLabel` | Short plain-English function name |
| `LNClass` | IEC 61850 LN class (`MMXU`, `DWMX`, …) |
| `LNRef` | Logical node path (`LD_Plant/WlimDWMX1`) |
| `Clause` | Primary annex clause (`O.8.3`, `O.9.2.2`, …) |
| `PointKind` | `value`, `quality`, `timestamp`, `status`, `operate`, … |
| `O11Priority` | Annex **O.11 Table 1** index (`1`–`7`); blank for observability rows |
| `O11PriorityLabel` | Plain-English O.11 function name (control rows only) |

Schema **v4** adds **O.11 priority** on control-function rows (`active_ctrl_*`, `reactive_ctrl_*`), aligned with `signal_map.yaml` `controls.*.o11_priority` and `CCI_Annex_O_Extract.md` §8.

| O11 index | Function | `FunctionClass` |
|----------:|----------|-----------------|
| 1 | W110 autonomous V limit | *(no MMS LN — yaml/math only)* |
| 2 | DSO power limit | `active_ctrl_wlim` |
| 3 | DSO power modulation | `active_ctrl_wsd` |
| 4 | MSD active-P setpoint | `active_ctrl_wsa` *(CID gap)* |
| 5 | DSO reactive setpoint | `reactive_ctrl_varsd` |
| 6 | PFSP / Q(V) / cosφ(P) | `reactive_ctrl_pfsp`, `reactive_ctrl_varv`, `reactive_ctrl_pfw` |
| 7 | MSD reactive setpoint | `reactive_ctrl_varsa` *(CID gap)* |

Schema **v3** added **function classes** on every row, plus **q/t** companions and **DSO Operate** rows (Annex T Table 98 / 62351-8 RBAC).

---

## Generated outputs

| Output | Source |
|--------|--------|
| `generated/plant_modbus_map.yaml` | `PLANT_ASSIGNMENT_PLANE.csv` |
| `generated/goose_subscribe.yaml` | `GOOSE_DATA_MAP.csv` (CCI subscribe + publish) |
| `generated/mms_point_map.yaml` | `MMS_POINT_MAP.csv` |
| `generated/GOOSE_DATA_MAP_wide.csv` | Wide Berlin pivot |

**Runtime:** `ccli` does not load CSV yet. Generated YAML is ready for a future `plant.plane_csv` config key in lab yaml.

---

## Validation rules

`plant-map-validate.py` checks:

- Unique `map_id` / `plane_id`
- GOOSE MAC `01-0C-CD-01-00-XX`, AppID `0x…`, `VBnnn` format
- Huawei registers ≥ 30000; PdC in `{0, 2, 40001, 40003}`
- 31 `LN_REF` rows present
- MMS required points: `PdC_TotW`, `PdC_TotW_q`, `PdC_TotW_t`, DSO Operate rows, `WSd_WSpt`, `Wlim_WMax`
- RBAC columns present; CONTROL rows require `DSO_OPERATOR`
- O.11 priority on control `FunctionClass` rows only; observability rows blank

---

## Traceability

| Requirement | CSV / artifact |
|-------------|----------------|
| REQ-NET-001 Eth_A MMS | `MMS_POINT_MAP.csv` |
| REQ-PLANT-001 Modbus RTU | `PLANT_ASSIGNMENT_PLANE.csv` META + MEASURE |
| REQ-PLANT-002 GOOSE plant | `GOOSE_DATA_MAP.csv` |
| REQ-DSO-001 setpoints | ASSIGN CONTROL + MMS DSO points |
| 31 LN single plane | ASSIGN `LN_REF` + `signal_map.yaml` |

---

## See also

- [`README.md`](README.md) — short workflow reference
- [`../icd/signal_map.yaml`](../icd/signal_map.yaml) — 31-LN equation/doc plane
- [`../../../lab/GOOSE_ADAPTATION.md`](../../../lab/GOOSE_ADAPTATION.md) — runtime GOOSE behaviour
- [`../../../Flowcharts/ANNEX_EQUATION_UTILIZATION_MATRIX.md`](../../../Flowcharts/ANNEX_EQUATION_UTILIZATION_MATRIX.md) — Eq traceability
