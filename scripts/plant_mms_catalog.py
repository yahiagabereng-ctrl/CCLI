#!/usr/bin/env python3
"""Expected MMS points derived from lab CID report datasets + DSO controls (schema v4)."""
from __future__ import annotations

import xml.etree.ElementTree as ET
from pathlib import Path

NS = {"scl": "http://www.iec.ch/61850/2003/SCL"}
ROOT = Path(__file__).resolve().parents[1]
CID = ROOT / "apps" / "ccli" / "config" / "icd" / "lab_tg544_eth_a.cid"
IED = "CCI016_01"
LD = "LD_Plant"
HOST = "192.168.10.1"
PORT = "3782"  # Product DSO MMS + TLS (lab TSP: client Port 102, TLS off)
DEV = "CCLI_LAB"
DID = "cci016-lab-01"

# Schema v4 — legacy TSP import uses first 15 columns; metadata columns appended.
MMS_HEADER: list[str] = [
    "Device",
    "DeviceID",
    "Host",
    "Port",
    "Name",
    "ObjectRef",
    "FC",
    "DataType",
    "Scale",
    "Unit",
    "Interval",
    "ChangeReport",
    "ReportKey",
    "Enabled",
    "Description",
    "AccessRole",
    "AccessRight",
    "AnnexScope",
    "OperateObjectRef",
    "AppGroup",
    "FunctionClass",
    "FunctionLabel",
    "LNClass",
    "LNRef",
    "Clause",
    "PointKind",
    "O11Priority",
    "O11PriorityLabel",
]

VALID_APP_GROUPS = frozenset({"A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8"})

# Annex O Table 1 (O.11) — lower index = higher priority.
# Aligned with signal_map.yaml controls.*.o11_priority and CCI_Annex_O_Extract.md §8.
O11_CONTROL_FUNCTIONS: dict[str, tuple[int, str]] = {
    "active_ctrl_w110": (1, "Autonomous V≈110% active power limit (W110)"),
    "active_ctrl_wlim": (2, "DSO active power limitation (Wlim)"),
    "active_ctrl_wsd": (3, "DSO active power modulation (WSd)"),
    "active_ctrl_wsa": (4, "MSD active power set-point (WSa)"),
    "reactive_ctrl_varsd": (5, "DSO reactive power setpoint (VArSd)"),
    "reactive_ctrl_pfsp": (6, "Fixed cos phi setpoint (PFSP)"),
    "reactive_ctrl_varv": (6, "Reactive Q(V) control (VArV)"),
    "reactive_ctrl_pfw": (6, "Reactive cosφ(P) control (PFW)"),
    "reactive_ctrl_varsa": (7, "MSD reactive power set-point (VArSa)"),
}

# (app_group, function_class, ln_class, clause) — matched by LN instance name prefix/order.
LN_CLASS_RULES: list[tuple[str, str, str, str, str]] = [
    ("LLN0", "A0", "common_controller", "LLN0", "T.3.3.1"),
    ("LPHD1", "A0", "common_controller", "LPHD", "T.3.3.4.7"),
    ("PdC_WiDPCC", "A1", "nameplate_pdc", "DPCC", "T.3.1"),
    ("PdC_WaDPCC", "A1", "nameplate_pdc", "DPCC", "T.3.1"),
    ("PdC_QiDPCC", "A1", "nameplate_pdc", "DPCC", "T.3.1"),
    ("PdC_QcDPCC", "A1", "nameplate_pdc", "DPCC", "T.3.1"),
    ("PdC_VADPCC", "A1", "nameplate_pdc", "DPCC", "T.3.1"),
    ("DisFRDECP", "A2", "availability_plant", "DECP", "O.8.6"),
    ("DisFRDGEN", "A2", "availability_generation", "DGEN", "O.8.6"),
    ("DisFRDSTO", "A2", "availability_storage", "DSTO", "O.8.6"),
    ("PdCMMXU", "A3", "measurement_pdc", "MMXU", "O.8.3"),
    ("GenPVMMXU", "A3", "measurement_gen_pv", "MMXU", "O.8.4"),
    ("GenTerMMXU", "A3", "measurement_gen_thermal", "MMXU", "O.8.4"),
    ("GenIdrMMXU", "A3", "measurement_gen_hydro", "MMXU", "O.8.4"),
    ("StMMXU", "A3", "measurement_storage", "MMXU", "O.8.4"),
    ("SGGMMXU", "A3", "measurement_sgg", "MMXU", "O.8.4"),
    ("SSGGDGEN", "A5", "gen_group_status", "DGEN", "T.3.3.1"),
    ("IDGXCBR", "A4", "breaker_idg", "XCBR", "O.8.6"),
    ("WlimDWMX", "A6", "active_ctrl_wlim", "DWMX", "O.9.2.2"),
    ("WSdDAGC", "A6", "active_ctrl_wsd", "DAGC", "O.9.2.3"),
    ("VArSdDVAR", "A7", "reactive_ctrl_varsd", "DVAR", "O.9.1.4"),
    ("PFSPDFPF", "A7", "reactive_ctrl_pfsp", "DFPF", "O.9.1.1"),
    ("VArVDVVR", "A7", "reactive_ctrl_varv", "DVVR", "O.9.1.3"),
    ("PFWDPFW", "A7", "reactive_ctrl_pfw", "DPFW", "O.9.1.2"),
]

