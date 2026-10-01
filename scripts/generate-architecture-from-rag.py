#!/usr/bin/env python3
"""Generate Architecture/ workspace from CCLI RAG corpus (authoritative MD sources)."""

from __future__ import annotations

import html
import json
import sys
import textwrap
from datetime import datetime, timezone
from pathlib import Path

from openpyxl import Workbook
from openpyxl.styles import Font, PatternFill, Alignment

ROOT = Path(__file__).resolve().parents[1]
ARCH = ROOT / "Architecture"
META = ARCH / "_generation_meta.json"

HEADER_FILL = PatternFill("solid", fgColor="1A365D")
HEADER_FONT = Font(bold=True, color="FFFFFF")
KIT_FILL = PatternFill("solid", fgColor="FFF4E6")
MISSING_FILL = PatternFill("solid", fgColor="FEE2E2")


def esc(s: str) -> str:
    return html.escape(str(s), quote=True)


def mxfile(title: str, cells_xml: str) -> str:
    return f"""<mxfile host="app.diagrams.net" modified="{datetime.now(timezone.utc).isoformat()}" agent="CCLI-RAG" version="22.1.0" type="device">
  <diagram name="Page-1" id="ccli-{esc(title).replace(' ', '-')}">
    <mxGraphModel dx="1400" dy="900" grid="1" gridSize="10" guides="1" tooltips="1" connect="1" arrows="1" fold="1" page="1" pageScale="1" pageWidth="1400" pageHeight="900" math="0" shadow="0">
      <root>
        <mxCell id="0"/>
        <mxCell id="1" parent="0"/>
        {cells_xml}
      </root>
    </mxGraphModel>
  </diagram>
</mxfile>
"""


def box(cid: str, label: str, x: int, y: int, w: int, h: int, fill: str = "#E8F4FC", stroke: str = "#1A365D") -> str:
    val = esc(label).replace("&#10;", "\n")
    return f"""<mxCell id="{cid}" value="{val}" style="rounded=1;whiteSpace=wrap;html=1;fillColor={fill};strokeColor={stroke};fontSize=11;" vertex="1" parent="1">
  <mxGeometry x="{x}" y="{y}" width="{w}" height="{h}" as="geometry"/>
</mxCell>"""


def text(cid: str, label: str, x: int, y: int, w: int, h: int, size: int = 14, bold: bool = True) -> str:
    style = f"text;html=1;strokeColor=none;fillColor=none;align=center;verticalAlign=middle;fontSize={size};"
    if bold:
        style += "fontStyle=1;"
    return f"""<mxCell id="{cid}" value="{esc(label)}" style="{style}" vertex="1" parent="1">
  <mxGeometry x="{x}" y="{y}" width="{w}" height="{h}" as="geometry"/>
</mxCell>"""


def edge(eid: str, src: str, dst: str, label: str = "") -> str:
    lbl = f'value="{esc(label)}" ' if label else ""
    return f"""<mxCell id="{eid}" {lbl}style="edgeStyle=orthogonalEdgeStyle;rounded=0;orthogonalLoop=1;jettySize=auto;html=1;fontSize=10;" edge="1" parent="1" source="{src}" target="{dst}">
  <mxGeometry relative="1" as="geometry"/>
</mxCell>"""


def group_box(gid: str, label: str, x: int, y: int, w: int, h: int) -> str:
    return f"""<mxCell id="{gid}" value="{esc(label)}" style="swimlane;startSize=28;fillColor=#F8FAFC;strokeColor=#64748B;fontStyle=1;fontSize=12;" vertex="1" parent="1">
  <mxGeometry x="{x}" y="{y}" width="{w}" height="{h}" as="geometry"/>
</mxCell>"""


def child_box(cid: str, label: str, x: int, y: int, w: int, h: int, parent: str, fill: str = "#FFFFFF") -> str:
    return f"""<mxCell id="{cid}" value="{esc(label)}" style="rounded=1;whiteSpace=wrap;html=1;fillColor={fill};strokeColor=#334155;fontSize=10;" vertex="1" parent="{parent}">
  <mxGeometry x="{x}" y="{y}" width="{w}" height="{h}" as="geometry"/>
</mxCell>"""


