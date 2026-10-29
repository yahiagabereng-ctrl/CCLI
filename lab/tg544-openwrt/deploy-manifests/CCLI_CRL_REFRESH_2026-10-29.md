# TSP TLS CRL refresh — 2026-10-29

EJBCA container ccli-ejbca-ce, CA name CCLI-Lab-CA, issuer CN=CCLI_Lab_Root_CA_2026. Used scripts/ejbca_export_crl.py with explicit CA name to issue/export fresh public CRL.

Public CRL: output/crl-refresh-20261029/fresh-tsp-crl.pem. SHA256 66e1a67fd8c7dec1d5bd5c82f4d63604c77fab47da0beaa4b949e73d9e105782.

Signature verified OK against Tespro existing TLS CA bundle. thisUpdate 2026-10-29 13:07:28 UTC; nextUpdate 2026-10-30 13:07:27 UTC. Device and container clocks checked. Replaced only third entry in /etc/ccli/rbac-dual/trust/tls-crl-bundle-20261028.pem; other two entries preserved. Backup /mnt/mmcblk0/tls-crl-bundle.before-refresh-20261029.pem. Restarted CCLI; service running and 192.168.0.1:3782 listening. No binary update. TSP connection needs client retry to confirm end-to-end.
