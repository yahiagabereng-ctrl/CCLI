# CCLI unified cert — handoff for Alireza (@alirezavzand)

**Date:** 2026-10-14  
**Repo:** https://github.com/yahiagabereng-ctrl/CCLI  
**GitHub:** `alirezavzand` (collaborator)

---

## Your roles on the bench

| Role | When |
|------|------|
| **PC-A (plant / logger)** | Two-PC unified run — LAN2/LAN3/COM5, SSH, 104, P7, **U-T1-11** |
| **TLS / Wireshark / PKI** | Deep review when Masomeh (PC-B TSP) or DUT sends pcaps / ACSE traces |

**Do not** run Test Suite Pro on **LAN1** if you are PC-A only ([TSP_TWO_PC_BENCH_RUNBOOK.md](../../lab/conformance/TSP_TWO_PC_BENCH_RUNBOOK.md)).

---

## Start here

1. **[PCA_UNIFIED_CERT_START_ALIREZA.md](PCA_UNIFIED_CERT_START_ALIREZA.md)** — matrix rows + commands  
2. **[MESSAGE_TO_SEND.txt](MESSAGE_TO_SEND.txt)** — short copy-paste for chat  
3. Master checklist: **`lab/conformance/LAB_UNIFIED_CERT_TEST_MATRIX.md`** rev **1.3**  
4. Session pin: **`lab/conformance/LAB_UNIFIED_SESSION_PIN_2026-10-14.md`** (DUT **0.1.0-r53**)

---

## TLS / EJBCA (when asked)

| Doc | Use |
|-----|-----|
| [TLS_WIRESHARK_HANDOFF_ALIREZA.md](../../lab/evidence/TLS_WIRESHARK_HANDOFF_ALIREZA.md) | P3-07 / MMS TLS decode |
| [EJBCA_HANDOFF_ALIREZA.md](../../lab/evidence/EJBCA_HANDOFF_ALIREZA.md) | Lab CA · PEM regen |
| [lab/EJBCA_LAB_SETUP.md](../../lab/EJBCA_LAB_SETUP.md) | Full runbook |

Field pack on PC-B also has **`00k_PC1_EJBCA_TLS_TESTPRO.md`** (Masomeh TSP paths).

---

## Evidence you produce (PC-A)

| Pattern | Matrix rows |
|---------|-------------|
| `PREFLIGHT_LAN2_*.txt` · `modbus_emt432_*.txt` | U-P0-03/04, U-P0b-01 |
| `PLANT_SUN2000_*.txt` | U-P0b-02 |
| `PREFLIGHT_DUT_UNIFIED_*.txt` | U-P0-07, U-P0-11 |
| `phase4/P4_*` | U-P4-01/02 |
| `phase7/P7_*` | U-P7-01/02/04 |
| `P3_11_*` (chrony) | U-P3-11 |
| Append to `*_DUT_LOG` after Wlim | U-T1-11 |

Return USB or git → `lab/evidence/` on repo PC (**U-CLO-01**).
