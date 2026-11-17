# CCLI deploy manifest — 0.1.0-r73

Date: 2026-10-29 (Europe/Rome)
Target: Tespro TG544 at `192.168.0.1`, TesproOS 32-2.1.0 r601.

## Artifact and install

- Signed APK: `apps/firmware-updater/artifacts/ccli-0.1.0-r73.apk`
- APK SHA-256: `9dc8a7d13025b94cde367614583bd8be15c3d056b4efe4961832dbf50ebf2a34`
- Manufacturer signature verification: PASS using the firmware-updater toolkit.
- Installed executable: `/usr/sbin/ccli`, extracted from the verified APK payload and deployed as a direct binary update to match the existing r72 deployment method.
- Installed executable SHA-256: `1f2f34ab0322bae7b6a54c81f17ebc1b22a60ba392ae708b3e59ed354ccb5e08`
- Runtime version: `0.1.0-r73`.

## Configuration and live verification

- The TCP meter poller remains active independently of the inverter poller; the CCLI listener on `192.168.0.1:3782` remained up.
- Inverter RTU: `/dev/rs485_2_uart`, unit 1, 9600 baud 8N1, 1000 ms poll interval; float registers 60000 (P), 60002 (Q), 60004 (line voltage).
- The simulator harness documentation maps its COM8 adapter to `/dev/rs485_2_uart`. An initial `/dev/ttyS1` setting produced stale data and was corrected.
- Final status: CCLI service running; `/var/run/ccli-status.json` reported inverter P `92.5321 kW`, Q `0 kvar`, quality `good`. TCP meter Modbus reads continued in the log.
