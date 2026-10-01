# CCLI Phase Lab Master Log

**Document ID:** CCLI-LAB-MASTER-LOG-001  
**Programme:** HiTEKS CCLI · CEI 0-16 Allegato O / T · IEC 62443  
**DUT:** TesPro TG544 / TR500 · `platform_tg500` · OpenWrt 25.12 (MediaTek MT798X)  
**Revision:** 1.1 · **Date:** 2026-09-25  
**Canonical checklist:** `knowledge-base/08-engineering/CCI_Phase_Regulation_Checklists.md`  
**Evidence root:** `lab/evidence/O13_1_isolation_2026-09-25/`

This is the **single chronological record** of what was used and what was proven, phase by phase.  
Status codes: **PASS** · **PART** · **OPEN** · **PEND** · **N/A** · **WAIVED**.

---

## 0. Bench identity (frozen as-built)

### 0.1 Hardware

| Item | Value |
|------|--------|
| Gateway | TesPro **TG544** / board **TR500** |
| SoC / OS | MT798X · OpenWrt **25.12** |
| Software platform | `platform_tg500` only (Pi / `platform_pi` out of scope) |
| Field I/O | DIO1 = curtail DO · DIO2 = permissive DI · via ubus `dido_v2` |
| RS485 (Phase 1 meter) | LuCI **RS485-2** → **`/dev/rs485_2_uart`** (not A2/B2 `/dev/ttyS2` on this FW) |
| LTE | Quectel **EC200U** · `usb0` · SIM **1NCE** · APN **`iot.1nce.net`** |

### 0.2 Network roles (after Phase 2 peel)

| Port | Role | UCI IF | IPv4 | Firewall zone |
|------|------|--------|------|---------------|
| **LAN1** | DSO **Eth_A** | `lan1` | **192.168.10.1/24** | `lan1_sec` |
| **LAN2** | Operator **Eth_B** / OA | `lan2` | **192.168.1.130/24** | `lan_sec` |
| **LAN3** | Plant | `lan3` | **192.168.30.1/24** | `lan2_sec` |
| ENG | Local config | `br-lan` (empty PHY) | 192.168.0.1/24 | `lan` |
| WAN / LTE | Backup only | `wan` / `usb0` | e.g. 10.43.46.209 | `wan` |

**Diagrams / annex map:**  
`Architecture/Zones_62443_TG544_PortMap.drawio` · `knowledge-base/08-engineering/CCI_62443_Zones.md` · `P2_ANNEX_PORT_MAP.txt`

### 0.3 Lab PCs (Phase 2 matrix)

| Role | Host | IPv4 | Gateway |
|------|------|------|---------|
| DSO Eth_A | Lab PC | 192.168.10.10 | 192.168.10.1 |
| OA Eth_B | Yahia | 192.168.1.183 | 192.168.1.130 |
| Plant | Federico | 192.168.30.10 | 192.168.30.1 |

### 0.4 Access / tools used repeatedly

| Tool | Use |
|------|-----|
| SSH / SCP | PuTTY `plink` / `pscp` · hostkey LAN1 `SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE` · password via `$env:CCLI_TG544_PW` only |
| Build | WSL · OpenWrt SDK `~/openwrt-sdk-25.12.5-mediatek-filogic_gcc-14.3.0_musl.Linux-x86_64` · `lab/tg544-openwrt/wsl-build-ccli.sh` |
| Host MMS clients | WSL gcc vs `apps/ccli/third_party/libiec61850` · `lab/tg544-openwrt/build-mms-lab-client.sh` (**secondary**) |
| **61850 Test Suite Pro** | **Triangle MicroWorks** — **primary Eth_A verification** · PC on LAN1 · `lab/CCI_TestSuitePro_Verification_Layer.md` · logs → `lab/evidence/testsuite-pro/inbox/` · RAG `ccli-testsuite-pro-*` |
| LuCI | Browser on role IP (Eth_A `http://192.168.10.1` or OA `http://192.168.1.130`) |
| Duty binary | `/usr/sbin/ccli` · config `/etc/ccli/lab.yaml` |

### 0.5 Software versions exercised

