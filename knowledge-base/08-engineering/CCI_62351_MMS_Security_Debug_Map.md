# IEC 62351-3 / 62351-4 / 62351-8 — MMS security debug map (clause-traced)

**Document ID:** CCLI-SEC-62351-DEBUG-001  
**Revision:** 1.0  
**Date:** 2026-10-01  
**RAG source_id:** `ccli-62351-mms-security-debug-map`  
**Purpose:** Precise norm ↔ implementation ↔ lab PKI ↔ log-line map for debugging **TLS handshake**, **AARQ/AARE authentication**, **DSO_OPERATOR RBAC**, and **CRL** on Eth_A MMS.  
**Parent extracts:** `ccli-62351-3-extract` · `ccli-62351-4-extract` · `ccli-62351-8-extract` · `ccli-62351-9-extract` · `cei-0-16-allegato-t-extract`  
**Code:** `apps/ccli/adapters/iec61850_mms/` · `apps/ccli/config/tls/`  
**Lab evidence:** `lab/evidence/O13_1_isolation_2026-09-25/P3_07_*`

---

## Requirements

| ID | Requirement | Norm | CCLI status |
|----|-------------|------|-------------|
| REQ-DBG-TLS-001 | Mutual TLS on **TCP :3782** with Table 1 cipher suites | 62351-3 §5 · 62351-4 §6.3 | **IMPL** lab P3 |
| REQ-DBG-AARE-001 | Server **AARE** carries certificate-based MMS auth (A-profile §11.2.2) | 62351-4 §11.2.2–11.2.4 | **IMPL** `mms_aare_auth.cpp` |
| REQ-DBG-ACSE-001 | Client **AARQ** ACSE mechanism **TLS** or **CERTIFICATE**; cert DER match | 62351-4 §11.1.4 · Annex G | **IMPL** `mms_acse_auth.cpp` |
| REQ-DBG-RBAC-001 | **DSO_OPERATOR (−1)** may Operate `Wlim`/`WSd`; **VIEWER (0)** read-only | 62351-8 §5.2 · Annex T T.3.3.4 | **IMPL** `mms_adapter.cpp` check handler |
| REQ-DBG-CRL-001 | Revoked cert on **lab_crl.pem** → association fail (not unknown-cert) | 62351-3 §5.6.4.4 · 62351-4 Annex G.5.3 | **IMPL** P3-09 |
| REQ-DBG-DUAL-001 | Separate TLS cert vs ACSE/AARE cert when both enabled | 62351-4 Annex G.2 | **CONFIG** `tls_own_*` vs `tls_acse_*` |

---

## 1. Stack layers (debug order)

```text
TCP :3782
  └─ TLS 1.2 mutual auth          ← 62351-3 (handshake, cipher, CRL at transport)
       └─ ACSE AARQ / AARE        ← 62351-4 §11 (mechanism, auth value)
            └─ MMS association
                 └─ RBAC on Operate ← 62351-8 + Annex T DSO_OPERATOR (−1)
```

**Lab cleartext bypass (P5):** `:102` tls=off — **does not exercise** this map. Product path: `:3782` + TLS on.

---

## 2. IEC 62351-3 — TLS handshake

| Norm clause | Requirement | Config key (YAML) | Lab file | Code | Pass log / symptom |
|-------------|-------------|-------------------|----------|------|------------------|
| **62351-3 §5.7** | Secure port **3782** vs cleartext **102** | `mms.tls.enabled`, listen port | — | `mms_adapter.cpp` TLS branch | `mms: listening … :3782` |
| **62351-3 Table 1** | Mandatory cipher suites (TLS 1.2) | libiec61850 TLS build | — | `TLSConfiguration_*` | Handshake completes; else client abort |
| **62351-3 §5.6.4** | Mutual X.509 auth | `tls_own_cert`, `tls_own_key`, `tls_ca_cert` | `server_tls.pem`, `server.key`, `root_CA.pem` | lines ~743–760 | `TLS CRL loaded` / cert load errors |
| **62351-3 §5.6.4.4** | CRL check; revoked → refuse | `tls_crl` | `lab_crl.pem` | `TLSConfiguration_addCRLFromFile` ~762 | P3-09: assoc fail with revoked on allowlist |
| **62351-3 §5.6.4.4** | CRL inaccessible **shall not** kill established session | — | — | libiec61850 behaviour | Warning only on live session |
| **62351-4 §6.3.4.2** | Compatibility ciphers when interop | — | — | — | TSP often negotiates AES-GCM-SHA256 |

