# P6-04 — Teledistacco predisposition (O.12 lab sketch)

**Status:** **PART** — installation-level drawing; lab maps logical signals.

## Fig. 128-class logical (text)

```text
                    ┌─────────────────┐
   DSO ──LTE/SMS──► │ TG544 LTE modem │
                    │  (EC200A/1NCE)  │
                    └────────┬────────┘
                             │ trip script / modem DO
                             ▼
                    ┌─────────────────┐
                    │  K_TRIP relay   │  DIO3 DO (lab)
                    └────────┬────────┘
                             │ 12 V coil (same as DIO1 pattern)
                             ▼
              PI "Scatto da segnale esterno"  [field — not on lab bench]

   PF2 curtail ──► K1 relay ◄── DIO1 DO
   Permissive  ──► DIO2 DI

   ccli O.11: IF DIO3 active → DIO1 curtail INHIBITED
```

## CCI terminals (lab)

| Signal | Channel | Mode |
|--------|---------|------|
| PF2 curtail | DIO1 | DO |
| Permissive | DIO2 | DI |
| Annex M trip | DIO3 | DO |
| Spare | DIO4 | DO |

**Product drawing:** Promote to `Architecture/` single-line when PI vendor confirmed.
