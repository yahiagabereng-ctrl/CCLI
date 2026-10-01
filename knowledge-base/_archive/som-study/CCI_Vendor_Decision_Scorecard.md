# CCI Vendor Decision Scorecard (Phase 1) — post Wave 1 ingest

Website Chapter 07 · `website/src/lib/vendorScorecard.ts` + `validationGate.ts`.

**Rule:** Validate datasheet evidence **before** treating scores as a buy decision.

## Wave 1 ingest (2026-07-25)

| `source_id` | Status |
|-------------|--------|
| `ccli-toradex-verdin-plus-som` | **completed** |
| `ccli-variscite-dart-mx8m-plus` | **completed** |
| `ccli-variscite-dt8m-customboard` | **completed** |
| `ccli-compulab-iot-gate-ref` | **completed** |
| `ccli-vendor-decision-scorecard` | **completed** |

Still **MISSING** for silicon industrial claims: `IMX8MPIEC.pdf` (manual NXP download).

## Evidence gates

| Candidate | Gate | Meaning |
|-----------|------|---------|
| Toradex Verdin 99991105 + **0063** | **CONDITIONAL** | SoM + kit DS ingested — OK to order; PCN still open |
| Variscite DART + DT8MCustomBoard | **PARTIAL** | Dual GbE DS ingested — lock IT PN before buy |
| NXP 8MPLUSLPD4-EVK | **SILICON ONLY** | IEC DS still missing |
| Compulab CL-SOM EVK | **BLOCKED** | No PDFs in KB |
| Compulab IOT-GATE | **STUDY ONLY** | Ref guide ingested — not CEI CCI |

## Ranking (after Wave 1)

| Rank | Candidate | Score | Role |
|------|-----------|------:|------|
| 1 | Toradex Verdin Dev **99991105** + Plus IT **0063** | **84** | Lead |
| 2 | Variscite DART-MX8M-PLUS kit path | **67** | Alternate |
| 3 | NXP 8MPLUSLPD4-EVK | **60** | Silicon / HAB |
| 4 | Compulab CL-SOM EVK | **42** | Blocked |
| 5 | Compulab IOT-GATE-iMX8 | **41** | I/O study only |
