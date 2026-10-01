import { useEffect, useState } from "react";
import { ROADMAP_CHAPTERS } from "../lib/roadmapNav";

export function RoadmapNav() {
  const [active, setActive] = useState(ROADMAP_CHAPTERS[0].id);

  useEffect(() => {
    const sections = ROADMAP_CHAPTERS.map((c) => document.getElementById(c.id)).filter(
      Boolean,
    ) as HTMLElement[];

    const obs = new IntersectionObserver(
      (entries) => {
        const visible = entries
          .filter((e) => e.isIntersecting)
          .sort((a, b) => b.intersectionRatio - a.intersectionRatio);
        if (visible[0]?.target?.id) setActive(visible[0].target.id);
      },
      { rootMargin: "-20% 0px -55% 0px", threshold: [0.15, 0.4, 0.7] },
    );

    sections.forEach((el) => obs.observe(el));
    return () => obs.disconnect();
  }, []);

  return (
    <nav className="roadmap-nav" aria-label="Roadmap chapters">
      <p className="roadmap-nav-label">Roadmap</p>
      <ol className="roadmap-nav-list">
        {ROADMAP_CHAPTERS.map((c) => (
          <li key={c.id}>
            <a
              href={`#${c.id}`}
              className={active === c.id ? "active" : undefined}
              onClick={() => setActive(c.id)}
            >
              <span className="rn-index">{c.index}</span>
              <span className="rn-text">
                <span className="rn-title">{c.title}</span>
                <span className="rn-blurb">{c.blurb}</span>
              </span>
            </a>
          </li>
        ))}
      </ol>
    </nav>
  );
}
