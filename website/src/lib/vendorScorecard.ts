/** Phase 1 vendor decision matrix — scores for engineering workshop. */

export type CandidateId =
  | "toradex"
  | "variscite"
  | "compulab-som"
  | "compulab-iotgate"
  | "nxp";

export interface VendorCandidate {
  id: CandidateId;
  vendor: string;
  shortName: string;
  kit: string;
  som: string;
  role: "lead" | "alternate" | "study" | "silicon";
  roleLabel: string;
  /** Pre-computed 0–100 Phase 1 kit-decision score */
  score: number;
  verdict: string;
  caveats: string[];
}

export interface ScoreParam {
  id: string;
  label: string;
  weight: number;
  /** 0–10 per candidate; contribution = (score/10) × weight */
  scores: Record<CandidateId, number>;
  notes: Record<CandidateId, string>;
}

export type CellMark = "F" | "P" | "N" | "?" | "N/A" | "S";

export interface ControlRow {
  id: string;
  label: string;
  productRequired: boolean;
  kitOkPartial: boolean;
  marks: Record<CandidateId, CellMark>;
  note: string;
}

export interface RegulationRow {
  id: string;
  norm: string;
  classCode: string;
  forKitPick: "yes" | "nice" | "no";
  forProductShip: "yes" | "if-claimed" | "context";
  kbStatus: "ingested" | "partial" | "missing";
  needCertToChooseKit: boolean;
  note: string;
}

export const SCORE_METHOD =
  "Scores are post-validation (2026-07-25). Evidence checklist is above the rank cards — BLOCKED candidates must not drive a buy. Field CCI always needs a custom carrier. Kit CE / vendor 61850 certificates are not required to pick a lab kit.";

export const VENDOR_CANDIDATES: VendorCandidate[] = [
  {
    id: "toradex",
    vendor: "Toradex",
    shortName: "Toradex Verdin",
    kit: "Verdin Development Board + HDMI (99991105)",
    som: "Verdin iMX8M Plus Quad 4GB IT (0063)",
    role: "lead",
    roleLabel: "Lead path",
    score: 84,
    verdict: "Lead after Wave 1 ingest — SoM DS + kit DS in RAG. Order 99991105 + 0063; request PCN next.",
    caveats: [
      "SoM datasheet ingested (ccli-toradex-verdin-plus-som) — Quad 4GB IT, no Wi-Fi, HAB, optional TPM",
      "RS485 is 1× non-isolated DB9 — not CEI 2× opto RJ45",
      "Longevity / PCN letter still to request",
      "Carrier still required for C1 remainder, full C2, C4, C5, C10",
    ],
  },
  {
    id: "variscite",
    vendor: "Variscite",
    shortName: "Variscite Plus",
    kit: "VAR-DVK-DT8M-PLUS (DART + DT8MCustomBoard)",
    som: "DART-MX8M-PLUS (IT options — PN TBD)",
    role: "alternate",
    roleLabel: "Alternate",
    score: 67,
    verdict: "Alternate after Wave 1 ingest — dual GbE DS in RAG; lock IT assembly PN before buy.",
    caveats: [
      "DART + DT8MCustomBoard ingested (ccli-variscite-*)",
      "Exact IT assembly PN still open",
      "No opto RS-485 plant pair on kit — carrier still required",
    ],
  },
  {
    id: "nxp",
    vendor: "NXP",
    shortName: "NXP EVK",
    kit: "i.MX 8M Plus LPDDR4 EVK (8MPLUSLPD4-EVK)",
    som: "Bare silicon reference (not lab DUT)",
    role: "silicon",
    roleLabel: "Silicon / HAB",
    score: 60,
    verdict: "HAB + UG10164 validated — use beside a SoM kit, not as CCI compute alone.",
    caveats: [
      "Not a DIN-rail SoM product",
      "IMX8MPIEC industrial datasheet still MISSING",
      "All field I/O still on custom carrier",
    ],
  },
  {
    id: "compulab-som",
    vendor: "Compulab",
    shortName: "Compulab SoM",
    kit: "CL-SOM-iMX8 + SBC-iMX8 Evaluation Kit",
    som: "CL-SOM-iMX8 / MCM-iMX8M-Mini (confirm SKU)",
    role: "alternate",
    roleLabel: "Alternate",
    score: 42,
    verdict: "BLOCKED for buy — no kit/SoM PDFs in KB yet (score provisional only).",
    caveats: [
      "Zero datasheets validated on disk",
      "Ethernet count unvalidated",
      "Do not order until DS pack filed",
    ],
  },
  {
    id: "compulab-iotgate",
    vendor: "Compulab",
    shortName: "IOT-GATE",
    kit: "IOT-GATE-iMX8 Evaluation Kit",
    som: "UCM-iMX8M-Mini inside industrial gateway",
    role: "study",
    roleLabel: "I/O study only",
    score: 41,
    verdict: "I/O modules validated in reference guide — study only, not CEI CCI form factor.",
    caveats: [
      "i.MX 8M Mini — not Plus lead silicon",
      "2× Ethernet gateway ≠ Eth_A/B + plant×2 CCI",
      "Do not confuse DIN-rail gateway with plant controller",
    ],
  },
];

