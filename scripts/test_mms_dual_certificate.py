from pathlib import Path
import datetime as dt
import hashlib
import json
import subprocess
import time
import sys
import argparse
from cryptography import x509
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import padding

root = Path(__file__).resolve().parents[1]
bundle = root / 'lab/ejbca-staging/rbac'
dual = bundle / 'dual-certificate'
parser = argparse.ArgumentParser(description='Explicit lab dual-certificate network verification; requires protected EJBCA bundle and built probe.')
mode = parser.add_mutually_exclusive_group()
mode.add_argument('--device', action='store_true', help='Use an already started isolated Tespro harness on port 3783.')
mode.add_argument('--main-read-only', action='store_true', help='Only associate/read and verify the main service AARE on port 3782; never write/control.')
parser.add_argument('--bundle-root', type=Path, default=bundle, help='Read existing protected lab PKI without copying its keys into this checkout.')
parser.add_argument('--output-root', type=Path, help='Separate evidence directory for merge validation.')
parser.add_argument('--probe', type=Path, help='Explicit compiled native probe executable.')
args = parser.parse_args()
bundle = args.bundle_root.resolve()
dual = bundle / 'dual-certificate'
evidence = args.output_root.resolve() if args.output_root else root / 'lab/ejbca-staging/rbac/dual-certificate'
evidence.mkdir(parents=True, exist_ok=True)
MAIN_READ_ONLY = args.main_read_only
DEVICE = args.device or MAIN_READ_ONLY
HOST = '192.168.0.1' if DEVICE else '127.0.0.1'
PORT = '3782' if MAIN_READ_ONLY else ('3783' if DEVICE else '3784')
out = evidence / ('main-test' if MAIN_READ_ONLY else ('device-test' if DEVICE else 'host-test'))
out.mkdir(exist_ok=True)
tls = root / 'apps/ccli/config/tls'
out.joinpath('ca-bundle.pem').write_bytes((tls/'root_CA.pem').read_bytes().rstrip()+b'\n'+(bundle/'rbac_ca.pem').read_bytes())
out.joinpath('crl-bundle.pem').write_bytes((tls/'lab_crl.pem').read_bytes().rstrip()+b'\n'+(bundle/'rbac_crl.pem').read_bytes())
config = f'''mms:
  enabled: true
  bind_address: 127.0.0.1
  tcp_port: 3784
  comms_loss_fallback_s: 0
  rbac_profile_a: true
  rbac_policy: {dual}/commissioning-sequence3/commissioning-policy.xml
  rbac_owner_cert: {bundle}/owner-signer/owner-signer.pem
  rbac_activation_state: {out}/activation.state
  rbac_identity_ca: {bundle}/rbac_ca.pem
  rbac_identity_crl: {bundle}/rbac_crl.pem
  tls_enabled: true
  tls_chain_validation: true
  tls_own_cert: {tls}/server_tls.pem
  tls_own_key: {tls}/server.key
  tls_acse_cert: {tls}/server.pem
  tls_acse_key: {tls}/server_acse.key
  tls_ca_cert: {out}/ca-bundle.pem
  tls_crl: {out}/crl-bundle.pem
  tls_client_cert: {bundle}/dso/dso.pem
  tls_acse_client_cert: {tls}/viewer_tls.pem
  tls_viewer_cert: {bundle}/viewer-r1/viewer-r1.pem
  tls_revoked_cert: {bundle}/negative-revoked/negative-revoked.pem
  tls_additional_client_cert: {tls}/client_tls.pem
event_log:
  enabled: true
  path: {out}/events.jsonl
  syslog_enabled: false
'''
(out/'test.yaml').write_text(config)