FUNCTION_LABELS: dict[str, str] = {
    "common_controller": "Controller and device identity",
    "nameplate_pdc": "Nameplate — plant power limits (Smax polygon)",
    "availability_plant": "Grid connection — ready for control",
    "availability_generation": "Generation block — ready for control",
    "availability_storage": "Storage block — ready for control",
    "measurement_pdc": "POC measurements — every 4 s",
    "measurement_gen_pv": "PV total power — every 4 s",
    "measurement_gen_thermal": "Thermal total power — every 4 s",
    "measurement_gen_hydro": "Hydro total power — every 4 s",
    "measurement_storage": "Battery storage power — every 4 s",
    "measurement_sgg": "Inverter group power — every 4 s",
    "gen_group_status": "Inverter group ID and health",
    "breaker_idg": "Main grid breaker position",
    "active_ctrl_wlim": "DSO power limit (Wlim)",
    "active_ctrl_wsd": "DSO power modulation (WSd)",
    "reactive_ctrl_varsd": "DSO reactive setpoint (VArSd)",
    "reactive_ctrl_pfsp": "Fixed cos phi (PFSP) — future",
    "reactive_ctrl_varv": "Reactive Q(V) curve (VArV) — future",
    "reactive_ctrl_pfw": "Cos phi vs power (PFW) — future",
}

# DO suffix rules for TestSuite Pro ObjectRef paths (lab-verified where noted).
DO_SUFFIX: dict[tuple[str, str], tuple[str, str, str]] = {
    ("TotW", "MX"): (".mag.f", "float32", "kW"),
    ("TotVAr", "MX"): (".mag.f", "float32", "kvar"),
    ("PPV", "MX"): (".phsAB.cVal.mag.f", "float32", "kV"),
    ("A", "MX"): (".phsA.cVal.mag.f", "float32", "A"),
    ("Beh", "ST"): (".stVal", "int32", ""),
    ("Pos", "ST"): (".stVal", "int32", ""),
    ("Health", "ST"): (".stVal", "int32", ""),
    ("FctOpSt", "ST"): (".stVal", "int32", ""),
    ("FctOpStAuto", "ST"): (".stVal", "int32", ""),
    ("FctOpStEx", "ST"): (".stVal", "int32", ""),
    ("GnGrId", "ST"): (".stVal", "int32", ""),
    ("Mod", "ST"): (".stVal", "int32", ""),
    ("PhyHealth", "ST"): (".stVal", "int32", ""),
    ("WMaxSptPct", "MX"): (".mxVal.f", "float32", "%"),
    ("WSptPct", "MX"): (".mxVal.f", "float32", "%"),
    ("VArTgtSptPct", "MX"): (".mxVal.f", "float32", "%"),
    ("PFGnTgtSpt", "MX"): (".mxVal.f", "float32", ""),
    ("PFLodTgtSpt", "MX"): (".mxVal.f", "float32", ""),
    ("WRtg", "SP"): (".setMag.f", "float32", "kW"),
    ("VArRtg", "SP"): (".setMag.f", "float32", "kVAr"),
    ("VARtg", "SP"): (".setMag.f", "float32", "kVA"),
    ("configRev", "ST"): (".stVal", "int32", ""),
}

