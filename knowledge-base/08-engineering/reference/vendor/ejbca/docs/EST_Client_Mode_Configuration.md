EST Client Mode Configuration | Keyfactor Docs

- Keyfactor Website 
- Service Catalog 
- Support Portal 
- Keyfactor Community 

# EST Client Mode Configuration

The following covers configuration of EST in Client Mode.

Configuring Aliases in Client Mode 

This covers configuring aliases in Client Mode.

Giving access to EST in Client Mode allows only pre-registered entities to enroll, using a one-time (unless configured to be allowed multiple times) enrollment code in the CSR, or a vendor client certificate (see Vendor Mode).

The following displays the EST alias configuration screen (System Configuration > EST Configuration).

 

The following lists available EST alias configuration options.

| Value | Description |
| --- | --- |
| Extract Username Component | When a request arrives, it should be matched against a pre-registered end entity in EJBCA. This value defines which subject DN field of the request DN, in the CSR of the EST request, is used to map the request to an end entity. This is the username used to register the end entity. Vendor mode works a little bit differently, see Vendor Mode. |
| Authentication Module | For authorizing initial certificate requests. Specifies where the enrollment code of the end entity is carried. Typically this is carried in a ChallengePassword attribute in the PKCS#10 CSR, see RFC 2985. One option if the client can not create a ChallengePassword attribute in the CSR is to use a specified request DN field from the CSR as enrollment code, the DnPartPwd option. |
| CA Name | The CA certificate(s) returned to the EST 'cacerts' call for this alias. As there is no end entity involved for cacerts, this may in fact be different than the CA issuing the end entity certificate, if the end entity was registered with another issuing CA. Make sure these match, depending on your use case and how you chose to use EST. |
| Vendor Certificate Mode | Instead of using an Authentication Module above, it is possible to authenticate an initial request with a TLS certificate (a vendor certificate). This is similar to Using CMP with 3GPP. |
| List of Vendor CAs | List of Certification Authorities, typically imported as External CAs, that can be used to authenticate client vendor certificates. |
| Allow ChangeSubjectName | When using Vendor Certificate Mode a common use case if that you want to modify the client subject DN, from what is given in the TLS certificate (vendor certificate), to what is provided in the CSR. According to RFC 7030 section 4.2.1, this can be allowed if the CSR carries a ChangeSubjectName attribute (RFC 6402 section 2.8), and that this EST alias allows it of course. |
| Certificate Renewal with Same Keys | If Allow is selected, a re-enrollment request may be performed for the same public key as before. Note that client certificate is always required for renewal, while username/password is never required, regardless of the settings above. |
| Enable server side key generation | This enables server side key generation via the serverkeygen functionality as per RFC 7030 for the corresponding alias. |

Network Ports and TLS Client Certificate Authentication 

Using EST can be challenging from a network configuration point of view. Some calls such as cacerts, and simpleenroll should be performed over TLS, but usually with no requirement of client certificate authentication as the client is not in possession of a usable key pair and certificate yet. A call to simplereenroll however requires client certificate authentication to authenticate with the clients current certificate, in order to automatically approve renewal and issuance of a new certificate. To make things more complex, when using Vendor mode also simpleenroll requires TLS client certificate authentication.

EJBCA offers different installation options, such as direct server deployment in Wildfly, containers, software and hardware appliances. The port for EST communication needs to be chosen carefully based on the installation type.

In the case of direct server deployment, you can set up TLS and client certificate authentication in Wildfly in the following ways:

- 3-port separation:

- port 8080: without TLS support for non-confidential public communication
- port 8442: TLS with only server authentication
- port 8443: TLS with server and client authentication (m-TLS)
- 2-port separation:

- port 8080: without TLS support for non-confidential public communication
- port 8443: TLS with server and client authentication (m-TLS)

For 3-port separation, port 8442 is used for communication without certificate authentication, and port 8443 is used when certificate authentication or m-TLS is needed. Note that the EST simplereenroll operation always needs certificate authentication. Additionally, if Vendor mode is used, then the EST simpleenroll operation also requires client certificate authentication.

For 2-port separation, port 8443 can be used for all scenarios.

