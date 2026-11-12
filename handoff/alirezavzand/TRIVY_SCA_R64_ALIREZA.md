# Trivy SCA — 0.1.0-r64 handoff for Alireza

**Date:** 2026-11-12  
**Audience:** @alirezavzand (SVV-3 / SI-1 course evidence · firmware review)  
**Git commit:** *(pinned after push — `git pull origin main`)*

---

## What to open first

| # | Document | Role |
|---|----------|------|
| 1 | [`lab/evidence/trivy/EVD-SCA-CCI-r64-Trivy.md`](../../lab/evidence/trivy/EVD-SCA-CCI-r64-Trivy.md) | Signed-off narrative (**FINAL** rev 2) |
| 2 | [`lab/evidence/trivy/CCLI-TRIVY-FINDINGS-DETAIL-0.1.0-r64.json`](../../lab/evidence/trivy/CCLI-TRIVY-FINDINGS-DETAIL-0.1.0-r64.json) | **65** findings · per-item `analysis.status` (ACCEPT / N_A / SM10 / DIFF_device_repo) |
| 3 | [`lab/evidence/trivy/README.md`](../../lab/evidence/trivy/README.md) | How to re-run scans |

**Regenerate JSON:** `powershell -File scripts/trivy/export-trivy-findings-detail-json.ps1`

---

## Verdict (short)

| Area | Result |
|------|--------|
| **Shipping `ccli` ELF** (staging pin) | **0 CVE** · SHA256 `967df2764819c3b616e5467a16a3f9a923b4002ad1ca4a73d156d16e91ab2df7` |
| **apps-ccli / product-apps-ccli** CVE | **0** blocking (mbedtls docs pip trimmed via `.trivyignore`) |
| **Trivy secrets** | **65 HIGH** — all **lab PKI** / LuCI / uhttpd keys; disposition **ACCEPT** in JSON |
| **Live DUT rootfs** | Run `0.1.0-r64-20261112-094123` — platform OS CVE → **SM-10** vendor SDL |

---

## Suggested review (Alireza)

1. Skim `summary` + `analysis_policy` in the JSON; spot-check `SECRET-*` rows for `DIFF_device_repo` (DUT vs repo TLS).  
2. Cross-read EVD § “EJBCA lab PKI vs Trivy” — duplicate paths are **expected**, not duplicate incidents.  
3. Optional bench: `verify-dut-tls-keys.ps1 -CompareRepo` after deploy.

---

## Chat blurb

See [`MESSAGE_TO_SEND.txt`](MESSAGE_TO_SEND.txt) (section **Trivy SCA r64**).