# Quality + timestamp leaf paths per MX DO (Annex O.8.3 / T.3.3.1).
QT_SUFFIX: dict[str, tuple[str, str]] = {
    "TotW": (".q", ".t"),
    "TotVAr": (".q", ".t"),
    "PPV": (".phsAB.q", ".phsAB.t"),
    "A": (".phsA.q", ".phsA.t"),
}

FRIENDLY_NAMES: dict[str, str] = {
    "PdCMMXU1_TotW": "PdC_TotW",
    "PdCMMXU1_TotVAr": "PdC_TotVAr",
    "PdCMMXU1_PPV": "PdC_PPV_AB",
    "PdCMMXU1_A": "PdC_A_phsA",
    "GenPVMMXU1_TotW": "GenPV_TotW",
    "GenTerMMXU1_TotW": "GenTer_TotW",
    "GenIdrMMXU1_TotW": "GenIdr_TotW",
    "StMMXU1_TotW": "St_TotW",
    "SGGMMXU1_TotW": "SGG1_TotW",
    "SGGMMXU2_TotW": "SGG2_TotW",
    "IDGXCBR1_Pos": "IDG_Pos",
    "WlimDWMX1_Beh": "Wlim_Beh",
    "WSdDAGC1_Beh": "WSd_Beh",
    "VArSdDVAR1_Beh": "VArSd_Beh",
    "DisFRDECP1_Beh": "DisFR_DECP_Beh",
    "DisFRDGEN1_Beh": "DisFR_DGEN_Beh",
    "DisFRDSTO1_Beh": "DisFR_DSTO_Beh",
    "SSGGDGEN1_Health": "SSGG1_Health",
    "SSGGDGEN2_Health": "SSGG2_Health",
    "SSGGDGEN1_GnGrId": "SSGG1_GnGrId",
    "SSGGDGEN2_GnGrId": "SSGG2_GnGrId",
    "WlimDWMX1_FctOpStAuto": "Wlim_FctOpStAuto",
    "WlimDWMX1_FctOpStEx": "Wlim_FctOpStEx",
    "WSdDAGC1_FctOpSt": "WSd_FctOpSt",
    "VArSdDVAR1_FctOpSt": "VArSd_FctOpSt",
    "PFSPDFPF1_Beh": "PFSP_Beh",
    "PFSPDFPF1_FctOpSt": "PFSP_FctOpSt",
    "VArVDVVR1_Beh": "VArV_Beh",
    "VArVDVVR1_FctOpSt": "VArV_FctOpSt",
    "PFWDPFW1_Beh": "PFW_Beh",
    "PFWDPFW1_FctOpSt": "PFW_FctOpSt",
}

DATASET_REPORT: dict[str, tuple[str, str, str]] = {
    "DS_R_PdC_Mis4sec": ("urcb_PdC_Mis4sec", "4", "N"),
    "DS_R_GenAcc_Mis4sec": ("urcb_GenAcc_Mis4sec", "4", "N"),
    "DS_R_SingGen_Mis4sec": ("urcb_SingGen_Mis4sec", "4", "N"),
    "DS_R_Stato_Allarmi_Segnali": ("brcb_Stato_Allarmi", "60", "Y"),
}

