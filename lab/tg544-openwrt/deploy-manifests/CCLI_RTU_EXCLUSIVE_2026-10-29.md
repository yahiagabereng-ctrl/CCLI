# Inverter RTU serial ownership correction — 2026-10-29

Target: Tespro 192.168.0.1, CCLI r74. No binary change.

Found CCLI and seriald both opening /dev/ttyUSB1 (/dev/rs485_2_uart). Disabled only serialservice.port3.enabled, committed UCI and restarted serialservice and CCLI. Other serial port configuration entries were preserved. Original configuration backup on device: /mnt/mmcblk0/serialservice.before-inverter-exclusive-20261029.

Verified only CCLI holds the inverter tty after restart. Repeated RTU samples reached the MMS update path: at 12:34:44, :45, :46 and :54 CET, P was 86.866, 84.2806, 82.9554 and 77.729 kW respectively, Q=0, quality=good. GenPVMMXU1.TotW is updated immediately before each such trace. This proves repeated acquisition/publication calls; Test Suite Pro readback was not independently exercised in this chat.

Preserve serialservice.port3.enabled=0 while CCLI owns RS485-2. Inverter unit1, 9600 8N1, device /dev/rs485_2_uart. TCP meter remains a separate connection.
