# TesPro TR-400 User Manual — CCLI Engineering Extract

**Document ID:** CCLI-HW-TR400-001
**Revision:** 1.0
**Date:** 2026-09-15
**RAG source_id:** `ccli-tr400-user-manual-extract`
**PDF source_id:** `ccli-tr400-user-manual`
**Source:** `TR400_User_Manual_EN_v1.0.pdf` (v1.0 EN)
**Corpus status:** **HAVE** — auto-extracted from OEM user manual
**Programme link:** K1.2 · K1.6 · K3.1 · K5 · `ccli-tg500-lab-platform`

---

## Summary

TesPro **TR-400 series** user manual ingested from received hardware documentation (49 pages). This is the **product-facing OEM manual** for the lab DUT — use it to close **port map (K3.1)**, **DI/DO ratings (K5)**, **RS485 pinout (K5.1)**, and **SKU equivalence (K1.6)** vs frozen **TG-424 Pro / TG-524** programme names.

### SKU naming (verify on device label)

| Marketing / manual | CCLI frozen name | Status |
|--------------------|------------------|--------|
| **TR-400 series** (this manual) | TesPro industrial gateway line | **HAVE** manual |
| TR-424 / TR-425 (web) | **TG-424 Pro** (supplier PO) | **OPEN** — confirm equivalence |
| TG-524 / TG-525 (datasheet) | Product SKU in `ccli-soc-freeze` | **FROZEN** |

---

## Table of contents (detected headings)

| Section | PDF page |
|---------|----------|
| Tr400 | 1 |
| Steps to access the Web management interface: | 7 |
| The TR400 supports multiple interface languages. Steps to switch the language: | 8 |
| 5.5 Dhcp | 24 |
| 5.6 Dns | 25 |
| Configure communication parameters for each serial port: | 29 |
| Configure UDP multicast network settings for each serial port: | 30 |
| Key configuration items include: | 33 |
| Key configuration items: | 34 |
| Key configuration items: | 35 |
| 8.5 Pptp | 36 |
| Data points define BACnet object mappings within a device: | 38 |
| 512Mb | 46 |
| Lan Ip | 47 |

---

## Keyword index (auto-scan)

### `RS485`
- p.5: ●3 serial ports (2× RS485 + 1× RS232) with UDP multicast service support
- p.29: ●RS485-1: /dev/ttyS1
- p.29: ●RS485-2: /dev/ttyS2
- p.46: 2× RS485 + 1× RS232, UDP multicast support
- p.48: ●Check physical wiring: RS485 requires correct A/B polarity and proper termination resistors.

### `RS232`
- p.5: ●3 serial ports (2× RS485 + 1× RS232) with UDP multicast service support
- p.29: ●RS232: /dev/ttyS0
- p.46: 2× RS485 + 1× RS232, UDP multicast support

### `DI`
- p.3: 5.7 Network Diagnostics............................................................................................................... 25
- p.3: 6.2 Port Forwarding...................................................................................................................... 26
- p.4: Appendix A: Technical Specifications..................................................................................... 46
- p.4: Appendix B: Default Configuration.......................................................................................... 47
- p.4: Appendix C: Troubleshooting...................................................................................................47
- p.5: Co., Ltd. in line with the Industry 4.0 trend, designed to support enterprise digital transformation. It
- p.5: and more, providing stable, secure, efficient, and intelligent network communication solutions for
- p.5: and reliable network connectivity through multiple WAN access methods including Gigabit
- p.8: Warning: Please change the default password immediately after your first login to ensure
- p.10: The Overview page displays a summary of the router's current status, including system
- p.10: network interface status, DHCP leases, and connected WiFi clients. This is the default landing
- p.10: The Routes page displays the current active IPv4 and IPv6 routing tables. Each entry includes the

### `DO`
- p.1: Document Version 1.0
- p.8: 3. Select your preferred language from the "Language" drop-down menu.
- p.18: TLS encryption, authentication token, heartbeat interval (seconds), and custom subdomain
- p.19: versions available from the cloud. You can check online and download the latest firmware for
- p.19: Warning: Do not disconnect power or close the browser during a firmware update. The
- p.20: Warning: Do not disconnect power during a firmware upgrade. This may permanently
- p.21: Perform a controlled system reboot. The router will gracefully shut down all services and restart.
- p.25: Configure DNS settings including local domain, DNS forwarding, caching, and custom DNS
- p.31: types: AO (Holding Register), DO (Coil), AI (Input Register), and DI (Discrete Input).
- p.36: the pptp-bundle package. PPTP is managed through UCI configuration files and does not provide
- p.46: Remote access URL format: https://<subdomain>.mgr.tespro.com
- p.49: please do not hesitate to contact us.

### `TesproOS`
- p.1: TesproOS 1.0000
- p.5: various IoT applications. Equipped with the self-developed TesproOS operating system designed
- p.46: TesproOS
- p.46: TesproOS 1.0.0

### `WAN`
- p.3: 8.4 IPsec (strongSwan).................................................................................................................35
- p.5: and reliable network connectivity through multiple WAN access methods including Gigabit
- p.5: ●5 Gigabit Ethernet ports (4 LAN + 1 WAN)
- p.22: Configure network interfaces including LAN, WAN, and cellular network (WWAN) connections.
- p.26: LAN (allow) and WAN (deny input, IP masquerading enabled) zones.
- p.33: menu allows you to configure and manage OpenVPN, WireGuard, IPsec (strongSwan), and other
- p.35: 8.4 IPsec (strongSwan)
- p.35: connections. The TR400 provides the ipsec-bundle package, implemented based on strongSwan.
- p.35: IPsec connection status can be viewed on the Status > strongSwan page.
- p.46: 5× Gigabit Ethernet ports (4 LAN + 1 WAN)
- p.47: ●Verify that the Ethernet cable is securely connected to a LAN port (not the WAN port).
- p.47: ●Verify that the WAN port is connected to your modem or upstream network.

### `LAN`
- p.2: 2.3 Switching Interface Language................................................................................................... 8
- p.5: ●5 Gigabit Ethernet ports (4 LAN + 1 WAN)
- p.7: 2. Use an Ethernet cable to connect your computer to any available LAN port.
- p.8: 2.3 Switching Interface Language
- p.8: The TR400 supports multiple interface languages. Steps to switch the language:
- p.8: 2. Click the "Language and Interface" tab.
- p.8: 3. Select your preferred language from the "Language" drop-down menu.
- p.9: Figure 2-3: Language and Interface Settings
- p.10: network interface status, DHCP leases, and connected WiFi clients. This is the default landing
- p.17: left blank, the MAC address will be used.
- p.22: Configure network interfaces including LAN, WAN, and cellular network (WWAN) connections.
- p.22: Tip: The LAN interface is configured in bridge mode (br-lan) by default, combining all LAN

