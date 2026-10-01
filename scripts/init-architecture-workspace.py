#!/usr/bin/env python3
"""Create Architecture/ engineering workspace (draw.io + Excel matrices)."""

from __future__ import annotations

from pathlib import Path

try:
    from openpyxl import Workbook
    from openpyxl.styles import Font
except ImportError as e:
    raise SystemExit("openpyxl required: pip install openpyxl") from e

ROOT = Path(__file__).resolve().parents[1]
ARCH = ROOT / "Architecture"

DRAWIO_DIAGRAMS = [
    ("System_Context.drawio", "System Context — CEI 0-16 PF2 CCI"),
    ("Functional_Architecture.drawio", "Functional Architecture — C1–C10"),
    ("Hardware_Architecture.drawio", "Hardware Architecture — SoM + Carrier"),
    ("Power_Tree.drawio", "Power Tree — 12–24 V input rails"),
    ("Network_Architecture.drawio", "Network Architecture — Eth_A / Eth_B / Plant"),
]


def drawio_stub(title: str) -> str:
    return f"""<mxfile host="app.diagrams.net" modified="2026-07-26T00:00:00.000Z" agent="CCLI" version="22.1.0" type="device">
  <diagram name="Page-1" id="ccli-arch">
    <mxGraphModel dx="1200" dy="800" grid="1" gridSize="10" guides="1" tooltips="1" connect="1" arrows="1" fold="1" page="1" pageScale="1" pageWidth="1169" pageHeight="827" math="0" shadow="0">
      <root>
        <mxCell id="0"/>
        <mxCell id="1" parent="0"/>
        <mxCell id="title" value="{title}" style="text;html=1;strokeColor=none;fillColor=none;align=center;verticalAlign=middle;fontSize=20;fontStyle=1" vertex="1" parent="1">
          <mxGeometry x="120" y="30" width="920" height="40" as="geometry"/>
        </mxCell>
        <mxCell id="note" value="Edit in draw.io / diagrams.net. Link to knowledge-base/08-engineering/CCI_Architecture_Framework.md" style="text;html=1;strokeColor=none;fillColor=#FFF4E6;align=left;verticalAlign=top;fontSize=11;spacing=8;" vertex="1" parent="1">
          <mxGeometry x="120" y="90" width="920" height="50" as="geometry"/>
        </mxCell>
        <mxCell id="box1" value="Placeholder — replace with blocks" style="rounded=1;whiteSpace=wrap;html=1;fillColor=#E8F4FC;strokeColor=#1A365D;" vertex="1" parent="1">
          <mxGeometry x="120" y="180" width="280" height="120" as="geometry"/>
        </mxCell>
        <mxCell id="box2" value="Placeholder — replace with blocks" style="rounded=1;whiteSpace=wrap;html=1;fillColor=#E8F4FC;strokeColor=#1A365D;" vertex="1" parent="1">
          <mxGeometry x="440" y="180" width="280" height="120" as="geometry"/>
        </mxCell>
        <mxCell id="box3" value="Placeholder — replace with blocks" style="rounded=1;whiteSpace=wrap;html=1;fillColor=#E8F4FC;strokeColor=#1A365D;" vertex="1" parent="1">
          <mxGeometry x="760" y="180" width="280" height="120" as="geometry"/>
        </mxCell>
      </root>
    </mxGraphModel>
  </diagram>
</mxfile>
"""


def header_row(ws, headers: list[str]) -> None:
    ws.append(headers)
    for cell in ws[1]:
        cell.font = Font(bold=True)


