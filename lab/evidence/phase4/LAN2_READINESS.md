# LAN2 readiness pack — Eth_B wire day (after LAN1 exit)

**Date:** 2026-10-05  
**DUT:** TG544 · target build **ccli 0.1.0-r30+**  
**Product HMI:** **Cloud UI** — see [CCI_Cloud_Operator_HMI.md](../../../knowledge-base/08-engineering/CCI_Cloud_Operator_HMI.md)  
**Phase 4 protocol:** **CLOSED** 2026-09-30 · this pack = **r30 regression + O.14 operator events**

---

## What LAN2 is for (and is not)

| In scope on LAN2 | Out of scope (cloud / other) |
|------------------|------------------------------|
| IEC 60870-5-104 TLS `:2404` on `192.168.1.130` | **Operator web HMI** → HiTEKS **cloud** |
| Direct **SCADA / aggregator** 104 clients | DSO MMS (LAN1 Eth_A) |
| O.14 `iec104` / `operator_asdu_*` events | Local REGALGRID-style LCD panel |
| r30 regression vs P4 evidence (r18–r19) | Annex M teletrip (LTE/SMS) |

Lab-only: `http://192.168.1.130/ccli/mock_console.html` — **not** product HMI ([REGULATION_MOCK_UI.md](../../REGULATION_MOCK_UI.md)).

---

## Prepare now (no LAN2 cable — do during LAN1 work)

| # | Task | Owner | Artifact |
|---|------|-------|----------|
| 1 | Confirm deploy yaml keeps `iec104.bind_address: 192.168.1.130` | Firmware | `lab_tr400_cleartext_tsp_ttyS1.yaml` |
| 2 | Plan enable `operator_monitor_totvar: true` (+ PPV when P5-M stable) | Firmware | `iec104_operator_rule.yaml` |
| 3 | Write cloud northbound schema draft (telemetry fields) | Cloud + firmware | `CCI_Cloud_Operator_HMI.md` |
| 4 | Keep scripts ready | Lab | `verify-lan2-eth-b.ps1`, `run-p4-03-tls-capture.ps1`, `run-p4-05-operator-rule-test.ps1` |
| 5 | P7-04 matrix: define PASS for `operator_asdu_reject` | Lab | `P7-04_EVENT_COVERAGE_MATRIX.md` |
| 6 | Decision log: monitor-only 104 vs future MSD commands | Product | `P4_OPERATOR_ROLE.md` |

**Sanity on LAN1 deploy (no cable):** DUT log should show `iec104: listening 192.168.1.130:2404` and P7 dump may show `iec104 comms_loss_fallback` — server alive, no client.

---

## Wire day checklist (LAN2 connected)

### Step 0 — PC + link

```cmd
lab\set_pc_lan2_oa.cmd
ping 192.168.1.130
```

```powershell
$env:CCLI_TG544_PW = '<pw>'
.\lab\tg544-openwrt\verify-lan2-eth-b.ps1
```

**PASS:** PC on `192.168.1.0/24` · ping/SSH OK · `:2404` on `.1.130` only · ccli r30+

### Step 1 — 104 TLS session (replay P4-02/03)

```powershell
.\lab\tg544-openwrt\run-p4-03-tls-capture.ps1
```

**PASS:** TLS handshake · GI · M_ME_NC_1 TotW ≈ plant P

### Step 2 — Monitor points (post–P5-M)

Deploy yaml with:

```yaml
operator_monitor_totvar: true
operator_monitor_ppv: false   # enable when P5-M08 ready
```

Re-run 104 client · read IOA **1001** TotW · **1002** TotVAr

### Step 3 — O.14 operator reject (P7-04)

Use monitor-only profile or send forbidden command ASDU:

```powershell
.\lab\tg544-openwrt\run-p4-05-operator-rule-test.ps1
```

On DUT:

```text
ccli --config /etc/ccli/lab.yaml --event-dump --count 20 | grep operator
```

**PASS:** line `iec104 operator_asdu_reject` (or accept if command profile)

### Step 4 — Evidence commit

Save as:

```text
lab/evidence/phase4/P4_LAN2_R30_REGRESSION_YYYY-MM-DD.txt
lab/evidence/phase4/P4_03_104_TLS_r30.pcapng   (if captured)
```

Update P7-04 matrix: External operator comms → **HAVE** (lab r30).

### Step 5 — Optional (not product HMI)

Browse `http://192.168.1.130/ccli/mock_console.html` — confirm lab tool only; note gaps vs [REGALGRID cloud screen map](../../../knowledge-base/08-engineering/CCI_Cloud_Operator_HMI.md).

### Step 6 — Return to LAN1

```cmd
lab\set_pc_lan1_dso.cmd
```

---

## Cloud vs LAN2 — operator roles

```text
Human operator  →  Cloud UI (LTE/HTTPS)     REQ-OP-CLOUD-001
SCADA machine   →  LAN2 104 TLS :2404       REQ-OP-CLOUD-005
DSO             →  LAN1 MMS :3782            Annex T (never cloud)
```

Both cloud and 104 can coexist; neither replaces DSO Eth_A.

---

## Exit criteria (LAN2 wire day)

| ID | Criterion |
|----|-----------|
| L2-01 | r30 104 TLS session PASS |
| L2-02 | TotW (+ TotVAr if enabled) match Modbus plant |
| L2-03 | `operator_asdu_*` in `--event-dump` |
| L2-04 | No `:2404` listener on `192.168.10.1` |
| L2-05 | Evidence file committed |

---

## Related

| Doc | Purpose |
|-----|---------|
| [P4_01_LAN2_CONFIGURE.md](P4_01_LAN2_CONFIGURE.md) | Cable map + PC IP |
| [P4_OPERATOR_ROLE.md](P4_OPERATOR_ROLE.md) | OA vs DSO |
| [SESSION_REPORT_P4_CLOSE_2026-09-30.md](SESSION_REPORT_P4_CLOSE_2026-09-30.md) | Original P4 close |
| [CCI_Cloud_Operator_HMI.md](../../../knowledge-base/08-engineering/CCI_Cloud_Operator_HMI.md) | Cloud HMI architecture |

---

*Run this pack once after LAN1 exit (P7-11 + P7-01). Cloud northbound is a separate stream.*