# Plain operator meaning for every Name tag.
OPERATOR_DESCRIPTIONS: dict[str, str] = {
    # POC values — every 4 s
    "PdC_TotW": "POC active power — how much kW the plant exports or imports",
    "PdC_TotVAr": "POC reactive power — kVAr at the grid connection point",
    "PdC_PPV_AB": "POC line voltage — kV between phases A and B",
    "PdC_A_phsA": "POC phase-A current — amps on the delivery point",
    "PdC_TotW_q": "POC active power quality — good, questionable, or invalid",
    "PdC_TotVAr_q": "POC reactive power quality — good, questionable, or invalid",
    "PdC_PPV_AB_q": "POC voltage quality — good, questionable, or invalid",
    "PdC_A_phsA_q": "POC current quality — good, questionable, or invalid",
    "PdC_TotW_t": "POC active power timestamp — when the 4 s sample was taken",
    "PdC_TotVAr_t": "POC reactive power timestamp — when the 4 s sample was taken",
    "PdC_PPV_AB_t": "POC voltage timestamp — when the 4 s sample was taken",
    "PdC_A_phsA_t": "POC current timestamp — when the 4 s sample was taken",
    # Plant totals by source
    "GenPV_TotW": "Total PV generation — sum of all solar inverters (kW)",
    "GenTer_TotW": "Total thermal generation — combined heat/power (kW)",
    "GenIdr_TotW": "Total hydro generation — combined hydro units (kW)",
    "St_TotW": "Storage power — charge (+) or discharge (−) in kW",
    "GenPV_TotW_q": "PV total power quality flag",
    "GenPV_TotW_t": "PV total power sample time",
    "GenTer_TotW_q": "Thermal total power quality flag",
    "GenTer_TotW_t": "Thermal total power sample time",
    "GenIdr_TotW_q": "Hydro total power quality flag",
    "GenIdr_TotW_t": "Hydro total power sample time",
    "St_TotW_q": "Storage power quality flag",
    "St_TotW_t": "Storage power sample time",
    # Single generator groups
    "SGG1_TotW": "Inverter group 1 — active power (kW)",
    "SGG2_TotW": "Inverter group 2 — active power (kW)",
    "SGG1_TotW_q": "Inverter group 1 — power quality flag",
    "SGG1_TotW_t": "Inverter group 1 — power sample time",
    "SGG2_TotW_q": "Inverter group 2 — power quality flag",
    "SGG2_TotW_t": "Inverter group 2 — power sample time",
    "SSGG1_GnGrId": "Inverter group 1 — numeric group ID",
    "SSGG2_GnGrId": "Inverter group 2 — numeric group ID",
    # Nameplate (Table 80)
    "PdC_Wi_WRtg": "Nameplate — maximum export power allowed (kW)",
    "PdC_Wa_WRtg": "Nameplate — maximum import power allowed (kW)",
    "PdC_Qi_VArRtg": "Nameplate — maximum inductive reactive (kVAr)",
    "PdC_Qc_VArRtg": "Nameplate — maximum capacitive reactive (kVAr)",
    "PdC_VA_VARtg": "Nameplate — maximum apparent power Smax (kVA)",
    # DSO active power — read status
    "Wlim_Mod": "Power limit mode — read ON/OFF state (1=on, 5=off)",
    "Wlim_WMax": "Power limit setpoint — read current limit (% of Smax)",
    "Wlim_Beh": "Power limit health — running OK or fault",
    "Wlim_FctOpStAuto": "Power limit — automatic mode active",
    "Wlim_FctOpStEx": "Power limit — DSO external control active",
    "WSd_Mod": "Power modulation mode — read ON/OFF state",
    "WSd_WSpt": "Power modulation setpoint — read target (% of Smax)",
    "WSd_Beh": "Power modulation health — running OK or fault",
    "WSd_FctOpSt": "Power modulation — active control state",
    # DSO reactive — read status
    "VArSd_Mod": "Reactive setpoint mode — read ON/OFF state",
    "VArSd_VArTgt": "Reactive setpoint — read target (% of Smax)",
    "VArSd_Beh": "Reactive control health — running OK or fault",
    "VArSd_FctOpSt": "Reactive control — active state",
    # DSO control — Operate (DSO_OPERATOR only)
    "Wlim_Mod_Operate": "DSO command — turn power limit function ON or OFF",
    "Wlim_WMax_Operate": "DSO command — set power export limit (% of Smax)",
    "WSd_Mod_Operate": "DSO command — turn power modulation ON or OFF",
    "WSd_WSpt_Operate": "DSO command — set power modulation target (% of Smax)",
    "VArSd_Mod_Operate": "DSO command — turn reactive setpoint ON or OFF",
    "VArSd_VArTgt_Operate": "DSO command — set reactive target (% of Smax)",
    "PFSP_Mod_Operate": "DSO command — fixed cos phi ON/OFF (future)",
    "VArV_Mod_Operate": "DSO command — Q(V) curve ON/OFF (future)",
    "PFW_Mod_Operate": "DSO command — cos phi(P) curve ON/OFF (future)",
    # Future reactive modes — read
    "PFSP_Mod": "Fixed cos phi mode — read ON/OFF (future function)",
    "PFSP_Beh": "Fixed cos phi health — running OK or fault",
    "PFSP_FctOpSt": "Fixed cos phi — active state",
    "PFSP_PFGnTgt": "Fixed cos phi — target when exporting",
    "PFSP_PFLodTgt": "Fixed cos phi — target when importing",
    "VArV_Mod": "Q(V) mode — read ON/OFF (future function)",
    "VArV_Beh": "Q(V) health — running OK or fault",
    "VArV_FctOpSt": "Q(V) — active state",
    "PFW_Mod": "Cos phi(P) mode — read ON/OFF (future function)",
    "PFW_Beh": "Cos phi(P) health — running OK or fault",
    "PFW_FctOpSt": "Cos phi(P) — active state",
    # Availability and breaker
    "DisFR_DECP_Beh": "Grid connection point — available for control",
    "DisFR_DGEN_Beh": "Generation block — available for control",
    "DisFR_DSTO_Beh": "Storage block — available for control",
    "IDG_Pos": "Main grid breaker — open or closed",
    "SSGG1_Health": "Inverter group 1 — health OK or fault",
    "SSGG2_Health": "Inverter group 2 — health OK or fault",
    # Controller identity
    "LLN0_Mod": "Controller mode — logical device ON or OFF",
    "LLN0_configRev": "Controller config version number",
    "LPHD_PhyHealth": "Controller hardware health — OK or fault",
    "LPHD_vendor": "Device manufacturer name (asset inventory)",
    "LPHD_location": "Delivery point ID / POD code (asset inventory)",
    "LPHD_swRev": "Controller firmware version string",
}

