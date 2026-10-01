# CCI Module — Hardware/Protocol Inventory & Regulations Classification  
# Modulo CCI — Inventario HW/protocolli e classificazione normative

**Doc ID:** CCLI-REGS-CLASS-001  
**Revision:** 1.2  
**Date:** 2026-09-21  
**Purpose / Scopo:** Classify every norm cited by the AiLux-class CCI module description into RAG categories, map features → regulations → `source_id`, and mark ingest status.  
**Audience / Destinatari:** Architecture freeze, certification planning, Telematry RAG routing.  
**Licensed IEC drop (2026-09-18):** `07-protocols/regulations/standards-drop-20260918/` — inventory `ccli-standards-pdf-inventory` (`CCI_Standards_PDF_Inventory.md`).

---

## 0. Italian ↔ English glossary / Glossario IT ↔ EN

| Italiano (source) | English |
|-------------------|---------|
| CCI (Centralina / Controllore di Impianto) | Plant Controller / Central Plant Controller |
| DSO | Distribution System Operator |
| Operatore Abilitato (OA) | Qualified / Authorized Operator |
| Elementi di Impianto | Plant elements / DER field devices |
| Allegato O / Allegato T | Annex O / Annex T |
| Normative di riferimento | Reference standards / normative references |
| Cyber Security | Cybersecurity |
| Secur Boot *(datasheet spelling)* | Secure Boot |
| Anti-Tamper / antimanomissione | Anti-tamper / anti-tampering |
| Porte Seriali optoisolate | Opto-isolated serial ports |
| Consolle di Configurazione e Manutenzione Locale | Local configuration & maintenance console |
| Gestione Armamenti | Arming management (telecontrol arming) |
| Protocolli sicuri | Secure protocols |
| Ingressi Digitali / Uscite Digitali | Digital inputs (DI) / Digital outputs (DO) |
| da energizzare con alimentazione ESTERNA | to be energized from EXTERNAL field supply |
| Temperatura di esercizio | Operating temperature |
| Delibera ARERA | ARERA Decision / Resolution |
| conforme a | compliant with / conforms to |

---

## 1. Module inventory (source description)  
## 1. Inventario modulo (descrizione sorgente)

### 1.0 Full source text (IT) + English translation

**Italiano (originale):**

> CPU iMX6D ARM Cortex-A9, 1GB DDR3, 4GByte eMMC, 800Mhz, CAAM, CSU, A-HAB SHA-256. Cyber Security: Secure Boot, Trusted Platform Module 2.0, FIPS 140-2 Level 3, RBAC, TRNG. Anti-Tamper hardware con switch collegato alla copertura e accelerometro X-Y-Z antimanomissione. Conforme a IEC-62443-4-1/2. Ethernet Switch a 4 porte 10/100 Mbps: una porta dedicata a DSO (ETH_A) e una porta dedicata a Operatore Abilitato (ETH_B); due porte per connessioni agli Elementi di Impianto; connettori RJ45 8/8. Due porte Seriali optoisolate RS-232/RS-485 su RJ45 8/8 standard ModBus. Una porta USB 2.0 per Consolle di Configurazione e Manutenzione Locale; due porte USB 2.0 per Elementi di Impianto. Protocolli seriali: IEC 61158-15 ModBus-RTU; IEC 60870-5-101 Balance/Unbalance; IEC 60870-5-103. Protocolli Ethernet: IEC 60870-5-104 Slave/Reverse/Redundancy (Gestione Armamenti Multicast/Unicast); IEC-61850 Server/Client/Goose; ModBus-TCP/IP Master/Slave; DNP3; S7-Comm; OPC-UA Client; C37.118 Client; IEC 62056-21 (DLMS/COSEM). Protocolli sicuri IEC 62351 parti 3–9 e 14. Servizi: http(s), sftp, ssh, snmp v2/v3 trap, ntpd, client/server PTP (IEEE-1588 power profile), redundancy cover system, firewall, RBAC, tasks ladder IEC 61131-3, web interface diagnostica/logs/SCADA. N°5 Ingressi Digitali optoisolati 10–120 Vdc (alimentazione esterna di campo); N°3 Uscite Digitali a relè 250 Vac 3A; GPS/GLONASS; modem 2G/3G/4G/LTE Cat.1; display OLED 1″; MicroSD fino a 32 GB; alimentazione 12–24 Vdc ±20% 5 W; temperatura di esercizio −5 °C…+45 °C. Normative di riferimento: CEI 0-16 allegati O–T; CEI EN 61557-12:2018; ARERA 540/2021; FIPS 140-2; IEC 62351-3/-4/-5/-6.

