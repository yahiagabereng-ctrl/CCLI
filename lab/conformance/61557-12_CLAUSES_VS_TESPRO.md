# 61557-12:2018 Ed 2.0 clauses vs TesPro respond (TesPro only)

**Document ID:** CCLI-LAB-61557-12-CLAUSES-VS-TESPRO-001  
**Revision:** 1.1  
**Date:** 2026-10-21  
**RAG source_id:** `ccli-61557-12-clauses-vs-tespro`  
**Scope:** TesPro **TG544 / TG-524** (`platform_tg500`) **only**. Analyzer, CT/VT, `ccli`, Chronos, ACCREDIA #2, HiTEKS datasheet = **out** (see vendor map / Full Validate).  
**Normative:** IEC **61557-12:2018 Edition 2.0** — [CCI_61557-12_2018_Ed2_Extract.md](../../knowledge-base/08-engineering/CCI_61557-12_2018_Ed2_Extract.md)  
**TesPro reply:** Ask **#6** · `TesPro-Certification-Scope-Allocation-Statement-EN.pdf` **v1.0** 2026-10-21 (**unsigned**)  
**Ask #2:** CE/DoC → CNAS L5868 + A2LA 4674.01 **menus** (no TG544 report)

**Edition pin:** 2018 Ed 2.0 has **no EPMF / no Annex H**. TesPro never wrote those words.

**TesPro object they named:** communication / data-forwarding gateway · SELV · Ethernet / cellular / DIO · **no measuring inputs**.

**Verdict key (this file)**


| Tag             | Meaning                                                            |
| --------------- | ------------------------------------------------------------------ |
| **N/A ALIGNED** | PMD clause. TesPro correctly refused. Do not ask again on this SKU |
| **HAVE**        | TesPro delivered the artefact                                      |
| **GAP TesPro**  | Still TesPro’s box even if 61557 is N/A                            |
| **CONFLICT**    | Only if TesPro puts **61557-12:2018 Ed 2.0** on the TG544 CB       |


---



## Summary

TesPro Scope §3 row 1:S

> Communication / data-forwarding gateway only (no measuring inputs, SELV supply, Ethernet / cellular / digital I/O) → **IEC/EN 62368-1**; **EN 61326-1 Class A** or **EN 61000-6-x**. **Neither the IEC 61557 series nor the IEC 61010 series applies.S**

That matches **§1**: object = **PMD**. TG544 has no V/I/CT terminals → **outside §1**. **Not PMD. Not EPMF.S**

**TesPro score:** split **HAVE** (unsigned) · 61557/61010 **N/A ALIGNED** · K55 **GAP** · TG544 62368+61326 reports **GAP**.

---



## Requirements



### TesPro letter (every row)


| TesPro §      | Their words                                                                                                                          | File                      |
| ------------- | ------------------------------------------------------------------------------------------------------------------------------------ | ------------------------- |
| **1.1**       | TesPro certifies the **gateway platform** baseline (HW rev, FW, PSU, enclosure, interfaces): safety + EMC                            | Scope p.1                 |
| **1.2**       | Integrator certifies the finished branded product. TesPro reports are **inputs**, not the final cert                                 | Scope p.1                 |
| **1.3**       | Enclosure / PSU / wiring / nameplate / port changes → integrator. Baseline-affecting → TesPro retest                                 | Scope p.1                 |
| **2**         | TesPro covers insulation, creepage, withstand, temperature rise, flammability, accessibility, **EMC**, **environmental performance** | Scope p.1                 |
| **3 row 1**   | Comms-only → **62368-1 + 61326-1 / 61000-6-x**. **61557 N/A. 61010 N/A**                                                             | Scope p.1                 |
| **3 row 2**   | If V/I/temperature/CT inputs or DO driving loads → **61010-1** + **61010-2-030**. **61557 still N/A**                                | Scope p.2                 |
| **3 row 3–5** | 61557-1 test set / 62053 MID / 61851                                                                                                 | **Not this SKU** — ignore |
| **Note**      | 61557-12 only if it is on **their CB**. Until then it is **not** on the platform cert                                                | Scope p.2                 |
| **4**         | Not a new cert commitment. Signatures **blank**                                                                                      | Scope p.2                 |
| Ask **#2**    | CE/DoC → “Recommended: CNAS / A2LA”                                                                                                  | xlsx Hiteks               |
| Ask **#6**    | Status = Scope Allocation Statement                                                                                                  | xlsx Hiteks               |




