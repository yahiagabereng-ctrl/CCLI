# SM-9 record — SBOM + risk review sign-off (0.1.0-r64)

**Document ID:** CCLI-SM9-REC-r64-001  
**Date:** 2026-11-13  
**Release:** **0.1.0-r64** (`P5-EMT432-CMV-ANG-URCB-PF1`)  
**ELF SHA256:** `967df2764819c3b616e5467a16a3f9a923b4002ad1ca4a73d156d16e91ab2df7`  
**Approver:** HiTEKS software lead (Yahia) — *pending formal signature*

---

## Checklist (EVD-SM-9 §5)

| # | Item | Status | Pointer |
|---|------|--------|---------|
| 1 | App CycloneDX SBOM generated | **DONE** | `CCLI_APP_SBOM_0.1.0-r64.cdx.json` |
| 2 | Platform runtime intake (apk) | **DONE** | `intake/20261112-104506/` |
| 3 | Architecture + implementation map | **DONE** | `CCLI_SW_ARCHITECTURE_AND_IMPLEMENTATION_0.1.0-r64.md` |
| 4 | High-impact libs versioned (61850, 60870, mbedtls) | **DONE** | SBOM JSON + git tags v1.6.0 / v2.3.5 / 3.6.0 |
| 5 | CVE review (app) | **DONE** | `lab/evidence/trivy/EVD-SCA-CCI-r64-Trivy.md` — 0 blocking |
| 6 | Vendor OS SBOM | **GAP** | TesPro SM-10 request |
| 7 | Re-eval cadence set | **DONE** | Each **rN** deploy + monthly CVE (Trivy script) |

---

## Accepted risks (this release)

| Component | Risk | Disposition |
|-----------|------|-------------|
| Lab TLS private keys in repo/DUT | Trivy HIGH secrets | **ACCEPT** — lab PKI; see Trivy JSON |
| TesproOS CVEs | Not in app SBOM | **SM-10** vendor closure |
| libiec61850 / lib60870 GPL | License | Commercial license path (MZ) |

---

## Next actions

1. Bump OpenWrt **apk** version string to **r64** on next `build-ccli-ipk-sdk.sh` / deploy.  
2. Request TesPro CycloneDX for image **r601-af70fcaaa2**.  
3. Re-run `generate-ccli-app-sbom.py` after any `third_party` tag change.
