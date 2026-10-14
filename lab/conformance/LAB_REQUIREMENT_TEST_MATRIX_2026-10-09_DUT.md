# Lab matrix — DUT finalize snapshot (2026-10-09)

**Source:** [LAB_REQUIREMENT_TEST_MATRIX.md](LAB_REQUIREMENT_TEST_MATRIX.md) rev 1.2  
**DUT:** `ccli 0.1.0-r52` · CID History **rev 9** (target)  
**Purpose:** Milestone **A** only — DUT/TSP bench closed; **not** accredited certificate.

**Legend:** ☑ PASS · ◐ PART (waiver/evidence) · ☐ OPEN · — N/A first-pass PICS

---

## Session

| Field | Value |
|-------|--------|
| Date | 2026-10-09 |
| Operator | *(fill)* |
| TSP version | *(fill)* |
| `ccli_pkg` | **0.1.0-r52** |
| Specimen pin | [LAB_SPECIMEN_VERSION.md](LAB_SPECIMEN_VERSION.md) |

---

## Master matrix (DUT-focused rows)

| Status | Lab / doc | TSP ID | Evidence | Note |
|--------|-----------|--------|----------|------|
| ☐ | D1 PICS Excel to lab | — | `PICS_*_draft.xlsx` | **Cert quote** — not DUT bench |
| ☐ | D2 PIXIT Word | — | PIXIT_DRAFT.md | Cert package |
| ◐ | D3 MICS + D5 Compare | T1-02 | `TSP_P3_COMPARE_*` + waivers | T1–T10 done; rev **9** cfg deploy ☐ |
| ☐ | D4 TICS | — | — | Lab date |
| ◐ | Specimen identity | — | headers in `TSP_P3_*` | Set **PASS** on all T1 stubs |
| ◐ | Server / 8-1 / Connect | T1-01 | `TSP_P3_01_CONNECT_*` | T1-10 **PASS** implies path OK |
| ◐ | Browse ~31 LN | T1-03 | `TSP_P3_03_BROWSE_*` | Close PENDING |
| ◐ | Read PdC | T1-04 | `TSP_P3_03_READ_PdC_*` | Close PENDING |
| ◐ | URCB 4 s | T1-05–06 | `TSP_P3_03_REPORT_*` | IntgPd 4000 ms |
| ◐ | Wlim | T1-07 | `TSP_P3_04_OPERATE_Wlim_*` | Close PENDING |
| ☑ | WSd | T1-08 | `TSP_P3_04_OPERATE_WSd_*` | PASS |
| ☑ | Comms fallback 15 s | T1-09 | `TSP_P3_05_FALLBACK_*` | PASS |
| ☑ | Reconnect | T1-10 | `TSP_P3_01_RECONNECT_*` | PASS |
| ◐ | TLS :3782 | T1-01 | Connect session | Not cleartext 102 |
| ☐ | TLS fail no cert | T3-01 | `TSP_P3_06_TLS_FAIL_*` | Recommended pre-ship |
| ☐ | RBAC viewer | T3-02–03 | `TSP_P3_08_*` | Recommended pre-ship |
| ☐ | CRL revoked | T3-04 | `TSP_P3_09_*` | Skip if crl=off |
| ◐ | Time quality | P3-11 | `TSP_P5_FULLCIRCLE_TLS_*` §1 | chrony in log |
| ◐ | P5 full circle TLS | Tier 2 | `TSP_P5_FULLCIRCLE_TLS_2026-10-09.txt` | Cross-ref T1 |
| ☐ | Actuation dido | — | SSH `dido_v2` | After Wlim |
| — | BRCB / GOOSE | Tier 4 | — | **N/A** first PICS |
| ◐ | IEC 104 :2404 | P4 | `phase4/P4_*_2026-10-09*` | Not in 61850 PICS |
| ☐ | P7-01 wrap | — | `P7_01_WRAP_*` on **r52** | **Cert / O.15 ship** |
| ☐ | P7-02 dump | — | `P7_02_DUMP_*` | Same |
| ☐ | P7-11 demo | — | `P7-11_DSO_DEMO_*` | Same |
| ☐ | **Formal UCA certificate** | — | Lab report | After PICS + lab test |

---

## DUT hardware preflight (rev 9)

| Status | Item |
|--------|------|
| ☐ | cfg SHA **E3285E8F…** on DUT |
| ☐ | `iec61850service` **stopped** |
| ☐ | Single **ccli** · **3782** LISTEN |

---

## Verdict

| Gate | Result |
|------|--------|
| **DUT Tier 1 (T1–T10)** | **☑ PASS** in TSP (2026-10-09); **◐** until rev **9** cfg on DUT + optional T3/P7 |
| **Ready for PICS → lab quote** | **☐** until D1 Excel sent |
| **Ready for accredited certificate** | **☐** until lab test + P7 on release image |

Update this file when □ flip to ☑.
