# Second bench PC — first-time setup (share from owner device)

**Document ID:** CCLI-HANDOFF-PC1-SETUP-002  
**Revision:** 1.0  
**Date:** 2026-10-22  
**Owner repo:** `https://github.com/yahiagabereng-ctrl/CCLI.git`  
**Pin check after pull:** `git log -1 --oneline` should reach at least **`220b3f4`** (Oct 2026 bench sync).

Use this when the **Test Suite Pro PC** (PC-B) must match the **logger / dev PC** and **hotspot blocks PC ↔ PC SSH**.

---

## What you share (safe)

| Item | How the other PC gets it |
|------|---------------------------|
| **Code + docs + scripts** | `git clone` / `git pull` from **`yahiagabereng-ctrl/CCLI`** |
| **DUT SSH bench password** | After clone: `powershell -File scripts\ensure-lab-env.ps1` → creates `lab\tg544-openwrt\lab-env.ps1` from committed **example** (see [LAB_ACCESS_FREEZE.md](../../lab/LAB_ACCESS_FREEZE.md)) |
| **TSP TLS + CID + bench scripts** | `git pull` **or** USB copy of **`CCLI_TSP_FIELD_PACK`** from owner PC (`scripts\stage-tsp-field-pack.ps1`) |
| **GitHub login** | Sign in **on the other PC only** (browser or Git Credential Manager) — **never** commit passwords or paste them into the repo |

---

## What you never put in GitHub

- GitHub account **passwords** or **PATs** in files  
- `lab/keys/`, `lab-env.ps1` custom secrets (gitignored)  
- `handoff/.../CCLI_TSP_FIELD_PACK/tls/` private keys (use field pack USB or staged pack)

Share GitHub access **out of band** (in person, password manager, or **GitHub → Settings → Collaborators** on `CCLI` for the other person’s GitHub user).

---

## Path A — Other PC has Git (recommended)

### On owner PC (once per push)

```powershell
cd C:\Yahia\projects\CCLI
git push origin main
```

Tell the other operator the latest tip: `git log -1 --oneline`.

### On the other PC (first time)

```powershell
git clone https://github.com/yahiagabereng-ctrl/CCLI.git D:\CCLI\CCLI\CCLI
cd D:\CCLI\CCLI\CCLI
```

When Git asks for credentials:

1. **Best:** Other person’s GitHub user added as **collaborator** → they sign in with **their** account.  
2. **Same owner account on PC1:** Open https://github.com in Edge/Chrome on PC1, sign in as **`yahiagabereng-ctrl`**, then retry `git pull` (Windows often reuses the session).  
3. **PAT:** GitHub → Settings → Developer settings → Personal access tokens → fine-grained or classic **`repo` read** → paste **once** when `git` prompts (stored in Credential Manager).

### Every session on the other PC

```powershell
cd D:\CCLI\CCLI\CCLI
git pull origin main
powershell -File scripts\ensure-lab-env.ps1
powershell -File scripts\source-lab-env.ps1
```

Full TSP steps: [PC1_GITHUB_PULL_RUNBOOK.md](PC1_GITHUB_PULL_RUNBOOK.md).

---

## Path B — No Git on other PC (USB)

On **owner PC**:

```powershell
cd C:\Yahia\projects\CCLI
powershell -File scripts\stage-tsp-field-pack.ps1
```

Copy folder **`D:\CCLI_TSP_FIELD_PACK`** (or path in `$env:CCLI_TSP_FIELD_PACK`) to USB → other PC.

On other PC, read **`00_START_HERE_P3_07_CONNECT.md`** inside the pack.  
Repo docs on USB won’t auto-update — prefer Path A when internet works.

---

## Path C — Cursor on the other PC

1. Install Cursor; sign in (same or separate Cursor account).  
2. **File → Open folder** → `D:\CCLI\CCLI\CCLI` after clone.  
3. Network doc: [PC1_GITHUB_SYNC_AND_NETWORK.md](PC1_GITHUB_SYNC_AND_NETWORK.md).

---

## Verify both PCs match

| Check | Command (either PC) |
|-------|---------------------|
| Same commit | `git rev-parse HEAD` vs owner’s `git rev-parse origin/main` |
| DUT SSH env | `powershell -File scripts\verify-lab-access-freeze.ps1` |
| LAN1 (TSP PC only) | `lab\set_pc_lan1_dso.cmd` then `ping 192.168.10.1` |

---

## RAG

`ccli-pc1-github-sync-network` · `ccli-lab-access-freeze` · `ccli-pc1-github-sync-network`
