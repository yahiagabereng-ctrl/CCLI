#!/usr/bin/env python3
"""Generate disposable signing fixtures for the E2E crypto provider test."""
from __future__ import annotations

import argparse
import datetime as dt
from pathlib import Path

from cryptography import x509
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import ec, rsa
from cryptography.x509.oid import NameOID


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out", type=Path, required=True)
    args = parser.parse_args()
    args.out.mkdir(parents=True, exist_ok=True, mode=0o700)
    args.out.chmod(0o700)

    now = dt.datetime.now(dt.timezone.utc).replace(microsecond=0)
    subject = x509.Name([x509.NameAttribute(NameOID.COMMON_NAME, "E2E test fixture")])

    fixtures = (
        ("rsa_2048", rsa.generate_private_key(public_exponent=65537, key_size=2048)),
        ("rsa_1024", rsa.generate_private_key(public_exponent=65537, key_size=1024)),
        ("secp256r1", ec.generate_private_key(ec.SECP256R1())),
        ("brainpool256r1", ec.generate_private_key(ec.BrainpoolP256R1())),
        ("secp224r1", ec.generate_private_key(ec.SECP224R1())),
    )

    for name, private_key in fixtures:
        certificate = (
            x509.CertificateBuilder()
            .subject_name(subject)
            .issuer_name(subject)
            .public_key(private_key.public_key())
            .serial_number(x509.random_serial_number())
            .not_valid_before(now - dt.timedelta(days=1))
            .not_valid_after(now + dt.timedelta(days=1))
            .add_extension(x509.BasicConstraints(ca=False, path_length=None), critical=True)
            .add_extension(
                x509.KeyUsage(
                    digital_signature=True,
                    content_commitment=False,
                    key_encipherment=False,
                    data_encipherment=False,
                    key_agreement=True,
                    key_cert_sign=False,
                    crl_sign=False,
                    encipher_only=False,
                    decipher_only=False,
                ),
                critical=True,
            )
            .sign(private_key, hashes.SHA256())
        )
        key_path = args.out / f"{name}.key.pem"
        key_path.write_bytes(
            private_key.private_bytes(
                serialization.Encoding.PEM,
                serialization.PrivateFormat.PKCS8,
                serialization.NoEncryption(),
            )
        )
        key_path.chmod(0o600)
        (args.out / f"{name}.cert.der").write_bytes(
            certificate.public_bytes(serialization.Encoding.DER)
        )


if __name__ == "__main__":
    main()
