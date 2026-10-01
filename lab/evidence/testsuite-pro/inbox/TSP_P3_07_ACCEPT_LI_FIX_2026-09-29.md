# TSP P3-07 — Session ACCEPT extended LI fix (2026-09-29)

## Check result (before this fix)

DUT log on TSP Connect (TLS paths corrected on PC):

```text
mms: client CONNECT count=1
mms: iso session CONNECT (payload=1405)
mms: ACSE auth ACCEPT — mech=TLS role=DSO_OPERATOR cert=872 octets
mms: iso session error (payload=5 spdu=0xa4)
mms: client DISCONNECT count=0
```

**Interpretation:** ACSE/RBAC **PASS** (via TLS peer-cert fallback). Failure is **after** AARQ — outbound **ACCEPT SPDU** was malformed for large AARE+InitiateResponse.

## Root cause

Inbound CONNECT fix (PGI 194 / extended LI) was deployed earlier, but **outbound** `IsoSession_createAcceptSpdu()` still used:

- 1-byte SPDU LI (max 254)
- PGI 193 user-data with 1-byte length (max 254)

AARE + 62351-4 responder auth + InitiateResponse exceeds 255 bytes → length truncation → TSP aborts (`Initiate Response Failed: Transport connection closed`).

## Fix (`iso_session.c`)

- `encodeSessionUserData()` — PGI **194** + 3-byte LI when payload > 254
- `encodeSessionSpduLi()` — extended X.225 LI on ACCEPT/CONNECT when spdu > 254
- Diagnostic: `mms: iso session ACCEPT extended LI (spdu=… payload=…)`

## Deploy

- Build: `ccli-0.1.0-r16.apk` → `lab/tg544-openwrt/ccli-bin`
- DUT pid **19018**, listening `192.168.10.1:3782`

## Retest

TSP System Status → Connect. Expected DUT log:

```text
mms: iso session CONNECT (payload=1405)
mms: ACSE auth ACCEPT — mech=TLS role=DSO_OPERATOR cert=872 octets
mms: iso session ACCEPT extended LI (spdu=… payload=…)
mms: client CONNECT count=1   (stays connected)
```

TSP: **Connected** (not AssociationTimeout).
