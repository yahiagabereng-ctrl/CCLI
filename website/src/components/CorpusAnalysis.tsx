import { useEffect, useState } from "react";
import {
  fmtQuality,
  loadCorpusStats,
  pct,
  refreshLiveCounts,
  type CorpusStats,
} from "../lib/corpusStats";
import { pdfInventoryMeta } from "../lib/kbData";

function fmtSize(kb: number): string {
  return kb >= 1024 ? `${(kb / 1024).toFixed(1)} MB` : `${kb} KB`;
}

export function CorpusAnalysis() {
  const [stats, setStats] = useState<CorpusStats | null>(null);
  const [error, setError] = useState<string | null>(null);
  const [liveNote, setLiveNote] = useState<string | null>(null);
  const [refreshing, setRefreshing] = useState(false);

  useEffect(() => {
    let cancelled = false;
    (async () => {
      try {
        const s = await loadCorpusStats();
        if (!cancelled) setStats(s);
      } catch (e) {
        if (!cancelled) {
          setError(e instanceof Error ? e.message : "Failed to load corpus stats");
        }
      }
    })();
    return () => {
      cancelled = true;
    };
  }, []);

  async function onRefresh() {
    setRefreshing(true);
    setLiveNote(null);
    const live = await refreshLiveCounts();
    setRefreshing(false);
    if (!live) {
      setLiveNote("RAG offline — showing snapshot counts only.");
      return;
    }
    setStats((prev) =>
      prev
        ? {
            ...prev,
            telematry_total: {
              ...prev.telematry_total,
              sources: live.sources,
              chunks: live.chunks,
            },
            ccli: {
              ...prev.ccli,
              sources_ingested: live.ccliSources,
              chunks: live.ccliChunks,
            },
          }
        : prev,
    );
    setLiveNote(
      `Live RAG: ${live.ccliSources} CCLI sources · ${live.ccliChunks.toLocaleString()} chunks (quality from snapshot).`,
    );
  }

  if (error) {
    return (
      <section className="section" id="corpus-analysis">
        <p className="section-label">Corpus analysis</p>
        <h2>Ingest & chunk quality</h2>
        <p className="section-intro">{error}. Run scripts/build-corpus-stats.ps1 with RAG up.</p>
      </section>
    );
  }

  if (!stats) {
    return (
      <section className="section" id="corpus-analysis">
        <p className="section-label">Corpus analysis</p>
        <h2>Ingest & chunk quality</h2>
        <p className="section-intro">Loading snapshot…</p>
      </section>
    );
  }

  const q = stats.ccli.quality;
  const bandTotal = q.bands.A + q.bands.B + q.bands.C + q.bands.D + q.bands.F;
  const topSources = [...stats.ccli.sources].sort((a, b) => b.chunks - a.chunks).slice(0, 12);

  return (
      <section className="section band" id="corpus-analysis">
      <p className="section-label">Chapter 10 · Ingest analysis</p>
      <h2>Ingested documents & chunk scores</h2>
      <p className="section-intro">
        Snapshot of the CCLI-scoped RAG corpus versus the full Telematry tenant. Chunk quality
        uses the pipeline <code>chunk_quality_score</code> (0–1). Rebuild with{" "}
        <code>scripts/build-corpus-stats.ps1</code>.
      </p>

      <div className="stat-strip">
        <div className="stat">
          <span className="stat-value">{stats.ccli.sources_ingested}</span>
          <span className="stat-label">CCLI sources ingested</span>
        </div>
        <div className="stat">
          <span className="stat-value">{stats.ccli.pdf_files_on_disk}</span>
          <span className="stat-label">PDF files on disk</span>
        </div>
        <div className="stat">
          <span className="stat-value">{stats.ccli.md_files_on_disk}</span>
          <span className="stat-label">Markdown files</span>
        </div>
        <div className="stat">
          <span className="stat-value">{stats.ccli.chunks.toLocaleString()}</span>
          <span className="stat-label">CCLI chunks</span>
        </div>
        <div className="stat">
          <span className={`stat-value grade-${q.grade.toLowerCase()}`}>{q.grade}</span>
          <span className="stat-label">
            Mean quality {fmtQuality(q.mean)} · median {fmtQuality(q.median)}
          </span>
        </div>
      </div>

      <div className="analysis-meta">
        <p>
          Telematry tenant: <strong>{stats.telematry_total.sources}</strong> sources ·{" "}
          <strong>{stats.telematry_total.chunks.toLocaleString()}</strong> chunks total. CCLI is{" "}
          {pct(stats.ccli.chunks, stats.telematry_total.chunks)} of chunks. Scored chunks:{" "}
          {stats.ccli.scored_chunks.toLocaleString()}. Needs review flag:{" "}
          {q.needs_review.toLocaleString()}.
        </p>
        <p className="snap-time">
          Snapshot {new Date(stats.generated_at).toLocaleString()}
          {liveNote ? ` · ${liveNote}` : ""}
        </p>
        <button type="button" className="btn btn-ghost" onClick={() => void onRefresh()} disabled={refreshing}>
          {refreshing ? "Refreshing…" : "Refresh live counts"}
        </button>
      </div>

      <div className="quality-panel">
        <h3>Chunk quality bands</h3>
        <div className="band-bars" role="img" aria-label="Quality band distribution">
          {(["A", "B", "C", "D", "F"] as const).map((key) => {
            const n = q.bands[key];
            const width = bandTotal ? Math.max(2, (n / bandTotal) * 100) : 0;
            return (
              <div key={key} className="band-row">
                <span className="band-key">{key}</span>
                <div className="band-track">
                  <div className={`band-fill band-${key.toLowerCase()}`} style={{ width: `${width}%` }} />
                </div>
                <span className="band-n">
                  {n.toLocaleString()} · {pct(n, bandTotal)}
                </span>
              </div>
            );
          })}
        </div>
        <p className="ocr-line">
          OCR tag on scored chunks: high {q.ocr.high.toLocaleString()} · medium {q.ocr.medium} ·
          low {q.ocr.low} · unknown {q.ocr.unknown.toLocaleString()}
        </p>
      </div>

      <div className="source-table-wrap">
        <h3>Top sources by chunk count</h3>
        <table className="source-table">
          <thead>
            <tr>
              <th>source_id</th>
              <th>Chunks</th>
              <th>Mean Q</th>
              <th>Grade</th>
              <th>Other %</th>
            </tr>
          </thead>
          <tbody>
            {topSources.map((s) => (
              <tr key={s.source_id}>
                <td>
                  <code>{s.source_id}</code>
                </td>
                <td>{s.chunks.toLocaleString()}</td>
                <td>{fmtQuality(s.mean_quality)}</td>
                <td>
                  <span className={`grade-tag grade-${s.grade.toLowerCase()}`}>{s.grade}</span>
                </td>
                <td>{pct(Math.round(s.other_ratio * s.chunks), s.chunks)}</td>
              </tr>
            ))}
          </tbody>
        </table>
      </div>

      <div className="pdf-list">
        <h3>PDF inventory ({stats.ccli.pdf_files_on_disk})</h3>
        <ul>
          {stats.ccli.pdf_inventory.map((p) => {
            const meta = pdfInventoryMeta(p.name, p.path);
            return (
              <li key={p.path}>
                <span className="pdf-name">{p.name}</span>
                <span className="pdf-meta">
                  <span className="pdf-category">{meta.category}</span>
                  <span className="pdf-sep" aria-hidden="true">
                    ·
                  </span>
                  <span>{fmtSize(p.kb)}</span>
                </span>
                <p className="pdf-brief">{meta.brief}</p>
              </li>
            );
          })}
        </ul>
      </div>
    </section>
  );
}
