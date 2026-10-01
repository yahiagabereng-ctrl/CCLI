# TSP P3-07 — Signature failure analysis (2026-09-29)

## Verdict

| Layer | Status | Evidence |
|-------|--------|----------|
| Transport TLS | **PASS** | Wireshark 5585–5601; full handshake |
| DUT AARE emission | **PASS** | `TX_ACCEPT len=1268`, `inner=1064 octets` |
| RSA signature (§11.2.2) | **PASS** | `verify_62351_auth.py` → `time_tlv_81` with `server.pem` |
| Test Suite Pro | **FAIL** | `Unable to verify signature (+)` → Encrypted Alert @5618 |

**Conclusion:** Not a signing bug on the DUT. Test Suite Pro (or its validation path) is almost certainly checking the signature against the **wrong public key** — the **Transport TLS certificate** (`server_tls.pem`) instead of the **embedded AARE certificate** (`server.pem`) per Annex **G.2**.

---

## Cryptographic proof (lab PC)

```
server.pem + signature from wire  → VERIFY PASS
server_tls.pem + same signature   → VERIFY FAIL
```

Annex G.2 uses **separate key pairs**:

| Role | Certificate | Private key |
|------|-------------|-------------|
| Transport TLS | `server_tls.pem` (CN=CCI016_01) | `server.key` |
| AARE auth | `server.pem` (2.5.4.106=1.1.1.999.1.12) | `server_acse.key` |

`server.pem` ↔ `server_acse.key` public key match: **yes**.

---

## Wire / DUT log (live connect)

```
mms: ACSE auth ACCEPT — mech=TLS role=DSO_OPERATOR cert=779 octets
mms: AARE responder auth [10]EXTERNAL ctx=5 mech=8 inner=1064 octets
mms: iso session ACCEPT extended LI (spdu=1264 payload=1244)
```

ACSE AARE fields present in `tmp_tx_dut_20260929.hex`:

| Field | Value |
|-------|-------|
| responding-AP-title `@112` | OID **1.1.1.999.1** |
| responding-AE-qualifier `@121` | **12** |
| mechanism-name | `{1 0 840 0 1 0 1 1}` |
| auth cert subject | UTF8String **1.1.1.999.1.12** under 2.5.4.106 |
| GeneralizedTime | `20260929190347.` |
| embedded cert | 779 octets, **byte-identical** to repo `server.pem` |

---

## Ingested corpus (`knowledge-base/08-engineering/`)

| Document | Status | Relevance |
|----------|--------|-----------|
| `CCI_62351-4_Extract.md` | **HAVE** Batches A–G | §11.2.2 auth value; Annex G.2 dual cert; G.6.2 binding |
| `CCI_62351-4_Capture_Plan.md` | **HAVE** | C1 native + A-profile compatibility |
| `CCI_62351-3_Extract.md` | **HAVE** | Transport TLS |
| `CCI_62351-9_Extract.md` | **HAVE** | PKI / CRL |
| §10 OSI (full) | **GAP** | pp. 33–36 not captured; §10.5.3 cited via Annex G only |

Normative rules applied:

- **REQ-3514-APROF-002** (§11.2.2): signature over **DER-encoded time field** (`0x81` TLV) — **satisfied on wire**
- **REQ-3514-PKI-001** (Annex G.2): separate TLS vs E2E certs — **implemented on DUT**
- **REQ-3514-PKI-006** (G.6.2): subject = responding AP + AE — **string matches**; value encoded as **UTF8String** (lab choice for OpenSSL decode)
- **R-354-011**: *Dual cert provisioning error — TLS cert used for E2E* — **matches observed failure mode**

---

## Recommended next steps

1. **Confirm hypothesis (quick lab test):** temporarily set DUT `tls_own_cert: server.pem` and `tls_own_key: server_acse.key` (single cert for TLS + AARE). If Test Suite Pro **Connect** passes, root cause is Test Suite Pro using TLS peer cert for §11.2.2 verify under G.2.
2. **Product-correct path:** keep G.2 dual cert; confirm with Triangle whether Test Suite Pro validates `responding-authentication-value` against **embedded** cert (required by §11.2.2) vs Transport TLS cert.
3. **Optional strict PKI:** regenerate G.6.2 subject with OBJECT IDENTIFIER value (`0x06`) if Test Suite Pro binding requires it (may break OpenSSL 3 `x509 -subject` — test before deploy).

Hex for offline verify: `lab/tmp_tx_dut_20260929.hex`
