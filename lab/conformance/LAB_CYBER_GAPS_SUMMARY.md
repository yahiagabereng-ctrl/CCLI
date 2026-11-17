# CCLI cybersecurity gaps — short summary

**Date:** 2026-10-14  
**Purpose:** Short handoff explaining the cybersecurity work still required before claiming the complete CEI profile.

## Summary

TLS on port 3782, mutual certificates, CRL checking, basic Viewer/DSO access control, and a 2048-event local logger are already present. The following four gaps remain.

| Gap | What is missing | What we need to do |
|-----|-----------------|--------------------|
| **1. IEC 62351-4 E2E security** | TLS protects the network connection, but the MMS messages themselves are not protected with native `SecPDU`. `ClearTransfer`, replay protection, integrity checks, and E2E rekey are missing. | Implement the native IEC 62351-4 E2E handshake and SecPDU data path, use Cert B, reject altered/replayed messages, and support rekey initiated by either side. |
| **2. PKI lifecycle** | EJBCA issues our lab certificates and CRL, but the CCI still uses manually installed PEM files. There is no device-side EST/SCEP enrolment, OCSP checking, automatic key/certificate replacement, or complete trust-anchor set. | Add an EST or SCEP client, OCSP with nonce, key/certificate renewal and replacement, and the DSO, Administrative Domain, and Manufacturer trust anchors. |
| **3. Complete RBAC** | The code mainly supports `VIEWER` and `DSO_OPERATOR`. The remaining IEC roles, `AGGREGATOR_OPERATOR`, the complete rights matrix, SCL revision handling, and full audit behavior are missing. | Implement the agreed IEC 62351-8 roles and rights, enforce them across all claimed MMS services, configure them through SCL, and test allowed and denied actions for every claimed role. |
| **4. Secure security logging** | The local 2048-event logger exists, but remote syslog is currently UDP, not TLS. Full severity mapping and all required PKI, E2E, certificate, RBAC, and update events are not complete. | Add RFC 5425 Syslog over TLS, complete the required event/severity catalogue, and prove that logs cannot be changed by an ordinary user and survive reboot/update. |

## Important distinction

- Passing TLS tests does not prove E2E security.
- Having EJBCA does not prove that the CCI manages its certificate lifecycle.
- Viewer read/control-denial tests do not prove full RBAC.
- A 2048-event wrap test does not prove secure remote logging.

## Recommended order

1. Confirm the exact CEI/IEC editions and PICS/PIXIT scope with the laboratory.
2. Implement E2E security.
3. Complete the PKI lifecycle.
4. Complete RBAC.
5. Finish secure logging and run the combined negative tests.

For engineering details and test evidence, see [LAB_CYBER_GAPS_ACTION_PLAN.md](LAB_CYBER_GAPS_ACTION_PLAN.md).
