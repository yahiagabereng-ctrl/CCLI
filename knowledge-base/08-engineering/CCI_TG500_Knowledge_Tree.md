# CCI Knowledge Tree — TG-500 Programme (MT798X / OpenWrt)

**Document ID:** CCLI-KT-TG500-001
**Revision:** 1.2
**Date:** 2026-09-15
**RAG source_id:** `ccli-tg500-knowledge-tree`
**Parents:** `ccli-soc-freeze` (silicon) · `ccli-tg500-lab-platform` (DUT) · `ccli-architecture-framework` (method)
**Status:** LIVING — regenerate status column whenever `Knowledge_Matrix.xlsx` is rebuilt

> Scope: everything the team must **know, own, or acquire** to deliver a PF2 / CEI 0-16 CCI on the frozen
> **OpenWrt / TesproOS is FROZEN** as the product OS. **TG544 / TR500** is the **only** on-target DUT (`platform_tg500`). **Raspberry Pi / platform_pi removed from scope** (2026-09-17). K2.2: upstream **25.12.5 mediatek/filogic** SDK in `lab/tg544-openwrt/`.
> Open items: mechanical (C9), full C4 I/O expansion, vendor SDK parity, certification evidence — **not** alternate silicon.

**Status legend:** `[HAVE]` owned and in corpus · `[PART]` partial / unverified · `[MISS]` not in corpus, must acquire · `[DEF]` deliberately deferred · `[BLOCK]` blocked on external party

---

## 1. Tree at a glance

```
CCI TG-500 Knowledge Tree
├── K0  Corpus & RAG governance ................. 5 HAVE
├── K1  Platform & silicon ...................... 3 HAVE · 4 PART · 0 MISS · 1 DEF
├── K2  OS, build & boot toolchain .............. 1 HAVE · 2 PART · 4 MISS   <-- weakest branch
├── K3  Network & segmentation .................. 1 HAVE · 4 PART · 2 MISS
├── K4  Protocols & application firmware ........ 1 HAVE · 6 PART · 3 MISS
├── K5  Field I/O & serial ...................... 0 HAVE · 3 PART · 2 MISS · 1 DEF
├── K6  Security & cyber compliance ............. 1 HAVE · 3 PART · 3 MISS
├── K7  Regulation & certification .............. 3 HAVE · 3 PART · 1 MISS
└── K8  Verification & lab operations ........... 3 HAVE · 3 PART · 0 MISS
```

**Programme maturity:** 18 HAVE · 28 PART · 15 MISS · 2 DEF · 0 BLOCK (63 leaf nodes).
The critical path runs **K2 → K3 → K4**: without the OpenWrt SDK (K2.2) nothing in K4 can be proven on target.

**Rendered tree + open-item report:** `Architecture/TG500_Knowledge_Tree.txt` · `Architecture/TG500_Remaining_Work.md` — regenerate with `python scripts/build-tg500-knowledge-tree.py`.

---

## 2. Branch detail

### K0 — Corpus & RAG governance

| Node | Knowledge item | Status | RAG source_id | Risk | Gate |
|------|----------------|--------|---------------|------|------|
| K0.1 | Single-silicon rule (no alternate SoC in active corpus) | `[HAVE]` | `ccli-soc-freeze` | KR-015 | D2 |
| K0.2 | OpenWrt OS freeze; Yocto removed; Pi test path | `[HAVE]` | `ccli-openwrt-freeze` | — | D6 |
| K0.3 | Knowledge/risk matrices regenerate from one corpus module | `[HAVE]` | `scripts/knowledge_risks_corpus.py` | — | — |
| K0.4 | SK0146 schematic corpus (15 sheets) ingested | `[HAVE]` | `sk0146-blk_ref_*` | KL-001 | — |
| K0.5 | RAG service reachable for ingest + probes | `[HAVE]` | — | — | — |

**Action:** bring RAG up (`docker compose up -d`), then run `ingest-ccli.ps1 -ProjectOnly`, `ingest-tg500-platform.ps1`, `ingest-sk0146-schematics.ps1`.