### `12V`
- p.5: ●DC 12V/2A Power Adapter × 1
- p.46: DC 12V/2A

### `tty`
- p.29: ●RS485-1: /dev/ttyS1
- p.29: ●RS485-2: /dev/ttyS2
- p.29: ●RS232: /dev/ttyS0

### `eth`
- p.5: Whether deployed in industrial facilities, retail stores, or remote offices, the TR400 delivers stable
- p.5: and reliable network connectivity through multiple WAN access methods including Gigabit
- p.5: Ethernet and 4G/5G cellular networks. It also supports serial port communication, Modbus
- p.5: ●5 Gigabit Ethernet ports (4 LAN + 1 WAN)
- p.6: ●Ethernet Cable (RJ45) × 1
- p.7: 2. Use an Ethernet cable to connect your computer to any available LAN port.
- p.22: Ethernet ports into a single network segment. The TR-22X provides 2 LAN ports, and the
- p.23: Configure WiFi settings for the 2.4GHz band, including SSID, encryption method, channel, and
- p.29: ●Parity: Error detection method (None, Even, Odd), default None
- p.30: Northbound channels support six data publishing methods: MQTT, TCP, UDP, HTTP, OPC UA
- p.33: authentication methods. The TR400 provides the openvpn-bundle package; once installed, you
- p.35: - Local and remote authentication methods (pre-shared key/certificate)

---

## Full text by page

### Page 1

```text
TR400
Industrial Router
User Manual
____________________________________
TesproOS 1.0000
Document Version 1.0
Tespro Electronics Co., Ltd.
www.tespro.com
© 2024-2026 Tespro Electronics Co., Ltd. All rights reserved.
```

### Page 2

```text
TR400 Industrial Router User Manual
Page 2 | © 2024-2026 Tespro Electronics Co., Ltd.
Table of Contents
Chapter 1 Product Overview....................................................................................................... 5
1.1 Key Features............................................................................................................................ 5
1.2 Package Contents.....................................................................................................................5
Chapter 2 Quick Start.................................................................................................................. 7
2.1 Default Configuration................................................................................................................ 7
Hardware Installation..............................................................................................................................7
2.2 Logging into the Management Interface.................................................................................... 7
2.3 Switching Interface Language................................................................................................... 8
Chapter 3 Status & Monitoring..................................................................................................10
3.1 Overview.................................................................................................................................10
3.2 Routing Table......................................................................................................................... 10
3.3 Firewall Status ........................................................................................................................ 11
3.4 System Log .............................................................................................................................12
3.5 Kernel Log .............................................................................................................................. 12
3.6 Channel Analysis .................................................................................................................... 13
3.7 Real-time Graphs....................................................................................................................13
3.8 Cellular Network Status...........................................................................................................14
Chapter 4 System Configuration.............................................................................................. 15
4.1 Basic Settings......................................................................................................................... 15
4.2 Administration......................................................................................................................... 15
4.3 Startup Services......................................................................................................................16
4.4 Scheduled Tasks.................................................................................................................... 17
4.5 Device Online Manager...........................................................................................................17
4.5.1 Settings........................................................................................................................................17
4.5.2 Package Management................................................................................................................ 18
4.5.3 Firmware Update.........................................................................................................................19
4.6 LED Configuration...................................................................................................................19
4.7 Backup & Update .................................................................................................................... 20
4.7.1 Backup / Flash Firmware.............................................................................................................20
4.7.2 Configuration Management.........................................................................................................21
4.8 Reboot....................................................................................................................................21
Chapter 5 Network Configuration.............................................................................................22
5.1 Network Interfaces.................................................................................................................. 22
5.2 Cellular Network Management................................................................................................ 22
5.3 WiFi Network.......................................................................................................................... 23
5.4 Static Routes.......................................................................................................................... 24
```

### Page 3

```text
TR400 Industrial Router User Manual
Page 3 | © 2024-2026 Tespro Electronics Co., Ltd.
5.5 DHCP ..................................................................................................................................... 24
5.6 DNS........................................................................................................................................25
5.7 Network Diagnostics............................................................................................................... 25
Chapter 6 Firewall...................................................................................................................... 26
6.1 General Settings (Zone Configuration).................................................................................... 26
6.2 Port Forwarding...................................................................................................................... 26
6.3 Traffic Rules ............................................................................................................................27
6.4 NAT Rules.............................................................................................................................. 27
6.5 IP Sets ....................................................................................................................................28
Chapter 7 Services.....................................................................................................................29
7.1 Serial Service - Basic Settings ................................................................................................ 29
7.2 Serial Parameters................................................................................................................... 29
7.3 Serial Network Settings...........................................................................................................30
7.4 Serial Debug & Test................................................................................................................30
7.5 Modbus Protocol Service - Overview.......................................................................................30
7.6 Northbound Channels............................................................................................................. 31
7.7 Devices & Data Points............................................................................................................ 31
7.8 Modbus Quick Start Guide...................................................................................................... 31
Chapter 8 VPN Connection Management.................................................................................33
8.1 VPN Menu Overview...............................................................................................................33
8.2 OpenVPN ................................................................................................................................33
8.3 WireGuard .............................................................................................................................. 34
8.4 IPsec (strongSwan).................................................................................................................35
8.5 PPTP...................................................................................................................................... 36
Chapter 9 BACnet Gateway Service......................................................................................... 37
9.1 Service Overview .................................................................................................................... 37
9.2 Service Start/Stop ................................................................................................................... 37
9.3 Northbound Channel Configuration......................................................................................... 38
9.4 Device Management............................................................................................................... 38
9.5 Southbound Connection Configuration....................................................................................38
9.6 Data Point Management..........................................................................................................38
9.7 Modbus Register Mapping...................................................................................................... 38
9.8 Real-time Monitoring & Control............................................................................................... 39
9.9 Breakpoint Resume.................................................................................................................39
9.10 Troubleshooting.................................................................................................................... 39
Chapter 10 Energy Management Service.................................................................................40
10.1 Dashboard ............................................................................................................................ 40
10.2 Device Management............................................................................................................. 40
```

### Page 4

