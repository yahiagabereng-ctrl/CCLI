# Check of CCI's calling AP/AE validation

27 October 2026. The user reports that TSP is configured with AP-title
1.1.1.999.1 and AE qualifier 12 and requests verification of CCI's behavior.

## Exact failed TSP request

Captured a fresh attempt from 192.168.0.112 after the user saved/restarted TSP
with the diagnostic TLS 1.2 suite AES128-SHA256. The server negotiated 0x003C,
which was offered by the client. The corresponding server certificate and
existing workstation lab key match; the key was used locally for Wireshark
decryption and was never copied into this evidence directory or sent to TSP.

Wireshark independently decodes the failed AARQ as:

| Field | Wire encoding | Value |
|---|---|---|
| Called/server AP-title | `a2 07 06 05 29 01 87 67 01` | 1.1.1.999.1 |
| Called/server AE qualifier | `a3 03 02 01 0c` | 12 |
| Calling/client AP-title | `a6 02 06 00` | Empty OBJECT IDENTIFIER |
| Calling/client AE qualifier | `a7 03 02 01 0c` | 12 |

The failed request therefore does not carry the required calling AP-title,
regardless of the value displayed in TSP's configuration window. The correct
remote/called AP-title is present and is a separate field.

## Actual parser replay

Extracted the exact 1,597-byte session request, 1,569-byte presentation request
and 1,471-byte AARQ from TLS stream 0. Replayed the presentation bytes through
the current generated libIEC61850 presentation/ACSE parser linked from the
existing r70 host build. The offline inspection callback printed:

```text
mechanism=2 calling_ap= calling_ae=12 arcs=0
acse_indication=1 callback=1
```

The offline callback deliberately only inspects parsing and returns true; this
is not an authentication-acceptance assertion. The actual application gate in
MmsAdapter::Impl::issue_profile_a_token rejects fewer than two AP-title arcs,
matching the device event `missing or invalid calling application reference`.
The application certificate requires the combined reference 1.1.1.999.1.12.

For comparison, the previous real TSP request captured on 20 October was
replayed through the same parser and printed:

```text
mechanism=2 calling_ap=1.1.1.999.1 calling_ae=12 arcs=5
acse_indication=1 callback=1
```

## Live positive check

Ran the read-only dual-certificate probe on the actual r70 listener, with the
commissioned DSO application identity and matching AP/AE reference. Results:

```text
PASS: main-readonly associated=1 readable=1 client_error=0
PASS: independent AARE server certificate, issuer, AP/AE, signature, mechanism and time verification
PASS: session CONNECT length and PGI 194
PASS: session ACCEPT length and PGI 194
PASS: main-service read-only dual-certificate association and AARE verified
```

Evidence is also retained under `tmp/cci-actual-apref-check/main-test/`.

## Conclusion and scope

For this captured failure, CCI parses the request as encoded and correctly
rejects the empty calling identity. It also accepts a valid matching identity
and completes a read. No CCI code, firmware, trust or security policy was
changed. This is a verification of the observed failure, not a claim that every
CCI feature is conformant.

The remaining client-side check is the local/calling AP-title field and its
inclusion bit under TSP's Connection Configuration advanced settings. The
server/called fields are already transmitted correctly. If the local field is
populated and saved but still emits an empty OID, investigate TSP's active
profile/configuration handling using this capture.

The temporary AES128-SHA256 cipher restriction was used only to enable this
diagnostic capture; the previous preferred cipher configuration can be restored
after the AP/AE verification.