# Bases that must have _q and _t companion rows (Annex O.8.3).
QT_MEASUREMENT_BASES: tuple[str, ...] = (
    "PdC_TotW",
    "PdC_TotVAr",
    "PdC_PPV_AB",
    "PdC_A_phsA",
    "GenPV_TotW",
    "GenTer_TotW",
    "GenIdr_TotW",
    "St_TotW",
    "SGG1_TotW",
    "SGG2_TotW",
)

# DSO Operate targets (Annex T Table 98 / T.3.3.4.3 RBAC).
OPERATE_SPECS: list[tuple[str, str, str, str, str, str]] = [
    # name, ln, do, datatype, enabled, interval
    ("Wlim_Mod_Operate", "WlimDWMX1", "Mod", "int32", "Y", "60"),
    ("Wlim_WMax_Operate", "WlimDWMX1", "WMaxSptPct", "float32", "Y", "60"),
    ("WSd_Mod_Operate", "WSdDAGC1", "Mod", "int32", "Y", "60"),
    ("WSd_WSpt_Operate", "WSdDAGC1", "WSptPct", "float32", "Y", "60"),
    ("VArSd_Mod_Operate", "VArSdDVAR1", "Mod", "int32", "Y", "60"),
    ("VArSd_VArTgt_Operate", "VArSdDVAR1", "VArTgtSptPct", "float32", "Y", "60"),
    ("PFSP_Mod_Operate", "PFSPDFPF1", "Mod", "int32", "N", "60"),
    ("VArV_Mod_Operate", "VArVDVVR1", "Mod", "int32", "N", "60"),
    ("PFW_Mod_Operate", "PFWDPFW1", "Mod", "int32", "N", "60"),
]


def ln_name(prefix: str, ln_class: str, inst: str) -> str:
    return f"{prefix}{ln_class}{inst}"


def parse_ln_from_object_ref(object_ref: str) -> tuple[str, str]:
    """Return (LNRef, ln_instance) e.g. (LD_Plant/WlimDWMX1, WlimDWMX1)."""
    if "/" not in object_ref:
        return "", ""
    after_ld = object_ref.split("/", 1)[1]
    ln_inst = after_ld.split(".", 1)[0]
    return f"{LD}/{ln_inst}", ln_inst


def classify_ln(ln_inst: str) -> tuple[str, str, str, str]:
    """Return (app_group, function_class, ln_class, clause)."""
    for prefix, app_group, function_class, ln_class, clause in LN_CLASS_RULES:
        if ln_inst == prefix or ln_inst.startswith(prefix):
            return app_group, function_class, ln_class, clause
    return "A0", "common_controller", "UNKNOWN", "T.3.3.1"


def infer_point_kind(name: str, fc: str, access_right: str) -> str:
    if access_right == "CONTROL" or fc == "CO":
        return "operate"
    if name.endswith("_q"):
        return "quality"
    if name.endswith("_t"):
        return "timestamp"
    if fc == "SP":
        return "nameplate"
    if fc == "DC":
        return "asset"
    if fc == "MX" and any(
        token in name
        for token in ("WMax", "WSpt", "VArTgt", "PFGn", "PFLod")
    ):
        return "setpoint_read"
    if fc == "ST":
        return "status"
    if fc == "MX":
        return "value"
    return "other"


