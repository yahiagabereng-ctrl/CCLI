"""Authoritative seed data for CCLI PF2 architecture DB and Excel generators."""

from __future__ import annotations

HARDWARE_BLOCKS = [
    {"block_code": "SoM", "block_name": "SoM", "function": "Linux Processing", "layer": "Product", "kit_status": "HAVE", "product_status": "Required"},
    {"block_code": "KIT", "block_name": "Eval Kit", "function": "Lab bring-up only", "layer": "Lab only", "kit_status": "HAVE", "product_status": "Not shipped"},
    {"block_code": "C1", "block_name": "Ethernet", "function": "Network Segregation — Eth_A, Eth_B, Plant x2", "layer": "Carrier", "kit_status": "Partial 2x GbE", "product_status": "Required"},
    {"block_code": "C2", "block_name": "RS485", "function": "Modbus Communication — 2x opto-isolated", "layer": "Carrier", "kit_status": "1x DB9 non-opto", "product_status": "Required"},
    {"block_code": "C3", "block_name": "Protocols", "function": "IEC61850 · 60870 · Modbus stacks", "layer": "Software", "kit_status": "SW mock L1", "product_status": "Required"},
    {"block_code": "C4", "block_name": "Digital I/O", "function": "DI 10-120V · DO dry contact", "layer": "Carrier", "kit_status": "None", "product_status": "Required"},
    {"block_code": "C5", "block_name": "GNSS", "function": "Time Synchronization", "layer": "Carrier", "kit_status": "None", "product_status": "Required"},
    {"block_code": "C6", "block_name": "Secure Element", "function": "Root of Trust — HAB + SE", "layer": "Product", "kit_status": "Partial BSP", "product_status": "Required"},
    {"block_code": "C7", "block_name": "LTE", "function": "Backup Communication", "layer": "Optional", "kit_status": "Optional", "product_status": "Optional rev A"},
    {"block_code": "C8", "block_name": "Console", "function": "USB commissioning", "layer": "Product", "kit_status": "HAVE USB", "product_status": "Required"},
    {"block_code": "C9", "block_name": "Mechanical", "function": "DIN-rail enclosure", "layer": "Product", "kit_status": "N/A", "product_status": "MISSING"},
    {"block_code": "C10", "block_name": "Power Supply", "function": "12-24V Input Conversion", "layer": "Carrier", "kit_status": "7-24V kit bench", "product_status": "Required"},
]

REQUIREMENTS = [
    {"req_id": "REQ-REG-001", "domain": "Regulatory", "title": "CEI 0-16 Annex O/T/M compliant Central Plant Controller", "source_reg": "cei-0-16-allegato-o/t", "kit_status": "N/A", "product_status": "Required", "priority": "Critical", "rag_source_id": "cei-0-16-allegato-o"},
    {"req_id": "REQ-NET-001", "domain": "Network", "title": "Eth_A logical channel to DSO per EN 61850 (Annex O O.13.1.2)", "source_reg": "cei-0-16-allegato-o", "kit_status": "Partial 2x GbE", "product_status": "4-port isolation", "priority": "Critical", "rag_source_id": "cei-0-16-allegato-o"},
    {"req_id": "REQ-NET-002", "domain": "Network", "title": "Eth_B to Operatore Abilitato — separate from Eth_A", "source_reg": "cei-0-16-allegato-t", "kit_status": "Partial", "product_status": "Required PHY/VLAN", "priority": "Critical", "rag_source_id": "cei-0-16-allegato-t"},
    {"req_id": "REQ-NET-003", "domain": "Network", "title": "Two outside plant network interfaces (O.13.1.1)", "source_reg": "cei-0-16-allegato-o", "kit_status": "None on kit", "product_status": "Plant x2 on carrier", "priority": "High", "rag_source_id": "cei-0-16-allegato-o"},
    {"req_id": "REQ-SER-001", "domain": "Serial", "title": "2x opto-isolated RS485 Modbus RTU to energy analyzer", "source_reg": "ccli-vendor-selection C2", "kit_status": "1x RS485 DB9 non-opto", "product_status": "Required carrier", "priority": "High", "rag_source_id": "ccli-vendor-selection"},
    {"req_id": "REQ-IO-001", "domain": "Digital I/O", "title": "DI 10-120 Vdc x5 external field supply", "source_reg": "ccli-vendor-selection C4", "kit_status": "None GPIO only", "product_status": "Carrier opto", "priority": "High", "rag_source_id": "ccli-vendor-selection"},
    {"req_id": "REQ-IO-002", "domain": "Digital I/O", "title": "DO dry contact x3 for PF2 curtailment", "source_reg": "ccli-validation-strategy", "kit_status": "SW stub", "product_status": "Carrier relay", "priority": "Critical", "rag_source_id": "ccli-validation-strategy"},
    {"req_id": "REQ-PWR-001", "domain": "Power", "title": "12-24 Vdc wide-range isolated product input", "source_reg": "ccli-prototype-bom C10", "kit_status": "7-24V kit bench", "product_status": "Carrier PSU", "priority": "High", "rag_source_id": "ccli-prototype-bom"},
    {"req_id": "REQ-SEC-001", "domain": "Security", "title": "Secure boot + signed firmware images", "source_reg": "ccli-tg500-lab-platform", "kit_status": "Partial BSP", "product_status": "Required product", "priority": "Critical", "rag_source_id": "ccli-tg500-lab-platform"},
    {"req_id": "REQ-SEC-002", "domain": "Security", "title": "Key storage in secure element / TPM path", "source_reg": "ccli-prototype-bom C6", "kit_status": "Verify SoM SKU", "product_status": "Required", "priority": "Critical", "rag_source_id": "ccli-prototype-bom"},
    {"req_id": "REQ-TIME-001", "domain": "Time", "title": "GNSS timestamp for measurements and events", "source_reg": "ccli-vendor-selection C5", "kit_status": "None on kit", "product_status": "Carrier GNSS", "priority": "Medium", "rag_source_id": "ccli-vendor-selection"},
    {"req_id": "REQ-LTE-001", "domain": "Cellular", "title": "LTE backup / remote monitoring (optional rev A)", "source_reg": "ccli-prototype-bom", "kit_status": "Optional", "product_status": "Carrier modem", "priority": "Low", "rag_source_id": "ccli-prototype-bom"},
    {"req_id": "REQ-PF2-001", "domain": "PF2", "title": "Curtailment command path with safe-state on comms loss", "source_reg": "ccli-validation-strategy", "kit_status": "SW mock L1", "product_status": "DO + logic", "priority": "Critical", "rag_source_id": "ccli-validation-strategy"},
]

