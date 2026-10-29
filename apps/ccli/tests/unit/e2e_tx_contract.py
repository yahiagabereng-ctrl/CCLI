"""Independent Python assertions for the native server's emitted profile.

Uses the peer's BER scanner, not CCLI's encoder/decoder. This validates the
configured publisher's fields and runs local negative controls against captured
TX structures. It does not redefine general extensible ASN.1 receiver policy.
"""
import csv
import hashlib
import json
import re
from pathlib import Path


class Contract:
    def __init__(self, probe):
        self.p = probe
        self.observed = {}
        self.samples = set()
        self.units = []
        self.algorithm_units = []
        self.negative_by_path = {}
        self.testing = False
        self.negative_checks = 0
        self.frames = 0
        self.acse_context, self.e2e_context = 1, 5

    def note(self, path, assertion):
        if not self.testing: self.observed.setdefault(path, set()).add(assertion)

    def require(self, condition, path):
        if not condition: raise AssertionError("TX contract: " + path)

    def tag(self, encoded, expected, path):
        tag, _, _, rest = self.p.split(encoded)
        self.require(not rest and (expected is None or tag == expected), path + "/tag/framing")
        self.note(path, "tag/framing")

    def integer(self, encoded, path, allowed=None):
        value = self.p.contents(encoded)
        self.require(value and not value[0] & 128 and
                     not (len(value) > 1 and value[0] == 0 and not value[1] & 128), path + "/integer")
        number = int.from_bytes(value, "big")
        self.require(allowed is None or number in allowed, path + "/range")
        self.note(path, "integer/range")
        return number

    def nonempty(self, encoded, path):
        self.require(bool(self.p.contents(encoded)), path + "/empty")
        self.note(path, "nonempty")

    def time(self, encoded, path):
        self.p.timestamp_ns(self.p.contents(encoded)); self.note(path, "time syntax")

    def bits(self, encoded, path):
        self.require(self.p.contents(encoded) == b"\x07\x80", path + "/selected bits")
        self.note(path, "selected bits")

    def boolean(self, encoded, path):
        self.require(self.p.contents(encoded) in (b"\0", b"\xff"), path + "/canonical boolean")
        self.note(path, "boolean")

    def any(self, encoded, path): self.tag(encoded, None, path)

    def application(self, encoded, path):
        self.tag(encoded, None, path)
        self.require(encoded[0] & 0xe0 == 0xa0 and encoded[0] & 31 <= 13, path + "/MMS open type")
        self.note(path, "MMS open type")

    def forbidden(self, encoded, path): raise AssertionError("TX forbidden field: " + path)

    def seq(self, encoded, tag, path, specs):
        self.tag(encoded, tag, path)
        values = self.p.children(encoded, tag)
        index = 0
        present = []
        for name, expected, required, validate in specs:
            target = path + "." + name
            matched = index < len(values) and (expected is None or values[index][0] == expected)
            self.require(matched or not required, target + "/missing")
            self.note(target, "present" if matched else "absent")
            if matched:
                self.tag(values[index], expected, target)
                validate(values[index], target)
                present.append((index, required, target))
                index += 1
        self.require(index == len(values), path + "/extra/order")
        self.note(path, "order/cardinality")
        if not self.testing: self.units.append((encoded, tag, path, specs, present))

    def algorithm(self, encoded, path):
        if not self.testing: self.algorithm_units.append((encoded, path))
        self.tag(encoded, 0x30, path)
        fields = self.p.children(encoded, 0x30)
        self.require(1 <= len(fields) <= 2 and fields[0][0] == 6, path + "/algorithm fields")
        self.note(path + ".algorithm", "OID/present")
        oid = self.p.contents(fields[0])
        known = (self.p.RSA, self.p.ECDSA, self.p.HMAC, self.p.GMAC128, self.p.GMAC256)
        self.require(oid in [self.p.oid_value(value) for value in known], path + "/supported algorithm")
        self.note(path + ".parameters", "present" if len(fields) == 2 else "absent")
        if len(fields) == 2:
            if oid in (self.p.oid_value(self.p.RSA), self.p.oid_value(self.p.HMAC)):
                self.require(fields[1] == b"\x05\0", path + "/NULL parameters")
            else:
                self.require(oid != self.p.oid_value(self.p.ECDSA), path + "/ECDSA parameters")
                params, = self.p.children(fields[1], 0x30)
                self.tag(params, 2, path + "/GMAC tag length")
                self.integer(params, path, range(12, 17))

    def dh(self, encoded, path):
        self.seq(encoded, 0x30, path, [
            ("groupId", 2, True, lambda e, p: self.integer(e, p, (14, 23, 28))),
            ("dhPublicKey", 4, True, self.nonempty)])
        group, public = self.p.children(encoded)
        width = 256 if int.from_bytes(self.p.contents(group), "big") == 14 else 65
        self.require(len(self.p.contents(public)) == width, path + "/public width")
        self.note(path + ".dhPublicKey", "group/public width")

    def mode(self, encoded, path):
        def selected(encoded, path):
            values = self.p.children(encoded, 0xa1)
            self.require(len(values) == 1, path + "/single selection")
            self.note(path, "single SET selection")
            self.algorithm(values[0], path + ".algo[*]")
        self.note(path + ".aea", "unselected")
        self.note(path + ".non-aea.encr", "unselected feature absent in integrity-only profile")
        self.note(path + ".non-aea.encr.algo[*]", "unselected feature absent in integrity-only profile")
        self.seq(encoded, 0xa0, path + ".non-aea", [
            ("encr", 0xa0, False, self.forbidden), ("icvAlgID", 0xa1, True, selected)])

    def certpath(self, encoded, path):
        values = self.p.children(encoded, 0x30)
        self.require(0 < len(values) <= 32, path + "/list bound")
        for value in values:
            self.tag(value, 4, path + ".pkCert[*]"); self.nonempty(value, path + ".pkCert[*]")

    def token1(self, encoded, path):
        self.note(path + ".attCert", "unselected optional feature absent in integrity-only profile")
        self.seq(encoded, 0x30, path, [
            ("sigAlg", 0x30, True, self.algorithm), ("pkCert", 4, True, self.nonempty),
            ("certPath", 0x30, False, self.certpath), ("version", 3, False, self.bits),
            ("assoID", 12, True, self.nonempty), ("dHKey", 0x30, True, self.dh),
            ("hmac", 0x30, True, self.algorithm), ("time", 24, True, self.time),
            ("encr-mode", 0xa0, True, self.mode), ("confParams", 3, True, self.bits),
            ("attCert", 4, False, self.forbidden)])

    def token3(self, encoded, path):
        self.seq(encoded, 0x30, path, [
            ("sigAlg", 0x30, True, self.algorithm), ("version", 3, False, self.bits),
            ("assoID", 12, False, self.nonempty), ("time", 24, True, self.time),
            ("pkCert", 4, True, self.nonempty), ("certPath", 0x30, False, self.certpath)])

    def token2(self, encoded, path):
        self.seq(encoded, 0x30, path, [
            ("version", 3, False, self.bits), ("assoID", 12, True, self.nonempty),
            ("time", 24, True, self.time), ("seq", 2, True, self.integer),
            ("iv", 0x80, False, self.forbidden), ("rekey", 0x81, False, self.forbidden),
            ("reqRekey", 0x82, False, self.boolean), ("changedKey", 0x83, False, self.boolean)])

    def signature(self, encoded, path):
        self.seq(encoded, 0x30, path, [("algo", 0x30, True, self.algorithm), ("sign", 4, True, self.nonempty)])

    def signed(self, encoded, path, name):
        accept = name == "HandshakeAcc"
        app = name in ("HandshakeAcc", "ApplicationReject", "ApplAbort")
        diag = name in ("HandshakeSecReject", "DtSecAbort")
        def body(encoded, path):
            specs = [("token1" if accept else "token3", 0x30, True, self.token1 if accept else self.token3)]
            if app:
                expected = 0xa9 if accept else 0xaa if name == "ApplicationReject" else None
                specs.append(("applData", expected, name != "ApplAbort", self.application))
            if diag: specs.append(("diag", 10, False, self.integer))
            self.seq(encoded, 0x30, path, specs)
        self.seq(encoded, encoded[0], path, [("tbs", 0x30, True, body), ("sign", 0x30, True, self.signature)])
        if path != name: self.signed(encoded, name, name)

    def clear(self, encoded, path):
        def body(encoded, path):
            self.seq(encoded, 0x30, path, [("token2", 0x30, True, self.token2), ("applData", None, True, self.application)])
        def auth(encoded, path):
            self.seq(encoded, 0x30, path, [("nonce", 4, False, self.nonempty),
                ("algo", 0x30, True, self.algorithm), ("icv", 4, True, self.nonempty)])
            values = self.p.children(encoded)
            nonce = values.pop(0) if values[0][0] == 4 else None
            algorithm, icv = values
            parameters = self.p.children(algorithm)
            hmac = self.p.contents(parameters[0]) == self.p.oid_value(self.p.HMAC)
            size = 32 if hmac else 12 if len(parameters) == 1 else int.from_bytes(
                self.p.contents(self.p.children(parameters[1])[0]), "big")
            self.require(len(self.p.contents(icv)) == size and (nonce is None if hmac else
                nonce is not None and 0 < len(self.p.contents(nonce)) <= 256), path + "/nonce/ICV profile")
            self.note(path + ".nonce", "negotiated conditional presence/length")
            self.note(path + ".icv", "negotiated length")
        self.seq(encoded, 0xa0, path, [("tbp", 0x30, True, body), ("auth", 0x30, True, auth)])
        if path != "ClearTransfer": self.clear(encoded, "ClearTransfer")

    def osi(self, encoded, path, kind):
        def entry(encoded, path):
            def explicit(encoded, path):
                value, = self.p.children(encoded, 0xa0)
                if kind == "release": self.signed(value, path, "ReleaseRsp")
                elif kind == "association":
                    name, component = {0xa1: ("HandshakeAcc", "assoAccept"),
                        0xa2: ("ApplicationReject", "applReject"), 0xa3: ("HandshakeSecReject", "secReject")}[value[0]]
                    self.signed(value, path + "." + component, name)
                elif value[0] == 0x83:
                    self.integer(value, path + ".osiDiag", range(4))
                else:
                    name, component = {0xa1: ("DtSecAbort", "dtSecAbort"), 0xa2: ("ApplAbort", "applAbort")}[value[0]]
                    self.signed(value, path + "." + component, name)
            self.seq(encoded, 0x28, path, [("indirect-reference", 2, True,
                lambda e, p: self.integer(e, p, (self.e2e_context,))), ("single-ASN1-type", 0xa0, True, explicit)])
        self.seq(encoded, 0xbe, path, [("entry[0]", 0x28, True, entry)])

    def payload(self, kind, pci, encoded):
        if kind == 1:
            self.require(pci == self.e2e_context, "transfer PCI")
            self.clear(encoded, "ClearTransfer"); return
        self.require(pci == self.acse_context, "ACSE PCI")
        if kind == 14:
            def diagnostic(encoded, path):
                source, = self.p.children(encoded, 0xa3)
                self.require(source[0] in (0xa1, 0xa2), path)
                code, = self.p.children(source)
                self.tag(code, 2, path); self.integer(code, path)
                self.note(path + (".acse-service-user" if source[0] == 0xa1 else ".acse-service-provider"), "source/cause")
            def explicit(validator, expected=2):
                def result(encoded, path):
                    item, = self.p.children(encoded)
                    self.tag(item, expected, path + "/explicit value"); validator(item, path)
                return result
            self.seq(encoded, 0x61, "AARE", [
                ("protocol-version", 0x80, False, self.bits),
                ("application-context-name", 0xa1, True, explicit(self.nonempty, 6)),
                ("result", 0xa2, True, explicit(lambda e, p: self.integer(e, p, (0, 1)))),
                ("result-source-diagnostic", 0xa3, True, diagnostic),
                ("responding-AP-title", 0xa4, True, explicit(self.nonempty, 6)),
                ("responding-AE-qualifier", 0xa5, True, explicit(self.integer)),
                ("responding-AP-invocation-id", 0xa6, False, self.forbidden),
                ("responding-AE-invocation-id", 0xa7, False, self.forbidden),
                ("responder-acse-requirements", 0x88, False, self.forbidden),
                ("mechanism-name", 0x89, False, self.forbidden),
                ("responding-authentication-value", 0xaa, False, self.forbidden),
                ("application-context-name-list", 0xab, False, self.forbidden),
                ("implementation-information", 0x9d, False, self.forbidden),
                ("user-information", 0xbe, False, lambda e, p: self.osi(e, p, "association"))])
            values = self.p.children(encoded)
            result = self.integer(self.p.children(next(v for v in values if v[0] == 0xa2))[0], "AARE.result", (0, 1))
            info = next((v for v in values if v[0] == 0xbe), None)
            self.require(result != 0 or info is not None, "accepted AARE missing security payload")
            context = self.p.children(next(v for v in values if v[0] == 0xa1))[0]
            self.require(self.p.contents(context) == self.p.oid_value(self.p.CONTEXT), "AARE context value")
            source, = self.p.children(next(v for v in values if v[0] == 0xa3))
            cause, = self.p.children(source)
            self.require((result != 0 or (source[0] == 0xa1 and self.p.contents(cause) == b"\0")),
                         "AARE accepted source/diagnostic")
            if info:
                external, = self.p.children(info)
                _, explicit_value = self.p.children(external)
                signed, = self.p.children(explicit_value)
                self.require(signed[0] in ((0xa1,) if result == 0 else (0xa2, 0xa3)), "AARE result/choice")
        elif kind == 25:
            self.seq(encoded, 0x64, "ABRT", [("abort-source", 0x80, True,
                lambda e, p: self.integer(e, p, (0, 1))), ("abort-diagnostic", 0x81, False, self.forbidden),
                ("user-information", 0xbe, False, lambda e, p: self.osi(e, p, "abort"))])
            values = self.p.children(encoded)
            provider = self.p.contents(values[0]) == b"\x01"
            self.require(len(values) == (1 if provider else 2), "ABRT source/payload correspondence")
        elif kind == 10:
            self.seq(encoded, 0x63, "RLRE", [("reason", 0x80, False,
                lambda e, p: self.integer(e, p, (0,))),
                ("user-information", 0xbe, True, lambda e, p: self.osi(e, p, "release"))])
        else: raise AssertionError("unexpected server SPDU kind: " + str(kind))

    def shape(self, encoded):
        return (encoded[0], tuple(self.shape(v) for v in self.p.children(encoded))) if encoded[0] & 32 else encoded[0]

    def local_controls(self, units):
        self.testing = True
        try:
            for encoded, tag, path, specs, present in units:
                values = self.p.children(encoded)
                for index, required, target in present:
                    # Mandatory deletion, duplicate, wrong tag and adjacent
                    # exchange must be caught by this independent TX validator.
                    variants = []
                    if required: variants.append(values[:index] + values[index+1:])
                    variants.append(values[:index] + [values[index]] + values[index:])
                    variants.append(values[:index] + [b"\x05\0"] + values[index+1:])
                    if index + 1 < len(values) and values[index][0] != values[index+1][0]:
                        changed = list(values); changed[index], changed[index+1] = changed[index+1], changed[index]
                        variants.append(changed)
                    for variant in variants:
                        try: self.seq(self.p.tlv(tag, b"".join(variant)), tag, path, specs)
                        except (AssertionError, ValueError, KeyError):
                            self.negative_checks += 1
                            self.negative_by_path[target] = self.negative_by_path.get(target, 0) + 1
                        else: raise AssertionError("TX negative control accepted: " + target)
            for encoded, path in self.algorithm_units:
                oid = self.p.children(encoded)[0]
                for value, component in [(b"", "algorithm"), (b"\x05\0", "algorithm"),
                    (self.p.algorithm("1.2.3")[2:], "algorithm"),
                    (oid + self.p.integer(1), "parameters"), (oid + b"\x05\0\x05\0", "parameters")]:
                    try: self.algorithm(self.p.tlv(0x30, value), path)
                    except (AssertionError, ValueError, KeyError):
                        self.negative_checks += 1
                        target = path + "." + component
                        self.negative_by_path[target] = self.negative_by_path.get(target, 0) + 1
                    else: raise AssertionError("TX malformed algorithm accepted: " + path)
        finally: self.testing = False

    def observe(self, kind, pci, encoded, presentation, contexts=(1, 5)):
        self.acse_context, self.e2e_context = contexts
        self.frames += 1
        self.units = []
        self.algorithm_units = []
        self.payload(kind, pci, encoded)
        def userdata(value):
            if value[0] == 0x61: return [value]
            if not value[0] & 32: return []
            return [found for child in self.p.children(value) for found in userdata(child)]
        found = userdata(presentation)
        self.require(len(found) == 1, "presentation single User-data")
        def item(value, path):
            def choice(value, path):
                payload, = self.p.children(value, 0xa0)
                self.require(payload == encoded, path + "/actual payload")
                if kind == 1: self.clear(payload, path + ".clearTransfer")
            self.seq(value, 0x30, path, [("presentation-context-identifier", 2, True,
                lambda e, p: self.integer(e, p, (pci,))), ("single-ASN1-type", 0xa0, True, choice)])
        self.seq(found[0], 0x61, "User-data", [("entry[0]", 0x30, True, item)])
        key = (kind, self.shape(encoded))
        if key not in self.samples:
            self.samples.add(key); self.local_controls(list(self.units))
            if kind in (14, 25) and any(value[0] == 0xbe for value in self.p.children(encoded)):
                values = self.p.children(encoded)
                required = kind == 25 or any(value[0] == 0xa2 and
                    self.p.contents(self.p.children(value)[0]) == b"\0" for value in values)
                if required:
                    self.testing = True
                    try:
                        invalid = self.p.tlv(encoded[0], b"".join(v for v in values if v[0] != 0xbe))
                        try: self.payload(kind, pci, invalid)
                        except (AssertionError, ValueError, KeyError): self.negative_checks += 1
                        else: raise AssertionError("TX missing mandatory information accepted")
                    finally: self.testing = False
        if kind == 1:
            self.note("EncrTransfer", "unselected feature; server emits integrity-only ClearTransfer")
            self.note("User-data.entry[0].single-ASN1-type.encrTransfer",
                      "unselected feature; server emits integrity-only ClearTransfer")

    def save(self, directory, mapping):
        directory = Path(directory)
        rows = list(csv.DictReader(Path(mapping).open(encoding="utf-8", newline="")))
        ledger = []
        rx = [case for file in directory.glob("field-campaign-*.json")
              for case in json.loads(file.read_text())["cases"] if "schema_path" in case]
        for row in rows:
            checks = sorted(self.observed.get(row["schema_path"], ())) if "TX" in row["direction"] else []
            canonical = row["schema_path"]
            if row["container"] == "AARQ" and ".single-ASN1-type." in canonical:
                canonical = "HandshakeReq." + canonical.split(".single-ASN1-type.", 1)[1]
            elif row["container"] == "AARE" and ".single-ASN1-type." in canonical:
                for alternative, root in (("assoAccept", "HandshakeAcc"),
                                          ("applReject", "ApplicationReject"),
                                          ("secReject", "HandshakeSecReject")):
                    marker = ".single-ASN1-type." + alternative
                    if marker in canonical:
                        canonical = root + canonical.split(marker, 1)[1]
                        break
            elif row["container"] == "RLRQ" and ".single-ASN1-type." in canonical:
                canonical = "ReleaseReq." + canonical.split(".single-ASN1-type.", 1)[1]
            elif row["container"] == "ABRT":
                for alternative, root in (("handsSecAbort", "HandshakeSecAbort"),
                                          ("dtSecAbort", "DtSecAbort"), ("applAbort", "ApplAbort")):
                    marker = ".single-ASN1-type." + alternative
                    if marker in canonical:
                        canonical = root + canonical.split(marker, 1)[1]
                        break
            elif row["container"] == "User-data" and ".clearTransfer." in canonical:
                canonical = "ClearTransfer." + canonical.split(".clearTransfer.", 1)[1]
            elif row["container"] == "User-data" and ".encrTransfer" in canonical:
                canonical = "EncrTransfer" + canonical.split(".encrTransfer", 1)[1]
            aliases = {canonical, row["schema_path"]}
            cases = sorted({case["case"] for case in rx if "RX" in row["direction"] and
                any(case["schema_path"] == alias or case["schema_path"].startswith(alias + ".")
                    for alias in aliases)})
            gates = []
            if row["applicability"].startswith("CONDITIONAL") and "RX" in row["direction"]:
                gates = sorted({case["case"] for case in rx if
                    case.get("assertion") == "unsupported-feature gate" and
                    (canonical == case["schema_path"] or canonical.startswith(case["schema_path"] + "."))})
            if "certPath.pkCert" in canonical:
                remaining = "Live intermediate-chain presence/order/fragmentation/lifecycle; applicable F01-F10 variants"
            elif row["applicability"].startswith("CONDITIONAL"):
                remaining = "Conditional feature unselected: positive execution requires implementation/profile selection; unsupported-choice gate only where recorded"
            elif checks or cases:
                remaining = "PARTIAL: complete remaining applicable F01-F10 variants and state/resource qualification; direct assertions are not a per-row completion claim"
            else:
                remaining = "No direct field assertion in this native campaign; standalone/resilience evidence is separate; add applicable F01-F10 cases"
            ledger.append(dict(map_id=row["map_id"], schema_path=row["schema_path"],
                direction=row["direction"], tx_assertions=";".join(checks),
                tx_negative_controls=self.negative_by_path.get(row["schema_path"], 0),
                rx_cases=";".join(cases), inherited_unsupported_gates=";".join(gates), applicability=row["applicability"],
                status="PARTIAL - recorded host assertions executed" if checks or cases else "OPEN - no direct assertion from this campaign",
                remaining=remaining))
        report = dict(frames=self.frames, emitted_shapes=len(self.samples), local_negative_controls=self.negative_checks,
            schema_paths={k: sorted(v) for k, v in sorted(self.observed.items())},
            rows_with_tx_assertions=sum(bool(row["tx_assertions"]) for row in ledger),
            rows_with_rx_cases=sum(bool(row["rx_cases"]) for row in ledger), mapped_rows=len(rows),
            mapping_sha256=hashlib.sha256(Path(mapping).read_bytes()).hexdigest())
        (directory / "tx-contract.json").write_text(json.dumps(report, indent=2) + "\n")
        with (directory / "field-execution-ledger.csv").open("w", newline="") as stream:
            writer = csv.DictWriter(stream, fieldnames=list(ledger[0])); writer.writeheader(); writer.writerows(ledger)
        print("PASS independent TX contract", {k: v for k, v in report.items() if k != "schema_paths"})
