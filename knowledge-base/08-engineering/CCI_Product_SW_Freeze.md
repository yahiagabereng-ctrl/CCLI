# CCI Product Software Freeze — Lab-first, cyber profile

**Document ID:** CCLI-PROD-SW-001  
**Revision:** 1.0  
**Date:** 2026-10-14  
**RAG source_id:** `ccli-product-sw-freeze`  
**Status:** **ACTIVE** — lab-first; product bundles promoted from closed lab gates  
**Parents:** `ccli-openwrt-freeze` · `ccli-soc-freeze` · `ccli-tg500-lab-platform` · `ccli-app-structure` · `ccli-62443-zones-extract`  
**Cyber parents:** `ccli-62351-3-extract` · `ccli-62351-4-extract` · `ccli-62351-100-3-extract` · `ccli-62351-100-4-extract` · `cei-0-16-allegato-t-extract`  
**Lab evidence:** `lab/evidence/README.md` · `lab/tg544-openwrt/deploy-manifests/` · `releases/product-cyber/README.md`  
**RAG init:** `lab/conformance/PRODUCT_DEPLOYMENT_INIT.md` (`ccli-product-deployment-init`) · `lab/conformance/RAG_SOURCE_INDEX.md` · `scripts/ingest-product-deploy-rag.ps1`

---

## Freeze decision

| Field | Value |
|-------|--------|
| **Source tree** | Single repo **`apps/ccli/`** — no fork for product |
| **Lab path** | Build → deploy → evidence on **TG544 / TR500** (`platform_tg500`) |
| **Product path** | **Promoted bundle** under `releases/product-cyber/<semver>-rN>/` after lab gates pass |
| **Cyber profile (product)** | MMS **TLS on :3782** (62351-3) + **A-profile ACSE** (62351-4 / Annex G.2 dual cert) — **no cleartext :102** |
| **62443 segmentation** | Zones **Z0–Z5**, conduits **C1–C5** — see `CCI_62443_Zones.md` |
| **MMS model** | Frozen **CID + `.cfg`** per release; traceability **`signal_map.yaml`** (31-LN plane; Figura 2 hierarchy) |
| **Config on DUT (today)** | `/etc/ccli/lab.yaml` — product yaml copied at deploy; **`product.yaml` name** deferred (K2.5 UCI) |
| **Version truth** | `apps/ccli/VERSION` → `ccli --version` → DUT `/etc/ccli/VERSION` |

**Rule:** Lab may use **cleartext / mock / bypass** yaml; product bundles **must not** ship those files as the active config.

---

## Lab-first validation ladder

Same silicon/OS as product (`ccli-openwrt-freeze`). Software maturity follows **lab phases** before promotion:

```
Host unit tests (core/, test_vectors/)
       │
       └── TG544 L1 — PF2 + Modbus + comm faults (L1-P-*, L1-C-*)
              │
              └── L2 — Physical RS485 + DIO + isolation (O.13.1)
                     │
                     └── L3 — DSO MMS TLS + 62351-4 (Eth_A :3782)
                            │
                            └── L4 — Operator 104 + multi-LAN (Eth_B)
                                   │
                                   └── L5 — TG-524 cabinet / field I/O (product DUT)
```

| Stage | Platform | Proves | Corpus |
|-------|----------|--------|--------|
| L1 | TG544 | PF2 FSM, adapters, yaml load | `ccli-validation-strategy` · `lab/PHASE1_TRACEABILITY.md` |
| L2 | TG544 + bench I/O | Serial, ubus DIO, curtailment | `ccli-tr500-io-peripheral` |
| L3 | TG544 + PKI | TLS + ACSE, P3 evidence | `lab/evidence/phase3/` · D7 PID |
| L4 | TG544 two-LAN | 104 + MMS coexistence | `lab/evidence/phase4/` |
| L5 | TG-524 / TR400 field | Thermal, EMC, plant config | `ccli-tg500-lab-platform` L4 row |

