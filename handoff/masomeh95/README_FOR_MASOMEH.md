# CCLI — code handoff for Masomeh (masomeh95)

**Date:** 2026-10-14  
**Repo:** https://github.com/yahiagabereng-ctrl/CCLI  
**Latest commit:** `85d7946` — cybersecurity lab test matrix (62351 / D7 / TSP Tier 3) + RAG hooks

---

## Option 1 — Clone from GitHub (recommended)

You already have collaborator access. Run:

```bash
git clone https://github.com/yahiagabereng-ctrl/CCLI.git
cd CCLI
git checkout main
git pull origin main
```

Then fetch third-party protocol libraries (required to build):

```bash
cd apps/ccli/third_party
git clone --depth 1 --branch v1.6.0 https://github.com/mz-automation/libiec61850 libiec61850
git clone --depth 1 --branch v2.3.5 https://github.com/mz-automation/lib60870 lib60870
```

See `apps/ccli/third_party/README.md` for versions and license notes.

---

## Option 2 — ZIP in this folder

| File | Contents |
|------|----------|
| `CCLI-main-a2b0886.zip` | Full committed snapshot from GitHub (no `.git` history) |
| `CCLI-full-with-libs.zip` | Same + populated `libiec61850` and `lib60870` (if present) |

---

## Good starting points

| Area | Path |
|------|------|
| Software architecture (your draft) | `Architecture/_extracted_reg_analysis/arch_sw.txt` |
| GOOSE / plant mapping | `apps/ccli/config/plant/` |
| Signal map (ICD) | `apps/ccli/config/icd/signal_map.yaml` |
| GOOSE adapter | `apps/ccli/adapters/iec61850_goose/` |
| Lab evidence P6/P7 | `lab/evidence/phase6/`, `lab/evidence/phase7/` |
| **Cyber test checklist (62351)** | `lab/conformance/LAB_REQUIREMENT_TEST_MATRIX_CYBER.md` |
| Functional TSP ↔ 61850 matrix | `lab/conformance/LAB_REQUIREMENT_TEST_MATRIX.md` |
| Engineering KB | `knowledge-base/08-engineering/` |

**Test Suite Pro on PC1 (your role = PC-B / LAN1):** read **[TSP_PC1_START_MASOMEH.md](TSP_PC1_START_MASOMEH.md)** first.

Yahia may push the field pack to `D:\CCLI_TSP_FIELD_PACK` on PC1. Open **`00_START_PC1_MASOMEH.md`** inside that folder.

Build pack yourself on PC1:

```powershell
powershell -File scripts\stage-tsp-field-pack.ps1 -Dest D:\CCLI_TSP_FIELD_PACK
```

Master checklist: `00_UNIFIED_CERT_TEST_MATRIX.md` + `00b_UNIFIED_CERT_ROADMAP.md` (not legacy-only 01_CHECKLIST).

---

## Git identity (first-time setup)

```bash
git config --global user.name "Masomeh"
git config --global user.email "masoumeh.hashemi.sarvestani2022@gmail.com"
```

---

## Questions?

Contact Yahia via the project channel. Push access is on the shared repo once you accept the collaborator invite.
