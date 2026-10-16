# TSP P3-07 — Live association pass (2026-10-16)

**Result:** **PASS** — Test Suite Pro **Connected**, **Initiate Response Success**.

| Item | Value |
|------|--------|
| DUT | TG544 `192.168.10.1:3782` (pid 20381) |
| Firmware | **0.1.0-r57** `P5-5LN-ISO-PRES-AARE` |
| ELF SHA-256 | `96963619dbebb87e366510a555fdac729a4b948847b76f1f91d94b5b6cd7d697` |
| Config | `product_tr400_p3_07_connect_5ln.yaml` |
| Model | `product_tr400_eth_a_5ln.cfg` (5 LN) |
| TLS | P3-07 Connect · crl=off |
| Freeze | `releases/product-cyber/0.1.0-r57/` |

```text
mms: iso session CONNECT (payload=1332)
mms: iso presentation skip non-ACSE/MMS context id=5 (mmsAuthenticationAS)
mms: ACSE auth ACCEPT — mech=CERTIFICATE role=DSO_OPERATOR cert=1071 octets
mms: AARE responder auth [10]EXTERNAL ctx=5 inner=1075 octets
mms: iso session ACCEPT extended LI (spdu=1264 payload=1244)
```
