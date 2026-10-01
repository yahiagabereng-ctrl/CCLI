# CCI Vendor Kit Catalog (Prototype BOM)

Source of truth for website Chapter 06 · Prototype BOM (`website/src/lib/bomCatalog.ts`).

**Rule:** Validate official datasheets first, then score kits against C1–C10. Eval kits are never the field CCI.

## Lead pick (Phase 1)

| Item | Full name | PN |
|------|-----------|-----|
| Eval kit | Verdin Development Board with HDMI Adapter | **99991105** |
| SoM | Verdin iMX8M Plus Quad 4GB IT | **0063** |

## Supplier categories

### Toradex (lead)

- Verdin Development Board with HDMI Adapter — **99991105**
- Verdin iMX8M Plus Quad 4GB IT — **0063**
- Verdin iMX8M Plus Quad 4GB Wi-Fi / Bluetooth IT — **0058** (radio usually unwanted)
- Dahlia Carrier Board kit (compact; typically 1× GbE)
- Verdin iMX8M Plus Evaluation Kit 1 (shop bundle — confirm SoM SKU)

### Variscite (alternate — validate before compare)

- VAR-DVK-DT8M-PLUS / VAR-STK-DT8M-PLUS + DART-MX8M-PLUS
- VAR-SOM-MX8M-PLUS Evaluation Kit (Symphony-Board V2)
- SoMs: DART-MX8M-PLUS, VAR-SOM-MX8M-PLUS

### Compulab

- CL-SOM-iMX8 and SBC-iMX8 Evaluation Kit (alternate SoM)
- IOT-GATE-iMX8 Evaluation Kit / IOT-GATE-iMX8 — **I/O study only**, not CEI Eth_A/B CCI

### NXP (silicon / HAB)

- **8MPLUSLPD4-EVK** — silicon reference (historical)
- IMX8MPCEC — consumer datasheet (on disk)
- IMX8MPIEC — industrial datasheet (**MISSING** — required for IT claims)

## Custom carrier (after SoM lock)

Shared lines C1 Ethernet · C2 RS-485 · C4 DI/DO · C5 GNSS · C6 security · C10 power — see `CCI_Prototype_BOM.md`.
