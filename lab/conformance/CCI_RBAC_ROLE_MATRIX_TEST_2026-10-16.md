# On-device MMS role and revision matrix

Date: 2026-10-16. Target: confirmed Tespro `192.168.0.1`.

**Result: all 17 role-certificate cases passed the selected network checks.**
This includes both revisions of all seven standard roles, DSO, Aggregator and
the combined Viewer/Engineer identity. Tests exercise the existing commissioned
policy; they do not establish complete CEI/IEC conformance or all duties implied
by a role's name.

## Test scope and safeguards

- Main CCLI **0.1.0-r58** stayed running as PID **3374**, with Profile A enabled
  on port **3782**. Its configuration, client pins, server credentials, installed
  executable, UCI and persistent activation state were not changed.
- Used the previously cross-built actual MMS adapter harness on **3783** with
  Profile A enabled, existing server TLS/ACSE credentials, EJBCA-issued role
  certificates, owner-signed sequence-2 package, CA and CRL checks, and audit.
  Hardware I/O/controller services are absent from this harness. Reads use
  synthetic process values; accepted writes/controls modify isolated memory.
- Harness SHA-256:
  `2af9d7f01648d26ea084634fc92d10eeb73deb179f850ed78cdc8a2f05ed7247`.
- Limited four-slot TLS pins were changed only in five separate test YAML files.
  All 17 identities were already present in the signed positive package; no
  policy edit, new signature, certificate issuance or new role grant was needed.
- Client/owner/CA private keys were not transferred to Tespro.
- The client refuses any port other than 3783. Server helper checks the harness
  binary hash, stops only a PID with the exact harness executable, uses bounded
  600-second runs and verifies the test listener is closed on cleanup.

## Results against the commissioned MMS policy

Allow/deny below refers to the instantiated DSO measurement/control objects,
static measurement dataset and report block. All standard-role rows passed at
**revision 0 and revision 1**; private roles use revision 1.

| Role | ID | Measurement read | Dataset directory | Reporting | CF configuration write | Control |
|---|---:|---|---|---|---|---|
| VIEWER | 0 | Allow | Deny | Allow | Deny | Deny |
| OPERATOR | 1 | Allow | Deny | Allow | Deny | Allow |
| ENGINEER | 2 | Allow | Allow | Allow | Allow | Deny |
| INSTALLER | 3 | Allow | Deny | Allow | Allow | Deny |
| SECADM | 4 | Allow | Allow | Deny | Allow | Deny |
| SECAUD | 5 | Allow | Deny | Allow | Deny | Deny |
| RBACMNT | 6 | Allow | Deny | Deny | Allow | Deny |
| DSO_OPERATOR | -1 | Allow | Allow | Allow | Allow | Allow |
| AGGREGATOR_OPERATOR | -2 | Deny DSO object | Deny DSO dataset | Deny DSO report | Deny DSO object | Not exercised: own control target absent |
| Viewer + Engineer | 0 + 2 | Allow | Allow | Allow | Allow | Deny |

Every identity authenticated through mutual TLS and MMS. Audit verifies the
identity bound to its authenticated transport and records role metadata.
LISTOBJECTS returned objects inside the selected allowed logical node for each
identity. Aggregator separately **read and discovered `WSaDAGC1.Mod`**, the
existing inactive stub, and was denied `PdCMMXU1.TotW` and `WlimDWMX1.Mod`.
This is partial Aggregator coverage: its functional setpoints, VArSaDVAR1 and
PdCiMMXU1 measurement/report paths are absent, so full positive operation cannot
be tested using this model.

Exact requests:

- READ: `CCI016_01LD_Plant/PdCMMXU1.TotW.mag.f` (MX) and
  `CCI016_01LD_Plant/WlimDWMX1.Mod.stVal` (ST).
