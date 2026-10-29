#!/usr/bin/env python3
"""Join executable field-level test evidence to every expanded map row."""
import argparse
import csv
import json
from collections import Counter, defaultdict
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[2]
EVIDENCE = ROOT / "lab/evidence/e2e-field-campaign-2026-10-23"


def read_csv(path):
    with Path(path).open(encoding="utf-8", newline="") as stream:
        return list(csv.DictReader(stream))


def matrix_key(path):
    """Return reusable decoder matrix types and their direct component."""
    for suffix, type_name in ((".token1", "ClearToken1"), (".token2", "ClearToken2"),
                              (".token3", "ClearToken3")):
        if path.endswith(suffix):
            return [(type_name, None)]
    for marker, type_name in (
        (".token1.", "ClearToken1"),
        (".token2.", "ClearToken2"),
        (".token3.", "ClearToken3"),
        (".auth.", "Authenticator"),
        (".dHKey.", "DiffieHellmanSet"),
        (".encr-mode.non-aea", "non-aea"),
    ):
        if marker in path:
            field = path.split(marker, 1)[1].split(".", 1)[0]
            return [(type_name, field)]
    if path == "HandshakeReq":
        return [("HandshakeReq", None)]
    if path.startswith("HandshakeReq."):
        part = path[len("HandshakeReq."):]
        field = part.split(".", 1)[0]
        keys = [("HandshakeReq", field)]
        if field == "sign" and "." in part:
            keys.append(("Signature", part.split(".", 1)[1].split(".", 1)[0]))
        return keys
    if path == "ClearTransfer":
        return [("ClearTransfer", None)]
    if path.startswith("ClearTransfer."):
        return [("ClearTransfer", path[len("ClearTransfer."):].split(".", 1)[0])]
    # Each signed CHOICE uses the same typed decoder and has its own seed.
    signed = {
        "HandshakeAcc": 0, "ApplicationReject": 1, "HandshakeSecReject": 2,
        "HandshakeSecAbort": 3, "DtSecAbort": 4, "ApplAbort": 5,
        "ReleaseReq": 6, "ReleaseRsp": 7,
    }
    for name, index in signed.items():
        if path == name:
            return [(f"SignedMessage-{index}", None)]
        if path.startswith(name + "."):
            tail = path[len(name) + 1:]
            part = tail.split(".", 1)[0]
            keys = [(f"SignedMessage-{index}", part)]
            if part == "sign":
                nested = tail.split(".", 1)[1] if "." in tail else ""
                keys.append(("Signature", nested.split(".", 1)[0] or None))
            return keys
    if path == "Signature" or path.startswith("Signature."):
        part = path[len("Signature"):].lstrip(".").split(".", 1)[0]
        return [("Signature", part or None)]
    return []


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--map", type=Path, default=HERE / "ber-field-map.csv")
    parser.add_argument("--native", type=Path, default=EVIDENCE / "field-execution-ledger.csv")
    parser.add_argument("--codec", type=Path, default=ROOT / "apps/ccli/build-e2e-ber-sanitizers/e2e-ber-field-assertions.tsv")
    parser.add_argument("--output", type=Path, default=EVIDENCE / "complete-test-evidence-ledger.csv")
    args = parser.parse_args()
    mapping = read_csv(args.map)
    native = {row["map_id"]: row for row in read_csv(args.native)}
    native_cases = defaultdict(set)
    unsupported_gates = []
    for path in EVIDENCE.glob("field-campaign-*.json"):
        for case in json.loads(path.read_text(encoding="utf-8"))["cases"]:
            if case.get("schema_path"):
                native_cases[case["schema_path"]].add(case["case"])
                if case["case"].startswith("RX-UNSUPPORTED") or "unsupported-feature gate" in case.get("assertion", ""):
                    unsupported_gates.append((case["case"], case["schema_path"]))
    contract = json.loads((EVIDENCE / "tx-contract.json").read_text(encoding="utf-8"))
    tx = {path: set(assertions) for path, assertions in contract.get("schema_paths", {}).items()}
    codec_cases = defaultdict(list)
    mutation_seeds = defaultdict(list)
    if args.codec.exists():
        with args.codec.open(encoding="utf-8", newline="") as stream:
            for item in csv.DictReader(stream, delimiter="\t"):
                if item["result"] != "PASS":
                    continue
                if item["kind"] == "SEED":
                    name = item["assertion"].rsplit("#", 1)[0]
                    mutation_seeds[name].append(item)
                    continue
                type_name, sep, remainder = item["assertion"].partition("/")
                if sep:
                    field = remainder.split(" ", 1)[0]
                    codec_cases[(type_name, field)].append(item["assertion"])

    rows = []
    for item in mapping:
        path = item["schema_path"]
        alias = path
        if item["container"] == "AARQ" and ".single-ASN1-type." in alias:
            alias = "HandshakeReq." + alias.split(".single-ASN1-type.", 1)[1]
        elif item["container"] == "AARE" and ".single-ASN1-type." in alias:
            for choice, root in (("assoAccept", "HandshakeAcc"), ("applReject", "ApplicationReject"),
                                 ("secReject", "HandshakeSecReject")):
                marker = ".single-ASN1-type." + choice
                if marker in alias:
                    alias = root + alias.split(marker, 1)[1]
                    break
        elif item["container"] == "RLRQ" and ".single-ASN1-type." in alias:
            alias = "ReleaseReq." + alias.split(".single-ASN1-type.", 1)[1]
        elif item["container"] == "ABRT":
            for choice, root in (("handsSecAbort", "HandshakeSecAbort"),
                                 ("dtSecAbort", "DtSecAbort"), ("applAbort", "ApplAbort")):
                marker = ".single-ASN1-type." + choice
                if marker in alias:
                    alias = root + alias.split(marker, 1)[1]
                    break
        elif item["container"] == "User-data" and ".clearTransfer." in alias:
            alias = "ClearTransfer." + alias.split(".clearTransfer.", 1)[1]
        elif item["container"] == "User-data" and ".encrTransfer" in alias:
            alias = "EncrTransfer" + alias.split(".encrTransfer", 1)[1]
        rx_cases = sorted(native_cases.get(path, set()) | native_cases.get(alias, set()))
        tx_cases = sorted(tx.get(path, set())) if "TX" in item["direction"] else []
        tx_disposition = sorted({f"{ancestor}:{assertion}"
                                 for ancestor, assertions in tx.items()
                                 if (path == ancestor or path.startswith(ancestor + "."))
                                 for assertion in assertions if "unselected" in assertion.lower()})
        gates = sorted({name for name, gate_path in unsupported_gates
                        if (alias == gate_path or alias.startswith(gate_path + ".") or
                            path == gate_path or path.startswith(gate_path + "."))
                        and "RX" in item["direction"]})
        codec = []
        for key in set(matrix_key(path) + matrix_key(alias)):
            codec.extend(codec_cases.get(key, []))
        codec = sorted(set(codec))
        seed_names = set()
        if not item["applicability"].startswith("CONDITIONAL"):
            for type_name, _ in set(matrix_key(path) + matrix_key(alias)):
                if mutation_seeds.get(type_name):
                    seed_names.add(type_name)
        wrapper_seed = {"AARQ": "OSI-AssoReq", "AARE": "OSI-AssoRsp", "ABRT": "OSI-AssoAbr",
                        "RLRQ": "OSI-AssoRelReq", "RLRE": "OSI-AssoRelRsp", "User-data": "Presentation"}.get(item["container"])
        if wrapper_seed and mutation_seeds.get(wrapper_seed):
            seed_names.add(wrapper_seed)
        seed_refs = sorted(seed_names)
        old = native.get(item["map_id"], {})
        sources = []
        if tx_cases or tx_disposition:
            sources.append("independent outgoing-message contract")
        if rx_cases:
            sources.append("native loopback cases")
        if codec:
            sources.append("standalone decoder field matrix")
        if seed_refs:
            sources.append("complete-container single-byte mutation corpus")
        if gates:
            sources.append("unsupported-feature rejection gate")
        if item["applicability"].startswith("CONDITIONAL"):
            status = ("CONDITIONAL - feature unselected; disposition/gate recorded"
                      if gates or tx_disposition else
                      "PARTIAL - active conditional path has direct evidence; variants remain"
                      if sources else
                      "CONDITIONAL - positive behavior unselected; active rejection disposition OPEN")
            remaining = ("Positive behavior is required only if the feature is selected; preserve the linked active negative/absence disposition"
                         if gates or tx_disposition else
                         "Applicable F01-F10 and B01-B12 variants remain for the evidenced conditional path"
                         if sources else
                         "Record an active unsupported-feature rejection gate for RX or profile absence for TX")
        else:
            status = "PARTIAL - applicable variants remain" if sources else "OPEN - no linked assertion"
            remaining = ("Applicable F01-F10 and B01-B12 variants; profile/device/external gates as applicable"
                         if sources else "Add an applicable assertion or a source-backed exclusion")
        rows.append({
            "map_id": item["map_id"], "table": item["table"],
            "requirement_ids": item["requirement_ids"], "schema_path": path,
            "asn_type": item["asn_type"], "presence_or_choice": item["presence_or_choice"],
            "direction": item["direction"], "applicability": item["applicability"],
            "outgoing_contract_cases": ";".join(tx_cases),
            "outgoing_profile_disposition": ";".join(tx_disposition),
            "native_receive_cases": ";".join(rx_cases),
            "standalone_codec_assertions": ";".join(codec),
            "whole_message_mutation_seeds": ";".join(seed_refs),
            "unsupported_feature_gates": ";".join(gates), "evidence_sources": ";".join(sources),
            "evidence_status": status, "remaining": remaining,
        })
    assert len(rows) == 695 and len({row["map_id"] for row in rows}) == 695
    args.output.parent.mkdir(parents=True, exist_ok=True)
    with args.output.open("w", encoding="utf-8", newline="") as stream:
        writer = csv.DictWriter(stream, fieldnames=list(rows[0]), lineterminator="\n")
        writer.writeheader()
        writer.writerows(rows)
    print("Rows:", len(rows))
    print("Evidence status:", Counter(row["evidence_status"] for row in rows))
    print("Evidence sources:", Counter(row["evidence_sources"] or "none" for row in rows))


if __name__ == "__main__":
    main()
