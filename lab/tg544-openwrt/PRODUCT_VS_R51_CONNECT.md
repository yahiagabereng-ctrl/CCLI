# Product (r55) vs GitHub r51 — why Connect differs

**Question:** Same TestPro certificates; **r51 connected**; **product deploy** → Initiate Response Failed.

**Short answer:** The product path usually changes **three things at once** (firmware + ICD model + sometimes PKI). Your DUT logs on r55 still show **transport TLS PASS** then **no AARQ** — the same **Type A** pattern as a **TSP/CID/MMS-key** abort, not an ACSE reject from new firmware. Separate **binary** from **yaml/TLS/model** before blaming r55 code.

---

## 1. Binary (what actually runs)

| Build | How it gets on DUT | Runtime identity | Connect role |
|-------|-------------------|------------------|--------------|
| **GitHub r51** | `apk add ccli-0.1.0-r51.apk` | `0.1.0-r51` **P3-07-AARE-SIGN-RAW-GT** | Oct-08 TSP PASS reference |
| **Product r55** | `deploy-product-fw-same-cyber.ps1` → `/tmp/ccli.new` | `0.1.0-r55` **P5-5LN-CERT-MODEL** | Cert / 5-LN bench line |

| SHA-256 | Size | Notes |
|---------|------|--------|
| APK `0a2d2782…` | 997024 (package) | [p3-07-security-pass](https://github.com/yahiagabereng-ctrl/CCLI/releases/tag/p3-07-security-pass) |
| `e192d3cc…` (`ccli-bin`) | 7802088 | Current workspace product binary |
| `2b166430…` | 7805952 | Oct-09 hand-deploy (PASS era); not in git |
| `18823014…` (`ccli-bin.r51`) | 7776624 | **Not r51** — strings say **`0.1.0-r30`** |

**Entry point (source tree):**

| Release | `ccli` main |
|---------|-------------|
| **r51** (`4c73add`) | `services/ccli_main.cpp` |
| **r55** (today) | `program/main.c` + `ccli_program` supervisor |

Both call the same **`MmsAdapter::start()`** (via `MmsService` / `CcliBridge_Mms_Start`). MMS/TLS/ACSE logic is shared; the supervisor loop differs.

---

## 2. Code delta r51 → r55 that touches MMS (git `4c73add..HEAD`)

| Area | Change | Connect impact |
|------|--------|----------------|
| `mms_adapter.cpp` | **TLS 1.2 max** (62351-3 PIXIT) | Low if TSP uses TLS 1.2 |
| `mms_adapter.cpp` | **5-LN:** VArSd / PFSP optional (boot with 5-LN cfg) | Does **not** explain pre-AARQ disconnect |
| `ccli_acse_bridge.c` | AARE builder hooks for patched `acse.c` | Needed for **AARE [10]EXTERNAL** after AARQ arrives |
| `program/main.c` | Product cyber gate (refuse cleartext :102) | Irrelevant when yaml has TLS :3782 |

There is **no** large r55 change that rejects clients at TLS while logging **`DC:9A:9E…`** allowlist match.

---

## 3. Config / deploy bundle (usual break)

Product deploy typically applies **all** of:

| Item | r51 PASS path | Product / unified path |
|------|---------------|-------------------------|
| **Firmware** | r51 APK | r55 `ccli-bin` |
| **MMS model** | `lab_tg544_eth_a.cfg` | Often **`product_tr400_eth_a_5ln.cfg`** |
| **TSP CID** (unchanged) | **`lab_tg544_eth_a.cid`** | Same file on PC |
| **TLS dir** | `tls/p3_07_connect`, **crl off** | Sometimes **`tls/` EJBCA + CRL** |

**CID/model mismatch:** TestPro still on **lab CID** but DUT on **5-LN model** → Initiate fails **before** DUT logs `ACSE auth ACCEPT` (Type A in `DUT_LOG_ERROR_ANALYSIS_2026-10-16.md`).

**PKI mismatch:** EJBCA `root_CA.pem` on DUT vs P3-07 CA on TestPro → TLS or trust failures (your recent captures show **P3-07 transport OK**, so this was fixed when `p3_07_connect` was restored).

Yaml **MMS TLS block** is the same shape between `lab_tr400_phase4_eth_b_ttyS1_p3_07_connect.yaml` and product bench yaml; difference is **`model_cfg`** and plant/modbus, not port or allowlist paths.

---

## 4. Log signatures

**Failing (r55 product deploy — 2026-10-16):**

```text
TLS: serial DC:9A:9E:26:D5:47:EF:38   ← transport OK
mms: client CONNECT count=1
mms: client DISCONNECT count=0
(no mms: ACSE auth ACCEPT)
```

**Oct-08 PASS (r51):**

```text
mms: ACSE auth ACCEPT — mech=… role=DSO_OPERATOR
mms: AARE responder auth [10]EXTERNAL …
```

So r55 is **not** logging ACSE reject; the client drops **after** TLS. That matches **wrong MMS cert/key or CID** on TestPro **or** CID≠model on DUT — not a proven r55 regression in ACSE code.

---

## 5. Fair A/B test (isolate firmware)

Hold **constant** on DUT:

- `/etc/ccli/tls/*` from **`p3_07_connect`**
- `/etc/ccli/lab.yaml` = **`lab_tr400_phase4_eth_b_ttyS1_p3_07_connect.yaml`**
- `/etc/ccli/icd/lab_tg544_eth_a.cfg`

Then only swap firmware:

1. `deploy-r51-github-p3-07-connect.ps1`
2. Connect → note PASS/FAIL + log
3. `deploy-product-fw-same-cyber.ps1` with **same** yaml (or yaml-only after r55 binary)
4. Connect again

- If **(1) PASS, (2) FAIL** → file a firmware bisect (`-DCCLI_LEGACY_MAIN=ON` rebuild at r55, or compare APK-extracted ELF SHA to `ccli-bin`).
- If **both FAIL** → fix **TestPro MMS slot + lab CID** (see `handoff/masomeh95/00_START_HERE_P3_07_CONNECT.md`); firmware is not the differentiator.

---

## 6. Commands

```powershell
# GitHub r51 + P3-07 yaml/TLS/model
powershell -File lab\tg544-openwrt\deploy-r51-github-p3-07-connect.ps1 -HostAddr 192.168.1.130

# Product r55 (changes binary — keep TestPro on p3_07 only)
powershell -File lab\tg544-openwrt\deploy-product-fw-same-cyber.ps1 -HostAddr 192.168.1.130
```

DUT check:

```bash
/usr/sbin/ccli --version
sha256sum /usr/sbin/ccli
grep model_cfg /etc/ccli/lab.yaml
logread -e ccli | tail -30
```

See also: [`BINARY_IDENTITY.md`](BINARY_IDENTITY.md) · [`DEPLOY_PRODUCT_KEEP_TSP.md`](DEPLOY_PRODUCT_KEEP_TSP.md)