export const SCORE_PARAMS: ScoreParam[] = [
  {
    id: "eth",
    label: "Dual GbE / Eth_A–B software path (C1)",
    weight: 14,
    scores: {
      toradex: 8,
      variscite: 8,
      "compulab-som": 4,
      "compulab-iotgate": 3,
      nxp: 3,
    },
    notes: {
      toradex: "VALIDATED DS: 2× GbE RJ45 — SW Eth_A/B demo",
      variscite: "VALIDATED DS: 2× GbE on DART + DT8MCustomBoard",
      "compulab-som": "UNVALIDATED — often 1× GbE",
      "compulab-iotgate": "VALIDATED: 2× Eth gateway — not CCI topology",
      nxp: "EVK ports ≠ CCI 4-port switch",
    },
  },
  {
    id: "bsp",
    label: "OpenWrt / lab BSP (C7)",
    weight: 14,
    scores: {
      toradex: 3,
      variscite: 3,
      "compulab-som": 3,
      "compulab-iotgate": 4,
      nxp: 3,
    },
    notes: {
      toradex: "Historical SoM study — not Phase 1 lab path",
      variscite: "Historical alternate — not lab path",
      "compulab-som": "Historical alternate — not lab path",
      "compulab-iotgate": "I/O study only",
      nxp: "Silicon reference only — TG-524 is lab DUT",
    },
  },
  {
    id: "hab",
    label: "HAB / signed-boot path & docs (C6, C9)",
    weight: 12,
    scores: {
      toradex: 9,
      variscite: 6,
      "compulab-som": 4,
      "compulab-iotgate": 4,
      nxp: 10,
    },
    notes: {
      toradex: "Signed bootloader project + security meta",
      variscite: "Vendor HAB docs — need on disk",
      "compulab-som": "TBD evidence",
      "compulab-iotgate": "TBD — not kit decision driver",
      nxp: "Canonical HAB / CST / CAAM reference",
    },
  },
  {
    id: "docs",
    label: "Datasheets validated in our KB",
    weight: 12,
    scores: {
      toradex: 10,
      variscite: 7,
      "compulab-som": 2,
      "compulab-iotgate": 6,
      nxp: 6,
    },
    notes: {
      toradex: "Kit + family + carrier + Plus SoM DS INGESTED",
      variscite: "DART + DT8MCustomBoard INGESTED",
      "compulab-som": "BLOCKED — no PDFs in KB",
      "compulab-iotgate": "Reference guide INGESTED (study)",
      nxp: "CEC + UG10164 HAVE; IEC industrial DS MISSING",
    },
  },
  {
    id: "plus",
    label: "i.MX 8M Plus industrial SoM path",
    weight: 10,
    scores: {
      toradex: 9,
      variscite: 8,
      "compulab-som": 5,
      "compulab-iotgate": 3,
      nxp: 8,
    },
    notes: {
      toradex: "Preferred Plus IT 0063",
      variscite: "DART / VAR-SOM Plus IT available",
      "compulab-som": "Confirm Plus vs Mini SKU",
      "compulab-iotgate": "Typically Mini-class inside",
      nxp: "Plus silicon — not a SoM module",
    },
  },
  {
    id: "longevity",
    label: "Longevity / PCN readiness (C8)",
    weight: 8,
    scores: {
      toradex: 7,
      variscite: 5,
      "compulab-som": 4,
      "compulab-iotgate": 3,
      nxp: 2,
    },
    notes: {
      toradex: "IT path clear — letter still to request",
      variscite: "Request after SKU lock",
      "compulab-som": "TBD",
      "compulab-iotgate": "Gateway longevity ≠ CCI SoM",
      nxp: "N/A for module longevity story",
    },
  },
  {
    id: "carrier",
    label: "Carrier design guides / pinmux maturity",
    weight: 8,
    scores: {
      toradex: 10,
      variscite: 6,
      "compulab-som": 3,
      "compulab-iotgate": 2,
      nxp: 7,
    },
    notes: {
      toradex: "Carrier design guide on disk",
      variscite: "Custom-board DS on disk — pinmux still to freeze",
      "compulab-som": "Need full design pack",
      "compulab-iotgate": "Not a carrier platform for CCI",
      nxp: "EVK schematics useful for silicon",
    },
  },
  {
    id: "budget",
    label: "Phase 1 budget fit (~€1k envelope)",
    weight: 6,
    scores: {
      toradex: 8,
      variscite: 6,
      "compulab-som": 6,
      "compulab-iotgate": 5,
      nxp: 4,
    },
    notes: {
      toradex: "Board ~€300 + SoM — fits envelope",
      variscite: "Price TBD — assume mid",
      "compulab-som": "Price TBD",
      "compulab-iotgate": "Separate study spend",
      nxp: "EVK + still need SoM path",
    },
  },
  {
    id: "second",
    label: "Second-source / pin ecosystem risk",
    weight: 5,
    scores: {
      toradex: 7,
      variscite: 8,
      "compulab-som": 4,
      "compulab-iotgate": 2,
      nxp: 3,
    },
    notes: {
      toradex: "Verdin family ecosystem",
      variscite: "DART Pin2Pin is a strength",
      "compulab-som": "Narrower ecosystem",
      "compulab-iotgate": "Closed gateway SKU",
      nxp: "Bare SoC — you own packaging",
    },
  },
  {
    id: "fieldio",
    label: "Kit covers C2 / C4 / C5 field I/O",
    weight: 4,
    scores: {
      toradex: 3,
      variscite: 2,
      "compulab-som": 2,
      "compulab-iotgate": 8,
      nxp: 1,
    },
    notes: {
      toradex: "VALIDATED: 1× RS485 DB9 (not opto) — carrier for product C2/C4/C5",
      variscite: "No plant RS-485/DI/DO on kit DS — carrier required",
      "compulab-som": "Unvalidated",
      "compulab-iotgate": "VALIDATED: isolated RS485 + DI/DO modules (study)",
      nxp: "None",
    },
  },
  {
    id: "din",
    label: "DIN-rail product form on the kit itself (C10)",
    weight: 3,
    scores: {
      toradex: 1,
      variscite: 1,
      "compulab-som": 1,
      "compulab-iotgate": 8,
      nxp: 0,
    },
    notes: {
      toradex: "Bench board — not product",
      variscite: "Eval — not product",
      "compulab-som": "Eval — not product",
      "compulab-iotgate": "DIN gateway — still ≠ CCI",
      nxp: "Lab EVK",
    },
  },
  {
    id: "clarity",
    label: "Clear “not field CCI” role (avoid false product)",
    weight: 4,
    scores: {
      toradex: 10,
      variscite: 9,
      "compulab-som": 9,
      "compulab-iotgate": 2,
      nxp: 10,
    },
    notes: {
      toradex: "Explicit eval kit",
      variscite: "Explicit eval kit",
      "compulab-som": "Explicit eval kit",
      "compulab-iotgate": "Looks like a product — study only",
      nxp: "Clearly silicon reference",
    },
  },
];

