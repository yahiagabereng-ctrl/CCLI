export type BranchStatus = "ok" | "partial" | "gap";

export interface KbBranch {
  name: string;
  summary: string;
  status: BranchStatus;
  statusLabel: string;
}

export const KB_BRANCHES: KbBranch[] = [
  {
    name: "Application Source",
    summary:
      "C/C++ control path on OpenWrt (TG-524) and Pi mock. SK0146 AT syntax ingested; IEC 61850 / Modbus / PF2 apps not started.",
    status: "partial",
    statusLabel: "Design only",
  },
  {
    name: "Protocol Documentation",
    summary:
      "CEI 0-16 Annex O/T, AiLux reference manual, EU RED II & CRA, bilingual regulations classification. libiec61850 / lib60870 refs on disk.",
    status: "partial",
    statusLabel: "Core ready",
  },
  {
    name: "Engineering Knowledge",
    summary:
      "TG-524 lab platform freeze, roadmap, validation ladder, mocking bench BOM, architecture framework.",
    status: "ok",
    statusLabel: "Strong",
  },
];

/** Project roadmap — system structure (from CCLI-ROADMAP / CCI_Project_Roadmap) */
export const ROADMAP_META = {
  target: "CEI 0-16 Annex O/T/M compliant Central Plant Controller (CCI)",
  range: "Flexible 100 kW–6 MW MT plant range",
  scope: "Original hardware/firmware design — not a clone of any commercial product",
  prototypeBudget: "~€1,000 single-unit BOM + fab + assembly",
  phase1: "Working prototype in ~10–12 weeks",
  phase2: "Grid/cyber certification separate track · 9–18 months · €45k–110k+ (ATEX already held)",
};

export const SYSTEM_BLOCKS: { block: string; function: string; note: string }[] = [
  {
    block: "TG-524 lab gateway",
    function: "OpenWrt, IEC 61850, Modbus, PF2 logic",
    note: "TesPro TG-500 series — mock on Raspberry Pi until hardware arrives",
  },
  {
    block: "Ethernet (≥4 ports)",
    function: "Eth_A → DSO · Eth_B → Qualified Operator · 2× plant",
    note: "Isolation required — dual MAC/PHY or managed switch on carrier",
  },
  {
    block: "Serial ×2 (isolated)",
    function: "RS-485/232 Modbus RTU to energy analyzer",
    note: "IEC 60870-5-101 optional on same ports",
  },
  {
    block: "GNSS + optional LTE",
    function: "Time sync for measurements/events; remote backup",
    note: "u-blox-class GNSS; LTE Cat-1 optional on rev A",
  },
  {
    block: "DI / DO",
    function: "DI 10–120 Vdc status · DO dry-contact curtailment",
    note: "Critical PF2 path — field conditioning on carrier",
  },
  {
    block: "Secure boot + SE",
    function: "Root of trust, keys, signed firmware",
    note: "HAB/TrustZone + external secure element recommended",
  },
  {
    block: "Local console",
    function: "USB commissioning / diagnostics",
    note: "Outside certified control-path boundary",
  },
  {
    block: "Power",
    function: "12–24 Vdc wide-range isolated DC-DC",
    note: "Cabinet UPS / battery context",
  },
];

export const PHASE1_TIMELINE: { weeks: string; milestone: string }[] = [
  { weeks: "1–2", milestone: "Architecture freeze, SoM selection, schematic start" },
  { weeks: "2–4", milestone: "Schematic complete, design review, long-lead parts" },
  { weeks: "4–6", milestone: "PCB layout, DFM, Gerbers to fab" },
  { weeks: "6–8", milestone: "Fab + assembly turnaround" },
  { weeks: "8–10", milestone: "Bring-up: power, boot, Ethernet ×4, RS-485, GNSS, DI/DO" },
  { weeks: "10–12", milestone: "Firmware: 61850 MMS on Eth_A, Modbus master, DO curtailment stub" },
];

