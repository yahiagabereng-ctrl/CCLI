#!/usr/bin/env python3
"""Create an additive CEI 57-142:2026-07 Aggregator CID; keep the DSO CID unchanged."""
import argparse
import copy
from pathlib import Path
import subprocess
import xml.etree.ElementTree as ET

NS='http://www.iec.ch/61850/2003/SCL'
ET.register_namespace('',NS)
ET.register_namespace('xsi','http://www.w3.org/2001/XMLSchema-instance')
def tag(name): return '{'+NS+'}'+name

def generate(source,output):
    if source.resolve()==output.resolve(): raise ValueError('Use a separate Aggregator CID output')
    tree=ET.parse(source); root=tree.getroot()
    templates=root.find(tag('DataTypeTemplates'))
    types={e.get('id'):e for e in templates}
    def clone(old,new):
        node=copy.deepcopy(types[old]);node.set('id',new);templates.append(node);types[new]=node;return node
    mode=ET.SubElement(templates,tag('EnumType'),id='AggregatorActivation')
    for value,name in [(-5,'inactive'),(-1,'active')]:ET.SubElement(mode,tag('EnumVal'),ord=str(value)).text=name
    behavior=ET.SubElement(templates,tag('EnumType'),id='AggregatorBehavior')
    for value,name in [(-5,'inactive'),(-1,'active'),(5,'off')]:ET.SubElement(behavior,tag('EnumVal'),ord=str(value)).text=name
    for old in ['ENCSelectWithValue_1','ENCOperate_1','ENCCancel_1']:
        node=clone(old,'Aggregator_'+old)
        node.find(tag('BDA')+"[@name='ctlVal']").set('type','AggregatorActivation')
    node=clone('ENC_2_Mod','Aggregator_Mod')
    for da in node:
        if da.get('name')=='stVal':da.set('type','AggregatorActivation')
        elif da.get('name') in ('SBOw','Oper','Cancel'):da.set('type','Aggregator_'+da.get('type'))
    node=clone('ENS_2_Beh','Aggregator_Beh');node.find(tag('DA')+"[@name='stVal']").set('type','AggregatorBehavior')
    # CEI Table 21: +1 is OFF/STOP while plant outputs are unavailable.
    health_enum=ET.SubElement(templates,tag('EnumType'),id='AggregatorHealth')
    for value,name in [(-4,'ACT'),(-3,'WORKING'),(-2,'READY'),(-1,'WAIT'),(1,'OFF'),(3,'KO')]:ET.SubElement(health_enum,tag('EnumVal'),ord=str(value)).text=name
    health=ET.SubElement(templates,tag('DOType'),id='Aggregator_Health',cdc='ENS')
    ET.SubElement(health,tag('DA'),name='stVal',bType='Enum',type='AggregatorHealth',fc='ST',dchg='true')
    ET.SubElement(health,tag('DA'),name='q',bType='Quality',fc='ST',qchg='true')
    ET.SubElement(health,tag('DA'),name='t',bType='Timestamp',fc='ST')
    for old in ('DAGC1','DVAR1'):
        node=clone(old,'Aggregator_'+old)
        for do in node:
            if do.get('name')=='Mod':do.set('type','Aggregator_Mod')
            elif do.get('name')=='Beh':do.set('type','Aggregator_Beh')
        ET.SubElement(node,tag('DO'),name='Health',type='Aggregator_Health')
    meter=clone('MMXU1','Aggregator_MMXU1')
    for do in list(meter):
        if do.get('name') not in ('Beh','TotW','TotVAr'):meter.remove(do)
    ld=root.find('.//'+tag('LDevice')+"[@inst='LD_Plant']")
    if any(e.get('prefix') in ('WSa','VArSa','PdCi') for e in ld.findall(tag('LN'))):
        raise ValueError('Base CID already contains Aggregator nodes')
    for cls,prefix,typ,target in [('DAGC','WSa','Aggregator_DAGC1','WSptPct'),('DVAR','VArSa','Aggregator_DVAR1','VArTgtSptPct'),('MMXU','PdCi','Aggregator_MMXU1',None)]:
        ln=ET.SubElement(ld,tag('LN'),lnClass=cls,prefix=prefix,inst='1',lnType=typ)
        def value(do,da,text):
            doi=next((x for x in ln if x.get('name')==do),None)
            if doi is None:doi=ET.SubElement(ln,tag('DOI'),name=do)
            ET.SubElement(ET.SubElement(doi,tag('DAI'),name=da),tag('Val')).text=text
        if target:
            namespace='(Tr)IEC 61850-CEI57-142:2026A'
            value('NamPlt','vendor','CCLI')
            value('NamPlt','swRev','0.1.0')
            value('NamPlt','configRev','1')
            value('NamPlt','ldNs',namespace)
            value('NamPlt','lnNs',namespace)
        value('Beh','stVal','off')
        if target:
            value('Health','stVal','OFF');value('Mod','stVal','inactive');value('Mod','ctlModel','direct-with-enhanced-security')
            value(target,'ctlModel','direct-with-enhanced-security');value('FctOpSt','stVal','Not Available')
    lln0=ld.find(tag('LN0'))
    ds=ET.Element(tag('DataSet'),name='DS_R_Aggregator_Mis4sec',desc='Aggregator POC P/Q every four seconds')
    for name in ('TotW','TotVAr'):ET.SubElement(ds,tag('FCDA'),ldInst='LD_Plant',prefix='PdCi',lnClass='MMXU',lnInst='1',doName=name,fc='MX')
    pos=next(i for i,e in enumerate(lln0) if e.tag!=tag('DataSet'));lln0.insert(pos,ds)
    for buffered,prefix in [(False,'urcb'),(True,'brcb')]:
        name=prefix+'_Aggregator_Mis4sec'
        rc=ET.Element(tag('ReportControl'),name=name,confRev='1',datSet='DS_R_Aggregator_Mis4sec',rptID='CCI016_01LD_Plant/LLN0.'+name,intgPd='4000',bufTime='0',buffered=str(buffered).lower())
        ET.SubElement(rc,tag('TrgOps'),dchg='true',qchg='true',period='true')
        ET.SubElement(rc,tag('OptFields'),seqNum='true',timeStamp='true',dataSet='true',reasonCode='true',dataRef='true',entryID='false',configRef='true',bufOvfl=str(buffered).lower())
        # CEI 57-142 §4.3.1.4 requires multiple client instances of each report.
        ET.SubElement(rc,tag('RptEnabled'),max='2',desc='Aggregator')
        pos=next((i for i,e in enumerate(lln0) if e.tag not in (tag('DataSet'),tag('ReportControl'))),len(lln0));lln0.insert(pos,rc)
    header=root.find(tag('Header'));header.set('id',header.get('id')+'_Aggregator');header.set('revision','10')
    ET.SubElement(header.find(tag('History')),tag('Hitem'),version=header.get('version'),revision='10',when='2026-10-21',who='CCLI',what='Add Aggregator Table 23 interface',why='CEI 57-142:2026-07; physical outputs unavailable')
    order={tag(name):i for i,name in enumerate(['LNodeType','DOType','DAType','EnumType'])}
    templates[:]=sorted(templates,key=lambda e:order[e.tag])
    output.parent.mkdir(parents=True,exist_ok=True);ET.indent(tree,space='  ');tree.write(output,encoding='utf-8',xml_declaration=True)

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--source',type=Path,required=True);p.add_argument('--output',type=Path,required=True)
    p.add_argument('--jar',type=Path);p.add_argument('--cfg',type=Path)
    a=p.parse_args();generate(a.source,a.output)
    if a.cfg:
        if not a.jar:p.error('--cfg requires the existing --jar model generator')
        subprocess.run(['java','-jar',str(a.jar),str(a.output),'-ied','CCI016_01','-ap','accessPoint1',str(a.cfg)],check=True)
    print(a.output)