**Debug checklist — TLS fails before MMS:**

1. Confirm DUT listens **3782** not 102.
2. Client trusts `root_CA.pem`; presents `client_tls.pem` + key.
3. Check DUT log: `mms: TLS failed to load …` (own cert, CA, CRL, allowlist).
4. Evidence: `lab/evidence/.../P3_07_TLS_MUTUAL_OK.txt`, `P3_07_TLS_NOCLIENT_FAIL.txt`.

---

## 3. IEC 62351-4 — AARQ / AARE authentication

### 3.1 A-profile §11.2.2 — MMS-Authentication-value (responder)

| Field | Norm rule | CCLI implementation | Notes |
|-------|-----------|----------------------|-------|
| `[0] authentication-Certificate` | DER X.509, max **8192** octets | `g_server_cert_der` from PEM | Loaded in `mms_aare_auth_configure` |
| `[1] time` | **GeneralizedTime** | `format_a_profile_generalized_time` | Format **`YYYYMMDDHHMMSS.`** (15 chars + dot) — **no Z**, no ms |
| `[2] signature` | **sha256WithRSA** over **DER-encoded [1] time** TLV (tag `0x81`) | `sign_time_field_pkcs1_sha256` | Signs hash of time_field, not raw GT string |
| Wrapper | `[0] IMPLICIT SEQUENCE` tag **`0xa0`** | `build_mms_auth_value` | Placed in ACSE **responding-authentication-value [10]** |

**Norm:** 62351-4 **§11.2.3–11.2.4** — server generates matching value in **AARE** on accept.

**Code path:** `mms_aare_auth_register_acse_bridge()` → `AcseConnection_setCcliAareAuthBuilder` → `ccli_aare_auth_build_tramp`.

**Pass log:** `mms: AARE 62351-4 responder auth ready (cert=N octets)`  
**Fail log:** `mms: AARE auth sign/self-verify failed` · `pk_sign failed` · `failed to load server cert DER`

**Known lab caveat (mbedTLS):** G.6.2 **id-at-objectIdentifier** in server cert may fail `x509_crt_parse` on DUT — code **does not** parse full cert for AARE emission; uses raw DER only (see comment in `mms_aare_auth.cpp`).

### 3.2 ACSE AP-title / AE-qualifier (Annex G.6.2)

| Role | AP-title | AE-qualifier | Lab cert |
|------|----------|--------------|----------|
| IED responder | **1.1.1.999.1** | **12** | `server.pem` |
| DSO caller (TSP) | **1.1.999.1** | **12** | `client.pem` |

**Code:** `kMmsRespondingApTitle` / `kMmsRespondingAeQualifier` in `mms_aare_auth.hpp`.

### 3.3 Client AARQ — ACSE authenticator

| Check | Norm | Code | Log |
|-------|------|------|-----|
| Mechanism | **TLS (3)** or **CERTIFICATE (2)** only | `mms_acse_mechanism_supported` | `REJECT — mechanism=…` |
| Cert present | Non-empty DER in auth parameter | `evaluate_acse_authentication` | `REJECT — empty certificate` |
| DSO match | DER == `client.pem` (or `tls_acse_client_cert`) | `der_eq(creds.dso_der, …)` | `ACSE auth ACCEPT — mech=… role=DSO_OPERATOR` |
| VIEWER match | DER == `viewer.pem` | `AcceptViewer` | `role=VIEWER` |
| Unknown cert | RBAC on | `RejectUnknownCert` | `REJECT — unknown certificate` |

**62351-4 §11.2.3 client rules (TSP responsibility):** time skew **≤10 min**, no duplicate time, valid signature — **verified by client**, not re-implemented on DUT.

### 3.4 Dual certificate (Annex G.2)

| Purpose | YAML keys | Default lab files |
|---------|-----------|-------------------|
| **TLS** transport | `tls_own_cert`, `tls_own_key` | `server_tls.pem`, `server.key` |
| **ACSE / AARE** application | `tls_acse_cert`, `tls_acse_key` | `server.pem`, `server_acse.key` |
| DSO TLS allowlist | `tls_client_cert` | `client_tls.pem` |
| DSO ACSE DER match | `tls_acse_client_cert` (falls back to client_cert) | `client.pem` |

---

## 4. IEC 62351-8 — RBAC / DSO_OPERATOR

