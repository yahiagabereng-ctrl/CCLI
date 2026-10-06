#!/usr/bin/env python3
"""Issue Annex CCLI lab PEMs via EJBCA CLI (addendentity + createcert).

No REST admin P12 required — uses `docker exec ccli-ejbca-ce ejbca.sh`.
Requires: CCLI-Lab-CA imported; profiles from setup_ejbca_annex_profiles.py.

See lab/EJBCA_LAB_SETUP.md §12.
"""
from __future__ import annotations

import argparse
import ipaddress
import subprocess
import sys
import tempfile
from dataclasses import dataclass
from pathlib import Path

from cryptography import x509
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import rsa
from cryptography.x509.oid import NameOID, ObjectIdentifier

REPO = Path(__file__).resolve().parents[1]
DEFAULT_OUT = REPO / "apps" / "ccli" / "config" / "tls" / "ejbca"
CONTAINER = "ccli-ejbca-ce"
EJBCA_SH = "/opt/keyfactor/bin/ejbca.sh"
EE_PASSWORD_DEFAULT = "ccli-lab-ee"
EE_PROFILE_DEFAULT = "CCLI-Lab-EE"
OID_AT_OBJECT_IDENTIFIER = ObjectIdentifier("2.5.4.106")
MMS_OID = "1.1.1.999.1.12"


@dataclass(frozen=True)
class LeafSpec:
    username: str
    stem: str
    kind: str
    cn: str
    cert_profile: str
    ee_profile: str
    san_ip: str | None = None
    g62_oid: str | None = None
    dual_key: bool = False


LEAVES: list[LeafSpec] = [
    LeafSpec(
        "tg544-lab-001",
        "server",
        "tls",
        "CCI016_01",
        "CCLI-Cert-A-TLS",
        EE_PROFILE_DEFAULT,
        san_ip="192.168.10.1",
    ),
    LeafSpec(
        "tg544-lab-001-e2e",
        "server",
        "e2e",
        "CCI016_01",
        "CCLI-Cert-B-E2E-MMS",
        EE_PROFILE_DEFAULT,
        g62_oid=MMS_OID,
        dual_key=True,
    ),
    LeafSpec(
        "tsp-dso-operator",
        "client",
        "tls",
        "DSO_OPERATOR",
        "CCLI-Cert-A-TLS",
        EE_PROFILE_DEFAULT,
    ),
    LeafSpec(
        "tsp-dso-operator-e2e",
        "client",
        "e2e",
        "DSO_OPERATOR",
        "CCLI-Cert-B-E2E-MMS",
        EE_PROFILE_DEFAULT,
        g62_oid=MMS_OID,
    ),
    LeafSpec(
        "tsp-viewer",
        "viewer",
        "tls",
        "VIEWER",
        "CCLI-Cert-A-TLS",
        EE_PROFILE_DEFAULT,
    ),
    LeafSpec(
        "tsp-viewer-e2e",
        "viewer",
        "e2e",
        "VIEWER",
        "CCLI-Cert-B-E2E-MMS",
        EE_PROFILE_DEFAULT,
        g62_oid=MMS_OID,
    ),
    LeafSpec(
        "tsp-revoked",
        "revoked",
        "tls",
        "REVOKED_CLIENT",
        "CCLI-Cert-A-TLS",
        EE_PROFILE_DEFAULT,
    ),
    LeafSpec(
        "tsp-revoked-e2e",
        "revoked",
        "e2e",
        "REVOKED_CLIENT",
        "CCLI-Cert-B-E2E-MMS",
        EE_PROFILE_DEFAULT,
        g62_oid=MMS_OID,
    ),
]


def _run(args: list[str], *, check: bool = True) -> subprocess.CompletedProcess[str]:
    cmd = ["docker", "exec", CONTAINER, EJBCA_SH] + args
    cp = subprocess.run(cmd, capture_output=True, text=True)
    if check and cp.returncode != 0:
        raise RuntimeError(
            f"ejbca.sh {' '.join(args)} failed ({cp.returncode}):\n{cp.stderr or cp.stdout}"
        )
    return cp