INTERFACES = [
    {"name": "Eth_A", "source": "CCI SoM+PHY", "destination": "DSO", "protocol": "IEC 61850 MMS / GOOSE", "physical": "RJ45 GbE", "isolation": "Separate PHY/VLAN from Eth_B", "block_code": "C1", "kit_status": "GbE port 1 — SW demo", "carrier_impl": "Dedicated PHY + magjack", "rag_source_id": "cei-0-16-allegato-o"},
    {"name": "Eth_B", "source": "CCI SoM+PHY", "destination": "Operatore Abilitato", "protocol": "IEC 61850 / IEC 60870-5-104", "physical": "RJ45 GbE", "isolation": "Separate from Eth_A", "block_code": "C1", "kit_status": "GbE port 2 — SW demo", "carrier_impl": "Dedicated PHY + magjack", "rag_source_id": "cei-0-16-allegato-t"},
    {"name": "Plant-1", "source": "CCI", "destination": "Plant LAN / inverters", "protocol": "Modbus TCP / raw Ethernet", "physical": "RJ45", "isolation": "Logically separated from DSO path", "block_code": "C1", "kit_status": "Not on kit", "carrier_impl": "Switch port or PHY3", "rag_source_id": "ccli-prototype-bom"},
    {"name": "Plant-2", "source": "CCI", "destination": "Plant HMI / spare", "protocol": "Modbus TCP / spare", "physical": "RJ45", "isolation": "Same policy as Plant-1", "block_code": "C1", "kit_status": "Not on kit", "carrier_impl": "PHY4 / switch", "rag_source_id": "ccli-prototype-bom"},
    {"name": "RS485-A", "source": "CCI carrier", "destination": "Energy analyzer", "protocol": "Modbus RTU", "physical": "RJ45 8/8 plant serial", "isolation": "Opto-isolated", "block_code": "C2", "kit_status": "Not CEI-compliant", "carrier_impl": "U-485 transceiver x1", "rag_source_id": "ccli-vendor-selection"},
    {"name": "RS485-B", "source": "CCI carrier", "destination": "Spare / IEC 60870-101", "protocol": "Modbus RTU / 101 optional", "physical": "RJ45 8/8", "isolation": "Opto-isolated", "block_code": "C2", "kit_status": "Kit RS485 DB9 lab only", "carrier_impl": "U-485 transceiver x2", "rag_source_id": "ccli-prototype-bom"},
    {"name": "DI-1..5", "source": "Field 10-120 Vdc", "destination": "CCI GPIO via opto", "protocol": "Digital input", "physical": "Terminal block", "isolation": "Opto + external field supply", "block_code": "C4", "kit_status": "None", "carrier_impl": "U-DI conditioner", "rag_source_id": "ccli-vendor-selection"},
    {"name": "DO-1..3", "source": "CCI relay driver", "destination": "Curtailment contactors", "protocol": "Dry contact PF2", "physical": "Terminal block", "isolation": "Relay isolation", "block_code": "C4", "kit_status": "SW stub only", "carrier_impl": "K-DO relay", "rag_source_id": "ccli-validation-strategy"},
    {"name": "GNSS", "source": "Antenna SMA", "destination": "SoM UART/USB", "protocol": "NMEA + PPS", "physical": "SMA coax", "isolation": "ESD protection", "block_code": "C5", "kit_status": "None", "carrier_impl": "U-GNSS + ANT-G", "rag_source_id": "ccli-prototype-bom"},
    {"name": "LTE", "source": "Modem", "destination": "Mobile network", "protocol": "PPP/QMI", "physical": "SMA + SIM", "isolation": "Logical separation from Eth_A", "block_code": "C7", "kit_status": "Optional", "carrier_impl": "U-LTE rev A", "rag_source_id": "ccli-prototype-bom"},
    {"name": "USB", "source": "Service laptop", "destination": "SoM USB-C/A", "protocol": "Commissioning console", "physical": "USB", "isolation": "Out of certified control path", "block_code": "C8", "kit_status": "HAVE kit USB", "carrier_impl": "Product service port", "rag_source_id": "ccli-toradex-verdin-devboard"},
    {"name": "CAN", "source": "—", "destination": "—", "protocol": "Not in CEI CCI minimum", "physical": "DB9 on kit", "isolation": "N/A product", "block_code": "KIT", "kit_status": "2x CAN on kit", "carrier_impl": "Not required", "rag_source_id": "ccli-toradex-verdin-devboard", "required": 0},
]

