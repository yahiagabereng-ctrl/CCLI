# IEC 61850-7-3 — *Basic communication structure — Common data classes*

**RAG source_id:** `ccli-61850-7-3-ocr-corpus`  
**Purpose:** Verbatim OCR of all PDF file pages for search/RAG; verify CDC/LN tables against licensed PDF.  
**OCR path:** `Architecture/_extracted_reg_analysis/_pdf_ocr/61850-7-3/`  
**Licensed PDF:** `07-protocols/regulations/standards-drop-20260918/IEC 61850-7-3-2020.pdf`  

---



## File page 001

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
Cmprint |< 150 > @Q Q_ View A mark Y Annotations Y Search full text... Q)
IEC 61850-7-3
. Edition 2.1 2020-02
Bie
Communication networks and systems for power utility automation —
Part 7-3: Basic communication structure - Common data classes
} 4
/ 4 f
z f tH
g W/ i \
Fy ae
3 | aw
2 as
3 ° Tas
Aa
https://www.doc88.com/p-74754903218494, html 41151
```


## File page 002

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark ¥ Annotations v Q
THIS PUBLICATION IS COPYRIGHT PROTECTED
Copyright © 2020 IEC, Geneva, Switzerland

Al rights reserved. Unless otherwise specified, no part of this publication may be reproduced or utilized in any form

‘or by any means, electronic or mechanical, including photocopying and microfilm, without permission in writing from

either IEC or IEC's member National Committee in the country of the requester. If you have any questions about IEC

‘copyright or have an enquiry about obtaining additonal rights to this publication, please contact the address below or

your local IEC member National Committee for further information.

IEC Central Office Tel: +41 22 91902 11

3, rue de Varembé info@iec.ch

CH-1211 Geneva 20 www iec.ch

Switzerland
‘About the IEC
The International Electrotechnical Commission (IEC) is the leading global organization that prepares and publishes
International Standards for all electrical, electronic and related technologies.
About IEC publications
The technical content of IEC publications is kept under constant review by the IEC. Please make sure that you have the
latest edition, a corrigendum or an amendment might have been published.
IEC publications search - webstore iec.ch/advsearchform —_Electropedia - www.electropedia.org
The advanced search enables to find IEC publications by a The world’s leading online dictionary on electrotechnology,
variety of criteria (reference number, text. technical containing more than 22 000 terminological entries in English
comrmittee,...) It also gives information on projects, replaced —_and French, with equivalent terms in 16 additional languages.
and withdrawn publications ‘Also known as the Intemational Electrotechnical Vocabulary

t i
IEC Just Published - webstore.iec.chjustpublished (EV online
Stay up to date on all new IEC publications. Just Published IEC Glossary - std.ec.chiglossary
details all new publications released. Available online and 67 000 electrotechnical terminology entries in English and
‘once a month by email. French extracted from the Terms and definitions clause of
IEC publications issued between 2002 and 2015. Some
IEC Customer Service Centre - webstore.iec.chiesc entries have been collected from earlier publications of IEC
It you wish to give us your feedback on this publication or TC 37, 77, 86 and CISPR.
need further assistance, please contact the Customer Service
Centre: sales@iec.ch.
a
nw
https://www.doc88.com/p-74754903218494.html 2/151
```


## File page 003

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
1
Edition 2.1 2020-02
e colour
inside
Communication networks and systems for power utility automation —
Part 7-3: Basic communication structure - Common data classes
INTERNATIONAL
ELECTROTECHNICAL
COMMISSION
Ics 33.200 ISBN 978-2-8922-7868-0
Warning! Make sure that you obtained this publication from an authorized distributor.
a
cy
8
a
https:/www.doc88.com/p-74754903218494.htm! 3/151
```


## File page 004

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark ¥ Annotations v Q
-2- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
CONTENTS
1 SCOPE .ssssonsssssesnesnsntsnnneansnnnevnnannnensannnnnantnnesantantennsnneennantsneneeantnaneeneasnsennennneennassnnse V4
1.2 Namespace name and Version..sssssussessesusetnesnetenesinsennsesnseeinesneeinsnneennsenneee 14
1.3 Code Component distribution ..........csssssseseessssnneseesesnsnneessesssnnnnereesesnsnnnssseescennnneressssannanssees 1D,
6 Constructed attribute ClasSS........ssssesssssssesssssssesssssnsesesssnsnnensusnsessanunsesaunseaeeeaeeaee lO
6.2 Configuration of analogue value (ScaledValueConfig)..............-sssssueesssseecssnsessensesesseeeess QO
6.3 Range configuration (RangeContig) ....jss:siesenenenntnsnenenaratnenenenaniatenenaaenenee 2
6.4 Step position with transient indication (ValWithTrans) ...........-....:sssssssessssssesssnsesensneeesen OO
6.5 Pulse configuration (PulseConfig) .....:coosscsseossssssesensseesnssssssnsssantensssnssensennsssnnsses 22
6.6 (UnnR GOFMIIOT (LINN) ...ccsssessscserssececonseesoses vevenves soneesosseccusaseecensecsenes ssnsvecenseesases veveseos sasesensecee SD
6.7 Vector definition (Vector) ...esusesusessennnssnasinnetasennnttinssnaseenaseiseesnesneesnesnneernseeneeees 2B
6.8 Point definition (Poimt) .cseccssscsescvssersnesevarsnsssineiesnnesaesneennnennassanenenetnsaenenreanenenees 24
6.9 Cell (CM) .esesesesssesesnsseassnnessneninssneninessnsninsssanesessoessessssassnsssoastenessnssensesnsssnesses QA
6.10 Calendar time definition (CalendarTime) ...........c0ssssessssssessesseesssseeesrsssneesreseneennaseneensene eS
GB.11 — Amalogue Value...........sosessssoressessorsssessnenvnsonesnsontensasontennnsorsevnssnreesnsenteevasentenvasentenvanentenvanee@O
6.11.2 Analogue value (AnalogueValue) ..............:ssssessssssssssssesssseessseesesseesssseesssneesssneeessnee OO
6.11.3 Analogue value control (AnalogueValUeCtl) .............ssssssessssneeessnnesssnnessnsnesssneersene OO
6.11.4 Analogue float value control (AnalogueValueCtlF) ......cscsuonsuceunnscesnnsetsnenneenenns®?
6.11.5 Analogue integer value control (AnalogueValueCillit) ....ccccsccusnscesensetunenetneens®?
7 Common data class Specifications ............cccsscseessesessennnesecsecssnnessssesssnnnneeessessannueeseescsnnnnereeeeses ll
7A GOMerall .......ssossssesseseesssventesessessnnnnseeeesennnnesseseessnnunsseesennsnseseasessennnneseesessnnuasaseasesnnnnereeeesannanseeeO
722 <<abstract>> Common attributes for primitive CDC (BasePrimitiveCDC)............29
7.23 <<abstract>> Common attributes for composed CDC

(BaseComposedCDC) ....-.cscs:isesneennsntennintinniatinnnntinninasnesnasnnnsennentseeeateen es
7.24 <<abstract>> Common attributes for substitution (SubstitutionCDC)...........030
73.2 Single point status (SPS) ...cccscsescsssossseesseesseennenenenivenseeennsennneniasenenssnseeneeeneSS
7.3.3 Double point status (DPS)...c.cscscsscssscscnesstiscietetntseieietatineietetntneseieneen GA
7.3.4 <<Statistics>> Integer status (INS) ..........csssssssesssseessssneesssnsensnneessnnecennenecesatesssneeses OD
7.3.6 Protection activation information (ACT) ........-.cs:ssse:sssssesssssesessstessneeseeneneessneesssneeses OO
7.3.7 Directional protection indication information (ACD)...
7.3.8 Security violation Counting (SEC) ...........sess:sssessesssesseseneesseeneesneeeneesneeareesteereenneeeres OB

a

cy

8

nw

https://www.doc88.com/p-74754903218494.html 4/151
```


## File page 005

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark ¥ Annotations v Q
IEC 7-34 5 -3-
CLARE RRA Ooty
S17 Visible string status (VSS)....sscsccseetsessetennetneinetneinenatineeneneeiaeetinennernenee Ad
7.312 — Object reference status (ORS)...cscscscsssussssssssusssusssussenssnstsnssuseetnseneesssesese AS
7.3.13 Time value status (TCOS)......ccccsesssssessessessnssnessnsssvnssssnnsonsermsensenmsensenensenseeesnneeee
74 Measurand informattion........sessssscsssssssesessssessesessessesesseseesessesessesseseesessssessessssessssasssssnsensesenses Mh
742 <<abstract,statistics>> Common harmonic measurand information
(HarmonicMeasurandCDC).....-.cscssossssssnsseesssesstsnsisnssisetinesinstinessissesnsteinseesseses 4
7.43 <<Statistics>> Measured ValU@ (MV) .s.ccsssssssusssusessssesnsesnseesssisennneenseneeeens®
744 <<Statistics>> Complex measured value (CMV) suscsssusssssssnseinennsinenaenneenennn 52
7A5 <<Statistics>> Sampled value (SAV) ..cscccsssssessesssesceesssesnsttsseesettnsieseereenreenees 4
746 <<statistics>> Phase to ground/neutral related measured values of a
747 <<statistics>> Phase to phase related measured values of a three-
phase System (DEL).csesssosssecssssssessnsesnsstinsieseninssniesniastinseiasiinseinsseseesseeeesses5B
748 <cStatistics>> Sequence (SEQ) ....ssssssssusesuseesesunssissesaseinassssssesaeesseneeeeeST
749 <cStatistics>> Harmonic value (HMV) .sssc:osssosctssesstsnssnsenseinseennenaenneneeenes 5B
7.410 <«statistics>> Harmonic value for WYE (HWYE)q..cccsccssssssscssscsseetnsesneeeneeeeees 59
7.411 <<statistics>> Harmonic value for DEL (HDEL)..............s:scscscssssssssnsseeneeessseeeeeene 60
75.2 <<abstract>> Control testing (ControlTestingCDC) ............cs:scssessessenerssnereesseee OD
75.3 Controllable single Point (SPC)...........-ssssscssseessssessessesessneecseneeeeenecssseessnneeseneeeessnee OO
75.4 Controllable double Point (DPC) ............sesse:ssnessesssvessssnessnsenessnnsenessnnsenessenssnessnrsere OB
755 <<statistics>> Controllable integer status (INC) ........sscsssssssssssssssmuesessesnnneereeesss OD
756 <<abstract>> Controllable enumerated status (ENC)...........::csssssseesesseeeeeeee TO
757 <<Statistics>> Binary controlled step position information (BSC).....:eeeeennT71
758 <<statistics>> Integer controlled step position information (ISC)..........-sssses 72
75.9 <<statistics>> Controllable analogue process value (APC).......ssssssssssssssssss TS
7.5.10  <cstatistics>> Binary controlled analogue process value (BAC) .....sscseenuTS
7.6 Status Settings ....sscscssnssensenssceenenetneesetneesstnesnstnesastnenastnsiaeeneneeneenetneesetneeneT®
76.2 Single point Setting....ccocssocsesssneesssnsnnesansnnseinennnsninensennnnaennsenasennaeeinsaeseesnneTT
7.63 Integer status Setting .....cscssscsenseneeneentneinetineinetineineineineinnnaeineeetneenetneeeeT®
7.6.4 Ensembl GRID GUI inns cnc ennscnennenscnees sacensennnen ences nccnsssennes nesanens sonenennsncnnene anal
76.5 Ciifect referee UU cesses ses zeccncccensnssecnsens sennecnsnscnsenccssanssesnnsses snsecns recanses sennnens sana
7.66 Time Setting ..scsecsescsnsenssneensineentenetseineisennesctneensetneenetneenetneeateneeseenesseenes 8B
76.7 CANTON OY SONG 22.csccesesancessensses senseconsecennes ceennens sanessee snsesenevesenssesensces eesecenrecensee seeancos sane
7.6.8 Visible string Setting......sscscsssotesssssnsntatusinintntineieiatasueianatntsieneeeaee Gl
7.7 Amalogue Settings .......ssssssnusseninetneenineesetneenetneneineasinsaeinenatinenetseesernenne
7.7.2 Analogue Setting...c.cscscsssssunscnetntiseieistntnsienntatistseienattienetatsienenennan Gh
774 Curve Shape Setting .........csess:sssessssssessossvessossnessntsnvassnssnnassnssnensensenensenserensensevessenee TOO.
7.8 Description information. .eosesossscsssovsssesnnnsnsesnneninnennnneinsansnennstnnnennnssenssnnaeasernaneseee I 4
78.2 Device name plate (DPL) ..........cccccssecsssesssssessssenessmssnessnsentesraseneesseneesnseneesees 105
78.3 Logical node name plate (LPL) .........:sssssessssssessesseressesseresnessneeveseneesrssentenrsseneesees 106
784 Curve shape description (CSD).-jcssc:ocsseucssesctnnestuannstanneinnnennanenneneenn TOT
785 Visible string description (VSD) -...ssc:ocssesstseuetseetistnetseinetneeetneeeeneeeeee I 08
a
cy
8
nw
https://www.doc88.com/p-74754903218494.html 5/151
```


## File page 006

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark ¥ Annotations v Q

7.9 Common data class specifications tot service {@kir&l850-7-3:2010+AMD1.:2020... C808
7.91 GONE sosnsstsnsntntntsennnntntnnnnnnnntinnsennnnntnenanntanenenaeannnennn ES, 2088
7.9.2 Common service tracking (CST) .....ssssissessenssessenusenaseinsesassnesesseneeeaeeneeed 10
793 Buffered report tracking service (BTS) ..vsssucnstsnssssinanneinennnnaennnaeennetne 10
7.9.4 Unbuffered report tracking Service (UTS) .......-.-cscsssssssscssssnneeesseessnnnessecsessnnerreeeee 112
7.95 Log control block tracking service (LTS) ....cssscssscsessssssesssssnseenseeseenseeseeneeed 1B
7.9.7 MSVGB tracking service (MTS).........cssssessssessmssersssmsseresnmseneeevensneesraseneenrssencenens 14
798 USVCB tracking service (NTS)..cc:scsscsonsstsnusnteisneineinnineinainnentsnneneieenetnee IS
79.9 SGCB tracking service (STS) .scccsvsseusuestuensetenneenennetnennennenaeenenarneenernee 116
7.9.10 <<abstract>> Control service tracking (CTS) .........0:-csesssssssseesssseesssnnecsenneeessneees 117,
© = Erngmmpemincl Cetten ATID GI iia ine nets css ces eee eee eens TID
8.1 GOTO EA oassescscssassecsescssssnssaccasossnsensosesscennusasecsnsossnsessoesesoussssssoassssssessessssnsssssecasessnsssseasssossaasee 1 1G,
8.2 Angle reference (AngleReferenceKind enumeration) ............scssssesesssessneressssnnneeed 1B,
8.3 Control model (CtIModelKind enumeration) ........scsssssssessssssesesseusessnssesnesseesnseeesee dD
84 Curve characteristic (CurveCharKind enumeration) ........s.svssssscsssssssssesessessnneereeesssnsaneeed 19)
8.5 Fault direction (FaultDirectionKind enumeration)..........-.-.---sssscsssscssssssesssssneeseeneeseeeee 121
8.6 Harmonic value reference (HvReferenceKind enumeration)................:ssssseseeeeee 21
8.7 Month (MonthKind enumeration) ......c:socssssssssssssesssessesssssnstssssnsssnsssnssensetnssenees IQ
8.8 Unit multiplier (MultiplierKind enumeration) ......1ssseseosssesssssssesssssaesnsseneseneesnsesneesT22
89 Occurrence (OccurrenceKind enumeration)...........00sss0resssseresesseeenressnrenrasenecnrsneneenees 12D
8.10 Output signal (OutputSignalKind enumeration) ....cccvssssenenenennnnneennnnennseneieeeeniee TB
8.11 — Period (PeriodKind enumeration) ............cvecsessessssssssesesssesescssnssesssnsscensesessecesssseesssseeesseeee TES.
8.12 Phase angle reference (PhaseAngleReferenceKind enumeration) ..............-scss0s 124
8.13 Phase fault direction (PhaseFaultDirectionKind enumeration) ............0c0eseeeeeee 124
8.14 Phase reference (PhaseReferenceKind enumeration) ........+o+vessssssseenseneenerene 12S
8.15 Range (RangeKind enumeration) ....csscscsscsssnsssnnnnsssnnneneeinannesnenntsnsanieseannes 25
8.16 — SI unit (SIUnitKind enumeration)..........srvsecsesessereseeeersnnnneesessessannaresecsesnnnneeesesssnnnnessessesses TOD
8.17 Select-before-operate class (SboClassKind enumeration) ....as:iccsscscsseserseieteeein 128
B.18 Sequence (SequenceKind enumeration) ......ccsscssssssssesssssusssssusessnssessuesseeenseeese 2B
B.19 Severity (SeverityKind enumeration) ....ccsssesesesseneseeinsenusseiaseinsesassnssseassenseeaeeneee 2B
8.20 Week day (WeekdayKind enumeration)......scsssecssesovensssesssessesnnenastnsnttnsaeneneneneneee 429
Annex A (normative) Value range for units and multiplier ...........cssosessecscsseneeeesesssneneseesessnneereeeee 13O
Annex B (informative) Functional constraints (FOKind) ...........s-::sssssssesssssnessenssnessensenessersenesseren TOT
Annex C (normative) Tracking of Configuration reviSiOnS .............scsssesssseesssueesesseecssnnessenneseesseees 13D
Annex D (normative) SCL enumerations...........ssssesscssussssssesenssessssesieunansssseteenunsseeseeesees 138
Annex E (informative) Conditions for element presence ........-..-scssosesscssssneeeseessnnnssessessnnneeeeeee 17
Annex F (normative) Compatibility of the different revisions of the standard ............sscssssse 139
FA ETI sss sssnssenesesovumssaccnsensnseasecessessmunasacsnsnsunsen essssmusessssonsosssnessessssuuenssasesessnomnses sesessesses TED
F.2 List of the modifications to consider for backward / forward compatibility ..........:000.139
F3 List of modifications requiring specific treatment..........-.o-cveressoeesssnsesssseessseneessnneesennee 42
F4 Special compatibility rules and iSCUSSION ............-.o1cscsseeessnsecssneesssneessnnessssnnesssnnesssseeees 143,
F.4d Use case 3a — Dead band, db and AbREf..........cscssesesssscsessnneeeessesssnnseesesscesnneereeees 143
F42 Use Case Sa — MAXPIB......cccssssesssssessseessssscccssssecsnsecsessscssssccsssssccessessssscsssssecsesseccesssces 144

a

cy

8

nw

https://www.doc88.com/p-74754903218494.html 6/151
```


## File page 007

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark ¥ Annotations v Q
IEC_61850-7-3:2010+AMD1:2020 CSV - 5-
© IEC 2020
F47 Uwe CASE £14 — NTS. eccessesssscsssseesosssssssssnsseconsossnscssssesonsassasossssssssscssssonsesssscassesessaseases 144

Bibliography ....o:ossssssssssnnsneinnsnenesneintinnintinaninetnansniasniasinsnnsinenstnnenesianetanaenenaenseeneen 14
Figure 0 — Fhamngye Ccweligguretnts secs ccnsenscscnnensonsnensen saccnses sennscns snenenscssansscsannsenenssnnen wecnsns snsaneea sanssensssssa Stl
Figure 2 — Configuration of command output pulse.............-..:-ssosesssseesssnersssnnesssnsesssnneesssneessnnensesneee QO
Figure 3 — Cell definition .........cssssssseseseessssnneesesenssnnnnsnsessennsnnnnesessesnnnnnssesesesnanessessessnnnneereceessnnnnssentesnnnen 4
Figure 4 — Interpretation of calendar time Settings ...........ssossssssssssessssnresssneeeeennseesnueessnversenneeessneee 2
Figure 5 — Class diagram CommonDataClasses::CommonDataClasse ............:css:-ssssesssseesesneeeesnnee 2B
Figure 6 — Class diagram CoreAbstractCDCs::CoreAbstractCDCs...........ssesesscssssssssessecsesnneesesseensnneee 2D
Figure 7 — Concept of Substitution ....ccscvsssscsnvessseensnasamenninsntisntiastsettneaneninentnennaasineenaseneOt
Figure 8 — Class diagram CDCStatusinfo::CDCStatusimto.........-..ssssssssersssnnesssnsessnsnesensneersnnentesneees OD
Figure 9 — One-dimensional histogram ............:ssssssssssssesscssseesssseesssnessssncesssscesssssesssncesssncerssseesesseeeeMl]
Figure 10 — Two-dimensional histograr..........ssssssssessessssneresessessnnnnessessessanessesessssnnneseeseessananessessesssns
Figure 11 — Class diagram CDCAnaloguelnto::CDCAnaloguelinto-t ..........ssssssseseeeenennnnesessesnsnnree 4S
Figure 12 — Class diagram CDCAnaloguelnfo::;CDCAnaloguelnto-2 ...........-sssssssessssseeeesnneesesneee GB
Figure 13 — Array indexing (‘har'/*Har’) based OM 'NUMCYC'.........sccccsssnessecssesnneecsesssnnnnseessesseneeh]
Figure 14 — Deadbanded Value -..cvcccsstesssssesseninsietininstisenetieeneineinenstinenetnennesneeneeneeeeenee
Figure 15 — Zero deadband ...suscsseemssnseesseinsesnsesnaeensenastinseineennsennsesnesiaeeiaesiseenasennseneeees@
Figure 16 — Relation between phase Values.............:csssssssmesssesssnessssnersssnnesssnsesssnnetsssneessnnestesseess OD
Figure 17 — Class diagram CDCControl::CDCControl-t ........ssssssssssccsesssnesssecsssnnneeeecesssnnnnessessessens OD
Figure 18 — Class diagram CDCControl::CDCControl-2.........sssssssesessssssneesecsssnnneeessesssnnnneseessesesns OD
Figure 19 — Attributes for command testing ......s:sssssnussmnseeennennensenstisnneianenenaeneenennn 6S
Figure 20 — Class diagram SPG::SPG........sccsscssessssssessssneeeessueessnsessoneesssnuesssnneessnsessssnesssneesssneneesneeee 2D
Figure 21 — Class diagram ING::ING ..........cccssssssssssecscsssssesesessessnsnseesecsessansssssesessnnscesessssnnnessessesssnee TD
Figure 22 — Class diagram ENG::ENG....cscssmsnmnenenemennenenennmneneienannnenenenaninenenenannneneB2
Figure 23 — Class diagram ORG::ORG ....sscsssssssstssnsstnsnienneeiennenntenenntiseietannseanaseeeseene BA
Figure 24 — Switching to test object reference.......coscssssesssssneesnsssnsssnsssseissenseennsesnseseeeees 85
Figure 25 — Class diagram TSG:TSG .........:ccssssssssesscssssnnsesssecssnnnnssesceesssnensesecsssnnnsesssesssnnanesecsseessns OD
Figure 26 — Class diagram CUG::CUG ..s.cssmsmsmsnnnnenenennenenenninenenenanameneienannenenenannnens 89
Figure 27 — Class diagram VSG:VSG ..ssssssmsstsnsennintatinenenntnensienainnneeieianeeneieenneneS2
Figure 28 — Class diagram ASG::ASG........:ccsssssssssseesesneeeessueessnsesssneesssnersssnneessnnesesnntessnrecsnneceesneees 4
Figure 29 — Class diagram CURVE: CURVE .....cscscscsssesneniennnsensiennintneieietannnnensneennee ST
Figure 30 — Class diagram CSG::CSG wesmsessmemnmneneneneenenenannmnenenenananenenenaneneneneensees 100
Figure 32 — Three-dimensional SUace ..............cscssssiesesseeecsseesssueessnuesssnnecsssnessssnsessnesessnesssneeeessee 1OT
Figure 33 — Class diagram CDCDescription:;CDCDescription .....cccscucteuseseueietntntnenenenenees 405
Figure 34 — Class diagram CDCServiceTracking::CDCService Tracking ...csccssvscssstensnennneneeeen 409
Table 1 — Attributes of ScaledValueConfig ............-ssssesscsesessesssneesssneessnueesssnesssnssessneesssneesesneessaeesei ld
Table 2 = Attributes of RangeConfig.....c.cssvscsssuneneensneenenntinenntineenntnetnatnennatneeneenenneeeene®d

a

cy

8

nw

https://www.doc88.com/p-74754903218494.html 751
```


## File page 008

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark ¥ Annotations v Q

-6- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
Table 3 — Attributes Of ValWithTrans ...........scssssssssessesssnessssesssnnnecsecsesssnneneseesesnnnneessessannasssessesnannereeesses 22
Table 4 — Attributes of PulseContig .........sssssssssesessssssnereseesnnsnnneeeseensnnsneseessennnnareseessnnnnnssesessssneneesesss@O
Table 5 — Attributes Of Uriit........ssessssssssssssssessessssesseseseeessessesesscesesesenenenesonenenescnenenasceenenesesseeneseseneneses 2D
Table 6 — Attributes Of VECtOl........ssssssssssssssssssssesssssssesssssnsssssssssnvessansesestsnsssesssuesesessnesesessaseeseseeeeeseseeee ld
Table 7 — Attributes Of Point ....-.--vsosossevsssssssssssessnssssessnsassesensnssevestessevesssssssesestssserestessereseesessesseseeresseee lt
Table 8 — Attributes Of Celll..........-sccssssesessesesssnnnneereeesnsnnensseesennnnessesenssnnnnsseessensnnaneseessnnnnnassstensennnneseesse@O
Table 9 — Attributes of CalendarTime........ssssssssssssssssssssssssssssnesnuveuusenuneansna 2D
Tele 10 — Adbtpuben OF Armbar Ose acs access nes ennnenneeenensenennens nsnsenonnsnsonses nanos nenenses nnenenssscenenseneen NGS
Table 11 — Attributes of AnalogueValUeCtt............cscssssessssessssnmeesessessanneesessessnnneressessannnesessesnnnnereessses Oe
Table 12 — Attributes of AnalogueValUueCtlF .........sssssssessssessssnnnesecsssssnnessseesssnnnneresesssnnnuessensssnnnnneesess Ol
Table 14 — Attributes of BasePrimitiVeCDC............sssesesssssesesssssesesssesenssseseesaseeeereese IO
Table 15 — Attributes of BaseComposedCDC........-.scsssssssessssnmeesersssssnnnesessssnnnneessesssnnnarssessesnnneeressssss OO
Table 16 — Attributes of SubstitUtiONCDC............scssssssnessssessnnnnneseesennsnnnneseessnnnnnereseensnnnueseeseannnnereessse Ge
Table 17 — Attributes Of SPS .......sesccssssessnssessssssessneessssseconteccnssssecansessenescssnnsessnssecansssesnneessnsessonsecensescess Mh
Table 18 — Attributes Of DPS.......a..nsensesensessessssesssssssssssssssesessssssssesessessssessssssesscseneasecsnnnaneccnnneneseeneenesee
Table 20 — Attributes Of ENS.........cssssssssssesssssnsnneereessnsnnensssesnssnnnensseenssnssneseessensnneneesesansannasestensessnvesseses OO
Table 21 — Attributes Of ACT ......snsccsssccsssssscssssssssncessssceconsecenssssessscessnseccansecosseseseneescossecesssscssassecenses sess
Table 22 — Attributes Of ACD ..n.ncensensessnssssnsssssssesssssssnsessessnsestessnsestessnsestsssssesssssssesessaseseesseesssecseeeeseesesee
Table 25 — Attributes Of HST .....ssesccssssesssssscossssessnecssnsseconseccessssecenseceanescssnssesonssscanssscanscessssseconsecensssseee
Table 26 — Attributes Of VES ....-..-sessessessssesssssssesesssssesessessesestessenessessneessssetenseteseeneatesteeesseaeeeesseseesesse
Table 27 — Attributes Of ORS.........ccssssesesesssssnsmneecssssnnnessssessnsnnnesessesssnnnesesssssnneeresesssnnnaessessesnsnenressssns de
Table 29 — Attributes of HarmonicMeasurandCDC ..............cesssseeesssneessenessesneeessnessenneeesnneesssneesssneeeesee
Talble 33 — Attributes Of WIE ......2..cscsccsssssssssssssssscesssscccsnseconses essscssensecsansscessnsessnsescossecesssscssassecensss sess SO
Table 35 — Attributes Of SEQ........cssssssssssesssssnsnnesesnesssnnmnsssessssnnnenesesnsonsnvesensssssnnoesssssnnannonestessessavessssss OB.
Table 37 — Attributes Of HWYE............-ccsssscossssesssssssnssessnseccansssssnssessnsecsesnecesssseccnsessessecessnssesasseceasesessee OO
Table 39 — Attributes of ControlTestingCDC ........-cssssssssssesssssnneresserssnnnueseessessnneessessnsannanestessensnvereses OO
Table 43 — Attributes Of ENC.........ccssssesessssssssssesessnsssnsesssnssnssnneeesnsnssnsasesessnsssnveresesssnsssssseasessnnsereesnses 10

a

cy

8

nw

https://www.doc88.com/p-74754903218494.html 8/151
```


## File page 009

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark ¥ Annotations v Q

IEC_61850-7-3:2010+AMD1:2020 CSV -7-
© IEC 2020
Table 46 — At@iDutes OF APC.....c.sscsscsessescesorssseesosssnesnsveeveccesossnsesssesesansscsesossnsnseescesovsusesssccssorsanessecseees Ft
Table 47 — Attributes Of BAC.........csssssssssssssssnnnneersessnnnnereseesnnnnnnensseensnnnussseesssnnnnasesesssnnnnnasesessssnsnessessss 2
Table 48 — Attributes Of SPG............esssosssesssssesssseesesssssesesesecenesesennsenencnsnenescnnnenancnenenescncnenesescseneeee TT,
Table 49 — Attributes Of SPG_SP.........sssssssssssssssssssessessssessesesscesssesensesseseeceeneceneneneccceneseseneneneseseneeeeee 1B.
Table 50 — Attributes Of SPG_SG .......cssssessesssesesessessereseeseerereeerereseeereresecererceecerereeceerereseneneesecsereesees 1B.
Table 51 — Attributes Of SPG_SE.......sssssssssssesecsssnnnneressessnsnnnereeeenssnnarsseesesnnnnereseessnnnnnasessenssnnneeseseee
Table 52 — Attributes Of ING...........ssssssssssessssssesssssssesstssscesesesecenerecsnsnnnescnenenascnnnenanceenenesenseeneseseseneeee OO
Table 53 — Attributes Of ING_SP oo... eesseessesseesneessessneessessneesneesneesneenessesanesseenesseeneeseeeeeee BO
Table 55 — Attributes Of ING_SE.......sssssssssssnnsseseesssnsnnesessesansnnnercessnssnsareseesssnnnescecenssnsanessesssssnnesressesss OT
Table 56 — Attributes Of ENG.......-....sssssesssssssssssssssssssssssssssscesssssscenessccnencneccnsnenencnsnenencennenescessenesesesensseeB2,
Table 57 — Attributes Of ENG_SP..........csscssssssssssecsssssosssssesesssssnsesssessannssssensersnssassssesonnassessssessssmssssesses OO
Table 58 — Attributes Of ENG_SG........:ssssssssssseeeceesssnenseseesnnsnnnersseensnnnnresessesnnneneesessannannaesssssssneneesesss OO.
Table 59 — Attributes Of ENG_SE.......sssssssssssseeecessnnnneressesnnnnnneressensnnnnvsseesesnnneeeessessnnannssesessssnenessesss OD
Table 60 — Attributes Of ORG ........-cscsccccsssssssnssccecsesnsueesessesssnnneeecssesssnnasesccssssnneseceesssnaneeessessenneeesessss OO
Table 61 — Attributes Of TSG... -..-..nsessssessesssssssetssscssessssessesesscnnesscsnsnanecennnnnaccnnnanacennnenascnnnenaneeenenenee BT,
Table 62 — Attributes Of TSG_SP.......ssosssssssssmsesecsssnsnneessesnnsnnnereseenssnnarasenssssnneeesenssnnauneestesssneneesssss OO
Table 63 — Attributes Of TSG_SG......ssossssssssnmseeecsesnnnneessesnnnnnneresennsnnsnneseesssnnnenrecesssnnsunseesessssnneresssss OO
Table 64 — Attributes Of TSG_SE ............ssccssssscsssessssecsssssscssesessssesssnessssacecssssessusessesessssasessaseesesseessses OQ
Table 65 — Attributes Of CUG ..........ssssesssssesesseeessssssessesesessessesenenesesensnenececenenenceenesecenenseeneneneneneseeeneees OO.
Table 66 — Attributes Of CUG_SP........sosssssssssmsessesssssnnessssessnsnnneeersesssnnnessesssssnnneeessessnnnanssessesnnnenressssss OO
Table 67 — Attributes Of CUG_SG.......sessssssssssmsseecsnsnnnnessssesansnnneeesssnssnneseseessssnnversesensnnnasssessessnnesseeessss OO
Table 68 — Attributes Of CUG_SE -.......ccccscssssssssessccsssssnuesessesssssnneeecsesssnsnsessessssnnsesecsesssnnueasesscesenneeeeesee DT
Table 72 — Attributes Of VSG_SE..........sssccsssscsssssssseesssnssccsssesssseessesscessnsessasecssssessssasesssseesssneeessaseesss OD
Table 76 — Attributes Of ASG_SE..........ssscssssscsssscssnseesesssccssssesssssessssecsessecessasescaseesssseeessaseessssecsasessssee IO
Table 82 — Attributes Of CSG_SP.......sesssssssssmsessesssssnnessssesssnnnnesessenssnsnesessnssnnneessesssnnnarssentessnnenressss 102

a

cy

8

nw

https://www.doc88.com/p-74754903218494.html 9/151
```


## File page 010

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark ¥ Annotations v Q

-8- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
Table 90 — Attributes Of BTS ........scssssssssesssssnnmnesecsssnnnnsresessannnnneresssnssnnnreseesesnnnesrecessansnneasessesssnenensee DD
Table 91 — Attributes Of UTS .........ccsssssssssssssnnesesessssnnueesssssssnnnusseesssnssnssssssessnsunsseeceessnnssessesssssnnesceeeee 12
Table 98 — Literals of AngleReferenceKind ..........sssssssssessessnsmseseesssssnuensesesssnnnnesessesnsnssssscsssssnneneeeene 119
Table 99 — Literals Of CtIMOdeIKING...........s.ssessssusesssssesssnsscssesessnuessssneccssssecssnsesssnessssseccssnnesssnessessssees 119)
Table 100 — Literals of CurveCharKind............sescssseesscsessessesesssnessssueecssnsesssnessssnassssnecessnnesssnessesseeees 119)
Table 103 — Literals Of MOMthKiNd........ssssssssssssssssssssssssssssesssssssesesssssesesssssesessessesessssaeseseseaseereseeeeeeree TO
Table 105 — Literals of OccurrenceKing .......ssvsssssssssssssssssssesssssesennnnsnvennssnseressnsnssseeeraneerenserere TOD
Table 106 — Literals of OutputSignalKind ............00ssssssssssssssssssenesssnssnenssnsnnenasneeeanenn TD
Table 107 — Literals Of PeriodKind .......ssssssssssssssssssssesssssssssssssssesesssssevessssvessssssessssssesessseeeseseseeeserees LOM
Table 108 — Literals of PhaseAngleReferenceKind ..............csssssssscsessssessssessssnnneeessessasnseesecsessnnnereeees LOE
Table 109 — Literals of PhaseFaultDirectionKind...........sesssssuneeeesenssnvereseessssnnnereseessannarsressesnnneerenees TOM
Table 110 — Literals of PhaseReferenceKind ..........s0sssssssssesssssssesssssesnsssenssneaaeeeeneee 12D
Table 111 — Literals Of RamgeKind ...........scsssssssssssssssseessssescssssasssesssseesssnscessssecssssessssecessasecssssecesssees 12D.
Table 112 — Literals Of SIUMIRKINd .............sscsssssssesessessnsessscessnsnasessesessennesescessesnnesseesessannsssseessssssneseesss 12D
Table 113 — Literals Of SboCIaSSKiN...........sssssesessessnessssesnsnnnneeecsesssnneeseesssnnnvereseessnnnasasessesnnneereeess 1B
Table 114 — Literals of SequenceKind ...........0.sessesssessssssessnsssnessnsssvnssnsssnessseneessnsenensanserensnnsenessnnee 12D,
Teoh: 115 — Likpratin cof Spwepriig igh ences nesses nnnnesnennansencennsenensensnssns snsen tnsssennenens enencansncenena ns 1D
Table B.1 — Functional constraints (FCKiNG) ....svssssesssseeresesseereresecerereserereresenerereseeerererenereresenseeees VOT
Table E.1 — Conditions for presence of elements within & CONTEXt ........cs.csscccsssseesessesnnneeeeeee IST

a

cy

8

nw

https://www.doc88.com/p-74754903218494.html 10/151
```


## File page 011

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark ¥ Annotations v Q
IEC 61850-7-3:2010+AMD1:2020 CSV -9-
© IEC 2020
INTERNATIONAL ELECTROTECHNICAL COMMISSION
COMMUNICATION NETWORKS AND
SYSTEMS FOR POWER UTILITY AUTOMATION —
Part 7-3: Basic communication structure —
Common data classes
FOREWORD

1) The Intemational Electrotechnical Commission (IEC) is a worldwide organization for standardization comprising
all national electrotechnical committees (IEC National Committees). The object of IEC is to promote international
co-operation on all questions conceming standardization in the electrical and electronic fields. To this end and
in adgition to other activities, IEC publishes International Standards, Technical Specifications, Technical Reports,
Publicly Available Specifications (PAS) and Guides (hereafter referred to as “IEC Publication(s)"), Their
preparation is entrusted to technical committees; any IEC National Committee interested in the subject dealt with
may participate in this preparatory work. International, governmental and non-governmental organizations liaising
with the IEC also participate in this preparation. IEC collaborates closely with the Intemational Organization for
Standardization (ISO) in accordance with conditions determined by agreement between the two organizations.

2) The formal decisions or agreements of IEC on technical matters express, as nearly as possible, an intemational
consensus of opinion on the relevant subjects since each technical committee has representation from all
interested IEC National Committees.