---

### K1 — Platform & silicon

| Node | Knowledge item | Status | RAG source_id | Risk | Gate |
|------|----------------|--------|---------------|------|------|
| K1.1 | MT798X freeze — Cortex-A53 dual 1.6 GHz, HW NAT | `[HAVE]` | `ccli-soc-freeze` | KR-015 | D2 |
| K1.2 | TG-524 hardware spec (RAM/eMMC/ports/temp) | `[HAVE]` | `ccli-tespro-tg500-ds` | — | D5 |
| K1.3 | 12–36 V DC input class (C10) | `[HAVE]` | `ccli-tg500-lab-platform` | — | — |
| K1.4 | Thermal −40…+75 °C vs cabinet budget | `[PART]` | `ccli-tespro-tg500-ds` | KR-013 | — |
| K1.5 | Vendor response form vs datasheet deltas | `[PART]` | `ccli-tespro-tg424-response`, `ccli-tespro-supplier-correspondence` | R-TG-004 | D5 |
| K1.6 | **SKU confirmation TG-424 Pro vs TG-524** (written) | `[PART]` | `ccli-tespro-supplier-correspondence` | R-TG-004 | D3 |
| K1.7 | MT798X reference manual / GPIO + pinmux detail | `[PART]` | `ccli-mt7986a-datasheet`, `ccli-mt7986a-datasheet-extract` | R-SOC-001, R-SOC-006 | — |
| K1.8 | Field cabinet / DIN mounting (C9) | `[DEF]` | — | — | — |
| K1.9 | **TR-400 OEM user manual** (port map, DI/DO, RS485 pinout) | `[HAVE]` | `ccli-tr400-user-manual`, `ccli-tr400-user-manual-extract` | R-TG-004 | **D5** — 49 pp.; confirm SKU vs TG-424 Pro on label |

**Note:** K1.7 full TRM is NDA-gated at MediaTek. **MT7986A datasheet v1.15** (Filogic pin mux) is **PART** in corpus; **TG-524 board DTS** from TesPro SDK (K2.2) remains the product pin authority.

---

### K2 — OS, build & boot toolchain — **critical path**

| Node | Knowledge item | Status | RAG source_id | Risk | Gate |
|------|----------------|--------|---------------|------|------|
| K2.1 | Host / on-target test build (`platform_tg500`) | `[PART]` | `lab/tg544-openwrt/` | — | — |
| K2.2 | **OpenWrt SDK / cross-toolchain + sysroot (MT798X)** | `[PART]` | `lab/tg544-openwrt/` | R-SOC-001, R-TG-003 | **D6 P0** |
| K2.3 | OpenWrt build system fundamentals (feeds, menuconfig) | `[PART]` | `ccli-tespro-supplier-correspondence`, `ccli-openwrt-freeze` | KR-004 | D6 |
| K2.4 | `.ipk` packaging + procd init for CCI services | `[MISS]` | — | — | D6 |
| K2.5 | UCI config model + overlay persistence policy | `[MISS]` | — | — | — |
| K2.6 | Boot chain: bootloader → kernel → OpenWrt rootfs | `[PART]` | `ccli-tg500-lab-platform` | KR-004 | — |
| K2.7 | Signed / A-B firmware update path on OpenWrt | `[MISS]` | — | KR-005 | — |

**Validation path:** K4 runs **Pi L1** then **TR400 L2+** on bench. Host cross-compile waits on **K2.2 SDK**; on-device bootstrap is interim path.

---

### K3 — Network & segmentation

