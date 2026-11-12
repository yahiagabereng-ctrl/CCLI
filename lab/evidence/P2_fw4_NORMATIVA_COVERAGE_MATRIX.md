# P2 / fw4 — normativa coverage matrix (TR500 / TG544)

**Document ID:** CCLI-LAB-EVD-P2-FW4-NORM-001  
**Revision:** 1.0  
**Date:** 2026-11-12  
**RAG source_id:** `ccli-p2-fw4-normativa-coverage`  
**Parents:** `lab/conformance/Full Validate.md` · `CCI_62443_Zones.md` · `CCI_62443_TesPro_HiTEKS_Gap_Matrix.md` · `CCI_Phase_Regulation_Checklists.md` · `VENDOR_CYBER_SW_REQUIREMENTS.md`  
**Cyber series index (all annexes & IEC families):** `lab/evidence/CYBER_NORMATIVA_SERIES_COVERAGE.md`  
**Evidence baseline:** OpenWrt **fw4** dump — TesproOS **32-2.1.0 r601-af70fcaaa2** (LuCI “Traffic rules” export, 2026-11-12)  
**Lab hard-gate reference:** `lab/evidence/O13_1_isolation_2026-09-25/P2_01_CROSS_PING_MATRIX.txt` (2025-09-25, **re-verify on r601**)

This document is the **single coverage map** for “firewall / zoning / network segregation” normativa referenced by the CCLI programme. Each row is in scope for a **fw4 + UCI + bind audit** or explicitly **OUT OF SCOPE** with reason.

---

## 1. Scope

### 1.1 In scope (this analysis shall cover)

| Family | IDs / clauses |
|--------|----------------|
| **CEI 0-16 Allegato O** | **O.13.1.1.1** (interfaces, no-bridge, roles, stack items that affect routing/filtering), **O.13.1.1.2** (local config IP exclusion) |
| **CEI 0-16 Allegato T** | **T.3.3.4.10** (traffic segregation: NAT/VLAN/fw/VPN), **T.3.3.4.11** (local / user remote access) |
| **IEC 62443-3-2** | **REQ-SEC-ZC-003…006** (zone/conduit separation; wireless/enterprise) |
| **IEC 62443-4-2** | **CR 5.1**, **NDR 5.2(1)** + **FSA RE(1–3)**, **NDR 5.3**, **NDR 1.6**, **NDR 1.13**, **CR 7.1(1)** (SYN flood), **CR 7.6**, **CR 7.7**, **CCSC 1G** (zone island — fw-related) |
| **Programme** | **REQ-NET-001…005**, **REQ-LTE-001**, **REQ-HW-COM-002**, **REQ-V-16…21**, **REQ-V-19/36**, **REQ-PROD-SW-006**, **REQ-IMPL-CYB-010**, **U-CYB-PROD-01** |
| **Phase checks** | **P2-01…P2-05**, **P2-G01**, **P0-01**, **P3-01**, **P4-01** (bind + fw together) |
| **ISA FSA (CSA 311)** | **FSA-CR 5.1**, **FSA-NDR 5.2 / 5.3**, **FSA-NDR 1.6 / 1.13** (where mapped in gap matrix) |
| **Board / Jira** | **Z0_Firewall_Network** — R1 no LAN1/LAN2/plant/WAN bridge, R2 domain separation |

### 1.2 Out of scope (other evidence tracks — not fw4 rows)

| Topic | Primary track |
|-------|----------------|
| **O.13.1.2 / O.13.1.3** comms-loss timers | App **P3-05 / P4-04** |
| **O.13.2** metrology, **O.13.3** UPS, **O.13.5** clock, **O.13.4 / T.3.3.4.8** signed update, **O.13.7** tamper, **O.13.10** RAMS | HW / platform / **P7** |
| **O.13.1.1.1 PHY** (100BaseFX, 1310 nm) | Product PHY / **ACC** |
| **T.3.3.4.1–9** crypto/PKI/RBAC/session (except **.10/.11** segregation) | **62351** / **P3-06…P3-09** |
| **PF1/PF2/PF3** control | **P3/P5** conformance |
| **61557-12 / 61010** | **U-ACC-04** |
| **62443 ZCR-2 / ZCR-7 / REQ-SEC-ZC-010** workshops & asset-owner sign-off | Governance |
| **Annex M** teletrip (except “no conflicting route”) | **P6** |

---

## 2. Port ↔ zone map (normative binding)