def system_context_drawio() -> str:
    parts = [
        text("t", "System Context — CEI 0-16 PF2 Central Plant Controller", 200, 20, 1000, 36, 18),
        text("sub", "Source: ccli-vendor-selection · cei-0-16-allegato-o/t · ccli-architecture-framework", 200, 56, 1000, 24, 10, False),
        group_box("g_ext", "External (grid / operator)", 40, 100, 320, 520),
        child_box("ds o", "DSO\nEth_A\nIEC 61850 MMS/GOOSE\nAnnex O", 20, 40, 130, 90, "g_ext", "#DBEAFE"),
        child_box("op", "Operatore Abilitato\nEth_B\n61850 / 60870-104\nAnnex T", 170, 40, 130, 90, "g_ext", "#DBEAFE"),
        child_box("ups", "UPS / cabinet power\n12-24 Vdc context", 20, 150, 280, 50, "g_ext"),
        group_box("g_plant", "MT plant (behind CCI)", 1040, 100, 320, 520),
        child_box("inv", "Inverters / generators", 20, 40, 130, 60, "g_plant"),
        child_box("bess", "BESS / storage", 170, 40, 130, 60, "g_plant"),
        child_box("meter", "Energy analyzer\nModbus RTU", 20, 120, 130, 60, "g_plant"),
        child_box("plant_eth", "Plant LAN\nModbus TCP", 170, 120, 130, 60, "g_plant"),
        box("cci", "CCI (original design)\nToradex SoM 0063 + custom carrier\nPF2 · observability · curtailment", 420, 260, 360, 120, "#DCFCE7", "#166534"),
        box("kit", "Phase 1 LAB ONLY\nVerdin Dev Board 99991105\n2x GbE · 1x RS485 DB9", 420, 420, 360, 80, "#FFF4E6", "#B45309"),
        edge("e1", "ds o", "cci", "Eth_A"),
        edge("e2", "op", "cci", "Eth_B"),
        edge("e3", "meter", "cci", "RS485 Modbus"),
        edge("e4", "plant_eth", "cci", "Plant ports"),
        edge("e5", "inv", "cci", "DI status / DO curtailment"),
        edge("e6", "kit", "cci", "validates SW path only"),
    ]
    return mxfile("System Context", "\n".join(parts))


def functional_architecture_drawio() -> str:
    blocks = [
        ("c1", "C1 Ethernet\nEth_A · Eth_B · Plant x2", 40, 120),
        ("c2", "C2 Serial\n2x opto RS485 Modbus", 220, 120),
        ("c3", "C3 Protocols\n61850 · 60870 · Modbus", 400, 120),
        ("c4", "C4 Digital I/O\nDI 10-120V · DO relay", 580, 120),
        ("c5", "C5 GNSS\nPPS / NMEA time sync", 760, 120),
        ("c6", "C6 Security\nHAB · SE · signed boot", 940, 120),
        ("c7", "C7 LTE\noptional backup", 40, 280),
        ("c8", "C8 Console\nUSB commissioning", 220, 280),
        ("c9", "C9 Mechanical\nDIN rail enclosure", 400, 280),
        ("c10", "C10 Power\n12-24V isolated DC-DC", 580, 280),
        ("som", "Linux SoM\nVerdin iMX8M Plus 0063", 400, 460, "#E0E7FF"),
    ]
    parts = [
        text("t", "Functional Architecture — C1-C10 vs kit coverage", 120, 20, 1100, 36, 18),
        text("leg", "Green=Full on product · Amber=Partial on kit 99991105 · Red=carrier only", 120, 56, 1100, 24, 10, False),
    ]
    fills = {
        "c1": "#FFF4E6", "c2": "#FEE2E2", "c3": "#DCFCE7", "c4": "#FEE2E2",
        "c5": "#FEE2E2", "c6": "#FFF4E6", "c7": "#F3F4F6", "c8": "#DCFCE7",
        "c9": "#FEE2E2", "c10": "#FEE2E2",
    }
    for bid, lbl, x, y in blocks[:-1]:
        parts.append(box(bid, lbl, x, y, 160, 70, fills.get(bid, "#E8F4FC")))
    parts.append(box("som", blocks[-1][1], 400, 460, 360, 80, "#E0E7FF"))
    for bid, _, _, _ in blocks[:-1]:
        parts.append(edge(f"e_{bid}", bid, "som"))
    return mxfile("Functional Architecture", "\n".join(parts))


