#!/usr/bin/env python3
"""Generate the reviewable BER/field inventory; does not run implementation tests."""
import csv
import hashlib
import json
import re
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[2]
SPEC = ROOT / 'apps/ccli/adapters/iec61850_mms/e2e/spec/iec62351-4-ed1.1'

# Each tuple is (component name, ASN.1 type, presence). Shared structures are
# expanded at each occurrence; aliases retain their exact semantic type name.
SCHEMA = {
    'HandshakeReq': [('pp', 'NMTOKEN', 'required'), ('tbs', 'HandshakeReqTBS', 'required'), ('sign', 'Signature', 'required')],
    'HandshakeReqTBS': [('token1', 'ClearToken1', 'required'), ('applData', 'ApplData', 'ASN optional; MMS InitiateRequest required')],
    'HandshakeAccTBS': [('token1', 'ClearToken1', 'required'), ('applData', 'ApplData', 'ASN optional; MMS InitiateResponse required')],
    'ApplicationRejectTBS': [('token3', 'ClearToken3', 'required'), ('applData', 'ApplData', 'required MMS rejection diagnostic')],
    'HandshakeSecRejectTBS': [('token3', 'ClearToken3', 'required'), ('diag', 'HandshakeDiagnostics', 'optional; absent on alarm')],
    'HandshakeSecAbortTBS': [('token3', 'ClearToken3', 'required'), ('diag', 'HandshakeDiagnostics', 'optional; absent on alarm')],
    'DtSecAbortTBS': [('token3', 'ClearToken3', 'required'), ('diag', 'DtDiagnostics', 'optional; absent on alarm')],
    'ApplAbortTBS': [('token3', 'ClearToken3', 'required'), ('applData', 'ApplData', 'optional MMS abort diagnostic')],
    'ReleaseReqTBS': [('token3', 'ClearToken3', 'required')],
    'ReleaseRspTBS': [('token3', 'ClearToken3', 'required')],
    'ClearTransfer': [('tbp', 'ClearTBP', 'required'), ('auth', 'Authenticator', 'required')],
    'ClearTBP': [('token2', 'ClearToken2', 'required'), ('applData', 'ApplData', 'required MMS PrPDU')],
    'EncrTransfer': [('tbp', 'EncryptedTBP', 'required'), ('auth', 'Authenticator', 'conditional non-AE; absent for AE')],
    'EncryptedTBP': [('token2', 'ClearToken2', 'required'), ('applData', 'Encrypted-ApplData', 'required ciphertext')],
    'ClearToken1': [('sigAlg', 'AlgorithmIdentifier', 'required'), ('pkCert', 'PKCert', 'required'),
        ('certPath', 'CertPath', 'conditional optional'), ('version', 'Version', 'DEFAULT {v1}'),
        ('assoID', 'AssoID', 'required; request client-only / acceptance composed'), ('dHKey', 'DiffieHellmanSet', 'required'),
        ('hmac', 'AlgorithmIdentifier', 'required'), ('time', 'TimeStamp', 'required'),
        ('encr-mode', 'EncryptionMode', 'required'), ('confParams', 'ConfidentialityParms', 'required'), ('attCert', 'ACert', 'optional unsupported feature')],
    'EncryptionMode': [('aea', 'AEAAlgorithms', 'CHOICE conditional unsupported AE'), ('non-aea', 'NonAEA', 'CHOICE selected')],
    'AEAAlgorithms': [('algo[*]', 'AlgorithmIdentifier', 'SEQUENCE SIZE(1..MAX) OF')],
    'NonAEA': [('encr', 'EncryptionAlgorithms', 'optional; unsupported encryption'), ('icvAlgID', 'ICVAlgorithms', 'required')],
    'EncryptionAlgorithms': [('algo[*]', 'AlgorithmIdentifier', 'SEQUENCE SIZE(1..MAX) OF')],
    'ICVAlgorithms': [('algo[*]', 'AlgorithmIdentifier', 'SET SIZE(1..MAX) OF; unordered')],
    'DiffieHellmanSet': [('groupId', 'INTEGER', 'required supported group'), ('dhPublicKey', 'DhPublicKey', 'required SIZE(1..MAX)')],
    'CertPath': [('pkCert[*]', 'PKCert', 'SEQUENCE SIZE(1..MAX) OF; ordered chain')],
    'ClearToken2': [('version', 'Version', 'DEFAULT {v1}'), ('assoID', 'AssoID', 'required full accepted ID'),
        ('time', 'TimeStamp', 'required'), ('seq', 'SequenceNumber', 'required'), ('iv', 'InitializationVector', 'optional [0]; absent clear'),
        ('rekey', 'DhPublicKey', 'optional [1]'), ('reqRekey', 'BOOLEAN', '[2] DEFAULT FALSE; server-only'),
        ('changedKey', 'BOOLEAN', '[3] DEFAULT FALSE; server-only')],
    'ClearToken3': [('sigAlg', 'AlgorithmIdentifier', 'required'), ('version', 'Version', 'DEFAULT {v1}'),
        ('assoID', 'AssoID', 'optional; full established / known partial error ID'), ('time', 'TimeStamp', 'required'),
        ('pkCert', 'PKCert', 'required'), ('certPath', 'CertPath', 'conditional optional')],
    'Signature': [('algo', 'AlgorithmIdentifier', 'required; equals token sigAlg'), ('sign', 'OCTET STRING', 'required nonempty signature')],
    'Authenticator': [('nonce', 'OCTET STRING', 'ASN optional; GMAC required / HMAC absent'),
        ('algo', 'AlgorithmIdentifier', 'required negotiated ICV'), ('icv', 'OCTET STRING', 'required exact negotiated length')],
    'AlgorithmIdentifier': [('algorithm', 'OBJECT IDENTIFIER', 'required supported OID'), ('parameters', 'ANY', 'algorithm-specific optional/required/absent')],
    'HandshakeRsp': [('assoAccept', 'HandshakeAcc', 'CHOICE [1]'), ('applReject', 'ApplicationReject', 'CHOICE [2]'), ('secReject', 'HandshakeSecReject', 'CHOICE [3]')],
    'AssoAbort': [('handsSecAbort', 'HandshakeSecAbort', 'CHOICE [0]; RX only'), ('dtSecAbort', 'DtSecAbort', 'CHOICE [1]'),
        ('applAbort', 'ApplAbort', 'CHOICE [2]'), ('osiDiag', 'OsiDiagnostics', 'CHOICE [3]')],
    'DataTransfer': [('clearTransfer', 'ClearTransfer', 'CHOICE [0] selected'), ('encrTransfer', 'EncrTransfer', 'CHOICE [1] unsupported selected profile')],
    'User-data': [('entry[0]', 'PresentationEntry', 'SEQUENCE SIZE(1) OF')],
    'PresentationEntry': [('presentation-context-identifier', 'Presentation-context-identifier', 'required INTEGER'), ('single-ASN1-type', 'DataTransfer', '[0] EXPLICIT; exactly one')],
    'AARQ': [('protocol-version', 'Version', 'optional/default v1'), ('application-context-name', 'OBJECT IDENTIFIER', 'required native context'),
        ('called-AP-title', 'AP-title', 'optional/inherited profile disposition'), ('called-AE-qualifier', 'AE-qualifier', 'optional/inherited profile disposition'),
        ('called-AP-invocation-id', 'INTEGER', 'not required; safe support disposition'), ('called-AE-invocation-id', 'INTEGER', 'not required; safe support disposition'),
        ('calling-AP-title', 'AP-title', 'required selected identity profile'), ('calling-AE-qualifier', 'AE-qualifier', 'required selected identity profile'),
        ('calling-AP-invocation-id', 'INTEGER', 'not required; safe support disposition'), ('calling-AE-invocation-id', 'INTEGER', 'not required; safe support disposition'),
        ('sender-acse-requirements', 'BIT STRING', 'forbidden native'), ('mechanism-name', 'OBJECT IDENTIFIER', 'forbidden native'),
        ('calling-authentication-value', 'ANY', 'forbidden native'), ('application-context-name-list', 'SEQUENCE OF OID', 'forbidden native'),
        ('implementation-information', 'GraphicString', 'not required; safe support disposition'), ('user-information', 'OSI-AssoReq', 'required native')],
    'AARE': [('protocol-version', 'Version', 'optional/default v1'), ('application-context-name', 'OBJECT IDENTIFIER', 'required native context'),
        ('result', 'INTEGER', 'required accepted/rejected-permanent mapping'), ('result-source-diagnostic', 'ACSESourceDiagnostic', 'required user/provider CHOICE'),
        ('responding-AP-title', 'AP-title', 'native response identity'), ('responding-AE-qualifier', 'AE-qualifier', 'native response identity'),
        ('responding-AP-invocation-id', 'INTEGER', 'not required; safe support disposition'), ('responding-AE-invocation-id', 'INTEGER', 'not required; safe support disposition'),
        ('responder-acse-requirements', 'BIT STRING', 'forbidden native'), ('mechanism-name', 'OBJECT IDENTIFIER', 'forbidden native'),
        ('responding-authentication-value', 'ANY', 'forbidden native'), ('application-context-name-list', 'SEQUENCE OF OID', 'forbidden native'),
        ('implementation-information', 'GraphicString', 'not required; safe support disposition'), ('user-information', 'OSI-AssoRsp', 'conditional; absent outer ACSE rejection')],
    'ACSESourceDiagnostic': [('acse-service-user', 'INTEGER', 'CHOICE user'), ('acse-service-provider', 'INTEGER', 'CHOICE provider')],
    'ABRT': [('abort-source', 'INTEGER', 'required user/provider'), ('abort-diagnostic', 'INTEGER', 'absent per native mapping'), ('user-information', 'OSI-AssoAbr', 'conditional by error layer')],
    'RLRQ': [('reason', 'INTEGER', 'optional permitted native reason'), ('user-information', 'OSI-AssoRelReq', 'required signed release')],
    'RLRE': [('reason', 'INTEGER', 'optional permitted native reason'), ('user-information', 'OSI-AssoRelRsp', 'required signed release')],
    'PresentationConnect': [('mode-selector', 'INTEGER', 'required normal mode'), ('calling-selector', 'OCTET STRING', 'profile optional'),
        ('called-selector', 'OCTET STRING', 'profile optional'), ('context-list[*]', 'PresentationContext', 'distinct odd ACSE/MMS/E2E contexts'), ('user-data', 'ANY', 'required one native AARQ')],
    'PresentationContext': [('identifier', 'INTEGER', 'distinct positive odd stack-bounded'), ('abstract-syntax-name', 'OBJECT IDENTIFIER', 'required ACSE/MMS/E2E'),
        ('transfer-syntax-name[*]', 'OBJECT IDENTIFIER', 'required supported BER syntax')],
    'SessionConnect': [('version', 'INTEGER', 'required profile session v2'), ('duplex', 'BIT STRING', 'required negotiated profile'),
        ('length', 'SessionLength', 'short/extended consistent'), ('user-data', 'ANY', 'complete CONNECT bound 10240 octets')],
}
SIGNED = ['HandshakeAcc', 'ApplicationReject', 'HandshakeSecReject', 'HandshakeSecAbort', 'DtSecAbort', 'ApplAbort', 'ReleaseReq', 'ReleaseRsp']
for name in SIGNED:
    SCHEMA[name] = [('tbs', name + 'TBS', 'required'), ('sign', 'Signature', 'required')]
