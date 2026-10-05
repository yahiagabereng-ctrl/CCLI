#!/usr/bin/env python3
"""Bootstrap plant CSV mapping files (source of truth for commissioning).

Run once or with --force to regenerate templates:
  python scripts/plant-map-init.py
  python scripts/plant-map-init.py --force
"""
from __future__ import annotations

import argparse
import csv
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
PLANT = ROOT / "apps" / "ccli" / "config" / "plant"

BERLIN_SUBSCRIBERS = [
    "1-1TA/B",
    "1-IGA/B",
    "2-3A1/2B1",
    "G-25A LGC",
    "2-R10",
    "2-1T1",
    "1-10MA/B",
    "1-20MA/B",
    "1-30MA/B",
    "1-40MA/B",
    "7-T10A/B",
    "7-T20A/B",
    "7-T30A/B",
    "7-T40A/B",
    "CCI016_01",
]

# Per-subscriber VB base for Dset_BF template (Berlin-style sparse matrix).
BF_VB_BASE = {
    "1-1TA/B": 1,
    "1-IGA/B": 1,
    "2-3A1/2B1": 7,
    "G-25A LGC": 10,
    "2-R10": 13,
    "2-1T1": 16,
    "1-10MA/B": 4,
    "1-20MA/B": 1,
    "1-30MA/B": 7,
    "1-40MA/B": 10,
    "7-T10A/B": 13,
    "7-T20A/B": 1,
    "7-T30A/B": 7,
    "7-T40A/B": 10,
    "CCI016_01": 20,
}

TT_VB_BASE = {
    "1-1TA/B": 4,
    "1-IGA/B": 4,
    "1-10MA/B": 5,
    "1-20MA/B": 7,
    "1-30MA/B": 10,
    "7-T20A/B": 5,
    "CCI016_01": 30,
}

GOOSE_HEADER = [
    "map_id",
    "multicast_address",
    "dataset_name",
    "member_index",
    "member_desc",
    "sending_variable",
    "subscriber_ied",
    "subscriber_local",
    "sending_relay",
    "cb_name",
    "cb_go_id",
    "vlan_dec",
    "vlan_hex",
    "cos",
    "app_id_hex",
    "mms_path",
    "bind_kind",
    "enabled",
    "runtime_status",
    "notes",
]

ASSIGN_HEADER = [
    "plane_id",
    "record_type",
    "app_group",
    "enabled",
    "ied_ld_ln",
    "do_fc",
    "mms_path",
    "clause",
    "equation_id",
    "regulation_id",
    "field_bus",
    "field_device",
    "modbus_slave_id",
    "modbus_fc_read",
    "modbus_reg_read",
    "modbus_type_read",
    "modbus_gain_read",
    "modbus_fc_write",
    "modbus_reg_write",
    "modbus_gain_write",
    "dso_source",
    "transform",
    "runtime_status",
    "notes",
]

def _mms_header() -> list[str]:
    import sys

    sys.path.insert(0, str(ROOT / "scripts"))
    from plant_mms_catalog import MMS_HEADER

    return MMS_HEADER


MMS_HEADER = _mms_header()

INDEX_HEADER = ["file_id", "path", "schema_version", "cid_ref", "generator_script", "description"]

