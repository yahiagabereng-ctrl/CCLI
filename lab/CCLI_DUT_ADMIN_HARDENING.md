# DUT admin hardening — LuCI off, SSH keys, single application config

**Document ID:** CCLI-LAB-ADMIN-HARDEN-001  
**Date:** 2026-10-20  
**Pairs with:** [LAB_ACCESS_FREEZE.md](LAB_ACCESS_FREEZE.md)

## Goal

| Item | Action |
|------|--------|
| LuCI / web UI | **Off** (`uhttpd`) — aligns with O.13.1.1.2 (no IP maintenance on plant/DSO ports) |
| TesPro `iec61850-mmsd` | **Off** — `ccli` owns `:3782` |
| SSH | **Public key** on DUT; optional **password auth off** |
| Application | **One yaml:** `apps/ccli/config/ccli_application.yaml` → `/etc/ccli/ccli_application.yaml` |

TLS for MMS/104 remains under `/etc/ccli/tls/` (not merged into yaml).

## One-time on PC

```powershell
# Generate key (OpenSSH)
ssh-keygen -t ed25519 -f $env:USERPROFILE\.ssh\ccli_lab_ed25519 -C "ccli-lab-deploy"

# lab-env.ps1 (gitignored):
#   $env:CCLI_SSH_KEY = "$env:USERPROFILE\.ssh\ccli_lab_ed25519"
#   $env:CCLI_SSH_PUBKEY = "$env:USERPROFILE\.ssh\ccli_lab_ed25519.pub"
```

## Apply on DUT (LAN2)

```powershell
cd CCLI
. .\lab\tg544-openwrt\lab-env.ps1

# 1) Single application config + UCI pointer
powershell -File scripts\push-ccli-application-config.ps1 -RestartCcli

# 2) LuCI off + install pubkey (password still works)
powershell -File scripts\harden-dut-admin.ps1 -PubKeyPath $env:CCLI_SSH_PUBKEY

# 3) Test key login, then disable password auth
powershell -File scripts\harden-dut-admin.ps1 -PubKeyPath $env:CCLI_SSH_PUBKEY -DisablePasswordAuth
```

## Verification

```powershell
. .\scripts\Invoke-CcliPlink.ps1
Invoke-CcliPlink -HostAddr 192.168.1.130 -Command "uci get ccli.main.config_file; ps w | grep uhttpd | grep -v grep || echo no_uhttpd"
```

From browser: `http://192.168.1.130` should **not** serve LuCI.

## Rollback (bench)

- Re-enable uhttpd: `ssh root@DUT '/etc/init.d/uhttpd enable; /etc/init.d/uhttpd start'`
- Password auth: `uci set dropbear.@dropbear[0].PasswordAuth=on; uci commit dropbear; /etc/init.d/dropbear restart`

## Scripts

| Script | Role |
|--------|------|
| `scripts/push-ccli-application-config.ps1` | Canonical yaml + UCI |
| `scripts/harden-dut-admin.ps1` | LuCI/mmsd + SSH |
| `scripts/Invoke-CcliPlink.ps1` | Key or password plink/pscp |
