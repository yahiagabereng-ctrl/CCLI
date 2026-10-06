#!/usr/bin/env python3
"""Issue Annex CCLI lab PEMs via EJBCA REST (pkcs10enroll).

Auth is mTLS with an EJBCA admin/RA client certificate (P12 or PEM) — not an API key.
Requires: REST Certificate Management enabled; profiles CCLI-Cert-A-TLS / CCLI-Cert-B-E2E-MMS;
CA CCLI-Lab-CA (or --ca-name).

See lab/EJBCA_LAB_SETUP.md §12.
"""
from __future__ import annotations

import argparse
import base64
import ipaddress
import json
import ssl
import sys
import tempfile
from dataclasses import dataclass
from pathlib import Path

import requests
import urllib3
from cryptography import x509
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import rsa
from cryptography.hazmat.primitives.serialization import pkcs12
from cryptography.x509.oid import NameOID, ObjectIdentifier

REPO = Path(__file__).resolve().parents[1]
DEFAULT_OUT = REPO / "apps" / "ccli" / "config" / "tls" / "ejbca"
DEFAULT_BASE = "https://localhost:8443/ejbca/ejbca-rest-api"

OID_AT_OBJECT_IDENTIFIER = ObjectIdentifier("2.5.4.106")
MMS_OID = "1.1.1.999.1.12"
EE_PASSWORD_DEFAULT = "ccli-lab-ee"


@dataclass(frozen=True)
class LeafSpec:
    username: str
    stem: str  # file stem: server / client / viewer / revoked
    kind: str  # "tls" | "e2e"
    cn: str
    cert_profile: str
    ee_profile: str
    san_ip: str | None = None
    g62_oid: str | None = None
    dual_key: bool = False  # True → separate key file for e2e (server only)


LEAVES: list[LeafSpec] = [
    LeafSpec(
        "tg544-lab-001",
        "server",
        "tls",
        "CCI016_01",
        "CCLI-Cert-A-TLS",
        "CCLI-Server-Profile",
        san_ip="192.168.10.1",
    ),
    LeafSpec(
        "tg544-lab-001-e2e",
        "server",
        "e2e",
        "CCI016_01",
        "CCLI-Cert-B-E2E-MMS",
        "CCLI-Server-Profile",
        g62_oid=MMS_OID,
        dual_key=True,
    ),
    LeafSpec(
        "tsp-dso-operator",
        "client",
        "tls",
        "DSO_OPERATOR",
        "CCLI-Cert-A-TLS",
        "CCLI-Operator-Profile",
    ),
    LeafSpec(
        "tsp-dso-operator-e2e",
        "client",
        "e2e",
        "DSO_OPERATOR",
        "CCLI-Cert-B-E2E-MMS",
        "CCLI-Operator-Profile",
        g62_oid=MMS_OID,
    ),
    LeafSpec(
        "tsp-viewer",
        "viewer",
        "tls",
        "VIEWER",
        "CCLI-Cert-A-TLS",
        "CCLI-Operator-Profile",
    ),
    LeafSpec(
        "tsp-viewer-e2e",
        "viewer",
        "e2e",
        "VIEWER",
        "CCLI-Cert-B-E2E-MMS",
        "CCLI-Operator-Profile",
        g62_oid=MMS_OID,
    ),
    LeafSpec(
        "tsp-revoked",
        "revoked",
        "tls",
        "REVOKED_CLIENT",
        "CCLI-Cert-A-TLS",
        "CCLI-Operator-Profile",
    ),
    LeafSpec(
        "tsp-revoked-e2e",
        "revoked",
        "e2e",
        "REVOKED_CLIENT",
        "CCLI-Cert-B-E2E-MMS",
        "CCLI-Operator-Profile",
        g62_oid=MMS_OID,
    ),
]


def _write_pem(path: Path, data: bytes) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(data)


def _key_pem(key: rsa.RSAPrivateKey) -> bytes:
    return key.private_bytes(
        serialization.Encoding.PEM,
        serialization.PrivateFormat.TraditionalOpenSSL,
        serialization.NoEncryption(),
    )


