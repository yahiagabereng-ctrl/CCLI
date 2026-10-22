# TestPro (TesPro) — vendor requirements handoff (2026-10-21 pack)

**Date:** 2026-10-22  
**Full map:** [61557-12_TESPRO_VENDOR_MAP.md](../../lab/conformance/61557-12_TESPRO_VENDOR_MAP.md)  
**Extract:** [CCI_TesPro_Cert_Reply_2026-10_Extract.md](../../knowledge-base/08-engineering/CCI_TesPro_Cert_Reply_2026-10_Extract.md)  
**O.15 matrix:** [Full Validate.md](../../lab/conformance/Full Validate.md) § **A.8**

**Scope:** TesPro **TG544/TR500** = **comms gateway only** (no V/I). **61557-12 does not apply** to TesPro box per scope letter. **HiTEKS** owns **O.15 #2** on **assembled CCI** (meter + `ccli`).

---

## 9-row vendor ask — minimal HiTEKS status

| NO. | Deliverable | Minimal comment |
|-----|-------------|-----------------|
| **1** | TPM FIPS 140-2 **L3** | **PARTIAL.** L2 policy only — need CMVP **L3** cert no. **or** written **no FIPS L3 / no O.15 #3** claim. |
| **2** | CE + DoC TG544 | **PARTIAL.** CNAS/A2LA = lab scope only — need **TG544 CE DoC** + EMC/safety **report IDs**. |
| **3** | ISO 9001 | **OPEN** — ISO 9001 PDF not in drop. |
| **4** | Secure Boot | **PARTIAL** — guide v0.1; need signed release + fuse evidence. |
| **5** | Anti-tamper Hall | **PARTIAL** — datasheet only; need GPIO/ubus map + tamper event path. |
| **6** | 61557-12 / 61010 path | **ALIGNED** — scope letter: TesPro comms-only; **61557/61010 N/A** on gateway; HiTEKS owns **#2** (Chronos meter). |
| **7** | Image identity | **PARTIAL** — image name given; freeze **25.12** tag + SHA256. |
| **8** | 61850 coexist :3782 | **PARTIAL** — policy v0.1; confirm DUT :3782 + no conflicting 61850 ipks. |
| **9** | IDevID | **PARTIAL** — options draft; pick A/B/C + factory enrolment in writing. |

---

## O.15 §12 mapping (do not confuse with row numbers)

| O.15 # | Standard | TesPro pack |
|--------|----------|-------------|
| **#2** | 61557-12 + 61010 + K55 + 17025 | **NOT TesPro** — use **Chronos** + HiTEKS assembly |
| **#3** | FIPS 140-2 L3 | Row **1** — **L2 ≠ L3** |
| **#8** | CE + DoC | Row **2** — lab cards ≠ DoC |

---

## E2E (programme)

**PART Milestone A.** TSP/MMS bench OK on **5-LN**; vendor rows **1–2** block **#3/#8** claims; **#2** needs **Chronos** + ACCREDIA type-test — not closed on TesPro xlsx alone.

**Cyber E2E:** TLS + dual-cert bench **PASS**; **62351-4** native E2E **PART** — **100-3** for O.15 **#6**.

---

## Masomeh (PC-B) — action

- Keep running **TSP** per [TSP_PC1_START_MASOMEH.md](TSP_PC1_START_MASOMEH.md) — **do not** mark O.15 **#2/#3/#8 PASS** from TesPro Status column alone.
- When bench uses **EMT432 yaml**, log MMS totals for Compare; meter proof is **PC-A** / Yahia.