3) IEC Publications have the form of recommendations for international use and are accepted by IEC National
‘Committees. in that sense. While all reasonable efforts are made to ensure that the technical content of IEC
Publications is accurate, IEC cannot be held responsible for the way in which they are used or for any
misinterpretation by any end user.

4) In order to promote international uniformity, IEC National Committees undertake to apply IEC Publications
‘vansparently 10 the maximum extent possible in their national and regional publications. Any divergence between
any IEC Publication and the corresponding national or regional publication shall be clearly indicated in the latter.

5) IEC itself does not provide any attestation of conformity. Independent certification bodies provide conformity
assessment services and, in some areas, access to IEC marks of conformity. IEC Is not responsible for any
services carried out by independent certification bodies.

6) All users should ensure that they have the latest edition of this publication.

7) No liability shall attach to IEC or its directors, employees, servants or agents including individual experts and
members of its technical committees and IEC National Committees for any personal injury, property damage or
‘other damage of any nature whatsoever, whether direct or indirect, or for costs (including legal fees) and
expenses arising out of the publication, use of, or reliance upon, this IEC Publication or any other IEC
Publications.

8) Attention is drawn to the Normative references cited in this publication. Use of the referenced publications is
indispensable for the correct application of this publication.

9) Attention is drawn to the possibility that some of the elements of this IEC Publication may be the subject of patent
fights, IEC shall not be held responsible for identifying any or all such patent rights,

DISCLAIMER

This Consolidated version is not an official IEC Standard and has been prepared for
user convenience. Only the current versions of the standard and its amendment(s) are
to be considered the official documents.

This Consolidated version of IEC 61850-7-3 bears the edition number 2.1. It consists of

the second edition (2010-12) [documents 57/1087/FDIS and 57/1085/RVD] and its

amendment 1 (2020-02) [documents 57/2101/FDIS and 57/2132/RVD]. The technical
content is identical to the base edition and its amendment.

International Standard IEC 61850-7-3 has been prepared by IEC technical committee 57: Power

systems management and associated information exchange.

This second edition cancels and replaces the first edition, published in 2003.

“a
https:/Awww.doc88.com/p-74754903218494.htm! 11/151
```


## File page 012

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark ¥ Annotations v Q
-10- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020

Compared to the first edition, this second edition:

* defines new common data classes used for new standards defining object models for other
domains based on IEC 61850 and for the representation of statistical and historical data;

* provides clarifications and corrections to the first edition of IEC 61850-7-3;

Compared to the second edition, this first revision of the second edition:

a) provides clarifications and corrections to the second edition of IEC 61850-7-3, based on the
tissues = { 690, 691, 692, 697, 698, 707, 709, 711, 722, 814, 816, 819, 832, 839, 846, 868,
887, 919, 924, 925, 926, 929, 953, 954, 962, 968, 996, 1078, 1079, 1122, 1127, 1184, 1187,
1189, 1220, 1233, 1240, 1242, 1247, 1253, 1265, 1270, 1311, 1372, 1387, 1388, 1403,
1430, 1438, 1578, 1581, 1598, 1602, 1623 };

b) includes semantic of attributes within tables in clauses 6 and 7 and thus removes the need
for explicit semantic definition in Clause 8;

c) Clause 8 now contains definitions of newly introduced explicit enumerated types (with
tables); this is fully backward compatible as the value of the literals have not changed;

d) some subclauses in clause 7 have different numbering because of introduction of some
abstract types (that group common attributes for several concrete types);

e) first subclause under any CDC group in Clause 7, that contained the tables with applicable
services with respect to functional constraints, have been removed; that information is
explicitly defined in IEC 61850-7-2 with functional constraints, and temporarily included as
Annex B, Functional constraints;

f) content of 6.2.7 and 6.2.8 has been moved to the normative Annex D of IEC 61850-7-2:
Clarification on usage of quality;

g) implements extension introduced by IEC 62351-6 for security;

h) presence conditions have been redesigned and renamed to support their uniform usage in
all of the IEC 61850-7-xxx series as necessary. Below is the table containing the old and
the new presence conditions:

new original Notes

M M

° °

MOcond(condID) Various C, G1, ... |In IEC 61850-7-4

MFcondicondlD) Various C, C1, ... In IEC 61850-7-4

OF cond{condiD) Various C, C1, ... | In IEC 61850-7-4

MFsubst PICS_SUBST

‘AtLeastOne(1) ec

‘AtMostOne GC_1_EXCL

AllOrNonePerGroup(n) Gc2n

AllOnlyOneGroupin) GC_2 XOR_n

Mo{sibling) GC_CON attr

Mono ‘AC_LNO_M

MFIno ‘AC_LNO_EX

MOrootLD Ct in Common

MOInNs ‘AC_OLD_M

oan woONM

MOcdcNs ‘AC_OLNDA_M

MFscaledAV ‘AC_SCAV

MFscaledMagV AC_SCAV

a
nw
https://www.doc88.com/p-74754903218494.html 12/151
```


