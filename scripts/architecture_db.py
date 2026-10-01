#!/usr/bin/env python3
"""SQLite architecture metadata store for CCLI PF2 traceability + RAG bridge."""

from __future__ import annotations

import json
import sqlite3
import urllib.error
import urllib.request
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

from architecture_corpus import (
    ARCHITECTURE_DECISIONS,
    BOM_CANDIDATES,
    DOCUMENT_GATES,
    HARDWARE_BLOCKS,
    INTERFACES,
    KNOWLEDGE_AREAS,
    REQ_INTERFACES,
    REQ_RISKS,
    REQ_TRACEABILITY,
    REQUIREMENTS,
    RISKS,
    SK0146_KNOWLEDGE_GAPS,
    VERIFICATION_CASES,
)
from knowledge_risks_corpus import KNOWLEDGE_RISKS
from project_file_classification import ROOT_CLASSIFICATION

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_DB = ROOT / "Architecture" / "architecture.db"
SCHEMA = ROOT / "Architecture" / "schema.sql"
# ROOT also used when seeding project_files on_disk flags

RAG_BASE = "http://localhost:8001"
TENANT = "telematry"
BOT = "default"


def utc_now() -> str:
    return datetime.now(timezone.utc).isoformat()


def rag_probe(source_id: str | None) -> str:
    if not source_id:
        return "LOCAL_ONLY"
    body = json.dumps(
        {
            "tenant_id": TENANT,
            "bot_id": BOT,
            "query": f"probe {source_id}",
            "source_ids": [source_id],
            "top_k": 1,
        }
    ).encode()
    req = urllib.request.Request(
        f"{RAG_BASE}/v1/query",
        data=body,
        headers={"Content-Type": "application/json"},
        method="POST",
    )
    try:
        with urllib.request.urlopen(req, timeout=15) as resp:
            data = json.loads(resp.read().decode())
        retrieved = int(data.get("traces", {}).get("retrieved") or 0)
        return "INGESTED" if retrieved > 0 else "NOT_IN_RAG"
    except (urllib.error.URLError, TimeoutError, json.JSONDecodeError, OSError):
        return "RAG_OFFLINE"


def probe_all_source_ids(conn: sqlite3.Connection, probe_rag: bool = True) -> dict[str, str]:
    """Update rag_status on all tables that carry rag_source_id."""
    if not probe_rag:
        return {}

    cur = conn.cursor()
    source_ids: set[str] = set()
    for table in (
        "requirements",
        "interfaces",
        "bom_candidates",
        "document_gates",
        "knowledge_gaps",
        "knowledge_risks",
        "risks",
    ):
        for row in cur.execute(
            f"SELECT DISTINCT rag_source_id FROM {table} WHERE rag_source_id IS NOT NULL AND rag_source_id <> ''"
        ):
            if row[0]:
                source_ids.add(row[0])

    status_map: dict[str, str] = {}
    for sid in sorted(source_ids):
        status_map[sid] = rag_probe(sid)

    now = utc_now()
    for table in (
        "requirements",
        "interfaces",
        "bom_candidates",
        "document_gates",
        "knowledge_gaps",
        "knowledge_risks",
        "risks",
    ):
        for sid, status in status_map.items():
            cur.execute(
                f"UPDATE {table} SET rag_status = ?, rag_last_probed_at = ? WHERE rag_source_id = ?",
                (status, now, sid),
            )
    conn.commit()
    return status_map