LN_REFS = [
    ("LN.LLN0", "A0", "LD_Plant/LLN0", "", "IMPL"),
    ("LN.LPHD1", "A0", "LD_Plant/LPHD1", "", "PART"),
    ("LN.PdC_WiDPCC1", "A1", "LD_Plant/PdC_WiDPCC1", "eq1", "PART"),
    ("LN.PdC_WaDPCC1", "A1", "LD_Plant/PdC_WaDPCC1", "eq1", "PART"),
    ("LN.PdC_QiDPCC1", "A1", "LD_Plant/PdC_QiDPCC1", "eq1", "PART"),
    ("LN.PdC_QcDPCC1", "A1", "LD_Plant/PdC_QcDPCC1", "eq1", "PART"),
    ("LN.PdC_VADPCC1", "A1", "LD_Plant/PdC_VADPCC1", "eq1", "PART"),
    ("LN.DisFRDECP1", "A2", "LD_Plant/DisFRDECP1", "", "STUB"),
    ("LN.DisFRDGEN1", "A2", "LD_Plant/DisFRDGEN1", "", "STUB"),
    ("LN.DisFRDSTO1", "A2", "LD_Plant/DisFRDSTO1", "", "STUB"),
    ("LN.PdCMMXU1", "A3", "LD_Plant/PdCMMXU1", "eq13_17", "PART"),
    ("LN.GenPVMMXU1", "A3", "LD_Plant/GenPVMMXU1", "eq2", "DEFERRED"),
    ("LN.GenWiMMXU1", "A3", "LD_Plant/GenWiMMXU1", "", "DEFERRED"),
    ("LN.GenTerMMXU1", "A3", "LD_Plant/GenTerMMXU1", "", "DEFERRED"),
    ("LN.GenIdrMMXU1", "A3", "LD_Plant/GenIdrMMXU1", "", "DEFERRED"),
    ("LN.StMMXU1", "A3", "LD_Plant/StMMXU1", "", "DEFERRED"),
    ("LN.SGGMMXU1", "A3", "LD_Plant/SGGMMXU1", "eq2", "DEFERRED"),
    ("LN.SGGMMXU2", "A3", "LD_Plant/SGGMMXU2", "eq2", "DEFERRED"),
    ("LN.IDGXCBR1", "A4", "LD_Plant/IDGXCBR1", "", "STUB"),
    ("LN.SSGGDGEN1", "A5", "LD_Plant/SSGGDGEN1", "", "STUB"),
    ("LN.SSGGDGEN2", "A5", "LD_Plant/SSGGDGEN2", "", "STUB"),
    ("LN.WlimDWMX1", "A6", "LD_Plant/WlimDWMX1", "eq2", "IMPL"),
    ("LN.WSdDAGC1", "A6", "LD_Plant/WSdDAGC1", "eq2", "IMPL"),
    ("LN.VArSdDVAR1", "A7", "LD_Plant/VArSdDVAR1", "eq3", "IMPL"),
    ("LN.PFSPDFPF1", "A7", "LD_Plant/PFSPDFPF1", "eq12", "STUB"),
    ("LN.VArVDVVR1", "A7", "LD_Plant/VArVDVVR1", "eq10", "STUB"),
    ("LN.PFWDPFW1", "A7", "LD_Plant/PFWDPFW1", "eq11", "STUB"),
    ("LN.VArVDPMC1", "A8", "LD_Plant/VArVDPMC1", "eq10", "STUB"),
    ("LN.VArVDPMC2", "A8", "LD_Plant/VArVDPMC2", "eq10", "STUB"),
    ("LN.VArVDECP1", "A8", "LD_Plant/VArVDECP1", "eq10", "STUB"),
    ("LN.VArVDECP2", "A8", "LD_Plant/VArVDECP2", "eq10", "STUB"),
]

ALARM_MEMBERS = [
    ("DisFRDECP1.Beh", "LD_Plant/DisFRDECP1.Beh"),
    ("DisFRDGEN1.Beh", "LD_Plant/DisFRDGEN1.Beh"),
    ("DisFRDSTO1.Beh", "LD_Plant/DisFRDSTO1.Beh"),
    ("IDGXCBR1.Pos", "LD_Plant/IDGXCBR1.Pos"),
    ("SSGGDGEN1.Health", "LD_Plant/SSGGDGEN1.Health"),
    ("SSGGDGEN2.Health", "LD_Plant/SSGGDGEN2.Health"),
    ("WlimDWMX1.Beh", "LD_Plant/WlimDWMX1.Beh"),
    ("WlimDWMX1.FctOpStAuto", "LD_Plant/WlimDWMX1.FctOpStAuto"),
    ("WlimDWMX1.FctOpStEx", "LD_Plant/WlimDWMX1.FctOpStEx"),
    ("WSdDAGC1.Beh", "LD_Plant/WSdDAGC1.Beh"),
    ("WSdDAGC1.FctOpSt", "LD_Plant/WSdDAGC1.FctOpSt"),
    ("VArSdDVAR1.Beh", "LD_Plant/VArSdDVAR1.Beh"),
    ("VArSdDVAR1.FctOpSt", "LD_Plant/VArSdDVAR1.FctOpSt"),
    ("PFSPDFPF1.Beh", "LD_Plant/PFSPDFPF1.Beh"),
    ("PFSPDFPF1.FctOpSt", "LD_Plant/PFSPDFPF1.FctOpSt"),
    ("VArVDVVR1.Beh", "LD_Plant/VArVDVVR1.Beh"),
    ("VArVDVVR1.FctOpSt", "LD_Plant/VArVDVVR1.FctOpSt"),
    ("PFWDPFW1.Beh", "LD_Plant/PFWDPFW1.Beh"),
    ("PFWDPFW1.FctOpSt", "LD_Plant/PFWDPFW1.FctOpSt"),
]