## File page 013

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark ¥ Annotations v Q
IEC 61850-7-3:2010+AMD1:2020 CSV -1-
© IEC 2020
new | origins =| Notes
MFscaledAngv ‘AC_SCAV
MAlIOrNonePerGroup(n) | AC_ ST
° |AC_co_O [Documentation provided in ControllableCDC class,
AC_SG_M ‘Split into explicit subtype, no need for presence condition.
AC_SG_O Split into explicit subtype, no need for presence condition.
AC_SG_C1 ‘Split into explicit subtype, no need for presence condition,
‘AC_NSG_M Split into explicit subtype, no need for presence condition.
‘AC_NSG_O Split into explicit subtype, no need for presence condition.
‘AC_NSG_C1 Split into explicit subtype, no need for presence condition.
MOrms ‘AC_RMS_M
° AC_CLC_O Eliminated presence condition on Vector.ang in favour of
documenting the relevant DO {in IEC 61850-7-4).
Clauses 5 to 8 and their subclauses, replacement for Annex A, Annex B and XML enumerations
from Annex D are automatically generated from the UML model.
This publication has been drafted in accordance with the ISO/IEC Directives, Part 2.
A list of all parts in the IEC 61850 series, published under the general title: Communication
networks and systems for power utility automation, can be found on the IEC website.
Contrary to usual IEC practice, for ease of use in this case, all tables and figures (including
those which have been added since Edition 2) have been numbered consecutively in the
amendment and the consolidated version.
This IEC standard includes Code Components i.e. components that are intended to be directly
processed by a computer. Such content is any text found between the markers <CODE BEGINS>
and <CODE ENDS>, or otherwise is clearly labeled in this standard as a Code Component. In
the current version of this document, such indication is made at the beginning of each
concerned top-level clauses
The purchase of this IEC standard carries a copyright license for the purchaser to sell software
containing Code Components from this standard directly to end users and to end users via
distributors, subject to IEC software licensing conditions, which can be found at:
httpv/www.iec.ch/CCv1.
If any updates are required to the published code component that needs to apply immediately
and can not wait for an amendment (ie. fixing a major problem), a new release of the Code
Component will be issued and distributed through the IEC WebSite. Any new release of the
Code Component related to this part will supersede any previously published Code Component
including the one published within the current document.
This publication contains attached nsd files which compose the Code Component of this part.
These files are intended to be used as a complement and do not form an integral part of this
standard.
a
nw
https://www.doc88.com/p-74754903218494.html 13/151
```


## File page 014

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark Y Annotations ¥ Q
-12- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
The committee has decided that the contents of the base publication and its amendment will
remain unchanged until the stability date indicated on the IEC website under
“http:/webstore.iec.ch" in the data related to the specific publication. At this date, the
publication will be
* reconfirmed,
* withdrawn,
* replaced by a revised edition, or
* amended.
IMPORTANT ~ The ‘colour inside’ logo on the cover page of this publication indicates
that it contains colours which are considered to be useful for the correct understanding
of its contents. Users should therefore print this document using a colour printer.
A
https:/mww.doc88.com/p-74754903218494.html 14/151
```


## File page 015

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark ¥ Annotations v Q
IEC 61850-7-3:2010+AMD1:2020 CSV -13-
© IEC 2020
INTRODUCTION
This document is part of a set of specifications, that details layered substation communication
architecture, This architecture has been chosen to provide abstract definitions of classes and
services such that the specifications are independent of specific protocol stacks and objects.
The mapping of these abstract classes and services to communication stacks is outside the
scope of IEC 61850-7-x and may be found in IEC 61850-8-x (station bus) and IEC 61850-9-x
(process bus).
IEC 61850-7-1 gives an overview of this communication architecture. This part of IEC 61850
defines constructed attributed classes and common data classes related to applications in the
power system using IEC 61850 modeling concepts such as substations, hydro power or
distributed energy resources. These common data classes are used in IEC 61850-7-4 to define
compatible dataObject classes. The SubDataObjects, DataAttributes or SubAttributes of the
instances of dataObject are accessed using services defined in IEC 61850-7-2.
This part of IEC 61850 is used to specify the abstract common data class and constructed
attribute class definitions. These abstract definitions are mapped into concrete object definitions
that are to be used for a particular protocol (for example MMS, ISO 9506 series).
nw
https://www.doc88.com/p-74754903218494.html 15/151
```


## File page 016

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark ¥ Annotations v Q
-14- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
COMMUNICATION NETWORKS AND
SYSTEMS FOR POWER UTILITY AUTOMATION -
Part 7-3: Basic communication structure —
Common data classes
1 Scope
1.1 General
This part of IEC 61850 specifies constructed attribute classes and common data classes related
to substation applications. In particular, it specifies:
= common data classes for status information,
= common data classes for measured information,
— common data classes for control,
— common data classes for status settings,
— common data classes for analogue settings and
— attribute types used in these common data classes.
This International Standard is applicable to the description of device models and functions of
substations and feeder equipment.
This International Standard may also be applied, for example, to describe device models and
functions for:
— substation to substation information exchange,
— substation to control centre information exchange,
- power plant to control centre information exchange,
— information exchange for distributed generation, or
— information exchange for metering.
1.2 Namespace name and version
This new section is mandatory for any IEC 61850 namespace (as defined by IEC 61850-7-
1:2011).
The parameters which are identifying this new release of this namespace are:
— Namespace Version: 2007
— Namespace Revision: B
- Namespace name: “IEC 61850-7-3:2007B"
- Namespace release: 3
— Namespace release date: 2019-10-02
IEC 61850-7-3 depends on IEC 61850-7-2:2007B latest release.
The table below provides an overview of all published versions of this namespace.
nw
https://www.doc88.com/p-74754903218494.html 16/151
```


## File page 017

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark ¥ Annotations v Q

IEC 61850-7-3:2010+AMD1:2020 CSV -15-

© IEC 2020

a

[—eason 20 [awe | __ eresoraanvo 0 reso samo _|

Edition 2.0

[eavon ar _[__weowe |  wienrazonorze cov | eo esorsare |

1.3. Code Component distribution

The Code Component will be available in light and full version:

— Full version will contain definition of the whole LNs defined in this standard with the
documentation associated and access will be restricted to purchaser of this part.

- Light version will not contain the documentation but will contain the whole definition of the
LNs as per full version, and this light version will be freely accessible on the IEC website
for download, but the usage remains under the licensing conditions.

The link for downloading the light version of this code component is:

httpy/www.iec.ch/public/TC57/supportdocuments/IEC_61850-7-3.NSD.2007B3.light.zip

The Code Components for IEC 61850 data models (like basic types, presence conditions, ...

definition in this IEC standard) are available as the file format NSD defined by IEC 61850-7-7.

The Code Component(s) included in this IEC standard are potentially subject to maintenance

works and user shall select the latest release in the repository located at:

http:/www.iec.ch/TC57/supportdocuments

The latest version/release of the document will be found by selecting the file IEC_61850-7-

3.NSD.{VersionStatelnfo}.light.zip with the filed VersionStatelnfo of the highest value.

Each Code Component is a ZIP package containing the electronic representation of the Code

Component itself, with a file describing the content of the package (IECManifest.xml).

The IECManifest contains different sections giving information on:

= The copyright notice

— The identification of the code component

— The publication related to the code component

— The list of the electronic files which compose the code component

— An optional list of history files to track changes during the evolution process of the code
component

The life cycle of a code component is not restricted to the life cycle of the related publication.

The publication life cycle goes through two stages, Version (corresponding to an edition) and

Revision (corresponding to an amendment). A third publication stage (Release) allows

publication of Code Component without need to publish an amendment.

This is useful when InterOp Tissues need to be fixed. Then a new release of the Code

Component will be released, which supersedes the previous release, and distributed through

the IEC TC57 web site.

nw
https://www.doc88.com/p-74754903218494.html 17/151
```


## File page 018

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark ¥ Annotations v Q
-16- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020

2 Normative references

The following documents are referred to in the text in such a way that some or all of their content

constitutes requirements of this document. For dated references, only the edition cited applies.

For undated references, the latest edition of the referenced document (including any

amendments) applies.

IEC 60255-151:2009, Measuring relays and protection equipment - Part 151: Functional

requirements for over/under current protection

IEC TS 61850-2, Communication networks and systems for power utility automation - Part 2:

Glossary

IEC 61850-7-1, Communication networks and systems for power utility automation - Part 7-1:

Basic communication structure - Principles and models

IEC 61850-7-2, Communication networks and systems for power utility automation - Part 7-2:

Basic information and communication structure - Abstract communication service interface

(ACSI)

IEC 61850-7-4, Communication networks and systems for power utility automation - Part 7-4:

Basic communication structure - Compatible logical node classes and data object classes

IEC TS 61850-7-7, Communication networks and systems for power utility automation - Part 7-

7: Machine-processable format of IEC 61850-related data models for tools

IEC TS 62351-6:-, Power systems management and associated information exchange data and

communication security — Part 6: Security for IEC 61850'

IEC/IEEE 60255-118-1, Measuring relays and protection equipment - Part 118-1:

Synchrophasor for power systems — Measurements

ISO 4217, Codes for the representation of currencies and funds

3 Terms and definitions

For the purposes of this document, the terms and definitions given in IEC TS 61850-2 and

IEC 61850-7-2 apply.

ISO and IEC maintain terminological databases for use in standardization at the following

addresses:

* IEC Electropedia: available at http:/www.electropedia.org!

* ISO Online browsing platform: available at httpy/www.iso.org/obp

34

<abstract> common data class

data class which is never instantiated, used to group common attributes into a semantically

meaningful entity and reuse them in a concrete common data class through inheritance

1 Under preparation. Stage at the time of publication: IEC/PRVC 62351-6:2020.

nw
https://www.doc88.com/p-74754903218494.html 18/151
```


## File page 019

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark ¥ Annotations v Q
IEC 61850-7-3:2010+AMD1:2020 CSV -17-
© IEC 2020
3.2
<statistics> common data class
data class which is allowed to be used as a type of a data object within the derived statistics
logical node instance (as well as the non-derived statistics logical node)
Note 1 to entry: A common data class not designated as statistics is forbidden for use in the context of a derived
Statistics logical node.
33
<deprecated> element
element, marked as deprecated is still maintained in this edition of the standard, for backwards
compatibility purpose, but is intended to be phased out in the future
Note 1 to entry: A deprecated element by definition indicates what should be used instead.
4 Abbreviated terms
act actual
add ditional
addr address
altitude altitude
ang angle
BL blocking
bik block
° ‘sequence component
c ‘config,
cal calender
cat category
ce control. block
coc common data class
call cell
cr configuration
charac characteristic
asses classes
md ‘command
ont ‘counter
contig contig
ov curve
ofl control
our currency
Val complex value
oe oye
4 description
data data
day day
db dead band
oe description
dehg trigger option for data-change
dir direction
dupa ‘tigger option for data-update
ur duration
ena enable
end end
Ps electrical power system
eval evaluate
a
cy
8
nw
https://www.doc88.com/p-74754903218494.html 19/151
```


## File page 020

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark ¥ Annotations v Q
-18- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
Ex ‘extended
t float
factor factor
Fo functional constraint
fr frozen
frequency ‘frequency
‘general general
h high
har ‘harmonic
hh high high
hr hour
ht histogram
w harmonic value
hw hardware
i integer
i identifier
ident identifier
ind indication
info information
inst instantaneous
int intemal
i] jow
latitude labtude
LJ logical device
im fini
off off
Li tow low
hh logical node
location locaton
longitude fongitude
mag magnitude
max maximum
min srinimum
ma minute
model model
‘month month
me master
multiplier multiplier
mm measured
rd measurands (analogue values)
name name
net net
neut neutral
ns name space
um number
od occurence
offset offset
ok ok
on on
op operate:
oper operate
or origin
OR oper received
ry
cy
8
Aa
https://www.doc88.com/p-74754903218494.html 20/151
```


## File page 021

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark ¥ Annotations v Q
IEC 61850-7-3:2010+AMD1:2020 CSV -19-
© IEC 2020
‘owner ‘owner
param parameter
par parameter
pd period
per period
persistent persistent
phs phase
phsA phase A
phsB phase B
phsc phase ©
pls pulse
point point
pos position
prime primary
pts points
puls puls
pulse pulse
purpose purpose
q quality
eng ‘wigger option for quality-change
aly quantity
qual qualityer
range range
rate rate
rove received
ref reterence
res residual
rev revision
rms mms
s reset
sbo select before operate
scale ‘scale
SE setting group editable
second second
seid ‘selected
ser ‘serial
seq sequence
set set
sev severity
8G ‘setting group
st ‘System international
size size
sm sample
SR service response
se source
st ‘state
ST ‘status information
step step
sr start
start stan
sub ‘substituted
‘sP ‘setting
sv ‘substitution
ry
cy
8
Aa
https://www.doc88.com/p-74754903218494.html 21/151
```


## File page 022

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark ¥ Annotations v Q
-20- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
svc scale value config
ow software
t time
T type
timeout timeout
‘issue Technical Issue - See IEC 61850-1
im time
to to
© transient
rans transient
TrgOp ‘igger option
tst test
type ‘ype
u unicode
unit unit
units units
val value
vendor vendor
w with
x x coordinate
y y coordinate
week week
z 2 coordinate
ze10 2600
NOTE Abbreviations used for the identification of the common data classes and as name of attributes are specified
in the specific clauses of this document and are not repeated here.
5 Conditions for element inclusion
From this edition on, presence conditions are normatively defined in IEC 61850-7-2. For the
reader's convenience, they are also available in Annex E.
6 Constructed attribute classes
6.1 General
The constructed attribute classes structure and descriptions are part of the Code Component
of this IEC standard and are available as electronic machine readable file in related NSD file.
Constructed attribute classes are defined for the use in common data classes (CDC) in Clause
7.
IEC 61850-7-1 provides an overview of all IEC 61850-7 documents (IEC 61850-7-2, IEC 61850-
7-3, and IEC 61850-7-4). IEC 61850-7-1 also describes the basic notation used in IEC 61850-
7-3 and the description of the relations between the IEC 61850-7 documents.
NOTE The domain type Timestamp’ like all other basic and domain types are specified in IEC 61850-7-2.
6.2 Configuration of analogue value (ScaledValueConfig)
This type shall be used to configure the integer value representation of the analogue value. See
AnalogueValue.i.
nw
https://www.doc88.com/p-74754903218494.html 22/151
```


## File page 023

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
IEC 61850-7-3:2010+AMD1:2020 CSV -21-
© IEC 2020
NOTE If a server does not support transmission of floating point values, the client may retrieve these values from
the SCL file.
Table 1 shows all attributes of ScaledValueConfig.
Table 1 — Attributes of ScaledValueConfig
[Awe mame | Atte pe | (aes ange) Dasepton——_|_—PresCond |
aa =a
analogue value.
= SF
analogue value.
6.3 Range configuration (RangeConfig)
This type shall be used to configure the limits that define the range of a measured value.
fangeC ____tange __validity __detailQual
hhigh-high questionable outOfRange
mex
high-high good
htm
high good
hhLim
normal ‘good
tin
low good
him
low-low good
min
low-low questionable out OfRange
Figure 1 - Range configuration
Figure 1: This diagram illustrates relationships between range, range configuration and quality
of measured process value.
Table 2 shows all attributes of RangeContig.
Table 2 — Attributes of RangeConfig
| _Atibute rane | —Atibue ype | (Vaal range) essipton | Prextond
(ial Gana 2° alae eal
>= ae
‘normal and “high’.
= se |
‘normal and ‘ow’.
= =
‘low’ and iow-low.
a
“a
https:/Awww.doc88.com/p-74754903218494.htm! 23/151
```


## File page 024

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
-22- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
[Atte name | Abe type | (Vaal range) Oeserpon | Prescond_|
AnalogueValue The minimum process measurement for
which ‘AnalogueValue.i.T is considered
within process limits. If the value is lower,
quality shail be set accordingly
(Quality. detailQual.outOfRange'=true =>
‘Quality. validity'«'questionable’).
The maximum process measurement for
which ‘AnalogueValue.|if is considered
within process limits. If the value is higher,
quality shall be set accordingly
(Quality. detailQual.outOtRange'=true =>
‘Quality. validity’="questionable’)
INT32U (range=[0...100000]) When present, the
value used to introduce a hysteresis in the
calculation of ‘range’. When a highvlow limit
has been crossed, ‘range’ is immediately set
to the higher/lower value, However, ‘range’
is only set back to the lowerihigher value
when the value of the high limit minus/ow
limit plus limDb’ has been crossed. The
value shall represent the percentage
between ‘max’ and ‘min’ in units of 0.001 %
6.4 Step position with transient indication (ValWithTrans)
This type shall be used to indicate the position of tap changers.
Table 3 shows all attributes of ValWithTrans.
Table 3 — Attributes of ValWithTrans
| _Aibute rane | _Atibuie ype | (Vaal range) ossipton | PrexCond
rr er Co
[vara [BOOLEAN | Ft, he eave ina arson wan [ 0 |
6.5 Pulse configuration (PulseConfig)
This type shall be used to configure the output generated to the on or off input of a switching
device as a result of receiving an ‘operate’ service request.
If ‘omdQual=persistent’, the output stays in the state indicated in the ‘operate’ service request.
If ‘cmdQual=pulse’, the output is a pulsed signal as defined below. If ‘cmdQual=persistent-
feedback’, the output stays in the state indicated in the ‘operate’ service request until the state
is reached.
a
“a
https:/Awww.doc88.com/p-74754903218494.htm! 24/151
```


## File page 025

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
IEC 61850-7-3:2010+AMD1:2020 CSV -23-
© IEC 2020
1 2 nurs:
+> ao
+> cc nes
Figure 2 - Configuration of command output pulse
Figure 2: This diagram illustrates how attributes ‘onDur’, ‘offDur’ and ‘numPls' are used to
configure output pulse.
Table 4 shows all attributes of PulseConfig.
Table 4 — Attributes of PulseConfig

[Atrios vane | Atibue ype | (aheVaue range) Oneopten | PresCond_|
~~ ("| Be |

value ‘persistent’ is reserved for controlled

objects that have a single control Output. In

that case, the command ‘on’ activates the

Output, the command ‘off’ deactivates it.
= aera =

duration is defined locally.
== mae

0, the duration is defined locally.
[name | wma | Nome ot pes wat ae gorecicd w=
6.6 Unit definition (Unit)
This type shall be used to represent unit and multiplier information.
Table 5 shows all attributes of Unit.

Table 5 — Attributes of Unit
[Avot name | Abe pe | ate range) Oespion | —resond
[sunt | stunitins [Stunt of measure, tM
[imac aionr [tun Unt mas ——SCSCSC~sr C=
6.7 Vector definition (Vector)
This type shall be used to represent a coherent complex value (phasor), with magnitude and
angle acquired or determined simultaneously.
Table 6 shows all attributes of Vector.
a
nw
https://www.doc88.com/p-74754903218494.html 25/151
```


## File page 026

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
—24- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
Table 6 — Attributes of Vector
[strove mae | Aut pe | Yavin range) esoton | —restond_|
a er
(range=[-180...180]) Angle of the complex
value (Unit SiUnit='deg" and
Unit.muttiplier="); angle reference is defined
in the context where this type is used.
6.8 Point definition (Point)
This type shall be used to represent points in a two- or three-dimensional coordinate system.
Table 7 shows all attributes of Point.
Table 7 — Attributes of Point
[awe rane | Aiba pe | ate range) Oesipion | reson
[wa —S*d owes as waeSid
[wa one Yan
[wa [owtse | zis va
6.9 Cell (Cell)
This type shall be used to define a rectangle area in a two-dimensional environment. It can also
be used to describe a range within a one-dimensional environment.
|_| -
xStart / yStart
wee 258110
Figure 3 - Cell definition
Figure 3: This diagram illustrates the definition of Cell.
Table 8 shows all attributes of Cell.
a
“a
https:/Awww.doc88.com/p-74754903218494.htm! 26/151
```


## File page 027

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
IEC 61850-7-3:2010+AMD1:2020 CSV -25-
© IEC 2020
Table 8 — Attributes of Cell
[stove mane | Aut pe | Yavin ange) esetton | stort
FLOAT32 X value of the lower left comer of the
square.
X value of the upper right comer of the
square. Absence of the attribute indicates
infinity in the direction of the x axis,
yStart FLOATS2 Y value of the lower left comer of the
square. For one-dimensional range, this
attribute shall be absent.
FLOATS2 Y value of the upper right corner of the
square. For two-dimensional range,
absence of the attribute indicates infinity in
the direction of the y axis. For one-
dimensional range, this attribute shall be
absent.
6.10 Calendar time definition (CalendarTime)
This type shall be used to define a time setting in reference to the calendar. It allows the
specification of times like the last day of the month or the second Sunday in March at 03.00 h.
The time shall be expressed as UTC unless specified different where used.
ecsPer _oceType 0
Hour __Time__At <mn> minute every hoye
Day ___Time __As <hr>, <mn> everyday 0
Week _WeekDay _At <waekDay>, <hr>, <mn> avery week 0
‘Month __WeekDay _At <occ>, <weekDay>, <hr>, <mn> every month __
Month __DayOfMonth _At <oce>, <hr>, <mn> every month
‘Year___Time ____At <month>, <dav>, <hr>, <mn> every year __
Year ___WeekDay__At <occ>, <weekDay>, <month>, <hr>, <mn> every vear,
‘Year ___WeskOfvear__At week <occ>, <weekDay>, <hr>, <mn>
Year __DayOfvear At <oce>, <hr>, <mn> every year 00
Figure 4 - Interpretation of calendar time settings
Figure 4: This diagram shows the semantic interpretation of the calendar time attributes.
Table 9 shows all attributes of CalendarTime.
Table 9 — Attributes of CalendarTime
[Atrios vane | Albus ype | (Vaheaue range) Onecpten | reson _|
INTIEU. Occurrence of a calendar element. The
value 0 is used to indicate the last. For the
identification of week numbers, week
umber 1 shall always be the first week in
January (according to definition of UN /
CEFACT).
== ee
occurrence.
a
nw
https://www.doc88.com/p-74754903218494.html 27/151
```


## File page 028

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
—26- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
[Atte name | _Atbue type | (Valea range) Deserpon | Prescond_ |
Periodkind Repetition period of a calendar-based time
setting.
[re age.) Bay a iw
[mary | era 2) Ho oy co
[mm wre [var Mee ft iw
6.11 Analogue value
6.11.1 General
This clause groups all the variants of the analogue value types used for monitoring, control,
settings and tracking.
6.11.2 Analogue value (AnalogueValue)
Analogue values may be represented as a basic type integer (attribute 7) or as a floating point
(attribute ‘f). At least one of the attributes shall be used. If both ‘7 and ‘f exist, the application
in the server shall insure that both values remain consistent. When the analogue values
represent measured process value, they shall be the primary values.
NOTE Except for the usage in the CDC SAV, it Is recommended to support the representation as a floating point
{attribute 'f) since the representation as an integer (attribute ‘/) may be deprecated in the future.
Table 10 shows all attributes of AnalogueValue.
Table 10 - Attributes of AnalogueValue
[Avo rare | Ate pe | etnate ang) Dozipion | —PresCond
| | Bees
value, The formula to convert between ‘? )
and the process value (pVal) shall be:
pVal=i('i" ScaledValueConiig.scaleFactor}+
"ScaledValueContig.offset’) in [Unit.SIUnit).
|" Bee
measured value. The formula to convert D)
between 'T and the process value (pVal)
shall be: pVal=¥*10exp('Unit. multiplier’) in
[Unit SiUnit).
6.11.3 Analogue value control (AnalogueValueCtl)
This type shall be used for control value service parameter only. See AnalogueValue; the only
difference is the presence condition for attributes (either ‘i' or ‘f present).
Table 11 shows all attributes of AnalogueValueCtl.
a
nw
https://www.doc88.com/p-74754903218494.html 28/151
```


## File page 029

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
IEC 61850-7-3:2010+AMD1:2020 CSV -27-
© IEC 2020
Table 11 — Attributes of AnalogueValueCtl
[Ato mame | Atte pe | ‘Yavin range) esepton | —restond_|
I ol oie
value, See ‘AnalogueValve.’ roup(1)
eres, |
measured value. See ‘AnalogueValue.. roup(2)
6.11.4 Analogue float value control (AnalogueValueCtiF)
This type shall be used for tracking of float control value service parameter only. See
AnalogueValue.
Table 12 shows all attributes of AnalogueValueCtlF.
Table 12 — Attributes of AnalogueValueCtlF
[Ato mame | Aut Ye | ——Yavae range) esepton | escort
ee ieee, |
measured value. See ‘AnalogueValve.,
6.11.5 Analogue integer value control (AnalogueValueCtlint)
This type shall be used for tracking of integer control value service parameter only. See
AnalogueValue.
Table 13 shows all attributes of AnalogueValueCtlint.
Table 13 - Attributes of AnalogueValueCtlint
[—Abite name | Abu pe | (Valea range) Ossepion | PrsCond_|
i em |
value. See “AnalogueValue.’.
7 Common data class specifications
7.1 General
The common data classes structure and descriptions are part of the Code Component of this
IEC standard and are available as electronic machine readable file in related NSD file.
Common data classes are defined for use in IEC 61850-7-4. Common data classes are
composed of the following:
— constructed attribute classes defined in Clause 6,
= types defined in IEC 61850-7-2,
— common data classes defined in this clause,
— enumerated data attribute types defined in Clause 8.
a
cy
8
nw
https://www.doc88.com/p-74754903218494.html 29/151
```


## File page 030

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark ¥ Annotations v Q
—28- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
IEC 61850-7-1 provides the basic notation used in this clause.
The common data classes define the relation between their attributes and the functional
constraint as well as the possible trigger options. Sometimes, both dchg and dupd are specified
as a possible trigger option for the DataAttribute. In that case, the concrete implementation
shall select one of them, based on the purpose of the data object typed by that common data
class. Trigger option dchg shall be used for DataAttribute where a change of the value is
necessary to create an event, whereas trigger option dupd shall be used for DataAttribute where
an update of the value (with or without change) is enough to create an event.
[<tass CommonDataciasses //
[CoreAbeeracscoe [eocseenetnfe |]  [cocanstogestate | [cocceneret |
[El + BasePrimtivecoe acs B+ Harmonictleasurandcoc | | [Sj + Contro/TestingCOC
[Ee] + SaseComposedCOC [toes gem fase
[B+ Substittioncoc ecm B+cu f+ oec
res Bes farinc
[-act G-we rac
taco ton facesc
tse +s fa-wsc
Bcece = +Huv tare
Eichsr thm free
Brus i +Hoe
Bros
firts
+ ASC ton prose
+ CURVE pr eters
+o fB+cso Brus
pty fetus
fro
fetes
Bos
css
pres
Figure 5 — Class diagram CommonDataClasses::CommonDataClasses
Figure 5: This diagram shows all the common data class groups, with their contents.
Classes displayed in italic and ending with "CDC" are abstract common data classes that allow
avoiding duplication in definition of attributes used by multiple concrete common data classes.
Abstract common data classes are never instantiated; their attributes are inherited by concrete
common data classes which are instantiable.
7.2 Modelling introduction
7.24 General
All common data classes defined in this document inherit their structure from the abstract
common data class defined in IEC 61850-7-2. Common data classes (CDC-s) are used as types
for data objects of logical nodes, and are composed of:
- DataAttribute, which can be either of a basic type, or of a predefined structured type, or
— SubDataObject, which is of type CDC.
a
nw
https://www.doc88.com/p-74754903218494.html 30/151
```


## File page 031

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark ¥ Annotations v Q
IEC 61850-7-3:2010+AMD1:2020 CSV -29-
© IEC 2020
The generic structure of common data classes defined in IEC 61850-7-2 comprises the
attributes (e.g., name and reference, functional constraints and trigger options), and the
services depending on a functional constraint.
ee
BasePrimitiveCDC BaseComposedCDC
+d: VisString2S$_0C [0..1] +d: VisString2SS_0¢ [0..1]
+ dU: Unicode2S$_0C [0.1] + dU: Unicode2S5_0C [0.1]
+ cdcName: VisString255_Ex [0..1] + cdeNvame: VisString25$_Ex [01]
+ dataNs: VisString255_& [0..1] + dataNs: VisString25S_EX [0..1]
constraints constraints
(MOdatans} (MOdatans}
L\
‘+ subEna: BOOLEAN.SV [0.1]
+ subQ: Quality SV [01]
+ subID: VisString64_SV [0..1]
+ _blkena: BOOLEAN.BL [0.1]
constraints
IMFsubsti
Figure 6 — Class diagram CoreAbstractCDCs::CoreAbstractCDCs
Figure 6: This diagram shows all the core abstract common data classes with their attributes
and constraints in the UML notation.
Non-mandatory attributes have multiplicity [0..1]; in classes displayed in this diagram, there are
no mandatory attributes. Possible constraints, used for presence conditions, are only listed in
the diagram, while their applicability to individual attributes is given in the common data class
tables. Presence conditions are normatively defined in IEC 61850-7-2 and available in Annex
E in this document.
NOTE The two of the shown abstract classes, BaseComposedCDC and BasePrimitiveCDC look the same and their
Content is identical. However, they are different from the perspective of the underlying meta-model, which is out of
scope of this part.
7.2.2 <<abstract>> Common attributes for primitive CDC (BasePrimitiveCDC)
Abstract type, holding attributes common to all primitive common data classes.
Table 14 shows all attributes of BasePrimitiveCDC.
nw
https://www.doc88.com/p-74754903218494.html 31/151
```


## File page 032

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
-30- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
Table 14 — Attributes of BasePrimitiveCDC
fase [mem |e [g | enn [em
Textual description of the data. In case
it is used within the CDC LPL, the
Gescription refers to the logical node.
Unicode2s5 Textual description of the data using
unicode characters. In case it is used
within the CDC LPL, the description
relers to the logical node,
Sie Na al =a
details see IEC 61850-7-1.
IEC 61850-7-1. If present, the valve
shall be initialized through the SCL
configuration file to a valid name space.
7.2.3 <<abstract>> Common attributes for composed CDC (BaseComposedCDC)
Abstract type, holding attributes common to all composed common data classes.
Table 15 shows all attributes of BaseComposedCDC.
Table 15 — Attributes of BaseComposedCDC
[ase | em [fg | tmmnoen [rem
DataAttribute for configuration, description and extension
[2 | vaseneess [00 | | Sw tanoramaacoce. ——«fo—_—|
[a | unomass [oc | [see taerinecocar fo _|
[cxniene | vasungess [ex | | Sen Baerimnecoceanane fo _|
[sans [wasweatss [ex |_| Soe easernavecocanans. | womans |
7.24  <<abstract>> Common attributes for substitution (SubstitutionCDC)
Abstract type, holding attributes common to those common data classes that provide values
that can be substituted.
In the typical use case for substitution, an operator on the client side enters manually a value
for a data attribute located in a specific device. The client sets the data attribute to the value
entered. If a client accesses the value of that attribute (for example, using a GetDataValue
service or subscribing to a report), the client shall receive the manually entered (substituted)
value instead of the value determined by the process.
a
“a
https:/Awww.doc88.com/p-74754903218494.htm! 32/151
```


## File page 033

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark ¥ Annotations v Q
IEC 61850-7-3:2010+AMD1:2020 CSV -H-
© IEC 2020
i xy.subEna Ixy subVal = value for substitution
rue = substituted

paver]

Ixy.q.validity — _

bie IN

[=|

Example: Common data class
"SPS" (see IEC 61850-7-3)
Figure 7 - Concept of substitution
Figure 7: This diagram illustrates the concept of substitution. Usually, input from the process or
the result of the calculation from a function provides the value of a data attribute. In that case,
"CDC.q.source' = ‘process’. In case of substitution, the value of a data attribute may be provided
by an operator making use of a client, and in that case ‘CDC.q.source’ = ‘substituted’. This
selection of the source of the value (substitution value or process value) shall be controlled by
the service SetDataValue (‘SubstitutionCDC.subEna’ = true) to substitute or SetDataValue
(‘SubstitutionCDC.subEna’ = false) to unsubstitute. The service SetDataValue shall also be
used to set the substituted value (CDC.subVal' = value-for-substitution) and quality
(CDC.subQ’ = quality-for-substitution). There may be cases where a local automatic function
disables substitution, for example, if blocking of information exchange is disabled or
communication is no longer interrupted.
It is the responsibility of the client application, in particular in the case of multiple attributes to
be ‘substituted, to set all relevant substitution values
({SPS,DPS,INS,ENS,SPC,DPC, INC,ENC}.subVa'’, "MV.subMag’, "CMV.subCVal,
“SubstitutionCDC.subQ', “SubstitutionCDC.subID') before enabling ‘substitution
(‘SubstitutionCDC.subEna’ = true). While substitution is enabled, changing of all substitution-
related data attributes is allowed but it is the responsibility of the implementation to avoid
inconsistent transient value combination.
Table 16 shows all attributes of SubstitutionCDC.
a
nw
https://www.doc88.com/p-74754903218494.html 33/151
```


## File page 034

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
-32- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
Table 16 — Attributes of SubstitutionCDC
Pe [mem [| me [rm
sv Used to enable and disable
substitution. If ‘subEna’ = true, the main
data value and quality shall always be
Set to the same value as the attributes
used to store the substitution data
value and quality, as follows:
for SPS, DPS, INS, ENS, SPC,
DPC, INC, ENC set: 'sVal' to value
from ‘subVal’, ‘q' to value from
— for MV set: ‘instMag’ to value from
‘subMag’, ‘q' to value trom ‘subQ';
— for CMV set: ‘instCVal' to value from
‘subCVal, ‘q' to value trom ‘subQ’;
- for BSC, ISC set: valWT? to value
from ‘subVal, 'q' to value from
“subQ’; and,
for APC, BAC set: ‘mxVat' to value
from ‘subVal; ‘q' to value from
Otherwise, the data value shall be
based on the process value.
Quality Value used to substitute 'q’. Any
element other than the “q.source’ can
be substituted.
VisString64 Identitication of the device or operator
that made the substitution, It shall be
Set to NULL if ‘subEna’ = false or if the
attribute is not set by the client
BOOLEAN tue, ‘qoperatorBlocked’ = true, and
the process value is no longer updated.
[s [vases [0c | | wieaes ton: sxaPimnacoc [Oo _|
[| Uneoseass | 0c |_| mera tom BasePinmecoc [0 _|
[care | vesigess [© | | motes tan BasPrmmrecoc fo |
[caus | wesrnaess [x | heme vor easehimiecoe | womans
7.3 Status information
7.3.1 General
This subclause defines all the common data classes for status information. Status information
is typically of type Boolean, integer or enumeration and is information collected from the process
‘or produced by an application function. Status information has the functional constraint ST.
Status information cannot be written, but could be substituted.
For applicable services, see Annex B.
a
nw
https://www.doc88.com/p-74754903218494.html 34/151
```


## File page 035

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
IEC 61850-7-3:2010+AMD1:2020 CSV -33-
© IEC 2020
[<tase-cocsteomtate 7
CoreAbstractCOCs::
2 du: Uneode255.0¢ 1.11
+ denne Visering2 55,210.11
> decane vistring255. 10.11
CoraabstractCDCs::
+ tn s00uA SVE |__ =
suber Quay 5¥ 8-1) > geval BOOLEAN ST ache
+ nblo: Viakringt5¥ 9.11 + phoA: BOOMAN.ST. deh 10.13
——— : Seams |
Bhs: BOOUAN.ST.dehg 10.1}
| |: eee
+ Quality sT.9che > weeval ree 57-dehg a)
+E Timestang. ST > feval were4 57dupd 10.13
+ erignsre: Originator ST 10-1] tm Timestamp. 5710.11
> tperTmPhad: Timttame SF 1B.11| +e Quahey.sTaxcha
ee : ae
+ tperTnthn€: Torstar SF >: Unie
7 wa ROOT : = > pokey nostra
+ Quaiy-SF9ehg rena: BOOLEAN. CF seh [0-1]
+e Timestame.sT [Fe + strToe: Timestamp.CF chy 10.12
> soba BOOGEAN.SV 10.11 > feed ne cr dehy 1
> greek WOOUAN ST ache + teks BOOLIAN. tcha 11
——— | =
+ phod: BOOLEAN. ST deg 101)
+ phat: BOOLEAN. ST. cha D1]
xk | + dirPhaB. Phase aultDirection ST_dehg 10.11 ofits
——— —
+ Val Opiate sT.aehg + iePhac:PhaseFoukiresion.ST-dehg 101]
+ Steen + en ane
+E Tiestame SF + dennue PhaetautDwecion STAehg 1.11 +t Qealy stack
+ svat Opstans. SV 10.11 +g cual St ache +e Teeeneap St
len | +E Timestane. ST > runs: NTI SUCK dehg = 1_manPes
+ bathanget: Call. CF de "7
wourrnnts Mie
atornenaecracg > fine: cea 1
AllorNonaPer Croup > is: Uni CF deg 1)
| = (tornonaereseoe 2 mate 60a
AllorNonePer Crowes + xD. Vusuring255.0E
+ val weraa sacha dopa x0: Uneode2$5.0€ 10.11
+ 4: Qualy 579ch9 yO: VaSering2S5-0¢ [0.11
+E rimestane. 5 2 you Uncode255,0¢ 10.11
=a
umes: une CF deg 1011
Tce Ta ST Achy
[af
=e =o
+ addr ocets4 5710.11
ee
+ 4: Quality sToaehg
—
: Sain —
+E Timestang ST
> wevat Enum, 10.11 > aval Tawa Stach dopa
2 Soin state [ os
+E nimestane SF
——
ne —
Figure 8 - Class diagram CDCStatusinfo::CDCStatusinfo
Figure 8: This diagram shows all status information CDCs defined in the standard with
‘supertypes that factor their common attributes.
7.3.2 Single point status (SPS)
This common data class shall be used to represent single point status values.
a
nw
https://www.doc88.com/p-74754903218494.html 35/151
```


## File page 036

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
-34- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
Table 17 shows all attributes of SPS.
Table 17 — Attributes of SPS
[eer [mem [eg | term me [rom
[wr [eonem [st [ow [vacaneam [wi
[e [oat (st [tw | Oty ot te nnn war
| nee
value in any of ‘stVal’ or ‘q.
[sacra [eooen [sv | | ema ton: Sammisrcoe | wravt_|
[neva [eoocem [sv |_| vate uses stsnse var wast |
[sco | oaty (sv |_| ered tom: Subinancoc ‘| Wret |
[sei | wesweaee [sv | | menos von: susstencoc | wrast |
[nen | pootean [at _| | ewes om: Sibanaoncoe [0
[s[vasureass [oc | [wins tan: arontecoe fo _|
[ao | voconass [00 | | mes tom aaePamavecoo [0
[nano | wasweaess [ex [| mone von Saspimtecoc [|
[sine | vaseess [ex | | ered fom BasPrravecoe | Oca
7.3.3 Double point status (DPS)
This common data class shall be used to represent double point status values.
Table 18 shows all attributes of DPS.
Table 18 — Attributes of DPS
i_Seoer |e ore |r| ap | Seine men ony | ee
ee
[ava | ossanakns [st [are |vaveotme aia Swi
[e [aay iS [tw [ety te en wr
Oi al
value in any of ‘stVar" or ‘q'.

re
[saver | Oost | SV_| | abe used abate war ‘| Mra |
[seo [omy [S| | moms vans summancoc | wast |
[smi | vases | v_| | ered fom Subencoc ‘| Wet |
[omen —[soocemn [a | | meaes vor: Summaancoc fo _|
[2 [waswrazss [0c [| [menos vom: eexspimivecoe [0 |
[a | ves [oc | [etait tan asopimmecoc fo |

a

cy

8

“a

https:/Awww.doc88.com/p-74754903218494.htm! 36/151
```


## File page 037

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark Y Annotations ¥ Q
IEC_61850-7-3:2010+AMD1:2020 CSV - 35-
© IEC 2020
a eG
a
[nis [vases | © [| ewes tom esePimivecoe | woman
7.34  <cstatistics>> Integer status (INS)
This common data class shall be used to represent integer status values.
Table 19 shows all attributes of INS.
Table 19 — Attributes of INS
[ase [memo [re] ag [ some meen [me
‘dchg | Value of the data.
upd
a a
Timestamp Timestamp of the last change or update
event of ‘'stVal’ or the last change of
value in ‘q.
[seem [soot [sv |__| heed tom steaencoe | Wat _ |
[seve [wre [sv |_| abe wed to wbeiie evar ——~( rt |
re
[sno | vasurass | sv | | ead tom: Samitncoe | wraiat_|
[ewes [eoctemn [at | | wiwes von: sammaorcoo fo _|
a oa
a
[| wsenwass | 56 [ [wort tans asoinenecoc | o_|
[esawne | waserass [ex | [wets van: euarintecoo | o—_|
[one | wasureass | © | [ies tom: BasePimevecoo | wean
7.3.5 <<abstract>> Enumerated status (ENS)
This common data class shall be used to represent integer status values with the value
restricted to those in an enumeration.
Table 20 shows all attributes of ENS.
a
nw
https://www.doc88.com/p-74754903218494.html 37/151
```


## File page 038

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
-36- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
Table 20 — Attributes of ENS
= aE iG
‘dchg | Value of the data.
upd
Ca
event of 'stVal’ or the last change of
value in ‘a.
[seem [eoccean [sv | | mows toms Summsancoc [wana |
[eeve[emmoa [sv || Ye ures tate svat et |
[sea [omy [5 | | mene vans susaencoc | wrast—|
[smi | vasergee | sv_| | eed om Siboncoc | Wraat —|
[oem [sou [| | menos vor Susmaencoc fo _|
DataAttribute for configuration, description and extension
[| vaseraass [00 |_| memes tom aaaPanavecoo [|
[a _[unarss [0c [| meee von easpimtecoc [Oo _|
[casas | vasursass [© |_| weriad tom: soPunmecos [0 |
[awane | vores [x | [wt tan saaPimiecoc | wos |
7.3.6 Protection activation information (ACT)
This common data class shall be used for phase related status information like e.g. protection
activation information. Like much of the status information, it can be used for command
operations through GOOSE messages.
Table 21 shows all attributes of ACT.
Table 21 — Attributes of ACT
[ase [meme [re] ag [ semen mee men [re
‘operation or of a protection activation
(e.g. by the fault). jing on the
function, ‘general’ may or may not be
resulting from the phase attributes
(phsA\, ‘phsB', ‘phsC’, ‘neut}). Le,
‘generar may be set while none of the
‘phsX’/neut’ is. set, ‘general shall be
Set if one of the ‘phsX’’neut’ is set.
a a ad cre
or a start event of phase A.
Peer
or a start event of phase B.
Value true indicates a command, a trip
or a start event of phase C.
a
“a
https:/Awww.doc88.com/p-74754903218494.htm! 38/151
```


## File page 039

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
IEC 61850-7-3:2010+AMD1:2020 CSV -37-
© IEC 2020
‘mer | fee ome Fe | tannin me oct [Pees
Value true indicates @ start event with
earth current,
Quality of the values in ‘general’,
‘phsA’, ‘phsB’, ‘phsC’, ‘neuf.
Timestamp Timestamp of the last change of the
value in any of ‘general’, ‘phsA’, ‘phsB',
‘phsC, ‘neut' or 'q’.
Originator of a control action forwarded
by a GOOSE message,
NOTE This attribute may be used to
identity the originator when a data of
the ACT is used to perform an
operation. An example would be the
data object ‘CSWLOpOpn’ used to open
‘a breaker (XCBR) through a GOOSE
message. The LN XCBR receives
‘CSWI.OpOpn’ including the originator
as a GOOSE message. Once operated,
the new status information in
'XCBR.Pos' will include the originator
information it received as part of the
GOOSE message that triggered the
operation
operTmPhsA | Timestamp Operation time for phase A, used for
point on wave switching.
= eee
point on wave switching.
‘operTmPhsC | Timestamp Operation time for phase C, used for
point on wave switching,
[| vesineass [5c |_| riot tam: sarinewwcos —[O_|
a oo
[eanane | wasureass | © | | ema tom: BasePunewecoc 0 _|
[ene | vesteass |_| [ern an seinen | wan
7.3.7 Directional protection indication information (ACD)
This common data class shall be used to represent directional protection indication information.
Table 22 shows all attributes of ACD.
Table 22 — Attributes of ACD
ase [mem Je] | teen [rem
st General indication of a protection
activation (@.9. by the fault). Depending
on the function, ‘general’ may or may
not be resulting from the phase
attributes (phsA’, ‘phsB', ‘phsC’, ‘neut).
Le. ‘genera’ may be set while none of
the ‘phsX’/neut’ is set, ‘general’ shall
be set if one of the ‘phsX’'neut' is set.
a
nw
https://www.doc88.com/p-74754903218494.html 39/151
```


## File page 040

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
-38- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
a el Gd
FaultDirectionKind General direction of the fault. If the
faults of individual phases have
Gifferent directions, this attribute shall
be set to ‘dirGeneral="both’.
Valve true indicates a trip or a start AllOrNone
event of phase A. PerGroup
D
PhaseFaultDirectionki Direction of the fault for phase A. ‘AllOrNone
nd PerGroup(
1)
event of phase B. PerGroupi
2
itPhsB PhaseFaultDirectionki | ST Direction of the fault for phase B, AllOrNone
nd PerGroup
2)
st Value true indicates a trip or a start AllOrNone
event of phase C. PerGroupy
3
PhaseFaultDirectionki | ST Direction of the fault for phase C. ‘AllOrNone
vd PerGroup
3)
See 'ACT.neut, AllOrNone
PerGroup
4)
PhaseFaultDirectionki Direction of the fault for earth current. | AllOrNone.
nd PerGroup(
4)
st Quality of the values in ‘generar,
‘dirGeneral, ‘phsA’, ‘ditPhsA’, ‘phsB,
‘dirPhsB’, ‘phsC’, ‘dirPhsC’, ‘neu,
‘dirNeut.
Timestamp Timestamp of the last change of the
value in any of ‘general’, ‘dirGeneral’,
‘phsA’, ‘dirPhsA’, ‘phsB’, ‘dirPhsB',
‘phsC’, ‘dirPhsC’, ‘neut’, ‘ditNeut” or “q.
DataAttribute for configuration, description and extension
[=] [wears [60 |_| rete tor enahimtecoe [0 |
[oo | vnenass | 06 | [wort ton asoPinevecnc [|
a
[aman | vaserass [ex | [twit tan eaerniecoc | won|
73.8 Security violation counting (SEC)
This common data class shall be used to represent the security violation counter.
Table 23 shows all attributes of SEC.
a
nw
https://www.doc88.com/p-74754903218494.html 40/151
```


## File page 041

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
IEC 61850-7-3:2010+AMD1:2020 CSV -39-
© IEC 2020
Table 23 — Attributes of SEC
Pe [mem [eg | me [
[ee _[wray et | aa | Comer vate of roarty vais [Ww __|
[| seeping | st |_| Soy fe ot voter esc [|
[1 [tess | st | Teng oft ooe of oe [e
st ‘Address of the remote source that last
caused the count to be incremented.
NOTE The kind of address stored
(application address, IP address, link
address, etc.) is whatever the server
can detect and may depend on the
specific mapping,
‘Additional clarification on the last
detected violation.
DataAttribute for configuration, description and extension
[< | vaswreass [00 |_| eras tom: eaoPunmecos [0 __|
[> | Wrens | 56 | [rtrd ton aroinenecoc | o_|
[tare | vases |x| | mote tans asoPimnecoc —_[o_|
[ene | vesooass |_| [rer on asoinenecoc | Oana
7.39  <cstatistics>> Binary counter reading (BCR)
This common data class shall be used to represent binary counter reading.
Table 24 shows all attributes of BCR.
Table 24 — Attributes of BCR
Pas | mem [eg | terre [rem
Binary counter status represented as AllAtLeast
an integer value; wraps to 0 at the ‘OneGroup
maximum or minimum value of INT64. | (2)
Frozen binary counter status AllAiLeast
represented as an integer value. It gets | OneGroup
its value from ‘actVal at the time of (1)
freezing (time-stamped with ‘Tm’.
Freezing process is defined by
attributes “‘YrEna’, ‘strTm’ and ‘Pd.
rtm Time of the last counter freeze that led | AllAtLeast
to an update of the value in ‘trVal. ‘OneGroup
i]
Se
Timestamp Timestamp of the last change of value | AllAtLeast
in actVar or’g. ‘OneGroup
(2)
a
a
nw
https://www.doc88.com/p-74754903218494.html 41/151
```


## File page 042

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
-40- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
Paar [seem [e |g | come nme [roe
count. Used together with ‘actVal’ and
‘fal’ to caloulate the value. E.g. for
‘actVal': value = ‘actVal * pulsQty’ *
1exp(‘units.muttiplier) [units SiUnit]
Controls the freezing process. If true, | AllAtLeast
freezing shall occur as. specified in ‘OneGroup
‘strTm’, ‘YrPd? and ‘fs’; otherwise, no a)
freezing shall occur.
Timestamp Starting time of the freeze process. It
the current time is later than ‘strTm',
the first freeze shall occur at the
expiration of the next freeze interval
‘Pd, computed from ‘strTm’ setting
Time interval between freeze AlAtLeast
operations [ms]. If value is 0, only a ‘OneGroup
single freeze is performed at the time | (1)
indicated in ‘strTm'.
Wf true, the counter ‘actVal’ is to be AllAtLeast
automatically reset to zero after each ‘OneGroup:
freezing process (‘rVal’ is by definition (1)
Updated from ‘actVal and not reset).
[= | vasogsss [00 | | ees tom suaPinavecoe [Oo _|
[au [Uneowass | 0c | | emaa tom: BasePunewecoc [0 _|
[axare | wasingess [ex [| moms tons eaopimiecoe fo _|
[une | vasuneess [ex | | eet tom SaaPinavecoe | Maan
7.3.10 Histogram (HST)
This common data class shall be used to represent histograms. A histogram evaluates a series
of values and classifies them according to the configured range ‘hstRangeCliJ'. The evaluation
and classification result ‘hstVal[i]’ can typically be:
— acount (e.g., how many times voltage drop was in a certain range);
— a measurement of duration (e.g. for how long the temperature of transformer winding was
in a certain range), or
— the calculation of an average.
A histogram can be calculated based on a one-dimensional or a two-dimensional range.
a
“a
https:/Awww.doc88.com/p-74754903218494.htm! 42/151
```


## File page 043

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark Y Annotations ¥ Q
IEC 61850-7-3:2010+AMD1:2020 CSV -41-
© IEC 2020
Value (counts or other)
nstVak(O)
(1)
hstvaK3)
pial)
wWoits:
GY A
i j i i
ZEEE z veo ase
Figure 9 - One-dimensional histogram
Figure 9: This diagram illustrates usage of data attributes for a one-dimensional histogram
representation.
a
https:/mww.doc88.com/p-74754903218494.html 43/151
```


## File page 044

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
-42- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
10-
¢
$
Xons = ee 280
Index 0 1 2 3 4 5
hstval 0. 10 9 1 5 3
hstRangeC 0:0/4;10 4:0/10,4 10;0/12;4 4:4/12;8 4:8/8;10 8;8/12;10
Figure 10 - Two-dimensional histogram
Figure 10: This diagram shows an example of a two-dimensional histogram along with the
corresponding values for related data attributes. Each of the rectangles represents one range;
there is no rule on how to order the ranges.
Table 25 shows all attributes of HST.
Table 25 - Attributes of HST
ase [mem |e |g | mene [rem
ARRAY 0...maxPIs- dehg | Array of data values for the histogram
OF INT32. : entries.
dup
[fem | iy oa aes maT [|
Timestamp Timestamp of the last change or update
event of any of ‘hstVall! oF the last
change of the value in ‘q.
elements used in ‘hstVall],
‘nstRangeC.
= se [alana |
OF Cel the ranges for the histogram.
[ents [unt Yor [emg | unit of the ants,
a
nw
https://www.doc88.com/p-74754903218494.html 44/151
```


## File page 045

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q

IEC 61850-7-3:2010+AMD1:2020 CSV -43-

© IEC 2020

‘mer fue ome Fe | |_tuennin me omc [Pees

[yenis [unt for | cetg | unit ot the yan,

[re ere |
for “hstVall], "hstRangeCi)".

[a __| weswrss [00 | omcipion oe vate ote ems [w

a = ake
Unicode.

a oo

Sa ca al tea a
Unicode.

[=| wasingss [bo [| ees tan BaoPimnecoe [|

[a | vosowass [0c | | ert tom saPinaveco [|

a

[one | wasureass | | [ers ton: BasePinenecoo | wos

7.3.11 Visible string status (VSS)

This common data class shall be used to represent visible string status values.

Table 26 shows all attributes of VSS.

Table 26 — Attributes of VSS

Ps [mere [eg emer [rem

[vei | vasongass [st [aro | vote otteana sd

fe [oaty (St [wtp | Oty a te mn war

Oe Gl
value in any of ‘stVal or 'q.

[x [wasvss [60 | | moms ton sawpimiecoe [O_|

[a | unessass [0c | | motes vans saspimiecoc [Oo _|

[aetare | vesureass [x | | rte vans aserinanecoc [Oo _|

[om | wasuneass | © |_| eas tom: BasePommecoo | wasn |

7.3.12 Object reference status (ORS)

This common data class shall be used to hold the reference to an element the data is referring

to.

Table 27 shows all attributes of ORS.

ry
an
https://www.doc88.com/p-74754903218494.html 45/151
```


## File page 046

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
-44- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
Table 27 — Attributes of ORS
fase [mem |e] | teem meme [re
[ace lo Sle
referring to.
[aq | avaity St | cata | cuaty of the vane in iver, TM
OI i Gal he
the last change of value in ‘q’.
[=e | vasoneess [00 |_| ewes tom: eaPanavecos [|
[a _[unensass [0c [| menes von saopimiecoe [Oo _|
[canine | vasuness [ex | | hed tom SasPinavaco —_[o_|
[oe [wastes [x |_| meee tore aepimiecoc | woanats_|
7.313 Time value status (TCS)
This common data class shall be used to represent calculated time values.
Table 28 shows all attributes of TCS.
Table 28 — Attributes of TCS
fase [seme [re] ag [ semen [ro
Timestamp, dchg | Value of the calculated time.
upd
[a iererere
value in any of ‘stVal’ or ‘q!
[= [waswss [0c | | mone vor: saxpimiecoe [O_|
[ai _[urwomass [oc | [wr tan eusPintecoc [0 _|
[tans | veswezss [x |_| metas von: SPannecno | O
[suave | vasungsss [ex | | ee tom BasPinavecoe | MO
7.4 Measurand information
7.4.1 General
This subclause defines all the common data classes for measurands. Measurand information is
of type analogue or group of analogues and is information collected from the process or
produced by an application function. Measurand information has the functional constraint MX.
Measurand information cannot be written, but could be substituted.
For applicable services, see Annex B.
a
“a
https:/Awww.doc88.com/p-74754903218494.htm! 46/151
```


## File page 047

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
IEC 61850-7-3:2010+AMD1:2020 CSV - 45-
© IEC 2020
[:tase-coCAratowsetafe-t/
CoreAbstractC0Cs::
+: Unicode255.06 (1)
: isering255_00 10.1)
+ dacads: Visering?55,210-11
[ncn |
Modaeanal
CoraabrractCOCs::
> seb: Quality SV i011
+ sob: Vinsring64.SV 19.11
+ _biksna: BOOLEAN. 80-1}
. 5
+ Val Vector. écha. dod = mag: AnalogeeValve NOC dcha.dupd +e Oualiy Macha
+ range: Range OK dehg 0.1) + range: Range. Nox deg 1.1 +E Tis ND.
+ rangeang Range AIC deg [0.1) + Quabey Acacha wets: Vet CF. dehg 11)
+ Quality. gchp + Temestamp. 80k + SVC: SealedvalueContig.CF debs 1.11
+E Tmascame 6 = subhlag:Analopuavalve 5¥ (0.11 = in: AnaogueValeCFdchg [0-1]
+ sebCvab Vector-5¥ 19.1 its: Unit cFchy 11 + _ax: AnalogueVe.€F-dehg 1.11
+ is: Ute [1 + db; nT22U.CF dchg [11 = 0.100000
+ do: WeT32U.CF.dehg 10.1 = 0.100000 + nero 7820. dchg 10.11» 0.100000
{Eee lem | f Sezai] So |
+ zero: n732U.CF.dehg 10.11 0.100000 | |= range: RangeConfg.F. ch 10.1]
+ range: RangaCenfig.CF-dcg 1.1) > spate: NTB2U_CF-dehg 8-1)
+ rangeange: RangeConig CF dehg [2-1] + dbhef: ROATS2.CF.dchg 10-1)
+ tmagiVC:SeledValeeCinfgF-aehg 8.11 + tarsal FLOATIRACF deh 1.11
1 Sngsve-sealedvaluecontigCFdehg [11
+ angle PhaveAnglaferenc. Fea 11
> mptae: 732U.CF-dehg (0-1) Laeiraeed
+ eet: FLOATI2_CF debe 10.11 [sina A
+ areOliel: FLOATS2.CF dc [0-1] —
+ Sedna: FLOKT22.CF deb 101} bees
rsa
(aotrange
Duorrangedng
inrscaldnagv
nvscalndangv
wou
uoizere0bi
Moidbangh
Figure 11 - Class diagram CDCAnalogueinfo::CDCAnalogueinfo-1
Figure 11: This diagram shows all measurand info primitive CDCs defined in the standard with
‘supertypes that factor their common attributes.
a
nw
https://www.doc88.com/p-74754903218494.html 47/151
```


## File page 048

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark ¥ Annotations v Q
-46- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
[Hesecicingeanent )
BaseComposedCDC
+ dU; Unicode2s5_0C [9.11
+ cdeName: Vistring?S5.£% [0.1]
+ datas: VisString2$5.6¢10..1)
HarmonichbeasurandCOC
[| __ orn
+ mumCye: INTI6U.CF dchg
+ evalTm: INTISU_CF_dchg
+ phak: CMV (0.11 + umpRate:NT32U_CF_dehg 10.1]
> phs®: CMV0.11 frequency: FLOATE2_CFdechg
+ phsc: mv [0.11 + hve Hvteferance.CF deg [0.11
+ neut CMY [0.11 + rmaCye: INTI6U.CF_dchg 10.1]
+ net: MV (0.1) + _maxPts:INTIGU_CF deh
+ ras CMV (0.11
—
+ _phsToNeut: BOOLEAN.CF.dchg [0-1]
statistics
+ phaBC: CMY [0.1] > c
2 ca ew 2 Poorer cv 0-7
+ _Anael PhaseAnalaReference.CF.dcha [0.11 > phsCHar: CMV [0°]
+ neuthiar: CMV [0."]
+ reshar: CMV 10.71
= _anghat:PrazeAngleRaference.CF_dchg 10.1]
> acu [a
+ acu > phaABHar MVD]
+ eeu + phsteHar: CMV [0.“]
i ceo + phacAHar: CMV [0."]
constraints
Figure 12 - Class diagram CDCAnalogueinfo::CDCAnalogueinfo-2
Figure 12: This diagram shows all measurand info composed CDCs defined in the standard with
supertypes that factor their common attributes.
7.4.2 <<abstract,statistics>> Common harmonic measurand information
(HarmonicMeasurandCDC)
Abstract type, holding configuration attributes common to common data classes that provide
harmonic and interharmonic measurand information. Attributes are used to configure arrays of
harmonics and interharmonic in concrete CDCs {‘har{]} in HMV, {‘phsAHar{]’, ‘phsBHarlJ’...} in
HWYE, and {‘phsABHarl’, ‘phsBCHariJ’..} in HDEL.
a
nw
https://www.doc88.com/p-74754903218494.html 48/151
```


## File page 049

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
IEC 61850-7-3:2010+AMD1:2020 CSV -47-
© IEC 2020
Se |
cy
——
[4
—
—
—
—
i
[io]
ai
pi]
rs]
a
rs
[eT
[iy
Ts
fis]
{20
foc
[ harmonies
[Tinterharmonies
Figure 13 — Array indexing (‘har'/*Har’) based on ‘numCyc’
Figure 13: This diagram illustrates the relationship of attributes ‘har'/*Har’ and ‘numCyc’.
The first array element always contains the de component.
If (‘numCyc’ is 1), the array (e.g., ‘har[1...numHar}’) contains the value of the harmonics.
If (‘numCyc’ is greater than one), the array (e.g., ‘har[1...numHar]) contains the value of the
harmonics, and interharmonics only. In that case, index = 0 (mod ‘numCyc’) contains the value
of the harmonics. Other indexes contain the value of the interharmonics.
Table 29 shows all attributes of HarmonicMeasurandCDC.
a
“a
https:/Awww.doc88.com/p-74754903218494.htm! 49/151
```


## File page 050

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
-48- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
Table 29 — Attributes of HarmonicMeasurandCDC
= cE ciel
(range=[1...maxPts-1]) Number of
harmonic, and interharmonic values
that are valid. Array index 0 refers to
the do component. therefore ‘numHar’ >
0. The maximum value can be
calculated as follows:
‘numHar <= 1 + 1/2 * (‘smpRate’*
‘numOyc}).
numeyc INTIEU cr Number of cycles of power frequency,
which are used for harmonic,
subharmonic and interharmonic
calculation.
a lal ==
interharmonic calculations.
Determines the highest possible
harmonic or interharmonic detectable,
according to the sampling theorem; the
minimum value is ‘smpRate’ = 2 *
‘frequency’. ‘smpRate’ shall be the
number of samples per nominal period.
In the case of a de system, it shall be
the number of samples per second.
FLOAT32 oF Nominal frequency of the power system
‘or some other fundamental frequency
{He}.
twRef HvReferenceKind Indicates how the magnitude values of
the harmonics ((CMV[i}.instCVal.mag’)
are provided.
rmsCyc INTI6U Number of cycles of power frequency
used for the calculation of ms values.
INTIEU Maximum supported size for
"HMV.har[)’, "HWYE.{phsAHar,
phsBHar, phsCHar, neutHar, netHar,
resHar)’ and 'HOEL (phsABHar,
phsBCHar, phsCAHar)’
NOTE For backwards compatibility
reasons, if ‘maxPts’ is missing in legacy
devices that did conform to an
implementation of a previous name
space, the resulting value of ‘maxPts’ is
‘numHar’.
[s__|waswsss [bo |_| meres vane Sestonponccos [|
[so | vocomass [0c |_| meres tom: Saeconpeseococ [|
[axare | waswss [ex [| mone von Basconpsnococ [Oo _|
[aman | vasurass [ex | [ra tan awcanpsnacoc | wean |
74.3 <<statistics>> Measured value (MV)
This common data class shall be used to represent measured values.
a
“a
https:/Awww.doc88.com/p-74754903218494.htm! 50/151
```


## File page 051

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark ¥ Annotations v Q
IEC 61850-7-3:2010+AMD1:2020 CSV -49-
© IEC 2020
inst ag
mag
db
sec assoro
Figure 14 - Deadbanded value
Figure 14: This diagram illustrates the relationship of attributes ‘instMag’, ‘mag’ and ‘db’ (used
for deadband calculation).
NOTE The figure above is an example. There can be other algorithms providing a comparable result; for example
as an alternative solution, the deadband calculation can use the integral of the change of ‘instMag’. The algorithm
used is a local issue
Zero forcing
a
instMag mag
Figure 15 — Zero deadband
Figure 15: This diagram illustrates the relationship of attributes ‘instMag', ‘mag’ and ‘zeroDb’
(used for zero forcing).
Table 30 shows all attributes of MV.
a
https:/mww.doc88.com/p-74754903218494.html 51/151
```


## File page 052

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
-50- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
Table 30 — Attributes of MV
\_Sroor | Mums ore || oy |_teemte mn cece | ri
Instantaneous value of the magnitude.
NOTE ‘instMag’ is optional from the
Perspective of the visibility of that valve
to the communication. The
instantaneous value may be required
for the internal behaviour of the
function, @.g. to perform the deadband
calculation for ‘magi.
chg | Value of the magnitude based on a
. deadband calculation from the
dupd | instantaneous value ‘insiMag’. The
value of ‘mag’ shall be updated to the
current instantaneous value ‘instMag’
when the value has changed according
to the configuration parameter ‘db’. If
‘db'=0, 'mag'=instMag’.
NOTE 1 This value is typically used to
create reports for analogue valves,
Such a report sent "by exception” is not
comparable to the transfer of sampled
measured values as supported by the
CDC SAY.
NOTE 2 This ‘mag’ is not the same as
‘mag’ of the constructed attribute class
"Vector.
Range in which the current
instantaneous value ‘instMag’ is. A
transition of ‘instMag’ to another range
generates a change in this attribute that
may be used to trigger a report with
trigger option ‘data-change’ (see
RangeConfig). ‘instMag’ can be a local
value, ie. does not need to be visible
over the communication for
implementing the range attribute.
NOTE The use of algorithms to fiter
events based on transition from one
range to another is a local issue.
Depending on the update rate of the
‘instMag’, a fast change of the vaive
may result in non-consecutive values of
this ‘range’; e.g. ‘range’ can be
reported as ‘low-low’ and ‘high-high’ in
two consecutive updates.
Quality of the values in ‘instMag’, ‘mag’,
‘range’.
Timestamp of the last refresh of the
value in ‘mag’ or of the last change of
the valve in any of ‘range’ or 'q.
[astm [eocueas [ev | [meres ton: suaerenc | wraet_|
[sein | Anionavaie | SV |_| Vabe weed o iat etags | Wat |
[as [ aay [sv | | mes on: Susmercnc | wrswat|
[sa30 | wasowoss [|_| wn Yon Sibinsore0e | wat |
[wera | pootean | at _ | inhorted trom: Substnutoncoc jo
a
“a
https:/Awww.doc88.com/p-74754903218494.htm! 52/151
```


## File page 053

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88

< 150 > @ Q_ View A mark ¥ Annotations» Q

IEC 61850-7-3:2010+AMD1:2020 CSV -51-
© IEC 2020
[se [seme [re [ig | tome meme [rem
DataAttribute for configuration, description and extension
‘Common Unit for: instNtag’, ‘mag’
‘subMag’, ‘rangeC’, ‘dbRef.

INT32U (range=[0...100000}) Deadband is a
configuration parameter used to
calculate deadbanded value ‘mag’. The
value of ‘db’ shail represent the
percentage of ‘dbRef’ in units of
0.001 %. Theretore, ‘db’ = (0...100000},
corresponding to [0 %...100 %J,
respectively. If an integral calculation is
used to determine the deadbanded
value, the value of ‘db’ shall be
represented as 0.001 %s.

With a ‘db’ = 0 the attribute ‘mag’
follows the instantaneous value.
Ht ‘ob’ is: not present in the model, then
the deadband calculation is a local
issue.

zeroDb INT32U (range=[0...100000]) Configuration
parameter used to calculate the range
around zero, where the deadbanded
vaiue ‘mag’ will be forced to zero. The
value of ‘zeroDb’ shall represent the
percentage of ‘zeroDbRef in units of
0.001 %. Therefore, 'zeroDb' =
{0...100000}, corresponding to
[0 %...100 %}, respectively.

svc ‘ScaledValueContig Configuration for scaled value
representation (instMag’, ‘mag’,
‘subMag’, ‘rangeC’).

[race | Rarecantg | OF |e | Contraton tor "aoe [woven

INT32U Number of samples per second that has
been used to determine instantaneous.
value ‘instMag’, In the case of an ac
system, it is number of samples per
nominal period.

FLOATS2 Deadband reference used for the
calculation of deadband.

A value of 0 means that the value ‘db’
shall be used as the percentage of the
last transmitted value in units of
0.001 %.
A value > 0 means that the value ‘db’
shall represent the percentage of the
deadband reference (dbRef) in units of
0.001 %
‘doRef’ will normally not be changed
when a system is in operation; if
adjustments of the deadbanding are
needed during the operation, the
attribute ‘db’ shall be changed.
zeroDbRet Zero deadband reference used for the MOj\zeroD:
zero deadband calculation. b)
A value of 0 is not allowed.
A value > 0 means that the value
‘zeroDb’ shall represent the percentage
of the zero deadband reference
(zeroDbRet’) in units of 0.001 %.
‘zeroDbRef' will normally not be:
changed when a system is in operation;
Fy
Aa
https://www.doc88.com/p-74754903218494.html 53/151
```


## File page 054

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
-52- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
fase [seem [ef | come meme [roe
W adjusiments of the deadbanding are
needed during the operation, the
attribute ‘zeroDb’ shall be changed.
[= | wasnngass [06 | | meet tom: sasPinaveco [|
[ao [oneness [0c [ | moos von saopimieco [|
[casas | vasureass [ex | [eta tom: asoPinecoc [0 |
[ane | vows [x | [tas tan saaPimiecoc | wos |
744 <<statistics>> Complex measured value (CMV)
This common data class shall be used to represent complex measured values.
Table 31 shows all attributes of CMV.
Table 31 — Attributes of CMV
= cea] il
a al ==
'MV.instMag’ for details.
Vector ‘dchg | Complex value based on a deadband
: calculation from the instantaneous
dupd | value ‘instCVal.mag’. The deadband
calculation is done both on
‘instCVal.mag’ (based on ‘db’) and on
‘instCVal.ang’ (based on ‘dbAng’),
independently. See ‘MV.mag’.
RangeKind Range in which the current
instantaneous value ‘instCVal.mag’ is.
‘See 'MV.range’.
Range in which the current
instantaneous angle ‘instCVal.ang’ is.
See MV.range’.
Gaal al a =
‘cVal’, ‘range’, ‘rangeAng’.
Time: Timestamp of the last refresh of the
value in ‘cVal or of the last change of
the value in any of ‘range’, ‘rangeAng’
or'g.
[seem [eoocem [sv |_| meas tor: Susmscncoc | wranst |
[aecve [vec [sv |_| Ve ures © wtsete wacvor | rast |
[seo [omy (sv |_| eas tom: Smmmisncoc | Maat _ |
[sei | vases [sv_| | ted tom Siboncoc ‘| rat
[em [cote [a | | heed tow: Siiinamncoc ‘fo
Common Unit for: instCVal.mag’,
‘cVal.mag', ‘subCVal. mag’, 'rangeC’,
‘dbRef.
a
“a
https:/Awww.doc88.com/p-74754903218494.htm! 54/151
```


## File page 055

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark Y Annotations ¥ Q
IEC 61850-7-3:2010+AMD1:2020 CSV -53-
© IEC 2020
ee ee
(range=[0...100000]) Deadband is a
configuration parameter used to
calculate deadbanded value ‘cVal_mag’.
‘See 'MV.db’.
INT32U (range=[0...100000]) Deadband is a
Configuration parameter used to
calculate deadbanded angle of a
complex value (cVal.ang)). The value
of ‘dbAng’ shall represent the
percentage of ‘dbAngRef' in units of
0.001 %. See 'MV.cb’.
zeroDb (range=[0...100000]) Configuration
Parameter used to calculate the range
‘around zero, where the deadbanded
value ‘cVal.mag’ will be forced to zero.
‘See 'MV.zeroDb'.
| vangec | Rangecontg | GF | cera | Configuration for ange | Movange) |
Gl ee lal enc l
magSVvC ‘ScaledValueConfig Configuration for scaled value MFscaled
representation of magnitudes Mag
‘instCVal.mag’, ‘cVal.mag’,
‘subCVal.mag’, ‘rangeC’.
‘angSvC. ‘ScaledValueConfig Configuration for scaled value
representation of angles ‘instCVal.ang’,
‘cValang’, ‘subCVal.ang’, ‘rangeAngC’.
PhaseAngleReterence Angle reference, indicating the quantity
King that is used as reference for the phase
angles ‘cVal.ang’, ‘instCVal.ang’,
‘subCVal.ang’, or that the values are
‘synchrophasors. For the indicated
quantity, the fundamental frequency
(index = 1)is used as reference by
convention.
Number of samples per second that has
been used to determine instantaneous
value ‘instCVal.mag’.In the case of an
‘ac system, it is number of samples per
nominal period.
FLOAT32 Deadband reference used for the
calculation of deadband. See
‘MV.dbRef.
zeroDbRet Zero deadband reference used for the. MOj\zeroD:
zero deadband calculation. See b)
"MV.zeroDbRef.
dbAngRet Angle deadband reference used for the | MO(dbAng
calculation of deadband. ’
A value of 0 means that the value
‘dbAng’ shall be used as the
percentage of the last transmitted value
in units of 0.001 %,
A value > 0 means that the value
‘dbAng’ shall represent the percentage
‘of the angle deadband reference
(@bAngRef) in units of 0.001 %.
‘dbAngRef will normally not be
changed when a system is in operation;
if adjustments of the deadbanding are
needed during the operation, the
attribute ‘dbAng’ shall be changed.
[s___[Weswreass [00 | [moras tom: SasoPinwecoc [0 |
Py
a
https:/mww.doc88.com/p-74754903218494.html 55/151
```


## File page 056

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark Y Annotations ¥ Q
-54- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
fase [soem Te] | come meme [roe
[canine | vasungess [ex | | ees tom BaaPinavecoe [0 _|
[smne | wasureass | | | ems tom: BasePunenecoc | moan
7.4.5  <<statistics>> Sampled value (SAV)
This common data class shall be used to represent samples of instantaneous analogue values.
The values are usually transmitted using the “transmission of sampled value model” as defined
in IEC 61850-7-2.
Table 32 shows all attributes of SAV.
Table 32 — Attributes of SAV
Fase [memo [rm] ag | soem [ron
[rans [Asooevaue [sx |_| Magne of te hrannis aime [|
[a [eamty id | ae | ay oe va nag [|
value in ‘instMag’ or of the last change
of the value in 'q’
DataAttribute for configuration, description and extension
ee
SS he ld === al
representation (‘instMag’, ‘min’, ‘max’. AV
which values of ‘instMag.i’ or ‘instMag.f
are considered within process limits,
See ‘RangeContig.min’.
which values of ‘instMagi’ or ‘instMag
are considered within process limits.
See 'RangeContig.max.
ee
[a [ oneness [bo [| os tons saorimtecoc [|
[eaname | vasureass | © |_| era tom: BasePinmwecoc [0 _|
[sane | vases [ex |_| ering tom: BaePinavecoe | MO |
7.46  <cstatistics>> Phase to ground/neutral related measured values of a three-
phase system (WYE)
This common data class is a collection of simultaneous measurements of values in a three-
phase system that represent phase to ground values.
Values for ‘phsA’, ‘phsB’, 'phsC’, ‘neut’, ‘net’ and ‘res’ have been simultaneously acquired or
determined, and their respective time stamps hold the same value 't.. It shall be assumed that
any jitter between the acquisition times dedicated for these values is neglectable. The jitter for
a
“a
https:/Awww.doc88.com/p-74754903218494.htm! 56/151
```


## File page 057

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark ¥ Annotations v Q
IEC 61850-7-3:2010+AMD1:2020 CSV -55-
© IEC 2020
simultaneity shall be as indicated in any of the respective ‘ttimeQuality’ attributes of the above
values.
The actualization of one of the component due to a dead band calculation results in the
actualization of all other components and a restart of the dead band calculation for all
components.
The transmission of the values in a report follows the Trigger Options definitions - ie.
component with Trigger Option:
-  dChg will only be included in the data-change Reports if it has really changed,
= dUpd will be included in the data-update Reports in any case.
Attribute ‘angRef' is used in place of individual ‘angRef’ attributes of ‘phsA’, ‘phsB', ‘phsC’,
“neut', ‘net’ and ‘res’.
9 LN LN A
vos ow Ef \ Tl
a
nae -~D J
bres
+> ‘neut
Traut WY
Ground Inet
ec 255870
Figure 16 — Relation between phase values
Figure 16: This diagram illustrates relation between the phase values and neutral, net and
residual.
Table 33 shows all attributes of WYE.
a
“a
https:/Awww.doc88.com/p-74754903218494.htm! 57/151
```


## File page 058

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
-56- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
Table 33 — Attributes of WYE
ee GE Rie
‘AtLeastOn
(1)
NC La sl
Value of the measured phase neutral. if | AtLeastOn
a direct measurement of this value is | e(1)
not available, it is acceptable to
substitute an estimate computed by
creating the algebraic sum of the
instantaneous values of currents
flowing through all live conductors
(’phs A instC Val'+ phsB.instCVal'+'phsC.
instCVar); in that case, ‘neut='res’
Net current, as the algebraic sum of the | AtLeastOn
instantaneous values of currents ett)
flowing through all live conductors. and
the neutral of a circuit at one point of
the electrical installation
((phsA instC Val's"phsB.instCVal's‘phsC.
InstCVar+‘neut instvar).
Residual current, as the algebraic sum | AtLeastOn
of the instantaneous values of currents | e(1)
flowing through all live conductors of a
circuit at one point of the electrical
installation
(‘phsA jnstCVal+'phsB.JnstCValsphsC.
instCvar).
PhaseAngleReference ‘Angle reference, indicating the quantity
King that is used as reference for the
respective phase angle (@.9.,
‘phsA instCVal.ang’), or that the values
are synchrophasors; used instead of
their own ‘angRet. For the indicated
‘quantity, the fundamental frequency
(index = 1) is used as reference by
Convention.
phsToNeut True indicates that this WYE instance is
used for phase to neutral values
instead of phase to ground values
(neut’ always indicates the neutral to
ground value).
a
[au | Uneouass [0c | | ema tom: Baseconpusescoc [0 _|
[senare | vastness | © | | heed tor: aseconesnocnc [oO _|
[ene | vases [| [mo tan aseconpencnc | woanan
7.4.7 <<statistics>> Phase to phase related measured values of a three-phase system
(DEL)
This common data class is a collection of measurements of values in a three-phase system that
represents phase-to-phase values.
a
nw
https://www.doc88.com/p-74754903218494.html 58/151
```


## File page 059

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
IEC 61850-7-3:2010+AMD1:2020 CSV -57-
© IEC 2020
Values for 'phsAB', 'phsBC' and 'phsCA' have been simultaneously acquired or determined, and
their respective time stamps hold the same value "t’. It shall be assumed that any jitter between
the acquisition times dedicated for these values is neglectable. The jitter for simultaneity shall
be as indicated in any of the respective ‘ttimeQuality’ attributes of the above values.
The actualization of one of the component due to a dead band calculation results in the
actualization of all other components and a restart of the dead band calculation for all
components.
The transmission of the values in a report follows the Trigger Options definitions - i.e.
component with Trigger Option:
= dChg will only be included in the data-change Reports if it has really changed,
—  dUpd will be included in the data-update Reports in any case.
Attribute ‘angRef’ is used in place of individual ‘angRef attributes of ‘phsAB’, ‘phsBC’ and
‘phsCA'.
Table 34 shows all attributes of DEL.
Table 34 — Attributes of DEL
Pe [mem [| cme [rm
an nn == cal
vent. eit)
B= cel
measurement. et)
ee
measurement.
angRef PhaseAngleReterence | CF ‘Angle reference, indicating the quantity
Kind that is used as'reference for the
respective phase angle (e.g.
'phsAB.instCVal.ang’), or that the
values are synchrophasors; used
instead of their own ‘angRet. For the
indicated quantity, the fundamental
frequency (index = 1) is used as
relerence by convention,
a
[oo | Wnseass [06 | [motes tan Basecanpaescoc fo _|
[satan | vases | €x_| | more vom eancanpseeacoe _[o_|
[aman [vasurass [ex | [rome tor Saconpsecoc | mosane_|
7.4.8 <cstatistics>> Sequence (SEQ)
This common data class is a collection of sequence components of a value.
Values for ‘cl’, ‘c2' and ‘c3' have been simultaneously calculated, and their respective time
stamps hold the same value 't.
a
nw
https://www.doc88.com/p-74754903218494.html 59/151
```


## File page 060

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
-58- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
The actualization of one of the component due to a dead band calculation results in the
actualization of all other components and a restart of the dead band calculation for all
components.
The transmission of the values in a report follows the Trigger Options definitions - i.e.
component with Trigger Option:
- dChg will only be included in the data-change Reports if it has really changed,
—  dUpd will be included in the data-update Reports in any case.
Table 35 shows all attributes of SEQ.
Table 35 — Attributes of SEQ
Smet | Mmew ere |p _mein  onceen pee
Positive (if ‘seqT'='pos-neg-zero’) or
Girect (it ‘seqT’='dir-quad-zero’)
sequence component.
quadratic (if ‘seqT'='dir-quad-zero’)
Sequence component.
a
oe eee |
components ‘ot’, ‘c2" and ‘cS’.
PhaseRelerenceKind The phase that has been used as
reference for the transformation of
phase values to sequence values.
[=| wesgass [56 | oe ta: asecarpmescos fo —_|
[ai | wisoass [oc | tes tan saconpneenc [0 |
[stare | veswazss [© | | motes tan easecorpnescoc fo —_|
[aman | vasunass [ex | [woes ton saconpsecoc | won|
7.4.9  <<statistics>> Harmonic value (HMV)
This common data class is a collection of non-phase-related values that represent the harmonic
and sub-harmonic or interharmonic content of a process value.
NOTE Harmonics for a single circuit may have phase angles, but need no reference for the angle (angRef), since
by convention, the reference is always the fundamental frequency (index 1).
Table 36 shows all attributes of HMV.
a
nw
https://www.doc88.com/p-74754903218494.html 60/151
```


## File page 061

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
IEC 61850-7-3:2010+AMD1:2020 CSV -59-
© IEC 2020
Table 36 — Attributes of HMV
= eI eG
ARRAY 0...maxPIs-t ‘Array of harmonic and subharmonic, or
OF CMV the interharmonic values (see
HarmonicMeasurandCDC).
DataAttribute for configuration, description and extension
INTI6U inherited from:
HarmonicMeasurandCDC
numCye INTIEU inherited from:
HarmonicMeasurandCDG
evalTm INTI6U inherited from:

HarmonicMeasurandCDC

inherited from:

HarmonicMeasurandCDC

FLOAT32 inherited from:

HarmonicMeasurandCDC

hvRet HvReferenceKind inherited from:

HarmonicMeasurandCDC

mmsCyc INTI6U inherited from:

HarmonicMeasurandCDC

INTI6U inherited from:

HarmonicMeasurandCDC
Cc
[au | Uneoseass | 00 |_| ies tom: Baseconpusescoc 0 _—|
[cate | vasungess [ex | | ed tom Basconpeseacoc |_|
[enone [vases [ex | [wr tan awcanpenacoe | wena |
7.4.10 <cstatistics>> Harmonic value for WYE (HWYE)

This common data class is a collection of simultaneous measurements (or evaluations) of
values that represent the harmonic and sub-harmonic or interharmonic content of a process
value in a three-phase system with phase to ground values.
Table 37 shows all attributes of HWYE.
Table 37 — Attributes of HWYE
Pas [em [eg] term me [rom
ARRAY 0...maxPts-1 Array of harmonic and subharmonics,
OF CMV or interharmonic values related to
phase A.
phsBHar ARRAY 0...maxPts-1 Array of harmonic and subharmonics,
OF CMV or intetharmonic values related to
phase B.
‘ARRAY O...maxPs-1 Array of harmonic and subharmonics,
OF CMV or intetharmonic values related to
phase C.
a
“a
https:/Awww.doc88.com/p-74754903218494.htm! 61/151
```


## File page 062

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
-60- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
ea ta Gd
ARRAY 0...maxPts-1 ‘Array of harmonic and subharmonics,
OF CMV or interharmonic values related to
neutral.
ARRAY 0..maxPIs-1 Array of harmonic and subharmonics,
OF CMV or interharmonic values related to net
current
‘ARRAY 0...maxPIs-t Array of harmonic and subharmonics,
OF CMV or interharmonic values related to
residual current,
a ca lal ="
HarmonicMeasurandCDC
numeyc INTIEU CF inherited from:
HarmonicMeasurandCDC
evalTm INTIBU inherited from:
HarmonicMeasurandCDC
PhaseAngleReference ‘Angle reference, indicating the quantity
Kind that is used as'reference for the
respective phase angle (e.9.,
‘phsAHarfi].ang’), or that the values are
synchrophasors; used instead of their
own ‘angRef. For the indicated
quantity, the fundamental frequency
(index = 1)is used as reference by
convention.
INT32U inherited from:
HarmonicMeasurandCDC
frequency inherited from:
HarmonicMeasurandCDG
inherited. from:
HarmonicMeasurandCDC
INTIEU. inherited from:
HarmonicMeasurandCDC
INTIEU inherited from:
HarmonicMeasurandCDC
[| waswreass [66 | [ert tom: asoconpusstne [0 |
[ai | visas [oc | | tas ta susconpnscsc [0 |
[senare | wastes | © | | ee tom seGonpoeacsc fo _|
[xe | veseass | | [ror on aeeonpencnc | woanan
7.4.11 <<statistics>> Harmonic value for DEL (HDEL)
This common data class is a collection of simultaneous measurements (or evaluations) of
values that represent the harmonic and sub-harmonic or interharmonic content of a process
value in a three phase-system with phase to phase values.
Table 38 shows all attributes of HDEL.
a
nw
https://www.doc88.com/p-74754903218494.html 62/151
```


## File page 063

```
9/18/26, 9:26 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
IEC 61850-7-3:2010+AMD1:2020 CSV -61-
© IEC 2020
Table 38 — Attributes of HDEL
ee dE le
ARRAY 0...maxPts-1 ‘Array of harmonic and subharmonics,
OF CMV or intetharmonic values related to
phase A to phase B.
‘ARRAY 0...maxPts-1 ‘Array of harmonic and subharmonics,
OF CMV + interharmonic values related to
Phase B to phase C.
ARRAY 0...maxPIs-1 ‘Array of harmonic and subharmonics,
OF CMV + interharmonic values related to
phase C to phase A
DataAttribute for configuration, description and extension
INTIEU inherited from:
HarmonicMeasurandCDC
numCyc INTIEU. inherited from:
HarmonicMeasurandCDC
evalTm INTIEU inherited from:
HarmonicMeasurandCDG
PhaseAngleReference ‘Angle reference, indicating the quantity
King that is used as reference for the
respective phase angle (e.9.,
‘phsABHari.ang’), or that the values
are synchrophasors; used instead of
their own ‘angRef. For the indicated
‘quantity, the fundamental frequency
(index = 1) is used as reference by
convention.
INT32U inherited from:
HarmonicMeasurandCDC
FLOATS2 inherited. from:
HarmonicMeasurandCOC
HvReferenceKing inherited from:
HarmonicMeasurandCDC
mmsCyc INTIEU. oF inherited from:
HarmonicMeasurandCDC
INTIBU inherited from:
HarmonicMeasurandCDC
[=| wesreass [66 | [rors vans areconpenescoo [|
a
[sare | vases |x| [mot tans Beconpenecoe [Oo _|
[ene | veseeass [| [teria ons BaseConpenecnc | MORN
7.5 Controls
7.5.1 General
This subclause defines all the common data classes for controls. Objects that are controlled
can be of any data type. The common data classes for controls include both the control and the
related status (functional constraint ST) or measurand (functional constraint MX) information.
Controls are used as part of the operation of the system. We may differentiate between the
following kinds of controls:
a
nw
https://www.doc88.com/p-74754903218494.html 63/151
```


## File page 064

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark ¥ Annotations v Q
-62- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
- controls used to operate equipment like switches, tap changers or gates (typically DPC,
BAC, APC, BSC, ISC);
- controls used to change the behavior of the automation system like set a group of functions
into test mode (typically ENC, INC, SPC);
— controls used to change setpoints like a voltage setpoint. (typically APC);
= controls used to activate or deactivate a function (for a transient object, no deactivation is
required) (typically SPC).
The common data class APC is used for both setpoints as well as to operate equipment with an
analogue control interface like a gate of a hydro system. The semantics of the measurand within
the CDC APC depends on the usage:
- if APC is used to operate an equipment, the measurand information is the process value
from the operated equipment (e.g. the actual position of the gate);
— if APC is used for setpoints, the measurand information is the value of the setpoint that is
applied.
For applicable services, see Annex B.
NOTE 1 The service parameter of the control, which belongs to the control model defined in IEC 61850-7-2, is
included here, since the type is defined by the COC.
NOTE 2 Although all control CDCs have a number of common attributes, they have different presence conditions
or functional constraints or order, and are therefore not defined within the common abstract class.
Some concrete controllable common data classes may be used in the context of a derived
Statistical logical node, in which case their attribute ‘ctlModel’ shall have value ‘status-only’.
a
nw
https://www.doc88.com/p-74754903218494.html 64/151
```


## File page 065

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark ¥ Annotations v | Q
IEC 61850-7-3:2010+AMD1:2020 CSV -63-
© IEC 2020
CoreabrtractCOCs:: CoreabseractCOCs::
+ 4: Uncode?$5.0€ 18.11 > ba: ual 5¥ 0-11
+ edovame Vistrng255.0¢10-1] > seb: VistringS4 SV
> daca: Vsering? 68.08 10.1b bike: BOOLEAN. S11)
> Sox e1
2 fOp0k:Foestamp.OR 0.1
+ cin erwusT 16-1) > cine rau STD.
+ stVak: BOOLEAN.ST.dehg 10.1] + stVal: EmemDAST_dchg
+ © Quality sT-acha 0.11 + Qealy.staache
> Timestamp. st 10.1) > Teena SF
+ sind: BOOLIAN Sd 1.11 + ssld: BOOLEAN. ST. eh 0-11
+ futvat: 2oouEAN.S¥ 0.11 + bal trum 5V%0.11
+ pulaeConfig:PleeConfig. CF. Seba 0.1) 2 inode: cmos Fd
Gino Cuntodl c.achg + thetimeout:IT320.CF-dehg 1.11
+ Shotimeout NT32U.CF dca 0-1) > thectane:sbeclas.cF deh 0-1) = perate-nce
+ sbeclans Sboclass.CFdchg 1 = eparatronce + opertimeou: NTH2U.CF cha 0-11
> cpertimeout NT32U,CF. deh 1-1] + aval ona
+ civak BOOUAN = |
Ee
raves
‘uallorNorePerrou
‘morte’
‘Moucbarced
Twp Organs > Seeman
+ in: THU.ST ED.) + sevak nrs2.st.deha. sud
+ svat DpStata, ST. deb + Goalny Stace
2 SS Sache 2 Enmestame 5
+ sel: SOOUAN.ST. eho 1.11 2 sah eee
+ Nabvat: Dpsuns_5V 0.11 iMod: CiMode Fateh
? oe > ete F220. CF- dehy [11
2 erimeoue: W220, CF. dha 111 > alabes Boma cedsie al a
Hq sboClans Leeteprey (+ maxVal INTI2.CFdchg (01)
t oo 2 Seen be ee
— =a —
— 2 ivar rae
uorte!
—
ast!
| Mowrhanced!
Figure 17 — Class diagram CDCControl::CDCControl-1
Figure 17: This diagram shows first part of controllable CDCs defined in the standard with
supertypes that factor their common attributes.
°
an
https://www.doc88.com/p-74754903218494.html 65/151
```


## File page 066

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 7150 > QQ View A mark Y Annotations Y Q)
-64- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
| comme | [| comer |
‘BesePrimitiecOC ‘SubstiutiomcDe
+ & varingt 55-0 WT) > yebtne SOOUAN-SV AT
+ BU: Unicode2$5_DC 10.1) ‘+ ub Quality SV 10.1)
+ edeName: VisString25$_0X 101]. (+ subiD: VisString64_$V 10.1]
+ dataNs: VisString255_EX [0.1] + bikEna: BOOLEAN. Bt (0.1)
(+ ep Revd: BOOLEAN_OR_dehg {0..1]_
+ OpOk: BOOLEAN _OR_dchg [0.1]
+ tO pOk: Timestamp_Of [0.1]
in origin. gpecnperat + Gaara a
‘hom: TUT > Sinem wreu acto
+ vaIWTr: ValWithTrans_ST_dehg 00.1) + logueValue, NOX dechg |
2 Seat aca > Foumatetget
$e teeeee sre Heedemns
+ eld: BOOLEANST. deh 1.11 S Soe ponme teresa
aa vain. 2 Sova anlogeevabe3¥ 0.11
> Ginode cabled cP dche > Stetimoue haa0- Fath 0.32
$4 poleeeesip sired ‘+ sboClass: SboClass CF dehg 10.1] = operate-once
+ eens 2 imau.crade 11 = 0100000
> cperTimeoutIT32U-CF- Ache 101 > Svat acueperveke Se adm.
+ _etiVak StepConerol (+ maxval AnalogueValet_CF_dchg [0..1)
constraints: (+ stepSize: AnalogueValue_CF_dchg [0_1) = 0_imaxVal-minVal
persue + oparTimaout: NT32U.CF-dehg [1]
{Malo NonaPerGroust + iat: FLOKT32_CF dha
faeces 2 Gival Anaoguaanct
MOerhanced|, ‘constraimes:
urbe
MatornonaPercrovpt
hrseaida
oxbet
+ wri Onginee S701 ee
+ etihum: INTBU_ST 10.10
+ valWTr: ValWithTrans ST_dehg 00.1),
2 Sonal Steaha 0-1)
+ Gaceccnseseae + origin: Originator MX (0.11
+ subVal: ValwithTrans_SV [01] Dpilbeoseyoa omens
> eelModek: titled CF dchg + mx Val: AnalogueValue_ NOC dehg (0.1)
+ sbeTimeout: INT32U_CF_dchg [0..1] FE Qualey MOgchg 1.11
+ sboClass: SboClass_CF_dchg [0.1] = operate-once + €Timerneep 0 1)
minal: INTS_CF_dchg [0-11 + stSeld: BOOLEAN_MOLdehg [0.1
+ maxVal: INTE_CF_dehg [0.11 + mubVel: mapnetinpaf rs
+ eperTimeout: INT32U_CF.dchg 10.1) S Sees
+ ave (NTS = ~64..63, (+ ctiMedel: CelModel_CF_dchg
+ sboTimeout: INT32U_CF_dehg [0.1]
a 2 Shccisr:soclas.cF-dch 11 =operate-once
urate 2 Cate: Unc. eh 0-11
(MANOrNonePerGrouptl) ‘+ db: INT32U_CF_dehg [0..1] = 0.100000
‘Morte! > Sve seadvakacon.Cr de 0-1
‘(MOerhanced!, ‘+ minVal: AnalogueValue_CF_dehg [0.1]
+ maxVal AnalogueValue_CF_dehg 10.1)
(+ stepSiae AnalogueValoe_CF_dchg 10..1] = 0. (maxVal-minVad
+ eperTimeout: INTS2U_CF_dchg [0..1)
2 Ghnet noariacr.dehe
+ Siva eect
omen
rob
MAboronePerrevp
‘Mrseaada
‘Mose
‘Moeshenced)
0
Aa
https://www.doc88.com/p-74754903218494.htm| 66/151
```


## File page 067

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark ¥ Annotations v Q
IEC 61850-7-3:2010+AMD1:2020 CSV -65-
© IEC 2020
Figure 18 - Class diagram CDCControl::CDCControl-2
Figure 18: This diagram shows the second part of controllable CDCs defined in the standard
with supertypes that factor their common attributes.
75.2 <<abstract>> Control testing (ControlTestingCDC)
Abstract type, holding control-related attributes common to control common data classes.
This class provides a common set of attributes used for command testing: ‘opRevd', ‘opOk' and
“tOpOk’.
Control serv
= — sofa Wired ouput

opRevd

op0k

t0p0k

sem

Figure 19 — Attributes for command testing
Figure 19: This diagram illustrates usage of attributes related to testing commands.
The command is received (‘opRevd') by the IED as a control service or as a GOOSE message
with a data object semantic resulting in a change of the controllable object (e.g.
CSWI.OpOpn/OpCls, CPOW.OpOpn/OpCls, PTRC.Tr, RREC.OpCis, RBRF.OpEx,
ATCC.TapOpR/TapOpL, SIMG.InsTr, ...). The command is then processed. If the command is
accepted, the wired output may be activated (depending on the mode ‘Mod’ of the function).
The data attribute ‘opOk’ confirms that the command has been accepted and reflects the timing
of the wired output; i.e. the duration of that signal is determined by the configuration attribute
‘pulseConfig’ if the output is a pulse. The data attribute 'tOpOk’ is a timestamp indicating when
‘opOk' is set, i.e. when the wired output may be activated.
Table 39 shows all attributes of ControlTestingCDC.

a
nw
https://www.doc88.com/p-74754903218494.html 67/151
```


## File page 068

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark Y Annotations ¥ Q
-66- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
Table 39 — Attributes of ControlTestingCDC
[ase [memo [re [| soem mmo [rm
opRevd True indicates that an ‘operate’
command for a controllable data object
has been received. It can be used for
testing purposes together with “opOk’
and 10p0k.
True indicates that an operate
command for a controllable data object
has been evaluated and accepted.
Timestamp The timestamp when ‘opOk’ becomes
true, ie., the timestamp when an output
of @ control object would be activated
following an evaluated and accepted
command.
DataAttribute for substitution and blocked
[ser | B00UcN [sv |__| lerted tom: SubamaoncoG | WFaat _|
[20 [uy [5 | [wes an: Sumroc | Wat |
[seo | vesurose (sv || mos vans Sumercoo | raat |
[enc —_[soouem [| | menos von: susmaencoc fo _|
DataAttribute for configuration, description and extension
[| vasisess [0c | [meas von: Saspimtecoc [0 |
[a _| uressrss [oc |_| meee tore saerimnecoc [oO _|
[esnare | vaswirgss [x | | hertea tom: SasPrmaveco _—-[O_|
[ane | vasuess [ex | | eed tom BasPinavecoe | Maan |
7.5.3 Controllable single point (SPC)
This common data class shall be used to represent single point controls with or without
associated status information.
Table 40 shows all attributes of SPC.
Table 40 — Attributes of SPC
fase [meno [ag [ soem meen [rm
st Information related to the originator of
the last accepted operation on the
controllable data object. It mirrors the
appropriate contents of the control
service. Substitution will not affect the
value of ‘origin’.
st ‘The control sequence number of the
last control service. It mirrors the
appropriate contents of the control
service.
st Status value of the controllable data MAIIO‘No
‘object. nePerGro
up{t)
ry
Aa
https://www.doc88.com/p-74754903218494.html 68/151
```


## File page 069

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
IEC 61850-7-3:2010+AMD1:2020 CSV -67-
© IEC 2020
‘mer Aueeowe | Fe |  |_ tatenain me oct [wens |
Quality of the value in ‘stVar. MAIIO:No
nePerGro
up{t)
Timestamp Timestamp of the last change of the MAIIOrNo-
value in any of ‘stVal or 'q. nePerGro
up(t)
True means that the controllable data
object is in the status “selected’.
DataAttribute for control mirror
[ones [oot | on | ew | mowes ton: conarenmgcoe [0 _|
[sox [soocem [oF | ean | mone tons Conrenngcoc [|
[rowo | Tmesire [on |_| motes tore conesingcoc [0 _|
DataAttribute for substitution and blocked
[sae [cote [SV |_| memes tom: Sibsmaoncoc ‘| waa
[seve [sou [sv |_| Vane uses © sss svar ast |
[s20 | owsy sv | | mes Hons Susmaercoc ‘| west |
[smi | vasungse |v |_| memes tom Siboncoc ‘| Maat |
[ones [sou [| | menos von: summaencoc [Oo _|
DataAttribute for configuration, description and extension
Used to configure the output pulse
generated with the command, if
applicable.
ctlModel ‘CilModelKind Control model of IEC 61850-7-2 that
refiects the behaviour of the data.
NOTE If the controllable data object
has no status information associated
(or if that information is not required),
then ‘stVal' of the controllable data
object does not exist. In that case, the
value range for this attribute is
restricted to ‘direct-with-normal-
security’ and 'sbo-with-normal-securty’
sboTimeout Timeout [ms] between a ‘select’ and an
‘operate’ command according to the
control model of IEC 61850-7-2 (ie., if
there is no ‘operate’ command after
‘select’ during this time interval, the
controllable data object shall become
unselected: ‘stSeld’ = false). This
applies also if the control is done
locally and not via communication.
‘SboCiassKind (dotauit=operate-once) Specifies the
SBO-class according to the control
model of IEC 61850-7-2 that
corresponds to the behaviour of the
controllable data object.
operTimeout Timeout [ms] used to supervise an
operation according the control model
Gelined in IEC 61850-7-2. When this
time expires without an indication of a
new valid state, the command action
shall be terminated. In the control
models with enhanced security, a
negative command termination is sent
as response. This attribute is used also
if the control is done locally and not via
ry
Aa
https://www.doc88.com/p-74754903218494.html 69/151
```


## File page 070

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
-68- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
Pas [seem Te] | come meme [roe
Cs
[oo | uneness [oo [| meres tons saorimtecoc [|
[eaname | wasureass | © |_| emia tom: BasePunewecoc [0 _|
[one | vaseess [ex | | ered fom BaPanavecoe | WO
nv ee eS |
control activity (‘talse’ for off or
deactivation, ‘true’ for on or activation).
7.5.4 Controllable double point (DPC)
This common data class shall be used to represent double point controls.
Table 41 shows all attributes of DPC.
Table 41 — Attributes of DPC
L_Mone’ | wor oe Fe | Og |_ neat one owcin | sco
re
[eign J ovignator st | [see secon,
jatum feu st || see seccanum: fo
epee [lee YT
Cs ee
a  ierenere =
value in any of ‘stVar or ‘q.
[ss [eooun dst | ae [ser srcamie ——SSC«*d |
[oonoa [cota | on | aa | reed tow: covarenacoe [0 |
[eon | oouean [on | a [res to cetareeiaeac [oO |
[reno | Twesarp [On| | ewes tom canaengcoc [0
[ase [soousen [sv | | was tan: ibansoncoc (wrt |
[saver | Opsnstrs [sv | | abe weed tative svar ‘| Mra |
[seo [omy [sv | | mows vans susmaencoc | wast |
[ssi | vases | sV_| | eed fom Subnncoc ‘| Wret —|
[ecm [soouemn | a_| | meres vars surmaencoc fo _|
[pasecona | Pumeconeg = GF | aa | Ser sPCpancong ———=SSCdO =
[ance | cmiosiind (oF | ew | en sPcamewst SS
ae oe
Sl ead lal =a
ry
cy
8
Aa
https://www.doc88.com/p-74754903218494.html 70/151
```


## File page 071

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
IEC 61850-7-3:2010+AMD1:2020 CSV -69-
© IEC 2020
fase [soem [re] | come me meen [roe
Cc i lac il
Co
[av | uncosess [0c | | ered to easPinavecoe [|
[are | wasnnacss [ex |_| menos vor: saspimiecoc [Oo _|
[ne | vasuneess [ex | | ete fom SaaPinavecoe | MO
jm fee eaves]
Control activity (false’ for off, ‘true’ for
on).
7.5.5 <<statistics>> Controllable integer status (INC)
This common data class shall be used to represent integer controls.
Table 42 shows all attributes of INC.
Table 42 — Attributes of INC
rs | meee [e || teemmnmeen [rem
[own [own [st | [sw srcom «tO
[amm [wre [st | [se srcamm ——S—idto CS
we eee
: object.
upd
Cs
ee
event of ‘stVal’, or the last change of
value in‘.
a
[eens [sootem | om | eng | motes tor: Conresingsss [0 _|
[ook | pooiean | om | aa | meres ton: Conaeiaco 0 _—|
[tov | Tresum [8 |_| moms von ConrTaningcoc [|
[weem [soot [sv |__| mae vor: susmaoncoc | wast |
ce
[seo [omy (sv | | memes tom: Suamitcoe | waa _ |
[sao | waswrose [sv |_| was van Sibanaoncoc | wast |
[ence [soousan [a | | wins von: Sisansercoc fo _—|
[cote [Omtosnns (oF [are | Sor sPcemoaer SSS i
[sorimean[mroeu [oF [on | Sov sPCsbotimwowe SSC |
a
cy
8
“a
https:/Awww.doc88.com/p-74754903218494.htm! 71151
```


## File page 072

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
-70- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
fase [som Te] | comme me meen [roe
a Gla =a
[eva —[wrne [oF [te [oman song rene ————«dtO
[ewer [wae =i | aw | acm eating er eer SiO
the i q values
of ‘ctiVal’
Gi ic el lec el
a Glad ==
‘maxVaf, ‘stepSize’ctiVal’.
co
[a __[unesszss [0c | | menos von: Beerimieco [oO _|
[are | wasvewess [x [| moms tom BaoPimmecoc [|
ee
eT eee TT
control activity.
7.5.6 <<abstract>> Controllable enumerated status (ENC)
This common data class shall be used to represent integer controls with the value restricted to
those in an enumeration.
Table 43 shows all attributes of ENC.
Table 43 - Attributes of ENC
[ase [seme [re] ag [ semen [rm
po ataattribute for states
[ovin | ovginator st | [seosecomi TO
[eum inte st || seo seco.
Status value of the controllable data
object.
[a | avatty | St] cera | unity of the vate in iver, TM
SN hn lO Fr
value in any of ‘stVal or 'q.
[sas [Boor [st [oy [sor srcaser = |
[somos [oot | on | aw | moans tom: convronacoc [0 |
[ese | sootemy [oF | ny | motes tors conaeningcoc [|
[repo | Teesanp on |_| iets tom: Conarenmaco 0 _|
[seem [eoocem [sv |__| motes tors susmaencoc —( wranst_|
[seve [emmoa [sv |_| voe ures © wtsese aver rat
a
cy
8
“a
https:/Awww.doc88.com/p-74754903218494.htm! 72/151
```


## File page 073

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
IEC 61850-7-3:2010+AMD1:2020 CSV -71-
© IEC 2020
ea dG
a
[seo | vesirost | sv_| [wots ors Sutsmaercoc | wast |
[encna [ooo [a | | ems ton: samarcos [0 _|
[ss [ominsonns [oF | oy [en srocmomr |
[worms [wrmey | oF [aay [sor sPCsbotmo i ms
Gl kl lal =~ al
"SPC.sboCiass
Ca i Gal
[=| wesneass [66 | [wr tans Ssorinewene ——_[o _|
[a [unwomcss [oe | [writ tan: eaaPintecoe [0 _|
[eanane | wasereass [© |_| imeras ton: BasePunmrecoc 0 _—|
[ne | vasress | © || ewe tor esaPmivecoe | won|
TT ie
control activity.
7.5.7 <«statistics>> Binary controlled step position information (BSC)
This common data class shall be used to represent binary-controlled step positions.
Table 44 shows all attributes of BSC.
Table 44 - Attributes of BSC
ese | mem [e |g | em meen [rem
[oon [owner [sr | [seesrcame ——~—S*dt OC
[num [wry st | [see seccmm fo
ee Pee |
object. nePerGro
pit)
poe Reese |
nePerGro
pit)
ee ec
value in any of ValWTr or ‘q’ nePerGro
upit)
[wsus_[oocucwn [sr [ato [seo srcsses | woto
[opRew | Bootean ——| OR | ang | inherited trom: Contoesingcoc TO
rox | Bootean | on | ng | inherted tom: ContotTesingcoc ||
[rove [teen on | [interes tom Corveterinacnc [To |
a
cy
8
nw
https://www.doc88.com/p-74754903218494.html 73/151
```


## File page 074

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
-72- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020

[se [sow [ [ig | toe meme [roe
[seem [Boom [sv |_| motes Yor summaencoc ———‘( wraet—|
[saver —_[Vawawrans | sv |_| Vae ied soainae vie | wast _ |
[seo [aay (sv |_| red tom: Sieinncoc | Mra |
[ssi | vasinase | v_| | em om Sibetnancoc | Mra |
[one [soot | | | menos vor: susmaencoc [Oo _|

Configures the control output. i

‘persistent’ = false, the ‘Operate’

service results in the change of exactly

one step higher or lower, as defined

with ‘cial’ (Le.,‘etlVal’ =

higherflower’),

It ‘persistent’ = true, the ‘Operate’

service initiates the persistent

activation of the output (and ‘ctiModel

shall be set to ‘direct-with-normal-

security). The output will be

deactivated by an ‘OperateWithValue’

service with ‘ctiVal' = ‘stop’, or by a

local timeout. A client may repeat

sending the ‘Operate’ service in order

to retrigger the output.
[amcor | Games ———id oF | ay | see crocs. SSS
[etineon[wrsay (oF | ony [See secamtmene wo |

‘SboCiassKind (default-operate-once) See
"SPC.sboCiass'
minVal INTB Minimum setting for ‘valWTr.posVar

below which ‘ctiVal'stower’ will have no

effect.

Maximum setting for valWTr.posVar

above which ‘ctlVal'="higher’ will have

no effect,
Pe |
[| wastes [be [ [mes vans Bearimnecoe [|
[a [unenoass [oo [| mes ton saopimnecoc [|
[eanane | vaswszss [© | | motes vans Seopimiecoc [Oo _|
[aman | vaserass [ex | [wat ton: asPontecoc | won|
em TT eee

Control activity.
7.5.8  <<statistics>> Integer controlled step position information (ISC)
This common data class shall be used to represent integer-controlled step positions.
Table 45 shows all attributes of ISC.

a
“a
https:/Awww.doc88.com/p-74754903218494.htm! 74/151
```


## File page 075

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
IEC 61850-7-3:2010+AMD1:2020 CSV -73-
© IEC 2020
Table 45 — Attributes of ISC
Troe | Mme ore || By | seine me cet |
|
[oon [ovina [st | [seo secon. fo
[enum [wre fer [| [seosrcemm. fo
ValWithTrans Status value of the controllable data MAIIOrNo
er fee lee eS |
p(t)
poe epee |
nePerGro
upit)
ee ee
value in any of ValWT? or ‘q’ nePerGro
upit)
|sses__[pooean [st [ame [seo secsisos. | Moto __|
[estos [occ [on [are [riots ton: Conoreacoo [0
[so | Boouem | 0 | | reed rom conaaxingsoe [|
[ero [Tessar [on | [wera tom ConaTeincoe [0 |
[seem [eoouem [sv |__| eae tom stmasncoo | wrnt _|
[sev | vawnrars | sv | | vate ure wee wo | wrist
[sso [omy [sv | | ee ta etenancoe | wat |
a
[ewes [eoctean [st _| [wes von: sasmaorcoo fo _|
[eis —[omesonns [oF | ana [sen srocmoat. =
[norma [wey [oF [aay [Soe sree =i me
Sa ala =a
a
[nent [wre | oF [a | wasn ating or wari
em fee alee
es
[> | voewnass [56 _[ [wrt tan soinenwcnc | o_|
[snare | vasrssss | x | | mow tor: enorimecoe fo _|
[ne | wasureass | © | | emia tom: BasePunawecoc | won|
= TT ieaecer |
that determines the control activity.
75.9 <<statistics>> Controllable analogue process value (APC)
This common data class shall be used to represent analogue controls.
Table 46 shows all attributes of APC.
a
cy
8
nw
https://www.doc88.com/p-74754903218494.html 75/151
```


## File page 076

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
-74- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
Table 46 — Attributes of APC
fase [mem [e |g | terme [rm
[emm [mrou wx | (swe srcamm —SSSC«dO Cd
AnalogueValue Current value of the controllable MAIIO:No

analogue process value or of the nePerGro

‘setpoint; details must be provided in up{t)

the semantic definition of the

Controllable data object using this CDC.

Quality of the value in ‘mxVar. MAIIONo
nePerGro
ptt)

Timestamp ‘Timestamp of the last change of the MAIIO‘No

value in any of ‘mxVal' or 'q. noPerGro

pit)
a
[sonea [cota [on | ea | meted tom: conmTennacoe [0 |
[eso | eocusy _| on [ae | reins tons Conorenncoo [oO _|
[oro | Twesarp [On |_| meted tom convaengcoc [0 |
[stem [coum |v |__| heed fom: Sibincoc ‘(| Wa |
[seve | Arscomvaie [sv |_| Vobe uses © state mar rat |
[seo [omy [5 | | moms von susmaencoc | wrast_|
[smo [wasurass [sv | [ema tom: Samisncoe | wrest |
[oem [soocem [a | | meas vor: Surmaencoc fo _|
DataAttribute for configuration, description and extension

[amcor | cmiosiins =| GF | ea | Son sPcamiewss
[worineat [may [oF | | Sn SPC atmo [woe |
el lal =a

Common Unit for: ‘mxVal’ ‘subVar,

‘minVar, ‘maxVal’ ‘stepSize’, ‘dbRef,

‘etvar.

(range=[0...100000)) See ‘MV.db’. Used

to deadband ‘mxVal' for reporting.

‘ScaledValueConfig Configuration for scaled value

representation of 'mxVal', ‘subVaf,

‘minVal, ‘maxVal’, ‘stepSize’, ‘ctiVal.

[ava [Arsoomvane [oF [oe [wan song rene =f
[newer | Anopavaue | F | ew | Macmum eoting rear «tO
stepSize (range=[0...(maxVal-minVal)}) Step

between the individual accepted values

of ‘ctival.

Sl i ll st
ed
FLOATS2 See "MV.dbRe?. Used to deadband
‘mxVal' for reporting.

[s[ weseass [66 [ [wernt vans eaeohimnecno [|

ry

Aa

https://www.doc88.com/p-74754903218494.html 76/151
```


## File page 077

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
IEC 61850-7-3:2010+AMD1:2020 CSV -75-
© IEC 2020
fase [seem Te] | come meme [roe
es
[easane | waswnaess [x [| memes tons saepimnecoc [|
[ane | wasureass | © | [emia tom: BasePunewecoc | woaans_|
[~ ae TT ese
control activity.
7.5.10 <cstatistics>> Binary controlled analogue process value (BAC)
This common data class shall be used to represent binary-controlled analogue values.
Table 47 shows all attributes of BAC.
Table 47 — Attributes of BAC
‘mer | Mee owe Fe |p | Seine my econ
| atnatrivute for measured attibutes
[oon | ovgnator ux | [see secon
[num [wry wx || See sccm:
mxVal MAIIO‘No
a ee eee a |
pit)
pee |
nePerGro
p(t)
Timestamp Timestamp of the last change of the MAIIO“No
value in any of ‘mxVal’ or 'q’ veer
4
[ssos___[eooean | oe [ety | see spcatsoe. [Moto |
[somes [0otea | on | ap | mowed tom: conarenngcoe [0 |
[so [aoouswn | on | aa | irs ton: ConaTeiacoe [0 _|
[rose | Tesre [on | [etwas tar Conaeningcoc fo _|
[meem [eoocem [sv | | mowes von: summaancoo | wast _ |
[sev [aniopevanw [S| [Vain ued snan star | wa |
[x20 [owsy sv |_| rts Hons Sutmrcoc | wat |
[ss20 | wssorese [|_| wes von: Submsorcoc | wast |
[nena [oootcan |_| | memes ton: samrcos [0 __|
[pene [sou or | ong [son escpemsee
[ence —[omasonns [oF [ng [sor srcemowr w=
[sonic [wmaey | oF | ay | Ser scatman ——~SCSC*d |
el ee al =a
a
cy
8
“a
https:/Awww.doc88.com/p-74754903218494.htm! 77151
```


## File page 078

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
-76- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
fase [seem Te] | cee meme [ro
‘Common Unit for: ‘mxVar, ‘subVal’,
‘minVal’, ‘maxVal'’, ‘stepSize’, ‘dbRef.
a ad ==
to deadband 'mxVal’ for reporting.
ee ie
representation of ‘mxVa’, ‘subVal, AV
‘minVat’, ‘maxVal'’, ‘stepSize’.
which ‘ctlVal = ‘lower’ will have no
effect.
Maximum setting for ‘mxVal' above
which ‘ctlVal' = ‘higher’ will have no
effect.
cr (range=(0..(maxVal-minVal)]) Step
between the individual values of ‘mxVal’
that result from applying ‘ctiVal’ =
‘higher lower’
=|
a la ="
‘mxVal' for reporting,

[2 | vaseneass [06 | | heres tom sasrininecne fo _|

[a | uneomass [0 | [mies ton saerimiecoc fo |

[cxniene | vasuneass [ex | | meres om SuePinevecoo _[o_|

[tne | asureass | x | | era tor asePinewcds | won|

‘StepControlKind Service parameter that determines the
control activity.

7.6 Status settings

7.6.1 General

This subclause defines all the common data classes for status settings. Status settings are of

type boolean, integer or enumeration. The values of settings are defined based on system

requirements and they are initially configured. In contrast to setpoints, settings do not change
as part of operation. In some cases, settings can be used to represent ratings or inherent
properties that cannot be changed.

In a realisation, we may have two variants of settings:

— individual settings: they have a functional constraint SP. Individual settings can be written
and the new value is applied immediately.

— settings that belong to a setting group: they have a functional constraint SG. The values of
these settings can be edited through the functional constraint SE. For details of the use of
setting groups see IEC 61850-7-2.

Modelling note: whenever setting groups may apply, the setting common data class is split into

one <<abstract>> and three dedicated classes, one for each of the functional constraints SP,

SG, SE:

= the class XXX_SP shall be used for status settings that do not belong to a setting group. It
represents the active value of the status setting that can optionally be set.

a
“a
https:/Awww.doc88.com/p-74754903218494.htm! 78/151
```


## File page 079

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark ¥ Annotations v Q
IEC 61850-7-3:2010+AMD1:2020 CSV -77-
© IEC 2020
- the class XXX_SG shall be used for status settings that do belong to a setting group. It
represents the active value of the status setting. To get or to optionally set value of the
status setting from the group selected for editing (see IEC 61850-7-2), the class XXX_SE
shall be used.
For applicable services, see Annex B.
7.6.2 Single point setting
7.6.2.1 General
This subclause defines the common data class for single point setting.
BasePrimitiveCDC
+d: VisString2SS_DC [0.1]
+ dU: Unicode25S_DC [0..1]
(+ cdeName: VisString255_€X [0.1]
+ dataNs: VisString255_ [0.11
constraints
(MOdataNs}
Figure 20 - Class diagram SPG::SPG
Figure 20: This diagram illustrates the specialisation of the single point setting CDC.
7.6.2.2 <<abstract>> Single point setting (SPG)
Abstract type, holding attributes common to single point settings.
Table 48 shows all attributes of SPG.
Table 48 — Attributes of SPG
fase [mem |e] g| cnn [mem
[< | vases [00 |_| ted tom eaPintecos [Oo __|
[av | vneosass [0c | [mre tom easPininecoe _[o_|
[estene | veweeass [ex | | wis tan: eaaPimiecoc [|
[ems | vesueass [© | | owes ton sasremmecoc | mos |
a
“a
https:/Awww.doc88.com/p-74754903218494.htm! 79/151
```


## File page 080

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
-78- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
7.6.2.3 Single point setting (FC=SP) (SPG_SP)
This common data class shall be used to represent single point settings with FC = SP.
Table 49 shows all attributes of SPG_SP.

Table 49 — Attributes of SPG_SP
ae ed
a ca ld =

off, true is on).
a
[a | uncomass [oo | [tetas tan eaerontecoo fo _|
[snare | vasweaess | © | | memes tom eaahumnecoe [|
[sane | Waswreass | © | [ert tom: asoPanewecoe | won|
7.6.2.4 Single point setting (FC=SG) (SPG_SG)
This common data class shall be used to represent single point settings with FC = SG.
Table 50 shows all attributes of SPG_SG.

Table 50 — Attributes of SPG_SG
a
a ca a EC

off, true is on).

[= [vases [60 | | reas tom enerimecoe [O_|
[> | vnenass | 06 | [morn tons aeoPnevecnc [|
[se | veseeass |_| [wort ton ssoPinecnc [|
[une | vesingass [| [eto an seoinenecoc | wean
7.6.2.5 Single point setting (FC=SE) (SPG_SE)
This common data class shall be used to represent single point settings with FC = SE.
Table 51 shows all attributes of SPG_SE.

a

cy

8

nw

https://www.doc88.com/p-74754903218494.html 80/151
```


## File page 081

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
IEC 61850-7-3:2010+AMD1:2020 CSV -79-
© IEC 2020
Table 51 — Attributes of SPG_SE
Pe [mmm [lg] mre [rm
a ca lala
off, true is on).
DataAttribute for configuration, description and extension
[so __|vasureass [0c |_| meres tom: asePunmeco 0 __|
[| Wesess [06 | [oe tan ssPenmrecoc fo |
[scare | vases [© | | motes tan eanPuntrecoc fo |
[swans | vases | | [motes tan asPemmvecoc ‘a
7.6.3 Integer status setting
7.6.3.1 General
This subclause defines the common data class for integer status setting.
+ 4: VisSteing28S_0¢ [0..1]
+ U: Unicode2SS_DC [0..1]
+ cdeName: VisString255_&X [0.1]
+ dataNs: VisSteing2SS_£X [0.1]
constraints
(MOdatans}
‘+ minVal: INT32_CF_dehg [0..1]
+ maxVal: INT32_CF_dehg [0.1]
‘+ stepSize: INT32U_CF_dehg [01] = 1_(maxVal-minVal)
+ _units: Unit_CF_dchg [0..1]
Figure 21 - Class diagram ING::ING
Figure 21: This diagram illustrates the specialisation of the integer status setting CDC.
7.6.3.2 <<abstract>> Integer status setting (ING)
Abstract type, holding attributes common to integer settings.
Table 52 shows all attributes of ING,
a
nw
https://www.doc88.com/p-74754903218494.html 81/151
```


## File page 082

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
-80- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
Table 52 — Attributes of ING
= aE a
[neva [wre [oF | a [ womun song tr wove ———=*[O—|
es
between the individual values of
‘setVal'.
Sa La la =
‘stepSize’
[~ __| vasosss [06 | | ones ton SasPinvecos [|
[a | unessass [0c |_| meres vans sespimiecoc [Oo _|
[axtare | veswreass [=x | | mens tons eaeerinenecoc [Oo
[sone | vaserness [x |_| memes Yom: BaaPanavecoo | MO
7.6.3.3 Integer status setting (FC=SP) (ING_SP)
This common data class shall be used to represent integer settings with FC = SP.
Table 53 shows all attributes of ING_SP.
Table 53 — Attributes of ING_SP
[ase [soem [e |g | se meme [rem
[ew [wae [5 | eo | Tw sabe ote wate ong M_—
[mma [wre === or | ae |imoma tom nc ———SCSCidO i
[naw [mrs [oF [oe [mows von ng ———SSC*dt =
[moose [mroay «ior ne [imenes wore nc SSSC«dOC
[vets unt oF | cata | innertes tom: wa TO
[« | vasosss [00 | | mowed tom: easinaecd [0
[ai | nesaass | 0c | | werd tom: asoPanmecoc [0 |
[axtane | vesureass [x | | mets vans Baserinecoc [|
[sume | vasungess [ex | | ewes fom SaaPinavecoe | Msn
7.6.3.4 Integer status setting (FC=SG) (ING_SG)
This common data class shall be used to represent integer settings with FC = SG.
Table 54 shows all attributes of ING_SG.
ry
Aa
https://www.doc88.com/p-74754903218494.html 82/151
```


## File page 083

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
IEC 61850-7-3:2010+AMD1:2020 CSV -81-
© IEC 2020
Table 54 — Attributes of ING_SG
fe | mem [el] nme [
[mwa [wre [86 |__| Thwvabe ftw sana wire fw ‘|
[ews [wee [or [ate [innertea toms ng tO
[_mawa_ [nse [oF [ame [innerted tom: we JO
[seosve [wrszu | oF [ae | innerted tom: wa tO
[ute [uw cr [acta [interted tom: we fo
[oo | Wneuass | 06 | [wort van ssoPienecnc [|
[canine | vasureass | ©x | [herd tom: asoPunmecoc [0 |
[aman [vaserass [ex | [wins an aaPintecoc | won|
7.6.3.5 Integer status setting (FC=SE) (ING_SE)
This common data class shall be used to represent integer settings with FC = SE.
Table 55 shows all attributes of ING_SE.
Table 55 - Attributes of ING_SE

esr [mem |e] g| terme [rem
[| ataattbute for seting
[sows [nse [se | | evaw ne sans ory fw |
[rv [wrse [oF [a [innertes tom: ing
[marvel [mtsz | or | eng | innerted toms wo
[soso [wee [oF [ay [mom roms ne ———SS=itO
a
[s | weseneas [oc | [wees von: ewarinnecoo ——[o_—|
[| vneuass [06 | [wore tan asoPinnecoc [0
[stare | vesegass [x | [mom tons Bxineecoc [|
[aman | vasurass [ex | [tia tan aaPintecoc | won|
7.64 Enumerated status setting
7.6.41 General
This subclause defines the common data class for enumerated status setting.

a

cy

8

nw

https://www.doc88.com/p-74754903218494.html 83/151
```


## File page 084

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark ¥ Annotations v Q
-82- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
BasePrimitiveCDC
+d: VisString255.0C [0..1]
+ dU: Unicode255_0C [0..1]
+ cdeName: VisString255_Ex [0..1]
+ dataNs: VisString25$_& [0.1]
constraints
{MOdataNs}
Figure 22 - Class diagram ENG::ENG
Figure 22: This diagram illustrates the specialisation of the enumerated status setting CDC.
7.6.4.2 <<abstract>> Enumerated status setting (ENG)
Abstract type, holding attributes common to integer status settings with the value restricted to
those in an enumeration.
Table 56 shows all attributes of ENG.

Table 56 — Attributes of ENG
A Ril
[| vases [06 |__[moues tan: easPenmrecoc To _|
[oo | weness [56 | [oe tan asPenmecoc fo |
[aetane | vesasss | | | motes tan Basrunmecoc fo |
[ie | westnaess | &x [| eed tom asePimiecoe | woanans_|
7.6.4.3 Enumerated status setting (FC=SP) (ENG_SP)

This common data class shall be used to represent integer settings with FC = SP, with the value
restricted to those in an enumeration.
Table 57 shows all attributes of ENG_SP.
a
nw
https://www.doc88.com/p-74754903218494.html 84/151
```


## File page 085

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
IEC 61850-7-3:2010+AMD1:2020 CSV -83-
© IEC 2020

Table 57 — Attributes of ENG_SP
0 ee I Ril
[savas [enmon [5° [ay | The vauw fe sans wry fw |

DataAttribute for configuration, description and extension
a
a
[stare | vases | | [woe tons ineecoc [|
[aman [vases [ex | [wa tan aaPintecoc | won|
7.6.4.4 Enumerated status setting (FC=SG) (ENG_SG)

This common data class shall be used to represent integer settings with FC = SG, with the value
restricted to those in an enumeration.
Table 58 shows all attributes of ENG_SG.

Table 58 — Attributes of ENG_SG
ea ce
[we [emmoa [80 | | Twvabn ft sana wire fw ‘|
[| veseass [00 |_| ross ton: aroPinenecoc [0 |
[a7 [neowass [00 | [eas tom: BasePinewecoc [0 _|
[scare | vesigess | | | mote tan BasPenmrecoc fo |
[ie [vases | © || hee tor soPimiecoe | worn
7.6.4.5 Enumerated status setting (FC=SE) (ENG_SE)

This common data class shall be used to represent integer settings with FC = SE, with the value
restricted to those in an enumeration.
Table 59 shows all attributes of ENG_SE.

Table 59 - Attributes of ENG_SE
[ase [meno [re] ag [ soem meen [rm
[ssa [Emmons |_| Teva of ti sana wire fw |
[=| veswreass [0c |_| reins ton: arepinenecoo [|
[ai [neowass | 0c | [era tom: BasePunewecoc [0 _|

a

nw

https://www.doc88.com/p-74754903218494.html 85/151
```


## File page 086

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
-84- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
fase [mem [Rg] em meen [rm
[ssawne [vases [ex |_| memes tons eaPimmecoo fo _|
[ie | vasraess | © | | meme tom eePimmecoe | woanane_|
76.5 Object reference setting
7.6.5.1 General
This subclause defines the common data class for object reference setting.
BasePrimitiveCDC
+d: VisString2ss_DC [0.11
+ dU: Unicode255_0¢ [0..1]
+ cdeName: VisString255_&X [0.1]
+ dataNs: VisString255_&X [0..1]
constraints
(MOdataNs}
‘+ setSreRef: ObjectReference SP_dehg
+ setTstRef: ObjectReference_SP_dchg [0..1]
+ setSreCB: ObjectReference.SP_dchg [0..1]
+ setTstCB: ObjectReference.SP_dchg [0..1]
+ intAddr: VisString255_SP.dchg [0..1]
‘+ tstEna: BOOLEAN_SP_dchg [0..1]
+ purpose: VisString255_0C [0..1]
constraints
{AllOrNonePerGroup(I)}
{OFsetTstRef
Figure 23 — Class diagram ORG::ORG
Figure 23: This diagram illustrates the specialisation of the object reference setting CDC.
7.6.5.2 Object reference setting (ORG)
This common data class shall be used to represent object reference settings.
a
nw
https://www.doc88.com/p-74754903218494.html 86/151
```


## File page 087

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
IEC 61850-7-3:2010+AMD1:2020 CSV -85-
© IEC 2020
[caseom Sutechann 00 seen object reference /)
tstEna=TRUE >>> J [ Funct |
Figure 24 — Switching to test object reference
Figure 24: This diagram illustrates switching from normal to test object reference by setting
‘tstEna’ = true.
In a normal operation, the LN xxxx receives as an input the signal Out from LN yyyy. The data
attribute ‘xxxx.InRef1.setSrcRef’ points to yyyy.Out. For functional testing of the LN xxxx, a
logical node tttt may be used to generate test patterns. In that case, the LN xxx shall receive
the input from LN ttt; e.g. the signal SPCSO1. This is indicated by the data attribute
‘xxxx.InReft.setTstRef. By setting ‘xxxx.InRef1.tstEna’ = true, the LN xxxx will start receiving
the signal from tttt instead of yyyy.
Table 60 shows all attributes of ORG.
Table 60 — Attributes of ORG
= ea I Ro
ObjectReterence The value of the object reference
setting as specified in the context
where the common data class is used.
setting, used as alternative to PerGroup
‘setSrcRef’ when ‘istEna’ = true for 1)
testing purpose.
control block, indicating from where the
abject referred to with ‘setSrcRef shall
be received,
melee | |S SRS aes | |
control block indicating from where the | Ref)
object referred to with ‘setTstRef shall
a
nw
https://www.doc88.com/p-74754903218494.html 87/151
```


## File page 088

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
— 86 - IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
Pree [sem [ag | erm mere [rew
|| rs ||
for testing purposes.
[arc | vases | 5° [ae | wansacinspacte Firat asines [6 |
Switch between original data source ‘AllOrNone
(as defined with ‘setSrcRef’ and PerGroup
‘setSrcCB)) for a reference and test 1)
data source (as defined with ‘setTstRef
and 'setTstCB').
Ca a bl =a
relerence.
a
[>| voenass | 06 | [wort ton essoPinewcnc [Oo _|
a
[ene | veseroass | © | [mtr ons areinevecoc | MORN
7.6.6 — Time setting
7.6.6.1 General
This subclause defines the common data class for time setting. If both attributes ‘setTm’ and
‘setCal’ are present, to have the possibility to declare the one that shall be used, the following
convention applies:
- a-value setTm with a NULL timestamp indicates that the time value exposed in setTm shall
not be used.
- a valid setCal.occType = NONE indicates that the time value exposed in setCal shall not be
used.
a
“a
https:/Awww.doc88.com/p-74754903218494.htm! 88/151
```


## File page 089

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
IEC 61850-7-3:2010+AMD1:2020 CSV -87-
© IEC 2020
CoreAbstractCDCs::
+ 4: VisSering2S$_0C [0.1]
+ dU: Unicode2ss_0C [0..1]
+ cdeName: VisString255_6X [0..1]
+ dataNs: VisString255_&(0..1]
‘constraints
(MOdataNs}
[=]
a ee eee =
‘+ setTm: Timestamp_SP_dehg [0.1] ‘+ setTm: Timestamp_SC [0.1] + seeTm: Timestamp. SE [0_1]
+ setCal: CalendarTime sP_dehg [0.1] |] + setCal: CalendarTime SC [0.1] } | +  setCal: CalendarTime SE [01]
‘constraints ‘constraints constraints
AtLaastOne(t)) [AtteastOne(t)} (AtLeastOne(1))
Figure 25 —- Class diagram TSG::TSG
Figure 25: This diagram illustrates the specialisation of the time setting CDC.
7.6.6.2 <<abstract>> Time setting (TSG)
Abstract type, holding attributes common to time settings.
Table 61 shows all attributes of TSG.
Table 61 — Attributes of TSG
pase [mem |e |g | cern [mem
[= | vesngass [56 | __[ moan tan: eanrentnecsc fo _|
[oo | Wenseass [06 | | oues tan asrummnecoc fo |
[eaame | vasureass | x |_| meta ton: BasePinmeco [0 _|
[anne [vasurass [ex | [waa ton saaPumeecoc [wos |
7.6.6.3 Time setting (FC=SP) (TSG_SP)
This common data class shall be used to represent time settings with FC = SP.
Table 62 shows all attributes of TSG_SP.
a
nw
https://www.doc88.com/p-74754903218494.html 89/151
```


## File page 090

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
-88- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020

Table 62 - Attributes of TSG_SP
Pe | mee [el | nme [mm
Sc a a Gad all

is set with a time stamp.
Sc Gad eal
is set with a calendar time.
[s [vases [oc | | wens tan: aarontecoo fo _|
[oo | vneuass [06 | [were ton easePiinecoc [|
[stare | vesueass | x | [mot tons asoPineecoc [|
[sins [vistas [© [| eed wom asehmivecoe | woman
7.6.6.4 Time setting (FC=SG) (TSG_SG)
This common data class shall be used to represent time settings with FC = SG.
Table 63 shows all attributes of TSG_SG.

Table 63 — Attributes of TSG_SG
ed
Ga Pa

is set with a time stamp.
CalendarTime The value of the time setting, if the time
is set with a calendar time.
[2 | waswwess [00 |_| heed tom eaarimtwcde [|
[ao | uneowass [oo | [tetas tan eaerontecoo —[o_|
[scare | vases |_| [most tons aseinenecnc [|
[one | wasureass | © | [era tom: BasePunenecoo | won|
7.6.6.5 Time setting (FC=SE) (TSG_SE)
This common data class shall be used to represent time settings with FC = SE.
Table 64 shows all attributes of TSG_SE.
a
nw
https://www.doc88.com/p-74754903218494.html 90/151
```


## File page 091

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
IEC 61850-7-3:2010+AMD1:2020 CSV —89-
© IEC 2020
Table 64 — Attributes of TSG_SE
Pe [mem [fg] mee [rm
a a al = al
is set with a time stamp.
=| ates = [a]
is set with a calendar time.
[s [vases [oc | | wteaes ton: saPimiacoc fo _|
[oo | Wneneass [06 | [moe tan Basremmecoc fo |
[stare | vases [© | | momo tan BasPrmmrecoc fo |
[caine [varias [x | | eed vom easehimnecde | woamans_|
7.6.7 Currency setting
7.6.7.1 General
This subclause defines the common data class for curve setting.
BasePrimitiveCDC

+ & VisString2S$_0C [0.1]

+ dU: Unicode2S5_0¢ [0.1]

+ cdetame: VisString25S_EX [0.1]

+ _dataNs: VisString25S_Ex [0.1]

constraints
IMOdatans}
Figure 26 — Class diagram CUG::CUG
Figure 26: This diagram illustrates the specialisation of the currency setting CDC.
7.6.7.2 <<abstract>> Currency setting (CUG)
Abstract type, holding attributes common to currency settings.
Table 65 shows all attributes of CUG.
a
nw
https://www.doc88.com/p-74754903218494.html 91/151
```


## File page 092

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
-90- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
Table 65 — Attributes of CUG

= aE hao
[= [vases [0c | | moms vor sapimecoe [0 |
[oo _|unenswrss [oc [| meres tors saspimnecoc [O_|
[scene | veswezss [x |_| mots von: SuePaneecnc [0
[ne | vases [x | | eed fom BasPinavecoe | woes
7.6.7.3 Currency setting (FC=SP) (CUG_SP)
This common data class shall be used to represent currency settings with FC = SP.
Table 66 shows all attributes of CUG_SP.

Table 66 — Attributes of CUG_SP
[se [sewmm [|g | some meme [roe
ll =a

according to ISO 4217,

[¢ | vases [00 |_| hered tom: tasPinavaco [|
[a | vneomass [0c | | ers tom: eaaPinavacoe [|
[canons | vasungess [ex | | memes tom: BaaPinaveco [0
[sane | wasureass | © | [ert tom: asoPinewecoe | woaan_|
7.6.7.4 Currency setting (FC=SG) (CUG_SG)
This common data class shall be used to represent currency settings with FC = SG.
Table 67 shows all attributes of CUG_SG.

Table 67 — Attributes of CUG_SG
\_Mone’_| Mammen ere | |_ neat on cin | ico
a a

according to ISO 4217.
[= | vasuneess [0c |_| meres tom: eaaPinavecos [0 |
[au | Uneoaass | 00 | | eras tom: BasePunewecoc 0 _|
[sare | vases [ex | | ered tom SasPinavacos [|
ry
Aa
https://www.doc88.com/p-74754903218494.html 92/151
```


## File page 093

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
IEC 61850-7-3:2010+AMD1:2020 CSV -91-
© IEC 2020
rar [mem [eg | een [mo
[anne | vaiess [|_| etd van anPimecoo | wan |
7.6.7.5 Currency setting (FC=SE) (CUG_SE)
This common data class shall be used to represent currency settings with FC = SE.
Table 68 shows all attributes of CUG_SE.
Table 68 — Attributes of CUG_SE
a
a al
according to ISO 4217.
fs _[wasersass [00 |_| meas tor: SaePommecoo [0 _—|
[oo | vnenass | 06 | [worn ton ssoinenecnc [|
[senare | wesnngees [x || ee vars easehimtvecoe ‘fo |
[anane | vores [| [wt tan snaPimiecoc | wou |
7.6.8 Visible string setting
7.6.8.1 General
This subclause defines the common data class for visible string setting.
a
nw
https://www.doc88.com/p-74754903218494.html 93/151
```


## File page 094

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
-92- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
BasePrimitiveCDC

+d: VisString2SS_0C [0..1]

+ dU: Unicode25$_D¢ [0.1]

‘+ cdeName: VisString25S_EX [0.1]

+ _dataNs: VisString25S_Ex [0..1]

‘constraints
(MOdataNs}
Figure 27 — Class diagram VSG::VSG
Figure 27: This diagram illustrates the specialisation of the visible string setting CDC.
7.6.8.2 <<abstract>> Visible string setting (VSG)
Abstract type, holding attributes common to visible string settings.
Table 69 shows all attributes of VSG.
Table 69 - Attributes of VSG
ase [mem |e] g | nner [mem
[s [vases [oc | [wate ton: saaPimmacoc fo _|
[oo | Wneneass [06 | [meet tan easPummecoc fo |
[stare | vesgess |_| ome tan BasPenmrecoc fo _|
[aman [vasurass [ex | [wom tor saPimmcoc [moss |
7.6.8.3 Visible string setting (FC=SP) (VSG_SP)
This common data class shall be used to represent visible string settings with FC = SP.
Table 70 shows all attributes of VSG_SP.
a
nw
https://www.doc88.com/p-74754903218494.html 94/151
```


## File page 095

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q

IEC 61850-7-3:2010+AMD1:2020 CSV -93-
© IEC 2020

Table 70 — Attributes of VSG_SP
fase [mem |e] | teem [re
[sat | vaseass |S? [tq | The vabe of sate wong [M_|
[| waswratss [0c | | mones vor: saopimiecos [0 |
[a | voeomass [00 | | ewe tom: easPrnavacoe [|
[anne | vasweaess [ex [| mone vans saspummecoc [Oo _|
[aman | vasurass [ex | [wat tan aeaPintecoc | won|
7.6.8.4 Visible string setting (FC=SG) (VSG_SG)
This common data class shall be used to represent visible string settings with FC = SG.
Table 71 shows all attributes of VSG_SG.

Table 71 — Attributes of VSG_SG
[ase [memo [re] ag [ soem mee en [re
[sear | vasoess [80 | | Terabe oft wane eotng ———‘[M—_|
[se _[waswrasss [oc |_| meee wore saerimiecos [0 _|
[au | Uneoseass | 00 |_| era tom: BasePunewecoc 0 _|
[canine | vases [ex | | eed tom easPanavaco [|
ee
7.6.8.5 Visible string setting (FC=SE) (VSG_SE)
This common data class shall be used to represent visible string settings with FC = SE.
Table 72 shows all attributes of VSG_SE.

Table 72 — Attributes of VSG_SE
fre | mew [e [ig | emer [re
[sai [vasureass [se | | Te vabedine sims sing [w _|
[= [vasiszss [oc | [meee tors saspimtecoc [0 |
[ao _[ oneness [0c |_| mes tors saepimieco _[o_|
[care | vasoneass [ex | | mowed tom: easPinavacoe [0 _|

a
“a
https:/Awww.doc88.com/p-74754903218494.htm! 95/151
```


## File page 096

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark ¥ Annotations v Q
-94- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
a ed
ee
7.7 Analogue settings
7.7.1 General
This subclause defines all the common data classes for analogue settings. Analogue settings
are of type analogue or group of analogues. For further details see the chapter about status
settings.
7.7.2 Analogue setting
772A General
This subclause defines the common data class for analogue setting. Analogue settings are of
type analogue or group of analogues. For further details see the subclause on status settings.
BasePrimitiveCDC
+d: VisString255_DC [0..1]
+ dU: Unicode255_0¢ [0..1]
+ cdcName: VisString255_6X [0.1]
+ _dataNs: Visstring255_€X [0.1]
constraints
[Modatans)
+ units: Unit.CFdehg (0.1)
+ SVC: SealedValueConfig CF_dehg [0.11
> minVal: AnalogueValue_CF_dchg [0..1}
+ maxVal: AnalogueValue_CF_dehg [0.1]
= _stepSize: AnalogueValue.CF.dchg [0..1] = 0...maxVal-minVal)
‘constraints
(MFscaledAVi
Figure 28 - Class diagram ASG::ASG
Figure 28: This diagram illustrates the specialisation of the analogue setting CDC.
7.7.2.2 <<abstract>> Analogue setting (ASG)
Abstract type, holding attributes common to analogue settings.
a
nw
https://www.doc88.com/p-74754903218494.html 96/151
```


## File page 097

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
IEC 61850-7-3:2010+AMD1:2020 CSV -95-
© IEC 2020
Table 73 shows all attributes of ASG.
Table 73 — Attributes of ASG
eer | mem [eg] terme mee [mom
So a lal Se
‘stepSize’.
‘ScaledValueConig Configuration for scaled value
representation (‘setMag’, 'minVal’,
‘maxVa, ‘stepSize).
[ava | maoavane | oF [a [woman song or bona +f |
ee
bee eee ld =e P|
between the individual values of
‘setMag’.
[= | vasensass [00 | | ered tom: eaePamavacde ‘|_|
[oo |Urenonss [oo |_| meres ors saerimtecoc—*[O—|
[are | wasweazss [x [| mone vans saspimtecoc [Oo _|
ee
7.7.2.3 Analogue setting (FC=SP) (ASG_SP)
This common data class shall be used to represent analogue settings with FC = SP.
Table 74 shows all attributes of ASG_SP,
Table 74 — Attributes of ASG_SP
[eer [mem |e] | cere neen [rom
[seniog _[orsomievauecs [S| aa | The vabe o ne ange ating [uw _|
DataAttribute for configuration, description and extension
[i a Kl alec ll
[neva | Aracoovane [oF [ay [moms vom asa ——SSC*dt =
ee
[sesce | Arsonavaue [oF [ng | mowes von: 5a ————S«dtO i
[| vasonesss [00 | | ewes tom eashinaacd [0
[oo _[unensarss [0c [| mess vars saspimtecoc fo _|
ee
[sons | wasurgass | © | |e tom: BasePinevecc | won|
7.7.24 Analogue setting (FC=SG) (ASG_SG)
This common data class shall be used to represent analogue settings with FC = SG.
a
“a
https:/Awww.doc88.com/p-74754903218494.htm! 97/151
```


## File page 098

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
- 96 - IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
Table 75 shows all attributes of ASG_SG.
Table 75 — Attributes of ASG_SG
ese | mem [R[x meme [rem
[seo —[Arsonavauect [55 | | Te vn fe wmnin soma [Ww _|
[units unt fo | cen [innestes tom: aso
[ee [serene | [se [emer se |e
[nwa | maomievane [GF | au [moma ton ass——SCSC~idr =
[naw | Araooavaue [oF [og | mows von asa———S«fO
| senszo [Araioguevane | cr | at [interted tom: asco
[a __|waswraess [0c |_| menos vor: eexPimnecse [Oo _|
[a [oneness [oo [| moos ton saopimwecoc [|
[nana [vases [ex |_| motes tons Saspimiecoc [Oo _|
ee
7.7.25 Analogue setting (FC=SE) (ASG_SE)
This common data class shall be used to represent analogue settings with FC = SE.
Table 76 shows all attributes of ASG_SE.
Table 76 — Attributes of ASG_SE

[ase [memo [re] ag seme [rm
[ atantibute for setting
[sebieg | Ariogmvauect [© |_| The abo o i aramove etn [|
[ts unt oF cet Tinnortes tom: aso TO
fe [seesworts [|e [monet se eee
[raver | Araoovoue [oF [ow [momo ton ass «if_id
[newer | Aaoonvaue [OF [ety [mews ton aso ——SC=dt OS
| sepsizo | Anaioguovatie | CF | eng | innerted tom: ASG tO
[= |vasurss [oc | [wat ton: uaPinecoe —[o_|
[a _| neoieass ‘(00 |_| heres ton: BasePonmecoc 0 —_—|
[canine | vasuneass [ex | | mowed tom BasPimavecoc [0 _|
[ne | vastness [ex | | ered fom BasPamavecoe | MO

a

cy

8

“a

https:/Awww.doc88.com/p-74754903218494.htm! 98/151
```


## File page 099

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark ¥ Annotations v Q

IEC 61850-7-3:2010+AMD1:2020 CSV -97-

© IEC 2020

7.7.3 Setting curve

773A General

This subclause defines the common data class for setting curve setting.

BasePrimitiveCDC

+d: VisString25S_0¢ [0.1]

+ dU: Unicode255_DC [0.1]

+ cdeName: VisString25$_EXx (0.1)

+ dataNs: VisString255_Ex [01]

constraints

{MOdataNs}
+ setCharact: CurveChar_sP_dchg | [> setCharact: CurveChars¢ | [+ setCharact: CurveChar SE
+ setParA: FLOAT32_SP_dehg [0.1] ] + setParA: FLOAT32.SG 0.1] | |+  setParA: FLOAT32_SE [01]
+ setParB: FLOAT32_SP_dchg [0.1] | | +  setPar8: FLOAT32_SG 0.1] | |+ setParB: FLOAT32_SE0.1]
+ setParC: FLOAT32_SP_dchg [0.1] | | + setParC: FLOAT32_SC [0.1] + setParC: FLOAT32_SE [0.1]
+ setParD: FLOAT32.SP.dchg [0.1] | |=  setParD: FLOAT32.SG(0.1] | |+  setParD: FLOAT32_SE [0.1]
+ setParE: FLOAT32_SP_dehg [0..1] | |+  setPart:FLOAT32.SG[0..1] | |+  setParE: FLOAT32_SE [0.1]
+ setParF: FLOAT32_SP.dchg [0..1] | [+  setParF: FLOAT32.5G(0..11 | [+_setParf: FLOAT32_SE [0.1]

Figure 29 — Class diagram CURVE::CURVE

Figure 29: This diagram illustrates the specialisation of the curve setting CDC.

7.7.3.2  <cabstract>> Setting curve (CURVE)

Abstract type, holding attributes common to setting curves used in protection equipment. It

allows the selection with the attribute 'setChar one of up to 48 predefined curve characteristics

x = f(y). The currently used curve may be read from the device using a dedicated data of the

cbc csp.

There are 3 options to configure the curve x = fly):

1) ‘setCharact’ = 1...16: As a formula based on up to 6 parameters: A, B, C, D, E and F. The
formula is standardized by ANSI or IEC, who also specify the values for A, B, C, D, E and
F. The corresponding attributes ‘setParA’, ... ‘setParF’ are read-only.

2) 'setCharact' = 17...32 (polynom): As a definable formula based on up to 6 parameters A, B,
C, D, E and F. The corresponding attributes ‘setParA’, ... 'setParF’ may be modifiable. The
specification of the formula is a local issue.

3) ‘setCharact' = 33...48 (multiline): As a definable curve specified as an array of n (x,y) pairs.
The specification of the array can be performed using data of CDC CSG where applicable,
otherwise it is a local issue.

If no curve is configured, ‘setCharact’ shall be set to 0

a
“a
https:/Awww.doc88.com/p-74754903218494.htm! 99/151
```


## File page 100

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
-98- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
Table 77 shows all attributes of CURVE.
Table 77 — Attributes of CURVE
rs | mee [e [| emer [mem
[s[vasureass [oc | [wees van: ewarintecoo [oO _|
[oo | wneuass [06 | [wert an ssopienecoc [|
[stare | vases | x | [mot tons asoPinewcoc [Oo _|
[mans [vestiges [ex || eed tor serimivecoe | woanans_|
7.7.3.3 Setting curve (FC=SP) (CURVE_SP)
This common data class shall be used to describe setting curves with FC = SP, used in
protection equipment.
Table 78 shows all attributes of CURVE_SP.
Table 78 — Attributes of CURVE_SP
[ase [meme [re] | semen [rm
a
[spun [roarse | sP [ ace | setiog tor parameter Af
[sexo [nome | 9° [ao | some tr pares
[sarc _[rioarse | s* | ao | Senng tor saamewrc ——~SCido =
[sea [ome | s* [aay | some tr paanr Sit
[sewer [rome | 5° [a2 | Seung pera «df
[seear [ome | 9° [a2 | soto tr para «tO
[< | vaswreass [00 | | weed tom: SaoPaninecos [0 |
[ai [unwomass [oc | [writ tan: eaarontecoc [0 _|
[cone | vasuraess [ex |_| mows tom SuePinavecoo [0 |
[ene | vesioass | | [torn ton seoinevecoc | won
7.7.3.4 Setting curve (FC=SG) (CURVE_SG)
This common data class shall be used to describe setting curves with FC = SG, used in
protection equipment.
Table 79 shows all attributes of CURVE_SG.
a
“a
https:/Awww.doc88.com/p-74754903218494.htm! 100/151
```


## File page 101

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
IEC 61850-7-3:2010+AMD1:2020 CSV -99-
© IEC 2020

Table 79 — Attributes of CURVE_SG
fs | mem [el | me [
[scent [Gunecains [sa | [Knduteane iw —_—|
[sewn [ome [sa | | Song wr pans ———S=dtO
[seeso [nome [90 | | soto pera «if
[wuc [Rone [sa | | song tr panne SiO
[1600 [ome | so | | somo tr panmor it
[seeue[rioare [ss | [seme wr para ito —_—|
[sero [nome | s0.| | soto rar «dt
[« | vaswreass [0c | | ortaa tom: eaoPunmecos [0 |
ee
[senare | vasraess | x | | ree tor enerimtecoe fo _|
[sane | vesgass | | [ern on seoinenecoc | won
7.7.3.5 Setting curve (FC=SE) (CURVE_SE)

This common data class shall be used to describe setting curves with FC = SE, used in
protection equipment.
Table 80 shows all attributes of CURVE_SE.

Table 80 — Attributes of CURVE_SE
ll
[secre [OnwGwna [8 | [tnaotteane iw
[wean [Ronme [se | | eta tr panera ———SCit Oo —d
[meee [none [se | | sotna tr pearewra «ito =|
[seeac [nome | se | | sono tr panei cit
[saruo [rox (se |_| Seng rseaneer0—~SCSC~*d =
[eae [ome | se | | sono  panmiore =i
[sear [nome | se | | sore poner =i
a
[2 | woenass | 56 [ [wo tan asoinenacoc [|
[sso | vasurass [ex | [wat ton: uarintecoc [0 |
[ene | vaswreass | x | mtr ons aeoinevecoc | MORAN

a
cy
8
nw
https://www.doc88.com/p-74754903218494.html 101/151
```


## File page 102

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark ¥ Annotations v Q
— 100 - IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
7.7.4 Curve shape setting
774A General
This subclause defines the common data class for curve shape setting.
cy,
CoreAbstractCDCs::
BasePrimitiveCOC
+: Vissering2S$.06 [0-1]
+ aU: Unicode2S5_0¢ [0.1]
+ cdeName: VisString?S5_& 10.1]
+ _dataNs: VisString?55_€X10..11
snes: Unit CF dehy
+ YUnies: Uni CFadehg
= Units: UnitCFdchg [0.1]
+ maxPts INTIGU_CF dehg = 2.
+ x0: Visstring2s5.0¢
+ xOU: Unicode255_0¢ [0.1]
+ vO: VisString255_0C
+ YOU: Uricode255_0¢ [0.1]
‘+ 2D: VisString2SS_DC [0.1]
‘+ _2DU: Unicode255_0¢ [0.1]
+ mumPts:INT)6U_SP_dchg = 2.maxPes || nurmPes:INTIGU_SC = 2.maxPts + numPts: INT) 6U_SE = 2.maxPre
+_crvPts: Point.SP_dchg [1.*] + _crvPts: Point S611_“1 + _crvPts: Point SE1.1
Figure 30 - Class diagram CSG::CSG
Figure 30: This diagram illustrates the specialisation of the curve shape setting CDC.
7.74.2 <<abstract>> Curve shape setting (CSG)
Abstract type, holding attributes common to curve points and curve settings. Using data objects
of this type allows two-dimensional and three-dimensional curves to be defined, as well as
polygons (two-dimensional surfaces) and three-dimensional surfaces.
a
nw
https://www.doc88.com/p-74754903218494.html 102/151
```


## File page 103

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark ¥ Annotations v Q
IEC 61850-7-3:2010+AMD1:2020 CSV —101-
© IEC 2020
‘cevPts (2)
y’
evts (0).
x
ec 2ss2779
Figure 31 - Two-dimensional curve
Figure 31: This diagram illustrates a two-dimensional curve. The curve is created by the
connection of ‘crvPts[i]’ with ‘crvPts[i+1]' with 0 < i < ‘numPts’.
a crv0 (CSG)
eo)
y
pointz —> 2
IEC 288379
Figure 32 - Three-dimensional surface
Figure 32: This diagram shows how a three-dimensional surface can be created with multiple
instances of a CSG data object. In that kind of usage, ‘crvPts.z' is ignored, and attribute ‘pointZ’
is used instead to represent the value of the curve on the z-axis. The three-dimensional shape
is created by connecting the curves with each other.
Table 81 shows all attributes of CSG.
nw
https://www.doc88.com/p-74754903218494.html 103/151
```


## File page 104

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
-102- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
Table 81 — Attributes of CSG
= EI che
a
ee a
[2m [oe ior [om [Unt otrezaistaane fo |
(range=[2...]) Maximum number of
elements available in ‘orvPIs\}.
Description of the value of the x-axis of
a curve.
ie Gl =i
a curve in Unicode.
(ba Go eae
a curve.
a a a =a
a curve in Unicode.
a a =e
acurve.
Ge Gl = atl
@ curve in Unicode.
a
[a [unenouss [bo [| mes won saepimnveco [|
[cane | vaswreass | © | [teria tom: asoPunmmecos [0 |
[caine | vastness [x |_| ered fo: asPimavacoe | MO
7.7.4.3 Curve shape setting (FC=SP) (CSG_SP)
ms common data class shall be used to hold curve points and describe curve settings with FC
Table 82 shows all attributes of CSG_SP.
Table 82 - Attributes of CSG_SP
[se [somo [|g | soe mre mene [roe
[ponz [Rowse [5° | om [Poston ot tw ane orem [0 |
Gal aa lad ===
elements used in ‘crvPIsi.
ARRAY 0...maxPts-1 ‘The array with the points specitying a
OF Point curve shape.
DataAttribute for configuration, description and extension
a
[une [um =i | tw | ome tom cso
a a
[mars [wry | oF | ara [ions ton cso Swi
ry
Aa
https://www.doc88.com/p-74754903218494.html 104/151
```


## File page 105

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
IEC 61850-7-3:2010+AMD1:2020 CSV — 103 —
© IEC 2020
Pace [seme [e |g | some meme [roe
a
[ou [oneness [oo [| memes von css —SSSC*i =
fe _|wasureass [0c | [ema ton cso Swi
[you | vncomss [oc | [heme tom csa——=SSC=idt
ee
[200 _[unenswss [oc [| mones von css ——«dtO =
[3s | wasureass | 0c | | emia tom: easoPineecos [0 |
[a [oneness [oc [| mens vans Saspimtecoc [Oo _|
[canine | vasuneess [ex | | ees tom: eaaPinavecoc [0
[mans [waswnatss [ex |_| motes von: Sesepimivecoc | won|
7.744 Curve shape setting (FC=SG) (CSG_SG)
This common data class shall be used to hold curve points and describe curve settings with FC
Table 83 shows all attributes of CSG_SG.
Table 83 — Attributes of CSG_SG
mer | Mee wwe Fe |p | teense my owen [Pens |
L ataativate for setting
[anz [Roars [80 | | Poston of he ne on zum «iO —
a i a === ea
elements used in ‘crvPts{l.
(oa "Selo "se
OF Point curve shape.

om [um or [orm [moms tom sai
a
[ame [uw [oF [ors [menos von css ———S«ifO id
[mere [wigs [oF [aw [ewes ton cso Sid
[0 [waswssss [oc |_| menes von css ii
[ou | unessrss [oc |_| menos vos css —SSSS*dt id
[ye | vasereess [oo | [meme rom cso
[ou [unensass [oc [| mones vans esa ———S—=dt id
[0 | vastness [0c | | weaea ton csa——=SCSC=*idt
a
Ce
[ai | neowass | 0c | | ema tom: asePunmwecoc [0 _|
[canine | vastness [ex | | heed to easPinavacoe |_|
[ne | vasuneess [ex | | rt tom eaaPinavacoe | Moan

ry

cy

8

an

https://www.doc88.com/p-74754903218494.html 105/151
```


## File page 106

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark Y Annotations ¥ Q
-104- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
7.745 Curve shape setting (FC=SE) (CSG_SE)
This common data class shall be used to hold curve points and describe curve settings with FC
= SE.
Table 84 shows all attributes of CSG_SE.
Table 84 — Attributes of CSG_SE
Smet | fmm ome |e] | _ tame) ovrwin | psc
[pone [Rowe [se | [Posten of tare rram [0 |
al al ==
elements used in ‘crvPts(y.
Gia El al ="
OF Point curve shape.
a
a a
a
[sire [wre | oF | ag | owes sew
a
[soo | views [oc | [ms wonscso df
[pe | vases [06 | | meues tons
[sou | Wsenass [06 | | momee ness =f
[2 | vasurgass [oc | [wens ness ito
[200 | vocomass [oc | _[rmeies vom cso ——SC*dtO =
a
[ai | neoaass | 00 | [whet ton asoPinewecoc [0 _|
[esene | veeass [ex | [tas tan: saaPimiecoc [0 |
[iis [wastes | © || eae tom SsePimiwecoe | woanan_|
7.8 Description information
7.8.1 General
This subclause defines all the common data classes for description settings. Description
settings can be of any type and represent descriptive information like a name plate. Description
information has the functional constraint DC.
For applicable services, see Annex B.
a
nw
https://www.doc88.com/p-74754903218494.html 106/151
```


## File page 107

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
IEC 61850-7-3:2010+AMD1:2020 CSV — 105 -
© IEC 2020
[:tase-cocoescrtstton )
CoreAbstractC0Cs::
+ 4u- Uniode2S5.06 1.11
= cdevame:Vissring?58.6110-1]
dena: Vinsering255 010.11
+ heer: Vistring258.0C 10.1) + pote: VaString2 88.06 + x0: Visiring265.0
+ suey: VsSring?$5_0C 10.1) + confighe vistringz$5.0€ 10.11 | |= xoU:unicoda2$8.0€ 12.1)
+ sertom: VisSering258.06 10.12 + paramew:iurs2.st-dcha 0M | |+ yunts: Unt.oc
+ mod Vistvng255.0¢ 10.1) ite: wera2 ST dc [1 = YO vasering?S5,0¢
+ locaton: Vsering285.0C 0.1) | |+ tas: Vsering2 $5,010.11 + YOU: Unicode255.0C 10.11
+ name: VisString64.DC 10-11 > fe: Viner? $5010.11 > una Unit_DC 18.11
+ owner: VsSring?38,0€ [0.1] + 20: Vasering2S5.0¢ 1.11
+ itm: Vistring® 98.06 10.1 >:D: Unicodn255_0¢ 1.11
; ore a :
+ tcondOper:Vestring255.DE B11 > eras: Point DEN ot
+ laueude: FLOAT32,0¢ 10.1) 2 Saupe WT16U,CE. eg = 2
+ lengiude: ROATI2_0€ 10.11
+ attude: FLOATS2,DC [0.1]
+ mo: Visering2$8.0€ 10.11
Figure 33 - Class diagram CDCDescription::CDCDescription
Figure 33: This diagram shows all description CDCs defined in the standard with supertypes
that factor their common attributes.
7.8.2 Device name plate (DPL)
This common data class shall be used to identify entities like primary equipment or physical
devices.
Table 85 shows all attributes of DPL.
Table 85 — Attributes of DPL
= cae cad
[vendor | viswingass [0c [| Name of the vendo,
[meee | vesigass [0c | | Hacmave wvaun ———SS=dtO
[ter | veswreass | 0c | | sotwae son ——SSS«dtO
[ sonwum | vswingass [oc [ [sera nmbe
[rates | Wasvrass | 0c |_| Vendor susie pi care io =|
Location where the equipment is
installed.
VisString64 The name of the IED (if DPL is used in
the context of a LPHD) or of a device
like @_circuit breaker (if used for the
data EEName).
[owner | wsuingass [0c | [owner othe deve, JO
a
nw
https://www.doc88.com/p-74754903218494.html 107/151
```


## File page 108

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
— 106 - IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
ume | Aue ome Fe |p |_ teenie me oct [Pees
VisString255 Name of electric power system the
Gevice is connected to.
[penne | vases | 6 |_| nary operate oft ons [|
[eonsoper | vasigees [00 | | econ opoatr ot he ance‘ |
FLOAT32 Geographical position of device in
WGS8¢ coordinates ~ latitude,
longitude FLOATS2 Geographical position of device in
WGS86 coordinates — longitude.
FLOAT32 Geographical position of device in
WGS84 coordinates ~ altitude,
=e ere
identiication of an asset or device.
[=| wesereass [66 | [mons vans essorinivecoc [|
[oo | Wneuass | 06 | [worn tan asoPinnecoc [|
[stare | veseeess [x | [mom tons BxoPineecoc [|
[ene | vesieass [| [rte ons aseinenecoc | WORN
7.8.3 Logical node name plate (LPL)
This common data class shall be used for nameplate information of logical nodes.
Table 86 shows all attributes of LPL.
Table 86 — Attributes of LPL
[ase [memo [re] ag [ soem [rw
Uniquely identifies the parameter
revision of a logical device or logical
node instance, ‘paramRev’ has to be
changed at least on any change of a
parameter (FC=SE or FC=SP) within
this logical device or logical node. How
this is detected and performed is left to
the implementation. For further details,
see Annex C.
The value change of ‘paramRev’ shall
be done as follows:
= if the parameter change is done in
the IED only through communication
services or through the local HMI,
this valve shall be increased by
one;
~ ifthe parameter change is done in
the configuration file, this. value
shall be increased by 10 000.
st Uniquely identifies the revision of the
configuration values (FC= CF) in a
logical device or logical node instance.
‘valev’ has to be changed at least on
any change of configuration values for
this logical device or logical node. How
this is detected and performed is left to
a
an
https://www.doc88.com/p-74754903218494.html 108/151
```


## File page 109

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
IEC 61850-7-3:2010+AMD1:2020 CSV — 107—
© IEC 2020
fase [seem [ef | come meme [roe
the implementation. For further details,
see as well Annex C,
The value change of ‘valRev shall be
done according to the same rules as for
‘paramRev’.
| vendor | vistingass [0c || Name ot the vendor,
| swfev | vissuingass [0c || Sonware revs,
VisString255 Logical device name space, for
example "IEC 61850-7-4:2007B8". For
Getails see IEC 61850-7-1. If present,
the value shall be initialized through the
SCL configuration file to a valid
standardized name space.
Logical node name space. For details
see IEC 61850-7-1. If present, the
vaiue shall be initialized through the
SCL configuration file to a valid name
space.
[ao _[ oneness [oc [| meres tons saepimiecoc [|
‘configRev Uniquely identifies the configuration of
a logical device instance.
'LLNO.NamPIt.configRev’ has to be
changed at least on any semantic
change of the data model of the logical
device that may atfect interpretation of
the data by the client. How this is
detected and performed is left to the
user. For further details, see Annex C.
ee
[omnes | wasureass | | [era ton: BasePunevecoo | wasn
7.84 Curve shape description (CSD)
This common data class shall be used to hold the shape of the curve currently used for, e.g.,
protection settings. The curve is created by the connection of ‘crvPts|i' with ‘crvPts[i+1]' with 0
<i <‘numPts’.
Table 87 shows all attributes of CSD.
Table 87 - Attributes of CSD
fear | mem |e] | meme [rom
DataAttribute for configuration, description and extension
a
Sc lalate
acurve.
Unicode2s5 Description of the value of the x-axis of
a curve in Unicode.
a
a
“a
https:/Awww.doc88.com/p-74754903218494.htm! 109/151
```


## File page 110

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
-108- IEC. 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
ea Gd
CS Gl = ada
acurve.
La a ==
a curve in Unicode.
[ame [uw [oe | | Untottesaisotecne fo
a a =a
acurve,
Unicode255 Description of the value of the z-axis of
a curve in Unicode.
= FT ema
elements used in ‘crvPtsif.
Ga El l="
OF Point curve shape.
Sc ad ==
elements available in “orvPIs].
Cs
[oo | Wneueass | 00 | [moss tons Baseinenecoc [|
[canane | vasureass | © |_| ema tom: BasePinewecoc [0 _|
[ssine [vrs [© [| here tar soPimivecoe | woman
7.8.5 Visible string description (VSD)
This common data class shall be used for description information that is typical for displaying
purposes such as human-machine interface, and not for automation purposes. Automation
functions rely on CDCs that hold strongly typed values (e.g., ASG with setVal of type
AnalogueValue, as opposed to VisString).
Table 88 shows all attributes of VSD.
Table 88 — Attributes of VSD
rer | mem [ag] erm [mem
DataAttribute for configuration, description and extension
[rs | vesgass [6c | | oompion ann ve
[= | weseass | 06 | [mons tans aropinenecno [|
a
[stare | vases [x _| [mote tons Binewcnc [|
[ene | veseass [© | [mera ons Bseeinenecoc | moan
7.9 Common data class specifications for service tracking
7.9.1 General
This subclause defines all the common data classes for service tracking.
NOTE Although all service tracking CDCs have a number of common attributes, they have different order when
inherited, and are therefore not defined within the common abstract class.
a
“a
https:/Awww.doc88.com/p-74754903218494.htm! 110/151
```


## File page 111

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark Y Annotations v | Q
IEC_61850-7-3:2010+AMD1:2020 CSV - 109 -
© IEC 2020
[tase cocserviestreckine 7
CoreabstractC0Cs::
= du: vncoan255.De 10.11
> bia Obpctnteremcn Rtn derma Visering? 5.01181
2 FervceType: bervcovae Sk + devas Vastrng? 88. 000.1
+ trorcode Sarvasatn 5t
= | Li |
+ eTeertame st
> caeiuar Uncada2S55 1011
+ iver onse
> Sperm: Timestamp.
> Stic Ongar sk
> Ginn nau sn
: _ =
2 rer BO0uAN.S
> Check CheckCondions St Tobe Oberon tae
> ratpAddCusne ContretSarasates 8 ? See
a a
+ Srigiatord Oca
banca! Z +e Tonatame 5k
| _ ar oe —
(+ chief Obyectteference SR dupd
: Seems —
+ Sripnator: Od SB-11 = cael Octave dup
2 eTimetame + SerwesTypn Service. St
2 Cartier Unicode2S5 5121] 2 crrrcode servant 3k
+ mmotse TU SR + oinator Os 0-1)
> See: pray se + Timestamp St
2 Gorse nerau sn > coroner Uniode2$5.5R 1.11
+ Safed: BOOLEAN SR 2 peo: Vaserng) 29.58
> teri: Tews 5k + Feta: POOL St
2 rents: 60 18.11 diet Obpctatorenee 3A
> conte: era2U.38
— =
+ Sore nerazusn
T etiRat Onecare SR dupa sare
+ serviceT ype: ServicwName SR + melee Tee,
> errercode Sarveaiate 3k > tg eau.
+ origmatoriO: Ccten64_s8 10.1], pe aber)
+e Timestamp. St ¢ cmrastek SOCLIE
+ én, BOOUAN SR + Greyo: ery st
> fonsoouen + imwOtnry tryin
> cave: Vsering 28.58 2 renvimn 16 SRID A
> deter Obctatarnce Sk + Sonar Oct-80-11
> contre era2UR
> Seoltod Samatingode A
: =
> Speide svitentageopion Sk
2 Sieaddress Prvcomaa sk = cael Once A dond
> tarerType Servicohame, Sk
+ at Onctatercestdued | aes =e Timestamp. Sk
+ sarviceT ype: ServiewName SR + cbjRat ObjectRaterence SR dupd | | certisswar: Unicode?S5_sh (0.1)
+ wrrorcode Serveitaen sk || ehfah Obeitereen shaun | | verviceType: Servicehame sk || epi Varig 29.38
+ Srpmaono: ocwes4.ski0.1} || > servextype servewname.se | |= ervorcode servcestans sk | |= roténa: SOOLEAN.SR
+ Timestamp. + trorcodeserveasians sk || ccgintord: Ocweeeste-1) | |= ‘env 8OOLEAN. SR
> cartnear Unicoda255.5810.11 |] Srigiatorto Octete sai0.11 |] +e Tomatamp 5k + decee Obpestlarnea
> Seana sooUAN-SR + Tween 5k + certnnver Umicode2ss.sa00.10 | |> confer 9er320.58
= vo: Vesting 28. > crasuar Unieoderss.se 0.1} | }= logena BOOUAN SR = Sports nceneportOpuons sk
+ Geese Objctaarenca se || getne BocurAN st = loghel Objctatranca > Serr nraaucsn
> contre era2U-38 + Soo: Viasering 293k + deer Objctaereen ak > ore THUR
2 epnave: 858 + Sacer Obecteterence st + cldeerte treryTine + Uy0p: ThggeCondons sk
+ coud svatstageoption.st || cont: rs2U 58 > ent: try SR 2 Ste rasa.
+ Stplted Sampinghtode || > nduCom: BOOUAN SK + Sh: erik > pe aoouan se
2 Geraddres Prycomadirse || Sieaddress reycomadérse | |= rewtne errvio-sk + Sane: cet SR10.11
> troOpe:FrioperCondions, sk
+ ingh maa sk
Figure 34 - Class diagram CDCServiceTracking::CDCServiceTracking
Figure 34: This diagram shows service tracking CDCs defined in the standard with supertypes
that factor their common attributes.
Depending on the value of ‘serviceType’, the tracking data object allows to track the service
parameters used within a given service, or the internal change of a control block and the
associated changed control block attributes.
°
an
https://www.doc88.com/p-74754903218494.html 111/151
```


## File page 112

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark ¥ Annotations v Q
-—110- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
In the case of a tracking data attribute that mirrors a read-only service parameter (not present
in the SetXX service), the value in the tracking data attribute reflects the value that would have
been returned by the GetXXX service on the control block referenced by ‘objRef at the
completion of either the SetXXX service ('serviceType'=<service-name>) or internal change
(‘serviceType'='InternalChange’). All other tracking data attributes mirror the service parameter,
even the wrong ones (the wrong parameters lead to a negative response from the service).
An optional attribute in a tracking CDC is optional at a data model level, i.e., it shall be included
in the CDC if the corresponding control block has them.
For applicable services, see Annex B.
7.9.2 Common service tracking (CST)
This common data class shall be used for tracking of all the services for which no specific
tracking CDC has been defined; its base structure is repeated in all specific tracking CDCs.
Table 89 shows all attributes of CST.
Table 89 - Attributes of CST
pase [mem IR] g | meme [rem
ObjectReterence Reference of the object being accessed
by the service. For control block related
services, this is the reference of the
control block whose attributes shall be
tracked. For control services, it is the
relerence of the controllable data
object. For other services (generic
tracking), this Is the reference of the
object (DataSet, DataObject,
DataAttribute, ...) whose access shall
be tracked,
a
ServiceStatuskind sa Return status of the service specified
by ‘serviceType’.
service. See further requirements in
IEC 62351-6
[rene [sa [| Tiree oe sais cons]
ere
IEC 62351-6.
a
[2 | wseness | 56 | [moe tan asPrnmecoc fo |
a
[enmne | vanes [| | tas van exsPimmecoc | wos |
7.9.3 Buffered report tracking service (BTS)
This common data class shall be used to track following services dedicated to a buffered report
control block access:
— 'serviceType’ = 'SetBRCBValues’,
a
nw
https://www.doc88.com/p-74754903218494.html 112/151
```


## File page 113

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark Y Annotations ¥ Q
IEC 61850-7-3:2010+AMD1:2020 CSV - 111-
© IEC 2020
— ‘serviceType’ = ‘InternalChange’.
Other buffered report control block services are not tracked.
Table 90 shows all attributes of BTS.
Table 90 — Attributes of BTS
|_Sroe’ | “mew nme | Fe | Op |_taimnaie owe onion | scot
[coher | OvpetRotwerce | SR | owod | Sev CSTobRet Sidi
| serviceType | ServceNamekind | sR_| | See CSTserviceType. TM
| emrorCode | ServiceStatuskind | sR_| | See CSTemorCode tM
[eignaiond [oxen _| | See cSTorgraio.——~SC~d OO —|
[1 | Twestane [sa || swosts —SSCSCid id
[coisoer | Uneoseass | sn |__| Sev cSTontiewer ——=SC~dCC*
VisSiringt29 Mapping for service parameter
'SelBACBValues. Reportidenttier,
Mapping for service parameter
"SetBRCBValues.ReportEnable’ and
internal change in attribute
"BRCB.AptEna’ (report enabled at loss
of association with the client).
ObjectReference Mapping for service parameter
'SeIBRCBValues, DataSetReterence’
[conv | wTa@u | S| | Manning for atibte BACB Cone. |W —_|
optFids RCBReportOptions Mapping for service parameter
Mapping for service parameter
'SelBRCBValues.ButferTime’.
| savum [riya |_| Mapping tor attribute BRCB.SqNum. | M_ |
‘TriggerConditions Mapping for service parameter
'SelBRCBValves. TiggerOptionsEnable
¢.
Mapping for service parameter
sR Mapping for service parameter
"Set8RCBValues.Generalinterrogation’.
purgeBut Mapping for service parameter
'SelBACBValues PurgeBur.
entryiD Mapping for service parameter
'SelBRCBValues.Entryidentiier.
EntryTime Mapping for attribute
"BRCB. TimeOfEntry’.
resvIms INTIC Mapping for service parameter
'Sel8RCBValues.ReserveTimeSecond’
and intemal change in attribute
‘BRCB.ResvTms' (at expiration of
Reservation).
[omer | Geese | SR | | Mapping tor atibito BACBOwer [0 |
[a | wesingass [00 |_| itortod ror easePrmteCD0 [0 |
[au_____ | Unicodeass [0c |_| inherited trom: BasePrimitveCDC | O |
[casera | vesuigass |x| | here tor: asePrimtveCOC |_|
[ctans | vissuingass | ex | | ire for BasePitveCDC | MO ata
a
“a
https:/Awww.doc88.com/p-74754903218494.htm! 113/151
```


## File page 114

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark Y Annotations ¥ Q
-112- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
7.9.4 Unbuffered report tracking service (UTS)
This common data class shall be used to track following services dedicated to an unbuffered
report control block access:
- ‘serviceType’ = ‘SetURCBValues',
— 'serviceType’ = ‘InternalChange’.
Other unbuffered report control block services are not tracked.
Table 91 shows all attributes of UTS.
Table 91 — Attributes of UTS
|_Stone’ | farms we |S |p | Gatevane reo een | Peso |
| objRet | Objecteterence | S| dupd [Seo cstobiet TM
| serviceType | SeniceNamekind | sR_| | See CSTservceTwe TM
| errorCode | ServceStauskind | SR_| | See CSTemorCode, TM
| arginatond | Octetss | SR | | See csTorignati. tO
[1 | Timestam sR | | seocsre
| conssuer | Unicodeass | SR | | See csToonissue TO
pee [wee | [Statin |" |
'SelURCB Valves. Reportidentitier.
Mapping for service parameter
‘SetURCBValues ReportEnable’ and
intemal change in attribute
"URCB.RptEna’ (report enabled, at loss
of association with the client)
"SetURCBValues.Reserve’ and internal
change in attribute 'URCB.Resv’ (state
of reservation).
ObjectReterence Mapping for service parameter
'SetURCB Values DataSetReterence’
[coniRay [Wray | SA | | Manning for atte URCB.ConRev | —_
Fill aan ===
"SetURCBVaives.OptionalFields'.
a i Gl =
"SetURCBValves.ButferTime'’
[satm [TU ————~( S| | Maing for atrbute URCESaNm: [wt __|
‘TriggerConditions Mapping for service parameter
"SeHURC Vales. THggerOploraEnatie
Lc al ==
"SetURCB Valves ntegrtyPeriog.
a Gol =
'SelURCBValues.Generalinterrogation’
[emer | Oaaee | @R | | Manning for atibvte URCB Owner. [0
[¢ | vesvigass [0c |__| iherid ror: GasePimtveCDC [0 |
[au | vricoseass [0c | | ihertog trom: BasoPrmtvecoc _[o_|
[esciame | vesuigass | ex_| | hora tor: asePrmtecoC |__|
[eats | Veswingass |_| | theres trom: BasePrmtvecDc | MOdsiaNs_|
a
“a
https:/Awww.doc88.com/p-74754903218494.htm! 114/151
```


## File page 115

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
IEC 61850-7-3:2010+AMD1:2020 CSV — 113 —
© IEC 2020
7.9.5 Log control block tracking service (LTS)
This common data class shall be used to track following services dedicated to a log control
block access:
- ‘serviceType’ = ‘SetLCBValues’.
Other log control block services are not tracked.
Table 92 shows all attributes of LTS.
Table 92 — Attributes of LTS
esr | sre [e]g | eee [mem
a
[serine | Sevnhanernd [sR |_| Sen CoTamncetipe SSCs =
[eracase | Seveasinatna [6m | | ev csteracase. iw —|
[roman [oases | | Sew cstorgramw ———~=Sidto =
[| tmp se [sees Cid
[anesse | uncowass [sa |_| Sercstamnane —=SC*=‘idt =
Gea ac al E>
"SelLCBValues.LogEnable’.
Sc Kasia al ===
'SetLCBValues.LogReference’
Sea Gl ==
'SetLCBValues.DataSelReterence’.
Sl La al ==
‘GetLogStatus Values. OidestEntryTime’
li Gl ==
‘GelLogStatus Values. NewestEntryTime"
aca Gl ===
‘GetLogStatusValues, OidestEntry’
Values.N
‘TriggerConditions Mapping for service parameter
'SetLCBValues. TriggerOptionsEnabled’
Sa Gl ===
‘SetLCBValues.IntegrityPeriod’.
[= __[wasweaess [0c | | moms vom saspimtecoe [0 |
[a _[unwomess [0 [ [wo ton suPimmacoe fo |
[rane | wasnnaess [ex | | mows von: Beepimbecoc [0 _|
[ne | vasueass [ex | [mes vom SaaPintecoc | moans |
7.9.6  GOOSE control block tracking service (GTS)
This common data class shall be used to track following services dedicated to a GOOSE control
block access:
— 'serviceType’ = ‘SetGoCBValues’.
a
“a
https:/Awww.doc88.com/p-74754903218494.htm! 115/151
```


## File page 116

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
-114- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
Other GOOSE control block services are not tracked.
Table 93 shows all attributes of GTS.
Table 93 — Attributes of GTS
Pass [em |e] | tere reser
[caer [ovesowwnen [sm [and [soo startet tw
[sevestipe | Sevewanerns [sa |_| soe cstaevestvon Si
[eracase | Sevessinstna [sn | | See stereos iw
a
Ce
[extsnow —[uncss [sn |_| sevcstamnmme ———~S«dfO id
Mapping for service parameter
‘SetGoCBValves. GoEnabie’.
= aeenmas
'SetGoCBValues.GOOSEID’.
Cal nell ll ==
‘SelGoCBValves.DataSetReterence’
[afer [wey [5 | | woo tr ara Gocaconner [Ww _|
[nmcom | pone | sn | | mapper ate ocenescons | w_
Sal ac Gal =o
‘GoCB.DstAddress'.
[so _[waswreass [0c |_| memes ton: SasePoneecoo [0 _—|
[ao [ oneness [bo [| mes von saopimiecoe [|
[canare | vastness [ex | | tered fom: BasPamavaco [|
[ene | vesureass [x | [mens tons aeeinenecoc | Moan |
7.9.7 MSVCB tracking service (MTS)
This common data class shall be used to track following services dedicated to a multicast
sampled values control block access:
— ‘serviceType’ = 'SetMSVCBValues’.
Other multicast sampled values control block services are not tracked.
Table 94 shows all attributes of MTS.
ry
Aa
https://www.doc88.com/p-74754903218494.html 116/151
```


## File page 117

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
IEC 61850-7-3:2010+AMD1:2020 CSV — 115 -
© IEC 2020
Table 94 — Attributes of MTS

= aE ciao

[caer [ovesoownen [sm [and [sow cota Sid —*|

[sevestipe | Sevetanerns [se |_| Sew cstsnvestion i

[eracase | seveesinntna [sn | | Sew csteratawe tw —|

[oman once (sR |_| Sen cstorgmon. ——SC=idtO

[| tmesump [se [| sewcste iid

[sstener[unwowass [st | [seecstastmmer =i =|

al al l=
‘SeMSVCBValves.SvEnable’.
‘SetMSVCBValues.MulticastSampleVal
veld’.

= =F) same
'SeiMSVCBValves.DataSetRelerence’

=e esse
"MSVCB.ConfRev’

a ic al ==
‘SelMSVCBValues.SampieRate’.

Cell ese al ==
'SeMSVCBValves.OptonalFields’

= lease
'SetMSVCBValues.SampleMode’.

Sal a ll
"MSVCB.DstAddress’.

[* | vasonsess [00 |_| moms tom: eaaPinavacos [|

[ao [oneness [0c |_| moos von: saopimteco [Oo _|

[eanane | wasureass | | | ema tom: BasePinenecoc [0 _|

[enane | vanes [x | [was on saaPimiecoc | wos |

7.9.8 USVCB tracking service (NTS)

This common data class shall be used to track following services dedicated to a unicast sampled

values control block access:

— ‘serviceType’ = 'SetUSVCBValues’.

Other unicast sampled values control block services are not tracked.

Table 95 shows all attributes of NTS.

a
“a
https:/Awww.doc88.com/p-74754903218494.htm! 117/151
```


## File page 118

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
— 116 - IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
Table 95 — Attributes of NTS
Pe | mem [el] me [om
| tiRet___| ovjetReteenco | SR [ound | Seo cero
[sevestion | soveotanerns [sa | soe cstaenestn tw
[cose | seweeomatea [9m | | soe cstarucoss tw
[sora [ome [sn | | ew stororamo——S—=ito—d
[| tesa [se [ [sever i
Sa a al ==
'SetUSVCBValues.SvEnable.
"SeUSVCBValues. Reserve’ and
intemal change in
"USVCB.Resv’ (state of reservation).
‘VisString129 Mapping for service parameter
‘SAUSVCBV elves. UnicantSempleValve
ee [oem [| [Steere ||
"SeiUSVCBValues.DataSetReference’.
confRev INT32U Mapping for attribute
"USVCB.ContRev.
aaa aaa al ==
"SetUSVCBValues. SampleMode’.
a i a ==
'SetUSVCBValues.SampleRate’,
Ga cial Gl ===
"SetUSVCBValues. OptionalFields’
PhyComAddr Mapping for
'USVCB.DstAddress’.
[2 | vasinss [60 | | heed tor tasimecoe [0 |
[> | vneneass | 06 | [west ton: aseinenecoc | O_|
[tare | veseeass | | [mot tans aeoinenecnc [|
[nis [vistas [© [| hee tom sahmtvecoe | woman
7.9.9 SGCB tracking service (STS)
This common data class shall be used to track following services dedicated to a setting group
control block access:
— ‘serviceType’ = ‘SelectActiveSG’,
— ‘'serviceType’ = ‘SelectEditSG’,
~ 'serviceType’ = ‘ConfirmEditSG’.
For 'serviceType’ = 'SetEditSGValue’, CST is used for tracking.
Other setting group control block services are not tracked.
Table 96 shows all attributes of STS.
a
nw
https://www.doc88.com/p-74754903218494.html 118/151
```


## File page 119

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
IEC 61850-7-3:2010+AMD1:2020 CSV -N7-
© IEC 2020
Table 96 — Attributes of STS
Pe [mem [fg] men [rm
[caer | oveaaownce [on | ana [seo cotonet iw ——_—|
[ssveston | seveonerns | sR |_| Soe cotaoventpe tw
[eco | sevessmatra [98 | | see cstarocom iw
[storai6[oanee [sa | | so ostaigrano. ‘fo |
[| rewire [sa | [severe Si
[sstener [uncomass [sr | [soecstawmmmer———S=ifo
i a=
Mapping for service parameter
= rl eens
"SelectEditSG SettingGroupNumber.
(as a consequence of service
‘ContirmEditSGValves).

[ate [Team | 6 || Wass rate Sacanarar [|
[reving[wricu | 58 | | Nip tr aban scorers [oO _|
DataAttribute for configuration, description and extension
[x | vasersass [00 |_| heres vom onaPrmeecoc [0 |
[oo | Wensess [56 | [oe ta easrenmrecoc fo |
[scare | vesgess [© | | motes tan easrrmmecoc fo |
[swans | vesoass | | mete tan asremivecoc wos |

7.9.10 <<abstract>> Control service tracking (CTS)
This common data class shall be used to track any control service applied to a data object:
— ‘serviceType’ = ‘Select’,
—  ‘serviceType’ = ‘SelectWithValue’,
— 'serviceType’ = ‘Cancel’,
— ‘'serviceType’ = ‘Operate’,
- ‘serviceType’ = ‘'CommandTermination’,
= 'serviceType’ = ‘TimeActivatedOperate’.
This class is abstract because the type of 'ctlVal’ is unknown at this stage.
NOTE The value of the attribute Check when the serviceType is Cancel can reflect the value of the latest Check
used during the service Select request respectively Operate request associated with the controllable object whose
control sequence is being cancelled,
Table 97 shows all attributes of CTS.
a
nw
https://www.doc88.com/p-74754903218494.html 119/151
```


## File page 120

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
—118- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
Table 97 - Attributes of CTS
Pe [mem [ef] mene [rm
[safer | obesaownce [om | ana [seo cotanet Siw —_—|
[seveston | seveoanetns | sa |_| Soe cotaoventne tw
[eco | seveeomatra [5a | | see cstarocom iw
[stra [ome [an | | secstoromoo Sito —*d
[| rewire [sa | [sovcss Si
a
Mapping for service parameter ‘ctVal.
NOTE The type used is different for
each data object in LTRK logical node,
and is thus defined in IEC 61850-7-4,
For this type, the placeholder “DA" is
used.
ev | ee F |
‘operTm’, Has value NULL. if the m
tracked service is not
‘TimeActivatedOperate’.
a
[anon [wry [sa | | nip tr svn parte tr [|
[7 [Teste [se || an tree panes. [Mw _|
[rer [eoouems [sn | | arn tr rove pranetr est [w |
[mk | Cmiconsions | 98 | | Mapp tr sve pane Grek [wm —_|
Fal eel kal ===
ind parameter "AddCause’
[=| weswsess [00 | [mews vom eahimmecse [0 |
[a7 | neowass [0c | [ert ton BasoPineacos [0 _|
[estene | waswesss [ex | | mes ton: eaaPimnecoe [0 _|
[ie [vases [x || eae om SsePimiecoe | woanan_|
8 Enumerated data attribute types
8.1 General
The enumerated types structure and descriptions are part of the Code Component of this IEC
standard and are available as electronic machine readable file in related NSD file.
This subclause contains explicit definitions of enumerated types used in IEC 61850-7-3; some
of them are also used in IEC 61850-7-4.
8.2 Angle reference (AngleReferenceKind enumeration)
Kind of angle reference.
Table 98 shows all enumeration items of AngleReferenceKind.
a
nw
https://www.doc88.com/p-74754903218494.html 120/151
```


## File page 121

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark Y Annotations v | Q
IEC 61850-7-3:2010+AMD1:2020 CSV —119-
© IEC 2020
Table 98 — Literals of AngleReferenceKind
|__ enumeration tem | vaio | enerpton |
are
60255-118-1
8.3 Control model (CtlModelKind enumeration)
Kind of control model.
Table 99 shows all enumeration items of CtlModelKind.
Table 99 — Literals of CtlModelKind
|__erwmaraton wom | wae | enerpton
= eee
that apply to a status object are supported.

Pa aan
IEC 61850-7-2.

‘sbo-with-normal-security S80 (select before operate) contro! with normal

security according to IEC 61850-7-2.

See |
to IEC 61850-7-2.
to IEC 61850-7-2.

8.4 Curve characteristic (CurveCharKind enumeration)

Kind of curve characteristics as used for protection functions.

Table 100 shows all enumeration items of CurveCharKind.

Table 100 — Literals of CurveCharKind
|____enwmeraton tem | vate | eserpton
a CT
[st Senay ene ~idg  e R—_—
[mis voy mene —SSCSCSCS~*dr Ci gS RR,
[ ANSI Normal iwese 8 | tseprecatesy
a
Se
[Laine Econey wee =o
[Lacatine mene eg

°
an
https://www.doc88.com/p-74754903218494.html 121/151
```


## File page 122

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark ¥ Annotations v | Q
—120- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
|_______erwmeration item | vate | eseription |
[IEC Normal verso | tseprcatesy
[ic vey mews i rg wa oR |
a
[icc eenay ewe [12 seg 1 ors 11 coc.
[ic shine ene nn
[ie tgtiw mene ug |
[ic oeints Te ag HO
Potlynom 1 Detinable curve 1 based on formula
tty ABC.D.EF).
Polynom 2 Definable curve 2 based on formula
xaHlyABCO.EF).
Polynom 3 Definable curve 3 based on formula
xfyWABCO.EF).
Polynom 4 Definable curve 4 based on formula
xfyWABCO.EF).
Polynom 5 Definable curve 5 based on formula
xaflyABCO.EF).
Polynom 6 2 Definable curve 6 based on formula
xalyABCO.EF).
Polynom 7 Definable curve 7 based on formula
xatly ABC.D.EF),
Polynom 8 Definable curve 6 based on formula
xHyVABCO.EF).
Polynom 9 Definable curve 9 based on formula
xefly,A,B,C.D.E.F).
Polynom 10 Dotinable curve 10 based on formula
xy ABC.D.EF).
Polynom 11 Definable curve 11 based on formula
xallyABCO.EF).
Polynom 12 Detinable curve 12 based on formula
x=yABCO.EF).
Polynom 13 Definable curve 13 based on formula
xlyWABCO.EF).
Dotinable curve 14 based on formula
xtly ABC.D.EF.
Definable curve 15 based on formula
x2llyABCO.EF).
Polynom 16 Definable curve 16 based on formula
xy ABC.D.EF).
TF gremerrearern |
(xy).
ce
(xy).
A A canal
(xy)
= eae
(xy).
a occ
(xy)
= arene]
by)
°
nw
https:/mww.doc88.com/p-74754903218494.html 122/151
```


## File page 123

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark Y Annotations v | Q

IEC 61850-7-3:2010+AMD1:2020 CSV —121-

© IEC 2020

| ______erwmeration tem | vate | description

SE Se
(xy).
xy).

a elie
xy).

ah ied
bey).

a GN sianiaietial
by).

SF ae
(xy).

ak GO sianiaieeitia
(xy).

ah GO cid
(ey).

a selec
(x,

a sealed
(xy).

8.5 Fault direction (FaultDirectionKind enumeration)

Kind of fault direction.

Table 101 shows all enumeration items of FaultDirectionKind.

Table 101 — Literals of FaultDirectionKind

|____ enumeration tem | vane | escent

8.6 Harmonic value reference (HvReferenceKind enumeration)

Kind of reference for harmonic value.

Table 102 shows all enumeration items of HvReferenceKind.

Table 102 — Literals of HvReferenceKind
| _____ enumeration item | vate | description
°
an
https://www.doc88.com/p-74754903218494.html 123/151
```


## File page 124

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark Y Annotations v | Q
-122- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
8.7. Month (MonthKind enumeration)
Month in a year.
Table 103 shows all enumeration items of MonthKind.
Table 103 — Literals of MonthKind

|_____ enumeration item | vatue | eseription |
[vamoary
a
a 6
a
a
[December eT
8.8 Unit multiplier (MultiplierKind enumeration)
Unit multiplier, where the value of literal equals the exponent of the multiplier value in base 10.
NOTE A value that is representing a percentage can use the unit 1 (dimensionless) and a multiplier -2 (cent).
Table 104 shows all enumeration items of MultiplierKind.

Table 104 — Literals of MultiplierKind
|__emumeration item | value | eseription
EO
a
a
a
a
[nS S*d fr egy =
ee
a
Ca
a
a

°

cy

8

an
https:/Awww.doc88.com/p-74754903218494.htm! 124/151
```


## File page 125

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark Y Annotations v | Q
IEC 61850-7-3:2010+AMD1:2020 CSV ~ 123 -
© IEC 2020
| ____erwmeration tem | vate | description
a 0)
el
[x 8 Pin
[wT mega teensy
je ga teeny
a OY
a
tT
a CD 0)
8.9 Occurrence (OccurrenceKind enumeration)
Kind of occurrence.
Table 105 shows all enumeration items of OccurrenceKind.
Table 105 — Literals of OccurrenceKind
|____ enumeration tem | vate | serpin
[rm
[ Dayorvesr
eC
8.10 Output signal (OutputSignalKind enumeration)
Kind of control output signal.
Table 106 shows all enumeration items of OutputSignalKind.
Table 106 — Literals of OutputSignalKind
| _____—erwmeration tem | vate | description
a Ce
a
[pwrtoninomck «(| al we wt Hd Sten
8.11 Period (PeriodKind enumeration)
Kind of period.
°

cy

8

an

https:/Awww.doc88.com/p-74754903218494.htm! 125/151
```


## File page 126

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark Y Annotations v | Q
—124- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
Table 107 shows all enumeration items of PeriodKind.
Table 107 — Literals of PeriodKind
|__ enumeration tem | vate | enerpton
[vo
8.12 Phase angle reference (PhaseAngleReferenceKind enumeration)
Kind of phase angle reference.
Table 108 shows all enumeration items of PhaseAngleReferenceKind.

Table 108 — Literals of PhaseAngleReferenceKind
|____enwmeraton tem | vate | desert
[vee
[ve fe
6
= are

60255-118-1

8.13 Phase fault direction (PhaseFaultDirectionKind enumeration)

Kind of phase fault direction.
Table 109 shows all enumeration items of PhaseFaultDirectionKind.

Table 109 — Literals of PhaseFaultDirectionKind

|__erwmaraton tem | vate | enerton

backward a

0

cy

8

an

https://www.doc88.com/p-74754903218494.html 126/151
```


## File page 127

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
IEC 61850-7-3:2010+AMD1:2020 CSV —125-
© IEC 2020
8.14 Phase reference (PhaseReferenceKind enumeration)
Kind of phase reference.
Table 110 shows all enumeration items of PhaseReferenceKind.
Table 110 — Literals of PhaseReferenceKind
|___ enumeration tem | vate | eserpton
a 6
[symeroprasor
8.15 Range (RangeKind enumeration)
Kind of value range.
Table 111 shows all enumeration items of RangeKind,
Table 111 — Literals of RangeKind

|__ enumeration tem | vate | eserpton |
8.16 SI unit (SIUnitKind enumeration)
SI units, with sub-categories as follows:
- 1-8: base SI units
— 9-39: derived SI units
- 41-60: extended SI units
- 61-89: industry-specific SI units.
Table 112 shows all enumeration items of SlUnitKind.

a

cy

8

an

https://www.doc88.com/p-74754903218494.html 127/151
```


## File page 128

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 1150 > @ Q_ View A mark Y Annotations v Q)
-126- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
Table 112 — Literals of SiUnitKind

|___erumeration tem | vatue | serpin |
[er tg]
[eo i
CC
[mt 7 Tle: Amount of substance |
a Ce eT
eC
ee
[er Py ha td ne |
ee a oe
es ae
ee
eT
—
[hoes Thor ci: Electric inductanco |
[en ri pr |
[ow 90 Totem (vin Electic resistance |
[yo Tse rotor tka mr ey Forco |
[om 85 Plume fed tuminous tux |
2

watt (J’s): Power

watt (I? R): Real power
[Tp 07 rs |
[ea on re |
[or fae Teac metre (my: Volum |
eC
[me | rete per secon (ist: Acceleration |
(A lohaniecnl

rate
a
[Mf a7 [tora metre eg my: Moment of mass |
a
[ms 48 motte squaretsocona (mei): viscosity |

0
Aa
https:/Awww.doo88.com/p-74754903218494.html 128/151
```


## File page 129

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark ¥ Annotations v | Q
IEC 61850-7-3:2010+AMD1:2020 CSV - 127-
© IEC 2020
es
a
conductivity
a
es
[ier rns id i i |
a Oe
[we | | at se mate: ection |
ee Saale
energy
[ae ner pr ne Bee my |
[xe |» [tein per scr Torpoire care io]
[pas [vec per scr Passe cage a |
a Ce
a Ce CT
err
Real power.
_ rr
power
a de
degrees: Phase angio.
[ eosin) | cimensioniess: Power factor |
[vs 6 voit scons Wal: Vo second |
[ve for | voit sarod (wns: Vo squared |
[as fe amp second (as): Amp second |
[ef amp square (x: Amp severed |
[arf 70 | amp squares time (Ar: Amps squared te |
[van 7 | voit amporo ours: Apparent enoray |
[who fre att rs: Real enersy
[ae Tr [ret ar race a eae cy |
a
[hes ______[ [tr st of os iy |
es er
[ewe [rn [anc pr werent |
CC
TT
oo
[we rr Yt ar ect Rare ie |
es ci
mene |
level
bo ine
a
[ommm | | eon pr mer: ais aan por rah |
rr
0
cy
8
nw
https://www.doc88.com/p-74754903218494.html 129/151
```


## File page 130

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark Y Annotations v | Q
—128- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
| ___erwmeration tem | vate | description
|
[er i Re |
8.17 Select-before-operate class (SboClassKind enumeration)
Kind of select-before-operate class.
Table 113 shows all enumeration items of SboClassKind.
Table 113 — Literals of SboClassKind
|___ enumeration tem | vate | enero
a eee
controllable data object shall retum in the
unselected state.
~~ RSE
controllable data object shall remain in the
ready state, as long as ‘sboTimeout' did not
expire.
8.18 Sequence (SequenceKind enumeration)
Kind of sequence.
Table 114 shows all enumeration items of SequenceKind.
Table 114 — Literals of SequenceKind
|___ enumeration tem | vate | eseripton
a a
positive, negative and zero, respectively.
aa a
direct, quadratic and zero, respectively.
8.19 Severity (SeverityXind enumeration)
Kind of severity.
Table 115 shows all enumeration items of SeverityKind.
Table 115 — Literals of SeverityKind
|__ enumeration tem | vate | enero
a
ee ee ee
data is considered critical and priviledged
access was attempted.
0
cy
8
an
https://www.doc88.com/p-74754903218494.html 130/151
```


## File page 131

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< /150 > QQ View A mark Y Annotations Y Q)
IEC 61850-7-3:2010+AMD1:2020 CSV — 129 -
© IEC 2020
Severity is major in terms of safe operation, or
data is considered of major importance and
Priviledged access was attempted,
Severity is minor in the sense that access
control was denied to data considered
priviledged.
[waning tes severe tan minor,
8.20 Week day (WeekdayKind enumeration)
Day in a week.
Table 116 shows all enumeration items of WeekdayKind.
Table 116 — Literals of WeekdayKind
| enumeration tem | vate | description
[tuesday
[ Mowrsday
[sunny
°
cy
8
an
https://www.doc88.com/p-74754903218494.html 131/151
```


## File page 132

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark ¥ Annotations v Q
-130- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
Annex A
(normative)
Value range for units and multiplier
NOTE These are printed automatically in Clause 8, with all other enumerations. Annex A has been kept to preserve
clause numbering,
a
https://www.doc88.com/p-74754903218494.html 132/151
```


## File page 133

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
IEC 61850-7-3:2010+AMD1:2020 CSV — 131 —
© IEC 2020
Annex B
(informative)
Functional constraints (FcKind)
From an application point of view, functional constraints classify data attributes according to
their specific use like e.g. status information, measurement, setting or description.
FC serves as a data filter in the sense of defining the services applicable to specific data
attributes of common data classes (defined in IEC 61850-7-3).
Functional constraints and applicable services are part of the Code Component of this IEC
standard and are available as electronic machine readable file in related NSD file.
NOTE The possibility to access a data attribute can be further constrained by a view, access control or an
implementation.
EXAMPLE The common data class single point status (SPS) according to IEC 61850-7-3 has the following data
attributes related to process state: stVal (status value), q (quality), and t (time stamp) with the functional constraint
ST (status information). The write service is not allowed for these attributes.
Table B.1 shows all functional constraints.
Table B.1 — Functional constraints (FcKind)
| FC | Semantic | Description (services allowed, initial values, storage)
Status Data attribute shall represent status information,
‘sformation Initial valve shall be taken from the process.
Modelling note: Applicable ACS! services:
= GetDataValves
-  GetDataDefinition
—  GetDataDirectory
= GetDataSetValues
— GetaliDataValues
— may be a DataSetMember of a DataSet referred to by any of: GOOSE control
block, report control block, log control block, sampled value control block.
Measurands Data attribute shall represent measurand information,
bones a Initial value shall be taken from the process.
Modelling note: Applicable ACSI services:
= GetDataValves
—  GetDataDefinition
—  GetDataDirectory
= GetDataSetValues
= GetAliDataValues
— may be a DataSetMember of a DataSet referred to by any of: GOOSE control
block, report control block, log control block, sampled value control block.
sP | Setting (outside | Data attribute shall represent setting parameter information
setting gouP) | ital value shall be as configured; value shall be non-volatile.
Modelling note: Applicable ACS! services:
= GetDataValves
— _SetDataValves
a
“a
https:/Awww.doc88.com/p-74754903218494.htm! 133/151
```


## File page 134

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
-132- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
[Fo | Semante | ___Deseipion (srees allowed, nal wes, strat) |
—  GetDataDefinition
= GetDataDirectory
—  GetDataSetValues
—  SetDataSetValues
~ GetAliDataValues
may be a DataSetMember of a DataSet referred to by any of: GOOSE control
block, report control block, log contro! block.
‘Substitution Data attribute shall be used to handle substitution (see IEC 61850-7-3)
Initial behaviour of substitution shall be substitution disabled.
If the substitution handling relies on non-volatile DataAttributes, then the
behaviour of substitution at restart of the IED shall be as set before the restart.
Modelling note: Applicable ACSI services:
= GetDataVaives
— SetDataValves
—  GetDataDefinition
= GetDataDirectory
— GetDataSetValues
—  SetDataSetValues
= GetAliDataValues
= may be a DataSetMember of a DataSet referred to by any of: report control
block, log contro! block.
Data attribute shall represent configuration information.
Initial value shall be as configured; value shall be non-volatile,
Modelling note: Applicable ACS! services:
= GotDataValues
= SetDataValues
— GetDataDefinition
— GotDataDirectory
= GetDataSetValues
—  SetDataSetValves
— GotAliDataValues
= may be a DataSetMember of a DataSet referred to by any of: report control
block, log contro! block,
Data attribute shall represent description (intended for humans) information.
Initial value shall be as configured; value shall be non-volatile.
Modelling note: Applicable ACSI services:
— GetDataValues
~  SetDataValues
= GetDataDefinition
— GetDataDirectory
~ GetDataSetValues
—  SetDataSetValues
= GetAllDataValues
= may be a DataSetMember of a DataSet referred to by any of: report control
block, log control block.
ry
cy
8
Aa
https://www.doc88.com/p-74754903218494.html 134/151
```


## File page 135

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
IEC 61850-7-3:2010+AMD1:2020 CSV — 133 —
© IEC 2020
[Fo | Semante | ____Deseiion (ares allowed, nal wives, strge) |
Setting group | Data attribute shall represent the current active value of a setting member of a
setting. See SETTING GROUP CONTROL BLOCK model.
Initial valve shall be as configured; value shall be non-volatile.
Modelling note: Applicable ACSI services:
— GetDataValues
—  GetDataDetfinition
= GetDataDirectory
— GotDataSetValues
= GetaliDataValues
— may be a DataSetMember of a DataSet referred to by any of: report control
block, log contro! block,
Setting group Data attribute shall belong to the editing services associated to a setting group.
editable ‘See SETTING GROUP CONTROL BLOCK model.
Modelling note: Applicable ACSI services:
= GetDataDefinition
= GetDataDirectory
—  GelEditsGValves
—_ SelEditsGVaives.
Service Data attribute shall represent data from different process objects with the same
response tracking object. These attributes are used for service tracking.
Initial value of the data attribute is a private issue, ¢.g., all zero.
Modelling note: Applicable ACSI services:
—  GetDataValues
= GetDataDefinition
= GetDataDirectory
—  GetDataSetValues
= GotaliDataValues
— may be a DataSetMember of a DataSet referred to by any of: report control
block, log contro! block.
Operate Data attribute shall represent the result of an Operate request at the data object
received receiving the Operate request, even if the execution of the Operate is blocked.
Initial value is irrelevant / arbitrary.
Modelling note: Applicable ACSI services:
— GetDataValues
— GetDataDefinition
= GetDataDirectory
— GetDataSetValves
— GetAlDataValues
= may be a DataSetMember of a DataSet referred to by any of: GOOSE control
block, report control block, log control block.
Blocking Data attribute shall be used for blocking value updates.
It the value of the data attribute is volatile then the initial value shall be false,
‘otherwise the value should be as set or configured.
Modelling note: Applicable ACSI services:
—  GetDataValves
— SetDataVaives
= GetDataDetfinition
a
cy
8
“a
https:/Awww.doc88.com/p-74754903218494.htm! 135/151
```


## File page 136

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< /150 > QQ View A mark Y Annotations v Q
-134- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
[Fo | Semante | ____Deseipion (ares allowed, nal wales, store) |
— GetDataDirectory
~ GetDataSetVaiues
— SetDataSetValues
— GetAliDataValues
~ may be a DataSetMember of a DataSet referred to by any of: report control
block, log contro! block.
Extended Data attribute shall represent an application name space. See IEC 61850-7-1.
foemeaton Value of the data attribute shall be as configured; value shall be non-volatile,
ame space) Modeling note: Applicable ACSI services:
— GetDataValues
= GetDataDefiniton
= GetDataDirectory
— GetDataSetVaiues
= GetaliDataValues
- may be a DataSetMember of a DataSet referred to by any of: report control
block, log control block.
a
cy
8
an
https:/Awww.doc88.com/p-74754903218494.htm! 136/151
```


## File page 137

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > QQ View A mark ¥ Annotations ¥
© IEC 2020
oe
é i
3 tis 8 i
fe5|) ifseie| fa lied) | ip
lp \itpeia| g/k [288 :
et, (Hen | dip) ERE) |G
et
HN) WEEE | [i
i
3 ‘J g
1 lath flat
5 8358 fg |? le g
gs > Be Fy
UNE lis Hels ||
a |
nts [sentient
cy
8
nw
https://www.doc88.com/p-74754903218494.htm| 137/151
```


## File page 138

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark Y Annotations ¥ Q
-136- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
Annex D
(normative)
SCL enumerations
Enumerations are defined in Clause 8.
The SCL representation as it has been part of this Annex is not needed anymore as a machine
processable representation of the complete namespace will be available electronically (see
Subclause 1.3).
A
https:/mww.doc88.com/p-74754903218494.html 138/151
```


## File page 139

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< /150 > QQ View A mark Y Annotations v Q
IEC 61850-7-3:2010+AMD1:2020 CSV - 137 -
© IEC 2020
Annex E
(informative)
Conditions for element presence
This annex introduces conditions that specify presence of elements in a given context (one LN,
or one CDC, or one data attribute type, or one data object for dataNs). The name of the attribute
type of presence is PresenceCondition.
The presence conditions are part of the Code Component of this IEC standard and are available
as electronic machine readable file in related NSD file.
Table E.1 shows presence conditions.
Table E.1 — Conditions for presence of elements within a context
[_conaon name [nite
[e [Soneemantiog CCS
a
a
[meena ent aes SSCS
[tuna [A at om ort sal be psa a tna Fave ar tins mbar >|
[om | 2a a mov Sorat my be ese artes have an nce robe > |
Parameter n: group number (> 0).
wm" [ecamncramensessoseameme |
AllOrNonePerGroupin | Parameter n: group number (> 0).
’ Al or none of the elements of a group n shall be present.
‘AlOnlyOneGroup(n) Parameter n: group number (> 0).
pone [imesononporauwren |
AllAtLeastOneGroup( | Parameter n: group number (> 0)
Parameter sibling: sibling element name.
pm [iam anerreare wow tan |
Parameter sibling: sibling element name.
[Oi rare seers eee wee |
Parameter sibling: sibling element name.
 [itagcomromen amare |
Parameter sibling: sibling element name.
[errors men anne, |
Textual presence condition (non-machine processable) with reference condiD to
Context specific text. If satisfied, the element is mandatory, otherwise optional.
MFeond(condiD) Parameter condiD: condition number (> 0).
ns
context specific text. It satisfied, the element is mandatory, otherwise forbidden.
Textual presence condition (non-machine processable) with reference condID to
Context specific text. If satisfied, the element is optional, otherwise forbidden.
a
cy
8
nw
https://www.doc88.com/p-74754903218494.html 139/151
```


## File page 140

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
-138- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
[conten wane [es Ci
MmultfRange(min, Parameters min, max: limits for instance number (> 0).
ead One oF more elements shall be present; all instances have an instance number
within range [min, max] (see IEC 61850-7-1).
‘OmultiRange(min, Parameters min, max: limits for instance number (> 0).
mm Zero or more elements may be present; all instances have an instance number
within range [min, max] (see IEC 61850-7-1).
Element is mandatory it substitution is supported (for substitution, see IEC 61850-7-
3), otherwise forbidden.
[aro [at many son of UNO; onoie apn
| MFing —————__| Element is mandatory in the context of LLNO; otherwise forbidden.
Element is mandatory if the name space of its logical node deviates from the name
space of the containing logical device, otherwise optional. See IEC 61850-7-1 for
Use of name space.
Element is mandatory if the name space of its data object deviates from the name
space of its logical node, otherwise optional. See IEC 61850-7-1 for use of name
space,
MFscaledAV Element is mandatory’ if any sibling elements of type AnalogueValue include ‘i! as a
child, otherwise forbidden.
“Even though devices without floating point capability cannot exchange floating point
values through ACSI services, the description of scaling remains mandatory for their
(SCL) configuration.
MF scaledMagV Element is mandatory" if any sibling elements of type Vector include 'T as a child of
their ‘mag’ attribute, otherwise forbidden.
*See MFscaledAV.
MFecaledAngV Element is mandatory” it any sibling elements of type Vector include T as a child of
their ‘ang’ attibute, otherwise forbidden.
*See MFscaledAV.
Element is mandatory if the harmonic values in the context are calculated as a ratio
to RMS value (value of data attribute ‘hvRef is ‘rms'), optional otherwise.
MOoperTm Element is mandatory if at least one controlled object on the IED supports time
activation service; otherwise it is optional.
Mma (sibting) Parameter sibling: sibling element name.
One or more elements must be present it sibling element is present, otherwise
forbidden,
Element is mandatory if declared control model supports ‘sbo-with-normal-security’
or ‘sbo-with-enhanced-security, otherwise optional and value is of no impact.
Element is mandatory if declared control model supports ‘irect-with-enhanced-
security’ or ‘sbo-with-enhanced-security,, otherwise optional and value is of no
t
Element is mandatory if the name space of its logical node deviates from the name
space of the containing logical device, otherwise optional. See IEC 61850-7-1 for
use of name space.
Parameter sibling: sibling element name.
Optional if sibling element is present, otherwise forbidden.
Element is mandatory if the measured value associated (amplitude respectively
angle) exposes the range eventing (with the attribute range respectively rangeAng).
This attribute is optional if value of ‘phsRef" is Synchrophasor otherwise Mandatory.
MAlIONonePerGroup | Parameter n: group number {> 0).
Mg Element is mandatory it declared contro! mode! supports ‘dlrect-with-enhanced-
‘security’ or ‘sbo-with-enhanced-security, otherwise all or none of the elements of a
group n shall be present.
ry
Aa
https://www.doc88.com/p-74754903218494.html 140/151
```


## File page 141

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark Y Annotations ¥ Q
IEC 61850-7-3:2010+AMD1:2020 CSV - 139 -
© IEC 2020
Annex F
(normative)
Compatibility of the different revisions of the standard
F.1 General
IEC 61850-7-1/AMD1:2 standardizes in Annex K rules and associated behaviours following
modification use cases to specify the expectation on implementations with regards to backward
/ forward compatibility of systems.
This annex explains changes related to this revision of IEC 61850-7-3 where one of the rules
has to apply. It also defines special compatibility rules where needed.
F.2 List of the modifications to consider for backward / forward compatibility
[sewrszere med
[ssesorsaur [mre
ee
[ssesorsawre [med
Ee
a
[swwrszere med
[namesace | Use ce 1 Ala rw COG wih DA oa nw pe andrew FS
[ssesorsawre [mre Cd
Ee
| Namespace _—_—_—| Use case tf: Extend existing constructed DA type with Sub DA of new type
[svesorsawre [med
| Namespace | Use case 2b: Extend existing CDC with DA of new FC
a
[namesico | Us ete 25 Aa row COC wih OAarow RO SCC*d
PS
2 Under preparation. Stage at the time of publication: IEC/PRVC 61850-7-1/AMD1:2020.
Aa
https://www.doc88.com/p-74754903218494.html 141/151
```


## File page 142

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark Y Annotations ¥ Q
-140- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
|___ ae case 3: Add oloments of existing types, existing FC
a
61850-7-3:2007B ‘CDC MV - DA “dbRef' has been added as conditional: MO(db), DA “zeroDbRef”
has been added as conditional: MO(zeroDb).
‘CDC CMV — DA “dbRef’ has been added as conditional: MO(db), DA *zeroDbRef
has been added as conditional; MO(zeroDb), DA “dbAngRef' has been added as
conditional: MO(dbAng),
fp eeeeeemrcem |
61850-7-3:2007 CDC INS, ING — DA “units” has been added as optional
CDC ACT — DA “originSrc”, “operTmPhsA", “operTmPhsB” and “operTmPhsC” have
been added as optional
CDC CMV - DA “rangeAng’, “dbAng™ and
“rangeAngC” COC WYE — DA “phsToNeut"
CDC INC — DA “operTimeout”, “units”
‘CDC SPC, DPC, BSC, ISC - DA “‘operTimeout”
CDC DPL — DA “name”, ‘owner’, “ePSName", “primeOper’, “secondOper’,
“latitude”, ‘longitude’, “altitude”, “mRID”
CDC LPL - DA “paramRev, “vaiRev"
CDC CSD — DA “zUnits", *zD", "2DU"
[Renasace | Us cae 0: Add SDautinae (Sa) DA) we comveted OA Wwe =|
Fe
[namespace | Use caso 9: Aid SabanOopct (Siw OO wae
a
a
[ Namespace | Use case 2c: Ad a new constructed DA twee
a
[Renesace | Us ese oe: Aad athe OA) oa now Enoaion wpe
a
a
|____Use case 4: Using new COC based on existing types, existing FCs |
| Namespace | Use case da: Add anew coc
a
cy
8
a
https:/mww.doc88.com/p-74754903218494.html 142/151
```


## File page 143

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
IEC _61850-7-3:2010+AMD1:2020 CSV - 141-
© IEC 2020
wmorsawe frm
61850-7-32007 CDC BAC - DA name “dB has been corrected to DA name db” in IEC 61850-7-
3:2007 with edition 2 interoperability tissue htip/tissue.lec6 1850.comfissuel698.
a
Ee
a
[ewsorsawre [we SSS
[sosorezr [we SCS
[Renesas Use ease :Depecion ot # DA
[sesoremor [owe
[namespace Use eso 0: Ramovl of OA
161850-7-3:2007 (CDC HMV, HWYE, HDEL - DA “units”
CDC BSC, ISC - DA “stepSize"
| Namespace —_‘| Use case 12a: Presence condition of a DA
61850-7-320078 (CDC BOR - DA “actvar’, , “stTm"
‘CDC SPC, DPC, ENC, INC, BSC, ISC, APC, BAC - DA name “sboClass” has been
changed from presence condition AC_CO_O to O with default value set to “operate
once”.
| Namespace —_—_—| Use case 12b: Presence condition of a Sub DA
| Namespace —_—«| Use case 12c: Presence condition of a Sub DO
COC SEO = ib D0 “oF om Mio OM
a
a
a
https://www.doc88.com/p-74754903218494. htm! 143/151
```


## File page 144

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 150 > @ Q_ View A mark ¥ Annotations» Q
—142- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
[ Namespace —_| Use case 19a: Presence condition of a DA
61850-7-32007B CDC SPC, DPC, ENC, INC, BSC, ISC, APC, BAC — DA ‘stSeld” has been changed
from presence condition O to MOsbo.
‘CDC SPC, DPC, ENC, INC, BSC, ISC, APC, BAC — DA “sboTimeout” has been
changed from presence condition AC_CO_O to MOsbo.
‘CDC SPC, DPC, ENC, INC, BSC, ISC, APC, BAC — DA “operTimeout” has been
changed from presence condition O to MOenhanced.
[sosorame we SY
a
[namespace ——*d Une ee 1 Parca eonion ra smo OOOSSCSCSC~*d
a
[sosorazer [re SS
[Renesas Use eave 14 Ens esing exmerion It win an enimemiea wave ——_—|
61850-7-320078 SiUnit — 88 and 87
phsRef ~ 3: Synchrophasor
61850-7-32007 ‘SiUnit — from 55 to 60, from 75 to 65
‘angRef — 11: Synchrophasor
a
a
a
61850-7-3:2007B ‘SiUnit — 62: Watts
SetCharact - 3: ANSI Normal Inverse, 6: Long-Time Extremely Inverse, 7: Long-
Time Very Inverse, 8: Long-Time Inverse, 9: IEC Normal Inverse, 13: IEC Short-
Time Inverse, 14: IEC Long-Time Inverse
[sosrame we SSCS
F.3 List of modifications requiring specific treatment
For Edition 1 and 2, some modifications have been made that require a specific treatment. Such
modification will not be allowed anymore in the future.
a
a
https:/mww.doc88.com/p-74754903218494.html 144/151
```


## File page 145

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark ¥ Annotations v Q
IEC 61850-7-3:2010+AMD1:2020 CSV — 143 -
© IEC 2020
[Nenespae Use eae fe: Reval of an enema vibe id
a
[ssesorazor [ree CY
a
[sesoramom fee SCSC~*d
[ssoraaor [re
Use case 113: Change type of @ DA
[Ranespace | Ur ot 9: Charge pe of # DA
[swsoreaorm [rw CCS
61850-7-3:2007 ‘CDC BCR - DA “actVal” and “frVal" from INT128 to INT32 (Interop Tissue 1199) to

INTE4

CDC HMV — DA “har” from “ARRAY OF Vector” to “ARRAY OF CMV"

COG HWYE - DA “phsAHar’, ‘phsBHar’, “phsCHar’ “neutHar’, “netHar’ and

“resHar’ from “ARRAY OF Vector” to “ARRAY OF CMV"

CDG HDEL - DA “phsABHar’, “phsBCHar’ and “phsCAHar" trom “ARRAY OF

Vector" to “ARRAY OF CMV"

CDG CMV — DA “angRef from {V, A, other) to {Va, Vb, Vc, Aa, Ab, Ac, Vab, Vbe,

Vea, Vother, Aother, Synchrophasor}
| Namespace __—_| Use case 114: Deprecation of a CDC
[sesoreaer fC
[Neves __[Gioge ne onan ace SSSCS—S
[eworsawe SOS
[siesoraaer [we
[nenespae ‘(Seow coomme Sid
[swsorsmoe PSCC
F.4_ Special compatibility rules and discussion
F.4d Use case 3a —- Dead band, db and dbRef
Dead band calculations specified in the first and second edition of the standard relied on a
relation with the range of the measured value. Interpretation of deadband was related to the
knowledge of the associated rangeC.min and rangeC.max optional attributes. However, in some cases,
it is reasonable to have a deadband calculation that is related to the last refreshed value (variable
deadband); whereas in other cases it is necessary to have a deadband value that is constant
(independant of the last refreshed value), and additionally without being able to express or to
semantically define a rangeC.min or rangeC.max (typical for angles).

nw
https://www.doc88.com/p-74754903218494.html 145/151
```


## File page 146

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark ¥ Annotations v Q
—144- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
The first revision of the second version of IEC 61850-7-3 introduces a set of mandatory
conditional deadband related attributes to explicitely expose the deadband behaviour without
variance in the implementation of consuming tools (see dbRef, dbZeroRef, dbAngRef). Forward
compatibility is given as the interpretation of deadband by former tool relies on the optional
presence of the attribute rangeC.min and rangeC.max. However, the same dead band
behaviour can be achieved when setting the attribute dbRef, dbZeroRef to (rangeC.max-
rangeC.min) resp. dbAngRef to (rangeAngC.max-rangeAngC.min).
Specific compatibility rule for backward compatibility: Tools and clients need to understand the
definition of db from IEC 61850-7-3:2007
F.4.2 Use case 3a — maxPts
Specific compatibility rule for backwards compatibility: If maxPts is missing, the size of the array
is determined by numPts.
F.4.3 Use case 10 — cdeNs
The data attribute cdcNs, with a FC of ‘EX’ and a type of VISIBLE STRING255 defined in the
first and second edition of the IEC 61850-7-3 in all common data classes has been deprecated
in the first revision of the second version of the standard, as extension of Common data class
are solely allowed by the namespace owner of the Common data classes: IEC 61850-7-3.
The version and revision of the IEC 61850-7-3 used in the model of the device is determined
by the Logical device name space and shall not be locally overwritten with the use of the cdcNs.
See IEC 61850-7-1 for the proper use of the Logical Device name space.
F.44 Use case f13 - CDC BCR
Specific compatibility rule for backwards compatibility: Client and Subscriber supporting
namespace IEC 61850-7-3:2007 (or newer) shall support the reception of INT32 to be able to
receive the data attribute BCR actVal from publisher or servers supporting name space
IEC 61850-7-3:2003 with TISSUE 1199.
F458 Use case f13 - CDC HMW, HWYE, HDEL
Support of these CDC from IEC 61850-7-3:2003 shall be declared in the PICS
F.4.6 Use case f13 - CDC CMV
The data attribute angRef used a different enumeration list in IEC 61850-7-3:2003. For
compatibility, in order to do the right interpretationof the values, a client or subscriber need to
get the definition of the type exposed in the SCL file from the associated Tool.
F.4.7 Use case f14 - NTS
Support of this CDC from IEC 61850-7-3:2007 shall be declared in the PICS
F.48 CDC APC
Support of this CDC from IEC 61850-7-3:2003 shall be declared in the PICS
F49 CDC ENS, ENC, ENG
INS from IEC 61850-7-3:2003 has been renamed ENS for cdcd instances where stVal follows
an enumerated type
INC from IEC 61850-7-3:2003 has been renamed ENC for cded instances where stVal follows
an enumerated type.
nw
https://www.doc88.com/p-74754903218494.html 146/151
```


## File page 147

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark Y Annotations ¥ Q

IEC 61850-7-3:2010+AMD1:2020 CSV - 145 —

© IEC 2020

ING from IEC 61850-7-3:2003 has been renamed ENG for cdcd instances where setVal follows

an enumerated type.

Specific downgrading / upgrading rules have been specified in IEC 61850-6:2009/AMD1:2018

Annex | so that the change is transparent for any application.

a

https:/mww.doc88.com/p-74754903218494.html 147/151
```


## File page 148

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark Y Annotations ¥ Q
-146- IEC 61850-7-3:2010+AMD1:2020 CSV
© IEC 2020
Bibliography
IEC 61850-8-x (all parts), Communication networks and systems for power utility automation —
Part 8: Specific communication service mapping (SCSM)
IEC 61850-9-x (all parts), Communication networks and systems for power utility automation —
Part 9: Specific communication service mapping (SCSM)
ISO 9506 (all parts), Industrial automation systems — Manufacturing Message Specification
Aa
https://www.doc88.com/p-74754903218494.html 148/151
```


## File page 149

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark Y Annotations ¥ Q
A
https:/mww.doc88.com/p-74754903218494.html 149/151
```


## File page 150

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark ¥ Annotations v Q
INTERNATIONAL
ELECTROTECHNICAL
COMMISSION
3, rue de Varembé
PO Box 131
CH-1211 Geneva 20
Switzerland
Tel: + 41229190211
info@iec.ch
www.iec.ch
The full text reading has ended. Downloading this article requires [method/access].
“ 5000 points
Download this
document
https://www.doc88.com/p-74754903218494.html 150/151
```


## File page 151

```
9/18/26, 9:27 AM IEC 61850-7-3-2020 - Doc88
< 7150 > @ Q_ View A mark ¥ Annotations v Q
Users who read this document also read these documents
lec IEC 618 directory IEC Analysis of IEC101, IEC lec
(iec 618 directory) IEC103 and |EC104
Post a comment
Verification code: 5A GS] Change one C Anonymous comment
subm
about Us Help Center Follow us [OF
About Docks Website Statement Member Registration sina Weibo
Talent Recruitment Site Map Document Download fat
Contact Us APP Download How to earn points Follow our Wec
Nn
https://www.doc88.com/p-74754903218494.htm| 151/151
```
