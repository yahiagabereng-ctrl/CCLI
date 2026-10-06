#!/usr/bin/env python3
"""Issue Annex-aligned CCLI lab PKI signed by CCLI-Lab-CA.

Lab hybrid:
  1. Generate Annex CA (RSA-2048 / SHA-256 / 3650d) as PKCS#12 + PEM
  2. Import that CA into EJBCA CE (so CRL / Admin UI see CCLI-Lab-CA)
  3. Sign Cert A (TLS) + Cert B (G.6.2 E2E) with the same CA key

Extensions follow Annex T T.3.3.4 / 62351-9 / 62351-4 Annex G (see lab/EJBCA_LAB_SETUP.md §10).
"""
from __future__ import annotations

import argparse
import datetime
import importlib.util
import ipaddress
import secrets
import sys
from pathlib import Path

from cryptography import x509
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import rsa
from cryptography.hazmat.primitives.serialization import pkcs12
from cryptography.x509.oid import ExtendedKeyUsageOID, NameOID

REPO = Path(__file__).resolve().parents[1]
GEN_LAB = REPO / "apps" / "ccli" / "config" / "tls" / "gen_lab_pki.py"
DEFAULT_OUT = REPO / "apps" / "ccli" / "config" / "tls" / "ejbca"

SERVER_AP = "1.1.1.999.1"
SERVER_AE = 12
MMS_OID = f"{SERVER_AP}.{SERVER_AE}"
CA_CN = "CCLI Lab CA"


def _load_gen_lab():
    spec = importlib.util.spec_from_file_location("gen_lab_pki", GEN_LAB)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"Cannot load {GEN_LAB}")
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


def _load_ca_from_p12(p12_path: Path, password: str) -> tuple[rsa.RSAPrivateKey, x509.Certificate]:
    data = p12_path.read_bytes()
    key, cert, _ = pkcs12.load_key_and_certificates(data, password.encode("utf-8"))
    if key is None or cert is None:
        raise ValueError(f"No key/cert in {p12_path}")
    if not isinstance(key, rsa.RSAPrivateKey):
        raise TypeError("EJBCA CA key must be RSA for CCLI lab profile")
    return key, cert


def _write_pem(path: Path, data: bytes) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(data)


def _der_to_pem(label: str, der: bytes) -> bytes:
    import base64

    b64 = base64.encodebytes(der).decode("ascii")
    lines = "".join(f"{line}\n" for line in b64.splitlines())
    return f"-----BEGIN {label}-----\n{lines}-----END {label}-----\n".encode("ascii")


def _key_pem(key: rsa.RSAPrivateKey) -> bytes:
    return key.private_bytes(
        serialization.Encoding.PEM,
        serialization.PrivateFormat.TraditionalOpenSSL,
        serialization.NoEncryption(),
    )


def _generate_ca(gen, password: str) -> tuple[rsa.RSAPrivateKey, x509.Certificate]:
    """Annex CA: RSA ≥2048, SHA-256, X.509 v3 (T.3.3.4.1 / 62351-4 G.3)."""
    key = rsa.generate_private_key(public_exponent=65537, key_size=2048)
    name = x509.Name(
        [
            x509.NameAttribute(NameOID.COUNTRY_NAME, "IT"),
            x509.NameAttribute(NameOID.ORGANIZATION_NAME, "HiTEKS"),
            x509.NameAttribute(NameOID.ORGANIZATIONAL_UNIT_NAME, "CCLI-Lab"),
            x509.NameAttribute(NameOID.COMMON_NAME, CA_CN),
        ]
    )
    now = gen._utc_now()
    cert = (
        x509.CertificateBuilder()
        .subject_name(name)
        .issuer_name(name)
        .public_key(key.public_key())
        .serial_number(secrets.randbits(64))
        .not_valid_before(now)
        .not_valid_after(now + datetime.timedelta(days=3650))
        .add_extension(x509.BasicConstraints(ca=True, path_length=None), critical=True)
        .add_extension(gen._ca_key_usage(), critical=True)
        .sign(key, hashes.SHA256())
    )
    return key, cert