BOM_CANDIDATES = [
    {"ref": "U-SOM", "block_code": "SoM", "description": "Verdin iMX8M Plus Quad 4GB IT SoM", "manufacturer": "Toradex", "part_number": "Verdin Plus IT", "pn_locked": "0063", "qty": "1", "est_eur": "150-350", "doc_status": "HAVE/INGESTED", "layer": "Product", "rag_source_id": "ccli-toradex-verdin-plus-som"},
    {"ref": "KIT", "block_code": "KIT", "description": "Verdin Development Board + HDMI", "manufacturer": "Toradex", "part_number": "99991105", "pn_locked": "99991105", "qty": "1", "est_eur": "~300", "doc_status": "HAVE/INGESTED", "layer": "Lab only", "rag_source_id": "ccli-toradex-verdin-devboard"},
    {"ref": "SW1", "block_code": "C1", "description": "4-port switch or 2x PHY + dual MAC", "manufacturer": "TBD", "part_number": "TBD", "pn_locked": "—", "qty": "1", "est_eur": "8-20", "doc_status": "MISSING", "layer": "Carrier", "rag_source_id": "ccli-prototype-bom"},
    {"ref": "J-ETH", "block_code": "C1", "description": "RJ45 magjack x4 Eth_A/B/Plant", "manufacturer": "TBD", "part_number": "TBD", "pn_locked": "—", "qty": "4", "est_eur": "4-10", "doc_status": "MISSING", "layer": "Carrier", "rag_source_id": "ccli-prototype-bom"},
    {"ref": "U-485", "block_code": "C2", "description": "Opto-isolated RS-485 transceiver", "manufacturer": "TBD", "part_number": "TBD", "pn_locked": "—", "qty": "2", "est_eur": "10-15", "doc_status": "MISSING", "layer": "Carrier", "rag_source_id": "ccli-prototype-bom"},
    {"ref": "J-SER", "block_code": "C2", "description": "RJ45 Modbus plant serial", "manufacturer": "TBD", "part_number": "TBD", "pn_locked": "—", "qty": "2", "est_eur": "2-4", "doc_status": "MISSING", "layer": "Carrier", "rag_source_id": "ccli-prototype-bom"},
    {"ref": "U-DI", "block_code": "C4", "description": "DI conditioner 10-120 Vdc x5", "manufacturer": "TBD", "part_number": "TBD", "pn_locked": "—", "qty": "5", "est_eur": "8-15", "doc_status": "MISSING", "layer": "Carrier", "rag_source_id": "ccli-prototype-bom"},
    {"ref": "K-DO", "block_code": "C4", "description": "Relay DO dry contact x3", "manufacturer": "TBD", "part_number": "TBD", "pn_locked": "—", "qty": "3", "est_eur": "5-12", "doc_status": "MISSING", "layer": "Carrier", "rag_source_id": "ccli-prototype-bom"},
    {"ref": "U-GNSS", "block_code": "C5", "description": "GNSS module u-blox-class", "manufacturer": "TBD", "part_number": "TBD", "pn_locked": "—", "qty": "1", "est_eur": "20-35", "doc_status": "MISSING", "layer": "Carrier", "rag_source_id": "ccli-prototype-bom"},
    {"ref": "ANT-G", "block_code": "C5", "description": "Active GNSS antenna SMA", "manufacturer": "TBD", "part_number": "TBD", "pn_locked": "—", "qty": "1", "est_eur": "5-15", "doc_status": "MISSING", "layer": "Carrier", "rag_source_id": "ccli-prototype-bom"},
    {"ref": "U-LTE", "block_code": "C7", "description": "LTE Cat.1 Quectel-class", "manufacturer": "TBD", "part_number": "TBD", "pn_locked": "—", "qty": "0-1", "est_eur": "25-40", "doc_status": "PARTIAL", "layer": "Optional", "rag_source_id": "ccli-prototype-bom"},
    {"ref": "U-SE", "block_code": "C6", "description": "Secure element ATECC608B/STSAFE", "manufacturer": "TBD", "part_number": "TBD", "pn_locked": "—", "qty": "0-1", "est_eur": "2-5", "doc_status": "TBD", "layer": "Carrier", "rag_source_id": "ccli-prototype-bom"},
    {"ref": "SEC-BOOT", "block_code": "C6", "description": "Secure boot + TPM (TG-524)", "manufacturer": "TesPro", "part_number": "TG-524", "pn_locked": "TG-524", "qty": "1", "est_eur": "—", "doc_status": "PARTIAL", "layer": "Lab DUT", "rag_source_id": "ccli-tg500-lab-platform"},
    {"ref": "U-PWR", "block_code": "C10", "description": "12-24V isolated DC-DC", "manufacturer": "TBD", "part_number": "TBD", "pn_locked": "—", "qty": "1", "est_eur": "10-25", "doc_status": "MISSING", "layer": "Carrier", "rag_source_id": "ccli-prototype-bom"},
    {"ref": "PCB", "block_code": "C9", "description": "4-layer carrier proto x5-10", "manufacturer": "JLCPCB/PCBWay", "part_number": "—", "pn_locked": "—", "qty": "1", "est_eur": "50-150", "doc_status": "MISSING", "layer": "Carrier", "rag_source_id": "ccli-prototype-bom"},
    {"ref": "ENC", "block_code": "C9", "description": "DIN-rail enclosure", "manufacturer": "TBD", "part_number": "TBD", "pn_locked": "—", "qty": "1", "est_eur": "15-40", "doc_status": "MISSING", "layer": "Product", "rag_source_id": "ccli-prototype-bom"},
    {"ref": "SW-61850", "block_code": "C3", "description": "libiec61850 (prototype GPLv3)", "manufacturer": "MZ Automation", "part_number": "—", "pn_locked": "—", "qty": "1", "est_eur": "0+license", "doc_status": "HAVE ref", "layer": "Software", "rag_source_id": "ccli-project-roadmap"},
]