def _b64_to_cert_pem(b64: str) -> bytes:
    der = base64.b64decode(b64)
    cert = x509.load_der_x509_certificate(der)
    return cert.public_bytes(serialization.Encoding.PEM)


def _extract_admin_pems(p12_path: Path, password: str) -> tuple[Path, Path, tempfile.TemporaryDirectory]:
    """Write admin cert+key PEM to a temp dir; caller must keep TemporaryDirectory alive."""
    tmp = tempfile.TemporaryDirectory(prefix="ccli-ejbca-admin-")
    key, cert, _ = pkcs12.load_key_and_certificates(
        p12_path.read_bytes(), password.encode("utf-8")
    )
    if key is None or cert is None:
        raise ValueError(f"No key/cert in admin P12: {p12_path}")
    cert_pem = Path(tmp.name) / "admin.crt.pem"
    key_pem = Path(tmp.name) / "admin.key.pem"
    cert_pem.write_bytes(cert.public_bytes(serialization.Encoding.PEM))
    key_pem.write_bytes(
        key.private_bytes(
            serialization.Encoding.PEM,
            serialization.PrivateFormat.TraditionalOpenSSL,
            serialization.NoEncryption(),
        )
    )
    return cert_pem, key_pem, tmp


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
        # EJBCA / OpenSSL often store 2.5.4.106 as UTF8String of the OID text (lab TSP).
        attrs.append(x509.NameAttribute(OID_AT_OBJECT_IDENTIFIER, g62_oid))
    else:
        attrs.append(x509.NameAttribute(NameOID.COMMON_NAME, cn))
    name = x509.Name(attrs)
    builder = x509.CertificateSigningRequestBuilder().subject_name(name)
    if san_ip:
        builder = builder.add_extension(
            x509.SubjectAlternativeName(
                [x509.IPAddress(ipaddress.ip_address(san_ip))]
            ),
            critical=False,
        )
    csr = builder.sign(key, hashes.SHA256())
    return csr.public_bytes(serialization.Encoding.PEM)


class EjbcaRest:
    def __init__(
        self,
        base_url: str,
        cert_pem: Path,
        key_pem: Path,
        *,
        verify_tls: bool | str,
        ee_password: str,
    ) -> None:
        self.base = base_url.rstrip("/")
        self.session = requests.Session()
        self.session.cert = (str(cert_pem), str(key_pem))
        self.session.verify = verify_tls
        self.session.headers.update(
            {"Content-Type": "application/json", "Accept": "application/json"}
        )
        self.ee_password = ee_password
        if verify_tls is False:
            urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)

    def get(self, path: str) -> requests.Response:
        return self.session.get(f"{self.base}{path}", timeout=60)

    def post(self, path: str, body: dict) -> requests.Response:
        return self.session.post(f"{self.base}{path}", data=json.dumps(body), timeout=120)

    def ping(self) -> None:
        # OpenAPI / status varies by version; try certificate endpoint OPTIONS or ca list.
        r = self.get("/v1/ca")
        if r.status_code in (200, 401, 403):
            if r.status_code in (401, 403):
                raise RuntimeError(
                    f"REST auth failed HTTP {r.status_code}: {r.text[:400]}\n"
                    "Use a SuperAdmin/RA P12 with REST privileges; enable REST Certificate Management."
                )
            return
        # Some builds need trailing slash or different path
        r2 = self.get("/v1/certificate/status")
        if r2.status_code >= 400 and r.status_code >= 400:
            raise RuntimeError(
                f"REST not reachable ({r.status_code} / {r2.status_code}). "
                "Enable Protocol Configuration → REST Certificate Management. "
                f"Body: {r.text[:300]}"
            )

    def pkcs10enroll(
        self,
        *,
        csr_pem: bytes,
        username: str,
        cert_profile: str,
        ee_profile: str,
        ca_name: str,
    ) -> dict:
        body = {
            "certificate_request": csr_pem.decode("ascii"),
            "certificate_profile_name": cert_profile,
            "end_entity_profile_name": ee_profile,
            "certificate_authority_name": ca_name,
            "username": username,
            "password": self.ee_password,
            "include_chain": True,
        }
        r = self.post("/v1/certificate/pkcs10enroll", body)
        if r.status_code >= 400:
            raise RuntimeError(
                f"pkcs10enroll failed for {username}: HTTP {r.status_code}\n{r.text[:800]}"
            )
        return r.json()

    def ca_certificate_pem(self, ca_name: str) -> bytes | None:
        r = self.get("/v1/ca")
        if r.status_code != 200:
            return None
        data = r.json()
        cas = data.get("certificate_authorities") or data.get("cas") or data
        if not isinstance(cas, list):
            return None
        for ca in cas:
            name = ca.get("name") or ca.get("id") or ""
            if name != ca_name:
                continue
            for key in ("certificate", "ca_certificate", "pem"):
                if key in ca and ca[key]:
                    val = ca[key]
                    if isinstance(val, str) and "BEGIN CERTIFICATE" in val:
                        return val.encode("ascii")
                    if isinstance(val, str):
                        return _b64_to_cert_pem(val)
        return None

    def revoke(self, issuer_dn: str, serial_hex: str) -> None:
        body = {
            "issuer_dn": issuer_dn,
            "certificate_serial_number": serial_hex,
            "reason": "CESSATION_OF_OPERATION",
        }
        r = self.post("/v1/certificate/revoke", body)
        if r.status_code >= 400:
            print(f"  WARN revoke {serial_hex}: HTTP {r.status_code} {r.text[:200]}", file=sys.stderr)


