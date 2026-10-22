# PC1 — GitHub sync when hotspot blocks PC ↔ PC SSH

**Document ID:** CCLI-HANDOFF-PC1-SYNC-001  
**Revision:** 1.1  
**Date:** 2026-10-22  
**Audience:** PC-B (Test Suite Pro), logger PC (PC-A), dev with Cursor  
**Companion:** [PC1_GITHUB_PULL_RUNBOOK.md](PC1_GITHUB_PULL_RUNBOOK.md) · [TSP_TWO_PC_BENCH_RUNBOOK.md](../../lab/conformance/TSP_TWO_PC_BENCH_RUNBOOK.md)

---

## Repository (single canonical remote)

| Role | GitHub | Git |
|------|--------|-----|
| **Owner / push** | **`yahiagabereng-ctrl/CCLI`** (Nicola programme account) | `git push origin main` from dev or logger PC only |
| **PC1 bench (TestPro)** | Same repo — **pull read-only** | `git pull origin main` — do **not** push to a personal fork |
| **`handoff/masomeh95/`** | Folder name = bench operator handoff | **Not** a separate GitHub user or remote |

```text
origin  https://github.com/yahiagabereng-ctrl/CCLI.git  (fetch + push on owner machines)
```

PC1 may use any GitHub login that has **read** access (collaborator or shared PAT); commits always land on **`yahiagabereng-ctrl/CCLI`**, not Masomeh’s account.

---

## Situation

Two Windows PCs share one **mobile hotspot** (e.g. dev on **Wi‑Fi**, PC1 on **USB tether**). **Cursor Remote SSH** between them only works if both get IPs on the **same subnet** and the phone allows **client-to-client** traffic. Many hotspots **isolate** clients: internet works, **ping between laptops fails**.

**Programme default:** sync bench scripts, CID, yaml, and docs via **GitHub** (**push** on **owner** dev/logger → **`git pull`** on PC1). Do **not** commit TLS private keys, `lab/keys/`, or field-pack PEM trees.

---

## Roles (do not mix cables)

| PC | Typical NICs | Lab IP (examples) | Use for |
|----|--------------|-------------------|---------|
| **PC-B / PC1** | Hotspot + **LAN1 cable to DUT** | Hotspot `10.x.x.x` · LAN1 **`192.168.10.10/24`** | Test Suite Pro → **`192.168.10.1:3782`** |
| **PC-A / logger** | Hotspot + **Ethernet to DUT Eth_B** | Hotspot `10.x.x.x` · LAN2 **`192.168.1.183/24`** | SSH DUT **`192.168.1.130:22`**, deploy, `event-dump` |

From the **logger** PC: **`ping 192.168.1.130`** and **SSH :22** should PASS; **`192.168.10.1`** and **`:3782` on `.1.130`** are expected **FAIL** (MMS is on Eth_A only).

---

## Check hotspot peer reachability (optional)

On **each** PC:

```powershell
Get-NetIPAddress -AddressFamily IPv4 |
  Where-Object { $_.IPAddress -notlike '127.*' } |
  Format-Table InterfaceAlias, IPAddress -AutoSize
```

From dev PC, ping PC1’s hotspot IP. If **only the gateway** replies on a `/24` sweep, use **GitHub** (below), not SSH.

To use SSH later: enable **OpenSSH Server** on PC1, allow **TCP 22**, and fix hotspot isolation or use a small router.

---

## GitHub workflow (primary)

### Owner dev / logger (after changes — **yahiagabereng-ctrl**)

```powershell
cd C:\Yahia\projects\CCLI   # clone must point at yahiagabereng-ctrl/CCLI
git remote -v                # expect origin → github.com/yahiagabereng-ctrl/CCLI.git
git status
git add …                    # never add lab/keys, CCLI_TSP_FIELD_PACK/tls, passwords
git commit -m "…"
git push origin main         # owner credentials only
```

Note the tip: `git log -1 --oneline` (e.g. bench sync commits include `lab/conformance/ANNEX_OT_SW_GAP_MATRIX.md`, 5-LN CID, `lab/tg544-openwrt/ccli-bin`).

### PC1 (every TSP session)

```powershell
cd D:\CCLI\CCLI\CCLI
git fetch origin
git pull origin main
git log -1 --oneline
```

Then continue [PC1_GITHUB_PULL_RUNBOOK.md](PC1_GITHUB_PULL_RUNBOOK.md): `ensure-lab-env.ps1`, `pc1-p3-07-connect-preflight.ps1`, **`set_pc_lan1_dso.cmd`**, `ping 192.168.10.1`.

**GitHub auth on PC1:** Windows Credential Manager or a **PAT** — never store passwords in the repo or in chat.  
**First-time other PC:** [PC1_OTHER_DEVICE_SETUP.md](PC1_OTHER_DEVICE_SETUP.md) (clone, collaborator, USB field pack).

---

## Cursor on PC1

- Sign in with the **same Cursor account** as dev; **Settings Sync** does not move an Agent session.
- Open the **cloned repo** on PC1, or use **Remote SSH** only when hotspot peer ping works.
- **Annex O/T software gaps (5-LN):** [lab/conformance/ANNEX_OT_SW_GAP_MATRIX.md](../../lab/conformance/ANNEX_OT_SW_GAP_MATRIX.md)

---

## Fallback without git on PC1

```powershell
# On dev PC — stages TLS/CID/scripts to USB path
powershell -File scripts\stage-tsp-field-pack.ps1
```

Copy `CCLI_TSP_FIELD_PACK` to PC1. See [00_COPY_TO_OTHER_PC_OCT08.md](../../lab/tmp/TSP_OCT08_COPY_PACK/00_COPY_TO_OTHER_PC_OCT08.md).

---

## RAG

`ccli-lab-access-freeze` · `ccli-tsp-two-pc-bench-runbook` · `ccli-annex-ot-sw-gap-matrix`
