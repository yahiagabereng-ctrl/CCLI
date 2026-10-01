# Run ON the other PC (Pc17 / LAN3 / eng) — direct, no tunnel
# SSH:  root@192.168.0.1   password: TesPro default 000000
# LuCI: http://192.168.0.1

$hk = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"
& "C:\Program Files\PuTTY\plink.exe" -ssh root@192.168.0.1 -pw "000000" -hostkey $hk
