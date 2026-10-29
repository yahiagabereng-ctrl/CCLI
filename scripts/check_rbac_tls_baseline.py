#!/usr/bin/env python3
"""Check the repository TLS/ACSE baseline without connecting to or changing a DUT."""
import argparse
import hashlib
import json
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
BASELINE = ROOT / 'lab/conformance/CCI_RBAC_TLS_BASELINE.json'
EXTENSION = ROOT / 'lab/conformance/CCI_RBAC_DUAL_CERTIFICATE_EXTENSION.json'
NATIVE_EXTENSION = ROOT / 'lab/conformance/CCI_RBAC_NATIVE_E2E_EXTENSION.json'
TLS_EXTENSION = ROOT / 'lab/conformance/IEC62351_TLS_EXTENSION.json'
PROTECTED = [
    'apps/ccli/adapters/iec61850_mms/mms_acse_auth.cpp',
    'apps/ccli/adapters/iec61850_mms/mms_aare_auth.cpp',
    'apps/ccli/adapters/iec61850_mms/ccli_acse_bridge.c',
    'apps/ccli/adapters/iec61850_mms/ccli_acse_bridge.h',
    'apps/ccli/third_party/libiec61850/src/mms/iso_acse/acse.c',
    'apps/ccli/third_party/libiec61850/src/mms/iso_session/iso_session.c',
    'apps/ccli/third_party/libiec61850/src/mms/iso_presentation/iso_presentation.c',
    'apps/ccli/third_party/libiec61850/hal/tls/mbedtls3/tls_mbedtls.c',
]


def matches_reviewed_text(path, expected, normalized_expected=None):
    """Accept only the reviewed bytes or their LF/CRLF checkout variants."""
    data = path.read_bytes()
    lf = data.replace(b'\r\n', b'\n')
    return (expected in {hashlib.sha256(value).hexdigest()
                         for value in (data, lf, lf.replace(b'\n', b'\r\n'))}
            or (normalized_expected is not None and hashlib.sha256(lf).hexdigest() == normalized_expected))