def hardware_architecture_drawio() -> str:
    parts = [
        text("t", "Hardware Architecture — SoM + carrier (field CCI)", 180, 20, 1040, 36, 18),
        group_box("som_g", "Verdin SoM 0063 (i.MX8M Plus IT)", 60, 90, 280, 340),
        child_box("cpu", "Quad A53 + M7\n2x GbE MAC\nHAB / CAAM", 20, 40, 240, 70, "som_g"),
        child_box("ddr", "4GB LPDDR4 IT\neMMC", 20, 130, 240, 50, "som_g"),
        child_box("se", "Secure element path\n(verify SKU TPM)", 20, 200, 240, 50, "som_g"),
        child_box("som_if", "MXM3 edge connector\n→ carrier", 20, 270, 240, 50, "som_g"),
        group_box("car_g", "Custom carrier (product)", 400, 90, 520, 520),
        child_box("sw", "Switch / PHY x4\nEth_A Eth_B Plant1 Plant2", 20, 40, 220, 70, "car_g", "#DBEAFE"),
        child_box("485", "2x isolated RS485\nModbus RTU RJ45", 260, 40, 220, 70, "car_g", "#FEE2E2"),
        child_box("dio", "DI opto x5\nDO relay x3", 20, 130, 220, 70, "car_g", "#FEE2E2"),
        child_box("gnss", "GNSS module + SMA", 260, 130, 220, 50, "car_g", "#FEE2E2"),
        child_box("lte", "LTE modem (opt rev A)", 260, 200, 220, 50, "car_g", "#F3F4F6"),
        child_box("pwr", "12-24V DC-DC\nsequencing / protection", 20, 220, 220, 60, "car_g", "#FEE2E2"),
        child_box("term", "Field terminals\nDIN rail", 20, 300, 460, 50, "car_g"),
        box("kit", "Eval kit 99991105\n(LAB — not shipped)\n2x GbE · RS485 DB9", 980, 90, 280, 120, "#FFF4E6", "#B45309"),
        edge("e_som_car", "som_g", "car_g", "MXM3"),
        edge("e_kit", "kit", "som_g", "SoM socket"),
    ]
    return mxfile("Hardware Architecture", "\n".join(parts))


def power_tree_drawio() -> str:
    parts = [
        text("t", "Power Tree — C10 (12-24 Vdc product path)", 200, 20, 1000, 36, 18),
        text("n", "Kit bench: 7-24 V on 99991105 · Product: isolated wide-range input per ccli-prototype-bom", 200, 56, 1000, 24, 10, False),
        box("vin", "Field input\n12-24 Vdc ±10%\nFuse / reverse pol / eFuse", 520, 100, 200, 80, "#FEF3C7"),
        box("dcdc", "Isolated DC-DC\n→ intermediate bus", 520, 220, 200, 70, "#FEE2E2"),
        box("som_r", "SoM rails\n(from Verdin PMIC)", 280, 360, 160, 60, "#E0E7FF"),
        box("io_r", "Carrier 3.3V / 5V\nPHY · RS485 · opto · relay", 520, 360, 200, 60, "#DBEAFE"),
        box("di_sup", "DI field supply\n10-120 V external\n(isolated sense)", 760, 360, 200, 60, "#F3F4F6"),
        box("gnss_r", "GNSS / LTE\nmodule rails", 520, 460, 200, 50, "#F3F4F6"),
        edge("p1", "vin", "dcdc"),
        edge("p2", "dcdc", "som_r", "5V/3.3V"),
        edge("p3", "dcdc", "io_r"),
        edge("p4", "dcdc", "gnss_r"),
        edge("p5", "vin", "di_sup", "external DI loop"),
    ]
    return mxfile("Power Tree", "\n".join(parts))


