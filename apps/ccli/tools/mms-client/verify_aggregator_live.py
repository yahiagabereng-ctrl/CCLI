#!/usr/bin/env python3
"""Explicit low-power proof on the known COM8/Tespro bench, with cleanup."""
import argparse
import copy
import json
import os
from pathlib import Path
import subprocess
import time
from cryptography import x509
from cryptography.hazmat.primitives import serialization
import client


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--pki', type=Path, required=True)
    p.add_argument('--output', type=Path, required=True)
    p.add_argument('--yes', action='store_true')
    o = p.parse_args()
    if not o.yes: p.error('This sends the explicit bench P/Q controls; --yes is required')
    password = os.environ['CCLI_TESPRO_PASSWORD']
    o.output.mkdir(parents=True, exist_ok=True)
    records = []
    state_path = client.ROOT/'lab/inverter-simulator-bridge/state.json'
    original_state = state_path.read_text(encoding='utf-8-sig')
    state = json.loads(original_state)
    if state.get('mode') != 'fixed' or float(state['power_kw']) != 10:
        raise RuntimeError('This proof requires the known fixed 10 kW simulator state')
    def status():
        r = subprocess.run(['/mnt/c/Program Files/PuTTY/plink.exe','-batch','-ssh','root@192.168.0.1',
            '-pw',password,'-hostkey','SHA256:fy0pZiLnyHHY6hKpcR7ukqBAb22qfNxU72Qpv/hseJM',
            'cat /var/run/ccli-status.json'], check=True, capture_output=True, text=True)
        return json.loads(r.stdout)
    def save(name, **data):
        records.append(dict(case=name, **data))
        (o.output/'results.json').write_text(json.dumps(records,indent=2)+'\n')
        print('PASS: '+name, flush=True)
    base = argparse.Namespace(host='192.168.0.1',port=3782,auth_mode='tls',
        ca=client.ROOT/'apps/ccli/config/tls/root_CA.pem',
        binary=client.ROOT/'apps/ccli/build-mms-client/ccli-mms-client',
        ap_title='1.1.1.999.1',ae_qualifier=12,verbose=False,
        command='read',reference='',fc='ST',type='int32',count=1,value=None,members=None,yes=True,report=None)
    def session(label):
        a = copy.copy(base)
        a.tls_cert,a.tls_key=o.pki/label/(label+'.pem'),o.pki/label/(label+'.key')
        cert=x509.load_pem_x509_certificate(a.tls_cert.read_bytes())
        key=serialization.load_pem_private_key(a.tls_key.read_bytes(),password=None)
        s=client.PersistentSession(a,cert,key)
        s.test_args,s.test_cert,s.test_key=a,cert,key
        return s
    def request(s,name,cmd,ref,**kw):
        a=copy.copy(s.test_args)
        a.command,a.reference=cmd,client.PLANT+'/'+ref
        for k,v in kw.items(): setattr(a,k,v)
        code,events=client.run_operation(a,s.test_cert,s.test_key,quiet=True,session=s)
        if code: raise RuntimeError(name+': '+json.dumps(events))
        save(name,events=events)
        return events
    def wait_status(name,predicate):
        deadline=time.monotonic()+45
        while time.monotonic()<deadline:
            current=status()
            (o.output/'last-observed-status.json').write_text(json.dumps(current,indent=2)+'\n')
            if predicate(current): save(name,status=current); return
            time.sleep(.5)
        raise RuntimeError(name+' timed out')
    initial=status()
    z=initial['zones']['dso']
    if any(z.get(k) for k in ('wlim_active','varsd_active','pfsp_active','wsa_active','varsa_active')):
        raise RuntimeError('Another live power function is active; refusing to override it')
    if initial['quality'] != 'good' or initial['p_kw'] > 15 or initial['do_curtail_on']:
        raise RuntimeError('Bench power/output state does not match this proof')
    save('initial-state',status=initial)
    ap,aq='WSaDAGC1','VArSaDVAR1'
    controls_started=False
    q_started=False
    with session('aggregator') as agg, session('dso') as monitor:
        try:
            controls_started=True
            request(agg,'P-setpoint-10pct','operate',ap+'.WSptPct',value='10',type='float')
            request(agg,'P-mode-on','operate',ap+'.Mod',value='1')
            wait_status('export-ceiling-21kW-with-output-off',lambda s:
                abs(s['derived']['p_effective_kw']-21)<.01 and not s['do_curtail_on'])
            time.sleep(3.1)
            request(agg,'Q-setpoint-2pct','operate',aq+'.VArTgtSptPct',value='2',type='float')
            request(agg,'Q-mode-on','operate',aq+'.Mod',value='1')
            q_started=True
            wait_status('Q-4point2kvar-roundtrip-over-RS485',lambda s: abs(s['q_kvar']-4.2)<.02)
            events=request(monitor,'Q-4point2kvar-read-via-MMS','read','PdCMMXU1.TotVAr.mag.f',fc='MX')
            if not any(e['kind']=='read' and abs(float(e['value'])-4.2)<.02 for e in events):
                raise RuntimeError('MMS Q did not match serial command')
        finally:
            if controls_started:
                errors=[]
                for name, ref in [('Q',aq+'.Mod'),('P',ap+'.Mod')]:
                    try: request(agg,'cleanup-'+name+'-off','operate',ref,value='5')
                    except Exception as exc: errors.append(str(exc))
                if q_started:
                    try: wait_status('Q-off-clears-serial-command',lambda s: abs(s['q_kvar'])<.02)
                    except Exception as exc: errors.append(str(exc))
                # Restore the pre-test setpoint values and mock electrical state.
                time.sleep(3.1)
                request(agg,'cleanup-P-default','operate',ap+'.WSptPct',value='100',type='float')
                time.sleep(3.1)
                request(agg,'cleanup-Q-default','operate',aq+'.VArTgtSptPct',value='0',type='float')
                state_path.write_text(original_state)
                wait_status('restored-original-mock-state-and-modes-off',lambda s:
                    abs(s['q_kvar']-float(state['q_kvar']))<.02 and
                    not s['zones']['dso']['wsa_active'] and not s['zones']['dso']['varsa_active'] and
                    not s['do_curtail_on'])
                if errors: raise RuntimeError('Cleanup needs review: '+'; '.join(errors))
    print('RESULT: live Aggregator proof and cleanup passed',flush=True)


if __name__=='__main__': main()
