"""Knowledge risk (KR) matrix, learning plan, and document mapping for PF2 DRB."""

from __future__ import annotations

# Sheet: Knowledge_Matrix (area summary)
KNOWLEDGE_MATRIX_ROWS = [
    # Area, Current, Required, Risk, Impact, Priority, Mitigation, Reuse_ATEX
    ["SoC MT798X (TG-524)", "Frozen — final product", "High", "Low", "Wrong target if drift", "Critical", "ccli-soc-freeze rev 2.0 — only active silicon", "Done"],
    ["OpenWrt SDK / cross-compile", "None", "High", "High", "No on-target binaries", "Critical", "Request TesPro SDK with PO", "New"],
    ["IEC 61850 MMS", "Low", "High", "High", "Communication failure with DSO", "Critical", "libiec61850 on Pi then TG-524", "New"],
    ["IEC 61850 GOOSE", "None", "High", "High", "Incorrect event/curtailment behavior", "Critical", "Build GOOSE test environment", "New"],
    ["Lab platform (TG-524)", "Partial", "High", "Medium", "Delayed L2 HIL", "Critical", "Order gateway; label Eth_A/B/plant ports", "New"],
    ["Linux Boot Chain", "Low", "High", "Medium", "System startup failures", "High", "OpenWrt boot on TG-524; Pi mock meanwhile", "New"],
    ["Secure Boot / TPM", "Partial", "High", "High", "Cybersecurity vulnerability", "Critical", "Verify TPM 2.0 + Secure Boot on receipt", "New"],
    ["Secure Element", "Partial", "Medium", "Medium", "Weak key management", "High", "Use TG-524 TPM; document key hierarchy", "New"],
    ["Ethernet C1 topology", "Low", "High", "High", "Wrong network segmentation", "Critical", "WAN/LAN map; firewall Eth_A ↔ Eth_B", "New"],
    ["IEEE1588/PTP", "None", "Medium", "Medium", "Time synchronization errors", "High", "Confirm CEI need; GNSS first on TG-524", "New"],
    ["GNSS Timing", "Partial", "Medium", "Medium", "Event timestamp mismatch", "Medium", "Optional modem GNSS on TG-524", "New"],
    ["IEC 60870-5-104", "Low", "Medium", "Medium", "Utility integration delay", "High", "Implement 104 simulator", "New"],
    ["Linux Networking", "Medium", "High", "Medium", "Routing/firewall issues", "High", "Lab VLAN/firewall on TG-524 + Pi", "New"],
    ["IEC 62443", "None", "Medium", "High", "Security compliance gap", "Critical", "Zones/conduits + SL target review", "New"],
    ["Thermal Design", "Medium", "High", "Medium", "Overheating in cabinet", "Medium", "TG-524 −40…+75 °C; cabinet budget later", "Partial"],
    ["EMC for Ethernet", "Medium", "High", "Medium", "Certification failures", "High", "Lab on integrated gateway; product EMC deferred", "Medium reuse"],
    ["RS485 / Modbus", "Medium", "High", "Low", "Plant serial issues", "Low", "Reuse SK0146/ATEX craft; confirm isolation", "High reuse"],
    ["LTE Integration", "Medium", "Medium", "Low", "Optional rev A delay", "Low", "TG-524 WAN/cellular; defer cert path", "High reuse"],
    ["Power Supply Basics", "Medium", "High", "Medium", "Brownout / field failure", "Medium", "12–36 V on TG-524; expand for field PSU", "High reuse"],
    ["Digital Inputs 10-120V", "Medium", "High", "Medium", "False PF2 / damage", "Medium", "Wave B Pi fixture; TG-524 has 2 DI only", "High reuse craft"],
    ["Dry Contact DO", "Medium", "High", "Medium", "Curtailment fail", "Medium", "TG-524 2 DO + Wave B relay fixture", "High reuse craft"],
]

