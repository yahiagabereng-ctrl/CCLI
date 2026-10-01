import { useMemo, useState } from "react";
import {
  BENCH_BOM_WAVES,
  BOM_CARRIER_LINES,
  BOM_LEAD_PICK,
  BOM_ROLES,
  BOM_SUPPLIERS,
  type BomRole,
  type DocStatus,
} from "../lib/bomCatalog";

function statusClass(s: DocStatus): string {
  if (s === "have") return "ok";
  if (s === "partial") return "partial";
  return "miss";
}

function statusLabel(s: DocStatus): string {
  if (s === "have") return "Docs on disk";
  if (s === "partial") return "Partial docs";
  return "Docs missing";
}

export function BomSelector() {
  const [supplierId, setSupplierId] = useState(BOM_SUPPLIERS[0]?.id ?? "toradex");
  const [roleFilter, setRoleFilter] = useState<BomRole | "all">("all");

  const supplier = useMemo(
    () => BOM_SUPPLIERS.find((s) => s.id === supplierId) ?? BOM_SUPPLIERS[0],
    [supplierId],
  );

  const kits = useMemo(() => {
    if (!supplier) return [];
    if (roleFilter === "all") return supplier.kits;
    return supplier.kits.filter((k) => k.role === roleFilter);
  }, [supplier, roleFilter]);

  return (
    <section className="section band" id="prototype-bom">
      <p className="section-label">Chapter 06 · Prototype BOM</p>
      <h2>Choose by supplier category</h2>
      <p className="section-intro">
        Eval kits bring up the SoM path. The field CCI is always a custom carrier (C1–C10). Validate
        datasheets before comparing vendors — {BOM_LEAD_PICK.rule}
      </p>

      <aside className="bom-lead" aria-label="Lead pick">
        <p className="bom-lead-label">Lead pick (Phase 1)</p>
        <p>
          <strong>{BOM_LEAD_PICK.kit}</strong>
          <span className="bom-lead-sep">+</span>
          {BOM_LEAD_PICK.som}
        </p>
      </aside>

      <div className="bom-supplier-tabs" role="tablist" aria-label="BOM supplier categories">
        {BOM_SUPPLIERS.map((s) => (
          <button
            key={s.id}
            type="button"
            role="tab"
            aria-selected={supplierId === s.id}
            className={`bom-tab${supplierId === s.id ? " active" : ""}${s.recommended ? " recommended" : ""}`}
            onClick={() => setSupplierId(s.id)}
          >
            <span className="bom-tab-vendor">{s.vendor}</span>
            {s.recommended ? <span className="bom-tab-badge">Lead</span> : null}
          </button>
        ))}
      </div>

      {supplier ? (
        <p className="bom-tagline">{supplier.tagline}</p>
      ) : null}

      <div className="bom-role-filters" role="group" aria-label="Filter by kit role">
        {BOM_ROLES.map((r) => (
          <button
            key={r.id}
            type="button"
            className={`bom-chip${roleFilter === r.id ? " active" : ""}`}
            onClick={() => setRoleFilter(r.id)}
          >
            {r.label}
          </button>
        ))}
      </div>

      <div className="bom-kit-grid">
        {kits.length === 0 ? (
          <p className="bom-empty">No kits in this role for {supplier?.vendor}.</p>
        ) : (
          kits.map((k) => (
            <article key={k.id} className="bom-kit">
              <header className="bom-kit-head">
                <h3>{k.name}</h3>
                <span className={`status-pill ${statusClass(k.docStatus)}`}>
                  {statusLabel(k.docStatus)}
                </span>
              </header>
              {k.pn ? (
                <p className="bom-pn">
                  PN <code>{k.pn}</code>
                </p>
              ) : null}
              <p className="bom-role-line">{k.roleLabel}</p>
              <p className="bom-pairs">
                Pairs with <span>{k.pairsWith}</span>
              </p>
              <p className="bom-note">{k.note}</p>
              <a className="bom-link" href={k.url} target="_blank" rel="noreferrer">
                Vendor page
              </a>
            </article>
          ))
        )}
      </div>

      <div className="bom-carrier-block">
        <h3>Custom carrier lines (after SoM lock)</h3>
        <p>
          Shared across all suppliers — order only after pin budget. Docs mostly still missing.
        </p>
        <ul className="bom-carrier-list">
          {BOM_CARRIER_LINES.map((line) => (
            <li key={line.id}>
              <span className="bom-carrier-cat">{line.category}</span>
              <span className="bom-carrier-items">{line.items}</span>
              <span className={`status-pill ${statusClass(line.docStatus)}`}>
                {statusLabel(line.docStatus)}
              </span>
            </li>
          ))}
        </ul>
      </div>

      <div className="bom-carrier-block" id="mocking-bench-bom">
        <h3>Mocking / PF2 bench BOM (lab CapEx)</h3>
        <p>
          Separate from the ~€1,000 product prototype. Wave A first — see{" "}
          <code>CCI_Mocking_Bench_BOM.md</code>.
        </p>
        <ul className="bom-carrier-list">
          {BENCH_BOM_WAVES.map((wave) => (
            <li key={wave.id}>
              <span className="bom-carrier-cat">
                Wave {wave.id} · {wave.level}
              </span>
              <span className="bom-carrier-items">
                {wave.goal} — {wave.buyNow.join("; ")} ({wave.estEur})
              </span>
              <span
                className={`status-pill ${
                  wave.status === "buy_now"
                    ? "ok"
                    : wave.status === "buy_next"
                      ? "warn"
                      : "miss"
                }`}
              >
                {wave.statusLabel}
              </span>
            </li>
          ))}
        </ul>
      </div>
    </section>
  );
}