**Promotion gate:** a `releases/product-cyber/<semver>-rN>/` folder is created only when the **product cyber checklist** (§ Verification) passes on a reference DUT for that **rN**.

---

## Requirements

| ID | Requirement | Status |
|----|-------------|--------|
| REQ-PROD-SW-001 | One identifiable product release (semver-**rN** + manifest + git SHA) | **ACTIVE** |
| REQ-PROD-SW-002 | Product MMS **only** secured profile (**:3782**, `tls_enabled: true`) | **FROZEN** intent · **PART** field sign-off |
| REQ-PROD-SW-003 | Frozen ICD/CID + `.cfg` + `signal_map` rev in release bundle | **ACTIVE** |
| REQ-PROD-SW-004 | Plant yaml (Modbus, PF2, I/O) — no lab-only mock unless documented | **PART** |
| REQ-PROD-SW-005 | SHA256 of `.apk` and configs in manifest; DUT `/etc/ccli/VERSION` match | **HAVE** pattern · `deploy-manifests/` |
| REQ-PROD-SW-006 | **62443** zone rules — no WAN→Eth_A bridge; Eth_A/B separation | **PART** · `ccli-62443-zones-extract` |
| REQ-PROD-SW-007 | Lab cleartext configs **excluded** from product bundle | **FROZEN** |
| REQ-PROD-SW-008 | Accredited **62351-100-3** cert plan tracked separately (**P7-07**); TSP ≠ certificate | **OPEN** · `ccli-62351-100-3-extract` |

---

## Architecture

### Functional

| Layer | Lab | Product |
|-------|-----|---------|
| PF2 / Modbus / I/O | Mock + regulation yaml OK | Plant-specific yaml; real meters/relays |
| MMS server | Cleartext **:102** allowed for TSP only | **:3782** TLS + ACSE only |
| IEC 104 | Phase 4 yaml on LAN2 | Same stack; operator rule mode per site |
| Commissioning | `lab/mock_console`, bench CGI | Out of cert path; lock down on ship |

### Security (five conduits C1–C5)

Align with `CCI_62443_Zones.md`:

| Conduit | Zones | Product note |
|---------|-------|----------------|
| **C1** | Z1 ↔ Z0 | DSO MMS — **62351-3/4** on Eth_A |
| **C2** | Z2 ↔ Z0 | 104 / OA on Eth_B |
| **C3** | Z3 ↔ Z0 | Modbus RTU/TCP — no IP forwarding to Eth_A |
| **C4** | Z4 ↔ Z0 | USB maint — temporary, policy TBD |
| **C5** | Z5 ↔ Z0 | WAN/LTE — **no route to Eth_A** |

### Hardware / network

Product SKU **TG-524**; lab DUT **TG544/TR400** — same **MT798X / OpenWrt 25.12** line (`ccli-soc-freeze`).

### Power

Unchanged from platform freeze — 12–36 V class on TG-524; TR400 panel 12–30 V.

---

## Interface matrix

| Interface | Lab canonical yaml | Product template | Protocol / security |
|-----------|-------------------|------------------|---------------------|
| Eth_A DSO | `lab_tr400_phase4_eth_b.yaml` (TLS) | `releases/.../config/product_tr400.yaml` | MMS **62351-3/4** :3782 |
| Eth_A TSP bypass | `lab_tr400_cleartext_tsp.yaml` | **Excluded** | :102 cleartext — lab only |
| Eth_B OA | `lab_tr400_phase4_eth_b.yaml` | product yaml | IEC 60870-5-104 |
| Plant RS485 | phase yaml `modbus.device` | plant map under `config/plant/` | Modbus RTU |
| DIO | `lab_tr400*.yaml` | site wiring | ubus `dido_v2` |

---

## BOM matrix