### Clauses TesPro must answer (this SKU)


| Clause                   | Shall (Ed 2.0)                                      | TesPro respond                                                                                              | Verdict                                                                                      |
| ------------------------ | --------------------------------------------------- | ----------------------------------------------------------------------------------------------------------- | -------------------------------------------------------------------------------------------- |
| **§1**                   | Object = **PMD**                                    | No measuring inputs. **61557 series does not apply**                                                        | **N/A ALIGNED**                                                                              |
| **§3.1.1**               | PMD = meters/monitors electrical parameters         | “data-forwarding gateway only”                                                                              | **N/A ALIGNED**                                                                              |
| **§3.1.17 / Annex H**    | **Not in Ed 2.0**                                   | **0 hits** EPMF / Annex H / SS                                                                              | **N/A** — do not file their letter as EPMF                                                   |
| **§4.4 Table 2**         | SS/SD/DS/DD                                         | No V/I sensors on TG544                                                                                     | **N/A ALIGNED**                                                                              |
| **§4.6.2.3 Table 6 K55** | Fixed rated **−5…+55 °C**                           | §2 claims **environmental performance**. **No K55 / 60068 report**. A2LA 4674.01 has **no** climate methods | **GAP TesPro**                                                                               |
| **§4.6.2.4 Table 7**     | Humidity 0–75 % RH (no cond.)                       | Same — no env report                                                                                        | **GAP TesPro**                                                                               |
| **§4.9.2 IP**            | Document IP (60529) for **their** enclosure         | Baseline enclosure is TesPro’s object (§1.1 / §2)                                                           | **GAP TesPro** if they claim a platform IP (report ID)                                       |
| **§4.10.1**              | **61010-1 + 61010-2-030**                           | **61010 series does not apply**. Row 2 only if measuring I/O added                                          | **N/A ALIGNED**. **CONFLICT** if they add Ed 2.0 to the CB                                   |
| **§4.11**                | PMD: 61326-1:2012 Table 2                           | They name **61326-1 Class A / 61000-6-x** as **ITE**. Lab **can** test (4674.01). **No TG544 report**       | **Named ALIGNED**. Report **GAP TesPro**                                                     |
| **§5**                   | 61010 marking + Table 45 PMD form                   | They do not claim PMD. Nameplate of **baseline** is TesPro’s if they issue a platform cert                  | **N/A ALIGNED** as 61557 Table 45. Platform marking = 62368 path **GAP** until report exists |
| **§6.2.15**              | K55: +70 °C / −25 °C / damp +55 °C 6 cycles (60068) | Env claimed; **no report**                                                                                  | **GAP TesPro**                                                                               |
| **§6.2.16**              | PMD EMC vs Tables 9/13/17                           | ITE 61326/61000-6 — not PMD tables                                                                          | **N/A ALIGNED** as PMD. ITE EMC report **GAP TesPro**                                        |
| **Ask #2 / CE**          | Platform CE/DoC they said they certify (§1.1)       | CNAS/A2LA **menus** only                                                                                    | **GAP TesPro**                                                                               |
| **Ask #6 / §4 sign**     | Written split                                       | PDF **HAVE**, signatures **blank**                                                                          | **HAVE / PART**                                                                              |




### PMD clauses TesPro refused (do not ask again)

