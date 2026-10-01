#!/usr/bin/env python3
"""Generate Document Coverage Matrix + Missing Docs + Knowledge Risks + Learning Roadmap."""

from __future__ import annotations

import json
import sys
from datetime import datetime, timezone
from pathlib import Path

from openpyxl import Workbook
from openpyxl.styles import Alignment, Font, PatternFill

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))

from document_coverage_corpus import (  # noqa: E402
    DOCUMENT_COVERAGE,
    HEAT_MAP,
    KNOWLEDGE_RISKS,
    LEARNING_PLAN,
    MISSING_DOCUMENTS,
    REUSE_MATRIX,
)

ARCH = ROOT / "Architecture"
KB_ENG = ROOT / "knowledge-base" / "08-engineering"

HEADER_FILL = PatternFill("solid", fgColor="1A365D")
HEADER_FONT = Font(bold=True, color="FFFFFF")
P0_FILL = PatternFill("solid", fgColor="FEE2E2")
P1_FILL = PatternFill("solid", fgColor="FEF3C7")
HAVE_FILL = PatternFill("solid", fgColor="DCFCE7")
PARTIAL_FILL = PatternFill("solid", fgColor="DBEAFE")


def style_header(ws, headers: list[str]) -> None:
    ws.append(headers)
    for cell in ws[1]:
        cell.fill = HEADER_FILL
        cell.font = HEADER_FONT
        cell.alignment = Alignment(wrap_text=True, vertical="top")


def paint(ws, priority: str | None = None, status: str | None = None) -> None:
    fill = None
    if priority == "P0" or priority == "Critical":
        fill = P0_FILL
    elif priority == "P1" or priority == "High":
        fill = P1_FILL
    elif status == "HAVE":
        fill = HAVE_FILL
    elif status == "PARTIAL":
        fill = PARTIAL_FILL
    if fill:
        for cell in ws[ws.max_row]:
            cell.fill = fill


def save_wb(wb: Workbook, name: str) -> Path:
    path = ARCH / name
    try:
        wb.save(path)
        return path
    except PermissionError:
        alt = ARCH / "_generated" / name
        alt.parent.mkdir(parents=True, exist_ok=True)
        wb.save(alt)
        return alt