```text
TR400 Industrial Router User Manual
Page 4 | © 2024-2026 Tespro Electronics Co., Ltd.
10.3 Energy Analysis .................................................................................................................... 41
10.4 Carbon Management............................................................................................................ 42
10.5 Operations Management .......................................................................................................43
10.6 System Settings .................................................................................................................... 43
Chapter 11 Cloud Device Management Service.......................................................................45
11.1 Service Overview.................................................................................................................. 45
11.2 Registration & Login ..............................................................................................................45
11.3 Remote Device Access ......................................................................................................... 46
11.4 Connection Architecture........................................................................................................46
Appendix A: Technical Specifications..................................................................................... 46
A.1 Hardware Specifications......................................................................................................... 46
A.2 Software Specifications...........................................................................................................46
Appendix B: Default Configuration.......................................................................................... 47
Appendix C: Troubleshooting...................................................................................................47
Unable to Access the Web Management Interface........................................................................47
WiFi Not Working..........................................................................................................................47
No Internet Connection................................................................................................................. 47
4G/5G Cellular Network Not Connecting....................................................................................... 47
Modbus Communication Error.......................................................................................................48
VPN Connection Issues ................................................................................................................ 48
Forgot Administrator Password ..................................................................................................... 48
Contact Us..................................................................................................................................49
```

### Page 5

```text
TR400 Industrial Router User Manual
Page 5 | © 2024-2026 Tespro Electronics Co., Ltd.
Chapter 1 Product Overview
The TR400 series is an industrial-grade router professionally developed by Tespro Electronics
Co., Ltd. in line with the Industry 4.0 trend, designed to support enterprise digital transformation. It
integrates Wi-Fi, cellular networks (4G/5G), rich IoT interfaces, VPN private network technology,
and more, providing stable, secure, efficient, and intelligent network communication solutions for
various IoT applications. Equipped with the self-developed TesproOS operating system designed
specifically for IoT connectivity, the TR400 combines high-performance hardware with an intuitive
Web management interface for convenient configuration and monitoring, enabling comprehensive
control over devices, networks, protocols, and remote access.
Whether deployed in industrial facilities, retail stores, or remote offices, the TR400 delivers stable
and reliable network connectivity through multiple WAN access methods including Gigabit
Ethernet and 4G/5G cellular networks. It also supports serial port communication, Modbus
protocol gateway, BACnet gateway, and energy management functions, making it ideal for IoT
and industrial automation applications.
Tip: This version (v1.0) introduces VPN connection management, BACnet gateway service,
and energy management service, with improvements to other chapters.
1.1 Key Features
●Dual-core ARM Cortex-A53 @ 1.3GHz with hardware NAT acceleration
●512MB memory + 256MB storage
●WiFi 6 AX3000 dual-band: 2.4GHz 574Mbps + 5GHz 2402Mbps
●5 Gigabit Ethernet ports (4 LAN + 1 WAN)
●Built-in cellular expansion interface for 4G/5G connectivity
●3 serial ports (2× RS485 + 1× RS232) with UDP multicast service support
●Modbus RTU/TCP gateway supporting both Master and Slave roles; northbound channels
support MQTT/TCP/UDP/HTTP/OPC UA Client/OPC UA Server
●BACnet gateway service supporting BACnet/IP and BACnet MS/TP southbound data
collection
●Energy management service with real-time multi-device energy consumption monitoring
and alerting
●VPN connection management supporting OpenVPN, WireGuard, IPsec, and PPTP
●USB 3.0 / USB 2.0 interfaces for storage and cellular expansion
●Industrial-grade firewall with zone-based security policies
●Application-layer traffic identification (NetCtrl/DPI engine)
●Modern Web management interface with dark/light theme switching
1.2 Package Contents
●TR400 Industrial Router × 1
●DC 12V/2A Power Adapter × 1
●Wi-Fi Antenna × 2 + 4G Antenna × 2
```

### Page 6

```text
TR400 Industrial Router User Manual
Page 6 | © 2024-2026 Tespro Electronics Co., Ltd.
●Ethernet Cable (RJ45) × 1
●Quick Start Guide × 1
```

### Page 7

```text
TR400 Industrial Router User Manual
Page 7 | © 2024-2026 Tespro Electronics Co., Ltd.
Chapter 2 Quick Start
2.1 Default Configuration
Before accessing the router, please note the following default configuration information:
Item
Details
Management IP Address
192.168.0.1
Username
root
Password
000000
WiFi 2.4GHz SSID
TR400-2.4G (Password: 12345678)
Web Protocol
HTTP (Port 80) / HTTPS (Port 443)
Hardware Installation
1. Connect the power adapter to the router and plug it into a power outlet.
2. Use an Ethernet cable to connect your computer to any available LAN port.
3. Attach the external antennas to the antenna connectors.
4. (Optional) Insert a SIM card into the SIM card slot to enable cellular network connectivity.
5. (Optional) Insert an SD card to expand storage capacity.
2.2 Logging into the Management Interface
Steps to access the Web management interface:
1. Open a web browser (Chrome, Firefox, Edge, or Safari recommended).
2. Enter http://192.168.0.1 in the address bar and press Enter.
3. Enter the username (root) and default password (000000).
4. Click the "Login" button to enter the management dashboard.
```

### Page 8

```text
TR400 Industrial Router User Manual
Page 8 | © 2024-2026 Tespro Electronics Co., Ltd.
Figure 2-1: Login Page
Warning: Please change the default password immediately after your first login to ensure
security. Navigate to System > Administration to set a new password.
2.3 Switching Interface Language
The TR400 supports multiple interface languages. Steps to switch the language:
1. Navigate to System > System > Basic Settings.
2. Click the "Language and Interface" tab.
3. Select your preferred language from the "Language" drop-down menu.
4. Click "Save & Apply" to take effect.
```

### Page 9

```text
TR400 Industrial Router User Manual
Page 9 | © 2024-2026 Tespro Electronics Co., Ltd.
Figure 2-3: Language and Interface Settings
```

### Page 10

```text
TR400 Industrial Router User Manual
Page 10 | © 2024-2026 Tespro Electronics Co., Ltd.
Chapter 3 Status & Monitoring
The Status section provides real-time information on router operational status, network
connectivity, and system health. Access these pages through the "Status" menu in the top
navigation bar.
3.1 Overview
The Overview page displays a summary of the router's current status, including system
information (hostname, model, firmware version, kernel version), memory and storage usage,
network interface status, DHCP leases, and connected WiFi clients. This is the default landing
page after login.
Figure 3-1: Status Overview
3.2 Routing Table
The Routes page displays the current active IPv4 and IPv6 routing tables. Each entry includes the
destination network, gateway address, metric, and associated network interface.
```

### Page 11

```text
TR400 Industrial Router User Manual
Page 11 | © 2024-2026 Tespro Electronics Co., Ltd.
Figure 3-2: Routing Table
3.3 Firewall Status
View active IPv4 and IPv6 firewall rules and chains. This page displays all configured rules,
packet counters, and byte counters.
Figure 3-3: Firewall Status
```

