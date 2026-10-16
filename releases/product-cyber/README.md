# Product cyber release bundles

**Purpose:** Immutable **product** software drops (cyber profile on), promoted from **lab** evidence — not ad-hoc bench uploads.

**Normative doc:** [`knowledge-base/08-engineering/CCI_Product_SW_Freeze.md`](../../knowledge-base/08-engineering/CCI_Product_SW_Freeze.md) (`ccli-product-sw-freeze`)

---

## When to create a folder

Create **`releases/product-cyber/<semver>-rN>/`** only after:

- Lab phase gates for that **rN** are satisfied (minimum: **P3 MMS TLS** closed; target **P4** per programme plan).
- Reference DUT verification in **Product SW Freeze** § Verification passes with **product yaml** (TLS **:3782**, no cleartext **:102**).

---

## Folder layout (per release)

```text
releases/product-cyber/0.1.0-r52/
├── FREEZE.md              # Copy of promote record: git SHA, TesproOS, gates, sign-off
├── MANIFEST.json          # SHA256: apk, yaml, cid, cfg, signal_map rev
├── ccli-0.1.0-r52.apk     # Copy from lab/tg544-openwrt/ after build
├── config/
│   └── product_tr400.yaml # From lab_tr400_phase4_eth_b.yaml (site-specific edits)
├── icd/
│   ├── *.cid
│   └── *.cfg              # Must match mms.model_cfg in product yaml
├── tls/
│   └── README.md          # Cert roles only — never commit private keys
└── verification/
    └── CHECKLIST.md       # Tick § Verification from CCI_Product_SW_Freeze.md
```

---

## Lab vs product (do not mix)

| Artifact | Lab | Product bundle |
|----------|-----|----------------|
| Deploy manifest | `lab/tg544-openwrt/deploy-manifests/CCLI_DEPLOY_*.md` | Summarized in `MANIFEST.json` |
| Config | `lab_tr400_cleartext_tsp.yaml`, mocks, bypass | **Excluded** |
| MMS port | :102 allowed for TSP | **:3782** only |
| DUT path | `/etc/ccli/lab.yaml` | Same path today; content = product yaml |

---

## Version bump (before build)

1. `apps/ccli/VERSION`
2. `scripts/sync-ccli-version.ps1`
3. `apps/ccli/CHANGELOG.md`
4. `lab/tg544-openwrt/DEPLOY_CCLI.md` workflow

---

## RAG

Ingest after adding or updating `FREEZE.md`:

```powershell
powershell -File scripts\ingest-product-deploy-rag.ps1
powershell -File scripts\ingest-next-phases.ps1 -SkipDocker
```

Registry: `scripts/rag-product-deploy-sources.json` · index: `lab/conformance/RAG_SOURCE_INDEX.md`
