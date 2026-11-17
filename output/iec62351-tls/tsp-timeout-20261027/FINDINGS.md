# TSP timeout capture — 27 October 2026

Tespro runs CCLI r70, PID 12220, with TCP 192.168.0.1:3782 listening.
A control connection from this workstation successfully authenticated with
TLS 1.2 / ECDHE-RSA-AES128-GCM-SHA256 and verified the server certificate.
That control connection was made before the clean retry capture.

The user reported another TSP timeout while the clean capture was running.
The capture contains 26 packets across four connections from 192.168.0.112
to 192.168.0.1:3782 (client ports 60223, 60224, 60226 and 54454).

Each connection completes SYN/SYN-ACK/ACK. The client then sends FIN with
**zero TCP payload bytes**. The server acknowledges/closes normally.
Client close occurs approximately 1–103 ms after connection establishment.
There is no TLS ClientHello, TLS alert, COTP request, MMS association or
certificate exchange in these connections.

The corresponding server log reports generic TLS handshake failure (-1),
consistent with the peer closing the socket while the TLS provider awaits
the ClientHello. This capture does not identify which local TSP setting or
initialization operation causes the close. TSP's own log is required to
distinguish file/key/CA/cipher configuration or another local error.

These attempts provide no evidence of a server-side CRL rejection: the peer
never starts TLS certificate exchange. No firmware or security policy was
changed during this investigation. The temporary capture helper was bounded
to TCP port 3782 and was stopped after the attempt.

Evidence: `tsp-retry.pcap`, `packets.tsv`.

## Later successful association and subsequent AP/AE errors

Read the persistent log again after the user's report of one successful TSP
connection. These later attempts were outside the earlier packet capture.

- 11:22:49 UTC (12:22:49 Rome): TLS established from 192.168.0.112:54602 using
  ECDHE-RSA-AES128-GCM-SHA256. ASSOCIATION_AUTH_OK recorded for actor DSO,
  application certificate CN=CCLI-RBAC-dso-mms, fingerprint
  `e48c2cee2237eb58fd821055335e00bd9c91fe9c04c7b03883aa320c7539cdd8`.
- The same session recorded authorized READVALUES operations on
  `LD_Plant/WlimDWMX1.Mod` and `LD_Plant/WSdDAGC1.Mod`. A read of
  `LD_Plant/LLN0.Mod` was separately denied by object/permission scope; it did
  not prevent the successful association.
- 11:25:50 UTC (12:25:50 Rome): that session disconnected after about three minutes.
- Later sessions from the same TSP PC complete TLS, then fail explicit MMS
  certificate authentication. At 11:44:56 UTC and several later attempts the
  reason is `missing or invalid calling application reference`.
- At 11:49:35, 11:49:37 and 11:49:38 UTC, the reason is
  `certificate AP/AE does not match calling application reference`.

The successful DSO application certificate binds the combined reference
`1.1.1.999.1.12`. Therefore configure the local/calling application reference
in TSP's Connection Configuration advanced fields with AP-title
`1,1,1,999,1` (OID 1.1.1.999.1) and separate AE qualifier `12`, and enable both
inclusion bits. Setting only the remote/IED called reference does not supply
the calling reference that the server checks. No firmware change was made.
