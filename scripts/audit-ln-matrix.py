#!/usr/bin/env python3
"""Audit TR 57-126 CID LNs vs mms_adapter runtime wiring (MVP vs r23 full cfg)."""
from __future__ import annotations

import argparse
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CID = ROOT / "apps/ccli/config/icd/lab_tg544_eth_a.cid"
CFG = ROOT / "apps/ccli/config/icd/lab_tg544_eth_a.cfg"

# Hand-built MVP nodes explicitly coded in mms_adapter.cpp (legacy path).
MVP_RUNTIME = {
    "LLN0": {"lnClass": "LLN0", "mvp": "IMPL", "note": "reports + DataSets"},
    "WlimDWMX1": {"lnClass": "DWMX", "mvp": "IMPL", "note": "Mod + WMaxSptPct APC"},
    "WSdDAGC1": {"lnClass": "DAGC", "mvp": "IMPL", "note": "Mod + WSptPct APC"},
    "WSaDAGC1": {"lnClass": "DAGC", "mvp": "STUB", "note": "Mod only; MVP-only, not in CID/cfg"},
    "VArSdDVAR1": {"lnClass": "DVAR", "mvp": "STUB", "note": "Mod only in MVP"},
    "PFSPDFPF1": {"lnClass": "DFPF", "mvp": "STUB", "note": "Mod only in MVP"},
    "VArVDVVR1": {"lnClass": "DVVR", "mvp": "STUB", "note": "Mod only in MVP"},
    "PFWDPFW1": {"lnClass": "DPFW", "mvp": "STUB", "note": "Mod only in MVP"},
    "PdCMMXU1": {"lnClass": "MMXU", "mvp": "PART", "note": "TotW+TotVAr+PPV.phsAB"},
}

# ccli code paths that refresh values or accept Operate (r23 full cfg).
LIVE_UPDATE = {"LLN0", "PdCMMXU1", "WlimDWMX1", "WSdDAGC1"}
CONTROL = {"WlimDWMX1", "WSdDAGC1"}
REACTIVE_CTRL_STRUCT = {"VArSdDVAR1", "PFSPDFPF1", "VArVDVVR1", "PFWDPFW1"}


def parse_cid_lns(text: str) -> dict[str, dict]:
    out: dict[str, dict] = {}
    for m in re.finditer(
        r'<LN\s+lnClass="([^"]+)"\s+lnType="([^"]+)"\s+inst="([^"]*)"\s+prefix="([^"]*)"',
        text,
    ):
        cls, lntype, inst, pref = m.groups()
        name = f"{pref}{cls}{inst}" if pref else f"{cls}{inst}"
        out[name] = {"lnClass": cls, "lnType": lntype, "prefix": pref, "inst": inst}
    if "<LN0" in text:
        out["LLN0"] = {"lnClass": "LLN0", "lnType": "LLN01", "prefix": "", "inst": ""}
    return out


def parse_cfg_lns(text: str) -> list[str]:
    return re.findall(r"^LN\(([^)]+)\)", text, re.MULTILINE)


def wire_tier(name: str, in_cfg: bool) -> str:
    if not in_cfg:
        return "—"
    if name in LIVE_UPDATE and name in CONTROL:
        return "W-L-C"
    if name in LIVE_UPDATE:
        return "W-L-"
    if name in REACTIVE_CTRL_STRUCT:
        return "W--C"
    return "W--S"


def main() -> None:
    parser = argparse.ArgumentParser(description="CID vs MMS runtime LN audit")
    parser.add_argument(
        "--r23",
        action="store_true",
        help="Show r23 full-cfg wire tiers (default when --r23 or always with tier column)",
    )
    args = parser.parse_args()

    cid_lns = parse_cid_lns(CID.read_text(encoding="utf-8", errors="replace"))
    cfg_lns = set(parse_cfg_lns(CFG.read_text(encoding="utf-8", errors="replace")))
    all_names = sorted(set(cid_lns) | set(MVP_RUNTIME))

    print("# LN audit — CID vs MMS runtime")
    print(f"# CID: {CID.name} — {len(cid_lns)} LNs")
    print(f"# CFG: {CFG.name} — {len(cfg_lns)} LNs (r23 model_cfg)")
    print(f"# MVP hand-built: {len(MVP_RUNTIME)} LNs (fallback if model_cfg empty)")
    print()
    print(
        "| # | MMS name | lnClass | CID | CFG | MVP | "
        "Wire (r23) | Live | Control | TSP note |"
    )
    print(
        "|---|----------|---------|-----|-----|-----|"
        "-----------|------|---------|----------|"
    )

    static_count = 0
    for i, name in enumerate(all_names, 1):
        c = cid_lns.get(name)
        m = MVP_RUNTIME.get(name)
        in_cid = "YES" if c else "—"
        in_cfg = "YES" if name in cfg_lns else "—"
        in_mvp = "YES" if m else "—"
        cls = (c or m or {}).get("lnClass", "?")
        tier = wire_tier(name, name in cfg_lns)
        live = "YES" if name in LIVE_UPDATE else "—"
        ctrl = "YES" if name in CONTROL else ("struct" if name in REACTIVE_CTRL_STRUCT else "—")

        if tier == "W--S":
            static_count += 1
            tsp = "static cfg; browse OK"
        elif tier == "W--C":
            tsp = "Operate no plant path"
        elif tier == "W-L-C":
            tsp = "Read+Operate+PF2"
        elif tier == "W-L-":
            tsp = "live Modbus/yaml"
        elif name == "WSaDAGC1":
            tsp = "MVP-only; absent r23"
        else:
            tsp = "not on wire"

        print(
            f"| {i} | `{name}` | {cls} | {in_cid} | {in_cfg} | {in_mvp} | "
            f"{tier} | {live} | {ctrl} | {tsp} |"
        )

    print()
    print("## r23 summary")
    print(f"- On MMS wire (CFG): **{len(cfg_lns)}** LNs")
    print(f"- Live-updated: **{len(LIVE_UPDATE)}** ({', '.join(sorted(LIVE_UPDATE))})")
    print(f"- Control handlers: **{len(CONTROL)}** ({', '.join(sorted(CONTROL))})")
    print(f"- Static on wire (W--S): **{static_count}** LNs")
    print(f"- Reactive struct only (W--C): **{len(REACTIVE_CTRL_STRUCT)}** LNs")
    print("- MVP-only (not in cfg): **WSaDAGC1**")
    print()
    print("Canonical TSP prep doc: lab/evidence/phase5/P5_LN_WIRE_STATUS.md")
    print()
    print("## Legacy MVP-only LNs (not in CID)")
    for name in sorted(MVP_RUNTIME):
        if name not in cid_lns:
            print(f"  - {name} ({MVP_RUNTIME[name]['lnClass']})")

    print()
    print("## CFG missing from CID (should be empty)")
    for name in sorted(cfg_lns):
        if name not in cid_lns:
            print(f"  - {name}")


if __name__ == "__main__":
    main()
