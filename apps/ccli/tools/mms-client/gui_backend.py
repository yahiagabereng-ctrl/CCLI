#!/usr/bin/env python3
"""JSON-lines bridge between the Windows desktop shell and the MMS client."""
import contextlib
import datetime as dt
import getpass
import importlib.util
import io
import json
import sys
from pathlib import Path
from types import SimpleNamespace

HERE = Path(__file__).resolve().parent
spec = importlib.util.spec_from_file_location("ccli_mms_client", HERE / "client.py")
client = importlib.util.module_from_spec(spec)
sys.modules[spec.name] = client
spec.loader.exec_module(client)


def emit(value):
    print(json.dumps(value, ensure_ascii=False), flush=True)


def certificate_details(path):
    from cryptography import x509
    from cryptography.hazmat.primitives import hashes
    cert = x509.load_pem_x509_certificate(Path(path).read_bytes())
    return {
        "path": str(path), "subject": cert.subject.rfc4514_string(),
        "issuer": cert.issuer.rfc4514_string(), "serial": format(cert.serial_number, "X"),
        "not_before": cert.not_valid_before_utc.isoformat(),
        "not_after": cert.not_valid_after_utc.isoformat(),
        "sha256": cert.fingerprint(hashes.SHA256()).hex(":"),
        "roles": identity_text(cert),
    }


def identity_text(cert):
    from contextlib import redirect_stdout
    stream = io.StringIO()
    with redirect_stdout(stream):
        client.identity(cert)
    return stream.getvalue().strip()


def connect(config):
    from cryptography import x509
    from cryptography.hazmat.primitives import serialization
    paths = {key: Path(config[key]) for key in
             ("identity_cert", "identity_key", "ca", "tls_cert", "tls_key", "binary")}
    for label, path in paths.items():
        if not path.is_file():
            raise FileNotFoundError(f"{label} file does not exist: {path}")
    cert = x509.load_pem_x509_certificate(paths["identity_cert"].read_bytes())
    tls_cert = x509.load_pem_x509_certificate(paths["tls_cert"].read_bytes())
    password = config.get("key_passphrase") or None
    key_data = paths["identity_key"].read_bytes()
    try:
        key = serialization.load_pem_private_key(key_data, password=password.encode() if password else None)
    except TypeError as exc:
        raise ValueError("Application key is encrypted; enter its passphrase in the Configuration tab and reconnect.") from exc
    public = serialization.Encoding.DER
    fmt = serialization.PublicFormat.SubjectPublicKeyInfo
    if key.public_key().public_bytes(public, fmt) != cert.public_key().public_bytes(public, fmt):
        raise ValueError("Application identity certificate and private key do not match")
    args = SimpleNamespace(
        host=config["host"], port=int(config["port"]), ca=paths["ca"], tls_cert=paths["tls_cert"],
        tls_key=paths["tls_key"], identity_cert=paths["identity_cert"], identity_key=paths["identity_key"],
        auth_mode=config.get("auth_mode", "certificate"), ap_title=config.get("ap_title", "1.1.1.999.1"),
        ae_qualifier=int(config.get("ae_qualifier", 12)), binary=paths["binary"], verbose=False,
        report=None, command="menu", reference="", fc="MX", count=1, type="int32", value=None,
        members=None, yes=True, mms_ca=Path(config["mms_ca"]) if config.get("mms_ca") else paths["ca"],
    )
    diagnostics = io.StringIO()
    args.verbose = True
    try:
        with contextlib.redirect_stdout(diagnostics), contextlib.redirect_stderr(diagnostics):
            session = client.PersistentSession(args, cert, key)
    except Exception as exc:
        exc.diagnostics = diagnostics.getvalue().strip()
        raise
    args.verbose = False
    session.args.verbose = False
    return session, cert, tls_cert, args


def main():
    session = None
    for raw in sys.stdin:
        message = {}
        try:
            message = json.loads(raw)
            action = message.get("action")
            if action == "connect":
                if session:
                    session.close()
                session, app_cert, tls_cert, base = connect(message["config"])
                try:
                    application_certificate = certificate_details(message["config"]["identity_cert"])
                    tls_certificate = certificate_details(message["config"]["tls_cert"])
                except Exception:
                    session.close()
                    session = None
                    raise
                emit({"type": "connected", "status": session.status(),
                      "application_certificate": application_certificate,
                      "tls_certificate": tls_certificate,
                      "server_certificate": getattr(session, "peer_certificate", None),
                      "wire_profile": session.wire_profile,
                      "auth_mode": message["config"].get("auth_mode", "certificate"),
                      "transport": f"{base.host}:{base.port}"})
            elif action == "request":
                if not session:
                    raise RuntimeError("Not connected")
                command = message["command"]
                args = SimpleNamespace(command=command, host=base.host, port=base.port,
                    reference=message.get("reference", ""),
                    fc=message.get("fc", "MX"), count=int(message.get("count", 1)),
                    type=message.get("value_type", "int32"), value=message.get("value"),
                    members=message.get("members"), yes=True)
                client.validate(args)
                result, events = session.execute(args, quiet=True)
                emit({"type": "result", "id": message.get("id"), "code": result,
                      "status": session.status(), "events": events})
            elif action == "disconnect":
                if session:
                    session.close()
                    session = None
                emit({"type": "disconnected"})
                break
            else:
                raise ValueError(f"Unknown action: {action}")
        except Exception as exc:
            emit({"type": "error", "id": message.get("id") if "message" in locals() else None,
                  "error": str(exc), "diagnostics": getattr(exc, "diagnostics", ""),
                  "status": session.status() if session else "DISCONNECTED"})
    if session:
        session.close()


if __name__ == "__main__":
    main()