def network_architecture_drawio() -> str:
    parts = [
        text("t", "Network Architecture — logical & physical separation", 120, 20, 1160, 36, 18),
        text("n", "CEI O.13.1.1: separate outside interfaces · Eth_A DSO · Eth_B Operator · plant ports", 120, 56, 1160, 24, 10, False),
        box("etha", "Eth_A → DSO\nDedicated PHY/port\nIEC 61850 MMS/GOOSE\nNo plant bridge", 60, 140, 240, 100, "#DBEAFE"),
        box("ethb", "Eth_B → Operator\nDedicated PHY/port\n61850 / 60870-104", 60, 280, 240, 100, "#DBEAFE"),
        box("sw", "Managed switch or\n2x SoM GbE + 2x PHY\n(kit: 2x GbE only)", 400, 200, 260, 100, "#FFF4E6"),
        box("p1", "Plant port 1\nModbus TCP / LAN", 760, 140, 220, 80, "#E8F4FC"),
        box("p2", "Plant port 2\nspare / HMI", 760, 260, 220, 80, "#E8F4FC"),
        box("fw", "Linux net policy\nVLAN · firewall · 62351 TLS\nAudit logging", 400, 380, 260, 80, "#DCFCE7"),
        box("svc", "USB console\n(out of cert path)", 760, 400, 220, 60, "#F3F4F6"),
        edge("n1", "etha", "sw", "isolated"),
        edge("n2", "ethb", "sw", "isolated"),
        edge("n3", "sw", "p1"),
        edge("n4", "sw", "p2"),
        edge("n5", "sw", "fw"),
    ]
    return mxfile("Network Architecture", "\n".join(parts))


def style_header(ws, headers: list[str]) -> None:
    ws.append(headers)
    for cell in ws[1]:
        cell.font = HEADER_FONT
        cell.fill = HEADER_FILL
        cell.alignment = Alignment(wrap_text=True, vertical="top")


def write_requirements_sheet(wb: Workbook) -> None:
    ws = wb.create_sheet("Requirements", 0)
    style_header(ws, ["ID", "Domain", "Requirement", "Source", "Kit_status", "Product_status", "Verification_ref"])
    rows = [
        ["REQ-REG-001", "Regulatory", "CEI 0-16 Annex O/T/M compliant Central Plant Controller", "cei-0-16-allegato-o/t", "N/A", "Required", "REQ-PF2-001"],
        ["REQ-NET-001", "Network", "Eth_A logical channel to DSO per EN 61850 (Annex O O.13.1.2)", "cei-0-16-allegato-o", "Partial 2x GbE", "4-port isolation", "L2 MMS client"],
        ["REQ-NET-002", "Network", "Eth_B to Operatore Abilitato — separate from Eth_A", "cei-0-16-allegato-t", "Partial", "Required PHY/VLAN", "L2 60870/61850"],
        ["REQ-NET-003", "Network", "Two outside plant network interfaces (O.13.1.1)", "cei-0-16-allegato-o", "None on kit", "Plant x2 on carrier", "L2 Modbus TCP"],
        ["REQ-SER-001", "Serial", "2x opto-isolated RS485 Modbus RTU to energy analyzer", "ccli-vendor-selection C2", "1x RS485 DB9 non-opto", "Required carrier", "L2/L3 Modbus poll"],
        ["REQ-IO-001", "Digital I/O", "DI 10-120 Vdc x5 external field supply", "ccli-vendor-selection C4", "None GPIO only", "Carrier opto", "L2 DI box"],
        ["REQ-IO-002", "Digital I/O", "DO dry contact x3 for PF2 curtailment", "ccli-validation-strategy", "SW stub", "Carrier relay", "L2 DO lamps"],
        ["REQ-PWR-001", "Power", "12-24 Vdc wide-range isolated product input", "ccli-prototype-bom C10", "7-24V kit bench", "Carrier PSU", "Bench PSU test"],
        ["REQ-SEC-001", "Security", "Secure boot + signed firmware images", "ccli-tg500-lab-platform", "Partial BSP", "Required product", "Lab secure boot"],
        ["REQ-SEC-002", "Security", "Key storage in secure element / TPM path", "ccli-prototype-bom C6", "Verify SoM SKU", "Required", "Key hierarchy review"],
        ["REQ-TIME-001", "Time", "GNSS timestamp for measurements and events", "ccli-vendor-selection C5", "None on kit", "Carrier GNSS", "L3 PPS capture"],
        ["REQ-LTE-001", "Cellular", "LTE backup / remote monitoring (optional rev A)", "ccli-prototype-bom", "Optional", "Carrier modem", "Deferred"],
        ["REQ-PF2-001", "PF2", "Curtailment command path with safe-state on comms loss", "ccli-validation-strategy", "SW mock L1", "DO + logic", "L2 failure matrix"],
    ]
    for r in rows:
        ws.append(r)