**English:**

> CPU: i.MX6D ARM Cortex-A9, 1 GB DDR3, 4 GB eMMC, 800 MHz, CAAM, CSU, A-HAB SHA-256. Cybersecurity: Secure Boot, TPM 2.0, FIPS 140-2 Level 3, RBAC, TRNG. Hardware anti-tamper with cover-linked switch and X-Y-Z anti-tampering accelerometer. Compliant with IEC 62443-4-1/2. 4-port 10/100 Mbps Ethernet switch: one port dedicated to the DSO (ETH_A), one port dedicated to the Qualified/Authorized Operator (ETH_B); two ports for connections to plant elements (DER field devices); RJ45 8/8 connectors. Two opto-isolated serial ports RS-232/RS-485 on RJ45 8/8 Modbus pinout. One USB 2.0 port for local configuration & maintenance console; two USB 2.0 ports for plant elements. Serial protocols: IEC 61158-15 Modbus-RTU; IEC 60870-5-101 balanced/unbalanced; IEC 60870-5-103. Ethernet protocols: IEC 60870-5-104 Slave/Reverse/Redundancy (arming management via multicast and unicast); IEC 61850 Server/Client/GOOSE; Modbus-TCP/IP Master/Slave; DNP3; S7-Comm; OPC-UA Client; C37.118 Client; IEC 62056-21 (DLMS/COSEM). Secure protocols: IEC 62351 parts 3–9 and 14. Services: http(s), sftp, ssh, SNMP v2/v3 trap, ntpd, PTP client/server (IEEE 1588 power profile), redundancy cover system, firewall, RBAC, IEC 61131-3 ladder tasks, web UI for diagnostics/logs/SCADA. 5 opto-isolated digital inputs 10–120 Vdc (external field supply); 3 relay digital outputs 250 Vac / 3 A; GPS/GLONASS; 2G/3G/4G/LTE Cat.1 modem; 1″ OLED; MicroSD up to 32 GB; supply 12–24 Vdc ±20%, 5 W; operating temperature −5 °C…+45 °C. Reference standards: CEI 0-16 Annexes O–T; CEI EN 61557-12:2018; ARERA Decision 540/2021; FIPS 140-2; IEC 62351-3/-4/-5/-6.

### 1.1 Platform / CPU

| Item (EN) | Voce (IT) | Spec |
|-----------|-----------|------|
| CPU | CPU | i.MX6D ARM Cortex-A9, 800 MHz |
| RAM / storage | RAM / memoria | 1 GB DDR3, 4 GB eMMC |
| Crypto silicon | Silicio crittografico | CAAM, CSU, A-HAB SHA-256 |
| Secure boot | Secure Boot | Secure Boot + HAB-class chain |
| TPM | TPM | TPM 2.0, FIPS 140-2 Level 3 |
| Access control | Controllo accessi | RBAC |
| Entropy | Entropia | TRNG |
| IACS process | Processo IACS | Conforms to / conforme a IEC 62443-4-1 / 4-2 |
| Anti-tamper | Anti-manomissione | Cover switch + X-Y-Z accelerometer |

### 1.2 Networking & I/O

