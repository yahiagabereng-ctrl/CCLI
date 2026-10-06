# EST — EJBCA doc snapshot

**Source:** https://docs.keyfactor.com/ejbca/latest/est (Keyfactor, ingested 2026-10-06)

## Summary

Enrollment over Secure Transport (EST, RFC 7030) — certificate enrolment over TLS. EJBCA supports EST since v6.11.

**Advantages over SCEP:** TLS-native, ECC, implicit auth on TLS, renewal built-in.

## EJBCA modes

| Mode | CCLI use |
|------|----------|
| **Client mode** | Pre-registered end entity + enrollment code in CSR — matches **factory/vendor → domain** handoff |
| **RA mode** | CSR drives subject; auto-creates end entity — pilot lab when TG544 sends CSR via EST |

## Supported operations

| Operation | Purpose |
|-----------|---------|
| `cacerts` | GET CA chain (no client auth) |
| `simpleenroll` | POST PKCS#10 CSR → signed cert |
| `simplereenroll` | Renewal with existing client cert |
| `serverkeygen` | Server-side key gen (optional; not for TPM CCI keys) |

## URL pattern

```
https://<host>/.well-known/est/<alias>/<operation>
```

Default alias `est` → `https://<host>/.well-known/est/simpleenroll`

## CCLI mapping

- Annex T T.3.3.4.9: EST preferred over SCEP
- K6.3 Phase 2: **two** `simpleenroll` cycles (Cert A TLS, Cert B E2E)
- Firmware gap: EST client **not yet in ccli** — manual PEM export until implemented