KNOWLEDGE_AREAS = [
    {"name": "IEC61850", "current_level": "Low", "target_level": "High", "priority": "Critical", "status": "Learning"},
    {"name": "Carrier Board Design", "current_level": "Medium", "target_level": "High", "priority": "High", "status": "Learning"},
    {"name": "Secure Boot", "current_level": "Low", "target_level": "High", "priority": "Critical", "status": "Learning"},
    {"name": "Ethernet Architecture", "current_level": "Low", "target_level": "High", "priority": "Critical", "status": "Learning"},
    {"name": "Power Electronics", "current_level": "Medium", "target_level": "High", "priority": "Medium", "status": "Learning"},
    {"name": "GNSS", "current_level": "Low", "target_level": "Medium", "priority": "Medium", "status": "Learning"},
    {"name": "RS485 / Modbus", "current_level": "Medium", "target_level": "High", "priority": "High", "status": "Learning"},
    {"name": "PF2 Validation", "current_level": "Medium", "target_level": "High", "priority": "Critical", "status": "Learning"},
    {"name": "ATEX / Regulatory", "current_level": "Low", "target_level": "Medium", "priority": "Medium", "status": "Learning"},
]

DOCUMENT_GATES = [
    {"gate_id": "D0", "title": "Architecture block diagram", "current_status": "HAVE mermaid/Architecture/", "required_artifact": "Frozen functional view", "gap": "Update draw.io", "priority": "P1", "action": "Review generated diagrams", "rag_source_id": "ccli-architecture-framework"},
    {"gate_id": "D1", "title": "Project roadmap", "current_status": "HAVE/INGESTED", "required_artifact": "Budget envelope", "gap": "None", "priority": "—", "action": "—", "rag_source_id": "ccli-project-roadmap"},
    {"gate_id": "D2", "title": "Vendor selection C1-C10", "current_status": "HAVE/INGESTED", "required_artifact": "Scorecard filled", "gap": "Variscite/Compulab prices", "priority": "P2", "action": "Order kit 99991105+0063", "rag_source_id": "ccli-vendor-selection"},
    {"gate_id": "D3", "title": "Pin budget / connector map", "current_status": "MISSING", "required_artifact": "Every ball→net→connector", "gap": "BLOCKER", "priority": "P0", "action": "Author CCI_Pin_Budget.md", "rag_source_id": "ccli-pin-budget"},
    {"gate_id": "D4", "title": "Carrier block diagram", "current_status": "MISSING", "required_artifact": "Altium-ready freeze", "gap": "BLOCKER", "priority": "P0", "action": "Export from Hardware_Architecture.drawio", "rag_source_id": "ccli-carrier-block"},
    {"gate_id": "D5", "title": "Verdin Plus IT SoM DS", "current_status": "HAVE/INGESTED", "required_artifact": "PN 0063 locked", "gap": "None", "priority": "—", "action": "—", "rag_source_id": "ccli-toradex-verdin-plus-som"},
    {"gate_id": "D5b", "title": "NXP IMX8MPIEC", "current_status": "MISSING", "required_artifact": "Industrial Ethernet silicon DS", "gap": "PARTIAL", "priority": "P0", "action": "Download from NXP", "rag_source_id": "ccli-nxp-imx8m-plus-iec"},
    {"gate_id": "D6", "title": "Toradex PCN longevity 0063", "current_status": "MISSING", "required_artifact": "10-year letter", "gap": "BLOCKER", "priority": "P0", "action": "Request from Toradex", "rag_source_id": "ccli-toradex-verdin-plus-som"},
    {"gate_id": "D8", "title": "Modbus analyzer register map", "current_status": "MISSING", "required_artifact": "Real analyzer model", "gap": "Software block", "priority": "P1", "action": "Pick analyzer L3", "rag_source_id": "ccli-modbus-analyzer-map"},
    {"gate_id": "D12", "title": "Ethernet switch/PHY DS", "current_status": "MISSING", "required_artifact": "MPN for C1", "gap": "Carrier BOM", "priority": "P1", "action": "Select after pin budget", "rag_source_id": "ccli-prototype-bom"},
    {"gate_id": "D13", "title": "RS485 transceiver DS", "current_status": "MISSING", "required_artifact": "2x isolated MPN", "gap": "Carrier BOM", "priority": "P1", "action": "Select after pin budget", "rag_source_id": "ccli-prototype-bom"},
    {"gate_id": "D-LIC", "title": "61850 commercial license", "current_status": "GPLv3 prototype", "required_artifact": "MZ commercial ship license", "gap": "Budget", "priority": "P1", "action": "Quote before product", "rag_source_id": "ccli-project-roadmap"},
    {"gate_id": "D-CERT", "title": "62443/61850 lab scope", "current_status": "ATEX NB reuse", "required_artifact": "Accredited test quote", "gap": "Cert track", "priority": "P2", "action": "Early lab engagement", "rag_source_id": "ccli-cci-module-regs-class"},
    {"gate_id": "D-SK0146", "title": "SK0146 schematic RAG corpus", "current_status": "15 MD on disk", "required_artifact": "Ingested + queryable", "gap": "LEAK P0", "priority": "P0", "action": "ingest-sk0146-schematics.ps1", "rag_source_id": "sk0146-schematic-index"},
]