for name, inner in [('OSI-AssoReq', 'HandshakeReq'), ('OSI-AssoRsp', 'HandshakeRsp'), ('OSI-AssoAbr', 'AssoAbort'),
                    ('OSI-AssoRelReq', 'ReleaseReq'), ('OSI-AssoRelRsp', 'ReleaseRsp')]:
    SCHEMA[name] = [('entry[0]', name + 'Entry', 'SEQUENCE SIZE(1) OF UNIVERSAL 8')]
    SCHEMA[name + 'Entry'] = [('indirect-reference', 'INTEGER', 'required negotiated E2E reference'), ('single-ASN1-type', inner, '[0] EXPLICIT; exactly one')]

EXTENSIBLE = {'ClearToken1', 'ClearToken2', 'ClearToken3', 'Signature', 'Authenticator', 'AlgorithmIdentifier',
    'DiffieHellmanSet', 'EncryptionMode', 'HandshakeRsp', 'AssoAbort', 'DataTransfer', 'ClearTBP', 'EncryptedTBP'} | {name + 'TBS' for name in SIGNED} | {'HandshakeReqTBS'}
ALIASES = {'NMTOKEN', 'AssoID', 'TimeStamp', 'Version', 'PKCert', 'ACert', 'DhPublicKey', 'ConfidentialityParms',
    'SequenceNumber', 'InitializationVector', 'Encrypted-ApplData', 'HandshakeDiagnostics', 'DtDiagnostics',
    'OsiDiagnostics', 'Presentation-context-identifier', 'AES-InitializationVector'}
