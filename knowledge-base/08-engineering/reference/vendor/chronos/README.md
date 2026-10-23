# Chronos srl — EMT432 reference corpus

**Vendor:** [Chronos srl](https://chronos-tech.it/) · P.IVA 05248100280  
**Product:** EMT432 three-phase network analyzer (Base / Full)  
**CCLI role:** Frozen POC analyzer — **EMT432.V.FULL** + **FRC-350-100mV**. Pin: `lab/conformance/METER_SPECIMEN_EMT432_V_FULL.md`.

## Files

| File | Source |
|------|--------|
| `EMT432_MI-ENG.pdf` | Installation manual EN rev 2 |
| `EMT432_MI-ITA.pdf` | Installation manual IT rev 2 (wiring diagrams) |
| `EMT432_FL-ITA.pdf` | Product flyer IT |
| `EMT430_MR.pdf` | Modbus register map Rev2 (EMT430/432 family) |
| `TA_FL_ITA.pdf` | CT / Rogowski catalog IT (FRC-350, KCT, CP, …) |

## Type-test reports (S.T.A. — 2026)

**Drop folder:** `CHRONOS/CHRONOS/` (intake)  
**Canonical:** `2026-sta-reports/` · extract `CCI_Chronos_STA_Test_Reports_Extract.md`  
**Annex map:** `lab/conformance/CHRONOS_STA_REPORTS_ANNEX_MAP.md` (**ingest PDF + `_extracts` first**, then map)

```powershell
# Manual extract after adding PDFs under 2026-sta-reports/
$dest = "knowledge-base/08-engineering/reference/vendor/chronos/2026-sta-reports"
python -c "from pathlib import Path; import re; from pypdf import PdfReader; d=Path(r'$dest'); o=d/'_extracts'; o.mkdir(exist_ok=True); [ (o/(re.sub(r'[^\\w\\-]+','_',p.stem)[:80]+'.txt')).write_text('\\n'.join('\\n----- PAGE %d -----\\n'%(i+1)+(pg.extract_text() or '') for i,pg in enumerate(PdfReader(str(p)).pages)), encoding='utf-8') for p in sorted(d.glob('*.pdf')) ]"
```

## Ingest (manuals + Modbus)

```powershell
.\scripts\ingest-chronos-emt432.ps1
```

## RAG (all Chronos PDF + MD + chunk check)

Requires Telematry RAG on `http://localhost:8001` (`docker compose up -d rag-python` in Telematry System).

```powershell
powershell -File scripts\ingest-chronos-all-rag.ps1
powershell -File scripts\ingest-chronos-all-rag.ps1 -CheckScoresOnly -SkipSync
```

- **Markdown:** pass = chunks present (no `chunk_quality_score` on structured MD).
- **PDF:** pass = `chunk_quality_score` mean ≥ 0.50; scorecards under `_chunk_score/`.
- Report: `lab/evidence/CHRONOS_RAG_CHUNK_REPORT.json`.

Outputs:

- `knowledge-base/08-engineering/CCI_Chronos_EMT432_Extract.md`
- `knowledge-base/08-engineering/CCI_Chronos_Rogowski_Bench_Extract.md`
- `apps/ccli/config/modbus/chronos_emt432_map.yaml`

## Lab note

Order code **`I`** = TA 5 A inputs. **`V`** = 333 mV / Rogowski + internal integrator (e.g. **FRC-350** from `TA_FL_ITA.pdf`).  
230 V **single-phase** bench: MI-ITA § *Monofase, 2 fili, 1 TA* + Modbus `Measurement_Configuration_Register` single-phase bit.

**Rogowski LAN3 @ .102:** `apps/ccli/config/lab_tr400_emt432_lan3_tcp_102.yaml` · [P5_ROGOWSKI_FRC350_BENCH.md](../../../../lab/evidence/phase5/P5_ROGOWSKI_FRC350_BENCH.md)

**Support (85 vs 100 mV):** coil is **100 mV/kA**, not 85. Flyer **85–265 V** is L-N voltage range. Write **40235 = 0.0001 V/A**. PDFs pulled 2026-10-21 (CDN).
