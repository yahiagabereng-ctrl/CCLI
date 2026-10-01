export interface RoadmapChapter {
  id: string;
  index: string;
  title: string;
  blurb: string;
}

/** Ordered chapters for professional roadmap navigation */
export const ROADMAP_CHAPTERS: RoadmapChapter[] = [
  {
    id: "overview",
    index: "01",
    title: "Overview",
    blurb: "Target, scope, budget, and dual-track phases",
  },
  {
    id: "system",
    index: "02",
    title: "System architecture",
    blurb: "SoM, Ethernet, serial, I/O, security, power",
  },
  {
    id: "phases",
    index: "03",
    title: "Phase 1 timeline",
    blurb: "Ten to twelve weeks to DIN-rail prototype",
  },
  {
    id: "firmware",
    index: "04",
    title: "Firmware architecture",
    blurb: "Certified control path vs tooling path",
  },
  {
    id: "next",
    index: "05",
    title: "Next decisions",
    blurb: "SoM lock, pin budget, lab scoping",
  },
  {
    id: "prototype-bom",
    index: "06",
    title: "Prototype BOM",
    blurb: "Supplier categories — Toradex, Variscite, Compulab, NXP",
  },
  {
    id: "vendor-decision",
    index: "07",
    title: "Vendor decision",
    blurb: "Scored comparison — choose Phase 1 kit together",
  },
  {
    id: "knowledge-tree",
    index: "08",
    title: "Knowledge base",
    blurb: "Eight branches aligned to the build",
  },
  {
    id: "coverage",
    index: "09",
    title: "Corpus coverage",
    blurb: "Grid, BSP, and cyber documents in place",
  },
  {
    id: "corpus-analysis",
    index: "10",
    title: "Ingest analysis",
    blurb: "Sources, PDFs, chunk quality scores",
  },
];
