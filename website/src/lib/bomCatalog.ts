/** Prototype BOM — supplier categories & kit choices (validate docs before compare). */

export type BomRole = "lead" | "alternate" | "study" | "silicon" | "carrier";

export type DocStatus = "have" | "partial" | "missing";

export interface BomKit {
  id: string;
  name: string;
  pn?: string;
  role: BomRole;
  roleLabel: string;
  pairsWith: string;
  note: string;
  url: string;
  docStatus: DocStatus;
}

export interface BomSupplierCategory {
  id: string;
  vendor: string;
  tagline: string;
  recommended?: boolean;
  kits: BomKit[];
}

export interface BomCarrierLine {
  id: string;
  category: string;
  items: string;
  docStatus: DocStatus;
  cci?: string;
}

export const BOM_ROLES: { id: BomRole | "all"; label: string }[] = [
  { id: "all", label: "All kits" },
  { id: "lead", label: "Lead path" },
  { id: "alternate", label: "Alternate SoM" },
  { id: "study", label: "I/O study only" },
  { id: "silicon", label: "Silicon / HAB" },
  { id: "carrier", label: "Carrier boards" },
];

export const BOM_SUPPLIERS: BomSupplierCategory[] = [
  {
    id: "toradex",
    vendor: "Toradex",
    tagline: "Verdin family — lead Phase 1 path (SoM + eval board, not field CCI).",
    recommended: true,
    kits: [
      {
        id: "KIT-TOR-VERDIN-DEV-99991105",
        name: "Verdin Development Board with HDMI Adapter",
        pn: "99991105",
        role: "lead",
        roleLabel: "Lead eval kit",
        pairsWith: "Verdin iMX8M Plus Quad 4GB IT (PN 0063)",
        note: "VALIDATED DS: 2× GbE + 1× RS485 DB9 (non-opto) + RS232. Product still needs 2× opto RJ45 Modbus, plant Eth, DI/DO, GNSS on carrier.",
        url: "https://www.toradex.com/products/carrier-board/verdin-development-board-kit",
        docStatus: "have",
      },
      {
        id: "SOM-TOR-0063",
        name: "Verdin iMX8M Plus Quad 4GB IT",
        pn: "0063",
        role: "lead",
        roleLabel: "Preferred SoM",
        pairsWith: "Kit 99991105",
        note: "INGESTED DS: no Wi-Fi, HAB, 32 GB eMMC, optional TPM — preferred for CCI cabinet.",
        url: "https://www.toradex.com/computer-on-modules/verdin-arm-family/nxp-imx-8m-plus",
        docStatus: "have",
      },
      {
        id: "SOM-TOR-0058",
        name: "Verdin iMX8M Plus Quad 4GB Wi-Fi / Bluetooth IT",
        pn: "0058",
        role: "alternate",
        roleLabel: "SoM + radio",
        pairsWith: "Kit 99991105",
        note: "Same silicon class; radio usually unwanted in MT CCI enclosure.",
        url: "https://www.toradex.com/computer-on-modules/verdin-arm-family/nxp-imx-8m-plus",
        docStatus: "partial",
      },
      {
        id: "KIT-TOR-DAHLIA",
        name: "Dahlia Carrier Board with HDMI Adapter",
        pn: "9998 / 0155…",
        role: "carrier",
        roleLabel: "Compact eval carrier",
        pairsWith: "Any Verdin SoM",
        note: "Smaller than Dev Board; typically 1× GbE — weaker Eth_A/B lab story.",
        url: "https://www.toradex.com/products/carrier-board/dahlia-carrier-board-kit",
        docStatus: "missing",
      },
      {
        id: "KIT-TOR-EVAL-PLUS-1",
        name: "Verdin iMX8M Plus Evaluation Kit 1",
        role: "lead",
        roleLabel: "Bundled starter",
        pairsWith: "Shop bundle (Dev Board + SoM)",
        note: "Pre-configured shop kit. Confirm SoM is IT without Wi-Fi when ordering.",
        url: "https://www.toradex.com/computer-on-modules/verdin-arm-family/nxp-imx-8m-plus",
        docStatus: "partial",
      },
    ],
  },
  {
    id: "variscite",
    vendor: "Variscite",
    tagline: "DART or VAR-SOM Plus — validate datasheet + pinmux before scoring vs Toradex.",
    kits: [
      {
        id: "KIT-VAR-DVK-DT8M-PLUS",
        name: "VAR-DVK-DT8M-PLUS",
        role: "alternate",
        roleLabel: "DART eval kit",
        pairsWith: "DART-MX8M-PLUS + VAR-DT8MCustomBoard",
        note: "VALIDATED: DART + DT8MCustomBoard DS on disk — 2× GbE. Lock IT SoM PN before buy.",
        url: "https://variscite.com/system-on-module-som/i-mx-8/i-mx-8m-plus/dart-mx8m-plus-evaluation-kits/",
        docStatus: "partial",
      },
      {
        id: "KIT-VAR-STK-DT8M-PLUS",
        name: "VAR-STK-DT8M-PLUS",
        role: "alternate",
        roleLabel: "DART starter kit",
        pairsWith: "DART-MX8M-PLUS",
        note: "Starter variant of the DART Plus kit line.",
        url: "https://variscite.com/system-on-module-som/i-mx-8/i-mx-8m-plus/dart-mx8m-plus-evaluation-kits/",
        docStatus: "missing",
      },
      {
        id: "KIT-VAR-DVK-VS8M-PLUS",
        name: "VAR-SOM-MX8M-PLUS Evaluation Kit",
        role: "alternate",
        roleLabel: "VAR-SOM + Symphony",
        pairsWith: "VAR-SOM-MX8M-PLUS + Symphony-Board V2",
        note: "SO-DIMM path. Validate V2 datasheet + pinmux table before compare.",
        url: "https://variscite.com/",
        docStatus: "missing",
      },
      {
        id: "SOM-VAR-DART-PLUS",
        name: "DART-MX8M-PLUS",
        role: "alternate",
        roleLabel: "SoM only",
        pairsWith: "VAR-DT8MCustomBoard",
        note: "VALIDATED DS: dual GbE + industrial options — exact IT assembly PN TBD.",
        url: "https://variscite.com/product/system-on-module-som/cortex-a53-krait/dart-mx8m-plus-nxp-i-mx-8m-plus/",
        docStatus: "partial",
      },
      {
        id: "SOM-VAR-SOM-PLUS",
        name: "VAR-SOM-MX8M-PLUS",
        role: "alternate",
        roleLabel: "SoM only",
        pairsWith: "Symphony-Board",
        note: "Reference manuals: brief, V2 DS, QSG, pinmux, family chart.",
        url: "https://variscite.com/",
        docStatus: "missing",
      },
    ],
  },
  {
    id: "compulab",
    vendor: "Compulab",
    tagline: "SoM eval or IOT-GATE for industrial I/O patterns — not CEI Eth_A/B CCI form factor.",
    kits: [
      {
        id: "KIT-CMP-CL-SOM-EVK",
        name: "CL-SOM-iMX8 and SBC-iMX8 Evaluation Kit",
        role: "alternate",
        roleLabel: "SoM eval kit",
        pairsWith: "CL-SOM-iMX8 + SB-iMX8 carrier",
        note: "Standard Compulab SoM bring-up kit.",
        url: "https://www.compulab.com/products/computer-on-modules/cl-som-imx8-nxp-i-mx-8-system-on-module-computer/",
        docStatus: "missing",
      },
      {
        id: "KIT-CMP-IOT-GATE-EVK",
        name: "IOT-GATE-iMX8 Evaluation Kit",
        role: "study",
        roleLabel: "I/O study only",
        pairsWith: "UCM-iMX8M-Mini inside gateway",
        note: "VALIDATED ref guide: Mini SoC, modular RS-485/DI/DO — study only, not CEI CCI.",
        url: "https://www.compulab.com/products/iot-gateways/iot-gate-imx8-industrial-arm-iot-gateway/",
        docStatus: "partial",
      },
      {
        id: "GW-CMP-IOT-GATE",
        name: "IOT-GATE-iMX8",
        role: "study",
        roleLabel: "Product gateway",
        pairsWith: "—",
        note: "Industrial gateway SKU. Reference for field I/O, not plant-controller topology.",
        url: "https://www.compulab.com/products/iot-gateways/iot-gate-imx8-industrial-arm-iot-gateway/",
        docStatus: "missing",
      },
    ],
  },
  {
    id: "nxp",
    vendor: "NXP",
    tagline: "Bare silicon / EVK reference — not a DIN-rail CCI product.",
    kits: [
      {
        id: "KIT-NXP-8MPLUSLPD4-EVK",
        name: "i.MX 8M Plus LPDDR4 Evaluation Kit",
        pn: "8MPLUSLPD4-EVK",
        role: "silicon",
        roleLabel: "Silicon / HAB lab",
        pairsWith: "IMX8MPIEC / IMX8MPCEC datasheets",
        note: "HAB, device tree, UG10164. Everything else still on custom carrier.",
        url: "https://www.nxp.com/design/design-center/development-boards-and-designs/8MPLUSLPD4-EVK",
        docStatus: "partial",
      },
      {
        id: "SOC-NXP-PLUS-CEC",
        name: "i.MX 8M Plus Applications Processor Datasheet (Consumer)",
        pn: "IMX8MPCEC",
        role: "silicon",
        roleLabel: "SoC DS on disk",
        pairsWith: "Prefer industrial IMX8MPIEC for IT claims",
        note: "Ingested as ccli-nxp-imx8m-plus-cec. Validate IEC before COMPARE_READY.",
        url: "https://www.nxp.com/products/i.MX8MPLUS",
        docStatus: "have",
      },
      {
        id: "SOC-NXP-PLUS-IEC",
        name: "i.MX 8M Plus Applications Processor Datasheet (Industrial)",
        pn: "IMX8MPIEC",
        role: "silicon",
        roleLabel: "SoC DS required",
        pairsWith: "CCI industrial-temp path",
        note: "Manual download from NXP product page — still MISSING.",
        url: "https://www.nxp.com/products/i.MX8MPLUS",
        docStatus: "missing",
      },
    ],
  },
];