### Page 12

```text
TR400 Industrial Router User Manual
Page 12 | © 2024-2026 Tespro Electronics Co., Ltd.
3.4 System Log
The System Log displays operational messages from system services, network events,
authentication attempts, and more. Messages are displayed in chronological order with
timestamps.
Figure 3-4: System Log
3.5 Kernel Log
The Kernel Log displays messages from the Linux kernel, useful for diagnosing hardware
initialization, driver loading, and low-level system events.
```

### Page 13

```text
TR400 Industrial Router User Manual
Page 13 | © 2024-2026 Tespro Electronics Co., Ltd.
Figure 3-5: Kernel Log
3.6 Channel Analysis
The Channel Analysis tool scans WiFi frequencies to identify channel usage in your environment,
helping you select the optimal channel for your WiFi network.
Figure 3-6: WiFi Channel Analysis
3.7 Real-time Graphs
View real-time graphs of network traffic throughput, system CPU load, and active connection
statistics.
```

### Page 14

```text
TR400 Industrial Router User Manual
Page 14 | © 2024-2026 Tespro Electronics Co., Ltd.
Figure 3-7: Real-time Graphs
3.8 Cellular Network Status
When a 4G/5G cellular module is installed, this page displays the cellular network connection
status, including signal strength, carrier information, and connection details. Navigate to
Network > Cellular Network for detailed information.
Figure 3-8: Cellular Network Status
```

### Page 15

```text
TR400 Industrial Router User Manual
Page 15 | © 2024-2026 Tespro Electronics Co., Ltd.
Chapter 4 System Configuration
The System section allows you to configure core router settings, including basic system
properties, administrative access, startup services, scheduled tasks, device online management,
LED behavior, software updates, and firmware management.
4.1 Basic Settings
Configure basic system settings including device hostname, timezone selection, and NTP server
(for automatic time synchronization).
Figure 4-1: Basic System Settings
4.2 Administration
Configure router administration settings including password management, SSH access settings,
and Web interface options.
```

### Page 16

```text
TR400 Industrial Router User Manual
Page 16 | © 2024-2026 Tespro Electronics Co., Ltd.
Figure 4-2: Administration Settings
Warning: Always use a strong password. The default password (000000) should be
changed immediately after your first login.
4.3 Startup Services
Manage system services and their startup behavior. Enable or disable services and view their
current status.
Figure 4-3: Startup Services
```

### Page 17

```text
TR400 Industrial Router User Manual
Page 17 | © 2024-2026 Tespro Electronics Co., Ltd.
4.4 Scheduled Tasks
Configure scheduled tasks using standard cron syntax for automated time-based task execution.
Figure 4-4: Scheduled Tasks (Cron)
4.5 Device Online Manager
The Device Online Manager is a built-in unified device management service on the TR400,
providing device status reporting, remote access tunnels, package management, and online
firmware updates. By connecting to the Tespro Cloud Management Platform (mgr.tespro.com),
you can remotely monitor and manage devices anytime, anywhere.
4.5.1 Settings
The Settings page includes a service status display and four configuration sections. The service
status shows the current service running status, cloud connection status, and device information.
●General Settings: Enable/disable the Device Online Manager service, set the device name
and server address. The device name is used to identify the device on the cloud platform; if
left blank, the MAC address will be used.
●Cloud Management: Configure MQTT status reporting. When enabled, the device will
periodically report its operational status to the cloud server via the MQTT protocol.
Configurable parameters include: MQTT broker address and port (default:
mgr.tespro.com:8883), TLS encryption (recommended), CA certificate path, authentication
username and password, reporting interval (seconds), and reporting only changed data after
the initial full data report.
●Remote Access: Configure WebSocket tunnels for remote device access. When enabled,
the device's Web management interface can be accessed remotely through the cloud
platform's Web access URL. Configurable parameters include: tunnel server address and port,
```

### Page 18

```text
TR400 Industrial Router User Manual
Page 18 | © 2024-2026 Tespro Electronics Co., Ltd.
TLS encryption, authentication token, heartbeat interval (seconds), and custom subdomain
(defaults to device name).
●Proxy Services: Configure the list of local services accessible through the remote tunnel.
By default, this includes the Web management interface (HTTP, port 80) and SSH access
(TCP, port 22). Service entries can be added, edited, or removed.
Figure 4-5: Device Online Manager - Service Status
4.5.2 Package Management
The Package Management page allows you to install, update, or remove software packages on
the device from a cloud repository. The page displays device disk space usage and a list of
available packages.
```

### Page 19

```text
TR400 Industrial Router User Manual
Page 19 | © 2024-2026 Tespro Electronics Co., Ltd.
Figure 4-7: Package Management
4.5.3 Firmware Update
The Firmware Update page displays the current device firmware information and a list of firmware
versions available from the cloud. You can check online and download the latest firmware for
upgrade.
Figure 4-8: Online Firmware Update
Warning: Do not disconnect power or close the browser during a firmware update. The
device will automatically restart after the update is complete.
4.6 LED Configuration
Customize device LED indicator behavior by assigning different triggers to each LED.
```

### Page 20

```text
TR400 Industrial Router User Manual
Page 20 | © 2024-2026 Tespro Electronics Co., Ltd.
Figure 4-9: LED Configuration
4.7 Backup & Update
4.7.1 Backup / Flash Firmware
Create system backups, restore from backup files, or flash new firmware images.
Tip: Always create a backup before flashing new firmware.
Figure 4-11: Backup & Flash Firmware
Warning: Do not disconnect power during a firmware upgrade. This may permanently
damage the device.
```

### Page 21

```text
TR400 Industrial Router User Manual
Page 21 | © 2024-2026 Tespro Electronics Co., Ltd.
4.7.2 Configuration Management
Import or export device configuration files for backup or multi-device deployment.
Figure 4-12: Configuration Management - Backup & Restore
4.8 Reboot
Perform a controlled system reboot. The router will gracefully shut down all services and restart.
Figure 4-14: System Reboot
```

### Page 22

```text
TR400 Industrial Router User Manual
Page 22 | © 2024-2026 Tespro Electronics Co., Ltd.
Chapter 5 Network Configuration
The Network section provides comprehensive control over all network-related settings.
5.1 Network Interfaces
Configure network interfaces including LAN, WAN, and cellular network (WWAN) connections.
Each interface can be configured with independent protocols, IP addresses, and parameters.
Tip: The LAN interface is configured in bridge mode (br-lan) by default, combining all LAN
Ethernet ports into a single network segment. The TR-22X provides 2 LAN ports, and the
TR-24X provides 4 LAN ports.
Figure 5-1: Network Interfaces
5.2 Cellular Network Management
Manage cellular module SIM card settings. View SIM status, signal quality, and configure cellular
parameters. Supports SIM card slot management, switching policies, and eSIM configuration.
```

