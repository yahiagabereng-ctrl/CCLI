# TLS + Wireshark analysis handoff — @alirezavzand

**Repo:** https://github.com/yahiagabereng-ctrl/CCLI (private — accept collaborator invite)  
**Date:** 2026-10-02  
**Build on DUT:** `0.1.0-r25` (lab cleartext `:102` for P5; product TLS `:3782`)  
**Code focus:** `apps/ccli/adapters/iec61850_mms/mms_aare_auth.cpp` · `mms_adapter.cpp` · `apps/ccli/config/tls/`

---

## 1. Start here (symptom → capture)

| Issue | Primary capture | Analysis doc | DUT log |
|-------|-----------------|--------------|---------|
| **TSP Step 7 FAIL** — Encrypted Alert ~2 ms after AARE | `testsuite-pro/pcap/TSP_P3_07_mms_2026-09-29_222559.pcapng` (108 frames) | `testsuite-pro/inbox/TSP_P3_07_WIRE_EVIDENCE_2026-09-29_222559.md` | `O13_1_isolation_2026-09-25/P3_07_SECURITY_ASSOC.txt` |
| **TLS mutual OK** (no client cert → fail) | — | `O13_1_isolation_2026-09-25/P3_07_TLS_MUTUAL_OK.txt` · `P3_07_TLS_NOCLIENT_FAIL.txt` | same session dir |
| **P5 MMS TLS PASS** (TotW/TotVAr over :3782) | `phase5/P5_MMS_TLS.pcapng` (80 frames) | `phase5/P5_MMS_TLS_ANALYSIS.txt` | `phase5/P5_MMS_TLS_SESSION.txt` |
| **P4 IEC 104 TLS** (Eth_B) | `phase4/P4_03_104_TLS.pcapng` (164 frames) | `phase4/P4_03_104_TLS_ANALYSIS.txt` | `phase4/P4_03_104_TLS_SESSION.txt` |
| **Lab PKI / OpenSSL verify** | — | `testsuite-pro/inbox/TSP_P3_07_OPENSSL_VERIFY_2026-10-01_210622.txt` | `apps/ccli/config/tls/README.md` |

---

## 2. Wireshark files (all in repo)

```text
lab/evidence/testsuite-pro/pcap/
  TSP_P3_07_mms_2026-09-29_222559.pcapng   ← main Step 7 failure (USE THIS)
  TSP_P3_07_mms_2026-09-29_222127.pcapng   ← empty/minimal (timing miss)
  TSP_P3_07_mms_2026-09-29_221458.pcapng   ← empty/minimal

lab/evidence/phase5/
  P5_MMS_TLS.pcapng                        ← TLS :3782 PASS + live measurements

lab/evidence/phase4/
  P4_03_104_TLS.pcapng                     ← IEC 60870-5-104 TLS on Eth_B
```

**Filter suggestions:** `tcp.port == 3782` · `tls` · `ip.addr == 192.168.10.1`

---

## 3. Known Step 7 wire timeline (222559 capture)

1. TLS 1.2 handshake completes (frames 8–20).
2. Client sends encrypted AARQ (~1093 B records).
3. Server sends encrypted **AARE** (1088 + 304 B) at **t ≈ 122.7 ms**.
4. Client **Encrypted Alert** at **t ≈ 125.3 ms** (**Δ ≈ 2.6 ms**).
5. DUT log same session: `AARE responder auth [10]EXTERNAL … 1065 octets` then `HEX TX_ACCEPT_SPDU len=1269`.

**Hypothesis for review:** TSP rejects **62351-4 §11.2.2 AARE auth-value** (GeneralizedTime format, sha256WithRSA over time TLV, `0xa0` wrapper) — not TLS layer.

Cross-ref: `knowledge-base/08-engineering/CCI_62351_MMS_Security_Debug_Map.md`

---

## 4. Text logs bundle (P3 security)

| File | Content |
|------|---------|
| `O13_1_isolation_2026-09-25/P3_06_MMS_TLS_ETH_A.txt` | Eth_A TLS listen :3782 |
| `O13_1_isolation_2026-09-25/P3_07_SECURITY_ASSOC.txt` | Full assoc + ACSE + AARE hex |
| `O13_1_isolation_2026-09-25/P3_08_09_RBAC_PKI.txt` | DSO_OPERATOR / VIEWER / CRL |
| `testsuite-pro/inbox/TSP_P3_07_62351-9_CROSSCHECK_2026-09-30.md` | keyUsage TLS vs E2E |
| `testsuite-pro/inbox/TSP_P3_07_SIGNATURE_ANALYSIS_2026-09-29.md` | offline sig verify |
| `testsuite-pro/inbox/TSP_P3_07_G62_DECODE_FIX_2026-09-29.md` | Annex G.6.2 cert OID |

---

## 5. Lab PKI (dual cert — Annex G.2)

| Cert | Role | AP/AE |
|------|------|-------|
| `server_tls.pem` | TLS transport | — |
| `server.pem` | ACSE / AARE auth | **1.1.1.999.1 / 12** |
| `client_tls.pem` | Client TLS | — |
| `client.pem` | DSO ACSE match | **1.1.999.1 / 12** |
| `lab_crl.pem` | Revoked serial test | P3-09 |

Regenerate: `python3 apps/ccli/config/tls/gen_lab_pki.py`

---

## 6. Suggested analysis order

1. Open **222559.pcapng** — confirm AARE → Alert timing on both connect attempts.
2. Read **TSP_P3_07_WIRE_EVIDENCE** — frame table + DUT log correlation.
3. Compare **server.pem** vs **server_tls.pem** in OpenSSL verify log (2026-10-01).
4. Trace **mms_aare_auth.cpp** build path vs TSP expected §11.2.2 BER.
5. Contrast with **P5_MMS_TLS.pcapng** (lab `mms_tls_client` — PASS).

---

## 7. Phase 5 context (cleartext vs TLS)

P5 lab gate used **cleartext MMS :102** for TSP full-circle. TLS path is documented but not TSP-validated in P5 closeout:

- `phase5/P5_TSP_CLEARTEXT_README.md`
- `phase5/P5_MMS_TLS_README.md` (parallel TLS evidence)

Product path remains **:3782 + IEC 62351-3/4** per Annex T T.3.3.4.1.
