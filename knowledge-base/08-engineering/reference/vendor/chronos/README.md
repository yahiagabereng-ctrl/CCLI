# Chronos srl — EMT432 reference corpus

**Vendor:** [Chronos srl](https://chronos-tech.it/) · P.IVA 05248100280  
**Product:** EMT432 three-phase network analyzer (Base / Full)  
**CCLI role:** POC energy analyzer candidate — Modbus RTU on TG544 A1/B1 (`/dev/ttyS1`)

## Files

| File | Source |
|------|--------|
| `EMT432_MI-ENG.pdf` | Installation manual EN rev 2 |
| `EMT432_MI-ITA.pdf` | Installation manual IT rev 2 (wiring diagrams) |
| `EMT432_FL-ITA.pdf` | Product flyer IT |
| `EMT430_MR.pdf` | Modbus register map Rev2 (EMT430/432 family) |

## Ingest

```powershell
.\scripts\ingest-chronos-emt432.ps1
```

Outputs:

- `knowledge-base/08-engineering/CCI_Chronos_EMT432_Extract.md`
- `apps/ccli/config/modbus/chronos_emt432_map.yaml`

## Lab note

Order code **`I`** = TA 5 A inputs. **`V`** = 333 mV / Rogowski.  
230 V **single-phase** bench: MI-ITA § *Monofase, 2 fili, 1 TA* + Modbus `Measurement_Configuration_Register` single-phase bit.
