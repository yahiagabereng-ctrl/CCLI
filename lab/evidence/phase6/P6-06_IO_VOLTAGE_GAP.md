# P6-06 — REQ-IO-004 voltage class gap

**Status:** **GAP (documented)** — acceptable for Phase 6 lab close per checklist.

## Requirement

Product: **5× DI 10–120 V**, **3× DO 250 Vac / 3 A** (Annex M modem class + competitor baseline).

## TG544 lab reality

| Resource | Class | Count |
|----------|-------|-------|
| GD32 DIO via `dido_v2` | **~12 V lab** (open-drain) | 4 ch used |
| Annex M modem DO (field) | **24–265 V~ / 1 A** (M.3.1) | Not on GPIO |

## Mitigation

| Track | Action |
|-------|--------|
| **BOM** | Omron **G2RL-1A-E-DC12** ×3 (power relay) — see `CCI_BOM_Component_Review.md` |
| **Expansion** | Opto-isolated DI board 10–120 V for DDI feedback |
| **Lab** | DIO1–3 at 12 V proves **logic** only; not M voltage certification |

**Owner:** Hardware architecture — not blocking P6-03 software PASS.