### Page 23

```text
TR400 Industrial Router User Manual
Page 23 | © 2024-2026 Tespro Electronics Co., Ltd.
Figure 5-2: Cellular Network Management
5.3 WiFi Network
Configure WiFi settings for the 2.4GHz band, including SSID, encryption method, channel, and
transmit power.
Figure 5-3: WiFi Network Configuration
Warning: Change the default WiFi password after initial setup to prevent unauthorized
access.
```

### Page 24

```text
TR400 Industrial Router User Manual
Page 24 | © 2024-2026 Tespro Electronics Co., Ltd.
5.4 Static Routes
Add or manage static route entries to define custom paths to specific destinations for network
traffic.
Figure 5-4: Static Routes
5.5 DHCP
Configure the DHCP server for automatic IP address assignment, manage static leases, and view
active leases.
Figure 5-5: DHCP Configuration
```

### Page 25

```text
TR400 Industrial Router User Manual
Page 25 | © 2024-2026 Tespro Electronics Co., Ltd.
5.6 DNS
Configure DNS settings including local domain, DNS forwarding, caching, and custom DNS
records.
Figure 5-6: DNS Settings
5.7 Network Diagnostics
Built-in network diagnostic tools including Ping, Traceroute, and Nslookup for troubleshooting
connectivity issues.
Figure 5-7: Network Diagnostics
```

### Page 26

```text
TR400 Industrial Router User Manual
Page 26 | © 2024-2026 Tespro Electronics Co., Ltd.
Chapter 6 Firewall
The TR400 features a built-in industrial-grade firewall based on nftables (firewall4) with a zone-
based architecture.
6.1 General Settings (Zone Configuration)
Configure firewall zones and their default security policies. The TR400 comes pre-configured with
LAN (allow) and WAN (deny input, IP masquerading enabled) zones.
Tip: SYN flood protection is enabled by default to defend against denial-of-service attacks.
Figure 6-1: Firewall Zone Settings
6.2 Port Forwarding
Configure port forwarding rules to allow external access to services on the LAN.
```

### Page 27

```text
TR400 Industrial Router User Manual
Page 27 | © 2024-2026 Tespro Electronics Co., Ltd.
Figure 6-2: Port Forwarding
6.3 Traffic Rules
Define custom firewall rules to allow or block specific traffic. Pre-configured rules include DHCP
renewal, Ping (ICMP), IPv6 support, and IPSec VPN passthrough.
Figure 6-3: Traffic Rules
6.4 NAT Rules
Configure Source NAT (SNAT) rules for fine-grained control over the source IP address used by
outbound traffic.
```

### Page 28

```text
TR400 Industrial Router User Manual
Page 28 | © 2024-2026 Tespro Electronics Co., Ltd.
Figure 6-4: NAT Rules
6.5 IP Sets
Create and manage IP sets for efficient matching of large address groups in firewall rules.
Figure 6-5: IP Sets
```

### Page 29

```text
TR400 Industrial Router User Manual
Page 29 | © 2024-2026 Tespro Electronics Co., Ltd.
Chapter 7 Services
The Services section provides professional service features of the TR400, including serial port
communication services and Modbus protocol gateway functions, suitable for industrial and IoT
applications.
7.1 Serial Service - Basic Settings
The TR400 features a built-in serial communication service with UDP multicast support. The
router provides 3 configurable serial ports:
●RS485-1: /dev/ttyS1
●RS485-2: /dev/ttyS2
●RS232: /dev/ttyS0
Figure 7-1: Serial Service - Basic Settings
7.2 Serial Parameters
Configure communication parameters for each serial port:
Tip: The router serial port and connected device must use identical communication
parameters.
●Baud Rate: Communication speed (300 to 115200 bps), default 9600
●Parity: Error detection method (None, Even, Odd), default None
●Data Bits: Number of data bits per character (6, 7, or 8), default 8
●Stop Bits: Number of stop bits (1, 1.5, or 2), default 1
```

### Page 30

```text
TR400 Industrial Router User Manual
Page 30 | © 2024-2026 Tespro Electronics Co., Ltd.
7.3 Serial Network Settings
Configure UDP multicast network settings for each serial port:
●Multicast Address: UDP multicast group address (default 239.0.0.x)
●Send/Receive/Control Port: Network ports for data and management
●Lock Timeout: Timeout for automatic lock release (seconds)
7.4 Serial Debug & Test
Built-in serial communication debugging and testing interface. Supports sending/receiving data in
ASCII or HEX format, real-time adjustment of serial settings, and communication log viewing.
Figure 7-4: Serial Debug & Test
7.5 Modbus Protocol Service - Overview
The TR400 features a fully functional Modbus protocol gateway service that bridges industrial
Modbus devices with modern IT/cloud systems. The service supports dual-role mode: as a
Master, it polls and collects data from slave devices via southbound serial or TCP connections; as
a Slave, it accepts access from external masters and responds to register read/write requests.
Northbound channels support six data publishing methods: MQTT, TCP, UDP, HTTP, OPC UA
Client, and OPC UA Server.
Tip: The Modbus service is integrated with the serial service module. Serial parameters are
configured in the Serial Service section (7.2).
```

### Page 31

```text
TR400 Industrial Router User Manual
Page 31 | © 2024-2026 Tespro Electronics Co., Ltd.
Figure 7-5: Modbus Protocol Service Overview
7.6 Northbound Channels
Northbound channels define how the TR400 forwards collected Modbus data to upstream
systems. Six channel types are supported:
- MQTT: Publish data to a message broker via the MQTT protocol.
- TCP: Send data via direct TCP Socket connection.
- UDP: Send data via UDP packets.
- HTTP: Report data via HTTP REST API.
- OPC UA Client: Write data to remote OPC UA server nodes as an OPC UA client.
- OPC UA Server: Act as an OPC UA server, mapping collected data to OPC UA nodes for
subscription by external clients.
Multiple channels can be configured simultaneously to send data to different destinations.
7.7 Devices & Data Points
The Devices & Data Points section allows you to define Modbus devices and their register
mappings. In Master mode, each device includes slave address, polling interval, data point
configuration, and southbound connection settings; in Slave mode, you can configure the
TR400's own slave address and externally exposed register mappings, supporting four register
types: AO (Holding Register), DO (Coil), AI (Input Register), and DI (Discrete Input).
7.8 Modbus Quick Start Guide
Step 1: Configure serial parameters in the Serial Service.
Step 2: Create a northbound channel (e.g., MQTT connection to a cloud platform).
Step 3: Add Modbus slave devices.
Step 4: Add data points to map registers.
```

