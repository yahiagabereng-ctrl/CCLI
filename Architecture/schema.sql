-- CCLI PF2 Architecture metadata database
-- Master for traceability; RAG chunk store lives in Telematry System separately.
-- Regenerate: python scripts/init-architecture-db.py

PRAGMA foreign_keys = ON;

CREATE TABLE IF NOT EXISTS schema_version (
    version     INTEGER PRIMARY KEY,
    applied_at  TEXT NOT NULL DEFAULT (datetime('now'))
);

CREATE TABLE IF NOT EXISTS hardware_blocks (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    block_code      TEXT UNIQUE NOT NULL,
    block_name      TEXT NOT NULL,
    function        TEXT,
    layer           TEXT,           -- Product | Carrier | Lab only | Software | Optional
    kit_status      TEXT,
    product_status  TEXT,
    parent_id       INTEGER REFERENCES hardware_blocks(id),
    notes           TEXT
);

CREATE TABLE IF NOT EXISTS requirements (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    req_id          TEXT UNIQUE NOT NULL,
    domain          TEXT NOT NULL,
    title           TEXT NOT NULL,
    description     TEXT,
    source_reg      TEXT,
    kit_status      TEXT,
    product_status  TEXT,
    priority        TEXT DEFAULT 'High',
    status          TEXT DEFAULT 'Open',
    rag_source_id   TEXT,
    rag_status      TEXT,
    rag_last_probed_at TEXT
);

CREATE TABLE IF NOT EXISTS interfaces (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    name            TEXT UNIQUE NOT NULL,
    source          TEXT,
    destination     TEXT,
    protocol        TEXT,
    physical        TEXT,
    isolation       TEXT,
    hardware_block_id INTEGER REFERENCES hardware_blocks(id),
    kit_status      TEXT,
    carrier_impl    TEXT,
    required        INTEGER DEFAULT 1,
    rag_source_id   TEXT,
    rag_status      TEXT,
    rag_last_probed_at TEXT
);

CREATE TABLE IF NOT EXISTS bom_candidates (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    ref             TEXT UNIQUE NOT NULL,
    hardware_block_id INTEGER REFERENCES hardware_blocks(id),
    description     TEXT NOT NULL,
    manufacturer    TEXT,
    part_number     TEXT,
    pn_locked       TEXT,
    qty             TEXT,
    est_eur         TEXT,
    doc_status      TEXT,
    layer           TEXT,
    lifecycle       TEXT,
    risk_level      TEXT,
    rag_source_id   TEXT,
    rag_status      TEXT,
    rag_last_probed_at TEXT
);

CREATE TABLE IF NOT EXISTS knowledge_areas (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    name            TEXT UNIQUE NOT NULL,
    current_level   TEXT,
    target_level    TEXT,
    priority        TEXT,
    status          TEXT DEFAULT 'Learning'
);

CREATE TABLE IF NOT EXISTS document_gates (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    gate_id         TEXT UNIQUE NOT NULL,
    title           TEXT NOT NULL,
    current_status  TEXT,
    required_artifact TEXT,
    gap             TEXT,
    priority        TEXT,
    action          TEXT,
    rag_source_id   TEXT,
    rag_status      TEXT,
    rag_last_probed_at TEXT
);

CREATE TABLE IF NOT EXISTS knowledge_risks (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    kr_id           TEXT UNIQUE NOT NULL,
    area            TEXT NOT NULL,
    current_level   TEXT,
    required_level  TEXT,
    risk            TEXT,
    impact          TEXT,
    priority        TEXT,
    mitigation      TEXT,
    target_gate     TEXT,
    learning_status TEXT DEFAULT 'Open',
    rag_source_id   TEXT,
    rag_status      TEXT,
    rag_last_probed_at TEXT
);

CREATE TABLE IF NOT EXISTS knowledge_gaps (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    gap_id          TEXT UNIQUE,
    area_id         INTEGER REFERENCES knowledge_areas(id),
    gate_id         TEXT,
    domain          TEXT,
    gap_description TEXT NOT NULL,
    have_now        TEXT,
    required_knowledge TEXT,
    leak_type       TEXT,
    priority        TEXT,
    learning_status TEXT DEFAULT 'Open',
    action          TEXT,
    hardware_block_id INTEGER REFERENCES hardware_blocks(id),
    sk0146_bench_need TEXT,
    ccli_pf2_need   TEXT,
    atex_note       TEXT,
    rag_source_id   TEXT,
    rag_status      TEXT,
    rag_last_probed_at TEXT
);

