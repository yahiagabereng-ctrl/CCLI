# Product 17-LN — lab handoff for Alireza

**Date:** 2026-11-17  
**Audience:** @alirezavzand  
**Tree:** `product/apps/ccli` · entry `program/main.c`  
**Index:** [`product/README.md`](../../product/README.md)

This is the ERDA Wave-1 **17 logical node** product. It replaces the 5-LN ship model. The old 5-LN CID stays in the tree and is not what the ship yaml loads.

---

## Pull

```text
git pull origin main
```

Start at [`product/README.md`](../../product/README.md). Cyber and the 17 names: [`product/PRODUCT_17LN_CYBER.md`](../../product/PRODUCT_17LN_CYBER.md).

---

## What to put on the TG544

| File | DUT path |
|------|----------|
| `product/apps/ccli/config/product_tr400.yaml` | `/etc/ccli/lab.yaml` when the meter is EMT432 TCP on LAN3 |
| `product/apps/ccli/config/lab_tr400_phase4_eth_b_ttyS1_17ln.yaml` | `/etc/ccli/lab.yaml` when the meter is USB-RS485 on **A1/B1** |
| `product/apps/ccli/config/icd/product/product_tr400_eth_a_17ln.cfg` | `/etc/ccli/icd/product_tr400_eth_a_17ln.cfg` |
| `product/apps/ccli/config/icd/product/product_tr400_eth_a_17ln.cid` | Import in Test Suite Pro |

`lab/tg544-openwrt/deploy-ccli-session-fix.ps1` still uploads the **5-LN** cfg and builds repo `apps/ccli`. Do not use it unchanged. There is **no** `.apk` of this product entry yet. The SDK line is in `product/README.md` (`CCLI_SRC_DIR=product/apps/ccli`).

Host check already run on 2026-11-17: `python lab/tmp/validate_product_17ln.py` → 18 pass, 0 gap. Host `ctest` in `build/wsl-product` → 8/8. That is not a DUT pass.

---

## 17 logical nodes

`LLN0` `LPHD1` `PdC_WiDPCC1` `PdC_WaDPCC1` `PdC_QiDPCC1` `PdC_QcDPCC1` `PdC_VADPCC1` `DisFRDECP1` `DisFRDGEN1` `DisFRDSTO1` `PdCMMXU1` `StMMXU1` `SGGMMXU1` `IDGXCBR1` `SSGGDGEN1` `WlimDWMX1` `WSdDAGC1`

Live on the publish tick: `PdCMMXU1` P/Q/V, and `StMMXU1.TotW` plus `SGGMMXU1.TotW` from that same plant meter (one storage group). `WlimDWMX1` and `WSdDAGC1` take Operate. `IDGXCBR1.Pos` is written only when `io.idg_pos` is enabled; the ship yaml leaves it disabled, so Pos stays Dbpos bad. Ratings, readiness, and `SSGGDGEN1` are nameplate in the CID.

Not in this model: Gen* MMXU, VArSd, PFSP, VArV, PFW, GOOSE.

---

## Bench limits (do not expect these in MMS)

| Item | State |
|------|--------|
| DIO1 relay / DIO2 key | In the yaml. UCI channel 1 = DO, channel 2 = DI |
| DIO3 Annex M, DIO4 breaker key | Off |
| DRC-60A one-hour backup | Power on CON2 only. AC-OK and battery-low contacts are not yaml inputs |
| Second USB-RS485 (inverter) | Not polled. One Modbus master |
| `program/program_flash.c` | Retain requests are accepted and not written |

Version file `product/apps/ccli/VERSION` still reads `P5-5LN-ISO-PRES-AARE`. That codename is the old label. The model path in the yaml is the 17-LN cfg.