def apply_function_class(row: dict[str, str]) -> None:
    ln_ref, ln_inst = parse_ln_from_object_ref(row.get("ObjectRef", ""))
    app_group, function_class, ln_class, clause = classify_ln(ln_inst)
    row["AppGroup"] = app_group
    row["FunctionClass"] = function_class
    row["FunctionLabel"] = FUNCTION_LABELS.get(function_class, function_class.replace("_", " "))
    row["LNClass"] = ln_class
    row["LNRef"] = ln_ref
    row["Clause"] = clause
    row["PointKind"] = infer_point_kind(
        row.get("Name", ""), row.get("FC", ""), row.get("AccessRight", "")
    )
    apply_o11_priority(row)


def apply_o11_priority(row: dict[str, str]) -> None:
    """Set O.11 priority only on control-function rows (Annex O Table 1)."""
    function_class = row.get("FunctionClass", "")
    if function_class in O11_CONTROL_FUNCTIONS:
        idx, label = O11_CONTROL_FUNCTIONS[function_class]
        row["O11Priority"] = str(idx)
        row["O11PriorityLabel"] = label
    else:
        row["O11Priority"] = ""
        row["O11PriorityLabel"] = ""


def _point(
    name: str,
    object_ref: str,
    fc: str,
    dtype: str,
    unit: str,
    report_key: str,
    interval: str,
    change_report: str,
    enabled: str,
    description: str,
    *,
    access_role: str = "ANY",
    access_right: str = "READ",
    annex_scope: str = "OBSERVABILITY",
    operate_object_ref: str = "",
) -> dict[str, str]:
    return {
        "Device": DEV,
        "DeviceID": DID,
        "Host": HOST,
        "Port": PORT,
        "Name": name,
        "ObjectRef": object_ref,
        "FC": fc,
        "DataType": dtype,
        "Scale": "1",
        "Unit": unit,
        "Interval": interval,
        "ChangeReport": change_report,
        "ReportKey": report_key,
        "Enabled": enabled,
        "Description": description,
        "AccessRole": access_role,
        "AccessRight": access_right,
        "AnnexScope": annex_scope,
        "OperateObjectRef": operate_object_ref,
        "AppGroup": "",
        "FunctionClass": "",
        "FunctionLabel": "",
        "LNClass": "",
        "LNRef": "",
        "Clause": "",
        "PointKind": "",
        "O11Priority": "",
        "O11PriorityLabel": "",
    }


def _value_point(
    name: str,
    ln: str,
    do: str,
    fc: str,
    report_key: str,
    interval: str,
    change_report: str,
    enabled: str,
    *,
    access_role: str = "ANY",
    access_right: str = "READ",
    annex_scope: str = "OBSERVABILITY",
) -> dict[str, str]:
    suffix, dtype, unit = DO_SUFFIX.get((do, fc), (".stVal" if fc == "ST" else ".mag.f", "float32", ""))
    tag = name or f"{ln}_{do}"
    desc = OPERATOR_DESCRIPTIONS.get(tag, f"{ln}.{do} — measured value")
    return _point(
        tag,
        f"{IED}{LD}/{ln}.{do}{suffix}",
        fc,
        dtype,
        unit,
        report_key,
        interval,
        change_report,
        enabled,
        desc,
        access_role=access_role,
        access_right=access_right,
        annex_scope=annex_scope,
    )


def _qt_companions(
    base_name: str,
    ln: str,
    do: str,
    report_key: str,
    interval: str,
    change_report: str,
    enabled: str,
    annex_scope: str = "OBSERVABILITY",
) -> list[dict[str, str]]:
    q_suffix, t_suffix = QT_SUFFIX[do]
    q_name = f"{base_name}_q"
    t_name = f"{base_name}_t"
    return [
        _point(
            q_name,
            f"{IED}{LD}/{ln}.{do}{q_suffix}",
            "MX",
            "int32",
            "",
            report_key,
            interval,
            change_report,
            enabled,
            OPERATOR_DESCRIPTIONS.get(q_name, f"{base_name} quality"),
            annex_scope=annex_scope,
        ),
        _point(
            t_name,
            f"{IED}{LD}/{ln}.{do}{t_suffix}",
            "MX",
            "int64",
            "",
            report_key,
            interval,
            change_report,
            enabled,
            OPERATOR_DESCRIPTIONS.get(t_name, f"{base_name} timestamp"),
            annex_scope=annex_scope,
        ),
    ]


