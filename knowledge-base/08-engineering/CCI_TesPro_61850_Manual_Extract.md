# TesPro IEC 61850 Protocol Service — User Manual Extract

**Document ID:** CCLI-VENDOR-TESPRO-61850-EXT-001
**Revision:** 1.0
**Date:** 2026-10-01
**RAG source_id:** `ccli-tespro-61850-manual-extract`
**Source file:** `knowledge-base/08-engineering/reference/vendor/tespro/IEC61850-User-Manual.docx`
**DOCX source_id:** `ccli-tespro-61850-manual`
**Parent:** `ccli-tespro-supplier-correspondence` · `ccli-tg500-lab-platform` · K4.1
**Programme link:** Closes supplier item **V-005** — vendor IEC 61850 capability

---

## Summary

TesPro **IEC 61850 Protocol Service** is a **northbound data-collection gateway feature**, not a
CEI Allegato T **DSO-facing MMS server**. It uses **libiec61850** (`iec61850-mmsd` daemon) to
**poll remote IEDs** (default MMS port **102**), collect points, and **upload JSON to MQTT/TCP**.
TLS in this manual applies to **northbound MQTT/TCP only** — **not** 62351-4 secure MMS on Eth_A.

**CCLI implication:** `apps/ccli` remains the DSO MMS server path. This manual does **not**
document A-profile ACSE auth, port **3782**, or Annex T / `Wlim` control.

| Metric | Value |
|--------|-------|
| Paragraphs extracted | 228 |
| Source on disk | `knowledge-base/08-engineering/reference/vendor/tespro/IEC61850-User-Manual.docx` |

---

## Software packages (§1.0)

| Package | Purpose |
|---------|---------|
| `libiec61850` | IEC 61850 / MMS protocol library |
| `libopen62541` | Supporting library |
| `iec61850-mmsd` | IEC 61850 MMS access **daemon** |
| `iec61850-proto-tespro-combined` | Web UI + collection + upload service |

Delivered as **add-on opkg packages** with TesproOS firmware.

---

## Architecture (inferred)

```text
Remote IED(s)  --MMS/TCP:102-->  iec61850-mmsd (on TG544)
                                      |
                                      v
                              collect / RCB / points
                                      |
                                      v
                         MQTT or TCP northbound (optional TLS)
```

**Not described:** MMS **server** on Eth_A, DSO client connect, 62351-3 TLS on 3782,
62351-4 A-profile AARQ/AARE certificate authentication.

---

## Security / TLS findings (keyword scan)

