# GNSS startup fix — 2026-10-19

**Subsequent boot incident:** Console capture now shows BL2 failing to decompress an early boot image, before Linux starts. The GNSS init script is not reached and does not cause this observed stop. See `BOOT-INCIDENT.md` and `boot-console-115200.bin`. The running-system test below succeeded, but a full boot of the patched script remains unverified because of the earlier-stage failure. No firmware flash, reset or remote rollback was performed.

Installed the one-line `USE_PROCD=1` fix in Tespro `/etc/init.d/gnss`.
Local maintained copy: `lab/tg544-openwrt/gnss.init`.
Original backed up on Tespro at `/etc/init.d/gnss.before-20261019` and locally as `gnss.before`.

The original script defined `start_service`, `stop_service`, and service triggers but did not enable rc.common's procd wrappers. Consequently its boot `start` did nothing despite the enabled UCI setting and S99 symlink.

Validation:
- `sh -n` passed before installation; the only script change is the new flag.
- Started through `/etc/init.d/gnss start`; ubus returned success and `powered_on=true`.
- Acquired a valid fix with 15 satellites; GNSS feeder sample delivery succeeded.
- Chrony naturally selected `#* GNSS`, stratum 1, Leap status Normal.
- Existing chrony `makestep 1.0 3` policy automatically corrected the approximately 2227.63-second clock error. No manual date setting, `chronyc makestep`, chrony restart, or time configuration change was performed.
- Initial post-correction source sample was approximately -13 ms; the estimator is still settling with large dispersion. This does not establish certified +/-100 ms UTC accuracy.

No reboot was performed. The startup hook and enabled boot symlink were inspected; a complete cold-boot acquisition test remains unexecuted.
See `verification.txt` and `chrony-followup.txt` for live outputs. GNSS output contains bench coordinates; treat these files as local lab evidence.