def _da_point(
    name: str,
    ln: str,
    do: str,
    da: str,
    fc: str,
    dtype: str,
    report_key: str,
    interval: str,
    change_report: str,
    enabled: str,
) -> dict[str, str]:
    desc = OPERATOR_DESCRIPTIONS.get(name, f"{ln}.{do}.{da}")
    return _point(
        name,
        f"{IED}{LD}/{ln}.{do}.{da}",
        fc,
        dtype,
        "",
        report_key,
        interval,
        change_report,
        enabled,
        desc,
    )


def parse_cid_datasets(cid_path: Path = CID) -> list[dict[str, str]]:
    tree = ET.parse(cid_path)
    points: list[dict[str, str]] = []
    for ds in tree.findall(".//scl:DataSet", NS):
        ds_name = ds.get("name", "")
        if ds_name not in DATASET_REPORT:
            continue
        rkey, interval, chg = DATASET_REPORT[ds_name]
        for fcda in ds.findall("scl:FCDA", NS):
            prefix = fcda.get("prefix", "")
            ln_class = fcda.get("lnClass", "")
            inst = fcda.get("lnInst", "")
            do = fcda.get("doName", "")
            fc = fcda.get("fc", "ST")
            ln = ln_name(prefix, ln_class, inst)
            raw = f"{ln}_{do}"
            name = FRIENDLY_NAMES.get(raw, raw)
            points.append(
                _value_point(name, ln, do, fc, rkey, interval, chg, "Y")
            )
            if fc == "MX" and do in QT_SUFFIX:
                points.extend(
                    _qt_companions(name, ln, do, rkey, interval, chg, "Y")
                )
    return points


def manual_points() -> list[dict[str, str]]:
    """Control reads, nameplate, asset inventory, and health outside report datasets."""
    t98 = "T98_DSO"
    specs: list[tuple] = [
        ("LLN0_Mod", "LLN0", "Mod", "ST", "brcb_Stato_Allarmi", "300", "N", "Y"),
        ("LLN0_configRev", "LLN0", "configRev", "ST", "brcb_Stato_Allarmi", "300", "N", "N"),
        ("LPHD_PhyHealth", "LPHD1", "PhyHealth", "ST", "brcb_Stato_Allarmi", "300", "Y", "Y"),
        ("PdC_Wi_WRtg", "PdC_WiDPCC1", "WRtg", "SP", "nameplate", "3600", "N", "Y"),
        ("PdC_Wa_WRtg", "PdC_WaDPCC1", "WRtg", "SP", "nameplate", "3600", "N", "Y"),
        ("PdC_Qi_VArRtg", "PdC_QiDPCC1", "VArRtg", "SP", "nameplate", "3600", "N", "Y"),
        ("PdC_Qc_VArRtg", "PdC_QcDPCC1", "VArRtg", "SP", "nameplate", "3600", "N", "Y"),
        ("PdC_VA_VARtg", "PdC_VADPCC1", "VARtg", "SP", "nameplate", "3600", "N", "Y"),
        ("Wlim_Mod", "WlimDWMX1", "Mod", "ST", "brcb_Stato_Allarmi", "60", "Y", "Y"),
        ("Wlim_WMax", "WlimDWMX1", "WMaxSptPct", "MX", "brcb_Stato_Allarmi", "60", "Y", "Y"),
        ("WSd_Mod", "WSdDAGC1", "Mod", "ST", "brcb_Stato_Allarmi", "60", "Y", "Y"),
        ("WSd_WSpt", "WSdDAGC1", "WSptPct", "MX", "brcb_Stato_Allarmi", "60", "Y", "Y"),
        ("VArSd_Mod", "VArSdDVAR1", "Mod", "ST", "brcb_Stato_Allarmi", "60", "Y", "Y"),
        ("VArSd_VArTgt", "VArSdDVAR1", "VArTgtSptPct", "MX", "brcb_Stato_Allarmi", "60", "Y", "Y"),
        ("PFSP_Mod", "PFSPDFPF1", "Mod", "ST", "brcb_Stato_Allarmi", "60", "Y", "N"),
        ("PFSP_PFGnTgt", "PFSPDFPF1", "PFGnTgtSpt", "MX", "brcb_Stato_Allarmi", "60", "Y", "N"),
        ("PFSP_PFLodTgt", "PFSPDFPF1", "PFLodTgtSpt", "MX", "brcb_Stato_Allarmi", "60", "Y", "N"),
        ("VArV_Mod", "VArVDVVR1", "Mod", "ST", "brcb_Stato_Allarmi", "60", "Y", "N"),
        ("PFW_Mod", "PFWDPFW1", "Mod", "ST", "brcb_Stato_Allarmi", "60", "Y", "N"),
    ]
    out: list[dict[str, str]] = []
    for name, ln, do, fc, rkey, interval, chg, enabled in specs:
        annex = t98 if name.startswith(("Wlim_", "WSd_", "VArSd_", "PFSP_", "VArV_", "PFW_")) else "OBSERVABILITY"
        out.append(
            _value_point(
                name,
                ln,
                do,
                fc,
                rkey,
                interval,
                chg,
                enabled,
                annex_scope=annex,
            )
        )

    out.extend(
        [
            _da_point(
                "LPHD_vendor",
                "LPHD1",
                "PhyNam",
                "vendor",
                "DC",
                "string",
                "nameplate",
                "3600",
                "N",
                "Y",
            ),
            _da_point(
                "LPHD_location",
                "LPHD1",
                "PhyNam",
                "location",
                "DC",
                "string",
                "nameplate",
                "3600",
                "N",
                "Y",
            ),
            _da_point(
                "LPHD_swRev",
                "LPHD1",
                "PhyNam",
                "swRev",
                "DC",
                "string",
                "nameplate",
                "3600",
                "N",
                "Y",
            ),
        ]
    )
    return out