def _write_pem(path: Path, data: bytes) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(data)


def _key_pem(key: rsa.RSAPrivateKey) -> bytes:
    return key.private_bytes(
        serialization.Encoding.PEM,
        serialization.PrivateFormat.TraditionalOpenSSL,
        serialization.NoEncryption(),
    )


def _build_csr(
    key: rsa.RSAPrivateKey,
    *,
    cn: str,
    san_ip: str | None,
    g62_oid: str | None,
) -> bytes:
    attrs = [
        x509.NameAttribute(NameOID.COUNTRY_NAME, "IT"),
        x509.NameAttribute(NameOID.ORGANIZATION_NAME, "HiTEKS"),
        x509.NameAttribute(NameOID.ORGANIZATIONAL_UNIT_NAME, "CCLI-Lab"),
    ]
    if g62_oid:
        attrs.append(x509.NameAttribute(OID_AT_OBJECT_IDENTIFIER, g62_oid))
    else:
        attrs.append(x509.NameAttribute(NameOID.COMMON_NAME, cn))
    name = x509.Name(attrs)
    builder = x509.CertificateSigningRequestBuilder().subject_name(name)
    if san_ip:
        builder = builder.add_extension(
            x509.SubjectAlternativeName([x509.IPAddress(ipaddress.ip_address(san_ip))]),
            critical=False,
        )
    csr = builder.sign(key, hashes.SHA256())
    return csr.public_bytes(serialization.Encoding.PEM)


def _ldap_dn(spec: LeafSpec) -> str:
    # EE profile CCLI-Lab-EE only constrains CN. Cert B G.6.2 RDN comes from CSR (allowDNOverride).
    return f"CN={spec.cn}"


def _ensure_end_entity(spec: LeafSpec, *, ca_name: str, ee_password: str) -> None:
    # Remove stale EE so re-runs are idempotent.
    _run(["ra", "delendentity", spec.username, "-force"], check=False)
    args = [
        "ra",
        "addendentity",
        "--username",
        spec.username,
        "--dn",
        _ldap_dn(spec),
        "--caname",
        ca_name,
        "--type",
        "1",
        "--token",
        "USERGENERATED",
        "--certprofile",
        spec.cert_profile,
        "--eeprofile",
        spec.ee_profile,
        "--password",
        ee_password,
    ]
    # SAN comes from CSR (cert profile allowDNOverride); EE profile has no IP SAN field.
    _run(args)


def _issue_cert(spec: LeafSpec, *, csr_pem: bytes, ee_password: str) -> bytes:
    with tempfile.NamedTemporaryFile(suffix=".csr.pem", delete=False) as tf:
        tf.write(csr_pem)
        csr_host = tf.name
    cert_host = csr_host.replace(".csr.pem", ".crt.pem")
    cert_in = "/tmp/ccli-issue.crt.pem"
    csr_in = "/tmp/ccli-issue.csr.pem"
    try:
        subprocess.run(
            ["docker", "cp", csr_host, f"{CONTAINER}:{csr_in}"],
            check=True,
            capture_output=True,
            text=True,
        )
        _run(
            [
                "createcert",
                "--username",
                spec.username,
                "--password",
                ee_password,
                "-c",
                csr_in,
                "-f",
                cert_in,
            ]
        )
        subprocess.run(
            ["docker", "cp", f"{CONTAINER}:{cert_in}", cert_host],
            check=True,
            capture_output=True,
            text=True,
        )
        return Path(cert_host).read_bytes()
    finally:
        Path(csr_host).unlink(missing_ok=True)
        Path(cert_host).unlink(missing_ok=True)
        _run(["sh", "-c", f"rm -f {csr_in} {cert_in}"], check=False)


def _export_root_ca(ca_name: str, out: Path) -> None:
    cert_in = "/tmp/ccli-root-ca.pem"
    _run(["ca", "getcacert", "--caname", ca_name, "-f", cert_in])
    subprocess.run(
        ["docker", "cp", f"{CONTAINER}:{cert_in}", str(out / "root_CA.pem")],
        check=True,
        capture_output=True,
        text=True,
    )


