from pathlib import Path
import hashlib
import shutil
import tarfile
root=Path(__file__).resolve().parents[1]
bundle=root/'lab/ejbca-staging/rbac'
dual=bundle/'dual-certificate'
stage=dual/'device-stage'
stage.mkdir(exist_ok=True)
for folder in ('package','trust','clients','state'): (stage/folder).mkdir(exist_ok=True)
for file in (dual/'commissioning-sequence3').glob('*.xml'): shutil.copyfile(file,stage/'package'/file.name)
for name in ('dso-mms','viewer-mms'): shutil.copyfile(dual/(name+'.pem'),stage/'clients'/(name+'.pem'))
for source,name in [(bundle/'rbac_ca.pem','rbac_ca.pem'),(bundle/'rbac_crl.pem','rbac_crl.pem'),(bundle/'owner-signer/owner-signer.pem','owner.pem')]:
    shutil.copyfile(source,stage/'trust'/name)
tls=root/'apps/ccli/config/tls'
(stage/'trust/tls-ca-bundle.pem').write_bytes((tls/'root_CA.pem').read_bytes().rstrip()+b'\n'+(bundle/'rbac_ca.pem').read_bytes())
(stage/'trust/tls-crl-bundle.pem').write_bytes((tls/'lab_crl.pem').read_bytes().rstrip()+b'\n'+(bundle/'rbac_crl.pem').read_bytes())
target='/etc/ccli/rbac-test-20261016/dual-certificate'
config=f'''mms:
  enabled: true
  bind_address: 192.168.0.1
  tcp_port: 3783
  comms_loss_fallback_s: 0
  rbac_profile_a: true
  rbac_policy: {target}/package/commissioning-policy.xml
  rbac_owner_cert: {target}/trust/owner.pem
  rbac_activation_state: {target}/state/activation.state
  rbac_identity_ca: {target}/trust/rbac_ca.pem
  rbac_identity_crl: {target}/trust/rbac_crl.pem
  tls_enabled: true
  tls_chain_validation: true
  tls_own_cert: /etc/ccli/tls/server_tls.pem
  tls_own_key: /etc/ccli/tls/server.key
  tls_acse_cert: /etc/ccli/tls/server.pem
  tls_acse_key: /etc/ccli/tls/server_acse.key
  tls_ca_cert: {target}/trust/tls-ca-bundle.pem
  tls_crl: {target}/trust/tls-crl-bundle.pem
  tls_client_cert: /etc/ccli/rbac-clients/dso.pem
  tls_acse_client_cert: /etc/ccli/tls/viewer_tls.pem
  tls_viewer_cert: /etc/ccli/rbac-clients/viewer-r1.pem
  tls_revoked_cert: /etc/ccli/rbac-clients/negative-revoked.pem
  tls_additional_client_cert: /etc/ccli/tls/client_tls.pem
event_log:
  enabled: true
  path: {target}/events.jsonl
  syslog_enabled: false
'''
(stage/'test.yaml').write_bytes(config.encode())
manifest=[]
for file in sorted(stage.rglob('*')):
    if file.is_file() and file.name!='stage.sha256':
        data=file.read_bytes()
        assert b'PRIVATE KEY' not in data and file.suffix not in ('.key','.p12')
        manifest.append(hashlib.sha256(data).hexdigest()+'  '+file.relative_to(stage).as_posix())
(stage/'stage.sha256').write_bytes(('\n'.join(manifest)+'\n').encode())
with tarfile.open(dual/'device-stage.tar.gz','w:gz') as archive:
    for file in sorted(stage.rglob('*')):
        archive.add(file,arcname=file.relative_to(stage).as_posix(),recursive=False)
print('Prepared public-only device archive:',len(manifest),'files')
