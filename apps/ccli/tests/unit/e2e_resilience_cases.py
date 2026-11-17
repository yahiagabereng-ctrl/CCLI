"""Native wire resilience cases; all certificates/keys are disposable fixtures."""
import datetime as dt
from concurrent.futures import ThreadPoolExecutor


def run_resilience(args, process, p):
    reads = 0

    def change(token, index, value):
        fields = p.children(token)
        fields[index] = value
        return p.tlv(0x30, b"".join(fields))

    def version(token):
        fields = p.children(token)
        fields.insert(2, p.tlv(3, b"\x06\x40"))
        return p.tlv(0x30, b"".join(fields))

    def dh(token, group=None, public=None):
        fields = p.children(token)
        offered_group, offered_public = p.children(fields[3])
        fields[3] = p.tlv(0x30, (offered_group if group is None else p.integer(group)) +
                          (offered_public if public is None else p.tlv(4, public)))
        return p.tlv(0x30, b"".join(fields))

    def closed(peer):
        if peer.sock.recv(1): raise AssertionError("terminal association sent unexpected additional data")

    def terminal(peer, *, diagnostic=None, osi=None, provider=False, application=None):
        kind, pci, response = peer.receive()
        if (kind, pci, response[0]) != (25, 1, 0x64):
            raise AssertionError("expected terminal ACSE abort")
        if provider:
            if response != p.tlv(0x64, p.tlv(0x80, b"\x01")):
                raise AssertionError("provider abort contains wrong source or unexpected information")
        elif osi is not None:
            if peer.verify_osi_abort(response) != osi: raise AssertionError("wrong OSI diagnostic")
        else:
            result = peer.verify_control(response)
            if result["diagnostic"] != diagnostic: raise AssertionError("wrong security diagnostic: " + str(result))
            if application is not None and (result["kind"] != 0xa2 or result["application"] != application):
                raise AssertionError("ApplAbort lost application diagnostic")
        closed(peer)

    # All signed mutations are applied before signing; signature failure is a
    # separate case so semantic faults cannot pass by merely failing the ICV.
    args.icv = "hmac"
    handshakes = [
        ("version", {"token_mutator": version}, 4),
        ("signature-before-version", {"token_mutator": version, "corrupt_signature": True}, None),
        ("protocol", {"protected_protocol": b"unsupported"}, 2),
        ("hmac", {"token_mutator": lambda t: change(t, 4, p.algorithm("1.3.14.3.2.26"))}, 5),
        ("group", {"token_mutator": lambda t: dh(t, group=999)}, 11),
        ("public-value", {"token_mutator": lambda t: dh(t, public=b"\0")}, 11),
        ("time", {"token_mutator": lambda t: change(t, 5, p.tlv(24, b"20000101000000Z"))}, 10),
        ("future-time", {"token_mutator": lambda t: change(t, 5, p.tlv(24, b"20990101000000Z"))}, 10),
        ("icv", {"token_mutator": lambda t: change(t, 6, p.tlv(0xa0, p.tlv(0xa1, p.algorithm("1.2.3.4"))))}, 13),
        ("AEA", {"token_mutator": lambda t: change(t, 6, p.tlv(0x30, p.algorithm(p.GMAC128)))}, 17),
        ("encryption", {"token_mutator": lambda t: change(t, 7, p.tlv(3, b"\x05\xc0"))}, 6),
        ("signature-algorithm-mismatch", {"token_mutator": lambda t: change(t, 0,
            p.algorithm(p.RSA if p.children(t)[0] != p.algorithm(p.RSA) else p.ECDSA))}, None),
        ("invalid-client-ID", {"token_mutator": lambda t: change(t, 2, p.tlv(12, b"1:2"))}, 1),
        ("application-init", {"application": p.tlv(0xa8, p.tlv(0xa4, p.tlv(0x80, b"\0")))}, "application"),
    ]
    for label, options, expected in handshakes:
        peer = p.NativePeer(args)
        try:
            try: peer.connect(**options)
            except ValueError as error:
                if str(error) != "signed native handshake rejection": raise
            else: raise AssertionError("accepted invalid signed handshake: " + label)
            if expected == "application":
                if peer.rejection_kind != 0xa2: raise AssertionError("missing signed ApplicationReject")
            elif peer.rejection_diagnostic != expected:
                raise AssertionError(f"{label}: wrong rejection diagnostic {peer.rejection_diagnostic}, expected {expected}")
            if peer.rejection_id is None: raise AssertionError("known client association ID omitted from reject")
            if expected != "application" and b":" in peer.rejection_id and label != "invalid-client-ID":
                raise AssertionError("server appended an ID to a rejected handshake")
            closed(peer)
        finally: peer.close()
        print("PASS handshake resilience", label)

    def acse_version(fields): return p.tlv(0x80, b"\x06\x40") + fields
    def insert_acse(fields, field):
        return b"".join(sorted(p.children(p.tlv(0x30, fields)) + [field], key=lambda v: v[0] & 31))
    acse = [
        ("duplicate", lambda f: f + p.tlv(0xa6, p.tlv(6, p.oid_value(args.ap_title))), "native ACSE provider abort"),
        ("version", acse_version, "native ACSE rejection: provider:2"),
        ("called-AP", lambda f: insert_acse(f, p.tlv(0xa2, p.tlv(6, p.oid_value("1.2.3.4")))), "native ACSE rejection: user:7"),
        ("called-AE", lambda f: insert_acse(f, p.tlv(0xa3, p.integer(999))), "native ACSE rejection: user:9"),
        ("context", lambda f: b"".join(p.tlv(0xa1, p.tlv(6, p.oid_value("1.0.9506.2.3"))) if v[0] == 0xa1 else v
                                      for v in p.children(p.tlv(0x30, f))),
         "native ACSE rejection: user:2"),
        ("malformed-context", lambda f: b"".join(p.tlv(0xa1, p.integer(1)) if v[0] == 0xa1 else v
                                      for v in p.children(p.tlv(0x30, f))), "native ACSE provider abort"),
        ("missing-AP", lambda f: b"".join(v for v in p.children(p.tlv(0x30, f)) if v[0] != 0xa6), "native ACSE provider abort"),
    ]
    for label, mutate, expected in acse:
        peer = p.NativePeer(args)
        try:
            try: peer.connect(aarq_mutator=mutate)
            except ValueError as error:
                if str(error) != expected: raise
            else: raise AssertionError("invalid ACSE accepted: " + label)
            closed(peer)
        finally: peer.close()
        print("PASS ACSE resilience", label)

    def packet(peer, label, timestamp=None, sequence=None):
        original = peer.protect(p.mms_read("E2ELD0", "LLN0$ST$Health$stVal", 1))
        tbp, auth = p.children(original)
        token, app = p.children(tbp)
        fields = p.children(token)
        if timestamp is not None: fields[1] = p.tlv(24, timestamp)
        if sequence is not None: fields[2] = p.integer(sequence)
        if label in ("version", "ICV-before-version"): fields.insert(0, p.tlv(3, b"\x06\x40"))
        if label == "negative-sequence": fields[2] = p.tlv(2, b"\xff")
        if label == "duplicate-token": fields.insert(1, fields[0])
        if label == "future-time": fields[1] = p.tlv(24, b"20990101000000Z")
        if label == "invalid-rekey": fields.append(p.tlv(0x81, b"\0"))
        if label == "oversized-MMS": app = p.tlv(0xa0, p.tlv(4, b"\0" * 64000))
        body = p.tlv(0x30, b"".join(fields)) + app
        tbp = p.tlv(0x30, body)
        protected = body
        if label == "indefinite-TBP":
            tbp = b"\x30\x80" + body + b"\0\0"
            protected = body + b"\0\0"
        auth = p.tlv(0x30, peer.icv + p.tlv(4, peer.mac(peer.send_key, protected)))
        result = p.tlv(0xa0, tbp + auth)
        if label == "missing-auth": result = p.tlv(0xa0, tbp)
        if label == "ICV-before-version": result = result[:-1] + bytes([result[-1] ^ 1])
        if label == "encrypted-choice": result = p.tlv(0xa1, p.contents(result))
        return result

    for label, expected in [("version", 2), ("ICV-before-version", None), ("negative-sequence", 1),
            ("duplicate-token", 1), ("future-time", 7), ("invalid-rekey", 10),
            ("oversized-MMS", 1), ("missing-auth", 1), ("encrypted-choice", 3)]:
        peer = p.NativePeer(args)
        try:
            peer.connect()
            peer.send_cotp(b"\x01\0\x01\0" + p.presentation(5, packet(peer, label)))
            terminal(peer, diagnostic=expected)
        finally: peer.close()
        print("PASS authenticated transfer resilience", label)

    for label, user_data, expected in [
            ("missing-PCI", p.tlv(0x61, p.tlv(0x30, p.tlv(0xa0, b"\xa0\0"))), 1),
            ("wide-PCI", p.presentation(261, b"\xa0\0"), 3),
            ("empty-user-data", p.tlv(0x61, b""), 1),
            ("truncated-BER", b"\x61\x82\xff\xff", 1),
            ("deep-indefinite", b"\x61\x80" + b"\x30\x80" * 40 + b"\0\0" * 41, 1)]:
        peer = p.NativePeer(args)
        try:
            peer.connect(); peer.send_cotp(b"\x01\0\x01\0" + user_data)
            terminal(peer, osi=expected)
        finally: peer.close()
        print("PASS presentation resilience", label)

    peer = p.NativePeer(args)
    try:
        peer.connect()
        peer.send_cotp(b"\x01\0\x01\0" + p.presentation(5, packet(peer, "indefinite-TBP")))
        kind, pci, reply = peer.receive(); result = peer.unprotect(reply)
        if (kind, pci, result[0]) != (1, 5, 0xa1): raise AssertionError("valid indefinite TBP rejected")
        reads += 1; peer.release()
    finally: peer.close()
    print("PASS exact indefinite-TBP ICV/EOC")

    def exchange_pair(peer, timestamp, sequence):
        peer.send_cotp(b"\x01\0\x01\0" + p.presentation(5, packet(peer, "pair", timestamp, sequence)))
        kind, pci, reply = peer.receive()
        if (kind, pci, peer.unprotect(reply)[0]) != (1, 5, 0xa1): raise AssertionError("valid time/sequence rejected")
    instant = dt.datetime.now(dt.timezone.utc)
    t0 = instant.strftime("%Y%m%d%H%M%S.%fZ").encode()
    t1 = (instant + dt.timedelta(seconds=1)).strftime("%Y%m%d%H%M%S.%fZ").encode()
    peer = p.NativePeer(args)
    try:
        peer.connect()
        for timestamp, seq in [(t0, 0), (t0, 1), (t1, 0)]:
            exchange_pair(peer, timestamp, seq); reads += 1
        peer.release()
    finally: peer.close()
    print("PASS same-time increment and new-time sequence reset")
    for label, first, second, invalid, expected in [
        ("older-pair-replay", (t0, 0), (t1, 0), (t0, 0), None),
        ("unseen-regressed-time", (t1, 0), None, (t0, 0), 7),
        ("same-time-gap", (t0, 0), None, (t0, 2), 8),
        ("new-time-nonzero", (t0, 0), None, (t1, 1), 8)]:
        peer = p.NativePeer(args)
        try:
            peer.connect(); exchange_pair(peer, *first); reads += 1
            if second: exchange_pair(peer, *second); reads += 1
            peer.send_cotp(b"\x01\0\x01\0" + p.presentation(5, packet(peer, "pair", *invalid)))
            terminal(peer, diagnostic=expected)
        finally: peer.close()
        print("PASS timestamp/sequence resilience", label)

    peer = p.NativePeer(args)
    try:
        proposed = p.children(p.initiate())
        proposed[0] = p.integer(65000, 0x80)
        response = peer.connect(application=p.tlv(0xa8, b"".join(proposed)))
        components = p.children(response)
        info = next(v for v in components if v[0] == 0xbe)
        external, = p.children(info)
        _, encoded = p.children(external)
        tbs, _ = p.children(p.contents(encoded))
        mms_response = p.children(tbs)[1]
        maximum = int.from_bytes(p.contents(p.children(mms_response)[0]), "big")
        if maximum != 64000: raise AssertionError("native MMS negotiation exceeded native buffer policy")
        peer.release()
    finally: peer.close()
    print("PASS MMS negotiated PDU cap matches native serialization bounds")

    for label, options, expected, osi, provider in [
            ("release-signature", {"corrupt_signature": True}, None, None, False),
            ("release-ID", {"token_mutator": lambda t: change(t, 1, p.tlv(12, b"999:999"))}, None, None, False),
            ("release-time", {"token_mutator": lambda t: change(t, 2, p.tlv(24, b"20000101000000Z"))}, 7, None, False),
            ("release-version", {"token_mutator": lambda t: p.tlv(0x30,
                p.children(t)[0] + p.tlv(3, b"\x06\x40") + b"".join(p.children(t)[1:]))}, 2, None, False),
            ("release-path", {"token_mutator": lambda t: p.tlv(0x30,
                p.contents(t) + p.tlv(0x30, p.tlv(4, b"\x30\0")))}, None, None, False),
            ("release-reference", {"indirect_reference": 7}, None, 2, False),
            ("release-kernel", {"extra_acse": p.tlv(0x8b, b"\0")}, None, None, True),
            ("abort-signature", {"abort_choice": 0xa0, "corrupt_signature": True}, None, None, False)]:
        peer = p.NativePeer(args)
        try:
            peer.connect(); peer.send_control(**options)
            terminal(peer, diagnostic=expected, osi=osi, provider=provider)
        finally: peer.close()
        print("PASS control resilience", label)

    for choice in (0xa0, 0xa1, 0xa2):
        peer = p.NativePeer(args)
        try: peer.connect(); peer.send_control(abort_choice=choice); closed(peer)
        finally: peer.close()
        print("PASS signed peer abort", hex(choice))

    for diagnostic in (None, 1, 2, 3):
        peer = p.NativePeer(args)
        try:
            peer.connect()
            body = p.tlv(0x80, b"\x01" if diagnostic is None else b"\0")
            if diagnostic is not None:
                body += p.tlv(0xbe, p.tlv(0x28, p.integer(5) +
                    p.tlv(0xa0, p.tlv(0x83, bytes([diagnostic])))))
            user = p.presentation(1, p.tlv(0x64, body))
            parameters = b"\x11\x01\x0b" + p.session_parameter(0xc1, user)
            peer.send_cotp(b"\x19" + p.session_length(len(parameters)) + parameters)
            closed(peer)
        finally: peer.close()
        print("PASS peer provider/OSI abort without response", diagnostic)

    peer = p.NativePeer(args)
    try:
        peer.connect()
        peer.send_cotp(b"\x01\0\x01\0" + p.presentation(5,
            peer.protect(p.mms_read("E2ELD0", "LLN0$ST$Abort$stVal", 1))))
        terminal(peer, application=bytes.fromhex("a403850101")); reads += 1
    finally: peer.close()
    print("PASS MMS callback requests signed ApplAbort with preserved diagnostic")

    def concurrent(_):
        peer = p.NativePeer(args)
        try:
            peer.connect()
            for renew in (False, True, False): peer.read("E2ELD0", "LLN0$ST$Health$stVal", rekey=renew)
            peer.release()
        finally: peer.close()
    with ThreadPoolExecutor(max_workers=4) as pool: list(pool.map(concurrent, range(4)))
    reads += 12
    print("PASS four concurrent independent associations/rekey/cleanup")

    for _ in range(3):
        peer = p.NativePeer(args)
        try:
            peer.connect()
            peer.report_enable("E2ELD0", "LLN0$RP$Events$RptEna", True); reads += 1
            kind, pci, report = peer.receive()
            if (kind, pci) != (1, 5) or peer.unprotect(report)[0] != 0xa3:
                raise AssertionError("missing protected report before abrupt disconnect")
        finally: peer.close()
    peer = p.NativePeer(args)
    try:
        peer.connect(); peer.read("E2ELD0", "LLN0$ST$Health$stVal"); reads += 1; peer.release()
    finally: peer.close()
    print("PASS abrupt disconnect during reporting releases ownership and permits reconnect")

    def advance(seconds):
        process.stdin.write(f"advance {seconds}\n"); process.stdin.flush()
        if process.stdout.readline().strip() != "ADVANCED": raise AssertionError("epoch harness did not advance")
    for mode in ("hmac", "gmac128", "gmac256"):
        args.icv = mode
        peer = p.NativePeer(args)
        try:
            peer.connect(); advance(840)
            peer.read("E2ELD0", "LLN0$ST$Health$stVal"); reads += 1
            if not peer.rekey_requested: raise AssertionError("server did not request renewal at deadline margin")
            peer.read("E2ELD0", "LLN0$ST$Health$stVal"); reads += 1
            if peer.pending_receive_key is not None: raise AssertionError("renewal not acknowledged")
            advance(70)
            peer.read("E2ELD0", "LLN0$ST$Health$stVal"); reads += 1
            peer.release()
        finally: peer.close()
        peer = p.NativePeer(args)
        try:
            peer.connect(); advance(840)
            peer.read("E2ELD0", "LLN0$ST$Health$stVal"); reads += 1
            if not peer.rekey_requested: raise AssertionError("missing renewal request")
            advance(61); terminal(peer)
        finally: peer.close()
        print("PASS injected monotonic epoch renewal and ignored-request expiry", mode)
    args.icv = "hmac"
    peer = p.NativePeer(args)
    try:
        peer.connect()
        for _ in range(64):
            peer.read("E2ELD0", "LLN0$ST$Health$stVal"); reads += 1
        peer.send_cotp(b"\x01\0\x01\0" + p.presentation(5,
            peer.protect(p.mms_read("E2ELD0", "LLN0$ST$Health$stVal", 65))))
        terminal(peer)
    finally: peer.close()
    peer = p.NativePeer(args)
    try:
        peer.connect(); peer.read("E2ELD0", "LLN0$ST$Health$stVal"); reads += 1; peer.release()
    finally: peer.close()
    print("PASS bounded replay-cache exhaustion, no excess dispatch and healthy reconnect")
    return reads
