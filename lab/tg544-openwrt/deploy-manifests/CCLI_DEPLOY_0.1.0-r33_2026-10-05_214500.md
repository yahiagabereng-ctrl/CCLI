# CCLI deploy manifest — 0.1.0-r33

| Field | Value |
|-------|-------|
| **Date (UTC)** | 2026-10-05 19:45:00 UTC |
| **Version** | **0.1.0-r33** |
| **Codename** | P5-EMT432-TCP-PFSP |
| **SHA256** | PENDING_BUILD (run `wsl-build-ccli.sh`) |
| **Revert to** | **0.1.0-r32** — tag `42899bf8`, manifest `CCLI_DEPLOY_0.1.0-r32_2026-10-05_210600.*` |
| **Lab yaml** | `lab_tr400_emt432_1p.yaml` (EMT432 TCP) or existing cleartext yaml for RS485 bench |

## Changes

- PFSP MMS Operate + reactive derive (fixes incomplete PFSP in r32 tag)
- Modbus TCP (`LibmodbusTcp`) for EMT432 on LAN3
- P5 evidence, Chronos docs, handoff package

## Rollback to r32

1. `git checkout 0.1.0-r32`
2. Rebuild or redeploy r32-era `ccli-bin`
3. `/etc/init.d/ccli restart`
4. `ccli --version` → **0.1.0-r32**

Revert **r34** separately if the zone dashboard alone should be removed.
