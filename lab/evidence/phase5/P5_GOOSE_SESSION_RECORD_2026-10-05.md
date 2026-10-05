# P5-G02 GOOSE session record — resume from here

**Session end:** 2026-10-05  
**Gate closed:** **P5-G02** (GOOSE publish) — **PASS**  
**Next gate:** **P5-G01** (GOOSE subscribe) — not started  
**Bench today:** LAN1 + RS485 only (LAN3 unplugged; wire evidence already captured)

---

## Resume checklist (start next session here)

| # | Action | Notes |
|---|--------|-------|
| 1 | Connect **LAN1** PC ↔ DUT | PC `192.168.10.10/24` → DUT `192.168.10.1` |
| 2 | Start Modbus slave | `python -u lab/modbus_rtu_slave.py --port COM5 --power-kw 450 --trace` |
| 3 | Confirm ccli running | SSH: `pgrep -a ccli`; log: `GOOSE publishing enabled on lan3` |
| 4 | Quick GoCB check | WSL: `lab/tg544-openwrt/mms_goena_client 192.168.10.1 102` → GoEna=1 |
| 5 | **Next work:** P5-G01 | Reconnect **LAN3** when subscribe test needed |

**Do not change** `goose.interface: lan3` — correct for TesproOS TR544.

---

## Programme context

| Item | Value |
|------|-------|
| Product | HiTEKS CCLI PF2 / CEI 0-16 CCI |
| Platform | TesPro TG544 (OpenWrt 25.12, `platform_tg500`) |
| DUT build | **ccli 0.1.0-r29** |
| CID | `apps/ccli/config/icd/lab_tg544_eth_a.cid` **rev 5** |
| Lab yaml | `apps/ccli/config/lab_tr400_cleartext_tsp_ttyS1.yaml` → `/etc/ccli/lab.yaml` |
| Model cfg | `/etc/ccli/icd/lab_tg544_eth_a.cfg` (must contain `GC(gcb_...)` blocks) |

Prior P5 gate (MMS / Modbus / reactive): **CLOSED** 2026-10-01 — see [P5_FINAL_CLOSEOUT.md](P5_FINAL_CLOSEOUT.md).

---

## Lab topology (validated on TR544)

| Port | CCI role | TesproOS | IP | Protocol |
|------|----------|----------|-----|----------|
| **LAN1** | Eth_A DSO | `lan1_sec` / `lan1` | 192.168.10.1/24 | MMS `:102` cleartext |
| **LAN2** | Eth_B Operator | `lan_sec` / `lan2` | 192.168.1.130/24 | IEC 60870-5-104 |
| **LAN3** | Plant | `lan2_sec` / `lan3` | 192.168.30.1/24 | **GOOSE L2** |
| RS485 A1/B1 | Plant RTU | `/dev/ttyS1` | — | Modbus RTU ← PC **COM5** |
| **br-lan** | — | empty bridge | 192.168.0.1 (no carrier) | **Not used for GOOSE** |

PC setup scripts:
- DSO: `lab/set_pc_lan1_dso.cmd` → 192.168.10.10
- Plant: `lab/set_pc_lan3_plant.cmd` → 192.168.30.10

---

## Objective (P5-G02)

Prove integrated **GOOSE publish** from DUT `IedServer`:

1. GoCB defined in CID + `.cfg`
2. MMS `GetGoCBValues` / GoEna=1
3. Modbus → MMS dataset updates (~4 s)
4. L2 GOOSE frames on **plant LAN3**

**Result:** All four **PASS**.

---

## CID / GoCB summary

Two GoCBs on `LLN0`:

| GoCB | Dataset | GoID | APPID | Dst MAC |
|------|---------|------|-------|---------|
| `gcb_PdC_Mis4sec` | `DS_R_PdC_Mis4sec` (TotW, TotVAr, PPV, A) | PdCMis4 | 0x1000 | 01:0c:cd:01:00:01 |
| `gcb_Stato_Allarmi` | `DS_R_Stato_Allarmi_Segnali` (19 ST) | CCIAlrm | 0x1001 | 01:0c:cd:01:00:02 |

Cross-refs: `apps/ccli/config/plant/GOOSE_DATA_MAP.csv` rows 68–90 · `signal_map.yaml` `goose_map`

---

## Configuration (runtime)

```yaml
# apps/ccli/config/lab_tr400_cleartext_tsp_ttyS1.yaml
goose:
  enabled: false          # subscribe path — P5-G01
  publish_enabled: true   # integrated IedServer publisher
  interface: lan3         # NOT br-lan (empty on TR544)
```

MMS must load full model: `mms.model_cfg: /etc/ccli/icd/lab_tg544_eth_a.cfg`

---

## Software / code touched this session

| Artifact | Purpose |
|----------|---------|
| `apps/ccli/adapters/iec61850_mms/mms_adapter.cpp` | `enable_goose_publishing()` → `IedServer_enableGoosePublishing` on `lan3` |
| `lab/mms_goena_client.c` | **GetGoCBValues / SetGoCBValues** (fixed from wrong `$ST$/$CO$GoEna`) |
| `lab/tg544-openwrt/build-mms-lab-client.sh` | Builds `mms_goena_client` |
| `lab/tg544-openwrt/run-p5-goose-evidence.ps1` | Automated evidence script |
| `lab/set_pc_lan3_plant.cmd` | Plant NIC IP helper |
| `lab/GOOSE_ADAPTATION.md` | Protocol map + lab notes |

Build: `CCLI_WITH_GOOSE=ON` · `IedServer_enableGoosePublishing()` sets **GoEna=true** at boot.

