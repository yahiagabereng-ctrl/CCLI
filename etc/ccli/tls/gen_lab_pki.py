#!/usr/bin/env python3
"""Generate lab PKI for P3-07/P3-08/P3-09 with 62351-4 Annex G profile.

62351-4 AMD1 §10.5.3: AP-title + AE-qualifier → one object identifier (AE = final arc).
G.6.2: subject = single objectIdentifier RDN with OBJECT IDENTIFIER value.
"""
from __future__ import annotations

import base64
import datetime
import secrets
import subprocess
import sys
from pathlib import Path

TLS = Path(__file__).resolve().parent

try:
    from cryptography import x509
    from cryptography.hazmat.primitives import hashes, serialization
    from cryptography.hazmat.primitives.asymmetric import padding, rsa
    from cryptography.x509.oid import NameOID, ObjectIdentifier
    from pyasn1.codec.der import decoder, encoder
    from pyasn1.type import univ
    from pyasn1_modules import rfc2459
except ImportError as exc:
    print("Install: pip install cryptography pyasn1 pyasn1-modules", file=sys.stderr)
    print(exc, file=sys.stderr)
    sys.exit(1)

# ITU-T X.501 id-at-objectIdentifier (X.520 §6.2.18) — NOT 2.5.4.45 uniqueIdentifier.
OID_AT_OBJECT_IDENTIFIER = ObjectIdentifier("2.5.4.106")
SERVER_AP = "1.1.1.999.1"
SERVER_AE = 12
# TSP/SCL calling AP for lab IED workspace (CCLI_OFF_SCL / lab_tg544_eth_a.cid).
CLIENT_AP = "1.1.1.999.1"
CLIENT_AE = 12


def _utc_now() -> datetime.datetime:
    return datetime.datetime.now(datetime.timezone.utc)


def _combined_oid(ap: str, ae: int) -> str:
    return f"{ap}.{ae}"


def _g62_subject(combined_oid: str) -> x509.Name:
    """Placeholder for cryptography builder — patched to ASN.1 OBJECT IDENTIFIER after sign."""
    return x509.Name([x509.NameAttribute(OID_AT_OBJECT_IDENTIFIER, combined_oid)])


def _g62_subject_pyasn1(combined_oid: str) -> rfc2459.Name:
    """62351-4 Annex G.6.2 — RDN value must be OBJECT IDENTIFIER (tag 0x06), not UTF8String."""
    atv = rfc2459.AttributeTypeAndValue()
    atv["type"] = univ.ObjectIdentifier("2.5.4.106")
    atv["value"] = univ.ObjectIdentifier(combined_oid)
    rdn = rfc2459.RelativeDistinguishedName()
    rdn.setComponentByPosition(0, atv)
    rdn_seq = rfc2459.RDNSequence()
    rdn_seq.setComponentByPosition(0, rdn)
    name = rfc2459.Name()
    name.setComponentByPosition(0, rdn_seq)
    return name


def _resign_pyasn1_cert(
    ca_key: rsa.RSAPrivateKey, py_cert: rfc2459.Certificate
) -> bytes:
    """Re-sign TBSCertificate after pyasn1 subject patch."""
    tbs_der = encoder.encode(py_cert["tbsCertificate"])
    sig = ca_key.sign(tbs_der, padding.PKCS1v15(), hashes.SHA256())
    py_cert["signatureAlgorithm"] = rfc2459.AlgorithmIdentifier()
    py_cert["signatureAlgorithm"]["algorithm"] = univ.ObjectIdentifier("1.2.840.113549.1.1.11")
    py_cert["signatureAlgorithm"]["parameters"] = univ.Null("")
    py_cert["signatureValue"] = univ.BitString.fromOctetString(sig)
    return encoder.encode(py_cert)


