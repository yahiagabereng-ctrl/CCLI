# Annex O + T → software (5-LN DUT gap matrix)

**Document ID:** CCLI-LAB-ANNEX-OT-SW-001  
**Revision:** 1.0  
**Date:** 2026-10-22  
**RAG source_id:** `ccli-annex-ot-sw-gap-matrix`  
**Normative:** CEI 0-16 Allegato O + T (consolidated **2025-12**)  
**Specimen:** [PRODUCT_SPECIMEN_5LN.md](PRODUCT_SPECIMEN_5LN.md) · `product_tr400_eth_a_5ln.cid`  
**Phase gates:** [CCI_Phase_Regulation_Checklists.md](../../knowledge-base/08-engineering/CCI_Phase_Regulation_Checklists.md)  
**Cert book (8 cols + Lines):** [Full Validate.md](Full%20Validate.md)  
**Living lab map (not 5-LN):** `apps/ccli/config/icd/signal_map.yaml`

**Matrix shape:** **67 × 6** (45 Annex O + 22 Annex T rows) — columns **Clause · Shall · SW · 5-LN DUT · Gap · Phase**. After PIXIT waivers, ~**55** rows remain **claimed** on the first 5-LN specimen.

**Legend — 5-LN DUT:** **HAVE** / **PART** / **GAP** / **N/A** / **—** (docs only). **Gap:** open item or *None*.

---

## Annex O → software

| Clause | Shall | SW | 5-LN DUT | Gap | Phase |
|--------|-------|-----|----------|-----|-------|
| O.1–O.4 | Scope, CCI description | Docs / architecture | — | None (spec) | P7 |
| O.5 | Operating modes | PF2 FSM + MMS slaved | Local + DSO | — | P1/P3 |
| O.6.0 PF1 | Monitor + exchange | MMXU + URCB + 104 monitor | PdC only | O.8.4 list, UTC grid | P5-M |
| O.6.1 PF2 | V + P limit (≥100 kW wind/PV: Wlim mandatory) | Wlim/WSd → PF2 → DIO1 | Wlim/WSd HAVE | V-functions waived/stub | P3/P5 |
| O.6.2 / O.10 PF3 | Plant mgmt + MSD | Off | Off | Intentional | N/A |
| O.7.1 | Interface scheme | OpenWrt zones + binds | LAN1/2/3 | LuCI still on OA (P2-04) | P2 |
| O.7.2 | Regulation principle | pf2_fsm + dso_* | Active P | No full Q FSM | P1/P3 |
| O.7.3.1 | TsP ≤60 s, TsQ ≤10 s, MC200 | No settle bench; no 200 ms block | — | GAP | P5-R / P7 |
| O.7.3.2 | ΔT 10–600 s (def 60) | Config/claim | — | Slow ring GAP | P5-R |
| O.7.3.3 | External SP ≥3 s | r29+ gate (lab) | Need confirm on 5-LN | PART | P5-R |
| O.7.4 | 61557-12 blocks, sync V/I | Last Modbus sample | EMT432 | Fixed-block GAP | P5-M |
| O.8.1 | Monitor general | Path exists | — | — | P5-M |
| O.8.2 | Polygon / Smax | plant_envelope yaml 210 kVA | yaml | DPCC LNs not in 5-LN | P5-M09 |
| O.8.3 | P/Q/V @ PdC, 4 s, t+q, :00/:04, 61557-12 | TotW/TotVAr/PPV + URCB 4000 | HAVE objects/Δt | UTC align + block mean | P5-M |
| O.8.4 | TSO: PdC P/Q + by-source + unit ≥1 MW | Lab CID Gen*; 5-LN none | PdC only | N/A at 210 kVA for (c); (b) waive or GAP | P5-M |
| O.8.5 | Instant P/Q to Aggregator (PF3) | 104 TotW last sample | MSD off | N/A | P4 |
| O.8.6 | DG/DDG status ≤4 s | Lab BRCB; 5-LN no XCBR/DGEN | — | GAP (PF1) | P3/P5 |
| O.9.1.1 | cosφ SP (PFSP) | Lab r31; not in 5-LN CID | — | GAP on product model | P5-R |
| O.9.1.2 | cosφ=f(P) (PFW) | WAIVED Figura 2 | — | Curve GAP if DSO requires | P5-R |
| O.9.1.3 | Q=f(V) (VArV) | WAIVED | — | Same | P5-R |
| O.9.1.4 | VArSd DSO Q | Lab IMPL; not in 5-LN | — | GAP on product CID | P5-R |
| O.9.2.1 | P limit @ V≈110% Un | Math stub, no V trip | — | GAP | P5-05 |
| O.9.2.2 | Wlim DSO | WlimDWMX1 → PF2 | HAVE T1-07 | Relay needs P>enter + DIO2 | P3 |
| O.9.2.3 | WSd DSO | WSdDAGC1 | HAVE Operate; Mod often off | T1-08 PART | P3 |
| O.9.3 | Defence plans | Annex M + inhibit | Lab IO3 | Modem HW; 5-LN no SPI LN | P6 |
| O.10.1–.2 | Start / reconnect ramp 20% Pn/min | — | — | N/A PF3 | N/A |
| O.10.3.1–.2 | MSD WSa / VArSa | Stub / yaml command | Off | N/A | P5-R08 |
| O.10.4 | Plant EMS | — | — | OUT of annex | N/A |
| O.11 | Priority table | Active-P min (W110/Wlim/WSd) | PART | Full table + reactive + log of priority change GAP | P5-R07 / P6 |
| O.12 | Installation figures | Drawings | — | Doc PART | P6 |
| O.13.1.1.1 | Eth_A/B, no bridge | Zones P2-01/02/03 | HAVE | Optical 100BaseFX HW (copper lab) | P2 |
| O.13.1.1.2 | Local config USB/serial only | LuCI on .1.130 | — | GAP | P2 |
| O.13.1.2 | DSO loss → autonomous | P3-05 / T1-09 15 s clear Wlim | HAVE | Product delay often ≥60 s (rule) | P3 |
| O.13.1.3 | OA/Aggregator Eth_B + T cyber + prompt data + restore | 104 TLS, P4-03/04 | HAVE port/TLS/restore | Dual publish TotW-only; MSD mode-switch N/A | P4 |
| O.13.1.4–.5 | Plant IF, monitor link | Modbus; GOOSE parked | EMT432 | Plant-link monitor PART; no Eth_A/B reuse HAVE | P5-G |
| O.13.2 / .2.1 | Measure inputs; class 0.2 / CT-VT 0.5 | EMT432 map | P/Q/V live | Accredited class GAP | P5-M |
| O.13.3 | PSU ≥1 h backup | — | — | HW | P7 |
| O.13.4 | Signed FW update | OpenWrt / TPM path | — | PART (T.3.3.4.8) | P7 |
| O.13.5 | UTC GNSS, ±100 ms, drift 1 s/d | GNSS + chrony | HAVE lab | NTS GAP; holdover GAP | P3 / P7 |
| O.13.6 | Autodiagnostic | Event/health partial | — | PART | P7 |
| O.13.7 / .7.1 | HW cyber, secure boot | TG-524 target | — | HW / P7 | P7 |
| O.13.7.2 | Asset inventory | LPHD.PhyNam partial | LPHD in CID | API GAP | P3 / P7 |
| O.13.10 | Reliability A.13 | — | — | Programme OPEN | P7 |
| O.14 | Logger ≥2048, categories, syslog | EventRing r35 | HAVE SW | Link status, PSU, DG/DI GAP | P1→P7 |
| O.15 | Functional / EMC / UCA / 62351-100-3 / 61557-12 / 62443 | Lab gates P0–P6 | Lab CLOSED to P6 | Accredited certs OPEN (P7) | P7 |

