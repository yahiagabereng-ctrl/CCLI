"""Native PDU/allocator boundaries using the test-only application callback.

The synthetic callback returns valid padded ReadResponses to isolate native
transport/protection limits from legacy MMS object-model string-size limits.
"""
import json
import itertools
from pathlib import Path


def run_campaign(args, process, p, use_ec):
    records = []
    start = None

    def command(text, expected=None):
        process.stdin.write(text + "\n"); process.stdin.flush()
        result = process.stdout.readline().strip()
        if expected is not None and result != expected:
            raise AssertionError(f"test fixture command {text}: {result}")
        return result

    def metrics():
        result = command("metrics").split()
        if len(result) != 3 or result[0] != "METRICS": raise AssertionError(result)
        return int(result[1]), int(result[2])

    def record(case, **details):
        records.append(dict(case=case, result="PASS", profile="EC" if use_ec else "RSA", **details))
        print("PASS FIELD", case, details)

    def peer():
        result = p.NativePeer(args); result.connect(); return result

    def terminal(client, application=False, diagnostic="any"):
        kind, pci, response = client.receive()
        if (kind, pci, response[0]) != (25, 1, 0x64): raise AssertionError("missing terminal response")
        value = client.verify_control(response)
        if value["kind"] != (0xa2 if application else 0xa1): raise AssertionError(value)
        if diagnostic != "any" and value["diagnostic"] != diagnostic: raise AssertionError(value)
        if client.sock.recv(1): raise AssertionError("extra bytes after terminal response")

    def exact_request(size):
        count = size
        for _ in range(8):
            value = p.mms_read("E2ELD0", "Boundary" + "x" * count, 1)
            if len(value) == size: return value
            count += size - len(value)
        raise AssertionError("unable to construct exact-size MMS request")

    start = metrics()[0]
    command("config-boundaries", "CONFIG_BOUNDARIES 12")
    record("CONFIG-BOUNDS", checks=12, assertion="rekey/PDU/replay limits at configured extrema and outside")
    args.icv = "gmac256"
    for contexts in ((1, 3, 5), (3, 5, 7), (123, 125, 127)):
        args.contexts = contexts
        for order in itertools.permutations(range(3)):
            client = p.NativePeer(args)
            try:
                client.connect(context_order=order)
                client.read("E2ELD0", "LLN0$ST$Health$stVal")
                client.release()
                record("CONTEXT-" + "-".join(map(str, contexts)) + "-" + "".join(map(str, order)),
                       schema_path="PresentationConnect.context-list[*]", assertion="ID/order negotiation and protected transfer")
                # One successful native association is positive receive
                # evidence for all fields that this development peer emitted.
                # These are positive-presence records; optional certPath and
                # ASN DEFAULT fields are deliberately excluded.
                positive_paths = [
                    "AARQ", "AARQ.application-context-name", "AARQ.calling-AP-title",
                    "AARQ.calling-AE-qualifier", "AARQ.user-information",
                    "AARQ.user-information.entry[0]",
                    "AARQ.user-information.entry[0].indirect-reference",
                    "AARQ.user-information.entry[0].single-ASN1-type",
                    "HandshakeReq", "HandshakeReq.pp", "HandshakeReq.tbs",
                    "HandshakeReq.tbs.token1", "HandshakeReq.tbs.token1.sigAlg",
                    "HandshakeReq.tbs.token1.sigAlg.algorithm", "HandshakeReq.tbs.token1.pkCert",
                    "HandshakeReq.tbs.token1.assoID", "HandshakeReq.tbs.token1.dHKey",
                    "HandshakeReq.tbs.token1.dHKey.groupId", "HandshakeReq.tbs.token1.dHKey.dhPublicKey",
                    "HandshakeReq.tbs.token1.hmac", "HandshakeReq.tbs.token1.hmac.algorithm",
                    "HandshakeReq.tbs.token1.time", "HandshakeReq.tbs.token1.encr-mode",
                    "HandshakeReq.tbs.token1.encr-mode.non-aea",
                    "HandshakeReq.tbs.token1.encr-mode.non-aea.icvAlgID",
                    "HandshakeReq.tbs.token1.encr-mode.non-aea.icvAlgID.algo[*]",
                    "HandshakeReq.tbs.token1.encr-mode.non-aea.icvAlgID.algo[*].algorithm",
                    "HandshakeReq.tbs.token1.confParams", "HandshakeReq.tbs.applData",
                    "HandshakeReq.sign", "HandshakeReq.sign.algo", "HandshakeReq.sign.sign",
                    "PresentationConnect", "PresentationConnect.mode-selector",
                    "PresentationConnect.calling-selector", "PresentationConnect.called-selector",
                    "PresentationConnect.context-list[*]", "PresentationConnect.context-list[*].identifier",
                    "PresentationConnect.context-list[*].abstract-syntax-name",
                    "PresentationConnect.context-list[*].transfer-syntax-name[*]",
                    "PresentationConnect.user-data", "SessionConnect", "SessionConnect.version",
                    "SessionConnect.duplex", "SessionConnect.length", "SessionConnect.user-data",
                    "ReleaseReq", "ReleaseReq.tbs.token3", "ReleaseReq.tbs.token3.sigAlg",
                    "ReleaseReq.tbs.token3.sigAlg.algorithm", "ReleaseReq.tbs.token3.assoID",
                    "ReleaseReq.tbs.token3.time", "ReleaseReq.tbs.token3.pkCert",
                    "ReleaseReq.sign", "ReleaseReq.sign.algo", "ReleaseReq.sign.sign",
                    "ClearTransfer", "ClearTransfer.tbp", "ClearTransfer.tbp.token2",
                    "ClearTransfer.tbp.token2.assoID", "ClearTransfer.tbp.token2.time",
                    "ClearTransfer.tbp.token2.seq", "ClearTransfer.tbp.applData",
                    "ClearTransfer.auth", "ClearTransfer.auth.algo", "ClearTransfer.auth.algo.algorithm",
                    "ClearTransfer.auth.icv", "RLRQ", "RLRQ.user-information",
                    "RLRQ.user-information.entry[0]", "RLRQ.user-information.entry[0].indirect-reference",
                    "RLRQ.user-information.entry[0].single-ASN1-type",
                    "RLRQ.user-information.entry[0].single-ASN1-type.tbs",
                    "RLRQ.user-information.entry[0].single-ASN1-type.tbs.token3",
                    "RLRQ.user-information.entry[0].single-ASN1-type.sign"]
                for path in positive_paths:
                    record("NATIVE-POSITIVE-" + path, schema_path=path,
                           assertion="present in valid peer wire message; native acceptance; read/release passed")
                optional_absences = [
                    "AARQ.protocol-version", "AARQ.called-AP-title", "AARQ.called-AE-qualifier",
                    "AARQ.called-AP-invocation-id", "AARQ.called-AE-invocation-id",
                    "AARQ.calling-AP-invocation-id", "AARQ.calling-AE-invocation-id",
                    "AARQ.implementation-information", "AARQ.sender-acse-requirements",
                    "AARQ.mechanism-name", "AARQ.calling-authentication-value",
                    "AARQ.application-context-name-list", "RLRQ.reason", "ABRT.abort-diagnostic",
                    "ABRT.user-information.entry[0].single-ASN1-type.osiDiag",
                    "HandshakeReq.tbs.token1.version", "HandshakeReq.tbs.token1.certPath",
                    "HandshakeReq.tbs.token1.attCert", "ClearTransfer.tbp.token2.version",
                    "ClearTransfer.tbp.token2.iv", "ClearTransfer.tbp.token2.rekey",
                    "ClearTransfer.tbp.token2.reqRekey", "ClearTransfer.tbp.token2.changedKey"]
                for path in optional_absences:
                    record("NATIVE-OPTIONAL-ABSENT-" + path, schema_path=path,
                           assertion="optional/default field omitted in valid baseline; native association accepted")
            finally: client.close()
    args.contexts = (1, 3, 5)
    for size in (10239, 10240, 10241):
        client = p.NativePeer(args)
        try:
            try: client.connect(connect_user_data_size=size)
            except (EOFError, ConnectionError):
                if size <= 10240: raise
            else:
                if size > 10240: raise AssertionError("oversized complete CONNECT accepted")
                client.release()
            if client.last_connect_user_data_size != size: raise AssertionError("CONNECT case did not hit exact boundary")
            record(f"CONNECT-{size}", schema_path="SessionConnect.user-data", assertion="complete CONNECT user-data limit", encoded_size=size)
        finally: client.close()

    def mutate_sequence(encoded, index, variant):
        values = p.children(encoded)
        if variant == "missing": del values[index]
        elif variant == "duplicate": values.insert(index, values[index])
        elif variant == "wrong-tag": values[index] = b"\x05\0"
        elif variant == "order": values[index], values[index+1] = values[index+1], values[index]
        else: raise AssertionError(variant)
        return p.tlv(encoded[0], b"".join(values))

    def acse_mutate(fields, index, variant):
        return p.contents(mutate_sequence(p.tlv(0x30, fields), index, variant))

    for index, name in enumerate(("application-context-name", "calling-AP-title", "calling-AE-qualifier", "user-information")):
        for variant in ("missing", "duplicate", "wrong-tag", "order"):
            if variant == "order" and index == 3: continue
            client = p.NativePeer(args); before = metrics()[0]
            try:
                try: client.connect(aarq_mutator=lambda value, i=index, v=variant: acse_mutate(value, i, v))
                except ValueError as error:
                    expected = "native OSI diagnostic: 1" if index == 3 and variant == "missing" else "native ACSE provider abort"
                    if str(error) != expected: raise
                else: raise AssertionError("malformed AARQ accepted")
                if client.sock.recv(1): raise AssertionError("AARQ failure remained open")
                if metrics()[0] != before: raise AssertionError("AARQ fault dispatched read")
                record(f"RX-AARQ-{name}-{variant}", schema_path="AARQ." + name, assertion=variant)
            finally: client.close()

    def add_acse(fields, field):
        values = p.children(p.tlv(0x30, fields)) + [field]
        return b"".join(sorted(values, key=lambda value: value[0] & 31))

    for name, field, accepted in [
        ("protocol-version", p.tlv(0x80, b"\x07\x80"), True),
        ("called-AP-title", p.tlv(0xa2, p.tlv(6, p.oid_value(args.server_ap_title))), True),
        ("called-AE-qualifier", p.tlv(0xa3, p.integer(args.server_ae_qualifier)), True),
        ("called-AP-invocation-id", p.tlv(0xa4, p.integer(0)), True),
        ("called-AE-invocation-id", p.tlv(0xa5, p.integer(0)), True),
        ("calling-AP-invocation-id", p.tlv(0xa8, p.integer(0)), True),
        ("calling-AE-invocation-id", p.tlv(0xa9, p.integer(0)), True),
        ("sender-acse-requirements", p.tlv(0x8a, b"\x07\x80"), False),
        ("mechanism-name", p.tlv(0x8b, p.oid_value(p.HMAC)), False),
        ("calling-authentication-value", p.tlv(0xac, p.tlv(0x80, b"auth")), False),
        ("application-context-name-list", p.tlv(0xad, p.tlv(6, p.oid_value(p.CONTEXT))), False),
        ("implementation-information", p.tlv(0x9d, b"test-implementation"), False)]:
        client = p.NativePeer(args); before = metrics()[0]
        try:
            try: client.connect(aarq_mutator=lambda value, field=field: add_acse(value, field))
            except ValueError as error:
                if accepted or str(error) != "native ACSE provider abort": raise
                if client.sock.recv(1): raise AssertionError("forbidden AARQ field remained open")
            else:
                if not accepted: raise AssertionError("forbidden AARQ field accepted")
                client.release()
            if metrics()[0] != before: raise AssertionError("AARQ field case dispatched read")
            record(f"RX-AARQ-{name}-present", schema_path="AARQ." + name, assertion="presence disposition", accepted=accepted,
                   source_gate="optional implementation-information support disposition" if name == "implementation-information" else "")
        finally: client.close()

    def change_token(token, index, replacement):
        values = p.children(token); values[index] = replacement
        return p.tlv(0x30, b"".join(values))

    for name, mutate, expected, path in [
        ("AEA", lambda token: change_token(token, 6, p.tlv(0x30, p.algorithm(p.GMAC128))), 17, "encr-mode.aea"),
        ("encryption-list", lambda token: change_token(token, 6, p.tlv(0xa0,
            p.tlv(0xa0, p.algorithm("2.16.840.1.101.3.4.1.2")) + p.tlv(0xa1, p.algorithm(p.HMAC)))), 6, "encr-mode.non-aea.encr"),
        ("attribute-certificate", lambda token: p.tlv(0x30, p.contents(token) + p.tlv(4, b"\x30\0")), 16, "attCert")]:
        client = p.NativePeer(args)
        try:
            try: client.connect(token_mutator=mutate)
            except ValueError as error:
                if str(error) != "signed native handshake rejection" or client.rejection_diagnostic != expected: raise
            else: raise AssertionError("unselected feature accepted")
            if client.sock.recv(1): raise AssertionError("unsupported feature remained open")
            record("RX-UNSUPPORTED-" + name, schema_path="HandshakeReq.tbs.token1." + path,
                   assertion="unsupported-feature gate", diagnostic=expected)
        finally: client.close()

    # Mutations are made before signing/MAC, so a structural fault cannot hide
    # behind an invalid authenticator. Legal tail-extension ambiguities get a
    # positive assertion instead of an invented duplicate rejection rule.
    for index, name in enumerate(("sigAlg", "pkCert", "assoID", "dHKey", "hmac", "time", "encr-mode", "confParams")):
        for variant in ("missing", "duplicate", "wrong-tag", "order"):
            if variant == "order" and index == 7: continue
            client = p.NativePeer(args); accepted = index == 7 and variant == "duplicate"
            before = metrics()[0]
            try:
                try: client.connect(token_mutator=lambda value, i=index, v=variant: mutate_sequence(value, i, v))
                except ValueError as error:
                    if accepted or str(error) != "signed native handshake rejection": raise
                    expected = 3 if index == 0 and variant == "wrong-tag" else 1
                    if client.rejection_kind != 0xa3 or client.rejection_diagnostic != expected:
                        raise AssertionError((name, variant, client.rejection_diagnostic, expected))
                    if client.sock.recv(1): raise AssertionError("handshake rejection left connection open")
                else:
                    if not accepted: raise AssertionError("invalid handshake root accepted")
                    client.release()
                if metrics()[0] != before: raise AssertionError("handshake field fault dispatched application read")
                record(f"RX-TOKEN1-{name}-{variant}", schema_path="HandshakeReq.tbs.token1." + name,
                       assertion=variant, accepted=accepted)
            finally: client.close()

    # Each nested mutation is authenticated, with an otherwise valid handshake.
    nested = []
    for outer, name, fields in ((0, "sigAlg", ("algorithm",)),
                                (3, "dHKey", ("groupId", "dhPublicKey")),
                                (4, "hmac", ("algorithm",))):
        for index, child in enumerate(fields):
            for variant in ("missing", "wrong-tag", "order"):
                if variant == "order" and index + 1 == len(fields): continue
                nested.append((f"{name}-{child}-{variant}", name + "." + child,
                    lambda token, o=outer, i=index, v=variant: change_token(token, o,
                        mutate_sequence(p.children(token)[o], i, v))))
    nested += [
        ("sigAlg-unknown", "sigAlg.algorithm", lambda t: change_token(t, 0, p.algorithm("1.2.3"))),
        ("sigAlg-parameters", "sigAlg.parameters", lambda t: change_token(t, 0,
            p.tlv(0x30, p.children(p.children(t)[0])[0] + p.integer(1)))),
        ("hmac-unknown", "hmac.algorithm", lambda t: change_token(t, 4, p.algorithm("1.2.3"))),
        ("hmac-parameters", "hmac.parameters", lambda t: change_token(t, 4,
            p.tlv(0x30, p.children(p.children(t)[4])[0] + p.integer(1)))),
        ("DH-unsupported-group", "dHKey.groupId", lambda t: change_token(t, 3,
            p.tlv(0x30, p.integer(999) + p.children(p.children(t)[3])[1]))),
        ("DH-empty-public", "dHKey.dhPublicKey", lambda t: change_token(t, 3,
            p.tlv(0x30, p.children(p.children(t)[3])[0] + p.tlv(4, b"")))),
        ("DH-zero-public", "dHKey.dhPublicKey", lambda t: change_token(t, 3,
            p.tlv(0x30, p.children(p.children(t)[3])[0] + p.tlv(4, b"\0" * (65 if use_ec else 256))))),
        ("empty-path", "certPath", lambda t: p.tlv(0x30,
            b"".join(p.children(t)[:2]) + p.tlv(0x30, b"") + b"".join(p.children(t)[2:]))),
        ("invalid-version", "version", lambda t: p.tlv(0x30,
            b"".join(p.children(t)[:2]) + p.tlv(3, b"\x06\x40") + b"".join(p.children(t)[2:]))),
        ("empty-icv-set", "encr-mode.non-aea.icvAlgID", lambda t: change_token(t, 6,
            p.tlv(0xa0, p.tlv(0xa1, b"")))),
        ("unknown-icv", "encr-mode.non-aea.icvAlgID.algo[*].algorithm", lambda t: change_token(t, 6,
            p.tlv(0xa0, p.tlv(0xa1, p.algorithm("1.2.3"))))),
        ("invalid-icv-params", "encr-mode.non-aea.icvAlgID.algo[*].parameters", lambda t: change_token(t, 6,
            p.tlv(0xa0, p.tlv(0xa1, p.tlv(0x30, p.tlv(6, p.oid_value(p.GMAC256)) + p.tlv(0x30, p.integer(11)))))))
    ]
    nested_diagnostics = {"hmac-unknown": 5, "hmac-parameters": 5,
        "DH-unsupported-group": 11, "DH-zero-public": 11, "invalid-version": 4,
        "unknown-icv": 13, "invalid-icv-params": 13}
    for label, path, mutation in nested:
        client = p.NativePeer(args); before = metrics()[0]
        try:
            try: client.connect(token_mutator=mutation)
            except ValueError as error:
                if str(error) != "signed native handshake rejection": raise
                expected = 3 if label.startswith("sigAlg-") else nested_diagnostics.get(label, 1)
                if (client.rejection_kind, client.rejection_diagnostic) != (0xa3, expected):
                    raise AssertionError((label, client.rejection_kind, client.rejection_diagnostic, expected))
                if client.sock.recv(1): raise AssertionError("nested field rejection remained open")
            else: raise AssertionError("invalid nested field accepted: " + label)
            if metrics()[0] != before: raise AssertionError("nested field dispatched application")
            record("RX-NESTED-" + label, schema_path="HandshakeReq.tbs.token1." + path,
                   assertion="authenticated nested invalid value/structure", diagnostic=client.rejection_diagnostic)
        finally: client.close()

    for abort_choice, container in ((None, "ReleaseReq"), (0xa0, "HandshakeSecAbort"),
                                    (0xa1, "DtSecAbort"), (0xa2, "ApplAbort")):
        for index, name in enumerate(("sigAlg", "assoID", "time", "pkCert")):
            for variant in ("missing", "duplicate", "wrong-tag", "order"):
                if variant == "order" and index == 3: continue
                client = peer(); before = metrics()[0]
                accepted = index == 3 and variant == "duplicate"
                try:
                    client.send_control(abort_choice=abort_choice, token_mutator=lambda value, i=index, v=variant: mutate_sequence(value, i, v))
                    if accepted and abort_choice is not None:
                        if client.sock.recv(1): raise AssertionError("valid peer abort received a response")
                        inner = container
                        for path in ("ABRT", "ABRT.abort-source", "ABRT.user-information",
                            "ABRT.user-information.entry[0]", "ABRT.user-information.entry[0].indirect-reference",
                            "ABRT.user-information.entry[0].single-ASN1-type",
                            f"ABRT.user-information.entry[0].single-ASN1-type.{('handsSecAbort' if abort_choice == 0xa0 else 'dtSecAbort' if abort_choice == 0xa1 else 'applAbort')}",
                            "ABRT.user-information.entry[0].single-ASN1-type." +
                                ("handsSecAbort" if abort_choice == 0xa0 else "dtSecAbort" if abort_choice == 0xa1 else "applAbort") + ".tbs",
                            "ABRT.user-information.entry[0].single-ASN1-type." +
                                ("handsSecAbort" if abort_choice == 0xa0 else "dtSecAbort" if abort_choice == 0xa1 else "applAbort") + ".sign"):
                            record("RX-VALID-" + inner, schema_path=path,
                                   assertion="valid signed ABRT alternative; silent terminal close; no response")
                    else:
                        kind, pci, response = client.receive()
                        if (kind, pci, response[0]) != ((10, 1, 0x63) if accepted else (25, 1, 0x64)):
                            raise AssertionError("wrong control field response")
                        result = client.verify_control(response)
                        if not accepted and result["diagnostic"] != (None if index == 1 and variant == "missing" else 1):
                            raise AssertionError((container, name, variant, result))
                        if client.sock.recv(1): raise AssertionError("terminal control remained open")
                    if metrics()[0] != before: raise AssertionError("control field fault dispatched read")
                    record(f"RX-{container}-TOKEN3-{name}-{variant}", schema_path=container + ".tbs.token3." + name,
                           assertion=variant, accepted=accepted)
                finally: client.close()

    for index, name in enumerate(("assoID", "time", "seq")):
        for variant in ("missing", "duplicate", "wrong-tag", "order"):
            if variant == "order" and index == 2: continue
            client = peer(); before = metrics()[0]
            accepted = index == 2 and variant == "duplicate"
            def mutate_tbp(encoded, i=index, v=variant):
                token, application = p.children(encoded)
                return p.tlv(0x30, mutate_sequence(token, i, v) + application)
            try:
                packet = client.protect(p.mms_read("E2ELD0", "LLN0$ST$Health$stVal", 1), tbp_mutator=mutate_tbp)
                client.send_cotp(b"\x01\0\x01\0" + p.presentation(5, packet))
                if accepted:
                    kind, pci, response = client.receive()
                    if (kind, pci) != (1, 5) or b"\x85\x01\x07" not in client.unprotect(response):
                        raise AssertionError("legal token extension rejected")
                    client.release()
                else: terminal(client, diagnostic=1)
                after = metrics()[0]
                if after - before != accepted: raise AssertionError("token2 field dispatch mismatch")
                record(f"RX-TOKEN2-{name}-{variant}", schema_path="ClearTransfer.tbp.token2." + name,
                       assertion=variant, accepted=accepted)
            finally: client.close()

    for index, name in enumerate(("nonce", "algo", "icv")):
        for variant in ("missing", "duplicate", "wrong-tag", "order"):
            if variant == "order" and index == 2: continue
            client = peer(); before = metrics()[0]
            accepted = index == 2 and variant == "duplicate"
            try:
                packet = client.protect(p.mms_read("E2ELD0", "LLN0$ST$Health$stVal", 1),
                    auth_mutator=lambda value, i=index, v=variant: mutate_sequence(value, i, v))
                client.send_cotp(b"\x01\0\x01\0" + p.presentation(5, packet))
                if accepted:
                    kind, pci, response = client.receive()
                    if (kind, pci) != (1, 5) or b"\x85\x01\x07" not in client.unprotect(response):
                        raise AssertionError("authenticator extension changed application data")
                    client.release()
                else: terminal(client, diagnostic=6 if index == 0 and variant in ("missing", "order") else 1)
                if metrics()[0] - before != accepted: raise AssertionError("authenticator field dispatch mismatch")
                record(f"RX-AUTH-{name}-{variant}", schema_path="ClearTransfer.auth." + name,
                       assertion=variant, accepted=accepted)
            finally: client.close()

    for variant in ("missing", "duplicate", "wrong-tag"):
        client = peer(); before = metrics()[0]
        try:
            packet = client.protect(p.mms_read("E2ELD0", "LLN0$ST$Health$stVal", 1),
                tbp_mutator=lambda value, v=variant: mutate_sequence(value, 1, v))
            client.send_cotp(b"\x01\0\x01\0" + p.presentation(5, packet))
            if variant == "duplicate":
                kind, pci, response = client.receive()
                if (kind, pci) != (1, 5) or b"\x85\x01\x07" not in client.unprotect(response):
                    raise AssertionError("TBP extension altered dispatch")
                client.release()
            else: terminal(client, diagnostic=1)
            if metrics()[0] - before != (variant == "duplicate"): raise AssertionError("application root dispatch mismatch")
            record(f"RX-APPLICATION-{variant}", schema_path="ClearTransfer.tbp.applData", assertion=variant)
        finally: client.close()

    for mode in ("hmac", "gmac128", "gmac256"):
        args.icv = mode
        for size in (63999, 64000, 64001):
            command("response-size 128", "SIZE_SET")
            client = peer()
            before = metrics()[0]
            try:
                request = exact_request(size)
                client.send_cotp(b"\x01\0\x01\0" + p.presentation(5, client.protect(request)))
                if size <= 64000:
                    kind, pci, response = client.receive()
                    if (kind, pci) != (1, 5) or len(client.unprotect(response)) != 128:
                        raise AssertionError("valid native request size rejected")
                    client.release()
                else: terminal(client, diagnostic=1)
                after = metrics()[0]
                if after - before != (size <= 64000): raise AssertionError("request size dispatch boundary")
                record(f"PDU-RX-{mode}-{size}", request_size=size, dispatches=after-before)
            finally: client.close()
        for size in (63999, 64000, 64001, 65535, 65536):
            command(f"response-size {size}", "SIZE_SET")
            client = peer()
            before = metrics()[0]
            try:
                client.invoke = 1
                request = p.mms_read("E2ELD0", "Boundary", 1)
                client.send_cotp(b"\x01\0\x01\0" + p.presentation(5, client.protect(request)))
                if size <= 64000:
                    kind, pci, response = client.receive()
                    application = client.unprotect(response)
                    if (kind, pci) != (1, 5) or len(application) != size:
                        raise AssertionError("valid maximum native response lost or truncated")
                    invoke, read_result = p.children(application, 0xa1)
                    result_list, = p.children(read_result, 0xa4)
                    data, = p.children(result_list, 0xa1)
                    if data[0] != 0x89 or p.contents(data) != b"Z" * len(p.contents(data)):
                        raise AssertionError("maximum ReadResponse content corrupted")
                    client.release()
                else: terminal(client, application=size <= 65535, diagnostic=None)
                after = metrics()[0]
                if after - before != 1: raise AssertionError("response size dispatch count")
                record(f"PDU-TX-{mode}-{size}", response_size=size, dispatches=1)
            finally: client.close()
    # The selected profile is integrity-only. A structurally valid protected
    # payload under the encrypted CHOICE is rejected before dispatch.
    client = peer(); before = metrics()[0]
    try:
        protected = client.protect(p.mms_read("E2ELD0", "LLN0$ST$Health$stVal", 1))
        encrypted = p.tlv(0xa1, p.contents(protected))
        client.send_cotp(b"\x01\0\x01\0" + p.presentation(5, encrypted))
        terminal(client, diagnostic=3)
        if metrics()[0] != before: raise AssertionError("unselected encrypted transfer reached MMS")
        record("RX-UNSUPPORTED-EncrTransfer", schema_path="EncrTransfer",
               assertion="encrypted CHOICE rejected in integrity-only profile", diagnostic=3)
    finally: client.close()
    command("response-size -1", "SIZE_SET")
    args.icv = "gmac256"
    injected = 0
    for position in range(128):
        client = peer()
        before = metrics()[0]
        try:
            command(f"fail-alloc {position}", "FAULT_SET")
            try:
                packet = client.protect(p.mms_read("E2ELD0", "LLN0$ST$Health$stVal", 1))
                client.send_cotp(b"\x01\0\x01\0" + p.presentation(5, packet))
                kind, pci, response = client.receive()
                if (kind, pci) == (1, 5):
                    if b"\x85\x01\x07" not in client.unprotect(response):
                        raise AssertionError("wrong response after allocation injection")
                    client.release()
                elif (kind, pci, response[0]) == (25, 1, 0x64):
                    client.verify_control(response)
                    if client.sock.recv(1): raise AssertionError("allocator fault did not close association")
                else: raise AssertionError("unexpected allocator failure response")
            except (EOFError, ConnectionError):
                pass  # Exhausted error-path allocations can prevent an abort.
            after, hits = metrics()
            if not 0 <= after - before <= 1: raise AssertionError("allocation fault caused excess dispatch")
            record(f"ALLOC-RX-{position}", fault_hits=hits, dispatches=after-before)
            if hits: injected += 1
            else: break
        finally: client.close()
        # The failed connection must release its IDs, keys and authorization
        # state; a fresh fully authenticated connection must remain usable.
        healthy = peer()
        try: healthy.read("E2ELD0", "LLN0$ST$Health$stVal"); healthy.release()
        finally: healthy.close()
    else: raise AssertionError("allocation sweep did not reach a fault-free receive")
    if not injected: raise AssertionError("allocator injection never exercised")
    total = metrics()[0] - start
    output = Path(args.key).parent / ("field-campaign-ec.json" if use_ec else "field-campaign-rsa.json")
    output.write_text(json.dumps({"cases": records, "authorized_reads": total}, indent=2) + "\n")
    return total
