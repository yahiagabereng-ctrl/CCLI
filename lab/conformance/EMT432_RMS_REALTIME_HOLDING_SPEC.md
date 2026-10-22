# EMT432 — RMS realtime holding registers (EMT430_MR)

**Document ID:** CCLI-LAB-EMT432-RMS-RT-001  
**Revision:** 1.0  
**Date:** 2026-10-22  
**MR:** `EMT430_MR.pdf` pages 24–26 (Modbus map ITA Rev 0)  
**Offsets file:** [EMT430_RMS_REGISTER_OFFSETS.txt](EMT430_RMS_REGISTER_OFFSETS.txt)

All entries: **float32**, **2 registers**, access **RO** (peaks 40975+ may be ALWAYS/ALWAYS per MR).  
Decode on EMT432 TCP: **`float_word_swap: true`**. Offset = **PLC − 40001**.

**40906** is **u16** alarm — **not** `V_L1_N`. First star voltage is **40907 / 906**.

---

## 1P cert poll (ccli yaml today)

| PLC | Off | Signal | yaml |
|-----|-----|--------|------|
| 40907 | 906 | V_L1_N | `reg_line_voltage` |
| 40923 | 922 | I_L1 | (diagnostic; add poll later) |
| 40933 | 932 | P1 | `reg_active_power` |
| 40941 | 940 | Q1 | `reg_reactive_power` |
| 40973 | 972 | Frequency | optional |

Do **not** use **938 P_SUM**, **964 CF1**, **986 I_L1_peak**, **980 V_L1_L2_peak** for 1P TotW/PPV without review.

---

## Full RMS block (40907–40985)

| PLC | Off | Name | Unit |
|-----|-----|------|------|
| 40907 | 906 | V_L1_N | V |
| 40909 | 908 | V_L2_N | V |
| 40911 | 910 | V_L3_N | V |
| 40913 | 912 | V_STAR_AVG | V |
| 40915 | 914 | V_L1_L2 | V |
| 40917 | 916 | V_L2_L3 | V |
| 40919 | 918 | V_L3_L1 | V |
| 40921 | 920 | V_LINE_AVG | V |
| 40923 | 922 | I_L1 | A |
| 40925 | 924 | I_L2 | A |
| 40927 | 926 | I_L3 | A |
| 40929 | 928 | I_N | A |
| 40931 | 930 | I_AVG | A |
| 40933 | 932 | P1 | W |
| 40935 | 934 | P2 | W |
| 40937 | 936 | P3 | W |
| 40939 | 938 | P_SUM | W |
| 40941 | 940 | Q1 | VAR |
| 40943 | 942 | Q2 | VAR |
| 40945 | 944 | Q3 | VAR |
| 40947 | 946 | Q_SUM | VAR |
| 40949 | 948 | S1 | VA |
| 40951 | 950 | S2 | VA |
| 40953 | 952 | S3 | VA |
| 40955 | 954 | S_SUM | VA |
| 40957 | 956 | PF1 | — |
| 40959 | 958 | PF2 | — |
| 40961 | 960 | PF3 | — |
| 40963 | 962 | PF_3PH | — |
| 40965 | 964 | CF1 | — |
| 40967 | 966 | CF2 | — |
| 40969 | 968 | CF3 | — |
| 40971 | 970 | CF_N | — |
| 40973 | 972 | Frequency | Hz |
| 40975 | 974 | V_L1_N_peak | V |
| 40977 | 976 | V_L2_N_peak | V |
| 40979 | 978 | V_L3_N_peak | V |
| 40981 | 980 | V_L1_L2_peak | V |
| 40983 | 982 | V_L2_L3_peak | V |
| 40985 | 984 | V_L3_L1_peak | V |

**Single FC3 read:** start offset **906**, count **80** (through 40985 low word).

---

## Tooling

| Tool | Role |
|------|------|
| `chronos_emt432_basjes.yaml` block `realtime_rms_mr_full` | Basjes decode all fields |
| `python lab/emt432_validate_register_map.py` | Offset cross-check |
| `python lab/emt432_rogowski_config.py --read --full-rms` | Bench print full block |