| Node | Knowledge item | Status | RAG source_id | Risk | Gate |
|------|----------------|--------|---------------|------|------|
| K3.1 | Port map: WAN/LAN1-4 → Eth_A / Eth_B / plant | `[PART]` | `ccli-tr400-user-manual-extract`, `ccli-tg500-lab-platform` | KR-007 | **D4 P0** — `ip link` on TR400 |
| K3.2 | Port-to-port galvanic isolation proof | `[MISS]` | — | KR-007 | — |
| K3.3 | No-L2-bridge firewall policy (REQ-NET-LAB-001) | `[PART]` | `ccli-validation-strategy` | KR-011 | D4 |
| K3.4 | LTE/WAN backup + VPN posture (no bridge to Eth_A) | `[PART]` | `ccli-tg500-lab-platform` | R-TG-005 | — |
| K3.5 | GNSS time source via modem | `[PART]` | `ccli-tg500-lab-platform` | KR-009 | — |
| K3.6 | IEEE 1588 / PTP need + capability | `[MISS]` | — | KR-008 | — |
| K3.7 | Lab profile disables Wi-Fi / OPC / DLMS extras | `[HAVE]` | `ccli-tg500-lab-platform` | R-TG-005 | — |

**Note:** K3.1 closes for free on hardware receipt — label the physical ports before any protocol bring-up, or every later test result is ambiguous.

---

### K4 — Protocols & application firmware

| Node | Knowledge item | Status | RAG source_id | Risk | Gate |
|------|----------------|--------|---------------|------|------|
| K4.1 | IEC 61850 MMS server (libiec61850) | `[PART]` | `ccli-github-protocol-libs` | KR-001 | — |
| K4.2 | IEC 61850 GOOSE publish/subscribe + L2 timing | `[MISS]` | — | KR-002 | — |
| K4.3 | Allegato T ICD / data model for CCI | `[HAVE]` | `cei-tr-57-126` | KR-001 | `cei-tr-57-126-example.cid` |
| K4.4 | Modbus RTU/TCP master (libmodbus) | `[PART]` | `ccli-github-protocol-libs`, `ccli-tespro-supplier-correspondence` | — | — |
| K4.5 | Energy analyzer register map (real model) | `[MISS]` | `ccli-modbus-analyzer-map` | — | **D8 P1** |
| K4.6 | IEC 60870-5-104 on Eth_B (lib60870) | `[PART]` | `ccli-github-protocol-libs` | KR-010 | — |
| K4.7 | PF2 / observability curtailment FSM (new C/C++) | `[PART]` | `ccli-app-structure` | KR-002 | — |
| K4.8 | Commissioning AT shell syntax + coding rules | `[HAVE]` | `fw-sk0146-*` | — | — |
| K4.9 | libiec61850 commercial licence (MZ quote) | `[MISS]` | `ccli-project-roadmap` | — | — |
| K4.10 | 61557-12 fixed-block aggregation + EPMF measurement path | `[PART]` | `ccli-61557-12-extract` | R-MET-001 | — |

**Licence note:** the GPLv3 prototype path is fine for the lab, but K4.9 must close before any customer-shipped binary — treat it as a commercial gate, not an engineering one.

---

### K5 — Field I/O & serial

| Node | Knowledge item | Status | RAG source_id | Risk | Gate |
|------|----------------|--------|---------------|------|------|
| K5.0 | **DIO silkscreen → ubus dido_v2 map (TG544 bench)** | `[HAVE]` | `ccli-tr500-io-peripheral` | R-IO-001 | **D5** — GD32 not SoC GPIO |
| K5.1 | 2× RS485 electrical behaviour + isolation (C2) | `[PART]` | `ccli-tr500-io-peripheral`, `ccli-tr400-user-manual-extract` | — | — |
| K5.2 | RS485 termination / TVS / connector craft | `[PART]` | `sk0146-blk_ref_a_rs485` | KL-002 | — |
| K5.3 | DI 10–120 Vdc conditioning for plant class (C4) | `[MISS]` | — | KL-003 | — |
| K5.4 | DO dry-contact / relay rating for curtailment (C4) | `[MISS]` | — | KL-004 | — |
| K5.5 | Wave B fixture to reach 5 DI / 3 DO on Pi | `[PART]` | `ccli-mocking-bench-bom` | R-TG-002 | — |
| K5.6 | 2× CAN interfaces | `[DEF]` | — | — | — |

**Reuse boundary:** SK0146 bench craft transfers for RS485 wiring (K5.2), but its 3.3 V contact-simulation limits must **not** be reused as CCI DI/DO pass criteria — that crosswalk is recorded in the `Bench_Crosswalk` sheet.