def issue_all(args: argparse.Namespace) -> int:
    out: Path = args.out
    out.mkdir(parents=True, exist_ok=True)
    print(f"EJBCA CLI issue  CA={args.ca_name}  out={out}")

    revoked_serial_hex: str | None = None
    revoked_issuer: str | None = None

    for spec in LEAVES:
        if args.server_only and not spec.stem.startswith("server"):
            continue
        print(f"  {spec.username} ({spec.kind}) profile={spec.cert_profile} ...")
        _ensure_end_entity(spec, ca_name=args.ca_name, ee_password=args.ee_password)
        key = rsa.generate_private_key(public_exponent=65537, key_size=2048)
        csr = _build_csr(key, cn=spec.cn, san_ip=spec.san_ip, g62_oid=spec.g62_oid)
        cert_pem = _issue_cert(spec, csr_pem=csr, ee_password=args.ee_password)

        if spec.kind == "tls":
            _write_pem(out / f"{spec.stem}_tls.pem", cert_pem)
            _write_pem(out / f"{spec.stem}.key", _key_pem(key))
        else:
            _write_pem(out / f"{spec.stem}.pem", cert_pem)
            if spec.dual_key:
                _write_pem(out / f"{spec.stem}_acse.key", _key_pem(key))

        if spec.stem == "revoked" and spec.kind == "tls":
            cert = x509.load_pem_x509_certificate(cert_pem)
            revoked_serial_hex = format(cert.serial_number, "X")
            revoked_issuer = cert.issuer.rfc4514_string()
        print("    -> OK")

    _export_root_ca(args.ca_name, out)

    client_key = out / "client.key"
    if client_key.is_file():
        _write_pem(out / "client_tsp.key", client_key.read_bytes())

    if revoked_serial_hex and revoked_issuer and not args.skip_revoke:
        print(f"  revoke {revoked_serial_hex} ...")
        _run(
            [
                "ra",
                "revokecert",
                "--dn",
                revoked_issuer,
                "-s",
                revoked_serial_hex,
                "-r",
                "5",
            ]
        )
        _run(["ca", "createcrl", "--caname", args.ca_name])

    readme = f"""# Annex-aligned CCLI lab PKI (EJBCA CLI)

Issued by `scripts/issue_ejbca_cli_pki.py` via addendentity + createcert.

| Artefact | Profile |
|----------|---------|
| `root_CA.pem` | {args.ca_name} |
| `server_tls.pem` + `server.key` | CCLI-Cert-A-TLS |
| `server.pem` + `server_acse.key` | CCLI-Cert-B-E2E-MMS |
| `client.pem` / `client_tls.pem` | operator dual |

Verify: `python scripts/verify_annex_pki.py --dir {out}`
"""
    _write_pem(out / "README.md", readme.encode("utf-8"))
    print(f"OK -> {out}")
    return 0


def main() -> int:
    ap = argparse.ArgumentParser(description="Issue CCLI PEMs via EJBCA CLI createcert")
    ap.add_argument("--ca-name", default="CCLI-Lab-CA")
    ap.add_argument("--out", type=Path, default=DEFAULT_OUT)
    ap.add_argument("--ee-password", default=EE_PASSWORD_DEFAULT)
    ap.add_argument("--server-only", action="store_true")
    ap.add_argument("--skip-revoke", action="store_true")
    ap.add_argument("--verify-after", action="store_true")
    ap.add_argument("--strict-g62", action="store_true")
    args = ap.parse_args()

    try:
        rc = issue_all(args)
    except (RuntimeError, subprocess.CalledProcessError) as exc:
        print(f"FAIL: {exc}", file=sys.stderr)
        return 1

    if rc != 0:
        return rc

    if args.verify_after:
        cmd = [
            sys.executable,
            str(REPO / "scripts" / "verify_annex_pki.py"),
            "--dir",
            str(args.out),
        ]
        if args.strict_g62:
            cmd.append("--strict-g62")
        print("\n=== Annex verify ===")
        return subprocess.call(cmd)

    return 0


if __name__ == "__main__":
    sys.exit(main())
