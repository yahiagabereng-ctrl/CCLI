#!/usr/bin/env python3
"""Build Architecture/Knowledge_Matrix.xlsx — SK0146 test bench knowledge leaks vs CCLI PF2.

Uses RAG API (when reachable) to probe ingestion; reads Test Bench + SK0146 structured docs from disk.
Reference product SK0146-EVC is ATEX zone level 0 (non-hazardous / safe-area lab bench).
"""

from __future__ import annotations

import json
import urllib.error
import urllib.request
from datetime import datetime, timezone
from pathlib import Path

from openpyxl import Workbook
from openpyxl.styles import Alignment, Font, PatternFill

ROOT = Path(__file__).resolve().parents[1]
import sys

sys.path.insert(0, str(ROOT / "scripts"))
from knowledge_risks_corpus import (  # noqa: E402
    DOCUMENT_MAPPING,
    HEAT_MAP,
    KNOWLEDGE_MATRIX_ROWS,
    KNOWLEDGE_RISKS,
    LEARNING_PLAN,
    REUSE_MATRIX,
)
ARCH = ROOT / "Architecture"
TEST_BENCH = Path(r"C:\Yahia\projects\Test Bench")
SK0146_RAG = Path(r"C:\Yahia\projects\Telematry System\documents\rag-structured\sk0146")
SK0146_BLOCKS = Path(
    r"C:\Yahia\projects\Telematry System\documents\projects\sk0146\reference-extracted-blocks.json"
)

RAG_BASE = "http://localhost:8001"
TENANT = "telematry"
BOT = "default"

HEADER_FILL = PatternFill("solid", fgColor="1A365D")
HEADER_FONT = Font(bold=True, color="FFFFFF")
LEAK_FILL = PatternFill("solid", fgColor="FEE2E2")
PARTIAL_FILL = PatternFill("solid", fgColor="FEF3C7")
HAVE_FILL = PatternFill("solid", fgColor="DCFCE7")
INGEST_FILL = PatternFill("solid", fgColor="E0E7FF")


def style_header(ws, headers: list[str]) -> None:
    ws.append(headers)
    for cell in ws[1]:
        cell.fill = HEADER_FILL
        cell.font = HEADER_FONT
        cell.alignment = Alignment(wrap_text=True, vertical="top")


def rag_probe(query: str, source_ids: list[str]) -> dict:
    body = json.dumps(
        {
            "tenant_id": TENANT,
            "bot_id": BOT,
            "query": query,
            "source_ids": source_ids,
            "top_k": 3,
        }
    ).encode()
    req = urllib.request.Request(
        f"{RAG_BASE}/v1/query",
        data=body,
        headers={"Content-Type": "application/json"},
        method="POST",
    )
    try:
        with urllib.request.urlopen(req, timeout=30) as resp:
            data = json.loads(resp.read().decode())
        retrieved = int(data.get("traces", {}).get("retrieved") or 0)
        return {"ok": True, "retrieved": retrieved, "ingested": retrieved > 0}
    except (urllib.error.URLError, TimeoutError, json.JSONDecodeError) as exc:
        return {"ok": False, "retrieved": 0, "ingested": False, "error": str(exc)}


def rag_status_label(probe: dict) -> str:
    if not probe.get("ok"):
        return "RAG_OFFLINE"
    return "INGESTED" if probe.get("ingested") else "NOT_IN_RAG"


def fill_row(ws, status: str) -> None:
    mapping = {
        "LEAK": LEAK_FILL,
        "MISSING": LEAK_FILL,
        "NOT_IN_RAG": INGEST_FILL,
        "PARTIAL": PARTIAL_FILL,
        "HAVE": HAVE_FILL,
    }
    fill = mapping.get(status)
    if fill:
        for cell in ws[ws.max_row]:
            cell.fill = fill