---

### K6 — Security & cyber compliance

| Node | Knowledge item | Status | RAG source_id | Risk | Gate |
|------|----------------|--------|---------------|------|------|
| K6.1 | Secure Boot chain verified on receipt | `[PART]` | `ccli-soc-freeze` | KR-005 | — |
| K6.2 | TPM 2.0 presence + `tpm2_tools` access | `[PASS]` | `P0_TPM_VERIFY.txt` 2026-09-25 · SLB9673 IFX | KR-005 | — |
| K6.3 | Key hierarchy + provisioning ceremony SOP | **HAVE** | `ccli-k63-ceremony-sop` | KR-006 | — |
| K6.4 | IEC 62443 zones/conduits + SL-T definition | **HAVE** | `ccli-62443-zones-extract` + `ccli-62443-3-3-extract` + `ccli-62443-4-2-extract` (Annex B) | KR-012 | — |
| K6.5 | IEC 62351 secure MMS/GOOSE profiles | **PART** | `ccli-62351-1-extract` + `ccli-62351-4-capture-plan` (+ 62351-4 body pending) | KR-001 | — |
| K6.6 | EU CRA 2024/2847 obligations | `[HAVE]` | `eu-2024-2847-cra` | — | — |
| K6.7 | Vulnerability handling / SBOM process | `[MISS]` | — | KR-012 | — |

**Path change from the archived SoM study:** security now rests on **TPM 2.0 + OpenWrt Secure Boot**, not NXP HAB/CAAM. Any inherited HAB assumption in older notes is obsolete.

---

### K7 — Regulation & certification

| Node | Knowledge item | Status | RAG source_id | Risk | Gate |
|------|----------------|--------|---------------|------|------|
| K7.1 | CEI 0-16 Allegato O (interface protection) | `[HAVE]` | `cei-0-16-allegato-o` | — | — |
| K7.2 | CEI 0-16 Allegato T (CCI communication) | `[HAVE]` | `cei-0-16-allegato-t` | — | — |
| K7.3 | Module regulation classification | `[HAVE]` | `ccli-cci-module-regs-class` | KR-012 | — |
| K7.4 | ARERA 540/2021 | `[HAVE]` | `arera-540-2021` | — | — |
| K7.5 | DSO demo / conformance route for CCI | `[PART]` | `ccli-project-roadmap`, `ccli-tespro-supplier-correspondence` | R-TG-001 | — |
| K7.6 | EMC / CE for a field product | `[PART]` | `ccli-tespro-supplier-correspondence` | KR-014 | — |
| K7.7 | CEI EN 61557-12 metrology (P/Q/V, Annex D fixed blocks) | `[PART]` | `ccli-61557-12-extract` | R-MET-005 | — |

**Standing correction:** a vendor datasheet listing "IEC 61850" is not a CEI qualification. Annex O/T compliance is **your software plus your evidence**, on capable hardware — captured as risk R-TG-001.

---

### K8 — Verification & lab operations

| Node | Knowledge item | Status | RAG source_id | Risk | Gate |
|------|----------------|--------|---------------|------|------|
| K8.1 | Validation & mocking strategy (waves A/B) | `[HAVE]` | `ccli-validation-strategy` | — | — |
| K8.2 | Mocking bench BOM | `[HAVE]` | `ccli-mocking-bench-bom` | — | — |
| K8.3 | SK0146 bench crosswalk (what transfers, what does not) | `[HAVE]` | `Knowledge_Matrix.xlsx` | KL-003/004 | — |
| K8.4 | Pi ↔ TR400 identical test vectors | `[PART]` | `ccli-app-structure`, `lab_tr400.yaml` | — | — |
| K8.5 | Evidence pack + records schema per REQ | `[PART]` | `Test Bench/records.py` | KL-017 | — |
| K8.6 | Verification matrix REQ → test → pass criteria | `[PART]` | `ccli-architecture-framework` | — | D0 |

---

