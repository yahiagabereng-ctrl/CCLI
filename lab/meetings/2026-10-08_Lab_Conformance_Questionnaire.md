# Lab conformance meeting — HiTEKS CCLI questionnaire

**Document ID:** CCLI-LAB-QA-001  
**Revision:** 1.0  
**Date:** 2026-10-08  
**RAG source_id:** `ccli-lab-conformance-questionnaire`  
**Audience:** Accredited test lab (DNV Netherlands / UCA IUG level A lab, or 62351 cert body) · HiTEKS engineering  
**Companion deck:** `lab/meetings/2026-10-08_HiTEKS_CCLI_Lab_Conformance_Brief.pptx`  
**Basis:** `ccli-61850-10-extract` · `ccli-62351-100-3-extract` · `ccli-62351-3-extract` · `ccli-62351-4-extract` · `cei-0-16-allegato-o` · `cei-0-16-allegato-t` · `ccli-certificate-samples-extract`

**How to use:** ask in order; record the lab answer in the last column. "Our position" is what HiTEKS says today — change it only after the lab answers. Items marked **⚑** decide cost or schedule.

---

## 0. Our facts (say these first, so answers are scoped)

| Topic | HiTEKS fact |
|-------|-------------|
| Product | Central Plant Controller (CCI) per CEI 0-16 Annex O / T, original design |
| Platform | TesPro TG544 (MediaTek MT798X), OpenWrt 25.12 frozen |
| DSO interface | IEC 61850 **MMS server** on Eth_A (secure port **3782**, plain **102** on bench only) |
| Claimed 61850 | Ed. 2 parts 6, 7-1, 7-2, 7-3, 7-4, 8-1 — **server** role; GOOSE publish exists but claim TBD; **no SV**, no client role |
| Security | TLS per IEC 62351-3 (TLS 1.2, mutual auth); IEC 62351-4 E2E (Cert B) planned; PKI per 62351-9 / Annex T |
| Data model | ICD/CID exists; TotW URCB integrity ~4 s; Wlim / WSd controls |
| PKI | Lab CA on EJBCA; Cert A (TLS) + Cert B (MMS) per Annex T G.2 |
| Internal evidence | P3 lab: browse, URCB 4 s, Wlim, time sync — not UCA formal cases yet |

---

## 1. Scope and scheme ⚑

| # | Question | Why we ask | Our position | Lab answer |
|---|----------|------------|--------------|------------|
| 1.1 | Which **61850-10 edition** does your current server procedure follow — **Ed. 2.1 (2025-07)** or Ed. 2 (2012)? Which **UCA server test procedure version** (v2.0.6 or later)? | Table numbering and cases changed; competitor cert used Ed. 2 + v2.0.6 | Prepare for Ed. 2.1 | |
| 1.2 | Which **UCA conformance blocks** are mandatory for a **server-only** DUT, and which are optional claims? | Decides PICS content and test days | Minimum: Basic Exchange, Data Sets, Unbuffered Reporting, Control, Time Sync | |
| 1.3 | Can we **exclude** Buffered Reporting, Logging, File Transfer, Setting Groups, SV from scope without affecting certificate validity? | Reduce scope / cost | Exclude unless DSO requires | |
| 1.4 | Is **GOOSE publisher** testing required if GOOSE is not needed by the DSO interface? What if we claim it later? | Decide whether to claim GOOSE now | Not claimed in first pass | |
| 1.5 | Is the **61850 certificate** issued as UCA IUG certificate (level A1) or lab test report only? What does the DSO (e-distribuzione etc.) actually accept under CEI 0-16 Annex O? | Legal deliverable for Annex O §12 | Need UCA-recognized certificate | |
| 1.6 | Do you also run **IEC 62351-100-3** (transport) and **62351-100-4** (MMS security)? If not, which accredited body do you recommend? | Annex O requires 62351-100-3 by accredited body | Prefer one lab for both | |
| 1.7 | Can the **61850-10** session and the **62351** sessions be run in **one visit** on the same DUT firmware? | Travel / freeze firmware once | Yes, if possible | |
| 1.8 | Is a **CEI 0-16 Annex O/T functional** test (TÜV-style) in your portfolio, or only protocol conformance? | Avoid two labs | Ask for referral | |

---

## 2. Documents to submit ⚑

