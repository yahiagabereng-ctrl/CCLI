#!/usr/bin/env python3
"""Issue verified 30-day CRLs for the two CCLI development EJBCA CAs only."""
import argparse
import datetime as dt
import json
from pathlib import Path

from cryptography import x509

from ejbca_lab_common import export_crl_pem_from_ejbca, run_ejbca


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--out-dir', type=Path, required=True,
                        help='New output directory retaining pre-change public CRLs and verification metadata')
    args = parser.parse_args()
    repo = Path(__file__).resolve().parents[1]
    trust = repo/'lab/ejbca-staging/mms-rbac-package-2026-10-28/trust'
    args.out_dir.mkdir(parents=True, exist_ok=False)
    records = []
    bundle = []
    for ca_name, prefix in [('CCLI-Lab-CA', 'ccli-lab-ca'),
                            ('CCLI-RBAC-Lab-CA', 'ccli-rbac-ca')]:
        anchor = x509.load_pem_x509_certificate((trust/f'{prefix}.pem').read_bytes())
        old_path = args.out_dir/f'{prefix}.previous.crl.pem'
        export_crl_pem_from_ejbca(ca_name, old_path, publish=False)
        old = x509.load_pem_x509_crl(old_path.read_bytes())
        if old.issuer != anchor.subject or not old.is_signature_valid(anchor.public_key()):
            raise RuntimeError(f'{ca_name}: old CRL does not match pinned CA')
        run_ejbca(['ca', 'editca', '--caname', ca_name, '--field', 'CRLPeriod',
                   '--value', str(30*24*60*60*1000)])
        path = args.out_dir/f'{prefix}.crl.pem'
        export_crl_pem_from_ejbca(ca_name, path)
        crl = x509.load_pem_x509_crl(path.read_bytes())
        if crl.issuer != anchor.subject or not crl.is_signature_valid(anchor.public_key()):
            raise RuntimeError(f'{ca_name}: new CRL signature/issuer invalid')
        now = dt.datetime.now(dt.timezone.utc)
        if crl.next_update_utc is None or not crl.last_update_utc <= now < crl.next_update_utc:
            raise RuntimeError(f'{ca_name}: new CRL not currently valid')
        if abs((crl.next_update_utc-crl.last_update_utc)-dt.timedelta(days=30)) > dt.timedelta(seconds=5):
            raise RuntimeError(f'{ca_name}: signed lifetime is not 30 days')
        if not {entry.serial_number for entry in old}.issubset({entry.serial_number for entry in crl}):
            raise RuntimeError(f'{ca_name}: previously listed revocations were dropped')
        number = crl.extensions.get_extension_for_class(x509.CRLNumber).value.crl_number
        old_number = old.extensions.get_extension_for_class(x509.CRLNumber).value.crl_number
        if number <= old_number:
            raise RuntimeError(f'{ca_name}: CRL number did not increase')
        bundle.append(path.read_bytes())
        records.append(dict(ca=ca_name, number=number, revoked_count=len(crl),
                            this_update=crl.last_update_utc.isoformat(),
                            next_update=crl.next_update_utc.isoformat()))
    (args.out_dir/'development-tls-crl-bundle.pem').write_bytes(b''.join(bundle))
    (args.out_dir/'verification.json').write_text(json.dumps(records, indent=2)+'\n')
    print(json.dumps(records, indent=2))


if __name__ == '__main__':
    main()
