import { useMemo, useState } from "react";
import {
  CANDIDATE_VALIDATION,
  VALIDATION_RULE,
  evidenceClass,
  gateClass,
  validationFor,
} from "../lib/validationGate";
import {
  CONTROL_ROWS,
  MARK_LABEL,
  REGULATION_ROWS,
  SCORE_METHOD,
  SCORE_PARAMS,
  VENDOR_CANDIDATES,
  paramContribution,
  rankedCandidates,
  type CandidateId,
  type CellMark,
} from "../lib/vendorScorecard";

type Tab = "validation" | "scores" | "controls" | "regulations" | "weights";

function markClass(m: CellMark): string {
  if (m === "F") return "ok";
  if (m === "P" || m === "S") return "warn";
  if (m === "?") return "warn";
  return "miss";
}

function scoreBand(score: number): string {
  if (score >= 75) return "high";
  if (score >= 50) return "mid";
  return "low";
}

function kitPickLabel(v: "yes" | "nice" | "no"): string {
  if (v === "yes") return "Needed for kit pick";
  if (v === "nice") return "Nice for kit pick";
  return "Not for kit pick";
}

export function VendorDecisionMatrix() {
  const ranked = useMemo(() => rankedCandidates(), []);
  const [tab, setTab] = useState<Tab>("validation");
  const [focusId, setFocusId] = useState<CandidateId>(ranked[0]?.id ?? "toradex");

  const focus = VENDOR_CANDIDATES.find((c) => c.id === focusId) ?? ranked[0];
  const focusVal = validationFor(focusId);
  const orderedIds = ranked.map((c) => c.id);

  return (
    <section className="section" id="vendor-decision">
      <p className="section-label">Chapter 07 · Vendor decision</p>
      <h2>Validate first — then score</h2>
      <p className="section-intro">{VALIDATION_RULE}</p>

      <aside className="vd-gate-banner" aria-label="Validation status">
        <p className="vd-focus-label">Evidence gates (2026-07-25)</p>
        <ul className="vd-gate-list">
          {CANDIDATE_VALIDATION.map((v) => (
            <li key={v.id}>
              <span className={`status-pill ${gateClass(v.gate)}`}>{v.gateLabel}</span>
              <span className="vd-gate-name">
                {VENDOR_CANDIDATES.find((c) => c.id === v.id)?.shortName}
              </span>
            </li>
          ))}
        </ul>
      </aside>

      <p className="section-intro vd-score-note">{SCORE_METHOD}</p>

      <div className="vd-rank-grid" role="list">
        {ranked.map((c, i) => {
          const val = validationFor(c.id);
          const blocked = val?.gate === "blocked";
          return (
            <button
              key={c.id}
              type="button"
              role="listitem"
              className={`vd-rank-card score-${scoreBand(c.score)}${focusId === c.id ? " active" : ""}${blocked ? " blocked" : ""}`}
              onClick={() => setFocusId(c.id)}
            >
              <span className="vd-rank-place">#{i + 1}</span>
              <span className="vd-rank-score" aria-label={`Score ${c.score} of 100`}>
                {c.score}
                <small>/100</small>
              </span>
              <span className="vd-rank-name">{c.shortName}</span>
              {val ? (
                <span className={`status-pill ${gateClass(val.gate)}`}>{val.gateLabel}</span>
              ) : null}
              <span className="vd-rank-role">{c.roleLabel}</span>
              <span className="vd-rank-verdict">{c.verdict}</span>
            </button>
          );
        })}
      </div>

      {focus ? (
        <aside className="vd-focus" aria-live="polite">
          <p className="vd-focus-label">
            Selected · {focus.vendor} · {focus.score}/100
            {focusVal ? ` · ${focusVal.gateLabel}` : ""}
          </p>
          <p>
            <strong>{focus.kit}</strong>
            <span className="bom-lead-sep">+</span>
            {focus.som}
          </p>
          {focusVal ? <p className="vd-val-summary">{focusVal.summary}</p> : null}
          <ul className="vd-caveats">
            {focus.caveats.map((x) => (
              <li key={x}>{x}</li>
            ))}
          </ul>
        </aside>
      ) : null}

      <div className="vd-tabs" role="tablist" aria-label="Decision matrix views">
        {(
          [
            ["validation", "Evidence checklist"],
            ["scores", "Weighted scores"],
            ["controls", "C1–C10 matrix"],
            ["regulations", "Regulations vs certify"],
            ["weights", "How scoring works"],
          ] as const
        ).map(([id, label]) => (
          <button
            key={id}
            type="button"
            role="tab"
            aria-selected={tab === id}
            className={`bom-chip${tab === id ? " active" : ""}`}
            onClick={() => setTab(id)}
          >
            {label}
          </button>
        ))}
      </div>

      {tab === "validation" && focusVal ? (
        <div className="vd-evidence">
          <h3>
            Evidence · {focus?.shortName} ·{" "}
            <span className={`status-pill ${gateClass(focusVal.gate)}`}>{focusVal.gateLabel}</span>
          </h3>
          <p>{focusVal.summary}</p>
          <ul className="vd-evidence-list">
            {focusVal.items.map((item) => (
              <li key={item.id}>
                <span className={`status-pill ${evidenceClass(item.status)}`}>{item.status}</span>
                <div>
                  <strong>{item.claim}</strong>
                  <p>{item.evidence}</p>
                  <code>{item.pathOrUrl}</code>
                </div>
              </li>
            ))}
          </ul>
        </div>
      ) : null}

      {tab === "scores" ? (
        <div className="vd-table-wrap">
          <table className="vd-table">
            <thead>
              <tr>
                <th scope="col">Parameter</th>
                <th scope="col">W</th>
                {orderedIds.map((id) => {
                  const c = VENDOR_CANDIDATES.find((x) => x.id === id)!;
                  return (
                    <th key={id} scope="col" className={focusId === id ? "focus-col" : undefined}>
                      {c.shortName}
                    </th>
                  );
                })}
              </tr>
            </thead>
            <tbody>
              {SCORE_PARAMS.map((p) => (
                <tr key={p.id}>
                  <th scope="row">{p.label}</th>
                  <td className="vd-w">{p.weight}</td>
                  {orderedIds.map((id) => {
                    const s = p.scores[id];
                    const contrib = paramContribution(p, id);
                    return (
                      <td
                        key={id}
                        className={focusId === id ? "focus-col" : undefined}
                        title={p.notes[id]}
                      >
                        <span className="vd-cell-score">{s}/10</span>
                        <span className="vd-cell-contrib">+{contrib.toFixed(1)}</span>
                      </td>
                    );
                  })}
                </tr>
              ))}
              <tr className="vd-total-row">
                <th scope="row">Total (rounded)</th>
                <td className="vd-w">100</td>
                {orderedIds.map((id) => {
                  const c = VENDOR_CANDIDATES.find((x) => x.id === id)!;
                  return (
                    <td key={id} className={focusId === id ? "focus-col" : undefined}>
                      <strong>{c.score}</strong>
                    </td>
                  );
                })}
              </tr>
            </tbody>
          </table>
          <p className="vd-hint">Hover a cell for the engineer note. Click a rank card to highlight a column.</p>
        </div>
      ) : null}

      {tab === "controls" ? (
        <div className="vd-table-wrap">
          <table className="vd-table vd-table-compact">
            <thead>
              <tr>
                <th scope="col">Control</th>
                {orderedIds.map((id) => {
                  const c = VENDOR_CANDIDATES.find((x) => x.id === id)!;
                  return (
                    <th key={id} scope="col">
                      {c.shortName}
                    </th>
                  );
                })}
              </tr>
            </thead>
            <tbody>
              {CONTROL_ROWS.map((row) => (
                <tr key={row.id}>
                  <th scope="row">
                    {row.label}
                    <span className="vd-row-note">{row.note}</span>
                  </th>
                  {orderedIds.map((id) => {
                    const m = row.marks[id];
                    return (
                      <td key={id}>
                        <span className={`status-pill ${markClass(m)}`}>{MARK_LABEL[m]}</span>
                      </td>
                    );
                  })}
                </tr>
              ))}
            </tbody>
          </table>
          <p className="vd-hint">
            F Full · P Partial · N None · ? Docs not validated · S Study only · N/A not applicable
          </p>
        </div>
      ) : null}

      {tab === "regulations" ? (
        <div className="vd-table-wrap">
          <table className="vd-table vd-table-compact">
            <thead>
              <tr>
                <th scope="col">Norm</th>
                <th scope="col">Class</th>
                <th scope="col">For kit pick?</th>
                <th scope="col">KB</th>
                <th scope="col">Need cert to choose kit?</th>
                <th scope="col">Note</th>
              </tr>
            </thead>
            <tbody>
              {REGULATION_ROWS.map((r) => (
                <tr key={r.id}>
                  <th scope="row">{r.norm}</th>
                  <td>
                    <code>{r.classCode}</code>
                  </td>
                  <td>{kitPickLabel(r.forKitPick)}</td>
                  <td>
                    <span
                      className={`status-pill ${
                        r.kbStatus === "ingested" ? "ok" : r.kbStatus === "partial" ? "warn" : "miss"
                      }`}
                    >
                      {r.kbStatus}
                    </span>
                  </td>
                  <td>
                    <span className={`status-pill ${r.needCertToChooseKit ? "miss" : "ok"}`}>
                      {r.needCertToChooseKit ? "Yes" : "No"}
                    </span>
                  </td>
                  <td className="vd-note-cell">{r.note}</td>
                </tr>
              ))}
            </tbody>
          </table>
          <p className="vd-hint">
            Already enough to decide Phase 1: CEI O/T + AiLux + OpenWrt lab path. Do not wait for kit
            CE marks or vendor “61850 certified” stickers.
          </p>
        </div>
      ) : null}

      {tab === "weights" ? (
        <div className="vd-weights">
          <ol>
            {SCORE_PARAMS.map((p) => (
              <li key={p.id}>
                <strong>
                  {p.label} ({p.weight} pts)
                </strong>
                <span> — each vendor scored 0–10; contribution = score÷10 × weight.</span>
              </li>
            ))}
          </ol>
          <p>
            Field I/O and DIN form have low weight on purpose: every serious CCI path still needs a
            custom carrier. Docs-missing vendors are capped until PDFs are filed.
          </p>
        </div>
      ) : null}
    </section>
  );
}
