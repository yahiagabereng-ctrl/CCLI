# CCI OpenWrt Freeze — TesPro Validation Path

**Document ID:** CCLI-OS-001  
**Revision:** 1.4  
**Date:** 2026-09-17  
**RAG source_id:** `ccli-openwrt-freeze`  
**Status:** **FROZEN**  
**Parents:** `ccli-soc-freeze` · `ccli-tg500-lab-platform` · `ccli-app-structure` · `ccli-tespro-supplier-correspondence`

---

## Freeze decision

| Field | Frozen value |
|-------|----------------|
| **Product OS** | **OpenWrt 25.12** (bench TG544, kernel **6.12** MediaTek) |
| **Application language** | **C/C++** user-space services (`apps/ccli/`) |
| **Lab DUT** | **TG544 / TR500** — TesproOS **32-2.1.0** / OpenWrt **25.12** (`platform_tg500`) |
| **Product SKU** | **TG-524** (same MT798X / OpenWrt line) |
| **Target build** | OpenWrt **25.12.5** `mediatek/filogic` SDK (`lab/tg544-openwrt/`) |
| **Removed** | Yocto, Buildroot-as-product-OS, **Raspberry Pi lab path**, NXP/Toradex Linux paths |

**OpenWrt is frozen.** **TesPro only** — no parallel Pi 4 / `platform_pi` validation path.

---

## Validation ladder

```
apps/ccli/  — portable C/C++ (PF2, Modbus, MMS, IEC104)
       │
       └── platform_tg500  →  OpenWrt / TesproOS on TG544 / TG-524
              SDK cross-compile (lab/tg544-openwrt/) or on-device build
              test_vectors/ · L1-P-* · L1-C-* on real DUT
```

| Stage | Platform | What it proves |
|-------|----------|----------------|
| **L1** | **TG544** — logic + unit tests on target or host | PF2 FSM, adapters, config load |
| **L2** | **TG544** + USB-RS485 A1/B1 + DIO1/DIO2 | Physical serial, ubus DIO, PF2 curtailment |
| **L3** | TG544 + real analyzer | Register map, `.apk`, TPM, Secure Boot |
| **L4** | TG-524 in cabinet | Thermal, EMC, field I/O expansion |

---

## Requirements

| ID | Requirement | Status |
|----|-------------|--------|
| REQ-OS-001 | OpenWrt is the only product OS | **FROZEN** |
| REQ-OS-002 | No Yocto in active build path | **FROZEN** — K0.2 |
| REQ-OS-003 | Validation on **TesPro DUT**, not Pi mock | **FROZEN** — 2026-09-17 |
| REQ-OS-004 | Same test vectors on TG544 / TG-524 | **ACTIVE** — K8.4 |
| REQ-OS-005 | OpenWrt SDK for host cross-compile (MT798X) | **PARTIAL** — `lab/tg544-openwrt/` |
| REQ-OS-006 | `.apk` + procd init for CCI on OpenWrt 25.12 | **PARTIAL** — first `.apk` built |
| REQ-OS-007 | OpenWrt **25.12** + kernel **6.12** on lab DUT | **CONFIRMED** — TG544 |
| REQ-OS-008 | libmodbus LGPL commercial use on OpenWrt | **CONFIRMED** — supplier email |

---

## Architecture

| Layer | TG-524 / TG544 (`platform_tg500`) |
|-------|-----------------------------------|
| **OS** | **OpenWrt 25.12** / TesproOS |
| **Build** | Cross via SDK or on-device |
| **HAL** | ubus `dido_v2`, `ttyS1`/`ttyS2`, net |
| **Config** | `lab_tr400.yaml` → `/etc/ccli/lab.yaml` |
| **Deploy** | `.apk` + procd |

**Portable:** `core/`, `adapters/`, `services/`, `test_vectors/`

---

## Interface matrix

| Test asset | TG544 binding |
|------------|-----------------|
| Engineering | LAN2 → `br-lan` |
| Plant Modbus RTU | A1/B1 → `/dev/ttyS1` |
| Curtailment DO | DIO1 → ubus ch1 |
| Permissive DI | DIO2 → ubus ch2 |
| PF2 config | `lab_tr400.yaml` |

---

## BOM matrix

| Block | Function | Part | Status |
|-------|----------|------|--------|
| OS-PROD | Product OS | OpenWrt 25.12 on TesPro | **FROZEN** |
| DUT-LAB | Lab gateway | TG544 / TR500 | **ACTIVE** |
| SDK | Cross-compile | OpenWrt 25.12.5 filogic | **PARTIAL** |
| APP | Firmware | `apps/ccli/` | **BUILD on TG544** |

---

## Knowledge gaps

| Area | Current | Required | Gap |
|------|---------|----------|-----|
| TesPro vendor SDK | Upstream SDK only | Image-matched sysroot | **OPEN** |
| procd / apk field update | First package built | Signed update path | **PARTIAL** |
| UCI loader | Stub | Persistent product config | **OPEN** — K2.5 |

---

## Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-OS-001 | Upstream SDK ≠ TesproOS image | ABI mismatch | Verify on TG544; request TesPro SDK |
| R-OS-002 | Yocto docs re-enter corpus | Wrong build path | K0.2; archive only |
| R-OS-003 | No Pi fallback | Slower host-only debug | Host unit tests; TG544 always available |

---

## Verification

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| REQ-OS-001 | RAG probe for Yocto in active path | Zero hits outside `_archive/` |
| REQ-OS-003 | No active doc references Pi lab path | Pi marked ARCHIVED |
| REQ-OS-005 | `lab/tg544-openwrt/build-ccli-ipk-sdk.sh` | `ccli-*.apk` for aarch64 |
| REQ-OS-007 | `cat /etc/openwrt_release` on TG544 | 25.12.x |

---

## Keywords

`OpenWrt`, `25.12`, `TesPro`, `TG544`, `platform_tg500`, `validation`, `cross-compile`, `ccli-openwrt-freeze`, `Pi removed`