| Norm | Annex T | CCLI behaviour | Log |
|------|---------|----------------|-----|
| **62351-8 §5.2.2** — access if required right ∈ session roles | **T.3.3.4** private role **DSO_OPERATOR = −1** | `securityToken = &g_token_dso` on DSO cert match | see ACSE logs |
| Operate **Wlim.Mod** / **WSd** | Table 98 DSO-reserved DOs | `check_tramp` requires `g_token_dso` | `RBAC DENY … not DSO_OPERATOR` |
| **VIEWER (0)** | Read / associate | `g_token_viewer` — no write | Operate → **ACCESS_DENIED** |
| RBAC disabled | — | `rbac_enabled = false` when no `dso_der` | All controls accepted |

**Enable RBAC:** load DSO cert DER into `impl->dso_der` (`mms_adapter.cpp` ~785–841); `rbac_enabled = !impl->dso_der.empty()`.

**Unit test:** `apps/ccli/tests/unit/mms_acse_auth_test.cpp` — TLS open RBAC, CERTIFICATE open RBAC.

---

## 5. CRL and certificate validation

| Scenario | Norm expectation | Lab setup | Expected result |
|----------|------------------|-----------|-----------------|
| Valid DSO | 62351-3 §5.6.4 | `client.pem` not on CRL | TLS + ACSE **ACCEPT** |
| Revoked cert | Annex G.5.3 · 62351-9 | `revoked.pem` on **`lab_crl.pem`** AND on TLS allowlist | **Reject** at TLS (P3-09) — allowlist ensures CRL path not masked as unknown |
| Wrong CA | PKI | Untrusted client cert | TLS handshake fail |
| Unknown cert (RBAC on) | 62351-8 | Random cert, valid TLS | ACSE **REJECT — unknown certificate** |

**Config:** `tls_revoked_cert` — must be allowlisted (`mms_adapter.cpp` comment ~829).

---

## 6. Annex T cross-refs (Eth_A cyber)

| Annex T clause | Topic | 62351 part | CCLI debug surface |
|----------------|-------|------------|-------------------|
| **T.3.3.4.1** | TLS port **3782**, min TLS 1.2 | 62351-3 + 62351-4 | TLS config |
| **T.3.3.4.3** | Role management | 62351-8 | ACSE + Operate RBAC |
| **T.3.3.4.5** | UTC **±100 ms** | — (time sync) | AARE **time** field validity; GNSS/chrony |
| **T.3.3.4.9** | SCEP/EST PKI (product) | 62351-9 | Lab uses static PEM + `gen_lab_pki.py` |

---

## 7. Failure symptom → first check

| Symptom | First check | Clause |
|---------|-------------|--------|
| Connection reset at TCP | Port 3782 vs 102 | 62351-3 §5.7 |
| TLS alert / no MMS | Cipher suite, client cert, CA | 62351-3 Table 1 |
| Associate OK, Operate denied | RBAC token — VIEWER vs DSO | 62351-8 · T.3.3.4 |
| Associate rejected | ACSE mechanism or cert DER mismatch | 62351-4 §11 |
| TSP rejects AARE | AARE auth: time format, signature, cert size | 62351-4 §11.2.2 |
| Revoked still connects | CRL not loaded or cert not on allowlist | P3-09 setup |
| AARE auth disabled log | `tls_acse_cert` / `tls_acse_key` load fail | Annex G.2 paths |

---

## 8. RAG routing pack (62351 MMS security)

Use these `source_id`s together when debugging MMS security:

```text
ccli-62351-mms-security-debug-map
ccli-62351-3-extract
ccli-62351-4-extract
ccli-62351-8-extract
ccli-62351-9-extract
cei-0-16-allegato-t-extract
```

**Example queries (high precision):**

- `62351-4 §11.2.2 GeneralizedTime AARE signature sha256WithRSA`
- `DSO_OPERATOR RBAC Wlim Operate 62351-8`
- `TLS 3782 CRL lab_crl.pem revoked certificate`
- `Annex G.6.2 AP-title 1.1.1.999.1 AE-qualifier 12`

---

## Keywords

`62351-3`, `62351-4`, `62351-8`, `62351-9`, `TLS`, `AARQ`, `AARE`, `ACSE`, `MMS-Authentication-value`, `GeneralizedTime`, `DSO_OPERATOR`, `VIEWER`, `RBAC`, `CRL`, `Annex G`, `3782`, `mms_aare_auth`, `mms_acse_auth`, `ccli-62351-mms-security-debug-map`
