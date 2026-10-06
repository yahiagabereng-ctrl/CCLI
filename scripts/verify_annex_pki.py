#!/usr/bin/env python3
"""Verify CCLI lab PKI PEMs against Annex / 62351 rules (EJBCA_LAB_SETUP.md §10).

Checks issued artefacts (EJBCA export or hybrid Python) against the same rules
that must be configured in EJBCA certificate / end-entity profiles.

Exit 0 = all gates pass; non-zero = fail (do not deploy).
"""
from __future__ import annotations

import argparse
import hashlib
import sys
from dataclasses import dataclass, field
from pathlib import Path

from cryptography import x509
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import padding, rsa
from cryptography.x509.oid import ExtendedKeyUsageOID, ExtensionOID, NameOID

REPO = Path(__file__).resolve().parents[1]
DEFAULT_DIR = REPO / "apps" / "ccli" / "config" / "tls" / "ejbca"

# --- Issuance rules (must match EJBCA profiles in lab/EJBCA_LAB_SETUP.md) ---
CA_CN = "CCLI Lab CA"
MMS_OID = "1.1.1.999.1.12"
OID_AT_OBJECT_IDENTIFIER_DER = b"\x55\x04\x6a"  # 2.5.4.106
SERVER_TLS_CN = "CCI016_01"
SERVER_SAN_IP = "192.168.10.1"
OPERATOR_CN = "DSO_OPERATOR"
MAX_CERT_OCTETS = 8192
MIN_RSA_BITS = 2048
SHA256_WITH_RSA_OID = "1.2.840.113549.1.1.11"


@dataclass
class GateResult:
    gate: str
    clause: str
    ok: bool
    detail: str


@dataclass
class Report:
    results: list[GateResult] = field(default_factory=list)

    def add(self, gate: str, clause: str, ok: bool, detail: str) -> None:
        self.results.append(GateResult(gate, clause, ok, detail))

    @property
    def failures(self) -> int:
        return sum(1 for r in self.results if not r.ok)


def _load_cert(path: Path) -> x509.Certificate:
    return x509.load_pem_x509_certificate(path.read_bytes())


def _require_file(path: Path, report: Report, gate: str, clause: str) -> bool:
    if path.is_file():
        return True
    report.add(gate, clause, False, f"missing file {path.name}")
    return False


def _cn(cert: x509.Certificate) -> str | None:
    attrs = cert.subject.get_attributes_for_oid(NameOID.COMMON_NAME)
    return attrs[0].value if attrs else None


def _issuer_cn(cert: x509.Certificate) -> str | None:
    attrs = cert.issuer.get_attributes_for_oid(NameOID.COMMON_NAME)
    return attrs[0].value if attrs else None


def _is_rsa2048(cert: x509.Certificate) -> tuple[bool, str]:
    pub = cert.public_key()
    if not isinstance(pub, rsa.RSAPublicKey):
        return False, f"public key is {type(pub).__name__}, want RSA"
    bits = pub.key_size
    return bits >= MIN_RSA_BITS, f"RSA-{bits}"


def _sig_sha256_rsa(cert: x509.Certificate) -> tuple[bool, str]:
    oid = cert.signature_algorithm_oid.dotted_string
    ok = oid == SHA256_WITH_RSA_OID
    return ok, f"sig OID {oid}"


def _ku(cert: x509.Certificate) -> x509.KeyUsage | None:
    try:
        return cert.extensions.get_extension_for_oid(ExtensionOID.KEY_USAGE).value
    except x509.ExtensionNotFound:
        return None


def _eku(cert: x509.Certificate) -> list | None:
    try:
        return list(cert.extensions.get_extension_for_oid(ExtensionOID.EXTENDED_KEY_USAGE).value)
    except x509.ExtensionNotFound:
        return None


def _san_ips(cert: x509.Certificate) -> list[str]:
    try:
        san = cert.extensions.get_extension_for_oid(ExtensionOID.SUBJECT_ALTERNATIVE_NAME).value
    except x509.ExtensionNotFound:
        return []
    return [str(ip) for ip in san.get_values_for_type(x509.IPAddress)]


def _der_size(cert: x509.Certificate) -> int:
    return len(cert.public_bytes(serialization.Encoding.DER))