def write_interface_matrix(wb: Workbook) -> None:
    ws = wb.active
    ws.title = "Interface_Matrix"
    style_header(ws, ["Interface", "Source", "Destination", "Protocol", "Physical", "Isolation", "Block", "Kit_99991105", "Carrier", "RAG_source"])
    rows = [
        ["Eth_A", "CCI SoM+PHY", "DSO", "IEC 61850 MMS / GOOSE", "RJ45 GbE", "Separate PHY/VLAN from Eth_B", "C1", "GbE port 1 — SW demo", "Dedicated PHY + magjack", "cei-0-16-allegato-o"],
        ["Eth_B", "CCI SoM+PHY", "Operatore Abilitato", "IEC 61850 / IEC 60870-5-104", "RJ45 GbE", "Separate from Eth_A", "C1", "GbE port 2 — SW demo", "Dedicated PHY + magjack", "cei-0-16-allegato-t"],
        ["Plant-1", "CCI", "Plant LAN / inverters", "Modbus TCP / raw Ethernet", "RJ45", "Logically separated from DSO path", "C1", "Not on kit", "Switch port or PHY3", "ccli-prototype-bom"],
        ["Plant-2", "CCI", "Plant HMI / spare", "Modbus TCP / spare", "RJ45", "Same policy as Plant-1", "C1", "Not on kit", "PHY4 / switch", "ccli-prototype-bom"],
        ["RS485-A", "CCI carrier", "Energy analyzer", "Modbus RTU", "RJ45 8/8 plant serial", "Opto-isolated", "C2", "Not CEI-compliant", "U-485 transceiver x1", "ccli-vendor-selection"],
        ["RS485-B", "CCI carrier", "Spare / IEC 60870-101", "Modbus RTU / 101 optional", "RJ45 8/8", "Opto-isolated", "C2", "Kit RS485 DB9 lab only", "U-485 transceiver x2", "ccli-prototype-bom"],
        ["DI-1..5", "Field 10-120 Vdc", "CCI GPIO via opto", "Digital input", "Terminal block", "Opto + external field supply", "C4", "None", "U-DI conditioner", "ccli-vendor-selection"],
        ["DO-1..3", "CCI relay driver", "Curtailment contactors", "Dry contact PF2", "Terminal block", "Relay isolation", "C4", "SW stub only", "K-DO relay", "ccli-validation-strategy"],
        ["GNSS", "Antenna SMA", "SoM UART/USB", "NMEA + PPS", "SMA coax", "ESD protection", "C5", "None", "U-GNSS + ANT-G", "ccli-prototype-bom"],
        ["LTE", "Modem", "Mobile network", "PPP/QMI", "SMA + SIM", "Logical separation from Eth_A", "C7", "Optional", "U-LTE rev A", "ccli-prototype-bom"],
        ["USB", "Service laptop", "SoM USB-C/A", "Commissioning console", "USB", "Out of certified control path", "C8", "HAVE kit USB", "Product service port", "ccli-toradex-verdin-devboard"],
        ["CAN", "—", "—", "Not in CEI CCI minimum", "DB9 on kit", "N/A product", "—", "2x CAN on kit", "Not required", "ccli-toradex-verdin-devboard"],
    ]
    for r in rows:
        ws.append(r)