---

## Annex T → software

| Clause | Shall | SW | 5-LN DUT | Gap | Phase |
|--------|-------|-----|----------|-----|-------|
| T.1 | 61850 mandatory to DSO | MMS on Eth_A only | :3782 TLS | — | P3 |
| T.3.1 Table 80 | Nameplate W/Q/Smax (DPCC) | yaml smax_kva | No DPCC LN | GAP model | P5-M09 |
| T.3.1.3 | PdC TotW, TotVAr, PPV M; A O; Gen*/St/SGG M in full model | PdC three HAVE | 5-LN = PdC only | Gen* GAP unless plant waived | P5-M |
| T.3.1.4 / Tab 85 | Wlim Mod + WMaxSptPct | HAVE | HAVE | — | P3 |
| T.3.1.4 WSd | WSd Mod + WSptPct | HAVE | HAVE | — | P3 |
| T.3.1.4 VArSd/PFSP/VArV/PFW/WSa | Control LNs | Lab stubs / some IMPL | Not in CID | Product model GAP | P5-R |
| T.3.2.1 | Type 3, 4 s | URCB IntgPd=4000 | T1-05/06 HAVE | Type 3 transit ~500 ms OPEN | P3 / P5-M |
| T.3.3.1 | NS IEC 61850-CEI016:2022, one LD_Plant | SCL 2007B4; LD_Plant | 5 LNs | Full LN set GAP vs TR 57-126 | P3 |
| T.3.3.1.1 | DECP/DGEN/DSTO/XCBR/SGG | Lab CID | Absent | GAP if PF1 status claimed | P3 |
| T.3.3.2 ACSI | Get*, Report; Set RCB N/A (T) | libiec61850; we Set URCB (TesPro) | Works | T says Set RCB N/A — PIXIT HOLD | P3 |
| T.3.3.3 | 8-1 MMS mapping | Ed.2 + SCL B4 | HAVE | Ed 2.1 UCA TP MISSING | P3 / P7 |
| T.3.3.4.1 | RSA ≥2048; ECDSA ≥256 on secp256r1 and brainpoolP256r1 | Source supports RSA/P-256/Brainpool P-256; deployed lab credentials are RSA | PART | Both ECDSA curves are now allowed by the certificate-validation profile; ECDSA-issued lab credentials and target-profile evidence remain open | P3 |
| T.3.3.4.2 | GOOSE security (info) | GOOSE parked/off | off | — | P5-G |
| T.3.3.4.3 | Roles DSO_OPERATOR / AGGREGATOR + 7 | Lab DSO vs VIEWER | P3-08 | IECUserRoles / token A | P3 |
| T.3.3.4.5 | UTC ±100 ms; NTP; NTS; 10 min peer; redundant NTP | chrony + GNSS q | HAVE accuracy | NTS, redundant NTP, 10 min fail | P3 / P7 |
| T.3.3.4.6 | Security event log | O.14 + IEC 62351-14 structured sender and RFC 5425 TLS; see `apps/ccli/docs/IEC62351_14_LOGGER.md` | PART | Final-edition comparison, platform-wide event coverage, clock/storage qualification and collector commissioning | P7 |
| T.3.3.4.7 | Asset ID via LPHD | LPHD1 | Present | Inventory API | P7 |
| T.3.3.4.8 | Secure boot / signed update | Platform | — | PART | P7 |
| T.3.3.4.9 | PKI, CRL ≤24 h, ≥5 roots | Lab CA + CRL | P3-07/09 | SCEP/EST, 5 roots, OCSP | P3 |
| T.3.3.4.10–11 | Segregation + local IF | P2 zones | HAVE | Local IP on OA GAP | P2 |
| 62351-3 | TLS 1.2, port 3782 | MMS + 104 TLS | HAVE | 100-3 cert OPEN | P3 / P7 |
| 62351-4 | ACSE / E2E | ACSE TLS lab | HAVE assoc | Native §13 E2E deferred | P3 |