| Item (EN) | Voce (IT) | Spec |
|-----------|-----------|------|
| Ethernet switch | Switch Ethernet | 4× 10/100 Mbps, RJ45 8/8 |
| ETH_A | ETH_A | Dedicated DSO / dedicata al DSO |
| ETH_B | ETH_B | Dedicated Qualified Operator (Operatore Abilitato) |
| ETH plant | ETH impianto | 2× plant elements / Elementi di Impianto |
| Serial | Porte seriali | 2× optoisolated RS-232 / RS-485 on RJ45 8/8 Modbus pinout |
| USB | USB | 1× USB 2.0 local config/maintenance console; 2× USB 2.0 plant elements |
| DI | Ingressi digitali | 5× optoisolated 10–120 Vdc (external field supply) |
| DO | Uscite digitali | 3× relay 250 Vac / 3 A |
| GNSS | GNSS | GPS/GLONASS, SMA-F, active antenna 5 V out, 5 m RG174 |
| Cellular | Modem cellulare | Integrated 2G/3G/4G/LTE Cat.1, SMA-F |
| HMI | Display | OLED 1″ local diagnostics |
| Storage | Storage | MicroSD ≤ 32 GB (logs/storage) |
| Power | Alimentazione | 12–24 Vdc ±20%, 5 W |
| Ambient | Temperatura di esercizio | −5 °C … +45 °C |

### 1.3 Serial protocols / Protocolli seriali (RS-232 / RS-485)

- IEC 61158-15 Modbus-RTU  
- IEC 60870-5-101 Balanced / Unbalanced *(bilanciato / sbilanciato)*  
- IEC 60870-5-103  

### 1.4 Ethernet protocols / Protocolli Ethernet

- IEC 60870-5-104 Slave / Reverse / Redundancy — arming management / *Gestione Armamenti* (multicast & unicast)  
- IEC 61850 Server / Client / GOOSE  
- IEC 61158-15 Modbus-TCP/IP Master and/or Slave  
- DNP3  
- S7-Comm  
- OPC-UA Client  
- IEEE C37.118 Client  
- IEC 62056-21 (DLMS/COSEM)  

### 1.5 Secure protocol stack / Protocolli sicuri (IEC 62351)

| Part | Scope EN (per module claim) | Ambito IT |
|------|-----------------------------|-----------|
| IEC 62351-3 | TLS for IEC 60870-5-101/104 and DNP3 | TLS per 101/104 e DNP3 |
| IEC 62351-4 | Security for IEC 61850 | Sicurezza per IEC 61850 |
| IEC 62351-5 | Security extensions for IEC 60870-5-101/104 and DNP3 | Estensioni di sicurezza 101/104 e DNP3 |
| IEC 62351-6 | Security for IEC 61850 (GOOSE/SV related) | Sicurezza IEC 61850 (GOOSE/SV) |
| IEC 62351-7 | SNMP network/system management security | Sicurezza SNMP |
| IEC 62351-8 / -9 | Role-based access / key management for IEC 60870-5-101/104, IEC 61850, DNP3 | RBAC / gestione chiavi |
| IEC 62351-14 | Syslog | Syslog |

### 1.6 Services / Servizi

http(s), sftp, ssh, SNMP v2/v3 trap, ntpd, PTP client/server (IEEE 1588 power profile), redundancy cover system, firewall, RBAC, IEC 61131-3 ladder tasks (*tasks ladder*), web UI (diagnostics / *diagnostica*, logs, SCADA).

### 1.7 Reference standards / Normative di riferimento (explicit on datasheet)

| EN | IT |
|----|-----|
| CEI 0-16 Annexes O–T | CEI 0-16 allegati O–T |
| CEI EN 61557-12:2018 | CEI EN 61557-12:2018 |
| ARERA Decision 540/2021 | Delibera ARERA 540/2021 |
| FIPS 140-2 | FIPS 140-2 |
| IEC 62351-3 / -4 / -5 / -6 | IEC 62351-3 / -4 / -5 / -6 |

