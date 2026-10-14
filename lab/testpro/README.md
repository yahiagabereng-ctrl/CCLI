# Test Suite Pro — PC1 configuration (CID + certificates)

**Audience:** PC-B (`192.168.10.10`) · Triangle MicroWorks **61850 Test Suite Pro**  
**RAG source_id:** `ccli-lab-testpro-pc1-config`  
**Quick start:** [PC1_TSP_IMPORT_CHECKLIST.md](PC1_TSP_IMPORT_CHECKLIST.md)

After `git pull`, run from repo root:

```powershell
powershell -File scripts\setup-pc1-testpro-from-git.ps1
```

That builds `D:\CCLI_TSP_FIELD_PACK\` (or `-Dest`) with **TLS**, **CID**, bench scripts, and matrix docs.

---

## What to load in Test Suite Pro (2026-10-14 session)

| Item | Repo path (after clone) | Field pack copy |
|------|-------------------------|-----------------|
| **CID (Connect + interim Compare)** | `apps/ccli/config/icd/lab_tg544_eth_a.cid` | `cid\lab_tg544_eth_a.cid` |
| **5-LN cert CID (Compare U-T1-02)** | `apps/ccli/config/icd/product/product_tr400_eth_a_5ln.cid` | `cid\` when present |
| **Trust anchor** | `apps/ccli/config/tls/root_CA.pem` | `tls\root_CA.pem` |
| **MMS Security (Cert B)** | `apps/ccli/config/tls/client.pem` + `client_tsp.key` | `tls\` |
| **Transport TLS (Cert A)** | `apps/ccli/config/tls/client_tls.pem` + same key | `tls\` |
| **CRL / revoked (T3-04)** | `revoked.pem`, `revoked_tls.pem`, `revoked_tsp.key`, `lab_crl.pem` | `tls\` |
| **Viewer (T3-02/T3-03)** | `tls\viewer.pem` + `viewer.key` | `tls\` |

**PKI source:** After EJBCA publish, PEMs in git are **`apps/ccli/config/tls/`** (see `PKI_SYNC_MANIFEST.json`). Regenerate: `scripts/publish-ejbca-lab-pki.ps1` on dev PC, commit TLS + redeploy DUT, then PC1 `git pull`.

**DUT (Yahia deploy):** `ccli 0.1.0-r53` · MMS **`192.168.10.1:3782`** TLS · model **`lab_tg544_eth_a.cfg`** (31-LN lab) until 5-LN product CID is on DUT.

**IED name:** `CCI016_01` · **Access point:** `accessPoint1`

Pin: [LAB_SPECIMEN_VERSION.md](../conformance/LAB_SPECIMEN_VERSION.md) (CID History **rev 9**) · [LAB_UNIFIED_SESSION_PIN_2026-10-14.md](../conformance/LAB_UNIFIED_SESSION_PIN_2026-10-14.md)

Machine-readable map: [tsp_tls_and_cid.json](tsp_tls_and_cid.json)
