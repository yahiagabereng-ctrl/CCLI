-- Curated queries for Cursor / sqlite3 Architecture/architecture.db
-- Usage: python scripts/init-architecture-db.py --query "SELECT ..."

-- =============================================================================
-- Dashboard
-- =============================================================================
-- SELECT * FROM v_architecture_dashboard;

-- =============================================================================
-- Project file classification (repo cleanup)
-- =============================================================================
-- SELECT class, COUNT(*) AS n FROM project_files GROUP BY class;
-- SELECT path, class, action, notes FROM project_files WHERE action='delete';
-- SELECT * FROM v_project_files_keep;

-- =============================================================================
-- Critical / High knowledge risks (KR matrix)
-- =============================================================================
-- SELECT * FROM v_critical_knowledge_risks;

-- =============================================================================
-- Open knowledge gaps (learning not completed)
-- =============================================================================
-- SELECT area, gap_id, gap_description, priority, leak_type, rag_source_id, rag_status, action
-- FROM v_open_knowledge_gaps
-- ORDER BY CASE priority WHEN 'P0' THEN 0 WHEN 'Critical' THEN 1 WHEN 'P1' THEN 2 WHEN 'High' THEN 3 ELSE 9 END;

-- =============================================================================
-- Requirements without hardware mapping
-- =============================================================================
-- SELECT req_id, title, domain, priority FROM v_requirements_untraced;

-- =============================================================================
-- Critical/high requirements without verification cases
-- =============================================================================
-- SELECT req_id, title, priority FROM v_requirements_unverified;

-- =============================================================================
-- BOM candidates for Ethernet block (C1)
-- =============================================================================
-- SELECT block_code, ref, manufacturer, part_number, doc_status, rag_status
-- FROM v_bom_by_block
-- WHERE block_code = 'C1';

-- =============================================================================
-- Requirement traceability report
-- =============================================================================
-- SELECT req_id, title, block_code, block_name, notes
-- FROM v_requirement_traceability_report
-- ORDER BY req_id;

-- =============================================================================
-- Verification plan
-- =============================================================================
-- SELECT req_id, case_id, test_method, level, bench_ref, status
-- FROM v_verification_report;

-- =============================================================================
-- RAG knowledge leaks (not ingested / offline)
-- =============================================================================
-- SELECT entity_type, entity_key, label, rag_source_id, rag_status, priority
-- FROM v_rag_gaps
-- ORDER BY priority, entity_type;

-- =============================================================================
-- PF2 safety-critical path
-- =============================================================================
-- SELECT r.req_id, r.title, hb.block_code, vc.test_method, vc.status, ri.risk_id
-- FROM requirements r
-- JOIN requirement_traceability rt ON rt.requirement_id = r.id
-- JOIN hardware_blocks hb ON hb.id = rt.hardware_block_id
-- LEFT JOIN verification_cases vc ON vc.requirement_id = r.id
-- LEFT JOIN requirement_risks rr ON rr.requirement_id = r.id
-- LEFT JOIN risks ri ON ri.id = rr.risk_id
-- WHERE r.req_id LIKE 'REQ-PF2-%';

-- =============================================================================
-- Interfaces without BOM (required interfaces only)
-- =============================================================================
-- SELECT i.name, i.protocol, hb.block_code
-- FROM interfaces i
-- JOIN hardware_blocks hb ON hb.id = i.hardware_block_id
-- LEFT JOIN bom_candidates bc ON bc.hardware_block_id = hb.id
-- WHERE i.required = 1 AND bc.id IS NULL;

-- =============================================================================
-- Architecture decisions with links
-- =============================================================================
-- SELECT ad.decision_id, ad.title, adl.entity_type, adl.entity_key
-- FROM architecture_decisions ad
-- JOIN architecture_decision_links adl ON adl.decision_id = ad.decision_id
-- ORDER BY ad.decision_id;

-- =============================================================================
-- SK0146 knowledge leaks
-- =============================================================================
-- SELECT gap_id, domain, gap_description, leak_type, sk0146_bench_need, ccli_pf2_need, rag_status, action
-- FROM knowledge_gaps
-- WHERE gap_id LIKE 'KL-%'
-- ORDER BY priority;

-- =============================================================================
-- Document gate blockers (P0)
-- =============================================================================
-- SELECT gate_id, title, gap, action, rag_source_id, rag_status
-- FROM document_gates
-- WHERE priority = 'P0'
-- ORDER BY gate_id;
