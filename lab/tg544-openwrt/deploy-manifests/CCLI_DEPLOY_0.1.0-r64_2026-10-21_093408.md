# CCLI deploy manifest — 0.1.0-r64

| Field | Value |
|-------|-------|
| Date (UTC) | 2026-10-21 09:34 |
| Target DUT | Tespro TG544 at 192.168.0.1 |
| Version | 0.1.0-r64 |
| Codename | CEI57-142-AGGREGATOR |
| Artifact | apps/firmware-updater/artifacts/ccli-0.1.0-r64.apk |
| SHA-256 | 2694837eed4cdd59e00335e31e6b5508b3b050a4a4cac02906de82a9af380efb |
| Size | 1,087,028 bytes |
| Install method | SSH package install, explicitly requested |
| Active RBAC | Profile A, sequence 4 |
| Policy digest | 5f4b7dde602f7ce20bc5004de633f8098404a9f5aeca000ebc4db1e337dfba8b |
| Runtime | PID 20894, listener 192.168.0.1:3782 |

The canonical firmware toolkit verified the manufacturer signature before installation. The IEC 61850 client passed mutual TLS/Profile A association, Aggregator reads, dataset access, P/Q control acceptance and readback, P/Q mode transitions, and the expected denial of DSO-only data. The BRCB replayed 93 buffered reports, some with old or epoch measurement timestamps. This is consistent with queued report delivery while no client is subscribed, not 93 new simulator events; the buffer-overflow flag was not checked. The first URCB check listened for only 3 seconds, shorter than its 4-second integrity period, so zero events was expected with static inputs. A 6-second retest received one periodic report and passed. Both report blocks were disabled after testing.

All 27 configured TLS/RBAC files matched the pre-upgrade backup. Aggregator plant outputs remained disabled and both modes were restored to -5/off with P=100% and Q=0%.

The user confirmed the inverter simulator was not running, so zero/stale Modbus measurements were expected in this validation. MMS still returned Good quality flags with zero values while device status was stale; verify quality propagation once the simulator is running. No physical P/Q regulation was tested.

Detailed results: lab/evidence/aggregator-live-install-2026-10-21/RESULTS.md