def _response_cert_pem(resp: dict) -> bytes:
    cert_field = resp.get("certificate")
    if not cert_field:
        raise RuntimeError(f"No certificate in response keys={list(resp)}")
    if isinstance(cert_field, str) and "BEGIN CERTIFICATE" in cert_field:
        return cert_field.encode("ascii")
    return _b64_to_cert_pem(cert_field)


def _response_chain_root_pem(resp: dict) -> bytes | None:
    chain = resp.get("certificate_chain") or resp.get("chain")
    if not chain:
        return None
    # Last or first may be root depending on version — pick self-signed if possible.
    pems: list[bytes] = []
    for item in chain:
        if isinstance(item, str) and "BEGIN CERTIFICATE" in item:
            pems.append(item.encode("ascii"))
        elif isinstance(item, str):
            pems.append(_b64_to_cert_pem(item))
    for pem in reversed(pems):
        c = x509.load_pem_x509_certificate(pem)
        if c.subject == c.issuer:
            return pem
    return pems[-1] if pems else None


def issue_all(args: argparse.Namespace) -> int:
    out: Path = args.out
    out.mkdir(parents=True, exist_ok=True)

    keep_tmp = None
    if args.admin_p12:
        cert_pem, key_pem, keep_tmp = _extract_admin_pems(args.admin_p12, args.admin_p12_password)
    else:
        if not args.admin_cert or not args.admin_key:
            print("Need --admin-p12 or --admin-cert + --admin-key", file=sys.stderr)
            return 2
        cert_pem, key_pem = args.admin_cert, args.admin_key

    verify: bool | str = False if args.insecure else (str(args.ca_bundle) if args.ca_bundle else True)
    api = EjbcaRest(
        args.base_url,
        cert_pem,
        key_pem,
        verify_tls=verify,
        ee_password=args.ee_password,
    )
    print(f"EJBCA REST: {args.base_url}")
    print(f"CA={args.ca_name}  out={out}")
    api.ping()
    print("  REST auth OK")

    root_pem = api.ca_certificate_pem(args.ca_name)
    revoked_serial_hex: str | None = None
    revoked_issuer: str | None = None

    for spec in LEAVES:
        if args.server_only and not spec.stem.startswith("server"):
            continue
        key = rsa.generate_private_key(public_exponent=65537, key_size=2048)
        csr = _build_csr(
            key, cn=spec.cn, san_ip=spec.san_ip, g62_oid=spec.g62_oid
        )
        print(f"  enroll {spec.username} ({spec.kind}) profile={spec.cert_profile} ...")
        resp = api.pkcs10enroll(
            csr_pem=csr,
            username=spec.username,
            cert_profile=spec.cert_profile,
            ee_profile=spec.ee_profile,
            ca_name=args.ca_name,
        )
        cert_pem_bytes = _response_cert_pem(resp)
        if root_pem is None:
            root_pem = _response_chain_root_pem(resp)

        if spec.kind == "tls":
            _write_pem(out / f"{spec.stem}_tls.pem", cert_pem_bytes)
            _write_pem(out / f"{spec.stem}.key", _key_pem(key))
        else:
            _write_pem(out / f"{spec.stem}.pem", cert_pem_bytes)
            if spec.dual_key:
                _write_pem(out / f"{spec.stem}_acse.key", _key_pem(key))
            # Operator: TLS key stays in client.key; e2e cert in client.pem (TSP MMS).

        if spec.stem == "revoked" and spec.kind == "tls":
            cert = x509.load_pem_x509_certificate(cert_pem_bytes)
            revoked_serial_hex = format(cert.serial_number, "X")
            revoked_issuer = cert.issuer.rfc4514_string()

        print(f"    -> OK serial={resp.get('serial_number', '?')}")

    if root_pem:
        _write_pem(out / "root_CA.pem", root_pem)
    elif args.root_ca_pem and Path(args.root_ca_pem).is_file():
        _write_pem(out / "root_CA.pem", Path(args.root_ca_pem).read_bytes())
    else:
        print("WARN: root_CA.pem not obtained from REST; copy CA PEM manually", file=sys.stderr)

    # TSP OpenSSL key alias
    client_key = out / "client.key"
    if client_key.is_file():
        _write_pem(out / "client_tsp.key", client_key.read_bytes())

    if revoked_serial_hex and revoked_issuer and not args.skip_revoke:
        print(f"  revoke {revoked_serial_hex} ...")
        api.revoke(revoked_issuer, revoked_serial_hex)

    readme = f"""# Annex-aligned CCLI lab PKI (EJBCA REST)

Issued by `scripts/issue_ejbca_rest_pki.py` via pkcs10enroll.

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

    if keep_tmp is not None:
        keep_tmp.cleanup()
    return 0


def main() -> int:
    ap = argparse.ArgumentParser(description="Issue CCLI PEMs via EJBCA REST pkcs10enroll")
    ap.add_argument("--base-url", default=DEFAULT_BASE)
    ap.add_argument("--ca-name", default="CCLI-Lab-CA")
    ap.add_argument("--out", type=Path, default=DEFAULT_OUT)
    ap.add_argument("--admin-p12", type=Path, default=None, help="Admin/RA PKCS#12 for mTLS")
    ap.add_argument("--admin-p12-password", default="foo123")
    ap.add_argument("--admin-cert", type=Path, default=None)
    ap.add_argument("--admin-key", type=Path, default=None)
    ap.add_argument("--ca-bundle", type=Path, default=None, help="Trust store for HTTPS")
    ap.add_argument("--insecure", action="store_true", help="Skip TLS verify (lab only)")
    ap.add_argument("--ee-password", default=EE_PASSWORD_DEFAULT)
    ap.add_argument("--root-ca-pem", type=Path, default=None, help="Fallback root_CA.pem")
    ap.add_argument("--server-only", action="store_true")
    ap.add_argument("--skip-revoke", action="store_true")
    ap.add_argument(
        "--verify-after",
        action="store_true",
        help="Run verify_annex_pki.py on --out after issue",
    )
    ap.add_argument("--strict-g62", action="store_true")
    args = ap.parse_args()

    try:
        rc = issue_all(args)
    except requests.RequestException as exc:
        print(f"FAIL REST transport: {exc}", file=sys.stderr)
        return 1
    except (RuntimeError, ValueError, ssl.SSLError) as exc:
        print(f"FAIL: {exc}", file=sys.stderr)
        return 1

    if rc != 0:
        return rc

    if args.verify_after:
        import subprocess

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