export const FIRMWARE_PILLARS: { title: string; body: string }[] = [
  {
    title: "OS / BSP",
    body: "OpenWrt on TesPro TG-524 (lab); cross-compile with vendor SDK. Raspberry Pi uses native Linux for mock.",
  },
  {
    title: "Certified control path",
    body: "C/C++ — IEC 61850 server, Modbus master, PF2 curtailment, GOOSE timing. Auditors prefer analyzable static code.",
  },
  {
    title: "Tooling path",
    body: "Python/web console for commissioning and logs — kept outside the certified boundary from day one.",
  },
  {
    title: "IEC 61850 stack",
    body: "libiec61850 for prototype (GPLv3). Commercial license from MZ Automation required to ship a closed product — free to prototype, pay to ship.",
  },
];

export const NEXT_STEPS = [
  "Order TesPro TG-524 and confirm SKU (TG-424 vs TG-524) with vendor.",
  "Request OpenWrt SDK / cross-compile toolchain from TesPro.",
  "Request IEC 62443 / 61850 lab scoping quote before schematic freeze.",
];

export const CORPUS_HIGHLIGHTS = [
  {
    title: "Grid mandate",
    body: "CEI 0-16 Allegati O–T, AiLux behaviour reference, regulations classification (IT+EN).",
  },
  {
    title: "Compute & lab",
    body: "TesPro TG-524 OpenWrt gateway freeze, Raspberry Pi mock path, protocol library refs.",
  },
  {
    title: "Cyber & EU law",
    body: "CRA 2024/2847, RED II 2018/2001; IEC 62351/62443 acquisition backlog mapped explicitly.",
  },
];

export interface PdfInventoryEntry {
  category: string;
  brief: string;
}

/** One-line labels for PDF inventory (Chapter 10). Keyed by filename. */
export const PDF_INVENTORY_LOOKUP: Record<string, PdfInventoryEntry> = {
  "CCI_Clone_RE_Option.pdf": {
    category: "Programme",
    brief: "Option B — gated clone / reverse-engineering path and constraints.",
  },
  "CCI_Project_Roadmap.pdf": {
    category: "Programme",
    brief: "Master delivery roadmap, phases, and Architecture Freeze gates.",
  },
  "CCI_SoM_Comparison.pdf": {
    category: "Programme",
    brief: "SoM silicon shortlist scored against CCI controls and certification needs.",
  },
  "CELEX_32018L2001_EN_TXT.pdf": {
    category: "Regulations · EU",
    brief: "RED II — EU Directive 2018/2001 on radio equipment.",
  },
  "OJ_L_202402847_EN_TXT.pdf": {
    category: "Regulations · EU",
    brief: "EU Cyber Resilience Act — Regulation 2024/2847.",
  },
  "EstrattoAllegatoO.pdf": {
    category: "Regulations · CEI 0-16",
    brief: "Annex O extract — grid interface and DSO communication requirements.",
  },
  "EstrattoAllegatoT.pdf": {
    category: "Regulations · CEI 0-16",
    brief: "Annex T extract — telemetry, monitoring, and plant communication.",
  },
  "Manuale_CCI_I_24_R6_240610.pdf": {
    category: "Regulations · CEI 0-16",
    brief: "Official CCI manual — AiLux-class reference behaviour and interfaces.",
  },
  "TG-500_Series_Datasheet.pdf": {
    category: "Engineering",
    brief: "TesPro TG-500 series datasheet — lab gateway TG-524/TG-525 (OpenWrt, MT798X).",
  },
  "TesPro_Gateway_Specifications_Response_Form.pdf": {
    category: "Engineering",
    brief: "TesPro gateway spec response — WAN/LAN isolation, RS485, TPM, OpenWrt.",
  },
  "IMX8MPCEC.pdf": {
    category: "Reference",
    brief: "NXP i.MX 8M Plus CEC SoC datasheet — silicon capabilities and peripherals.",
  },

  "IMX8MDQLQCEC-1280316.pdf": {
    category: "Reference",
    brief: "NXP i.MX 8M D/Q/LQ SoC reference — silicon capabilities and peripherals.",
  },
  "STM32L151RCTx.pdf": {
    category: "Reference",
    brief: "STM32L151 ultra-low-power MCU datasheet — auxiliary / companion MCU patterns.",
  },
  "iGate-850_V10_UM_EN.pdf": {
    category: "Reference",
    brief: "Industrial gateway user manual — I/O and field communication reference patterns.",
  },
  "moxa-mgate-5119-series-datasheet-v1.2.pdf": {
    category: "Reference",
    brief: "Moxa MGate 5119 Modbus/Ethernet gateway — plant serial interface reference.",
  },
};