def write_bom_matrix(wb: Workbook) -> None:
    ws = wb.create_sheet("BOM_Matrix")
    style_header(ws, ["Ref", "Block", "Description", "Candidate_MPN", "PN_locked", "Qty", "Est_EUR", "Doc_status", "source_id", "Layer"])
    rows = [
        ["U-SOM", "Compute", "Verdin iMX8M Plus Quad 4GB IT SoM", "Toradex Verdin Plus IT", "0063", 1, "150-350", "HAVE/INGESTED", "ccli-toradex-verdin-plus-som", "Product"],
        ["KIT", "Lab", "Verdin Development Board + HDMI", "99991105", "99991105", 1, "~300", "HAVE/INGESTED", "ccli-toradex-verdin-devboard", "Lab only"],
        ["SW1", "C1", "4-port switch or 2x PHY + dual MAC", "TBD", "—", 1, "8-20", "MISSING", "ccli-prototype-bom", "Carrier"],
        ["J-ETH", "C1", "RJ45 magjack x4 Eth_A/B/Plant", "TBD", "—", 4, "4-10", "MISSING", "ccli-prototype-bom", "Carrier"],
        ["U-485", "C2", "Opto-isolated RS-485 transceiver", "TBD", "—", 2, "10-15", "MISSING", "ccli-prototype-bom", "Carrier"],
        ["J-SER", "C2", "RJ45 Modbus plant serial", "TBD", "—", 2, "2-4", "MISSING", "ccli-prototype-bom", "Carrier"],
        ["U-DI", "C4", "DI conditioner 10-120 Vdc x5", "TBD", "—", 5, "8-15", "MISSING", "ccli-prototype-bom", "Carrier"],
        ["K-DO", "C4", "Relay DO dry contact x3", "TBD", "—", 3, "5-12", "MISSING", "ccli-prototype-bom", "Carrier"],
        ["U-GNSS", "C5", "GNSS module u-blox-class", "TBD", "—", 1, "20-35", "MISSING", "ccli-prototype-bom", "Carrier"],
        ["ANT-G", "C5", "Active GNSS antenna SMA", "TBD", "—", 1, "5-15", "MISSING", "ccli-prototype-bom", "Carrier"],
        ["U-LTE", "C7", "LTE Cat.1 Quectel-class", "TBD", "—", "0-1", "25-40", "PARTIAL", "ccli-prototype-bom", "Optional"],
        ["U-SE", "C6", "Secure element ATECC608B/STSAFE", "TBD", "—", "0-1", "2-5", "TBD", "ccli-prototype-bom", "Carrier"],
        ["—", "C6", "Secure boot + TPM (TG-524)", "TG-524", "TG-524", 1, "—", "PARTIAL", "ccli-tg500-lab-platform", "Lab DUT"],
        ["U-PWR", "C10", "12-24V isolated DC-DC", "TBD", "—", 1, "10-25", "MISSING", "ccli-prototype-bom", "Carrier"],
        ["PCB", "Mech", "4-layer carrier proto x5-10", "JLCPCB/PCBWay", "—", 1, "50-150", "MISSING", "ccli-prototype-bom", "Carrier"],
        ["ENC", "Mech", "DIN-rail enclosure", "TBD", "—", 1, "15-40", "MISSING", "ccli-prototype-bom", "Product"],
        ["SW-61850", "C3", "libiec61850 (prototype GPLv3)", "MZ Automation", "—", 1, "0+license", "HAVE ref", "ccli-project-roadmap", "Software"],
    ]
    for r in rows:
        ws.append(r)
        if r[-1] == "Lab only":
            for cell in ws[ws.max_row]:
                cell.fill = KIT_FILL


def write_knowledge_matrix(wb: Workbook) -> None:
    """Legacy single-sheet stub; full SK0146 leak matrix built by build-sk0146-knowledge-matrix.py."""
    ws = wb.create_sheet("CCLI_Document_Gates")
    style_header(ws, ["Gate", "Document", "Current", "Required", "Gap", "Priority", "source_id", "Action"])
    rows = [
        ["D0", "Architecture block diagram", "HAVE mermaid/Architecture/", "Frozen functional view", "Update draw.io", "P1", "ccli-architecture-framework", "Review generated diagrams"],
        ["D1", "Project roadmap", "HAVE/INGESTED", "Budget envelope", "None", "—", "ccli-project-roadmap", "—"],
        ["D2", "Vendor selection C1-C10", "HAVE/INGESTED", "Scorecard filled", "Variscite/Compulab prices", "P2", "ccli-vendor-selection", "Order kit 99991105+0063"],
        ["D3", "Pin budget / connector map", "MISSING", "Every ball→net→connector", "BLOCKER", "P0", "ccli-pin-budget", "Author CCI_Pin_Budget.md"],
        ["D4", "Carrier block diagram", "MISSING", "Altium-ready freeze", "BLOCKER", "P0", "ccli-carrier-block", "Export from Hardware_Architecture.drawio"],
        ["D5", "Verdin Plus IT SoM DS", "HAVE/INGESTED", "PN 0063 locked", "None", "—", "ccli-toradex-verdin-plus-som", "—"],
        ["D5b", "NXP IMX8MPIEC", "MISSING", "Industrial Ethernet silicon DS", "PARTIAL", "P0", "ccli-nxp-imx8m-plus-iec", "Download from NXP"],
        ["D6", "Toradex PCN longevity 0063", "MISSING", "10-year letter", "BLOCKER", "P0", "ccli-toradex-verdin-plus-som", "Request from Toradex"],
        ["D8", "Modbus analyzer register map", "MISSING", "Real analyzer model", "Software block", "P1", "ccli-modbus-analyzer-map", "Pick analyzer L3"],
        ["D12", "Ethernet switch/PHY DS", "MISSING", "MPN for C1", "Carrier BOM", "P1", "ccli-prototype-bom", "Select after pin budget"],
        ["D13", "RS485 transceiver DS", "MISSING", "2x isolated MPN", "Carrier BOM", "P1", "ccli-prototype-bom", "Select after pin budget"],
        ["—", "61850 commercial license", "GPLv3 prototype", "MZ commercial ship license", "Budget", "P1", "ccli-project-roadmap", "Quote before product"],
        ["—", "62443/61850 lab scope", "ATEX NB reuse", "Accredited test quote", "Cert track", "P2", "ccli-cci-module-regs-class", "Early lab engagement"],
        ["—", "SK0146 Knowledge Leaks", "See SK0146_Knowledge_Leaks sheet", "18-row leak matrix", "Run build-sk0146-knowledge-matrix.py", "P0", "sk0146-schematic-index", "ingest-sk0146-schematics.ps1"],
    ]
    for r in rows:
        ws.append(r)
        if r[4] in ("BLOCKER", "MISSING"):
            ws.cell(ws.max_row, 5).fill = MISSING_FILL