| # | Question | Why we ask | Our position | Lab answer |
|---|----------|------------|--------------|------------|
| 2.1 | Which **PICS template** do you want — 61850-7-2 Annex A proforma, UCA template, or your own form? | Avoid rework | Will use lab template | |
| 2.2 | Which **PIXIT template** (UCA PIXIT for server) — and which items are mandatory for us (association timeout, max clients, IntgPd range, segmentation, ctlModel)? | PIXIT not standardized | Draft from lab yaml | |
| 2.3 | Is **MICS** acceptable as the ICD alone, or do you need a separate MICS document? | Reduce documents | Prefer ICD + short MICS | |
| 2.4 | Do you require **TICS** (TISSUES conformance) and against which TISSUES database date? | Must list resolved TISSUES | Will submit minimal TICS | |
| 2.5 | Do you generate the **SCD** from our **ICD**, or must we deliver **SCD + SSD**? Which **ICT/SCT tools** do you use? | §5.4.2 of 61850-10 | Lab generates SCD | |
| 2.6 | For **62351-100-3**: your **PID (PICS + PIXIT)** template for TLS — do you accept the 62351-3 §8 PICS tables as is? | PID is mandatory | Will fill 62351-3 tables | |
| 2.7 | Must the **user manual** be final, or is a lab configuration guide enough at test time? | Docs still evolving | Lab configuration guide | |
| 2.8 | How do you verify **DUT version** (firmware hash, build ID, label)? What must appear on the certificate? | Table 1 sDoc cases | Provide SHA-256 of `ccli` + OpenWrt build | |
| 2.9 | How many **DUT units** do you need (1 or 2)? Spare? | Shipping | 1 + 1 spare | |

---

## 3. Test bench and configuration

| # | Question | Why we ask | Our position | Lab answer |
|---|----------|------------|--------------|------------|
| 3.1 | Which **client simulator / test tool** do you use for server tests? Can we get a trial or the same tool for pre-testing? | Pre-test at HiTEKS | Want pre-test tool | |
| 3.2 | Network: single Ethernet to Eth_A only, or do you need **all 4 Ethernet domains** reachable? | Isolation design | Eth_A only for conformance | |
| 3.3 | **Time master**: SNTP/NTP, PTP, or GNSS? Which accuracy do you verify for Time Sync block? | Our time source is chrony + GNSS | NTP/SNTP from lab | |
| 3.4 | Do you need **physical I/O stimulus** (DI/DO, analogue) to trigger data changes, or can changes be simulated by internal setpoints / Modbus mock? | 61850-10 bench includes signal generator | Software stimulus via lab mode | |
| 3.5 | Is a **lab-demo / test mode** in firmware acceptable, or must the DUT be tested in production configuration? | We have `--lab-demo` | Prefer production config + simulator | |
| 3.6 | Which **TCP port** do you test MMS on — 102 (plain) for 61850-10 and 3782 (TLS) for 62351? Both in one session? | Port plan | 102 for 61850-10; 3782 for 62351 | |
| 3.7 | **Max associations** / clients: how many simultaneous clients do you open during tests? | Server limits | Declare in PIXIT | |
| 3.8 | Can we **remote-connect** to the DUT during testing to inspect logs, or is the lab closed-box? | Debug | Request read-only remote | |
| 3.9 | **Reset / power cycle** rules: can the lab power-cycle the DUT between test groups? | Boot time, Secure Boot | Yes, ~60 s boot | |

---

## 4. 61850 data model, reporting, control

| # | Question | Why we ask | Our position | Lab answer |
|---|----------|------------|--------------|------------|
| 4.1 | Which **SCL schema** revision must the ICD validate against (2007B rel 4 / rel 5)? Any UCA-specific validator? | Table 2 sCnf cases | 2007B | |
| 4.2 | Are **private extensions / namespaces** in ICD a problem (TesPro vendor LNs, HiTEKS LNs)? | 7-1 §14 rules | Minimize extensions | |
| 4.3 | For **Unbuffered Reporting (Table 14)**: do you test all optional fields and all trigger options, or only those enabled in PIXIT? | Scope of URCB | Claim IntgPd + dchg + GI | |
| 4.4 | Do you require **Buffered Reporting** for DSO-facing CCI? (CEI Annex T uses URCB.) | Scope | Not claimed | |
| 4.5 | **Control model**: which ctlModel values will you test for Wlim / WSd — direct-with-normal-security, SBO, enhanced? | Table 26 cases | Direct normal security (confirm) | |
| 4.6 | **Segmentation**: minimum dataset size you use to force segmented reports? | Buffer limits | Declare max dataset in PIXIT | |
| 4.7 | **Quality / timestamp** rules on MX values — do you check `q` and `t` on every change? | Model correctness | Yes, implemented | |
| 4.8 | **Negative tests**: wrong object names, invalid writes — how many are mandatory? | Error path | Implemented, to verify | |
| 4.9 | Do you test **LPHD / LLN0 mandatory DOs** (Mod, Beh, Health, NamPlt) strictly? | Model minimum | Present | |
| 4.10 | **Dataset changes online** (ConfRev) — do you require dynamic datasets or only static? | Services caps | Static only | |