/** Custom carrier lines shared after SoM lock (not vendor kits). */
export const BOM_CARRIER_LINES: BomCarrierLine[] = [
  {
    id: "c1",
    category: "Ethernet (C1)",
    items: "Switch / PHYs + 4× RJ45 magjack (Eth_A, Eth_B, plant ×2)",
    docStatus: "missing",
    cci: "C1",
  },
  {
    id: "c2",
    category: "RS-485 signals (C2)",
    items: "2× opto-isolated RS-485 + RJ45 Modbus pinout",
    docStatus: "missing",
    cci: "C2",
  },
  {
    id: "c4",
    category: "DI / DO relays (C4)",
    items: "DI 10–120 Vdc ×5 opto · DO relay ×3 (250 Vac class)",
    docStatus: "missing",
    cci: "C4",
  },
  {
    id: "c5",
    category: "GNSS (C5)",
    items: "u-blox-class module + active antenna SMA",
    docStatus: "missing",
    cci: "C5",
  },
  {
    id: "c6",
    category: "Security (C6)",
    items: "HAB path on SoM · optional discrete TPM",
    docStatus: "partial",
    cci: "C6",
  },
  {
    id: "c10",
    category: "Power (C10)",
    items: "12–24 Vdc input tree · DIN-rail enclosure",
    docStatus: "missing",
    cci: "C10",
  },
];

