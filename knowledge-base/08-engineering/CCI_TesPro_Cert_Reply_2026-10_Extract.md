# TesPro vendor cert reply — 2026-10-21 (WeChat drop)

**Document ID:** CCLI-VENDOR-TESPRO-CERT-2026-10  
**Revision:** 1.1  
**Date:** 2026-10-21  
**RAG source_id:** `ccli-tespro-cert-reply-2026-10`  
**Drop:** `C:\Users\yahia\OneDrive\Documents\xwechat_files\wxid_9o7d32f54k1922_1b1e\msg\file\2026-10`  
**Repo mirror:** `knowledge-base/08-engineering/reference/vendor/tespro/2026-10-cert-reply/`  
**Corpus:** **HAVE** (text + OCR) · PDFs gitignored  
**Parents:** `ccli-tespro-supplier-correspondence` · `ccli-61557-12-extract` · `ccli-normative-cert-validation-matrix`

---

## Summary

TesPro answered the HiTEKS vendor-ask workbook (`TESPRO_VENDOR_ASK_CERT(Reply).xlsx`, sheet **Hiteks**) on **2026-10-21**. The pack is **platform evidence**, not O.15 accredited certificates.

**Platform they named:** Tespro **TG500** · MediaTek **MT7987A** · **TesproOS 32-2.x** on OpenWrt **25.12** · kernel **6.12.y**. Lab DUT remains **TG544 / TG-524** (`platform_tg500`).

**Hard facts from the files (not the Status column):**

| TesPro claim | What the file actually is | O.15 / 61557-12 |
|--------------|---------------------------|-----------------|
| FIPS 140-2 L3 | Infineon **SLB9673** policy **FIPS 140-2 Level 2** + **CC EAL4** BSI-DSZ-CC-**1278-2026** | **#3 FAIL as L3**. L2 + CC ≠ FIPS L3 |
| CE + DoC TG544 | **CNAS L5868** + **A2LA 4674.01** for **Shenzhen Microtest** (ISO/IEC **17025** lab) | **#8 miss**. Lab accred ≠ product CE/EMC/LVD |
| ISO 9001 | Status says attached | **PDF not in this drop** |
| 61557-12 / 61010 | Scope statement: comms gateway → **62368-1 + 61326**; **61557 series does not apply**; 61010 only if measuring inputs exist | **#2 not TesPro’s object**. HiTEKS owns EPMF SS chain |
| Secure Boot / 61850 / IDevID / image | `TG500-20261021.zip` **v0.1 drafts**, unsigned | Useful engineering; not certs |

---

## Files ingested (one by one)

| # | File | Extract | source_id | Status |
|---|------|---------|-----------|--------|
| 1 | `TESPRO_VENDOR_ASK_CERT(Reply).xlsx` | `_extracts/TESPRO_VENDOR_ASK_CERT_Reply_.txt` | `ccli-tespro-ask-cert-reply-xlsx` | **HAVE** |
| 2 | `TESPRO_VENDOR_ASK_CERT.xlsx` | ask blank | — | prior ask |
| 3 | `Copy of TESPRO_VENDOR_ASK_CERT.xlsx` | copy | — | prior ask |
| 4 | `01 CNAS.pdf` | `_extracts/01_CNAS_ocr.txt` (scanned) | `ccli-tespro-cnas-l5868` | **HAVE** OCR |
| 4b | CNAS unfolding p.1/83 (paste) | `CCI_TesPro_CNAS_L5868_62368_Scope_Extract.md` | `ccli-tespro-cnas-l5868-62368-scope` | **HAVE** ICT **62368-1** only · 8273 rows not fully ingested |
| 5 | `03 A2LA.pdf` | `_extracts/03_A2LA.txt` | `ccli-tespro-a2la-4674` | **HAVE** cover only |
| 5b | `4674-01.pdf` (Desktop) | `_extracts/4674-01.txt` | `ccli-tespro-a2la-4674-scope` | **HAVE** full Electrical scope, rev **2026-09-22** |
| 6 | `CC certificate.pdf` | `_extracts/CC_certificate.txt` | `ccli-tespro-tpm-cc-1278` | **HAVE** CC EAL4 |
| 7 | `FIPS 140 2 Level 2 Non-Proprietary Security Policy.pdf` | `_extracts/FIPS_140_2_Level_2_…txt` | `ccli-tespro-fips-140-2-l2-policy` | **HAVE** L2 |
| 8 | `Hall switch datasheet.pdf` | TI **DRV5032** | `ccli-tespro-hall-drv5032` | **HAVE** |
| 9 | `TesPro-Certification-Scope-Allocation-Statement-EN.pdf` | 2 pp. v1.0 | `ccli-tespro-scope-allocation-2026-10` | **HAVE** |
| 10 | `TG500-20261021.zip` | 11 DOCX → `_extracts/*.txt` | `ccli-tespro-tg500-deliv-2026-10` | **HAVE** drafts |
| 11 | `IEC TS 62351-100-4 2023 - 道客巴巴.pdf` | already `iec62351/` | `ccli-62351-100-4-ocr-corpus` | **HAVE** (no re-OCR) |