*(IEC 62443-4-1/2 is claimed under Cybersecurity / *Cyber Security*, not only in the normative bullet list.)*

---

## 2. Regulation taxonomy (RAG classes)  
## 2. Tassonomia normative (classi RAG)

Use these **class codes** when routing queries and when ingesting PDFs.  
Usare questi **codici classe** per il routing delle query e per l’ingest dei PDF.

| Class EN | Classe IT | Code | Meaning EN / Significato IT |
|----------|-----------|------|-----------------------------|
| Italian grid / CCI mandate | Mandato rete IT / CCI | `GRID-IT` | Connection rules for DER plant controller (CCI) / Regole di connessione per il Controllore di Impianto |
| Market / regulator | Mercato / regolatore | `REGULATOR-IT` | ARERA decisions binding Italian DSO/plant ops / Delibere ARERA vincolanti |
| Measurement / PQ | Misura / qualità | `METROLOGY` | Performance measuring / monitoring equipment / Apparecchiature di misura e monitoraggio |
| IACS cybersecurity process & component | Cybersecurity IACS | `CYBER-IACS` | IEC 62443 product/process |
| Crypto module validation | Validazione modulo crypto | `CYBER-CRYPTO` | FIPS / TPM claims / Dichiarazioni FIPS/TPM |
| Power-system protocol security | Sicurezza protocolli power | `CYBER-PROTO` | IEC 62351 family / Famiglia IEC 62351 |
| EU product cyber law | Normativa cyber UE prodotto | `CYBER-EU` | CRA (and related) / Cyber Resilience Act |
| EU energy policy | Politica energetica UE | `ENERGY-EU` | RED II (contextual) / Direttiva rinnovabili |
| Field / SCADA protocols | Protocolli di campo / SCADA | `PROTO-FIELD` | 60870 / 61850 / Modbus / DNP3 / … |
| Time sync | Sincronizzazione oraria | `TIME` | NTP / PTP / GNSS |
| PLC / application | PLC / applicazione | `APP-PLC` | IEC 61131-3 |
| ATEX / hazardous area | ATEX / area pericolosa | `ATEX` | Existing Telematry corpus (not CCI datasheet primary) |
| Product EMC / radio | EMC / radio prodotto | `EMC-RADIO` | Cellular modem / emissions (if certified later) |

---

## 3. Classification table — every cited norm  
## 3. Tabella di classificazione — ogni norma citata

