#!/usr/bin/env python3
"""Native IEC 62351-4 OSI/MMS lab peer. Performs read-only probes over mutual TLS.

Uses Python cryptography for signature, ECDH, HKDF and ICV verification independently
of the CCLI Mbed TLS provider. This development peer is not qualified conformance TEQ.
The application certificate in this initial probe must be directly issued by --app-ca.
"""
import argparse
import datetime as dt
import hashlib
import hmac
import secrets
import socket
import ssl
import struct
import time
from pathlib import Path

from cryptography import x509
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import dh, ec, padding, rsa
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.primitives.kdf.hkdf import HKDF

CONTEXT = "1.0.62351.4.2.1"
E2E_AS = "1.0.62351.4.1.2.1"
HMAC = "1.2.840.113549.2.9"
RSA = "1.2.840.113549.1.1.11"
ECDSA = "1.2.840.10045.4.3.2"
GMAC128 = "2.16.840.1.101.3.4.1.6"
GMAC256 = "2.16.840.1.101.3.4.1.46"
GROUP14_P = int(
    "FFFFFFFFFFFFFFFFC90FDAA22168C234C4C6628B80DC1CD1"
    "29024E088A67CC74020BBEA63B139B22514A08798E3404DD"
    "EF9519B3CD3A431B302B0A6DF25F14374FE1356D6D51C245"
    "E485B576625E7EC6F44C42E9A637ED6B0BFF5CB6F406B7ED"
    "EE386BFB5A899FA5AE9F24117C4B1FE649286651ECE45B3D"
    "C2007CB8A163BF0598DA48361C55D39A69163FA8FD24CF5F"
    "83655D23DCA3AD961C62F356208552BB9ED529077096966D"
    "670C354E4ABC9804F1746C08CA18217C32905E462E36CE3B"
    "E39E772C180E86039B2783A2EC07A28FB5C55DF06F4C52C9"
    "DE2BCBF6955817183995497CEA956AE515D2261898FA0510"
    "15728E5A8AACAA68FFFFFFFFFFFFFFFF", 16)