---

## Requirements (from the reply)

| ID | TesPro Status | File verdict | Still needed |
|----|---------------|--------------|--------------|
| V-CERT-001 TPM FIPS L3 | SLB9673 FIPS **L2** + policy | Policy Release **1.6** 2026-03-27. **No CMVP number** in the PDF. CC is **BSI-DSZ-CC-1278-2026** (SLB9672 FW16 + SLB9673 FW26), EAL4+ AVA_VAN.4, valid to **2031-02-22** | CMVP **L3** cert **or** drop O.15 #3 |
| V-CERT-002 CE/DoC | CNAS / A2LA recommended | CNAS **L5868** card + unfolding **p.1** (ICT **62368-1** Effectivity) + A2LA **4674.01** (62368 + **61326-1** + **61000-6-x**). **No 61557 / 61010 / K55**. **No TG544 test report** | EU CE DoC + **TG544** reports |
| V-CERT-003 ISO 9001 | “attached” | **Not in drop** | TesPro ISO 9001 PDF |
| V-CERT-004 Secure Boot | “Provided today” | Guides **v0.1**. MT7987 eFuse; FIT_SIGNATURE; fuse burn **irreversible**, needs module-vendor SOP | Signed R1 + fuse evidence on DUT |
| V-CERT-005 Tamper | Hall + magnet, GPIO LOW = open | TI **DRV5032**. **No GPIO/ubus map** | Pin / UCI / `ccli` event line |
| V-CERT-006 61557-12 | Scope allocation statement | TesPro certifies **platform safety/EMC** only. Comms-only → **62368-1 + 61326**. **61557 does not apply**. 61010 only if V/I/CT inputs | HiTEKS 17025 #2 on **EPMF SS** (analyzer+CT/VT+PMF) |
| V-CERT-007 Image ID | “Provided today” | Example `tesproos-32-2.1.0-r530+1-0eeca58f53-…sysupgrade.itb`. Release register **empty** | Frozen **25.12** tag + sha256 of lab image |
| V-CERT-008 61850 coexist | “Provided today” | Policy **v0.1**: `ccli` owns **:3782**; `iec61850service` yields if 3782 LISTEN. Factory **does not** pre-install three 61850 ipks | Confirm DUT image has policy **and** ipks absent |
| V-CERT-009 IDevID | “Provided today” | **Not factory-provisioned**. EK only. Options A/B/C unsigned | Choose A/B/C in writing |

---

## Architecture

### Functional

- TesPro object = **gateway platform** (structure, insulation, EMC, climate of the **baseline** SKU).
- HiTEKS object = **finished CCI** (brand, enclosure changes, ports, PSU, **61557-12 measurement chain**, UCA, 62351-100-3).
- TesPro 61850 stack = optional **MMS client** (`iec61850-mmsd` + `iec61850d`), southbound **:102**, MQTT 1883 / OPC-UA 4840. **Not** DSO server on :3782.

### Hardware

| Block | TesPro 2026-10-21 | CCLI freeze |
|-------|-------------------|-------------|
| SoC | MT7987A Filogic | MT798X / TG544 |
| TPM | SLB9673 I2C **0x2e** | Matches lab `P0_TPM_VERIFY.txt` |
| Tamper | TI DRV5032 Hall | GPIO map **MISS** |
| Boot | SPI-NOR; vendor BL 0x0–0xE0000 | Secure Boot PART |

### Security

- FIPS module claim is **L2**. O.15 #3 asks **L3**.
- Common Criteria on the **chip**, not the gateway.
- IDevID not in factory TPM.
- Coexistence: `ccli` wins :3782 (R1–R7). Draft, not frozen in a tagged image.

### 61557-12 mapping (scope statement §3)

| Product function (TesPro text) | Standards they name | CCLI mapping |
|--------------------------------|---------------------|--------------|
| Comms / data-forwarding gateway only (SELV, Eth/LTE/DIO, **no measuring inputs**) | IEC/EN **62368-1**; EN **61326-1** Class A or EN **61000-6-x** | **TG544 as shipped** = this row. **61557 and 61010 do not apply to TesPro** |
| Voltage / current / temperature / CT inputs, or DO driving loads | **61010-1** (+ 61010-2-030 if live measure) | Only if HiTEKS adds measuring I/O on the box |
| LV distribution test set (insulation, loop, earth) | **61557-1** + part | **Not CCI** |
| Stand-alone settlement meter | **62053-21 / 62053-12** MID | Chronos EMT432 path — **not TesPro** |
| “CCI side” charging comms | IEC **61851** | **Wrong domain** — ignore for CEI 0-16 CCI |

TesPro will only map **61557-12** if HiTEKS confirms the official title/year against **their CB certificate**. Until then they treat 61557-12 as **not on the platform cert**.

