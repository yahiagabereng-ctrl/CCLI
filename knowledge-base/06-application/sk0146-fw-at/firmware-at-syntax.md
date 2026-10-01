# SK0146 FW — AT command syntax

**source_id:** `fw-sk0146-at-syntax`  
**Source:** `at_commands.c` NOTES + DEFINES + GPI/GPO tables  
**Product:** SK0146-EVC  

## Grammar

```
AT+<NAME>                 Action (no args)     if ACT allowed
AT+<NAME>=<a1>[,<a2>…]    Action (with args)   if ARGS allowed
AT+<NAME>?                Read                 if READ allowed
AT+<NAME>=?               Test / ranges        if TEST allowed
```

- Names are **uppercase** functional tokens after `+` (no SKU prefix).  
- Args: comma-separated; strings may be **quoted** (`"..."`).  
- Success ends with `OK`; multi-line replies use `+NAME: …` then `OK`.  
- Max 5 arguments; max command length 150; max answer 195.

## Response pattern

```
+<NAME>: <fields…>
[+<NAME>: …]
OK
```

Errors: firmware returns `ERROR` (bench notes: legacy `AT+DIN?` → ERROR; use `AT+GPI?`).

## Core factory / board-test syntax (extract)

### AT+GPI (read only)

| Form | Syntax |
|------|--------|
| Read | `AT+GPI?` → `+GPI: <id>,<pin>,<label>,<status>` (repeated) → `OK` |
| Test | `AT+GPI=?` → `+GPI: (0-1)` → `OK` |

`<status>`: `0` low, `1` high.

### AT+GPO

| Form | Syntax |
|------|--------|
| Args | `AT+GPO=<gpo_id>,<gpo_status>` → `OK` |
| Read | `AT+GPO?` → `+GPO: <id>,<name>,<status>` … → `OK` |
| Test | `AT+GPO=?` → `+GPO: (0-19),(0,1)` → `OK` |

### AT+DEBUG (trace levels)

Modes 0–6 for masks, single level, read state, info, debug port.  
Read: `AT+DEBUG?` · Test: `AT+DEBUG=?` · Args: `AT+DEBUG=<mode>,…`

### Common production commands (forms used on Test Bench)

| Command | Typical use |
|---------|-------------|
| `AT+FACTORY` | Device identity / factory mode |
| `AT+VERSION` | FW version |
| `AT+PROTOCOL` | Active protocol (EVC → DLMS) |
| `AT+BATTERY` | Battery / RTC related |
| `AT+GSM_ON=<0\|1>` | Modem power path |
| `AT+GSM_TX=1` | TX stim (bench) |
| `AT+GSM_STATUS` | Registration |
| `AT+SMS="STATUS"` | SMS path / ADCT status (when enabled) |

Exact arg tables for every command live in `at_commands.c` NOTES blocks (ASCII tables). This ingest captures **grammar + factory subset**; expand per-command UGM when Guide.txt is located.

## Protocol transports

Same AT grammar can be started on:

| Path | API |
|------|-----|
| IR optical | `AtCommands_AtProtocolIr_Start/Stop` |
| RS-485 | `AtCommands_AtProtocolRs485_Start/Stop` |
| GSM | `AtCommands_AtProtocolGsm_Start/Stop` |

Application protocol on meter ports is **DLMS** (`AT+PROTOCOL` → DLMS) — not Modbus/61850.

## Escape / terminal

- Escape byte: `+` with guard time 200–1000 ms  
- Banner: `*** AT TERMINAL ***`

## CCLI / new product syntax rule

When writing CCI commissioning AT (or shell) layer:

1. Keep **same four forms** and `OK` / `+NAME:` replies.  
2. Keep **unprefixed** names.  
3. Document each command in-source with the same ASCII table style.  
4. Do **not** invent `AT+SK0xxx…` prefixes.  
5. Map electrical IDs carefully — SK0146 GPO map ≠ CCI 10–120 V DI / dry DO.