RISKS = [
    {"risk_id": "R-ISO-001", "category": "Network", "description": "Eth_A/Eth_B not physically/logically isolated", "impact": "Grid rejection / audit fail", "likelihood": "M", "mitigation": "Dual PHY or managed switch; strict VLAN; no shared L2 bridge", "owner": "HW", "rag_source_id": "cei-0-16-allegato-o"},
    {"risk_id": "R-PF2-001", "category": "Control", "description": "Stale meter data drives curtailment", "impact": "Wrong plant state / safety", "likelihood": "M", "mitigation": "Timestamp+validity; timeout→safe state; hysteresis", "owner": "FW", "rag_source_id": "ccli-validation-strategy"},
    {"risk_id": "R-SEC-001", "category": "Cyber", "description": "Private keys in filesystem", "impact": "62351/62443 assessment fail", "likelihood": "H", "mitigation": "Secure element; signed images; RBAC on console", "owner": "FW", "rag_source_id": "ccli-architecture-framework"},
    {"risk_id": "R-BOM-001", "category": "Process", "description": "Kit 99991105 mistaken for field CCI", "impact": "Wrong procurement / false validation", "likelihood": "M", "mitigation": "Label all docs: kit=lab carrier=product", "owner": "PM", "rag_source_id": "ccli-vendor-selection"},
    {"risk_id": "R-SCH-001", "category": "Schedule", "description": "Schematic before pin budget D3", "impact": "Respins / delay", "likelihood": "H", "mitigation": "Complete D3+D4 before Gerbers", "owner": "HW", "rag_source_id": "ccli-prototype-bom"},
    {"risk_id": "R-LIC-001", "category": "Legal", "description": "Ship product on GPLv3 libiec61850", "impact": "License violation", "likelihood": "H", "mitigation": "Budget MZ commercial license from day 1", "owner": "PM", "rag_source_id": "ccli-project-roadmap"},
    {"risk_id": "R-EMC-001", "category": "EMC", "description": "RS485/CAN on kit confused with product C2", "impact": "Wrong EMC test scope", "likelihood": "M", "mitigation": "Re-test on carrier with opto RJ45", "owner": "HW", "rag_source_id": "ccli-vendor-selection"},
    {"risk_id": "R-TIME-001", "category": "Time", "description": "No GNSS on kit; events untimestamped in lab", "impact": "False confidence in L2", "likelihood": "M", "mitigation": "Mark L2 time tests as stub until C5 carrier", "owner": "FW", "rag_source_id": "ccli-validation-strategy"},
]