---

## Gaps that matter (priority)

Must close for a **5-LN PF1+PF2** product claim:

| # | Gap | Clause |
|---|-----|--------|
| 1 | 4 s fixed block + :00/:04 report t | O.8.3 · O.7.4 · T.3.1.3 |
| 2 | O.8.6 DG/DDG (or equivalent) ≤4 s | O.8.6 · T XCBR/DGEN |
| 3 | O.9.2.1 V≈110% | O.9.2.1 |
| 4 | O.11 full priority + log | O.11 |
| 5 | O.7.3.1/2 TsP/TsQ/MC200/ΔT | O.7 |
| 6 | Local maint not IP on Eth_B | O.13.1.1.2 · T.3.3.4.11 |
| 7 | NTS + holdover 1 s/day | T.3.3.4.5 · O.13.5 |
| 8 | O.14 link / PSU / switch status | O.14 |
| 9 | Accredited UCA, 62351-100-3, 61557-12 | O.15 |

**OK to leave** if Operating Rule / PIXIT says so: O.10 / O.8.5 / WSa / VArSa (PF3 off); O.10.4 EMS; O.8.4(b)(c) Gen* at 210 kVA; O.9.1.2 / O.9.1.3 WAIVED Figura 2; VArSd/PFSP not in 5-LN specimen (add LNs or waive PF2-Q); Eth_B Aggregator mode-switch (MSD off); optical Eth_A (HW variant).

**Already mapped and evidenced (do not re-open):** Eth_A MMS TLS + Wlim/WSd · URCB 4 s Δt · TotW/TotVAr/PPV · P3-05 15 s · Eth_B 104 TLS · P2 isolation · P4-04 restore · GNSS q · EventRing 2048.

---

## How the SW layers sit

```text
O.8 / T.3.1.3   Modbus → MeasurementStore → mms_adapter → URCB
O.9.2.2/3       TesPro Operate → dso_phase1 → pf2_fsm → DIO1
O.11 (min)      dso_active_power min(Wlim, WSd, W110)
O.13.1.2        mms fallback 15 s → clear live → yaml
O.13.1.3        iec104 :2404 TLS → log 60 s; restore listen
T.3.3.4         product_cyber_gate + PEM + chrony/GNSS
O.14            EventRing
```

---

## Next cut (cert-facing)

1. **Freeze 5-LN PIXIT** — what you claim vs waive ([PIXIT_DRAFT.md](PIXIT_DRAFT.md)).  
2. **Implement gaps 1–4 only** for first UCA specimen; remainder = HW, accreditation, or PF3.

---

## Revision history

| Rev | Date | Change |
|-----|------|--------|
| 1.0 | 2026-10-22 | Initial 55×6 matrix (O+T → SW → 5-LN → Phase); priority gaps 1–9 |