/** Short folder label from Telematry documents relative path. */
export function pdfShortFolder(path: string): string {
  const parts = path.split("/").filter(Boolean);
  if (parts[0] === "regulations") {
    if (parts[1] === "eu") return "Regulations · EU";
    if (parts[1] === "cei-0-16") return "Regulations · CEI 0-16";
    return "Regulations";
  }
  if (parts.includes("cei-0-16")) return "Regulations · CEI 0-16";
  if (parts.includes("reference")) return "Reference";
  if (parts.includes("cei-0-16")) return "Regulations · CEI 0-16";
  if (parts.includes("reference")) return "Reference";
  if (parts[0] === "projects" && parts[1] === "ccli") {
    const folder = parts[2];
    if (folder === "engineering") return "Engineering";
    if (folder === "reference") return "Reference";
    if (folder === "protocols") return "Protocols";
    if (!folder || folder.endsWith(".pdf")) return "Programme";
    return folder.charAt(0).toUpperCase() + folder.slice(1);
  }
  return parts.slice(0, 2).join(" / ") || path;
}

export function pdfInventoryMeta(
  name: string,
  path: string,
): PdfInventoryEntry & { folder: string } {
  const entry = PDF_INVENTORY_LOOKUP[name];
  const folder = pdfShortFolder(path);
  return {
    folder,
    category: entry?.category ?? folder,
    brief: entry?.brief ?? "Ingested PDF — add entry to PDF_INVENTORY_LOOKUP in kbData.ts.",
  };
}

/** Default RAG focus for engineering chat */
export const CCLI_SOURCE_PACK = [
  "ccli-knowledge-base-brief",
  "ccli-vendor-selection",
  "ccli-vendor-decision-scorecard",
  "ccli-prototype-bom",
  "ccli-validation-strategy",
  "ccli-mocking-bench-bom",
  "ccli-rag-knowledge-base",
  "ccli-architecture-framework",
  "ccli-industrial-rag-arch",
  "ccli-project-roadmap",
  "ccli-github-protocol-libs",
  "ccli-soc-freeze",
  "ccli-tg500-lab-platform",
  "ccli-tespro-tg500-ds",
  "ccli-tespro-tg424-response",
  "ccli-3m-italy-nofab-plan",
  "ccli-cci-module-regs-class",
  "cei-0-16-allegato-o",
  "cei-0-16-allegato-t",
  "cei-0-16-cci-manual-ailux",
  "eu-2024-2847-cra",
  "eu-2018-2001-red",
  "ccli-moxa-mgate-5119",
  "ccli-igate-850-manual",
  "ccli-clone-re-option",
];

export const SOURCE_PACKS: { id: string; label: string; sources: string[] }[] = [
  { id: "ccli", label: "Full CCI corpus", sources: CCLI_SOURCE_PACK },
  {
    id: "vendor-bom",
    label: "Vendor / BOM / lab",
    sources: [
      "ccli-tg500-lab-platform",
      "ccli-soc-freeze",
      "ccli-tespro-tg500-ds",
      "ccli-tespro-tg424-response",
      "ccli-prototype-bom",
      "ccli-industrial-rag-arch",
    ],
  },
  {
    id: "validation-lab",
    label: "Validation / HIL / PF2",
    sources: [
      "ccli-validation-strategy",
      "ccli-mocking-bench-bom",
      "ccli-rag-knowledge-base",
      "ccli-architecture-framework",
      "ccli-github-protocol-libs",
      "ccli-prototype-bom",
      "ccli-vendor-selection",
      "ccli-project-roadmap",
      "ccli-cci-module-regs-class",
      "ccli-tg500-lab-platform",
      "ccli-moxa-mgate-5119",
      "ccli-igate-850-manual",
      "cei-0-16-allegato-o",
      "cei-0-16-allegato-t",
    ],
  },
  {
    id: "grid",
    label: "Italian grid (Pack A)",
    sources: [
      "cei-0-16-allegato-o",
      "cei-0-16-allegato-t",
      "cei-0-16-cci-manual-ailux",
      "ccli-cci-module-regs-class",
    ],
  },
  {
    id: "openwrt-lab",
    label: "OpenWrt / TG-524 lab",
    sources: [
      "ccli-tg500-lab-platform",
      "ccli-tespro-tg500-ds",
      "ccli-tespro-tg424-response",
      "ccli-github-protocol-libs",
      "ccli-vendor-selection",
      "ccli-prototype-bom",
      "ccli-industrial-rag-arch",
    ],
  },
  {
    id: "cyber",
    label: "Cyber (partial)",
    sources: [
      "ccli-cci-module-regs-class",
      "eu-2024-2847-cra",
      "cei-0-16-cci-manual-ailux",
      "ccli-github-protocol-libs",
      "ccli-tg500-lab-platform",
    ],
  },
];