| Linux IF | fw4 chain | OpenWrt zone | CEI role | Corpus subnet |
|----------|-----------|--------------|----------|---------------|
| `lan1` | `*_lan1_sec` | `lan1_sec` | **Eth_A DSO** | 192.168.10.0/24 |
| `lan2` | `*_lan_sec` | `lan_sec` | **Eth_B operator** | 192.168.1.130/24 |
| `lan3` | `*_lan2_sec` | `lan2_sec` | **Plant** | 192.168.30.0/24 |
| `br-lan` | `*_lan` | `lan` | **Eng / O.13.1.1.2*** | 192.168.0.0/24 |
| `wan`, `usb0` | `*_wan` | `wan` | **LTE / WAN (C5)** | uplink |

\* O.13.1.1.2 requires serial/USB-only local config — eng on `br-lan` is a **documented engineering exception** until hardened (**P2-04 PART**).

---

## 3. Master matrix — normativa vs fw4 (r601)

**Legend — fw role:** **E** = enforced by nft/fw4 · **P** = partial (fw + app/OS) · **N** = not firewall · **—** = out of scope  
**Corpus verdict:** PASS / PART / GAP / N/A / OPEN (programme status, not re-tested on r601 unless noted)

| # | Normative ref | Shall (summary) | fw role | Observed fw4 (r601 pattern) | Corpus | In prior chat table? |
|---|---------------|-----------------|---------|-----------------------------|--------|----------------------|
| **A — CEI O.13.1.1.1** |
| A1 | O.13.1.1.1 no-bridge | No L2/L3 exchange among Eth_A, Eth_B, local config, plant **inside CCI** | E/P | Per-IF chains; no cross-zone forward accept; `dstnat` empty | PASS (2025-09-25) | Yes |
| A2 | O.13.1.1.1 routing | Role hosts do not reach each other **via CCI router** | E | `forward` drop; role chains reject; only `forward_lan`→wan | PASS P2-01 | Yes |
| A3 | O.13.1.1.1 **roles** | Eth_A = DSO; Eth_B = other remote operators | P | IF→zone map matches architecture | PASS roles | **No — added** |
| A4 | O.13.1.1.1 **PHY Eth_A** | 100BaseFX 1310 nm (product) | N | — | GAP lab copper | **No — added (N/A fw)** |
| A5 | O.13.1.1.1 **PHY Eth_B** | 10/100 TX | N | — | PART | **No — added (N/A fw)** |
| A6 | O.13.1.1.1 **stack** | IPv4 M; IPv6 O; MMS; TLS; optional DHCP/DNS on Eth_A | P | fw does not filter DHCP/DNS on lan1; INPUT accept-all | PART | **No — added** |
| A7 | O.13.1.1.1 **GOOSE** | Stack support; no bridge to other IF | P | fw silent on L2; P2-G01 **OPEN** | OPEN | **No — added** |
| A8 | O.13.1.1.1 **link log** | Eth_A/B phys+data-link in logger | N | — | PART P2-06 | **No — added (N/A fw)** |
| A9 | O.13.1.1.2 | Local config **USB/serial only**; **IP excluded** on role IFs | P | Role `input_*` = **accept all** | GAP P2-04 | Yes |
| A10 | O.13.1.2 | DSO comms loss → autonomous (timer) | N | — | PASS P3-05 | No (out of scope) |
| A11 | O.13.1.3 | Eth_B loss → aggregator fallback | N | — | PASS P4-04 | No (out of scope) |
| **B — Allegato T segregation** |
| B1 | T.3.3.4.10 | NAT/VLAN/**firewall**/VPN; public net only via secure VPN | P | Zones + deny forward; **no VPN product** | PART VPN OPEN | Yes |
| B2 | T.3.3.4.10 | No IT path into plant control from Internet | P | wan input reject; no dstnat | PART P2-03 | Yes |
| B3 | T.3.3.4.11 | Local commissioning: authenticated, non-IP or segregated | P | Eng zone open; role zones open | GAP | Yes |
| B4 | T.3.3.4.11 | User remote: SSH/HTTPS, mutual auth, SoD | P | Not restricted by fw on lan2 | PART | **No — added** |
| B5 | T.3.3.4.10 + O | **§8.8** maps to O open items (VPN private network) | P | Same as B1 | PART | **No — added** |
| **C — REQ-NET / architecture** |
| C1 | REQ-NET-001 | No L2 bridge Eth_A↔Eth_B↔plant | P | L3 fw OK; L2 needs `brctl`/Wi‑Fi | PART | Yes |
| C2 | REQ-NET-002 | WAN/LTE backup only; no DSO path | E/P | wan/usb0 zone; forward wan drop | PART/HAVE P2-03 | Yes |
| C3 | REQ-NET-003 | Silkscreen ↔ `ip link` ↔ role | N | — | OPEN | **No — added (N/A fw)** |
| C4 | REQ-NET-004 | Eng 192.168.0.0/24 | N | `br-lan` zone | HAVE | Yes |
| C5 | REQ-NET-005 | Wi‑Fi disabled product profile | N | Not in nft dump | OPEN | Yes |
| C6 | REQ-LTE-001 | LTE isolated from Eth_A control | E/P | usb0 in wan | PART | Yes |
| C7 | REQ-HW-COM-002 | SSH/HTTPS engineering only | P | All role INPUT accept | PART | Yes |
| C8 | REQ-PROD-SW-006 | No WAN→Eth_A bridge / route | E | No wan→lan1 forward rule seen | PART | **No — added** |
| C9 | U-CYB-PROD-01 | Product yaml audit: no WAN→Eth_A bypass | P | fw + routing + yaml | PASS (yaml audit) | **No — added** |
| C10 | REQ-IMPL-CYB-010 | Segmentation: `net_policy` + OpenWrt | E | Matches P2 design | PART image freeze | Yes |
| **D — Vendor cyber (REQ-V)** |
| D1 | REQ-V-16 | No L3 forward Eth_A / Eth_B / plant / WAN-LTE | E | Same as A1/A2 | PASS lab | **No — added** (equiv A1) |
| D2 | REQ-V-17 | No route WAN/LTE → Eth_A | E | input_wan reject; forward wan drop | PART P2-03 | Yes |
| D3 | REQ-V-18 | No IT services bridged into DSO zone | P | INPUT accept-all on lan1 | GAP | Yes |
| D4 | REQ-V-19 | Reference **firewall/UCI export** per SKU | N | This doc + export file | **GAP** | Yes |
| D5 | REQ-V-20 | Local config USB/serial or hardened ≠ Eth_A | P | fw does not block LuCI on lan1/2/3 | GAP | **No — added** |
| D6 | REQ-V-21 | No unauthenticated maint UI on DSO LAN | P | Same | GAP | **No — added** |
| D7 | REQ-V-36 | Deliverable = REQ-V-19 export | N | File this matrix + `nft`/`uci` | GAP | **No — added** |
| **E — IEC 62443-3-2 (zones)** |
| E1 | REQ-SEC-ZC-003 | Partition zones/conduits | P | 4 LAN + wan | PART | **No — added** |
| E2 | REQ-SEC-ZC-004 | Separate enterprise/IT from IACS | P | No IT bridge to Eth_A in fw | HAVE (pattern) | **No — added** |
| E3 | REQ-SEC-ZC-005 | USB maint separate from IACS | N | USB not in fw4 | PART | **No — added (N/A fw)** |
| E4 | REQ-SEC-ZC-006 | Wireless separate from wired DSO/plant | E/P | wan+usb0; Wi‑Fi OPEN | PART | **No — added** |
| E5 | REQ-SEC-ZC-009 | Document CRS / drawings | N | `CCI_62443_Zones.md` | PART | **No — added (N/A fw)** |
| **F — IEC 62443-4-2 (NDR / CR on gateway)** |
| F1 | CR 5.1 | Network segmentation | E | 4 domains | PASS | Yes |
| F2 | NDR 5.2(1) | Deny-by-default | E | input/forward policy drop | PASS | Yes |
| F3 | FSA-NDR 5.2 RE(1) | Deny all, permit by exception | P | Engine yes; INPUT wide on roles | PART | **No — added** |
| F4 | FSA-NDR 5.2 RE(2–3) | Island mode / fail-close under attack | P | Not proven on r601 | PART | **No — added** |
| F5 | NDR 5.3 / FSA-NDR 5.3 | Restrict unused / P2P / IT on Z0 | P | accept-all INPUT | PART | Yes |
| F6 | NDR 1.6 / FSA-NDR 1.6 | Wireless access management | P | usb0=wan | PART | **No — added** |
| F7 | NDR 1.13 / FSA-NDR 1.13 | Untrusted network (LTE) access control | E/P | wan rules + P2-03 | PART | Yes |
| F8 | CR 7.1(1) | Essential fn under DoS (flood) | E | `syn_flood` 25 pps on input | PART (app too) | **No — added** |
| F9 | CR 7.6 / FSA-CR 7.6 | Export security settings (UCI/nft) | N | **This matrix**; file export GAP | PART | Yes |
| F10 | CR 7.7 | Least functionality (no :102, GOOSE off) | P | App gate; fw does not block :102 | PART | Yes |
| F11 | CCSC 1G / FSA-CCSC 1G | Zone isolation keeps essential functions | P | Forward isolate OK; services not | PART | **No — added** |
| F12 | CR 5.4 | — | — | N/A component | N/A | **No — added** |
| **G — Phase / lab gates** |
| G1 | P0-01 | Zone bind Eth_A/B in config | P | fw + yaml + `ccli_config` | PASS | **No — added** |
| G2 | P2-01 | Cross-ping matrix PC↔PC | E | Consistent with nft | PASS 2025-09-25 | Yes |
| G3 | P2-02 | Zone UCI forward=REJECT masq=0 | E/P | Pattern OK; confirm `uci` | PASS 2025-09-25 | Yes |
| G4 | P2-03 | LTE/WAN ↛ DSO | E | + IPsec→lan stock hole | PART/HAVE | Yes |
| G5 | P2-04 | LuCI/SSH not on Eth_A | P | INPUT accept | GAP | Yes |
| G6 | P2-05 | 62443 signed zones | N | Doc PART | PART | Yes |
| G7 | P2-G01 | GOOSE plant-only | P | L2 not in fw4 | OPEN | Yes |
| G8 | P3-01 | MMS Eth_A only | P | fw + `ss -tlnp` | PASS app | Yes |
| G9 | P4-01 | 104 not on Eth_A | P | fw + bind | PASS app | Yes |
| **H — Full Validate cross-refs (O.13 / T / 62443)** |
| H1 | Full Validate O.13.1.1.1 no-bridge | Same as A1 | E | — | PASS | (duplicate A1) |
| H2 | Full Validate T.3.3.4.10/.11 | Same as B1–B4 | P | — | PART | (duplicate B) |
| H3 | Full Validate CR 5.1 / NDR 5.2 / 5.3 | Same as F1–F5 | E/P | — | PASS/PART | (duplicate F) |
| **I — Stock / operational fw behaviours** |
| I1 | (good practice) | No DNAT to CCI | E | `dstnat` empty | PASS | Yes |
| I2 | (good practice) | WAN SYN rate limit | E | syn_flood chain | PASS | Yes |
| I3 | (good practice) | TCP MSS clamp on WAN | E | mangle forward/postrouting wan | PASS | Yes |
| I4 | (risk) | IPsec wan → `accept_to_lan` | P | Stock OpenWrt rule | PART | Yes |
| I5 | IPv6 | Same zone logic v6 | P | ICMPv6 on wan; role INPUT accept v6 | PART | Yes |
| I6 | Eng → Internet | `forward_lan` → wan masq only | E | By design lab | PASS pattern | Yes |