# Sheet: Knowledge_Risks (KR-xxx)
KNOWLEDGE_RISKS = [
    {"kr_id": "KR-001", "area": "IEC 61850 MMS", "current": "Low", "required": "High", "risk": "High", "impact": "Communication failure with DSO", "priority": "Critical", "mitigation": "libiec61850 lab on Pi then TG-524 Eth_A", "target_gate": "Lab L2", "rag_source_id": "cei-0-16-allegato-o"},
    {"kr_id": "KR-002", "area": "IEC 61850 GOOSE", "current": "None", "required": "High", "risk": "High", "impact": "Incorrect event/curtailment behavior", "priority": "Critical", "mitigation": "Build GOOSE test environment", "target_gate": "Lab L2", "rag_source_id": "ccli-validation-strategy"},
    {"kr_id": "KR-003", "area": "Lab platform integration", "current": "Medium", "required": "High", "risk": "Medium", "impact": "Delayed on-target tests", "priority": "Critical", "mitigation": "Order TG-524; request OpenWrt SDK; label port map", "target_gate": "Lab bring-up", "rag_source_id": "ccli-tg500-lab-platform"},
    {"kr_id": "KR-004", "area": "Linux Boot Chain", "current": "Low", "required": "High", "risk": "Medium", "impact": "System startup failures", "priority": "High", "mitigation": "OpenWrt boot on TG-524; Pi native build meanwhile", "target_gate": "Lab bring-up", "rag_source_id": "ccli-tg500-lab-platform"},
    {"kr_id": "KR-005", "area": "Secure Boot / TPM", "current": "Partial", "required": "High", "risk": "High", "impact": "Cybersecurity vulnerability", "priority": "Critical", "mitigation": "Verify TPM 2.0 + Secure Boot with TesPro SDK", "target_gate": "Lab bring-up", "rag_source_id": "ccli-soc-freeze"},
    {"kr_id": "KR-006", "area": "Secure Element", "current": "Partial", "required": "Medium", "risk": "Medium", "impact": "Weak key management", "priority": "High", "mitigation": "Document TPM key hierarchy; no filesystem keys", "target_gate": "Hardware Freeze", "rag_source_id": "ccli-prototype-bom"},
    {"kr_id": "KR-007", "area": "Ethernet C1 topology", "current": "Low", "required": "High", "risk": "High", "impact": "Wrong network segmentation", "priority": "Critical", "mitigation": "Map WAN/LAN to Eth_A/B/plant; write C1 ADR", "target_gate": "Lab L2", "rag_source_id": "ccli-prototype-bom"},
    {"kr_id": "KR-008", "area": "IEEE1588/PTP", "current": "None", "required": "Medium", "risk": "Medium", "impact": "Time synchronization errors", "priority": "High", "mitigation": "Confirm CEI need; GNSS on TG-524 if required", "target_gate": "System Integration", "rag_source_id": "ccli-tg500-lab-platform"},
    {"kr_id": "KR-009", "area": "GNSS Timing", "current": "Partial", "required": "Medium", "risk": "Medium", "impact": "Event timestamp mismatch", "priority": "Medium", "mitigation": "Enable modem GNSS / NTP lab path", "target_gate": "System Integration", "rag_source_id": "ccli-tg500-lab-platform"},
    {"kr_id": "KR-010", "area": "IEC 60870-5-104", "current": "Low", "required": "Medium", "risk": "Medium", "impact": "Utility integration delay", "priority": "High", "mitigation": "Implement IEC104 simulator on Eth_B", "target_gate": "Firmware Development", "rag_source_id": "cei-0-16-allegato-t"},
    {"kr_id": "KR-011", "area": "Linux Networking", "current": "Medium", "required": "High", "risk": "Medium", "impact": "Routing/firewall issues", "priority": "High", "mitigation": "Lab VLAN/firewall validation on TG-524", "target_gate": "Lab L2", "rag_source_id": "ccli-validation-strategy"},
    {"kr_id": "KR-012", "area": "IEC 62443", "current": "None", "required": "Medium", "risk": "High", "impact": "Security compliance gap", "priority": "Critical", "mitigation": "Security architecture review; zones/conduits", "target_gate": "Architecture Freeze", "rag_source_id": "ccli-cci-module-regs-class"},
    {"kr_id": "KR-013", "area": "Thermal Design", "current": "Medium", "required": "High", "risk": "Medium", "impact": "Overheating in cabinet", "priority": "Medium", "mitigation": "TG-524 datasheet limits; field enclosure TBD", "target_gate": "Field product", "rag_source_id": "ccli-tespro-tg500-ds"},
    {"kr_id": "KR-014", "area": "EMC for Ethernet", "current": "Medium", "required": "High", "risk": "Medium", "impact": "Certification failures", "priority": "High", "mitigation": "Lab on gateway; product EMC at field design", "target_gate": "Field product", "rag_source_id": "ccli-architecture-framework"},
    {"kr_id": "KR-015", "area": "SoC freeze MT798X", "current": "Frozen", "required": "High", "risk": "Low", "impact": "Corpus drift to alternate SoC", "priority": "Critical", "mitigation": "ccli-soc-freeze rev 2.0 — TG-524 final product SoC; archive alternate SoMs", "target_gate": "Done", "rag_source_id": "ccli-soc-freeze"},
]

LEARNING_PLAN = [
    ["Critical", "SoC MT798X + TG-524 lab path", "Done", "KR-015", "KR-003"],
    ["Critical", "OpenWrt SDK + cross-compile", "Lab bring-up", "KR-003", "KR-004"],
    ["Critical", "IEC 61850 MMS + GOOSE", "Lab L2", "KR-001", "KR-002"],
    ["Critical", "Ethernet C1 topology (Eth_A/B/plant)", "Lab L2", "KR-007", ""],
    ["Critical", "IEC 62443 zones / SL target", "Architecture Freeze", "KR-012", ""],
    ["Critical", "Secure Boot + TPM on TG-524", "Lab bring-up", "KR-005", "KR-006"],
    ["High", "IEC 60870-5-104 simulator", "Firmware Development", "KR-010", ""],
    ["High", "Linux networking / firewall lab", "Lab L2", "KR-011", ""],
    ["High", "PTP (only if required)", "System Integration", "KR-008", ""],
    ["Medium", "GNSS / NTP", "System Integration", "KR-009", ""],
    ["Medium", "Thermal design (field product)", "Field product", "KR-013", ""],
    ["Medium", "EMC Ethernet (field product)", "Field product", "KR-014", ""],
    ["Low", "LTE integration", "Rev A", "", "Reuse ATEX"],
    ["Low", "RS485 / Modbus craft", "Ongoing", "", "Reuse SK0146/ATEX"],
]