### Page 32

```text
TR400 Industrial Router User Manual
Page 32 | © 2024-2026 Tespro Electronics Co., Ltd.
Step 5: Configure southbound connections.
Step 6: Start the Modbus service.
Warning: Ensure that the serial parameters in the Serial Service exactly match those of the
Modbus slave device. Mismatched baud rates or parity settings will cause communication
failures.
```

### Page 33

```text
TR400 Industrial Router User Manual
Page 33 | © 2024-2026 Tespro Electronics Co., Ltd.
Chapter 8 VPN Connection Management
The TR400 supports multiple VPN protocols for secure remote network connectivity. The VPN
menu allows you to configure and manage OpenVPN, WireGuard, IPsec (strongSwan), and other
VPN services. VPN features are provided as software packages, and you can install the
corresponding VPN Bundle as needed.
8.1 VPN Menu Overview
In the left navigation bar of the Web management interface, the VPN menu provides configuration
entry points for each VPN protocol. Installed VPN services are displayed in the menu.
Figure 9-1: VPN Menu Overview
8.2 OpenVPN
OpenVPN is a mature open-source VPN protocol supporting TLS encryption and multiple
authentication methods. The TR400 provides the openvpn-bundle package; once installed, you
can configure OpenVPN clients or servers under the VPN menu.
Key configuration items include:
- VPN instance name and type (client/server)
- Remote server address and port
- Protocol selection (UDP/TCP)
- TLS certificate and key configuration
- Encryption algorithm selection
- Log level settings
Supports uploading .ovpn configuration files for quick VPN instance creation.
```

### Page 34

```text
TR400 Industrial Router User Manual
Page 34 | © 2024-2026 Tespro Electronics Co., Ltd.
Figure 9-2: OpenVPN Configuration
8.3 WireGuard
WireGuard is a modern high-performance VPN protocol known for its lean codebase and speed.
The TR400 provides the wireguard-bundle package. WireGuard configuration is managed
through the network interface.
Key configuration items:
- Private and public key generation
- Peer configuration: public key, allowed IP address ranges
- Listening port
- Persistent keepalive interval
WireGuard status can be viewed on the Status > WireGuard page, showing current connection
information, transfer statistics, and latest handshake time.
```

### Page 35

```text
TR400 Industrial Router User Manual
Page 35 | © 2024-2026 Tespro Electronics Co., Ltd.
Figure 9-3: WireGuard Configuration
8.4 IPsec (strongSwan)
IPsec is an industry-standard VPN protocol suitable for enterprise-grade site-to-site secure
connections. The TR400 provides the ipsec-bundle package, implemented based on strongSwan.
Key configuration items:
- IKE version selection (IKEv1/IKEv2)
- Local and remote authentication methods (pre-shared key/certificate)
- Encryption proposal configuration (encryption algorithm, integrity algorithm, DH group)
- ESP proposal configuration
- Local and remote subnets
- IKE and ESP lifetimes
IPsec connection status can be viewed on the Status > strongSwan page.
```

### Page 36

```text
TR400 Industrial Router User Manual
Page 36 | © 2024-2026 Tespro Electronics Co., Ltd.
Figure 9-4: IPsec Configuration
8.5 PPTP
PPTP is a legacy VPN protocol with simple configuration but lower security. The TR400 provides
the pptp-bundle package. PPTP is managed through UCI configuration files and does not provide
a dedicated Web interface.
Configuration method: Access the device via SSH, edit the /etc/config/pptp configuration file, or
manage PPTP service status through the System > Startup Services interface.
Tip: Due to the lower security of the PPTP protocol, it is recommended to use OpenVPN or
WireGuard for VPN connections.
```

### Page 37

```text
TR400 Industrial Router User Manual
Page 37 | © 2024-2026 Tespro Electronics Co., Ltd.
Chapter 9 BACnet Gateway Service
The BACnet Gateway Service collects data from BACnet/IP or BACnet MS/TP devices in building
automation systems to the TR400 gateway and forwards it to upper-level systems through
multiple channels. It also provides a standard Modbus TCP Server interface for direct read/write
access by SCADA systems.
9.1 Service Overview
The BACnet Gateway Service page features a service status bar at the top showing the running
status and start/stop buttons; the lower section is divided into two main areas: northbound
channel configuration and device & data point management.
Key features:
- Southbound data collection from BACnet/IP and BACnet MS/TP (RS-485) devices
- Northbound reporting to cloud platforms via MQTT/TCP/UDP/HTTP
- Built-in Modbus TCP Server with automatic mapping of BACnet objects to Modbus registers
- BACnet WriteProperty reverse control support
- Breakpoint resume and change-of-value reporting
- CSV/XLSX batch import/export of data point configurations
Figure 10-1: BACnet Gateway Service Overview
9.2 Service Start/Stop
Click the Start button to launch the service; the status indicator turns green showing Running. To
stop the service, click the Stop button.
```

### Page 38

```text
TR400 Industrial Router User Manual
Page 38 | © 2024-2026 Tespro Electronics Co., Ltd.
9.3 Northbound Channel Configuration
Northbound channels are used to report collected data to remote servers. Multiple channels can
be configured simultaneously for parallel operation.
Channel types:
- MQTT: Publish data to an MQTT broker
- HTTP: Send data to an HTTP/HTTPS endpoint
- TCP/UDP: Send data via raw sockets
- TCP Server: TR400 acts as a passive server listening for connections
MQTT channel configuration items: server address and port, topic, client ID, QoS,
username/password, TLS enable, reporting interval, data format (JSON/binary).
9.4 Device Management
Click "Add Device" to define a new BACnet slave device:
- Device name (required, must be unique)
- Modbus Slave ID (1-247)
- BACnet instance number
- Southbound connection type (BACnet/IP or BACnet MS/TP)
Supported operations: save device, delete device, control panel, report format, copy device.
9.5 Southbound Connection Configuration
BACnet/IP mode: Configure the target IP address, port (default 47808), timeout, and retry count.
BACnet MS/TP mode: Configure serial port selection, local MAC address, device MAC address,
and maximum master number.
Tip: Serial port baud rate and other parameters are configured centrally in the Serial
Service module.
9.6 Data Point Management
Data points define BACnet object mappings within a device:
- Name and report key
- BACnet object type (AI/AO/AV/BI/BO/BV/MSI/MSO/MSV)
- Object instance number
- Data type (float32/int16, etc.)
- Collection interval
- Writable flag
- Unit
Supports CSV/XLSX batch import/export of data point configurations.
9.7 Modbus Register Mapping
BACnet object types are automatically mapped to corresponding Modbus register types:
- AI/MSI →Input Register (FC 04, read-only)
```