export const SUGGESTED_PROMPTS = [
  "Design the Ethernet architecture for the PF2 controller using the architecture framework.",
  "What does TesPro TG-524 cover vs miss for C1–C10 lab validation?",
  "Why is TG-524 the frozen lab platform instead of a custom SoM kit?",
  "Design the cheapest L1 PF2 bench and list its required test vectors.",
  "What does L2 RS-485 HIL validate on TG-524 vs Raspberry Pi mock?",
  "Give me the failure-injection matrix for meter, Ethernet, GNSS, and stale data.",
  "What should I buy this week for Wave A of the PF2 mocking bench BOM?",
  "Show the clustered document gate D0-D21 and what blocks Architecture Freeze.",
  "List protocol libraries to build for OpenWrt on TG-524.",
  "List datasheets still MISSING before lab bring-up from the prototype BOM.",
  "Summarize Eth_A vs Eth_B from CEI 0-16 Annex O/T.",
  "What OpenWrt SDK steps are needed to cross-compile libiec61850 for TG-524?",
];

export const GROUNDING_PROMPT = `You are the HiTEKS CCLI engineering assistant for an original CEI 0-16 Central Plant Controller (CCI) programme.

## Grounding rules
1. Answer ONLY from retrieved excerpts. Cite concrete PNs, source_ids, file names, and status (HAVE / INGESTED / PARTIAL / MISSING / TBD).
2. Never invent certification, scores, prices, or pinmux. If absent, say what document is missing and where it should be filed.
3. Always separate layers:
   - Lab DUT = TesPro TG-524 (OpenWrt) — mock on Raspberry Pi until hardware arrives
   - Field CCI = future custom industrial product — not the TG-500 gateway form factor
4. Lead Phase 1 path (when corpus supports it): TesPro **TG-524** on OpenWrt + Pi mock. Cross-compile with TesPro SDK when available.
5. Lab facts already validated in corpus when present: TG-524 has isolated WAN/LAN, 2× RS485, limited DI/DO — product still needs full C1/C2/C4/C5 on field hardware.
6. Validate-before-compare: do not score or prefer a vendor without datasheet evidence. Mark Compulab CL-SOM as BLOCKED until PDFs exist. IMX8MPIEC may still be MISSING.
7. Regulations: CEI 0-16 O/T and AiLux behaviour drive topology; kit CE marks / vendor "61850 certified" stickers are NOT required to choose a lab kit. Do not claim IEC 62443/62351 product certification unless the excerpt says so.
8. Licensing: libiec61850 is GPLv3 for prototype; commercial license required to ship a closed product.
9. Validation ladder: distinguish L1 software mocks, L2 physical-interface HIL, L3 real analyzer, and L4 power-system HIL. State what each level proves and does not prove. HIL and bench tests never replace accredited CEI 0-16 / IEC conformance testing.
10. Safety and control: never assume relay-open is safe, never use stale meter data as current, and flag 110 Vdc DI testing as hazardous. Require explicit hysteresis, debounce, timeout, command priority, and safe-state policies for PF2.
11. Format: short structured answer (bullets or tables). End with "Gaps / next docs" when relevant.
12. Prefer English; keep Italian normative names (Allegato O/T, Operatore Abilitato, Eth_A/Eth_B) when citing grid rules.
13. For hardware/system design questions, follow CCI_Architecture_Framework.md: Requirements → Architecture → Interface matrix → BOM matrix → Knowledge gaps → Risks → Verification.`;