| Norm / Norma | Class | Applies to EN / IT | Priority | RAG status | `source_id` (target or actual) |
|--------------|-------|--------------------|----------|------------|--------------------------------|
| CEI 0-16 consolidated 2025-12 (working) | `GRID-IT` | Full norm + V1–V5 incl. ARERA 385/2025 | P0 | **INGESTED** | `cei-0-16-consolidata-2025-12` → `07-protocols/0-16consolidata.pdf` |
| CEI 0-16 Annex O / Allegato O | `GRID-IT` | CCI functional / plant controller role | P0 | **INGESTED** | `cei-0-16-allegato-o` → `CCI_Annex_O_Extract.md` |
| CEI 0-16 Annex T / Allegato T | `GRID-IT` | IEC 61850 exchange with DSO / scambio IEC 61850 con DSO | P0 | **INGESTED** | `cei-0-16-allegato-t` → `CCI_Annex_T_Extract.md` |
| CEI 0-16 Annex M / Allegato M | `GRID-IT` | Defence plan telescatto / DI-DO to PI | P1 | **INGESTED** | `cei-0-16-allegato-m` → `CCI_Annex_M_Extract.md` |
| CEI TR 57-126 (SCL example for CCI) | `GRID-IT` | Concrete IEC 61850 CID/SCL per Allegato T §T.3 | P0 | **INGESTED** | `cei-tr-57-126` → `CCI_TR_57-126_Extract.md` + `config/icd/cei-tr-57-126-example.cid` |
| ARERA 385/2025/R/EEL | `REGULATOR-IT` | V5 Allegati O/T/U driver (100–500 kW) | P1 | **PART** | Summarised in `CCI_Annex_O_Extract.md` §2.1; full delibera **MISSING** |
| AiLux CCI manual (impl. baseline) | `GRID-IT` | Full module behaviour reference / riferimento comportamento modulo | P0 | **INGESTED** | `cei-0-16-cci-manual-ailux` |
| ARERA Decision / Delibera 540/2021 | `REGULATOR-IT` | Italian market obligations tied to CEI 0-16 CCI | P0 | **INGESTED** | `arera-540-2021` → `CCI_ARERA_540_2021_Extract.md` |
| CEI EN 61557-12:2018 | `METROLOGY` | Performance measuring / monitoring claims | P1 | **INGESTED** | `ccli-61557-12-extract` + licensed PDF in drop |
| IEC 62443-3-2 | `CYBER-IACS` | Zones & conduits (architecture) | P0 | **INGESTED** | `ccli-62443-zones` + PDF `IEC 62443-3-2-2020.pdf` (drop) |
| IEC 62443-3-3 | `CYBER-IACS` | System security requirements | P0 | **INGESTED** | `ccli-62443-3-3-extract` + PDF `IEC 62443-3-3 2013.pdf` (drop) |
| IEC 62443-4-1 | `CYBER-IACS` | Secure development lifecycle | P0 | **INGESTED** | `ccli-62443-4-1-extract` + PDF `IEC 62443-4-1 2018.pdf` (drop) |
| IEC 62443-4-2 | `CYBER-IACS` | Technical security requirements for components | P0 | **INGESTED** | `ccli-62443-4-2-extract` + PDF `IEC 62443-4-2-2019.pdf` (drop) |
| FIPS 140-2 (L3 TPM claim) | `CYBER-CRYPTO` | TPM 2.0 / crypto boundary | P1 | **PART** | Module **policy** PDF in drop (`FIPS 140-2 Security Policy.pdf`); not full FIPS 140-2 standard |
| IEC 62351-3 | `CYBER-PROTO` | TLS for 101/104, DNP3 | P0 | **INGESTED** | `ccli-62351-3-extract` + PDF `EN IEC 62351-3-2023.pdf` (drop) |
| IEC 62351-4 | `CYBER-PROTO` | IEC 61850 security | P0 | **INGESTED** | `ccli-62351-4-extract` + PDF `IEC 62351-4 2020.pdf` (drop, ~485 MB) |
| IEC 62351-5 | `CYBER-PROTO` | 101/104 & DNP3 security | P0 | **INGESTED** | `ccli-62351-5-extract` + PDF in drop |
| IEC 62351-6 | `CYBER-PROTO` | IEC 61850 GOOSE/SV security | P0 | **INGESTED** | `ccli-62351-6-extract` + PDF `IEC 62351-6-2020.pdf` (drop) |
| IEC 62351-7 | `CYBER-PROTO` | SNMP security | P1 | **INGESTED** | `ccli-62351-7-extract` + PDF in drop |
| IEC 62351-8 | `CYBER-PROTO` | RBAC for power protocols | P1 | **PART** | `ccli-62351-8-extract` + PDF `IEC 62351-8-2020.pdf` (drop) |
| IEC 62351-9 | `CYBER-PROTO` | Key management | P0 | **INGESTED** | `ccli-62351-9-extract` — **PDF not in drop** (procure) |
| IEC 62351-14 | `CYBER-PROTO` | Syslog | P1 | **INGESTED** | `ccli-62351-14-extract` — **PDF not in drop** (procure) |
| Regulation (EU) 2024/2847 CRA | `CYBER-EU` | Products with digital elements (clone path) | P1 | **INGESTED** | `eu-2024-2847-cra` |
| Directive (EU) 2018/2001 RED II | `ENERGY-EU` | Renewables policy context | P2 | **INGESTED** | `eu-2018-2001-red` |
| IEC 60870-5-101 | `PROTO-FIELD` | Serial telecontrol | P1 | **PDF ONLY** | `IEC 60870-5-101-2003.pdf` (drop); extract optional |
| IEC 60870-5-103 | `PROTO-FIELD` | Protection equipment serial | P2 | **PDF ONLY** | `IEC 60870-5-103-1997.pdf` (drop); out of CCLI P0 |
| IEC 60870-5-104 | `PROTO-FIELD` | Ethernet telecontrol + arming | P1 | **INGESTED** | `ccli-60870-5-104-extract` + PDF in drop |
| IEC 61850-5 | `PROTO-FIELD` | Performance classes / message types | P1 | **INGESTED** | `ccli-61850-5-extract` + PDF `IEC 61850-5.pdf` (drop) |
| IEC 61850-6 (SCL) | `PROTO-FIELD` | CID/SCL grammar | P0 | **INGESTED** | `ccli-61850-6-extract` + PDF `IEC 61850-6.pdf`; capture batches **A–K COMPLETE** |
| IEC 61850-7-2 (ACSI/MMS) | `PROTO-FIELD` | MMS server services | P0 | **INGESTED** | `ccli-61850-7-2-extract` + PDF in drop |
| IEC 61850-7-3 (CDC) | `PROTO-FIELD` | Common data classes (MV, APC, …) | P0 | **INGESTED** | `ccli-61850-7-3-extract` + OCR + PDF `IEC 61850-7-3-2020.pdf` (2026-09-30) |
| IEC 61850-7-4 (LN/DO) | `PROTO-FIELD` | Compatible LN classes + DO names | P0 | **INGESTED** | `ccli-61850-7-4-extract` + OCR + PDF `IEC 61850-7-4-2010.pdf` (2026-09-30) |
| IEC 61850-8-1 | `PROTO-FIELD` | MMS + GOOSE mapping | P0 | **PART** | `ccli-61850-8-1-extract` + **full OCR** — **2004 Ed.1** PDF; target **2011+AMD2018** |
| IEC 61850-8-2 | `PROTO-FIELD` | ACSI → **XMPP** (not MMS) | P2 | **PART** | `ccli-61850-8-2-extract` rev **1.17** — screenshot body through **p. 222**; **W1d/W8** pending; PDF optional |
| IEC 61850 (core + GOOSE) | `PROTO-FIELD` | Server/Client/GOOSE | P0 | **PART** | Allegato T + TR 57-126 + 61850-5/6/7-2/7-3/7-4 + **8-1 (2004 OCR)**; **2011 8-1** edition upgrade open |
| IEC 61158-5-15 / Modbus | `PROTO-FIELD` | RTU + TCP fieldbus ref | P1 | **PDF ONLY** | `IEC 61158-5-15-2010.pdf` (drop); Modbus.org optional |
| IEEE 1815 / DNP3 | `PROTO-FIELD` | DNP3 (+ 62351-3/5) | P2 | **PDF ONLY** | `IEEE Std 1815-2012.pdf` (drop); **out of CCLI scope** |
| OPC UA (IEC 62541) | `PROTO-FIELD` | OPC-UA Client | P2 | **PDF ONLY** | `BS EN IEC 62541-100-2026.pdf` (drop); out of scope |
| IEEE C37.118 | `PROTO-FIELD` | Synchrophasor client | P2 | **PDF ONLY** | `IEEE Std C37.118-2005.pdf` (drop); out of scope |
| IEC 62056-21 DLMS/COSEM | `PROTO-FIELD` | Metering serial/optical profile | P2 | **PDF ONLY** | `IEC62056-21.pdf` (drop); out of scope |
| S7-Comm | `PROTO-FIELD` | Siemens PLC link | P3 | **PART** | Whitepaper `S7Comm Critical Infrastructure.pdf` (drop); not normative |
| IEC 61131-3 | `APP-PLC` | Ladder tasks | P2 | **BLOCKED** | `IEC 61131-3-2025.pdf` in drop **0 B** — re-download |
| IEEE 1588 (power profile) | `TIME` | PTP client/server | P2 | **PDF ONLY** | `IEEE 1588.pdf` + `IEC IEEE 61850-9-3.pdf` (drop); extract optional |
| GNSS timing practice | `TIME` | GPS/GLONASS module | P3 | notes only | `ccli-cci-module-regs-class` (this doc) |

