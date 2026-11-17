#!/usr/bin/env python3
"""Prepare an owner-signed RBAC package without changing device TLS files.

Requires Python cryptography. Owner private keys stay on the commissioning host.
No command copies certificates/keys into the TLS directory or enables RBAC.
"""
from __future__ import annotations

import argparse
import hashlib
import os
from pathlib import Path
import xml.etree.ElementTree as ET

from cryptography import x509
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import ec, padding, rsa

FILES = (
    "commissioning-policy.xml", "permission-library.xml",
    "commissioning-scopes.xml", "identity-bindings.xml",
)


def certificate_pin(path: Path) -> str:
    data = path.read_bytes()
    cert = x509.load_pem_x509_certificate(data) if b"-----BEGIN" in data else x509.load_der_x509_certificate(data)
    return hashlib.sha256(cert.public_bytes(serialization.Encoding.DER)).hexdigest()


def write_xml(path: Path, root: ET.Element) -> None:
    ET.indent(root, space="  ")
    data = ET.tostring(root, encoding="utf-8", xml_declaration=True) + b"\n"
    temporary = path.with_name(path.name + ".new")
    with temporary.open("xb") as handle:
        handle.write(data)
        handle.flush()
        os.fsync(handle.fileno())
    os.replace(temporary, path)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest="command", required=True)
    bind = commands.add_parser("bind", help="pin an existing identity and its existing TLS transport certificate")
    bind.add_argument("--package", type=Path, required=True)
    bind.add_argument("--identity", type=Path, required=True)
    bind.add_argument("--transport", type=Path, required=True)
    bind.add_argument("--actor", choices=("DSO", "AGGREGATOR", "OWNER"), required=True)
    bind.add_argument("--roles", required=True, help="comma-separated role IDs; for a negative ID use --roles=-1,0")
    sign = commands.add_parser("sign", help="sign all four package files using the owner's private key")
    sign.add_argument("--package", type=Path, required=True)
    sign.add_argument("--sequence", type=int, required=True)
    sign.add_argument("--owner-key", type=Path, required=True)
    args = parser.parse_args()
    if args.command == "bind":
        roles = [int(value) for value in args.roles.split(",")]
        allowed = {"DSO": {-1, 0}, "AGGREGATOR": {-2, 0}, "OWNER": set(range(7))}[args.actor]
        if not roles or len(set(roles)) != len(roles) or not set(roles) <= allowed:
            parser.error("roles do not match the actor")
        path = args.package / "identity-bindings.xml"
        root = ET.parse(path).getroot() if path.exists() else ET.Element("IdentityBindings", Version="1.0")
        if root.tag != "IdentityBindings" or root.attrib != {"Version": "1.0"}:
            parser.error("invalid identity binding document")
        pin = certificate_pin(args.identity)
        for entry in list(root):
            if entry.attrib.get("Sha256") == pin:
                root.remove(entry)
        ET.SubElement(root, "Identity", Sha256=pin, TransportSha256=certificate_pin(args.transport),
                      Actor=args.actor, Roles=",".join(str(role) for role in roles))
        write_xml(path, root)
        print(f"Bound {args.actor} identity {pin}; sign the package before activation.")
    else:
        if not 0 < args.sequence < 2**64:
            parser.error("sequence must be in 1..2^64-1")
        entries = [(name, hashlib.sha256((args.package / name).read_bytes()).hexdigest()) for name in FILES]
        payload = ("CCLI-RBAC-PACKAGE-1\n" + str(args.sequence) + "\n" +
                   "".join(f"{name} {digest}\n" for name, digest in entries)).encode("ascii")
        key = serialization.load_pem_private_key(args.owner_key.read_bytes(), password=None)
        if isinstance(key, rsa.RSAPrivateKey):
            if key.key_size < 2048:
                parser.error("owner RSA key must be at least 2048 bits")
            signature = key.sign(payload, padding.PKCS1v15(), hashes.SHA256())
        elif isinstance(key, ec.EllipticCurvePrivateKey):
            if key.key_size < 256:
                parser.error("owner EC key must be at least 256 bits")
            signature = key.sign(payload, ec.ECDSA(hashes.SHA256()))
        else:
            parser.error("supported owner signing keys: RSA or ECDSA")
        manifest = ET.Element("CommissioningManifest", Version="1.0", Sequence=str(args.sequence), Signature=signature.hex())
        for name, digest in entries:
            ET.SubElement(manifest, "File", Name=name, Sha256=digest)
        write_xml(args.package / "commissioning-manifest.xml", manifest)
        print(f"Signed RBAC package sequence {args.sequence}; no TLS files were modified.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