| # | Extract |
|---|---------|
| 8 | IEC 61850 / MMS protocol library |
| 10 | Supporting library required by the service |
| 11 | iec61850-mmsd |
| 12 | IEC 61850 MMS access daemon |
| 36 | Define where collected data is uploaded (MQTT / TCP). |
| 43 | Top-right buttons: Add Device, Breakpoint, Import Points, Export Points. |
| 45 | Bottom action buttons: Save Device, Delete Device, Control Panel, Report Format, Copy Device, Write Logs. |
| 53 | Unique identifier of the device. It is reported as deviceCode in upload messages. |
| 56 | MMS Port |
| 57 | MMS/TCP port of the IED (default 102). |
| 58 | ICD/CID Path |
| 59 | Path of the device model file (ICD/CID). Use Upload ICD to upload a file from your computer. |
| 60 | Report Control Blocks (RCB) |
| 72 | Copy: click Copy Device to duplicate the current device (useful for several identical IEDs); then adjust Host / Port / Device ID and save. |
| 99 | Report Key |
| 111 | Report Interval (s) |
| 113 | Report Triggers |
| 119 | Report on Change |
| 120 | Tick to upload immediately when the value changes (in addition to periodic reporting). |
| 125 | The header shows the model source, e.g. Offline model (ICD): /etc/....icd. When the device is online the live model is shown instead. |
| 128 | 2.5 Import data points (bulk) |
| 129 | Import Data Points |
| 130 | Click Import Points to bulk-load points from a spreadsheet: |
| 133 | The expected column headers are shown in the dialog: Device, DeviceID, Host, Port, Name, ObjectRef, FC, DataType, Scale, Unit, Interval, ChangeReport, ReportKey, Enabled, Descri... |
| 134 | Click Parse & Preview, check the preview, then confirm to import. |
| 135 | Tip: Devices referenced in the file that do not exist yet are created automatically in a disabled state; enable them and set the RCB / ICD path afterwards if needed. |
| 136 | 2.6 Export data points |
| 137 | Export Data Points |
| 138 | Click Export Points and choose a target and format: |
| 148 | 2.7 Report format (upload JSON template) |
| 149 | Report Format |
| 150 | Click Report Format to define the JSON message sent to the northbound channels. |
| 151 | Template Mode: choose a preset, e.g. Platform Standard Format (REPORT_PROPERTY). |
| 154 | Inner (per point, inside ${properties}): ${value}, ${unit}, ${point_name}, ${report_key}, ${data_type}, ${point_timestamp}, ${register_type}, ${point_address} |
| 156 | { "headers": { "timeSource": "DEVICE", "replay": false }, "messageId": "report_${device_id}_${timestamp}", "messageType": "REPORT_PROPERTY", "deviceCode": "${device_id}", "dataT... |
| 170 | MQTT or TCP. |
| 172 | Broker / server address (host:port). |
| 189 | MQTT or TCP. |
| 192 | Host / Port |
| 193 | Broker (MQTT) or server (TCP) address. |
| 195 | MQTT topic the reports are published to. |
| 197 | MQTT topic for downlink commands (supports ${gatewayCode} / ${device_id} placeholders). |
| 199 | MQTT topic used to reply to downlink commands. |
| 200 | Client ID |
| 201 | MQTT client identifier (leave blank to auto-generate). |
| 203 | MQTT QoS level (0/1/2). |
| 206 | TLS Enabled |
| 207 | Tick to use TLS/SSL for the connection. |
| 208 | CA Certificate |
| 209 | Path to the CA cert; use Browse / Upload to provide a file. |
| 210 | Client Certificate / Client Key |
| 211 | Client cert & key for mutual-TLS, if required. |
| 212 | For a plain (non-TLS) TCP/MQTT link, leave TLS Enabled unticked and the certificate fields empty. Save the channel; the Status column then shows Connected once the link is up. |
| 219 | Use the logs to confirm connections, reporting activity and errors. When contacting support, attach a refreshed log capture. |
| 222 | Device Configuration → Add Device: set Device ID, IED Host, MMS Port; upload the ICD/CID; optionally set RCB references; Save Device. |
| 223 | Add data points: Add Point (or Model Tree to browse, or Import Points for bulk). |
| 224 | Optionally tune the upload payload via Report Format. |
| 225 | Northbound Configuration → Add Channel: configure MQTT/TCP target (and TLS if needed). |

---

## Full section extract

### Preamble

IEC 61850 Protocol Service - User Manual

This manual describes how to operate the IEC 61850 Protocol Service from the router's web management interface. It covers day-to-day configuration

1. Getting Started


### 1.0 Service packages (delivered with the device)

The IEC 61850 service is provided as add-on software packages that accompany the TesproOS firmware. A complete installation consists of the firmware plus the following four packages:

Package

Purpose

libiec61850

IEC 61850 / MMS protocol library

libopen62541

Supporting library required by the service

iec61850-mmsd

IEC 61850 MMS access daemon

iec61850-proto-tespro-combined

The IEC 61850 Protocol Service (web UI + collection + upload)

All other required components are already built into the TesproOS firmware, so the four packages above are the complete IEC 61850 add-on set.

> Tip: Obtaining and installing: These packages are supplied by your vendor together with, or as an update to, the device firmware. Please contact your supplier to obtain the correct package set for your device model and firmware version, and to have the service installed and enabled on the device. No manual package handling or low-level configuration is required from the user.

Once the service is installed and running, everything below is operated from the web interface.


### 1.1 Open the service page

Log in to the router web interface.

In the left menu choose Services → IEC 61850 Protocol.

The service page opens on the Device Configuration tab.


### 1.2 Service status and start / stop

**At the top of the page you will see:**

Service title and version, e.g. IEC 61850 Protocol Service v1.0.0-r64.

A status badge: Running (green) when the service is active.

**A Stop / Start button on the top-right:**

Click Stop to halt the service (no collection / no upload while stopped).

Click Start to run it again.

> Tip: Configuration changes are applied after saving; if a change does not take effect, stop and start the service once.


### 1.3 The three tabs

Tab

Purpose

Device Configuration

Define IED devices and the data points to collect.

Northbound Configuration

Define where collected data is uploaded (MQTT / TCP).

Service Logs

View runtime logs for troubleshooting.

2. Device Configuration

Device Configuration


### 2.1 Page layout

Device tabs (e.g. Simulated IED, Simulated IED 2, …): one tab per configured IED. A green dot next to a device name means the device is currently connected; no dot / grey means disconnected.

Top-right buttons: Add Device, Breakpoint, Import Points, Export Points.

Device form: the connection and identity settings of the selected device.

Bottom action buttons: Save Device, Delete Device, Control Panel, Report Format, Copy Device, Write Logs.

Data Points table (below the form): the points collected from this device.


### 2.2 Device fields

Field

Meaning

Device Name

Friendly name; also used as the device tab label.

Device ID (required)

Unique identifier of the device. It is reported as deviceCode in upload messages.

IED Host

IP address or hostname of the IED to connect to.

MMS Port

MMS/TCP port of the IED (default 102).

ICD/CID Path

Path of the device model file (ICD/CID). Use Upload ICD to upload a file from your computer.

Report Control Blocks (RCB)

Comma-separated list of full RCB references (with the FC segment, BR=buffered / RP=unbuffered). Leave blank to use polled reads only.

Enabled

Tick to activate this device.

Description

Optional free text.

Default Point Codes

Comma-separated point codes used when a read request does not specify points. Blank = read all points of the device.


### 2.3 Add / edit / delete / copy a device

Add: click Add Device (top-right), fill the form, then click Save Device.

Edit: select the device tab, change fields, click Save Device.

Delete: select the device tab, click Delete Device and confirm.

Copy: click Copy Device to duplicate the current device (useful for several identical IEDs); then adjust Host / Port / Device ID and save.


### 2.4 Data Points

**The Data Points table lists what is collected from the selected device:**

Column

Meaning

Name

Point name (unique per device).

Object Reference

IEC 61850 object reference, e.g. simpleIOGenericIO/GGIO1.AnIn1.mag.f.

FC

Functional constraint, e.g. MX (measurement).

Data Type

e.g. float32.

Interval (s)

Polling interval in seconds.

Realtime Value

Latest collected value (auto-refreshed).

Actions

Edit / Delete the point.

Buttons above the table: Model Tree and Add Point.


### 2.4.1 Add or edit a data point

Data Point editor

**Click Add Point (or Edit on a row) to open the editor:**

Field

Meaning

Name

Point name.

Report Key

Key used for this point inside the uploaded properties object.

Object Reference (required)

The IEC 61850 reference to read. Use Browse to pick it from the model tree instead of typing.

FC

Functional constraint (e.g. MX).

Data Type

Value type (e.g. float32).

Scale Factor

Multiplier applied to the raw value (default 1).

Unit

Engineering unit label (e.g. V, A, kW).

Report Interval (s)

Polling interval in seconds.

Report Triggers

RCB trigger options: Data change / Quality change / Data update / Integrity / GI.

Description

Optional free text.

Enabled

Tick to collect this point.

Report on Change

Tick to upload immediately when the value changes (in addition to periodic reporting).

Click Save to store the point, Cancel to discard.


### 2.4.2 Browse the model tree

Model Tree

**Click Model Tree (or Browse inside the point editor) to open the model browser:**

The header shows the model source, e.g. Offline model (ICD): /etc/....icd. When the device is online the live model is shown instead.

Expand LD → LN → DO → DA nodes; each leaf shows its functional constraint (e.g. [ST], [MX], [DC]).

Tick the checkboxes of the leaves you want (or use Select all) to add them as data points, then close the dialog.


### 2.5 Import data points (bulk)

Import Data Points

**Click Import Points to bulk-load points from a spreadsheet:**

File: choose a .csv or .xlsx file.

Conflict mode: e.g. Append - skip existing (key=Name) - rows whose Name already exists are skipped.

The expected column headers are shown in the dialog: Device, DeviceID, Host, Port, Name, ObjectRef, FC, DataType, Scale, Unit, Interval, ChangeReport, ReportKey, Enabled, Description.

Click Parse & Preview, check the preview, then confirm to import.

> Tip: Devices referenced in the file that do not exist yet are created automatically in a disabled state; enable them and set the RCB / ICD path afterwards if needed.


### 2.6 Export data points

Export Data Points

**Click Export Points and choose a target and format:**

Target

Content

Current device

Points of the selected device only.

All devices

Points of every configured device.

Empty template (with dropdowns)

A blank template with valid-value dropdowns, for filling in offline.

Formats: XLSX or CSV. The XLSX template includes hint rows for the ObjectRef / FC / DataType / Y-N columns.


### 2.7 Report format (upload JSON template)

Report Format

Click Report Format to define the JSON message sent to the northbound channels.

Template Mode: choose a preset, e.g. Platform Standard Format (REPORT_PROPERTY).

**Placeholders you can use in the template:**

Outer: ${device_id}, ${timestamp}, ${device_name}, ${device_address}, ${properties}

Inner (per point, inside ${properties}): ${value}, ${unit}, ${point_name}, ${report_key}, ${data_type}, ${point_timestamp}, ${register_type}, ${point_address}

JSON Template: the editable message skeleton. The default standard format looks like:

{ "headers": { "timeSource": "DEVICE", "replay": false }, "messageId": "report_${device_id}_${timestamp}", "messageType": "REPORT_PROPERTY", "deviceCode": "${device_id}", "dataTime": ${timestamp}000, "reportTime": ${timestamp}000, "properties": ${properties}}

Edit only if your platform requires a different payload; otherwise keep the standard format.


### 2.8 Other device buttons

Control Panel: open the manual control / test panel for the selected device.

Write Logs: write a diagnostic snapshot to the service log.

Breakpoint: resume/continue a paused (breakpoint) collection session.

3. Northbound Configuration (data upload)

Northbound Channels

The Northbound Configuration tab lists the upload channels.

Column

Meaning

Name

Channel name (e.g. channel_1).

Type

MQTT or TCP.

Target

Broker / server address (host:port).

Enabled

Whether the channel is active.

Status

Live link state: Connected (green) / Disconnected (red).

Sent

Number of messages sent on this channel.

Actions

Edit / Delete.

Click Add Channel to create a channel; Edit to modify one.


### 3.1 Channel settings

Edit Northbound Channel

Field

Meaning

Name

Channel name.

Type

MQTT or TCP.

Enabled

Tick to activate the channel.

Host / Port

Broker (MQTT) or server (TCP) address.

Publish Topic

MQTT topic the reports are published to.

Subscribe Topic

MQTT topic for downlink commands (supports ${gatewayCode} / ${device_id} placeholders).

Reply Topic

MQTT topic used to reply to downlink commands.

Client ID

MQTT client identifier (leave blank to auto-generate).

QoS

MQTT QoS level (0/1/2).

Username / Password

Broker credentials, if required.

TLS Enabled

Tick to use TLS/SSL for the connection.

CA Certificate

Path to the CA cert; use Browse / Upload to provide a file.

Client Certificate / Client Key

Client cert & key for mutual-TLS, if required.

For a plain (non-TLS) TCP/MQTT link, leave TLS Enabled unticked and the certificate fields empty. Save the channel; the Status column then shows Connected once the link is up.

4. Service Logs

Service Logs

**The Service Logs tab shows runtime logs for troubleshooting:**

Log Type: choose the log source (e.g. Southbound).

Lines: how many recent lines to display (e.g. 100).

Refresh: reload the log view.

Use the logs to confirm connections, reporting activity and errors. When contacting support, attach a refreshed log capture.

5. Typical workflow (quick reference)

Services → IEC 61850 Protocol, ensure the service is Running.

Device Configuration → Add Device: set Device ID, IED Host, MMS Port; upload the ICD/CID; optionally set RCB references; Save Device.

Add data points: Add Point (or Model Tree to browse, or Import Points for bulk).

Optionally tune the upload payload via Report Format.

Northbound Configuration → Add Channel: configure MQTT/TCP target (and TLS if needed).

Verify: device tab shows a green dot, channel Status = Connected, Sent counter increases, and Realtime Value updates.

If anything misbehaves, check Service Logs.


---

## Requirements traceability

| ID | Requirement | Manual evidence | Status |
|----|-------------|-----------------|--------|
| REQ-VEND-006 | CEI / Allegato T DSO MMS server | Not covered — collector only | **NOT MET** |
| REQ-3514-TPROF | 62351-3 TLS MMS port 3782 | Not mentioned | **MISSING** |
| REQ-3514-APROF | 62351-4 ACSE certificate auth | Not mentioned | **MISSING** |
| REQ-61850-001 | MMS server on Eth_A (CCLI) | Different product path | **N/A** — use `apps/ccli` |
| REQ-VEND-005 | Vendor 61850 stack identity | libiec61850 + mmsd + web UI | **HAVE** |

---

## Knowledge gaps

| Area | Current | Required | Gap |
|------|---------|----------|-----|
| DSO MMS server | Not in manual | Eth_A :3782 TLS + Allegato T | **CCLI owns** |
| 62351-4 A-profile | Not in manual | AARQ/AARE auth | **CCLI owns** |
| Northbound MQTT TLS | Documented | Optional upload path | **HAVE** (out of CCI scope) |
| Coexistence with `ccli` | Not documented | Port conflict policy | **OPEN** — ask TesPro |

---

## Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-VEND-003 | Vendor IEC 61850 ≠ CEI Allegato T | False cert confidence | Keep libiec61850 in `apps/ccli` |
| R-TG-001 | Manual describes **client/collector** role | Team assumes TesPro covers DSO | This extract + supplier V-005 close |

---

## Verification

| Check | Pass criteria |
|-------|---------------|
| Packages on DUT | `opkg list-installed \| grep iec61850` shows mmsd + proto service |
| Web UI | Services → IEC 61850 Protocol page loads |
| vs CCLI | `ccli` on :3782 can run without mmsd binding same Eth_A port |

---

## Keywords

`TesPro`, `iec61850-mmsd`, `libiec61850`, `northbound`, `MQTT`, `MMS client`,
`ccli-tespro-61850-manual`, `ccli-tespro-61850-manual-extract`, `V-005`
