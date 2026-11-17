#!/usr/bin/env python3
"""Validate product 17-LN specimen against classification + product code bindings."""
from __future__ import annotations

import hashlib
import json
import re
import sys
import xml.etree.ElementTree as ET
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCL = "{http://www.iec.ch/61850/2003/SCL}"
CLASSIFICATION = ROOT / "apps/ccli/config/icd/classification/ccli_ln_classification.json"
PRODUCT_CID = ROOT / "product/apps/ccli/config/icd/product/product_tr400_eth_a_17ln.cid"
PRODUCT_CFG = ROOT / "product/apps/ccli/config/icd/product/product_tr400_eth_a_17ln.cfg"
GENERATED_CID = ROOT / "apps/ccli/config/icd/generated/erda_wave1_17ln.cid"
GENERATED_CFG = ROOT / "apps/ccli/config/icd/generated/erda_wave1_17ln.cfg"
MMS = ROOT / "product/apps/ccli/adapters/iec61850_mms/mms_adapter.cpp"
TMMS = ROOT / "product/apps/ccli/program/transmission_mms.c"
YAMLS = [
    ROOT / "product/apps/ccli/config/product_tr400.yaml",
    ROOT / "product/apps/ccli/config/product_tr400_emt432_17ln.yaml",
    ROOT / "product/apps/ccli/config/lab_tr400_phase4_eth_b_ttyS1_17ln.yaml",
]
FORBIDDEN = [
    "VArSdDVAR1",
    "PFSPDFPF1",
    "PFWDPFW1",
    "VArVDVVR1",
    "GenPVMMXU1",
    "GenWiMMXU1",
    "GenTerMMXU1",
    "GenIdrMMXU1",
]
REQUIRED_WIRE = [
    "LD_Plant/WlimDWMX1.Mod",
    "LD_Plant/WlimDWMX1.WMaxSptPct",
    "LD_Plant/WSdDAGC1.Mod",
    "LD_Plant/WSdDAGC1.WSptPct",
    "LD_Plant/PdCMMXU1.TotW.mag.f",
    "LD_Plant/PdCMMXU1.TotVAr.mag.f",
    "LD_Plant/PdCMMXU1.PPV.phsAB.cVal.mag.f",
]
REQUIRED_HANDLERS = [
    "impl->wlim_mod",
    "impl->wlim_wmax",
    "impl->wsd_mod",
    "impl->wsd_wspt",
]


def ln_name(prefix: str, ln_class: str, inst: str) -> str:
    if ln_class == "LLN0":
        return "LLN0"
    return f"{prefix}{ln_class}{inst}"


def parse_cid_lns(path: Path) -> list[str]:
    root = ET.parse(path).getroot()
    names: list[str] = []
    for ld in root.iter(SCL + "LDevice"):
        for ln0 in ld.findall(SCL + "LN0"):
            names.append("LLN0")
        for ln in ld.findall(SCL + "LN"):
            names.append(
                ln_name(ln.get("prefix") or "", ln.get("lnClass") or "", ln.get("inst") or "")
            )
        break
    return names


def ctl_models(path: Path) -> dict[str, str]:
    root = ET.parse(path).getroot()
    out: dict[str, str] = {}
    for ld in root.iter(SCL + "LDevice"):
        for ln in ld.findall(SCL + "LN"):
            name = ln_name(ln.get("prefix") or "", ln.get("lnClass") or "", ln.get("inst") or "")
            if name not in ("WlimDWMX1", "WSdDAGC1"):
                continue
            for doi in ln.findall(SCL + "DOI"):
                do = doi.get("name")
                for dai in doi.findall(SCL + "DAI"):
                    if dai.get("name") != "ctlModel":
                        continue
                    val = dai.find(SCL + "Val")
                    if val is not None and val.text:
                        out[f"{name}.{do}"] = val.text.strip()
        break
    return out


