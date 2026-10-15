Masomeh — TLS login pack (matches DUT 2026-10-15 r55 + p3_07_connect)

1. Extract to D:\CCLI_TSP_FIELD_PACK (or any fixed path).
2. Read 00t_PC1_FIX_WRONG_TRANSPORT_CERT.md
3. Test Suite Pro: browse tls\ paths from MESSAGE_TO_SEND.txt
4. Import cid\lab_tg544_eth_a.cid
5. Connect: CCI016_01 @ 192.168.10.1:3782

Do NOT use revoked*.pem or lab_crl.pem for this session.
