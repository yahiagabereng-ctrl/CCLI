# P3-07 — ISO Session extended LI fix (2026-09-29)

## Problem

TSP Connect: `Initiate Response Failed: Transport connection closed`

DUT: TLS OK → `client CONNECT` → `client DISCONNECT` with **no** ACSE/presentation logs.

## Root cause

TSP **62351-4 MMS Security** P-CONNECT exceeds **255 bytes** (extra presentation context + cert in AARQ).

X.225 requires **3-octet LI** (`0xFF` + 2 length bytes) for lengths 255–65535. `iso_session.c` only read **one** LI byte; **PGI 194** (Extended User Data) was ignored → `SESSION_ERROR` with no stderr.

## Fix (libiec61850 vendor patch)

| File | Change |
|------|--------|
| `iso_session.c` | `decodeSessionLiField()`; CONNECT/FINISH/DISCONNECT use extended SPdu LI; PGI 193/194 user-data; skip unknown PGIs |
| `iso_connection.c` | Log `mms: iso session CONNECT/error` on session indications |

## Build verification

- Host: `apps/ccli/build-mms-host` — **PASS** (iec61850 + ccli link)
- Unit: `mms_acse_auth_test` — **PASS**
- Cross: `lab/tg544-openwrt/ccli-bin` — **built** (aarch64 musl)

Binary contains: `mms: iso session CONNECT`, `mms: iso session error`.

## Deploy (DUT)

```bash
# From WSL on lab PC
scp lab/tg544-openwrt/ccli-bin root@192.168.10.1:/tmp/ccli.new
ssh root@192.168.10.1 'killall ccli; mv /tmp/ccli.new /usr/sbin/ccli; chmod +x /usr/sbin/ccli; /usr/sbin/ccli -c /etc/ccli/lab.yaml >>/tmp/ccli.log 2>&1 &'
```

## TSP retest — expect in `/tmp/ccli.log`

```text
mms: iso session CONNECT (payload=…)
mms: ACSE auth ACCEPT — mech=…
```

If still failing: `mms: iso session error (payload=… spdu=0x0d)` → capture payload size for next triage.

## TSP PC checklist

- Port **3782**, TLS ON, key **`client_tsp.key`**
- **Directory to CA = empty**
- Connect from **System Status**
