# PC1 — hardware alignment + certificate preflight (before TSP Connect)

**Audience:** Masomeh (PC-B / Test Suite Pro)  
**Pack paths:** `D:\CCLI_TSP_FIELD_PACK\` (or `C:\CCLI_TSP_FIELD_PACK\`)

Use with **`00_UNIFIED_CERT_TEST_MATRIX.md`** session header and **`08_TSP_PC_FULL_BENCH_HW.md`**.

---

## A. PC1-only checks (you run these)

| □ | Check | Pass criteria |
|---|--------|----------------|
| A1 | Field pack present | Folder has `PACK_MANIFEST.json`, `00_UNIFIED_CERT_TEST_MATRIX.md`, `tls\`, `cid\` |
| A2 | LAN1 NIC | Admin: `bench_scripts\set_pc_lan1_dso.cmd` → PC = **192.168.10.10/24** |
| A3 | DUT reachability | `ping 192.168.10.1` OK (DUT powered, cable LAN1) |
| A4 | TLS folder (EJBCA sync) | `tls\root_CA.pem`, `client.pem`, `client_tls.pem`, **`client_tsp.key`**, `lab_crl.pem`, `revoked*.pem`, `revoked_tsp.key` — see **`00k_PC1_EJBCA_TLS_TESTPRO.md`** |
| A5 | TSP TLS profile | Mutual TLS · CA **`root_CA.pem`** · Transport **`client_tls.pem`** + **`client_tsp.key`** · MMS **`client.pem`** + **`client_acse.key`** · `verify-tsp-tls-key-match.ps1` PASS |
| A5b | OpenSSL on PC1 | `openssl version` → 3.x · `openssl verify -CAfile tls\root_CA.pem tls\client.pem` → OK (see **`00h_PC1_LOG_CAPTURE_REQUIREMENTS.md`**) |
| A6 | CID for Compare | **Cert:** `cid\product_tr400_eth_a_5ln.cid` if present; else read **`00d_PRODUCT_SPECIMEN_5LN_VALIDATION.md`** (Compare blocked until product CID) |
| A7 | 5-LN validator | From repo clone: `powershell -File scripts\validate-5ln-cid.ps1 -CidPath cid\product_tr400_eth_a_5ln.cid` → **PASS** before T1-02 |
| A8 | IED / endpoint | IED **`CCI016_01`** · host **`192.168.10.1:3782`** |
| A9 | O.15 doc pack in folder | Skim **`09_PICS_FILL.md`** … **`13_D7_62351-3_PID.md`** — cert submission set |
| A10 | No plant clash | This PC does **not** run meter probe / COM5 / 104 unless Yahia assigns dual-role |

Record in matrix header: TSP version, date, operator name.

---

## B. Two-PC bench (coordinate with PC-A / Yahia)

PC1 does **not** need LAN3 for TSP Phase 1, but **live TotW / PPV** (T1-04) needs plant path up on **PC-A**:

| □ | Item | PC-A action | DUT yaml (reference in pack `config\`) |
|---|------|-------------|----------------------------------------|
| B1 | EMT432 meter LAN3 | `bench_scripts\set_pc_lan3_plant_178.cmd` or `set_pc_lan3_plant.cmd` · `tools\modbus_emt432_tcp_probe.py` | **`lab_tr400_phase4_emt432_lan3_tcp.yaml`** |
| B2 | PPV from meter | Probe shows **V_L1_L2 @980** → kV; DUT firmware **r52+** with PPV-from-meter build when deployed | same |
| B3 | SUN2000 inverter mock | `tools\start-bench-plant.ps1 -SunOnly -Port COM5` (not `--huawei` RTU slave) | RS485 A1/B1; meter still **TCP** |
| B4 | IEC 104 | PC-A LAN2 **`192.168.1.183`** → DUT **`192.168.1.130:2404`** | phase4 yaml with `iec104:` enabled |
| B5 | Single ccli | SSH: stop **`iec61850service`**, one **`ccli`** PID | Avoid 3782/2404 bind clash |

Matrix rows: **U-P0-04**, **U-P0b-01**, **U-P0b-02**, **U-P0-07**.

---

## C. Certificate artefacts map (TSP + submission)

| Pack file | O.15 / lab use |
|-----------|----------------|
| `tls\*` | TSP Connect + cyber cases |
| `cid\product_tr400_eth_a_5ln.cid` | Compare / MICS (5-LN) |
| `09c_PICS_CCI016_01_filled.xlsx` | PICS workbook |
| `10_PIXIT_DRAFT.md` | PIXIT |
| `11_MICS_DRAFT.md` | MICS |
| `12_TICS_DRAFT.md` | TICS (after lab date) |
| `13_D7_62351-3_PID.md` | 62351-3 PID |
| `14_CID_PICS_ALIGNMENT.md` | CID ↔ PICS trace |
| `15_LAB_CONFIG_GUIDE.md` | D6 lab config |
| `16_LAB_SPECIMEN_VERSION.md` | Lab CID/cfg SHA pin |

---

## D. Refresh pack (GitHub)

```powershell
cd D:\CCLI\CCLI
git pull origin main
powershell -File scripts\stage-tsp-field-pack.ps1 -Dest D:\CCLI_TSP_FIELD_PACK
powershell -File scripts\push-tsp-field-pack-to-pc1.ps1 -StageOnly
```

Or wait for Yahia **robocopy** to `D:\CCLI_TSP_FIELD_PACK`.

---

**Keywords:** PC1, preflight, TLS, 5-LN, EMT432, SUN2000, unified matrix
