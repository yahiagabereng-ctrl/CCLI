# Deploy revision recap — accurate TG544 uploads

Use this when you need a **named lab profile** (pre–5LN r51 vs P3-07 Connect vs unified 5-LN cert), not “whatever is in `ccli-bin` today”.

**Pins:** [`revision-pins/`](revision-pins/) · **Per-upload record:** [`deploy-manifests/`](deploy-manifests/) · **Access (SSH + TSP TLS):** [`LAB_ACCESS_FREEZE.md`](../LAB_ACCESS_FREEZE.md)

---

## Requirements (every deploy)

| ID | Requirement | Pass criteria |
|----|-------------|---------------|
| REQ-VER-001 | `apps/ccli/VERSION` semver + **release** + codename | Matches intended **0.1.0-rN** |
| REQ-VER-002 | `scripts/sync-ccli-version.ps1` | `package/ccli/Makefile` `PKG_RELEASE:=N` |
| REQ-VER-003 | `apps/ccli/CHANGELOG.md` entry | Describes this upload |
| REQ-BLD-001 | Build for **aarch64** OpenWrt | `lab/tg544-openwrt/ccli-bin` updated |
| REQ-BLD-002 | Version gate | `deploy-ccli-session-fix.ps1` runs WSL `ccli-bin --version` or strings match **rN** |
| REQ-MAN-001 | Manifest before upload | `write-deploy-manifest.ps1` → `CCLI_DEPLOY_*` + `.json` |
| REQ-DUT-001 | Single MMS owner | `iec61850service` / `iec61850-mmsd` off; **ccli** on **3782** / **2404** |
| REQ-DUT-002 | Post-deploy identity | DUT: `ccli --version`, `sha256sum /usr/sbin/ccli` = manifest |

**Do not** treat “we copied a file named r51” as r51 until **REQ-BLD-002** and DUT SHA match a manifest.

---

## Lab profiles (pick one)

### 1 — `R51-P3-07-SOURCE` (code **before** 5-LN / cert-model)

| Field | Value |
|-------|--------|
| **When** | Reproduce Oct-08 TSP MMS pass firmware from **source**, not current `main` binary |
| **Git pin** | **`4c73add`** — `Release 0.1.0-r51: TSP P3-07 MMS Security association pass` |
| **VERSION** | `0.1.0` / **51** / `P3-07-AARE-SIGN-RAW-GT` |
| **Reference DUT SHA** | `2b166430…` · **7 805 952** B (Oct-09 manifest; may require rebuild, not git blob at `4c73add`) |
| **Yaml (at pin)** | `lab_tr400_phase4_eth_b.yaml` |
| **Tls** | `apps/ccli/config/tls/` (EJBCA/lab at commit time) |
| **ICD** | `lab_tg544_eth_a.cfg` |
| **Network** | LAN1 logger `192.168.10.20` → DUT `192.168.10.1` · [`set_pc_lan1_logger.cmd`](../set_pc_lan1_logger.cmd) |

```powershell
git checkout 4c73add -- apps/ccli package/ccli   # or full worktree at pin
# Set VERSION 51 / codename, sync, build, manifest, deploy
```

Pin file: [`revision-pins/R51-P3-07-SOURCE.json`](revision-pins/R51-P3-07-SOURCE.json)

---

### 2 — `P3-07-TSP-CONNECT` (Connect repro on **current** tree)

| Field | Value |
|-------|--------|
| **When** | Masomeh / PC1 pack; **crl=off**; dedicated **`p3_07_connect`** PEM set |
| **Git (profile added)** | **`4c0d6b2`** · MMS fixes from **`4c73add`** |
| **Firmware** | Usually **`0.1.0-r55`** rebuilt on `main` (AARE stack retained) or rebuilt **r51** |
| **Yaml** | `lab_tr400_phase4_eth_b_ttyS1_p3_07_connect.yaml` |
| **Tls** | `apps/ccli/config/tls/p3_07_connect/` |
| **ICD** | `lab_tg544_eth_a.cfg` |
| **Verify** | `scripts/verify-dut-p3-07-connect-alignment.ps1` |
| **Guide** | [`LAN1_LOGGER_OCT08_DEPLOY.md`](../LAN1_LOGGER_OCT08_DEPLOY.md) |

```powershell
. .\lab\tg544-openwrt\lab-env.ps1
powershell -File scripts\deploy-from-revision-pin.ps1 -Profile P3-07-TSP-CONNECT
```

---

### 3 — `PRODUCT-5LN-CERT` (ship yaml on DUT — cert specimen)