def sk0146_knowledge_rows(rag_probes: dict[str, dict]) -> list[list]:
    """Rows: ID, Domain, SK0146_bench_need, CCLI_PF2_need, Have_now, Required_knowledge,
    source_id_or_path, RAG_status, Leak_type, ATEX_zone0_note, Action, Priority"""

    def rag(sid: str) -> str:
        return rag_status_label(rag_probes.get(sid, {"ingested": False}))

    rows = [
        [
            "KL-001",
            "Corpus / RAG",
            "Query schematic TP nets (RS485, DIN, DOUT) during FCT debug",
            "Reuse SK0146 FCT patterns for TG-524 RS485/DI/DO lab bring-up",
            "PARTIAL — markdown on disk only",
            "14 SchDoc sheets + schematic-index (ADM3483, opto DIN/OUT, GSM, sensors)",
            "sk0146-schematic-index … sk0146-blk_ref_* (15 source_ids)",
            rag("sk0146-blk_ref_a_rs485"),
            "RAG_NOT_INGESTED",
            "Product zone 0 — bench is safe-area lab; no Ex-rated fixture required",
            "Run Telematry: scripts/ingest-sk0146-schematics.ps1",
            "P0",
        ],
        [
            "KL-002",
            "RS485 / Modbus",
            "Stage 04: TP50 V486 via VS12; AT+RS485_PWR; U17 ADM3483",
            "Wave A: USB-RS485 or TG-524 RS485; product C2 needs full opto RJ45 (field TBD)",
            "HAVE — Test Bench code + FIXTURE_TEST_PLAN §4.1",
            "ADM3483 DS; termination 47Ω; TVS DZ13–16; M10 field connector",
            "Test Bench/devices/FIXTURE_TEST_PLAN.md; sk0146-blk_ref_a_rs485.md",
            rag("sk0146-blk_ref_a_rs485"),
            "CROSS_PROJECT",
            "Safe-area RS485 lab cable — not Ex 'd' wiring",
            "Ingest SK0146 RS485 sheet; map to ccli-prototype-bom C2 MPN selection",
            "P1",
        ],
        [
            "KL-003",
            "Field DI",
            "Stages 08/15: TP18/21/11/14 active-LOW; short TP→block GND; AT+DIN?",
            "Wave B: DI 10–120 Vdc ×5 via opto conditioner (not 3.3 V contact sim)",
            "HAVE — SK0146 bench methodology differs from CCI",
            "DZ3–8 zener clamps; M4/M9 terminals; 33 kΩ pull network",
            "sk0146-blk_ref_a_digital_in; ccli-validation-strategy",
            rag("ccli-validation-strategy"),
            "METHODOLOGY_GAP",
            "SK0146 DIN is meter contact sim; CCI is plant 24–120 V — do not reuse limits",
            "Document crosswalk: SK0146 contact test ≠ CCI DI box; keep separate pass criteria",
            "P1",
        ],
        [
            "KL-004",
            "Field DO",
            "Stages 08/16: TP59/TP81; fixture 10 kΩ pull-up; AT+GPO?",
            "Wave B: DO relay ×3 curtailment dry contacts",
            "HAVE — Test Bench + schematic",
            "OP1/OP2 opto; M7 terminal; GPO command map",
            "sk0146-blk_ref_a_digital_out; Test Bench/test_program/phases_sk0146.py",
            rag("sk0146-blk_ref_a_digital_out"),
            "CROSS_PROJECT",
            "DO bench is 3.3 V sense — CCI DO is mains-rated relay",
            "Extract DO timing from SK0146; apply to TG-524 DI/DO or Pi GPIO lab stub",
            "P2",
        ],
        [
            "KL-005",
            "Fixture mapping",
            "VS01–VS16 ADC channel assignment for every TP",
            "CCI mocking bench has no VS matrix — different DUT",
            "PARTIAL — 38 pogos in sk0146_evc.json; fixture_map.json",
            "Complete VS assignment for TP57, TP4; keypad flying leads",
            "Test Bench/devices/fixture_map.json; FIXTURE_TEST_PLAN.md",
            "LOCAL_ONLY",
            "FIXTURE_TBD",
            "Zone 0 product — bench ESD/grounding per normal electronics lab",
            "Close TBD rows: TP57→VS?, TP4→VS?, KEY1–6 flying leads on drawing",
            "P0",
        ],
        [
            "KL-006",
            "Keypad / HMI",
            "Stage 13: operator press KEY1–6; ADC 3.0–3.45 V released",
            "CCI has no keypad — operator via Eth_B / local HMI",
            "PARTIAL — phases exist; fixture TBD",
            "TP111–TP141 net→MCU map from KEYBOARD sheet",
            "sk0146-blk_ref_b_keyboard.md; Test Bench §4.2",
            rag("sk0146-blk_ref_b_keyboard"),
            "FIXTURE_TBD",
            "N/A",
            "Assign fixture pogo/flying lead per KEYBOARD schematic; ingest sheet to RAG",
            "P1",
        ],
        [
            "KL-007",
            "Pressure / sensors",
            "Stage 16: M2 1 kΩ fixture; AT+DEBUG pressure Pa; golden limits",
            "CCI: Modbus analyzer registers — different sensor stack",
            "PARTIAL — test runs; limits TBD",
            "M2 AIN0–4 front-end; golden unit capture",
            "sk0146-blk_ref_a_sensors.md; SK0146_EVC_Test_Flow.md",
            rag("sk0146-blk_ref_a_sensors"),
            "CALIBRATION_GAP",
            "N/A",
            "Record golden-unit Pa limits in sk0146_evc.json; ingest SENSORS sheet",
            "P1",
        ],
        [
            "KL-008",
            "GSM / RF",
            "Stage 06: TP43 VGSM, TP49 gate, AT+GSM_*",
            "CCI C7 LTE optional — different modem stack",
            "HAVE — bench UART + ADC gates",
            "GSM power sequencing; antenna coupler or anechoic policy",
            "sk0146-blk_ref_a_gsm.md",
            rag("sk0146-blk_ref_a_gsm"),
            "CROSS_PROJECT",
            "Lab RF — not Ex i equipment; follow local RF exposure limits",
            "Ingest GSM sheet; CCI LTE bench doc separate (Quectel-class)",
            "P2",
        ],
        [
            "KL-009",
            "DLMS / metrology",
            "Stage 09: AT+PROTOCOL DLMS identity",
            "CCI: IEC 61850 MMS/GOOSE — different protocol stack",
            "HAVE — UART stub responses in sk0146_evc.json",
            "Full DLMS conformance scope vs identity-only FCT",
            "Test Bench/devices/sk0146_evc.json",
            "LOCAL_ONLY",
            "PROTOCOL_MISMATCH",
            "Metrology Ex marking on product — FCT in safe area does not replace notified-body review",
            "Do not map DLMS stage to 61850; keep REQ-NET-* separate in Verification_Matrix",
            "P1",
        ],
        [
            "KL-010",
            "IR / optical",
            "Stage 14: TP4 VDD_IR; AT+IR_ON/OFF + AT+IR_TEST loopback",
            "CCI — no IR port in PF2 minimum",
            "PARTIAL — VS channel TBD on bench",
            "IR_62056 block; display board B netlist",
            "sk0146-blk_ref_b_ir.md; FIXTURE_TEST_PLAN §4.1",
            rag("sk0146-blk_ref_b_ir"),
            "FIXTURE_TBD",
            "N/A",
            "Assign VS for TP4; ingest IR sheet",
            "P2",
        ],
        [
            "KL-011",
            "Power / metrology",
            "Stages 01–02, 10: TP49 VMAIN, rail scan VS01–VS16",
            "Wave A: 12–36 V PSU on TG-524 lab input",
            "HAVE — INA228 + ADS1256 HAL",
            "POWER + CPU sheets; boot vs active current limits",
            "sk0146-blk_ref_a_power.md; sk0146-blk_ref_a_cpu.md",
            rag("sk0146-blk_ref_a_power"),
            "CROSS_PROJECT",
            "SK0146 battery/metrology rails ≠ CCI 12–24 V DIN input",
            "Ingest POWER/CPU; CCI REQ-PWR-001 uses separate limits",
            "P2",
        ],
        [
            "KL-012",
            "Valve / EV driver",
            "Stage 05–06E: TP77 HV_MON, TP57 EN_EV, LM3478",
            "CCI — no valve driver in scope",
            "PARTIAL — TP57 VS TBD",
            "DRIVER EV sheet; EN_EV command timing",
            "sk0146-blk_ref_a_driver_ev.md",
            rag("sk0146-blk_ref_a_driver_ev"),
            "FIXTURE_TBD",
            "High-voltage valve monitor — use qualified HV probe policy in lab",
            "Assign VS for TP57; document HV safety in bench SOP",
            "P1",
        ],
        [
            "KL-013",
            "ADCT stimulus",
            "Stages 18–19: MCP4725 @ 2.0 V on TP34/160/162",
            "CCI — analog inputs via Modbus analyzer path",
            "HAVE — MCP4725 in fixture plan",
            "MCP4725 I2C 0x60; M5/M6 terminal mapping",
            "Test Bench/FIXTURE_TEST_PLAN §4.7",
            "LOCAL_ONLY",
            "NONE",
            "N/A",
            "No CCI action — SK0146-only knowledge",
            "—",
        ],
        [
            "KL-014",
            "CCI validation",
            "SK0146 FCT JSON records + pytest phases as reference HIL pattern",
            "L2 PF2 bench: failure matrix, safe state, Wave A/B BOM",
            "HAVE — ccli-validation-strategy INGESTED",
            "Mocking bench Waves A–D; DI box 24 V first",
            "ccli-validation-strategy; ccli-mocking-bench-bom",
            rag("ccli-validation-strategy"),
            "NONE",
            "ATEX QMS applies to CCI product design; lab bench ATEX level 0 = non-hazardous area",
            "Link Verification_Matrix REQ-* to mocking bench rows",
            "P1",
        ],
        [
            "KL-015",
            "ATEX / regulatory",
            "reference_zone_level: 0 in SK0146 extract — non-Ex product reference",
            "CCI cabinet classification + NB path",
            "PARTIAL — ccli-cci-module-regs-class INGESTED",
            "IEC 60079-0 safe area vs product Ex marking; org procedure 7.x",
            "reference-extracted-blocks.json; ccli-cci-module-regs-class",
            rag("ccli-cci-module-regs-class"),
            "SCOPE_CLARIFICATION",
            "Bench ATEX lvl 0 = ordinary lab; does not certify DUT; SK0146 zone 0 ≠ CCI final zone",
            "Add bench SOP: safe-area only; separate CCI Ex design dossier",
            "P1",
        ],
        [
            "KL-016",
            "Datasheets",
            "ADM3483, LM3478, ADS1256, MCP23017, INA228 for bench debug",
            "TG-524 integrated RS485; field transceiver MPNs deferred",
            "MISSING in RAG",
            "Vendor DS PDFs linked from BOM rows",
            "ccli-prototype-bom; Altium BOM in sk0146 sheets",
            rag("ccli-prototype-bom"),
            "DATASHEET_GAP",
            "Use non-Ex lab instruments only in zone 0 bench",
            "Ingest key IC datasheets or link DOWNLOADS.md entries",
            "P2",
        ],
        [
            "KL-017",
            "Test records",
            "reports/*.json schema; operator GUI; serial after PASS",
            "CCI evidence pack for REQ-* verification",
            "HAVE — Test Bench only",
            "JSON schema + LaTeX report pipeline",
            "Test Bench/records.py; latex_report.py",
            "LOCAL_ONLY",
            "CROSS_PROJECT",
            "N/A",
            "Mirror evidence-path pattern in CCLI Verification_Matrix Evidence_path column",
            "P2",
        ],
        [
            "KL-018",
            "Lab port map",
            "SK0146 STM32L152 net→TP map (fixture-centric)",
            "Eth_A/B/plant labels on TG-524 WAN/LAN",
            "PARTIAL — CCI_TG500_Lab_Platform.md",
            "Confirm port roles on hardware receipt",
            "ccli-tg500-lab-platform",
            rag("ccli-tg500-lab-platform"),
            "PARTIAL",
            "N/A",
            "Label LAN1/LAN2/WAN on receipt; update lab platform doc",
            "P0",
        ],
    ]
    return rows