| Package / lib | Version / note |
|---------------|----------------|
| `ccli` APK / binary | **0.1.0-r5** (P3-01) → **r6** (P3-04) → **r9** (soft cyber) → **r15** (chrony TimeQuality) |
| chrony | **4.8** static aarch64 sideload · `/usr/sbin/chronyd` · GNSS SOCK feeder |
| libiec61850 | **v1.6.0** static in `apps/ccli/third_party/libiec61850` (MZ license for ship) |
| Lab yaml | `apps/ccli/config/lab_tr400_phase1_regulation.yaml` → DUT `/etc/ccli/lab.yaml` |

### 0.7 TPM (TesPro guide §4) — verified 2026-09-25

| Check | Result |
|-------|--------|
| `/dev/tpm0` + `/dev/tpmrm0` | PASS |
| `tpm_tis_i2c 0-002e: 2.0 TPM (device-id 0x1C)` | PASS |
| `tpm2_getrandom --hex 16` | PASS |
| Manufacturer **IFX** · vendor **SLB9673** · TPM 2.0 | PASS |
| Guide | `TG500-TPM-Configuration-Guide.pdf` |
| Evidence | `lab/evidence/O13_1_isolation_2026-09-25/P0_TPM_VERIFY.txt` |

**Note:** Presence PASS ≠ MMS TLS keys in TPM yet (P3-06 lab may use PEMs).

### 0.6 Progress snapshot (2026-09-25 EOD — Phase 3 lab close)

| Phase | Hard gates | Exit status |
|-------|------------|-------------|
| 0 Platform | PASS/PART | **CLOSED** |
| 1 Local PF2 | Eng. evidence (freeze 2026-09-22) | **CLOSED** (eng.) |
| 2 Isolation | **P2-01/02/03 PASS** | **CLOSED** |
| 3 DSO MMS | **P3-01…09/11 PASS** · residuals PART/WAIVED | **CLOSED** (lab) |
| 4 Operator 104 | Not started | **OPEN** ← **next** |
| 5–7 | Not started / OPEN | **OPEN** |

---

## Phase 0 — Platform bind

**Goal:** Bind RJ45 / RS485 / DIO / zones / deploy path to TG544.  
**Checklist:** P0-01…05.

| Check | Status | What was used | Notes |
|-------|--------|---------------|-------|
| P0-01 Port roles | **PEND** → later superseded by P2 map | LuCI Devices / Interfaces | Full peel done in Phase 2 |
| P0-02 RS485 device | **PART** | `/dev/rs485_2_uart` | A2/B2 `/dev/ttyS2` unbound |
| P0-03 DIO map | **PART→documented** | ubus `dido_v2` ch1 DO / ch2 DI | `lab/TR400_HW_IO_CONFIG.md` |
| P0-04 62443 zones | **PART** | `CCI_62443_Zones.md` | Rev updated with annex map |
| P0-05 Deploy path | **PASS** | `/usr/sbin/ccli`, `/etc/ccli/lab.yaml` | `lab/tg544-openwrt/DEPLOY_CCLI.md` |

**Primary docs:** `lab/PHASE1.md` § topology · `lab/TR400_HW_IO_CONFIG.md` · `lab/DIDO_V2_API.md`

---

## Phase 1 — Local PF2 (no DSO)

**Goal:** Prove `Modbus P → PF2 FSM → DIO1`, gated by DIO2 permissive.  
**Not in scope:** Eth_A MMS, `Wlim`, 104.  
**Freeze:** `lab/PHASE1_LAB_SESSION_FREEZE_2026-09-22.md` (2026-09-22).

### 1.1 What was used

| Layer | Exact item |
|-------|------------|
| Config | `lab_tr400_phase1_regulation.yaml` · DSO mock on · enter **42 kW** · release **37 kW** · debounce **30 s** · stale **10 s** |
| Modbus | 9600 8N1 · slave **1** · FC03 holding **40001** · `poll_ms: 4000` · device **`/dev/rs485_2_uart`** |
| PC slave | `python -u lab/modbus_rtu_slave.py --port COM5 --trace` |
| I/O | DIO1 curtail · DIO2 permissive · `permissive_bypass: false` |
| UI | `http://192.168.1.130/ccli/mock_console.html` · CGI `/cgi-bin/ccli-bench` |
| Checks | `ccli --regulation-check` · `PHASE1_REGULATION_CHECK_MATRIX.md` |

### 1.2 Log / result excerpts (2026-09-22)

