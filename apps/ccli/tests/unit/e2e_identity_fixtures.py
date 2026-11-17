#!/usr/bin/env python3
"""Create disposable IEC 62351-4 Annex G certificate-path fixtures."""

import argparse
import datetime as dt
from pathlib import Path

from cryptography import x509
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import ec, padding, rsa
from cryptography.x509.oid import NameOID


def der_length(length):
    if length < 128:
        return bytes([length])
    encoded = length.to_bytes((length.bit_length() + 7) // 8, "big")
    return bytes([0x80 | len(encoded)]) + encoded


def der_tlv(tag, contents):
    return bytes([tag]) + der_length(len(contents)) + contents


def read_tlv(encoded, offset):
    tag = encoded[offset]
    offset += 1
    first = encoded[offset]
    offset += 1
    if first < 0x80:
        length = first
    else:
        count = first & 0x7F
        if count == 0 or count > 4:
            raise ValueError("invalid DER length")
        length = int.from_bytes(encoded[offset:offset + count], "big")
        offset += count
    end = offset + length
    if end > len(encoded):
        raise ValueError("truncated DER value")
    return tag, encoded[offset:end], end


def der_children(encoded, expected_tag):
    tag, contents, end = read_tlv(encoded, 0)
    if tag != expected_tag or end != len(encoded):
        raise ValueError("unexpected DER container")
    children = []
    offset = 0
    while offset < len(contents):
        _, _, child_end = read_tlv(contents, offset)
        children.append(contents[offset:child_end])
        offset = child_end
    return contents, children


def encode_oid(arcs):
    if len(arcs) < 2 or arcs[0] not in (0, 1, 2):
        raise ValueError("invalid OID arcs")
    if arcs[0] < 2 and not 0 <= arcs[1] <= 39:
        raise ValueError("invalid second OID arc")
    encoded = bytearray()
    for value in [40 * arcs[0] + arcs[1], *arcs[2:]]:
        if value < 0:
            raise ValueError("negative OID arc")
        subidentifier = [value & 0x7F]
        value >>= 7
        while value:
            subidentifier.append(0x80 | (value & 0x7F))
            value >>= 7
        encoded.extend(reversed(subidentifier))
    return bytes(encoded)


def replace_subject_with_oid_value(certificate_der, private_key, oid_value):
    _, certificate_fields = der_children(certificate_der, 0x30)
    _, tbs_fields = der_children(certificate_fields[0], 0x30)
    subject_index = 5 if tbs_fields[0][0] == 0xA0 else 4

    type_oid = der_tlv(0x06, encode_oid([2, 5, 4, 106]))
    value_oid = der_tlv(0x06, oid_value)
    attribute = der_tlv(0x30, type_oid + value_oid)
    relative_name = der_tlv(0x31, attribute)
    subject = der_tlv(0x30, relative_name)

    tbs_fields[subject_index] = subject
    tbs_der = der_tlv(0x30, b"".join(tbs_fields))
    signature = private_key.sign(tbs_der, padding.PKCS1v15(), hashes.SHA256())
    signature_bits = der_tlv(0x03, b"\x00" + signature)
    return der_tlv(0x30, tbs_der + certificate_fields[1] + signature_bits)


def build_leaf(root_key, root_name, peer_key, serial, now, key_agreement=True,
               include_crl_distribution_points=True, extra_extensions=()):
    usage = x509.KeyUsage(
        digital_signature=True,
        content_commitment=False,
        key_encipherment=False,
        data_encipherment=False,
        key_agreement=key_agreement,
        key_cert_sign=False,
        crl_sign=False,
        encipher_only=False if key_agreement else None,
        decipher_only=False if key_agreement else None,
    )
    builder = (
        x509.CertificateBuilder()
        .subject_name(x509.Name([x509.NameAttribute(NameOID.COMMON_NAME, "E2E peer")]))
        .issuer_name(root_name)
        .public_key(peer_key.public_key())
        .serial_number(serial)
        .not_valid_before(now - dt.timedelta(days=1))
        .not_valid_after(now + dt.timedelta(days=30))
        .add_extension(x509.BasicConstraints(ca=False, path_length=None), critical=True)
        .add_extension(usage, critical=True)
    )
    if include_crl_distribution_points:
        builder = builder.add_extension(
            x509.CRLDistributionPoints(
                [x509.DistributionPoint(
                    full_name=[x509.UniformResourceIdentifier("http://e2e.test/root.crl")],
                    relative_name=None,
                    reasons=None,
                    crl_issuer=None,
                )]
            ),
            critical=False,
        )
    for extension in extra_extensions:
        builder = builder.add_extension(extension, critical=False)
    return builder.sign(root_key, hashes.SHA256())


def write(path, contents):
    path.write_bytes(contents)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out", required=True)
    args = parser.parse_args()
    out = Path(args.out)
    out.mkdir(parents=True, exist_ok=True)

    now = dt.datetime.now(dt.timezone.utc)
    root_key = rsa.generate_private_key(public_exponent=65537, key_size=2048)
    root_name = x509.Name([x509.NameAttribute(NameOID.COMMON_NAME, "E2E test root")])
    root_cert = (
        x509.CertificateBuilder()
        .subject_name(root_name)
        .issuer_name(root_name)
        .public_key(root_key.public_key())
        .serial_number(1)
        .not_valid_before(now - dt.timedelta(days=1))
        .not_valid_after(now + dt.timedelta(days=365))
        .add_extension(x509.BasicConstraints(ca=True, path_length=1), critical=True)
        .add_extension(
            x509.KeyUsage(
                digital_signature=True,
                content_commitment=False,
                key_encipherment=False,
                data_encipherment=False,
                key_agreement=False,
                key_cert_sign=True,
                crl_sign=True,
                encipher_only=None,
                decipher_only=None,
            ),
            critical=True,
        )
        .add_extension(x509.SubjectKeyIdentifier.from_public_key(root_key.public_key()), critical=False)
        .add_extension(x509.AuthorityKeyIdentifier.from_issuer_public_key(root_key.public_key()), critical=False)
        .sign(root_key, hashes.SHA256())
    )
    root_der = root_cert.public_bytes(serialization.Encoding.DER)
    write(out / "root.der", root_der)

    identity_oid = encode_oid([1, 3, 6, 1, 4, 1, 55555, 62351, 4, 1, 7])
    write(out / "identity_oid.bin", identity_oid)

    rsa_key = rsa.generate_private_key(public_exponent=65537, key_size=2048)
    rsa_leaf = build_leaf(root_key, root_name, rsa_key, 101, now).public_bytes(serialization.Encoding.DER)
    rsa_oid = replace_subject_with_oid_value(rsa_leaf, root_key, identity_oid)
    write(out / "leaf_rsa_oid.der", rsa_oid)

    ec_key = ec.generate_private_key(ec.SECP256R1())
    ec_leaf = build_leaf(root_key, root_name, ec_key, 102, now).public_bytes(serialization.Encoding.DER)
    write(out / "leaf_ec_oid.der", replace_subject_with_oid_value(ec_leaf, root_key, identity_oid))

    weak_key = rsa.generate_private_key(public_exponent=65537, key_size=1024)
    weak_leaf = build_leaf(root_key, root_name, weak_key, 103, now).public_bytes(serialization.Encoding.DER)
    write(out / "leaf_weak_rsa_oid.der", replace_subject_with_oid_value(weak_leaf, root_key, identity_oid))

    no_agreement = build_leaf(root_key, root_name, rsa_key, 104, now, key_agreement=False)
    write(out / "leaf_no_key_agreement_oid.der", replace_subject_with_oid_value(
        no_agreement.public_bytes(serialization.Encoding.DER), root_key, identity_oid))

    text_leaf = build_leaf(root_key, root_name, rsa_key, 105, now)
    write(out / "leaf_utf8_subject.der", text_leaf.public_bytes(serialization.Encoding.DER))

    no_distribution_point = build_leaf(
        root_key, root_name, rsa_key, 106, now,
        include_crl_distribution_points=False)
    write(out / "leaf_no_crl_dp_oid.der", replace_subject_with_oid_value(
        no_distribution_point.public_bytes(serialization.Encoding.DER),
        root_key, identity_oid))

    empty_crl = (
        x509.CertificateRevocationListBuilder()
        .issuer_name(root_name)
        .last_update(now - dt.timedelta(hours=1))
        .next_update(now + dt.timedelta(days=30))
        .sign(root_key, hashes.SHA256())
    )
    revoked_entry = (
        x509.RevokedCertificateBuilder()
        .serial_number(101)
        .revocation_date(now - dt.timedelta(minutes=10))
        .build()
    )
    revoked_crl = (
        x509.CertificateRevocationListBuilder()
        .issuer_name(root_name)
        .last_update(now - dt.timedelta(hours=1))
        .next_update(now + dt.timedelta(days=30))
        .add_revoked_certificate(revoked_entry)
        .sign(root_key, hashes.SHA256())
    )
    write(out / "root.crl.der", empty_crl.public_bytes(serialization.Encoding.DER))
    write(out / "root_revoked.crl.der", revoked_crl.public_bytes(serialization.Encoding.DER))
    for label, uri, ca_only, reasons in [
            ("matching_scope", "http://e2e.test/root.crl", False, None),
            ("wrong_scope", "http://e2e.test/other.crl", False, None),
            ("ca_only", "http://e2e.test/root.crl", True, None),
            ("partial_reasons", "http://e2e.test/root.crl", False,
             frozenset([x509.ReasonFlags.key_compromise]))]:
        point = x509.IssuingDistributionPoint(
            full_name=[x509.UniformResourceIdentifier(uri)], relative_name=None,
            only_contains_user_certs=False, only_contains_ca_certs=ca_only,
            only_some_reasons=reasons, indirect_crl=False,
            only_contains_attribute_certs=False)
        scoped = (x509.CertificateRevocationListBuilder().issuer_name(root_name)
                  .last_update(now - dt.timedelta(hours=1)).next_update(now + dt.timedelta(days=30))
                  .add_extension(point, critical=True).sign(root_key, hashes.SHA256()))
        write(out / (label + ".crl.der"), scoped.public_bytes(serialization.Encoding.DER))
    malformed = (x509.CertificateRevocationListBuilder().issuer_name(root_name)
                 .last_update(now - dt.timedelta(hours=1)).next_update(now + dt.timedelta(days=30))
                 .add_extension(x509.UnrecognizedExtension(
                     x509.ObjectIdentifier("2.5.29.28"), b"\x30\x02\x86\x01"), critical=True)
                 .sign(root_key, hashes.SHA256()))
    write(out / "malformed_scope.crl.der", malformed.public_bytes(serialization.Encoding.DER))


if __name__ == "__main__":
    main()
