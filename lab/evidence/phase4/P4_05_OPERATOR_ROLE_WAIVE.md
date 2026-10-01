# P4-05 — O.14 operator command log — PART (reject audit + Operating Rule config)

**Date:** 2026-09-30  
**Gate:** P4-05 · Annex O **O.14**  
**Owner:** Programme / Operating Rule  
**Build:** `0.1.0-r19`  
**Evidence:** `P4_05_OPERATOR_RULE_VERIFY.txt`

---

## Status

**P4-05: PART (lab)** — Operating Rule is **configurable in yaml**; operator **command ASDUs are rejected and audited** in `monitor_only` mode. Full command accept + parameter store deferred until `operator_rule_mode: command`.

---

## Annex clause traceability

| Clause | CEI 0-16 Allegato O requirement | Lab result |
|--------|----------------------------------|------------|
| **O.13.1.1.1** | Eth_B for remote operators; no internal bridge | 104 on `192.168.1.130:2404` only |
| **O.8.3** | P, Q at POC with quality (4 s block) | TotW IOA 1001 enabled; TotVAr/PPV yaml-off until P5 |
| **O.10.3.1** | Active power set-point on external command | C_SC_NA_1 type 45 **rejected** + logged |
| **O.13.1.3** | Eth_B comms loss → agreed fallback | `comms_loss_fallback_s: 60` (P4-04 PASS) |
| **O.13.5** | Clock / UTC sync | C_CS_NA_1 type 103 **rejected** (`operator_allow_clock_sync: false`) |
| **O.14** | Log commands from authorised external operator **with parameters** | Reject audit: `operator_asdu_reject` + stderr O.14 tag |

Normative extract: `knowledge-base/08-engineering/CCI_Annex_O_Extract.md` §9.1, §11 (O.14 mandatory categories).

---

## Verified log (2026-09-30 lab)

**DUT startup:**
```
iec104: Operating Rule mode=monitor_only allow_commands=no gi=yes monitor: TotW@1001=on [O.14]
```

**Reject-test (C_SC_NA_1 + C_CS_NA_1 from `p4_cs104_client --reject-test --tls`):**
```
iec104: operator command rejected type=45 mode=monitor_only allow_commands=0 [O.14 / Operating Rule]
iec104: O.14 operator ASDU type=45 operator_asdu_reject
iec104: clock sync rejected type=103 [Operating Rule monitor_only]
iec104: O.14 operator ASDU type=103 operator_asdu_reject
```

---

## Implemented (r19)

| Item | Detail |
|------|--------|
| Config | `iec104:` keys in `lab.yaml` — see `apps/ccli/config/iec104_operator_rule.yaml` |
| Catalog | `iec104_operator_rule.hpp` — annex monitor/command type IDs + clause refs |
| Reject path | `asduHandler` + GI gate — stderr + `operator_asdu_reject` event |
| Accept path | When `operator_rule_mode=command` and `operator_allow_commands=true` (future MSD) |
| Monitor IOAs | TotW (1001 on), TotVAr/PPV off until P5 |

---

## Pass criteria

| Criterion | Lab result |
|-----------|------------|
| Rejected operator ASDU → audit event | **PASS** — see `P4_05_OPERATOR_RULE_VERIFY.txt` |
| Allowed parameters configurable per annex catalog | **PASS** (yaml + catalog) |
| Accepted command + params in persistent store | **DEFER** — requires command mode + Phase 7 |

---

## VERDICT

**P4-05: PART (lab)** — reject audit + Operating Rule config **verified on DUT**. Re-open for **PASS** when operator writes (O.10.3.1) are enabled and full parameter logging is required.