PDC_MEMBERS = [
    ("TotW", "LT_N/A", "LD_Plant/PdCMMXU1.TotW"),
    ("TotVAr", "LT_N/A", "LD_Plant/PdCMMXU1.TotVAr"),
    ("PPV", "LT_N/A", "LD_Plant/PdCMMXU1.PPV"),
    ("A", "LT_N/A", "LD_Plant/PdCMMXU1.A"),
]


def _vb(base: int, member_index: int) -> str:
    # Quality=+2, TestMode=+0, BF/Trip=+1 relative to base VB001 pattern
    offsets = {0: 2, 1: 0, 2: 1}
    return f"VB{base + offsets[member_index]:03d}"


def build_goose_rows() -> list[dict]:
    rows: list[dict] = []

    bf_members = [
        (0, "Quality", "N/A"),
        (1, "Test Mode", "LT05"),
        (2, "Breaker Failure", "LT09"),
    ]
    tt_members = [
        (0, "Quality", "N/A"),
        (1, "Test Mode", "LT05"),
        (2, "Transfer Trip", "SV11T"),
    ]

    for sub in BERLIN_SUBSCRIBERS:
        if sub not in BF_VB_BASE:
            continue
        base = BF_VB_BASE[sub]
        enabled = "Y" if sub in {"1-1TA/B", "1-IGA/B", "1-20MA/B", "7-T20A/B"} else "N"
        for idx, desc, svar in bf_members:
            rows.append(
                {
                    "map_id": f"Dset_BF.{desc.replace(' ', '')}->{sub}",
                    "multicast_address": "01-0C-CD-01-00-14",
                    "dataset_name": "Dset_BF",
                    "member_index": str(idx),
                    "member_desc": desc,
                    "sending_variable": svar,
                    "subscriber_ied": sub,
                    "subscriber_local": _vb(base, idx),
                    "sending_relay": "I-1AMB1",
                    "cb_name": "GooseDset_BF",
                    "cb_go_id": "I-1AMB1_Dset_BF",
                    "vlan_dec": "20",
                    "vlan_hex": "14",
                    "cos": "7",
                    "app_id_hex": "0x0002",
                    "mms_path": "LD_Plant/IDGXCBR1.Pos" if sub == "CCI016_01" and idx == 2 else "",
                    "bind_kind": "goose_sub" if sub == "CCI016_01" else "",
                    "enabled": enabled,
                    "runtime_status": "STUB" if sub != "CCI016_01" else "STUB",
                    "notes": "Berlin BF template" if sub != "CCI016_01" else "CCI subscribe stub — IDG from bay BF",
                }
            )

    for sub in BERLIN_SUBSCRIBERS:
        if sub not in TT_VB_BASE:
            continue
        base = TT_VB_BASE[sub]
        enabled = "Y" if sub in {"1-10MA/B", "7-T20A/B"} else "N"
        for idx, desc, svar in tt_members:
            rows.append(
                {
                    "map_id": f"Dset_TT.{desc.replace(' ', '')}->{sub}",
                    "multicast_address": "01-0C-CD-01-00-01",
                    "dataset_name": "Dset_TT",
                    "member_index": str(idx),
                    "member_desc": desc,
                    "sending_variable": svar,
                    "subscriber_ied": sub,
                    "subscriber_local": _vb(base, idx),
                    "sending_relay": "I-1AMB1",
                    "cb_name": "GooseDset_TT",
                    "cb_go_id": "I-1AMB1_Dset_TT",
                    "vlan_dec": "1",
                    "vlan_hex": "1",
                    "cos": "7",
                    "app_id_hex": "0x0001",
                    "mms_path": "",
                    "bind_kind": "goose_sub" if sub == "CCI016_01" else "",
                    "enabled": enabled,
                    "runtime_status": "STUB",
                    "notes": "Berlin TT template",
                }
            )

    for idx, (desc, svar, mms_path) in enumerate(PDC_MEMBERS):
        rows.append(
            {
                "map_id": f"CCI.PdC.{desc}",
                "multicast_address": "01-0C-CD-01-00-01",
                "dataset_name": "DS_R_PdC_Mis4sec",
                "member_index": str(idx),
                "member_desc": desc,
                "sending_variable": svar,
                "subscriber_ied": "CCI016_01",
                "subscriber_local": "",
                "sending_relay": "CCI016_01",
                "cb_name": "gcb_PdC_Mis4sec",
                "cb_go_id": "PdCMis4",
                "vlan_dec": "0",
                "vlan_hex": "0",
                "cos": "4",
                "app_id_hex": "0x1000",
                "mms_path": mms_path,
                "bind_kind": "mms_int",
                "enabled": "Y",
                "runtime_status": "IMPL",
                "notes": "CCLI publish — matches lab_tg544_eth_a.cid",
            }
        )

    for idx, (desc, mms_path) in enumerate(ALARM_MEMBERS):
        rows.append(
            {
                "map_id": f"CCI.Alarm.{desc}",
                "multicast_address": "01-0C-CD-01-00-02",
                "dataset_name": "DS_R_Stato_Allarmi_Segnali",
                "member_index": str(idx),
                "member_desc": desc,
                "sending_variable": "N/A",
                "subscriber_ied": "CCI016_01",
                "subscriber_local": "",
                "sending_relay": "CCI016_01",
                "cb_name": "gcb_Stato_Allarmi",
                "cb_go_id": "CCIAlrm",
                "vlan_dec": "0",
                "vlan_hex": "0",
                "cos": "4",
                "app_id_hex": "0x1001",
                "mms_path": mms_path,
                "bind_kind": "mms_int",
                "enabled": "Y",
                "runtime_status": "IMPL",
                "notes": "CCLI alarm GOOSE publish member",
            }
        )

    return rows


