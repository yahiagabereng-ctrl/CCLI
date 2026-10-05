# P5 — LN,DO.xlsx row-by-row compare vs `lab_tg544_eth_a`

**Source:** `C:\Yahia\projects\CCLI\New folder\LN,DO.xlsx`
**CID:** `apps\ccli\config\icd\lab_tg544_eth_a.cid` (31 LNs)
**CFG:** `apps\ccli\config\icd\lab_tg544_eth_a.cfg` (31 LNs)

**Verdict legend:** `MATCH` = DO on all CID instances + cfg · `PARTIAL`/`MISSING` = gaps · `NO_LN_CLASS` = lnClass absent

| Row | LN class | DO | Result (CCI) | XLSX CID col | **Our verdict** | Detail | Missing instances |
|-----|----------|----|--------------|--------------|-----------------|--------|-------------------|
| 2 | `LLN0` | `Beh` | MANDATORY | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 3 | `LLN0` | `Mod` | MANDATORY | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 4 | `LLN0` | `Health` | MANDATORY | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 5 | `LLN0` | `NamPlt` | MANDATORY | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 6 | `LPHD` | `PhyNam` | MANDATORY | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 7 | `LPHD` | `PhyHealth` | MANDATORY | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 8 | `LPHD` | `Proxy` | MANDATORY | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 9 | `DPCC` | `ElcRefId` | MANDATORY | YES (all 5 inst.) | **MATCH** | CID+CFG all 5 inst. | — |
| 10 | `DPCC` | `VRef` | MANDATORY | YES (all 5 inst.) | **MATCH** | CID+CFG all 5 inst. | — |
| 11 | `DPCC` | `VArRtg` | REQUIRED (Annex T, for Annex O function) | PARTIAL | **MATCH** | CID+CFG all 5 inst. | — |
| 12 | `DPCC` | `VARtg` | REQUIRED (Annex T, for Annex O function) | PARTIAL | **MATCH** | CID+CFG all 5 inst. | — |
| 13 | `DPCC` | `WRtg` | REQUIRED (Annex T, for Annex O function) | PARTIAL | **MATCH** | CID+CFG all 5 inst. | — |
| 14 | `DPCC` | `RefFrm` | MANDATORY | YES (all 5 inst.) | **MATCH** | CID+CFG all 5 inst. | — |
| 15 | `DPCC` | `AreaEpsEcpId` | MANDATORY | YES (all 5 inst.) | **MATCH** | CID+CFG all 5 inst. | — |
| 16 | `DPCC` | `AreaEpsWMax` | MANDATORY | YES (all 5 inst.) | **MATCH** | CID+CFG all 5 inst. | — |
| 17 | `DPCC` | `Beh` | MANDATORY | YES (all 5 inst.) | **MATCH** | CID+CFG all 5 inst. | — |
| 18 | `DECP` | `ElcRefId` | MANDATORY | PARTIAL | **MATCH** | CID+CFG all 3 inst. | — |
| 19 | `DECP` | `VRef` | MANDATORY | PARTIAL | **MATCH** | CID+CFG all 3 inst. | — |
| 20 | `DECP` | `VMax` | REQUIRED (Annex T, for Annex O function) | NO | **MATCH** | CID+CFG all 3 inst. | — |
| 21 | `DECP` | `VMin` | REQUIRED (Annex T, for Annex O function) | NO | **MATCH** | CID+CFG all 3 inst. | — |
| 22 | `DECP` | `RefFrm` | MANDATORY | PARTIAL | **MATCH** | CID+CFG all 3 inst. | — |
| 23 | `DECP` | `Beh` | MANDATORY | YES (all 3 inst.) | **MATCH** | CID+CFG all 3 inst. | — |
| 24 | `DGEN` | `OutEcpRef` | MANDATORY | PARTIAL | **MATCH** | CID+CFG all 3 inst. | — |
| 25 | `DGEN` | `PhsConnTyp` | MANDATORY | PARTIAL | **MATCH** | CID+CFG all 3 inst. | — |
| 26 | `DGEN` | `DERTyp` | MANDATORY | PARTIAL | **MATCH** | CID+CFG all 3 inst. | — |
| 27 | `DGEN` | `DEROpSt` | MANDATORY | YES (all 3 inst.) | **MATCH** | CID+CFG all 3 inst. | — |
| 28 | `DGEN` | `WMax` | MANDATORY | NO | **MATCH** | CID+CFG all 3 inst. | — |
| 29 | `DGEN` | `WRmp` | MANDATORY | NO | **MATCH** | CID+CFG all 3 inst. | — |
| 30 | `DGEN` | `VMax` | MANDATORY | NO | **MATCH** | CID+CFG all 3 inst. | — |
| 31 | `DGEN` | `WMaxRtg` | MANDATORY | NO | **MATCH** | CID+CFG all 3 inst. | — |
| 32 | `DGEN` | `VMaxRtg` | MANDATORY | NO | **MATCH** | CID+CFG all 3 inst. | — |
| 33 | `DGEN` | `GnOpSt` | MANDATORY | NO | **MATCH** | CID+CFG all 3 inst. | — |
| 34 | `DGEN` | `OpTmsRs` | MANDATORY | NO | **MATCH** | CID+CFG all 3 inst. | — |
| 35 | `DGEN` | `Beh` | MANDATORY | YES (all 3 inst.) | **MATCH** | CID+CFG all 3 inst. | — |
| 36 | `DGEN` | `Health` | REQUIRED (Annex T, for Annex O function) | PARTIAL | **PARTIAL** | CID 2/3; CFG 2/3 | DisFRDGEN1 |
| 37 | `DGEN` | `GnGrId` | REQUIRED (CEI extension) | PARTIAL | **PARTIAL** | CID 2/3; CFG 2/3 | DisFRDGEN1 |
| 38 | `DSTO` | `OutEcpRef` | MANDATORY | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 39 | `DSTO` | `PhsConnTyp` | MANDATORY | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 40 | `DSTO` | `DERTyp` | MANDATORY | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 41 | `DSTO` | `DEROpSt` | MANDATORY | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 42 | `DSTO` | `EffWh` | Optional | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 43 | `DSTO` | `EffWhPct` | Optional | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 44 | `DSTO` | `ChaWMax` | MANDATORY | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 45 | `DSTO` | `EqSto` | MANDATORY | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 46 | `DSTO` | `Beh` | MANDATORY | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 47 | `MMXU` | `Beh` | MANDATORY | PARTIAL | **MATCH** | CID+CFG all 8 inst. | — |
| 48 | `MMXU` | `TotW` | REQUIRED (Annex T, for Annex O function) | PARTIAL | **MATCH** | CID+CFG all 8 inst. | — |
| 49 | `MMXU` | `TotVAr` | REQUIRED (Annex T, for Annex O function) | PARTIAL | **MATCH** | CID+CFG all 8 inst. | — |
| 50 | `MMXU` | `PPV` | REQUIRED (Annex T, for Annex O function) | PARTIAL | **MATCH** | CID+CFG all 8 inst. | — |
| 51 | `MMXU` | `A` | Optional | PARTIAL | **MATCH** | CID+CFG all 8 inst. | — |
| 52 | `XCBR` | `Beh` | MANDATORY | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 53 | `XCBR` | `Loc` | MANDATORY | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 54 | `XCBR` | `OpCnt` | MANDATORY | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 55 | `XCBR` | `Pos` | MANDATORY | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 56 | `XCBR` | `BlkOpn` | MANDATORY | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 57 | `XCBR` | `BlkCls` | MANDATORY | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 58 | `DWMX` | `InEcpRef` | MANDATORY | NO | **MATCH** | CID+CFG all 1 inst. | — |
| 59 | `DWMX` | `RmpRteUse` | MANDATORY | NO | **MATCH** | CID+CFG all 1 inst. | — |
| 60 | `DWMX` | `WMaxSptPct` | REQUIRED (Annex T, for Annex O function) | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 61 | `DWMX` | `WMaxSpt` | Optional | NO | **MATCH** | CID+CFG all 1 inst. (audit `WMaxSpt` → CID `WMaxSptPct`) | — |
| 62 | `DWMX` | `Beh` | MANDATORY | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 63 | `DWMX` | `Mod` | REQUIRED (Annex T, for Annex O function) | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 64 | `DWMX` | `FctOpStAuto` | REQUIRED (CEI extension) | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 65 | `DWMX` | `FctOpStEx` | REQUIRED (CEI extension) | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 66 | `DAGC` | `InEcpRef` | MANDATORY | NO | **MATCH** | CID+CFG all 1 inst. | — |
| 67 | `DAGC` | `RmpRteUse` | MANDATORY | NO | **MATCH** | CID+CFG all 1 inst. | — |
| 68 | `DAGC` | `ReqW` | MANDATORY | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 69 | `DAGC` | `WSpt` | Optional | NO | **MATCH** | CID+CFG all 1 inst. (audit `WSpt` → CID `WSptPct`) | — |
| 70 | `DAGC` | `WSptPct` | REQUIRED (Annex T, for Annex O function) | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 71 | `DAGC` | `Beh` | MANDATORY | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 72 | `DAGC` | `Mod` | REQUIRED (Annex T, for Annex O function) | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 73 | `DAGC` | `FctOpSt` | REQUIRED (CEI extension) | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 74 | `DVAR` | `InEcpRef` | MANDATORY | NO | **MATCH** | CID+CFG all 1 inst. | — |
| 75 | `DVAR` | `RmpRteUse` | MANDATORY | NO | **MATCH** | CID+CFG all 1 inst. | — |
| 76 | `DVAR` | `ReqVAr` | MANDATORY | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 77 | `DVAR` | `VArTgtSpt` | Optional | NO | **MATCH** | CID+CFG all 1 inst. (audit `VArTgtSpt` → CID `VArTgtSptPct`) | — |
| 78 | `DVAR` | `VArTgtSptPct` | REQUIRED (Annex T, for Annex O function) | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 79 | `DVAR` | `Beh` | MANDATORY | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 80 | `DVAR` | `Mod` | REQUIRED (Annex T, for Annex O function) | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 81 | `DVAR` | `FctOpSt` | REQUIRED (CEI extension) | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 82 | `DFPF` | `InEcpRef` | MANDATORY | NO | **MATCH** | CID+CFG all 1 inst. | — |
| 83 | `DFPF` | `RmpRteUse` | MANDATORY | NO | **MATCH** | CID+CFG all 1 inst. | — |
| 84 | `DFPF` | `ReqPFExt` | MANDATORY | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 85 | `DFPF` | `ReqPF` | MANDATORY | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 86 | `DFPF` | `PFGnTgtSpt` | MANDATORY | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 87 | `DFPF` | `PFLodTgtSpt` | REQUIRED (Annex T, for Annex O function) | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 88 | `DFPF` | `Beh` | MANDATORY | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 89 | `DFPF` | `Mod` | REQUIRED (Annex T, for Annex O function) | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 90 | `DFPF` | `FctOpSt` | REQUIRED (CEI extension) | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 91 | `DVVR` | `InEcpRef` | MANDATORY | NO | **MATCH** | CID+CFG all 1 inst. | — |
| 92 | `DVVR` | `RmpRteUse` | MANDATORY | NO | **MATCH** | CID+CFG all 1 inst. | — |
| 93 | `DVVR` | `ReqVAr` | MANDATORY | NO | **MATCH** | CID+CFG all 1 inst. | — |
| 94 | `DVVR` | `VArTgtSpt` | Optional | NO | **MATCH** | CID+CFG all 1 inst. (audit `VArTgtSpt` → CID `VArTgtSptPct`) | — |
| 95 | `DVVR` | `VArTgtSptPct` | Optional | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 96 | `DVVR` | `VRefEsp` | MANDATORY | NO | **MATCH** | CID+CFG all 1 inst. | — |
| 97 | `DVVR` | `VVArCrv` | Optional | NO | **MATCH** | CID+CFG all 1 inst. | — |
| 98 | `DVVR` | `VVArCrvDel` | Optional | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 99 | `DVVR` | `Beh` | MANDATORY | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 100 | `DVVR` | `Mod` | REQUIRED (Annex T, for Annex O function) | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 101 | `DVVR` | `FctOpSt` | REQUIRED (CEI extension) | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 102 | `DVVR` | `K` | REQUIRED (CEI extension) | NO | **MATCH** | CID+CFG all 1 inst. | — |
| 103 | `DPMC` | `DERRef` | MANDATORY | NO | **MATCH** | CID+CFG all 2 inst. | — |
| 104 | `DPMC` | `OutEcpRef` | MANDATORY | NO | **MATCH** | CID+CFG all 2 inst. | — |
| 105 | `DPMC` | `WSpt1` | REQUIRED (Annex T, for Annex O function) | YES (all 2 inst.) | **MATCH** | CID+CFG all 2 inst. | — |
| 106 | `DPMC` | `Beh` | MANDATORY | YES (all 2 inst.) | **MATCH** | CID+CFG all 2 inst. | — |
| 107 | `DPFW` | `Beh` | MANDATORY | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 108 | `DPFW` | `Mod` | REQUIRED (Annex T, for Annex O function) | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 109 | `DPFW` | `FctOpSt` | MANDATORY | YES (all 1 inst.) | **MATCH** | CID+CFG all 1 inst. | — |
| 110 | `DPFW` | `WSetA` | MANDATORY | NO | **MATCH** | CID+CFG all 1 inst. | — |
| 111 | `DPFW` | `PFSetA` | MANDATORY | NO | **MATCH** | CID+CFG all 1 inst. | — |
| 112 | `DPFW` | `WSetB` | MANDATORY | NO | **MATCH** | CID+CFG all 1 inst. | — |
| 113 | `DPFW` | `PFSetB` | MANDATORY | NO | **MATCH** | CID+CFG all 1 inst. | — |
| 114 | `DPFW` | `WSetC` | MANDATORY | NO | **MATCH** | CID+CFG all 1 inst. | — |
| 115 | `DPFW` | `PFSetC` | MANDATORY | NO | **MATCH** | CID+CFG all 1 inst. | — |
| 116 | `DPFW` | `VLkIn` | MANDATORY | NO | **MATCH** | CID+CFG all 1 inst. | — |
| 117 | `DPFW` | `VLkOut` | MANDATORY | NO | **MATCH** | CID+CFG all 1 inst. | — |

