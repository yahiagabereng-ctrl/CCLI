#!/usr/bin/env python3
"""Explicit isolated Tespro MMS/IEC104 checks; no controller process or plant I/O."""
import argparse
import json
from pathlib import Path
import shlex
import socket
import ssl
import subprocess
import sys
import tempfile
import time
import uuid

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT/'apps/ccli/tests/integration'))
from tls_remote import Remote


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--host', default='192.168.0.1')
    parser.add_argument('--hostkey', required=True)
    parser.add_argument('--server', type=Path, required=True)
    parser.add_argument('--probe', type=Path, required=True)
    parser.add_argument('--bundle-root', type=Path, required=True)
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    args.out.mkdir(parents=True, exist_ok=True)
    stage = '/etc/ccli/rbac-test-20261016/tls-'+uuid.uuid4().hex
    results = []
    with tempfile.TemporaryDirectory(prefix='ccli-device-adapter-') as tmp:
        fixtures = Path(tmp)
        (fixtures/'candidate.crl.pem').write_bytes((ROOT/'output/iec62351-tls/candidate-tls-crl-bundle.pem').read_bytes())
        remote = Remote(args.host, args.hostkey, args.server, fixtures)
        pid = None
        try:
            before = remote.run('sha256sum /usr/sbin/ccli /etc/ccli/lab.yaml /etc/ccli/rbac-dual/trust/tls-crl-bundle.pem; '
                                'ps w | grep "[/]usr/sbin/ccli --config"')
            (args.out/'installed-before.txt').write_bytes(before)
            remote.run('umask 077; mkdir -p '+shlex.quote(stage+'/package')+' '+shlex.quote(stage+'/state')+
                       '; cp /etc/ccli/rbac-dual/package/*.xml '+shlex.quote(stage+'/package/'))
            config = f'''mms:
  enabled: true
  bind_address: {args.host}
  tcp_port: 3783
  comms_loss_fallback_s: 0
  rbac_profile_a: true
  rbac_policy: {stage}/package/commissioning-policy.xml
  rbac_owner_cert: /etc/ccli/rbac-trust/owner.pem
  rbac_activation_state: {stage}/state/activation.state
  rbac_identity_ca: /etc/ccli/rbac-trust/rbac_ca.pem
  rbac_identity_crl: /etc/ccli/rbac-dual/trust/rbac_crl.pem
  tls_enabled: true
  tls_chain_validation: true
  tls_own_cert: /etc/ccli/tls/server_tls.pem
  tls_own_key: /etc/ccli/tls/server.key
  tls_acse_cert: /etc/ccli/tls/server.pem
  tls_acse_key: /etc/ccli/tls/server_acse.key
  tls_ca_cert: /etc/ccli/rbac-trust/tls-ca-bundle.pem
  tls_crl: {remote.root}/candidate.crl.pem
  tls_client_cert: /etc/ccli/rbac-clients/dso.pem
  tls_acse_client_cert: /etc/ccli/tls/viewer_tls.pem
  tls_viewer_cert: /etc/ccli/rbac-clients/viewer-r1.pem
  tls_revoked_cert: /etc/ccli/rbac-clients/negative-revoked.pem
  tls_additional_client_cert: /etc/ccli/tls/client_tls.pem
  tls_revocation_check_s: 1
  tls_renegotiation_s: 600
iec104:
  enabled: true
  bind_address: {args.host}
  tcp_port: 2405
  comms_loss_fallback_s: 0
  operator_mode: monitor_only
  tls_enabled: true
  tls_own_cert: /etc/ccli/tls/server_tls.pem
  tls_own_key: /etc/ccli/tls/server.key
  tls_ca_cert: /etc/ccli/rbac-trust/tls-ca-bundle.pem
  tls_client_cert: /etc/ccli/tls/client_tls.pem
  tls_crl: {remote.root}/candidate.crl.pem
  tls_revocation_check_s: 1
  tls_renegotiation_s: 600
event_log:
  enabled: true
  path: {stage}/events.jsonl
  syslog_enabled: false
'''
            (args.out/'isolated-config.yaml').write_text(config)
            remote.run('umask 077; cat > '+shlex.quote(remote.root+'/config.yaml'), config.encode())
            command = '(trap "" HUP; exec '+shlex.quote(remote.root+'/probe')+' '+shlex.quote(remote.root+'/config.yaml')+' 240) > '+shlex.quote(remote.root+'/server.log')+' 2>&1 < /dev/null & echo $!'
            pid = int(remote.run(command).decode().strip())
            for _ in range(20):
                log = remote.run('cat '+shlex.quote(remote.root+'/server.log')).decode(errors='replace')
                if 'ISOLATED_RBAC_TEST_READY' in log: break
                time.sleep(.25)
            else: raise RuntimeError('New-build adapter harness failed to start: '+log[-5000:])
            print('PASS new-build ARM64 MMS/IEC104 adapter harness started; plant_io=false', flush=True)
            results.append('new-build-adapter-startup')
            command = [sys.executable, str(ROOT/'scripts/test_mms_dual_certificate.py'), '--device',
                '--bundle-root', str(args.bundle_root), '--output-root', str(args.out/'mms'), '--probe', str(args.probe)]
            result = subprocess.run(command, capture_output=True, text=True, timeout=130)
            (args.out/'mms-campaign.log').write_text(result.stdout+result.stderr)
            print(result.stdout, end='', flush=True)
            if result.returncode: raise RuntimeError('MMS association/read campaign failed; see mms-campaign.log')
            results.append('mms-seven-dual-certificate-cases')

            tls = ROOT/'apps/ccli/config/tls'
            context = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
            context.minimum_version = context.maximum_version = ssl.TLSVersion.TLSv1_2
            context.check_hostname = False
            context.load_verify_locations(str(tls/'root_CA.pem'))
            context.load_cert_chain(str(tls/'client_tls.pem'), str(tls/'client_tsp.key'))
            with socket.create_connection((args.host,2405),timeout=5) as raw:
                with context.wrap_socket(raw,server_hostname='test') as connection:
                    connection.sendall(bytes.fromhex('680407000000'))  # STARTDT act, no control ASDU
                    received = b''
                    until = time.monotonic()+5
                    while bytes.fromhex('68040b000000') not in received and time.monotonic()<until:
                        received += connection.recv(1024)
                    assert bytes.fromhex('68040b000000') in received, 'IEC104 STARTDT confirmation missing'
                    # General interrogation reads monitoring data; no setpoint/control ASDU.
                    connection.sendall(bytes.fromhex('680e0000000064010600010000000014'))
                    frames = []
                    pending = b''
                    confirmed = completed = False
                    next_receive = 0
                    until = time.monotonic()+10
                    while not completed:
                        if time.monotonic() >= until: raise TimeoutError('IEC104 interrogation deadline')
                        chunk = connection.recv(4096)
                        if not chunk: raise EOFError('IEC104 closed during interrogation')
                        pending += chunk
                        while len(pending) >= 2 and len(pending) >= pending[1]+2:
                            frame, pending = pending[:pending[1]+2], pending[pending[1]+2:]
                            frames.append(frame.hex())
                            if len(frame) >= 10 and not frame[2]&1:
                                next_receive = ((frame[2] | frame[3]<<8)>>1)+1
                                if frame[6] == 100:
                                    confirmed |= frame[8]&63 == 7
                                    completed |= frame[8]&63 == 10
                    assert confirmed and completed, 'IEC104 interrogation confirmation/termination missing'
                    connection.sendall(b'\x68\x04\x01\x00'+(next_receive<<1).to_bytes(2,'little'))
                    # Stop streaming before TLS close_notify, then drain its confirmation.
                    connection.sendall(bytes.fromhex('680413000000'))
                    stopped = b''
                    stop_pending = pending
                    stop_confirmed = False
                    while not stop_confirmed:
                        chunk = connection.recv(4096)
                        if not chunk: raise EOFError('IEC104 closed before STOPDT confirmation')
                        stopped += chunk
                        stop_pending += chunk
                        while len(stop_pending) >= 2 and len(stop_pending) >= stop_pending[1]+2:
                            frame = stop_pending[:stop_pending[1]+2]
                            stop_pending = stop_pending[stop_pending[1]+2:]
                            if not frame[2]&1:
                                number = (((frame[2] | frame[3]<<8)>>1)+1)<<1
                                connection.sendall(b'\x68\x04\x01\x00'+number.to_bytes(2,'little'))
                            elif frame == bytes.fromhex('680423000000'):
                                stop_confirmed = True
                    (args.out/'iec104-startdt.json').write_text(json.dumps({
                        'tls_version':connection.version(),'cipher':connection.cipher(),
                        'received_hex':received.hex(),'general_interrogation_frames':frames,
                        'stopdt_confirmation_hex':stopped.hex(),
                        'control_asdu_sent':False},indent=2)+'\n')
                    connection.unwrap().close()
            print('PASS new-build IEC104 mutual TLS, STARTDT and general interrogation; no control ASDU sent', flush=True)
            results.append('iec104-tls-startdt-and-interrogation')
        finally:
            if pid is not None:
                remote.run(f'kill -TERM {pid} 2>/dev/null || true')
                time.sleep(.5)
                (args.out/'device-server.log').write_bytes(remote.run('cat '+shlex.quote(remote.root+'/server.log')))
                if 'mms-seven-dual-certificate-cases' in results:
                    text = (args.out/'device-server.log').read_text()
                    for reason in ('replayed authentication proof', 'MMS authentication signature invalid',
                        'stale or future authentication proof', 'certificate AP/AE does not match calling application reference',
                        'transport/identity pairing mismatch; all access denied'):
                        assert reason in text, 'Device rejection audit missing: '+reason
                    assert 'TLS_SECURITY_EVENT severity=notice code=16' in text, 'IEC104 security event missing'
                    results.append('device-rejection-audits-verified')
            (args.out/'security-events.jsonl').write_bytes(remote.run('cat '+shlex.quote(stage+'/events.jsonl')+' 2>/dev/null || true'))
            after = remote.run('sha256sum /usr/sbin/ccli /etc/ccli/lab.yaml /etc/ccli/rbac-dual/trust/tls-crl-bundle.pem; '
                               'ps w | grep "[/]usr/sbin/ccli --config"')
            (args.out/'installed-after.txt').write_bytes(after)
            original = before.decode().splitlines()
            current = after.decode().splitlines()
            assert original[:3] == current[:3] and original[3].split()[0] == current[3].split()[0], \
                'Installed service hash/PID/config changed during test'
            results.append('installed-service-unchanged')
            remote.run('rm -rf -- '+shlex.quote(stage))
            remote.close()
            (args.out/'results.json').write_text(json.dumps(results,indent=2)+'\n')


if __name__ == '__main__': main()
