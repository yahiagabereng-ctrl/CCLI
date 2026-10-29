#!/bin/bash
cd '/mnt/c/Users/Administrator/Desktop/CCLI Alireza/CCLI Merged'
for suite in AES128-SHA256 DHE-RSA-AES128-GCM-SHA256 ECDHE-ECDSA-AES128-GCM-SHA256 AES128-SHA256:DHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES128-GCM-SHA256; do
 echo "$suite"
 timeout 12 openssl s_client -connect 192.168.0.1:3782 -tls1_2 -cipher "$suite" -cert lab/ejbca-staging/annex-tls-2026-10-28/ecdsa-clients/tls-client/ecdsa-client.chain.pem -key lab/ejbca-staging/annex-tls-2026-10-28/ecdsa-clients/tls-client/ecdsa-client.key.pem -CAfile lab/ejbca-staging/annex-tls-2026-10-28/root_CA.pem -verify_ip 192.168.0.1 -verify_return_error -brief </dev/null 2>&1
done