/** Contribution for one param/candidate */
export function paramContribution(param: ScoreParam, id: CandidateId): number {
  return (param.scores[id] / 10) * param.weight;
}

export function recomputedScore(id: CandidateId): number {
  const sum = SCORE_PARAMS.reduce((acc, p) => acc + paramContribution(p, id), 0);
  return Math.round(sum);
}

export const CONTROL_ROWS: ControlRow[] = [
  {
    id: "C1",
    label: "C1 · ≥4× Ethernet Eth_A / Eth_B / plant×2",
    productRequired: true,
    kitOkPartial: true,
    marks: {
      toradex: "P",
      variscite: "P",
      "compulab-som": "?",
      "compulab-iotgate": "N",
      nxp: "N",
    },
    note: "VALIDATED Toradex+Variscite: 2 ports OK for SW. Product needs 4 on carrier.",
  },
  {
    id: "C2",
    label: "C2 · 2× opto RS-485 Modbus RJ45",
    productRequired: true,
    kitOkPartial: true,
    marks: {
      toradex: "P",
      variscite: "N",
      "compulab-som": "?",
      "compulab-iotgate": "S",
      nxp: "N",
    },
    note: "Toradex VALIDATED: 1× RS485 DB9 non-opto (Partial lab). Product still carrier.",
  },
  {
    id: "C3",
    label: "C3 · IEC 61850 Server/Client/GOOSE on Eth_A",
    productRequired: true,
    kitOkPartial: false,
    marks: {
      toradex: "F",
      variscite: "P",
      "compulab-som": "?",
      "compulab-iotgate": "N",
      nxp: "P",
    },
    note: "F/P = SW capability on kit Ethernet — not a vendor 61850 certificate.",
  },
  {
    id: "C4",
    label: "C4 · DI 10–120 Vdc ×5 / DO relay ×3",
    productRequired: true,
    kitOkPartial: true,
    marks: {
      toradex: "N",
      variscite: "N",
      "compulab-som": "?",
      "compulab-iotgate": "S",
      nxp: "N",
    },
    note: "IOT-GATE study: 4 DI+4 DO @24 V modules — not CEI 5/3 field class.",
  },
  {
    id: "C5",
    label: "C5 · GNSS + SMA",
    productRequired: true,
    kitOkPartial: true,
    marks: {
      toradex: "N",
      variscite: "N",
      "compulab-som": "?",
      "compulab-iotgate": "S",
      nxp: "N",
    },
    note: "IOT-GATE may include GNSS option — study only.",
  },
  {
    id: "C6",
    label: "C6 · Secure boot + key storage",
    productRequired: true,
    kitOkPartial: false,
    marks: {
      toradex: "P",
      variscite: "P",
      "compulab-som": "?",
      "compulab-iotgate": "P",
      nxp: "F",
    },
    note: "Must prove HAB path in Phase 1 on chosen SoM.",
  },
  {
    id: "C7",
    label: "C7 · OpenWrt lab BSP (TG-524)",
    productRequired: true,
    kitOkPartial: false,
    marks: {
      toradex: "F",
      variscite: "P",
      "compulab-som": "?",
      "compulab-iotgate": "P",
      nxp: "F",
    },
    note: "Compulab SoM BSP unvalidated in KB.",
  },
  {
    id: "C8",
    label: "C8 · Longevity / PCN (~10 y)",
    productRequired: true,
    kitOkPartial: true,
    marks: {
      toradex: "P",
      variscite: "?",
      "compulab-som": "?",
      "compulab-iotgate": "N/A",
      nxp: "N/A",
    },
    note: "Letter for chosen SoM SKU — not the kit.",
  },
  {
    id: "C9",
    label: "C9 · Vendor signed-boot docs",
    productRequired: true,
    kitOkPartial: false,
    marks: {
      toradex: "F",
      variscite: "?",
      "compulab-som": "?",
      "compulab-iotgate": "?",
      nxp: "F",
    },
    note: "Toradex signed-boot GitHub + NXP HAB docs validated as available.",
  },
  {
    id: "C10",
    label: "C10 · 12–24 Vdc + DIN-rail product path",
    productRequired: true,
    kitOkPartial: true,
    marks: {
      toradex: "P",
      variscite: "P",
      "compulab-som": "?",
      "compulab-iotgate": "S",
      nxp: "N",
    },
    note: "Toradex VALIDATED 7–24 V bench. IOT-GATE DIN = study form, not CCI.",
  },
];

