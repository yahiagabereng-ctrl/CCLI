#!/usr/bin/env python3
"""Exercise the actual libiec61850 native hooks with an independent Python peer."""
import argparse
import datetime as dt
import importlib.util
import hashlib
import os
from pathlib import Path
import selectors
import socket
import subprocess
import shutil
import sys
import time
import tempfile
import uuid
import xml.etree.ElementTree as ET
from types import SimpleNamespace

from cryptography import x509
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import ec, rsa
from cryptography.x509.oid import ExtendedKeyUsageOID, NameOID
import e2e_identity_fixtures as fixtures
from e2e_resilience_cases import run_resilience


def save(path, data):
    path.write_bytes(data)
    path.chmod(0o600)


def generate(root, source):
    root.mkdir(parents=True, exist_ok=True)
    for index, text in enumerate(["e2e_enabled: tru", "e2e_enabled:", "e2e_rekey_seconds: -1",
                                 "e2e_ae_qualifier: 7junk", "e2e_enbaled: true"]):
        save(root / ("invalid-" + str(index) + ".yaml"), ("mms:\n  " + text + "\n").encode())
    save(root / "valid-e2e.yaml",
         b"mms:\n  e2e_enabled: true\n  e2e_allow_compatibility: false\n  e2e_ae_qualifier: 7\n  e2e_rekey_seconds: 900\n")
    now = dt.datetime.now(dt.timezone.utc)
    root_key = rsa.generate_private_key(public_exponent=65537, key_size=2048)
    name = x509.Name([x509.NameAttribute(NameOID.COMMON_NAME, "Native E2E loopback root")])
    ca = (x509.CertificateBuilder().subject_name(name).issuer_name(name).public_key(root_key.public_key())
          .serial_number(1).not_valid_before(now - dt.timedelta(days=1)).not_valid_after(now + dt.timedelta(days=30))
          .add_extension(x509.BasicConstraints(ca=True, path_length=1), critical=True)
          .add_extension(x509.KeyUsage(True, False, False, False, False, True, True, None, None), critical=True)
          .sign(root_key, hashes.SHA256()))
    save(root / "root.pem", ca.public_bytes(serialization.Encoding.PEM))
    crl = (x509.CertificateRevocationListBuilder().issuer_name(name).last_update(now - dt.timedelta(hours=1))
           .next_update(now + dt.timedelta(days=7)).sign(root_key, hashes.SHA256()))
    save(root / "root.crl.der", crl.public_bytes(serialization.Encoding.DER))
    for index, (label, tail, elliptic) in enumerate([
            ("server", [2, 1], False), ("server_ec", [2, 1], True),
            ("peer", [1, 7], False), ("peer_ec", [1, 7], True),
            ("peer_unbound", [1, 7], False)]):
        key = ec.generate_private_key(ec.SECP256R1()) if elliptic else rsa.generate_private_key(
            public_exponent=65537, key_size=2048)
        extra = []
        if label.startswith("peer"):
            role = fixtures.der_tlv(0x30, fixtures.der_tlv(2, b"\xff")) + fixtures.der_tlv(
                12, b"DSO") + fixtures.der_tlv(2, b"\x01") + fixtures.der_tlv(12, b"IEC 62351-8-CEI016:2021")
            roles = fixtures.der_tlv(0x30, fixtures.der_tlv(0x30, role))
            extra = [x509.UnrecognizedExtension(x509.ObjectIdentifier("1.0.62351.8.1"), roles)]
        certificate = fixtures.build_leaf(root_key, name, key, 100 + index, now,
            extra_extensions=extra).public_bytes(serialization.Encoding.DER)
        identity = fixtures.encode_oid([1, 3, 6, 1, 4, 1, 55555, 62351, 4] + tail)
        certificate = fixtures.replace_subject_with_oid_value(certificate, root_key, identity)
        save(root / (label + ".der"), certificate)
        save(root / (label + ".pem"), x509.load_der_x509_certificate(certificate).public_bytes(serialization.Encoding.PEM))
        save(root / (label + ".key"), key.private_bytes(serialization.Encoding.PEM,
             serialization.PrivateFormat.PKCS8, serialization.NoEncryption()))
    for index, label in enumerate(["tls_server", "tls_client"]):
        key = rsa.generate_private_key(public_exponent=65537, key_size=2048)
        certificate = (x509.CertificateBuilder().subject_name(x509.Name([x509.NameAttribute(NameOID.COMMON_NAME, label)]))
            .issuer_name(name).public_key(key.public_key()).serial_number(200 + index)
            .not_valid_before(now - dt.timedelta(days=1)).not_valid_after(now + dt.timedelta(days=7))
            .add_extension(x509.BasicConstraints(ca=False, path_length=None), critical=True)
            .add_extension(x509.KeyUsage(True, False, True, False, False, False, False, None, None), critical=True)
            .add_extension(x509.ExtendedKeyUsage([ExtendedKeyUsageOID.SERVER_AUTH if not index else
                                                  ExtendedKeyUsageOID.CLIENT_AUTH]), critical=False)
            .sign(root_key, hashes.SHA256()))
        save(root / (label + ".pem"), certificate.public_bytes(serialization.Encoding.PEM))
        save(root / (label + ".key"), key.private_bytes(serialization.Encoding.PEM,
             serialization.PrivateFormat.PKCS8, serialization.NoEncryption()))
    owner_key = rsa.generate_private_key(public_exponent=65537, key_size=2048)
    owner_name = x509.Name([x509.NameAttribute(NameOID.COMMON_NAME, "Native test policy owner")])
    owner = (x509.CertificateBuilder().subject_name(owner_name).issuer_name(owner_name)
        .public_key(owner_key.public_key()).serial_number(300).not_valid_before(now - dt.timedelta(days=1))
        .not_valid_after(now + dt.timedelta(days=7)).sign(owner_key, hashes.SHA256()))
    save(root / "owner.pem", owner.public_bytes(serialization.Encoding.PEM))
    save(root / "owner.key", owner_key.private_bytes(serialization.Encoding.PEM,
        serialization.PrivateFormat.PKCS8, serialization.NoEncryption()))
    package = root / "policy"; package.mkdir()
    for item in ["commissioning-policy.xml", "permission-library.xml", "commissioning-scopes.xml"]:
        shutil.copyfile(source / "config/rbac" / item, package / item)
    bindings = ET.Element("IdentityBindings", Version="1.0")
    tls_cert = x509.load_pem_x509_certificate((root / "tls_client.pem").read_bytes())
    transport = hashlib.sha256(tls_cert.public_bytes(serialization.Encoding.DER)).hexdigest()
    for label in ["peer", "peer_ec"]:
        ET.SubElement(bindings, "Identity", Sha256=hashlib.sha256((root / (label + ".der")).read_bytes()).hexdigest(),
            TransportSha256=transport, Actor="DSO", Roles="-1,0")
    ET.ElementTree(bindings).write(package / "identity-bindings.xml", encoding="utf-8", xml_declaration=True)
    tool = source.parents[1] / "scripts/commission_rbac.py"
    subprocess.run([sys.executable, str(tool), "sign", "--package", str(package),
        "--sequence", "1", "--owner-key", str(root / "owner.key")], check=True, capture_output=True)