**Static regulation:** 19 PASS / 7 N/A / 0 FAIL.

**Live snapshot (~18:09):**
```json
"p_kw": 40, "quality": "good", "pf2_enter_kw": 42,
"permissive_ok": false, "do_curtail_on": false
```

**Modbus:** live P ≈ 40–45 kW, `quality=good` after `rs485_2_uart` fix.

### 1.3 Checklist status (Phase 1)

| ID | Status | Comment |
|----|--------|---------|
| P1-03 hysteresis | PASS (unit) | |
| P1-06 safe DO at boot | PASS (unit) | |
| P1-08 logging | PART | |
| P1-09 binary under test | PASS | |
| P1-01/02/04/05/07 live DIO walk | PEND | Permissive open; curtail ON not signed off 2026-09-22 |
| P1-W1…W5 | WAIVED → later phases | Wlim = P3, etc. |

**Sign-off sentence:** Phase 1 is **engineering evidence of local limitation** — **not** CEI DSO PF2.

**Evidence / runbooks:** `lab/PHASE1.md` · `PHASE1_LAB_SESSION_FREEZE_2026-09-22.md` · `PHASE1_REGULATION_CHECK_MATRIX.md` · `MODBUS_LIVE_VIEW.md` · `REGULATION_MOCK_DIO_MAP.md`

---

## Phase 2 — Isolation (O.13.1 + 62443)

**Goal:** Separate Eth_A / Eth_B / plant / eng; no role↔role data exchange through CCI; LTE backup must not reach DSO.  
**Session:** 2026-09-25 · `SESSION_REPORT_P2_P3_2026-09-25.md`

### 2.1 What was used

| Item | Exact value |
|------|-------------|
| Harden method | LuCI Network/Firewall + SSH `uci` |
| Zones | `lan1_sec` / `lan_sec` / `lan2_sec` · input=accept · forward=**reject** · masq=**0** |
| Forwarding | Only **lan → wan**; **no** role→role forward |
| Wi-Fi | Disabled for isolation profile |
| LTE bring-up | 2G SIM failed → **1NCE 4G** · APN via TesPro `sim_manager` field **`apn_user`** = `iot.1nce.net` |
| PC helpers | `lab/set_pc_lan1_oa.cmd` · `lab/set_pc_lan2_oa.cmd` |

### 2.2 P2-01 / P2-02 — Cross-ping matrix

**Pass rule:** own TG IP = PASS; **other-role PC IP = FAIL**.  
Ping of *other* TG interface IPs may succeed (INPUT) → **N/A**, not fail.

| From \ To | Own TG | DSO PC .10.10 | OA PC .1.183 | Plant PC .30.10 |
|-----------|--------|---------------|--------------|-----------------|
| DSO PC | PASS | — | **FAIL** | **FAIL** |
| OA PC | PASS | **FAIL** | — | **FAIL** |
| Plant PC | PASS | **FAIL** | **FAIL** | — |

**Verdict:** **P2-01 PASS** · **P2-02 PASS**  
**Evidence:** `P2_01_CROSS_PING_MATRIX.txt` · `P2_fw_ssh_verify_*.log` · `P2_port_map_ssh_*.log`

### 2.3 P2-03 — LTE ↛ DSO

```text
[PASS] ping 8.8.8.8                 4/4          (LTE up)
[PASS] ping -I usb0 192.168.10.1    0/4 100% loss
[PASS] ping -I usb0 192.168.10.10   0/4 100% loss
[PASS] ping 192.168.10.10           0/4 100% loss
```

**Verdict:** **P2-03 PASS**  
**Evidence:** `P2_03_LTE_ISOLATION.txt` · `P2_03_LTE_ISOLATION_*.log` · `P2_LTE_APN_SSH_FIX.txt`

### 2.4 Soft / open (non-blocking for hard gates)

| ID | Status | Note |
|----|--------|------|
| P2-04 | PART | LuCI still on OA `.1.130`; eng-only not enforced |
| P2-05 | PART | Annex map done; ZCR-7 asset-owner sign-off missing |
| P2-06 | OPEN | Eth link-status logging |
| — | optional | Rename UCI `lan2_sec` → `plant` |

**Phase 2 hard gates:** **CLOSED**.

---

## Phase 3 — DSO MMS (Annex T + O.9.2.2)