def operate_points() -> list[dict[str, str]]:
    out: list[dict[str, str]] = []
    for name, ln, do, dtype, enabled, interval in OPERATE_SPECS:
        ctl_ref = f"{IED}{LD}/{ln}.{do}"
        unit = DO_SUFFIX.get((do, "MX"), ("", dtype, ""))[2]
        out.append(
            _point(
                name,
                ctl_ref,
                "CO",
                dtype,
                unit,
                "brcb_Stato_Allarmi",
                interval,
                "Y",
                enabled,
                OPERATOR_DESCRIPTIONS.get(name, f"DSO Operate {ln}.{do}"),
                access_role="DSO_OPERATOR",
                access_right="CONTROL",
                annex_scope="T98_DSO",
                operate_object_ref=ctl_ref,
            )
        )
    return out


def build_full_mms_catalog(port: str = PORT) -> list[dict[str, str]]:
    """Merge CID dataset members + q/t + manual + Operate rows; dedupe by ObjectRef."""
    merged: dict[str, dict[str, str]] = {}
    for p in parse_cid_datasets():
        merged[p["ObjectRef"]] = p
    for p in manual_points():
        merged[p["ObjectRef"]] = p
    for p in operate_points():
        merged[p["ObjectRef"]] = p
    rows = sorted(merged.values(), key=lambda r: r["Name"])
    for row in rows:
        row["Port"] = port
        if row["Name"] in OPERATOR_DESCRIPTIONS:
            row["Description"] = OPERATOR_DESCRIPTIONS[row["Name"]]
        apply_function_class(row)
    return rows


def expected_qt_companion_names() -> set[str]:
    names: set[str] = set()
    for base in QT_MEASUREMENT_BASES:
        names.add(f"{base}_q")
        names.add(f"{base}_t")
    return names


def expected_operate_names() -> set[str]:
    return {spec[0] for spec in OPERATE_SPECS}


def expected_object_refs(enabled_only: bool = False) -> set[str]:
    refs: set[str] = set()
    for p in build_full_mms_catalog():
        if enabled_only and p.get("Enabled", "").upper() != "Y":
            continue
        refs.add(p["ObjectRef"])
    return refs


def expected_names(enabled_only: bool = False) -> set[str]:
    names: set[str] = set()
    for p in build_full_mms_catalog():
        if enabled_only and p.get("Enabled", "").upper() != "Y":
            continue
        names.add(p["Name"])
    return names