---

## 5. Time synchronisation

| # | Question | Why we ask | Our position | Lab answer |
|---|----------|------------|--------------|------------|
| 5.1 | Which **time sync** protocol is mandatory for the UCA server block — SNTP per 8-1? Is **PTP** (61850-9-3) optional? | Our product: GNSS + chrony NTP | SNTP client supported | |
| 5.2 | Which **accuracy class** do you verify (T1/T2…)? Annex T asks UTC ±100 ms. | Pass criteria | T1 at least | |
| 5.3 | How do you check **leap second / ClockFailure / ClockNotSynchronized** bits? | TimeQuality | Implemented via chrony state | |

---

## 6. Security — IEC 62351-3 / 100-3 (transport) ⚑

| # | Question | Why we ask | Our position | Lab answer |
|---|----------|------------|--------------|------------|
| 6.1 | Do you execute **IEC TS 62351-100-3:2020 Tables 1–5 verbatim**, or a national scheme (e.g. IMQ IND-F-006_CYB.62351)? | Report format / acceptance | Prefer IEC tables | |
| 6.2 | Which **62351-3 edition** is the reference: 2014+AMD1+AMD2 or **62351-3:2023 Ed. 2**? | TLS 1.3 handling differs | 2014+AMD2 (TLS 1.2) | |
| 6.3 | Which **cipher suites** must be supported / must be rejected? Is **TLS 1.3** required or optional? | Cipher policy on OpenWrt | TLS 1.2 AES-GCM SHA256, ECDHE | |
| 6.4 | **Renegotiation / session resumption** intervals you test — do you simulate 24 h with shortened PIXIT values? | Long-lived DSO link | Shortened via PIXIT | |
| 6.5 | **Certificate revocation**: do you test **CRL**, **OCSP**, or both? Do you require OCSP nonce? | Annex T requires both | Both planned | |
| 6.6 | **CRL unreachable** mid-session must not drop the session — how is it tested? | 62351-3 §5.6.4.4 | Implemented policy | |
| 6.7 | Do you need our **lab CA** (EJBCA) or do you issue test certificates from the lab PKI? What key sizes / curves? | PKI inputs | Can use either; RSA 2048 / P-256 | |
| 6.8 | Are **client-station** TLS tests waived for a **server-only** DUT? | Scope | Request waiver | |
| 6.9 | Which **security events** must be visible (syslog, 62351-14, 62351-7 objects)? Is syslog export enough? | §4.3.4 logging | Syslog RFC 5424 | |
| 6.10 | Is the **application** for 100-3 tests required to be MMS (61850-8-1) or can it be a generic TCP app? | §4.3.1 | MMS on 3782 | |

---

## 7. Security — IEC 62351-4 / 100-4 (MMS E2E) and PKI

| # | Question | Why we ask | Our position | Lab answer |
|---|----------|------------|--------------|------------|
| 7.1 | Is **62351-4** testing required by the DSO today, or is TLS (62351-3) sufficient for Annex T acceptance? | Scope / cost | Ask DSO; prepare E2E | |
| 7.2 | Which **62351-4 mode** do you test: compatibility (A-security, ACSE auth) or **native E2E**? | Firmware choice | Native E2E with Cert B | |
| 7.3 | Is **62351-100-4** a published test spec you execute, or still draft? | Availability | Unknown | |
| 7.4 | **Certificate profiles**: must certs follow 62351-9 / Annex T profiles (SAN, EKU, key usage)? Do you check them? | PKI profile | Follow Annex T | |
| 7.5 | **Enrolment (EST / SCEP)**: do you test enrolment, or only pre-installed certs? | Annex T requires EST or SCEP | EST planned | |
| 7.6 | **RBAC (62351-8)** — any test today? | Future | Not now | |
| 7.7 | **Trust anchors**: DSO CA, admin domain CA, manufacturer CA — do you simulate all three? | Annex T minimum | Provide all three in lab | |

---

## 8. Other Annex O certificates (referrals)

| # | Question | Why we ask | Our position | Lab answer |
|---|----------|------------|--------------|------------|
| 8.1 | **CEI EN 61557-12** + IEC 61010 (environmental, EMC, insulation): do you have an ISO 17025 lab or partner (ACCREDIA recognised)? | Annex O §12 | Need referral | |
| 8.2 | **IEC 62443-4-1 / 4-2** (ISASecure SDLA / CSA): partner lab? Do you accept SL-C 1 for embedded device? | Competitor had SL-C 1 | Target SL-T 2 | |
| 8.3 | **FIPS 140-2 / 140-3** crypto module: is a TPM vendor certificate sufficient, or must the product be evaluated? | O.15 | TPM vendor cert | |
| 8.4 | **CE / Dichiarazione di conformità** — any lab deliverable needed beyond our DoC? | Legal | Manufacturer DoC | |