---

## 4. Feature → regulation map (for RAG queries)  
## 4. Mappa funzione → normativa (per query RAG)

| CCI feature cluster (EN / IT) | Primary classes | Preferred `source_id`s (available now) | Preferred when ingested |
|-------------------------------|-----------------|----------------------------------------|-------------------------|
| ETH_A DSO / ETH_B OA (Operatore Abilitato) / plant ports (Elementi di Impianto) | `GRID-IT` | `cei-0-16-allegato-o`, `cei-0-16-allegato-t`, `cei-0-16-cci-manual-ailux` | `arera-540-2021` |
| IEC 61850 / GOOSE | `GRID-IT`, `PROTO-FIELD`, `CYBER-PROTO` | `cei-0-16-allegato-t`, `cei-tr-57-126`, `ccli-61850-6-extract`, `ccli-61850-7-2-extract`, `ccli-61850-7-3-extract`, `ccli-61850-7-4-extract`, `ccli-61850-8-1-extract`, `ccli-61850-8-1-ocr-corpus` | `iec-62351-4`, `iec-62351-6` |
| IEC 104 arming multicast/unicast | `PROTO-FIELD`, `CYBER-PROTO` | `cei-0-16-cci-manual-ailux` | `iec-60870-5-104`, `iec-62351-3`, `iec-62351-5` |
| Modbus RTU/TCP | `PROTO-FIELD` | `cei-0-16-cci-manual-ailux` | `iec-61158-15-modbus` |
| Secure Boot / HAB / CAAM / TPM / FIPS | `CYBER-CRYPTO`, `CYBER-IACS` | this class doc + roadmap | `fips-140-2`, `iec-62443-4-1`, `iec-62443-4-2` |
| Anti-tamper + RBAC + firewall | `CYBER-IACS`, `CYBER-PROTO` | this class doc | `iec-62443-4-2`, `iec-62351-8` |
| SNMP / Syslog secure | `CYBER-PROTO` | this class doc | `iec-62351-7`, `iec-62351-14` |
| DI/DO / GPS / LTE / OLED / microSD | product HW (manual) | `cei-0-16-cci-manual-ailux` | — |
| Metrology / monitoring accuracy | `METROLOGY` | — | `ccli-61557-12-extract` (PART); `cei-en-61557-12-2018` (target) |
| Clone product CE cyber | `CYBER-EU` | `eu-2024-2847-cra` | + 62443 / 62351 corpus |
| Renewables policy context | `ENERGY-EU` | `eu-2018-2001-red` | — |

