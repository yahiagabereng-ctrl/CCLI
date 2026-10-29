# r63 server repair and validation — 20 October 2026

## Applied change

`scripts/generate_mms_certificate_auth.py` now recognizes the authentication
presentation abstract syntax sent by TSP, `1.0.840.0.1.1.4.1`. The previous
constant contained an extra zero. Accepting TSP's context removes the run 3
presentation rejection before the ACSE authentication callback.

The old extra-zero OID is retained as one explicit compatibility alias for the
already-installed custom client. Certificate chain/CRL verification, transport
binding, signed proof/freshness/replay checks and RBAC are unchanged. Unknown
authentication context syntaxes remain rejected. Encoder BER lengths are
computed from the authentication OID size rather than the old literal lengths.

Source VERSION and package recipe release are 63. Only the server binary was
installed on the device; no APK was built/installed and no device reboot occurred.
The package database therefore still identifies r60. This is not a USB update.

## Regression and device evidence

`r63-presentation-test.txt` records seven passing cases: unchanged captured TSP
CP, explicit legacy alias, rejection of an unknown authentication syntax,
two-context compatibility, rejection of four contexts, and independent strict
BER-length validation plus parser round-trip for certificate and TLS modes.
Tests exercise a newly generated presentation overlay, never modify the vendor
sources, and do not rebuild the installed client.

`tsp-shape-isolated.json` records acceptance by the isolated on-device harness
on port 3783 in 102.6 ms. `tsp-shape-live.json` records acceptance by the live
port 3782 in **91.6 ms**, with COTP confirmation in **5.4 ms**. Both returned
AARE result 0, MMS Initiate response, and a verified server-proof signature.

The disposable probe retains TSP's captured 1,586-byte session request,
1,558-byte presentation CP, context order/identifiers, ACSE/MMS parameters and
1,021/565-byte COTP fragment split. Only the existing DSO identity's timestamp
and signature are refreshed; captured identity certificate DER remains intact.
This proves acceptance of TSP's observed association shape. It is not a live
test of the separate TSP application, which still needs to reconnect.

`tsp-shape-isolated-stale.json` records rejection/closure for the original old
captured proof. `r63-isolated-status.txt` explicitly records
`ASSOCIATION_AUTH_FAIL` with `stale or future authentication proof`. Freshness
checks have not been disabled to obtain the positive result.

`../mms-r63-isolated-read.json` and `../mms-r63-live-read.json` record successful
signed associations and reads using the **unchanged** client executable. The
isolated value 13 is synthetic. The live value 0.0 is not evidence of a healthy
inverter/Modbus link; live device logs still show read failures.

## Deployment and preservation

`r63-deployment-receipt.txt` verifies PID 12191 actually owns the port 3782
listening socket and its `/proc/PID/exe` hash matches the staged r63 binary:

`cd1b233b41c8d5360846ec447458fb1fbb599d9eaf8e57c8b162a1c2cdfda2de`.

The previous r62 executable is preserved in
`/etc/ccli/tsp-r63-backup-20261020/ccli-r62`; the directory is mode 700 and also
holds configuration, settled audit files and a protected-file checksum manifest.
YAML/UCI, TLS, role/trust and activation files passed unchanged checksum checks
after deployment and again after network validation. Both pre-existing audit
segments were preserved, with startup events appended to the current segment.

Only CCI was stopped/restarted. The deploy script waited for the old daemon and
inherited listener descriptors to close, then verified ownership of the new
listener rather than accepting any LISTEN entry as success. The temporary
isolated harness was stopped afterwards; port 3783 is no longer listening.

TSP behavior for later custom-client work is retained in
`apps/ccli/tools/mms-client/TSP_INTEROPERABILITY_NOTES.md`. No custom-client
application source or executable was changed during this repair.
