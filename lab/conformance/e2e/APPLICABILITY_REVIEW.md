# Encryption and attribute-certificate applicability review

Reviewed 23 October 2026 against the source PDFs, including rendered source pages. This corrects the previous suggestion that all 108 conditionally dispositioned evidence rows could be excluded for the CEI CCI target.

## Decision

| Capability | CEI CCI decision | Source |
|---|---|---|
| Application encryption capability | Required; cannot be excluded by selecting an integrity-only operating policy | CEI 57-142:2026-07, 5.1, pp. 47-48 |
| AES-128-CBC and AES-256-CBC | Both required | CEI 57-142:2026-07, 5.1, p. 47 |
| AES-128-GCM and AES-256-GCM | Both required, including operation without confidentiality as specified | CEI 57-142:2026-07, 5.1, p. 47 |
| Encryption enabled on every association | Not required; agreed security policy may enable or disable it | CEI 57-142:2026-07, 5.1, pp. 47-48 |
| Separate X.509 attribute certificates / RBAC Profile B | Profile A or Profile B is required; Profile B positive capability can be excluded when Profile A is the declared supported alternative | CEI 57-142:2026-07, 5.3.2, p. 53; 5.3.2.2, p. 54 |
| Profile B certificate distribution when B is selected | Both PUSH and PULL are required by the PMI clause | CEI 57-142:2026-07, 5.9.1, p. 71 |

IEC TS 62351-100-4:2023 by itself uses declaration conditions: Table 11 permits non-AE with no encryption, and Table 12 conditions C1/C2 require CBC/GCM algorithms when the corresponding encryption mode is declared (printed pp. 24-25, PDF pages 26-27). These generic conditions do not waive CEI's stronger required product capabilities. The base IEC 62351-4:2018+AMD1:2020 text also permits E2E without encryption (printed p. 28) and describes conditional encryption fields (13.3.1.1, printed p. 52).

Attribute certificates must not be confused with Profile A role information carried in an identity-certificate extension. Section 5.3.2 explicitly permits A or B. Section 5.9.1 introduces PMI in support of B; its PUSH/PULL requirements must be applied when B is selected. Received unsupported or invalid credential/message paths still need correct rejection behavior.

## Effect on implementation and evidence

The current integrity-only E2E path remains a useful development and operating configuration. It does not establish the required switchable encryption capability. Implement native negotiation, key derivation, encrypted transfer, IV/AAD/authentication rules and positive/negative TX/RX tests for both CBC key sizes and both GCM key sizes.

The 108 conditional dispositions in the saved evidence ledger describe what the current implementation exercised or omitted. They are not 108 approved CEI exclusions. Preserve the results as historical evidence and use this CEI applicability overlay when planning closure. Profile B-specific positive tests may be marked not applicable with the explicit Profile A declaration; application encryption tests remain required capability gaps.

## Source files and page indexing

- CEI 57-142:2026-07: `C:/Users/Administrator/Desktop/CCLI Alireza/CCLI/Normativa/CEI 57-142-2026-07.pdf`; printed pages equal PDF page numbers.
- IEC TS 62351-100-4:2023: `C:/Users/Administrator/Desktop/IEC TS 62351-100-4 2023.pdf`; PDF page number = printed page + 2.
- IEC 62351-4:2018+AMD1:2020 CSV: `C:/Users/Administrator/Desktop/CCLI Alireza/CCLI/Normativa/IEC 62351-4 2020.pdf`; clean English PDF page number = printed page + 268.
- CEI 0-16:2026-07 first-page summary directs Annex T communication requirements to CEI 57-142.