VERIFICATION_CASES = [
    {"req_id": "REQ-NET-001", "case_id": "VC-NET-001", "description": "Eth_A MMS server", "test_method": "L2: libiec61850 client on isolated NIC", "level": "L2", "pass_criteria": "Read/write logical nodes on Eth_A only", "bench_ref": "Wave A switch", "status": "Planned"},
    {"req_id": "REQ-NET-002", "case_id": "VC-NET-002", "description": "Eth_B operator channel", "test_method": "60870/61850 client on second GbE", "level": "L2", "pass_criteria": "No crosstalk from Eth_A under VLAN test", "bench_ref": "Wave A", "status": "Planned"},
    {"req_id": "REQ-SER-001", "case_id": "VC-SER-001", "description": "Modbus RTU poll analyzer", "test_method": "USB-RS485 + pymodbus / real analyzer", "level": "L2/L3", "pass_criteria": "Register map matches DS", "bench_ref": "Wave A/C", "status": "Planned"},
    {"req_id": "REQ-IO-001", "case_id": "VC-IO-001", "description": "DI 10-120V sense", "test_method": "24V DI test box injection", "level": "L2", "pass_criteria": "All channels debounced; no false PF2", "bench_ref": "Wave B", "status": "Planned"},
    {"req_id": "REQ-IO-002", "case_id": "VC-IO-002", "description": "DO curtailment relay", "test_method": "Simulated DSO command toggles DO", "level": "L2", "pass_criteria": "Lamp/relay within timing budget", "bench_ref": "Wave B", "status": "Planned"},
    {"req_id": "REQ-PF2-001", "case_id": "VC-PF2-001", "description": "Comms loss safe state", "test_method": "Failure matrix: meter/Eth/GNSS stale", "level": "L2", "pass_criteria": "Defined safe state; no unsafe curtailment", "bench_ref": "Validation strategy §9", "status": "Planned"},
    {"req_id": "REQ-TIME-001", "case_id": "VC-TIME-001", "description": "GNSS timestamp", "test_method": "PPS/NMEA capture on events", "level": "L3", "pass_criteria": "Sub-second event ordering", "bench_ref": "Wave C + carrier", "status": "Deferred"},
    {"req_id": "REQ-SEC-001", "case_id": "VC-SEC-001", "description": "Secure boot chain", "test_method": "Signed vs unsigned image boot", "level": "Lab", "pass_criteria": "Unsigned rejected; rollback policy", "bench_ref": "Toradex signed-boot repo", "status": "Planned"},
    {"req_id": "REQ-PWR-001", "case_id": "VC-PWR-001", "description": "Input 12-24V operation", "test_method": "Bench PSU sweep", "level": "L2", "pass_criteria": "SoM boots across range; no rail collapse", "bench_ref": "Wave A PSU", "status": "Planned"},
]