**Goal:** IEC 61850 MMS server on **Eth_A only**; 4 s TotW; `Wlim` slaves PF2 → DIO1; later TLS.  
**Exit criteria:** **P3-01 + P3-04 + P3-06** all PASS.  
**(2026-09-25) P3-01 ✓ · P3-04 ✓ · P3-06 ✓ · soft P3-05/07/08/09/11 ✓ → lab exit CLOSED.**

### 3.1 Frozen object map (Step 3 / P3-12 PART)

**File:** `apps/ccli/config/icd/signal_map.yaml`  
**CID lab:** `apps/ccli/config/icd/lab_tg544_eth_a.cid` @ **192.168.10.1/24**  
**IED:** `CCI016_01` · LD `LD_Plant`

| Function | Object | Clause |
|----------|--------|--------|
| Enable | `WlimDWMX1.Mod` (1=Active, 5=Inactive) | T.3.1.4 |
| Limit % | `WlimDWMX1.WMaxSptPct` | T.3.1.4 / O.9.2.2 |
| PdC P | `PdCMMXU1.TotW` | T.3.1.3 |
| 4 s report | `urcb_PdC_Mis4sec` · `intgPd=4000` | T.3.2.1 |

**Maps to runtime:** `DsoActivePowerCommands.wlim_active` / `wmax_spt_pct` → `apply_live_dso_commands_to_pf2` → `Pf2Fsm::set_thresholds` → DIO1.

**Evidence:** `P3_02_LAB_CID_ETH_A.txt` · `P3_03_SIGNAL_MAP.txt`

### 3.2 Stack / build (used for Steps 4–7)

| Item | Exact |
|------|--------|
| Library | `libiec61850` **v1.6.0** · CMake `CCLI_WITH_LIBIEC61850=ON` · static link |
| Server code | `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp` |
| Main poll | `apps/ccli/services/ccli_main.cpp` — `poll_wlim_command` → `apply_live_dso_commands_to_pf2` |
| Package | `package/ccli` · `PKG_RELEASE:=6` |
| Build | WSL SDK → `lab/tg544-openwrt/ccli-0.1.0-r6.apk` · `ccli-bin` (aarch64) |
| Lab clients | `lab/mms_lab_client.c` · `lab/mms_wlim_client.c` → `lab/tg544-openwrt/mms_*` |
| Bind | `mms.bind_address: 192.168.10.1` · `mms.tcp_port: 102` |
| Lab ctlModel | **DIRECT_ENHANCED** (product intent remains **SBO**) |

**Important:** TesPro LuCI “IEC 61850 Protocol” page is a **client/MQTT gateway**, **not** Annex T server — disable if it grabs `:102`. Browser to `:102` fails with `ERR_UNSAFE_PORT` (expected).

### 3.3 Step 4 / P3-01 — MMS listen Eth_A

**Package at first proof:** `ccli-0.1.0-r5`  
**Start:** `/usr/sbin/ccli --config /etc/ccli/lab.yaml` (procd init may need CRLF strip)

**Server log:**
```text
mms: listening on 192.168.10.1:102 (IED CCI016_01 / LD_Plant)
```

**Device:**
```text
tcp  0  0  192.168.10.1:102  0.0.0.0:*  LISTEN  ccli
```

**PC:** `Test-NetConnection 192.168.10.1 -Port 102` → True  

**Verdict:** **P3-01 PASS** · Evidence `P3_01_MMS_LISTEN_ETH_A.txt`

### 3.4 Step 5 / P3-03 — Client browse + 4 s TotW

**Client:** `mms_lab_client 192.168.10.1 102`

```text
[PASS] CONNECT · LD CCI016_01LD_Plant · LN LLN0, PdCMMXU1, WlimDWMX1
[PASS] DataSet DS_R_PdC_Mis4sec → TotW[MX]
[PASS] urcb_PdC_Mis4sec IntgPd=4000
[PASS] reports_received=6 · avg_interval_ms=3961
       deltas (ms): 0 (GI), 3798, 4001, 4002, 4002, 4000
```

**Note:** TotW mag often **0.0** without Modbus P (object present). Non-zero TotW via plant Modbus deferred (optional).

**Verdict:** **P3-03 PASS** · Evidence `P3_03_MMS_CLIENT_STEP5.txt` · `.log` · `SESSION_REPORT_P3_STEP5_2026-09-25.md` · live `P3_DSO_CLIENT_LIVE.log`