METADATA = {'ALGORITHM': 'Information object class; its Type/id become AlgorithmIdentifier.parameters/algorithm; not an extra runtime PDU field'}
ROOTS = [(55, 'HandshakeReq', 'RX'), (56, 'HandshakeAcc', 'TX'), (57, 'ApplicationReject', 'TX'), (58, 'HandshakeSecReject', 'TX'),
    (59, 'HandshakeSecAbort', 'RX'), (60, 'DtSecAbort', 'RX/TX'), (61, 'ApplAbort', 'RX/TX'), (62, 'ClearTransfer', 'RX'),
    (63, 'ClearTransfer', 'TX'), (64, 'EncrTransfer', 'RX conditional'), (65, 'EncrTransfer', 'TX conditional'),
    (66, 'ReleaseReq', 'RX'), (67, 'ReleaseRsp', 'TX'), (68, 'AARQ', 'RX'), (69, 'AARE', 'TX'),
    (70, 'ABRT', 'RX'), (71, 'ABRT', 'TX'), (72, 'User-data', 'RX/TX'), (73, 'User-data', 'RX/TX conditional'),
    (74, 'RLRQ', 'RX'), (75, 'RLRE', 'TX'), ('CORE-005/006', 'PresentationConnect', 'RX; verify CPA TX'),
    ('CORE-005', 'SessionConnect', 'RX; verify ACCEPT/FINISH/DISCONNECT/ABORT TX')]