def ccli_gate_rows() -> list[list]:
    return [
        ["D0", "Architecture block diagram", "HAVE mermaid/Architecture/", "Frozen functional view", "Update draw.io", "P1", "ccli-architecture-framework", "Review generated diagrams"],
        ["D1", "Project roadmap", "HAVE/INGESTED", "Budget envelope", "None", "—", "ccli-project-roadmap", "—"],
        ["D2", "SoC freeze MT798X", "FROZEN", "Single lab silicon", "None", "—", "ccli-soc-freeze", "—"],
        ["D3", "TG-524 lab platform", "FROZEN", "OpenWrt lab DUT", "SDK missing", "P0", "ccli-tg500-lab-platform", "Request OpenWrt SDK"],
        ["D4", "Lab port map", "PARTIAL", "Eth_A/B/plant labels", "Confirm on receipt", "P0", "ccli-tg500-lab-platform", "Label WAN/LAN ports"],
        ["D5", "TG-500 datasheet + response", "HAVE/INGESTED", "Hardware spec", "None", "—", "ccli-tespro-tg500-ds", "—"],
        ["D6", "OpenWrt SDK (TesPro)", "MISSING", "Cross-compile toolchain", "BLOCKER", "P0", "ccli-soc-freeze", "Request with TG-524 PO"],
        ["D8", "Modbus analyzer register map", "MISSING", "Real analyzer model", "Software block", "P1", "ccli-modbus-analyzer-map", "Pick analyzer L3"],
        ["—", "61850 commercial license", "GPLv3 prototype", "MZ commercial ship license", "Budget", "P1", "ccli-project-roadmap", "Quote before product"],
        ["—", "62443/61850 lab scope", "ATEX NB reuse", "Accredited test quote", "Cert track", "P2", "ccli-cci-module-regs-class", "Early lab engagement"],
        ["—", "SK0146 schematic RAG", "15 MD files on disk", "Ingested + queryable", "LEAK P0", "P0", "sk0146-schematic-index", "ingest-sk0146-schematics.ps1"],
        ["—", "SK0146 FCT bench crosswalk", "Test Bench repo local", "Linked in Architecture matrix", "PARTIAL", "P1", "Test Bench/README.md", "This matrix"],
        ["—", "Field custom carrier", "DEFERRED", "Product design TBD", "Not Phase 1", "—", "_archive/som-study/", "Historical SoM study only"],
    ]