### Page 39

```text
TR400 Industrial Router User Manual
Page 39 | © 2024-2026 Tespro Electronics Co., Ltd.
- AO/AV/MSO/MSV →Holding Register (FC 03, read/write)
- BI →Discrete Input (FC 02, read-only)
- BO/BV →Coil (FC 01, read/write)
Click "Export Modbus Map" to export the register mapping table as a CSV file.
9.8 Real-time Monitoring & Control
Click the "Control Panel" button to open the real-time monitoring dashboard: real-time value
display, manual write (triggering BACnet WriteProperty), and communication log.
9.9 Breakpoint Resume
When breakpoint resume is enabled, if the northbound connection is interrupted, pending data is
automatically cached to local files and retransmitted when the connection is restored.
9.10 Troubleshooting
●Service fails to start: Verify configuration is correct, check system logs
●BACnet device connection failure: Confirm the target IP is reachable and port 47808 is not
blocked by the firewall
●Object read returns error: Verify that the BACnet object type and instance number match
the DDC configuration
●Modbus TCP not responding: Confirm the slave ID is configured correctly and port 502 is
not occupied
●MQTT connection failure: Check the Broker address, port, and username/password
```

### Page 40

```text
TR400 Industrial Router User Manual
Page 40 | © 2024-2026 Tespro Electronics Co., Ltd.
Chapter 10 Energy Management Service
The Energy Management Service is an energy consumption monitoring and management module
on the TR400, providing device management, energy analysis, carbon management, operations
management, and system settings capabilities, suitable for industrial and commercial energy
management scenarios.
10.1 Dashboard
The Dashboard page provides a global overview of energy management, including key metrics
such as total power, today's energy consumption, today's water usage, active alerts, and online
devices. The service status is displayed at the top, with a "Start Service" button to enable the
energy management service.
Figure 10-1: Energy Management Dashboard
10.2 Device Management
The Device Management page is used to add and manage energy monitoring devices. It supports
device list viewing, device addition, product definition, variable classification, and control
management.
●Device List: View all configured energy monitoring devices, including device name, energy
type, protocol type, host/port, enabled status, and online status
●Product Definition: Define device product models and data point templates
●Variable Classification: Manage data collection variables and categories
●Control Management: Configure remote control parameters for devices
```

### Page 41

```text
TR400 Industrial Router User Manual
Page 41 | © 2024-2026 Tespro Electronics Co., Ltd.
Figure 10-2: Device Management - Device List
10.3 Energy Analysis
The Energy Analysis page provides multi-dimensional energy consumption data viewing and
analysis features, including the following sub-functions:
●Real-time Data: Auto-refreshes every 3 seconds, displaying real-time collected values from
all devices, including voltage, current, power, and energy parameters
●Historical Data: Provides trend charts and tabular views of energy consumption data, with
filtering by time range, device, and data type
●Time-of-Use Pricing: Configure peak/valley/flat rate time periods and view consumption and
cost distribution by period
●Energy Reports: Generate energy consumption analysis reports, supporting multiple time
ranges and data export formats
```

### Page 42

```text
TR400 Industrial Router User Manual
Page 42 | © 2024-2026 Tespro Electronics Co., Ltd.
Figure 10-4: Energy Analysis - Real-time Data
10.4 Carbon Management
The Carbon Management page provides carbon emission monitoring and calculation features,
including the following sub-functions:
●Emission Factors: Configure carbon emission conversion factors for various energy types
●Carbon Footprint: Calculate carbon emissions based on energy consumption data and
track carbon footprint trends
●Carbon Ledger: Record carbon emission details, supporting export and auditing
●Carbon Calculation: Customize carbon emission calculation rules and parameters
```

### Page 43

```text
TR400 Industrial Router User Manual
Page 43 | © 2024-2026 Tespro Electronics Co., Ltd.
Figure 10-6: Carbon Management - Carbon Calculation
10.5 Operations Management
The Operations Management page provides device operations and alarm management features,
including the following sub-functions:
●Alarm Center: Real-time monitoring of alarm status, displaying statistics for active, critical,
warning, and informational alarms, supporting both real-time and historical alarm viewing
●Alarm Rules: Define alarm conditions for energy consumption anomalies, including
threshold alarms, rate-of-change alarms, and offline alarms, with configurable trigger
conditions, notification methods, and severity levels
●Notification Configuration: Configure alarm notification delivery methods, including email,
SMS, and WebHook channels
●Event Log: Record and view system operation events and operation logs
●Pricing Configuration: Set electricity billing rules, including peak/valley/flat rate time periods
and corresponding rates for automatic electricity cost calculation
Figure 10-8: Operations Management - Alarm Center
10.6 System Settings
The System Settings page configures basic parameters of the Energy Management Service,
including the following functions:
●Service Control: Start, stop, and restart the Energy Management Service, with auto-start on
boot option
●Data Simulator: Generate simulated meter data for testing when no physical devices are
connected
●General Settings: Configure deployment mode (flash storage), data root path, and
collection thread count
```

### Page 44

```text
TR400 Industrial Router User Manual
Page 44 | © 2024-2026 Tespro Electronics Co., Ltd.
Figure 10-10: System Settings
```

### Page 45

```text
TR400 Industrial Router User Manual
Page 45 | © 2024-2026 Tespro Electronics Co., Ltd.
Chapter 11 Cloud Device Management Service
The TR400 supports remote management via the Tespro Cloud Device Management Platform
(mgr.tespro.com). This platform provides device status monitoring, remote Web access, package
distribution, and online firmware upgrade capabilities.
11.1 Service Overview
The Tespro Device Manager is a cloud-based industrial router management platform with the
following key features:
●Real-time device status monitoring: Receive device operational status data via the MQTT
protocol
●Remote Web access: Access the device Web management interface remotely via
WebSocket tunnels
●Package management: Distribute and install software packages on devices
●Online firmware upgrade: Centrally manage and push firmware updates
●Device group management: Organize devices into groups for batch operations
11.2 Registration & Login
Visit https://mgr.tespro.com to access the device management platform.
New users need to register: Enter your email address, send a verification code, and complete
registration.
Registered users log in: Enter your email, obtain a login verification code, verify, and log in.
Figure 11-1: Tespro Device Manager Login Page
```

### Page 46

