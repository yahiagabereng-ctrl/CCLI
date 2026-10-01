# TG-500 Knowledge Tree — Remaining Work

Generated: 2026-09-16T09:31:01.603426+00:00

Derived from `knowledge-base/08-engineering/CCI_TG500_Knowledge_Tree.md`.
Regenerate with `python scripts/build-tg500-knowledge-tree.py`.

## P0 — 4 open

| Node | Branch | Status | Knowledge item | Risk |
|------|--------|--------|----------------|------|
| K1.6 | K1 | `PART` | SKU confirmation TG-424 Pro vs TG-524 (written) | R-TG-004 |
| K1.7 | K1 | `PART` | MT798X reference manual / GPIO + pinmux detail | R-SOC-001, R-SOC-006 |
| K2.2 | K2 | `MISS` | TesPro OpenWrt SDK / cross-toolchain + sysroot | R-SOC-001, R-TG-003 |
| K3.1 | K3 | `PART` | Port map: WAN/LAN1-4 → Eth_A / Eth_B / plant | KR-007 |

## P1 — 5 open

| Node | Branch | Status | Knowledge item | Risk |
|------|--------|--------|----------------|------|
| K4.2 | K4 | `MISS` | IEC 61850 GOOSE publish/subscribe + L2 timing | KR-002 |
| K4.5 | K4 | `MISS` | Energy analyzer register map (real model) | — |
| K5.3 | K5 | `MISS` | DI 10–120 Vdc conditioning for plant class (C4) | KL-003 |
| K5.4 | K5 | `MISS` | DO dry-contact / relay rating for curtailment (C4) | KL-004 |
| K7.7 | K7 | `PART` | CEI EN 61557-12 metrology (P/Q/V, Annex D fixed blocks) | R-MET-005 |

## P2 — 7 open

| Node | Branch | Status | Knowledge item | Risk |
|------|--------|--------|----------------|------|
| K2.4 | K2 | `MISS` | .ipk packaging + procd init for CCI services | — |
| K2.5 | K2 | `MISS` | UCI config model + overlay persistence policy | — |
| K2.7 | K2 | `MISS` | Signed / A-B firmware update path on OpenWrt | KR-005 |
| K3.2 | K3 | `MISS` | Port-to-port galvanic isolation proof | KR-007 |
| K3.6 | K3 | `MISS` | IEEE 1588 / PTP need + capability | KR-008 |
| K4.9 | K4 | `MISS` | libiec61850 commercial licence (MZ quote) | — |
| K6.7 | K6 | `MISS` | Vulnerability handling / SBOM process | KR-012 |

## P3 — 22 open

| Node | Branch | Status | Knowledge item | Risk |
|------|--------|--------|----------------|------|
| K1.4 | K1 | `PART` | Thermal −40…+75 °C vs cabinet budget | KR-013 |
| K1.5 | K1 | `PART` | Vendor response form vs datasheet deltas | R-TG-004 |
| K2.3 | K2 | `PART` | OpenWrt build system fundamentals (feeds, menuconfig) | KR-004 |
| K2.6 | K2 | `PART` | Boot chain: bootloader → kernel → OpenWrt rootfs | KR-004 |
| K3.3 | K3 | `PART` | No-L2-bridge firewall policy (REQ-NET-LAB-001) | KR-011 |
| K3.4 | K3 | `PART` | LTE/WAN backup + VPN posture (no bridge to Eth_A) | R-TG-005 |
| K3.5 | K3 | `PART` | GNSS time source via modem | KR-009 |
| K4.1 | K4 | `PART` | IEC 61850 MMS server (libiec61850) | KR-001 |
| K4.4 | K4 | `PART` | Modbus RTU/TCP master (libmodbus) | — |
| K4.6 | K4 | `PART` | IEC 60870-5-104 on Eth_B (lib60870) | KR-010 |
| K4.7 | K4 | `PART` | PF2 / observability curtailment FSM (new C/C++) | KR-002 |
| K4.10 | K4 | `PART` | 61557-12 fixed-block aggregation + EPMF measurement path | R-MET-001 |
| K5.1 | K5 | `PART` | 2× RS485 electrical behaviour + isolation (C2) | — |
| K5.2 | K5 | `PART` | RS485 termination / TVS / connector craft | KL-002 |
| K5.5 | K5 | `PART` | Wave B fixture to reach 5 DI / 3 DO on Pi | R-TG-002 |
| K6.1 | K6 | `PART` | Secure Boot chain verified on receipt | KR-005 |
| K6.2 | K6 | `PART` | TPM 2.0 presence + tpm2_tools access | KR-005 |
| K7.5 | K7 | `PART` | DSO demo / conformance route for CCI | R-TG-001 |
| K7.6 | K7 | `PART` | EMC / CE for a field product | KR-014 |
| K8.4 | K8 | `PART` | Pi ↔ TR400 identical test vectors | — |
| K8.5 | K8 | `PART` | Evidence pack + records schema per REQ | KL-017 |
| K8.6 | K8 | `PART` | Verification matrix REQ → test → pass criteria | — |

## Roll-up

| Branch | Title | Status |
|--------|-------|--------|
| K0 | Corpus & RAG governance | 5 HAVE |
| K1 | Platform & silicon | 4 HAVE · 4 PART · 1 DEF |
| K2 | OS, build & boot toolchain — critical path | 1 HAVE · 2 PART · 4 MISS |
| K3 | Network & segmentation | 1 HAVE · 4 PART · 2 MISS |
| K4 | Protocols & application firmware | 2 HAVE · 5 PART · 3 MISS |
| K5 | Field I/O & serial | 3 PART · 2 MISS · 1 DEF |
| K6 | Security & cyber compliance | 1 HAVE · 2 PART · 1 MISS |
| K7 | Regulation & certification | 4 HAVE · 3 PART |
| K8 | Verification & lab operations | 3 HAVE · 3 PART |