def _verify_g62_subject_der(
    der: bytes, combined_oid: str, *, require_oid_value: bool = True
) -> None:
    """Fail fast if G.6.2 subject RDN is wrong."""
    marker = der.find(b"\x55\x04\x6a")
    if marker < 0:
        raise ValueError("G.6.2 subject missing id-at-objectIdentifier (2.5.4.106)")
    val_tag = der[marker + 3]
    if require_oid_value:
        if val_tag != 0x06:
            raise ValueError(
                f"G.6.2 subject value tag is 0x{val_tag:02x}, want 0x06 OBJECT IDENTIFIER"
            )
        val_der = der[marker + 3 : marker + 3 + der[marker + 4] + 2]
        oid, _ = decoder.decode(val_der, asn1Spec=univ.ObjectIdentifier())
        if str(oid) != combined_oid:
            raise ValueError(f"subject OID {oid} != expected {combined_oid}")
        print(f"    verify: subject OBJECT IDENTIFIER {combined_oid} (tag 0x06)")
    else:
        if val_tag != 0x0C:
            raise ValueError(
                f"G.6.2 subject value tag is 0x{val_tag:02x}, want 0x0c UTF8String (OpenSSL/TSP)"
            )
        val_len = der[marker + 4]
        val = der[marker + 5 : marker + 5 + val_len].decode("ascii")
        if val != combined_oid:
            raise ValueError(f"subject UTF8 {val!r} != expected {combined_oid!r}")
        print(f"    verify: subject 2.5.4.106 UTF8String {combined_oid} (OpenSSL/TSP)")


def _ca_key_usage() -> x509.KeyUsage:
    return x509.KeyUsage(
        digital_signature=True,
        key_cert_sign=True,
        crl_sign=True,
        content_commitment=False,
        key_encipherment=False,
        data_encipherment=False,
        key_agreement=False,
        encipher_only=False,
        decipher_only=False,
    )


def _mms_key_usage() -> x509.KeyUsage:
    """62351-4 Annex G.5.2 — MMS / E2E authentication certs."""
    return x509.KeyUsage(
        digital_signature=True,
        key_agreement=True,
        content_commitment=False,
        key_encipherment=False,
        data_encipherment=False,
        key_cert_sign=False,
        crl_sign=False,
        encipher_only=False,
        decipher_only=False,
    )


def _tls_key_usage() -> x509.KeyUsage:
    """Transport TLS RSA server/client (62351-3 / RFC 5246)."""
    return x509.KeyUsage(
        digital_signature=True,
        key_encipherment=True,
        content_commitment=False,
        data_encipherment=False,
        key_agreement=False,
        key_cert_sign=False,
        crl_sign=False,
        encipher_only=False,
        decipher_only=False,
    )


def _combined_key_usage() -> x509.KeyUsage:
    """IOP COMBINED cert — TLS (62351-3) + MMS auth (62351-4 G.5.2) in one end-entity."""
    return x509.KeyUsage(
        digital_signature=True,
        key_encipherment=True,
        key_agreement=True,
        content_commitment=False,
        data_encipherment=False,
        key_cert_sign=False,
        crl_sign=False,
        encipher_only=False,
        decipher_only=False,
    )


def _write_pem(path: Path, data: bytes) -> None:
    path.write_bytes(data)


def _der_to_pem(label: str, der: bytes) -> bytes:
    b64 = base64.encodebytes(der).decode("ascii")
    lines = "".join(f"{line}\n" for line in b64.splitlines())
    return f"-----BEGIN {label}-----\n{lines}-----END {label}-----\n".encode("ascii")


def _sign_g62_acse_cert(
    ca_key: rsa.RSAPrivateKey,
    ca_cert: x509.Certificate,
    key: rsa.RSAPrivateKey,
    combined_oid: str,
    days: int = 825,
) -> bytes:
    """62351-4 G.6.2 MMS auth cert — OpenSSL/TSP-parseable id-at-objectIdentifier subject."""
    now = _utc_now()
    cert = (
        x509.CertificateBuilder()
        .subject_name(_g62_subject(combined_oid))
        .issuer_name(ca_cert.subject)
        .public_key(key.public_key())
        .serial_number(secrets.randbits(64))
        .not_valid_before(now)
        .not_valid_after(now + datetime.timedelta(days=days))
        .add_extension(_mms_key_usage(), critical=True)
        .add_extension(x509.BasicConstraints(ca=False, path_length=None), critical=True)
        .sign(ca_key, hashes.SHA256())
    )
    der = cert.public_bytes(serialization.Encoding.DER)
    # TSP/OpenSSL x509 loads 2.5.4.106 when value is UTF8String (OID text).
    # OBJECT IDENTIFIER value makes OpenSSL 3 "unsupported" (TSP decode failure).
    _verify_g62_subject_der(der, combined_oid, require_oid_value=False)
    return der


