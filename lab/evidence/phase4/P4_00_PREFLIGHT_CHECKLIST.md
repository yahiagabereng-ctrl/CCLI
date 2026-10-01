# Phase 4 preflight checklist

Run before first 104 bind on DUT.

## P4-00 — TesPro supplier IEC 61850 (prerequisite)

Install per manual §1.0 — **apk upload order** on LuCI Device Online Management → Software Packages:

1. `libopen62541-1.3.6-r2.apk`
2. `libiec61850-1.6.2-r1.apk` (or supplier revision for your FW)
3. `iec61850-mmsd-1.0.0-r3.apk`
4. `iec61850-proto-tespro-combined-1.0.0-r6.apk`

- [x] All four packages installed (**PASS** 2026-09-30 — see `P4_00_TESPRO_61850_SSH_VERIFY.txt`)
- [x] `iec61850-mmsd` + `iec61850d` running
- [ ] LuCI **Services → IEC 61850 Protocol** → **Running**
- [ ] Optional: plant IED on LAN3 configured (Host `:102`, plant ICD)
- [ ] Coexistence: `ccli` on `:3782` + TesPro both up (V-005)

**Verify:** `.\lab\tg544-openwrt\check-tespro-61850-remote.ps1`  
**Runbook:** `P4_00_TESPRO_61850_SUPPLIER_SW.md`

## Phase 3 still valid?

- [ ] `ccli` on DUT · MMS `192.168.10.1:3782` LISTEN (LAN1)
- [ ] Last known deploy: Solution C yaml OR regulation yaml documented
- [ ] Phase 3 close report read: `SESSION_REPORT_P3_LAB_CLOSE_2026-09-25.md`

## Phase 2 isolation still valid?

- [ ] `P2_ANNEX_PORT_MAP.txt` — LAN2 = Eth_B `192.168.1.130`
- [ ] No bridge Eth_A ↔ Eth_B (P2-01 matrix)

## Phase 4 network

- [ ] Operator PC cable on **LAN2** (for 104 client tests)
- [ ] PC IP on `192.168.1.0/24` (DHCP or static)
- [ ] Ping `192.168.1.130` OK
- [ ] SSH works: `root@192.168.1.130` (or `192.168.10.1` from LAN1 for deploy)

## Software

- [ ] `iec104_adapter` implementation merged (currently stub)
- [ ] lib60870 linked in build
- [ ] Lab yaml has `iec104:` section with `bind_address: 192.168.1.130`

## Corpus (no new ingest required)

- [ ] `CCI_60870-5-104_Extract.md` — HAVE
- [ ] `CCI_62351-3_Extract.md` — HAVE (TLS phase)
- [ ] `CCI_62351-5_Extract.md` — HAVE (optional STAS)