ARCHITECTURE_DECISIONS = [
    {
        "decision_id": "ADR-001",
        "title": "Dual Ethernet isolation",
        "decision": "Eth_A (DSO) and Eth_B (Operator) use separate PHY ports or managed switch with strict VLAN — no L2 bridge.",
        "rationale": "CEI 0-16 Annex O/T requires outside interface separation.",
        "alternatives": "Single PHY + VLAN only (rejected — audit risk)",
        "status": "Accepted",
        "links": [("requirement", "REQ-NET-001"), ("requirement", "REQ-NET-002"), ("interface", "Eth_A"), ("interface", "Eth_B"), ("risk", "R-ISO-001")],
    },
    {
        "decision_id": "ADR-002",
        "title": "Eval kit is lab-only",
        "decision": "Toradex Dev Board 99991105 validates software path only; never ship as field CCI.",
        "rationale": "Kit RS485/CAN/DI are not CEI-compliant product interfaces.",
        "alternatives": "Use kit as production DUT (rejected)",
        "status": "Accepted",
        "links": [("hardware_block", "KIT"), ("risk", "R-BOM-001"), ("risk", "R-EMC-001")],
    },
    {
        "decision_id": "ADR-003",
        "title": "SK0146 DIN test ≠ CCI DI box",
        "decision": "SK0146 FCT uses 3.3 V contact simulation; CCI Wave B uses 10–120 V opto DI — separate fixtures and pass criteria.",
        "rationale": "Methodology gap identified in SK0146 knowledge matrix.",
        "alternatives": "Reuse SK0146 short-to-GND test for CCI (rejected)",
        "status": "Accepted",
        "links": [("knowledge_gap", "KL-003"), ("requirement", "REQ-IO-001")],
    },
    {
        "decision_id": "ADR-004",
        "title": "Pin budget before schematic",
        "decision": "Complete document gates D3 (pin budget) and D4 (carrier block) before carrier Gerbers.",
        "rationale": "Prevents respins and schedule slip.",
        "alternatives": "Schematic-first (rejected)",
        "status": "Accepted",
        "links": [("document_gate", "D3"), ("document_gate", "D4"), ("risk", "R-SCH-001")],
    },
    {
        "decision_id": "ADR-005",
        "title": "C1 Ethernet topology unresolved — Architecture Freeze blocker",
        "decision": "Do not select SW1/PHY MPNs until ADR chooses: (A) industrial managed 4-port switch with hard port isolation, or (B) SoM dual GbE + 2× external PHY for Plant ports. VLAN-only shared L2 is rejected for Eth_A/Eth_B.",
        "rationale": "DRB CCLI-DRB-001 — Eth_A/Eth_B Critical; GOOSE and CEI outside-interface separation require physical/port policy clarity.",
        "alternatives": "Unmanaged switch + VLAN (rejected for product)",
        "status": "Proposed",
        "links": [
            ("requirement", "REQ-NET-001"),
            ("requirement", "REQ-NET-002"),
            ("hardware_block", "C1"),
            ("knowledge_risk", "KR-007"),
            ("risk", "R-ISO-001"),
        ],
    },
]

# Requirement → block traceability (req_id, block_code, notes)
REQ_TRACEABILITY = [
    ("REQ-REG-001", "C3", "Protocol compliance stack"),
    ("REQ-NET-001", "C1", "Eth_A DSO path"),
    ("REQ-NET-002", "C1", "Eth_B operator path"),
    ("REQ-NET-003", "C1", "Plant ports"),
    ("REQ-SER-001", "C2", "Modbus RTU"),
    ("REQ-IO-001", "C4", "DI conditioner"),
    ("REQ-IO-002", "C4", "DO relay PF2"),
    ("REQ-PWR-001", "C10", "Wide-range PSU"),
    ("REQ-SEC-001", "C6", "HAB on SoM"),
    ("REQ-SEC-002", "C6", "Secure element"),
    ("REQ-TIME-001", "C5", "GNSS module"),
    ("REQ-LTE-001", "C7", "Optional modem"),
    ("REQ-PF2-001", "C3", "PF2 logic"),
    ("REQ-PF2-001", "C4", "Curtailment DO"),
    ("REQ-PF2-001", "C1", "DSO command path"),
]