class ArchitectureDB:
    def __init__(self, path: Path | str = DEFAULT_DB) -> None:
        self.path = Path(path)
        self.path.parent.mkdir(parents=True, exist_ok=True)

    def connect(self) -> sqlite3.Connection:
        conn = sqlite3.connect(self.path)
        conn.row_factory = sqlite3.Row
        conn.execute("PRAGMA foreign_keys = ON")
        return conn

    def init_schema(self) -> None:
        sql = SCHEMA.read_text(encoding="utf-8")
        with self.connect() as conn:
            conn.executescript(sql)
            conn.execute(
                "INSERT OR IGNORE INTO schema_version (version) VALUES (1)"
            )
            conn.commit()

    def reset_and_seed(self, probe_rag: bool = True) -> dict[str, Any]:
        if self.path.exists():
            self.path.unlink()
        self.init_schema()
        with self.connect() as conn:
            self._seed(conn)
            conn.commit()
        rag_map = {}
        with self.connect() as conn:
            if probe_rag:
                rag_map = probe_all_source_ids(conn, probe_rag=True)
        return self.dashboard(probe_rag=False) | {"rag_probes": rag_map}

    def _seed(self, conn: sqlite3.Connection) -> None:
        cur = conn.cursor()

        for b in HARDWARE_BLOCKS:
            cur.execute(
                """INSERT INTO hardware_blocks
                   (block_code, block_name, function, layer, kit_status, product_status)
                   VALUES (?, ?, ?, ?, ?, ?)""",
                (b["block_code"], b["block_name"], b["function"], b["layer"], b["kit_status"], b["product_status"]),
            )

        block_ids = {r[1]: r[0] for r in cur.execute("SELECT id, block_code FROM hardware_blocks")}

        for r in REQUIREMENTS:
            cur.execute(
                """INSERT INTO requirements
                   (req_id, domain, title, description, source_reg, kit_status, product_status,
                    priority, status, rag_source_id)
                   VALUES (?, ?, ?, ?, ?, ?, ?, ?, 'Open', ?)""",
                (
                    r["req_id"], r["domain"], r["title"], r["title"],
                    r.get("source_reg"), r.get("kit_status"), r.get("product_status"),
                    r.get("priority", "High"), r.get("rag_source_id"),
                ),
            )

        req_ids = {r[1]: r[0] for r in cur.execute("SELECT id, req_id FROM requirements")}

        for i in INTERFACES:
            cur.execute(
                """INSERT INTO interfaces
                   (name, source, destination, protocol, physical, isolation,
                    hardware_block_id, kit_status, carrier_impl, required, rag_source_id)
                   VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)""",
                (
                    i["name"], i["source"], i["destination"], i["protocol"],
                    i["physical"], i["isolation"], block_ids.get(i["block_code"]),
                    i["kit_status"], i["carrier_impl"], i.get("required", 1), i.get("rag_source_id"),
                ),
            )

        iface_ids = {r[1]: r[0] for r in cur.execute("SELECT id, name FROM interfaces")}

        for b in BOM_CANDIDATES:
            cur.execute(
                """INSERT INTO bom_candidates
                   (ref, hardware_block_id, description, manufacturer, part_number, pn_locked,
                    qty, est_eur, doc_status, layer, rag_source_id)
                   VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)""",
                (
                    b["ref"], block_ids.get(b["block_code"]), b["description"],
                    b.get("manufacturer"), b.get("part_number"), b.get("pn_locked"),
                    b.get("qty"), b.get("est_eur"), b.get("doc_status"), b.get("layer"),
                    b.get("rag_source_id"),
                ),
            )

        for ka in KNOWLEDGE_AREAS:
            cur.execute(
                """INSERT INTO knowledge_areas (name, current_level, target_level, priority, status)
                   VALUES (?, ?, ?, ?, ?)""",
                (ka["name"], ka["current_level"], ka["target_level"], ka["priority"], ka["status"]),
            )

        area_ids = {r[1]: r[0] for r in cur.execute("SELECT id, name FROM knowledge_areas")}

        for kr in KNOWLEDGE_RISKS:
            cur.execute(
                """INSERT INTO knowledge_risks
                   (kr_id, area, current_level, required_level, risk, impact, priority,
                    mitigation, target_gate, learning_status, rag_source_id)
                   VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, 'Open', ?)""",
                (
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
                ),
            )

        for g in DOCUMENT_GATES:
            cur.execute(
                """INSERT INTO document_gates
                   (gate_id, title, current_status, required_artifact, gap, priority, action, rag_source_id)
                   VALUES (?, ?, ?, ?, ?, ?, ?, ?)""",
                (
                    g["gate_id"], g["title"], g["current_status"], g["required_artifact"],
                    g["gap"], g["priority"], g["action"], g.get("rag_source_id"),
                ),
            )

        for kg in SK0146_KNOWLEDGE_GAPS:
            area_name = kg.get("area")
            cur.execute(
                """INSERT INTO knowledge_gaps
                   (gap_id, area_id, domain, gap_description, have_now, required_knowledge,
                    leak_type, priority, learning_status, action, hardware_block_id,
                    sk0146_bench_need, ccli_pf2_need, atex_note, rag_source_id)
                   VALUES (?, ?, ?, ?, ?, ?, ?, ?, 'Open', ?, ?, ?, ?, ?, ?)""",
                (
                    kg["gap_id"], area_ids.get(area_name) if area_name else None,
                    kg.get("domain"), kg["gap_description"], kg.get("have_now"),
                    kg.get("required_knowledge"), kg.get("leak_type"), kg.get("priority"),
                    kg.get("action"), block_ids.get(kg.get("block_code")),
                    kg.get("sk0146_bench_need"), kg.get("ccli_pf2_need"), kg.get("atex_note"),
                    kg.get("rag_source_id"),
                ),
            )

        # Document gate gaps as knowledge_gaps too
        for g in DOCUMENT_GATES:
            if g["gap"] in ("None", "—", ""):
                continue
            cur.execute(
                """INSERT INTO knowledge_gaps
                   (gap_id, gate_id, domain, gap_description, have_now, required_knowledge,
                    leak_type, priority, learning_status, action, rag_source_id)
                   VALUES (?, ?, 'Document gate', ?, ?, ?, 'DOC_GATE', ?, 'Open', ?, ?)""",
                (
                    f"KG-{g['gate_id']}", g["gate_id"], g["title"],
                    g["current_status"], g["required_artifact"],
                    g["priority"], g["action"], g.get("rag_source_id"),
                ),
            )

        for r in RISKS:
            cur.execute(
                """INSERT INTO risks
                   (risk_id, category, description, impact, likelihood, mitigation, owner, status, rag_source_id)
                   VALUES (?, ?, ?, ?, ?, ?, ?, 'Open', ?)""",
                (
                    r["risk_id"], r["category"], r["description"], r["impact"],
                    r["likelihood"], r["mitigation"], r["owner"], r.get("rag_source_id"),
                ),
            )

        risk_ids = {r[1]: r[0] for r in cur.execute("SELECT id, risk_id FROM risks")}

        for v in VERIFICATION_CASES:
            cur.execute(
                """INSERT INTO verification_cases
                   (case_id, requirement_id, description, test_method, level,
                    pass_criteria, bench_ref, evidence_path, status)
                   VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)""",
                (
                    v["case_id"], req_ids[v["req_id"]], v["description"],
                    v["test_method"], v["level"], v["pass_criteria"],
                    v["bench_ref"], v.get("evidence_path", "—"), v.get("status", "Planned"),
                ),
            )

        for req_id, block_code, notes in REQ_TRACEABILITY:
            cur.execute(
                """INSERT OR IGNORE INTO requirement_traceability
                   (requirement_id, hardware_block_id, notes) VALUES (?, ?, ?)""",
                (req_ids[req_id], block_ids[block_code], notes),
            )

        for req_id, iface_name in REQ_INTERFACES:
            cur.execute(
                "INSERT OR IGNORE INTO requirement_interfaces (requirement_id, interface_id) VALUES (?, ?)",
                (req_ids[req_id], iface_ids[iface_name]),
            )

        for req_id, risk_id in REQ_RISKS:
            cur.execute(
                "INSERT OR IGNORE INTO requirement_risks (requirement_id, risk_id) VALUES (?, ?)",
                (req_ids[req_id], risk_ids[risk_id]),
            )

        gap_ids = {r[1]: r[0] for r in cur.execute("SELECT id, gap_id FROM knowledge_gaps")}

        for d in ARCHITECTURE_DECISIONS:
            cur.execute(
                """INSERT INTO architecture_decisions
                   (decision_id, title, decision, rationale, alternatives, status)
                   VALUES (?, ?, ?, ?, ?, ?)""",
                (d["decision_id"], d["title"], d["decision"], d["rationale"], d["alternatives"], d["status"]),
            )
            for entity_type, entity_key in d.get("links", []):
                cur.execute(
                    """INSERT INTO architecture_decision_links (decision_id, entity_type, entity_key)
                       VALUES (?, ?, ?)""",
                    (d["decision_id"], entity_type, entity_key),
                )

        # Link KL-003 to REQ-IO-001
        if "KL-003" in gap_ids:
            cur.execute(
                "INSERT OR IGNORE INTO requirement_knowledge_gaps (requirement_id, knowledge_gap_id) VALUES (?, ?)",
                (req_ids["REQ-IO-001"], gap_ids["KL-003"]),
            )

        # Project file classification inventory (paths relative to repo root)
        for pf in ROOT_CLASSIFICATION:
            on_disk = 1 if (ROOT / pf["path"]).exists() else 0
            cur.execute(
                """INSERT OR REPLACE INTO project_files
                   (path, class, role, action, target, on_disk, notes)
                   VALUES (?, ?, ?, ?, ?, ?, ?)""",
                (
                    pf["path"],
                    pf["class"],
                    pf["role"],
                    pf["action"],
                    pf.get("target"),
                    on_disk,
                    "seeded from project_file_classification",
                ),
            )

    def dashboard(self, probe_rag: bool = False) -> dict[str, Any]:
        if probe_rag:
            with self.connect() as conn:
                probe_all_source_ids(conn, probe_rag=True)
        with self.connect() as conn:
            row = conn.execute("SELECT * FROM v_architecture_dashboard").fetchone()
            return dict(row) if row else {}

    def query(self, sql: str, params: tuple = ()) -> list[dict[str, Any]]:
        with self.connect() as conn:
            rows = conn.execute(sql, params).fetchall()
            return [dict(r) for r in rows]

    def export_json_snapshot(self, out_path: Path) -> None:
        tables = [
            "hardware_blocks", "requirements", "interfaces", "bom_candidates",
            "knowledge_areas", "document_gates", "knowledge_gaps", "risks",
            "verification_cases", "architecture_decisions", "v_architecture_dashboard",
        ]
        snapshot: dict[str, Any] = {"generated_at": utc_now(), "db": str(self.path)}
        with self.connect() as conn:
            for t in tables:
                rows = conn.execute(f"SELECT * FROM {t}").fetchall()
                snapshot[t] = [dict(r) for r in rows]
        out_path.write_text(json.dumps(snapshot, indent=2), encoding="utf-8")