def build_assignment_rows() -> list[dict]:
    rows: list[dict] = []

    rows.append(
        {
            "plane_id": "META.RTU",
            "record_type": "META",
            "app_group": "A0",
            "enabled": "Y",
            "ied_ld_ln": "LD_Plant/LLN0",
            "do_fc": "",
            "mms_path": "",
            "clause": "",
            "equation_id": "",
            "regulation_id": "",
            "field_bus": "modbus_rtu",
            "field_device": "Huawei V3.0 RTU",
            "modbus_slave_id": "",
            "modbus_fc_read": "",
            "modbus_reg_read": "",
            "modbus_type_read": "",
            "modbus_gain_read": "",
            "modbus_fc_write": "",
            "modbus_reg_write": "",
            "modbus_gain_write": "",
            "dso_source": "",
            "transform": "",
            "runtime_status": "PART",
            "notes": "9600 8N1 /dev/ttyS2; addressing=huawei_direct; poll_ms=4000",
        }
    )

    rows.append(
        {
            "plane_id": "PdC.TotW",
            "record_type": "MEASURE",
            "app_group": "A3",
            "enabled": "Y",
            "ied_ld_ln": "LD_Plant/PdCMMXU1",
            "do_fc": "TotW/MX",
            "mms_path": "LD_Plant/PdCMMXU1.TotW",
            "clause": "O.8.3",
            "equation_id": "eq13_17",
            "regulation_id": "R09",
            "field_bus": "modbus_rtu",
            "field_device": "Grid analyzer",
            "modbus_slave_id": "1",
            "modbus_fc_read": "3",
            "modbus_reg_read": "40001",
            "modbus_type_read": "float32",
            "modbus_gain_read": "1",
            "modbus_fc_write": "",
            "modbus_reg_write": "",
            "modbus_gain_write": "",
            "dso_source": "",
            "transform": "40001-offset lab map",
            "runtime_status": "IMPL",
            "notes": "POC meter — not inverter sum",
        }
    )
    rows.append(
        {
            "plane_id": "PdC.TotVAr",
            "record_type": "MEASURE",
            "app_group": "A3",
            "enabled": "Y",
            "ied_ld_ln": "LD_Plant/PdCMMXU1",
            "do_fc": "TotVAr/MX",
            "mms_path": "LD_Plant/PdCMMXU1.TotVAr",
            "clause": "O.8.3",
            "equation_id": "eq13_17",
            "regulation_id": "R09",
            "field_bus": "modbus_rtu",
            "field_device": "Grid analyzer",
            "modbus_slave_id": "1",
            "modbus_fc_read": "3",
            "modbus_reg_read": "40003",
            "modbus_type_read": "float32",
            "modbus_gain_read": "1",
            "modbus_fc_write": "",
            "modbus_reg_write": "",
            "modbus_gain_write": "",
            "dso_source": "",
            "transform": "",
            "runtime_status": "IMPL",
            "notes": "POC reactive",
        }
    )

    for n in range(1, 11):
        ln = f"LD_Plant/SGGMMXU{n}" if n <= 2 else f"LD_Plant/SGGMMXU{n}"
        status = "IMPL" if n <= 2 else "CID_GAP"
        rows.append(
            {
                "plane_id": f"INV{n:02d}.TotW",
                "record_type": "MEASURE",
                "app_group": "A3",
                "enabled": "Y" if n <= 10 else "N",
                "ied_ld_ln": ln,
                "do_fc": "TotW/MX",
                "mms_path": f"{ln}.TotW",
                "clause": "O.8.4",
                "equation_id": "eq2",
                "regulation_id": "R09",
                "field_bus": "modbus_rtu",
                "field_device": "Huawei SUN2000-100KTL-M2",
                "modbus_slave_id": str(n),
                "modbus_fc_read": "3",
                "modbus_reg_read": "32080",
                "modbus_type_read": "I32",
                "modbus_gain_read": "1000",
                "modbus_fc_write": "",
                "modbus_reg_write": "",
                "modbus_gain_write": "",
                "dso_source": "",
                "transform": "Model ID 150 @30070",
                "runtime_status": status,
                "notes": f"Inverter sample INV-{n:02d}",
            }
        )
        rows.append(
            {
                "plane_id": f"INV{n:02d}.Status",
                "record_type": "STATUS",
                "app_group": "A3",
                "enabled": "Y",
                "ied_ld_ln": ln,
                "do_fc": "Health/ST",
                "mms_path": f"{ln}.Beh",
                "clause": "O.8.4",
                "equation_id": "",
                "regulation_id": "",
                "field_bus": "modbus_rtu",
                "field_device": "Huawei SUN2000-100KTL-M2",
                "modbus_slave_id": str(n),
                "modbus_fc_read": "3",
                "modbus_reg_read": "32089",
                "modbus_type_read": "U16",
                "modbus_gain_read": "1",
                "modbus_fc_write": "",
                "modbus_reg_write": "",
                "modbus_gain_write": "",
                "dso_source": "",
                "transform": "",
                "runtime_status": status,
                "notes": "Device status word",
            }
        )

    rows.append(
        {
            "plane_id": "GenPV.TotW",
            "record_type": "AGGREGATE",
            "app_group": "A3",
            "enabled": "Y",
            "ied_ld_ln": "LD_Plant/GenPVMMXU1",
            "do_fc": "TotW/MX",
            "mms_path": "LD_Plant/GenPVMMXU1.TotW",
            "clause": "O.8.4",
            "equation_id": "eq2",
            "regulation_id": "R09",
            "field_bus": "none",
            "field_device": "",
            "modbus_slave_id": "",
            "modbus_fc_read": "",
            "modbus_reg_read": "",
            "modbus_type_read": "",
            "modbus_gain_read": "",
            "modbus_fc_write": "",
            "modbus_reg_write": "",
            "modbus_gain_write": "",
            "dso_source": "",
            "transform": "sum(INV01..INV10.TotW)",
            "runtime_status": "PART",
            "notes": "Clears PLANT-GAP-01 when multi-slave Modbus wired",
        }
    )

    dso_controls = [
        ("DSO.WSd.PctWrite", "WSdDAGC1.WSptPct", "40125", "10", "WSdDAGC1.WSptPct", "fanout slaves 1-10"),
        ("DSO.Wlim.PctWrite", "WlimDWMX1.WMaxSptPct", "40125", "10", "WlimDWMX1.WMaxSptPct", "fanout slaves 1-10"),
        ("DSO.VArSd.QWrite", "VArSdDVAR1.VArTgtSptPct", "40123", "1000", "VArSdDVAR1.VArTgtSptPct", "Q/S signed"),
        ("DSO.PFSP.PFWrite", "PFSPDFPF1.PFGnTgtSpt", "40122", "1000", "PFSPDFPF1.PFGnTgtSpt", "cosφ*1000 export target"),
        ("COMM.42014", "", "42014", "1", "", "Remote scheduling enable"),
        ("COMM.42000", "", "42000", "1", "", "Grid code 70=CEI0-16 Italy"),
    ]
    for pid, dso, reg, gain, src, note in dso_controls:
        rows.append(
            {
                "plane_id": pid,
                "record_type": "CONTROL",
                "app_group": (
                    "A7"
                    if "VArSd" in pid or "PFSP" in pid
                    else "A6"
                    if "DSO" in pid
                    else "A0"
                ),
                "enabled": "Y" if "420" in pid or "WSd" in pid or "Wlim" in pid or "VArSd" in pid else "N",
                "ied_ld_ln": f"LD_Plant/{dso.split('.')[0]}" if dso else "",
                "do_fc": "CO",
                "mms_path": f"LD_Plant/{dso}" if dso else "",
                "clause": "O.9",
                "equation_id": "eq2" if "WSd" in pid or "Wlim" in pid else "eq3",
                "regulation_id": "R13" if "VArSd" in pid else "R03",
                "field_bus": "modbus_rtu",
                "field_device": "Huawei SUN2000-100KTL-M2",
                "modbus_slave_id": "ALL",
                "modbus_fc_read": "",
                "modbus_reg_read": "",
                "modbus_type_read": "",
                "modbus_gain_read": "",
                "modbus_fc_write": "6",
                "modbus_reg_write": reg,
                "modbus_gain_write": gain,
                "dso_source": src,
                "transform": note,
                "runtime_status": "PART" if "DSO" in pid else "STUB",
                "notes": note,
            }
        )

    for pid, grp, ln, eq, status in LN_REFS:
        rows.append(
            {
                "plane_id": pid,
                "record_type": "LN_REF",
                "app_group": grp,
                "enabled": "Y",
                "ied_ld_ln": ln,
                "do_fc": "",
                "mms_path": ln,
                "clause": "",
                "equation_id": eq,
                "regulation_id": "",
                "field_bus": "none",
                "field_device": "",
                "modbus_slave_id": "",
                "modbus_fc_read": "",
                "modbus_reg_read": "",
                "modbus_type_read": "",
                "modbus_gain_read": "",
                "modbus_fc_write": "",
                "modbus_reg_write": "",
                "modbus_gain_write": "",
                "dso_source": "",
                "transform": "",
                "runtime_status": status,
                "notes": "31-LN traceability from signal_map.yaml",
            }
        )

    return rows