CREATE TABLE IF NOT EXISTS risks (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    risk_id         TEXT UNIQUE NOT NULL,
    category        TEXT,
    description     TEXT NOT NULL,
    impact          TEXT,
    likelihood      TEXT,
    mitigation      TEXT,
    owner           TEXT,
    status          TEXT DEFAULT 'Open',
    rag_source_id   TEXT,
    rag_status      TEXT,
    rag_last_probed_at TEXT
);

CREATE TABLE IF NOT EXISTS verification_cases (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    case_id         TEXT UNIQUE,
    requirement_id  INTEGER NOT NULL REFERENCES requirements(id),
    description     TEXT,
    test_method     TEXT,
    level           TEXT,
    pass_criteria   TEXT,
    bench_ref       TEXT,
    evidence_path   TEXT,
    status          TEXT DEFAULT 'Planned'
);

CREATE TABLE IF NOT EXISTS architecture_decisions (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    decision_id     TEXT UNIQUE NOT NULL,
    title           TEXT NOT NULL,
    decision        TEXT NOT NULL,
    rationale       TEXT,
    alternatives    TEXT,
    status          TEXT DEFAULT 'Accepted'
);

CREATE TABLE IF NOT EXISTS architecture_decision_links (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    decision_id     TEXT NOT NULL REFERENCES architecture_decisions(decision_id),
    entity_type     TEXT NOT NULL,  -- requirement | hardware_block | interface | risk | knowledge_gap
    entity_key      TEXT NOT NULL,
    UNIQUE(decision_id, entity_type, entity_key)
);

CREATE TABLE IF NOT EXISTS requirement_traceability (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    requirement_id  INTEGER NOT NULL REFERENCES requirements(id),
    hardware_block_id INTEGER NOT NULL REFERENCES hardware_blocks(id),
    notes           TEXT,
    UNIQUE(requirement_id, hardware_block_id)
);

CREATE TABLE IF NOT EXISTS requirement_interfaces (
    requirement_id  INTEGER NOT NULL REFERENCES requirements(id),
    interface_id    INTEGER NOT NULL REFERENCES interfaces(id),
    PRIMARY KEY (requirement_id, interface_id)
);

CREATE TABLE IF NOT EXISTS requirement_risks (
    requirement_id  INTEGER NOT NULL REFERENCES requirements(id),
    risk_id         INTEGER NOT NULL REFERENCES risks(id),
    PRIMARY KEY (requirement_id, risk_id)
);

CREATE TABLE IF NOT EXISTS requirement_knowledge_gaps (
    requirement_id  INTEGER NOT NULL REFERENCES requirements(id),
    knowledge_gap_id INTEGER NOT NULL REFERENCES knowledge_gaps(id),
    PRIMARY KEY (requirement_id, knowledge_gap_id)
);

-- Views for Cursor / dashboard

CREATE VIEW IF NOT EXISTS v_critical_knowledge_risks AS
SELECT kr_id, area, current_level, required_level, impact, priority, mitigation, target_gate, rag_source_id, rag_status
FROM knowledge_risks
WHERE priority IN ('Critical', 'High') AND learning_status <> 'Completed'
ORDER BY CASE priority WHEN 'Critical' THEN 0 ELSE 1 END, kr_id;

CREATE VIEW IF NOT EXISTS v_open_knowledge_gaps AS
SELECT
    COALESCE(ka.name, kg.domain, 'General') AS area,
    kg.gap_id,
    kg.gap_description,
    kg.have_now,
    kg.required_knowledge,
    kg.leak_type,
    kg.priority,
    kg.learning_status,
    kg.rag_source_id,
    kg.rag_status,
    hb.block_code,
    kg.action
FROM knowledge_gaps kg
LEFT JOIN knowledge_areas ka ON ka.id = kg.area_id
LEFT JOIN hardware_blocks hb ON hb.id = kg.hardware_block_id
WHERE kg.learning_status <> 'Completed';

