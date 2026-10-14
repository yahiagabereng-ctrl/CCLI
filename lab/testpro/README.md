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
| **CID for cert run (Connect + Compare U-T1-02/03)** | `apps/ccli/config/icd/product/product_tr400_eth_a_5ln.cid` — **DUT model since r54** | `cid\product_tr400_eth_a_5ln.cid` |
| **31-LN lab CID (fallback yaml only)** | `apps/ccli/config/icd/lab_tg544_eth_a.cid` | `cid\lab_tg544_eth_a.cid` |
| **Trust anchor** | `apps/ccli/config/tls/root_CA.pem` | `tls\root_CA.pem` |
| **MMS Security (Cert B)** | `apps/ccli/config/tls/client.pem` + `client_tsp.key` | `tls\` |
| **Transport TLS (Cert A)** | `apps/ccli/config/tls/client_tls.pem` + same key | `tls\` |
| **CRL / revoked (T3-04)** | `revoked.pem`, `revoked_tls.pem`, `revoked_tsp.key`, `lab_crl.pem` | `tls\` |
| **Viewer (T3-02/T3-03)** | `tls\viewer.pem` + `viewer.key` | `tls\` |

**PKI source:** After EJBCA publish, PEMs in git are **`apps/ccli/config/tls/`** (see `PKI_SYNC_MANIFEST.json`). Regenerate: `scripts/publish-ejbca-lab-pki.ps1` on dev PC, commit TLS + redeploy DUT, then PC1 `git pull`.

**DUT (Yahia deploy 2026-10-14):** `ccli 0.1.0-r54` · MMS **`192.168.10.1:3782`** TLS · model **`product_tr400_eth_a_5ln.cfg`** (**5-LN product**, configRev **20261014**). Browse `LD_Plant` shows exactly **LLN0, LPHD1, PdCMMXU1, WlimDWMX1, WSdDAGC1**.

**IED name:** `CCI016_01` · **Access point:** `accessPoint1`

Pin: [LAB_SPECIMEN_VERSION.md](../conformance/LAB_SPECIMEN_VERSION.md) (CID History **rev 9**) · [LAB_UNIFIED_SESSION_PIN_2026-10-14.md](../conformance/LAB_UNIFIED_SESSION_PIN_2026-10-14.md)

Machine-readable map: [tsp_tls_and_cid.json](tsp_tls_and_cid.json)