One row covers: §3.4.2 gapless · §4.2 Fig 1 · §4.3 Table 1 · §4.4.2 · §4.5 Table 3 · §4.6.1 · §4.6.2.2 K40 · §4.7 · **all §4.8** (P/Q/S/f/I/U/PF/PQ) · §4.8.17 · **Annex B/C/D/E/F/G** · §4.9.1 61557 vibration · §4.10.1.2–1.6 / 4.10.2–3 as 61010 · §4.12 analog/pulse · **§6.2.2–6.2.14 / 6.2.17 / 6.2.18 / 6.2.19 / 6.3**.


| TesPro respond                                                       | Verdict         |
| -------------------------------------------------------------------- | --------------- |
| No measuring inputs · no PMD functions · 61557 series does not apply | **N/A ALIGNED** |




### If TesPro adds 61557-12 to the **TG544 CB**


| Path                   | What they put on the CB | TesPro then owes                                                 | Conflicts §3 row 1?                                     |
| ---------------------- | ----------------------- | ---------------------------------------------------------------- | ------------------------------------------------------- |
| **Stay off (current)** | 61557 **absent**        | **62368-1 + 61326-1** reports + **K55/60068** + **signed** Scope | **No**                                                  |
| **B**                  | **2018+AMD1:2021** only | Written EPMF/Annex H on **their** cert list + still K55          | **No** (still not PMD) — they have **not** offered this |
| **A**                  | **2018 Ed 2.0 only**    | Full PMD including **§4.10 61010-2-030**                         | **YES — CONFLICT**                                      |


**Tell TesPro:** do **not** add IEC 61557-12:**2018 Ed 2.0** to the TG544 certificate.

---



## Architecture



### Functional

TesPro object = **platform** safety + EMC + env of the **baseline SKU**. Not a PMD.

### Hardware


| Block              | TesPro respond                                                                            |
| ------------------ | ----------------------------------------------------------------------------------------- |
| TG544              | Comms-only · 61557 N/A · 61010 N/A                                                        |
| Measuring I/O      | None on this SKU. If added → their row 2 (61010-1 + 2-030; 61557 still N/A)               |
| Shenzhen Microtest | CNAS L5868 + A2LA 4674.01 = **62368 / 61326 menu**. Not a 61557 lab · **no TG544 report** |




### Security

Out of this file (FIPS / CC / IDevID).

### Power

SELV auxiliary. §4.4.2 self-powered PMD **N/A**.

---



## Interface matrix


| Interface   | Source      | Destination           | Artefact                           |
| ----------- | ----------- | --------------------- | ---------------------------------- |
| Ask #6      | HiTEKS xlsx | TesPro                | Scope Allocation v1.0              |
| §3 row 1    | TesPro      | This file             | 62368-1 + 61326-1; 61557/61010 N/A |
| §3 row 2    | TesPro      | Future SKU only       | 61010 if I/O added                 |
| Ask #2      | TesPro      | HiTEKS technical file | CE/DoC — **menus only**            |
| K55 / 60068 | TesPro      | Platform file         | **Report missing**                 |


---



## BOM matrix


| Block         | Function                        | TesPro file                                              | Status                        |
| ------------- | ------------------------------- | -------------------------------------------------------- | ----------------------------- |
| Scope §3      | 61557/61010 refuse              | `TesPro-Certification-Scope-Allocation-Statement-EN.pdf` | **HAVE — unsigned**           |
| Ask #6        | Written split                   | xlsx Hiteks                                              | **HAVE**                      |
| CNAS L5868    | Lab identity                    | `01 CNAS.pdf`                                            | **HAVE — not a TG544 report** |
| A2LA 4674.01  | 62368 / 61326 / 61000-6 methods | `4674-01.pdf`                                            | **HAVE — menu**               |
| TG544 62368-1 | Platform safety report          | —                                                        | **GAP TesPro**                |
| TG544 61326-1 | Platform EMC report             | —                                                        | **GAP TesPro**                |
| K55 / 60068   | Table 6 + §6.2.15               | —                                                        | **GAP TesPro**                |
| 61557-12 cert | PMD type-test                   | TesPro will not issue                                    | **N/A ALIGNED**               |


