# P6-07 — Annex M.3.1 vs TR400 GPIO delta

**Status:** **PART** — do **not** claim lab GPIO = M modem I/O class.

| Parameter | CEI M.3.1 (modem) | TG544 lab (GD32 DIO) | Delta |
|-----------|-------------------|----------------------|-------|
| DI voltage | 24–48 Vdc/Vac; 115–230 Vac variants | ~12 V bench | **Not equivalent** |
| DO contact | Dry **24…265 V~**, **1 A** | Open-drain ~12 V relay driver | **Not equivalent** |
| Signal relay (G6K) | N/A for M trip | Used in early sketches | **Reject** for M trip |
| TVS SMBJ33 | Ethernet/GPIO ESD | Not M modem class | **Do not map** |

## Lab validity

Software path **O.11 inhibit** is voltage-independent (reads ubus relay **state**).

Field product requires **separate modem or interface board** meeting M.3.1, wired per M.5.1.

**Ref:** `knowledge-base/08-engineering/CCI_Annex_OTM_Verification.md`