CREATE VIEW IF NOT EXISTS v_requirements_untraced AS
SELECT r.req_id, r.title, r.domain, r.priority
FROM requirements r
LEFT JOIN requirement_traceability rt ON rt.requirement_id = r.id
WHERE rt.id IS NULL;

CREATE VIEW IF NOT EXISTS v_requirements_unverified AS
SELECT r.req_id, r.title, r.priority
FROM requirements r
LEFT JOIN verification_cases vc ON vc.requirement_id = r.id
WHERE r.priority IN ('Critical', 'High') AND vc.id IS NULL;

CREATE VIEW IF NOT EXISTS v_bom_by_block AS
SELECT
    hb.block_code,
    hb.block_name,
    bc.ref,
    bc.manufacturer,
    bc.part_number,
    bc.doc_status,
    bc.lifecycle,
    bc.risk_level,
    bc.rag_source_id,
    bc.rag_status
FROM bom_candidates bc
JOIN hardware_blocks hb ON hb.id = bc.hardware_block_id;

CREATE VIEW IF NOT EXISTS v_requirement_traceability_report AS
SELECT
    r.req_id,
    r.title,
    r.domain,
    hb.block_code,
    hb.block_name,
    rt.notes
FROM requirements r
JOIN requirement_traceability rt ON rt.requirement_id = r.id
JOIN hardware_blocks hb ON hb.id = rt.hardware_block_id
ORDER BY r.req_id, hb.block_code;

CREATE VIEW IF NOT EXISTS v_verification_report AS
SELECT
    r.req_id,
    r.title,
    vc.case_id,
    vc.test_method,
    vc.level,
    vc.pass_criteria,
    vc.bench_ref,
    vc.status
FROM verification_cases vc
JOIN requirements r ON r.id = vc.requirement_id
ORDER BY r.req_id;

CREATE VIEW IF NOT EXISTS v_rag_gaps AS
SELECT
    'knowledge_gap' AS entity_type,
    gap_id AS entity_key,
    gap_description AS label,
    rag_source_id,
    rag_status,
    priority
FROM knowledge_gaps
WHERE rag_status IN ('NOT_IN_RAG', 'RAG_OFFLINE', 'LOCAL_ONLY')
UNION ALL
SELECT 'document_gate', gate_id, title, rag_source_id, rag_status, priority
FROM document_gates
WHERE rag_status IN ('NOT_IN_RAG', 'RAG_OFFLINE')
UNION ALL
SELECT 'bom', ref, description, rag_source_id, rag_status, 'Medium'
FROM bom_candidates
WHERE doc_status LIKE '%MISSING%' OR rag_status = 'NOT_IN_RAG';

CREATE VIEW IF NOT EXISTS v_architecture_dashboard AS
SELECT
    (SELECT COUNT(*) FROM requirements) AS requirements,
    (SELECT COUNT(*) FROM hardware_blocks) AS hw_blocks,
    (SELECT COUNT(*) FROM interfaces) AS interfaces,
    (SELECT COUNT(*) FROM knowledge_gaps WHERE learning_status <> 'Completed') AS open_gaps,
    (SELECT COUNT(*) FROM risks WHERE status = 'Open') AS open_risks,
    (SELECT COUNT(*) FROM verification_cases WHERE status <> 'Passed') AS pending_verification,
    (SELECT COUNT(*) FROM v_rag_gaps) AS rag_gaps,
    (SELECT COUNT(*) FROM v_requirements_untraced) AS untraced_requirements;


CREATE TABLE IF NOT EXISTS project_files (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    path            TEXT UNIQUE NOT NULL,
    class           TEXT NOT NULL,
    role            TEXT,
    action          TEXT,
    target          TEXT,
    on_disk         INTEGER DEFAULT 0,
    cleaned_at      TEXT,
    notes           TEXT
);

CREATE VIEW IF NOT EXISTS v_project_files_dispose AS
SELECT path, class, role, action, on_disk
FROM project_files
WHERE action IN ('delete') OR class IN ('DISPOSE_BUILD', 'DUPLICATE_OF_KB');

CREATE VIEW IF NOT EXISTS v_project_files_keep AS
SELECT path, class, role, action
FROM project_files
WHERE action = 'keep' OR class IN ('KEEP_SOURCE', 'KEEP_DELIVERABLE');

