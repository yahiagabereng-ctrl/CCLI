# SK0146 FW — AT coding conventions & rules

**source_id:** `fw-sk0146-coding-conventions`  
**Product:** SK0146-EVC (Electronic Volume Converter)  
**Primary file:** `Src/at_commands.c` (Shitek Technology SRL)  
**Platform:** STM32L152VE · FreeRTOS · HAL  
**Purpose for CCLI:** reuse **syntax, naming, and add-command rules** — do **not** copy DLMS/meter domain logic into CEI CCI firmware.

## File banner (required style)

```
/*=============================================================================
 * File       :  AT_COMMANDS.C
 *
 * Project    :  SK0146 - EVC
 * Description:  AT commands
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/
```

## Naming rules

| Element | Pattern | Example |
|---------|---------|---------|
| AT command string | Unprefixed `+NAME` (no product SKU in name) | `+GPO`, `+FACTORY`, `+GSM_ON` |
| Enum member | `AT_COMMAND__NAME` (some legacy without `__`) | `AT_COMMAND__GPO` |
| Enable flag | `AT_COMMAND_STATUS__NAME` = `0` or `1` | `AT_COMMAND_STATUS__GPO 1` |
| Callback | `AtCommands_Callback_AtCommand_NAME` | `AtCommands_Callback_AtCommand_GPO` |
| Module prefix | `AtCommands_` for AT protocol API | `AtCommands_AtProtocolRs485_Start` |
| HAL wrappers | `*_my.c` / `*_my.h` | `usart_my.c`, `adc_my.c` |

**Never** use product-prefixed AT names (`AT+SK0146…`). Plain functional names only.

## Four command forms (mandatory)

| Mask | Symbol | Syntax | Meaning |
|------|--------|--------|---------|
| `0x01` | `AT_CMD_TYPE_MASK_ACT` | `AT+DUMMY` | Action, no args |
| `0x02` | `AT_CMD_TYPE_MASK_ARGS` | `AT+DUMMY=1,0` | Action with args |
| `0x04` | `AT_CMD_TYPE_MASK_READ` | `AT+DUMMY?` | Read |
| `0x08` | `AT_CMD_TYPE_MASK_TEST` | `AT+DUMMY=?` | Test / help range |

Type enum (handler switch): `AT_CMD_TYPE_ACT|ARGS|READ|TEST|SMS`.

## Limits (from DEFINES)

| Constant | Value |
|----------|-------|
| `MAX_LEN_AT_COMMAND` | 150 |
| `MAX_LEN_AT_ANSWER` | 195 |
| `MAX_LEN_AT_COMMAND_ARG` | 130 + 2 (quotes) |
| `MAX_NUM_AT_COMMAND_ARGS` | 5 |
| Escape guard (OS ticks ≈ ms) | 200–1000 |
| Terminal banner | `\r\n*** AT TERMINAL ***\r\n\r\n` |

## Data structures

```c
typedef struct {
    u16   type;
    u8    num_of_args;
    ascii args[MAX_NUM_AT_COMMAND_ARGS][MAX_LEN_AT_COMMAND_ARG + 1];
} AT_COMMAND_INFO;

typedef void (*AT_COMMAND_HANDLER)(AT_COMMAND_INFO *ptr_at_command_rx);

typedef struct {
    ascii             *string_name;
    u8                 string_name_length;
    u8                 cmd_types;      /* bitmask of allowed forms */
    AT_COMMAND_HANDLER cmd_handler;
} AT_COMMAND_DEF;
```

Table: `static const AT_COMMAND_DEF AtCommands_AtCommandsDef[NUM_AT_COMMANDS]`.  
**Order of table rows MUST match `AT_COMMAND_AT` enum order.**

Final enum sentinels (do not reorder): `AT_COMMAND__AT`, `AT_COMMAND__UKNOWN`, `NUM_AT_COMMANDS`.

## How to add a new AT command (11 steps — house rule)

1. Define the **syntax** of the new command.  
2. Add it to the **AT commands list** in the NOTES section (comments only).  
3. Add the **syntax description table** in NOTES (comments only).  
4. Add the syntax table to **`UGM AT commands - Guide.txt`**.  
5. Add examples to **`UGM AT commands - Quick guide with examples.txt`** (if important).  
6. Add `#define AT_COMMAND_STATUS__XXXXX` in DEFINES (`0`/`1`).  
7. Add enum value in `AT_COMMAND_AT`.  
8. Add callback prototype `AtCommands_Callback_AtCommand_XXXXX`.  
9. Add row in `AtCommands_AtCommandsDef` with correct type masks (order = enum).  
10. Implement callback; copy a similar command; `switch` on ACT/ARGS/READ/TEST.  
11. Test on the device communication port.

## Transport start/stop API (pattern)

- `AtCommands_AtProtocolIr_Start` / `_Stop`  
- `AtCommands_AtProtocolRs485_Start` / `_Stop`  
- `AtCommands_AtProtocolGsm_Start` / `_Stop`  

Same AT grammar on IR / RS-485 / GSM paths.

## Production enable policy

Status `1` = compiled in; `0` = omitted from enum/table.  
Comments mark *mandatory for production (test bench / SMQ / MEX / board test)*.

## CCLI adoption guidance

| Reuse for CCI / TG-500 / Pi | Do not reuse |
|-----------------------------|--------------|
| Unprefixed AT names + 4 forms | DLMS, billing, valve, IR meter, FOIR |
| 11-step + NOTES-in-source docs | STM32L1-only HAL as Linux app |
| Table + callback dispatcher | Whole `at_commands.c` binary port |
| Factory `GPI`/`GPO` command shape | 3.3 V EVC electrical limits on 10–120 V DI |

Suggested CCI factory-style names (new product): `AT+GPO`, `AT+GPI`, `AT+VERSION`, `AT+FACTORY`, plus grid-specific commands — still unprefixed.

## Corpus status

| Asset | Status |
|-------|--------|
| Curated conventions (this file) | HAVE — for ingest |
| Full `at_commands.c` in RAG | MISSING (too large; use extracts) |
| UGM Guide / Quick guide txt | MISSING from archive path used for extract |
| Source archive | `New folder/FW-…/EVC/SK0146 dlms/` |
