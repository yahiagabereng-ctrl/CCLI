# TSP P3-07 Connect failure — log triage (2026-09-29)

## TSP error (PC)

```text
CCI016_01: Initiate Response Failed: Transport connection closed
```

## DUT snapshot (pull ~19:56 CST)

| Item | Value |
|------|-------|
| Process | `26370 /usr/sbin/ccli` |
| Listen | `192.168.10.1:3782` → ccli |
| Startup | `AARE 62351-4 responder auth ready (cert=869 octets)` |
| TLS | ON · RBAC ON · CRL ON |

## Latest TSP attempt pattern (×3 in log)

```text
TLS: Verify cert … DSO_OPERATOR … OK
TLS: Check against list of allowed certs … match OK
mms: client CONNECT count=1
mms: client DISCONNECT count=0
mms: no clients — Operating Rule fallback armed (15 s)
```

## Not seen (important)

| Expected if… | Line |
|--------------|------|
| Presentation reject | `mms: iso presentation connect failed` |
| ACSE auth reject | `mms: acse AARQ auth failed` |
| ACSE user-info fail | `mms: acse AARQ user-information invalid` |
| ACSE auth OK | `mms: ACSE auth ACCEPT — mech=…` |
| ISO ACSE fail | `mms: iso acse association failed` |

**Interpretation:** TLS mutual auth **passes**. TCP association opens (`CONNECT`) then drops **before or without** reaching `auth_tramp` logging. Likely ISO/MMS layer after TLS (presentation / ACSE / initiate) — needs deeper debug or packet capture.

## PC config note (from user screenshot)

- **Directory to CA** was set to `root_CA.pem` — must be **empty** (file path belongs in CA file field only).

## Evidence files

| File | Content |
|------|---------|
| `TSP_P3_07_DUT_ccli_2026-09-29_*.log` | Full `/tmp/ccli.log` + DUT status |
| This file | Triage summary |

## Next debug (Agent)

1. Confirm **Directory to CA** cleared on PC; retry once.
2. Add stderr trace at presentation / ACSE / MMS initiate (or enable DEBUG_ISO_SERVER build).
3. Optional: TSP sniffer or `tcpdump -i br-lan port 3782` on DUT during Connect.