- Aggregator own-scope READ: `CCI016_01LD_Plant/WSaDAGC1.Mod.stVal` (ST).
- LISTOBJECTS: WlimDWMX1; Aggregator uses WSaDAGC1.
- DATASET directory: `CCI016_01LD_Plant/LLN0.DS_R_PdC_Mis4sec`.
- REPORTING: `CCI016_01LD_Plant/LLN0.RP.urcb_PdC_Mis4sec`; allowed cases
  read the RCB, enable RptEna, receive at least one report and disable RptEna.
  Denied cases receive access denied when reading the RCB.
- CONFIG: CF write of `WlimDWMX1.WMaxSptPct.ctlModel = 3` in isolated memory.
- CONTROL: `WlimDWMX1.Mod = 5` in isolated memory; allowed only for OPERATOR
  and DSO among the tested single-role identities.

The combined role certificate correctly combines Viewer and Engineer permissions
without acquiring OPERATOR's CONTROL permission.

## Probe correction and retained failures

The first pass accepted 16 cases and flagged Aggregator's first denied read.
The read was correctly blocked: server audit recorded MMS_ACCESS_DENIED for the
authenticated Aggregator. The probe had only recognized an IED client-level
access-denied error, whereas this read returned access denied inside the MMS
value. Corrected the probe to recognize either encoding, explicitly requiring
access denied, and reran **only Aggregator**. Its full available case passed.
No product code or policy was changed to make this pass. Initial result/logs
are retained alongside the corrected results.

## Final device state and evidence

- Harness stopped; no listener on 3783.
- Main PID 3374 remains running; final Viewer TLS/MMS measurement read on
  3782 passed. No main-service restart was performed for this matrix.
- Active YAML SHA-256 remains
  `8b2854bd844eb7e219b3a7a33257d29ef2ca78a2854ba3c81a25fd4dbc80be15`.
- Main CCLI executable SHA-256 remains
  `12dae230d5d45257de88ae9585dcc15744e53ed1f54f0a6dd0c48c881f580786`.
- Production state unchanged:
  `1 85f1f4679c9ad84382f76aaab564a32d47c8a20c031412f23f2f9c91ad2b57e7`.
- Isolated state unchanged:
  `2 f4f13dacbe36b631d86ca99e3e396f143c417a2881a7239d96d122ffec3da45a`.
- Original TLS files and UCI match their pre-activation hashes.
- Test configs/logs remain under
  `/etc/ccli/rbac-test-20261016/matrix-20261016/`.
- Protected PC evidence: `lab/ejbca-staging/rbac/role-matrix-20261016/` contains
  `results.json`, 17 client logs, initial failure evidence, `server.log`,
  `events.jsonl`, `audit-summary.json`, five batch configurations, baseline
  checks and `main-service-viewer-final.log`. All 17 identities appear in the
  server's successful-authentication records. The 153 printed PASS lines include
  per-certificate summary lines, not 153 independent test cases.
- Probe source:
  `apps/ccli/tests/integration/mms_rbac_role_matrix_probe.cpp`.
  Build/runner helpers: `tmp/build_role_matrix_probe.py`,
  `tmp/run_tespro_role_matrix.py`, `tmp/retry_tespro_aggregator.py` and
  `tmp/tespro_role_matrix_server.sh`. Runners read SSH password from the
  environment; it is not stored in source.

## Remaining limits

The main-service allowlist still selects its three commissioned working clients;
the other roles were pinned only on the isolated test process. This does not
claim all roles were exercised against the installed main CCLI executable.
Actual TSP workflows, remaining invalid-credential cases, live revocation/expiry,
reboot persistence, complete service/object coverage and full Aggregator model
functions remain open. File transfer, remote CA/certificate administration,
remote policy administration and an MMS SECAUD log service were not tested or
added. Security events were checked through the persistent local event store.
The CRL replacement/offline-maintenance work described in
`CCI_RBAC_CRL_AUTOMATION.md` is separate and unchanged by these role tests.