def fault_packet(probe, peer, packet, fault):
    """Alter authenticated fields and recompute ICV to exercise semantic checks."""
    tbp, auth = probe.children(packet, 0xa0)
    token, application = probe.children(tbp, 0x30)
    fields = probe.children(token, 0x30)
    authenticator = probe.children(auth, 0x30)
    nonce = probe.contents(authenticator[0]) if authenticator[0][0] == 4 else b""
    if fault == "wrong_association":
        fields[0] = probe.tlv(12, b"998:998" if peer.association == b"999:999" else b"999:999")
    elif fault == "expired_time": fields[1] = probe.tlv(24, b"20000101000000Z")
    elif fault == "initial_sequence": fields[2] = probe.integer(1)
    elif fault == "client_reqrekey": fields.append(probe.tlv(0x82, b"\xff"))
    elif fault == "client_changedkey": fields.append(probe.tlv(0x83, b"\xff"))
    elif fault == "unexpected_iv": fields.append(probe.tlv(0x80, b"\0" * 16))
    elif fault == "reused_nonce":
        # Fresh time/sequence and valid ICV isolate nonce reuse from packet replay.
        current = probe.now()
        previous = probe.contents(fields[1])
        fields[1] = probe.tlv(24, current)
        fields[2] = probe.integer(int.from_bytes(probe.contents(fields[2]), "big") + 1
                                 if current == previous else 0)
    elif fault not in ("missing_nonce", "wrong_algorithm"):
        raise AssertionError("unknown authenticated fault")
    tbp = probe.tlv(0x30, probe.tlv(0x30, b"".join(fields)) + application)
    algorithm = probe.algorithm(probe.HMAC) if fault == "wrong_algorithm" else peer.icv
    auth = probe.tlv(0x30,
        (probe.tlv(4, nonce) if nonce and fault != "missing_nonce" else b"") + algorithm +
        probe.tlv(4, peer.mac(peer.send_key, probe.contents(tbp), nonce)))
    return probe.tlv(0xa0, tbp + auth)