def write_interface_matrix(wb: Workbook) -> None:
    ws = wb.active
    ws.title = "Interface_Matrix"
    header_row(
        ws,
        ["Interface", "Source", "Destination", "Protocol", "Isolation", "Block", "Notes", "Status"],
    )
    rows = [
        ["Eth_A", "CCI", "DSO", "IEC 61850 MMS/GOOSE", "PHY/VLAN vs Eth_B", "C1", "Annex O", "TBD"],
        ["Eth_B", "CCI", "Operatore Abilitato", "IEC 61850 / 60870-104", "Separate from Eth_A", "C1", "Annex T", "TBD"],
        ["Plant-1", "CCI", "Plant LAN", "Modbus TCP", "From DSO path", "C1", "Plant port", "TBD"],
        ["Plant-2", "CCI", "Plant LAN", "Modbus TCP", "From DSO path", "C1", "Plant port", "TBD"],
        ["RS485-A", "CCI", "Energy analyzer", "Modbus RTU", "Opto-isolated", "C2", "Carrier only", "MISSING"],
        ["RS485-B", "CCI", "Spare / 60870-101", "Modbus RTU / 101", "Opto-isolated", "C2", "Carrier only", "MISSING"],
        ["DI-1..n", "Field 10-120 V", "CCI GPIO", "Digital in", "Opto", "C4", "Status inputs", "MISSING"],
        ["DO-1..n", "CCI relay", "Curtailment contact", "Dry contact", "—", "C4", "PF2 critical", "MISSING"],
        ["GNSS", "Antenna", "SoM", "NMEA / PPS", "—", "C5", "Time sync", "MISSING"],
        ["LTE", "Modem", "Carrier network", "PPP / QMI", "Logical", "C7", "Optional rev A", "TBD"],
        ["USB", "Service PC", "SoM", "USB", "—", "C8", "Commissioning", "Kit HAVE"],
    ]
    for r in rows:
        ws.append(r)


def write_bom_matrix(wb: Workbook) -> None:
    ws = wb.create_sheet("BOM_Matrix")
    header_row(
        ws,
        ["Block", "Function", "Candidate", "PN", "Qty", "Est_EUR", "Doc_status", "Architecture_Freeze"],
    )
    rows = [
        ["C1", "SoM compute", "Verdin iMX8M Plus IT", "0063", "1", "150-350", "HAVE", "Locked lead"],
        ["C1", "Eval kit (lab)", "Verdin Development Board", "99991105", "1", "~300", "HAVE", "Lab only"],
        ["C1", "Ethernet PHY/magnetics", "TBD", "—", "4", "15-25", "MISSING", "D3 pin budget"],
        ["C2", "RS485 transceiver x2", "Isolated", "—", "2", "10-15", "MISSING", "Carrier"],
        ["C4", "DI opto + DO relay", "TBD", "—", "—", "15-25", "MISSING", "Carrier"],
        ["C5", "GNSS module", "u-blox-class", "—", "1", "25-40", "MISSING", "Carrier"],
        ["C6", "Secure element", "ATECC608B / STSAFE", "—", "1", "2-5", "TBD", "—"],
        ["C10", "Wide-range PSU", "Isolated DC-DC", "—", "1", "15-25", "MISSING", "Carrier"],
    ]
    for r in rows:
        ws.append(r)


def write_knowledge_matrix(wb: Workbook) -> None:
    ws = wb.create_sheet("Knowledge_Matrix")
    header_row(ws, ["Area", "Current", "Required", "Gap", "Doc_gate", "RAG_source_id", "Priority"])
    rows = [
        ["SoC freeze MT798X", "FROZEN", "TG-524 lab silicon", "HAVE", "D2", "ccli-soc-freeze", "P0"],
        ["OpenWrt SDK", "—", "TesPro cross-compile", "MISSING", "D6", "ccli-soc-freeze", "P0"],
        ["Lab port map", "PARTIAL", "Eth_A/B/plant labels", "PARTIAL", "D4", "ccli-tg500-lab-platform", "P0"],
        ["Modbus analyzer map", "—", "Register map PDF", "MISSING", "D8", "ccli-modbus-analyzer-map", "P1"],
        ["61850 commercial license", "GPLv3 prototype", "MZ commercial quote", "GAP", "—", "ccli-project-roadmap", "P1"],
    ]
    for r in rows:
        ws.append(r)


