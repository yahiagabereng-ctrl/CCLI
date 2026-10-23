# P3-07 Connect session PKI (gen_lab_pki)

**Not** the EJBCA unified-cert tree. Use only while debugging TestPro Connect (Oct-08 PASS profile).

| TSP slot | Files |
|----------|--------|
| CA | root_CA.pem |
| Transport | client_tls.pem + client_tsp.key |
| MMS | client.pem + client_tsp.key (**same key both slots**) |

DUT yaml: `lab_tr400_phase4_eth_b_ttyS1_p3_07_connect.yaml` â€” **crl=off**, no revoked allowlist.

Regenerate: `powershell -File scripts\publish-p3-07-connect-profile.ps1`
