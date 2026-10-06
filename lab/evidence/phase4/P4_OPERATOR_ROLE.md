# Operatore Abilitato (Eth_B) — role and rules

**Date:** 2026-09-30 (updated 2026-10-05)  
**Programme:** HiTEKS CCLI · CEI 0-16 Allegato O  
**Build:** `ccli-0.1.0-r19` (104 TLS + Operating Rule config on Eth_B)  
**Product HMI:** **Cloud UI** — [CCI_Cloud_Operator_HMI.md](../../../knowledge-base/08-engineering/CCI_Cloud_Operator_HMI.md) · LAN2 wire pack: [LAN2_READINESS.md](LAN2_READINESS.md)

---

## Who is the operator?

| Term | Meaning |
|------|---------|
| **Operatore Abilitato (OA)** | *Qualified / authorised remote operator* on **Eth_B** (Annex O **O.13.1.1.1**) |
| Lab role | Aggregator / BSP / third-party **supervision client** — **not** the DSO |
| **DSO** | Separate actor on **Eth_A** only (Annex T MMS `:3782`) |
| **Human operator (product)** | **HiTEKS cloud UI/HMI** over LTE/WAN — not local panel; see cloud architecture doc |

Normative **wire** for machine clients: Eth_B — RJ45, isolated from Eth_A (Phase 2 PASS).  
Normative **human** interface: cloud (REQ-OP-CLOUD-001); Eth_B 104 remains for direct SCADA.

---

## What the operator does (lab / product slice)

| Allowed on Eth_B (104 TLS `:2404`) | Not on Eth_B (DSO / Annex T) |
|-----------------------------------|------------------------------|
| Connect (TLS 62351-3), STARTDT/STOPDT | `Wlim` / `WSd` active-power commands |
| General interrogation (GI) | IEC 61850 MMS server |
| Read monitored quantities (e.g. **TotW** IOA 1001, M_ME_NC_1) | Replace or bypass Eth_A regulation |
| Supervision / test (lib60870 lab client) | Internal bridge to Eth_A or plant |

**Operating Rule (lab):** Eth_B 104 is **monitor and supervision only** (`operator_rule_mode: monitor_only`). Command ASDUs are **rejected and logged** (O.14 reject audit). Allowed annex parameters are set in yaml.

### Configurable parameters (`/etc/ccli/lab.yaml` → `iec104:`)

Reference template: `apps/ccli/config/iec104_operator_rule.yaml`  
Catalog: `apps/ccli/adapters/iec60870_104/iec104_operator_rule.hpp`

| Yaml key | Lab default | Annex / function |
|----------|-------------|------------------|
| `operator_rule_mode` | `monitor_only` | O.14 — no command accept |
| `operator_allow_commands` | `false` | O.10.3.1 MSD (future) |
| `operator_allow_gi` | `true` | Supervision GI |
| `operator_allow_clock_sync` | `false` | C_CS_NA_1 |
| `operator_monitor_tot_w` | `true` | O.8.3 / T.3.1.3 TotW |
| `ioa_tot_w` | `1001` | M_ME_NC_1 IOA |
| `operator_monitor_totvar` | `false` | TotVAr (P5) |
| `ioa_tot_var` | `1002` | M_ME_NC_1 IOA |
| `operator_monitor_ppv` | `false` | PPV (P5-M08) |
| `ioa_ppv` | `1003` | M_ME_NC_1 IOA |

---

## Who sets commands?

| Source | Interface | Protocol | Function |
|--------|-----------|----------|----------|
| **DSO** | Eth_A `192.168.10.1:3782` | MMS TLS | **Commands** — O.9.2.2 `Wlim`, O.9.2.3 `WSd` → PF2 → DIO1 |
| **Operator (OA)** | Eth_B `192.168.1.130:2404` | 104 TLS | **Monitor only** — read TotW, GI (P4-02/03) |
| **Aggregator MSD** (future) | Eth_B | 104 or 61850 | O.10.3.1 set-points — **deferred** (`iec104_regulation.hpp`) |
| **Autonomous CCI** | Internal | Timers | O.13.1.2 Eth_A loss (P3-05, 15 s); O.13.1.3 Eth_B loss (P4-04, 60 s) |
| **User / site** | Operating Rule | Manual | Offline reduction procedure (O.9.2.2) — not 104 |

**CEI core (Annex T) remains on Eth_A.** 104 on Eth_B does not carry DSO Figura 2 control in this release.

---

## Annex mapping (operator path)

| Clause | Requirement | Phase 4 gate |
|--------|-------------|--------------|
| O.13.1.1.1 | Eth_B for enabled remote operators; no bridge | P4-01 PASS |
| IEC 60870-5-104 | Session + supervision | P4-02 PASS |
| IEC 62351-3 | TLS on operator port | P4-03 PASS |
| O.13.1.3 | Eth_B comms loss → agreed fallback + restore | P4-04 PASS |
| O.14 | Log **commands** from authorised external operator | **P4-05 PART** — reject audit + yaml Operating Rule |

Annex **T** applies to **DSO Eth_A**, not operator 104.

---

## Evidence index

| Gate | File |
|------|------|
| P4-01 | `P4_01_ETH_B_ONLY.txt` |
| P4-02 | `P4_02_104_SESSION.txt` |
| P4-03 | `P4_03_104_TLS.pcapng`, `P4_03_104_TLS_ANALYSIS.txt` |
| P4-04 | `P4_04_104_FALLBACK.txt` |
| P4-05 | `P4_05_OPERATOR_RULE_VERIFY.txt`, `P4_05_OPERATOR_ROLE_WAIVE.md` |
| Close | `SESSION_REPORT_P4_CLOSE_2026-09-30.md` |
