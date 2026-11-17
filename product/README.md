# CCLI product

**Document ID:** CCLI-PROD-TREE-001  
**Date:** 2026-11-17  
**Entry:** `apps/ccli/program/main.c`  
**Version file:** `apps/ccli/VERSION` → `0.1.0` / `57` / `P5-5LN-ISO-PRES-AARE` (codename is the old A0 label; the ship model is 17 LN)

This folder is the product firmware line. The lab line stays at the repo root `apps/ccli` (`ccli_main`). Send **this** folder to the lab. Do not deploy it with the lab script as that script is written today.

Cyber and the 17 logical nodes: [`PRODUCT_17LN_CYBER.md`](PRODUCT_17LN_CYBER.md).  
Config index: [`apps/ccli/config/README.md`](apps/ccli/config/README.md).

---

## Send these files

| Path | DUT path | Role |
|------|----------|------|
| `apps/ccli/config/product_tr400.yaml` | `/etc/ccli/lab.yaml` | Ship. EMT432 Modbus TCP on LAN3. DSO mock off |
| `apps/ccli/config/product_tr400_emt432_17ln.yaml` | same | Same plant and the same 17-LN model |
| `apps/ccli/config/lab_tr400_phase4_eth_b_ttyS1_17ln.yaml` | same | USB-RS485 meter on **A1/B1** `/dev/ttyS1`. DSO mock **on** |
| `apps/ccli/config/icd/product/product_tr400_eth_a_17ln.cfg` | `/etc/ccli/icd/product_tr400_eth_a_17ln.cfg` | Model the three yamls name |
| `apps/ccli/config/icd/product/product_tr400_eth_a_17ln.cid` | TSP import | Same specimen, for the client |

Host check before the drop: `python lab/tmp/validate_product_17ln.py` from the repo root (18 pass on 2026-11-17). Host build: `cmake -S product/apps/ccli -B build/wsl-product -DBUILD_PLATFORM=tg500 -DCCLI_LEGACY_MAIN=OFF -DCCLI_BUILD_TESTS=ON`, then `ctest` (8/8).

There is no TG544 `.apk` of this entry yet. The OpenWrt package builds repo `apps/ccli` unless the SDK is given:

```bash
make package/feeds/ccli/ccli/compile V=s \
  CCLI_SRC_DIR=/mnt/c/Yahia/projects/CCLI/product/apps/ccli
```

`lab/tg544-openwrt/deploy-ccli-session-fix.ps1` uploads `lab_tg544_eth_a.cfg` and `product_tr400_eth_a_5ln.cfg`. It does not upload the 17-LN cfg. Copy the `.cfg` onto the path in `mms.model_cfg` or the process starts the wrong model.

---

## Which yaml on the bench

| Bench | Yaml |
|-------|------|
| EMT432 on LAN3 `192.168.178.250:502` | `product_tr400.yaml` |
| USB-RS485 meter, COM dongle to **A1/B1** | `lab_tr400_phase4_eth_b_ttyS1_17ln.yaml` |

`ccli` has one Modbus master. A second USB-RS485 for the inverter is not polled. A2/B2 (`/dev/ttyS2`) is not the meter port on this unit.

---

## I/O this drop actually drives

| Silk | Yaml | On the DUT |
|------|------|------------|
| DIO1 | `io.do_curtail` gpio 1, active low | Curtail relay |
| DIO2 | `io.di_permissive` gpio 2, active low | Permissive key |
| DIO3 | `annex_m_trip_monitor.enabled: false` | Not read |
| DIO4 | `io.idg_pos.enabled: false` | `IDGXCBR1.Pos` stays Dbpos bad |

UCI: channel 1 = DO, channel 2 = DI.

Not in this application: DRC-60A AC-OK and battery-low contacts, AI1 battery voltage, a second RS485 master. The one-hour backup (O.13.3) is a power test on CON2 from the DRC-60A. Wiring notes: `lab/DRC60A_UPS_WIRING.md`, `lab/CURTAIL_BUTTON_AND_RELAY_SETUP.md`.

---

## Do not load on this binary

| File | Why |
|------|-----|
| `config/icd/product/product_tr400_eth_a_5ln.*` | A0 pin. Not selected by the ship yaml |
| `config/icd/lab_tg544_eth_a.*` | Lab 31-LN model |
| `config/lab_tr400_cleartext_tsp*.yaml` | MMS port 102. Startup refuses it |
| `config/product_tr400_p3_07_connect.yaml` | Old TestPro trust. Loads the lab CID |
| `config/lab_tr400.yaml` | Phase-1 lab profile, not the 17-LN ship |

Startup refuses MMS without TLS or on port 102, and IEC 104 without TLS. GOOSE is off (`CCLI_WITH_GOOSE=OFF` and `goose.enabled: false`).

---

## Layout

| Path | Role |
|------|------|
| `apps/ccli/program/` | SK0153 loop. `main.c` → boot → startup → regulation, Modbus, MMS, 104, I/O |
| `apps/ccli/core/` `adapters/` `services/` | Copied from the lab tree. Merge on purpose |
| `apps/ccli/config/` | Yamls and the ICD |
| `apps/ccli/third_party` | Junction to `apps/ccli/third_party` at the repo root. `scripts/link-product-third-party.ps1` after a fresh clone |
| `PRODUCT_17LN_CYBER.md` | 17 LN table and the cyber stack |

Workspace file at the repo root: `CCLI-Lab-And-Product.code-workspace`.
