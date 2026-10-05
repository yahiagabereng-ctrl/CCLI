# GOOSE plant adaptation — HiTEKS CCLI on TG544

**Status:** IED-integrated GOOSE publisher (r28+) · subscribe path MVP · P5-G01 lab evidence **OPEN**  
**Normative:** CEI 0-16 Annex T (DSO = MMS/TLS); plant = Modbus and/or GOOSE per site  
**Competitor reference:** Higeco UCA cert = MMS only; Tesmec uses internal Report + GOOSE on plant bus

---

## Protocol map (one device, four domains)

| Domain | Interface | Protocol | Role | Phase gate |
|--------|-----------|----------|------|------------|
| **Eth_A** | `lan1_sec` / 192.168.10.x | IEC 61850 MMS + 62351-3 TLS `:3782` | DSO server | P3 **CLOSED** |
| **Eth_B** | `lan2_sec` / operator VLAN | IEC 60870-5-104 + TLS `:2404` | Operator monitor | P4 **CLOSED** |
| **Plant RTU** | RS485 A2/B2 `/dev/ttyS2` | Modbus RTU | P/Q from inverter/analyzer | P5-R **PASS** (lab) |
| **Plant LAN** | `br-lan` or dedicated `lan3` | IEC 61850 GOOSE (L2) | Optional subscribe to inverter IED | **P5-G01 OPEN** |

GOOSE is **not** a substitute for Modbus on the screw-terminal RS485 path. Sites may use **either** or **both** depending on inverter vendor.

---

## Configuration

YAML section `goose:` in `apps/ccli/core/config/ccli_config.hpp`:

| Key | Purpose |
|-----|---------|
| `enabled` | Start `GooseReceiver` on `interface` |
| `subscribe_go_cb_ref` | GoCB reference (required when enabled) |
| `subscribe_app_id` | Optional APPID filter (0 = any) |
| `subscribe_dst_mac` | Optional multicast MAC filter |
| `publish_*` | Reserved for P5-G02 publisher |

Lab profile: [`apps/ccli/config/lab_tr400_goose_plant.yaml`](../apps/ccli/config/lab_tr400_goose_plant.yaml) (`enabled: false` by default).

Plant CSV control plane (Berlin GOOSE matrix + assignment): [`apps/ccli/config/plant/READ.md`](../apps/ccli/config/plant/READ.md) — validate with `python scripts/plant-map-validate.py`.

Build flag: `CCLI_WITH_GOOSE=ON` (default) → `CONFIG_IEC61850_L2_GOOSE` in libiec61850.

---

## Runtime behaviour

### IED GOOSE sender (P5-G02 — primary for IED Explorer / TSP)

1. CID `lab_tg544_eth_a.cid` defines two GoCBs on `LLN0`:
   - `gcb_PdC_Mis4sec` → dataset `DS_R_PdC_Mis4sec` (TotW, TotVAr, PPV, A) · APPID **0x1000**
   - `gcb_Stato_Allarmi` → dataset `DS_R_Stato_Allarmi_Segnali` · APPID **0x1001**
2. Set `goose.publish_enabled: true` in lab yaml (MMS must load `model_cfg`).
3. `MmsAdapter::enable_goose_publishing()` calls libiec61850 `IedServer_enableGoosePublishing()` on `goose.interface` (default `br-lan`).
4. When `PdCMMXU1` values update (Modbus → MMS every 4 s), integrated publisher emits GOOSE if GoEna is true.
5. **IED Explorer:** connect to `192.168.10.1:102`, open `LLN0` → GoCB → enable / use **GooseSender**; Wireshark filter `goose && eth.addr == 01:0c:cd:01:00:01`.

Regenerate `.cfg` after CID edit: `powershell -File scripts/gen-mms-model-cfg.ps1`

### Plant GOOSE subscribe (optional)

1. `ccli_main` starts `GooseService` when `goose.enabled=true`.
2. `GooseAdapter` registers `GooseSubscriber` + listener on the plant interface.
3. Each received GOOSE frame logs to stderr and **O.14** `EventStore` as category `goose`.
4. **No** automatic merge into `MeasurementStore` yet — Modbus remains authoritative for P5-M TotW/TotVAr until P5-G03 mapping is defined.

---

## Lab verification (P5-G01)

| Step | Action | Pass |
|------|--------|------|
| 1 | PC or second IED publishes GOOSE on plant segment | Wireshark shows GOOSE ethertype |
| 2 | Set `goose.enabled=true` + matching `subscribe_go_cb_ref` | `goose: listening on …` in log |
| 3 | Trigger publisher stNum increment | stderr `goose: rx appId=… stNum=…` |
| 4 | `ccli --event-dump` | Row `goose` / `appId=… stNum=…` |

Evidence folder (when run): `lab/evidence/phase5/P5_GOOSE_RX_*.txt`

---

## Isolation (P2-G01)

| Rule | Check |
|------|-------|
| GOOSE RX only on plant interface | Not bound to Eth_A DSO address |
| No GOOSE → MMS bridge | DSO path unchanged |
| Firewall | Plant multicast must not forward to DSO zone |

---

## Backlog

| ID | Item |
|----|------|
| P5-G02 | ~~Publisher~~ **HAVE** (integrated IedServer) · lab evidence pending |
| P5-G03 | Map subscribed dataset → `MeasurementStore` / reactive path |
| P5-G04 | GOOSE timeout / comms-loss event (plant element comms O.14) |
| P7-04 | Extend matrix: plant GOOSE comms **HAVE** after P5-G01 |