export const REGULATION_ROWS: RegulationRow[] = [
  {
    id: "cei-o",
    norm: "CEI 0-16 Allegato O",
    classCode: "GRID-IT",
    forKitPick: "yes",
    forProductShip: "yes",
    kbStatus: "ingested",
    needCertToChooseKit: false,
    note: "Drives C1/C2/C4 topology — mandate, not a kit certificate.",
  },
  {
    id: "cei-t",
    norm: "CEI 0-16 Allegato T",
    classCode: "GRID-IT",
    forKitPick: "yes",
    forProductShip: "yes",
    kbStatus: "ingested",
    needCertToChooseKit: false,
    note: "Eth_A / IEC 61850 exchange with DSO.",
  },
  {
    id: "ailux",
    norm: "AiLux CCI manual (behaviour ref)",
    classCode: "GRID-IT",
    forKitPick: "yes",
    forProductShip: "yes",
    kbStatus: "ingested",
    needCertToChooseKit: false,
    note: "Reference module behaviour — competitor baseline.",
  },
  {
    id: "arera",
    norm: "ARERA 540/2021",
    classCode: "REGULATOR-IT",
    forKitPick: "nice",
    forProductShip: "yes",
    kbStatus: "missing",
    needCertToChooseKit: false,
    note: "Does not change SoM brand by itself.",
  },
  {
    id: "61850",
    norm: "IEC 61850 stack on kit (C3)",
    classCode: "PROTO-FIELD",
    forKitPick: "yes",
    forProductShip: "yes",
    kbStatus: "partial",
    needCertToChooseKit: false,
    note: "Run SW on kit — no 3rd-party 61850 cert required to pick kit.",
  },
  {
    id: "62351",
    norm: "IEC 62351-3…6",
    classCode: "CYBER-PROTO",
    forKitPick: "nice",
    forProductShip: "yes",
    kbStatus: "missing",
    needCertToChooseKit: false,
    note: "Design later — not a kit-order blocker.",
  },
  {
    id: "62443",
    norm: "IEC 62443-4-1 / 4-2",
    classCode: "CYBER-IACS",
    forKitPick: "nice",
    forProductShip: "yes",
    kbStatus: "missing",
    needCertToChooseKit: false,
    note: "Prefer HAB-ready SoM; lab scoping before schematic freeze.",
  },
  {
    id: "fips",
    norm: "FIPS 140-2 / TPM L3 claim",
    classCode: "CYBER-CRYPTO",
    forKitPick: "nice",
    forProductShip: "if-claimed",
    kbStatus: "missing",
    needCertToChooseKit: false,
    note: "Only if product claims it — verify SoM TPM SKU.",
  },
  {
    id: "cra",
    norm: "CRA (EU) 2024/2847",
    classCode: "CYBER-EU",
    forKitPick: "nice",
    forProductShip: "yes",
    kbStatus: "ingested",
    needCertToChooseKit: false,
    note: "Product law — not eval-board choice.",
  },
  {
    id: "red",
    norm: "RED II 2018/2001",
    classCode: "ENERGY-EU",
    forKitPick: "no",
    forProductShip: "context",
    kbStatus: "ingested",
    needCertToChooseKit: false,
    note: "Policy context only.",
  },
  {
    id: "61557",
    norm: "CEI EN 61557-12",
    classCode: "METROLOGY",
    forKitPick: "no",
    forProductShip: "if-claimed",
    kbStatus: "missing",
    needCertToChooseKit: false,
    note: "Metering path — not SoM selector.",
  },
];

export const MARK_LABEL: Record<CellMark, string> = {
  F: "Full",
  P: "Partial",
  N: "None",
  "?": "Unvalidated",
  "N/A": "N/A",
  S: "Study only",
};

export function rankedCandidates(): VendorCandidate[] {
  return [...VENDOR_CANDIDATES].sort((a, b) => b.score - a.score);
}