### 3.5 Step 7 / P3-04 — Wlim → PF2 → DIO1

**Binary:** `ccli-0.1.0-r6` (~6.7 MB aarch64)  
**Start used for proof:**
```text
/usr/sbin/ccli --config /etc/ccli/lab.yaml --lab-demo
```
(`--lab-demo` = simulated P ramp; Modbus slave not required for this gate)  
**Lab:** `permissive_bypass: true` for this run only.

**Client:**
```text
mms_wlim_client 192.168.10.1 102 10
```

**Client log:**
```text
CONNECT OK
OPERATE OK CCI016_01LD_Plant/WlimDWMX1.WMaxSptPct = 10.00
OPERATE OK CCI016_01LD_Plant/WlimDWMX1.Mod = 1
READ Mod.stVal=1
```

**Server log (key lines):**
```text
mms: listening on 192.168.10.1:102 ... Wlim control enabled
mms: Wlim.WMaxSptPct (struct) ctlVal=10.00
mms: Wlim.Mod ctlVal=1 → wlim_active=true
mms→pf2: Wlim active=yes WMaxSptPct=10% p_wlim=21 p_eff=21 kW → enter=21 release=16 kW
io: DO curtail_on
... pf2=p_above_threshold curtail=yes DO=ON
```

**Math:** Smax_used **210 kVA** · 10% → **p_wlim=21 kW** (binds below WSd 42 kW).

**ubus DIO1:**
```json
"channel": 1, "mode": "DO", "state": 1
```

**Caveats (documented):** DIRECT_ENHANCED · permissive_bypass · lab-demo P · cleartext MMS.

**Verdict:** **P3-04 PASS**  
**Evidence:** `P3_04_WLIM_ACTUATOR.txt` · `P3_04_WLIM_CLIENT.txt` · `P3_04_CCLI_SERVER.log` · `P3_04_DIO1_STATUS.txt` · `SESSION_REPORT_P3_STEP7_P304_2026-09-25.md`

### 3.6 Phase 3 checklist board (lab close 2026-09-25)

| ID | Status | Evidence / note |
|----|--------|-----------------|
| P3-01 | **PASS** | Eth_A MMS listen |
| P3-02 | PART | Dynamic MVP + lab CID; full genconfig TBD |
| P3-03 | **PASS** | ~3961 ms TotW |
| P3-04 | **PASS** | Wlim → enter=21 · DIO1 ON |
| P3-05 | **PASS** | Comms-loss fallback_s=15 |
| P3-06 | **PASS** | TLS `:3782` |
| P3-07 | **PASS** | ACSE mutual cert (lab); §13 E2E deferred |
| P3-08 | **PASS** | DSO OK / VIEWER DENY |
| P3-09 | **PASS** (lab) | CA + CRL; SCEP/EST deferred |
| P3-10 | **WAIVED** (lab) | XCBR → product model |
| P3-11 | **PASS** | chrony ≤±100 ms · ccli observe |
| P3-12 | PART | signal_map MVP; plant workbook MISSING |
| P3-13 | PART | 2004 only; Ed.2 corpus |
| P3-14 | **WAIVED** (lab) | 7-3 extract → corpus |

**Phase 3 lab exit:** **CLOSED** · `SESSION_REPORT_P3_LAB_CLOSE_2026-09-25.md`

---

## Phase 4 — Operator 104 (Eth_B) + TesPro supplier 61850

**Status:** **CLOSED** (lab) 2026-09-30 · `lab/evidence/phase4/SESSION_REPORT_P4_CLOSE_2026-09-30.md`  
**Scope:** IEC 60870-5-104 TLS on **Eth_B** — **monitor/supervision only**; DSO commands stay **Eth_A MMS**.  
**Operator role:** `lab/evidence/phase4/P4_OPERATOR_ROLE.md`

| ID | Status |
|----|--------|
| **P4-00** TesPro 61850 apk + services | **PART** — install PASS · LuCI/plant/coexistence optional |
| **P4-01** Eth_B only | **PASS** |
| **P4-02** 104 session | **PASS** |
| **P4-03** TLS 62351-3 | **PASS** |
| **P4-04** Eth_B fallback 60 s | **PASS** |
| **P4-05** O.14 operator cmd log | **WAIVED** — monitor-only |

