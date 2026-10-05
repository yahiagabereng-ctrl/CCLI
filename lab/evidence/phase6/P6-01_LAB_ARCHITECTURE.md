# P6-01 — Annex M lab architecture (M.1)

**Status:** **PART (lab)** — mock path validated; field DSO modem→PI not on bench.

## Normative (M.1)

Plants ≥ 100 kW: DSO sends telescatto via **GSM/LTE receiver** → **PI** external trip → inhibits generation.

## Lab mock (TG544)

| Block | Implementation |
|-------|----------------|
| DSO command | **1NCE Portal** MT-SMS `TRIP` / `CLEAR` (not consumer P2P SMS) |
| LTE receiver | Quectel **EC200A** on `usb0`, SIM **1NCE** ICCID `89882280666900497532` |
| Trip DO | **DIO3** (ubus ch3) via `annex-m-trip.sh` |
| PI equivalent | *Not wired* — GD32 DO state simulates telescatto relay |
| CCI inhibit | **ccli r26** reads ch3 → blocks **DIO1** PF2 curtail (**O.11**) |

## Field product (target)

```text
DSO LTE modem (M.3 voltage class DO) → PI "Scatto da segnale esterno"
DDI feedback → modem DI (optional) → CCI monitor
```

## Gap / waiver

| Gap | Mitigation |
|-----|------------|
| No DSO-provided modem/SIM | Lab uses 1NCE dev SIM |
| No PI on bench | DIO3 relay mock + software monitor |
| DSO connection agreement | **OPEN** — letter or M.1 footnote waiver at connect |

**Evidence:** LTE READY (`10.43.46.209`), SMS receive log `/tmp/annex-m-sms.log`.
