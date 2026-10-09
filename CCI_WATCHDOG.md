# CCI Watchdog

This document records the watchdog work completed for CCI on the TesPro TR500/TG544 OpenWrt platform and the remaining production tasks.

## Implemented

### 1. CCI main-loop liveness tracking

The CCI main loop calls `ServiceSupervision::watchdog_main_loop_tick()` once per completed iteration. The timestamp uses `CLOCK_MONOTONIC`, so wall-clock changes do not affect the health decision.

The existing:

```cpp
++loop_count;
usleep(100000);
```

remains in place. It schedules the normal control loop at approximately 10 Hz and is not the watchdog itself.

### 2. Independent watchdog timer thread

`ServiceSupervision::start_watchdog()` starts a separate C++ timer thread. The thread:

- wakes every 30 seconds;
- checks the age of the last main-loop tick;
- sends the ubus `service event` watchdog heartbeat only when the main loop is healthy;
- treats a 90-second stale timestamp as a main-loop stall;
- logs the stall and raises `SIGABRT` so the existing fatal path runs and `procd` can respawn CCI.

`stop_watchdog()` joins the timer thread during a normal shutdown. The timer is linked with `Threads::Threads` in CMake.

### 3. OpenWrt service supervision

The CCI init script uses `procd` respawn settings and a configurable watchdog timeout:

```text
respawn 3600 5 5
watchdog timeout 90
```

The runtime configuration is stored in UCI as `ccli.main.watchdog_timeout`, with validation and a minimum accepted value of 60 seconds.

## Build and installation

The watchdog build was compiled for the TesPro MT798X/aarch64 target and signed with the CCLI release key.

The resulting package was:

```text
lab/tg544-openwrt/ccli-0.1.0-r34.apk
```

The package was verified with `apk-tools` and installed on the TesPro without `--allow-untrusted`. The installed trusted public key fingerprint was:

```text
SHA-256 dab241ed8f74f283fefb0ae6640902c26e8febb65c1df511fc5dbf7b924638c
```

## Tests completed

### Crash and exit recovery

- CCI termination and `SIGSEGV` tests caused the process to exit.
- `procd` respawned CCI with a new PID.
- The crash recovery log reported `previous crash recovered`.

### Main-loop stall recovery

The installed build was frozen with `SIGSTOP` for more than 90 seconds. After `SIGCONT`, the watchdog timer resumed, detected the stale main-loop timestamp, and logged:

```text
watchdog: CCI main loop stalled for 92442 ms; aborting for procd respawn
```

CCI then aborted and `procd` restarted it with a new PID. The service was confirmed running afterward.

## Important limitation

`SIGSTOP` suspends the entire process, including the watchdog thread. Therefore, a userspace timer cannot kill or restart a process while every thread is stopped. It can only detect the stale timestamp after the process resumes.

The TesPro `procd` service watchdog was also tested and did not independently terminate a process held in `SIGSTOP`. Consequently, the current design does **not** cover every possible process or kernel failure.

## Current recovery coverage

| Failure | Current result |
|---|---|
| CCI crashes | `procd` respawns CCI |
| CCI exits unexpectedly | `procd` respawns CCI |
| Main loop blocks while timer thread runs | CCI aborts after 90 s; `procd` respawns |
| Entire process is stopped with `SIGSTOP` | Not recoverable by CCI itself while stopped |
| Kernel/system-wide hang | Not covered by the CCI userspace timer |

## Remaining work

### 1. Independent watchdog supervisor

Create a separate signed package/service, for example `ccli-watchdog`, containing a small daemon that:

- monitors a CCI health record or heartbeat independently;
- uses a monotonic deadline and bounded I/O;
- restarts CCI when the heartbeat becomes stale;
- has its own `procd` respawn policy;
- does not depend on MQTT or cellular connectivity.

This supervisor must run as a separate process. Putting the watchdog code in the same CCI process cannot protect against a whole-process freeze.

### 2. Hardware watchdog integration

The device exposes `/dev/watchdog` and `/dev/watchdog0`. A production watchdog service should be designed and tested to feed the hardware watchdog only while the system and CCI health checks are valid. Ownership, timeout, `nowayout`, boot behavior, and safe-state behavior must be specified before enabling it in production.

### 3. Controlled recovery and safe state

Define what happens to PF2 and digital outputs during:

- CCI restart;
- watchdog timeout;
- hardware watchdog reset;
- loss of network or MQTT connectivity.

The recovery path must leave outputs in the approved safe state and record a durable diagnostic event.

### 4. Production testing

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
