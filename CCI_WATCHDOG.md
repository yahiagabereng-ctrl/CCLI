# CCI Watchdog

This document records the watchdog work completed for CCI on the TesPro TR500/TG544 OpenWrt platform and the remaining production tasks.

## Implemented

### 1. CCI main-loop liveness tracking

The CCI main loop calls `ServiceSupervision::watchdog_main_loop_tick()` after each completed iteration.
Every 250 ms it atomically publishes its PID, sequence and `CLOCK_MONOTONIC` timestamp to
`/var/run/ccli-health`, so wall-clock changes do not affect the health decision.

The existing:

```cpp
++loop_count;
usleep(100000);
```

remains in place. It schedules the normal control loop at approximately 10 Hz and is not the watchdog itself.

### 2. Independent watchdog supervisor

`ccli-watchdog` is a separately packaged C process, started and respawned by `procd`. It:

- wakes every second;
- checks the age of CCI's monotonic health record;
- at 8 seconds stale, sends `SIGABRT` so CCI's existing fatal safe-state path runs;
- after a two-second grace period, uses `SIGKILL` if CCI is still frozen; and
- leaves `procd` to respawn CCI.

This process is independent of CCI, so it can recover a whole-process freeze such as `SIGSTOP`.

### 3. OpenWrt service supervision

The CCI init script retains `procd` respawn settings and a secondary service heartbeat timeout:

```text
respawn 3600 5 5
watchdog timeout 90
```

The 90-second `procd` setting is deliberately secondary; the safety-relevant CCI restart
threshold is `ccli.watchdog.stale_timeout_s`, which defaults to 8 seconds. It is below the
existing PF2 10-second stale-data bound and above the configured Modbus retry window.

## Build and installation

The source produces two signed packages: `ccli.apk` and `ccli-watchdog.apk`. The supervisor has
its own executable and init script, but depends on `ccli.apk`; both must be installed together.
No new watchdog package has been deployed to the DUT yet.

`lab/tg544-openwrt/build-ccli-ipk-sdk.sh` builds the controller APK and
`lab/tg544-openwrt/build-ccli-watchdog-apk.sh` builds the watchdog APK for the TesPro SDK target;
each signs its output with the manufacturer release key. For an offline update, pass both APK paths to
`scripts/new-firmware-update-bundle.ps1`; the signed manifest then carries one SHA-256 entry for
each APK. The watchdog APK post-install script owns its own enable/start sequence because the
updater's special restart handling remains scoped to `ccli`.

## Tests completed

### Crash and exit recovery

- CCI termination and `SIGSEGV` tests caused the process to exit.
- `procd` respawned CCI with a new PID.
- The crash recovery log reported `previous crash recovered`.

### Historical in-process stall test

The previous in-process design detected a `SIGSTOP` only after `SIGCONT`. That observation
motivated the separate `ccli-watchdog` process. The new 8-second supervisor has compiled on the
host, but remains to be tested on the TG544.

## Important limitation

`SIGSTOP` suspends the CCI process, but not the separate `ccli-watchdog` process. The supervisor
can therefore send `SIGKILL` after its safe-abort grace period and allow `procd` to respawn CCI.
This still does not cover a kernel/system-wide hang; that requires the unimplemented hardware
watchdog integration.

## Current recovery coverage

| Failure | Current result |
|---|---|
| CCI crashes | `procd` respawns CCI |
| CCI exits unexpectedly | `procd` respawns CCI |
| Main loop blocks | `ccli-watchdog` acts after 8 s; `procd` respawns |
| Entire process is stopped with `SIGSTOP` | `ccli-watchdog` sends SIGKILL after the 2 s grace period |
| Kernel/system-wide hang | Not covered until hardware WDT integration is validated |

## Remaining work

### 1. Hardware watchdog integration

The device exposes `/dev/watchdog` and `/dev/watchdog0`. A production watchdog service should be designed and tested to feed the hardware watchdog only while the system and CCI health checks are valid. Ownership, timeout, `nowayout`, boot behavior, and safe-state behavior must be specified before enabling it in production.

### 2. Controlled recovery and safe state

Define what happens to PF2 and digital outputs during:

- CCI restart;
- watchdog timeout;
- hardware watchdog reset;
- loss of network or MQTT connectivity.

The recovery path must leave outputs in the approved safe state and record a durable diagnostic event.

### 3. Production testing

Repeat tests with:

- a deliberately blocked Modbus operation;
- a blocked MMS/network operation;
- a deadlocked main loop with other threads still scheduled;
- loss of the CCI health record;
- hardware watchdog feeding disabled;
- power-cycle and reboot recovery.

Record restart latency, output state, logs, and boot-count behavior for each test.

## Recommended production architecture

Keep the functions separate:

- `ccli.apk`: control logic, Modbus, PF2, MMS, and I/O;
- `ccli-watchdog.apk`: independent health monitor and restart supervisor;
- `ccli-telemetry.apk`: MQTT publishing;
- `ccli-updater.apk`: HTTPS package download and signature verification.

MQTT and HTTPS must not be prerequisites for local safety recovery. APK updates must be verified against the installed trusted public key before installation and should be performed with bounded, rollback-safe procedures.