def write_excel() -> Path:
    wb = Workbook()

    ws = wb.active
    ws.title = "Document_Coverage"
    style_header(
        ws,
        [
            "Block",
            "Subsystem",
            "Required_Knowledge",
            "Available_Documents",
            "Missing_Documents",
            "Priority",
            "Impact_If_Missing",
            "Status",
            "Gate",
            "source_ids",
        ],
    )
    for r in DOCUMENT_COVERAGE:
        ws.append(
            [
                r["block"],
                r["subsystem"],
                r["required_knowledge"],
                r["available"],
                r["missing"],
                r["priority"],
                r["impact"],
                r["status"],
                r["gate"],
                r["source_ids"],
            ]
        )
        paint(ws, priority=r["priority"], status=r["status"])
    for col in ws.columns:
        ws.column_dimensions[col[0].column_letter].width = 18
    ws.column_dimensions["C"].width = 36
    ws.column_dimensions["D"].width = 40
    ws.column_dimensions["E"].width = 40
    ws.column_dimensions["G"].width = 36

    ws2 = wb.create_sheet("Missing_Documents")
    style_header(
        ws2,
        ["Doc_ID", "Document", "Subsystem", "Target_Path", "source_id", "Priority", "Impact", "Action", "Blocks_Gate"],
    )
    for m in sorted(MISSING_DOCUMENTS, key=lambda x: (x["priority"], x["doc_id"])):
        ws2.append(
            [
                m["doc_id"],
                m["document"],
                m["subsystem"],
                m["target"],
                m["source_id"],
                m["priority"],
                m["impact"],
                m["action"],
                m["blocks_gate"],
            ]
        )
        paint(ws2, priority=m["priority"])

    ws3 = wb.create_sheet("Knowledge_Risk_Matrix")
    style_header(
        ws3,
        ["KR_ID", "Knowledge_Area", "Current", "Required", "Risk", "Impact", "Priority", "Mitigation", "Target_Gate", "rag_source_id"],
    )
    for kr in KNOWLEDGE_RISKS:
        ws3.append(
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
        paint(ws3, priority=kr["priority"])

    ws4 = wb.create_sheet("Learning_Roadmap")
    style_header(ws4, ["Priority", "Topic", "Target_Before", "KR_Primary", "KR_Secondary", "Phase_Order"])
    order = {"Critical": 1, "High": 2, "Medium": 3, "Low": 4}
    for i, row in enumerate(sorted(LEARNING_PLAN, key=lambda r: order.get(r[0], 9)), start=1):
        ws4.append([*row, i])
        paint(ws4, priority=row[0])

    ws5 = wb.create_sheet("Heat_Map")
    style_header(ws5, ["Band", "Topic", "Note"])
    for row in HEAT_MAP:
        ws5.append(row)
        paint(ws5, priority=row[0])

    ws6 = wb.create_sheet("Reuse_Matrix")
    style_header(ws6, ["Area", "Reuse_from_ATEX", "Caution"])
    for row in REUSE_MATRIX:
        ws6.append(row)

    return save_wb(wb, "Document_Coverage_Matrix.xlsx")


def write_markdown(excel_path: Path) -> Path:
    p0 = [m for m in MISSING_DOCUMENTS if m["priority"] == "P0"]
    p1 = [m for m in MISSING_DOCUMENTS if m["priority"] == "P1"]
    p2 = [m for m in MISSING_DOCUMENTS if m["priority"] in ("P2", "P3")]

    lines = [
        "# Document Coverage Matrix — PF2 CCI",
        "",
        f"**Document ID:** CCLI-DOC-COV-001  ",
        f"**Revision:** 1.0  ",
        f"**Date:** {datetime.now(timezone.utc).date().isoformat()}  ",
        f"**Role:** Chief Hardware Architect  ",
        f"**Excel:** `{excel_path}`  ",
        f"**Corpus:** DOWNLOADS.md · CCI_Prototype_BOM.md · Architecture/* · Design_Review_Board_PF2.md",
        "",
        "---",
        "",
        "## Document Coverage Matrix (by subsystem)",
        "",
        "| Block | Subsystem | Required knowledge | Available documents | Missing documents | Priority | Impact if missing | Status |",
        "|-------|-----------|--------------------|---------------------|-------------------|----------|-------------------|--------|",
    ]
    for r in DOCUMENT_COVERAGE:
        lines.append(
            f"| {r['block']} | {r['subsystem']} | {r['required_knowledge']} | {r['available']} | "
            f"{r['missing']} | {r['priority']} | {r['impact']} | {r['status']} |"
        )

    lines += [
        "",
        "---",
        "",
        "## 1. Missing Documents Report",
        "",
        f"**Total missing/partial actions:** {len(MISSING_DOCUMENTS)} · **P0:** {len(p0)} · **P1:** {len(p1)} · **P2/P3:** {len(p2)}",
        "",
        "### P0 — Architecture / Hardware Freeze blockers",
        "",
        "| ID | Document | Subsystem | Impact | Action | Gate |",
        "|----|----------|-----------|--------|--------|------|",
    ]
    for m in p0:
        lines.append(
            f"| {m['doc_id']} | {m['document']} | {m['subsystem']} | {m['impact']} | {m['action']} | {m['blocks_gate']} |"
        )

    lines += [
        "",
        "### P1 — Needed before schematic / firmware",
        "",
        "| ID | Document | Subsystem | Impact | Action | Gate |",
        "|----|----------|-----------|--------|--------|------|",
    ]
    for m in p1:
        lines.append(
            f"| {m['doc_id']} | {m['document']} | {m['subsystem']} | {m['impact']} | {m['action']} | {m['blocks_gate']} |"
        )

    lines += [
        "",
        "### P2/P3 — PCB / product / optional",
        "",
        "| ID | Document | Subsystem | Impact | Action | Gate |",
        "|----|----------|-----------|--------|--------|------|",
    ]
    for m in p2:
        lines.append(
            f"| {m['doc_id']} | {m['document']} | {m['subsystem']} | {m['impact']} | {m['action']} | {m['blocks_gate']} |"
        )

    lines += [
        "",
        "---",
        "",
        "## 2. Knowledge Risk Matrix",
        "",
        "| KR | Area | Current → Required | Risk | Impact | Priority | Mitigation | Target before |",
        "|----|------|-------------------|------|--------|----------|------------|---------------|",
    ]
    for kr in KNOWLEDGE_RISKS:
        lines.append(
            f"| {kr['kr_id']} | {kr['area']} | {kr['current']} → {kr['required']} | {kr['risk']} | "
            f"{kr['impact']} | {kr['priority']} | {kr['mitigation']} | {kr['target_gate']} |"
        )

    lines += [
        "",
        "### Heat map",
        "",
        "| Band | Topics |",
        "|------|--------|",
    ]
    bands: dict[str, list[str]] = {}
    for band, topic, _ in HEAT_MAP:
        bands.setdefault(band, []).append(topic)
    for band in ("Critical", "High", "Medium", "Low"):
        lines.append(f"| **{band}** | {'; '.join(bands.get(band, []))} |")

    lines += [
        "",
        "### Reuse from ATEX / SK0146",
        "",
        "| Area | Reuse | Caution |",
        "|------|-------|---------|",
    ]
    for area, reuse, caution in REUSE_MATRIX:
        lines.append(f"| {area} | {reuse} | {caution} |")

    lines += [
        "",
        "---",
        "",
        "## 3. Learning Roadmap",
        "",
        "| Order | Priority | Topic | Must complete before | KR |",
        "|------:|----------|-------|---------------------|----|",
    ]
    order = {"Critical": 1, "High": 2, "Medium": 3, "Low": 4}
    for i, row in enumerate(sorted(LEARNING_PLAN, key=lambda r: order.get(r[0], 9)), start=1):
        kr = row[3] + (f", {row[4]}" if row[4] else "")
        lines.append(f"| {i} | {row[0]} | {row[1]} | {row[2]} | {kr} |")

    lines += [
        "",
        "### Roadmap phases",
        "",
        "```text",
        "NOW → Architecture Freeze",
        "  ├─ IEC 61850 MMS + GOOSE lab (KR-001/002)",
        "  ├─ C1 Ethernet ADR locked (KR-007)",
        "  ├─ IEC 62443 zones (KR-012)",
        "  └─ Linux networking lab (KR-011)",
        "",
        "Architecture Freeze → Hardware Freeze",
        "  ├─ Secure Boot + key ceremony (KR-005/004)",
        "  └─ Secure Element prototype (KR-006)",
        "",
        "Hardware Freeze → Schematic Capture",
        "  └─ Carrier + pin budget D3/D4 (KR-003) + P0 docs MD-001…005",
        "",
        "Schematic → PCB Layout",
        "  ├─ Thermal (KR-013)",
        "  └─ EMC Ethernet (KR-014)",
        "",
        "Integration",
        "  ├─ GNSS/PPS (KR-009)",
        "  └─ PTP only if required (KR-008)",
        "```",
        "",
        "---",
        "",
        "## Chief Architect summary",
        "",
        "| Metric | Value |",
        "|--------|------:|",
        f"| Subsystems reviewed | {len(DOCUMENT_COVERAGE)} |",
        f"| Missing document actions | {len(MISSING_DOCUMENTS)} |",
        f"| P0 blockers | {len(p0)} |",
        f"| Knowledge risks (KR) | {len(KNOWLEDGE_RISKS)} |",
        f"| Critical KR | {sum(1 for k in KNOWLEDGE_RISKS if k['priority']=='Critical')} |",
        "",
        "**Immediate P0 pack:** MD-001 OpenWrt SDK · MD-002 SKU confirm · MD-003 Lab port map · MD-005 C1 ADR · "
        "MD-005 C1 ADR · MD-014 Key ceremony · MD-015 62443 zones · MD-018 GOOSE procedure · MD-022 SK0146 RAG ingest.",
        "",
        f"Generated: {datetime.now(timezone.utc).isoformat()}",
    ]

    md_arch = ARCH / "Document_Coverage_Matrix.md"
    md_arch.write_text("\n".join(lines) + "\n", encoding="utf-8")
    KB_ENG.mkdir(parents=True, exist_ok=True)
    md_kb = KB_ENG / "CCI_Document_Coverage_Matrix.md"
    md_kb.write_text("\n".join(lines) + "\n", encoding="utf-8")
    return md_arch


def main() -> int:
    ARCH.mkdir(parents=True, exist_ok=True)
    excel = write_excel()
    md = write_markdown(excel)
    meta = {
        "generated_at": datetime.now(timezone.utc).isoformat(),
        "excel": str(excel),
        "markdown": str(md),
        "subsystems": len(DOCUMENT_COVERAGE),
        "missing_docs": len(MISSING_DOCUMENTS),
        "p0": sum(1 for m in MISSING_DOCUMENTS if m["priority"] == "P0"),
        "knowledge_risks": len(KNOWLEDGE_RISKS),
    }
    (ARCH / "_document_coverage_meta.json").write_text(json.dumps(meta, indent=2), encoding="utf-8")
    print(f"Excel: {excel}")
    print(f"Markdown: {md}")
    print(f"P0 missing docs: {meta['p0']} / {meta['missing_docs']}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
