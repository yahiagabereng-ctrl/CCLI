# GNSS readiness/retry fix

Replaced synchronous GNSS startup with a procd-supervised `/usr/sbin/gnss-start-worker`.
Source files: `lab/tg544-openwrt/gnss.init`, `gnss-start-worker`.

The init callback only registers the worker and returned in 0.05 seconds on Tespro. The worker observes the SIM collector's 90-second boot quiet period, checks that the AT broker is running with a live PID and character-device port, uses bounded ubus calls, and retries every 15 seconds when readiness/startup/verification fails. Only an explicit `powered_on=true` result logs verified success. Once verified, it checks health every 60 seconds and retries if the receiver is no longer verified. It does not open the modem port itself or set the system clock. Stop kills the monitor before requesting GNSS off, and validates the stop response.

Backed up previous init script to `/etc/ccli/gnss-startup-retry-backup-20261019/gnss.init`. The earlier vendor-original backup remains `/etc/init.d/gnss.before-20261019`.

Shell syntax checks passed both locally and on Tespro. Seven checks in `lab/tg544-openwrt/test_gnss_startup.py` passed: broker unavailable, already on, failed start, unverified power after accepted request, verified start, transport failure, and nonblocking boot callback. The tests run the real shell functions with fake modem responses; they are not hardware cold-boot tests.

Live verification: procd startup instance running, receiver verified powered on, valid satellite fix acquired with 6 satellites, and chrony feeder successfully delivered GNSS samples. Chrony subsequently selected `#* GNSS`, stratum 1, Leap status Normal, and automatically corrected the 1984-second clock error under its existing policy. The estimator still has high initial dispersion; this does not establish +/-100 ms UTC accuracy. See `retry-worker-verification.txt`, `retry-worker-chrony.txt` and `retry-worker-chrony-final.txt`.

No reboot, bootloader change, forced date setting, `chronyc makestep`, chrony restart, or time-configuration change was performed. Full cold-boot verification remains pending because of the separate intermittent early-boot problem.
