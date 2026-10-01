# TSP P3-07 — Solution B wire crypto proof (2026-09-29)

## Source

DUT plaintext hex from `/tmp/ccli.log` (`TX_ACCEPT_SPDU len=1312`, Solution B):

- File: `lab/tmp_tx_solution_b_20260929.hex`
- Verify script: `lab/verify_solution_b.py`

## Wire auth block

| Field | Value |
|-------|-------|
| Embedded cert | **823 octets**, byte-identical to `server_tls.pem` |
| Subject | `CN=CCI016_01` (not G.6.2 OID) |
| GeneralizedTime | `20260929192858.` (15 bytes) |
| Signature | 256 octets, prefix `f358d9d7…` |

## Offline verify

| Key / cert | `time_tlv_81` (§11.2.2) |
|------------|-------------------------|
| Embedded cert pubkey | **PASS** |
| `server_tls.pem` | **PASS** |
| `server.key` pubkey | **PASS** |
| `server.pem` | FAIL (wrong cert) |

Local re-sign with `server.key` over same `0x81` TLV → **byte-identical** signature to wire.

## TSP result (same session)

- Wireshark frames ~13036–13041: full TLS OK → AARE ~1147+411 B → Encrypted Alert **~1.7 ms**
- TSP log: `Unable to verify signature (+)`

## Conclusion

Solution B **rejects** the hypothesis that TSP only needed AARE cert = TLS cert.

Crypto on the wire is **correct** for §11.2.2 (`time_tlv_81` + `server_tls.pem` + `server.key`).

TSP is failing on a **non-RSA check** (likely G.6.2 AP/AE binding, alternate signed payload, or client AARQ verify path).

## Wireshark note

Packet 13036 is **TLS Application Data (encrypted)** unless RSA keys / key log configured. For MMS/ACSE inspection use **DUT hex dump** (`TX_ACCEPT_SPDU`) — not Wireshark decrypt alone.
