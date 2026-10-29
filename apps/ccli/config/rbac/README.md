# IEC 62351-8 role policy package

This package follows the supplied IEC XACML template:

| File | Purpose |
|---|---|
| commissioning-policy.xml | Root Role PolicySet and nine Role PolicySets. Each role matches integer RoleID, string roleDefinition and integer revision, then references its PermissionGroup. |
| permission-library.xml | Reusable PermissionGroup PolicySets reference six shared Permission PolicySets. Each Permission contains a Policy/Permit Rule with separate resource and action Target clauses. |
| commissioning-scopes.xml | Separate product commissioning configuration binds a role to its token AoR and allowed plant object prefixes for each permission. This file is not an IEC role definition. |
| identity-bindings.xml | Owner-authorized identity certificate fingerprints, transport certificate fingerprints, actor and permitted role IDs. The supplied template is empty. |
| commissioning-manifest.xml | Generated signature, sequence and SHA-256 digest of each of the four files above. Required for runtime activation. |

The XACML documents use the IEC attribute identifiers, UUID identifiers,
Version 1.0, integer-equal/string-equal, MustBePresent=false, and the template's
permit-overrides algorithms. The action designator uses the access-subject
category exactly as in the supplied template. The root includes an empty
Target to satisfy the XACML schema. Permission-group and permission definitions
are kept in one local library and resolved by their PolicySetIdReference IDs.
The library is a definition store; its root is never evaluated as an entry point.

`IEC61850/MMS` is the ObjectID for the MMS service resource in these permission
definitions. LISTOBJECTS, READVALUES, DATASET, REPORTING, CONTROL and CONFIG
are its ActionIDs. The actual commissioned LD/LN/DO allocation and AoR are
applied separately as an intersection, so commissioning cannot add an action
that the role definition does not grant. Object prefixes match only exact
objects or descendants separated by `.` or `/`. A private role cannot receive
the `*` object scope. The runtime also enforces CEI 57-142 Tables 22/23 private
role RO/RW bounds independently of this XML. Standard role revisions 0 and 1
are supported; private roles use revision 1. The standard roleDefinition is
`IEC62351-8`. SECAUD has no DATASET permission.

Private C3 CONFIG grants for `Oper.ctlVal` are evaluated by the control service,
with the same role/session checks, value validation and command execution as
CONTROL. They never authorize raw writes into the control cache. Standard-role
CONFIG does not imply CONTROL. The Aggregator CONFIG scope contains exactly the
four allocated control-value paths under WSaDAGC1 and VArSaDVAR1; a hard runtime
bound prevents a broader signed scope from exposing `ctlModel`, status,
measurements or other configuration attributes (CEI 57-142 Table 23).

Set `mms.rbac_policy` to the role file path and keep the entire signed package
in that directory. The policy compiler requires the first three files; runtime
activation additionally requires signed identity bindings and manifest. References resolve only
within this package, with no network retrieval or path-based references.
Duplicate IDs, missing references, cycles, wrong types/categories, DTDs and
unsupported constructs reject the entire load and clear grants. Targets retain
their AND/OR structure while compilation propagates role restrictions through
references. AoR is matched separately against the authenticated token record.

The supported subset accepts the template's Permit rules and permit-overrides;
other effects, Conditions, obligations, policy-version selectors and other
combining algorithms are rejected. Remove an allowed object/action from the
commissioning scopes to restrict access. Stable IDs must be retained when
reviewing or updating these definitions; UUIDs identify definitions rather than
runtime sessions.

## Owner commissioning

Prepare a copy of these templates on the owner's engineering host. With Python
and `cryptography` installed, bind each approved existing identity and transport
certificate, then sign all four files using the owner's signing key:

```text
python scripts/commission_rbac.py bind --package PACKAGE --identity DSO_IDENTITY.pem --transport DSO_TLS.pem --actor DSO --roles=-1,0
python scripts/commission_rbac.py sign --package PACKAGE --sequence 1 --owner-key OWNER_SIGNING.key
```

DSO identities can assert only -1/0, AGGREGATOR only -2/0 and OWNER standard
roles 0–6. Bind only the roles actually issued to that identity. A transport and
identity certificate may differ; they must share a public key or the MMS
authentication must supply a valid fresh signature proof from the identity key.
The tool accepts an unencrypted PEM signing key kept on the protected engineering
host; that private key is not installed on the device. RSA keys need at least
2048 bits and ECDSA keys at least 256 bits.

Provision the owner's public signing certificate through the device owner's
trusted commissioning process. Configure these fields under `mms`:

```yaml
rbac_profile_a: true
rbac_policy: /etc/ccli/rbac/commissioning-policy.xml
rbac_owner_cert: /etc/ccli/rbac-trust/owner.pem
rbac_activation_state: /var/lib/ccli/rbac/activation.state
rbac_identity_ca: /etc/ccli/tls/root_CA.pem
rbac_identity_crl: /etc/ccli/rbac-trust/identity.crl.pem
```

These are commissioning examples; the repository lab YAML keeps Profile A
disabled. Create the activation-state directory with owner-only write access
and persist it across upgrades. Protect owner trust, CA/CRL paths and package
files with the device's administrative filesystem permissions. Never delete
activation state to install an older package. A higher sequence activates a new
package; the same sequence is accepted only for identical signed content.
Sign again after editing any package file. Install the complete verified package
before restarting the service; no policy-editing MMS endpoint is provided.

TLS must already be enabled with chain validation. Existing TLS protocol,
allowlist, credential and trust settings remain the communication baseline.
RBAC identity CA/CRL validation is additional; the optional identity paths
default to the existing TLS CA/CRL paths. A valid applicable identity CRL and
persistent security event logging are required. Do not enable the example until
the actual EJBCA identities, owner signature and device/client regression have
been verified.

Runtime enforcement checks static dataset members and report delivery against
the identity's READVALUES scope and current certificate validity. Private report
writes are limited to RptEna; report dataset selection is fixed. File, journal
and dynamic dataset services are disabled in this selected MMS profile.
Administrative token Operation/sequence fields are parsed but cannot serve as
normal session access tokens. Owner commissioning updates permissions through
the signed package.

Current verification and remaining release gates are recorded in
`lab/conformance/CCI_RBAC_ROLE_IMPLEMENTATION.md`. Host tests and XACML schema
validation do not establish appliance conformance.
The 28 October source changes and service coverage are recorded in
`lab/conformance/ANNEX_T_RBAC_GAP_CLOSURE_2026-10-28.md`. A changed commissioning
scope requires a newly signed package and a higher activation sequence before
device activation.
# TLS peer enrollment

The signed role policy and TLS peer certificate pins are separate checks. For
additional commissioned peers, the CCLI configuration reader accepts repeatable
`mms.tls_allowed_client_cert` scalar entries:

```yaml
mms:
  tls_enabled: true
  tls_chain_validation: true
  tls_allowed_client_cert: /etc/ccli/rbac-clients/operator-r1.pem
  tls_allowed_client_cert: /etc/ccli/rbac-clients/engineer-r1.pem
```

Repeated scalar entries are a CCLI reader extension; generic YAML parsers may
reject duplicate keys. Each entry adds one public leaf certificate pin alongside
the existing legacy certificate slots. A missing or invalid certificate prevents
MMS startup. Pins preserve chain/expiry/revocation validation and do not assign
roles or bypass commissioned identity/transport bindings. Apply changes with a
CCI service restart; existing MMS associations will disconnect.