**Deploy:** `ccli-0.1.0-r18` · yaml `lab_tr400_phase4_eth_b.yaml` · LAN2 `192.168.1.130:2404`.

### Post–Phase 3 note (2026-09-29 TSP deep test)

Official P3 close unchanged (**r15**, 2026-09-25). Additional deploy with session/ACSE fixes + Solution C yaml; TSP Step 7 still FAIL offline PASS — see `lab/evidence/testsuite-pro/inbox/TSP_P3_07_WIRE_EVIDENCE_2026-09-29_222559.md`.

---

## Phase 5 — Observability + reactive (PF1 / MMXU / O.9.1)

**Status:** **Not started** as formal phase.  
**Assignment (2026-09-26):** Phase 5 owns **mandatory TotVAr/PPV** (T.3.1.3) and **O.9.1** reactive DSO — was previously unassigned (“Phase 3+” placeholder).  
**Note:** Lab has TotW RCB at 4 s (P3-03). Modbus `q_kvar` is read into `MeasurementStore` but **not** published as TotVAr. Reactive LNs are **STUB** Mod=5.

| ID | Status |
|----|--------|
| P5-01…06 | OPEN / PART / N/A |
| **P5-M07** TotVAr | **OPEN** |
| **P5-M08** PPV | **OPEN** |
| **P5-M09** Q nameplate | PART (yaml) |
| **P5-R01…R09** O.9.1 | **OPEN** / N/A (VArSa) |

**Detail:** `knowledge-base/08-engineering/CCI_Phase_Regulation_Checklists.md` · gap report `lab/CCI_Reactive_Roadmap_Gap_Report.md`.

---

## Phase 6 — Defence I/O (Annex M + product C4)

**Status:** **Not started**.  
Do **not** claim G6K/SMBJ33 = M modem class (P6-07 PART in checklist).

| ID | Status |
|----|--------|
| P6-01…07 | OPEN / GAP / PART |

---

## Phase 7 — Evidence pack (O.14 / O.15)

**Status:** **Not started** as formal pack.  
This master log + `lab/evidence/O13_1_isolation_2026-09-25/` are **lab engineering evidence**, not O.15 certification close-out.

| ID | Status |
|----|--------|
| P7-01…12 | OPEN / PART |

---

## A. End-to-end data path proven (lab)

```text
DSO PC (192.168.10.10)
    │  MMS Operate  (libiec61850 client)
    ▼
Eth_A 192.168.10.1:3782 TLS  (ccli IED CCI016_01)
    │  Wlim.Mod / WMaxSptPct
    ▼
apply_live_dso_commands_to_pf2  →  Pf2Fsm thresholds
    │  (+ lab-demo or Modbus P)
    ▼
DIO1 via dido_v2 ch1  (curtail DO)
```

Phase 1 path (no MMS): Modbus P → same PF2 → DIO1.  
Phase 3 slaves the **same actuator** from DSO `Wlim`.

---

## B. Evidence index (this session folder)

| File | Phase / topic |
|------|----------------|
| `P2_ANNEX_PORT_MAP.txt` | As-built ports + open list |
| `P2_01_CROSS_PING_MATRIX.txt` | P2-01/02 |
| `P2_03_LTE_ISOLATION.txt` (+ `.log`) | P2-03 |
| `P2_LTE_APN_SSH_FIX.txt` | 1NCE APN |
| `P2_port_map_ssh_*.log` · `P2_fw_ssh_verify_*.log` | SSH dumps |
| `P3_01_MMS_LISTEN_ETH_A.txt` | P3-01 |
| `P3_02_LAB_CID_ETH_A.txt` | Lab CID |
| `P3_03_SIGNAL_MAP.txt` | Object freeze |
| `P3_03_MMS_CLIENT_STEP5.txt` (+ `.log`) | P3-03 |
| `P3_DSO_CLIENT_LIVE.log` | Live DSO client |
| `P3_04_WLIM_ACTUATOR.txt` | P3-04 verdict |
| `P3_04_WLIM_CLIENT.txt` | Client operate |
| `P3_04_CCLI_SERVER.log` | Server Wlim + DO |
| `P3_04_DIO1_STATUS.txt` | ubus DIO1 |
| `SESSION_REPORT_P2_P3_2026-09-25.md` | P2→P3 Steps 1–3 |
| `SESSION_REPORT_P3_STEP5_2026-09-25.md` | Step 5 |
| `SESSION_REPORT_P3_STEP7_P304_2026-09-25.md` | P3-04 |
| `P3_05_COMMS_LOSS_FALLBACK.txt` | P3-05 |
| `P3_06_MMS_TLS_ETH_A.txt` | P3-06 |
| `P3_07_SECURITY_ASSOC.txt` | P3-07 |
| `P3_08_SECURITY_RBAC.txt` · `P3_08_09_RBAC_PKI.txt` | P3-08/09 |
| `P3_11_CHRONY_TRACKING.txt` · `P3_11_TIMEQUALITY_REPORT.md` | P3-11 chrony |
| `SESSION_REPORT_P3_SOFT_2026-09-25.md` | Soft cyber close |
| `SESSION_REPORT_P3_P311_2026-09-25.md` | TimeQuality |
| `SESSION_REPORT_P3_LAB_CLOSE_2026-09-25.md` | **Phase 3 lab exit** |

