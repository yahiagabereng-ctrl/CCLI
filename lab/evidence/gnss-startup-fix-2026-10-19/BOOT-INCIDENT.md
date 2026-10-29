# Boot incident after GNSS init change

## Console finding — startup script is not reached

The user connected the Tespro boot console to COM8. The inverter simulator PID 6772 was stopped to release COM8. Capture at 115200 8N1 recorded two boot sequences, both ending at:

```text
NOTICE:  BL2: v2.13.0(release):OpenWrt v2025.07.11~78a0dfd9-1 (mt7987-nor-comb-gainstrong)
NOTICE:  CPU: MT7987A
NOTICE:  EMI: complex R/W mem test passed
ERROR:   xz: xz_dec_run() failed (err = 7)
ERROR:   Failed to decompress image (err=-5)
ERROR:   BL2: Failure in post image load handling (-5)
```

Raw evidence: `boot-console-115200.bin`. This failure is in early boot firmware before Linux/userspace or the GNSS init script. The earlier suspected GNSS startup hang is therefore ruled out as the cause of this observed stop. The logs establish decompression failure, but do not establish whether the underlying cause is a damaged/incompatible image, flash reads, memory behavior or another early-boot problem. No U-Boot/Linux recovery prompt appeared in the captured sequences. No flash, erase, factory reset or bootloader command was sent. Board-specific recovery requires exact model/revision and verified recovery instructions/images.

## Earlier LAN-only observations (before console was available)

User reports a reboot followed by SYS LED off, while power and Wi-Fi LEDs are on. Only LAN is connected; COM8 is the inverter adapter, not a boot console.

Read-only checks: PC Ethernet 2 link is up at 1 Gbit/s. PC DHCP address had fallen back to 169.254.94.88/16; an active-only 192.168.0.137/24 address was added for diagnosis. Once this address was Preferred, direct ping produced no Tespro response and ARP neighbor 192.168.0.1 remained Unreachable. SSH could not connect. A brief passive ARP/DHCP capture contained zero matching packets. This proves loss of LAN reachability, not a kernel crash or its cause.

The deployed GNSS change enabled a previously unused synchronous startup path. A boot dependency/blocking problem is possible. Its running-system acquisition test does not prove safe cold boot. Boot logs are required to establish the cause.

First recovery action, once an authenticated shell is available, is to inspect the init script and restore the saved original:

```sh
test -f /etc/init.d/gnss.before-20261019 &&
cp -p /etc/init.d/gnss.before-20261019 /etc/init.d/gnss
```

This restores only the prior GNSS startup script. Do not execute additional reboot, reset, flashing, or broad configuration recovery without inspecting the boot state. The device backup and local `gnss.before` both preserve the original.

No rollback was executed: LAN is unreachable. Exact device model and console/recovery access have been requested. The temporary PC address is retained to support recovery; after access is restored, restore the prior DHCP network configuration as appropriate.
