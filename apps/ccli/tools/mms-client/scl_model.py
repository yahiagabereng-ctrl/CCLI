#!/usr/bin/env python3
"""Read IEC 61850 SCL (SCD/CID/ICD) files into a navigable object tree."""
from pathlib import Path
import xml.etree.ElementTree as ET


def _children(element, name):
    return [child for child in element if child.tag.rsplit("}", 1)[-1] == name]


def _node(label, reference, kind, fc="", type_name="", children=None):
    return {"label": label, "reference": reference, "kind": kind, "fc": fc,
            "type": type_name, "children": children or []}


def load_scl(path):
    """Return IED/LD/LN/DO/DA nodes with MMS references and FCs."""
    source = Path(path)
    if source.stat().st_size > 64 * 1024 * 1024:
        raise ValueError("SCL file is larger than 64 MiB")
    root = ET.parse(source).getroot()
    templates = next((element for element in _children(root, "DataTypeTemplates")), None)
    if templates is None:
        raise ValueError("SCL file has no DataTypeTemplates section")

    ln_types = {element.get("id"): element for element in _children(templates, "LNodeType")}
    do_types = {element.get("id"): element for element in _children(templates, "DOType")}
    da_types = {element.get("id"): element for element in _children(templates, "DAType")}

    def append_data_type(parent, type_id, reference, stack=()):
        if not type_id or type_id in stack:
            return
        do_type = do_types.get(type_id)
        if do_type is None:
            return
        for sdo in _children(do_type, "SDO"):
            name = sdo.get("name", "")
            sdo_ref = f"{reference}.{name}"
            sdo_type = do_types.get(sdo.get("type"))
            cdc = sdo_type.get("cdc", "") if sdo_type is not None else ""
            child = _node(f"{name}  ({cdc or 'SDO'})", sdo_ref, "sdo", type_name=cdc)
            parent["children"].append(child)
            append_data_type(child, sdo.get("type"), sdo_ref, stack + (type_id,))
        for da in _children(do_type, "DA"):
            append_attribute(parent, da, reference, stack + (type_id,))

    def append_attribute(parent, declaration, parent_ref, stack):
        name = declaration.get("name", "")
        ref = f"{parent_ref}.{name}"
        fc = declaration.get("fc", "")
        btype = declaration.get("bType", "")
        type_id = declaration.get("type", "")
        label = name + (f"  [{fc}]" if fc else "")
        if btype:
            label += f"  : {btype}"
        child = _node(label, ref, "attribute", fc, btype)
        parent["children"].append(child)
        if btype != "Struct" or not type_id or type_id in stack:
            return
        da_type = da_types.get(type_id)
        if da_type is None:
            return
        for bda in _children(da_type, "BDA"):
            append_bda(child, bda, ref, fc, stack + (type_id,))

    def append_bda(parent, declaration, parent_ref, inherited_fc, stack):
        name = declaration.get("name", "")
        ref = f"{parent_ref}.{name}"
        fc = inherited_fc
        btype = declaration.get("bType", "")
        type_id = declaration.get("type", "")
        label = name + (f"  [{fc}]" if fc else "")
        if btype:
            label += f"  : {btype}"
        child = _node(label, ref, "attribute", fc, btype)
        parent["children"].append(child)
        if btype != "Struct" or not type_id or type_id in stack:
            return
        da_type = da_types.get(type_id)
        if da_type is not None:
            for nested in _children(da_type, "BDA"):
                append_bda(child, nested, ref, fc, stack + (type_id,))

    result = []
    for ied in _children(root, "IED"):
        ied_name = ied.get("name", "IED")
        ied_node = _node(ied_name, "", "ied")
        for access_point in _children(ied, "AccessPoint"):
            for server in _children(access_point, "Server"):
                for ldevice in _children(server, "LDevice"):
                    ld_name = ldevice.get("ldName") or (ied_name + ldevice.get("inst", ""))
                    ld_node = _node(ld_name, ld_name, "logical-device")
                    for ln in ldevice:
                        tag = ln.tag.rsplit("}", 1)[-1]
                        if tag not in ("LN", "LN0"):
                            continue
                        ln_class = ln.get("lnClass", "")
                        ln_name = ln_class if tag == "LN0" else (
                            ln.get("prefix", "") + ln_class + ln.get("inst", ""))
                        ln_ref = f"{ld_name}/{ln_name}"
                        ln_node = _node(ln_name, ln_ref, "logical-node", type_name=ln.get("lnType", ""))
                        ln_type = ln_types.get(ln.get("lnType"))
                        if ln_type is not None:
                            for do in _children(ln_type, "DO"):
                                name = do.get("name", "")
                                do_ref = f"{ln_ref}.{name}"
                                do_type = do_types.get(do.get("type"))
                                cdc = do_type.get("cdc", "") if do_type is not None else ""
                                do_node = _node(f"{name}  ({cdc or 'DO'})", do_ref, "data-object", type_name=cdc)
                                ln_node["children"].append(do_node)
                                append_data_type(do_node, do.get("type"), do_ref)
                        ld_node["children"].append(ln_node)
                    ied_node["children"].append(ld_node)
        if ied_node["children"]:
            result.append(ied_node)
    if not result:
        raise ValueError("SCL file contains no IED server logical devices")
    return result