GROUPS = {
    1: ('55.1-3;56.1-2', 'handshake container/pp/token'), 2: ('55.4-6;56.3-5', 'certificate/path/signature algorithm'),
    3: ('55.7-12;56.6-11', 'version/ID/DH/HMAC'), 4: ('55.13;56.12', 'handshake time'), 5: ('55.14-19;56.13-18', 'negotiation/confidentiality'),
    6: ('55.20;56.19', 'conditional attribute certificate'), 7: ('55.21;56.20', 'MMS initialization'), 8: ('55.22-24;56.21-23', 'handshake exact signature'),
    9: ('57.1-8;58.1-8;59.1-8;60.1-8;61.1-8', 'signed token3 errors'), 10: ('57.9;61.9', 'application diagnostic'),
    11: ('58.9;59.9;60.9', 'diagnostic versus alarm omission'), 12: ('57.10-12;58.10-12;59.10-12;60.10-12;61.10-12', 'error signatures'),
    13: ('62.1-6;63.1-6', 'token2 identity/time/sequence'), 14: ('62.7;63.7', 'clear IV absence'), 15: ('62.8;63.8-9', 'directional rekey'),
    16: ('62.9;63.10', 'MMS transfer open type'), 17: ('62.10-13;63.11-14', 'authenticator/nonce/ICV'),
    18: ('64.1-12;65.1-14', 'conditional encrypted fields'), 19: ('66.1-8;67.1-8', 'release token3'), 20: ('66.9-11;67.9-11', 'release signature'),
    21: ('68.1-5,8-9;69.1-7', 'native ACSE titles/context/result'), 22: ('68.6-7,10-11,16;69.8-9,14', 'optional invocation/implementation'),
    23: ('68.12-15;69.10-13', 'forbidden native authentication'), 24: ('68.17-19;69.15-17', 'OSI association mapping/response choice'),
    25: ('70.1-9;71.1-9', 'abort source/layer/payload'), 26: ('72.1-3', 'clear presentation'), 27: ('73.1-3', 'conditional encrypted presentation'),
    28: ('74.1-5;75.1-5', 'release wrapper'),
}