---



## Knowledge gaps


| Area              | TesPro said                  | Required from TesPro                   | Gap      |
| ----------------- | ---------------------------- | -------------------------------------- | -------- |
| Signature         | Blank §4                     | Signed + sealed                        | **PART** |
| K55               | “environmental performance”  | Report ID · −5…+55 °C · 60068-2-1/2/30 | **MISS** |
| 62368-1           | Named §3                     | **TG544** report number                | **MISS** |
| 61326-1 / 61000-6 | Named §3                     | **TG544** report number                | **MISS** |
| CB list           | 61557-12 only if on their CB | Written: Ed 2.0 **will not** be added  | **MISS** |


---



## Risks


| ID       | Description                        | Impact                                   | Mitigation                       |
| -------- | ---------------------------------- | ---------------------------------------- | -------------------------------- |
| R-TV-001 | File this letter as EPMF / Annex H | False TesPro 61557 PASS                  | Verdict is **not PMD**, not EPMF |
| R-TV-002 | TesPro adds 61557-12:2018 only     | §4.10 forces 61010-2-030                 | Keep 61557 **off** the CB        |
| R-TV-003 | Skip K55 because “61557 N/A”       | Platform env they claimed in §2 is empty | Demand 60068 report              |
| R-TV-004 | File CNAS/A2LA as product cert     | Menu ≠ TG544 report                      | Demand report IDs                |


---



## Verification


| Requirement       | Test                      | Pass                                                  |
| ----------------- | ------------------------- | ----------------------------------------------------- |
| §1 vs §3 row 1    | Read Scope p.1            | “no measuring inputs” + “61557 series does not apply” |
| §4.10 vs §3 row 1 | Same page                 | “61010 series does not apply”                         |
| Path A            | 61557-12:2018 on TG544 CB | **FAIL**                                              |
| Table 6 / §6.2.15 | TesPro report             | K55 −5…+55 °C · 60068 dry/cold/damp                   |
| §4.11 ITE         | TesPro report             | 61326-1 Class A or 61000-6-x **on TG544**             |
| Ask #2            | TesPro report / DoC       | Report numbers on **this SKU**                        |
| Signature         | Scope p.2                 | TesPro signed                                         |


**Ask TesPro (only these five):**

1. Sign Scope §4.
2. TG544 **62368-1** report ID.
3. TG544 **61326-1** or **61000-6-x** report ID.
4. **K55 / IEC 60068** report ID.
5. Written: **61557-12:2018 Ed 2.0 will not** be added to the CB.

Do **not** ask TesPro for §4.8, §6.2.18, Annex D, Annex F, Table 3/8/12/26, EPMF, or ACCREDIA.

---



## Related


| Doc                                                                                                      | Role                 |
| -------------------------------------------------------------------------------------------------------- | -------------------- |
| [61557-12_TESPRO_VENDOR_MAP.md](61557-12_TESPRO_VENDOR_MAP.md)                                           | Ask #1–9 TesPro pack |
| `CCI_TesPro_Cert_Reply_2026-10_Extract.md`                                                               | 2026-10-21 drop      |
| [CCI_61557-12_2018_Ed2_Extract.md](../../knowledge-base/08-engineering/CCI_61557-12_2018_Ed2_Extract.md) | Clause text          |


---



## Revision history


| Rev | Date       | Change                                               |
| --- | ---------- | ---------------------------------------------------- |
| 1.0 | 2026-10-21 | Every Ed 2.0 clause vs Scope                         |
| 1.1 | 2026-10-21 | **TesPro only** — PMD/analyzer/ACCREDIA rows removed |


