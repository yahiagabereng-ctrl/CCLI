# Phase 6 — Defence I/O (Annex M)

**Gate:** P6-01…P6-07 · **Closeout:** [P6_FINAL_CLOSEOUT.md](P6_FINAL_CLOSEOUT.md)  
**Status:** **CLOSED (lab)** 2026-10-02 · build **0.1.0-r26**

## Lab architecture

```text
1NCE MT-SMS TRIP/CLEAR → annex-m-sms-poller.sh → DIO3 → annex_m_trip_monitor
ccli O.11: annex_m_trip ACTIVE → inhibit DIO1 PF2 curtail
DIO1 = curtail · DIO2 = permissive · DIO3 = telescatto mock
```

## Checklist

| Check | Status | Evidence |
|-------|--------|----------|
| P6-01 | **PART** | [P6-01_LAB_ARCHITECTURE.md](P6-01_LAB_ARCHITECTURE.md) |
| P6-02 | **DEFERRED** | Relay photo — ubus trip verified |
| P6-03 | **PASS** | [P6-03_ANNEX_M_INHIBIT.md](P6-03_ANNEX_M_INHIBIT.md) |
| P6-04 | **PART** | [P6-04_TELEDISTACCO_SKETCH.md](P6-04_TELEDISTACCO_SKETCH.md) |
| P6-05 | **PASS** | [P6-05_EVENT_LOG.md](P6-05_EVENT_LOG.md) |
| P6-06 | **GAP** | [P6-06_IO_VOLTAGE_GAP.md](P6-06_IO_VOLTAGE_GAP.md) |
| P6-07 | **PART** | [P6-07_M_VOLTAGE_DELTA.md](P6-07_M_VOLTAGE_DELTA.md) |

## Deploy (replay)

```powershell
$env:CCLI_TG544_PW = '<pw>'
pscp -scp lab\tg544-openwrt\ccli-bin root@192.168.10.1:/tmp/ccli.new
plink ... "sh /tmp/deploy-ccli-dut.sh"
.\lab\tg544-openwrt\deploy-p6-03-config.ps1
.\lab\tg544-openwrt\deploy-annex-m-sms.ps1
.\lab\tg544-openwrt\run-p6-annex-m-inhibit.ps1
```

## Normative note

Field product: **DSO LTE → modem DO (M.3.1 class) → PI**. Lab uses **DIO3 @ 12 V** + SMS poller.
