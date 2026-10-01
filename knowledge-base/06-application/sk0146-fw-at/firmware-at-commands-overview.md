# SK0146 FW — AT commands overview & enable map

**source_id:** `fw-sk0146-at-commands-overview`  
**Product:** SK0146-EVC  
**MCU:** STM32L152VE  
**Archive:** `FW-20260707T121440Z-3-001/.../EVC/SK0146 dlms/`  
**Canonical implementation:** `Src/at_commands.c`

## Scope

House **AT command catalog** and compile-time enable flags from the SK0146 DLMS firmware tree.  
For coding rules → `fw-sk0146-coding-conventions`. For grammar → `fw-sk0146-at-syntax`.

**Not CCI Allegato T firmware.** Reuse AT style only; domain is gas EVC (DLMS + GSM + metrology).

## Stack context

| Layer | SK0146 |
|-------|--------|
| RTOS | FreeRTOS (CMSIS-RTOS) |
| HAL | STM32L1xx HAL + `*_my` wrappers |
| Meter protocol | Gurux DLMS (`Development/`, `dlms_grx.c`) |
| Factory/debug | AT over UART / IR / RS-485 / GSM |
| Cellular | GSM module control (`gsm.c`, `transmission_gprs.c`, `com_module.c`) |

## Command catalog (NOTES list + STATUS)

| # | Command | STATUS (1=on) | Production note |
|---|---------|---------------|-----------------|
| 2 | `AT+DEBUG` | 1 | — |
| 3 | `AT+RESET` | 1 | — |
| 4 | `AT+VERSION` | 1 | mandatory test bench |
| 5 | `AT+DEFAULT` | 0 | mandatory SMQ (disabled in this build) |
| 6 | `AT+GPI` | 1 | mandatory manual board test |
| 7 | `AT+GPO` | 1 | — |
| 8 | `AT+FACTORY` | 1 | mandatory test bench |
| 9 | `AT+PROTOCOL` | 1 | mandatory SMQ |
| 10 | `AT+TEST` | 0 | mandatory auto board test (off) |
| 12 | `AT+RTOS_TASK` | 0 | — |
| 13 | `AT+RTOS_HEAP` | 0 | — |
| 15 | `AT+UNIX_TIME` | 0 | — |
| 17 | `AT+BATTERY` | 1 | mandatory manual board test |
| 18 | `AT+BATTERY_LIFE` | 1 | — |
| 32 | `AT+IR_PORT` | 0 | — |
| 33 | `AT+IR_TIME` | 1 | — |
| 34 | `AT+RTC_ALARM` | 1 | — |
| 35 | `AT+RTC_MODE` | 0 | — |
| 36 | `AT+EEPROM` | 0 | — |
| 38 | `AT+GSM_TEST` | 1 | — |
| 40 | `AT+SET_PRESSURE_SENSOR_SN` | 1 | mandatory test bench |
| 41 | `AT+SET_TEMPERATURE_SENSOR_SN` | 1 | mandatory test bench |
| 42 | `AT+SET_LDN` | 1 | mandatory MEX |
| 43 | `AT+SET_PIN` | 0 | — |
| 44 | `AT+SET_APN` | 1 | — |
| 45 | `AT+SET_DNS` | 0 | — |
| 46 | `AT+SET_HOST` | 1 | — |
| 49 | `AT+CRC32` | 0 | — |
| 50 | `AT+SIGNATURE` | 0 | — |
| 51 | `AT+INTELHEX` | 0 | — |
| 55 | `AT+SET_KEYS` | 1 | mandatory MEX |
| 58 | `AT+METER_DATA` | 0 | — |
| 61 | `AT+GSM_ON` | 1 | mandatory test bench |
| 62 | `AT+CALLS` | 0 | — |
| 63 | `AT+SET_SOCKET_TYPE` | 1 | mandatory board test |
| 64 | `AT+GSM_INFO` | 1 | mandatory test bench |
| 65 | `AT+SIM_INFO` | 1 | mandatory test bench |
| 66 | `AT+GSM_STATUS` | 1 | — |
| 67 | `AT+GSM_DIAGNOSTICS` | 1 | — |
| 68 | `AT+SET_COM_MODULE` | 1 | mandatory board test |
| 69 | `AT+SET_GSM_RF_A` | 1 | mandatory board test |
| 70 | `AT+SET_GSM_RF_S` | 1 | mandatory board test |
| 71 | `AT+SET_PLMN` | (see source) | NOTES list |
| 73 | `AT+DIG_IN` | 0 | mandatory manual board (off in build) |
| 74 | `AT+FOIR` | 1 | — |
| 75 | `AT+FLASH` | 1 | — |
| 76 | `AT+SMS` | commented | sh29 |
| 77 | `AT+QUEUE` | 0 | — |
| 78 | `AT+CAL_PRESSURE` | 1 | mandatory test bench |
| 79 | `AT+CAL_TEMPERATURE` | 1 | mandatory test bench |

## App modules related to communications (Src)

| File | Role |
|------|------|
| `at_commands.c` | AT dispatcher + syntax docs |
| `parser_rx.c` / `parser_tx.c` | Serial parse/emit |
| `usart_my.c` | UART HAL wrapper |
| `com_module.c` | Communication module select |
| `gsm.c` / `calling.c` / `transmission_gprs.c` | Cellular |
| `dlms_grx.c` | DLMS server glue |
| `http.c` | HTTP client path |
| `ir.c` (via includes) | IR optical |
| `factory.c` | Factory identity |
| `protocol.c` | Protocol selection |

## Test Bench alignment (RAD.1)

Known GPO IDs used by FCT (electrical map — product-specific):

| GPO id | Function |
|--------|----------|
| 6 | IR_EN (active-LOW) |
| 4 | RS485_PWR |
| 5 | RS485_EN |
| 11 | EN_VGSM (active-LOW) |
| 13 | EN_EV (active-LOW) |
| 15 | VALVE_V |
| 17 | DOUT1 |
| 18 | DOUT2 |
| 2 | SNS_EN |
| 9/10 | POL_INa/b |
| 3 | DISP_POWER |

GPI: keys, TELE_PRESENCE, DIN1–4 — see Test Bench Appendix A.

## Keywords

`sk0146`, `at commands`, `AT+GPO`, `AT+GPI`, `AT+FACTORY`, `AT+PROTOCOL`, `DLMS`, `FreeRTOS`, `STM32L152`, `Shitek`, `firmware syntax`, `test bench`