The network configuration is less complicated when using a hardware or software appliance, where EST can be used without mentioning a port, e.g. https://ejbca.example.com. This is applicable to both with and without client certificate authentication. The proxy in front of EJBCA allows optional client authentication similar to a 2-port separation but with the default HTTP port 80 and HTTPS port 443. Thus, all HTTPS communication is subject to optional client authentication. 

If using a proxy in front of JBoss/WildFly you can enable EST working on the single TLS port (i.e. a simple URL like https://ejbca.example.com/), by making client certificate authentication optional. Such an endpoint proxy configuration is easy to do with Apache or Nginx and is used in PrimeKey's Appliance and Cloud products.

Client Mode with Challenge Password Authentication

Example enrollment, where an end entity is pre-registered in EJBCA, the client gets a CA certificate first and then enrolls for a client certificate, using a ChallengePassword in the CSR (the EST client is an untrusted end entity). Next, the client performs a renewal, authenticating with the client certificate issued previously.

EST configuration is an EST alias with the name est and the following settings:

| Setting | Value |
| --- | --- |
| EST Operational Mode | Client |
| Extract Username Components | CN |
| Authentication Module | ChallengePwd |
| CA Name | ManagementCA |
| Vendor Certificate Mode | false |
| Certificate Renewal with Same Keys | true |
| Enable server side key generation | false |

Download the CA certificate of the EJBCA TLS connection (usually the default is Management CA), and set the environment variable needed for estclient:

```
$ export EST_OPENSSL_CACERT=/tmp/ManagementCA.cacert.pem

```

Using the Cisco EST Client libEST, run the following estclient commands to generate a key and get a certificate from the CA:

```
mkdir certs
 
# Get CA certificate, by "RA:
./estclient -g -s 127.0.0.1 -p 8442 -o certs --pem-output
 
# Inspect the fetched CA certificate
openssl x509 -in certs/cacert-0-0.pem -text -noout
 
# Get client certificate, for username selected by CN (myclient) authenticated with enrollment code as ChallengePassword in the CSR
# First create the CSR and inspect it
cat > openssl.conf
[ req ]
distinguished_name     = req_distinguished_name
attributes             = req_attributes

[ req_distinguished_name ]
commonName                     = Common Name (eg, YOUR name)
commonName_max                 = 64

[ req_attributes ]
challengePassword              = A challenge password
challengePassword_min          = 4
challengePassword_max          = 20
^D

openssl req -nodes -newkey rsa:2048 -keyout device.key -out device.csr -config openssl.conf
# Enter Common Name '123456789' and challenge password 'password'
openssl req -in device.csr -text
# Then make the EST request
./estclient -e -s 127.0.0.1 -p 8442 -o certs --pem-output -y device.csr
 
# Inspect the fetched client certificate
openssl x509 -in certs/cert-0-0.pem -text -noout
 
# Re-enroll, directly by the client when certificate is about to expire, using the old client cert to authenticate with:
# There is a bug in the estclient making it impossible to use a new key/csr, so you have to allow renewal with same key
./estclient -r -s 127.0.0.1 -p 8443 -o certs -c certs/cert-0-0.pem -k device.key --pem-output
 
# Inspect the new, renewed, client certificate 
$ openssl x509 -in certs/cert-0-0.pem -text -noout
 
# Revoke the clients certificates by going to CA UI and using Search End Entities to find the end entity and revoke certificates.
# Try to re-enroll again, which will not work with a revoked client certificates.
./estclient -r -s 127.0.0.1 -p 8443 -o certs -c certs/cert-0-0.pem -k device.key --pem-output
 
# Enroll with an EC key. First we have to generate the key, a Prime256v1 key on this case, and then use this key for enrollment.
openssl ecparam -name prime256v1 -genkey -noout -out prime256v1-key.pem
openssl req -nodes -new -key prime256v1-key.pem -out device-ec.csr -config openssl.conf
# Enter Common Name '123456789' and challenge password 'password'
openssl req -in device-ec.csr -text
./estclient -e -s 127.0.0.1 -p 8442 -o certs --pem-output -y device-ec.csr
openssl x509 -in certs/cert-0-0.pem -text -noout
 
# Re-enroll, also with EC, using the same key (using the same key can be dissalowed in the EST alias).
./estclient -r -s 127.0.0.1 -p 8443 -o certs -c certs/cert-0-0.pem -k prime256v1-key.pem --pem-output
openssl x509 -in certs/cert-0-0.pem -text -noout

```

You can also use curl to make the EST calls. Note that the base64 encoding must be over the binary CSR. The command for initial enrollment using curl would be (using the same generated CSR as above).

```
# Convert the PEM encoded CSR to DER encoded and base64 encode
openssl req -in device.csr -outform DER -out device.csr.der
openssl base64 -in device.csr.der -out device.b64 -e
# Make the EST request
curl -v --cacert /tmp/ManagementCA.cacert.pem --data @device.b64 -o device-p7.b64 -H "Content-Type: application/pkcs10" -H "Content-Transfer-Encoding: base64" https://127.0.0.1:8442/.well-known/est/est/simpleenroll
# Decode the response and look at the cert
openssl base64 -in device-p7.b64 -out device-p7.der -d
openssl pkcs7 -inform DER -in device-p7.der -print_certs -out device-cert.pem
openssl x509 -in device-cert.pem -text -noout

```

Server side key generation in client mode 

Clients can request a keypair generated by server in a very similar manner to enroll or re-enroll operations by libest or curl/HTTP(s) calls. Administrator first needs to enable "Enable server side key generation" for the corresponding EST alias. Administrator must also register the client or end entity in EJBCA similar to enroll. We also need to set the TLS ca certificate same as previous section. Then,

Client mode serverkeygen

```
# Generate a CSR with same parameters in registered in EJBCA and private key and then base64 encode
openssl req -nodes -newkey rsa:2048 -keyout device.key -out device.csr -outform DER -subj "/CN=registered_client"
openssl base64 -in device.csr -out device.b64 -e

# with libest: -q is used for reenroll, only the generated private key is used to construct the request
./estclient -q -s 127.0.0.1 -p 8442 -o outputDirName --pem-output --common-name registered_client -x device.key -v
# libest will split the certificate and private key in separate files in outputDirName with names cert-0-0.pem and key-0-0.key respectively. We need to add -----BEGIN RSA PRIVATE KEY----- header and -----END RSA PRIVATE KEY----- footer manually to private key file key-0-0.key to use it during reenroll or to verify with openssl.

# inspect the certificate
openssl x509 -in "outputDirName/cert-0-0.pem" -noout -text

# inspect the generated key used in request and notice the modulus is different as mentioned in certificate
openssl rsa -in device.key -check -text
# but it is same as the private key recieved in response
openssl rsa -in key-0-0.key -check -text

# with curl command: the base64 encoded CSR is used as request, similar to simpleenroll
curl -v --cacert ManagementCA.cacert.pem --data @device.b64 -o device-keypair -H "Content-Type: application/pkcs10" -H "Content-Transfer-Encoding: base64" https://172.17.2.104:8442/.well-known/est/est/serverkeygen

# we may manually parse the contents of device-keypair file and split it to separate files for key and certificate or use the script in mentioned below
./parse_server_keygen_response device-keypair

# parse the certificate response similar to during simpleenroll
openssl base64 -in device-p7.b64 -out device-p7.der -d
openssl pkcs7 -inform DER -in device-p7.der -print_certs -out device-cert.pem
openssl x509 -in device-cert.pem -noout -text

### reenroll or subsequent key generation port 8443 is used for mutual TLS with the key and certificate generated previously
# with libest
./estclient -q -s 127.0.0.1 -p 8443 -o outputDirName --pem-output --common-name registered_client   -x device.key -v -c "outputDirName/cert-0-0.pem" -k "outputDirName/key-0-0.key"

# with curl
curl -v --cacert ManagementCA.cacert.pem --cert device-cert.pem --key enrolled-key.key --data @device.b64 -o device-keypair2 -H "Content-Type: application/pkcs10" -H "Content-Transfer-Encoding: base64" https://127.0.0.1:8443/.well-known/est/est/serverkeygen

```

Script for separating the serverkeygen response,

parse_server_keygen_response

```
#!/bin/bash

line_nums=($(grep -wn "\-\-CONTENTBOUNDARY" $1| cut -d: -f1))

priv_key_start=$((${line_nums[0]} + 4))
priv_key_end=$((${line_nums[1]} - 1))
cert_start=$((${line_nums[1]} + 4))
cert_end=$((${line_nums[2]} - 1))

# only for RSA keys
echo "-----BEGIN RSA PRIVATE KEY-----" > enrolled-key.key
sed -n -e "$priv_key_start,${priv_key_end}p" $1 >> enrolled-key.key
echo "-----END RSA PRIVATE KEY-----" >> enrolled-key.key

echo "-----BEGIN CERTIFICATE-----" > device-p7.b64
sed -n -e "$cert_start,${cert_end}p" $1 >> device-p7.b64
echo "-----END CERTIFICATE-----" >> device-p7.b64

```

Vendor Mode 

Example enrollment, where an end entity is pre-registered in EJBCA, the client gets CA certificate first and then enrolls for a client certificate, using a TLS vendor client certificate pre-installed on the device. EJBCA knows of the certification authority issuing the vendor client certificate as an External CA.

Importing Vendor CAs into EJBCA

Here is the VendorRootCAEC384.cacert.pem certificate for our example.

```
cat > VendorRootCAEC384.cacert.pem
-----BEGIN CERTIFICATE-----
MIIB5zCCAY2gAwIBAgIIfIFBpHsapFAwCgYIKoZIzj0EAwMwRzELMAkGA1UEBhMC
U0UxGTAXBgNVBAoMEFByaW1lS2V5IEZhY3RvcnkxHTAbBgNVBAMMFFZlbmRvciBS
b290IENBIEVDMzg0MB4XDTIwMTAxOTA2MjExMloXDTQwMTAxNDA2MjExMlowRzEL
MAkGA1UEBhMCU0UxGTAXBgNVBAoMEFByaW1lS2V5IEZhY3RvcnkxHTAbBgNVBAMM
FFZlbmRvciBSb290IENBIEVDMzg0MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAE
Pg1gVL60dcs+pI0xqdt+ZxFsi0z0PM/++tXy3CTN1MIL+UL7hjIbHZoKHDydDjvX
Jh2wNEvR1uby3XIwg0+zVaNjMGEwDwYDVR0TAQH/BAUwAwEB/zAfBgNVHSMEGDAW
gBSK0LnIltrD+4XTciuLinXtYpk/yzAdBgNVHQ4EFgQUitC5yJbaw/uF03Iri4p1
7WKZP8swDgYDVR0PAQH/BAQDAgGGMAoGCCqGSM49BAMDA0gAMEUCIQDsFbCpZbxA
tPYrCaNm+8viymtxgW59rrUqbi4GweEqBQIgMvS0OeKVASGAeBIUoI3GBgszzVgj
M6MzBjOO586jzSs=
-----END CERTIFICATE-----

cat > VendorSubCAEC384.cacert.pem
-----BEGIN CERTIFICATE-----
MIIB5jCCAYugAwIBAgIIc8lyzz9D+tQwCgYIKoZIzj0EAwMwRzELMAkGA1UEBhMC
U0UxGTAXBgNVBAoMEFByaW1lS2V5IEZhY3RvcnkxHTAbBgNVBAMMFFZlbmRvciBS
b290IENBIEVDMzg0MB4XDTIwMTAxOTA2MjE0MVoXDTQwMTAxNDA2MjExMlowRTEL
MAkGA1UEBhMCU0UxGTAXBgNVBAoMEFByaW1lS2V5IEZhY3RvcnkxGzAZBgNVBAMM
ElZlbmRvciBTdWJDQSBFQzM4NDBZMBMGByqGSM49AgEGCCqGSM49AwEHA0IABP4c
764FiC2EPqHqkHmRn7OdQoinqWe73yKfBSdTxppnQntMW1oq4kkO1A9qxWgiBN4f
nvx/oByN38Pe1Sl0w1yjYzBhMA8GA1UdEwEB/wQFMAMBAf8wHwYDVR0jBBgwFoAU
itC5yJbaw/uF03Iri4p17WKZP8swHQYDVR0OBBYEFOAbHblwsKqYhl4u5DX9hBpN
KAAWMA4GA1UdDwEB/wQEAwIBhjAKBggqhkjOPQQDAwNJADBGAiEAhWK79GK75Y++
uVLcF6TK2+FWvF0bN820Fwsdmzqsk1ACIQCkwGxvCmZ5RBclq3aWS37wnfucxqeU
6efhzZTOTcKwGA==
-----END CERTIFICATE-----

```

Import these certificates into EJBCA as external CAs in the Admin UI with Certification Authorities→Import CA certificate, give it the names Vendor Root CA and Vendor Sub CA.

In order to accept TLS connections with this CA as trust anchor you must also import it into the JBoss/WildFly truststore. In a default installation of EJBCA this is located under $JBOSS_HOME/standalone/configuration/keystore/truststore.jks.

```
keytool -importcert -trustcacerts -keystore truststore.jks -alias VendorRootCA -file VendorRootCAEC384.cacert.pem
keytool -importcert -trustcacerts -keystore truststore.jks -alias VendorSubCA -file VendorSubCAEC384.cacert.pem

```

Creating EST Alias 

EST configuration is an EST alias with the name estvendor and the following settings:

| Setting | Value |
| --- | --- |
| EST Operational Mode | Client |
| Extract Username Components | CN |
| CA Name | ManagementCA |
| Authentication Module | None |
| Vendor Certificate Mode | true |
| List of Vendor CAs | Vendor Sub CA |
| Allow ChangeSubjectName | true |
| Certificate Renewal with Same Keys | false |
| Enable server side key generation | false |

Pre-registering client

The client to be authorized for enrollment must be added to EJBCA in order to authorize this specific client to receive a certificate. The end entity can be added using the CLI, UI or using the WS or REST API.

The CN typically matches the serial number of the device to be enrolled. The password passed on the command line is not needed for the enrollment and can be set to a random value, or if password is not required in the end entity profile, can be set to 'null'. In this example the Subject DN of the vendor certificate is the same as the requested certificate DN will be. When this is the case the ChangeSubjectName attribute in the CSR need not be used.

With EST, for renewal, it is important that the new request DN and altName are the same in the CSR as in the existing certificate used for TLS renewal authentication, or the renewal request will fail. The ChangeSubjectName CSR attribute can be used for initial enrollment in Vendor Mode, but not for renewal which is important in order to not let one device assume the identity of another device during renewal.

```
bin/ejbca.sh ra addendentity --username 1234.primekey.com --dn "CN=1234.primekey.com,O=PrimeKey,C=SE" --caname ManagementCA --eeprofile=TLSClientEEProfile --certprofile=TLSEndEntityCertProfile --type 1 --token USERGENERATED --password ZUhgGPuu-doesnt-matter-randomize

```

To enroll to an Operator certificate, the client must send the Vendor certificate, and Sub CA, in the TLS handshake field of the EST request.

The following provides the vendor-key.pem (device vendor private key), vendor-cert.pem (device vendor certificate), and VendorSubCAEC384.cacert.pem (vendor Sub CA certificate).

In the client terminal, create the needed certificate files:

```
cat > vendor-key.pem
-----BEGIN PRIVATE KEY-----
MIGTAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBHkwdwIBAQQgUmy7YVUMGuemV637
Vcq91ZNL9WAnF56fC1dThaGy5T6gCgYIKoZIzj0DAQehRANCAASyrEYCkTX4ApjD
R9E36V63cUgkZwD4raKpGMZ6jO2RnfPjtcalGK1sDdlF+GSh9XpLul9GibPiDLIX
Ten4Un2I
-----END PRIVATE KEY-----

cat > vendor-cert.pem
-----BEGIN CERTIFICATE-----
MIIB2DCCAX2gAwIBAgIIVZHqvoM7h7wwCgYIKoZIzj0EAwMwRTELMAkGA1UEBhMC
U0UxGTAXBgNVBAoMEFByaW1lS2V5IEZhY3RvcnkxGzAZBgNVBAMMElZlbmRvciBT
dWJDQSBFQzM4NDAeFw0yMDEwMTkwNjIyNTlaFw00MDEwMTQwNjIxMTJaMDwxCzAJ
BgNVBAYTAlNFMREwDwYDVQQKDAhQcmltZUtleTEaMBgGA1UEAwwRMTIzNC5wcmlt
ZWtleS5jb20wWTATBgcqhkjOPQIBBggqhkjOPQMBBwNCAASyrEYCkTX4ApjDR9E3
6V63cUgkZwD4raKpGMZ6jO2RnfPjtcalGK1sDdlF+GSh9XpLul9GibPiDLIXTen4
Un2Io2AwXjAMBgNVHRMBAf8EAjAAMB8GA1UdIwQYMBaAFOAbHblwsKqYhl4u5DX9
hBpNKAAWMB0GA1UdDgQWBBRln9C8ctMywHZ+Azk4IsZnu0HmXzAOBgNVHQ8BAf8E
BAMCB4AwCgYIKoZIzj0EAwMDSQAwRgIhAImjJNNR2LkSWBBmThF3SrhErXtFB5t6
T9AZPqRXnuGQAiEAt9RUxTqGc0d4olxtsMU7O/yJ/L2OayUruRjOOZZHLcc=
-----END CERTIFICATE-----

```

In order to perform renewal using EST you have to ensure that the certificate profiles used to issue the client certificate are usable for TLS client certificate authentication. This means for example typically to include 'Client authentication' in the extended key usage extension.

Enrolling with Vendor certificate TLS authentication 

Here is a full work-flow example using CURL, where initial enrollment uses TLS client certificate authentication using a Vendor certificate, and re-enroll using the existing client (operator) certificate.

```
# Get CA certificates
curl https://localhost:8442/.well-known/est/estvendor/cacerts -o cacerts.p7 --cacert ManagementCA.cacert.pem
# Generate a key and CSR for a "device"
# Make sure the subject DN and subject alternative name matches the requirements of the end entity profile.
openssl req -nodes -newkey rsa:2048 -keyout device.key -out device.csr -outform DER -subj "/CN=1234.primekey.com/O=PrimeKey/C=SE"
openssl req -inform DER -in device.csr -text -noout
openssl base64 -in device.csr -out device.b64 -e
# Make initial enrollment, using TLS client certificate authentication
curl -v --cacert ManagementCA.cacert.pem --cert vendor-cert.pem --key vendor-key.pem --data @device.b64 -o device-p7.b64 -H "Content-Type: application/pkcs10" \
-H "Content-Transfer-Encoding: base64" https://localhost:8443/.well-known/est/estvendor/simpleenroll
# Convert the response into a PEM encoded certificate
openssl base64 -in device-p7.b64 -out device-p7.der -d
openssl pkcs7 -inform DER -in device-p7.der -print_certs -out device-cert.pem
openssl x509 -in device-cert.pem -text -noout

# Generate a new key and CSR for the device, to renew with
openssl req -nodes -newkey rsa:2048 -keyout device-new.key -out device-new.csr -outform DER -subj "/CN=1234.primekey.com/O=PrimeKey/C=SE"
openssl base64 -in device-new.csr -out device-new.b64 -e
# Re-enroll by the device, authenticating with the existing key/certificate
curl -v --cacert ManagementCA.cacert.pem --key device.key --cert device-cert.pem --data @device-new.b64 -o device-new-p7.b64 -H "Content-Type: application/pkcs10" -H "Content-Transfer-Encoding: base64" https://localhost:8443/.well-known/est/estvendor/simplereenroll
# Convert the response into a PEM encoded certificate
openssl base64 -in device-new-p7.b64 -out device-new-p7.der -d
openssl pkcs7 -inform DER -in device-new-p7.der -print_certs -out device-new-cert.pem
openssl x509 -in device-new-cert.pem -text -noout

```

ManagementCA.cacert.pem is the Root CA certificate of the CA chain that issued the TLS server cert and needs to be configured with curl in order for the TLS connection to be established. ManagementCA is the name in a default EJBCA installation and the cert can be downloaded from the CA UI or RA UI.

The examples above use port 8442 for TLS with server authentication, and port 8443 for TLS with both client and server authentication (when using a client authentication certificate and private key). The standard TLS port, 443, is used when running an Apache proxy in front of EJBCA, such as is the case with an EJBCA Cloud instance or a EJBCA Hardware Appliance. For EJBCA Enterprise Cloud instances or the PrimeKey Hardware Appliance, port 443 will work for calls with or without client certificate authentication.

Server side key generation with Vendor mode 

We can also use serverkeygen functionality with vendor mode. Workflow is quite similar to vendor mode enrollment and the only difference is in the response. Hence commands in previous section can be used as they are. Administrator must enable "Enable server side key generation" in corresponding EST alias and register the end entity before the enrollment. The response can be split into private key and certificate using the script in parse_server_keygen_response. Then we can decode and inspect the certificate similar to last section. Note that, the public key in certificate is different than the one in CSR.