def _decode_ber_oid(oid_bytes: bytes) -> str:
    arcs: list[int] = []
    if not oid_bytes:
        return ""
    first = oid_bytes[0]
    arcs.extend([first // 40, first % 40])
    n = 0
    for b in oid_bytes[1:]:
        n = (n << 7) | (b & 0x7F)
        if not (b & 0x80):
            arcs.append(n)
            n = 0
    return ".".join(str(a) for a in arcs)


def _g62_subject(
    der: bytes, expected_oid: str, *, strict: bool
) -> tuple[bool, str]:
    """62351-4 G.6.2 subject RDN 2.5.4.106.

    Annex wants OBJECT IDENTIFIER (tag 0x06). Lab/TSP PEMs from gen_lab_pki use
    UTF8String (0x0C) with the OID text — accepted unless ``strict``.
    """
    marker = der.find(OID_AT_OBJECT_IDENTIFIER_DER)
    if marker < 0:
        return False, "subject missing id-at-objectIdentifier (2.5.4.106)"
    val_tag = der[marker + 3]
    length = der[marker + 4]
    value = der[marker + 5 : marker + 5 + length]
    if val_tag == 0x06:
        got = _decode_ber_oid(value)
        if got != expected_oid:
            return False, f"OID value {got} != {expected_oid}"
        return True, f"OBJECT IDENTIFIER {got} (tag 0x06)"
    if val_tag == 0x0C:
        text = value.decode("utf-8", errors="replace")
        if text != expected_oid:
            return False, f"UTF8String {text!r} != {expected_oid}"
        if strict:
            return False, (
                f"UTF8String {text} (tag 0x0C) — Annex G.6.2 requires OBJECT IDENTIFIER "
                "tag 0x06; fix EJBCA Cert B DN encoding (or drop --strict-g62 for lab UTF8)"
            )
        return True, f"UTF8String {text} (tag 0x0C) — lab/TSP encoding; use --strict-g62 for Annex OID"
    return False, f"subject value tag 0x{val_tag:02x}, want 0x06 (or 0x0C lab)"


def _file_sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def print_rules() -> None:
    print(
        """
EJBCA issuance rules (must be set before issue) — lab/EJBCA_LAB_SETUP.md §§2–5 / §10
==================================================================================
CA  CCLI-Lab-CA
    RSA-2048, SHA256WithRSA, CN=CCLI Lab CA, O=HiTEKS, OU=CCLI-Lab, C=IT

Cert profile CCLI-Cert-A-TLS (62351-3 / 62351-9 §7.4.4.10)
    KU: digitalSignature + keyEncipherment
    EKU: serverAuth + clientAuth
    EE: CN=CCI016_01 (server) / DSO_OPERATOR (client); SAN IP 192.168.10.1 on server

Cert profile CCLI-Cert-B-E2E-MMS (62351-4 G.5.2 / G.6.2)
    KU: digitalSignature + keyAgreement; NO TLS EKU
    Subject: 2.5.4.106 = 1.1.1.999.1.12
      Annex/product: OBJECT IDENTIFIER (ASN.1 tag 0x06)  → verify with --strict-g62
      Lab/TSP today: UTF8String of OID text (tag 0x0C)   → default verify accepts

Policy
    Issue only from CCLI-Lab-CA (never Management CA)
    G.2: separate Cert A / Cert B keys and serials
    T.3.3.4.2: each EE cert DER < 8192 octets
""".strip()
    )


def verify_dir(
    tls_dir: Path,
    *,
    strict_g62: bool = False,
    check_clients: bool = True,
) -> Report:
    report = Report()
    root_p = tls_dir / "root_CA.pem"
    a_p = tls_dir / "server_tls.pem"
    b_p = tls_dir / "server.pem"
    key_a = tls_dir / "server.key"
    key_b = tls_dir / "server_acse.key"
    client_b = tls_dir / "client.pem"
    client_a = tls_dir / "client_tls.pem"

    for p, gate in (
        (root_p, "files"),
        (a_p, "files"),
        (b_p, "files"),
        (key_a, "files"),
        (key_b, "files"),
    ):
        if not _require_file(p, report, gate, "artefacts present"):
            return report

    root = _load_cert(root_p)
    cert_a = _load_cert(a_p)
    cert_b = _load_cert(b_p)

    # --- CA / T.3.3.4.1 / G.3 ---
    ok, detail = _is_rsa2048(root)
    report.add("CA-RSA", "T.3.3.4.1", ok, detail)
    ok, detail = _sig_sha256_rsa(root)
    report.add("CA-SIG", "T.3.3.4.1", ok, detail)
    cn = _cn(root)
    report.add("CA-CN", "G.3 / lab", cn == CA_CN, f"CN={cn!r}")
    try:
        bc = root.extensions.get_extension_for_oid(ExtensionOID.BASIC_CONSTRAINTS).value
        report.add("CA-BC", "G.3", bc.ca is True, f"ca={bc.ca}")
    except x509.ExtensionNotFound:
        report.add("CA-BC", "G.3", False, "BasicConstraints missing")

    # --- Gate 1 issuer ---
    for label, cert in (("CertA", cert_a), ("CertB", cert_b)):
        icn = _issuer_cn(cert)
        ok = icn == CA_CN and "Management" not in (icn or "")
        report.add(f"issuer-{label}", "§10.4 Gate1", ok, f"issuer CN={icn!r}")

    # --- Gate 2 dual cert / keys ---
    report.add(
        "dual-serial",
        "62351-4 G.2",
        cert_a.serial_number != cert_b.serial_number,
        f"A={cert_a.serial_number:x} B={cert_b.serial_number:x}",
    )
    report.add(
        "dual-key",
        "62351-4 G.2",
        _file_sha256(key_a) != _file_sha256(key_b),
        "server.key vs server_acse.key",
    )

    # --- Cert A crypto ---
    ok, detail = _is_rsa2048(cert_a)
    report.add("CertA-RSA", "T.3.3.4.1 / G.4", ok, detail)
    ok, detail = _sig_sha256_rsa(cert_a)
    report.add("CertA-SIG", "T.3.3.4.1 / G.4", ok, detail)

    ku_a = _ku(cert_a)
    if ku_a is None:
        report.add("CertA-KU", "62351-3 / 62351-9", False, "KeyUsage missing")
    else:
        report.add(
            "CertA-KU",
            "62351-3 / 62351-9",
            ku_a.digital_signature and ku_a.key_encipherment,
            f"digitalSignature={ku_a.digital_signature} keyEncipherment={ku_a.key_encipherment}",
        )

    eku_a = _eku(cert_a)
    if eku_a is None:
        report.add("CertA-EKU", "62351-9 §7.4.4.10", False, "EKU missing")
    else:
        has_sa = ExtendedKeyUsageOID.SERVER_AUTH in eku_a
        has_ca = ExtendedKeyUsageOID.CLIENT_AUTH in eku_a
        report.add(
            "CertA-EKU",
            "62351-9 §7.4.4.10",
            has_sa and has_ca,
            f"serverAuth={has_sa} clientAuth={has_ca}",
        )

    cn_a = _cn(cert_a)
    report.add("CertA-CN", "62351-4 G.2", cn_a == SERVER_TLS_CN, f"CN={cn_a!r}")
    ips = _san_ips(cert_a)
    report.add(
        "CertA-SAN",
        "62351-4 G.2",
        SERVER_SAN_IP in ips,
        f"SAN IPs={ips}",
    )
    sz_a = _der_size(cert_a)
    report.add(
        "CertA-SIZE",
        "T.3.3.4.2",
        sz_a < MAX_CERT_OCTETS,
        f"{sz_a} octets",
    )

    # --- Cert B crypto / G.5.2 / G.6.2 ---
    ok, detail = _is_rsa2048(cert_b)
    report.add("CertB-RSA", "T.3.3.4.1 / G.4", ok, detail)
    ok, detail = _sig_sha256_rsa(cert_b)
    report.add("CertB-SIG", "T.3.3.4.1 / G.4", ok, detail)

    ku_b = _ku(cert_b)
    if ku_b is None:
        report.add("CertB-KU", "62351-4 G.5.2", False, "KeyUsage missing")
    else:
        ok_ku = (
            ku_b.digital_signature
            and ku_b.key_agreement
            and not ku_b.key_encipherment
            and not ku_b.key_cert_sign
        )
        report.add(
            "CertB-KU",
            "62351-4 G.5.2",
            ok_ku,
            (
                f"digitalSignature={ku_b.digital_signature} "
                f"keyAgreement={ku_b.key_agreement} "
                f"keyEncipherment={ku_b.key_encipherment}"
            ),
        )

    eku_b = _eku(cert_b)
    if eku_b is None:
        report.add("CertB-noTLS-EKU", "62351-4 G.5.2", True, "no EKU extension")
    else:
        tls_eku = (
            ExtendedKeyUsageOID.SERVER_AUTH in eku_b
            or ExtendedKeyUsageOID.CLIENT_AUTH in eku_b
        )
        report.add(
            "CertB-noTLS-EKU",
            "62351-4 G.5.2",
            not tls_eku,
            f"EKU={eku_b}",
        )

    der_b = cert_b.public_bytes(serialization.Encoding.DER)
    ok_g62, detail_g62 = _g62_subject(der_b, MMS_OID, strict=strict_g62)
    report.add("CertB-G62", "62351-4 G.6.2", ok_g62, detail_g62)

    sz_b = _der_size(cert_b)
    report.add(
        "CertB-SIZE",
        "T.3.3.4.2",
        sz_b < MAX_CERT_OCTETS,
        f"{sz_b} octets",
    )

    # --- Chain verify ---
    try:
        root.public_key().verify(
            cert_a.signature,
            cert_a.tbs_certificate_bytes,
            padding.PKCS1v15(),
            hashes.SHA256(),
        )
        report.add("chain-CertA", "§10.4 Gate6", True, "signature OK vs root_CA")
    except Exception as exc:  # noqa: BLE001 — report gate failure
        report.add("chain-CertA", "§10.4 Gate6", False, str(exc))
    try:
        root.public_key().verify(
            cert_b.signature,
            cert_b.tbs_certificate_bytes,
            padding.PKCS1v15(),
            hashes.SHA256(),
        )
        report.add("chain-CertB", "§10.4 Gate6", True, "signature OK vs root_CA")
    except Exception as exc:  # noqa: BLE001
        report.add("chain-CertB", "§10.4 Gate6", False, str(exc))

    # --- Operator dual (optional if missing) ---
    if check_clients:
        if client_a.is_file() and client_b.is_file():
            ca = _load_cert(client_a)
            cb = _load_cert(client_b)
            report.add(
                "client-dual-serial",
                "62351-4 G.2",
                ca.serial_number != cb.serial_number,
                f"tls={ca.serial_number:x} mms={cb.serial_number:x}",
            )
            report.add(
                "client-tls-CN",
                "lab operator",
                _cn(ca) == OPERATOR_CN,
                f"CN={_cn(ca)!r}",
            )
            eku_c = _eku(ca)
            if eku_c is None:
                report.add("client-tls-EKU", "62351-9 §7.4.4.10", False, "EKU missing")
            else:
                report.add(
                    "client-tls-EKU",
                    "62351-9 §7.4.4.10",
                    ExtendedKeyUsageOID.CLIENT_AUTH in eku_c,
                    f"clientAuth={ExtendedKeyUsageOID.CLIENT_AUTH in eku_c}",
                )
            ok_cg, detail_cg = _g62_subject(
                cb.public_bytes(serialization.Encoding.DER), MMS_OID, strict=strict_g62
            )
            report.add("client-mms-G62", "62351-4 G.6.2", ok_cg, detail_cg)
        else:
            report.add(
                "client-files",
                "§10.2 TSP operator",
                False,
                "client.pem / client_tls.pem missing",
            )

    return report


def main() -> int:
    ap = argparse.ArgumentParser(
        description="Verify Annex §10 PKI rules against issued PEMs (EJBCA or hybrid)"
    )
    ap.add_argument(
        "--dir",
        type=Path,
        default=DEFAULT_DIR,
        help=f"PEM directory (default: {DEFAULT_DIR})",
    )
    ap.add_argument(
        "--strict-g62",
        action="store_true",
        help="Require G.6.2 subject as OBJECT IDENTIFIER tag 0x06 (Annex); default also accepts lab UTF8String",
    )
    ap.add_argument(
        "--skip-clients",
        action="store_true",
        help="Do not require operator client.pem / client_tls.pem",
    )
    ap.add_argument(
        "--show-rules",
        action="store_true",
        help="Print EJBCA profile rules and exit",
    )
    args = ap.parse_args()

    if args.show_rules:
        print_rules()
        return 0

    tls_dir = args.dir.resolve()
    if not tls_dir.is_dir():
        print(f"FAIL: directory not found: {tls_dir}", file=sys.stderr)
        return 2

    print(f"Annex PKI verify: {tls_dir}")
    mode = "strict G.6.2 (OID tag 0x06)" if args.strict_g62 else "lab G.6.2 (OID or UTF8)"
    print(f"Rules: lab/EJBCA_LAB_SETUP.md §10  |  mode={mode}")
    print("  python scripts/verify_annex_pki.py --show-rules")
    print()

    report = verify_dir(
        tls_dir,
        strict_g62=args.strict_g62,
        check_clients=not args.skip_clients,
    )

    width = max(len(r.gate) for r in report.results) if report.results else 8
    for r in report.results:
        status = "PASS" if r.ok else "FAIL"
        colour_ok = r.ok
        line = f"  {status}  {r.gate:<{width}}  [{r.clause}]  {r.detail}"
        print(line)
        if not colour_ok:
            pass

    print()
    if report.failures:
        print(
            f"FAIL — {report.failures} gate(s) failed. "
            "Fix EJBCA profiles / DN encoding, re-issue, re-run. "
            "See lab/EJBCA_LAB_SETUP.md §10.",
            file=sys.stderr,
        )
        return 1

    print("OK — all Annex §10 lab gates passed (safe to stage/deploy).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
