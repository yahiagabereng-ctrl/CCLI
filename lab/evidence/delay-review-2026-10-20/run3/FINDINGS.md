# Run 3 — delay fixed; presentation OID mismatch blocks association

The DUT log identifies r62, PID 13080, listening on 192.168.0.1:3782.
The capture contains four connection attempts. All complete TLS and receive
COTP Confirm before sending their ACSE association request.

| Stream | COTP CR frame | COTP CC frame | COTP response | AARQ completed at frame | Server TLS close frame | Close after AARQ |
|---|---:|---:|---:|---:|---:|---:|
| 0 | 21 | 23 | 4.934 ms | 27 | 30 | 2.972 ms |
| 1 | 57 | 60 | 283.456 ms | 64 | 67 | 2.805 ms |
| 2 | 94 | 96 | 4.636 ms | 100 | 103 | 2.912 ms |
| 3 | 129 | 131 | 11.286 ms | 135 | 138 | 3.143 ms |

This is a different failure from runs 1 and 2. TSP is now sending its AARQ,
including the MMS Initiate request. CCI closes the connection immediately
afterwards and returns no AARE or MMS Initiate response. No usable association
or MMS read is established by the four captured attempts.

The successful ACSE acceptance shown in the DUT PRE log is from before the
capture; it does not establish that these four attempts succeeded.

## Concrete incompatibility

The presentation context definition list contains:

- Context 3: MMS, `1.0.9506.2.1`.
- Context 1: ACSE, `2.2.1.0.1`.
- Context 5: authentication abstract syntax, `1.0.840.0.1.1.4.1`.

The generated parser's `ccli_auth_as` constant instead encodes
`1.0.840.0.0.1.1.4.1`. The extra zero makes the exact-byte comparison fail.
The context is then neither MMS, ACSE nor recognized authentication, so
presentation parsing fails and the connection closes before `auth_tramp`.

| Source | Encoded OID value |
|---|---|
| TSP context 5 | `28 86 48 00 01 01 04 01` |
| Current generator/parser | `28 86 48 00 00 01 01 04 01` |

The generator definition is in `scripts/generate_mms_certificate_auth.py`,
in the `ccli_auth_as` replacement near line 315. This constant is used for both
recognizing and generating the authentication presentation context. That also
explains why our custom client and server can interoperate while this TSP
request is rejected: they are generated using the same constant.

## Offline proof

The TLS application payload was decrypted using the existing server key.
`extract_request.py` reassembles the two COTP fragments into the exact
1,586-byte session request and extracts its 1,558-byte presentation request.

`replay_presentation.sh` passes that unchanged presentation request through
the existing compiled presentation parser. It then compiles a disposable copy
of the generated parser with only the extra zero removed and repeats the parse.

Observed results:

```text
presentation_parse=REJECT payload_size=1558 acse_context=1 mms_context=3
presentation_parse=ACCEPT payload_size=1558 acse_context=1 mms_context=3
OFFLINE_OID_MISMATCH_CONFIRMED
```

This proves the presentation-layer blocker. It does not yet prove successful
ACSE signature verification, RBAC authorization or MMS operations after that
blocker is removed. The current capture never reaches those server checks.

During the initial investigation, only the temporary offline parser copy was
modified. The subsequent server repair and on-device validation are recorded
in `R63_FIX_AND_VALIDATION.md`.

Capture SHA-256:
`01426025f507f75d96d6b7b80e51006766e03482f98ada721358e2834bf33eb0`.
