# P5 — DSO verification client status (2026-10-01)

**Document ID:** P5-VER-CLIENT-001  
**Date:** 2026-10-01  
**Gate:** P5_FULLCIRCLE (lab **CLOSED** on build **0.1.0-r25**)

---

## Summary

| Client | Version | Lab role | Status |
|--------|---------|----------|--------|
| Triangle MicroWorks **Test Suite Pro** | 4.7.4.5037 | Primary DSO MMS client (Compare, Report Viewer, Sequencer) | **BLOCKED** — license |
| **[IEDExplorer](https://sourceforge.net/projects/iedexplorer/)** | beta (SourceForge) | Alternate DSO MMS client | **ACTIVE** for manual re-runs |
| **`mms_lab_client`** + PowerShell scripts | repo | Automated cleartext full-circle | **PASS** (r25 evidence) |
| **TesPro `iec61850d`** | OEM on TG544 | Northbound MQTT collector | **PASS** — not a DSO substitute |

---

## Test Suite Pro — license block

**Symptom:** Application starts; **“No license for this product could be found!”**

**Sentinel trial:** Key Id `1099954349370049388` — **expired** 2026-10-01 (machine-bound).

**Actions taken (lab PC):**

- Full uninstall/reinstall of TMW Test Suite Pro 4.7.4.5037
- Removed Program Files / ProgramData / AppData TMW trees
- Partial removal of Sentinel `installed\102099` token (backup under `C:\Yahia\University\TMW_OLD_TRIAL_BACKUP_*`)
- Helper script: `C:\Yahia\University\KILL_TMW_OLD_TRIAL.bat` (Run as administrator)
- Support draft: `C:\Yahia\University\TMW_TSP_SUPPORT_EMAIL_DRAFT.txt`

**Outcome:** Reinstall recreates the **same** expired provisional key — fresh trial not issued without **TMW support Product Key / `.v2c`**.

**Evidence impact:** P5 lab gate closed on **r25 automated log + prior TSP session exports**. No new TSP Compare or Sequencer exports until license restored.

---

## IEDExplorer — alternate DSO client

**Use when:** TSP license unavailable; need manual connect/read/Operate/URCB on cleartext MMS.

| Connection | Value |
|------------|-------|
| IED | `CCI016_01` |
| Host | `192.168.10.1:102` |
| TLS | **Off** (lab profile) |
| CID reference | `apps/ccli/config/icd/lab_tg544_eth_a.cid` |

**Can replace (lab):** Steps in [P5_TSP_FULLCIRCLE_SEQUENCER.md](P5_TSP_FULLCIRCLE_SEQUENCER.md) — connect, GetNameList, Read TotW/TotVAr/PPV, Direct Operate Wlim/WSd/VArSd, enable URCB, inspect reports.

**Cannot replace:**

- TSP **Compare Model** (step 12) — reuse [TSP_P5_COMPARE_FIX_2026-10-01.md](../testsuite-pro/inbox/TSP_P5_COMPARE_FIX_2026-10-01.md) + waivers
- Offline **SCL Verify** — reuse `testsuite-pro/offline-scl/TSP_OFF_SCLVERIFY_*`
- TSP Sequencer export format — use IEDExplorer screenshots + manual log with metadata header from [testsuite-pro/README.md](../testsuite-pro/README.md)

**Export naming (IEDExplorer manual runs):**

```text
lab/evidence/testsuite-pro/inbox/IEDEX_P5_FULLCIRCLE_CLEARTEXT_<YYYY-MM-DD>_<HHMMSS>.txt
```

---

## TesPro northbound (separate path)

TesPro MQTT collector validates **T.3.1.3** cross-check only. It does **not** satisfy DSO MMS gate requirements.

**PASS evidence:** [P5_TESPRO_NORTHBOUND_2026-10-01.md](P5_TESPRO_NORTHBOUND_2026-10-01.md)

---

## Recovery checklist (TSP)

1. Run `KILL_TMW_OLD_TRIAL.bat` as Administrator (or TMW support reset).
2. Request new trial / Product Key from Triangle MicroWorks support.
3. Reinstall TSP only after Sentinel token cleared.
4. Re-export Compare **before** URCB enable per sequencer order.

---

## Related

- [P5_README.md](P5_README.md)
- [lab/CCI_TestSuitePro_Verification_Layer.md](../../CCI_TestSuitePro_Verification_Layer.md)
- [lab/evidence/README.md](../README.md)