**Row count:** **62** normative rows (**in scope** for fw4 analysis).

---

## 4. Coverage audit — “do we cover all normativa?”

### 4.1 Sources cross-checked (corpus)

| Source document | fw-relevant clauses indexed? |
|-----------------|------------------------------|
| `lab/conformance/Full Validate.md` §A.6 O.13 + §T.3.3.4.10/.11 + §62443 FR5/NDR | **Yes** (rows A, B, H, F) |
| `CCI_62443_Zones.md` REQ-SEC-ZC-* + P2 as-built | **Yes** (rows C, E, G) |
| `CCI_62443_TesPro_HiTEKS_Gap_Matrix.md` REQ-V-16…21, NDR rows | **Yes** (rows D, F) |
| `CCI_Phase_Regulation_Checklists.md` Phase 2–4 fw-related | **Yes** (rows G) |
| `lab/conformance/VENDOR_CYBER_SW_REQUIREMENTS.md` §C–D | **Yes** (rows D) |
| `CCI_Implemented_Cyber_Profile.md` C1–C5 conduits | **Yes** (implicit in zone map §2) |
| `CCI_Architecture_Framework.md` REQ-NET-* | **Yes** (rows C) |
| `CCI_Product_SW_Freeze.md` REQ-PROD-SW-006 | **Yes** (C8) |
| `LAB_UNIFIED_CERT_TEST_MATRIX.md` U-CYB-PROD-01 | **Yes** (C9) |
| `scripts/jira/CCLI_Jira_Components.md` Z0_Firewall_Network | **Yes** (A1–A2) |
| `CCI_ISASecure_CSA_311_v1_5_Extract.md` FSA-NDR 5.x, 1.6, 1.13 | **Yes** (F3–F7) |