def tlv(tag, value):
    size = len(value)
    length = bytes([size]) if size < 128 else (lambda n: bytes([128 | len(n)]) + n)(
        size.to_bytes((size.bit_length() + 7) // 8, "big"))
    return bytes([tag]) + length + value


def integer(value, tag=2):
    raw = value.to_bytes(max(1, (value.bit_length() + 7) // 8), "big")
    return tlv(tag, (b"\0" if raw[0] & 128 else b"") + raw)


def oid_value(text):
    arcs = [int(a) for a in text.split(".")]
    result = b""
    for arc in [arcs[0] * 40 + arcs[1]] + arcs[2:]:
        encoded = bytes([arc & 127])
        while (arc := arc >> 7):
            encoded = bytes([128 | (arc & 127)]) + encoded
        result += encoded
    return result


def algorithm(oid):
    return tlv(0x30, tlv(6, oid_value(oid)))


def split(encoded):
    """One bounded BER TLV; preserve headers and any terminating EOC."""
    if len(encoded) > 1048576: raise ValueError("BER input limit")
    nodes = 0
    def end_at(start, depth):
        nonlocal nodes
        nodes += 1
        if depth > 32 or nodes > 4096 or start + 2 > len(encoded): raise ValueError("BER resource/truncation limit")
        cursor = start + 1
        if encoded[start] & 31 == 31:
            number = 0
            while True:
                if cursor >= len(encoded) or cursor - start > 5: raise ValueError("invalid high tag")
                octet = encoded[cursor]; cursor += 1
                if cursor == start + 2 and octet & 127 == 0: raise ValueError("nonminimal high tag")
                number = number * 128 + (octet & 127)
                if not octet & 128: break
            if number < 31 or number > 0xffffffff: raise ValueError("invalid high tag")
        if cursor >= len(encoded): raise ValueError("truncated BER length")
        length = encoded[cursor]; cursor += 1
        if length == 128:
            if not encoded[start] & 32: raise ValueError("indefinite primitive")
            content = cursor
            while encoded[cursor:cursor + 2] != b"\0\0":
                _, _, cursor = end_at(cursor, depth + 1)
            return content, cursor, cursor + 2
        if length & 128:
            count = length & 127
            if count > 8 or cursor + count > len(encoded): raise ValueError("invalid BER length")
            length = int.from_bytes(encoded[cursor:cursor + count], "big"); cursor += count
        value_end = cursor + length
        if value_end > len(encoded) or encoded[start] == 0: raise ValueError("invalid BER value")
        # A constructed BER value contains complete child TLVs. Validate them
        # here as well as in children(), so split() never accepts malformed
        # nested contents as an otherwise well-formed outer value.
        if encoded[start] & 32:
            child = cursor
            while child < value_end:
                _, _, child_end = end_at(child, depth + 1)
                if child_end > value_end or child_end <= child:
                    raise ValueError("constructed BER child exceeds parent")
                child = child_end
            if child != value_end:
                raise ValueError("invalid constructed BER contents")
        return cursor, value_end, value_end
    begin, end, encoded_end = end_at(0, 1)
    return encoded[0], encoded[begin:end], encoded[:encoded_end], encoded[encoded_end:]


def children(encoded, expected=None):
    tag, value, _, rest = split(encoded)
    if rest or (expected is not None and tag != expected):
        raise ValueError("unexpected BER tag/trailing data")
    result = []
    while value:
        _, _, item, value = split(value)
        result.append(item)
        if len(result) > 128:
            raise ValueError("too many BER fields")
    return result


def contents(encoded):
    return split(encoded)[1]


def authentication_contents(encoded):
    """TBS/TBP SEQUENCE contents, retaining EOC for the indefinite form."""
    tag, value, full, rest = split(encoded)
    if tag != 0x30 or rest: raise ValueError("invalid authentication container")
    return value + (b"\0\0" if encoded[1] == 128 else b"")


def now():
    return dt.datetime.now(dt.timezone.utc).strftime("%Y%m%d%H%M%S.%fZ").encode()


def timestamp_ns(raw):
    text = raw.decode()
    if not text.endswith("Z") or len(text) < 15:
        raise ValueError("invalid UTC GeneralizedTime")
    base = dt.datetime.strptime(text[:14], "%Y%m%d%H%M%S").replace(tzinfo=dt.timezone.utc)
    tail = text[14:-1]
    if tail:
        if tail[0] not in ".," or not tail[1:].isdigit() or len(tail) > 10:
            raise ValueError("invalid GeneralizedTime fraction")
        fraction = int(tail[1:].ljust(9, "0"))
    else:
        fraction = 0
    return int(base.timestamp()) * 1_000_000_000 + fraction


def read_certificate(path):
    raw = Path(path).read_bytes()
    cert = x509.load_pem_x509_certificate(raw) if raw.startswith(b"-----") else x509.load_der_x509_certificate(raw)
    return cert.public_bytes(serialization.Encoding.DER), cert


def sign(key, data):
    if isinstance(key, rsa.RSAPrivateKey):
        return algorithm(RSA), key.sign(data, padding.PKCS1v15(), hashes.SHA256())
    return algorithm(ECDSA), key.sign(data, ec.ECDSA(hashes.SHA256()))


def verify(cert, algo, signature, data):
    oid = contents(children(algo, 0x30)[0])
    if oid == oid_value(RSA) and isinstance(cert.public_key(), rsa.RSAPublicKey):
        cert.public_key().verify(signature, data, padding.PKCS1v15(), hashes.SHA256())
    elif oid == oid_value(ECDSA) and isinstance(cert.public_key(), ec.EllipticCurvePublicKey):
        cert.public_key().verify(signature, data, ec.ECDSA(hashes.SHA256()))
    else:
        raise ValueError("unsupported/mismatched signature algorithm")


def verify_application_certificate(der, ca, crl, identity):
    cert = x509.load_der_x509_certificate(der)
    if cert.version != x509.Version.v3 or cert.serial_number < 0 or cert.signature_hash_algorithm.digest_size < 32:
        raise ValueError("invalid Annex G version/serial/signature hash")
    public_key = cert.public_key()
    if not ((isinstance(public_key, rsa.RSAPublicKey) and public_key.key_size >= 2048) or
            (isinstance(public_key, ec.EllipticCurvePublicKey) and public_key.key_size >= 256)):
        raise ValueError("application public key below profile strength")
    # Verify the exact signed issuer name and Annex G OID-valued subject using
    # raw DER; some X.509 APIs only expose text-valued subject attributes.
    tbs = children(children(der, 0x30)[0], 0x30)
    if any(field[0] in (0x81, 0x82) for field in tbs[7:]):
        raise ValueError("forbidden issuer/subject unique identifier")
    ca_tbs = children(children(ca.public_bytes(serialization.Encoding.DER), 0x30)[0], 0x30)
    if tbs[3] != ca_tbs[5]:
        raise ValueError("application issuer is not the configured direct trust anchor")
    subject = children(tbs[5], 0x30)
    if len(subject) != 1:
        raise ValueError("invalid native subject")
    attributes = children(subject[0], 0x31)
    if len(attributes) != 1 or children(attributes[0], 0x30) != [
            tlv(6, oid_value("2.5.4.106")), tlv(6, oid_value(identity))]:
        raise ValueError("server application identity does not match responding AP/AE")
    public = ca.public_key()
    if isinstance(public, rsa.RSAPublicKey):
        public.verify(cert.signature, cert.tbs_certificate_bytes, padding.PKCS1v15(), cert.signature_hash_algorithm)
        public.verify(crl.signature, crl.tbs_certlist_bytes, padding.PKCS1v15(), crl.signature_hash_algorithm)
    else:
        public.verify(cert.signature, cert.tbs_certificate_bytes, ec.ECDSA(cert.signature_hash_algorithm))
        public.verify(crl.signature, crl.tbs_certlist_bytes, ec.ECDSA(crl.signature_hash_algorithm))
    current = dt.datetime.now(dt.timezone.utc)
    if not cert.not_valid_before_utc <= current <= cert.not_valid_after_utc:
        raise ValueError("expired/not-yet-valid application certificate")
    if not crl.last_update_utc <= current <= crl.next_update_utc or crl.get_revoked_certificate_by_serial_number(cert.serial_number):
        raise ValueError("application revocation check failed")
    if crl.issuer != ca.subject:
        raise ValueError("CRL issuer mismatch")
    points = cert.extensions.get_extension_for_class(x509.CRLDistributionPoints).value
    try:
        scope = crl.extensions.get_extension_for_class(x509.IssuingDistributionPoint).value
    except x509.ExtensionNotFound:
        scope = None
    if scope and (scope.relative_name or scope.indirect_crl or scope.only_contains_ca_certs or
                  scope.only_contains_attribute_certs or scope.only_some_reasons):
        raise ValueError("unsupported/inapplicable CRL scope")
    if not any(not point.relative_name and not point.crl_issuer and not point.reasons and
               (not scope or not scope.full_name or set(point.full_name or []).intersection(scope.full_name))
               for point in points):
        raise ValueError("CRL does not cover the certificate distribution point")
    supported = {x509.ObjectIdentifier(v) for v in ("2.5.29.28", "2.5.29.20", "2.5.29.35")}
    if any(extension.critical and extension.oid not in supported for extension in crl.extensions):
        raise ValueError("unsupported critical CRL extension")
    try:
        crl.extensions.get_extension_for_class(x509.DeltaCRLIndicator)
    except x509.ExtensionNotFound:
        pass
    else:
        raise ValueError("delta CRLs are not supported by this probe")
    usage = cert.extensions.get_extension_for_class(x509.KeyUsage).value
    if not usage.digital_signature or not usage.key_agreement:
        raise ValueError("invalid native application key usage")
    return cert


def session_length(size):
    return bytes([size]) if size < 255 else b"\xff" + struct.pack("!H", size)


def session_parameter(tag, data):
    return bytes([tag]) + session_length(len(data)) + data


def presentation(context, data):
    return tlv(0x61, tlv(0x30, integer(context) + tlv(0xa0, data)))


def presentation_payload(encoded):
    if encoded[0] == 0x31:
        normal = next(v for v in children(encoded, 0x31) if v[0] == 0xa2)
        encoded = next(v for v in children(normal, 0xa2) if v[0] == 0x61)
    entry, = children(encoded, 0x61)
    pci, payload = children(entry, 0x30)
    return int.from_bytes(contents(pci), "big"), contents(payload)


def initiate():
    detail = integer(1, 0x80) + tlv(0x81, bytes.fromhex("05f100")) + tlv(0x82, b"\x03" + b"\xff" * 10 + b"\xf8")
    return tlv(0xa8, integer(60000, 0x80) + integer(5, 0x81) + integer(5, 0x82) +
               integer(10, 0x83) + tlv(0xa4, detail))


def mms_read(domain, item, invoke=1):
    name = tlv(0xa0, tlv(0xa1, tlv(0x1a, domain.encode()) + tlv(0x1a, item.encode())))
    variables = tlv(0xa1, tlv(0xa0, tlv(0x30, name)))
    return tlv(0xa0, integer(invoke) + tlv(0xa4, variables))


def mms_write_bool(domain, item, value, invoke):
    name = tlv(0xa0, tlv(0xa1, tlv(0x1a, domain.encode()) + tlv(0x1a, item.encode())))
    variables = tlv(0xa0, tlv(0x30, name))
    return tlv(0xa0, integer(invoke) + tlv(0xa5, variables + tlv(0xa0, tlv(0x83, b"\xff" if value else b"\0"))))


class NativePeer:
    def __init__(self, args):
        self.args = args
        self.acse_context, self.mms_context, self.e2e_context = getattr(args, "contexts", (1, 3, 5))
        self.certificate, _ = read_certificate(args.certificate)
        self.key = serialization.load_pem_private_key(Path(args.key).read_bytes(), password=None)
        self.group = getattr(args, "group", None) or (14 if isinstance(self.key, rsa.RSAPrivateKey) else 23)
        _, self.ca = read_certificate(args.app_ca)
        raw = Path(args.crl).read_bytes()
        self.crl = x509.load_pem_x509_crl(raw) if raw.startswith(b"-----") else x509.load_der_x509_crl(raw)
        self.sock = None
        self.initial_salt = self.send_key = self.receive_key = b""
        self.pending_receive_key = None
        self.rekey_requested = False
        self.nonces = set()
        self.last_pair = None
        self.icv_oid = {"hmac": HMAC, "gmac128": GMAC128, "gmac256": GMAC256}[getattr(args, "icv", "hmac")]
        self.icv = algorithm(self.icv_oid)
        self.tag_size = 32 if self.icv_oid == HMAC else 12
        self.key_size = 16 if self.icv_oid == GMAC128 else 32
        self.invoke = 0
        self.reports = []

    def generate_dh(self):
        if self.group == 14:
            return dh.DHParameterNumbers(GROUP14_P, 2).parameters().generate_private_key()
        return ec.generate_private_key(ec.BrainpoolP256R1() if self.group == 28 else ec.SECP256R1())

    def public_dh(self, key):
        if self.group == 14:
            return key.public_key().public_numbers().y.to_bytes(256, "big")
        return key.public_key().public_bytes(serialization.Encoding.X962, serialization.PublicFormat.UncompressedPoint)

    def parse_public_dh(self, raw):
        if self.group == 14:
            if len(raw) != 256:
                raise ValueError("invalid MODP public width")
            y = int.from_bytes(raw, "big")
            if not 1 < y < GROUP14_P - 1 or pow(y, (GROUP14_P - 1) // 2, GROUP14_P) != 1:
                raise ValueError("invalid MODP public value")
            return dh.DHPublicNumbers(y, dh.DHParameterNumbers(GROUP14_P, 2)).public_key()
        return ec.EllipticCurvePublicKey.from_encoded_point(
            ec.BrainpoolP256R1() if self.group == 28 else ec.SECP256R1(), raw)

    def exchange(self, private, public):
        # Explicit development convention, pending external IEC peer/lab
        # confirmation: MODP Z is minimal-width; EC Z retains the curve width.
        return private.exchange(public).lstrip(b"\0") if self.group == 14 else private.exchange(ec.ECDH(), public)

    def close(self):
        if self.sock:
            self.sock.close()
            self.sock = None

    def exact(self, size):
        data = b""
        while len(data) < size:
            value = self.sock.recv(size - len(data))
            if not value:
                raise EOFError("peer closed connection")
            data += value
        return data

    def tpkt(self, data):
        self.sock.sendall(b"\x03\x00" + struct.pack("!H", len(data) + 4) + data)

    def receive_cotp(self):
        data = b""
        while True:
            header = self.exact(4)
            size = struct.unpack("!H", header[2:])[0]
            if header[:2] != b"\x03\0" or not 7 <= size <= 65535:
                raise ValueError("invalid TPKT")
            body = self.exact(size - 4)
            if body[1] == 0xd0:
                return body
            if body[:2] != b"\x02\xf0":
                raise ValueError("invalid COTP data")
            data += body[3:]
            if len(data) > 100000:
                raise ValueError("COTP payload too large")
            if body[2] & 128:
                return data

    def send_cotp(self, data):
        for offset in range(0, len(data), 1000):
            chunk = data[offset:offset + 1000]
            self.tpkt(b"\x02\xf0" + (b"\x80" if offset + len(chunk) == len(data) else b"\0") + chunk)

    def receive(self):
        spdu = self.receive_cotp()
        if spdu[:4] == b"\x01\0\x01\0":
            self.last_presentation = spdu[4:]
            return 1, *presentation_payload(spdu[4:])
        kind = spdu[0]
        offset = 2 if spdu[1] != 255 else 4
        while offset < len(spdu):
            tag = spdu[offset]
            size = spdu[offset + 1]
            offset += 2
            if size == 255:
                size = int.from_bytes(spdu[offset:offset+2], "big"); offset += 2
            value = spdu[offset:offset+size]; offset += size
            if tag in (0xc1, 0xc2):
                self.last_presentation = value
                return kind, *presentation_payload(value)
        raise ValueError("missing session user data")

    def connect(self, handshake=None, *, bad_identity=False, forbidden_auth=False, indirect_reference=None,
                token_mutator=None, aarq_mutator=None, application=None, protected_protocol=b"IEC 61850-8-1",
                corrupt_signature=False, tbs_mutator=None, request_mutator=None,
                connect_user_data_size=None, context_order=None):
        if indirect_reference is None: indirect_reference = self.e2e_context
        context = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
        context.minimum_version = context.maximum_version = ssl.TLSVersion.TLSv1_2
        context.check_hostname = False
        context.load_verify_locations(self.args.tls_ca)
        context.load_cert_chain(self.args.tls_certificate, self.args.tls_key)
        self.sock = context.wrap_socket(socket.create_connection((self.args.host, self.args.port), timeout=5))
        self.tpkt(bytes.fromhex("11e00000000100c0010ac1020001c2020001"))
        if self.receive_cotp()[1] != 0xd0:
            raise ValueError("COTP connect rejected")
        self.dh = self.generate_dh()
        self.client_id = str(secrets.randbelow(1000)).encode()
        pub = self.public_dh(self.dh)
        sigalg, _ = sign(self.key, b"")
        token = tlv(0x30, sigalg + tlv(4, self.certificate) + tlv(12, self.client_id) +
                    tlv(0x30, integer(self.group) + tlv(4, pub)) + algorithm(HMAC) + tlv(24, now()) +
                    tlv(0xa0, tlv(0xa1, self.icv)) + tlv(3, b"\x07\x80"))
        if token_mutator: token = token_mutator(token)
        self.request_tbs = tlv(0x30, token + (initiate() if application is None else application))
        if tbs_mutator: self.request_tbs = tbs_mutator(self.request_tbs)
        _, signature = sign(self.key, authentication_contents(self.request_tbs))
        if corrupt_signature: signature = signature[:-1] + bytes([signature[-1] ^ 1])
        request = tlv(0x30, tlv(12, protected_protocol) + self.request_tbs +
                      tlv(0x30, sigalg + tlv(4, signature)))
        if request_mutator: request = request_mutator(request)
        if handshake is not None:
            request, self.request_tbs = handshake
        self.handshake = (request, self.request_tbs)
        app_title = self.args.ap_title
        if bad_identity:
            prefix, tail = app_title.rsplit(".", 1)
            app_title = prefix + "." + str(int(tail) + 99)
        fields = tlv(0xa1, tlv(6, oid_value(CONTEXT))) + tlv(0xa6, tlv(6, oid_value(app_title))) + tlv(
            0xa7, integer(self.args.ae_qualifier))
        if forbidden_auth:
            fields += tlv(0x8b, oid_value(HMAC))
        fields += tlv(0xbe, tlv(0x28, integer(indirect_reference) + tlv(0xa0, request)))
        if aarq_mutator: fields = aarq_mutator(fields)
        aarq = tlv(0x60, fields)
        contexts = b"".join(tlv(0x30, integer(i) + tlv(6, oid_value(syntax)) + tlv(
            0x30, tlv(6, oid_value("2.1.1")))) for i, syntax in [
                (self.acse_context, "2.2.1.0.1"), (self.mms_context, "1.0.9506.2.1"), (self.e2e_context, E2E_AS)])
        if context_order is not None:
            offered = children(tlv(0x30, contexts))
            contexts = b"".join(offered[index] for index in context_order)
        def presentation_connect(aarq):
            return tlv(0x31, tlv(0xa0, tlv(0x80, b"\x01")) + tlv(0xa2,
                tlv(0x81, b"\0\0\0\x01") + tlv(0x82, b"\0\0\0\x01") +
                tlv(0xa4, contexts) + presentation(self.acse_context, aarq)))
        cp = presentation_connect(aarq)
        if connect_user_data_size is not None:
            if any(value is not None for value in (handshake, request_mutator, aarq_mutator)):
                raise ValueError("size fixture cannot combine outer mutations")
            original_tbs = self.request_tbs
            prefix = b"".join(value for value in children(tlv(0x30, fields)) if value[0] != 0xbe)
            padding = max(0, connect_user_data_size - len(cp) - 4)
            for _ in range(64):
                self.request_tbs = tlv(0x30, contents(original_tbs) + tlv(0x87, b"x" * padding))
                _, signature = sign(self.key, authentication_contents(self.request_tbs))
                request = tlv(0x30, tlv(12, protected_protocol) + self.request_tbs +
                    tlv(0x30, sigalg + tlv(4, signature)))
                aarq = tlv(0x60, prefix + tlv(0xbe, tlv(0x28, integer(indirect_reference) + tlv(0xa0, request))))
                cp = presentation_connect(aarq)
                if len(cp) == connect_user_data_size: break
                padding += connect_user_data_size - len(cp)
                if padding < 0: raise ValueError("CONNECT target below credential minimum")
            else: raise ValueError("CONNECT size fixture failed to converge")
            self.handshake = (request, self.request_tbs)
        self.last_connect_user_data_size = len(cp)
        parameters = bytes.fromhex("0506130100160102140200023302000134020001")
        parameters += session_parameter(0xc2, cp)
        self.send_cotp(bytes([13]) + session_length(len(parameters)) + parameters)
        kind, pci, response = self.receive()
        if kind == 25 and pci == self.acse_context and response[0] == 0x64:
            if response == tlv(0x64, tlv(0x80, b"\x01")):
                raise ValueError("native ACSE provider abort")
            code = self.verify_osi_abort(response)
            raise ValueError("native OSI diagnostic: " + str(code))
        if kind != 14 or pci != self.acse_context or response[0] != 0x61:
            raise ValueError("invalid native AARE")
        components = children(response, 0x61)
        if any(f[0] in (0x88, 0x89, 0xaa) for f in components):
            raise ValueError("forbidden native AARE authentication fields")
        result = int.from_bytes(contents(children(next(f for f in components if f[0] == 0xa2))[0]), "big")
        if not any(f[0] == 0xbe for f in components):
            source, = children(next(f for f in components if f[0] == 0xa3))
            code = int.from_bytes(contents(children(source)[0]), "big")
            if not result: raise ValueError("accepted AARE has no user-information")
            raise ValueError("native ACSE rejection: " + ("provider:" if source[0] == 0xa2 else "user:") + str(code))
        info = next(f for f in components if f[0] == 0xbe)
        external, = children(info, 0xbe)
        indirect, encoded = children(external, 0x28)
        if int.from_bytes(contents(indirect), "big") != self.e2e_context:
            raise ValueError("incorrect E2E indirect reference")
        message = contents(encoded)
        tbs, sign_value = children(message)
        token = children(tbs, 0x30)[0]
        values = children(token, 0x30)
        certificate = contents(values[1] if message[0] == 0xa1 else
                               next(v for v in values if v[0] == 4))
        peer_cert = verify_application_certificate(certificate, self.ca, self.crl,
            self.args.server_ap_title + "." + str(self.args.server_ae_qualifier))
        algo, signature = children(sign_value, 0x30)
        verify(peer_cert, algo, contents(signature), authentication_contents(tbs))
        if result or message[0] != 0xa1:
            self.rejection_kind = message[0]
            self.rejection_diagnostic = next((int.from_bytes(contents(v), "big")
                for v in children(tbs) if v[0] == 10), None)
            self.rejection_payload = next((v for v in children(tbs)[1:] if v[0] != 10), None)
            self.rejection_id = next((contents(v) for v in values if v[0] == 12), None)
            if message[0] == 0xa2 and (self.rejection_payload is None or self.rejection_payload[0] != 0xaa):
                raise ValueError("ApplicationReject lacks MMS InitiateError")
            raise ValueError("signed native handshake rejection")
        response_ap = contents(children(next(f for f in components if f[0] == 0xa4))[0])
        response_ae = int.from_bytes(contents(children(next(f for f in components if f[0] == 0xa5))[0]), "big")
        if response_ap != oid_value(self.args.server_ap_title) or response_ae != self.args.server_ae_qualifier:
            raise ValueError("responding AP/AE does not match peer identity")
        self.server_cert = peer_cert
        self.association = contents(next(v for v in values if v[0] == 12))
        if not self.association.startswith(self.client_id + b":"):
            raise ValueError("association id mismatch")
        index = next(i for i, v in enumerate(values) if v[0] == 12)
        if values[0] != algo or values[index + 2] != algorithm(HMAC):
            raise ValueError("signature/HKDF algorithm mismatch")
        if abs(time.time_ns() - timestamp_ns(contents(values[index + 3]))) > 600_000_000_000:
            raise ValueError("server handshake timestamp outside window")
        if children(values[index + 4], 0xa0) != [tlv(0xa1, self.icv)] or values[index + 5] != tlv(3, b"\x07\x80"):
            raise ValueError("incorrect negotiated confidentiality/ICV profile")
        if len(children(tbs, 0x30)) != 2 or children(tbs, 0x30)[1][0] != 0xa9:
            raise ValueError("missing MMS InitiateResponse")
        group, pub = children(values[index + 1], 0x30)
        if int.from_bytes(contents(group), "big") != self.group:
            raise ValueError("unexpected DH group")
        self.server_public = self.parse_public_dh(contents(pub))
        self.initial_salt = hashlib.sha256(authentication_contents(tbs) + hashlib.sha256(authentication_contents(self.request_tbs) + b"\0").digest()).digest()
        secret = self.exchange(self.dh, self.server_public)
        keys = self.derive(secret, self.initial_salt)
        self.send_key, self.receive_key = keys[:self.key_size], keys[self.key_size:]
        return response

    def derive(self, secret, salt):
        return HKDF(algorithm=hashes.SHA256(), length=2*self.key_size, salt=salt, info=b"").derive(secret)

    def mac(self, key, data, nonce=b""):
        if self.icv_oid == HMAC:
            return hmac.digest(key, data, "sha256")
        encryptor = Cipher(algorithms.AES(key), modes.GCM(nonce)).encryptor()
        encryptor.authenticate_additional_data(data); encryptor.finalize()
        return encryptor.tag[:self.tag_size]

    def protect(self, pdu, rekey=False, tbp_mutator=None, auth_mutator=None):
        token = tlv(12, self.association) + tlv(24, now()) + integer(0)
        new_key = None
        rekey = rekey or self.rekey_requested
        if rekey:
            if self.pending_receive_key is not None:
                raise ValueError("rekey already awaiting changedKey")
            new_key = self.generate_dh()
            public = self.public_dh(new_key)
            token += tlv(0x81, public)
        tbp = tlv(0x30, tlv(0x30, token) + pdu)
        if tbp_mutator: tbp = tbp_mutator(tbp)
        nonce = secrets.token_bytes(getattr(self.args, "nonce_size", 12)) if self.icv_oid != HMAC else b""
        auth = tlv(0x30, (tlv(4, nonce) if nonce else b"") + self.icv +
                   tlv(4, self.mac(self.send_key, authentication_contents(tbp), nonce)))
        if auth_mutator: auth = auth_mutator(auth)
        choice = tlv(0xa0, tbp + auth)
        if new_key:
            salt = hashlib.sha256(authentication_contents(tbp) + self.initial_salt).digest()
            keys = self.derive(self.exchange(new_key, self.server_public), salt)
            self.send_key, self.pending_receive_key = keys[:self.key_size], keys[self.key_size:]
            self.rekey_requested = False
        return choice

    def unprotect(self, data):
        tbp, auth = children(data, 0xa0)
        fields = children(auth, 0x30)
        nonce = contents(fields.pop(0)) if fields[0][0] == 4 else b""
        algo, icv = fields
        if algo != self.icv or not hmac.compare_digest(contents(icv), self.mac(self.receive_key, authentication_contents(tbp), nonce)):
            raise ValueError("server ICV check failed")
        token, pdu = children(tbp, 0x30)
        fields = children(token, 0x30)
        if contents(fields[0]) != self.association:
            raise ValueError("server transfer association mismatch")
        timestamp = contents(fields[1])
        instant = timestamp_ns(timestamp)
        sequence = int.from_bytes(contents(fields[2]), "big")
        if abs(time.time_ns() - instant) > 600_000_000_000:
            raise ValueError("server transfer timestamp outside window")
        if self.last_pair:
            previous_time, previous_sequence = self.last_pair
            if instant < previous_time or sequence != (previous_sequence + 1 if instant == previous_time else 0):
                raise ValueError("server transfer replay/sequence check failed")
        elif sequence != 0:
            raise ValueError("server initial sequence must be zero")
        self.last_pair = instant, sequence
        if nonce:
            if nonce in self.nonces:
                raise ValueError("server GMAC nonce reused")
            self.nonces.add(nonce)
        changed = any(v[0] == 0x83 and contents(v) != b"\0" for v in fields[3:])
        if any(v[0] == 0x82 and contents(v) != b"\0" for v in fields[3:]):
            self.rekey_requested = True
        if changed:
            if self.pending_receive_key is None:
                raise ValueError("unsolicited changedKey")
            self.receive_key, self.pending_receive_key = self.pending_receive_key, None
            self.nonces.clear()
        return pdu

    def read(self, domain, item, *, rekey=False, tbp_mutator=None, auth_mutator=None):
        self.invoke += 1
        choice = self.protect(mms_read(domain, item, self.invoke), rekey=rekey,
                              tbp_mutator=tbp_mutator, auth_mutator=auth_mutator)
        self.send_cotp(b"\x01\0\x01\0" + presentation(self.e2e_context, choice))
        pdu = self.confirmed_response()
        invoke, result = children(pdu, 0xa1)
        if int.from_bytes(contents(invoke), "big") != self.invoke or result[0] != 0xa4:
            raise ValueError("incorrect MMS read response")
        return pdu

    def confirmed_response(self):
        for _ in range(100):
            kind, pci, response = self.receive()
            if kind != 1 or pci != self.e2e_context:
                raise ValueError("expected protected MMS response")
            pdu = self.unprotect(response)
            if pdu[0] == 0xa3:
                self.reports.append(pdu)
                continue
            return pdu
        raise ValueError("too many reports without confirmed response")

    def report_enable(self, domain, item, value):
        """Lab test helper; the command-line read-only probe never calls this."""
        self.invoke += 1
        choice = self.protect(mms_write_bool(domain, item, value, self.invoke))
        self.send_cotp(b"\x01\0\x01\0" + presentation(self.e2e_context, choice))
        invoke, result = children(self.confirmed_response(), 0xa1)
        if int.from_bytes(contents(invoke), "big") != self.invoke or children(result, 0xa5) != [b"\x81\0"]:
            raise ValueError("report enable/disable denied")

    def release(self):
        sigalg, _ = sign(self.key, b"")
        token = tlv(0x30, sigalg + tlv(12, self.association) + tlv(24, now()) + tlv(4, self.certificate))
        tbs = tlv(0x30, token)
        _, signature = sign(self.key, authentication_contents(tbs))
        signed = tlv(0x30, tbs + tlv(0x30, sigalg + tlv(4, signature)))
        rlrq = tlv(0x62, tlv(0xbe, tlv(0x28, integer(self.e2e_context) + tlv(0xa0, signed))))
        user = presentation(self.acse_context, rlrq)
        parameters = session_parameter(0xc1 if len(user) < 255 else 0xc2, user)
        self.send_cotp(bytes([9]) + session_length(len(parameters)) + parameters)
        for _ in range(100):
            kind, pci, response = self.receive()
            if kind == 1 and pci == self.e2e_context:
                pdu = self.unprotect(response)
                if pdu[0] != 0xa3:
                    raise ValueError("unexpected MMS response during release")
                self.reports.append(pdu)
                continue
            break
        if kind != 10 or pci != self.acse_context or response[0] != 0x63:
            raise ValueError("invalid signed release response")
        self.verify_control(response)
        self.close()

    def send_control(self, *, abort_choice=None, token_mutator=None, corrupt_signature=False,
                     indirect_reference=None, extra_acse=b"", application=b"", algorithm_mutator=None):
        """Development harness control injection; CLI uses normal signed release."""
        if indirect_reference is None: indirect_reference = self.e2e_context
        sigalg, _ = sign(self.key, b"")
        if algorithm_mutator: sigalg = algorithm_mutator(sigalg)
        token = tlv(0x30, sigalg + tlv(12, self.association) + tlv(24, now()) + tlv(4, self.certificate))
        if token_mutator: token = token_mutator(token)
        tbs = tlv(0x30, token + application)
        _, signature = sign(self.key, authentication_contents(tbs))
        if corrupt_signature: signature = signature[:-1] + bytes([signature[-1] ^ 1])
        body = tlv(0x30 if abort_choice is None else abort_choice,
                   tbs + tlv(0x30, sigalg + tlv(4, signature)))
        info = tlv(0xbe, tlv(0x28, integer(indirect_reference) + tlv(0xa0, body)))
        acse = tlv(0x62 if abort_choice is None else 0x64,
                   (b"" if abort_choice is None else tlv(0x80, b"\0")) + info + extra_acse)
        user = presentation(self.acse_context, acse)
        parameters = (b"" if abort_choice is None else b"\x11\x01\x0b") + session_parameter(
            0xc1 if len(user) < 255 else 0xc2, user)
        self.send_cotp(bytes([9 if abort_choice is None else 25]) + session_length(len(parameters)) + parameters)

    def verify_osi_abort(self, response):
        source, info = children(response, 0x64)
        if source != tlv(0x80, b"\0") or info[0] != 0xbe:
            raise ValueError("incorrect OSI abort source/fields")
        external, = children(info, 0xbe)
        indirect, encoded = children(external, 0x28)
        if indirect != integer(self.e2e_context):
            raise ValueError("incorrect OSI abort context")
        choice = contents(encoded)
        if choice[0] != 0x83:
            raise ValueError("expected OSI diagnostic choice")
        code = int.from_bytes(contents(choice), "big")
        if code not in (0, 1, 2, 3):
            raise ValueError("unknown OSI diagnostic")
        return code

    def verify_control(self, response):
        info = next(v for v in children(response) if v[0] == 0xbe)
        external, = children(info, 0xbe)
        indirect, encoded = children(external, 0x28)
        if int.from_bytes(contents(indirect), "big") != self.e2e_context:
            raise ValueError("invalid control indirect reference")
        tbs, signature = children(contents(encoded))
        algo, value = children(signature, 0x30)
        verify(self.server_cert, algo, contents(value), authentication_contents(tbs))
        token = children(children(tbs, 0x30)[0], 0x30)
        if not any(v[0] == 12 and contents(v) == self.association for v in token):
            raise ValueError("control association mismatch")
        if token[0] != algo or not any(v[0] == 4 and contents(v) == self.server_cert.public_bytes(
                serialization.Encoding.DER) for v in token):
            raise ValueError("control signer mismatch")
        timestamp = next(contents(v) for v in token if v[0] == 24)
        if abs(time.time_ns() - timestamp_ns(timestamp)) > 600_000_000_000:
            raise ValueError("control timestamp outside window")
        return {"kind": contents(encoded)[0],
                "diagnostic": next((int.from_bytes(contents(v), "big") for v in children(tbs)[1:] if v[0] == 10), None),
                "application": next((v for v in children(tbs)[1:] if v[0] != 10), None)}


def parser():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--host", default="127.0.0.1"); p.add_argument("--port", type=int, default=3782)
    for value in ("certificate", "key", "app-ca", "crl", "tls-certificate", "tls-key", "tls-ca"):
        p.add_argument("--" + value, required=True)
    p.add_argument("--ap-title", required=True); p.add_argument("--ae-qualifier", type=int, required=True)
    p.add_argument("--server-ap-title", required=True); p.add_argument("--server-ae-qualifier", type=int, required=True)
    p.add_argument("--icv", choices=("hmac", "gmac128", "gmac256"), default="hmac")
    p.add_argument("--group", type=int, choices=(14, 23, 28))
    p.add_argument("--domain", required=True); p.add_argument("--item", required=True)
    return p


if __name__ == "__main__":
    args = parser().parse_args()
    peer = NativePeer(args)
    try:
        peer.connect()
        print("Native handshake/signature/identity verified")
        print("Protected read:", peer.read(args.domain, args.item).hex())
        print("Rekey acknowledgement:", peer.read(args.domain, args.item, rekey=True).hex())
        print("Protected read after rekey:", peer.read(args.domain, args.item).hex())
        peer.release()
        print("Signed release verified")
    finally:
        peer.close()