def bench_crosswalk_rows() -> list[list]:
    return [
        ["RS485", "TP50 V486 + ADM3483", "TG-524 2× RS485 or USB adapter", "Similar lab test; confirm isolation", "KL-002"],
        ["DI", "3.3 V contact short to GND", "TG-524 2 DI + Pi Wave B 10–120 V", "Different physics — separate fixtures", "KL-003"],
        ["DO", "3.3 V opto sense + pull-up", "TG-524 2 DO + Wave B relay stub", "SK0146 proves GPO pattern only", "KL-004"],
        ["Power", "3.76 V PPK2 / pogos", "TG-524 12–36 V input", "Different rails and limits", "KL-011"],
        ["Comms", "DLMS UART identity", "61850 MMS + 60870-104", "No shared protocol tests", "KL-009"],
        ["Sleep", "PPK2 µA on P01", "Not in CCI PF2 minimum", "SK0146-only", "KL-013"],
        ["Evidence", "JSON + LaTeX per serial", "Verification_Matrix Evidence_path", "Reuse schema idea", "KL-017"],
        ["ATEX", "Product zone level 0 ref", "CCI Ex / safe-area cabinet TBD", "Bench = safe area lab", "KL-015"],
    ]


def write_workbook(path: Path) -> dict:
    sk0146_sources = [
        "sk0146-schematic-index",
        "sk0146-blk_ref_a_rs485",
        "sk0146-blk_ref_a_digital_in",
        "sk0146-blk_ref_a_digital_out",
        "sk0146-blk_ref_b_keyboard",
        "sk0146-blk_ref_a_sensors",
        "sk0146-blk_ref_a_gsm",
        "sk0146-blk_ref_b_ir",
        "sk0146-blk_ref_a_power",
        "sk0146-blk_ref_a_driver_ev",
    ]
    ccli_sources = [
        "ccli-validation-strategy",
        "ccli-prototype-bom",
        "ccli-cci-module-regs-class",
        "ccli-soc-freeze",
        "ccli-tg500-lab-platform",
    ]

    probes: dict[str, dict] = {}
    for sid in sk0146_sources + ccli_sources:
        probes[sid] = rag_probe(f"probe ingestion {sid}", [sid])

    wb = Workbook()

    # --- DRB primary sheets (BizChat Knowledge Matrix structure) ---
    ws_km = wb.active
    ws_km.title = "Knowledge_Matrix"
    style_header(
        ws_km,
        ["Knowledge_Area", "Current_Level", "Required_Level", "Risk", "Impact", "Priority", "Mitigation", "Reuse_ATEX"],
    )
    for row in KNOWLEDGE_MATRIX_ROWS:
        ws_km.append(row)
        if row[5] == "Critical":
            fill_row(ws_km, "LEAK")
        elif row[5] == "High":
            fill_row(ws_km, "PARTIAL")

    ws_kr = wb.create_sheet("Knowledge_Risks")
    style_header(
        ws_kr,
        ["KR_ID", "Knowledge_Area", "Current", "Required", "Risk", "Impact", "Priority", "Mitigation", "Target_Gate", "rag_source_id"],
    )
    for kr in KNOWLEDGE_RISKS:
        ws_kr.append(
            [
                kr["kr_id"],
                kr["area"],
                kr["current"],
                kr["required"],
                kr["risk"],
                kr["impact"],
                kr["priority"],
                kr["mitigation"],
                kr["target_gate"],
                kr.get("rag_source_id"),
            ]
        )
        if kr["priority"] == "Critical":
            fill_row(ws_kr, "LEAK")
        elif kr["priority"] == "High":
            fill_row(ws_kr, "PARTIAL")

    ws_lp = wb.create_sheet("Learning_Plan")
    style_header(ws_lp, ["Priority", "Topic", "Target_Before", "KR_Primary", "KR_Secondary"])
    for row in LEARNING_PLAN:
        ws_lp.append(row)
        if row[0] == "Critical":
            fill_row(ws_lp, "LEAK")

    ws_dm = wb.create_sheet("Document_Mapping")
    style_header(ws_dm, ["KR_ID", "Topic", "rag_source_id", "Document", "Corpus_Status", "Gate"])
    for row in DOCUMENT_MAPPING:
        ws_dm.append(row)
        if "MISSING" in str(row[4]) or "NOT_IN_RAG" in str(row[4]):
            fill_row(ws_dm, "LEAK")
        elif "PARTIAL" in str(row[4]) or "TBD" in str(row[4]):
            fill_row(ws_dm, "PARTIAL")
        elif "HAVE" in str(row[4]):
            fill_row(ws_dm, "HAVE")

    ws_hm = wb.create_sheet("Heat_Map")
    style_header(ws_hm, ["Band", "Topic", "Note"])
    for row in HEAT_MAP:
        ws_hm.append(row)
        if row[0] == "Critical":
            fill_row(ws_hm, "LEAK")
        elif row[0] == "High":
            fill_row(ws_hm, "PARTIAL")

    ws_ru = wb.create_sheet("Reuse_Matrix")
    style_header(ws_ru, ["Area", "Reuse_from_ATEX", "Caution"])
    for row in REUSE_MATRIX:
        ws_ru.append(row)

    # --- SK0146 / gate / crosswalk sheets ---
    ws = wb.create_sheet("SK0146_Knowledge_Leaks")
    headers = [
        "ID",
        "Domain",
        "SK0146_bench_need",
        "CCLI_PF2_need",
        "Have_now",
        "Required_knowledge",
        "source_id_or_path",
        "RAG_status",
        "Leak_type",
        "ATEX_zone0_note",
        "Action",
        "Priority",
    ]
    style_header(ws, headers)
    for row in sk0146_knowledge_rows(probes):
        ws.append(row)
        leak = row[8]
        if leak in ("RAG_NOT_INGESTED", "FIXTURE_TBD", "BLOCKER", "METHODOLOGY_GAP"):
            fill_row(ws, "LEAK")
        elif leak in ("PARTIAL", "CALIBRATION_GAP", "DATASHEET_GAP", "SCOPE_CLARIFICATION"):
            fill_row(ws, "PARTIAL")
        elif leak in ("NONE",):
            fill_row(ws, "HAVE")
        elif row[7] == "NOT_IN_RAG":
            fill_row(ws, "NOT_IN_RAG")

    for col in ws.columns:
        ws.column_dimensions[col[0].column_letter].width = 18
    ws.column_dimensions["C"].width = 28
    ws.column_dimensions["D"].width = 28
    ws.column_dimensions["F"].width = 32
    ws.column_dimensions["K"].width = 36

    # Sheet 2 — CCLI gates
    ws2 = wb.create_sheet("CCLI_Document_Gates")
    style_header(ws2, ["Gate", "Document", "Current", "Required", "Gap", "Priority", "source_id", "Action"])
    for row in ccli_gate_rows():
        ws2.append(row)

    # Sheet 3 — crosswalk
    ws3 = wb.create_sheet("Bench_Crosswalk")
    style_header(
        ws3,
        ["Interface", "SK0146_Test_Bench", "CCLI_Mocking_Bench", "Transfer_note", "Matrix_ref"],
    )
    for row in bench_crosswalk_rows():
        ws3.append(row)

    # Sheet 4 — ATEX context
    ws4 = wb.create_sheet("ATEX_Zone0_Context")
    style_header(ws4, ["Topic", "SK0146_reference", "Test_bench_lab", "CCLI_product", "Knowledge_leak_risk"])
    atex_rows = [
        [
            "Zone classification",
            "reference_zone_level: 0 (non-hazardous gas atmosphere reference product)",
            "User stated ATEX level 0 — ordinary electronics lab / safe area",
            "Final Ex marking TBD via ccli-cci-module-regs-class",
            "Do not assume bench ATEX lvl 0 certifies CCI or SK0146 DUT",
        ],
        [
            "Equipment on bench",
            "Pi 4, USB instruments, non-Ex pogos",
            "Standard lab ESD + HV policy for valve stage",
            "Carrier + enclosure Ex design separate",
            "Using Ex-rated bench gear NOT required in safe area",
        ],
        [
            "Metrology / DLMS",
            "Identity FCT only",
            "UART loopback",
            "61850 + grid codes",
            "Regulatory knowledge leak if DLMS FCT confused with CEI compliance",
        ],
        [
            "RS485 field wiring",
            "M10 terminal + ADM3483",
            "USB adapter to DUT",
            "2× opto RJ45 plant serial",
            "EMC/isolation knowledge leak if kit DB9 treated as product",
        ],
    ]
    for row in atex_rows:
        ws4.append(row)

    path.parent.mkdir(parents=True, exist_ok=True)
    try:
        wb.save(path)
        saved = str(path)
    except PermissionError:
        alt = path.parent / "_generated" / path.name
        alt.parent.mkdir(parents=True, exist_ok=True)
        wb.save(alt)
        saved = str(alt)

    meta = {
        "generated_at": datetime.now(timezone.utc).isoformat(),
        "generator": "scripts/build-sk0146-knowledge-matrix.py",
        "test_bench": str(TEST_BENCH),
        "sk0146_zone_level": 0,
        "rag_probes": {k: {"ingested": v.get("ingested"), "retrieved": v.get("retrieved")} for k, v in probes.items()},
        "primary_leak": "SK0146 schematic corpus NOT_IN_RAG (15 markdown files on disk)",
        "output": saved,
    }
    return meta