### 4.2 Items that were **missing from the chat analysis** (now in matrix)

| ID | Why it matters |
|----|----------------|
| **O.13.1.1.1 roles / stack / GOOSE / link log** | Full Validate lists separately; only **no-bridge** was in chat |
| **REQ-V-16, V-20, V-21, V-36, REQ-PROD-SW-006, U-CYB-PROD-01** | Vendor/ programme IDs not all named |
| **REQ-SEC-ZC-003/004/006** | 62443-3-2 zone process tied to fw evidence |
| **NDR 5.2 RE(1–3), CCSC 1G, CR 7.1(1), FSA rows** | CSA 311 assessment paths |
| **T.3.3.4.11** user remote SSH/HTTPS SoD | Distinct from O.13.1.1.2 local |
| **P0-01** zone bind | Preconditions Phase 3/4 |

### 4.3 Intentionally **not** duplicated as separate fw rows

These appear in **Full Validate** or O.15 but are **not** firewall evidence (listed in §1.2 only):  
O.13.2–O.13.3, O.13.4, O.13.5, O.13.7, O.13.10, O.13.1.2–3, T.3.3.4.1–9 (crypto/PKI), 62351-100-3 labs, 61557-12, PF clauses, PHY optical, ZCR workshops.

### 4.4 Not covered by programme corpus (external norm)

