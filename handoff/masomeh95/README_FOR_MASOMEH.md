# CCLI — code handoff for Masomeh (masomeh95)

**Date:** 2026-10-05  
**Repo:** https://github.com/yahiagabereng-ctrl/CCLI  
**Latest commit:** `a2b0886` — GOOSE plant integration, phase 6–7 lab evidence, MMS/ICD mapping

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
| Engineering KB | `knowledge-base/08-engineering/` |

---

## Git identity (first-time setup)

```bash
git config --global user.name "Masomeh"
git config --global user.email "masoumeh.hashemi.sarvestani2022@gmail.com"
```

---

## Questions?

Contact Yahia via the project channel. Push access is on the shared repo once you accept the collaborator invite.
