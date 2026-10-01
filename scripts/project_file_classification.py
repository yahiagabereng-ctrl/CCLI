"""Classification of CCLI repo-root and related project files for cleanup."""

from __future__ import annotations

# class: KEEP_SOURCE | KEEP_DELIVERABLE | DISPOSE_BUILD | DUPLICATE_OF_KB | RELOCATE_CANDIDATE | SCRATCH
# action: keep | delete | relocate | review

ROOT_CLASSIFICATION = [
    # --- Screenshot / LaTeX set ---
    {"path": "CCI_Architecture.mermaid", "class": "KEEP_SOURCE", "role": "Architecture diagram source", "action": "keep", "target": "Architecture/ or knowledge-base/08-engineering/"},
    {"path": "CCI_Architecture.png", "class": "KEEP_DELIVERABLE", "role": "Rendered architecture diagram", "action": "keep", "target": "—"},
    {"path": "CCI_Clone_RE_Option.tex", "class": "KEEP_SOURCE", "role": "LaTeX source — clone/RE study", "action": "keep", "target": "—"},
    {"path": "CCI_Clone_RE_Option.pdf", "class": "KEEP_DELIVERABLE", "role": "PDF deliverable", "action": "keep", "target": "—"},
    {"path": "CCI_Clone_RE_Option.aux", "class": "DISPOSE_BUILD", "role": "LaTeX aux", "action": "delete", "target": "—"},
    {"path": "CCI_Clone_RE_Option.log", "class": "DISPOSE_BUILD", "role": "LaTeX log", "action": "delete", "target": "—"},
    {"path": "CCI_Clone_RE_Option.out", "class": "DISPOSE_BUILD", "role": "LaTeX hyperref out", "action": "delete", "target": "—"},
    {"path": "CCI_Clone_RE_Option.toc", "class": "DISPOSE_BUILD", "role": "LaTeX TOC", "action": "delete", "target": "—"},
    {"path": "CCI_Project_Roadmap.md", "class": "KEEP_SOURCE", "role": "Roadmap markdown (RAG ccli-project-roadmap)", "action": "keep", "target": "Also mirror under knowledge-base if not already"},
    {"path": "CCI_Project_Roadmap.tex", "class": "KEEP_SOURCE", "role": "Roadmap LaTeX source", "action": "keep", "target": "—"},
    {"path": "CCI_Project_Roadmap.pdf", "class": "KEEP_DELIVERABLE", "role": "Roadmap PDF", "action": "keep", "target": "—"},
    {"path": "CCI_Project_Roadmap.aux", "class": "DISPOSE_BUILD", "role": "LaTeX aux", "action": "delete", "target": "—"},
    {"path": "CCI_Project_Roadmap.log", "class": "DISPOSE_BUILD", "role": "LaTeX log", "action": "delete", "target": "—"},
    {"path": "CCI_Project_Roadmap.out", "class": "DISPOSE_BUILD", "role": "LaTeX out", "action": "delete", "target": "—"},
    {"path": "CCI_Project_Roadmap.toc", "class": "DISPOSE_BUILD", "role": "LaTeX TOC", "action": "delete", "target": "—"},
    {"path": "CCI_SoM_Comparison.tex", "class": "KEEP_SOURCE", "role": "SoM comparison LaTeX", "action": "keep", "target": "—"},
    {"path": "CCI_SoM_Comparison.pdf", "class": "KEEP_DELIVERABLE", "role": "SoM comparison PDF", "action": "keep", "target": "—"},
    {"path": "CCI_SoM_Comparison.aux", "class": "DISPOSE_BUILD", "role": "LaTeX aux", "action": "delete", "target": "—"},
    {"path": "CCI_SoM_Comparison.log", "class": "DISPOSE_BUILD", "role": "LaTeX log", "action": "delete", "target": "—"},
    {"path": "CCI_SoM_Comparison.out", "class": "DISPOSE_BUILD", "role": "LaTeX out", "action": "delete", "target": "—"},
    {"path": "CCI_SoM_Comparison.toc", "class": "DISPOSE_BUILD", "role": "LaTeX TOC", "action": "delete", "target": "—"},
    {"path": "CCI_Knowledge_Base_Brief.md", "class": "KEEP_SOURCE", "role": "KB brief", "action": "keep", "target": "—"},
    {"path": "CCI_NXP_IMX8MDQLQCEC_Precision_Extract.md", "class": "KEEP_SOURCE", "role": "NXP DualLite precision extract", "action": "keep", "target": "knowledge-base/08-engineering/ preferred"},
    {"path": "_pe_inventory.csv", "class": "SCRATCH", "role": "Power electronics inventory scratch", "action": "review", "target": "Telematry PE course or delete if obsolete"},
    # --- Other root clutter ---
    {"path": "CCI_Yocto_BSP_Notes.md", "class": "DISPOSE_BUILD", "role": "Removed — legacy Yocto BSP notes", "action": "delete", "target": "—"},
    {"path": "DOWNLOADS.md", "class": "SCRATCH", "role": "Stub — real index is knowledge-base/DOWNLOADS.md", "action": "review", "target": "Delete stub or point to KB"},
    {"path": "Designer.jpg", "class": "SCRATCH", "role": "Unrelated image asset", "action": "review", "target": "website/ or remove"},
    {"path": "CELEX_32018L2001_EN_TXT.pdf", "class": "DUPLICATE_OF_KB", "role": "EU RED directive", "action": "delete", "target": "knowledge-base/07-protocols/"},
    {"path": "OJ_L_202402847_EN_TXT.pdf", "class": "DUPLICATE_OF_KB", "role": "EU CRA", "action": "delete", "target": "knowledge-base/07-protocols/"},
    {"path": "EstrattoAllegatoO.pdf", "class": "DUPLICATE_OF_KB", "role": "CEI Allegato O extract", "action": "delete", "target": "Telematry/KB regulations"},
    {"path": "EstrattoAllegatoT.pdf", "class": "DUPLICATE_OF_KB", "role": "CEI Allegato T extract", "action": "delete", "target": "Telematry/KB regulations"},
    {"path": "iGate-850_V10_UM_EN.pdf", "class": "DUPLICATE_OF_KB", "role": "Vendor UM", "action": "delete", "target": "knowledge-base reference"},
    {"path": "IMX8MDQLQCEC-1280316.pdf", "class": "DUPLICATE_OF_KB", "role": "NXP DualLite CEC", "action": "delete", "target": "knowledge-base/08-engineering/reference/"},
    {"path": "moxa-mgate-5119-series-datasheet-v1.2.pdf", "class": "DUPLICATE_OF_KB", "role": "Moxa datasheet", "action": "delete", "target": "knowledge-base reference"},
    {"path": "STM32L151RCTx.pdf", "class": "DUPLICATE_OF_KB", "role": "STM32 DS (SK0146-related)", "action": "delete", "target": "knowledge-base or Test Bench"},
    {"path": "IMX8MMIEC.pdf", "class": "RELOCATE_CANDIDATE", "role": "NXP i.MX8M Mini IEC — NOT Plus IEC (D5b)", "action": "relocate", "target": "knowledge-base/08-engineering/reference/"},
    {"path": "UG10164.pdf", "class": "DISPOSE_BUILD", "role": "Legacy NXP Yocto UG — removed from corpus", "action": "delete", "target": "—"},
    {"path": "Manuale CCI_I_24_R6_240610.pdf", "class": "RELOCATE_CANDIDATE", "role": "CEI Manuale CCI", "action": "relocate", "target": "knowledge-base/07-protocols/ or Telematry regulations"},
]
