# Fresh COM8 boot capture

Files: `boot-console-reread.bin` (original bytes), `boot-console-reread.txt` (decoded view).
Passive receive at 115200 8N1; no console commands, resets, flashes or erases sent by the agent.

This attempt progressed beyond the previous BL2 decompression failure:
- BL2 booted BL31 and U-Boot.
- Board identified as Tespro TR500 (Oolite-MT7987A SPI-NOR), MT7987, 2 GiB RAM.
- U-Boot reported a bad environment CRC, selected defaults and applied its customer NOR recovery environment in RAM. This warning alone does not establish the cause of the earlier BL2 stop or loss of Linux configuration.
- Default boot from SPI-NOR proceeded automatically without key input.
- FIT kernel, device-tree and rootfs crc32/sha1 checks passed; Linux 6.12.68 started.
- Existing JFFS2 overlay was mounted; no formatting/erase was observed in this capture. The presence of each user configuration/key is not yet verified.
- USB devices on ports 1-1.4 and 1-1.7 failed enumeration with errors -32/-71.
- TPM startup logged a self-test error then a manual-start attempt; capture does not establish final TPM health.
- Last readable kernel output was near 18.15 seconds during wireless/bridge initialization, followed by NUL bytes. No kernel panic was captured. A baud/console ownership change or other serial issue remains possible; a crash is not proven by those bytes.
- Initial SSH retry to 192.168.0.1 did not connect. Full service readiness remains unverified.

Successful early boot on this attempt does not prove the earlier failure is permanently resolved. Preserve both captures for the manufacturer.

## Follow-up over restored SSH

SSH subsequently became reachable. The CCI r61 binary and lab.yaml hashes matched the pre-incident values, and CCI was running. No configuration reset was performed by the agent.

The user reports this successful boot occurred after unplugging the LAN cable. Kernel logs show external `lan1` first linked up at 69.396657 seconds, after Linux and userspace had already started. This supports the chronology of a successful boot without an external LAN link followed by reconnection, but does not prove that LAN caused the earlier BL2 decompression failure. Internal eth0/switch links earlier in the log are distinct from external lan1 carrier. Treat LAN-connected boot as a suspected trigger for manufacturer investigation, not a confirmed root cause. No controlled cable-connected/disconnected reboot comparison was performed.
