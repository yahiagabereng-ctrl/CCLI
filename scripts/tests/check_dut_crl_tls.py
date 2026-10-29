"""Read-only TLS acceptance checks after the approved lab trust migration.

Uses existing local client keys; never prints or copies key material.
This checks TLS only, not MMS association or active-session revocation.
"""
import argparse
import pathlib
import socket
import ssl


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--host', default='192.168.0.1')
    parser.add_argument('--port', type=int, default=3782)
    parser.add_argument('--ca', type=pathlib.Path, required=True)
    args = parser.parse_args()
    repo = pathlib.Path(__file__).resolve().parents[2]
    current = repo/'lab/ejbca-staging/mms-rbac-package-2026-10-28/tsp/tls'
    legacy = repo/'apps/ccli/config/tls'
    for label, cert, key, expected in [
        ('EJBCA client', current/'certificate-chain.pem', current/'private-key.pem', True),
        ('retired legacy client', legacy/'client.pem', legacy/'client.key', False),
    ]:
        context = ssl.create_default_context(cafile=str(args.ca))
        context.maximum_version = ssl.TLSVersion.TLSv1_2
        context.load_cert_chain(str(cert), str(key))
        try:
            with socket.create_connection((args.host, args.port), timeout=8) as raw:
                with context.wrap_socket(raw, server_hostname=args.host) as secure:
                    if not expected:
                        raise RuntimeError('retired legacy client completed TLS handshake')
                    print(f'PASS {label}: {secure.version()} {secure.cipher()[0]}')
        except ssl.SSLError as error:
            if expected:
                raise
            # A local server-chain verification failure does not prove rejection
            # of our client. Require an explicit alert from the remote verifier.
            if error.reason not in ('TLSV1_ALERT_UNKNOWN_CA', 'SSLV3_ALERT_BAD_CERTIFICATE',
                                    'SSLV3_ALERT_CERTIFICATE_UNKNOWN', 'SSLV3_ALERT_HANDSHAKE_FAILURE',
                                    'TLSV1_ALERT_ACCESS_DENIED'):
                raise
            print(f'PASS {label}: rejected by server ({error.reason})')


if __name__ == '__main__':
    main()
