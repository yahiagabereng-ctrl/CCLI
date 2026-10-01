import { NavLink } from "react-router-dom";

export function Nav() {
  return (
    <header className="site-nav">
      <NavLink to="/" className="brand-lockup" end>
        <img src="/hiteks-logo.png" alt="HiTEKS" className="brand-logo" width={148} height={48} />
        <span className="brand-divider" aria-hidden="true" />
        <span className="brand-project">
          CCI <em>Knowledge</em>
        </span>
      </NavLink>
      <nav className="nav-links" aria-label="Primary">
        <a href="#overview">Overview</a>
        <a href="#system">System</a>
        <a href="#phases">Timeline</a>
        <a href="#firmware">Firmware</a>
        <a href="#corpus-analysis">Analysis</a>
      </nav>
    </header>
  );
}
