# SK0146 Knowledge Leak Matrix

Generated: 2026-08-10T15:37:26.326359+00:00

**Test bench:** `C:\Yahia\projects\Test Bench`  
**Reference product:** SK0146-EVC — ATEX zone level **0** (non-hazardous reference)  
**Excel:** `C:\Yahia\projects\CCLI\Architecture\Knowledge_Matrix.xlsx`

## Executive summary

| Finding | Status |
|---------|--------|
| SK0146 schematic RAG corpus (15 sheets) | INGESTED |
| Test Bench FCT (FIXTURE_TEST_PLAN, pytest) | **HAVE** locally |
| CCLI PF2 mocking bench docs | **INGESTED** (`ccli-validation-strategy`, `ccli-mocking-bench-bom`) |
| Fixture TBD items (TP57, TP4, keypad leads) | **OPEN** — bench hardware leak |
| Lab platform (TG-524 / MT798X) | **FROZEN** — OpenWrt SDK still **MISSING** |
| Field custom carrier / alternate SoM | **DEFERRED** — see `_archive/som-study/` |

## Top actions (P0)

1. **Ingest SK0146 schematics to RAG:** `Telematry System/scripts/ingest-sk0146-schematics.ps1`
2. **Close fixture TBDs** in `Test Bench/devices/FIXTURE_TEST_PLAN.md` (TP57, TP4, KEY1–6)
3. **Do not conflate** SK0146 3.3 V DIN/DO tests with CCI 10–120 V DI / relay DO (see Bench_Crosswalk sheet)

## RAG probe results

```json
{
  "sk0146-schematic-index": {
    "ingested": true,
    "retrieved": 3
  },
  "sk0146-blk_ref_a_rs485": {
    "ingested": true,
    "retrieved": 2
  },
  "sk0146-blk_ref_a_digital_in": {
    "ingested": true,
    "retrieved": 2
  },
  "sk0146-blk_ref_a_digital_out": {
    "ingested": true,
    "retrieved": 2
  },
  "sk0146-blk_ref_b_keyboard": {
    "ingested": true,
    "retrieved": 2
  },
  "sk0146-blk_ref_a_sensors": {
    "ingested": true,
    "retrieved": 2
  },
  "sk0146-blk_ref_a_gsm": {
    "ingested": true,
    "retrieved": 2
  },
  "sk0146-blk_ref_b_ir": {
    "ingested": true,
    "retrieved": 2
  },
  "sk0146-blk_ref_a_power": {
    "ingested": true,
    "retrieved": 2
  },
  "sk0146-blk_ref_a_driver_ev": {
    "ingested": true,
    "retrieved": 2
  },
  "ccli-validation-strategy": {
    "ingested": true,
    "retrieved": 57
  },
  "ccli-prototype-bom": {
    "ingested": true,
    "retrieved": 56
  },
  "ccli-cci-module-regs-class": {
    "ingested": true,
    "retrieved": 100
  },
  "ccli-soc-freeze": {
    "ingested": true,
    "retrieved": 10
  },
  "ccli-tg500-lab-platform": {
    "ingested": true,
    "retrieved": 48
  }
}
```

Full matrix: open **SK0146_Knowledge_Leaks** sheet in `Architecture/Knowledge_Matrix.xlsx`.