def groups(table, path):
    if table in (55, 56):
        if '.sign' in path: return [8]
        for suffix, group in [('attCert', 6), ('confParams', 5), ('encr-mode', 5), ('time', 4), ('applData', 7),
                              ('certPath', 2), ('pkCert', 2), ('sigAlg', 2), ('dHKey', 3), ('version', 3), ('assoID', 3), ('hmac', 3)]:
            if suffix in path: return [group]
        return [1]
    if table in range(57, 62):
        return [12] if '.sign' in path else [10] if 'applData' in path else [11] if '.diag' in path else [9]
    if table in (62, 63):
        return [17] if '.auth' in path else [14] if '.iv' in path else [15] if any(v in path for v in ('rekey', 'reqRekey', 'changedKey')) else [16] if 'applData' in path else [13]
    if table in (64, 65): return [18]
    if table in (66, 67): return [20] if '.sign' in path else [19]
    if table in (68, 69):
        return [22] if any(v in path for v in ('invocation', 'implementation-information')) else [23] if any(v in path for v in ('acse-requirements', 'mechanism-name', 'authentication-value', 'context-name-list')) else [24] if 'user-information' in path else [21]
    return {70: [25], 71: [25], 72: [26], 73: [27], 74: [28], 75: [28]}.get(table, [])

rows = []
def walk(table, root, direction, typ, path='', presence='required'):
    current = path or root
    conditional = table in (64, 65, 73) or typ in ('EncrTransfer', 'EncryptedTBP', 'Encrypted-ApplData', 'AEAAlgorithms', 'EncryptionAlgorithms', 'ACert') or any(v in current for v in ('.aea', '.encr.', '.encr[', 'attCert', 'encrTransfer'))
    forbidden = 'forbidden' in presence
    group_ids = groups(table, current)
    owner = 'e2e_codec/ber_codec' if table not in (68, 69, 70, 71, 74, 75) else 'native OSI bridge/e2e_server'
    if any(v in current for v in ('pkCert', 'certPath', 'attCert')): owner += '; e2e_identity'
    if any(v in current for v in ('time', 'version', 'assoID', 'seq', 'rekey', 'changedKey', 'reqRekey')): owner += '; profile/state validator'
    if any(v in current for v in ('sign', 'icv', 'nonce', 'dHKey', 'hmac')): owner += '; crypto/association'
    rows.append({
        'map_id': f'BER-FIELD-{len(rows)+1:04}', 'table': table,
        'requirement_ids': ';'.join(f'E2E-FIELD-{i:03}' for i in group_ids) or str(table),
        'source_rows': ';'.join(GROUPS[i][0] for i in group_ids), 'container': root, 'schema_path': current,
        'asn_type': typ, 'presence_or_choice': presence, 'direction': direction,
        'extension_policy': 'EXTENSIBLE: F07 schema-aware' if typ in EXTENSIBLE else 'LIST/cardinality' if typ in ('CertPath', 'AEAAlgorithms', 'EncryptionAlgorithms', 'ICVAlgorithms') else 'CLOSED' if typ in SCHEMA else 'SCALAR/OPEN TYPE; use field rules',
        'applicability': 'ACTIVE negative: forbidden field' if forbidden else 'CONDITIONAL positive; ACTIVE unsupported-feature negative' if conditional else 'ACTIVE selected profile',
        'test_families': 'F01-F10 as applicable; B01-B12 per BER_FIELD_COVERAGE.md',
        'positive_assertion': 'Correct logical value/presence/direction; exact original authenticated spans; expected MMS dispatch/TX structure' if not forbidden else 'Omission; ordinary native association succeeds',
        'negative_assertion': 'Isolated structural/profile/authentication fault; exact layer/source/choice/diagnostic or alarm omission; no unauthorized dispatch; closure/reconnect per contract',
        'owner': owner, 'status': 'MAPPED - execution/qualification incomplete', 'source_gates': 'S02/S03/S04/S05/S06/S08/S11/S13 as applicable',
    })
    for name, child, required in SCHEMA.get(typ, []):
        walk(table, root, direction, child, current + '.' + name, required)

