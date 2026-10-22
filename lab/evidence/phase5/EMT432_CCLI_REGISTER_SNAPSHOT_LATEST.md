# CCLI EMT432 register snapshot
# host=127.0.0.1 unit=1 word_swap=True
# utc=2026-10-22T13:56:42.039438+00:00
# doc=CCLI-LAB-EMT432-CCLI-COMM-001 rev 5 + lab_tr400_emt432_lan3_tcp_102.yaml

## Commissioning / config (holding)
| PLC | Offset | Name | Value | Role |
|-----|--------|------|-------|------|
| 40011 | 10 | mounted_version | 1 | verify Rogowski=1 |
| 40232 | 231 | Measurement_Config | 41616 (0xa290) mode=0 int13=1 filt15=1 esave9=1 | 1P+integrator+filter+energy save |
| 40234 | 233 | TA_Chronos_register | 0 | 0=user TA |
| 40235 | 234 | TA_Transducer_ratio | 11764 | Rogowski sensitivity |
| 40237 | 236 | TA_Transducer_delay | 0 | bench 0 |
| 40239 | 238 | TV_Transducer_ratio | 1 | bench 1.0 |
| 40241 | 240 | TV_Transducer_delay | 0 | bench 0 |
| 40243 | 242 | minimum_voltage_ripple | 0 | bench 0 |
| 40245 | 244 | minimum_current_ripple | 0 | bench 0 |
| 40247 | 246 | minimum_power_ripple | 0 | bench 0 |
| 40256 | 255 | Energy_unit_factor | 4 | 4=kWh |

## Energy int64 [Wh/10] (commissioning log_energy_readings)
| PLC | Offset | Name | Raw | kWh | Note |
|-----|--------|------|-----|-----|------|
| 40716 | 715 | KWh1 Rev2 pin | READ_FAIL | - | |
| 40721 | 720 | KWh1 ITA Rev0 alt | 0 | 0.000000 | |
| 40729 | 728 | KWh_SUM | 0 | 0.000000 | |
| 40765 | 764 | KVARh1 | 0 | 0.000000 | |
| 40845 | 844 | KVAh1 | 0 | 0.000000 | |

## CCLI poll FC3 block (offset 906 count 36)
| PLC | Offset | Signal | Value | MMS / note |
|-----|--------|--------|-------|------------|
| 40907 | 906 | V_L1_N | 231.326 | reg_line_voltage -> MMS PPV |
| 40923 | 922 | I_L1 | 844.61 | diagnostic (not in yaml poll yet) |
| 40933 | 932 | P1 | -151125 | reg_active_power -> MMS TotW |
| 40941 | 940 | Q1 | -123834 | reg_reactive_power -> MMS TotVAr |

## CCLI diagnostic floats (outside poll block)
| PLC | Offset | Signal | Value | Note |
|-----|--------|--------|-------|------|
| 40973 | 972 | Frequency | 49.9875 | optional diagnostic |

## Not polled by CCLI (reference only)
- 40906 Command (offset 905): 0 (do not write casually)

## Full MR realtime block 40907-40985 (Basjes realtime_rms_mr_full)
- 40907 (906) V_L1_N: 231.408
- 40909 (908) V_L2_N: 0
- 40911 (910) V_L3_N: 0
- 40913 (912) V_STAR_AVG: 77.1361
- 40915 (914) V_L1_L2: 231.374
- 40917 (916) V_L2_L3: 0
- 40919 (918) V_L3_L1: 0
- 40921 (920) V_LINE_AVG: 77.1246
- 40923 (922) I_L1: 1111.94
- 40925 (924) I_L2: 0
- 40927 (926) I_L3: 0
- 40929 (928) I_N: 0
- 40931 (930) I_AVG: 370.648
- 40933 (932) P1: -171673
- 40935 (934) P2: 0
- 40937 (936) P3: 0
- 40939 (938) P_SUM: -171673
- 40941 (940) Q1: -191673
- 40943 (942) Q2: 0
- 40945 (944) Q3: 0
- 40947 (946) Q_SUM: -191673
- 40949 (948) S1: 257313
- 40951 (950) S2: 0
- 40953 (952) S3: 0
- 40955 (954) S_SUM: 257313
- 40957 (956) PF1: -0.667175
- 40959 (958) PF2: 0
- 40961 (960) PF3: 0
- 40963 (962) PF_3PH: -0.667175
- 40965 (964) CF1: 2.96306
- 40967 (966) CF2: 1
- 40969 (968) CF3: 1
- 40971 (970) CF_N: 1
- 40973 (972) Frequency: 50.0035
- 40975 (974) V_L1_N_peak: 339.256
- 40977 (976) V_L2_N_peak: 0.178003
- 40979 (978) V_L3_N_peak: 0.196726
- 40981 (980) V_L1_L2_peak: 338.691
- 40983 (982) V_L2_L3_peak: 0.032113
- 40985 (984) V_L3_L1_peak: 60.8474
