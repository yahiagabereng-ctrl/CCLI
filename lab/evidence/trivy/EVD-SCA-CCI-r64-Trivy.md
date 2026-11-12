# EVD — Trivy SCA on CCI firmware (0.1.0-r64)

**Document ID:** CCLI-EVD-SCA-TRIVY-r64-001  
**Date:** 2026-11-12  
**Revision:** 2 (**FINAL** for r64 bench cycle)  
**Maps to:** IEC 62443-4-1 **SI-1** / **SVV-3** (course: Docker + Trivy)  
**Pinned release:** `lab/tg544-openwrt/deploy-manifests/CCLI_DEPLOY_LATEST.md` → **0.1.0-r64**

## Evidence runs (canonical)

| Run ID | Mode | Folder | Scopes |
|--------|------|--------|--------|
| **r64-local** | Repo-only (no SSH) | `lab/evidence/trivy/0.1.0-r64-20261112-092220/` | `ccli-staging`, `dut-extract-r57`, `dut-extract-oct16`, `apps-ccli`, `product-apps-ccli` |
| r64-local (archive) | Repo-only | `lab/evidence/trivy/0.1.0-r64-20261111-164100/` | Same scopes; **27** pip CVE before `.trivyignore` mbedtls-docs trim |
| **r64-dut-lan1** | Live **TR400** @ `192.168.10.1` | `lab/evidence/trivy/0.1.0-r64-20261112-094123/` | `dut-rootfs` + **`dut-tls-keys-live.txt`** (keys **on device now**) |
| r64-dut-lan1 (archive) | Live DUT | `lab/evidence/trivy/0.1.0-r64-20261112-090342/` | Earlier pull same day |

**Tool:** Trivy **0.76.2** · vuln + secret · DB as of **2026-11-11**.

**Machine-readable findings (all severities + analysis):**  
`lab/evidence/trivy/CCLI-TRIVY-FINDINGS-DETAIL-0.1.0-r64.json` — regenerate with `scripts/trivy/export-trivy-findings-detail-json.ps1`.

## Corpus

| Source | Path | Role |
|--------|------|------|
| Deploy staging | `lab/tg544-openwrt/ccli-bin` + manifest SHA256 | Last labelled upload (**r64**) |
| OpenWrt package | `lab/tg544-openwrt/ccli-0.1.0-r64.apk` | Build artefact (apk v3; scan uses staging ELF when unpack N/A) |
| DUT extract pins | `dut-extract-connect-pass-r57-2026-10-16/ccli` | Prior bench **ELF** snapshot |
| Live DUT rootfs | `dut-rootfs.tgz` in r64-dut-lan1 folder | `usr lib bin sbin etc` via plink/pscp; extract on **WSL** for Trivy on Windows |
| Config / TLS | `apps/ccli/config/*` + on-device `etc/ccli/tls/` | Lab PEMs (secret scan) |
| Source SCA | `apps/ccli`, `product/apps/ccli` | Implementation + product tree |
| Identity backup | `lab/evidence/dut-fw-backup/` | Historical binary hashes (optional cross-check) |

**TesproOS / OpenWrt platform** CVE closure stays in **SM-10** vendor SDL; this evidence covers **HiTEKS ccli**, repo trees, and a **bounded live rootfs** sample from the lab DUT.

## Commands

**Repo-only (no SSH):**

```powershell
powershell -File scripts/trivy/scan-cci-firmware.ps1
```

**Live DUT on LAN1 (Eth_A, `192.168.10.1`) — Trivy on PC after pull:**

```powershell
. .\lab\tg544-openwrt\lab-env.ps1
powershell -File scripts\trivy\scan-dut-lan1-trivy.ps1
```

Outputs `dut-identity.txt`, `dut-rootfs.tgz`, `trivy-dut-rootfs.{json,txt,log}`. Windows cannot unpack all Unix paths; the script extracts in **WSL** and scans via `\\wsl.localhost\...`.