## Summary

| Verdict | Count |
|---------|------:|
| MATCH | 114 |
| PARTIAL | 2 |

**Total rows:** 116

## Intentional PARTIAL (regulatory — do not add to DisFRDGEN1)

| LN | DO | Missing on | Reason |
|----|----|-----------:|--------|
| `DGEN` | `Health` | `DisFRDGEN1` | Annex T: Health only on SSGG instances (SSGGDGEN1/2 have it) |
| `DGEN` | `GnGrId` | `DisFRDGEN1` | Annex T CEI extension: GnGrId only on SSGG instances |

## Disagreements vs xlsx «In your CID?» column

- R20 DECP.VMax: xlsx says NO, we say MATCH (CID+CFG all 3 inst.)
- R21 DECP.VMin: xlsx says NO, we say MATCH (CID+CFG all 3 inst.)
- R28 DGEN.WMax: xlsx says NO, we say MATCH (CID+CFG all 3 inst.)
- R29 DGEN.WRmp: xlsx says NO, we say MATCH (CID+CFG all 3 inst.)
- R30 DGEN.VMax: xlsx says NO, we say MATCH (CID+CFG all 3 inst.)
- R31 DGEN.WMaxRtg: xlsx says NO, we say MATCH (CID+CFG all 3 inst.)
- R32 DGEN.VMaxRtg: xlsx says NO, we say MATCH (CID+CFG all 3 inst.)
- R33 DGEN.GnOpSt: xlsx says NO, we say MATCH (CID+CFG all 3 inst.)
- R34 DGEN.OpTmsRs: xlsx says NO, we say MATCH (CID+CFG all 3 inst.)
- R58 DWMX.InEcpRef: xlsx says NO, we say MATCH (CID+CFG all 1 inst.)
- R59 DWMX.RmpRteUse: xlsx says NO, we say MATCH (CID+CFG all 1 inst.)
- R61 DWMX.WMaxSpt: xlsx says NO, we say MATCH (CID+CFG all 1 inst. (audit `WMaxSpt` → CID `WMaxSptPct`))
- R66 DAGC.InEcpRef: xlsx says NO, we say MATCH (CID+CFG all 1 inst.)
- R67 DAGC.RmpRteUse: xlsx says NO, we say MATCH (CID+CFG all 1 inst.)
- R69 DAGC.WSpt: xlsx says NO, we say MATCH (CID+CFG all 1 inst. (audit `WSpt` → CID `WSptPct`))
- R74 DVAR.InEcpRef: xlsx says NO, we say MATCH (CID+CFG all 1 inst.)
- R75 DVAR.RmpRteUse: xlsx says NO, we say MATCH (CID+CFG all 1 inst.)
- R77 DVAR.VArTgtSpt: xlsx says NO, we say MATCH (CID+CFG all 1 inst. (audit `VArTgtSpt` → CID `VArTgtSptPct`))
- R82 DFPF.InEcpRef: xlsx says NO, we say MATCH (CID+CFG all 1 inst.)
- R83 DFPF.RmpRteUse: xlsx says NO, we say MATCH (CID+CFG all 1 inst.)
- R91 DVVR.InEcpRef: xlsx says NO, we say MATCH (CID+CFG all 1 inst.)
- R92 DVVR.RmpRteUse: xlsx says NO, we say MATCH (CID+CFG all 1 inst.)
- R93 DVVR.ReqVAr: xlsx says NO, we say MATCH (CID+CFG all 1 inst.)
- R94 DVVR.VArTgtSpt: xlsx says NO, we say MATCH (CID+CFG all 1 inst. (audit `VArTgtSpt` → CID `VArTgtSptPct`))
- R96 DVVR.VRefEsp: xlsx says NO, we say MATCH (CID+CFG all 1 inst.)
- R97 DVVR.VVArCrv: xlsx says NO, we say MATCH (CID+CFG all 1 inst.)
- R102 DVVR.K: xlsx says NO, we say MATCH (CID+CFG all 1 inst.)
- R103 DPMC.DERRef: xlsx says NO, we say MATCH (CID+CFG all 2 inst.)
- R104 DPMC.OutEcpRef: xlsx says NO, we say MATCH (CID+CFG all 2 inst.)
- R110 DPFW.WSetA: xlsx says NO, we say MATCH (CID+CFG all 1 inst.)
- R111 DPFW.PFSetA: xlsx says NO, we say MATCH (CID+CFG all 1 inst.)
- R112 DPFW.WSetB: xlsx says NO, we say MATCH (CID+CFG all 1 inst.)
- R113 DPFW.PFSetB: xlsx says NO, we say MATCH (CID+CFG all 1 inst.)
- R114 DPFW.WSetC: xlsx says NO, we say MATCH (CID+CFG all 1 inst.)
- R115 DPFW.PFSetC: xlsx says NO, we say MATCH (CID+CFG all 1 inst.)
- R116 DPFW.VLkIn: xlsx says NO, we say MATCH (CID+CFG all 1 inst.)
- R117 DPFW.VLkOut: xlsx says NO, we say MATCH (CID+CFG all 1 inst.)

