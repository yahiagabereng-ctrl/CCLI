# RBAC CRL lifetime and automatic delivery

**Superseded development policy, 29 October 2026:** the user selected 30-day
signed CRL validity for both CCLI-Lab-CA and CCLI-RBAC-Lab-CA. Hourly TesPro
fetching is now commissioned in r75/r76. The seven-day settings and absent
automation statements below describe the historical 16 October state.
See `apps/ccli/docs/CRL_PULL_UPDATER.md` and the r75/r76 deployment records.

Date: 2026-10-16. Scope: CCLI IEC 61850/MMS Profile A.

## Seven-day lab lifetime

`scripts/provision_ejbca_rbac.py` now explicitly sets the dedicated
`CCLI-RBAC-Lab-CA` CRLPeriod to **604800000 milliseconds (seven days)** in both
`init` and `finish`. This includes an already provisioned CA. The `crl-settings`
stage applies only that setting. `finish` generates a replacement, verifies its
signature, revocations and seven-day signed lifetime, and exports it locally.
It does not deploy to Tespro. Existing deployed CRLs retain their own expiry.

Verified against the running lab EJBCA on 16 October: `finish` passed all 24
certificate/PKCS#12 checks and generated the signed CRL with thisUpdate
2026-10-16 08:59:50 UTC and nextUpdate **2026-10-23 08:59:49 UTC**
(10:59:49 Europe/Rome). Its five revoked entries remain present. The earlier CRL
on Tespro was due to expire on 17 October at 08:14:02 UTC; this was its scheduled expiry,
not an observed expiry event. It was replaced during the r59 commissioning on
16 October. The active configuration now loads the seven-day CRL from
`/etc/ccli/rbac-dual/trust/rbac_crl.pem` and its combined TLS bundle. Generation,
distribution and reload automation are still not enabled.

## Recommended automatic workflow

Generation, delivery and use by CCLI are separate steps:

1. **EJBCA:** configure a CRL Updater Service for the RBAC CA, with seven-day
   validity, a one-day CRL issue interval, and a frequent service check such as
   hourly. Publish the signed CRL at a stable reachable distribution endpoint.
   Generate and distribute a new CRL immediately after an urgent revocation.
   An issue interval alone does not enable the updater service.
2. **Tespro updater:** an hourly timer fetches the public CRL. Validate the
   pinned issuer, CA signature, thisUpdate/nextUpdate, CRL number and rejection
   of older versions before installation. Check the device clock. Keep the CA
   trust anchor fixed; never adopt a new CA supplied alongside a downloaded CRL.
3. **Install:** stage the new identity CRL and a combined TLS CRL bundle that
   retains the original TLS issuer's CRL. Prepare a complete version directory
   and atomically switch a single bundle reference so both paths refer to the
   same version. Retain the previous valid version and last accepted CRL number
   on persistent protected storage. No CA, owner or client private keys are
   needed for fetching or verifying a signed CRL.
4. **Reload:** add and verify a CCLI security-material reload path that updates
   identity and TLS validation consistently and deals with existing sessions
   and reports according to the required revocation response time. Test failed
   fetch, bad signature, rollback, clock error, expiry and active sessions.
5. **Monitor:** audit successful updates and failures, alert well before expiry
   (for example 48 hours), retry temporary failures and preserve the last valid
   CRL. Do not extend nextUpdate locally or disable revocation checks on expiry.

The one-day issuance/hourly-fetch timings are an engineering proposal for this
project, not an established CEI requirement. The operator must approve the
maximum time a newly revoked identity can remain usable; hourly polling alone
does not provide immediate revocation.

## Current limitation and lab option

No automatic generation service, device fetch timer or reload path is installed
by this change. The existing deployment loads CRLs at service startup. A daily
full CCLI restart also reinitializes hardware I/O, so it is not a suitable
unattended production refresh mechanism.

For the present PC-hosted lab, a scheduled administrative job could run
`python scripts/provision_ejbca_rbac.py finish`, verify/export and transfer only
the public CRL, then perform a controlled CCLI reload/restart in an agreed lab
window. The `finish` stage checks all lab fixtures; it is not a general-purpose
production CA agent. The PC and Docker/EJBCA must be running. Use protected SSH
keys and a pinned Tespro host key for unattended delivery, with a narrow upload
and install permission rather than embedding the root password in a job.

Before an actual scheduled job is enabled, implement/test deployment and reload,
choose its execution host and reachability, configure credentials and monitoring,
and agree the revocation response time. No scheduled job has been created.

## References

- [EJBCA CA fields](https://docs.keyfactor.com/ejbca/latest/ca-fields): CRL expire
  period, issue interval, overlap and the need for a CRL Updater Service.
- `CCI_RBAC_TESPRO_ACTIVATION_2026-10-16.md`: deployed trust, CRL paths and
  original activation evidence.
