#!/usr/bin/env python3
"""Local, ephemeral PKI and TLS collector: no device or production keys used."""
import hashlib
import json
import pathlib
import socket
import ssl
import subprocess
import sys
import tempfile
import threading
import time


def main(probe):
    with tempfile.TemporaryDirectory(prefix="ccli-syslog-tls-") as folder:
        root = pathlib.Path(folder)

        def openssl(*args):
            subprocess.run(["openssl", *map(str, args)], cwd=root, check=True,
                           stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)

        (root / "newcerts").mkdir()
        (root / "index").touch()
        (root / "serial").write_text("1000\n")
        (root / "crlnumber").write_text("1000\n")
        (root / "ca.cnf").write_text("""[ca]
default_ca=local
[local]
database=index
new_certs_dir=newcerts
serial=serial
crlnumber=crlnumber
certificate=ca.pem
private_key=ca.key
default_md=sha256
default_days=2
default_crl_days=1
policy=names
[names]
commonName=supplied
[client]
basicConstraints=critical,CA:FALSE
keyUsage=critical,digitalSignature,keyEncipherment
extendedKeyUsage=clientAuth
subjectKeyIdentifier=hash
authorityKeyIdentifier=keyid
[server]
basicConstraints=critical,CA:FALSE
keyUsage=critical,digitalSignature,keyEncipherment
extendedKeyUsage=serverAuth
subjectAltName=IP:127.0.0.1
subjectKeyIdentifier=hash
authorityKeyIdentifier=keyid
""")
        openssl("req", "-x509", "-newkey", "rsa:2048", "-nodes", "-sha256", "-days", "3",
                "-subj", "/CN=CCLI logger test CA", "-keyout", "ca.key", "-out", "ca.pem",
                "-addext", "basicConstraints=critical,CA:TRUE",
                "-addext", "keyUsage=critical,keyCertSign,cRLSign")
        for name in ("client", "server"):
            openssl("req", "-new", "-newkey", "rsa:2048", "-nodes", "-subj", f"/CN={name}",
                    "-keyout", f"{name}.key", "-out", f"{name}.csr")
            openssl("ca", "-batch", "-notext", "-config", "ca.cnf", "-extensions", name,
                    "-in", f"{name}.csr", "-out", f"{name}.pem")
        openssl("ca", "-gencrl", "-config", "ca.cnf", "-out", "current.crl")
        openssl("ca", "-gencrl", "-config", "ca.cnf", "-crl_lastupdate", "20200101000000Z",
                "-crl_nextupdate", "20200102000000Z", "-out", "expired.crl")
        openssl("req", "-x509", "-newkey", "rsa:2048", "-nodes", "-sha256", "-days", "1",
                "-subj", "/CN=untrusted collector", "-keyout", "untrusted.key", "-out", "untrusted.pem",
                "-addext", "extendedKeyUsage=serverAuth")
        pin = hashlib.sha256(ssl.PEM_cert_to_DER_cert((root / "server.pem").read_text())).hexdigest()
        results = []

        def run(name, *, bad_pin=False, cert="server", crl="current.crl", stall=False, fail=False):
            listener = socket.socket()
            listener.bind(("127.0.0.1", 0))
            listener.listen(4)
            listener.settimeout(6)
            port = listener.getsockname()[1]
            received = []
            errors = []

            def collect():
                try:
                    connection, _ = listener.accept()
                    with connection:
                        connection.settimeout(6)
                        if stall:
                            time.sleep(5)
                            return
                        context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
                        context.minimum_version = context.maximum_version = ssl.TLSVersion.TLSv1_2
                        context.set_ciphers("ECDHE-RSA-AES128-GCM-SHA256")
                        context.load_cert_chain(root / f"{cert}.pem", root / f"{cert}.key")
                        context.load_verify_locations(root / "ca.pem")
                        context.verify_mode = ssl.CERT_REQUIRED
                        with context.wrap_socket(connection, server_side=True) as tls:
                            data = b""
                            while b" " not in data:
                                chunk = tls.recv(8192)
                                if not chunk:
                                    return
                                data += chunk
                            length, body = data.split(b" ", 1)
                            assert length.isdigit() and int(length) <= 8192, "invalid octet count"
                            while len(body) < int(length):
                                chunk = tls.recv(8192)
                                if not chunk:
                                    break
                                body += chunk
                            assert len(body) == int(length), "incomplete frame"
                            received.append(body.decode("utf-8"))
                            # Keep the authenticated session open until the probe exits.
                            try:
                                while tls.recv(8192):
                                    pass
                            except (ssl.SSLError, OSError):
                                pass
                except (ssl.SSLError, ConnectionError, TimeoutError, OSError) as error:
                    errors.append(str(error))
                except Exception as error:
                    errors.append(f"ASSERT: {error}")

            thread = threading.Thread(target=collect, daemon=True)
            thread.start()
            config = root / f"{name}.yaml"
            config.write_text(f"""event_log:
  enabled: true
  path: {root / (name + '.jsonl')}
  syslog_enabled: true
  syslog_host: 127.0.0.1
  syslog_port: {port}
  syslog_source: cci-integration
  syslog_tls_ca: {root / 'ca.pem'}
  syslog_tls_cert: {root / 'client.pem'}
  syslog_tls_key: {root / 'client.key'}
  syslog_tls_crl: {root / crl}
  syslog_tls_peer_sha256: {'00' * 32 if bad_pin else pin}
""")
            start = time.monotonic()
            result = subprocess.run([probe, str(config), "fail" if fail else "ok"], capture_output=True, text=True, timeout=12)
            thread.join(timeout=7)
            listener.close()
            assert result.returncode == 0, (name, result.returncode, result.stderr)
            assert time.monotonic() - start < 10, f"{name}: unbounded network wait"
            assert not any(e.startswith("ASSERT:") for e in errors), errors
            if fail:
                assert not received, f"{name}: leaked application data to unauthenticated collector"
                rows = [json.loads(line) for line in (root / (name + '.jsonl')).read_text().splitlines()]
                assert any(row.get("iec_id") == "IEC62351-14:9.5" for row in rows), "missing local failure alarm"
            else:
                assert len(received) == 1, (name, errors, result.stderr)
                assert " - IEC62351-14:1 [62351-14@41912 ID=\"IEC62351-3:1.1\"" in received[0]
                assert 'SqNum="1"' in received[0]
            results.append(name)
            print(f"PASS {name}", flush=True)

        run("mutual-tls-octet-framing")
        run("wrong-collector-pin", bad_pin=True, fail=True)
        run("untrusted-collector", cert="untrusted", fail=True)
        run("expired-crl", crl="expired.crl", fail=True)
        run("stalled-handshake", stall=True, fail=True)
        openssl("ca", "-config", "ca.cnf", "-revoke", "server.pem")
        openssl("ca", "-gencrl", "-config", "ca.cnf", "-out", "revoked.crl")
        run("revoked-collector", crl="revoked.crl", fail=True)
        print(f"{len(results)} TLS logger scenarios passed")


if __name__ == "__main__":
    main(str(pathlib.Path(sys.argv[1]).resolve()))