## CID instances by lnClass

| lnClass | Instances |
|---------|-----------|
| `DAGC` | `WSdDAGC1` |
| `DECP` | `DisFRDECP1`, `VArVDECP1`, `VArVDECP2` |
| `DFPF` | `PFSPDFPF1` |
| `DGEN` | `DisFRDGEN1`, `SSGGDGEN1`, `SSGGDGEN2` |
| `DPCC` | `PdC_QcDPCC1`, `PdC_QiDPCC1`, `PdC_VADPCC1`, `PdC_WaDPCC1`, `PdC_WiDPCC1` |
| `DPFW` | `PFWDPFW1` |
| `DPMC` | `VArVDPMC1`, `VArVDPMC2` |
| `DSTO` | `DisFRDSTO1` |
| `DVAR` | `VArSdDVAR1` |
| `DVVR` | `VArVDVVR1` |
| `DWMX` | `WlimDWMX1` |
| `LLN0` | `LLN0` |
| `LPHD` | `LPHD1` |
| `MMXU` | `GenIdrMMXU1`, `GenPVMMXU1`, `GenTerMMXU1`, `GenWiMMXU1`, `PdCMMXU1`, `SGGMMXU1`, `SGGMMXU2`, `StMMXU1` |
| `XCBR` | `IDGXCBR1` |
