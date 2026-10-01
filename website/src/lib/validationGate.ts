/** Evidence validation — must pass before treating scores as COMPARE_READY. */

import type { CandidateId } from "./vendorScorecard";

export type EvidenceStatus = "pass" | "fail" | "partial" | "na";

export type CompareGate =
  | "compare_ready"
  | "conditional"
  | "partial_validated"
  | "silicon_only"
  | "blocked";

export interface EvidenceItem {
  id: string;
  claim: string;
  status: EvidenceStatus;
  evidence: string;
  pathOrUrl: string;
}

export interface CandidateValidation {
  id: CandidateId;
  gate: CompareGate;
  gateLabel: string;
  summary: string;
  items: EvidenceItem[];
}

export const VALIDATION_RULE =
  "Validate datasheet evidence first. Scores below are only for candidates that are not BLOCKED. ? cells mean unvalidated — do not treat them as engineering truth.";

export const CANDIDATE_VALIDATION: CandidateValidation[] = [
  {
    id: "toradex",
    gate: "conditional",
    gateLabel: "Conditional · SoM DS in RAG",
    summary:
      "Kit + Verdin iMX8M Plus datasheet (Quad 4GB IT / no Wi-Fi) on disk and ingested. Still need PCN/longevity letter before Architecture Freeze.",
    items: [
      {
        id: "t-kit-pn",
        claim: "Kit = Verdin Development Board + HDMI, shop PN 99991105 / V1.1F",
        status: "pass",
        evidence: "Toradex shop + Dev Board DS V1.1 on disk (2× GbE, RS485, RS232).",
        pathOrUrl: "08-engineering/Toradex_Verdin_Development_Board_V1.1_Datasheet.pdf",
      },
      {
        id: "t-eth",
        claim: "2× 10/100/1000 Ethernet (Eth_A/B SW demo)",
        status: "pass",
        evidence: "DS §2.7 — Ethernet_1 on SoM PHY, Ethernet_2 KSZ9131 on board.",
        pathOrUrl: "Toradex_Verdin_Development_Board_V1.1_Datasheet.pdf",
      },
      {
        id: "t-rs485",
        claim: "Serial for Modbus lab (not full C2 product)",
        status: "partial",
        evidence:
          "1× RS485 on DB9 via UART1 transceiver — NOT opto-isolated, NOT 2× RJ45 Modbus. CAN is isolated; RS485 is not. Product C2 still on carrier.",
        pathOrUrl: "Toradex_Verdin_Development_Board_V1.1_Datasheet.pdf",
      },
      {
        id: "t-som-0063",
        claim: "SoM PN 0063 = Verdin iMX8M Plus Quad 4GB IT (no Wi-Fi)",
        status: "pass",
        evidence: "Confirmed on Toradex Verdin iMX8M Plus product page (IT SKU list).",
        pathOrUrl: "https://www.toradex.com/computer-on-modules/verdin-arm-family/nxp-imx-8m-plus",
      },
      {
        id: "t-som-ds",
        claim: "Verdin iMX8M Plus SoM datasheet (covers Quad 4GB IT)",
        status: "pass",
        evidence:
          "ON DISK + INGESTED — HAB yes; eMMC 32 GB; Quad 4GB IT = no Wi-Fi; TPM 2.0 optional assembly. source_id ccli-toradex-verdin-plus-som.",
        pathOrUrl: "08-engineering/Toradex_Verdin_iMX8M_Plus_IT_Datasheet.pdf",
      },
      {
        id: "t-carrier-guide",
        claim: "Carrier design guide available",
        status: "pass",
        evidence: "ON DISK — Toradex_Verdin_Carrier_Board_Design_Guide.pdf",
        pathOrUrl: "08-engineering/Toradex_Verdin_Carrier_Board_Design_Guide.pdf",
      },
      {
        id: "t-pcn",
        claim: "Longevity / PCN letter for chosen SKU",
        status: "fail",
        evidence: "Not requested / not filed.",
        pathOrUrl: "—",
      },
    ],
  },
  {
    id: "variscite",
    gate: "partial_validated",
    gateLabel: "Partial · DS pack in RAG",
    summary:
      "DART-MX8M-PLUS + VAR-DT8MCustomBoard datasheets ingested (dual GbE confirmed). Lock exact IT assembly PN before buy; still not full COMPARE_READY vs Toradex.",
    items: [
      {
        id: "v-dart-ds",
        claim: "DART-MX8M-PLUS datasheet on disk + ingested",
        status: "pass",
        evidence: "INGESTED ccli-variscite-dart-mx8m-plus — 2× GbE, industrial options, TSN on one port.",
        pathOrUrl: "08-engineering/reference/vendor-validate/Variscite_DART-MX8M-PLUS_Datasheet.pdf",
      },
      {
        id: "v-board-ds",
        claim: "VAR-DT8MCustomBoard datasheet on disk + ingested",
        status: "pass",
        evidence: "INGESTED ccli-variscite-dt8m-customboard — 2× GbE RJ45; 2nd PHY on carrier for Plus.",
        pathOrUrl:
          "08-engineering/reference/vendor-validate/Variscite_VAR-DT8MCustomBoard_Datasheet.pdf",
      },
      {
        id: "v-eth",
        claim: "Dual GbE for Eth_A/B SW path",
        status: "pass",
        evidence: "Validated in both PDFs — Partial C1 (2 ports), same class as Toradex kit.",
        pathOrUrl: "Variscite DART + DT8MCustomBoard DS",
      },
      {
        id: "v-rs485",
        claim: "2× opto RS-485 Modbus RJ45 on kit",
        status: "fail",
        evidence: "Not found in DART / custom-board DS extracts — expect carrier for product C2.",
        pathOrUrl: "Variscite PDFs",
      },
      {
        id: "v-sku",
        claim: "Exact IT SoM orderable PN locked",
        status: "fail",
        evidence: "Not locked — assembly options (EC/AC/WBD/…) still open.",
        pathOrUrl: "—",
      },
      {
        id: "v-symphony",
        claim: "VAR-SOM-MX8M-PLUS / Symphony V2 pack validated",
        status: "fail",
        evidence: "Not downloaded — DART path validated instead for now.",
        pathOrUrl: "—",
      },
    ],
  },
  {
    id: "nxp",
    gate: "silicon_only",
    gateLabel: "Silicon only · not a SoM",
    summary:
      "IMX8MPCEC on disk as silicon reference only — lab path is TG-524 OpenWrt, not NXP EVK.",
    items: [
      {
        id: "n-cec",
        claim: "i.MX 8M Plus CEC datasheet",
        status: "pass",
        evidence: "ON DISK + ingested — IMX8MPCEC.pdf",
        pathOrUrl: "08-engineering/reference/IMX8MPCEC.pdf",
      },
      {
        id: "n-iec",
        claim: "i.MX 8M Plus industrial IEC datasheet",
        status: "fail",
        evidence: "MISSING — required before industrial-temp silicon claims.",
        pathOrUrl: "08-engineering/reference/IMX8MPIEC.pdf",
      },
      {
        id: "n-evk",
        claim: "EVK is DIN-rail CCI / SoM product",
        status: "fail",
        evidence: "No — bare silicon evaluation only.",
        pathOrUrl: "NXP 8MPLUSLPD4-EVK product class",
      },
    ],
  },
  {
    id: "compulab-som",
    gate: "blocked",
    gateLabel: "Blocked · no datasheets",
    summary:
      "CL-SOM-iMX8 EVK named from vendor pages only. No PDF evidence in knowledge-base — score is provisional and must not drive a buy decision.",
    items: [
      {
        id: "c-som-ds",
        claim: "CL-SOM-iMX8 / SBC eval kit datasheets on disk",
        status: "fail",
        evidence: "MISSING — DOWNLOADS.md still lists Compulab kit PDFs as to-do.",
        pathOrUrl: "08-engineering/",
      },
      {
        id: "c-eth",
        claim: "Dual GbE confirmed for Eth_A/B",
        status: "fail",
        evidence: "Unvalidated — often single GbE class; confirm SKU first.",
        pathOrUrl: "—",
      },
    ],
  },
  {
    id: "compulab-iotgate",
    gate: "partial_validated",
    gateLabel: "Study only · I/O patterns",
    summary:
      "Reference guide on disk: Mini SoC, 2× Ethernet, modular RS-485 / DI/DO, DIN-rail. Validated as I/O study — explicitly NOT a CEI Eth_A/B CCI product candidate.",
    items: [
      {
        id: "g-ref",
        claim: "IOT-GATE-iMX8 reference guide on disk + ingested",
        status: "pass",
        evidence: "INGESTED ccli-compulab-iot-gate-ref — study-only I/O patterns.",
        pathOrUrl:
          "08-engineering/reference/vendor-validate/Compulab_IOT-GATE-iMX8_Reference_Guide.pdf",
      },
      {
        id: "g-soc",
        claim: "SoC class suitable as CCI Plus lead",
        status: "fail",
        evidence: "UCM-iMX8M-Mini / i.MX 8M Mini — weaker Eth_A/B story than Plus.",
        pathOrUrl: "IOT-GATE product docs",
      },
      {
        id: "g-io",
        claim: "Isolated RS-485 / DI/DO modules exist for study",
        status: "pass",
        evidence: "Add-on modules: isolated RS485, 4 DI + 4 DO (24 V class) — patterns only.",
        pathOrUrl: "IOT-GATE reference guide §3.10",
      },
      {
        id: "g-cci",
        claim: "Matches CEI 4-port Eth_A/B + plant CCI form factor",
        status: "fail",
        evidence: "Industrial gateway topology — do not deploy as plant controller.",
        pathOrUrl: "—",
      },
    ],
  },
];

export function validationFor(id: CandidateId): CandidateValidation | undefined {
  return CANDIDATE_VALIDATION.find((v) => v.id === id);
}

export function gateClass(gate: CompareGate): string {
  if (gate === "compare_ready" || gate === "conditional") return "ok";
  if (gate === "partial_validated" || gate === "silicon_only") return "warn";
  return "miss";
}

export function evidenceClass(s: EvidenceStatus): string {
  if (s === "pass") return "ok";
  if (s === "partial" || s === "na") return "warn";
  return "miss";
}
