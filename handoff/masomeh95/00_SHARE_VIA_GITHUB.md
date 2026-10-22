# SHARE VIA GITHUB — other bench PC (start here)

**Owner:** Nicola · **`yahiagabereng-ctrl/CCLI`**  
**Web:** https://github.com/yahiagabereng-ctrl/CCLI  
**Branch:** `main`  
**Verify after pull:** `git log -1 --oneline` → should show **`9076f57`** or newer (includes **`product/`** 5-LN tree).

---

## Send this link to the other device

```
https://github.com/yahiagabereng-ctrl/CCLI
```

They need **internet** (mobile hotspot is fine). They do **not** need SSH between PCs.

---

## On the other PC — copy/paste

**First time:**

```powershell
git clone https://github.com/yahiagabereng-ctrl/CCLI.git D:\CCLI\CCLI\CCLI
cd D:\CCLI\CCLI\CCLI
git pull origin main
```

Sign in when Git or the browser asks:

- Same owner account **`yahiagabereng-ctrl`**, or  
- Their GitHub user if you added them under **Repo → Settings → Collaborators**.

**Every bench day:**

```powershell
cd D:\CCLI\CCLI\CCLI
git fetch origin
git pull origin main
git log -1 --oneline
powershell -File scripts\ensure-lab-env.ps1
powershell -File scripts\pc1-p3-07-connect-preflight.ps1
```

**TestPro LAN1:**

```powershell
D:\CCLI\CCLI\CCLI\lab\set_pc_lan1_dso.cmd
ping 192.168.10.1
```

---

## On owner PC (this machine) before they pull

```powershell
cd C:\Yahia\projects\CCLI
git push origin main
```

---

## More detail

| Doc | Purpose |
|-----|---------|
| [PC1_OTHER_DEVICE_SETUP.md](PC1_OTHER_DEVICE_SETUP.md) | Auth, PAT, USB fallback |
| [PC1_GITHUB_PULL_RUNBOOK.md](PC1_GITHUB_PULL_RUNBOOK.md) | TSP Connect + TLS |
| [PC1_GITHUB_SYNC_AND_NETWORK.md](PC1_GITHUB_SYNC_AND_NETWORK.md) | Hotspot · no peer SSH |
| [README_FOR_MASOMEH.md](README_FOR_MASOMEH.md) | Full handoff index |
| [PC1_PRODUCT_5LN_CURSOR_SETUP.md](PC1_PRODUCT_5LN_CURSOR_SETUP.md) | **Product tree + 5-LN** on PC1 / Cursor |

**Never** commit GitHub passwords or TLS private keys to the repo.