def _write_ca_p12(
    path: Path, key: rsa.RSAPrivateKey, cert: x509.Certificate, password: str
) -> None:
    # Friendly name signKey matches EJBCA importca / exportca convention.
    blob = pkcs12.serialize_key_and_certificates(
        name=b"signKey",
        key=key,
        cert=cert,
        cas=None,
        encryption_algorithm=serialization.BestAvailableEncryption(password.encode("utf-8")),
    )
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(blob)


def _sign_tls_cert_annex(
    gen,
    ca_key: rsa.RSAPrivateKey,
    ca_cert: x509.Certificate,
    cn: str,
    key: rsa.RSAPrivateKey,
    *,
    san_ip: str | None = None,
    days: int = 825,
) -> bytes:
    """Cert A — 62351-3 + 62351-9 §7.4.4.10 (serverAuth + clientAuth)."""
    name = x509.Name(
        [
            x509.NameAttribute(NameOID.COUNTRY_NAME, "IT"),
            x509.NameAttribute(NameOID.ORGANIZATION_NAME, "HiTEKS"),
            x509.NameAttribute(NameOID.ORGANIZATIONAL_UNIT_NAME, "CCLI-Lab"),
            x509.NameAttribute(NameOID.COMMON_NAME, cn),
        ]
    )
    now = gen._utc_now()
    builder = (
        x509.CertificateBuilder()
        .subject_name(name)
        .issuer_name(ca_cert.subject)
        .public_key(key.public_key())
        .serial_number(secrets.randbits(64))
        .not_valid_before(now)
        .not_valid_after(now + datetime.timedelta(days=days))
        .add_extension(gen._tls_key_usage(), critical=True)
        .add_extension(
            x509.ExtendedKeyUsage(
                [ExtendedKeyUsageOID.SERVER_AUTH, ExtendedKeyUsageOID.CLIENT_AUTH]
            ),
            critical=False,
        )
        .add_extension(x509.BasicConstraints(ca=False, path_length=None), critical=True)
    )
    if san_ip:
        builder = builder.add_extension(
            x509.SubjectAlternativeName([x509.IPAddress(ipaddress.ip_address(san_ip))]),
            critical=False,
        )
    return builder.sign(ca_key, hashes.SHA256()).public_bytes(serialization.Encoding.DER)


def cmd_generate_ca(args: argparse.Namespace) -> int:
    gen = _load_gen_lab()
    ca_key, ca_cert = _generate_ca(gen, args.p12_password)
    staging = args.staging
    staging.mkdir(parents=True, exist_ok=True)
    p12_path = staging / "ca-export.p12"
    pem_path = staging / "root_CA.pem"
    _write_ca_p12(p12_path, ca_key, ca_cert, args.p12_password)
    _write_pem(pem_path, ca_cert.public_bytes(serialization.Encoding.PEM))
    print(f"CA generated: CN={CA_CN}")
    print(f"  {p12_path}")
    print(f"  {pem_path}")
    return 0


