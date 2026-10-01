import { BomSelector } from "../components/BomSelector";
import { CorpusAnalysis } from "../components/CorpusAnalysis";
import { RoadmapNav } from "../components/RoadmapNav";
import { VendorDecisionMatrix } from "../components/VendorDecisionMatrix";
import {
  CORPUS_HIGHLIGHTS,
  FIRMWARE_PILLARS,
  KB_BRANCHES,
  NEXT_STEPS,
  PHASE1_TIMELINE,
  ROADMAP_META,
  SYSTEM_BLOCKS,
} from "../lib/kbData";

export function KnowledgePage() {
  return (
    <div className="page">
      <section className="hero" aria-label="Introduction">
        <div className="hero-copy">
          <img
            src="/hiteks-logo.png"
            alt="HiTEKS"
            className="hero-logo"
            width={220}
            height={72}
          />
          <h1>CEI 0-16 Central Plant Controller</h1>
          <p className="hero-lead">
            Original design programme — architecture, delivery phases, and engineering knowledge
            structured for Architecture Freeze.
          </p>
          <div className="cta-row">
            <a className="btn btn-primary" href="#overview">
              Enter roadmap
            </a>
            <a className="btn btn-ghost" href="#system">
              System structure
            </a>
          </div>
        </div>
        <div className="hero-visual">
          <img
            src="/cci-architecture.png"
            alt="Integrated embedded system for renewable energy plant monitoring, control and secure operation"
            className="hero-arch-img"
          />
        </div>
      </section>

      <div className="roadmap-layout">
        <RoadmapNav />

        <div className="roadmap-main">
          <section className="section" id="overview">
            <p className="section-label">Chapter 01 · Overview</p>
            <h2>What HiTEKS is delivering</h2>
            <p className="section-intro">
              {ROADMAP_META.target}. {ROADMAP_META.range}. {ROADMAP_META.scope}.
            </p>
            <dl className="meta-grid">
              <div>
                <dt>Prototype</dt>
                <dd>{ROADMAP_META.prototypeBudget}</dd>
              </div>
              <div>
                <dt>Phase 1</dt>
                <dd>{ROADMAP_META.phase1}</dd>
              </div>
              <div>
                <dt>Phase 2</dt>
                <dd>{ROADMAP_META.phase2}</dd>
              </div>
            </dl>
            <p className="roadmap-note">
              The organisation already holds <strong>ATEX</strong> (EN/IEC 60079). Standard MT CCI
              cabinets are usually non-hazardous — confirm per site.{" "}
              <strong>Option A</strong> is original SoM + carrier (primary).{" "}
              <strong>Option B</strong> (clone / reverse engineering) is gated and documented
              separately.
            </p>
          </section>

          <section className="section band" id="system">
            <p className="section-label">Chapter 02 · System architecture</p>
            <h2>How the CCI is structured</h2>
            <p className="section-intro">
              Plant elements and measurements feed an SoM-based edge controller — Ethernet, RS-485,
              GNSS, DI/DO, secure boot — with MMS/GOOSE to the DSO and Eth_B for the authorised
              operator.
            </p>
            <figure className="arch-figure">
              <img
                src="/cci-architecture.png"
                alt="CCI architecture: plant elements, measurements, SoM embedded system, MMS/GOOSE DSO and operator"
              />
              <figcaption>
                Integrated embedded system — monitoring, control, and secure operation
              </figcaption>
            </figure>
            <div className="system-blocks">
              {SYSTEM_BLOCKS.map((b) => (
                <article key={b.block} className="system-block">
                  <h3>{b.block}</h3>
                  <p className="system-fn">{b.function}</p>
                  <p className="system-note">{b.note}</p>
                </article>
              ))}
            </div>
          </section>

          <section className="section" id="phases">
            <p className="section-label">Chapter 03 · Phase 1 timeline</p>
            <h2>Ten to twelve weeks to a DIN-rail prototype</h2>
            <p className="section-intro">
              Exit criteria: boots reliably, IEC 61850 MMS on Eth_A, Modbus analyzer on RS-485, GNSS
              timestamps, DO from a simulated DSO command — demo-ready, not yet field-certified.
            </p>
            <ol className="timeline">
              {PHASE1_TIMELINE.map((t) => (
                <li key={t.weeks}>
                  <span className="timeline-weeks">{t.weeks}</span>
                  <span className="timeline-text">{t.milestone}</span>
                </li>
              ))}
            </ol>
          </section>

          <section className="section band" id="firmware">
            <p className="section-label">Chapter 04 · Firmware architecture</p>
            <h2>Control path vs tooling path</h2>
            <p className="section-intro">
              Keep the certified boundary clean from day one — it reduces IEC 62443-4-2 scoping cost
              later.
            </p>
            <div className="pillar-grid">
              {FIRMWARE_PILLARS.map((p) => (
                <article key={p.title}>
                  <h3>{p.title}</h3>
                  <p>{p.body}</p>
                </article>
              ))}
            </div>
          </section>

          <section className="section" id="next">
            <p className="section-label">Chapter 05 · Next decisions</p>
            <h2>Before schematic freeze</h2>
            <ol className="next-list">
              {NEXT_STEPS.map((s) => (
                <li key={s}>{s}</li>
              ))}
            </ol>
          </section>

          <BomSelector />

          <VendorDecisionMatrix />

          <section className="section band" id="knowledge-tree">
            <p className="section-label">Chapter 08 · Knowledge base</p>
            <h2>Eight branches that mirror the build</h2>
            <p className="section-intro">
              Structured under <code>knowledge-base/</code> and mirrored for retrieval. Master paths
              are listed in DOWNLOADS.md.
            </p>
            <div className="kb-tree">
              {KB_BRANCHES.map((b) => (
                <article key={b.name} className="kb-branch">
                  <h3>{b.name}</h3>
                  <p>{b.summary}</p>
                  <span className={`status-pill ${b.status}`}>{b.statusLabel}</span>
                </article>
              ))}
            </div>
          </section>

          <section className="section" id="coverage">
            <p className="section-label">Chapter 09 · Corpus coverage</p>
            <h2>What already supports the roadmap</h2>
            <div className="corpus-grid">
              {CORPUS_HIGHLIGHTS.map((c) => (
                <div key={c.title} className="corpus-item">
                  <h3>{c.title}</h3>
                  <p>{c.body}</p>
                </div>
              ))}
            </div>
          </section>

          <CorpusAnalysis />
        </div>
      </div>

      <footer className="site-footer">
        <span className="footer-brand">
          <img src="/hiteks-logo.png" alt="HiTEKS" width={96} height={32} />
          <span>CCI programme knowledge</span>
        </span>
        <span>CEI 0-16 · OpenWrt TG-524 · Architecture Freeze</span>
      </footer>
    </div>
  );
}