---

## 5. RAG routing packs (copy into chat / query filters)  
## 5. Pacchetti di routing RAG (da usare in chat / filtri query)

### Pack A — Italian CCI mandate / Mandato CCI italiano (available now)

```
cei-0-16-consolidata-2025-12, cei-0-16-allegato-o, cei-0-16-allegato-t, cei-tr-57-126, ccli-cci-module-regs-class
```

### Pack B — Cyber for CCI clone (partial now)

```
ccli-cci-module-regs-class, eu-2024-2847-cra, ccli-62443-zones, ccli-62443-3-3-extract, ccli-62443-4-1-extract, ccli-62443-4-2-extract, ccli-62351-3-extract, ccli-62351-4-extract, ccli-62351-6-extract, ccli-62351-9-extract, ccli-62351-14-extract, ccli-62351-8-extract
```

*Add for implementation depth:* `ccli-62351-5-extract`, `ccli-60870-5-104-extract`, `ccli-61850-7-2-extract`, `ccli-62351-7-extract` (PDFs already in drop).

### Pack C — Protocols (partial now)

```
ccli-cci-module-regs-class, cei-0-16-allegato-t, cei-tr-57-126, ccli-61850-6-extract, ccli-61850-7-2-extract, ccli-61850-7-3-extract, ccli-61850-7-4-extract, ccli-61850-8-1-extract, ccli-61850-8-1-ocr-corpus, ccli-61850-5-extract, ccli-60870-5-104-extract, ccli-62351-3-extract, ccli-62351-4-extract, ccli-62351-5-extract, ccli-62351-6-extract, ccli-62351-1-extract
```