def cmd_issue(args: argparse.Namespace) -> int:
    gen = _load_gen_lab()
    ca_key, ca_from_p12 = _load_ca_from_p12(args.ca_p12, args.p12_password)
    ca_cert = (
        x509.load_pem_x509_certificate(args.ca_pem.read_bytes())
        if args.ca_pem
        else ca_from_p12
    )
    out = args.out
    out.mkdir(parents=True, exist_ok=True)

    _write_pem(out / "root_CA.pem", ca_cert.public_bytes(serialization.Encoding.PEM))

    leaves = [
        ("server", "CCI016_01", MMS_OID, True),
        ("client", "DSO_OPERATOR", MMS_OID, True),
        ("viewer", "VIEWER", MMS_OID, False),
        ("revoked", "REVOKED_CLIENT", MMS_OID, False),
    ]

    revoked_serial = 0
    for stem, cn, subj_oid, g62_acse in leaves:
        tls_key = rsa.generate_private_key(public_exponent=65537, key_size=2048)
        san = args.san_ip if stem == "server" else None
        tls_der = _sign_tls_cert_annex(gen, ca_key, ca_cert, cn, tls_key, san_ip=san)
        acse_key = tls_key
        if stem == "server" and g62_acse:
            acse_key = rsa.generate_private_key(public_exponent=65537, key_size=2048)
            acse_der = gen._sign_g62_acse_cert(ca_key, ca_cert, acse_key, subj_oid)
        elif g62_acse:
            acse_der = gen._sign_g62_acse_cert(ca_key, ca_cert, tls_key, subj_oid)
        else:
            acse_der = tls_der
        if stem == "revoked":
            from pyasn1.codec.der import decoder
            from pyasn1_modules import rfc2459

            py_cert, _ = decoder.decode(tls_der, asn1Spec=rfc2459.Certificate())
            revoked_serial = int(py_cert["tbsCertificate"]["serialNumber"])

        _write_pem(out / f"{stem}.key", _key_pem(tls_key))
        if stem == "server" and g62_acse:
            _write_pem(out / "server_acse.key", _key_pem(acse_key))
        # DUT layout: stem.pem = Cert B (E2E) when dual; stem_tls.pem = Cert A
        _write_pem(out / f"{stem}.pem", _der_to_pem("CERTIFICATE", acse_der))
        _write_pem(out / f"{stem}_tls.pem", _der_to_pem("CERTIFICATE", tls_der))
        print(f"  issued {stem} (+ {stem}_tls.pem)")

    _write_pem(out / "client_tsp.key", (out / "client.key").read_bytes())

    now = gen._utc_now()
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
    _write_pem(out / "lab_crl.pem", crl.public_bytes(serialization.Encoding.PEM))

    readme = f"""# Annex-aligned CCLI lab PKI (EJBCA CCLI-Lab-CA)

Generated by `scripts/provision_ejbca_ccli_pki.py`.

| Artefact | Annex / clause | Profile |
|----------|----------------|---------|
| `root_CA.pem` | T.3.3.4.1 / G.3 | RSA-2048 SHA-256 self-signed CA |
| `server_tls.pem` + `server.key` | G.2 / 62351-9 §7.4.4.10 | Cert A TLS (serverAuth+clientAuth, SAN IP) |
| `server.pem` + `server_acse.key` | G.2 / G.5.2 / G.6.2 | Cert B E2E subject `{MMS_OID}` |
| `client.pem` / `client_tls.pem` | G.2 operator dual | DSO_OPERATOR |
| `lab_crl.pem` | T.3.3.4.9 | CRL with revoked leaf |

Deploy to DUT `/etc/ccli/tls/` and TSP `C:\\CCLI_product_tls\\`.
"""
    _write_pem(out / "README.md", readme.encode("utf-8"))
    print(f"OK -> {out}")
    return 0


def main() -> int:
    ap = argparse.ArgumentParser(description="CCLI Annex-aligned PKI for EJBCA lab")
    sub = ap.add_subparsers(dest="cmd", required=True)

    g = sub.add_parser("generate-ca", help="Create Annex CA PKCS#12 + PEM for EJBCA import")
    g.add_argument("--staging", type=Path, required=True)
    g.add_argument("--p12-password", default="ccli-lab-p12")

    i = sub.add_parser("issue", help="Issue Cert A/B leaves signed by CA PKCS#12")
    i.add_argument("--ca-p12", type=Path, required=True)
    i.add_argument("--ca-pem", type=Path, default=None)
    i.add_argument("--p12-password", default="ccli-lab-p12")
    i.add_argument("--out", type=Path, default=DEFAULT_OUT)
    i.add_argument("--san-ip", default="192.168.10.1")

    args = ap.parse_args()
    if args.cmd == "generate-ca":
        return cmd_generate_ca(args)
    if args.cmd == "issue":
        return cmd_issue(args)
    return 2


if __name__ == "__main__":
    sys.exit(main())