def build_mms_rows() -> list[dict]:
    import sys

    sys.path.insert(0, str(ROOT / "scripts"))
    from plant_mms_catalog import build_full_mms_catalog

    return build_full_mms_catalog()


def build_index_rows() -> list[dict]:
    return [
        {
            "file_id": "GOOSE",
            "path": "apps/ccli/config/plant/GOOSE_DATA_MAP.csv",
            "schema_version": "1",
            "cid_ref": "lab_tg544_eth_a.cid",
            "generator_script": "scripts/plant-map-generate.py",
            "description": "Berlin GOOSE data map (long format)",
        },
        {
            "file_id": "ASSIGN",
            "path": "apps/ccli/config/plant/PLANT_ASSIGNMENT_PLANE.csv",
            "schema_version": "1",
            "cid_ref": "lab_tg544_eth_a.cid",
            "generator_script": "scripts/plant-map-generate.py",
            "description": "Field to MMS assignment plane",
        },
        {
            "file_id": "MMS",
            "path": "apps/ccli/config/plant/MMS_POINT_MAP.csv",
            "schema_version": "4",
            "cid_ref": "lab_tg544_eth_a.cid",
            "generator_script": "scripts/plant-mms-sync.py",
            "description": "SCADA / TestSuite Pro MMS points (v4 O11 + classes + q/t + RBAC)",
        },
        {
            "file_id": "INDEX",
            "path": "apps/ccli/config/plant/PLANT_MAP_INDEX.csv",
            "schema_version": "1",
            "cid_ref": "lab_tg544_eth_a.cid",
            "generator_script": "scripts/plant-map-init.py",
            "description": "Manifest of plant CSV files",
        },
    ]


def write_csv(path: Path, header: list[str], rows: list[dict]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", newline="", encoding="utf-8-sig") as f:
        w = csv.DictWriter(f, fieldnames=header, extrasaction="ignore")
        w.writeheader()
        w.writerows(rows)
    print(f"Wrote {path} ({len(rows)} rows)")


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--force", action="store_true", help="Overwrite existing CSV files")
    args = ap.parse_args()

    targets = [
        (PLANT / "GOOSE_DATA_MAP.csv", GOOSE_HEADER, build_goose_rows),
        (PLANT / "PLANT_ASSIGNMENT_PLANE.csv", ASSIGN_HEADER, build_assignment_rows),
        (PLANT / "MMS_POINT_MAP.csv", MMS_HEADER, build_mms_rows),
        (PLANT / "PLANT_MAP_INDEX.csv", INDEX_HEADER, build_index_rows),
    ]

    for path, header, builder in targets:
        if path.exists() and not args.force:
            print(f"Skip {path} (exists; use --force)")
            continue
        write_csv(path, header, builder())

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