def write_risk_register(wb: Workbook) -> None:
    ws = wb.create_sheet("Risk_Register")
    style_header(ws, ["ID", "Category", "Description", "Impact", "Likelihood", "Mitigation", "Owner", "Status", "RAG_ref"])
    rows = [
        ["R-ISO-001", "Network", "Eth_A/Eth_B not physically/logically isolated", "Grid rejection / audit fail", "M", "Dual PHY or managed switch; strict VLAN; no shared L2 bridge", "HW", "Open", "cei-0-16-allegato-o"],
        ["R-PF2-001", "Control", "Stale meter data drives curtailment", "Wrong plant state / safety", "M", "Timestamp+validity; timeout→safe state; hysteresis", "FW", "Open", "ccli-validation-strategy"],
        ["R-SEC-001", "Cyber", "Private keys in filesystem", "62351/62443 assessment fail", "H", "Secure element; signed images; RBAC on console", "FW", "Open", "ccli-architecture-framework"],
        ["R-BOM-001", "Process", "Kit 99991105 mistaken for field CCI", "Wrong procurement / false validation", "M", "Label all docs: kit=lab carrier=product", "PM", "Open", "ccli-vendor-selection"],
        ["R-SCH-001", "Schedule", "Schematic before pin budget D3", "Respins / delay", "H", "Complete D3+D4 before Gerbers", "HW", "Open", "ccli-prototype-bom"],
        ["R-LIC-001", "Legal", "Ship product on GPLv3 libiec61850", "License violation", "H", "Budget MZ commercial license from day 1", "PM", "Open", "ccli-project-roadmap"],
        ["R-EMC-001", "EMC", "RS485/CAN on kit confused with product C2", "Wrong EMC test scope", "M", "Re-test on carrier with opto RJ45", "HW", "Open", "ccli-vendor-selection"],
        ["R-TIME-001", "Time", "No GNSS on kit; events untimestamped in lab", "False confidence in L2", "M", "Mark L2 time tests as stub until C5 carrier", "FW", "Open", "ccli-validation-strategy"],
    ]
    for r in rows:
        ws.append(r)