| Field | Value |
|-------|--------|
| **When** | Lab DUT must match **product** config delivered to accredited test (not bench RTU mock yaml) |
| **Firmware** | **`0.1.0-r55`** · `P5-5LN-CERT-MODEL` |
| **Yaml** | **`product_tr400.yaml`** → `/etc/ccli/lab.yaml` |
| **Plant** | Modbus **TCP** EMT432 **192.168.178.250** (LAN3) · **`use_dso_mock: false`** |
| **ICD** | `product/product_tr400_eth_a_5ln.cfg` |
| **Verify** | `validate-dut-unified-matrix.ps1` (**U-PROD-001/002**) |

```powershell
powershell -File scripts\deploy-from-revision-pin.ps1 -Profile PRODUCT-5LN-CERT -HostAddr 192.168.1.130
```

Pin: [`revision-pins/PRODUCT-5LN-CERT.json`](revision-pins/PRODUCT-5LN-CERT.json)

---

### 4 — `UNIFIED-5LN-CERT` (matrix / Compare / cyber rows — bench RTU yaml)

| Field | Value |
|-------|--------|
| **When** | Unified cert matrix, 5-LN product specimen, **crl=on** |
| **Firmware** | **`0.1.0-r55`** · `P5-5LN-CERT-MODEL` |
| **Yaml** | `lab_tr400_phase4_eth_b_ttyS1_5ln.yaml` |
| **Tls** | `apps/ccli/config/tls/` (EJBCA) |
| **ICD** | `product/product_tr400_eth_a_5ln.cfg` (+ CID in repo) |
| **CID revision** | After SCL edits: `scripts/bump-5ln-cid-revision.ps1` → validate → update `PRODUCT_SPECIMEN_5LN.md` |
| **Verify** | `scripts/validate-dut-unified-matrix.ps1 -FixServices -WriteEvidence` |
| **Pin doc** | [`LAB_UNIFIED_SESSION_PIN_2026-10-14.md`](../conformance/LAB_UNIFIED_SESSION_PIN_2026-10-14.md) |

```powershell
powershell -File scripts\deploy-from-revision-pin.ps1 -Profile UNIFIED-5LN-CERT -HostAddr 192.168.1.130
```

---

## Standard deploy pipeline

```mermaid
flowchart LR
  A[Pick profile pin] --> B[VERSION + sync + CHANGELOG]
  B --> C[Build ccli-bin]
  C --> D[write-deploy-manifest]
  D --> E[deploy-ccli-session-fix.ps1]
  E --> F[Profile verify script]
```

1. `powershell -File scripts\deploy-from-revision-pin.ps1 -Profile <ID> -WhatIf`
2. Fix VERSION / build until `-WhatIf` is green.
3. Run without `-WhatIf` (sets `$env:CCLI_TG544_PW` via `lab-env.ps1`).
4. Archive: copy manifest + optional `ccli-bin.rN` beside [`deploy-manifests/`](deploy-manifests/) if rolling `ccli-bin` will change again.

---

## Known failure modes

| Symptom | Cause | Fix |
|---------|--------|-----|
| `ccli --version` → **r30** after “r51 deploy” | Wrong bytes in `ccli-bin` / git blob at `4c73add` | Rebuild at **release 51**; match SHA to manifest |
| TLS OK, no `iso session CONNECT` | PC1 wrong cert slot / OEM on 3782 | `p3_07_connect` + disable **iec61850service** |
| Compare vs Connect confusion | 5-LN yaml + p3_07 tls mixed | Pick **one** profile pin; redeploy full tls + yaml |
| APK says rXX, binary says rYY | Hand `pscp` without `apk add` | Install IPK or always verify **/usr/sbin/ccli** |

---

## Interface matrix (profiles)

| Interface | R51 source | P3-07 Connect | Product 5-LN | Unified 5-LN bench |
|-----------|------------|---------------|--------------|-------------------|
| MMS TLS :3782 Eth_A | ✓ | ✓ crl=off | ✓ crl=on | ✓ crl=on |
| IEC 104 :2404 Eth_B | ✓ | ✓ | ✓ TLS | ✓ TLS |
| DUT yaml | phase4 | p3_07_connect | **product_tr400.yaml** | ttyS1_5ln.yaml |
| Model | 31-LN lab cfg | 31-LN lab cfg | 5-LN product cfg | 5-LN product cfg |
| TSP PC | 192.168.10.10 | 192.168.10.10 | matrix doc | matrix doc |

---

## Verification

| Profile | Script | Pass |
|---------|--------|------|
| R51 / build | `verify-dut-ccli-install.ps1` | Version + SHA = manifest |
| P3-07 | `verify-dut-p3-07-connect-alignment.ps1` | TLS MD5 + yaml flags |
| 5-LN | `validate-dut-unified-matrix.ps1` | Preflight evidence PASS |

**RAG / freeze:** `knowledge-base/08-engineering/CCI_Product_SW_Freeze.md` — product bundles under `releases/product-cyber/` after lab gates.
