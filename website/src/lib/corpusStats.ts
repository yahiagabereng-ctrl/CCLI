export interface SourceStat {
  source_id: string;
  chunks: number;
  scored_chunks: number;
  mean_quality: number | null;
  median_quality: number | null;
  grade: string;
  other_ratio: number;
  filenames: string[];
}

export interface CorpusStats {
  generated_at: string;
  telematry_total: {
    sources: number;
    chunks: number;
    unreviewed_chunks: number;
    other_chunks: number;
    other_ratio: number;
  };
  ccli: {
    sources_ingested: number;
    chunks: number;
    scored_chunks: number;
    pdf_files_on_disk: number;
    md_files_on_disk: number;
    project_files_md_pdf: number;
    quality: {
      mean: number;
      median: number;
      min: number | null;
      max: number | null;
      grade: string;
      bands: { A: number; B: number; C: number; D: number; F: number };
      needs_review: number;
      ocr: { high: number; medium: number; low: number; unknown: number };
    };
    pdf_inventory: { name: string; path: string; kb: number }[];
    sources: SourceStat[];
  };
}

export async function loadCorpusStats(): Promise<CorpusStats> {
  const res = await fetch(`/corpus-stats.json?t=${Date.now()}`, {
    signal: AbortSignal.timeout(8000),
  });
  if (!res.ok) throw new Error("corpus-stats.json unavailable");
  return res.json() as Promise<CorpusStats>;
}

/** Live document/chunk counts from RAG (quality stays from snapshot). */
export async function refreshLiveCounts(): Promise<{
  sources: number;
  chunks: number;
  ccliSources: number;
  ccliChunks: number;
} | null> {
  try {
    const res = await fetch(
      "/api/rag/v1/classification/summary?tenant_id=telematry&bot_id=default",
      { signal: AbortSignal.timeout(20000) },
    );
    if (!res.ok) return null;
    const data = (await res.json()) as {
      total_chunks: number;
      sources: { source_id: string; total_chunks: number }[];
    };
    const re = /^(ccli-|cei-0-16-|eu-2018-2001-red|eu-2024-2847-cra$)/;
    const ccli = data.sources.filter((s) => re.test(s.source_id));
    return {
      sources: data.sources.length,
      chunks: data.total_chunks,
      ccliSources: ccli.length,
      ccliChunks: ccli.reduce((a, s) => a + (s.total_chunks || 0), 0),
    };
  } catch {
    return null;
  }
}

export function pct(n: number, d: number): string {
  if (!d) return "—";
  return `${Math.round((n / d) * 100)}%`;
}

export function fmtQuality(n: number | null | undefined): string {
  if (n == null || Number.isNaN(n)) return "—";
  return n.toFixed(3);
}