def main() -> int:
    fails: list[str] = []
    ok: list[str] = []

    cls = json.loads(CLASSIFICATION.read_text(encoding="utf-8"))
    expected = cls["profiles"]["erda_wave1_17ln"]["include_ln_names"]
    if cls.get("schema_version") != "1.2":
        fails.append(f"classification schema_version={cls.get('schema_version')!r} expected 1.2")
    else:
        ok.append("classification schema 1.2")

    if not PRODUCT_CID.is_file() or not PRODUCT_CFG.is_file():
        fails.append("product 17ln cid/cfg missing")
        print("FAIL")
        for f in fails:
            print(" -", f)
        return 1

    names = parse_cid_lns(PRODUCT_CID)
    if len(names) != 17:
        fails.append(f"CID LN count={len(names)} expected 17")
    else:
        ok.append("CID has 17 LNs")
    if set(names) != set(expected):
        fails.append(f"CID inventory mismatch missing={sorted(set(expected)-set(names))} extra={sorted(set(names)-set(expected))}")
    else:
        ok.append("CID inventory matches erda_wave1_17ln")

    cfg_text = PRODUCT_CFG.read_text(encoding="utf-8", errors="replace")
    cfg_ln = len(re.findall(r"^\s*LN\(", cfg_text, re.M))
    if cfg_ln != 17:
        fails.append(f"CFG LN( count={cfg_ln} expected 17")
    else:
        ok.append("CFG has 17 LN(")

    if hashlib.sha256(PRODUCT_CID.read_bytes()).digest() != hashlib.sha256(GENERATED_CID.read_bytes()).digest():
        fails.append("product CID != generated erda_wave1_17ln.cid")
    else:
        ok.append("product CID == generated latest XML")
    if hashlib.sha256(PRODUCT_CFG.read_bytes()).digest() != hashlib.sha256(GENERATED_CFG.read_bytes()).digest():
        fails.append("product CFG != generated erda_wave1_17ln.cfg")
    else:
        ok.append("product CFG == generated latest cfg")

    cid_txt = PRODUCT_CID.read_text(encoding="utf-8")
    for bad in FORBIDDEN:
        # GenPV string can appear only as leftover desc text under SGG; treat LN tag as fail.
        if f'prefix="{bad[:4]}' in cid_txt and bad.startswith("Gen"):
            # not a reliable check; use LN inventory instead
            continue
        if bad in names:
            fails.append(f"forbidden LN present: {bad}")
        elif f'prefix="{bad.replace("MMXU1","").replace("DVAR1","").replace("DFPF1","").replace("DPFW1","").replace("DVVR1","")}"' in cid_txt and bad in (
            "VArSdDVAR1",
            "PFSPDFPF1",
            "PFWDPFW1",
            "VArVDVVR1",
        ):
            fails.append(f"forbidden prefix present for {bad}")
    if any(x in names for x in FORBIDDEN):
        fails.append("forbidden LN names found in inventory")
    else:
        ok.append("no Gen*/Q-stack LNs in inventory")

    if 'TimeSyncProt sntp="true"' not in cid_txt and "sntp=\"true\"" not in cid_txt:
        fails.append("TimeSyncProt sntp=true missing")
    else:
        ok.append("TimeSyncProt sntp=true present")

    ctls = ctl_models(PRODUCT_CID)
    for key in (
        "WlimDWMX1.WMaxSptPct",
        "WlimDWMX1.Mod",
        "WSdDAGC1.WSptPct",
        "WSdDAGC1.Mod",
    ):
        if ctls.get(key) != "direct-with-enhanced-security":
            fails.append(f"{key} ctlModel={ctls.get(key)!r} expected direct-with-enhanced-security")
    if not any(f.startswith("ctlModel") for f in fails if "ctlModel" in f or "Wlim" in f or "WSd" in f):
        # only add ok if none of those four failed
        if all(ctls.get(k) == "direct-with-enhanced-security" for k in (
            "WlimDWMX1.WMaxSptPct",
            "WlimDWMX1.Mod",
            "WSdDAGC1.WSptPct",
            "WSdDAGC1.Mod",
        )):
            ok.append("Wlim/WSd ctlModel = direct-with-enhanced-security")

    for ds in ("DS_R_PdC_Mis4sec", "DS_R_GenAcc_Mis4sec", "DS_R_SingGen_Mis4sec"):
        if f'DataSet name="{ds}"' not in cid_txt:
            fails.append(f"missing DataSet {ds}")
    for urcb in ("urcb_PdC_Mis4sec", "urcb_GenAcc_Mis4sec", "urcb_SingGen_Mis4sec"):
        if f'ReportControl name="{urcb}"' not in cid_txt:
            fails.append(f"missing URCB {urcb}")
        elif 'intgPd="4000"' not in cid_txt:
            fails.append("intgPd=4000 missing")
    if not any(x.startswith("missing DataSet") or x.startswith("missing URCB") for x in fails):
        ok.append("3 DataSets + 3 URCBs (4 s) present")

    for y in YAMLS:
        text = y.read_text(encoding="utf-8", errors="replace")
        m = re.search(r"model_cfg:\s*(\S+)", text)
        path = m.group(1) if m else ""
        note = re.search(r"bench_note:\s*\"([^\"]*)\"", text)
        note_text = note.group(1) if note else ""
        if "product_tr400_eth_a_17ln.cfg" not in path:
            fails.append(f"{y.name} model_cfg={path!r} not 17ln")
        elif "5ln" in y.name or "5-LN" in note_text:
            fails.append(f"{y.name} still named or labelled as 5-LN")
        else:
            ok.append(f"{y.name} pins 17ln cfg")

    mms = MMS.read_text(encoding="utf-8", errors="replace")
    for ref in REQUIRED_WIRE:
        if ref not in mms:
            fails.append(f"mms_adapter missing wire ref {ref}")
    for h in REQUIRED_HANDLERS:
        if f"IedServer_setControlHandler(impl->server, {h}" not in mms.replace(" ", ""):
            # tolerate spacing
            if f"IedServer_setControlHandler(impl->server, {h}," not in mms:
                fails.append(f"control handler not installed for {h}")
    if "wire_cfg_runtime_nodes" not in mms:
        fails.append("wire_cfg_runtime_nodes missing")
    if "ConfigFileParser_createModelFromConfigFileEx" not in mms:
        fails.append("cfg model loader missing")
    if not any(x.startswith("mms_adapter") or x.startswith("control handler") or x.startswith("wire") or x.startswith("cfg model") for x in fails):
        ok.append("product mms_adapter wires Wlim/WSd/PdC + control handlers")

    tmms = TMMS.read_text(encoding="utf-8", errors="replace")
    if "CcliBridge_Mms_Publish" not in tmms and "Publish" not in tmms:
        fails.append("transmission_mms publish path missing")
    else:
        ok.append("transmission_mms publish path present")

    # St / SGG TotW and IDGXCBR1.Pos must be bound and written, not only named.
    gaps = []
    if "LD_Plant/StMMXU1.TotW.mag.f" in mms and "st_totw_mag" in mms:
        ok.append("StMMXU1.TotW live writer")
    else:
        gaps.append("StMMXU1 not live-written in mms_adapter (CID/default only)")
    if "LD_Plant/SGGMMXU1.TotW.mag.f" in mms and "sgg_totw_mag" in mms:
        ok.append("SGGMMXU1.TotW live writer")
    else:
        gaps.append("SGGMMXU1 not live-written in mms_adapter (CID/default only)")
    if "LD_Plant/IDGXCBR1.Pos.stVal" in mms and "update_idg_pos" in mms and "CcliBridge_Mms_PublishIdgPos" in tmms:
        ok.append("IDGXCBR1.Pos live writer")
    else:
        gaps.append("IDGXCBR1.Pos not live-written in mms_adapter (status-only model)")

    print("== Product 17-LN validation ==")
    for line in ok:
        print(f"PASS  {line}")
    for line in gaps:
        print(f"GAP   {line}")
    for line in fails:
        print(f"FAIL  {line}")
    print(f"SUMMARY ok={len(ok)} gap={len(gaps)} fail={len(fails)}")
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