def write_risk_register(wb: Workbook) -> None:
    ws = wb.create_sheet("Risk_Register")
    header_row(ws, ["ID", "Description", "Impact", "Likelihood", "Mitigation", "Owner", "Status"])
    rows = [
        ["R-ISO-001", "Eth_A/Eth_B not physically isolated", "Grid rejection / audit fail", "M", "Dual PHY or strict VLAN; no shared bridge", "HW", "Open"],
        ["R-PF2-001", "Stale meter data drives curtailment", "Wrong plant state", "M", "Timestamp + validity; timeout → safe state", "FW", "Open"],
        ["R-SEC-001", "Keys in filesystem", "62351/62443 fail", "H", "Secure element + signed images", "FW", "Open"],
        ["R-BOM-001", "Kit mistaken for field CCI", "Wrong procurement", "M", "Label kit=lab, carrier=product", "PM", "Open"],
        ["R-SCH-001", "No pin budget before layout", "Respins / delay", "H", "Complete D3 before Gerbers", "HW", "Open"],
    ]
    for r in rows:
        ws.append(r)


def write_verification_matrix(wb: Workbook) -> None:
    ws = wb.create_sheet("Verification_Matrix")
    header_row(
        ws,
        ["Requirement_ID", "Description", "Test", "Level", "Pass_criteria", "Evidence", "Status"],
    )
    rows = [
        ["REQ-NET-001", "Eth_A MMS server", "L2 bench client read/write", "L2", "Isolated port responds", "—", "Planned"],
        ["REQ-SER-001", "Modbus RTU poll", "RS485 adapter + pymodbus", "L2", "Register map matches DS", "—", "Planned"],
        ["REQ-PF2-001", "DSO curtailment DO", "Simulated DSO command", "L2", "Relay toggles in time", "—", "Planned"],
        ["REQ-TIME-001", "GNSS timestamp", "PPS/NMEA capture", "L3", "Events timestamped", "—", "Deferred"],
        ["REQ-SEC-001", "Secure boot", "Signed image only boots", "Lab", "Unsigned rejected", "—", "Planned"],
    ]
    for r in rows:
        ws.append(r)


def write_xlsx(name: str, writer) -> None:
    wb = Workbook()
    writer(wb)
    path = ARCH / name
    wb.save(path)
    print(f"  {path.name}")


def main() -> int:
    ARCH.mkdir(parents=True, exist_ok=True)

    readme = ARCH / "README.md"
    readme.write_text(
        """# Architecture workspace

Engineering working folder for PF2 / CEI 0-16 CCI design.

**Methodology:** `knowledge-base/08-engineering/CCI_Architecture_Framework.md`  
**Cursor rule:** `.cursor/rules/system-architect.mdc`

## Structure

| File | Purpose |
|------|---------|
| `System_Context.drawio` | DSO, Operator, plant, CCI boundaries |
| `Functional_Architecture.drawio` | C1–C10 functional blocks |
| `Hardware_Architecture.drawio` | TG-524 lab platform + peripherals |
| `Power_Tree.drawio` | 12–24 V input, rails, sequencing |
| `Network_Architecture.drawio` | Eth_A, Eth_B, plant VLANs |
| `Interface_Matrix.xlsx` | Source → destination protocols |
| `BOM_Matrix.xlsx` | Block → candidate parts |
| `Knowledge_Matrix.xlsx` | HAVE / MISSING / doc gate |
| `Risk_Register.xlsx` | Architecture risks |
| `Verification_Matrix.xlsx` | REQ → test → pass criteria |

## Tools

- **Diagrams:** open `.drawio` in [diagrams.net](https://app.diagrams.net) or VS Code Draw.io extension
- **Matrices:** edit `.xlsx` in Excel; re-export or sync rows to `knowledge-base/` when stable

## Regenerate stubs

```powershell
python scripts/init-architecture-workspace.py
```

This overwrites draw.io stubs and Excel seed data (back up edits first).
""",
        encoding="utf-8",
    )

    print("Draw.io diagrams:")
    for filename, title in DRAWIO_DIAGRAMS:
        path = ARCH / filename
        path.write_text(drawio_stub(title), encoding="utf-8")
        print(f"  {filename}")

    print("Excel matrices:")
    write_xlsx("Interface_Matrix.xlsx", write_interface_matrix)
    write_xlsx("BOM_Matrix.xlsx", write_bom_matrix)
    write_xlsx("Knowledge_Matrix.xlsx", write_knowledge_matrix)
    write_xlsx("Risk_Register.xlsx", write_risk_register)
    write_xlsx("Verification_Matrix.xlsx", write_verification_matrix)

    print(f"\nDone: {ARCH}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
