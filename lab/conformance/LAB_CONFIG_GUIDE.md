# D6 — Lab configuration guide (DUT for UCA / TSP)

**Document ID:** CCLI-LAB-D6-001  
**Revision:** 1.1  
**Date:** 2026-10-09  
**Specimen pin:** [LAB_SPECIMEN_VERSION.md](LAB_SPECIMEN_VERSION.md) — CID History **rev 9** · `configRev` **20261009**
**RAG source_id:** `ccli-lab-config-guide`  
**Not** product HMI. Human operator UI is cloud (`CCI_Cloud_Operator_HMI.md`).

---

## Roles

| Actor | Wire | Address | Protocol |
|-------|------|---------|----------|
| DSO / Test Suite Pro | Eth_A LAN1 | DUT `192.168.10.1:3782` · PC `192.168.10.10` | MMS TLS |
| Operatore Abilitato | Eth_B LAN2 | DUT `192.168.1.130:2404` · PC `192.168.1.183` | 104 TLS, **monitor_only** |
| Plant | LAN3 / RS485 | DUT `192.168.30.1` / USB COM5 | Modbus / optional GOOSE |

No L2 bridge Eth_A ↔ Eth_B (Phase 2).

---

## Firmware identity (Table 1 sDoc)

On DUT:

```sh
ccli --version
# record SHA-256 of /usr/bin/ccli (or package name from deploy manifest)
```

Same string goes on PICS Cover and every `TSP_*` log header.

---

## Config files

| File | Use |
|------|-----|
| `/etc/ccli/lab.yaml` | Deployed from `lab_tr400_phase1_regulation.yaml` or `lab_tr400_phase4_eth_b.yaml` |
| CID / model | `lab_tg544_eth_a.cid` + `.cfg` — **rev 9** per [LAB_SPECIMEN_VERSION.md](LAB_SPECIMEN_VERSION.md) (cert freeze: `CID_PICS_ALIGNMENT.md`) |
| TLS | `/etc/ccli/tls/` — server_tls, server (ACSE), client, viewer, CRL |

Product MMS: `tls_enabled: true`, `tcp_port: 3782`, `bind_address: 192.168.10.1`, `comms_loss_fallback_s: 15`.

---

## Event log (O.14 / P7)

```sh
ccli --event-dump --count 20
ccli --event-wrap-test   # P7-01 2048 ring
ccli --event-clear       # must fail (O.14 no overwrite)
```

Store: `/var/lib/ccli/events.jsonl` (mode 0640, append). No user erase API.  
Syslog: UDP RFC 5424 to `event_log.syslog_host` (lab default `127.0.0.1:514`).

---

## PC scripts (repo)

| Script | Action |
|--------|--------|
| `lab/set_pc_lan1_dso.cmd` | PC LAN1 `192.168.10.10` |
| `lab/set_pc_lan2_oa.cmd` | PC LAN2 `192.168.1.183` |
| `scripts/stage-tsp-field-pack.ps1` | USB pack for TSP PC |

---

**RAG tags:** `lab`, `D6`, `guide`, `ccli-lab-config-guide`