### Pack D — Market + metrology / Mercato + metrologia

```
arera-540-2021, cei-0-16-allegato-o, ccli-61557-12-extract, cei-en-61557-12-2018
```

---

## 6. To do (regulation PDFs) / Backlog acquisizione (P0 first)

**Closed by standards drop (PDF on disk via junction):** 62443-3-2/3-3/4-1/4-2; 62351-3/4/6/8; 61850-5/6/7-2/7-3/7-4/**8-1 (2004)**; 60870-101/103/104; CRA; most out-of-scope protocol PDFs.

**Still open (P0):**

1. **IEC 61850-8-1:2011 Ed.2 (+ AMD1:2018)** — upgrade from **2004 Ed.1** on disk for conformance evidence  
2. **IEC 62351-9** and **62351-14** — engineering extracts **HAVE**; licensed PDFs **not in drop**  
3. ~~**IEC 61850-7-3 / 7-4**~~ — **INGESTED** 2026-09-30 (`ccli-61850-7-3-extract`, `ccli-61850-7-4-extract` + OCR)  
4. **Re-download:** `IEC 61131-3-2025.pdf` only (**61557-12** ✓ in drop)  
5. ~~Run capture plans → extracts~~ **DONE** (61850-7-2, 104, 62351-5/7, 61850-6 A–K, **61850-8-1 OCR**, 61557 batch C) — re-run **RAG ingest** after doc sync  
6. **ARERA 385/2025/R/EEL** full text  
7. **FIPS 140-2** full standard (drop has module security policy only)  
8. **61850-8-2** (P2 XMPP): **W1d** pp. 27–29, **W8** pp. 223+ if corpus completion desired  

Folder convention under Telematry:

```
documents/regulations/
  cei-0-16/…                    # already present
  eu/…                          # RED + CRA present
  arera/arera-540-2021/
  cei-en/cei-en-61557-12-2018/
  iec-62443/iec-62443-4-1/
  iec-62443/iec-62443-4-2/
  iec-62351/iec-62351-3/ … -6/ (and optional -7/-8/-9/-14)
  fips/fips-140-2/
  protocols/iec-60870-5-104/ …
```

Target `source_id` = folder leaf name (see §3).

---

## 7. Notes / caveats / Note

- **IEC 61158-15** is the fieldbus family reference often used commercially for Modbus; confirm exact Modbus.org / IEC edition when purchasing standards.  
- **S7-Comm** is proprietary — no public IEC PDF expected; keep as protocol capability only.  
- **FIPS 140-2 Level 3** is a *crypto module* claim (typically TPM); do not treat as whole-device FIPS certification unless a certificate number is provided.  
- **IEC 62443-4-1/2 “conforme” / “compliant with”** on a competitor datasheet is a claim — clone path needs evidence (SDL artifacts + component requirements) before CE/CRA arguments.  
- This classification document is **not** a substitute for licensed standard PDFs; it routes RAG until those PDFs are ingested. / Questo documento **non** sostituisce i PDF delle norme: serve al routing RAG finché non sono ingeriti.