| Block | Function | Part / path | Status |
|-------|----------|-------------|--------|
| APP-SRC | Firmware | `apps/ccli/` | **BUILD** |
| APP-REL | Product bundle | `releases/product-cyber/<semver>-rN>/` | **ACTIVE** (empty until promote) |
| OS | OpenWrt 25.12 | TesproOS on TesPro | **FROZEN** |
| SDK | Cross-compile | `lab/tg544-openwrt/` | **PART** |
| PKI | TLS + ACSE certs | `/etc/ccli/tls/` on DUT | **LAB HAVE** · product CA per site |
| EVIDENCE | Phase closeouts | `lab/evidence/` | **PART** P0–P4 closed |

---

## Knowledge gaps

| Area | Current | Required | Gap |
|------|---------|----------|-----|
| First promoted release | No folder under `releases/product-cyber/` yet | One golden **rN** + FREEZE.md | **OPEN** |
| Product yaml template | Phase4 yaml in repo | `config/product/` or release-only copy | **OPEN** |
| UCI / `product.yaml` | Single `lab.yaml` path | procd + persistent product config | **OPEN** — K2.5 |
| 62351-100-4 PDF | Planning extract only | Licensed PDF + OCR | **MISSING** |
| 62351-100-3 accreditation | Lab TSP evidence | Accredited body report | **OPEN** — P7-07 |
| TG-524 cabinet L5 | TG544 bench | Field DUT evidence | **OPEN** |

---

## Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-PROD-SW-001 | Lab `.apk` + cleartext yaml on field unit | Regulatory / DSO rejection | Product bundle checklist; ban `:102` in product FREEZE |
| R-PROD-SW-002 | SDK sysroot ≠ TesproOS image | Crash / TLS ABI issues | Record TesproOS build in manifest; TesPro SDK request |
| R-PROD-SW-003 | 31-LN CID vs runtime PART objects | Conformance fail | Pin `.cfg` + PICS/MICS with release |
| R-PROD-SW-004 | Secrets in git | Key compromise | Release `tls/README.md` layout only; keys on device/HSM |
| R-PROD-SW-005 | Promote before L3/L4 close | Weak cyber evidence | Lab-first gate in § Verification |

---

## Verification

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| REQ-PROD-SW-002 | Port scan Eth_A | **3782** listening with TLS; **no** **:102** with product config |
| REQ-PROD-SW-002 | MMS client connect | TLS + cert auth per closed P3 session |
| REQ-PROD-SW-003 | Model vs CID | LN set matches frozen `.cid` / MICS scope |
| REQ-PROD-SW-004 | `ccli --regulation-check` | 0 FAIL for claimed Allegato O functions |
| REQ-PROD-SW-005 | Manifest vs DUT | SHA256 match; `ccli --version` = bundle semver-rN |
| REQ-PROD-SW-006 | Routing / firewall | No path WAN → Eth_A DSO subnet |
| REQ-PROD-SW-007 | Config audit | No `cleartext_tsp` / `tcp_port: 102` in product yaml |
| Lab-first gate | Phase status | P3 MMS TLS + target P4 items **CLOSED** before first promote |

---

## Promote workflow (lab → clean product version)

1. Close lab gates for target **rN** (see `lab/evidence/README.md`).
2. Bump `apps/ccli/VERSION` · run `scripts/sync-ccli-version.ps1` · `CHANGELOG.md`.
3. Build `lab/tg544-openwrt/` → `ccli-<semver>-rN.apk`.
4. Deploy to reference DUT with **product** yaml (from `lab_tr400_phase4_eth_b.yaml` or regulation TLS variant); run § Verification.
5. Create `releases/product-cyber/<semver>-rN>/` per `releases/product-cyber/README.md`.
6. Optional git tag `product-cyber-v<semver>-rN`.
7. Re-ingest corpus: `scripts\ingest-product-deploy-rag.ps1` then `scripts\ingest-next-phases.ps1` (registry merge includes this doc).

---

## Keywords

`product freeze`, `lab-first`, `62351-3`, `62351-4`, `3782`, `releases/product-cyber`, `platform_tg500`, `TG-524`, `ccli-product-sw-freeze`, `C1-C5`, `cleartext excluded`