def write_markdown_summary(meta: dict, md_path: Path) -> None:
    leaks = sum(1 for v in meta["rag_probes"].values() if not v.get("ingested") and v.get("retrieved") == 0)
    sk_not_ingested = not meta["rag_probes"].get("sk0146-blk_ref_a_rs485", {}).get("ingested")
    body = f"""# SK0146 Knowledge Leak Matrix

Generated: {meta["generated_at"]}

**Test bench:** `{meta["test_bench"]}`  
**Reference product:** SK0146-EVC — ATEX zone level **0** (non-hazardous reference)  
**Excel:** `{meta["output"]}`

## Executive summary

| Finding | Status |
|---------|--------|
| SK0146 schematic RAG corpus (15 sheets) | {"**NOT INGESTED** — primary knowledge leak" if sk_not_ingested else "INGESTED"} |
| Test Bench FCT (FIXTURE_TEST_PLAN, pytest) | **HAVE** locally |
| CCLI PF2 mocking bench docs | **INGESTED** (`ccli-validation-strategy`, `ccli-mocking-bench-bom`) |
| Fixture TBD items (TP57, TP4, keypad leads) | **OPEN** — bench hardware leak |
| Lab platform (TG-524 / MT798X) | **FROZEN** — OpenWrt SDK still **MISSING** |
| Field custom carrier / alternate SoM | **DEFERRED** — see `_archive/som-study/` |

## Top actions (P0)

1. **Ingest SK0146 schematics to RAG:** `Telematry System/scripts/ingest-sk0146-schematics.ps1`
2. **Close fixture TBDs** in `Test Bench/devices/FIXTURE_TEST_PLAN.md` (TP57, TP4, KEY1–6)
3. **Do not conflate** SK0146 3.3 V DIN/DO tests with CCI 10–120 V DI / relay DO (see Bench_Crosswalk sheet)

## RAG probe results

```json
{json.dumps(meta["rag_probes"], indent=2)}
```

Full matrix: open **SK0146_Knowledge_Leaks** sheet in `Architecture/Knowledge_Matrix.xlsx`.
"""
    md_path.write_text(body, encoding="utf-8")


def main() -> int:
    out = ARCH / "Knowledge_Matrix.xlsx"
    meta = write_workbook(out)
    meta_path = ARCH / "_knowledge_matrix_meta.json"
    meta_path.write_text(json.dumps(meta, indent=2), encoding="utf-8")

    md_arch = ARCH / "Knowledge_Matrix_SK0146.md"
    write_markdown_summary(meta, md_arch)

    if TEST_BENCH.exists():
        md_bench = TEST_BENCH / "Knowledge_Matrix_SK0146.md"
        write_markdown_summary(meta, md_bench)

    print(f"Knowledge matrix: {meta['output']}")
    print(f"Meta: {meta_path.relative_to(ROOT)}")
    print(f"Summary: {md_arch.relative_to(ROOT)}")
    if meta["rag_probes"].get("sk0146-blk_ref_a_rs485", {}).get("retrieved", 0) == 0:
        print("\nLEAK: SK0146 sources not in RAG — run ingest-sk0146-schematics.ps1")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