def _sign_g62_combined_cert(
    ca_key: rsa.RSAPrivateKey,
    ca_cert: x509.Certificate,
    key: rsa.RSAPrivateKey,
    combined_oid: str,
    days: int = 825,
) -> bytes:
    """UCA IOP COMBINED — G.6.2 subject + merged keyUsage for TLS and AARE."""
    now = _utc_now()
    cert = (
        x509.CertificateBuilder()
        .subject_name(_g62_subject(combined_oid))
        .issuer_name(ca_cert.subject)
        .public_key(key.public_key())
        .serial_number(secrets.randbits(64))
        .not_valid_before(now)
        .not_valid_after(now + datetime.timedelta(days=days))
        .add_extension(_combined_key_usage(), critical=True)
        .add_extension(x509.BasicConstraints(ca=False, path_length=None), critical=True)
        .sign(ca_key, hashes.SHA256())
    )
    der = cert.public_bytes(serialization.Encoding.DER)
    _verify_g62_subject_der(der, combined_oid, require_oid_value=False)
    return der


def _gen_ca() -> tuple[rsa.RSAPrivateKey, x509.Certificate]:
    key = rsa.generate_private_key(public_exponent=65537, key_size=2048)
    name = x509.Name(
        [
            x509.NameAttribute(NameOID.COUNTRY_NAME, "IT"),
            x509.NameAttribute(NameOID.ORGANIZATION_NAME, "HiTEKS"),
            x509.NameAttribute(NameOID.ORGANIZATIONAL_UNIT_NAME, "CCLI-Lab"),
            x509.NameAttribute(NameOID.COMMON_NAME, "CCLI_Lab_Root_CA"),
        ]
    )
    now = _utc_now()
    cert = (
        x509.CertificateBuilder()
        .subject_name(name)
        .issuer_name(name)
        .public_key(key.public_key())
        .serial_number(secrets.randbits(64))
        .not_valid_before(now)
        .not_valid_after(now + datetime.timedelta(days=3650))
        .add_extension(x509.BasicConstraints(ca=True, path_length=None), critical=True)
        .add_extension(_ca_key_usage(), critical=True)
        .sign(key, hashes.SHA256())
    )
    return key, cert


def _sign_tls_cert(
    ca_key: rsa.RSAPrivateKey,
    ca_cert: x509.Certificate,
    cn: str,
    key: rsa.RSAPrivateKey,
    days: int = 825,
) -> bytes:
    """62351-4 G.2 transport cert — CN subject for mbedtls TLS / allow-list."""
    name = x509.Name(
        [
            x509.NameAttribute(NameOID.COUNTRY_NAME, "IT"),
            x509.NameAttribute(NameOID.ORGANIZATION_NAME, "HiTEKS"),
            x509.NameAttribute(NameOID.ORGANIZATIONAL_UNIT_NAME, "CCLI-Lab"),
            x509.NameAttribute(NameOID.COMMON_NAME, cn),
        ]
    )
    now = _utc_now()
    cert = (
        x509.CertificateBuilder()
        .subject_name(name)
        .issuer_name(ca_cert.subject)
        .public_key(key.public_key())
        .serial_number(secrets.randbits(64))
        .not_valid_before(now)
        .not_valid_after(now + datetime.timedelta(days=days))
        .add_extension(_tls_key_usage(), critical=True)
        .add_extension(x509.BasicConstraints(ca=False, path_length=None), critical=True)
        .sign(ca_key, hashes.SHA256())
    )
    return cert.public_bytes(serialization.Encoding.DER)


def _run(cmd: list[str]) -> None:
    print("+", " ".join(cmd))
    subprocess.check_call(cmd, cwd=TLS)


def _gen_crl(revoked_serial: int, ca_key: rsa.RSAPrivateKey, ca_cert: x509.Certificate) -> None:
    """Build lab CRL without requiring openssl on PATH."""
    now = _utc_now()
    builder = (
        x509.CertificateRevocationListBuilder()
        .issuer_name(ca_cert.subject)
        .last_update(now)
        .next_update(now + datetime.timedelta(days=30))
    )
    if revoked_serial:
        builder = builder.add_revoked_certificate(
            x509.RevokedCertificateBuilder()
            .serial_number(revoked_serial)
            .revocation_date(now - datetime.timedelta(days=1))
            .build()
        )
    crl = builder.sign(ca_key, hashes.SHA256())
    _write_pem(TLS / "lab_crl.pem", crl.public_bytes(serialization.Encoding.PEM))