def fragmented_string(probe, encoded, *, root_tag=None, bit_string=False, indefinite=True):
    tag, value, full, rest = probe.split(encoded)
    if rest:
        raise AssertionError("fragment source contains trailing data")
    root = tag if root_tag is None else root_tag
    if bit_string:
        if not value or value[0] > 7 or (len(value) == 1 and value[0]):
            raise AssertionError("invalid test BIT STRING source")
        split_at = max(0, (len(value) - 1) // 2)
        parts = (b"\0" + value[1:1 + split_at], bytes([value[0]]) + value[1 + split_at:])
        leaves = b"".join(probe.tlv(3, part) for part in parts)
    else:
        split_at = len(value) // 2
        leaves = probe.tlv(4, value[:split_at]) + probe.tlv(4, value[split_at:])
    constructed_tag = root | 0x20
    return bytes([constructed_tag, 0x80]) + leaves + b"\0\0" if indefinite else probe.tlv(constructed_tag, leaves)


def constructed_handshake_token(probe, token):
    fields = probe.children(token, 0x30)
    fields[0] = probe.tlv(0x30, probe.contents(fields[0]) + probe.tlv(0x80, b"extension"))
    fields[1] = fragmented_string(probe, fields[1])  # PKCert OCTET STRING
    fields.insert(2, fragmented_string(probe, probe.tlv(3, b"\x07\x80"), bit_string=True))
    fields[3] = fragmented_string(probe, fields[3], root_tag=0x0c)  # AssoID UTF8String
    dh_fields = probe.children(fields[4], 0x30)
    dh_fields[1] = fragmented_string(probe, dh_fields[1])  # DH public key
    fields[4] = probe.tlv(0x30, b"".join(dh_fields))
    fields[6] = fragmented_string(probe, fields[6], root_tag=0x18)  # GeneralizedTime
    fields[8] = fragmented_string(probe, fields[8], bit_string=True)  # ConfidentialityParms
    return probe.tlv(0x30, b"".join(fields))


def indefinite_sequence(probe, encoded):
    tag, value, full, rest = probe.split(encoded)
    if tag != 0x30 or rest:
        raise AssertionError("expected one complete SEQUENCE")
    return b"\x30\x80" + value + b"\0\0"


def constructed_handshake_request(probe, encoded):
    fields = probe.children(encoded, 0x30)
    fields[0] = fragmented_string(probe, fields[0], root_tag=0x0c)
    signature = probe.children(fields[2], 0x30)
    signature[0] = probe.tlv(0x30, probe.contents(signature[0]) + probe.tlv(0x80, b"extension"))
    signature[1] = fragmented_string(probe, signature[1])
    fields[2] = probe.tlv(0x30, b"".join(signature))
    return indefinite_sequence(probe, probe.tlv(0x30, b"".join(fields)))


def constructed_transfer_tbp(probe, encoded, *, invalid_segment=False, include_iv=False):
    token, application = probe.children(encoded, 0x30)
    token_fields = probe.children(token, 0x30)
    token_fields.insert(0, fragmented_string(probe, probe.tlv(3, b"\x07\x80"), bit_string=True))
    token_fields[1] = fragmented_string(probe, token_fields[1], root_tag=0x0c)
    token_fields[2] = fragmented_string(probe, token_fields[2], root_tag=0x18)
    if invalid_segment:
        token_fields[1] = b"\x2c\x03\x02\x01\x01"  # Constructed UTF8String with INTEGER leaf.
    if include_iv:
        token_fields.append(fragmented_string(probe, probe.tlv(0x04, bytes(16)), root_tag=0x80))
    elif any(field[0] == 0x81 for field in token_fields):
        index = next(i for i, field in enumerate(token_fields) if field[0] == 0x81)
        token_fields[index] = fragmented_string(probe, token_fields[index], root_tag=0x81)
    token = probe.tlv(0x30, b"".join(token_fields))
    body = token + application
    return b"\x30\x80" + body + b"\0\0"


def constructed_authenticator(probe, encoded):
    fields = probe.children(encoded, 0x30)
    for index, field in enumerate(fields):
        if field[0] == 0x04:
            fields[index] = fragmented_string(probe, field)
        elif field[0] == 0x30:
            fields[index] = probe.tlv(0x30, probe.contents(field) + probe.tlv(0x80, b"extension"))
    return probe.tlv(0x30, b"".join(fields))


def check_peer_ber_helpers(probe):
    checks = 0
    for size in (0, 1, 127, 128, 255, 256, 1024):
        encoded = probe.tlv(4, bytes([0x5a]) * size)
        tag, value, full, rest = probe.split(encoded)
        if tag != 4 or value != bytes([0x5a]) * size or full != encoded or rest:
            raise AssertionError("peer BER length boundary failed: " + str(size))
        checks += 1
    for encoded in (b"\x04\x81\x01A", b"\x04\x82\0\x01A"):
        if probe.contents(encoded) != b"A":
            raise AssertionError("peer rejected a BER long-form length alternative")
        checks += 1
    indefinite = b"\x30\x80\x04\x01a\x30\x80\x04\x01b\0\0\0\0"
    fields = probe.children(indefinite, 0x30)
    if len(fields) != 2 or probe.authentication_contents(indefinite) != indefinite[2:]:
        raise AssertionError("peer indefinite SEQUENCE/EOC handling failed")
    checks += 1
    # Primitive context-specific high tag 31; a constructed high-tag value
    # must itself contain child TLVs, so keep this check focused on tag decode.
    high_tag = b"\x9f\x1f\x01x"
    if probe.split(high_tag)[1] != b"x":
        raise AssertionError("peer high-tag parsing failed")
    checks += 1
    for malformed in (b"\x04\x02A", b"\x24\x80\x04\x01A", b"\x04\x80\0\0", b"\x30\x02\0\0"):
        try:
            probe.split(malformed)
        except ValueError:
            pass
        else:
            raise AssertionError("peer accepted malformed BER: " + malformed.hex())
        checks += 1
    print("PASS independent peer BER helper checks:", checks)


def run(server, root, probe, use_ec, extended=False, campaign=False):
    with socket.socket() as sock:
        sock.bind(("127.0.0.1", 0)); port = sock.getsockname()[1]
    args = SimpleNamespace(host="127.0.0.1", port=port, certificate=str(root / "peer.der"), key=str(root / "peer.key"),
        app_ca=str(root / "root.pem"), crl=str(root / "root.crl.der"), tls_ca=str(root / "root.pem"),
        tls_certificate=str(root / "tls_client.pem"), tls_key=str(root / "tls_client.key"),
        ap_title="1.3.6.1.4.1.55555.62351.4.1", ae_qualifier=7,
        server_ap_title="1.3.6.1.4.1.55555.62351.4.2", server_ae_qualifier=1, icv="hmac")
    stderr_file = tempfile.TemporaryFile(mode="w+t")
    process = subprocess.Popen([server, str(root), str(port)] + (["ec"] if use_ec else []) +
        (["resilience"] if extended else []),
        stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=stderr_file, text=True)
    expected_reads = 0
    try:
        selector = selectors.DefaultSelector(); selector.register(process.stdout, selectors.EVENT_READ)
        if not selector.select(10) or process.stdout.readline().strip() != "READY":
            raise AssertionError("native test server did not start")
        for mode in ["hmac", "gmac128", "gmac256"]:
            args.icv = mode
            args.nonce_size = 16 if mode == "gmac128" else 12
            args.certificate = str(root / ("peer_ec.der" if use_ec else "peer.der"))
            args.key = str(root / ("peer_ec.key" if use_ec else "peer.key"))
            peer = probe.NativePeer(args)
            try:
                peer.connect()
                peer.report_enable("E2ELD0", "LLN0$RP$Events$RptEna", True)
                kind, pci, report = peer.receive()
                if kind != 1 or pci != 5 or peer.unprotect(report)[0] != 0xa3:
                    raise AssertionError("unsolicited report bypassed native protection")
                # The stack checks read access to the dataset when enabling its RCB.
                expected_reads += 1
                for renew in [False, True, False]:
                    pdu = peer.read("E2ELD0", "LLN0$ST$Health$stVal", rekey=renew)
                    if b"\x85\x01\x07" not in pdu:
                        raise AssertionError("protected read did not return the actual model value 7: " + pdu.hex())
                    expected_reads += 1
                if peer.pending_receive_key is not None:
                    raise AssertionError("server did not acknowledge rekey")
                replay_handshake = peer.handshake
                peer.report_enable("E2ELD0", "LLN0$RP$Events$RptEna", False)
                peer.release()
            finally:
                peer.close()
            print("PASS", "ECDSA" if use_ec else "RSA", mode, "handshake/protected report/read/rekey/read/release")
        args.icv = "hmac"
        for negative in ["bad_identity", "forbidden_auth", "replayed_handshake", "invalid_signature", "indirect_reference"]:
            peer = probe.NativePeer(args)
            try:
                options = {negative: True} if negative in ("bad_identity", "forbidden_auth") else {}
                if negative == "indirect_reference": options["indirect_reference"] = 7
                if negative == "replayed_handshake": options["handshake"] = replay_handshake
                if negative == "invalid_signature":
                    encoded, tbs = replay_handshake
                    options["handshake"] = (encoded[:-1] + bytes([encoded[-1] ^ 1]), tbs)
                try:
                    peer.connect(**options)
                except ValueError as error:
                    expected = ("native OSI diagnostic: 2" if negative == "indirect_reference" else
                        "native ACSE provider abort" if negative == "forbidden_auth" else "signed native handshake rejection")
                    if str(error) != expected: raise
                else:
                    raise AssertionError("invalid handshake accepted: " + negative)
            finally:
                peer.close()
            print("PASS OSI diagnostic" if negative == "indirect_reference" else
                  "PASS provider abort" if negative == "forbidden_auth" else "PASS signed reject", negative)
        for negative in ["invalid_icv", "replayed_transfer", "wrong_context",
                "wrong_association", "expired_time", "initial_sequence", "client_reqrekey",
                "client_changedkey", "unexpected_iv", "missing_nonce", "reused_nonce", "wrong_algorithm"]:
            args.icv = "gmac256" if negative in ("missing_nonce", "reused_nonce", "wrong_algorithm") else "hmac"
            peer = probe.NativePeer(args)
            try:
                peer.connect()
                packet = peer.protect(probe.mms_read("E2ELD0", "LLN0$ST$Health$stVal", 1))
                if negative in ("replayed_transfer", "reused_nonce"):
                    peer.send_cotp(b"\x01\0\x01\0" + probe.presentation(5, packet))
                    kind, pci, response = peer.receive()
                    peer.unprotect(response); expected_reads += 1
                elif negative == "invalid_icv":
                    packet = packet[:-1] + bytes([packet[-1] ^ 1])
                if negative not in ("invalid_icv", "replayed_transfer", "wrong_context"):
                    packet = fault_packet(probe, peer, packet, negative)
                peer.send_cotp(b"\x01\0\x01\0" + probe.presentation(3 if negative == "wrong_context" else 5, packet))
                kind, pci, response = peer.receive()
                if kind != 25 or pci != 1 or response[0] != 0x64:
                    raise AssertionError("expected signed abort for " + negative)
                if negative == "wrong_context":
                    if peer.verify_osi_abort(response) != 3:
                        raise AssertionError("wrong PCI did not return invalid-pci OSI diagnostic")
                else:
                    peer.verify_control(response)
            finally:
                peer.close()
            print("PASS OSI diagnostic" if negative == "wrong_context" else "PASS signed abort", negative)
        if extended:
            expected_reads += run_resilience(args, process, probe)
        expected_reads += run_constructed_ber_cases(args, process, probe, use_ec)
        if campaign:
            from e2e_field_campaign import run_campaign
            expected_reads += run_campaign(args, process, probe, use_ec)
    finally:
        failed = sys.exc_info()[0] is not None
        try:
            stdout, _ = process.communicate("quit\n", timeout=10)
        except subprocess.TimeoutExpired:
            process.kill(); stdout, _ = process.communicate()
        stderr_file.seek(0); stderr = stderr_file.read(); stderr_file.close()
        if process.returncode or failed: print(stderr[-12000:], file=sys.stderr)
        if process.returncode != 0:
            raise AssertionError("native server failed: " + stderr)
        if not failed and stdout.strip() != "READS " + str(expected_reads):
            raise AssertionError("MMS dispatch count mismatch: " + stdout + stderr)
        if not failed:
            print("PASS invalid packets rejected before application read dispatch;", expected_reads, "authorized reads")


def run_constructed_ber_cases(args, process, probe, use_ec):
    args.icv = "gmac256"
    args.nonce_size = 12
    peer = probe.NativePeer(args)
    try:
        peer.connect(
            token_mutator=lambda token: constructed_handshake_token(probe, token),
            tbs_mutator=lambda tbs: indefinite_sequence(probe, tbs),
            request_mutator=lambda request: constructed_handshake_request(probe, request))
        response = peer.read("E2ELD0", "LLN0$ST$Health$stVal", rekey=True,
            tbp_mutator=lambda tbp: constructed_transfer_tbp(probe, tbp),
            auth_mutator=lambda auth: constructed_authenticator(probe, auth))
        if b"\x85\x01\x07" not in response or peer.pending_receive_key is not None:
            raise AssertionError("constructed BER handshake, GMAC, or rekey failed")
        peer.release()
    finally:
        peer.close()
    print("PASS constructed BER strings in handshake certificate/ID/DH/time/bits/signature, indefinite signed containers and GMAC rekey")

    # The same signature AlgorithmIdentifier policy applies on controls and
    # handshakes. Extensions remain supported; ECDSA NULL parameters do not.
    for label, suffix, reject in [("extension", probe.tlv(0x80, b"x"), False),
                                  ("NULL", probe.tlv(5, b""), use_ec)]:
        peer = probe.NativePeer(args)
        try:
            peer.connect()
            peer.send_control(algorithm_mutator=lambda alg, suffix=suffix:
                probe.tlv(0x30, probe.contents(alg) + suffix))
            kind, pci, response = peer.receive()
            if pci != 1 or kind != (25 if reject else 10) or response[0] != (0x64 if reject else 0x63):
                raise AssertionError("incorrect signature algorithm control policy: " + label)
            peer.verify_control(response)
        finally:
            peer.close()
        print("PASS signature AlgorithmIdentifier control policy", label, "reject" if reject else "accept")

    for label, options in [("wrong_segment", {"invalid_segment": True}),
                           ("unsupported_constructed_iv", {"include_iv": True})]:
        peer = probe.NativePeer(args)
        try:
            peer.connect()
            packet = peer.protect(probe.mms_read("E2ELD0", "LLN0$ST$Health$stVal", 1),
                tbp_mutator=lambda tbp, options=options: constructed_transfer_tbp(probe, tbp, **options))
            peer.send_cotp(b"\x01\0\x01\0" + probe.presentation(5, packet))
            kind, pci, response = peer.receive()
            if kind != 25 or pci != 1 or response[0] != 0x64:
                raise AssertionError("expected signed abort for constructed BER case: " + label)
            peer.verify_control(response)
        finally:
            peer.close()
        print("PASS authenticated constructed BER rejection", label)
    return 1


def run_product(server, root, probe):
    with socket.socket() as sock:
        sock.bind(("127.0.0.1", 0)); port = sock.getsockname()[1]
    args = SimpleNamespace(host="127.0.0.1", port=port, certificate=str(root / "peer.der"), key=str(root / "peer.key"),
        app_ca=str(root / "root.pem"), crl=str(root / "root.crl.der"), tls_ca=str(root / "root.pem"),
        tls_certificate=str(root / "tls_client.pem"), tls_key=str(root / "tls_client.key"),
        ap_title="1.3.6.1.4.1.55555.62351.4.1", ae_qualifier=7,
        server_ap_title="1.3.6.1.4.1.55555.62351.4.2", server_ae_qualifier=1, icv="hmac")
    process = subprocess.Popen([server, str(root), str(port), "product"],
        stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    try:
        selector = selectors.DefaultSelector(); selector.register(process.stdout, selectors.EVENT_READ)
        if not selector.select(10) or process.stdout.readline().strip() != "READY":
            raise AssertionError("product MMS adapter did not start")
        peer = probe.NativePeer(args)
        try:
            peer.connect()
            for renew in [False, True, False]:
                result = peer.read("CCI016_01LD_Plant", "PdCMMXU1$MX$TotW$mag$f", rekey=renew)
                read_result = probe.children(probe.children(result, 0xa1)[1], 0xa4)[-1]
                if probe.children(read_result)[0][0] == 0x80:
                    raise AssertionError("commissioned native DSO measurement read denied")
            result = peer.read("CCI016_01LD_Plant", "LLN0$ST$Mod$stVal")
            read_result = probe.children(probe.children(result, 0xa1)[1], 0xa4)[-1]
            if probe.children(read_result)[0][0] != 0x80:
                raise AssertionError("native DSO escaped commissioned object scope")
            peer.release()
        finally:
            peer.close()
        args.certificate = str(root / "peer_unbound.der"); args.key = str(root / "peer_unbound.key")
        peer = probe.NativePeer(args)
        try:
            try: peer.connect()
            except ValueError as error:
                if str(error) != "signed native handshake rejection": raise
            else: raise AssertionError("uncommissioned native identity accepted")
        finally:
            peer.close()
    finally:
        try: stdout, stderr = process.communicate("quit\n", timeout=10)
        except subprocess.TimeoutExpired:
            process.kill(); stdout, stderr = process.communicate()
        if process.returncode or sys.exc_info()[0] is not None:
            print(stderr, file=sys.stderr)
        if process.returncode or "PRODUCT_DONE" not in stdout:
            raise AssertionError("product adapter failed: " + stderr)
        for event in ["E2E_AUTHORIZATION_OK", "MMS_ACCESS_DENIED", "E2E_AUTHORIZATION_FAIL"]:
            if event not in stderr: raise AssertionError("missing product audit event: " + event)
    print("PASS actual MmsAdapter native DSO read/rekey/release, object-scope denial and uncommissioned-identity denial")


def run_timed_rekey(server, root, probe):
    """Real 900-second epoch; deliberately separate from the fast CTest suite."""
    with socket.socket() as sock:
        sock.bind(("127.0.0.1", 0)); port = sock.getsockname()[1]
    args = SimpleNamespace(host="127.0.0.1", port=port,
        certificate=str(root / "peer.der"), key=str(root / "peer.key"),
        app_ca=str(root / "root.pem"), crl=str(root / "root.crl.der"),
        tls_ca=str(root / "root.pem"), tls_certificate=str(root / "tls_client.pem"),
        tls_key=str(root / "tls_client.key"), ap_title="1.3.6.1.4.1.55555.62351.4.1",
        ae_qualifier=7, server_ap_title="1.3.6.1.4.1.55555.62351.4.2",
        server_ae_qualifier=1, icv="gmac256")
    process = subprocess.Popen([server, str(root), str(port)], stdin=subprocess.PIPE,
        stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    peer = probe.NativePeer(args)
    expected_reads = 0
    try:
        selector = selectors.DefaultSelector(); selector.register(process.stdout, selectors.EVENT_READ)
        if not selector.select(10) or process.stdout.readline().strip() != "READY":
            raise AssertionError("timed rekey server did not start")
        peer.connect()
        initial_send, initial_receive = peer.send_key, peer.receive_key
        started = time.monotonic()
        requested_at = None
        renewed = False
        while True:
            elapsed = time.monotonic() - started
            response = peer.read("E2ELD0", "LLN0$ST$Health$stVal")
            expected_reads += 1
            if b"\x85\x01\x07" not in response:
                raise AssertionError("timed protected read lost model value")
            if peer.rekey_requested and requested_at is None:
                requested_at = elapsed
                if elapsed < 839 or elapsed >= 900:
                    raise AssertionError("server reqRekey outside configured deadline margin")
                print(f"PASS server reqRekey at {elapsed:.1f}s", flush=True)
            if peer.send_key != initial_send and peer.receive_key != initial_receive:
                if peer.pending_receive_key is not None:
                    raise AssertionError("renewal was not acknowledged")
                if not renewed:
                    print(f"PASS server-requested GMAC-256 renewal acknowledged at {elapsed:.1f}s", flush=True)
                renewed = True
            if elapsed >= 905:
                break
            if expected_reads % 6 == 0:
                print(f"timed rekey: {elapsed:.0f}s; {expected_reads} protected reads", flush=True)
            time.sleep(10)
        if requested_at is None or not renewed:
            raise AssertionError("server did not request and complete timed renewal")
        peer.release()
        print("PASS protected traffic beyond original 900s epoch and signed release", flush=True)
    finally:
        peer.close()
        try: stdout, stderr = process.communicate("quit\n", timeout=10)
        except subprocess.TimeoutExpired:
            process.kill(); stdout, stderr = process.communicate()
        if process.returncode or sys.exc_info()[0] is not None:
            print(stderr, file=sys.stderr)
        if process.returncode or stdout.strip() != "READS " + str(expected_reads):
            raise AssertionError("timed rekey server/dispatch count failed: " + stdout)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--server", required=True); parser.add_argument("--fixtures", required=True)
    parser.add_argument("--probe", required=True)
    parser.add_argument("--timed-rekey", action="store_true",
                        help="Run a real 15-minute server-requested renewal instead of the fast campaign")
    parser.add_argument("--resilience", action="store_true", help="Run extended native error/abort/epoch cases")
    parser.add_argument("--field-campaign", action="store_true", help="Run exact PDU size and allocator failure campaigns")
    args = parser.parse_args()
    root = Path(args.fixtures) / ("run-" + uuid.uuid4().hex)
    generate(root, Path(args.probe).resolve().parents[2])
    spec = importlib.util.spec_from_file_location("native_probe", args.probe)
    probe = importlib.util.module_from_spec(spec); spec.loader.exec_module(probe)
    contract = None
    if args.field_campaign:
        from e2e_tx_contract import Contract
        contract = Contract(probe)
        original_receive = probe.NativePeer.receive
        def checked_receive(self):
            result = original_receive(self)
            contract.observe(*result, self.last_presentation, contexts=(self.acse_context, self.e2e_context))
            return result
        probe.NativePeer.receive = checked_receive
    check_peer_ber_helpers(probe)
    if args.timed_rekey:
        run_timed_rekey(args.server, root, probe)
    else:
        run(args.server, root, probe, False, args.resilience or args.field_campaign, args.field_campaign)
        run(args.server, root, probe, True, args.resilience or args.field_campaign, args.field_campaign)
        run_product(args.server, root, probe)
        if contract:
            contract.save(root, Path(__file__).resolve().parents[4] / "lab/conformance/e2e/ber-field-map.csv")
