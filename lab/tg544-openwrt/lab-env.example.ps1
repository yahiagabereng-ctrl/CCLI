# FROZEN lab access — copy to lab-env.ps1 on EVERY bench PC (logger + Test Suite Pro).
#   powershell -File scripts\ensure-lab-env.ps1
# Normative: lab/LAB_ACCESS_FREEZE.md
#
# lab-env.ps1 is gitignored; this example IS the committed freeze (TG544 bench only).

# DUT SSH (root @ 192.168.10.1 / 192.168.1.130)
# Prefer key auth after: scripts\harden-dut-admin.ps1 -PubKeyPath ... -DisablePasswordAuth
$env:CCLI_TG544_PW = '000000'
# OpenSSH private key or PuTTY .ppk (plink -i). Pubkey for DUT: matching .pub or CCLI_SSH_PUBKEY
# $env:CCLI_SSH_KEY = "$env:USERPROFILE\.ssh\ccli_lab_ed25519"
# $env:CCLI_SSH_PUBKEY = "$env:USERPROFILE\.ssh\ccli_lab_ed25519.pub"
# $env:CCLI_TG544_HOSTKEY = 'SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE'

# Test Suite Pro field pack root (PC-B)
$env:CCLI_TSP_FIELD_PACK = 'D:\CCLI_TSP_FIELD_PACK'

# Optional: full git clone on TSP PC
# $env:CCLI_REPO_ROOT = 'D:\CCLI\CCLI\CCLI'
