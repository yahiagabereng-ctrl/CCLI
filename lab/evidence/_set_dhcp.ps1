netsh interface ip set address name="Ethernet" dhcp
netsh interface ip set dns name="Ethernet" dhcp
ipconfig /renew Ethernet
"DONE" > "C:\Yahia\projects\CCLI\lab\evidence\_dhcp_result.txt"
ipconfig | findstr /i "Ethernet IPv4 Subnet Gateway DHCP"