def main() -> int:
    TLS.mkdir(parents=True, exist_ok=True)

    ca_key, ca_cert = _gen_ca()
    _write_pem(
        TLS / "root_CA.key",
        ca_key.private_bytes(
            serialization.Encoding.PEM,
            serialization.PrivateFormat.TraditionalOpenSSL,
            serialization.NoEncryption(),
        ),
    )
    _write_pem(
        TLS / "root_CA.pem",
        ca_cert.public_bytes(serialization.Encoding.PEM),
    )

    leaves = [
        ("server", "CCI016_01", _combined_oid(SERVER_AP, SERVER_AE), True),
        ("client", "DSO_OPERATOR", _combined_oid(CLIENT_AP, CLIENT_AE), True),
        ("viewer", "VIEWER", _combined_oid(CLIENT_AP, CLIENT_AE), False),
        ("revoked", "REVOKED_CLIENT", _combined_oid(CLIENT_AP, CLIENT_AE), False),
    ]

    revoked_serial = 0
    for stem, cn, subj_oid, g62_acse in leaves:
        tls_key = rsa.generate_private_key(public_exponent=65537, key_size=2048)
        tls_der = _sign_tls_cert(ca_key, ca_cert, cn, tls_key)
        acse_key = tls_key
        if stem == "server" and g62_acse:
            # Annex G.2: separate E2E auth key from TLS transport key (server only).
            acse_key = rsa.generate_private_key(public_exponent=65537, key_size=2048)
            acse_der = _sign_g62_acse_cert(ca_key, ca_cert, acse_key, subj_oid)
        elif g62_acse:
            acse_der = _sign_g62_acse_cert(ca_key, ca_cert, acse_key, subj_oid)
        else:
            acse_der = tls_der
        if stem == "revoked":
            py_cert, _ = decoder.decode(tls_der, asn1Spec=rfc2459.Certificate())
            revoked_serial = int(py_cert["tbsCertificate"]["serialNumber"])
        _write_pem(
            TLS / f"{stem}.key",
            tls_key.private_bytes(
                serialization.Encoding.PEM,
                serialization.PrivateFormat.TraditionalOpenSSL,
                serialization.NoEncryption(),
            ),
        )
        if stem == "server" and g62_acse:
            _write_pem(
                TLS / "server_acse.key",
                acse_key.private_bytes(
                    serialization.Encoding.PEM,
                    serialization.PrivateFormat.TraditionalOpenSSL,
                    serialization.NoEncryption(),
                ),
            )
        _write_pem(TLS / f"{stem}.pem", _der_to_pem("CERTIFICATE", acse_der))
        _write_pem(TLS / f"{stem}_tls.pem", _der_to_pem("CERTIFICATE", tls_der))
        tag = f"G.6.2 subjectOID={subj_oid}" if g62_acse else "CN subject (TSP/OpenSSL)"
        dual = " + server_acse.key (G.2)" if stem == "server" and g62_acse else ""
        print(f"  {stem}: {tag} + {stem}_tls.pem (transport){dual}")

    (TLS / "client_tsp.key").write_bytes((TLS / "client.key").read_bytes())

    # Solution C — single server cert/key for Transport TLS + AARE (IOP _COMBINED pattern).
    combined_key = rsa.generate_private_key(public_exponent=65537, key_size=2048)
    combined_oid = _combined_oid(SERVER_AP, SERVER_AE)
    combined_der = _sign_g62_combined_cert(ca_key, ca_cert, combined_key, combined_oid)
    _write_pem(
        TLS / "server_combined.key",
        combined_key.private_bytes(
            serialization.Encoding.PEM,
            serialization.PrivateFormat.TraditionalOpenSSL,
            serialization.NoEncryption(),
        ),
    )
    _write_pem(TLS / "server_combined.pem", _der_to_pem("CERTIFICATE", combined_der))
    print(f"  server_combined: G.6.2 subjectOID={combined_oid} + COMBINED keyUsage (TLS+AARE)")

    _gen_crl(revoked_serial, ca_key, ca_cert)

    print("OK lab PKI in", TLS)
    return 0


if __name__ == "__main__":
    sys.exit(main())
