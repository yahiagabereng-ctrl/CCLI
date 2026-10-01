#!/bin/sh
# Run on OpenWrt Pi 4 after first SSH login (root@192.168.1.1)
set -e

echo "=== OpenWrt first boot — CCLI lab Pi 4 ==="

# Password
passwd

# Lab network (matches lab_pi.yaml / TestBench)
uci set network.lan.ipaddr='192.168.10.20'
uci set network.lan.netmask='255.255.255.0'
uci set network.lan.gateway='192.168.10.10'
uci set network.lan.dns='192.168.10.10'
uci commit network

uci set system.@system[0].hostname='ccli-pi4'
uci commit system

/etc/init.d/network restart
/etc/init.d/system restart

opkg update
opkg install libgpiod gpiod-tools

echo ""
echo "Done. Reconnect: ssh root@192.168.10.20"
echo "GPIO test:"
echo "  gpiodetect"
echo "  gpioset gpiochip0 5=1   # relay ON"
echo "  gpioset gpiochip0 5=0   # relay OFF"