## 3. Knowledge gaps (top of tree)

| Area | Current | Required | Gap |
|------|---------|----------|-----|
| OpenWrt SDK (K2.2) | Upstream 25.12.5 filogic runbook | TesPro vendor SDK parity | **PARTIAL** — host build unblocked |
| OpenWrt baseline (K2.3) | **25.12** on TG544 (kernel 6.12) | — | **CONFIRMED** bench |
| Lab port map (K3.1) | Suggested binding only | Labelled physical ports | **P0 — closes on receipt** |
| SKU identity (K1.6) | TG-424 Pro quoted (TPM 2.0) | Written TG-424 Pro ≡ TG-524 mapping | **P0 — before PO** |
| SK0146 corpus (K0.4) | Markdown on disk | Ingested + queryable | **DONE** |
| DI/DO ratings (K5.3–4) | Count only (2+2) | Voltage/current for plant class | P1 |
| Analyzer registers (K4.5) | None | Register map for chosen model | P1 |
| 61557-12 licensed PDF (K7.7) | Screenshot extract | Full CEI EN 61557-12:2018 PDF | P1 |
| MT798X board DTS (K1.7/K2.2) | MT7986A pin mux extract | TesPro TG-524 device tree + SoC SKU | P0 |
| GOOSE (K4.2) | None | Publisher + timing proof | P1 |
| 62443 / 62351 (K6.4–5) | 3-2 + 3-3 + 4-1 + 4-2 HAVE; 62351-1 HAVE | **62351-4** Batch A; 62351-3/5; SAR p.63 | P0 |
| ARERA 540/2021 (K7.4) | None | Regulation text | P2 |

---

## 4. Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-KT-001 | SDK never supplied by TesPro | Whole K2/K4 on-target path stalls | Pi mock keeps K4 moving; escalate SDK as PO condition; hold a second gateway vendor as fallback |
| R-KT-002 | Tree status drifts from `Knowledge_Matrix.xlsx` | Reviews quote stale readiness | Regenerate both in the same session; status column is derived, not hand-edited |
| R-KT-003 | Vendor protocol claims taken as certification | False confidence at DSO demo | R-TG-001 discipline: own stack + own evidence |
| R-KT-004 | TG-524 I/O count (2+2) treated as full C4 without expansion plan | PF2 demo incomplete in cabinet | Wave B fixture; document external I/O module if 5 DI / 3 DO required |
| R-KT-005 | Archived SoM study re-enters reasoning | Wrong silicon assumptions in answers | `_archive/som-study/` excluded from ingest; K0.1 rule |

---

## 5. Verification

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| K0 governance | RAG probe for any alternate-SoC source_id | Zero hits outside `_archive/` |
| K1.1 | `uname -m` + SoC identity on TG-524 | Cortex-A53 / MT798X reported |
| K2.2 | Cross-compile hello-world with TesPro SDK | Binary executes on device |
| K3.1 | Ping each labelled port from known host | Port map documented and reproducible |
| K3.3 | Traffic injection Eth_A → Eth_B | Blocked by firewall policy |
| K4.1 | libiec61850 server on Eth_A | External client reads demo model |
| K4.4 | Modbus RTU poll on RS485-1 | Analyzer / pymodbus responds |
| K6.2 | `tpm2_getcap properties-fixed` | TPM 2.0 chip enumerated |
| K8.4 | Same vector set on Pi and TG-524 | Identical pass/fail verdicts |

---

## 6. Maintenance

Regenerate companion artefacts after any status change:

```powershell
python scripts/build-sk0146-knowledge-matrix.py      # Knowledge_Matrix.xlsx + summary
python scripts/build-document-coverage-matrix.py     # corpus HAVE/MISSING
```

Ingest this tree: `Telematry System/scripts/ingest-tg500-platform.ps1`

---

## 7. Keywords

`knowledge tree`, `TG-500`, `TG-524`, `MT798X`, `OpenWrt`, `PF2`, `CEI 0-16`, `knowledge gap`, `readiness`, `ccli-tg500-knowledge-tree`