def observed():
    source = (ROOT / 'apps/ccli/adapters/iec61850_mms/mms_adapter.cpp').read_text()
    calls = re.findall(r'TLSConfiguration_\w+\([^;]+\);', source)
    calls = [''.join(call.split()) for call in calls if 'setEventHandler' not in call]
    allow = re.search(r'const bool have_allow\s*=([^;]+);', source)
    return {
        'note': 'Repository source baseline; this is not a snapshot or live verification of the DUT.',
        'protected_source_sha256': {name: hashlib.sha256((ROOT / name).read_bytes()).hexdigest() for name in PROTECTED},
        'tls_configuration_calls_except_passive_event_observer': calls,
        'known_certificate_gate': ''.join(allow.group(1).split()),
        'lab_tls_settings': [line.strip() for line in (ROOT / 'apps/ccli/config/lab_tr400_phase4_eth_b.yaml').read_text().splitlines()
                             if line.lstrip().startswith('tls_')],
        'approved_tls_validation_overlay_sha256': hashlib.sha256(
            (ROOT / 'apps/ccli/cmake/tls_allowlist_validation.cmake').read_bytes()).hexdigest(),
    }


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--record', action='store_true', help='record a reviewed source baseline')
    args = parser.parse_args()
    current = observed()
    if args.record:
        BASELINE.write_text(json.dumps(current, indent=2) + '\n')
        print('Recorded repository baseline; review its diff before accepting it.')
        return 0
    expected = json.loads(BASELINE.read_text())
    extension = {}
    native = json.loads(NATIVE_EXTENSION.read_text()) if NATIVE_EXTENSION.exists() else {}
    tls = json.loads(TLS_EXTENSION.read_text()) if TLS_EXTENSION.exists() else {}
    for name, digest in tls.get('reviewed_source_lf_sha256', {}).items():
        if not matches_reviewed_text(ROOT / name, digest):
            print('IEC 62351 TLS EXTENSION DIFFERENCE:', name)
            return 1

    def reviewed(path, expected_digest):
        name = path.relative_to(ROOT).as_posix()
        override = tls.get('superseded_sources', {}).get(name)
        if override:
            if expected_digest != override['original']:
                return False
            return matches_reviewed_text(path, override['reviewed_lf_sha256'])
        return matches_reviewed_text(path, expected_digest)

    if tls:
        if current['tls_configuration_calls_except_passive_event_observer'] != tls['reviewed_tls_calls']:
            print('IEC 62351 TLS EXTENSION configuration calls differ')
            return 1
        # Preserve the earlier baseline and validate its integration history;
        # the current source and new policy are pinned separately above.
        current['tls_configuration_calls_except_passive_event_observer'] = tls['previous_tls_calls']
        # The IEC 104 strict profile now requires the same provisioned CRL as
        # MMS. Check this declared insertion without rewriting the old baseline.
        for insertion in tls.get('additional_lab_tls_settings', []):
            expected['lab_tls_settings'].insert(insertion['index'], insertion['line'])
    for name, digest in native.get('reviewed_source_lf_sha256', {}).items():
        if not reviewed(ROOT / name, digest):
            print('NATIVE E2E EXTENSION DIFFERENCE:', name)
            return 1
    if EXTENSION.exists():
        extension = json.loads(EXTENSION.read_text())
        for name, digest in extension['extension_source_sha256'].items():
            superseded = native.get('superseded_extension_sources', {}).get(name)
            if superseded:
                if superseded['original'] != digest:
                    print('NATIVE E2E EXTENSION original fingerprint mismatch:', name)
                    return 1
                digest = superseded['reviewed_lf_sha256']
            if not reviewed(ROOT / name, digest):
                print('DUAL CERTIFICATE EXTENSION DIFFERENCE:', name)
                return 1
        # Keep the original baseline intact. Accept only the explicitly listed
        # AARE integration and user-approved additional peer pin changes.
        for name, digest in extension['protected_source_overrides'].items():
            if expected['protected_source_sha256'].get(name) != digest['original']:
                print('DUAL CERTIFICATE EXTENSION original baseline mismatch:', name)
                return 1
            expected['protected_source_sha256'][name] = digest['extended']
        calls = expected['tls_configuration_calls_except_passive_event_observer']
        additions = extension['additional_tls_configuration_calls']
        # The added allowlist call and its failure cleanup form one exact block.
        actual = current['tls_configuration_calls_except_passive_event_observer']
        for call in additions:
            if call not in actual:
                print('DUAL CERTIFICATE EXTENSION missing additional TLS call')
                return 1
        reduced = list(actual)
        index = reduced.index(additions[0])
        if reduced[index:index + len(additions)] != additions:
            print('DUAL CERTIFICATE EXTENSION additional TLS block mismatch')
            return 1
        del reduced[index:index + len(additions)]
        # The adapter itself is pinned above. Accept only the reviewed repeated
        # client-pin block and the native startup-failure cleanup; all original
        # transport settings and source fingerprints remain checked below.
        native_additions = native.get('additional_tls_configuration_calls', [])
        if native_additions:
            if len(native_additions) != 3 or native_additions[-1] != 'TLSConfiguration_destroy(impl->tls);':
                print('NATIVE E2E EXTENSION malformed additional TLS block')
                return 1
            if native_additions[0] not in reduced:
                print('NATIVE E2E EXTENSION missing repeated client-pin block')
                return 1
            start = reduced.index(native_additions[0])
            if reduced[start:start + 2] != native_additions[:2]:
                print('NATIVE E2E EXTENSION repeated client-pin block mismatch')
                return 1
            del reduced[start:start + 2]
            if reduced[-2:] != [native_additions[-1], 'TLSConfiguration_destroy(impl_->tls);']:
                print('NATIVE E2E EXTENSION native cleanup mismatch')
                return 1
            del reduced[-2]
        if reduced != calls:
            print('TLS BASELINE DIFFERENCE: calls outside approved extension')
            return 1
        expected['tls_configuration_calls_except_passive_event_observer'] = actual
        expected['known_certificate_gate'] += '||!cfg.tls.additional_client_cert.empty()'
        expected['known_certificate_gate'] += native.get('additional_known_certificate_gate', '')
    for name, digest in expected['protected_source_sha256'].items():
        canonical = extension.get('protected_source_overrides', {}).get(name, {}).get('lf_sha256')
        if matches_reviewed_text(ROOT / name, digest, canonical):
            current['protected_source_sha256'][name] = digest
    overlay_key = 'approved_tls_validation_overlay_sha256'
    if reviewed(ROOT / 'apps/ccli/cmake/tls_allowlist_validation.cmake', expected[overlay_key]):
        current[overlay_key] = expected[overlay_key]
    if current != expected:
        for key in current:
            if current[key] != expected.get(key):
                print('TLS BASELINE DIFFERENCE:', key)
        return 1
    print('PASS: protected wire baseline and documented integration/TLS security extensions match.')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