```text
TR400 Industrial Router User Manual
Page 46 | © 2024-2026 Tespro Electronics Co., Ltd.
11.3 Remote Device Access
When the device's remote access tunnel is enabled and successfully connected to the cloud
platform, you can access the device remotely via its Web access URL.
Remote access URL format: https://<subdomain>.mgr.tespro.com
Tip: The remote access URL can be found on the device Web interface at System > Device
Online Manager > Settings.
11.4 Connection Architecture
The communication between the TR400 and the Cloud Management Platform uses the following
architecture:
This architecture enables devices to be remotely accessed and managed without a public IP
address, making it particularly suitable for devices deployed behind NAT or firewalls.
●Status reporting channel: The device periodically reports operational status to the cloud
platform via the MQTT protocol (TLS encrypted, port 8883)
●Remote access channel: A bidirectional communication channel between the device and
cloud platform is established via WebSocket tunnels (TLS encrypted, port 443)
●All communications support TLS encryption to ensure data transmission security
Appendix A: Technical Specifications
A.1 Hardware Specifications
Parameter
Specification
Processor
Dual-core ARM Cortex-A53 @ 1.3GHz
Memory
512MB
Storage
256MB Flash
Ethernet
5× Gigabit Ethernet ports (4 LAN + 1 WAN)
WiFi
WiFi 6 AX3000: 2.4GHz 574Mbps + 5GHz
2402Mbps
Serial Ports
2× RS485 + 1× RS232, UDP multicast support
Expansion Slot
1× Mini PCIe (4G/5G module)
Power
DC 12V/2A
Dimensions
150×115×35mm
Operating Temperature
-40°C ~ 70°C
Protection Rating
IP30
A.2 Software Specifications
Parameter
Specification
Operating System
TesproOS
Firmware Version
TesproOS 1.0.0
Firewall
nftables (firewall4)
VPN
OpenVPN / WireGuard / IPsec / PPTP
Modbus
RTU/TCP Master + Slave dual role, supports 64
devices and 128 data points
```

### Page 47

```text
TR400 Industrial Router User Manual
Page 47 | © 2024-2026 Tespro Electronics Co., Ltd.
BACnet
IP/MS/TP data collection, 64 devices and 128 data
points
Energy Management
12 sub-function modules
Northbound Protocols
MQTT / TCP / UDP / HTTP / OPC UA Client / OPC
UA Server
Appendix B: Default Configuration
Parameter
Default Value
LAN IP
192.168.0.1
LAN Subnet
192.168.0.0/24
DHCP Range
192.168.0.100 - 192.168.0.200
WiFi SSID (2.4G)
TR400-2.4G (password: 12345678)
WiFi SSID (5G)
TR400-5G (password: 12345678)
WiFi Password
12345678
Admin Username
root
Admin Password
000000
SSH Port
22
HTTP Port
80
NTP Server
pool.ntp.org
Appendix C: Troubleshooting
Unable to Access the Web Management Interface
●Verify that the Ethernet cable is securely connected to a LAN port (not the WAN port).
●Ensure your computer is configured to obtain an IP address automatically (DHCP).
●Try clearing the browser cache or using a different browser.
●Try accessing http://192.168.0.1.
WiFi Not Working
●Confirm that WiFi is enabled under Network > WiFi Network.
●Ensure the antenna is properly connected.
●Try switching to a different WiFi channel to avoid interference.
No Internet Connection
●Verify that the WAN port is connected to your modem or upstream network.
●Check the WAN interface status under Network > Network Interfaces.
●Use the diagnostic tool to Ping an external IP (e.g., 8.8.8.8).
4G/5G Cellular Network Not Connecting
●Confirm that the SIM card is properly inserted and the module is recognized.
```

### Page 48

```text
TR400 Industrial Router User Manual
Page 48 | © 2024-2026 Tespro Electronics Co., Ltd.
●Check signal strength under Status > Cellular Network.
●Verify that the APN settings match your carrier's requirements.
Modbus Communication Error
●Verify that the serial parameters (baud rate, parity, data bits, stop bits) exactly match those
of the slave device.
●Check physical wiring: RS485 requires correct A/B polarity and proper termination resistors.
●Confirm that the slave address is correct and unique on the bus.
●Increase the timeout value if devices respond slowly.
●Use the Serial Service's Debug & Test tool to verify raw serial communication first.
VPN Connection Issues
●Verify that the VPN package is properly installed.
●Check the server address, port, and authentication credentials in the VPN configuration.
●Review system logs for connection errors.
●Ensure the firewall is not blocking the ports and protocols required by the VPN.
Forgot Administrator Password
If you have forgotten the administrator password, perform a factory reset on the router:
1. Power on the router and wait for it to fully boot (approximately 60 seconds).
2. Press and hold the reset button for more than 10 seconds until the LED blinks rapidly.
3. Release the button. The router will restore factory settings and restart.
4. Connect via Ethernet and access http://192.168.0.1, then log in with username root and
password 000000.
Warning: A factory reset will erase all custom configurations. Create regular backups via
System > Backup.
```

### Page 49

```text
TR400 Industrial Router User Manual
Page 49 | © 2024-2026 Tespro Electronics Co., Ltd.
Contact Us
Thank you for choosing the TR400 Industrial Router. If you have any questions or requirements,
please do not hesitate to contact us.
____________________________________
Tespro Electronics Co., Ltd.
Website: www.tespro.com
Email: Info@tespro.com
____________________________________
© 2024-2026 Tespro Electronics Co., Ltd. All rights reserved.
```

---

## CCLI requirements crosswalk (manual-driven)

| ID | Topic | Manual section | Tree |
|----|-------|----------------|------|
| REQ-HW-003 | Ethernet WAN/LAN labels | TBD from manual | K3.1 |
| REQ-HW-004 | RS485 A/B/G + baud | TBD from manual | K5.1 |
| REQ-HW-005 | TPM / Secure Boot | TBD from manual | K6.1 |
| REQ-IO-* | DI/DO count, voltage, pinout | TBD from manual | K5 |
| REQ-VEND-002 | OpenWrt / TesproOS version | TBD from manual | K2 |

---

## Revision history

| Rev | Date | Notes |
|-----|------|-------|
| 1.0 | 2026-09-15 | Auto-extract from `TR400_User_Manual_EN_v1.0.pdf` (49 pp.) |

## Keywords

`TR-400`, `TR400`, `TesPro`, `TG-424`, `TG-524`, `OpenWrt`, `TesproOS`, `RS485`, `DI`, `DO`, `TPM`, `ccli-tr400-user-manual`, `ccli-tr400-user-manual-extract`