# Requirement → interface (req_id, interface name)
REQ_INTERFACES = [
    ("REQ-NET-001", "Eth_A"),
    ("REQ-NET-002", "Eth_B"),
    ("REQ-NET-003", "Plant-1"),
    ("REQ-NET-003", "Plant-2"),
    ("REQ-SER-001", "RS485-A"),
    ("REQ-SER-001", "RS485-B"),
    ("REQ-IO-001", "DI-1..5"),
    ("REQ-IO-002", "DO-1..3"),
    ("REQ-TIME-001", "GNSS"),
    ("REQ-LTE-001", "LTE"),
]

# Requirement → risk
REQ_RISKS = [
    ("REQ-NET-001", "R-ISO-001"),
    ("REQ-NET-002", "R-ISO-001"),
    ("REQ-PF2-001", "R-PF2-001"),
    ("REQ-SEC-001", "R-SEC-001"),
    ("REQ-SEC-002", "R-SEC-001"),
    ("REQ-TIME-001", "R-TIME-001"),
]

# SK0146 knowledge leak rows (subset — full set from build-sk0146-knowledge-matrix)
SK0146_KNOWLEDGE_GAPS = [
    {"gap_id": "KL-001", "domain": "Corpus / RAG", "area": "RS485 / Modbus", "gap_description": "SK0146 schematic corpus not ingested to RAG", "have_now": "PARTIAL — markdown on disk only", "required_knowledge": "14 SchDoc sheets + schematic-index", "leak_type": "RAG_NOT_INGESTED", "priority": "P0", "action": "Run ingest-sk0146-schematics.ps1", "rag_source_id": "sk0146-schematic-index", "sk0146_bench_need": "Query TP nets during FCT debug", "ccli_pf2_need": "Reuse FCT patterns for carrier bring-up", "atex_note": "Product zone 0 — bench is safe-area lab"},
    {"gap_id": "KL-003", "domain": "Field DI", "area": "RS485 / Modbus", "gap_description": "SK0146 DIN contact sim vs CCI 10-120V DI methodology", "have_now": "HAVE — different physics", "required_knowledge": "DZ clamps; M4/M9 terminals", "leak_type": "METHODOLOGY_GAP", "priority": "P1", "action": "Document crosswalk — separate pass criteria", "rag_source_id": "sk0146-blk_ref_a_digital_in", "block_code": "C4", "sk0146_bench_need": "TP18/21/11/14 short to GND", "ccli_pf2_need": "Wave B DI box 24V first", "atex_note": "Do not reuse voltage limits"},
    {"gap_id": "KL-005", "domain": "Fixture mapping", "area": "Carrier Board Design", "gap_description": "VS channels TBD for TP57, TP4; keypad flying leads", "have_now": "PARTIAL — fixture_map.json", "required_knowledge": "Complete VS assignment on drawing", "leak_type": "FIXTURE_TBD", "priority": "P0", "action": "Close TBD rows in FIXTURE_TEST_PLAN.md", "rag_source_id": None, "sk0146_bench_need": "VS01–VS16 ADC map", "ccli_pf2_need": "N/A — different DUT", "atex_note": "Zone 0 bench — normal ESD lab"},
    {"gap_id": "KL-015", "domain": "ATEX / regulatory", "area": "ATEX / Regulatory", "gap_description": "SK0146 zone 0 ref vs CCI Ex marking scope", "have_now": "PARTIAL — ccli-cci-module-regs-class INGESTED", "required_knowledge": "IEC 60079-0 safe area vs product Ex marking", "leak_type": "SCOPE_CLARIFICATION", "priority": "P1", "action": "Bench SOP: safe-area only", "rag_source_id": "ccli-cci-module-regs-class", "sk0146_bench_need": "reference_zone_level: 0", "ccli_pf2_need": "Final Ex marking TBD", "atex_note": "Bench ATEX lvl 0 ≠ CCI final zone"},
    {"gap_id": "KL-018", "domain": "Pin budget", "area": "Carrier Board Design", "gap_description": "CCI pin budget not authored (D3 blocker)", "have_now": "MISSING for CCI", "required_knowledge": "Verdin ball→net→connector", "leak_type": "BLOCKER", "priority": "P0", "action": "Author CCI_Pin_Budget.md", "rag_source_id": "ccli-pin-budget", "block_code": "C1", "sk0146_bench_need": "STM32 net→TP map (fixture only)", "ccli_pf2_need": "D3 pin budget freeze", "atex_note": "N/A"},
]
