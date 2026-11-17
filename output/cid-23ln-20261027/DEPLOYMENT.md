# 23-LN CID deployment — Tespro, 2026-10-27

Pre-change source checkpoint pushed to personal-final/main: e51b715 (r70).

Input: C:/Users/Administrator/Downloads/product_tr400_eth_a_all_mandatory.cid. Original preserved. Repository/output copy changes IP and IP-GATEWAY from 192.168.10.1 to 192.168.0.1, preserving subnet 255.255.255.0 and model/OSI fields.

CID SHA256: 8d56b735206fcf9f40e0e135048b7dc400abac10fa104a009a62f40af2e02393
CFG SHA256: 5b055efbfde98f188cbdb874121bef41f0485dd1ff1a1bcddfdd4a68b3cf42b5

Generated using the repository genconfig.jar in the existing container Java runtime; retained the repository URCB option correction to 159. Generated 23 logical nodes, one dataset and two indexed URCB instances from RptEnabled max=2.

Installed both files under /etc/ccli/icd/. Added mms.model_cfg pointing to /etc/ccli/icd/product_tr400_eth_a_all_mandatory.cfg in /etc/ccli/lab.yaml and restarted /etc/init.d/ccli. Device SHA256 values matched local files.

Rollback configuration: /etc/ccli/lab.yaml.before-23ln-20261027. Restore it to /etc/ccli/lab.yaml and restart /etc/init.d/ccli if needed.

Observed startup: full CID model loaded; TLS/RBAC/CRL enabled on 192.168.0.1:3782; PID 6582. Signed owner policy retained at sequence 4. Read-only dual-certificate probe passed association, authorized read, independent AARE certificate/signature/AP/AE/time and CONNECT/ACCEPT length checks.

Scope: structural model deployment, not implementation of new DPCC/DisFR/breaker/generation/storage runtime behavior. VArSdDVAR1 and PFSPDFPF1 remain absent and their respective runtime functions inactive. Existing meter read failures and GNSS unsynchronized status remain; this deployment does not establish live measurement accuracy or full compliance. Native E2E remains disabled.

Use the CID in this directory for Test Suite Pro. Existing working TLS and client calling AP/AE settings are retained; do not re-enable previously problematic optional calling-reference fields merely because the CID contains server selectors.