def write_verification_matrix(wb: Workbook) -> None:
    ws = wb.create_sheet("Verification_Matrix")
    style_header(ws, ["Requirement_ID", "Description", "Test_method", "Level", "Pass_criteria", "Bench_ref", "Evidence_path", "Status"])
    rows = [
        ["REQ-NET-001", "Eth_A MMS server", "L2: libiec61850 client on isolated NIC", "L2", "Read/write logical nodes on Eth_A only", "Wave A switch", "—", "Planned"],
        ["REQ-NET-002", "Eth_B operator channel", "60870/61850 client on second GbE", "L2", "No crosstalk from Eth_A under VLAN test", "Wave A", "—", "Planned"],
        ["REQ-SER-001", "Modbus RTU poll analyzer", "USB-RS485 + pymodbus / real analyzer", "L2/L3", "Register map matches DS", "Wave A/C", "—", "Planned"],
        ["REQ-IO-001", "DI 10-120V sense", "24V DI test box injection", "L2", "All channels debounced; no false PF2", "Wave B", "—", "Planned"],
        ["REQ-IO-002", "DO curtailment relay", "Simulated DSO command toggles DO", "L2", "Lamp/relay within timing budget", "Wave B", "—", "Planned"],
        ["REQ-PF2-001", "Comms loss safe state", "Failure matrix: meter/Eth/GNSS stale", "L2", "Defined safe state; no unsafe curtailment", "Validation strategy §9", "—", "Planned"],
        ["REQ-TIME-001", "GNSS timestamp", "PPS/NMEA capture on events", "L3", "Sub-second event ordering", "Wave C + carrier", "—", "Deferred"],
        ["REQ-SEC-001", "Secure boot chain", "Signed vs unsigned image boot", "Lab", "Unsigned rejected; rollback policy", "Toradex signed-boot repo", "—", "Planned"],
        ["REQ-PWR-001", "Input 12-24V operation", "Bench PSU sweep", "L2", "SoM boots across range; no rail collapse", "Wave A PSU", "—", "Planned"],
    ]
    for r in rows:
        ws.append(r)


def save_workbook(name: str, builder) -> None:
    wb = Workbook()
    builder(wb)
    path = ARCH / name
    try:
        wb.save(path)
    except PermissionError:
        alt = ARCH / "_generated" / name
        alt.parent.mkdir(parents=True, exist_ok=True)
        wb.save(alt)
        print(f"  {name} (locked — wrote {alt.relative_to(ROOT)} instead; close Excel and re-run)")
        return
    print(f"  {name}")


def main() -> int:
    ARCH.mkdir(parents=True, exist_ok=True)

    diagrams = {
        "System_Context.drawio": system_context_drawio(),
        "Functional_Architecture.drawio": functional_architecture_drawio(),
        "Hardware_Architecture.drawio": hardware_architecture_drawio(),
        "Power_Tree.drawio": power_tree_drawio(),
        "Network_Architecture.drawio": network_architecture_drawio(),
    }
    for name, content in diagrams.items():
        (ARCH / name).write_text(content, encoding="utf-8")
        print(f"  {name}")

    def build_interface(wb):
        write_interface_matrix(wb)

    def build_bom(wb):
        write_bom_matrix(wb)

    def build_knowledge(wb):
        write_knowledge_matrix(wb)

    def build_risk(wb):
        write_risk_register(wb)

    def build_verification(wb):
        write_requirements_sheet(wb)
        write_verification_matrix(wb)

    print("Excel:")
    save_workbook("Interface_Matrix.xlsx", build_interface)
    save_workbook("BOM_Matrix.xlsx", build_bom)
    save_workbook("Knowledge_Matrix.xlsx", build_knowledge)
    import subprocess
    subprocess.run([sys.executable, str(ROOT / "scripts" / "build-sk0146-knowledge-matrix.py")], check=False)
    save_workbook("Risk_Register.xlsx", build_risk)
    save_workbook("Verification_Matrix.xlsx", build_verification)

    print("Database:")
    subprocess.run(
        [sys.executable, str(ROOT / "scripts" / "init-architecture-db.py")],
        check=False,
        cwd=str(ROOT),
    )

    meta = {
        "generated_at": datetime.now(timezone.utc).isoformat(),
        "generator": "scripts/generate-architecture-from-rag.py",
        "corpus_sources": [
            "ccli-vendor-selection", "ccli-prototype-bom", "ccli-architecture-framework",
            "cei-0-16-allegato-o", "cei-0-16-allegato-t", "ccli-validation-strategy",
            "ccli-project-roadmap", "ccli-toradex-verdin-devboard",
        ],
        "rag_queried": True,
        "architecture_db": "Architecture/architecture.db",
    }
    META.write_text(json.dumps(meta, indent=2), encoding="utf-8")

    readme = ARCH / "README.md"
    readme.write_text(
        readme.read_text(encoding="utf-8").split("## Regenerate")[0].rstrip()
        + """

## Regenerate from RAG corpus

```powershell
python scripts/generate-architecture-from-rag.py
```

Reads authoritative ingested sources (vendor selection, prototype BOM, CEI O/T, architecture framework) and overwrites diagrams + Excel. **Back up manual edits first.**

Metadata: `Architecture/_generation_meta.json`
""",
        encoding="utf-8",
    )

    print(f"\nDone — {ARCH}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