for table, root, direction in ROOTS:
    walk(table, root, direction, root)

declared = {}
for name in ('IEC_62351-4_E2E-security.asn', 'IEC_62351-4_OSI-Env.asn'):
    text = (SPEC / name).read_text()
    declared[name] = sorted(set(re.findall(r'^([A-Za-z][\w-]*)(?:\{[^\n]*\})?\s*::=', text, re.M)))
    missing = set(declared[name]) - set(SCHEMA) - ALIASES - set(METADATA)
    assert not missing, (name, missing)
field_ids = {row['id'] for row in csv.DictReader((HERE / 'requirements.csv').open(newline='')) if row['id'].startswith('E2E-FIELD-')}
mapped = {value for row in rows for value in row['requirement_ids'].split(';') if value.startswith('E2E-FIELD-')}
assert mapped == field_ids == {f'E2E-FIELD-{i:03}' for i in range(1, 29)}, (mapped ^ field_ids)
assert set(range(55, 76)) <= {r['table'] for r in rows}
assert len({r['map_id'] for r in rows}) == len(rows)
with (HERE / 'ber-field-map.csv').open('w', newline='', encoding='utf-8') as stream:
    writer = csv.DictWriter(stream, fieldnames=list(rows[0]), lineterminator='\n'); writer.writeheader(); writer.writerows(rows)
inventory = {
    'note': 'Mapping completeness check only; no implementation tests or formal conformance pass. Implementation and qualification status are tracked separately.',
    'expanded_field_and_container_rows': len(rows), 'mapped_field_groups': sorted(mapped), 'mapped_subclass_tables': list(range(55, 76)),
    'declared_types_checked': declared, 'schema_supporting_aliases': sorted(ALIASES),
    'non_pdu_metadata_dispositions': METADATA,
    'sources': {name: hashlib.sha256((SPEC / name).read_bytes()).hexdigest() for name in declared},
    'explicit_exclusions': ['XMPP/XER and router role', 'full MMS application service ASN.1 suite beyond protected open-type boundary'],
    'conditional_unselected_positive_features': ['application encryption AE/non-AE', 'attribute certificates'],
    'negative_cases_for_unselected_features': 'Remain active; unsupported choice/field must fail safely with mapped response',
}
(HERE / 'ber-field-map-inventory.json').write_text(json.dumps(inventory, indent=2) + '\n')
print(f'Mapping inventory: {len(rows)} rows; 28/28 FIELD groups; 21/21 subclass tables; both ASN.1 type inventories dispositioned. No implementation tests run.')