DOCUMENT_MAPPING = [
    ["KR-015", "SoC MT798X", "ccli-soc-freeze", "CCI_SOC_Freeze.md", "FROZEN", "—"],
    ["KR-001", "IEC 61850 MMS", "cei-0-16-allegato-o", "CEI Annex O", "HAVE/INGESTED", "D0"],
    ["KR-002", "IEC 61850 GOOSE", "ccli-validation-strategy", "Validation L1-L4", "HAVE/INGESTED", "—"],
    ["KR-003", "Lab platform", "ccli-tg500-lab-platform", "TG-524 lab platform", "HAVE", "SDK MISSING"],
    ["KR-004", "Linux boot", "ccli-tg500-lab-platform", "TG-524 OpenWrt", "HAVE", "—"],
    ["KR-005", "Secure Boot / TPM", "ccli-soc-freeze", "MT798X / TG-524 security", "PARTIAL", "—"],
    ["KR-006", "Key storage", "ccli-prototype-bom", "TPM hierarchy doc", "MISSING", "—"],
    ["KR-007", "Ethernet C1", "ccli-tg500-lab-platform", "Port map + ADR", "PARTIAL", "—"],
    ["KR-008", "PTP", "ccli-tg500-lab-platform", "GNSS / time sync notes", "PARTIAL", "—"],
    ["KR-009", "GNSS", "ccli-tespro-tg500-ds", "Optional modem GNSS", "PARTIAL", "—"],
    ["KR-010", "IEC104", "cei-0-16-allegato-t", "Annex T Eth_B", "HAVE/INGESTED", "—"],
    ["KR-011", "Linux Networking", "ccli-validation-strategy", "Wave A Eth HIL", "HAVE/INGESTED", "—"],
    ["KR-012", "IEC 62443", "ccli-cci-module-regs-class", "Regs classification", "HAVE/INGESTED", "D-CERT"],
    ["KR-013", "Thermal", "ccli-tespro-tg500-ds", "TG-524 operating range", "HAVE", "—"],
    ["KR-014", "EMC", "ccli-architecture-framework", "Field product EMC TBD", "DEFERRED", "—"],
    ["DRB", "Design Review Board", "ccli-design-review-board-pf2", "Design_Review_Board_PF2.md", "LOCAL", "—"],
    ["Reuse", "SK0146 schematics", "sk0146-schematic-index", "rag-structured/sk0146/", "NOT_IN_RAG", "D-SK0146"],
]

HEAT_MAP = [
    ["Critical", "SoC MT798X / TG-524", "Lab path frozen"],
    ["Critical", "OpenWrt SDK", "Blocks on-target build"],
    ["Critical", "IEC61850 (MMS+GOOSE)", "Delays lab L2"],
    ["Critical", "Secure Boot / TPM", "Cyber assessment"],
    ["Critical", "Ethernet C1 topology", "Eth_A / Eth_B isolation"],
    ["Critical", "IEC62443", "Cert track"],
    ["High", "IEC104", "Eth_B integration"],
    ["High", "Linux Networking", "Before L2 HIL"],
    ["High", "PTP (if required)", "Time sync"],
    ["Medium", "GNSS", "Event timestamps"],
    ["Medium", "Thermal (field)", "Deferred to product"],
    ["Medium", "EMC (field)", "Deferred to product"],
    ["Low", "LTE Integration", "TG-524 WAN/cellular"],
    ["Low", "RS485 / Modbus", "SK0146 craft reuse"],
    ["Low", "Power Supply Basics", "12–36 V on TG-524"],
]

REUSE_MATRIX = [
    ["RS485", "High", "TG-524 2× RS485 lab; confirm isolation vs CEI"],
    ["LTE", "High", "TG-524 WAN/cellular integrated"],
    ["Digital Inputs", "High", "Craft only — CCI 10-120V ≠ SK0146 3.3V; Pi Wave B"],
    ["Relay Outputs", "High", "TG-524 2 DO + Wave B fixture"],
    ["Power Protection", "High", "12–36 V on TG-524; field PSU TBD"],
    ["EMC Basics", "Medium", "Integrated gateway lab; product re-qualify later"],
    ["Linux lab SoC", "Frozen", "MT798X on OpenWrt — ccli-soc-freeze"],
    ["Field carrier / SoM", "Deferred", "Archived _archive/som-study/"],
    ["IEC61850", "None — New", "MMS + GOOSE on libiec61850"],
    ["Secure Boot", "Partial", "TPM 2.0 on TG-524 — verify on receipt"],
    ["Ethernet Segmentation", "New", "Eth_A / Eth_B / plant on TG-524 LAN map"],
    ["IEC62443", "None — New", "SL / zones"],
]