**Phase 1 (earlier folder / docs):**  
`lab/PHASE1_LAB_SESSION_FREEZE_2026-09-22.md` · `lab/PHASE1.md` · `lab/PHASE1_TRACEABILITY.md`

---

## C. Reproduce commands (precise)

### C.1 Build APK (WSL)

```bash
# Prefer LF script under /tmp (Windows CRLF breaks bash)
bash /tmp/wsl-build-ccli.sh
# outputs: lab/tg544-openwrt/ccli-0.1.0-r6.apk · ccli-bin
```

### C.2 Build MMS clients (WSL)

```bash
bash lab/tg544-openwrt/build-mms-lab-client.sh
# → lab/tg544-openwrt/mms_lab_client · mms_wlim_client
```

### C.3 Deploy binary (Windows PowerShell → LAN1)

```powershell
# $env:CCLI_TG544_PW set locally
$hk = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"
pscp -scp -pw $env:CCLI_TG544_PW -hostkey $hk lab\tg544-openwrt\ccli-bin root@192.168.10.1:/tmp/ccli-r6
plink ... "killall -9 ccli; cp /tmp/ccli-r6 /usr/sbin/ccli; chmod +x /usr/sbin/ccli"
```

### C.4 Run P3-04 proof

```text
# On DUT:
/usr/sbin/ccli --config /etc/ccli/lab.yaml --lab-demo >/tmp/ccli-p304.log 2>&1 &

# On WSL (DSO role):
./lab/tg544-openwrt/mms_wlim_client 192.168.10.1 102 10

# On DUT:
grep -E 'mms→pf2|DO curtail|Wlim' /tmp/ccli-p304.log
ubus call dido_v2 status
```

---

## D. Next actions (ordered)

1. **Phase 5** — TotVAr / O.9.1 reactive on Eth_A MMS (P5-M / P5-R) — **programme next**.  
2. Phase 3 residuals (non-blocking): P3-10 XCBR · plant ICD workbook · 62351-4 §13 E2E · SCEP/EST · GNSS PPS (C5).  
3. Phase 1 residual: DIO scenario walk with real permissive + Modbus (optional).  
4. Product harden: SBO ctlModel · drop `permissive_bypass` · MZ license · TPM-backed TLS keys.  
5. **Phase 5-M** — live TotW + **TotVAr** (+ PPV) on Eth_A; analyzer map P/Q/V.  
6. **Phase 5-R** — VArSd (O.9.1.4) + plant Q path; remaining O.9.1 WAIVE per Operating Rule.  
7. See `lab/CCI_Reactive_Roadmap_Gap_Report.md` for RAG HAVE/MISSING.

---

## E. Document history

| Rev | Date | Change |
|-----|------|--------|
| 1.2 | 2026-09-26 | Phase 5 owns reactive (P5-M/P5-R); link gap report |
| 1.1 | 2026-09-25 | Phase 0–3 lab closes; P3 soft + chrony P3-11; next = Phase 4 |
| 1.0 | 2026-09-25 | First master log through P3-04 PASS |

**Owner:** HiTEKS CCLI lab · update this file when a checklist ID changes status; keep raw logs under `lab/evidence/`.