**Trivy scope trims:** `scripts/trivy/.trivyignore` (excludes vendored mbedtls tree, **mbedtls docs pip lock**, build dirs).

---

## Analysis — local (r64-local)

### What ships as HiTEKS app binary

| Artefact | SHA256 (prefix) | Trivy vuln | Trivy secret |
|----------|-------------------|------------|--------------|
| **r64 staging** (`ccli-bin`) | `967df276…` | None (ELF-only rootfs) | None |
| **DUT extract r57** | `96963619…` | None | None |
| **DUT extract Oct-16** | (see run folder README) | None | None |

**Conclusion:** The **musl `ccli` executable** pinned for deploy shows **no CVE** at Trivy rootfs scope. Linked runtime libs on device are part of **TesproOS** → **SM-10**.

### CVE findings (apps-ccli)

**r64-local (20261112-092220):** **0 CVE** (scope trim in `scripts/trivy/.trivyignore` excludes mbedtls **docs** pip lock).

**Archive run (20261111-164100):** **27 CVE** in `mbedtls-3.6.0/docs/requirements.txt` only — **N/A — not in PF2 image** (doc-build pip, not firmware). Disposition archived for audit trail; not counted in final verdict.

**product-apps-ccli:** **0 CVE** in rev 1; **secrets only** (lab TLS under `config/tls/`).

### Secret findings (repo trees)

Trivy **HIGH** on **RSA private keys** under `apps/ccli/config/tls/` (p3_07_connect, ejbca, revoked, viewer, server, root_CA, etc.) — **~30+** in lab tree; **~14** in product tree.

| Disposition | Rationale |
|-------------|-----------|
| **Accept — lab / conformance** | Bench PKI per `LAB_ACCESS_FREEZE.md` and P3-07 Connect packs; **not** SM-8 production signing. |
| **Control** | Repo access; field provisioning uses TesPro/HITEKS path. |

### EJBCA lab PKI vs Trivy (same keys, duplicate paths)

**Source of truth:** keys issued by EJBCA into `apps/ccli/config/tls/ejbca/` (`scripts/issue_ejbca_cli_pki.py` · see `ejbca/README.md`).

**Deploy layout:** `scripts/sync-ejbca-tls-to-product.ps1` copies those PEMs/keys into `apps/ccli/config/tls/` (and field pack / DUT `etc/ccli/tls/`). Manifest: `apps/ccli/config/tls/PKI_SYNC_MANIFEST.json`.

| Check | Command / artefact | Result (2026-11-12) |
|-------|-------------------|---------------------|
| EJBCA → parent **byte match** | SHA256 per `*.key` in `ejbca/` vs parent `config/tls/` | **MATCH** on all synced roles (`server`, `server_acse`, `client*`, `revoked*`, `viewer`); `viewer_acse.key` only under `ejbca/` (not in sync list) |
| Annex / 62351 lab gates | `sync-ejbca-tls-to-product.ps1 -Verify` → `verify_annex_pki.py` | **PASS** (safe to stage/deploy) |
| Trivy repo — **EJBCA copies** | `trivy-apps-ccli.json` targets `config/tls/ejbca/*.key` | **12** findings |
| Trivy repo — **deploy copies** | same JSON, `config/tls/*.key` (non-`ejbca`) | **Duplicates** of the EJBCA material + aliases (`client_tsp.key` = copy of `client.key` per sync script) |
| Trivy repo — **P3-07 Connect pack** | `config/tls/p3_07_connect/*.key` | **Separate** conformance pack; not the EJBCA Annex set — still lab, still Trivy HIGH |
| Trivy DUT | `trivy-dut-rootfs.json` | **18** secrets: `etc/ccli/tls/*.key` **and** `etc/ccli/tls/ejbca/*.key` for the same lab PKI + `uhttpd` / LuCI keys |

**Interpretation for auditors:** Trivy does **not** distinguish EJBCA from OpenSSL or “production”; it flags **every private key file**. For CCI lab you **expect** the **same logical credentials** in three places on disk:

1. `…/tls/ejbca/` — issuance export  
2. `…/tls/` — sync target used by deploy  
3. `etc/ccli/tls/` on the DUT — runtime config  

So **high secret counts are normal**; disposition is **one** accepted lab PKI (EJBCA), not N distinct incidents.

**Re-check when DUT is online:**

```powershell
# After LAN1 pull (WSL extract present) or SSH:
. .\lab\tg544-openwrt\lab-env.ps1
& "C:\Program Files\PuTTY\plink.exe" -ssh root@192.168.10.1 -pw $env:CCLI_TG544_PW -hostkey SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE -batch "sha256sum /etc/ccli/tls/server.key /etc/ccli/tls/ejbca/server.key"
# Compare to repo:
certutil -hashfile apps\ccli\config\tls\ejbca\server.key SHA256
```

**TSP pairing (not Trivy):** `scripts/verify-tsp-tls-key-match.ps1` — cert ↔ key modulus for Test Suite Pro.

---

## Analysis — live DUT (r64-dut-lan1)

**Platform (from `dut-identity.txt`):**

| Field | Value |
|-------|--------|
| OS | **TesproOS** `32-2.1.0` · `r601-af70fcaaa2` |
| Kernel | Linux **6.12.68** aarch64 |
| **ccli** on device | **0.1.0-r64** · SHA256 `967df276…` (matches deploy manifest) |
| Pull host | `192.168.10.1` (LAN1) |

**Trivy `dut-rootfs` summary (`trivy-dut-rootfs.json`):**

| Scanner | Count | Notes |
|---------|------:|-------|
| **Vulnerabilities** | **0** | Trivy **apk** DB read (`lib/apk/db/installed`); **OS family unsupported** for TesproOS → no mapped platform CVE list in-tool |
| **Secrets** | **18** | All **HIGH** asymmetric private keys |

**Secret locations on DUT (accepted lab):**

- `etc/ccli/tls/*.key` (15 files, incl. `ejbca/` subset)
- `etc/luci-encrypt/private.pem`
- `etc/uhttpd.key`

**Lab note:** `apk list` still shows package **ccli-0.1.0-r51** while `/usr/sbin/ccli` is **r64** — binary updated without apk metadata bump; identity evidence uses **`ccli --version`** + SHA256.

**Platform CVE gap (explicit):** **SM-10** TesPro SDL / vendor scan remains the authoritative path for **OpenWrt/TesproOS** CVE closure; this DUT pull proves **pull + scan mechanics** and **app binary identity**, not full vendor SBOM/CVE sign-off.

---

## Auditor / course narrative

| Layer | Evidence | Status |
|-------|----------|--------|
| Tool + method | Trivy 0.76.2, vuln+secret | **HAVE** |
| Pinned release | 0.1.0-r64 + SHA256 | **HAVE** |
| Repo SCA (CVE) | apps-ccli **0 CVE** (r64-local 092220) | **PASS** |
| Repo secret detection | Lab TLS in git tree | **HAVE** (accepted) |
| Shipping binary clean | r64 ELF (local + on-DUT SHA) | **PASS** |
| Live rootfs sample | r64-dut-lan1 | **HAVE** |
| Platform OS CVEs | SM-10 TesPro | **GAP** (vendor deliverable) |

**Verdict (r64):** **SI-1 / SVV-3 course evidence complete** for HiTEKS scope. **No blocking CVE** on shipping **`ccli`** ELF. **Secrets** are **expected lab artefacts** on bench and in repo. **TesproOS CVE** tracking is **out of scope** for this EVD until SM-10 is linked.

---

## Maintenance

1. After each `CCLI_DEPLOY_LATEST` bump: run **local** scan; if DUT online on LAN1, run **scan-dut-lan1-trivy.ps1**; update this EVD revision.
2. Keep `scripts/trivy/.trivyignore` aligned with vendored trees (do not ignore shipping paths).
3. Optionally register a **CSEC** DM row when Jira automation is wired for SCA re-runs (manual OK for course).