def tlv(tag, content):
    size = len(content)
    length = bytes([size]) if size<128 else bytes([0x80+(size.bit_length()+7)//8])+size.to_bytes((size.bit_length()+7)//8,'big')
    return bytes([tag])+length+content

def blob(name, identity='dso-mms', stale=False, bad=False):
    cert = x509.load_pem_x509_certificate((dual/(identity+'.pem')).read_bytes())
    key = serialization.load_pem_private_key((dual/(identity+'.key')).read_bytes(),password=None)
    now = dt.datetime.now(dt.timezone.utc)-(dt.timedelta(hours=1) if stale else dt.timedelta())
    stamp = now.strftime('%Y%m%d%H%M%S.%fZ').encode()
    signature = key.sign(stamp,padding.PKCS1v15(),hashes.SHA256())
    if bad: signature=bytes([signature[0]^1])+signature[1:]
    payload=tlv(0x80,cert.public_bytes(serialization.Encoding.DER))+tlv(0x81,stamp)+tlv(0x82,signature)
    data=tlv(0xa2,tlv(2,b'\x05')+tlv(0xa0,tlv(0xa0,payload)))
    path=out/(name+'.bin'); path.write_bytes(data); return path

def parse(data):
    result=[]; pos=0
    while pos<len(data):
        tag=data[pos]; pos+=1
        assert pos<len(data)
        length=data[pos]; pos+=1
        if length&128:
            n=length&127; assert 0<n<=3 and pos+n<=len(data)
            length=int.from_bytes(data[pos:pos+n],'big');pos+=n
        assert pos+length<=len(data)
        result.append((tag,data[pos:pos+length]));pos+=length
    return result

def verify_aare(path):
    top=parse(path.read_bytes()); assert len(top)==1 and top[0][0]==0x61
    fields=dict(parse(top[0][1]))
    assert fields[0x89]==bytes.fromhex('2886480001000101')
    outer=parse(fields[0xaa]); assert len(outer)==1 and outer[0][0]==0xa2
    external=dict(parse(outer[0][1])); assert external[2]==b'\x05'
    choice=parse(external[0xa0]); assert len(choice)==1 and choice[0][0]==0xa0
    values=dict(parse(choice[0][1]))
    cert=x509.load_der_x509_certificate(values[0x80])
    expected=x509.load_pem_x509_certificate((tls/'server.pem').read_bytes())
    ca=x509.load_pem_x509_certificate((tls/'root_CA.pem').read_bytes())
    assert cert==expected and cert.issuer==ca.subject
    ca.public_key().verify(cert.signature,cert.tbs_certificate_bytes,padding.PKCS1v15(),cert.signature_hash_algorithm)
    cert.public_key().verify(values[0x82],values[0x81],padding.PKCS1v15(),hashes.SHA256())
    signed=dt.datetime.strptime(values[0x81][:14].decode(),'%Y%m%d%H%M%S').replace(tzinfo=dt.timezone.utc)
    assert abs((dt.datetime.now(dt.timezone.utc)-signed).total_seconds())<120
    assert dict(parse(fields[0xa4]))[6]==bytes.fromhex('2901876701')
    assert dict(parse(fields[0xa5]))[2]==b'\x0c'
    print('PASS: independent AARE server certificate, issuer, AP/AE, signature, mechanism and time verification',flush=True)

def verify_session(path):
    data = path.read_bytes()
    assert data[0] in (13, 14)
    def length(pos):
        value = data[pos]; pos += 1
        if value == 255:
            assert pos+2 <= len(data)
            value = int.from_bytes(data[pos:pos+2], 'big'); pos += 2
        return value, pos
    size, pos = length(1)
    assert size == len(data)-pos, 'SPDU length mismatch'
    while pos < len(data):
        tag = data[pos]; pos += 1
        size, pos = length(pos)
        assert pos+size <= len(data)
        if tag in (193, 194):
            assert pos+size == len(data)
            assert tag == (194 if size >= 255 else 193), 'wrong short/extended user-data PGI'
            print('PASS: session '+('CONNECT' if data[0] == 13 else 'ACCEPT')+' length and PGI '+str(tag),flush=True)
            return
        pos += size
    raise AssertionError('session user data missing')

def probe(name, path, expected=True, viewer_tls=False, ap='1.1.1.999.1'):
    command=[str(args.probe or root/'apps/ccli/build-rbac-linux/mms-rbac-dual-probe'),HOST,PORT,str(tls/'root_CA.pem'),
             str(tls/('viewer_tls.pem' if viewer_tls else 'client_tls.pem')),
             str(tls/('viewer.key' if viewer_tls else 'client_tsp.key')),str(path),ap,'12',str(out/(name+'.aare.der')),
             'ALLOW' if expected else 'DENY',name]
    r=subprocess.run(command,capture_output=True,text=True,timeout=20)
    (out/(name+'.log')).write_text(r.stdout+r.stderr)
    assert (out/(name+'.aare.der.spdu')).stat().st_size > 0, 'client did not send request: '+name
    print('\n'.join(s for s in (r.stdout+r.stderr).splitlines() if s.startswith(('PASS:','FAIL:'))),flush=True)
    assert r.returncode==0,name
    if expected:
        verify_aare(out/(name+'.aare.der'))
        verify_session(out/(name+'.aare.der.spdu'))
        verify_session(out/(name+'.aare.der.accept.spdu'))
    results.append(dict(case=name,passed=True))

results=[]
log=None if DEVICE else open(out/'server.log','w')
server=None if DEVICE else subprocess.Popen([str(root/'apps/ccli/build-rbac-linux/mms-rbac-dual-host-server'),str(out/'test.yaml'),'180'],stdout=log,stderr=log)
try:
    for _ in range(0 if DEVICE else 40):
        if server.poll() is not None: raise RuntimeError((out/'server.log').read_text()[-5000:])
        if 'ISOLATED_RBAC_TEST_READY' in (out/'server.log').read_text(): break
        time.sleep(.25)
    if MAIN_READ_ONLY:
        probe('main-readonly', blob('main-readonly'))
    else:
        good=blob('dso-positive')
        probe('dso-positive',good)
        probe('replay',good,False)
        probe('viewer-positive',blob('viewer-positive','viewer-mms'),viewer_tls=True)
        probe('bad-signature',blob('bad-signature',bad=True),False)
        probe('stale-proof',blob('stale-proof',stale=True),False)
        probe('wrong-ap',blob('wrong-ap'),False,ap='1.1.1.999.2')
        probe('wrong-transport',blob('wrong-transport'),False,viewer_tls=True)
    if not DEVICE:
        server_events=(out/'server.log').read_text()
        assert 'mms: iso session CONNECT (payload=' in server_events
        assert 'mms: iso session ACCEPT extended LI' in server_events
        for reason in ('replayed authentication proof', 'MMS authentication signature invalid',
                       'stale or future authentication proof', 'certificate AP/AE does not match calling application reference',
                       'transport/identity pairing mismatch; all access denied'):
            assert reason in server_events, 'missing server rejection evidence: '+reason
finally:
    if server:
        server.terminate()
        try: server.wait(timeout=12)
        except subprocess.TimeoutExpired: server.kill();server.wait()
        log.close()
    (out/'results.json').write_text(json.dumps(results,indent=2)+'\n')
print('PASS: main-service read-only dual-certificate association and AARE verified' if MAIN_READ_ONLY else ('PASS: separate-key dual-certificate network checks complete' + ('; device audit verification pending retrieval' if DEVICE else '')), flush=True)

