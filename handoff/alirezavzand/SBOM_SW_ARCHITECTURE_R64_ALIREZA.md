# SBOM + software architecture — 0.1.0-r64 (Alireza)

**Date:** 2026-11-13  
**Audience:** @alirezavzand  
**Git commit:** **`a1956c4`** — https://github.com/yahiagabereng-ctrl/CCLI/commit/a1956c4

---

## Start here

| # | Document |
|---|----------|
| 1 | [`lab/evidence/EVD-SM-9-External-Components/CCLI_SW_ARCHITECTURE_AND_IMPLEMENTATION_0.1.0-r64.md`](../../lab/evidence/EVD-SM-9-External-Components/CCLI_SW_ARCHITECTURE_AND_IMPLEMENTATION_0.1.0-r64.md) |
| 2 | **App SBOM (CycloneDX):** [`CCLI_APP_SBOM_0.1.0-r64.cdx.json`](../../lab/evidence/EVD-SM-9-External-Components/CCLI_APP_SBOM_0.1.0-r64.cdx.json) |
| 3 | **Platform SBOM proxy (302 apk):** [`platform-runtime-inventory.cdx.json`](../../lab/evidence/EVD-SM-9-External-Components/intake/20261112-104506/platform-runtime-inventory.cdx.json) |
| 4 | **DUT identity + versions:** [`dut-platform-identity.txt`](../../lab/evidence/EVD-SM-9-External-Components/intake/20261112-104506/dut-platform-identity.txt) |
| 5 | KB structure (background): [`knowledge-base/06-application/CCI_Application_Structure.md`](../../knowledge-base/06-application/CCI_Application_Structure.md) |

**Regenerate app SBOM:** `python scripts/sbom/generate-ccli-app-sbom.py`

---

## Your review focus

- **fw4 / zones** ↔ app is separate — this pack is **SM-9 / CR 7.8** only.
- Confirm **r64 ELF SHA** on DUT vs `CCLI_DEPLOY_LATEST.md`.
- Note **apk package label r51** vs binary **r64** — flag for ipk bump.
- **Vendor OS SBOM** still **MISS** — SM-10 ask to TesPro.
