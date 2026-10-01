# Regulation bench interface

**Document ID:** CCLI-LAB-REG-BENCH-001  
**Purpose:** Compare **CEI / TR 57-126 / Phase 1** reference values against **yaml config** and optional **live** Modbus + DIO readings.

**Check matrix (values & bench actions):** [`lab/PHASE1_REGULATION_CHECK_MATRIX.md`](PHASE1_REGULATION_CHECK_MATRIX.md)

---

## CLI (TG544 or native build)

```bash
# Config + equations only (no hardware)
ccli --regulation-check --config /etc/ccli/lab.yaml

# One Modbus poll + permissive DI + PF2 tick
ccli --regulation-check --live --config /etc/ccli/lab.yaml

# JSON for web UI import (Modbus log + finalize summary)
ccli --regulation-check --live --live-watch 8 --json --config /etc/ccli/lab.yaml > /tmp/reg.json
```

**PC + TG544 live bus:** see [`lab/MODBUS_LIVE_VIEW.md`](MODBUS_LIVE_VIEW.md) and `lab/modbus-ccli-live-bench.ps1`.

Figura 2 profile:

```bash
ccli --regulation-check --config /etc/ccli/lab_tr400_dso_tr57126.yaml
```

---

## Web UI (PC)

Open in a browser:

`lab/regulation_bench/index.html`

1. Run CLI with `--json` and save output.  
2. Click **Import JSON** and select the file.  
3. Review **Expected vs Actual** and status (PASS / FAIL / PEND / NOT_IMPL).

---

## Code

| Component | Path |
|-----------|------|
| Report builder | `apps/ccli/core/regulation/regulation_bench.cpp` |
| CLI flags | `apps/ccli/services/ccli_main.cpp` (`--regulation-check`) |
| Master map | `lab/PF2_REGULATION_PHASE1.md` |
| Figura 2 params | `lab/CCI_Figura2_Parameters.md` |

---

## Row IDs

| ID | Meaning |
|----|---------|
| TR-T1, R01, R03, R05 | O.8.2 / O.9.2.3 / O.11 |
| L01–L03, L-deb | Lab PF2 FSM |
| LIVE-* | Present only with `--live` |
| REQ-61850, R06–R09 | Documented gaps (NOT_IMPL) |

---

## Document history

| Rev | Date | Change |
|-----|------|--------|
| 1.0 | 2026-09-22 | Initial regulation-check CLI + web UI |