**HAVE:** written EPMF vs PMD split.  
**MISSING:** K55 report ID, 62368/61326 test reports, TG544 CB cert, 17025 report on assembled CCI.

---

## Interface matrix

| Interface | Source | Destination | Protocol |
|-----------|--------|-------------|----------|
| Vendor ask | HiTEKS | TesPro | `TESPRO_VENDOR_ASK_CERT(Reply).xlsx` |
| Scope split | TesPro | HiTEKS DoC | Scope Allocation v1.0 2026-10-21 |
| MMS DSO | `ccli` | TSP / DSO | TCP **3782** TLS |
| TesPro 61850 | `iec61850d` | Plant IEDs | outbound **102** (if installed) |
| TPM | SLB9673 | OpenWrt | I2C 0x2e |
| Tamper | DRV5032 | SoC GPIO | digital; **pin TBD** |

---

## BOM matrix

| Block | Function | Candidate / PN | Status |
|-------|----------|----------------|--------|
| TPM | Crypto module | Infineon SLB9673 FW 26.25 | FIPS **L2** + CC **1278-2026** |
| Hall | Cover tamper | TI **DRV5032** | Datasheet HAVE; GPIO MISS |
| Lab (China) | TesPro type-test house | Shenzhen Microtest CNAS L5868 / A2LA 4674.01 | **Not** ACCREDIA / not product cert |
| OS | Frozen image | TesproOS 32-2.x / OpenWrt 25.12 | Example name only; R1 empty |

---

## Knowledge gaps

| Area | Current | Required | Gap |
|------|---------|----------|-----|
| FIPS L3 | L2 policy, no CMVP # | O.15 #3 L3 or no-claim | **OPEN** |
| CE/EMC/LVD | China 17025 lab cards | EU DoC + reports | **OPEN** |
| ISO 9001 | Claimed | PDF | **MISSING in drop** |
| 61557-12 | TesPro out of scope | HiTEKS 17025 on EPMF SS | **OPEN** (U-ACC-04) |
| GPIO tamper | Hall theory | Pin / ubus / `o14` | **MISS** |
| Image lock | Example r530 | Lab DUT sha256 vs 25.12 freeze | **PART** |
| Coexist / IDevID / SB | v0.1 unsigned | Signed + on DUT | **PART** |
| 61851 row | In TesPro table | Not CEI 0-16 CCI | Ignore |

---

## Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-TES-001 | File TesPro pack as O.15 #2/#3/#8 | DoC reject | Keep Full Validate **ACC/OPEN** |
| R-TES-002 | Call L2 + CC “FIPS L3” | #3 fail | Claim L2 **or** new module |
| R-TES-003 | Use CNAS/A2LA as CE | #8 fail in Italy | EU NB + ACCREDIA for #2 |
| R-TES-004 | TesPro 61850 starts on a SKU | Port fight | Factory: ipks **absent**; policy R1–R7 |
| R-TES-005 | IDevID assumed present | 62351-9 enroll fail | Pick option A/B/C |

---

## Verification

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| V-CERT-006 | Read scope PDF | “61557 does not apply” to comms gateway; HiTEKS signs confirmation line |
| V-CERT-001 | Policy title + CC number | L2 + BSI-DSZ-CC-1278-2026 only; no L3 |
| V-CERT-002 | CE folder | TG544 DoC + EMC/LVD — **fail today** |
| V-CERT-008 | DUT `opkg list-installed` + `:3782` | No `iec61850-*` **or** yield when `ccli` listens |
| V-CERT-009 | `tpm2_getcap` + EK | No IDevID; EK present |
| V-CERT-007 | `/etc/openwrt_release` | Matches frozen tag, not only example r530 |

**Ingest:** `powershell -File scripts/ingest-tespro-cert-reply-2026-10.ps1`

---

## Related RAG

| source_id | Role |
|-----------|------|
| `ccli-tespro-cert-reply-2026-10` | This extract |
| `ccli-tespro-scope-allocation-2026-10` | 61557/61010 split |
| `ccli-61557-12-tespro-vendor-map` | TesPro asks vs 61557-12 only |
| `ccli-61557-12-clauses-vs-tespro` | Every 2018 Ed 2.0 clause vs Scope §3 |
| `ccli-tespro-cnas-l5868-62368-scope` | CNAS unfolding p.1 ICT 62368-1 |
| `ccli-61557-12-extract` | REQ-MET-001…014 |
| `ccli-normative-cert-validation-matrix` | Full Validate O.15 #2/#3/#8 |
| `ccli-62351-100-4-extract` | Same drop PDF, already ingested |

---

## Revision history

| Rev | Date | Change |
|-----|------|--------|
| 1.0 | 2026-10-21 | Ingest WeChat `2026-10` drop; one-by-one extracts |
| 1.1 | 2026-10-21 | CNAS unfolding p.1 + 4674.01 scope; pointer to vendor map |