| Norm | Note |
|------|------|
| **ISO/IEC 27001** | Not mapped to TG544 fw in CCLI corpus |
| **NIS2 / national decree text** | Legal; referenced only via ARERA/CEI in programme |
| **DSO-specific “Operating Rule” firewall** | Contractual overlay on top of O.13 — **MISSING** until DSO YAML |

---

## 5. Verdict rollup (fw4-only, r601 pattern)

| Track | PASS | PART | GAP | OPEN | N/A |
|-------|------|------|-----|------|-----|
| O.13.1.1.1 L3 isolation | 2 | 3 | 0 | 1 (GOOSE) | 3 PHY/log/stack detail |
| O.13.1.1.2 + T.3.3.4.11 | 0 | 2 | 2 | 0 | 0 |
| REQ-V / REQ-NET / REQ-PROD | 1 | 8 | 3 | 2 | 1 |
| 62443 NDR/CR (fw) | 2 | 9 | 0 | 0 | 1 |
| Phase P2 | 2 | 3 | 1 | 1 | 2 |

**Bottom line:** **Normativa inventory for fw4 analysis is complete** in this file (62 rows). **Conformance** is **not** complete: main **GAPs** remain **O.13.1.1.2 / REQ-V-20/21 / REQ-V-18** (INPUT allowlist + service bind), **REQ-V-19/36** (export on file), **re-verify P2-01/03 on r601**, **REQ-NET-005** Wi‑Fi, **P2-G01** GOOSE L2.

---

## 6. Evidence actions (to close GAPs)

| Priority | Action | Closes |
|----------|--------|--------|
| P0 | On DUT: `uci export firewall`; `nft list ruleset` → `lab/evidence/P2_fw4_export_r601.txt` | REQ-V-19/36, CR 7.6 |
| P0 | Re-run P2-01 matrix + P2-03 LTE ping on **r601** | G2, G4, C2 |
| P1 | **INPUT** allowlist: lan1 **tcp 3782**; lan2 **tcp 2404**; lan3 plant ports; drop rest | A9, D3, D5, F5, B3 |
| P1 | Bind **ssh/uhttpd/ccli** to **br-lan** only | A9, D5, D6, G5 |
| P2 | Disable unused **IPsec→lan** forward rules | I4, G4 |
| P2 | Wi‑Fi off + confirm no bridge (`REQ-NET-005`) | C5, E4 |
| P3 | VPN policy for LTE if public Internet required (**T.3.3.4.10**) | B1, B5 |

---

## 7. Maintenance

| Event | Update |
|-------|--------|
| New TesproOS / firewall4 release | Re-export §3 “Observed” column + re-run §6 P0 |
| Product image freeze | Link `releases/product-cyber/<rN>/` to this file rev |
| DSO Operating Rule firewall addendum | Add section **4.4** row block |

## 8. Relation to full cyber normativa

This file covers **Layer 1 (network / fw4)** only (~62 rows). **All** cyber series and CEI/IEC annexes are indexed in **`CYBER_NORMATIVA_SERIES_COVERAGE.md`** (62351, 62443, O.13–O.15 #3–#6, T.3.3.4.1–11, etc.). Clause-level status remains in **`Full Validate.md`** and **`CCI_62351_Clause_Verification_Matrix.md`**.

**Changelog:** Rev 1.0 — initial full normativa coverage vs fw4 chat gap-fill. Rev 1.0 + §8 — link to cyber series index.