**Correct MMS ACSI refs:**
```
CCI016_01LD_Plant/LLN0.gcb_PdC_Mis4sec
CCI016_01LD_Plant/LLN0.gcb_Stato_Allarmi
```

---

## Key findings / pitfalls

| Issue | Resolution |
|-------|------------|
| GOOSE on `br-lan` — no wire | TR544 `br-lan` has **no bridge members** → use **`lan3`** |
| GoCB client err 10 / 22 | Wrong FC (`$ST$`/`$CO$`) → use **GetGoCBValues** |
| `LD_Plant/LLN0.gcb_*` fails | MMS domain is **`CCI016_01LD_Plant`** |
| PC IP 192.168.30.1 | **Conflict** with DUT — use **192.168.30.10** |
| Wireshark empty on LAN1 | GOOSE egress is **LAN3**, not DSO port |
| GOOSE over RS485 | **Not possible** — Modbus is input only; GOOSE is Ethernet L2 |
| `goose: disabled in config` log | **Subscribe** path off — publish still active |

---

## Session timeline

| When | Event | Verdict |
|------|-------|---------|
| Early | CID audit, GoCB in `.cfg`, r29 deploy | Model OK |
| Mid | GoCB MMS via fixed `mms_goena_client` | PASS |
| Mid | Initial publish on `br-lan` — no carrier | FAIL wire |
| Fix | `goose.interface: lan3` deployed | TX counters rise |
| 13:00 | Wrong bench PC (192.168.1.183) | FAIL |
| 13:12 | LAN3 link up, Npcap pending | PART |
| 14:01 | PC/DUT IP conflict on .1 | FAIL |
| **14:08** | **20 s Wireshark on LAN3** | **PASS** |
| End | Evidence + pcap committed; LAN3 unplugged | Housekeeping done |

---

## Evidence index (committed)

| File | Content |
|------|---------|
| **[P5_G02_CLOSEOUT_2026-10-05.md](P5_G02_CLOSEOUT_2026-10-05.md)** | Gate closeout |
| **[P5_GOOSE_WIRE_2026-10-05.txt](P5_GOOSE_WIRE_2026-10-05.txt)** | Wire index + timeline |
| **`P5_GOOSE_WIRE_2026-10-05_140838.pcapng`** | **Canonical pcap** (7,893 pkts; 3947×0x1000, 3946×0x1001) |
| `P5_GOOSE_WIRE_2026-10-05_140838.txt` | Capture summary |
| `P5_GOOSE_WIRE_2026-10-05_140838_capture.log.txt` | tshark decode log |
| `P5_GOCB_MMS_2026-10-05.txt` | GetGoCBValues PASS |
| `P5_GOOSE_LAN3_CHECK_*.txt` | Bench troubleshooting |
| `P5_GOOSE_PUBLISH_2026-10-05_112808.txt` | Early publish attempt |
| `P5_NPCAP_IEDEX_CHECK_2026-10-05.txt` | Npcap / IED Explorer notes |

Wireshark filter: `goose && eth.dst == 01:0c:cd:01:00:01`

---

## Git commits (GOOSE thread)

```
aadea00 P5-G02 housekeeping: commit LAN3 Wireshark pcap and closeout docs.
44d0e22 P5-G02 wire PASS: publish GOOSE on lan3 (TesproOS plant port).
603c7da P5-G02 GOOSE publish closeout: GoCB MMS verified, wire deferred.
a2b0886 Add GOOSE plant integration, phase 6-7 lab evidence, and MMS/ICD mapping expansion.
```

---

## Phase gate status

| Gate | Status | Evidence |
|------|--------|----------|
| **P5-G02** Publish | **PASS** | pcap + GoCB MMS |
| **P5-G01** Subscribe | **OPEN** | needs `goose.enabled: true` + external publisher |
| **P5-G03** Merge policy | **OPEN** | Modbus vs GOOSE → MeasurementStore |
| P5-M / core P5 | **CLOSED** (2026-10-01) | P5_FINAL_CLOSEOUT |

Checklist: `knowledge-base/08-engineering/CCI_Phase_Regulation_Checklists.md`

---

## Bench state at session end

- **LAN1:** connected — MMS/SSH/Modbus lab OK
- **LAN3:** **disconnected** — no live GOOSE; evidence already in repo
- **Config on DUT:** keep `goose.interface: lan3` — no revert needed

---

## Next session — recommended order

1. **P5-G01 GOOSE subscribe** (reconnect LAN3)
   - Profile: `apps/ccli/config/lab_tr400_goose_plant.yaml` or enable `goose.enabled: true`
   - External GOOSE publisher on plant segment (or loopback test)
   - Pass: DUT log `goose: rx appId=… stNum=…` + O.14 event

2. **P5-G03** — document measurement merge (Modbus authoritative today)

3. **P5-R02/R03** — PFSP / VArV Operate (if DSO reactive path is priority)

4. Optional: `git push` remote

---

## Quick commands

```powershell
# DSO / MMS
$env:CCLI_TG544_PW = '000000'
ssh root@192.168.10.1 "grep GOOSE /tmp/ccli.log | tail -3"

# GoCB verify (WSL)
wsl bash -lc "/mnt/c/Yahia/projects/CCLI/lab/tg544-openwrt/mms_goena_client 192.168.10.1 102"

# When LAN3 reconnected
# Admin: lab\set_pc_lan3_plant.cmd
# Wireshark on plant NIC: goose && eth.dst == 01:0c:cd:01:00:01
```

---

*This document is the handoff anchor for GOOSE work after P5-G02. Update when P5-G01 starts.*
