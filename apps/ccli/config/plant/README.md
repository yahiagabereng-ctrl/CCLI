# Plant CSV mapping plane

Single commissioning control plane for field bindings, GOOSE data map, and MMS SCADA points.

**Full guide:** [`READ.md`](READ.md) — column schemas, Berlin matrix rules, addressing, traceability.

## Files

| File | Purpose |
|------|---------|
| `PLANT_MAP_INDEX.csv` | Manifest |
| `GOOSE_DATA_MAP.csv` | Berlin GOOSE matrix (long format) |
| `PLANT_ASSIGNMENT_PLANE.csv` | Modbus/Huawei/MMS assignment + 31 LN refs |
| `MMS_POINT_MAP.csv` | TestSuite Pro / SCADA import (v4: O.11 priority + classes + q/t + RBAC) |
| `generated/` | Auto-generated YAML (do not edit) |

## Workflow

```powershell
# Create or refresh CSV templates
python scripts/plant-map-init.py --force

# Regenerate MMS points from CID (close CSV in Excel first)
python scripts/plant-mms-sync.py

# Validate
python scripts/plant-map-validate.py

# Generate runtime-oriented YAML fragments
python scripts/plant-map-generate.py

# Wide Berlin-style GOOSE view for Excel
python scripts/goose-map-to-wide.py
```

## GOOSE long format

One row per `(dataset member × subscriber IED)`. Empty cells in the Berlin Excel sheet = no row.

Pivot back to wide: `python scripts/goose-map-to-wide.py -o generated/GOOSE_DATA_MAP_wide.csv`

## Addressing rules

- **PdC analyzer (lab slave):** 40001-offset (`40001`, `40003`)
- **Chronos EMT432 (product candidate):** see `config/modbus/chronos_emt432_map.yaml` · corpus `CCI_Chronos_EMT432_Extract.md`
- **Huawei SUN2000:** direct registers (`32080`, `40125`, …)
- **CID gap:** `SGGMMXU3`–`SGGMMXU10` marked `CID_GAP` until CID extended

## Runtime status

CSV files are the **source of truth**. Generated YAML in `generated/` is not loaded by `ccli` yet — wire via future `plant.plane_csv` config key.
