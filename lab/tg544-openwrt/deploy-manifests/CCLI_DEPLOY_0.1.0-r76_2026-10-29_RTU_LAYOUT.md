# r76 continuous RTU polling correction

Date: 2026-10-29. Target: TesPro 192.168.0.1.

## Cause and correction

`CCLI_MODBUS_LIBMODBUS` is PRIVATE to ccli_adapters, but conditionally added
the context pointer to the public ModbusAdapter class. ModbusService embeds
that class by value without the private compile flag. The caller therefore
placed its atomic stop flag at the location where the backend wrote its
connection pointer. Opening the first RTU connection could overwrite the stop
flag and end the worker. The opaque pointer is now unconditional, so backend
and caller agree on the layout without exposing libmodbus headers.

The historical-header regression check fails as expected: caller class sizes
568/592 versus backend 576/600 on the host. The corrected header passes with
576/600 in both builds. Command: `python3 scripts/tests/test_modbus_layout.py`;
use `--header-ref HEAD` against the pre-fix repository revision to reproduce.

## Deployment

- APK: `lab/tg544-openwrt/ccli-0.1.0-r76.apk`.
- SHA-256: `10d3e52efd2ab9da829747c9bbd7c152335be4849e53f27c55852d228cf4acb2`.
- Installed executable SHA-256: `6c336dfcf401c48603d29eb7d282f9a770cf98a26720b12e743e8a0664cd18d9`.
- Installed over SSH with `apk --no-network add --upgrade --allow-untrusted`.
- Backups: `/etc/ccli-upgrade.EIOoeI` and volatile
  `/tmp/ccli-r76-pre-upgrade.tgz`.
- YAML checksum preserved across upgrade:
  `65786acda12b46a555900a6b920bc7bc43751e67de597617ec5b99ef3b2c76e0`.
- CRL UCI configuration checksum preserved:
  `49b0e1decf063181dd2d20aa55a8ea11685b8c84046afcb5bd04ab2665411d18`.
- No pre-existing hold/pending markers. New hold cleared after preservation
  checks; controller explicitly started after the old process had stopped.

## Live verification

Process 15829 reports r76 and retains the existing RTU map and poll_ms=1000.
The RTU port is /dev/rs485_2_uart -> /dev/ttyUSB1, unit 1, 9600 8N1.
The vendor serial service remains disabled on port3 and does not own ttyUSB1.

Repeated readings after startup were verified over more than 30 seconds:
68.893, 77.9174, 79.6263, 87.8972, 87.0523, 87.8431, 87.5477, 85.9569,
86.6343, 86.035 and 85.0109 kW; Q=0 kvar, quality=good. This is ongoing
acquisition evidence, unlike the single startup sample recorded after r75.
Foreground log spacing also depends on meter/main-loop work; it is not a
wire-level measurement of every RTU request interval.

MMS is listening on 192.168.0.1:3782. The EJBCA CRL updater remains enabled.
No P/Q command was issued and no simulator or wiring setting was changed.