export const BOM_LEAD_PICK = {
  kit: "Verdin Development Board with HDMI Adapter (99991105)",
  som: "Verdin iMX8M Plus Quad 4GB IT (0063)",
  rule: "Validate official datasheets first — then score kits against C1–C10. Eval kits are never the field CCI.",
};

/** Lab mocking / PF2 bench — separate CapEx from product prototype BOM. */
export type BenchWaveStatus = "buy_now" | "buy_next" | "defer";

export interface BenchBomWave {
  id: string;
  level: string;
  status: BenchWaveStatus;
  statusLabel: string;
  estEur: string;
  goal: string;
  buyNow: string[];
}

export const BENCH_BOM_WAVES: BenchBomWave[] = [
  {
    id: "A",
    level: "L1 + L2",
    status: "buy_now",
    statusLabel: "BUY NOW",
    estEur: "€180–350",
    goal: "Software Modbus/IEC mock + physical RS-485 / Ethernet HIL",
    buyNow: [
      "2× isolated USB-RS485 adapters",
      "5–8 port GbE switch + labelled CAT6",
      "RS-485 cable, DB9, 120 Ω termination",
      "24 V bench PSU + cable labels",
      "pymodbus + libiec61850 clients (SW)",
    ],
  },
  {
    id: "B",
    level: "L2 + C4",
    status: "buy_next",
    statusLabel: "BUY NEXT",
    estEur: "€80–180",
    goal: "24 V DI test box + DO indicator lamps (defer 110 Vdc)",
    buyNow: [
      "DIN enclosure + fused terminal blocks",
      "5× named DI toggles + E-stop",
      "3× 24 V DO lamps",
    ],
  },
  {
    id: "C",
    level: "L3",
    status: "buy_next",
    statusLabel: "BUY NEXT",
    estEur: "€400–1,200+",
    goal: "Real energy analyzer after Wave A Modbus/PF2 vectors pass",
    buyNow: [
      "Lock Schneider / Janitza / Gavazzi / Siemens PAC model",
      "Official Modbus register map (ingest first)",
    ],
  },
  {
    id: "D",
    level: "L4",
    status: "defer",
    statusLabel: "DEFER",
    estEur: "rental / 10k–100k+",
    goal: "Typhoon / OPAL-RT / RTDS closed-loop — not Phase 1",
    buyNow: ["Decide after L1–L3 evidence pack"],
  },
];