---

## 9. Process, cost, schedule ⚑

| # | Question | Why we ask | Our position | Lab answer |
|---|----------|------------|--------------|------------|
| 9.1 | **Lead time** from PICS/PIXIT submission to test slot? Typical test duration (days) for server + transport? | Planning | — | |
| 9.2 | **Pricing model**: per block, per day, fixed? Re-test fee after firmware fix? | Budget | — | |
| 9.3 | **Pre-test / consultancy**: do you offer a pre-conformance run or remote pre-check? | Reduce failure risk | Yes, want | |
| 9.4 | **Failure handling**: if one block fails, can we fix and re-run only that block in the same visit? | Risk | — | |
| 9.5 | **Firmware change after certificate**: which changes require re-test (Table 1 sDoc / TICS rule)? | Maintenance | — | |
| 9.6 | **Certificate validity** and renewal; does the certificate reference a single specimen or product type? | Competitor cert: single specimen | Product type preferred | |
| 9.7 | **Report language and format** acceptable to Italian DSO / CEI? | Legal pack | English + Italian cover | |
| 9.8 | **NDA / confidentiality** — can we share firmware internals, ICD, PKI? | IP | NDA before shipping | |
| 9.9 | Can the lab test **remotely** (DUT at HiTEKS with VPN) or on-site only? | Logistics | Prefer on-site first | |

---

## 10. Lab's questions to us (be ready)

| Lab will ask | Our answer |
|--------------|------------|
| Server role only? Any client? | Server only |
| Edition 2 or 2.1 model? | Ed. 2 model, Ed. 2.1 test procedure |
| Which ACSI services? | Association, GetServerDirectory, GetLogicalDevice/NodeDirectory, GetDataValues, SetDataValues, GetDataSetValues, URCB (GetURCBValues/SetURCBValues/Report), Control (Operate; SelectWithValue if SBO), TimeSync client |
| Max clients / associations | PIXIT value (e.g. 4) |
| IntgPd range | 1000–60000 ms (TotW 4000 ms default) |
| Dataset limits | Static datasets; max members per dataset in PIXIT |
| Control model | Direct normal security (confirm per DO) |
| TLS versions / ciphers | TLS 1.2; ECDHE-RSA-AES128/256-GCM-SHA256/384 |
| Certificate sizes | RSA 2048, ≤ 8192 octets |
| Ports | 102 plain (bench), 3782 TLS |
| Logging | Syslog RFC 5424; event store ≥ 2048 |
| Firmware ID | SHA-256 of `ccli` + OpenWrt build ID |

---

## 11. Decisions to record after the meeting

| Decision | Options | Chosen | Owner |
|----------|---------|--------|-------|
| 61850 scope blocks | minimum / extended | | |
| GOOSE claim | now / later | | |
| 62351-3 only vs 62351-3 + 62351-4 | | | |
| One lab vs two labs | | | |
| PICS/PIXIT templates | lab / IEC | **Lab templates in `lab/`** — see [UCA document checklist](../conformance/2026-10-08_UCA_Document_Phase_Checklist.md) | |
| Target test date | | | |
| Budget approval | | | |

---

## 12. Follow-up actions (HiTEKS)

0. Track templates vs phases: [2026-10-08_UCA_Document_Phase_Checklist.md](../conformance/2026-10-08_UCA_Document_Phase_Checklist.md) (add missing **PIXIT** Word file to `lab/`).  
1. Fill PICS (61850-7-2 Annex A / UCA template) from `mms_adapter.cpp` capabilities.  
2. Fill PIXIT: ports, IntgPd range, max clients, dataset limits, ctlModel, timeouts.  
3. Fill 62351-3 PID (PICS §8 tables + TLS PIXIT).  
4. Validate ICD against the lab's SCL validator.  
5. Deploy ISO session fix + EJBCA PEMs on TG544; capture TSP evidence on 3782.  
6. Share this questionnaire with answers to the programme file (`lab/meetings/`).

---

**RAG tags:** `lab`, `DNV`, `UCA`, `61850-10`, `62351-100-3`, `62351-3`, `62351-4`, `PICS`, `PIXIT`, `questionnaire`, `Annex O`, `ccli-lab-conformance-questionnaire`
