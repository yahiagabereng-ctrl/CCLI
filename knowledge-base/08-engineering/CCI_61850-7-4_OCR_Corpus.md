# IEC 61850-7-4 — *Basic communication structure — Compatible logical node classes and data object classes*

**RAG source_id:** `ccli-61850-7-4-ocr-corpus`  
**Purpose:** Verbatim OCR of all PDF file pages for search/RAG; verify CDC/LN tables against licensed PDF.  
**OCR path:** `Architecture/_extracted_reg_analysis/_pdf_ocr/61850-7-4/`  
**Licensed PDF:** `07-protocols/regulations/standards-drop-20260918/IEC 61850-7-4-2010.pdf`  

---



## File page 001

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
Cmprint |< 18 > @Q Q_ View A mark Y Annotations Y Search full text... Q)
IEC 61850-7-4
. Edition 2.0 2010-03
GK
Communication networks and systems for power utility automation —
Part 7-4: Basic communication structure - Compatible logical node classes and
data object classes
iy
@ os q
.) Ky
j ‘ WS =
a ~
https://www.doc88.com/p-80980482981320.html 1/185,
```


## File page 002

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
THIS PUBLICATION IS COPYRIGHT PROTECTED
Copyright © 2010 IEC, Geneva, Switzerland

Al rights reserved. Unless otherwise specified, no part of this publication may be reproduced or utilized in any form

or by any means, electronic or mechanical, including photocopying and microfilm, without permission in writing trom

either IEC or IEC’s member National Committee in the country of the requester.

It you have any questions about IEC copyright or have an enquiry about obtaining additional rights to this publication,

please contact the address below or your local IEC member National Committee for further information.

IEC Central Otfice

3, rue de Varembé

CH-1211 Geneva 20

Switzerland

Email: inmail@iec.ch

Web: www.iec.ch
About the IEC
The International Electrotechnical Commission (IEC) is the leading global organization that prepares and publishes
International Standards tor all electrical, electronic and related technologies.
About IEC publications
The technical content of IEC publications is kept under constant review by the IEC. Please make sure that you have the
latest edition, a corrigenda or an amendment might have been published.
* Catalogue of IEC publications: www.iec.ch/searchpub
The IEC on-line Catalogue enables you to search by a variety of criteria (reference number, text, technical committee...)
It also gives information on projects, withdrawn and replaced publications.
= IEC Just Published: www.iec.ch/online_news/ustoub
Stay up to date on all new IEC publications. Just Published details twice a month all new publications released. Available
‘on-line and also by email.
® Electropedia: www electropedia.org
The world's leading online dictionary of electronic and electrical terms containing more than 20 000 terms and definitions.
in English and French, with equivalent terms in additional languages. Also known as the International Electrotechnical
Vocabulary online.
* Customer Service Centre: www.iec,chiwebstore/cusisery
It you wish to give us your feedback on this publication or need further assistance, please visit the Customer Service
Centre FAQ or contact us:
Email: csc@iec.ch
Tel.: +41 22 919 02 11
Fax: +41 22 919 03 00

a
“a
https:/www.doc88.com/p-80980482981320.htm! 2/185
```


## File page 003

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
IEC 61850-7-4
° Edition 2.0 2010-03
GK
Communication networks and systems for power utility automation —
Part 7-4: Basic communication structure — Compatible logical node classes and
data object classes
INTERNATIONAL
ELECTROTECHNICAL
COMMISSION PRICE CODE xX H
ICS 33.200 ISBN 978-2-88910-577-9
rages tee ot amelis ExcreteicdConsen
8
a
https://ww.doc88.com/p-80980482981320.html 3/185
```


## File page 004

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
-2- 61850-7-4 © IEC:2010(E)
CONTENTS

1 BRD .o.0cczcsseceseerscosessensussoqsensesssoonssossseencessnessenesensssenssonsenessssmessosneteenecssensosecuesesescosesnsenes 18
2 Normative references.........csssecsessosessossersessssnssessssssonssessecsscasssssnssscssssssessecsscsssassscssssnconsoes 1
3 Terms And GOFINIIONS ......s.cersessesscsereerverscsscsnsvernssnconsenssnsescnvoncsessscsnseovorsncsncssconssesssssconsees 1D
SB Logical node Ch880.......eccesscsssosssoesecssenrvcecsecccnvesscenseonsocssonvecessecsesvessconseonvecssonvecessccscecees 1D)
5.1 Logical node groups .........ccssssssssssssssseesseseeessesssssnnnnmeessseeeeneeansssnnunnneesneeeeeseseensesnne 19
5.2 Interpretation of logical node tables ..........cccsssssssssssssessseccessessssssnnnuseessssssessecesssses 20
5.3 System logical nodes LN Qroup: L............secssscssseseesssesesneerssseesnsersnsnsneesseensnseeesseeree 2
5.3.2 LN: Physical device information Name: LPHD................:cs:scsesesesesesneerne 22
5.3.3 LN: common logical node Name: COMMON LN.........cssssesseesseennessneeneeneen Qe
5.3.4 LN: Logical node zero Name: LLNDO..........:sssessssssseenesnsensseneencenesnsensanennen 24
5.3.5 LN: Physical communication channel supervision Name: LCCH...............24
5.3.6 LN: GOOSE subscription Name: LGOS ..........ccsssesesneensrenrensenesnearenrenren 2D
5.3.7 LN: Sampled value subscription Name: LSVS.............2.-:c:cceseseeeseeeeeeere 25
5.3.8 LN: Time management Name: LTIM.............ccsssccseceessseeesneersneneeeesereneeee 2B
5.3.9 LN: Time master supervision Name: LTMG...........csesssssesseeneeesneeennenne 26
5.3.10 LN: Service tracking Name: LTRK..........sccsscsccssssessesesnsnessseneensenesneanenneneen 2
5.4 Logical nodes for automatic Control LN Group: A.......csssesssssseesneesnessnessnceseeneenneenne 2
5.4.2 LN: Neutral current regulator Name: ANCR ........sccesessseesesseeneseeesnennen Ol
5.4.3 LN: Reactive power control Name: ARCO..........csssessesnessseneeneeneeneanennennen 2D
5.4.5 LN: Automatic tap changer controller Name: ATCC .............0cese:eeseereeeeee BO
5.4.6 LN: Voltage control Name: AVCO..........:sscssseseseerenseneenseennsaraeeneeseeer SD
5.5 Logical nodes for control LN Group: C .........c.sssecssseeesesesssseeesnsersessseensersesnsneesseenss BO
5.5.1 Modelling remarks .............ssssssscssesssssnssssenssnssnsssenssssensenssnssnsarsssencensenssneneee
5.5.2 LN: Alarm handling Name: CALH ..........:ssssssssesssenesneenssnssreatenssnsensnennes D2
5.5.3 LN: Cooling group control Name: CCGR.........sssssssssssssssesseessssssssseeeseeeeeee 32
5.5.5 LN: Point-on-wave switching Name: CPOW............scsssseseeeeeeeseeneeeeeees BS
5.5.6 LN: Switch controller Name: CSWI.........ccsccseseesneeseenesneeaneenneeneeneeneen OM
5.5.7 LN: Synchronizer controller Name: CSYN.........s:sssessesseenreneenesneeennenre DD
5.6 Logical nodes for functional blocks LN group F .............sccsesesssesnesneeneenesneeenneneen OO.
5.6.1 Modelling remarks ..............csssssesseesseensesnnenserorsseensrensrsseeetenersserereseerseerees OO
5.6.2 LN: Counter Name: FONT.........ssscsssssensssssnssssnnsnsenssnsessssarsscenesaseneseesees OO
5.6.3 LN: Curve shape description Name: FCSD...........cccsessseeeneeneseenennenn OT
5.6.5 LN: Control function output limitation Name: FLIM.............-ceceseeeseeeee BB
5.6.7 LN: Ramp function Name: FRMP...........:.sssesessseeneesneneeneensenesearennenee SD
5.6.8 LN: Set-point control function Name: FSPT ............cssssssseeseeneenesneeeneeenes OD
5.6.9 LN: Action at over threshold Name: FXOT.........scsessesseessesnesneesseeseesees 0
5.6.10 LN: Action at under threshold Name: FXUT..........ssesssssseseeseenesnseenenree 40

a

cy

8

nw

https://www.doc88.com/p-80980482981320.html 4/185
```


## File page 005

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
61850-7-4 © IEC:2010(E) -3-

5.7 Logical nodes for generic references LN Group: G vocccsssssvvssssssssseeeeeeeeeeeeseeeeeseeee
5.7.1 Macalling COMETS ..200.c2cccercecscecscesesecesscsesessneesecessecssesssecosecscsecsvecssccescccccsce SD
5.7.2 LN: Generic automatic process control Name: GAPC ..........cssesesereeeenene 4d
5.7.3 LN: Generic process 1/0 Name: GGIO..........csesssessesneensneenreneenesneenennennen 2
5.7.5 LN: Generic security application Name: GSAL...........cccecereereeereee 4D

5.8 Logical nodes for interfacing and archiving LN Group .......:ssssssssseeeeeesseeeeeeeen 4D
GB.1 — Maciallinag Fem ...2..0.ccceseccscecscccesescssccesocscensesscsessscsssecscsssscssccsssssccccsscceoee
5.8.3 LN: Human machine interface Name: IHMI.........cssecssesseesseesseesnessneenrerneeen 44
5.8.4 LN: Safety alarm function Name: ISAF «0.0.0.0... 4
5.8.5 LN: Telecontrol interface Name: ITCI ......:scsssessrsssersessessersneesnessseesressess 4D
5.8.6 LN: Telemonitoring interface Name: ITMI ...............ccccsseseseeeeeeseeeeeeeeeeeee MS
5.8.7 LN: Teleprotection communication interfaces Name: ITPC .........0ccce 45

5.9 Logical nodes for mechanical and non-electric primary equipment LN group ts
5.9.2 LN: Fan Name: KFAN ........ssssssossssssesesesensessnsssssevsssesenssensessserssssvessscsssesseee 4]
5.9.4 LN: Pump Name: KPMP........ccccccssesesessseseensrsnserseensesseerseenseseersreneereeenes 4B
5.9.6 LN: Valve control Name: KVLV........:ccsessssesesnesneesseesnesnnesseesneesneasseenressees SD

5.10 Logical nodes for metering and measurement LN Group: M.....scccss:sssssssssssseenesssee 50
G.90.1 = Madang PORN IT 2.2..2cccececceccccscscsccessnsesosscenseecesecncccssccecsnscsesscsnsscscccccccccee SO)
5.10.2 LN: Environmental information Name: MENV ............0ccsseseseeeeseeeneeeee 50
5.10.3 LN: Flicker measurement name Name: MFLK ........0ssssesssesssssessneesneesees OF
5.10.4 LN: Harmonics or interharmonics Name: MHAI .................cceseseeseeeeeeee BZ
5.10.5 LN: Non-phase-related harmonics or interharmonics Name: MHAN........53
5.10.6 LN: Hydrological information Name: MHYD ..............::cssseseeeneeeeneeeee SS
5.10.7 LN: DC measurement Name: MMDC.........csesscsseessesseeseenesnessneeseeseess OD
5.10.8 LN: Meteorological information Name: MMET............sssss:seseesesseeeneeee SS
5.10.9 LN: Metering Name: MMTN...........:csssssssssssenssessseenesnsensaneaeatenesasenesnesees OO
5.10.10 LN: Metering Name: MMTR.............ccccscssssessseensnenesseensseseseneneeeerenseeees OD,
5.10.11 LN: Non-phase-related measurement Name: MMXN .............c0c0sssesseee D7,
5.10.13 LN: Sequence and imbalance Name: MSQI...........cccccceceseeeeeeereeee SD
5.10.14 LN: Metering statistics Name: MSTA..........:cs:-ssssesssessesseenseeesnsseseeeees 80

5.11 Logical nodes for protection functions LN Group: P...............ccseseseseenseeneseseensneeees OO
5.11.2 LN: Differential Name: PDIF.........cscsssssessessessressersneesnresneesnesnessneesnseseees OF
5.11.3 LN: Direction comparison Name: PDIR ............cssecsesseeerenesesneeneeeereneneee OZ
5.11.4 LN: Distance Name: PDIS...........sssssssssssssesssesesssenesnssesssssressenssassnsseesees OD
5.11.5 LN: Directional overpower Name: PDOP ................cccsscceseseeseseseeeseeeneeeee OD
5.11.6 LN: Directional underpower Name: PDUP ...........0ccccceccseseeenseeeseeseereeeeees OF
5.11.7 LN: Rate of change of frequency Name: PFRC...........::0ssssssssseeeeeeeeees 4
5.11.8 LN: Harmonic restraint Name: PHAR .........-s:sesssecssesseeseesnesneesneeseeseees OS
5.11.9 LN: Ground detector Name: PHIZ ........ssscsssessserseesnersnesnesnenneasneessseneess OD
5.11.10 LN: Instantaneous overcurrent Name: PIOC.........e-ssseessessersseesseeenresneess 6
5.11.11 LN: Motor restart inhibition Name: PMR ..............cccsscceseeseeeeeneeeesene OB
5.11.12 Une starting time supervision Name: PMSS ..........::ssssssseesss0ee00067

cy
8
nw
https://www.doc88.com/p-80980482981320.html 5/185
```


## File page 006

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q

-4- 61850-7-4 © IEC:2010(E)
5.11.13 LN: Over power factor Name: POPE ..........cccccssesesesssenseseeesseerseeeeeseenees OF
5.11.14 LN: Phase angle measuring Name: PPAM ........css:ssessssessesseensenesneereneeres OF
5.11.15 LN: Rotor protection Name: PRTR .............sccsssccssesesesessneeesneesesesereeserees OB
5.11.16 LN: Protection scheme Name: PSCH ............:ccssccsessseeeeneeeseseenenseeeeeeee OB
5.11.17 LN: Sensitive directional earthfault Name: PSDE............:csseeseseereenee 9
5.11.18 LN: Transient earth fault Name: PTEPF .............csscsesesseeseeseeneenesneeneseeeeee 10
5.11.19 LN: Thyristor protection Name: PTHF .............ccsscccessseeesseeeneeneeeereeeeees TO
5.11.20 LN: Time overcurrent Name: PTOC ......:sscssessesseessessnesssesnsesnessseesasseses TO
5.11.21 LN: Overfrequency Name: PTOF ....ccssscccssssssssmussssnnssnsssnsnnsessnseeceseeeeee TA
5.11.22 LN: Overvoltage Name: PTOV ............ccccesesecssseseseenessseersnsersnsnsnsenerensneees TE,
5.11.23 LN: Protection trip conditioning Name: PTRC...........:.cccssesesesenesrenseee TE
5.11.24 LN: Thermal overload Name: PTTR .........ccsessessseenesneenssneseeseenssnsareneenres 1D
5.11.25 LN: Undercurrent Name: PTUC.........cccsssesessseenesneensstesteaeenesnsaneaeenes 1D
5.11.27 LN: Undervoltage Name: PTUV...........c:ccesssessesrsesesrseeeseerersseesreneresernee 24
5.11.28 LN: Underpower factor Name: PUPF..............:csscccsssseesssseeeeeeeneeeeteneeeees TD
5.11.29 LN: Voltage controlled time overcurrent Name: PVOC.............:00:0e0 75
5.11.30 LN: Volts per Hz Name: PVPH.............:ccsesssesssesrsnenessseensnsersesnsnensarenserees 1B
5.11.31 LN: Zero speed or underspeed Name: PZSU.........-csscsessecseeseeeseenennennee TD
5.12 Logical nodes for power quality events LN Group: Q ........ssscssssssssssseeseesseeeeeeenseeene 77
5.12.2 LN: Frequency variation Name: QFVR............c.:ccssssessseeesnssesesesnseeeeenseceees PD
5.12.3 LN: Current transient Name: QUTR ........ssssessssesssesssneessseessnneessneessneessneees 7B
5.12.4 LN: Current unbalance variation Name: QIUB...........sssssssseensenesneeenenne 18
5.12.5 LN: Voltage transient Name: QVTR.............::ccsssccesesessseensnsetseeneneeserenseeees 1D
5.12.6 LN: Voltage unbalance variation Name: QVUB .............:scsecesecseseseeseeeees 1D
5.12.7 LN: Voltage variation Name: QVVR .........cccccsesssessesesesseenseerersseersrenereseerees BO
5.13. Logical nodes for protection related functions LN Group: R .....sscsss+:sssssseseeeeeeee 80
G.29.1 — MaBalling FORO IID .20000000.nccscccsccscsesecenensosscessessensececsseesesessncecsnocssscccsscssees Ol
5.13.2 LN: Disturbance recorder channel analogue Name: RADR ..........-0000081
5.13.3 LN: Disturbance recorder channel binary Name: RBDR............:.+c0e0+081
5.13.4 LN: Breaker failure Name: RBRPF ...........scccssesseesesneeeseeseeaeenesneeneseeees BS
5.13.5 LN: Directional element Name: RDIR .........sccsscssssesnesnsnesseeneeneeneeneanennennes B2
5.13.6 LN: Disturbance recorder function Name: RDRE ..............:secseseseeee 83
5.13.7 LN: Disturbance record handling Name: RDRS ................::cceeeeeeee BF
5.13.8 LN: Fault locator Name: RFLO........ssssessesssessesseesnessneesesniennessseeseesees OF
5.13.9 LN: Differential measurements Name: RMXU..............0ccseeereeeeeeeereneeene BF
5.13.10 LN: Power swing detection/blocking Name: RPSB................c::00:s000+0000 85,
5.13.11 LN: Autoreclosing Name: RREC.........:scssssesesnessneneneensenessareseenee OO
5.13.12 LN: Synchronism-check Name: RSYN.........sscsssssssesneesseneeseenesnsenesneere OO
5.14 Logical nodes for supervision and monitoring LN Group: S......sssssssssssseeeeeeeeeeeeeeee 87
5.14.2 LN: Monitoring and diagnostics for arcs Name: SARC... 88
5.14.3. LN: Circuit breaker supervision Name: SCBR...................::csseceeeeeeeeeeee BB
5.14.4 LN: Insulation medium supervision (gas) Name: SIMG................-.000+000+-89
5.14.5 LN: Insulation medium supervision (liquid) Name: SIML ...............-0--------90
5.14.6 LN: Tap changer supervision Name: SLTC...........:ccsssssssseesssseseseeneeeene DD
5.14.7 LN: Supervision of operating mechanism Name: SOPM ............0.00000000 91
5.14.8 LN: Monitoring and diagnostics for partial discharges Name: SPDC........92

a

Ly

8

nw

https://www.doc88.com/p-80980482981320.html 6/185
```


## File page 007

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
61850-7-4 © IEC:2010(E) -5-

5.14.9 LN: Power transformer supervision Name: SPTR ..........0ccc0seeeseereee 9D
5.14.10 LN: Circuit switch supervision Name: SSWI .........:.sssessssseeneeneseareseere 9S
5.14.11 LN: Temperature supervision Name: STMP................:cssscseseseseeseeenseeee 94
5.14.12 LN: Vibration supervision Name: SVBR............c.ccccssseesseeeseereneeeereneeee OS
5.15 Logical nodes for instrument transformers and sensors LN Group: T...........+0+00+-96
G.15.1 Modelling emesis ......cccercecececsecscsecensvenvossvensesssnessecsccecessnccsessvonseccesessccseee OO
5.15.3 LN: Axial displacement Name: TAXD ........c.cccccssesesesseenseeeeeseerseenerseeeees 96
5.15.4 LN: Current transformer Name: TCTR..........-s.cssssecsesnesnsseeneeseenesnsenesneeees OT
5.15.5 LN: Distance Name: TDST..........cseccsessseeseesesserseessessnessesniesneesneeseesees OT
5.15.6 LN: Liquid flow Name: TFLW ........:csesssssssessenrsneenesnsersstentensenssnsareseenee 9B
5.15.7 LN: Frequency Name: TFRQ..........ccccsscssssseesssnssssenssnssesssssressenesnseresesees OB
5.15.8 LN: Generic sensor Name: TGSN .........cscsccssessseenesneeeseeeteasenesnsaneseeenes 9D
5.15.10 LN: Media level Name: TLVL .........-esssssseeessesesseenesneensseareseenesatereseees 100
5.15.12 LN: Movement sensor Name: TMVM.........ccssssesssesssessnessesneeneesseessecees 100
5.15.13 LN: Position indicator Name: TPOS ..........-scssssessesseessseesneseessssessesneensees 101
5.15.14 LN: Pressure sensor Name: TPRS..........sccssessesseessessneeseeseeneesneesneenee VOT
5.15.15 LN: Rotation transmitter Name: TRTN........ccsscsseeseseesseeseeneenesnearenees 102
5.15.16 LN: Sound pressure sensor Name: TSND..........ss:ssesssessseseeneenesneeverees 102
5.15.17 LN: Temperature sensor Name: TTMP............cssscsesseessseeseeneenesnsenessees 103
5.15.18 LN: Mechanical tension / stress Name: TTNS ...........0cccsscseseeeeseeesreneees 103,
5.15.19 LN: Vibration sensor Name: TVBR...........csssssessseenesneessneseescensensenesees 104
5.15.20 LN: Voltage transformer Name: TVTR..............ccssccesesesrenseeseeeseeeeerensees 104
5.15.21 LN: Water acidity Name: TWPH............cc:s:sccssseseseesesseeeeseeersneeeeeerereesers 105,
5.16 Logical nodes for switchgear LN Group: X...cccssscvsssssessseeessessssssssssseeseseseeeneeeneees 105
5.16.2 LN: Circuit breaker Name: XCBR .........:ssccsessesseesseesnessnessesnesneesseesseeees 105
5.16.3 LN: Circuit switch Name: XSWL.......ccessssssseessenesneenesnserssseerensenesnsareseees 106
5.17 Logical nodes for power transformers LN Group: Y ........:secsesseessneeeessenrenseneensees 107,
5.17.2 LN: Earth fault neutralizer (Petersen coil) Name: YEFN.........:.s:csesesee 107
5.17.3 LN: Tap changer Name: YLTC ..........csecsessssessesesseeneenseeeseeetensenesnsareseese 107
5.17.5 LN: Power transformer Name: YPTR.......sscsssessesseessessnessesnsesnessseessseses 108
5.18 Logical nodes for further power system equipment LN Group: Z -.........--.sss0eeeee- 109
00.1 RUNING FOIIIIIS on ccscscecsccecccrscocscscevsnnsocncsenesnoossnncnnsscscsescsssssscsssccsssccces SOD
5.18.2 LN: Auxiliary network Name: ZAXN........ccsscessesesseenessnteneenrenseneensarestens 109
5.18.3 LN: Battery Name: ZBAT ..........ssecsesssenssnseressenrsnsenesnssssssarensenssnsereseeee 109
5.18.4 LN: Bushing Name: ZBSH..........:seccssseessseessenesneenesnssreseeerensenesnsareseeee 110
5.18.6 LN: Capacitor bank Name: ZCAP..........ccccccsesssessesrsrssresnerersseresrerereneees DUT
5.18.8 LN: Generator Name: ZGEN ........scccsessesseessessessesneesneesneeseenesseesneesee VET
5.18.9 LN: Gas insulated line Name: ZGIL............ccssesssecresnenreseeneeneeneseaeneene 112
5.18.10 LN: Power overhead line Name: ZLIN ........csssesssessesseesnessneesnessseeseeaes 112
5.18.11 LN: Motor Name: ZMOT.......cseccsessssesessesseessessessessessnsssessnessneassesseasses IVD
5.18.12 LN: Reactor Name: ZREA........:sscsssssssssseenesessseenesneensssearsasenssnsareseeee 113

a

Ly

8

nw

https://www.doc88.com/p-80980482981320.html 7/185
```


## File page 008

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
-6- 61850-7-4 © IEC:2010(E)
5.18.13 LN: Resistor Name: ZRES .......cssscsessesseessessesseesnessnessesseesnessseeseassee 114
5.18.14 LN: Rotating reactive component Name: ZRRC.......-..ccessecseseenesesreneeee 114
5.18.15 LN: Surge arrestor Name: ZSAR.........:scsccsssessseenesncensaneneescensensenennees 115
5.18.16 LN: Semi-conductor controlled rectifier Name: ZSCR .........eceseeeseee 115
5.18.17 LN: Synchronous machine Name: ZSMC.........cssssssesneesseseeneenseneanenvees 115
5.18.18 LN: Thyristor controlled frequency converter Name: ZTCF ............00.117
5.18.19 LN: Thyristor controlled reactive component Name: ZTCR............:0-0. 117
6 Data object name SeMANtics ............ccccsesessssesvseseesssesvsnseensansranseensanscensansranseensaesranseeneaeee DT,
Annex A (normative) Interpretation of mode and behaviour .............cccccceeeeeeeeeeeeeeeees 156
Annex B (normative) Local / Remote Concept ...........ssssesesseeessesestesesneeneseeneeessareseeneensaees 158
Annex C (informative) Deprecated logical node Classes ................csscccesseeeeeeeseeeeeeeeeensees 160
Annex D (informative) Relationship between this standard and IEC 61850-5 ............es.e0ss00+ 161
Annex E (informative) Algorithms used in logical nodes for automatic ContrOl..............e++0++ 162
Annex F (normative) Statistical Calculation .............csssssssseerseseseessseersnsersssrersnsersnsneeeeserensere TOT.
Annex G (normative) Functional relationship of data objects of autorecloser RREC.............172
Annex H (normative) SCL enumerations ...............c.:cssssseesersessseesssessserscssseessnsersnsnsecsseeeesere ITD
Bibliography ........ccssssssesssssssssssssesssseseesssessesssstimusnssnnnnnssesnsseneesseeesseesnssusnunsasssnaseseessseeesseeee 179)
Figure 1 — Overview of this standard ...............csscsssesessenesesnesssesesnsnseesnsensessnsesecesseenseceressneeses V2
Figure 2 — LOGICAL NODE relationships................00sccssesesesserseeesesseersesnsesseestsnsesseerseensersneenes Od
Figure E.1 — Example of curve based on an indexed gate position providing water flow........ 162
Figure E.2 — Example of curve based on an indexed guide vane position (x axis) vs. net
head (y axis) giving an interpolated runner blade position (Z axis)..........:...--sssssesssseeeseeeseeee 163
Figure E.3 - Example of a proportional-integral-derivate Controller.............ssssssecesssseeeeeenseeee 164
Figure E.4 —- Example of a power stabilisation system ............::c:scsseresesrsrsseerseerereseersrerereseees 165
Figure E.5 — Example of a ramp generattr ..............0cssecesesnessseensseneeesesneneecersnseresseensesererseees 165
Figure E.6 — Example of an interface with a set-point algorithm ............ccsescseseerseerseeseerseees 166
Figure F.1 — Statistical calculation Of & V@CtOF...........ssecsesseeesseenesennsaeenesncensaenteacenssnsareseees 168
Figure F.2 — Examples of statistical calculations .............csssssssssssssusssssseccscccsssenssnnaseeseeeseeeee 170
Figure G.1 — Diagram of autorecloser fUnction...............-csssssssss::sseesseseeeeaseensssnssssssevseeeesseee 172
Table 1 — List of logical node QrOUpS.......:sssssssssusssssssussssseesnsanseesnsanseesiuanseesennnnsesersnnseeeessee 19
Table 2 — Interpretation of logical node tables................:s.s::sssssssssessnssesnesnesnesnesnsssenensensensseseesees 20
Table 3 — Relation between IEC 61850-5 and IEC 61850-7-4 for automatic control LNs............. 27
Table 4 — Relation between IEC 61850-5 and IEC 61850-7-4 for Control LNS...........s:ssseesseeesesees 82
Table 5 — Conditional attributes in FPID.............ss:ssessssesesneesnessneesneennesnessnessnsesnseneesneesnessesensesnes BO
Table 6 — Relation between IEC 61850-5 and IEC 61850-7-4 for metering and
Table 7 — Relation between IEC 61850-5 and IEC 61850-7-4 (this standard) for protection LNs 60
Table 8 — Relation between IEC 61850-5 and IEC 61850-7-4 for protection related LN.............. 80
Table 9 — Relation between IEC 61850-5 and IEC 61850-7-4 for supervision and
GRGRMROIIRG LINB....200.02020sceseccsccssenssecsscesscnosesssecsesosccnnesssssnssesessosenanscsessesercscseeccensccscseccscccscsccceeee ST
Table 10 — Description of data Objects .......:--sssssssssssssessssssessssssseessunsssssesssssssssesessnsssseee 17
Table A.1 — Values of mode and DENAVIOUT............serssseesesneenesesreneennsneenssnensntentearenesearsaees 156
a
Ly
8
nw
https://www.doc88.com/p-80980482981320.html 8/185
```


## File page 009

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 1184 > @ Q_ View A mark Y Annotations ¥ Q
61850-7-4 © IEC:2010(E) -7-
Table B.1 — Relationship between Loc/Rem data objects and control authority ..........s..s.0000 159
Table D.1 — Relationship between IEC 61850-5 and this standard for some
miscellaneous LNS ........ssssssssssssssssseeeeesseeseeeseessnustnnsnssssssnsenesseeeeeeneseesseesnssnannnnssssseseeeeeeeeee 164
a
https://ww.doc88.com/p-80980482981320.html 9/185
```


## File page 010

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
-8- 61850-7-4 © IEC:2010(E)
INTERNATIONAL ELECTROTECHNICAL COMMISSION
COMMUNICATION NETWORKS AND
SYSTEMS FOR POWER UTILITY AUTOMATION —
Part 7-4: Basic communication structure —
Compatible logical node classes and data object classes
FOREWORD

1). The International Electrotechnical Commission (IEC) is a worldwide organization for standardization comprising
all national electrotechnical committees (IEC National Committees). The object of IEC is to promote
international co-operation on all questions concerning standardization in the electrical and electronic fields. To
this end and in addition to other activities, IEC publishes International Standards, Technical Specifications,
Technical Reports, Publicly Available Specifications (PAS) and Guides (hereafter referred to as “IEC
Publication(s)"). Their preparation is entrusted to technical committees; any IEC National Committee interested
in the subject dealt with may participate in this preparatory work. International, governmental and non-
governmental organizations liaising with the IEC also participate in this preparation. IEC collaborates closely
with the International Organization for Standardization (ISO) in accordance with conditions determined by
agreement between the two organizations.

2) The formal decisions or agreements of IEC on technical matters express, as nearly as possible, an international
consensus of opinion on the relevant subjects since each technical committee has representation from all
interested IEC National Committees.

3) IEC Publications have the form of recommendations for international use and are accepted by IEC National
Committees in that sense. While all reasonable efforts are made to ensure that the technical content of IEC
Publications is accurate, IEC cannot be held responsible for the way in which they are used or for any
misinterpretation by any end user.

4) In order to promote international uniformity, IEC National Committees undertake to apply IEC Publications
transparently to the maximum extent possible in their national and regional publications. Any divergence
between any IEC Publication and the corresponding national or regional publication shall be clearly indicated in
the latter.

5) IEC itself does not provide any attestation of conformity. Independent certification bodies provide conformity
‘assessment services and, in some areas, access to IEC marks of conformity. IEC is not responsible for any
services carried out by independent certification bodies.

6) All users should ensure that they have the latest edition of this publication.

7) No liability shall attach to IEC or its directors, employees, servants or agents including individual experts and
members of its technical committees and IEC National Committees for any personal injury, property damage or
‘other damage of any nature whatsoever, whether direct or indirect, or for costs (including legal fees) and
‘expenses arising out of the publication, use of, or reliance upon, this IEC Publication or any other IEC
Publications.

8) Attention is drawn to the Normative references cited in this publication. Use of the referenced publications is
indispensable for the correct application of this publication.

9) Attention is drawn to the possibility that some of the elements of this IEC Publication may be the subject of
Patent rights. IEC shall not be held responsible for identifying any or all such patent rights.

International Standard IEC 61850-7-4 has been prepared by IEC technical committee 57:

Power systems management and associated information exchange.

This second edition cancels and replaces the first edition published in 2003. It constitutes a

technical revision.

Future standards in this series will carry the new general title as cited above. Titles of existing

standards in this series will be updated at the time of the next edition.

The major technical changes with regard to the previous edition are as follows:

* corrections and clarifications according to information letter "IEC 61850-technical issues by

the IEC TC 57” (see document 57/963/INF, 2008-07-18);
* extensions for new logical nodes for the power quality domain;
a
nw
https://www.doc88.com/p-80980482981320.html 10/185
```


## File page 011

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
61850-7-4 © IEC:2010(E) -8-
* extensions for the model for statistical and historical statistical data;
* extensions regarding IEC 61850-90-1 (substation-substation communication);
* extensions for new logical nodes for monitoring functions according to IEC 62271;
* new logical nodes from IEC 61850-7-410 and IEC 61850-7-420 of general interest.
The text of this standard is based on the following documents:
a ec

This publication has been drafted in accordance with the ISO/IEC Directives, Part 2.
The content of this part of IEC 61850 is based on existing or emerging standards and
applications. In particular the definitions are based upon:
* the specific data objects types defined in IEC 60870-5-101 and IEC 60870-5-103;
* the common class definitions from the Utility Communication Architecture 2.0: Generic

Object Models for Substation and Feeder Equipment (GOMSFE) (IEEE TR 1550);
* CIGRE Report 34-03, Communication requirements in terms of data flow within substations,

December 1996.
A list of all parts of the IEC 61850 series under the general title Communication networks and
systems in substations, can be found on the IEC website.
The committee has decided that the contents of this publication will remain unchanged until the
stability date indicated on the IEC web site under "http://webstore.iec.ch” in the data related to
the specific publication. At this date, the publication will be
* reconfirmed,
+ withdrawn,
* replaced by a revised edition, or
* amended.
A bilingual version of this publication may be issued at a later date.
IMPORTANT - The ‘colour inside’ logo on the cover page of this publication indicates
that it contains colours which are considered to be useful for the correct understanding
of its contents. Users should therefore print this document using a colour printer.

a
nw
https://www.doc88.com/p-80980482981320.html 11/185
```


## File page 012

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
-10- 61850-7-4 © IEC:2010(E)
INTRODUCTION

This part of IEC 61850 is part of a set of standards, the IEC 61850 series. IEC 61850 defines
communication networks and systems for power utility automation, and more specially the
communication architecture for subsystems such as substation automation systems. The sum
of all subsystems may result also in the description of the communication architecture for the
overall power system management. The defined architecture provided in specific parts of
IEC 61850-7-x gives both a power utility specific data model and a substation domain specific
data model with abstract definitions of data objects classes and services independently from
the specific protocol stacks, implementations, and operating systems. The mapping of these
abstract classes and services to communication stacks is outside the scope of IEC 61850-7-x
and may be found in IEC 61850-8-x and in IEC 61850-9-x.
IEC 61850-7-1 gives an overview of the basic communication architecture to be used for all
applications in the power system domain. IEC 61850-7-3 defines common attribute types and
common data classes related to all applications in the power system domain. The attributes of
the common data classes may be accessed using services defined in IEC 61850-7-2. These
common data classes are used in this part to define the compatible data object classes.
To reach interoperability, all data objects in the data model need a strong definition with regard
to syntax and semantics. The semantics of the data objects is mainly provided by names
assigned to common logical nodes defined in this part and the data objects they contain, as
defined in this basic part, and dedicated logical nodes defined in domain specific parts such as
for hydro power control systems. Interoperability is easiest if as much as possible of the data
objects are defined as mandatory. Because of different approaches and technical features,
some data objects, especially settings, were declared as optional in this edition of the standard.
There are also data objects which were declared as conditional, i.e. they will become
mandatory under some well-defined conditions. After some experience has been gained with
this standard, this decision may be reviewed in the next edition of this part.
It should be noted that data objects with full semantics are only one of the elements required to
achieve interoperability. The standardized access to the data objects is defined in compatible,
power utility and domain specific services (see IEC 61850-7-2). Since data objects and
services are hosted by devices (IED), a proper device model is also needed. To describe both
the device capabilities and the interaction of the devices in the related system, a configuration
language is also needed, as defined in IEC 61850-6 by the substation configuration description
language (SCL).
The compatible logical node name and data object name definitions found in this part and the
associated semantics are fixed. The syntax of the type definitions of all data objects classes is
governed by abstract definitions provided in IEC 61850-7-2 and IEC 61850-7-3. Not all features
of logical nodes are listed in this part; for example, data sets and logs are covered in
IEC 61850-7-2.

a

Ly

8

nw

https://www.doc88.com/p-80980482981320.html 12/185
```


## File page 013

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
61850-7-4 © IEC:2010(E) -1M-
COMMUNICATION NETWORKS AND
SYSTEMS FOR POWER UTILITY AUTOMATION —
Part 7-4: Basic communication structure —
Compatible logical node classes and data object classes
1 Scope
This part of IEC 61850 specifies the information model of devices and functions generally
related to common use regarding applications in systems for power utility automation. It also
contains the information model of devices and function-related applications in substations. In
particular, it specifies the compatible logical node names and data object names for
communication between intelligent electronic devices (IED). This includes the relationship
between logical nodes and data objects.
The logical node names and data object names defined in this document are part of the class
model introduced in IEC 61850-7-1 and defined in IEC 61850-7-2. The names defined in this
document are used to build the hierarchical object references applied for communicating with
IEDs in systems for power utility automation and, especially, with IEDs in substations and on
distribution feeders. The naming conventions of IEC 61850-7-2 are applied in this part.
To avoid private, incompatible extensions, this part specifies normative naming rules for
multiple instances and private, compatible extensions of logical node (LN) classes and data
object names. Any definition is based on IEC 61850 or on referenced well identified public
documents.
This part does not provide tutorial material. It is recommended to read parts IEC 61850-5
and IEC 61850-7-1 first, in conjunction with IEC 61850-7-3, and IEC 61850-7-2.
This standard is applicable to describe device models and functions of substation and feeder
equipment. The concepts defined in this standard are also applied to describe device models
and functions for:
 substation-to-substation information exchange,
* substation-to-control centre information exchange,
© power plant-to-control centre information exchange,
* information exchange for distributed generation,
« information exchange for distributed automation, or
« information exchange for metering.
Figure 1 provides a general overview of this standard. The groups of logical nodes defined in
this standard are shown in Figure 1, ordered according to some semantic meaning, for
instance different control levels such as plant level, unit level, etc. For convenience, the logical
nodes are defined below in alphabetical order.
a
nw
https://www.doc88.com/p-80980482981320.html 13/185
```


## File page 014

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
-12- 61850-7-4 © IEC:2010(E)
tec 1102003
Figure 1 — Overview of this standard
2 Normative references
The following referenced documents are indispensable for the application of this document. For
dated references, only the edition cited applies. For undated references, the latest edition of
the referenced document (including any amendments) applies.
IEC 60270:2000, High-voltage test techniques — Partial discharge measurements
IEC 61000-4-7:2002, Electromagnetic compatibility (EMC) - Part 4-7: Testing and
measurement techniques — General guide on harmonics and interharmonics measurements
and instrumentation, for power supply systems and equipment connected thereto
IEC 61000-4-15, Electromagnetic compatibility (EMC) - Part 4-15: Testing and measurement
techniques — Flickermeter — Functional and design specifications
IEC 61850-2, Communication networks and systems in substations — Part 2: Glossary
IEC 61850-5, Communication networks and systems in substations — Part 5: Communication
requirements for functions and device models
IEC 61850-7-1:___ 1, Communication networks and systems for power utility automation — Part
7-1: Basic communication structure — Principles and models
1 To be published. 5
“a
https:/www.doc88.com/p-80980482981320.htm! 14/185
```


## File page 015

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q

61850-7-4 © IEC:2010(E) -13-
IEC 61850-7-2:___ 2, Communication networks and systems for power utility automation — Part
7-2: Basic information and communication structure — Abstract communication service interface
(ACSI)
IEC 61850-7-3:___ 3, Communication networks and systems for power utility automation — Part
7-3: Basic communication structure - Common data classes
IEC 61850-9-2, Communication networks and systems for power utility automation — Part 9-2:
Specific Communication Service Mapping (SCSM) — Sampled values over ISO/IEC 8802-3
IEEE C37.111:1999, /EEE Standard Common Format for Transient Data Exchange
(COMTRADE) for Power Systems
IEEE 519:1992, JEEE Recommended Practises and Requirements for Harmonic Control in
Electrical Power Systems
IEEE C37.2:1996, Electrical Power System Device Function Numbers and Contact Designation
IEEE 1459:2000, /EEE Trial-Use Standard Definitions for the Measurement of Electric Power
Quantities Under Sinusoidal, Nonsinusoidal, Balanced, or Unbalanced Conditions
IEEE 1588, Precision clock synchronization protocol for networked measurement and control
systems
3 Terms and definitions
For the purposes of this document, the terms and definitions given in IEC 61850-2 and
IEC 61850-7-2 apply.
4 Abbreviated terms
The following terms are used to build concatenated data object names. For example, ChNum is
constructed by using two terms "Ch" which stands for "Channel" and "Num" which stands for
“Number”. Thus the concatenated name represents a "channel number".
Term Description Term Description
A Current Alm Alarm
Acs Access Amp Current non-phase-related
Abr Abrasion An Analogue
Abs Absolute ‘Ang Angle
ac AC, alternating current Ap Access point
Acc ‘Accuracy ‘App ‘Apparent
Act Action, activity Are Are
Acu Acoustic Area Area
Adj Adjustment ‘Auth ‘Authorisation
Adp Adapter, adaptation ‘Auto Automatic
Age Ageing Aux Auxiliary
Air Air Av Average
Alg Algorithm ‘Watt Wattmetric component of current
2 To be published
3° To be published, 5

nw

https://www.doc88.com/p-80980482981320.html 15/185
```


## File page 016

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
-14- 61850-7-4 © IEC:2010(E)
Term Description Term Description
Ax Axial co Carbon monoxide
8 Bushing co2 Carbon dioxide
Base Base Col Coil
Bat Battery Cont Configuration
Beh Behaviour Cons Constant
Ber Bit error rate Con Contact
Bias Bias Cor Correction
Bin Binary Core Core
Bib Bulb Crd Coordination
Bik Block, blocked Crit Critical
Bnd Band cw Curve
Bo Bottom ct Current transducer
Bst Boost cu Control
Bus Bus ctr Center
c Carbon Cur Current
c2H2 Acetylene ovr Cover, cover level
coH4 Ethylene Cyc Cycle
c2H6 Ethane D Derivate
Cap Capability Day Day
Capac Capacitance 8 Decibel
Car Carrier Det Direct
ce Circuit breaker Dea Dead
Cat Credit Den Density
ce Cooling equipment Det Detected
Cel Cell Detun —_—_Detuning
ct Crest factor DExt De-excitation
crt Coetticient Dew Dew
Ctg Configuration Off Dittuse
ca Core ground Dor Degree
Ch Channel Diag Diagnostics
cHs Methane bit Differential, difference
Cha Charger Dip Dip
Chg Change Dir Direction
Chk Check Dis Distance
Chr Characteristic Dsp Displacement
Cire Circulating, circuit ol Delay
Cle Calculate, calculated bit Delete
Clk Clock, clockwise Dma Demand
Cloud Cloud On Down
Clr Clear DPCSO _Double point controllable status output
Cis Close pao Direct, quadrature, and zero axis
Cndct Conductivity quantities
Cnt Counter oso Braghans
Cmbu Combustible, combustion Ory Oey
cma Command Ow Orive
&’ os Device state
Ly
8
Aa
https://www.doc88.com/p-80980482981320.html 16/185
```


## File page 017

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 1184 > @ Q_ View A mark Y Annotations ¥ Q
61850-7-4 © IEC:2010(E) -15-
Term Description Term Description
Dse Discrepancy Gn Generator
Dsch Discharge Gnd Ground
Dur Duration Gr Group
Dv Deviation Grd Guard
EC Earth Coil Grn Green
Echo Echo Gr Grid
cE External equipment Gust Gust
eF Earth fault H Harmonics (phase-related)
mg Emergency H2 Hydrogen
Ems Emissions H20 Water
En Energy Ha Harmonics (non-phase-related)
Ena Enabled Health Health
End End Heat Heater, heating
Env Environment Hi High, highest
Eq Equalization, equal Hor Horizontal
Err Error HP Hot point
ev Evaluation Hum Humidity
Evt Event Hy Hydraulics, hydraulic system
Ex External Hyd Hydrological, hydro, water
Exe Exceeded Hz Frequency
Exel Exclusion t Integral
Exp Expired Imb Imbalance
Ext Excitation Imp Impedance non-phase-related
F Float In Input
FA Fault arc Ina Inactivity
Fact Factor Iner Inertia
Fail Failure Incr Increment
Fan Fan Ind Indication
Fer Frame error rate Inh Inhibit
Fil Filter, filtration Ins Insulation
Fish Fish Insol Insolation
Fid Field Int Integer
Fil Fall Inte Interrupt, interruption
Flood Flood Intv: Interval
Fit Fault Iscso Integer status controllable status output
Flush Flush K Constant
Filw Flow Kek Kicker
FPF Forward power flow Key Key
Fu Fuse km Kilometre
Full Full L Lower
Fwd Forward Last Last
Gas Gas ld Lead
Gen General Lo Logical device
Go Goose Loc Line drop compensation
GoCBRet os one" reterence (see LOCR Line drop compensation resistance:
ey
Ly
8
a
https://ww.doc88.com/p-80980482981320.html 17/185
```


## File page 018

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
-16- 61850-7-4 © IEC:2010(E)
Term —_ Description Term Description
LDCX —_Line drop compensation reactance Nit Nitrogen
Lez Line drop compensation impedance Ng Negative
Leak Leakage Nom Nominal, normalising
LED Light-emitting diode Num Number
Len Length Nsa ‘Average partial discharge current
Lev Level 02 Oxygen
lo Lag 03 zon, trioxygen
Lim Limit Ofs Offset
Lin Line Oil oil
Liv Live Oo Out of
iN Logical node Op Operate, operating
Lo Low Opn Open
Lo Lockout Out Output
Loc Local ov Over, override, overtiow
Lod Load, loading Ov Overload
Lok Locked P Proportional
Loop Loop Pa Partial
Los Loss Pap Paper
Lst List Par Parallel
ute Load tap changer Pet Percent, percentage
M Minutes Per Periodic, period
wore Data objects mandatory o optional or PF Power factor
Ph Phase
ven uae PH Acidity, value of pH
Mag Magnetic, magnitude Phe Phase Lt
Max Maximum PhsB Phase L2
al Mesuaee Phsc Phase L3
= Matton, PNV Phase-to-neutral voltage
Min Minimum Phy Physical
Mr Mirror Pi Instantaneous P
mat Multiplier, multiple is Pulse
bogs buaee Pit Plate, long-term flicker severity
Month Month Pep aan
wat Meer Po Polar
i“ tareeconeie Pol Polarizing
Mst Moisture Pos Position
ur Main tank PosA Position phase Lt
= wemed Pos8 —_Position phase L2
Mvm Movement, moving ae Postion phase L3
Ne Nitrogen dioxide Pot Potentiometer
‘on ail Pow Point on wave switching
Name Name (see Note) pp Phase to phase
NdsCom Se Taso ye ppm Parts per million
Net Net sum PPV Phase to phase voltage
Neut Neutral Pre Pre-
6’
cy
8
a
https://www.doc88.com/p-80980482981320. html 18/185
```


## File page 019

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
61850-7-4 © IEC:2010(E) -17-
Term Description Term Description
Pres Pressure st Step one
Pro Progress, in progress $10 coefficient S10
Pri Primary $12 coefficient S12
Pro Protection sz Step two
Proxy Proxy Sar ‘Surge arrestor
Prt Parts, part Sat Saturation
Ps Positive ‘Sbs ‘Subscription
Pst Post, short-term flicker severity Sch Scheme
Pt Point Sco ‘Supply change over
Pwr Power SCSM Specific communication service mapping
ty Quantity Sec Security
R Raise Sel Select
RO Zero sequence resistance Seq Sequence
Rat Ratio Set Setting
Red Record, recording Sig Signal
Reh Reach Sign Sign
Rel Reclaim ‘Sim Simulation, simulated
Ret Reaction sh Shunt
Ray Ready Sint Salinity, saline content
Re Retry Smok Smoke
React —_Reactance, reactive Snr Signal to noise ratio
Rec Reclose Sow Snow
Rect Rectifier Spd Speed
Red Reduction, redundant Spee Spectra
Ret Reterence SPI Single pole
Rel Release SPCSO Single point controllable status output
Rem Remote Spt Setpoint
Res Residual sre Source
Reso Resonance st Status, state
Rev Revision Sta Station
Rt Retreshment Step Step
Ris Resistance Sto Storage e.g. activity of storing data
Rl Relation, relative Stat Statistics
Amp Ramping, ramp Stop Stop
RMS Root mean square sid Standard
Rabk Runback ‘Stk Stroke
Rot Rotation, rotor Ste ‘Start
Rs Reset, resetable ‘Stuck Stuck
Rs! Result ‘Sup Supply
Rst Restraint, restriction ‘Sve Service
ee Rese SVCBRet inere. reterence (see
Rtg Rating sw Switch, switched
Rv Reverse Swo Swing
Re Receive, received ‘Syn Synchronisation
Tap Tap

ry

Ly

8

a

https://ww.doc88.com/p-80980482981320.html 19/185
```


## File page 020

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
-18- 61850-7-4 © IEC:2010(E)
Term Description Term Description
Ta Total distortion Ver Vertical
Tot Transformer derating factor Vbr Vibration
Tdp Td Viol Violation
‘Ta0P Td0" Vise Viscosity
Td0s 00" vim Volume
Tds Td viv Valve
Term Termination Vol Voltage non-phase-related
Test Test Volts Voltage
Tot Target vT Voltage transducer
Thd Total harmonic distortion w Active power
Thm Thermal Wac Watchdog
TiF Telephone influence factor Watt Active power non-phase-related
Time Wav Wave, waveform
Tmh = Time inh
Tm Tmm = Time in min wa Wind
Tms = Time ins Week Week
Tmms « Time in ms
Wei Weak end infeed
Tmp Temperature (°C) 7
Tok Tank wh Watt hours
ig Top ws wa
Tot Total " indow
1 Three pole Wem Ware
wm Warning
Tpe Teleprotection -
Tap iz 0 Zero sequence reactance
x Positi
ra0p roo" 1 ‘ositive sequence reactance
Ta0s Teo" x2 Negative sequence reactance X2
iiss ane xd synchronous reactance Xd
ie ne Xdp transient synchronous reactance Xd"
tis rade Xds Reactance Xd"
Tip Trip xq synchronous reactance Xq
ty Trigger Xqp transient reactance ;
Tk Track, tracking Xqs ‘sub-transient reactance Xq’
Ts Transient = Your
Ts Total signed - impedance
zo Zero sequence impedance
Tu Total unsigned
Téa ral Positive sequence impedance
Zer Zero
™ Transmit, transmitted
Zn Zone
Typ Type
20 Zero sequence method
Unt Ultra-high-trequency
NOTE The abbreviation “Name” should only be used
Un Under in data object EEName and LNName.
Up Up, upwards
v Voltage
vA Volt amperes
Va Variation
Vac Vacuum
val Value
VAr Volt amperes reactive
a
Aa
https://www.doc88.com/p-80980482981320.html 20/185
```


## File page 021

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
61850-7-4 © IEC:2010(E) -19-
5 Logical node classes
5.1 Logical node groups
Logical nodes are grouped according to the logical node groups listed in Table 1. The names of
logical nodes shall begin with the character representing the group to which the logical node
belongs. For modelling per phase (for example switches or instrument transformers), one
instance per phase shall be created; for modelling protection per zone or level, one instance
per zone or level shall be created also.
Table 1 — List of logical node groups
Group indicator Logical node groups

fA attomatic cont
fe Supervisory contros
foie ney recurs Cid
[FJ rnctonaroce ——SSC—~S
[ener ntntnces
[epee
a
a Ce
a
a
[pron ions
fo [rowan ons aoweionwates =|
a
a
a
a Te
@ LNs of this group exist in dedicated IEDs if a process bus is used. Without a process bus, LNs of this group are

the I/Os in the hardwired IED one level higher (for example in a bay unit) representing the external device by its

inputs and outputs (process image)

a
nw
https://www.doc88.com/p-80980482981320.html 21/185
```


## File page 022

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
-20- 61850-7-4 © IEC:2010(E)

5.2 Interpretation of logical node tables

The interpretation of the headings for the logical node tables is presented in Table 2.

Table 2 — Interpretation of logical node tables
a
[Bu concirane —_—|Waneattedaneneet

Common data class that defines the structure of the data object. See IEC 61850-7-3.
For common data classes regarding the service tracking logical node (LTRK), see
IEC 61850-7-2.

[Expansion | Sort expansion oe aaa obs and how Wused
Transient data objects - the status of data objects with this designation is
momentary and must be logged or reported to provide evidence of their momentary
state. Some T may be only valid on a modelling level. The TRANSIENT property of
DATA OBJECTS only applies to BOOLEAN process data attributes (FC=ST) of that
DATA OBJECTS. A transient DATA OBJECT is identical to normal DATA OBJECT,

t ‘except that for the process state change from TRUE to FALSE no event may be
‘generated for reporting and for logging.

For transient data objects, the falling edge is not reported if the transient attribute is
set to true in the SCL-ICD file. It is recommended to report both states (TRUE to
FALSE, and FALSE to TRUE), i.e. not to set the transient attribute in the SCL-ICD
file for those DOs, and that the clients filter the transitions that are not “desired”.
This column defines whether a data object is mandatory (M) or optional (O) or
conditional (C) for an instance of a specific logical node. When a data object is
marked mandatory (M), it shall be contained in the instance of the logical node.
‘When a data object is marked optional (0), it may be contained in the instance of the
logical node; the decision if the data object is contained or not is outside the scope
of this standard. The entry C is an indication that a condition exists for this data
‘object, given in a note under the LN table. The condition decides what conditional
wore data objects get mandatory. C may have an index to handle multiple conditions.
NOTE! Procurement specifications may require specific data objects marked
‘optional to be provided for a particular project. The amount of optional information to
be provided needs to be negotiated.
NOTE 2 The attributes for data objects that are instantiated may also be mandatory
or optional based on the CDC (attribute type) definition in IEC 61850-7-3.

The LNName attribute is inherited from Logical-Node class (see IEC 61850-7-2). The LN class

names are individually given in the logical node tables. The LN instance name shall be

composed of the class name, the LN-Prefix and LN-Instance-ID according to IEC 61850-7-2,

Clause 22.

All data object names are listed alphabetically in Clause 6. Despite some overlapping, the data

objects in the logical node classes are grouped for the convenience of the reader into the

following categories:

— Status information

Status information contains data object, which show either the status of the process or of
the function allocated to the LN class. This information is produced locally and cannot be
changed via communication for operational reasons unless substitution is applicable. Data
objects such as “start” or “trip” are listed in this category. Most of these data objects are
mandatory.

— Measured and metered values

Measured values are analogue data objects measured from the process or calculated in the
functions such as currents, voltages, power, etc. This information is produced locally and
cannot be changed remotely unless substitution is applicable.

Metered values are analogue data objects representing quantities measured over time, for
example energy. This information is produced locally and cannot be changed remotely
unless substitution a

nw
https://www.doc88.com/p-80980482981320.html 22/185
```


## File page 023

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q

61850-7-4 © IEC:2010(E) -21-
— Controls

Controls contain data objects which are changed by commands such as switchgear state

(ON/OFF), tap changer position or resettable counters. They are typically changed remotely,

and are changed during operation much more often than settings.
— Settings

Settings are data objects which configure the function for its operation. Since many settings

are dependent on the implementation of the function, only a commonly agreed minimum is

standardised. They may be changed from remote, but normally not very often.
— Descriptions

Descriptions are data objects, which give information about the LN itself or an allocated

device. This information consists of identification information and general properties like

configuration revision, hard and software revisions, etc.
5.3 System logical nodes LN group: L
5.3.1. LN relationships
In this subclause, the system specific information is defined. This includes common logical
node information (for example logical node behaviour, nameplate information, operation
counters) as well as information related to the physical device (LPHD) implementing the logical
devices and logical nodes. These logical nodes (LPHD and common LN) are independent of
the application domain. All other logical nodes are domain specific, but inherit mandatory and
optional data objects from the common logical node.

[OGICAL NODE seis
—IT
[we] | weer ioe | Ee —
1ec 110303
Figure 2 - LOGICAL NODE relationships
All logical node classes defined in this document inherit their structure from the
GenLogicalNodeClass (LN, see Figure 2) defined in IEC 61850-7-2. Apart from the logical node
class ‘Physical Device Information’ (LPHD), all logical node classes (LLNO and domain specific
LNs) defined in this document inherit at least the mandatory data objects of the common logical
node (Common LN).
NOTE Common logical node will never be instantiated.
a
nw
https://www.doc88.com/p-80980482981320.html 23/185
```


## File page 024

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
-2- 61850-7-4 © IEC:2010(E)
5.3.2 LN: Physical device information Name: LPHD
This LN is introduced in this part to model common issues for physical devices.
Descriptions
Iryien [OR [Pyne anven ramen SSS
a
louoy [ss ouput communcaions biter ovetow ——S~S*~S~S~«~dCidN—C*
Frey [5S _|ndeasivnis Winapo ——SSSS~S~w i
[nor 5 nou conmunestons outer ovetow —————SS~w
Iwinfwip |S omberofpowerupe —SSSSSCSC~*d
[wacrra ws Numer ofwatensog deveeessteceecee ———SSSS~«~d_
rwup [SPS [roweruponesws SSS
jrwon [ss [Ponorcom sowed SSS
Fersupain [5s [earslpowersvpy arm SSS
[Controts
lam |s°0 [eave sinunes G0O8E oraimauessy «dO
[Settings
[Data sets (see IEC 61850-7-2)
[ButferedReportControlBlock (see IEC 61850-7-2)
UnbutferedReportControlBlock (see IEC 61850-7-2)
[Services (see IEC 61850-7-2)
5.3.3. LN: common logical node Name: Common LN
The common logical node class provides data objects which are mandatory or conditional to all
dedicated LN classes. It contains also data which may be used in all dedicated logical node
classes, such as input references and data objects for the statistical calculation methods (refer
to Annex F).
Data ob|
a
Data objects
[Name fut |Namo pate fe
lax |s*5 [dynam backing of wnciondsenbedytwin | fo
[Controts
an
https:/www.doc88.com/p-80980482981320.htm! 24/185
```


## File page 025

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 718 > @ Q_ View A mark Y Annotations v | Q

61850-7-4 © IEC:2010(E) -23-
[Data objects
me a of contro! sequences and action triggers of controllable data ie |

objects,
[Settings
[BikRett ==“ [ORG ~—_ [Blocking reference shows the receiving of dynamically blocking signal | |O |
Logical node information (statistical calculation specific — refer to Annex F)
lect [oPS [baton pr spre om
[Controls

|Enables the calculation start at time operTm from the control model (if

\set) or immediately
[Settings
[Geum Ew [baton maa of stain oi wis] oa
|cicMod [ENG _| Calculation mode. Allowed values: TOTAL, PERIOD, SLIDING [les |
(CicintvTyp ENG —_|Calculation interval type [|cs |

In case CicintvTyp equals to MS, PER-CYCLE, CYCLE, DAY, WEEK,

MONTH, YEAR, number of units to consider to calculate the calculation

interval duration
= number of sub-intervals a calculation period interval duration He |

contains

In case CicintvTyp equals to MS, PER-CYCLE, CYCLE, DAY, WEEK,

MONTH, YEAR, number of units to consider to calculate the refreshment

interval duration
[csi [oR ject eterece suc oa ve ica
CleNxTmms Remaining time up to the end of the current calculation interval —

lexpressed in milliseconds

[Object reference to the source of the external synchronization signal for

ithe calculation interval
[Data sets (see IEC 61850-7-2)
Inherited and specialised trom logical node class (see IEC 61850-7-2)
[ButferedReportControiBlock (see IEC 61850-7-2)
Inherited and specialised trom logical node class (see IEC 61850-7-2)
[UnbutteredReportControlBlock (see IEC 61850-7-2) SS SSSSC~*S
Inherited and specialised trom logical node class (see IEC 61850-7-2)
[Services (see IEC 61850-7-2)
Inherited and specialised from logical node class (see IEC 61850-7-2)
(Condition C1: Mod, Health and NamPit shall be inherited by LLNO of the root LO of a hierarchy as mandatory and’
iby all other LN as optional.
|Condition G2: GmdBik shall be inherited as optional data object by all LNs which contain controllable data objects
ladditionally to Mod, if there is no BlkOpn/BIkCls available (like in XCBR).
ICondition C3: This data object is optional but mandatory when considering statistical calculation, especially the
|MMXU, MMXN LN.
[Condition C4: These data objects are mandatory, except when CicMth equals UNSPECIFIED.
(Condition C5: This data object is mandatory, if the considered LN is performing statistical calculation derived trom!
|another LN.

ry
nw
https://ww.doc88.com/p-80980482981320.html 25/185
```


## File page 026

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
-24- 61850-7-4 © IEC:2010(E)
All dedicated LN classes shall inherit all data objects, data objects sets, control blocks and
services from this common logical node class, if applicable. The data object beh shall be
inherited in any case as mandatory.
5.3.4 LN: Logical node zero Name: LLNO
This LN shall be used to address common issues for logical devices. For example, LLNO
contains common information for the LD like health, mode and beh and NamPIt.
aN tas
Mor
c
Dawes
(Status information
joptmm fins foreratontine Cf
[eexey [ss zeal puratonorconpiete wpealanves ————SSSS~«wd
a
[Comtroty
[ees 56 _[Swtchng awhoriyatsatoniew ———SSS~wi
Joao spc run diagnoses, =f
[corso ieee SSCS
[Settings
[amet [ORG [Reon angneriowogmalaeee ————SS~Sd
[Select mode of authority for local control (True — control from multiple
lievels is allowed, False — no other control level allowed) (see Annex B)
Setting Ce Block [0..1] (see 1EC 61850-7-2)
[tog (0.niseeteC 61es0-7-2)
LogControlBlock [0..n} (see IEC 61850-7-2)
(GOOSEControlBlock [0..n] (see IEC 61850-7-2)
Multicasts falueControlBlock (0..n} (see 1EC 61850-7-2)
r jalueControiBlock (0..n] (see IEC 61850-7-2)
5.3.5 LN: Physical communication channel supervision Name: LCCH
This LN is introduced in this part to model common issues for physical communication
channels. It is instantiated for each physical channel or each pair of link level redundant
physical channels.
eH ts
LNName [The name shall be composed of the class name, the LN-Prefix and LN-
Instance-ID according to IEC 61850-7-2, Clause 22,
Daw Obets
|Status information
SS pa TF
ispecitied time interval.
a
louor [soup conmncatonsbuterowrtow ———S~d
ino |5°5_|nputconmuncaton butrovetow —«
a
nw
https://www.doc88.com/p-80980482981320.html 26/185
```


## File page 027

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 718 > @ Q_ View A mark Y Annotations v | Q

61850-7-4 © IEC:2010(E) -25-

of redundancy) messages for each 1 000 messages forwarded to the

lapplication.

'Frame error rate on redundant channel; count of missed messages on

this channel for each 1 000 messages forwarded to the application.
(Measured and metered values
ncn —[BCRonberafvecenednesees «dO
earuGnr [BCR Number of renves messages on eoncantchamnet ——————*d
Fc Joc Number otseremessapes SS SSSC«d
Settings
— = Saaeaaeceasee

ithan one access point and more than one physical channel exist.
[cuivine [NG Timeout tine or chanel ve spervinonsdoaunss_————*d'O |
NOTE If channel redundancy with duplicate remove is used, the number of lost messages can be calculated as
‘messages forwarded to application as result of both channels - messages received on this channel’, In this case,
Ithe FER is calculated by counting the received messages per channel, until 1 000 messages are forwarded to the
application, and then using above formula per channel.
lObserve that in PRP any message received for a wrong channel is also forwarded to the application. Thus a wrong
|connection of cables to ports can be detected, it Fer and RedFer have a value around 500 (1 000 messages with
|wrong channel identification forwarded to application, 500 messages with wrong channel identification received on
leach channel).
[Condition C: is mandatory, if channel redundancy is used.
5.3.6 LN: GOOSE subscription Name: LGOS
The LN LGOS shall be used monitoring of GOOSE messages. There shall be one instance of
LGOS per GOOSE subscription for a given GOOSE source. It allows for instance to diagnose
the subscription state of a GOOSE message.
LNName [The name shall be composed of the class name, the LN-Prefix and LN-

Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
Insscom —_[9°S __[Sbssrpion needs commissioning _————SSCS~S~S~«wd
Ix____|5®5 [sus ofthe svscrpton Te = ative Faesnatacive) | [w _|
Ismet ____|S°5 [tus snowing hat ly Sim messapes ar reeves and accpied | [0
[aston [INS [ast sttenumbervecowea «dO
[conte [NS [specedconigvation reson nomber——SCS~di
[Gecsrer [ORG __[Releence oe absarbed GOOSE convlock ————S—«d
5.3.7 LN: Sampled value subscription Name: LSVS
The LN LSVS shall be used for diagnose and monitoring supervision of sampled value
messages. There shall be one instance of LSVS per SV subscription for a given server. It
allows for instance to diagnose the subscription of a SV message (status of subscription).

a
an
https:/www.doc88.com/p-80980482981320.htm! 27/185
```


## File page 028

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 1184 > QQ View A mark Y Annotations ¥ | Q)
—26- 61850-7-4 © IEC:2010(E)

LNName The name shall be composed of the class name, the LN-Prefix and LN-

Instance-ID according to IEC 61850-7-2, Clause 22,
Data objects
[Néscom [ses [Subscription needs commissioning |
ist |SPS___|Status of the subscription (True = active, False = not active) [lo |
fsimst___ [ss ___ [status showing that really Sim messages are received and accepted | |o_|
[comrevtiwm [ws [Expectesconsiguration revision number Jo
[svcaret fora __[Reterence tothe subsenibea Sv contolbick J
5.3.8 LN: Time management Name: LTIM
The LN LTIM shall be use for diverse configurations regarding the local time of an IED.
LNName ‘The name shall be composed of the class name, the LN-Prefix and LN-

Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
|Status information
frmot____ [ses [Incieating i tor this locaton dayigh saving ime is inettectnow [|u|
[Settings
[Fmotstmm ING Jortset of tocal ime trom UTC inminutes | IM
[TmUseDT —__—‘[SPG__[Flag indicating if this location is using daylight saving time [ [|
[Tmchgdaytim _[ts@__tacal time ot next change to dayight saving time | JO
[tmcroststm [tse __tocalime ot next change tostancardtime J

[Day of the start of the local week for statistical calculation (Monday

|(detault) | Tuesday | Wednesday | Thursday | Friday | Saturday | Sunday )
5.3.9 LN: Time master supervision Name: LTMS
The LN LTMS shall be used for the configuration and supervision of the time synchronization
function in an IED.
LNName [The name shall be composed of the class name, the LN-Prefix and LN-

Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects

[Number of significant bits in the Fraction Of Second in the time accuracy

lpart of the time stamp. See IEC 61850-7-2.
[nse _[veS_fourenttmesouce—SSSC~i
[insye [ENS Time ayers accom wiccoiemoee ———S—~=d
fncnstt __|s°S _[rinechmnetstaue pio) SSSSSS—=*dO
Settings

\vsG [Time source setting (“1588" in case the time source is a IEEE 1588
[Source or dotted IP-address)
an
https://www.doc88.com/p-80980482981320.html 28/185
```


## File page 029

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
61850-7-4 © IEC:2010(E) -27-
5.3.10 LN: Service tracking Name: LTRK
The LN LTRK allows to track service parameters. With this tracking, service parameters will
stay visible after the execution of service. For this purpose, common data classes are needed
which contain the parameters of the services according to IEC 61850-7-2.
PTR
Common
data class
LNName ‘The name shall be composed of the class name, the LN-Prefix and LN-
Iinstance-ID according to IEC 61850-7-2, Clause 22.
Data objects
ect [ors [Cmtoleonan racing wreowoiaieargemmt ————SC~«~di
[eet [ers _[omaleeveo ncingtrconauecnaiepont «| (O
incr lors _[omwatenveeinanatorconcaierneor ——SSS=*d OY
erat [es _[oalavee acing tremoanccowotie «| (O
heer [es [Gana vee rcig ona ran et pot wih at conmaré | [0
lApcintTrk [CTS __| Control service tracking for controllable analogue set point with Integer command| |O |
seta [ers _[analsevee rca comin epost omaten | [0
inctm [ews [Pots acing riper conttad sp postontomaion | |_|
scr lors [Cotsen vac roy cola arate poser vat | |_|
fe meneame |
lexists
[beer [os een enverrairg wrnbaowdreotcowstbea «dO
lncora [rs pees eve mci wrvowspoteowaiiee «| fo
[oaata irs essere asia orepcowotnec —————SSSSSSC*dO
oeoTa fos peeve racing wrosrconmioesk «LO
Imscora its ess sores racing rma sanped abs anraibock | |_|
[wear nts pcos src ang runes snpvabencanriboe +o
sere [ots _ccnssenenacinatrsetegoowenwatnex ~~ io |
5.4 Logical nodes for automatic contro! LN Group: A
5.4.1 Modelling remarks
Table 3 — Relation between IEC 61850-5 and IEC 61850-7-4 for automatic control LNs
Defined in | Modelied in
HEC 61850-5 |1EC 61850-7-4
by LN by LN
The start value has to discriminate between live
‘and dead. The delay time has to be reasonably
Exo wees tpemid AEN nm long to discriminate between a transient voltage
zero of a permanent switched off line.
‘Automatic control of suppression (Petersen) coil
Aaommats nettral (tarpoinn ‘Automatic wattmetric increase with thermal
supervision
5.4.2 LN: Neutral current regulator Name: ANCR
For a description of this LN, see IEC 61850-5. This LN shall be used for regulation of
suppression coils (ASC / Petersen coil) as tap coils and plunger core coils.
a
nw
https://www.doc88.com/p-80980482981320.html 29/185
```


## File page 030

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 1184 > @Q_~ View A mark Y Annotations ¥ Q)
—28- 61850-7-4 © IEC:2010(E)
— | haan |
linstance-ID according to IEC 61850-7-2, Clause 22.
[Data objects
[cckey (SPS octereenoney——SSCS~S
jcc [a8 [oeatconratneraviw i
[wcoPes [NS |Woneotpestion ————SCSC~S*~S~S~S YO
[ecorPos ins ftowcolposton SSCS
[copa [ses [orange cot postionse——SSSSCSC~CiN_
[Coton [PS ___|onangecotlositontower —SSSSCSCSCS~dCON_
(cacrooe [ss _[oraneecollposttonineperion —SSSSCSC~ii—CS
[Status of external fixcoil (True — fixcoil is connected, False - not
rect FS Siecage menue tentiscomenws raver |
~~ - Sea |
|Umax,Umax_nC(Umax- but not compensated), Umax_not compensated
due to U continuous limitation
JPoaim ses [Potentiometeratarm
[Motaim [ses [Motor crive alarm due tono movement | JO
[Morwin [ses ___|Motorfor Petersen coil operating time exceeded | [|
[cieSeqwen [SPS ___|Number of calevation sequence exceeded in automaticimanval mode | [O |
[corrosa [ov [Coit position (usually as current in Ampere) | JO
[aResoPt [wv [Current atthe resonancepomt | JO
fawatt __|v___|Wattmetric part ofthe residual current atthe fault location | JO |
[ADetwn [Mv ___[Detuning due to the actual coitpositon | JO
Joamp fv [Damping otthenetwork
[Capacima [wv __[Capactive imbatance of tro network | JO
JvoiResopt [cmv [Value of the voltage at the resonance point, | [|
[Neutvor_—femv _Neutratto ground vonage
[eons
loncnits [NC [Resetabinoperaioncouner———SSSCSC~=~“~*~‘“~*~*~*~*~drC~CC*d
lccsia [so _[Swiching autontyatsaioniewer «dO
Frapcre [85 _|orango wp postion top. ater owe)——SSSSCSC~*di
[comapPos __|'SC___|Move col to speciied aiscete coi postion —————SSS~«*di_|
[copes [APC [Move ott specfled continuous colpostion =i
Incor [PC [Raise pungercot postion SSSSCSC~«~di
lic [so ftowerstungercotposon id
Iauto [SPC [Automatic manual operation ———SSSCS~S~Swi
Isuce [SPC _[startcatuatonseqence CO
rao _|so_[Paratevnéepenent operation Tue —paal Fase —indopendon) | [0
a RS
seins
—- SS |
jith fixed slave coil position | Master/ Slave with variable slave coil
iposition | Parallel operation without communication)
[Pattos [ENG goo [set curt epusor mode ding contol (master, slave independent) | [O_|
cy
=)
nw
https://www.doc88.com/p-8098048298 1 320.htm! 30/185
```


## File page 031

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
61850-7-4 © IEC:2010(E) -29-
InDeunspt [asc [Spit orth detuning aft suppessonca——S~S~ri
raw [ASG [Band wth vote as votae or erent otroninalvotage | [0 |
[Condition C1: at least one of the described attributes shall be used (either TapChg or ColTapPos) for controlling
YEFN as a tap coil
5.4.3 LN: Reactive power control Name: ARCO
For a description of this LN, see IEC 61850-5. This LN shall be used for a reactive controller
independent of the control method being used.
Data object
name
LNName ‘The name shall be composed of the class name, the LN-Prefix and LN-
Instance-ID according to IEC 61850-7-2, Clause 22.
[Data objects
[Lockey |SPS__[tnsalorvemotetey—SSSSCS~S~S JO
[Loc [85 _|tecalconrotbenavow SSCS
[vows __|5°5__|votapeovergestnns SSS
[osenak SPS [Bank swien close nockes woe todacharge —~SCS*~*~*~*S*«~
|Comtrots
[opcnts [NC __[Resetabieopraioncouter ————SSSCS~S~S~«~dCO_
[Lessa [86 | Switching uory atsaioniewsi ——SCS~S~Sw
[Tapcne [5c | Change reactive power (top. ger, owe cm
[ato [5° __[awomateoperten SS SSSSS~«*di dO
5.4.4 LN: Resistor control Name: ARIS
For a description of this LN, see IEC 61850-5. This LN should be used for the automatic
wattmetric increase with thermal supervision.
LNName ‘The name shall be composed of the class name, the LN-Prefix and LN-
Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
lteckey [sPS[Lecatorremotekey
ltoc_ ses tocatcontrot behaviour
ew ses aches
[rmpaim [ses |rermararm
loncnrns Nc _[Resenabie operation counter dO
lLocsta__ [sec [Switching aunorty atstauonteve
ry
an
https:/www.doc88.com/p-80980482981320.htm! 31/185
```


## File page 032

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
-30- 61850-7-4 © IEC:2010(E)
Jato [sec [Paratevindependent operation
jsusea__ [sec |stansequenee
[Measured an metered values
Imewvor ou Newaliogroundvouge SSCS
[Ratnp jv [Resta temperate er watmavicmcwaws ——S—~id
Inetnpce WW [Reset tnpoaive caus ——————SSSSS~=d
5.4.5 LN: Automatic tap changer controller Name: ATCC
For a description of this LN, see IEC 61850-5.
Pena [ee] ee
name data class
-_| Savers |
linstance-ID according to IEC 61850-7-2, Clause 22.

Data objects
Lockey [SPS ____[Localorremotekey
[oc |s°5 scl contl boro ——SSSSSS—~d |
larapros|ns [Honan poston SSCS
ce
Fapook [ses [nage ep postenrae SSO
Fapopt [6° [orgs tap pontiontower SSS |
Hapopstp [6°85 [orgs tap postonsip SSCS
[epOpc [SPS Tap enangeeror or tp indication ovar(e. wong BcD cose) | |_|
[canis [SPS [TOinbn ave oundervetage SS SS*dO
[oanvii [SPS [Cin ueioovervatage ——SSS—~*d
[ewan [ses [Temi wveroovereunene————SSSS~S~S~Sw
JnaPosn [SPS nd postion rie orgs alowed ap poston acted =~
[naPost [SPS [End poston wer or owes alowedtp postion reaches + |_|
near |sP5__|evorotpwateiopeaten «dO
|Measured and metered values
jou Jv [eontronvontnge
Loar [Mv [Load-current otal transformer secondary curenty | ||
forea [av [evreutatingeurrent fo
[Pang [MV___[Phase angle of LodA relative to CtV at 1.0 powertactor, FPF | [o_|
lwicuy [Mv [Highest controtvontage ff
ltocuy [Mv lowestcontrotvorage
[Hiomca av [righ current demand (oad eurentdemana) | Jo
[Controis
[OpCntAs INC __‘[Resettable operation counter 0
[oesta [sr [swing autortyasitoniet ————SSSS~wd
Tapcng _|B5C__[Cnange ep poston op, haber ower) ———SSSSSC*di
faoroe SC |Settapponton SS SSSSSCS~di
lonscGhg [OAC a [and cone hago aie owen nowae ——————S—S—~*d YO]

Ly

8

an

https:/www.doc88.com/p-80980482981320.htm! 32/185
```


## File page 033

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /184 > QQ View A mark ¥ Annotations ¥ Q
61850-7-4 © IEC:2010(E) -31-
Data object | Common |
name data class

[roe —_—_[sPc_—_—Partavndopnaoriopeaion ————SSSSSCS~di
(osm |S lek mbt) atonat conve ———SSSSCSCS~S~S~ri CS
[cong [sec eset Cag ange SSSSSCSCSCS~S~S~«S
Ino __*[S®C—_‘awonatenansslopeation———SSSSCS~dCi
\viest [sec otagerwaictonstpt ———SSSCSCS~dCidO
[Settings
Jace [ASG _Bandconervotane FPFpeamed ———SSSC«dCO
Bnawid [Band width voltage (as voltage or percent of nominal voltage,

|FPF presumed)
[euormms [WG _[Conronentonaltine diay FPF presumes) ———————SS—=*d'
[loc |AsG__|neaopvotage sue tine esistance component __—~|t fo _|
ltocx _JASG__tne op votape se to ne esctaen component ———*(do_
Jantv [ASG_[conrlvotag blow which auto lower commands acres | |O_|
lanev _[ASG__|Conro votage above which auto rise commande Hockes + |_|
Janvio _[ASG__[Conot ata below win auto rae commands avebioced | [O
Janvri [ASG [Cont atage ove when ao tower commands are boced | [O_|
jrmerv (ASG —‘unbackrasevotawe id
[imtosa [ASG [mona curent CTO wuackioasewen) ————SSS~dCd
[toc__|seG une sop conpensatonie RaXerZmose———SSS~d
arog [ENG _|Parantvansiomermose —SSSS~rCi
[inom [SP __Time dey new oriversschaaciesie —————SS—*d
[luce |ps__[tne impedance for ine up conpensaton | fo_|
\veavar [ASG _Reauzton of band cane econ when votage waitin sep ave | [O |

locked

‘Tap position of load tap changer where automatic lower commands are

blocked
[Condition C1: depending on the tap-change method, at least one of the two controls, TapChg or TapPos shall be
lused for manual operation. BndCtrChg may be optionally used to change the value of BndCtr by commands.
5.4.6 LN: Voltage control Name: AVCO
For a description of this LN, see IEC 61850-5. This LN shall be used for a voltage controller
independent of the control method being used.
LNName The name shall be composed of the class name, the LN-Prefix and LN-

Instance-ID according to IEC 61850-7-2, Clause 22.
[Data objects
|tocKey [SPS ____flocalorremotekey 0
a
loner [ss __[boctesbyearmiaut ———SSSSSCS~S~S~
Jaxaov [SPS _[bostosycurentintowriow ——SSSCS~S~wCYO
lanvov __[s°5__[octesyotageimtovertow ————SSSSS~«wdC
[Controts
[OpCntAs JING. [Resettable operationcounter_ CJ

a
an
https:/www.doc88.com/p-80980482981320.htm! 33/185
```


## File page 034

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
-32- 61850-7-4 © IEC:2010(E)
tas
Stam’ disomy) mmm
name
[esa (src _——(Swtchrgautonyataitoniet ———SSSCSCS~«dCi
Tspcng [asc __|onangevoage (sop. nner. owe) ——SSSSS~S~di
svt nec _\otage seein SSSSSSCSCSC~S~dCS
Jao (5° [Auomateopeaion SSS
Settings
lunaov [ASG _[eunentinierovenownicwe ————SSSS~«~Ci CS
[unvor [ASG otage itr vertow oceng ————SSSS~«dO
5.5 Logical nodes for control LN Group: C
5.5.1. Modelling remarks
Table 4 — Relation between IEC 61850-5 and IEC 61850-7-4 for control LNs
Defined in | Modeled in
IEC 61850-5 | 1EC 61850-7-4
by LN by LN
feeorsenean fom fan [GEST
5.5.2 LN: Alarm handling Name: CALH
For a description of this LN, see IEC 61850-5. CALH allows the creation of group warnings,
group indications and group alarms. The individual alarms, which are used to calculate the
group indications/alarms/warnings, are subscribed from elsewhere. The calculation is a local
issue, usually performed by a logic scheme.
a ts
Data object
fame
LNName The name shall be composed of the class name, the LN-Prefix and LN-
Instance-ID according to IEC 61850-7-2, Clause 22.
[Data objects
nT
5.5.3. LN: Cooling group control Name: CCGR
This LN class shall be used to control the cooling equipment. One instance per cooling group
shall be used.
2 +
LNName [The name shall be composed of the class name, the LN-Prefix and LN-
Instance-1D according to IEC 61850-7-2, Clause 22.
Data objects
loptmm NS [Operationtime ———SOSCS~S YO
a
“a
https:/www.doc88.com/p-80980482981320.htm! 34/185
```


## File page 035

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
61850-7-4 © IEC:2010(E) -33-
pmpowcur [ss [Punpoverunentiip —SSSCS~S~S~S«~i
nO
[Measured and metered values
ac ac
[orrmpin Mv _[Oitemperaturecoolrin SCS
loutmpout __|Mv_—‘[oittemperature coolerout_ J
loimoia wv |owcneutation motor ive urent————SSSSSSSSCS~d
[FanFiw Mv |airflowintan
[ceTmoin wv [Temperature af secondary cooing medumin «i |
[CeTmpout |v Temperature of secondary cooing medium out ————~i fo |
[CePres WV Pressure of secondary cooing medum ———~—S~S~S~S~S
[CeFiw |v |Fow o secondary cooing medium SSCS J
[Fann |W |Wotorarve curenttan SSS JO
[eomrots
[cen [SPC [Cone af automate Tmanval peraion Boating) ~~ JO |
|cecu {sec [Control ot complete cooling group (pumps andtans) | JO
lpmecicen enc [cont otalpumps SS SSSSSCS~CS
RT
|FancuGen ENC __|Controiotattang
ncn [ene [eonslctasngetan SSCSCS~«dCiCC
jauo [spc [automatic ormanua
[settings
loirmpser [asc [Setpoint tor otemperatwre
5.5.4 LN: Interlocking Name: CILO
For a description of this LN, see IEC 61850-5. This LN shall be used to “enable” a switching
operation if the interlocking conditions are fulfilled. One instance per switching device is
needed. At least all related switchgear positions have to be subscribed. The interlocking
algorithm is a local issue.
ame dom) men
fame
LNName [The name shall be composed of the class name, the LN-Prefix and LN-
instance-ID according to IEC 61850-7-2, Clause 22.
[Data objects
5.5.5 LN: Point-on-wave switching Name: CPOW
For a description of this LN, see IEC 61850-5. This LN shall be used if the circuit breaker is
able to perform point-on-wave switching. In this case, the start signal for CPOW is OpOpn or
OpCls to be subscribed from CSWI. Then CPOW shall perform its entire dedicated algorithm
using data objects from the allocated TCTR or local and remote TVTR (local issue) and shall
then release a “Time Activated Control” (see IEC 61850-7-2) to XCBR. OpOpn and OpCis shall
be used if no “Time Activated Control" services is available between CPOW and XCBR.
Alternatively, CPOW mav be started by a control service acting on data object Pos.
an
https://www.doc88.com/p-80980482981320.html 35/185
```


## File page 036

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
-34- 61850-7-4 © IEC:2010(E)
— | Ear" ||
Instance-ID according to IEC 61850-7-2, Clause 22
[Data objects
[Status information
fingie (SPS [Maximum alonedtine ceed SSS
force fact open swicn fo
jovcis fact [ese swien Cf
[Comtrots
[Settings
IMexorimms [NG Masimum alowsaaeaytine ———SSSSCS~S~S~«~CiO
5.5.6 LN: Switch controller Name: CSWI
For a description of this LN, see IEC 61850-5. This LN class shall be used to control all
switching conditions above process level. CSWI shall subscribe the data object POWCap
(“point-on-wave switching capability") from XCBR if applicable. If a switching command (for
example Select-before-Operate) arrives and point-on-wave switching capability” is supported
by the breaker, the command shall be passed to CPOW. OpOpn and OpCls shall be used if no
Control Service is available between CSWI and XCBR (see GSE in IEC 61850-7-2).
a ee al
Instance-ID according to IEC 61850-7-2, Clause 22.
[Data objects
lckey (SPS acaloremowny SSCS
loc _[s®5_acaleonarnenavaur SSS
foropn _[acT [Operation Open swe” tO
[ston ses _[socton-Openswiew’———SSSSSCSCSC~S~S~Sw
loncs [ac _jopoaton"Cose wey” ——SSSCSCSC~S~S~
[sce [ges [seston Coss wen ———SSCSCS~S
[Controts
loscas [NC [Resenanieopeationcouner———SSSSCS~S~S~S~«~i
lesa [SPC __[swtenrgaunontyatstatoniewt «dO
ros orc _[swicn.omneas SSS
rk forcement SSCS
°
6
Ly
8
an
https:/www.doc88.com/p-80980482981320.htm! 36/185
```


## File page 037

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
61850-7-4 © IEC:2010(E) -35-
5.5.7 LN: Synchronizer controller Name: CSYN
For a description of this LN, see IEC 61850-5. This LN class shall be used to control the
synchronizing conditions i.e. voltage, frequency and phase.
o——_| _ Gavuneraisranr == [| |
Instance-ID according to IEC 61850-7-2, Clause 22
Data objects
Lockey srs localorremoteney
llc ss tccalcontrottenaviowr fo
joma_ ses [Breaker closing command
[Ret sPS [Breaker closing commandreteased
Av gps raisevotage fo
[vss fowersatage—SSSSS—~S
free [sais woavency (ncreaso spe ———SSSSSCS~w
(ue [ss _ltowertreqency owerspee) —SSSSSCS~S~S~C
[nd [SPS otage aternce naewor SSS J
[rane [ss row atoneningenir SSS
Ios |s5requenyateensingeatr SSO
[ror [ENS [Rotation recon (Cockwise | Couierciocwise Unknown) | [O |
|Measured and metered values
lowvcic av [Caused aterorce nvotaneamoiue vate) _———~—S—=diO_|
Jomtzcic fav ___Catcwated aiterence intrequency fo
Jouangce [uv __[Catcuated atterence ot prase angie | fo
vice fv ampinvde vate yy Cf
jwece fv ampmnoe vate Up
[Hevcie fw Frequeneyy fo
[azzcie [wy Frequeneyfg fo
Jacccie fv acceleration fo
Iccicdev ww [accomratonevaton———SSSSCS~S~S«~O_
[Controts
nn A
lccsia [sR [sienna auton atsttoniova SSS
[sre [so [stat andwopsrewonsing roars ———SSSSSSS~«d
JReoeasie [sc _|rneasing das bus ead ine uneion ————~—~S~wdC
pp pear LP |
[paralleling | Manual | Test)

Settings
|vnom ASG [Nominal secondary vottage
[HeNom —[as@_[Nominattequency
Wvaspract [as _[atptaton ator iid SSO
Idearadeo [ASG [paton ara e. setira soup canpensaion ——————_—+ fo
[ormms NG [Supervision ime for praleing (lay tme) __———S~wd
jutcnd [SP conmandgererion ———SSSS~S~S~i
[ong Aso __[oterene votane amotio vate)reonive————SSSS—*dO_
a

Ly

8

an

https://www.doc88.com/p-80980482981320.html 37/185
```


## File page 038

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
-36- 61850-7-4 © IEC:2010(E)
SYN ss
MO!
b c
[oteng ASG [tence temuesy nave SSS
[omurs asa _[otoroncerenvncy poste ———SSSSCSCS~«w
[avons aso _[oernce tase rule nepstve ————SSSSSCSC~dO
lotwars [aso [orca pase aniepostne —————SSSSSCS~wiO
lunvsyn —_AS0rinum votage ore sycheniaton —————SSSS—«d
Inaveyn [ASG Maximum votage for ive srewonsaion—————S—S—~id
[Detsyn [ASG [oetectionot synchronism ay
[wovancd leno |tveaeaanose SSS
[iminval [ASG ive nevane SSS
[wove aso [ive bunvaue SSCS fo
(ves [5° otape mat OWTOFF iO
a
a
lunvrams [NG primum votage adixinon pusewme ————SSSS—=d
Iwaxvtmms NG Maximum voage agusimentpuseme ——=SS~dCi_
a
[hic [ASG reqereyaohurinent haracionsis SSCS
[sims Wo reguney adusinentpuseinenat ——————SS—~d
luetetmns [NG Mina taquney agusinont uses —————SSSS—*d |
Imatetnms [NG Maumum teaver ausinentpuse me ———SSSS~«d
fetawal AS [remaneymacertwgstvave ———SSSSS—~d YO
irre [sro Ketorpusw onror SSS
losretams |G [boay ol syrewonzaton roses atersansensi —————~+ fo _|
Fertmms NG [Totatne ot synchronising process ———SSSS~*«~didN_
Condition C: at least one of the data objects (Cmd, Rel) shall be used.
5.6 Logical nodes for functional blocks LN group F
5.6.1. Modelling remarks
This group of logical nodes represents various types of control function blocks. Logical node
classes of this type do include some form of control algorithm. The LNs will normally be part of
a logical device providing overall functionality within the system. Therefore, no description and
requirements for the logical nodes of group F are given in IEC 61850-5. The LNs of the F-group
are never located at the border to the process.
The LN classes of the F-group shall be used only if another LN class from other groups does
not fit to the semantic and function to be modelled.
5.6.2 LN: Counter Name: FCNT
Logical node FCNT shall be used to count incoming pulses not related to the electrical network.
a
nw
https://www.doc88.com/p-80980482981320.html 38/185
```


## File page 039

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
61850-7-4 © IEC:2010(E) -37-
| Seine [| |
Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
[eS [antcomtanectonupwra SSS
lon ____|s®5 __lastcoun reciondowmard SCS
|Messured and meteredvalues
5.6.3 LN: Curve shape description Name: FCSD
Logical node FCSD shall comprise the data object classes that represent curve shaped output
values. The values can be dynamically modified online. The curves entered in the table can be
based on statistics obtained following a series of index tests.
The logical node is used to adapt an incoming value to a specified curve function. For example,
it can be used 2-dimensionally to adjust nonlinear transmitters to the correct physical values or,
by instantiation, can be used for 3-dimensional surface mapping.
em | bewenereinme ee [|
Instance-ID according to IEC 61850-7-2, Clause 22.
[Data objects
[Measured and metered values
fou Pv fous
jSettings
lew esc feurvesnape
5.6.4 LN: Generic filter Name: FFIL
Logical node FFIL shall be used to filter an incoming value. For a more detailed description of
the functionality behind FFIL, see Annex A of IEC 61850-7-410.
em | _beeenereinme = ||
Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
[Measured and metered values
lentom hv [eonroiop mination wrorvane———SSS~w
Settings
Ike (as_rnoriontgan SSS
[rmims fin rime tims fo
Fintims [NG ftmer(eadinsy SSCS
a
an
https://www.doc88.com/p-80980482981320.html 39/185
```


## File page 040

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
-38- 61850-7-4 © IEC:2010(E)
Fnaume NG ime ieasiml SSCS
fimoms NG -itmeaims) —SSSCSCSC~S~—S
loeasna asa [oeactang Cfo
5.6.5 LN: Control function output limitation Name: FLIM
This logical node is used to set temporary or permanent operational limits to an output signal
(MV) from a control function. The FLIM logical node should not be used to replace FXOT or
FXUT.
LNName The name shall be composed of the class name, the LN-Prefix and LN-
Instance-ID according to IEC 61850-7-2, Clause 22.
Dataobjects
[atin (SPS mit ocho put soa equal io arabove my | > _|
[ctim __|ss tow mit each input signal equal tor below iy | [0 _|
[Measured and metered values
Settings
Iauimser [586 ohinwsewomt———SSCSCSC~S~S~S«S
leumser [asc Mnimumimtsewont———SSSSCSCSC~S~dC_
5.6.6 LN: PID regulator Name: FPID
Logical node FPID shall comprise the data objects classes that represent proportional, integral
and derivative information for a PID controller. For a more detailed description of the
functionality behind FPID, see Annex E.
‘Common
__enetnome [ancmme mmm
| Earner |
Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
(Measured and metered values
Jom iow
Pati Prpontnaiacion——SSSSSSS~S
fwtem WW _[Conwolcopernnaionerervae—=S*~*~S~«~dCi Cd
Settings
Pong ENO opirowor@, SSS
ie ps _ronoronatgan SSCS
foo pene
ioteme no _[ootwatve mena) SSS |
a
an
https:/www.doc88.com/p-80980482981320.htm! 40/185
```


## File page 041

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
61850-7-4 © IEC:2010(E) -39-
Pe cl
data class
Joratmms [ING [Oerivaivertime ter(may
Bsr —~*(ASG—(iiasacetoprocesnvaravie ———SSCSCS*~*~*~“~*~*~*~*~dCiC~C*@d
jum so _[anininaupinegratinn ———————SSSSSS~d S|
loop [ASG eran change in tecivo upon at maximum ain | Jo |
PID algorithm selected.
Table 5 — Conditional attributes in FPID
PIDAIg
(M-Mandatory, O-Optional, Blank-Not Used)
rat fo | [fo fo | fo |
jut | fo [fo | fo fo |
jot | | fo | fo fo fo |
CC CC
a COC GC
[itmms | fw Tw ww
a AG CA CC CC
[ormms | ttm fw ww
[orutmms | | fw [fw [mw fm _|
5.6.7 LN: Ramp function Name: FRMP
Logical node FRMP shall be used as a generic ramp. The LN is required due to the fact that
the data object attributes of the ASG common data class do not contain all of the information
required to achieve a full ramping function with divergent up and down trends.
em | Eavenercinmer = [|
Instance-ID according to 1EC 61850-7-2, Clause 22.
Data objects
jaaist_ens _stato ot acjustmem process
ou RampaumeSSSCSC~—~—“—~sS*S~*~S
levtam [ww [Convo ieprninaton erorvane——~S~S~S~«w
Settings
[Rmoup [ASG ___|Ramping rate ona upwardtend JO
[Rmpon [ASG ___|Ramping rate ona downward tend
were [ASG [sop ze wren uring rom napuve oparine arecion | fo
[wong [ASG __|step size wnen suring rom postive onegatve restin | [0 _|
5.6.8 LN: Set-point control function Name: FSPT
Logical node FSPT shall be used to provide the common characteristics found in all controller
or regulator type logical nodes. The LN can be standalone or cascaded with other logical nodes
to form a complete controller.
a
Ly
8
an
https:/www.doc88.com/p-80980482981320.htm! 41/185
```


## File page 042

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
-40- 61850-7-4 © IEC:2010(E)
Data
object name
— | Emaar" ||
Instance-ID according to IEC 61850-7-2, Clause 22,

[Data objects

loc (SPS [osalconrbenavowr SSCS

[SerDvaim [ses [oeviation aterm

[setup ses [Setpoint going up raising) © fF

[son ses [sept gona ue (ower) ————SSSSCSCSCS~S~S~w

[soir ses [Setpoint direction Cf

[setenast [ens _[endsttusosetpontconrer ————SSSSCSCS~S~S~w

[Measured Values

[Semem [uv [Setpoint inmemory

Jevtom [uv [cont oop wminaton erorvae ——SCS~Sw

[Contras

Jeicna BAC [Spit charge at, owe) [ep _|

fsewvar fac |etpoime

fae [SPC __[Auomateopoaion SSCS

(Settings

5.6.9 LN: Action at over threshold Name: FXOT

Logical node FXOT shall be used to set a high-level threshold value to be used in control

sequences. If a second level is necessary, a second instance can be modelled. FXOT can

typically be used whenever a protection, control or alarm function is based on other physical

measurements than primary electrical data.

=| Eanes = |

Instance-ID according to IEC 61850-7-2, Clause 22.

Data objects

Settings

sortase [NG [Reset opereceayimetn) —————SSSSCS~S~S~«wdCi

5.6.10 LN: Action at under threshold Name: FXUT

Logical node FXUT shall be used to set a low-level threshold value to be used in control

sequences. If a second level is necessary, a second instance can be modelled. FXUT can

an
https:/www.doc88.com/p-80980482981320.htm! 42/185
```


## File page 043

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
61850-7-4 © IEC:2010(E) -41-
typically be used whenever a protection, control or alarm function is based on other physical
measurements than primary electric data.
0
| Sener [| |
Instance-ID according to IEC 61850-7-2, Clause 22.
jor __—act___levetotactonreacneg
Settings
a A
lopormns |G fopeaie ony meine) SSS JO
jsbitmme [NG [Reset operate coaytmetnsy SSS
5.7 Logical nodes for generic references LN Group: G
5.7.1. Modelling remarks
The logical nodes of group G shall be used only for modelling functions without a dedicated
logical node with appropriate semantics. Therefore, no description and requirements for the
logical nodes of group G are given in IEC 61850-5.
5.7.2 LN: Generic automatic process control Name: GAPC
This node shall be used only to model in a generic way the processing/automation of functions
that are not predefined by one of the groups A, C, M, P, or R. If needed, all data objects listed
in Clause 6 of this standard can be used single or multiple for a dedicated application of LN
GAPC. Data objects with proper semantic meaning should be preferred. The extension rules
according to IEC 61850-7-1, Clause 14 shall be followed.
0
Data
|_onpetname |antncane| SS mmmnem D
LNName [The name shall be composed of the class name, the LN-Prefix and LN-
Instance-ID according to IEC 61850-7-2, Clause 22.
[Data objects
locker SPS [iasaloreomoiwiey ——SSS~S
[oc [ss aca eontelnenvour ———SSSS~S
Into [so [puommicopwrton SSS
jors_ act foperate fo
Inn [55 foeneiesigieatem SSS JO
[wor |ss _[oeneneainge waning ——SSSCSCSC~S«~
inet |s*5_Joenenc angie nacaion SSS J
[Comtrots
lopcnte NO [Resoabe opomtoneaunr SSS
[vcs [sro _[swtcnng autor atwatoniove —_—————S~d
Icsor _|src [sae po convotabie situs pt ——————S—S~d
lopcsor [orc go [pow sont convolave sas oupt ——————SSS~«d YO
nw
https://www.doc88.com/p-80980482981320.html 43/185
```


## File page 044

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q

-42- 61850-7-4 © IEC:2010(E)

a <0

jscso1 [Nc |egerstausconvaiabentueoupa ————SSSSSC«dC

Settings

5.7.3. LN: Generic process /O Name: GGIO

This node shall be used only to model in a generic way process devices that are not predefined

by the groups S, T, X, Y, or Z. If needed, all data objects listed in Clause 6 can be used single

or multiple for a dedicated application of LN GGIO. Data objects with proper semantic meaning

should be preferred. The extension rules according to IEC 61850-7-1, Clause 14 shall be

followed.

po I tass

Data
Lente incom) men
e~__| Eeeaerennrar ne |
Instance-ID according to IEC 61850-7-2, Clause 22.

Data objects

[EENone [DRL [Eernalequmentnanepate———SS~wdCi

[cc |5°S [toca conotbenavowr SSCS

[oust |S |taeratnsit SSS

Jwent [ses [General single warning =f

inst __|s°5 [sonal naeaion Oranvnpwy) SSS

[Measured and metered values

frou [APC [Conran anaegieoupst ————SSSSCS~S~S~S

[Controis

loscas [NC [esenanieopeatoncouner———SSSCS~CiO CS

[ees [src __[swtenrgauonyatstatoniet ————S«d

[spcsor Pcs pot conrolabiestus upst ———S~S~S~w

lopcsor [orc [Dowie pam conrotane sus ouput —————SSSS~wdi

[scsor [NC |megerstans eowoiobestaus unt ——SSS~«~dC

5.7.4 LN: Generic log Name: GLOG

The LN GLOG refers to a function which allows to log not only changed data itself but also any

related data being defined in the settings of LN GLOG. The logging is started by the changed

data object (TrgRef1) or by the operator (LogTrg). The logged data are identified by the

references to the related source data objects in the data model.

a
“a
https:/www.doc88.com/p-80980482981320.htm! 44/185
```


## File page 045

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q

61850-7-4 © IEC:2010(E) -43-
po ROG tas
LNName ‘The name shall be composed of the class name, the LN-Prefix and LN-

Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
se | eee IE
lonly
[Contras
locus [Wo [Foon te wens oppaeeotabey ————SSSSSS~«d
[eats [sO roger twas by ewer SSS
(Settings
loonet on Rowenotog SSS
FraRett Jone ger rtrence shows he eavna vanersra—————S—*d

Reference to data objects / data attributes to include in LOG according to

IEC 61850-7-2, when the configured trigger was active
5.7.5 LN: Generic security application Name: GSAL
This node shall be used to monitor security violations regarding authorisation, access control,
service privileges and inactive associations.
SAR as
LNName [The name shall be composed of the class name, the LN-Prefix and LN-

Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
[Status information,
fcscurei _|s¢ [asa cont anes cose ————SSS~d i
[ever [550 [Seven prviege vans ———SSSSS~S~w i
ina ]8€0Inacve stocatone SSS
ca
[Controts
jopcmms inc [Resetiabie operation counter
5.8 Logical nodes for interfacing and archiving LN Group: !
5.8.1 Modelling remarks
The Logical Nodes of group | shall be used at the border of the IEC 61850 modelled system
with the exception of the process interface (e.g. to operator by IHMI, to external communication
system by ITC!) or where the modelling level is changed (proxy).
5.8.2 LN: Archiving Name: |ARC
For a description of this LN, see IEC 61850-5.

a
nw
https://www.doc88.com/p-80980482981320.html 45/185
```


## File page 046

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
-44- 61850-7-4 © IEC:2010(E)
—— | Samaras" [|
Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
ns
[Memusea NS [Memoryuseginge
INomncs INS [Acwainumber otros ——SSSCSC~S~S«~_
(Settings
ca
jntoor [ORG _[Reerenes wanwovens —SSSSCS~S~dCi
IMeawned [NG Maximum number ofreorde———SSSCS~S~S~w
sates [en [Recor peaton mode atuatonovewiey —————SS~«d
IMenfut (NG (Menowtatiwer SS SSSSC~«diO
5.8.3. LN: Human machine interface Name: IHMI
For a description of this LN, see IEC 61850-5.
— | iar ||
Instance-ID according to IEC 61850-7-2, Clause 22.
[EEName DP [Exemaienipmeninane pate ———SCS~S~SwdCO
[eekey [5° [scalorenoeney SS
[cc |ss hoes convattenavow——SSCS~S~S«w
[Controts
lecsta _|5°C__[Swichingautorivatationiewt ————SSSCSCS~S~S~drC~~=S
5.8.4 LN: Safety alarm function Name: ISAF
For a description of this LN, see IEC 61850-5. Logical node ISAF shall be used to represent an
alarm push-button or any other device that is used to provide an alarm in case of danger to
persons or property.
— | hana [|
Instance-ID according to IEC 61850-7-2, Clause 22.
[EENone [DRL [Exeraleqiomentnanepate———SSSSC«dC
ry
an
https://www.doc88.com/p-80980482981320.html 46/185
```


## File page 047

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)

61850-7-4 © IEC:2010(E) -45-
[eHeamn [ENS [Eenalequpmentneamm SSS
Jam [ss _[satey alam reson, aie ———SSS~S~S~i
loncnits [WG [Resonabio oporston commer ———SS~S~Sw
5.8.5 LN: Telecontrol interface Name: ITCI
For a description of this LN, see IEC 61850-5.
eo | beenerennras =e |

Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
[Nene [A [Exwraleaspneninanepate———SSS~S~S~wdiO
[cc [5S foealconvainenavow ———SSSCS~S~S~S~
[Controts
lecsia [SPO [Swicnng atom ataatoniowe SSS
5.8.6 LN: Telemonitoring interface Name: ITMI
For a description of this LN, see IEC 61850-5.
—_| aaa =e |

Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
[EeNane [oP [Eenalaupmentmame pate SSS
5.8.7 _ LN: Teleprotection communication interfaces Name: ITPC
For a description of this LN, see IEC 61850-5. The LN ITPC comprises all information for
communication channel setting and supervision. ITPC is not intended to generate direct
process data objects. Thus, it does not contain the input and output data objects to be
transmitted and it has no ‘operate’ data objects object.

Data object
a
LNName The name shall be composed of the class name, the LN-Prefix and LN-
 |Instance-10 according to IEG 61850-7-2, Clause 22.
an
https://www.doc88.com/p-80980482981320.html 47/185
```


## File page 048

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 718 > @ Q_ View A mark Y Annotations v | Q
-46- 61850-7-4 © IEC:2010(E)
]

[Data objects

Descriptions

icnane O° [Eaersegipnentranepive ———SSSSSSC~di

\GrdRxCmdRx [Alarm situation: Guard received together with the command, may indicate
interference on the channel. Used in case of an analogue communication

nel.

[LosSig [SPS __|Alarm situation: No signal received, indicates a channel problem ‘jo |

[TxCmdGntt __|INS__|For diagnostics: Transmitted commands counters (for each command) | |O |

[RxCmdCrtt | [For diagnostics: Received commands counters (for each command) ‘jo |
[Alarm situation: Loss of synchronism. Indicates that there is no
Isynchronization between the transmitter and the receiver, i.e., no

communication is possible. Used in case of a digital communication
shannel.

[Measured and metered values
[Bit error rate of the communication channel. Used in case of a digital
communication channel
Frame error rate of the communication channel. Used in case of a digital

ommunication channel. May be vendor specific

[Carlev __—_—_—([MV_|Power of received signal in case of an analogue communication channel | |O |

snr [MV._[Signal to noise ratio (in dB), used in case of analogue communication | |O |

[eostentin ww [tineneasuredatiasticontet ————SS*d

Settings

[NumTxCmé JING __ [Number of used binary transmit commands [jo |

Icom [NG obo ve iar ec commande i

TpcTxModt Teleprotection application mode in transmit direction for each command
(Unused, Blocking, Permissive, Direct, Unblocking, Status)

TpcRxModt Teleprotection application mode in receive direction for each command
(Unused, Blocking, Permissive, Direct, Unblocking, Status)

SecTmms Pickup security timer on loss of carrier guard signal: if a command is
|received within SecTmms after the guard has disappeared, this command
lis considered valid, used in case of an analogue communication channel.

[Level of increased power during the transmission of a command in dB.
|Used in case of an analogue communication channel

xPwr Transmit power (peak envelope power) in dBm. Used in case of an
lanalogue communication channel

ee a center frequency. Used in case of an analogue communication He |

+hannel

[rome ps Receive center frequency. Used in case of an analogue communication le |
channel

[TxBndWid _—-|ASG_—_| Transmit bandwidth. Used in case of an analogue communication channel| |O |

[RxBndWid [ASG _—_|Receive bandwidth. Used in case of an analogue communication channel | [0 |

INOTE EEHealth is used to indicate the state of the communication channel, whereas PhyHealth is used to

indicate the state of the (physical) communication device. If ITPC receives a GOOSE message with quality

attribute “invalid” or “questionable” or no GOOSE message at all within Tmax, it will set PhyHealth to “Warning”.

[Other actions are a local issue.

5.9 Logical nodes for mechanical and non-electric primary equipment LN group K

5.9.1 Modelling remarks

This group of logical nodes represents various devices that can be supervised, controlled or

operated but that are iy primarily of electrical nature. This group includes devices like tanks,

an
https://ww.doc88.com/p-80980482981320.html 48/185
```


## File page 049

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
61850-7-4 © IEC:2010(E) -47-
valves, fans etc. Therefore, description and requirements are given in IEC 61850-5 with
different level of detail.
5.9.2 LN: Fan Name: KFAN
Logical node KFAN shall be used to represent a fan. It can be seen as an extended nameplate
that allows the temporary setting of data object.
‘Common
|_onpetname |inrcuse) omen
ce a name shall be composed of the class name, the LN-Prefix and LN- |
linstance-ID according to IEC 61850-7-2, Clause 22.
Data objects
Descriptions
JEEName __[bPL_[Exteratequiomentnamepiate Jo
ae
lcckey [ss _sealorromotenay ——SSSCSCS~S
lootmn [Ins Opeatontine SSCS
|Measured and metered values
a
[Gomtroty
a a He]
[oesa [sr _[seterngautoryatstatonievt «|
Settings
IMnoptnm [NG [imum opertonine mines —————SSSSSC~«w |
\maostnm ING [Memon pwaiontine nmin «dO
5.9.3 LN: Filter Name: KFIL
Logical node KFIL shall be used to represent a (mechanical) filter. It can be seen as an
extended nameplate that allows the temporary setting of data object.
e——_| _Gaveneraisranr=—™ [| |
Instance-ID according to IEC 61850-7-2, Clause 22,

Data objects
JEEName __[oPt__[Esteratequpmentnamepiate J
eeHeatn ENS [Exel oquprentmetms ——SSSSSCSCS~«wdCi
lootmn [ns _fopoatontne SSCS
en
a
[More [SPS Motorprcion ions SSSSCSCS~«~dCi
[Fusn ses [Fimertushing Of

a

an

https://www.doc88.com/p-80980482981320.html 49/185
```


## File page 050

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
-48- 61850-7-4 © IEC:2010(E)
frum __|sps_[Fneraerm fo
[Measured and metered values
[orrosht [MV [Doron pessurooernemer ————SSSS~S~S~dCi CS
[Controts
Frusncnt [WG [Fer uhing counter vesemaby ————SS~S~S~S~«w
\ecsta [so _|swioring autoriyatatmioniever ———SSSSC~S~S~S~«~CO_—CS
Settings
5.9.4 LN: Pump Name: KPMP
Logical node KPMP shall be used to represent a pump. It can be seen as an extended
nameplate that allows the temporary setting of data objects.
Data
|_enetmme domes) em
| Sewanee [| |
Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
[EEName ___ [ort [Externatequipmentnamepiate
team [xe [eawraloqupeentewm id]
[oc [srs facaleanteloenao ——SSSSSCS~d
ltockey [ses tocatremotekey sf
[ota [ns fopeaton ne SSS
[sos (WV ——‘(Retatonalapoosotmepump ———SSSCS~S~S«wdi
[Comtrots
[s5a85t [APC |spod se in Gn cate aed reputed mot He
[osu [sro _[swtcnng aumortyatsatoniews = SSS—*di
Settings
Iunoptnn [WG inom opoaton ino wines —————SSSSSSCSC~«Sd
[msootmm NG Maximum operator neimminaes——————SSS~d YO
5.9.5 LN: Tank Name: KTNK
Logical node KTNK shall be used to represent the physical device of a tank, such as a
hydraulic oil tank. The tank can be pressurised or not. If used to represent a tank for
pressurised gas, only the pressure MV will be used. If used for an oil sump, only the level MV
will be used. For a simple level sensor, the SLVL logical node can be used instead.
‘Common
|_onjetnome [insmme, mem
LNName ‘The name shall be composed of the class name, the LN-Prefix and LN-
linstance-ID according to IEC 61850-7-2, Clause 22.
a
an
https:/www.doc88.com/p-80980482981320.htm! 50/185
```


## File page 051

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
61850-7-4 © IEC:2010(E) -49-
|_enjet mame _lentncnse| —mrmven IMD
0 data class
Data objects
[Name [pe [Enenslequpmentnanepate————SSSS~S~iO
eHean [ENS [Euenaloaupmonnam SSS
[Measured and metered values
levecr (MV [Lavolintho tank as porconage oltiankievs) ———~SC~drCdO
[Settings
\Wncap [ASG [oulvoumecanaay SSS
ina aaa
both pressure and level)
5.9.6 LN: Valve control Name: KVLV
Logical node KVLV shall be used to represent a valve or gate where the position can be given
as a percentage of full open position (optionally, the angle 0°-90° may be used). In case of dam
gates where either open or closed position depends on the water level of the dam, the HGTE
LN should be used.
Data
|_onjetname |cntncmme| mem
em | bevenereinmee me [|
Instance-ID according to IEC 61850-7-2, Clause 22.
[Data objects
JeeName __[oPL__[Exernai equipment nameplate J
[Status information,
ltcc_ ses local comtrotbehaviour fo
[oispos [ss [closed end postion reached (vate cannot move tune) | |r|
JopnPos [ses [Open end position reached (valve cannot move further) | ct |
fsck __|sPs_vaweris blocked cannot move trom present postion) | J
[Measured and metered values
rw__ fw ___[catcwatea tauis tow trougntnevave Jo
[Controts
JPosset__— [aPC Vane postion setpoint J
Jpos for Valve tout open orclosed position | J
JPoscha [asc [change vawve position (sop. raise, tower) | |?
[Poscnginer [inc incremental change of positon | fc
lence [spc alock opening tthevane fo
Jancis [sec [Block ctosing oftnevave ft
ltocsta [sec [Switching authority atstaiontevel ft
a
an
https:/www.doc88.com/p-80980482981320.htm! 51/185
```


## File page 052

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
-50- 61850-7-4 © IEC:2010(E)
KV as
Settings
[contin 86 Oponing init fae poston Gunporaywesvicioy | Jo _|
[Gat [ASG Joes it fave poston conpuray rescion) | Jo]
incr p80 ]nerament of poiton changer open/ ose commands |_|
Condition C1: At least one of the data objects shall be used. The use of both data objects is optional.
Condition C2: The use of the data objects is optional, but if used, only one shall be selected.
5.10 Logical nodes for metering and measurement LN Group: M
5.10.1 Modelling remarks
Table 6 — Relation between IEC 61850-5 and IEC 61850-7-4 for
metering and measurement LNs
Detined in | Modeled in
HEC 61850-5 | IEC 61850-7-4
by LN by LN
‘Three-phase version
Non-phase-related version (single phase)
DC-related version
waTr Metering (three-phase values)
MTN Metering (single-phase values)
Metering (statistics) ~ obsolete, moved to
Leschded annex
Three-phase version
Harmonics and interharmonics
Non-phase-related version (single phase)
MENV Environmental data objects
MENV
MMET Meteorological data objects
5.10.2 LN: Environmental information Name: MENV
Logical node MENV shall be used for modelling the characteristics of environmental conditions
such as emissions, and other key environmental data objects. In addition, many of the
environmental sensors may be located remotely from the instantiated logical node. This logical
node may therefore represent a collection of environmental information from many sources. It
does, however, not include basic meteorological and hydrological data objects. For such
information, see MHYD and MMET logical node classes.
po MEN ass
Data object MO!
name c
LNName ‘The name shall be composed of the class name, the LN-Prefix and LN-
Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
[Measured and metered values
[Noxems fv [NOvemissions
7 He
nw
https://www.doc88.com/p-80980482981320.html 52/185
```


## File page 053

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
61850-7-4 © IEC:2010(E) -S1-
a
lew [tod pessveieet———SSSSSSSS—id SO]
lzonoséas uv oneon in conbton gases io
josar_ fv ooneinay
(Settings
lors se [invotved incartontrading
ject [ASG [Carbon production credit vawe JO
[Gentag [sh [reentagintormation
[Partsens [ASG ___[Sensitiy toparicuiates
5.10.3 LN: Flicker measurement name Name: MFLK
This LN shall be used for calculation of flicker inducing voltage fluctuations according to
IEC 61000-4-15. The main use is for operative applications.
LNName ‘The name shall be composed of the class name, the LN-Prefix and LN-
Instance-ID according to IEC 61850-7-2, Clause 22.
[Data objects
[Measured and metered values
po eee LF |
imeasurements
ve: 'Short-term flicker severity of last complete interval for phase to ground
Imeasurements
ren oct gator flicker severity of last complete interval for phase to phase le |
Imeasurements
Joven ve fngterm flicker severity of last complete interval for phase to ground le |
|measurements
|PPPiMax _—*|[DEL_—_|[Output § — Instantaneous peak P value for phase to phase measurements| |O |
aS
Imeasurements
lPPaori pet Jou a~1 min average ol ou pave wo phase menswenens_| 0]
opuarn Joe ouput «1 mn average pu af rod meassromans | [O
[PPPioot [DEL [Output 8 - Square root of output 5 for phase to phase measurements | |O _|
spect [pe oupu 9 = Square root of ouput er phase 1 ground meanrenents | |
prefer erenstrmenerenn nerve]
ICA)
lepers usr [asin ins of last complete short interval for phase to ground (A, B, le |
)
Jpercou: usr —_|Gseter bins of last complete long interval for phase to phase (AB, BC, lo |
ICA)
[PhPcbLi [HST —_|Classitier bins of last complete long interval for phase to ground (A, B,C)| [0 _|
|PPPamWav __|HDEL _[Real time demodulated wavetorm for phase to phase (AB, BC, CA) [lo |
|PhPdmWav __—|HWYE _—_[Real time demodulated waveform for phase to ground (A, 8, C) [jo |
ry
nw
https://ww.doc88.com/p-80980482981320.html 53/185
```


## File page 054

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 718 > @ Q_ View A mark Y Annotations v | Q
-62- 61850-7-4 © IEC:2010(E)
Real ue cemoduated wavetorm spectra for phase to phase (AB, BC, le |
|PhPdmSpec _ |HWYE _[Real time demodulated waveform spectra for phase to ground (A,B,C) | [Oo |
5.10.4 LN: Harmonics or interharmonics Name: MHAI
For a description of this LN, see IEC 61850-5. This LN shall be used for calculation of
harmonics or interharmonics in a three-phase system. Instances either for harmonics (including
subharmonics and multiples) or interharmonics are possible depending on the value of the
basic settings, i.e.:
© frequency f ("Hz");
* evaluation window At (“EvTmms").
The frequency may either be given (HzSet) or calculated (Hz).
Both harmonics and interharmonics carry power and produce distortions. There are different
methods to calculate disturbances. For more information and definitions, see IEC 61000-4-7
(2002), IEEE 519-1992, and IEEE 1459-2000.
Same docs) me
name
— | eerie [| |
Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
(Measured and metered values 0
[HaA___[awve [Sequence ot harmonics oriternarmonics current | JO
JHpnv ______[awve [sequence ot harmonics or interharmonics phase to ground votes | |o_ |
[HPPV __—|HDEL__| Sequence of harmonics or interharmonics phase to phase voltages [jo |
[ew frwve [Sequence of harmonics or internarmonis active power | |
[Hvar [awve [sequence ot harmonics or internarmonics reacive power | |
IHva_____[awve [sequence ot harmonics or internarmonics apparent power | |_|
fears mnie [P|
distortion, Thd)
ia alll "ahead |
ito ground
al ee RMS harmonic or interharmonics (un-normalized Tha) for phase ie |
to phase
ia lll "=" lesional
lunsigned sum
WYE ‘Total phase harmonic or interharmonic active power (no fundamental)
signed sum
jiatn wre _foonentinepeae SSS
fee we eter SSS J
a
Fak WvE eure otal harmon or wisharmone dation faeen memos) | [O |
ThdOada WYE urrent total harmonic or interharmonic distortion (different methods ~
odd components)
[ThdEvnA WYE (Current total harmonic or interharmonic distortion (different methods ~
components)
a
an
https:/www.doc88.com/p-80980482981320.htm! 54/185
```


## File page 055

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 718 > @ Q_ View A mark Y Annotations v | Q
61850-7-4 © IEC:2010(E) -53-
]
Task (WE —_(Sonenoaldenardastoninperigeesio ———S«d
Tae0ssa Wve _|ourent tl demand sistorion per IEEE 519648 components) | (0 |
[TasEwa _WYE [Curent demand dtorton por IEE 51 (een conponeni) | fo _|
ThdPhV WYE Voltage total harmonic or Interharmonic Distortion (different methods) for
iphase to ground
a Sep
phase to ground (odd components)
[ThdEvnPhv WYE Voltage total harmonic or interharmonic distortion (different methods) for
iphase to ground (even components)
DEL Voltage total harmonic or interharmonic distortion (different methods) for
Iphase to phase
[ThdoddPPV DEL \Voltage total harmonic or interharmonic distortion (different methods) for
Iphase to phase (odd components)
|ThdEvnPPV DEL Voltage total harmonic or interharmonic distortion (different methods) for
iphase to phase (even components)
Voltage crest factors (peak waveform value/sqrt(2)/tundamental) tor
iphase to ground
DEL Voltage crest factors (peak waveform value/sqrt(2)/tundamental) for
Iphase to phase
[HCtA ———|WYE _| Current crest factors (peak waveform value/sqrt(2)/tundamental) [jo |
lett WWE _oteae weproneintventcor «LO
[Settings
[ertmme ING Evation me (ime window etrmines the west Hequeney | (0 _|
INimcre NG Number oteycentmeasciemeney ————SS«
Trsavat [ASG Tah lr sotingvabeeneregine ——SSS~«~dC
[mavvat [ASG __[TaPAV THAPPY tm sting vane enereain® ——————*dt
Imaatnms ING [IhsAalarmimeaiayinms ————SSSCSCS~diN—=S
[mavinms NG [ThaPav THAPPY atumnedoayinms ——————SS~w
Noma [ASG Noman demand cron veain IEEE S19 TOO cacvaton | [O_
5.10.5 LN: Non-phase-related harmonics or interharmonics Name: MHAN
This LN shall be used for calculation of harmonics or interharmonics in a single-phase system,
ie. a single line with no phase relations. Instances either for harmonics (including
subharmonics and multiples) or interharmonics are possible depending on the value of the
basic settings, i.e.:
© frequency f (“Hz”);
* evaluation window At (“EvTmms").
The frequency may either be given or calculated by means such as a phase-locked loop (only
possible for a dominant frequency like the basic power frequency). For the settings for
harmonics and interharmonics instances, see MHAI.
a
an
https:/www.doc88.com/p-80980482981320.htm! 55/185
```


## File page 056

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 1184 > @Q_~ View A mark Y Annotations ¥ Q)
-54- 61850-7-4 © IEC:2010(E)
|" SSaEee
linstance-ID according to IEC 61850-7-2, Clause 22.
[Data objects
[Measured and metered valves
[az fy Basic recency
[Havel |v [Sequence of harmonies orintrharmonis forvotages———*| 0 _|
Hawa [hmv [Sequence ot namenisortrhamenies fr ace power | (0 _|
IHaveianpr HV [Sequence of harmonies or temamoncs orreactve power | [0 _|
[Havoiamp [HV [Sequence of harmonies or interhermonies fr apparent power | |_|
IHanmsamp _wv_|Curont RMS narmoncoritrhaman (normalize Tos) | O _|
IHainsvei |v Votage RMS harmon or merharmenie(nemaizea Tho) | [0 _|
[HeTuwan MV [Tota hamonc or rterhamonc ate power (ofundarenta unsigned sum | [O |
ferewar PY fgimamenssr mena ene pone etnsanema ares |
|Haamptm [wv [Currenttime product
IMawract [wy ik tactor
[anne |v [Ourent ta harmon or herharmonieditorn (alerent mahoas) | [O |
Pee fr Ect rteneen eerie [P|
odd components)
=P ee
leven components)
[rasan WV [Curent demand stoion perigee sto —SS«d CO
[aacssamp av [Curent otal demand storton per IEEE 519 odd components) | |_|
[aaevmamp [MV [Ourent otal demand startin per EEE 519 even componens) | [0 _|
[Thavor____|wV___|Votage tot harmon or itemarmonie distortion (aiferent methods) | |O_|
i aac a
odd components)
ll aa" aldeeienaainedl tal
components)
[acta |W [ourent rst actor peak waveform vaelsr(@undamania) | [0 _|
Inacivel |v Votage crest actor peak waveorm vaulsq@vunamenta) | [0 _|
IHatiFact [wv Votage iphone intuenco racer SC*d (OY
[Settings
Hest [Basictequeney
lEvimms | ree envio ataneteloesteqany time (time window) determines the lowest frequency So
IMumcye NG Numberateyces ofthe baicteqweney |
[Thaaver [ASG _|ThaA alarm seting-vale enteeaine ————S~ri_
[mavwat [ASG _|TaV alarm soting vane norain%e ——SSS~*dt i
[thantams ING [ThsAatmiimecsayinns ———SSSCS~C~i~=CS
[tavinme NG [TheValumimedsiayinns CO
[Nowa ASG Noraising demand curent ved n IEEE 619 TOD calculation | |O_|
e’
cy
=)
nw
https://www.doc88.com/p-8098048298 1 320.htm! 56/185,
```


## File page 057

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
61850-7-4 © IEC:2010(E) -55-
5.10.6 LN: Hydrological information Name: MHYD
Logical node MHYD shall comprise the data objects that represent hydrological information
‘such as river, lake, pond, or oceanic water related information.
This logical node may represent a collection of meteorological information from many sources.
0
LNName The name shall be composed of the class name, the LN-Prefix and LN-
Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
a A
[rw Vive steam caaivounetietow ———SSS~*d
[eaSre WV [Sura apeodotairfow SSS
fine WW —empoatwootmar ——SSSS~d SO
enc ww feat onsvviyatwaier SSS
Inari iv otmar@ey ——SSSSCS~S
fn pw [sane conertotvairio ————SSSSS~d
Fiasco [oc [rncounrrsang SSO
5.10.7 LN: DC measurement Name: MMDC
Logical node MMDC shall be used to represent measurements in a DC system: current,
voltage, power and resistance.
cts
Data object
name
LNName ‘The name shall be composed of the class name, the LN-Prefix and LN-
Instance-ID according to IEC 61850-7-2, Clause 22
Data objects
jot bw vonage oC votape beweonpsies ————SSSSSSC~*d i
\voPsGnd [ww _\Votagebatweon postive pote argearn ————SCS~w*idC
[arwsand [nV Votage between ngaive pole andeath———SS~S~i
[nePeGrd WW [Resistance notwwon ponte pao angwarn ————SSS—*d |
[nsigooe [east botwwon pune pot an ween | Jo]
5.10.8 LN: Meteorological information Name: MMET
Logical node MMET shall comprise the data objects that represent meteorological information.
The data objects as shown in the following table focus on meteorological station information.
MMET may in reality represent a collection of meteorological information from many sources,
that is, from sensors located at different places.
a
nw
https://www.doc88.com/p-80980482981320.html 57/185
```


## File page 058

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
-56- 61850-7-4 © IEC:2010(E)
=| aime |
Instance-ID according to IEC 61850-7-2, Clause 22,
Data objects
lewtme fv [Ambient temperature
fwetserme [wv wetbuwtempernre
lcrouscw fv fctoudcoverver Cf
fEmvrum fy oumity fo
Joewer fw foeweome fo
[onnsor ww [omtuteinsoaton———SSSSSCSCS~S~S~S
lostnsor ww fovctormatinssnion SSCS
[now jw oie atone ops aweon sunrise andauneod | JO _|
Ionmaat [Mv [otal orzoninouton «dO
irowaor Yoon wnearecton SSS
ierwass fw owonaiwindspees ——SSSSSCSCSCS~dCO
jeewaor fw eriatwnaareion SSS JO
\vewssea _|w _\Weneaiwnaspees —SSSSCSC*~S«~
IWecusisos Wind guntspeed SSS
ewer [anomeric ress SSCS
freee fw rae fo
[swoon [uv [eneity ot snowien
feewtmp fv [Temperature ot nowt 0 fo
fsewcer fw soweover_ Cf
[sewn wy sown fo
[sewea fav [Waterequivaiontotsnowtan fo
5.10.9 LN: Metering Single Phase Name: MMTN
For a description of this LN, see IEC 61850-5. This LN shall be used for calculation of energy
in a single-phase system. The main use is for billing purposes.
i eae
Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
[Measured and metered values
[rowan facr___Netapparentenergy J
frown facr [Netreatenersy fo
ftowan fecr Netresctweenery
[Suswn BGR [Ret ney enoiy Gta sup racion’eery Row wars buna | [O |
[uevan [ocr Rss ers yo as pcr: en ow owt ba | O|
a ll =e
away)
SS See IF
lbusbar away)
a
Ly
8
an
https://www.doc88.com/p-80980482981320.html 58/185
```


## File page 059

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
61850-7-4 © IEC:2010(E) -57-
5.10.10 LN: Metering 3 Phase Name: MMTR
For a description of this LN, see IEC 61850-5. This LN shall be used for calculation of energy
in a three-phase system. The main use is for billing purposes.
tas
LNName [The name shall be composed of the class name, the LN-Prefix and LN-
Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
[Measured and metered values
favan (BGR etapa SSS
fravan [ocr Neteaanve ene SSS
[Supwh ___—_—[BCR__|Real energy supply (default supply direction: energy tlow towards busbar) | |O |
|Supvarh [BCR _[Reactive energy supply (dotault supply direction: energy flow towards busbar) | |O |
ina alll =a i
jaway)
in ll == dela i
lbusbar away)
5.10.11 LN: Non-phase-related measurement Name: MMXN
This LN shall be used for calculation of currents, voltages, powers and impedances in a single-
phase system, i.e. in a system where voltages and currents are not phase-related. The main
use is for operative applications.
<1 ~
Data object
name
LNName ‘The name shall be composed of the class name, the LN-Prefix and LN-
Instance-ID according to IEC 61850-7-2, Clause 22.
lamp _____(MV__[Gurtent notalocatedtoaphaseSSSSCSCSCS~S~S~S~diO
[vol ww wotage V not aocaiegio phase ——————SSSS—i
[wat |wv [Power (P)notalocatedtoaphase SSCS
\Vowsmpr |W _[Reactive power (@) not alocatogoaphate ———S~d
[Vowsmp wv [Apparent power (not allocated to aphase _——S~d |
[Pwract MV [Power factor not alocatedtoaphase————~—S~S~S~S
imp low [impedance SSS
Ie [Frequency SSCS CS J
5.10.12 LN: Measurement Name: MMXU
For a description of this LN, see IEC 61850-5. This LN shall be used for calculation of currents,
voltages, powers and impedances in a three-phase system. The main use is for operative
applications.
i tas
Data object mor
fname c
LNName The name shall be composed of the class name, the LN-Prefix and LN-
Pa Instance-ID according to IEC 61850-7-2, Clause 22.
“a
https:/www.doc88.com/p-80980482981320.htm! 59/185
```


## File page 060

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
-58- 61850-7-4 © IEC:2010(E)
Data obietts
[Measured and metered values
Row WW a ave gover ti in
owas acive pve ota! He
Tova jw ro appro over 9) He
Keir wv Tavera power act oa} i
a
[PPV = |DEL__|Phase to phase voltages (VL1,VL2, ...) [jo |
Jenv [Wve [Phasetonouralvoage
[hv |WYE __ [Phase to ground voltages (VLIER, ...) [lo |
AWE [Phase currents (us.uzius) fF
lw ]wee Par active power He
(vr wre Pas reacv owe ( He
(ve we Ps pret ower) ie
ir wre Pas poveracr i
2 wre Pave mpoaancs |e
|Arithmetic average of the magnitude of current of the 3 phases.
JAverage(la,tb, 1c)
[Arithmetic average of the magnitude of phase to phase voltage of the
'3 phases.
‘Average(PPVa, PPVb, PPVc)
[Arithmetic average of the magnitude of phase to reference voltage of the
|Average(PhVa, PhVb, PhVc)
[Arithmetic average of the magnitude of active power of the 3 phases.
|Average(Wa, Wb, Wc)
hmetic average of the magnitude of apparent power of the 3 phases.
lAverage(VAa, VAb, VAc)
‘Arithmetic average of the magnitude of reactive power of the 3 phases.
‘Average(VAra, VArb, VArc)
JArithmetic average of the magnitude of power factor of the 3 phases.
lAverage(PFa, PFb, PFc)
lArithmetic average of the magnitude of impedance of the 3 phases.
lAverage(Za, Zb, Zc)
‘Maximum magnitude of current of the 3 phases.
IMax(ta,tb,tc)
Maximum magnitude of phase to phase voltage of the 3 phases.
|Max(PPVa, PPVb, PPVc)
Maximum magnitude of phase to reference voltage of the 3 phases.
\Max(PhVa, PhVb, PhVc)
Iwoxwens MY (Masi magnitude of active power of the 3 phases. ie
‘Max(Wa, Wb, We)
Maximum magnitude of apparent power of the 3 phases.
MaxVAPhs we IMax(VAa, VAb, VAc) Ie
Maximum magnitude of reactive power of the 3 phases.
IMax(VAra, VArb, VArc)
Maximum magnitude of power tactor of the 3 phases.
IMax(PFa, PFb, PFc)
Maximum magnitude of impedance of the 3 phases.
IMaxiZa, Zo, Zc)
6’
nw
https://www.doc88.com/p-80980482981320. html 60/185
```


## File page 061

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)

61850-7-4 © IEC:2010(E) -59-

[Minimum magnitude of current of the 3 phases.

[Min(ta, tb, lc)

[Minimum magnitude of phase to phase voltage of the 3 phases.

IMin(PPVa, PPVb, PPVc)

[Minimum magnitude of phase to reference voltage of the 3 phases.

IMin(PhVa, PhVb, PhVc)
a a

Min(Wa, Wb, We)
Sa

IMin(VAra, VArb, VArc)

[Minimum magnitude of reactive power of the 3 phases.
pawns fis P|

|Minimum magnitude of power factor of the 3 phases.

IMin(PFa, PFb, PFc)

[Minimum magnitude of impedance of the 3 phases.

IMin(Za, Zb, Zc)
Settings
[GetawvA [ENG _[blasaton mead ued ort apparent power TWA | P_|
son [enG__[SonconventoniorvArandpovertaaer(®A) «dL
5.10.13 LN: Sequence and imbalance Name: MSQI
For a description of this LN, see IEC 61850-5.
LNName ‘The name shall be composed of the class name, the LN-Prefix and LN-

Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
[sean (SO [Polvo negative ard soo sequence caren ——SSCS*~*~S*«~iz~=C*d
[sav |S€0__[Postve,ngutve and eo seaereevotane «dC
loonsea _|se0 _[oodsenenee SS SSSSCSCSC~S~CO
moa WWE [moans cures ———SSSSSCSCS~S~S
mono pV |nosaneenepave seauencneonent——SSSC«dC
jmonov wv _|mbatanceneostve sequence vonage ——=SSCS~S~S~«~rCS_CS
jmorev DEL _|mbatance pase stare votane————SSSSSCS~S~S~dCi
mov WWE _|mosaneevotage SSS
imezon pV [noses zoo seavercocoret ———SSSSSCSCS~S~Ci
imazrov [wv _|mbaance sero sequence vote ———SSS~S~S~«~
[Msimarev nv Moxmum imbainee pss prasevenege ———SSSSC«dC
jMeimv pv Maxmuminbatreevotane ——SSSC«dC

ry
an
https://www.doc88.com/p-80980482981320.html 61/185
```


## File page 062

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
-60- 61850-7-4 © IEC:2010(E)

5.10.14 LN: Metering statistics Name: MSTA

This LN is moved to Annex C because it includes the calculation methods MAX, MIN, AVG etc.

and is therefore obsolete for this edition of the IEC 61850.

5.11 Logical nodes for protection functions LN Group: P

5.11.1 Modelling remarks

This subclause refers to modelling of protection and protection related logical nodes and shows

the relation (see Table 7) between IEC 61850-5 and the logical node class definitions

according to this standard.

«If there are several stages to one function (i.e. for multi-zone relay), each stage shall be a
separate instance of the LN. Examples are PDIS (n zones) or PTOV (2 stages).

* Multiple instances shall be used if LNs of the same LN class are operating with different
settings in parallel.

© If different measuring principles such as phase or ground are required, each shall be
represented by an instance of the same basic function. An example is PTOC (used for
phase or ground in dedicated instances).

© The logical nodes are defined in IEC 61850-5 from protection requirements (see Table 7),
however, for modelling purposes, some logical nodes have been split (see Table 7).

* Logical nodes from IEC 61850-5 are modelled using combinations of the LNs defined in this
standard (see Table 6).

* Other logical nodes have been added to model complex protection devices and schemes
(see the following subclauses). As an example, line protection uses LN PSCH to combine
the outputs from multiple protection LNs.

* The protection functions provide (if applicable) the data object Str (Start) with direction
information. In the case of a protection function which provides no direction information, the
direction “unknown” shall be transmitted. The data object Str is summarised by LN PTRC.

« If the fault direction is provided in Str (Start), the directional protection may be modelled
without the directional element LN RDIR. If any of the settings provided by LN RDIR are
needed, the LN RDIR shall be used.

* The protection functions provide (if applicable) the data object Op (Operate) without
direction information. The data object Op is conditioned by LN PTRC resulting in the data
object Tr (Real Trip), that is between every protection LN and the circuit breaker node
XCBR shall be a LN PTRC.

Table 7 — Relation between IEC 61850-5 and IEC 61850-7-4 (this standard)
for protection LNs
Defined in |  Modelled in

|__Pretwnany |" | as | ears | commen

To build line protection

schemes

Directional over power
ettied pan POOP Directional under power

power PoPR o Reverse power modelled by

PDuP POOP plus directional mode
“reverse”
i ’ . PTUC Undercurrent
—— PouP Underpower
a
a
nw
https://www.doc88.com/p-80980482981320.html 62/185
```


## File page 063

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 718 > @ Q_ View A mark Y Annotations v | Q
61850-7-4 © IEC:2010(E) -61-
Modeled in
IEC 61850-7-4
Time overcurrent (PTOC) with
three-phase information with
ruveree poase orjpness PPBR PToc sequence current as an input
balance current or even ratio of negative and
Positive sequence currents
eee CN
[Reoriormalovevoas [46h [POL [PTT | Thwmal oweroas |
[stor mermaloveioad [485 [Pca [prrR __|Themalovevoas |
POPF Over power factor
PUPF Under power factor
[Bc-vwvote _[=806 | Poov [prov ___|SemtrDcamaac |
pTov Overvoltage or overcurrent
Voltage or current balance PTO regarding the magnitude of the
difference
PTOC Time overcurrent
PHIZ
PTO Time overcurrent
PHIZ
[meron tat —————~ideaw [rite [proc [Tine ovrsanent |
[AC decal overcurent [er [Pood [Proc ___[Time oveeurent |
[oveconsleamniaut [erm [Poer [toc [Tine ovcurent |
femwemeee ere ieee
PTOF Over frequency
PTUF Under frequency
PFRC Rate of change of frequency
PSCH is used for line
Carrier or pilot wire protection protection schemes instead of
RCPW
[Pras conpareon [ere _[erorfeor |
[Diterentat ting fetter for |
PBOF PDIF or Busbar differential or
POIR fault direction comparison
[corer steeniai [ara [Poor [por [SS
49h, 66 Pera Motor restart inhibition
48, 51LR PMss Motor starting time supervision
Field short-circuit protection
Rotor protection 64/59AC | PROT using the 6” harmonic
(300 He).
5.11.2 LN: Differential Name: PDIF
See IEC 61850-5 (LNs PLOF, PNDF, PTDF, PBDF, PMDF, and PPDF). This LN shall be used
for all kinds of current differential protection. Proper current samples for the dedicated
application shall be subscribed.
a
an
https:/www.doc88.com/p-80980482981320.htm! 63/185
```


## File page 064

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)

-62- 61850-7-4 © IEC:2010(E)

LNName The name shall be composed of the class name, the LN-Prefix and LN-

Instance-ID according to IEC 61850-7-2, Clause 22,

Data objects

[Measured and metered values

Jowacie wre oiterentiatcurent J

jpn |wve  Restraitcurene fo

loscntis [6 [Resetabe opoaioncaunr———SSSS~S~S~«w

[Settings

luncapac [ASO [ve canacince (ormadeumeniy SSS

[loser ps0 ow owe vate porcenage fe rarinalconens | |e _|

[nse [ASG pura ve parcanage oft nominalcurent | [o_

[Minoptmms [iG [Minimum operatetime if

—— or

Frmacwaa [6S utin cuve characoisic anion —————SCS~wrCi

5.11.3. LN: Direction comparison Name: PDIR

For a description of this LN, see IEC 61850-5. The operate decision is based on an agreement

of the fault direction signals from all directional fault sensors (for example directional relays)

surrounding the fault. The directional comparison for lines is made with PSCH.

Data object
fame
LNName [The name shall be composed of the class name, the LN-Prefix and LN-
instance-ID according to IEC 61850-7-2, Clause 22.

Dataobjects

fra pearance of te tesrotea aut arose) |_|

lop [scr operate secsion ram al senor atte sounded objet ued) [|

loncnie [6 [Resetabe opoaioncaunr SSS

Isetngs

a
an
https://www.doc88.com/p-80980482981320.html 64/185
```


## File page 065

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 718 > @ Q_ View A mark Y Annotations v | Q
61850-7-4 © IEC:2010(E) -63-
5.11.4 LN: Distance Name: PDIS
For a description of this LN, see IEC 61850-5. The phase start value and ground start value are
minimum thresholds to release the impedance measurements depending on the distance
function characteristic given by the algorithm and defined by the settings. The settings replace
the data object curve as used for the characteristic on some other protection LNs. One
instance of PDIS per zone shall be used.
Data obj
Oram doco) nm
| ener [| |
Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
jow___— fact foverate
[eomrots
[opens 6 [Resoabe opomioncaunr SSS
(Settings
Pofen ASG [Poarreachis te damote ole ro degra ———~—SC=*diYO_
[onios [eno _Jovestonsimoge SSS
[Ratce [ns [Renstvoeachioroadwea——SSSCSCSCS~S~S~S
Inted nse _[amietorioadarea—SSSSCSCSCS~S~S~wY
Frmomed 5° __[operatvtme doy mage SSCS
loots wo fopomietmegeay SSS
Jrromos _|s@opoate une coy nipnaso mage ———SSCSCS~dCO
Jrornms No [Operat te ny for muinase wuts
[ensomog [sR lOperae ine day forse pase ourdnoge———SSS~*
naortnms wo opr ne doy er sg pase rundtaste | |_|
Kr [asoostve seavnce ine ssn scarce —————SSSS—~«d |
lunar —iASG—itmeamgeSSSC~d
[Recnahch [ASG [Rentive pomarexeh———SSSSCSCS
Ioract [ASG [Renu compensation acorxe SS
IoFaciing [nS [Reiual compensation corange SSO
5.11.5 LN: Directional overpower Name: PDOP
For a description of this LN, see IEC 61850-5 (LN PDPR). This LN shall be used for the
overpower part of PDPR. Additionally, PDOP is used to model a reverse overpower function
(IEEE device function number 32R, from IEEE C37.2:1996) when the DirMod is set to reverse.
a
an
https://www.doc88.com/p-80980482981320.html 65/185
```


## File page 066

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
-64- 61850-7-4 © IEC:2010(E)
a _| Eee = [|
Instance-ID according to IEC 61850-7-2, Clause 22,
Data objects
[Comtrots
loncans [NC [Resenanie opeaioncaumer ————SSSCSC~S~wdC
Settings
[bites [ERG [Bvocionsimese SSCS
5.11.6 LN: Directional underpower Name: PDUP
For a description of this LN, see IEC 61850-5 (LN PDPR). This LN shall be used for the
underpower part of PDPR.
=| aaa |
Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
[Comtrote
[opcnits NC [Resonabie opernioncoumer———SSSSCS~S~«~dCiO
(Settings
lores [enc _[oreetonatmode———SSCS~S
5.11.7 LN: Rate of change of frequency Name: PFRC
For a description of this LN, see IEC 61850-5 (LN PFRQ). This LN shall be used to model the
rate of frequency change of PFRQ. One instance shall be used per stage.
Data obj
_Smame docs) ee
LNName [The name shall be composed of the class name, the LN-Prefix and LN-
Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
janv _[5°5__[octeabecauseotvnnoe ———SSSSSCSCS~S~wCiO
°
6
Ly
8
an
https:/www.doc88.com/p-80980482981320.htm! 66/185
```


## File page 067

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
61850-7-4 © IEC:2010(E) -65-
[Contras
loscune [No [Restate opratonamer SSS
[Settings
nn
a Sc
5.11.8 LN: Harmonic restraint Name: PHAR
This LN shall be used to represent the harmonic restraint data object of the transformer
differential protection (see PDIF) in a dedicated node. There may be multiple instantiations of
this LN with different settings, especially with different data object HaRst.
— | imeem [| |
Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
[su __(ACO [San nate wenrestaininneoses) ————SSSSSS~wdri
[Comrie
loscuns [NC [Resstabieopoaioncoumer———SSSSCSCS~i CS
Settings
ifs [NG Nomberoftarmonerestaned SSS
josie [asa [sopvane SSS
5.11.9 LN: Ground detector Name: PHIZ
For a description of this LN, see IEC 61850-5. This LN shall be used for high-impedance
isolation faults only.
p= | aeeenereinme |
Instance-ID according to IEC 61850-7-2, Clause 22,
Data objects
|eomrote
[opens NO [Resoabe opoionsaonr SSS
(Settings
ivr [AS [Twanamoncvotapesuvane «dO
an
https:/www.doc88.com/p-80980482981320.htm! 67/185
```


## File page 068

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)

-66- 61850-7-4 © IEC:2010(E)

5.11.10 LN: Instantaneous overcurrent Name: PIOC

For a description of this LN, see IEC 61850-5. This LN shall be used for instantaneous

overcurrent protection only.

= _| sane = |

Instance-ID according to IEC 61850-7-2, Clause 22.

Data objects

[Controls

foncnrns inc ___[Reserabie operation counter

(Settings

5.11.11 LN: Motor restart inhibition Name: PMRI

For a description of this LN, see IEC 61850-5 (LN PMSU). This LN shall be used to model in a

dedicated LN the part from LN PMSU which protects a motor against thermal overload during

start-up.

—_| aan = |

Instance-ID according to IEC 61850-7-2, Clause 22.

[Data objects

[Status Information

[ston [ses |Restartinnibted

[suinntimm ins [Restart innbion ime

[Controte

[opcnits [WC [Resenabie opersioncomer———SS~S~S«~zCS

Settings

[ea ASG [Curent setingormotorsaup ——SSSCS~S~S~S~rCi~SC*

[stm [NG [Time setngormawrstrtwp ———S~Sw

[Mazon NG Maximum number of stro orcosansy —————SS—*d SO

imawmse [NG Maunum warm tars permisable numberof warmatra =O

IMexsiTmm ING [Tine perotorie maxinom numberotstans ———SSSSS* |

fcxtmm [NG _[Temperive onateaiontime ———=SSSSSSCSCSC~«wriO

a
an
https:/www.doc88.com/p-80980482981320.htm! 68/185
```


## File page 069

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 718 > @ Q_ View A mark Y Annotations v | Q
61850-7-4 © IEC:2010(E) -67-
5.11.12 LN: Motor starting time supervision Name: PMSS
For a description of this LN, see IEC 61850-5 (LN PMSU). This LN shall be used to model from
LN PMSU the part which protects a motor against excessive starting time/locked rotor during
start-up in a dedicated LN.
Data ob|
Pome’ (ance) mmm
LNName ‘The name shall be composed of the class name, the LN-Prefix and LN-
Instance-ID according to IEC 61850-7-2, Clause 22.
[Data objects
{a CS
[eomrots
loscus [Wo [Restate opraton comer SSS
Settings
[sk (ASG [ooren stig ormotrsamae ———SSCS~dOO
[urns [none sourptarnotorsatye —SSSS~w
Imose AS Maor sap cent pup vue at motor staring) —————S—*d IO
\omnartma ING took otortine,perisiveackesroiortime ———SSSC*dO
5.11.13 LN: Over power factor Name: POPF
For a description of this LN, see IEC 61850-5 (LN PPFR). This LN shall be used for the over
power factor part of PPFR.
=| Sai |
Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
jow act foperate
Jaxx br ockes ow minimum opeaina ore ————SSSSS—=d |
janv [bs [locked bet minimum operating vonage ———=SCS~«~didN_—
[Comtrots
locus [wo Roetabioopraton comer SSCS
Settings
lopormns no _fopeaieaayime SSCS
Janvar [AS [ook vane niimom opeaiog caren) ————SSSS~wd
lanvaw [ps0 ook vate rinimum operating vonage) | o_]
5.11.14 LN: Phase angle measuring Name: PPAM
For a description of this LN, see IEC 61850-5. This function shall be used to model “out-of-
step” protection of generators.
a
an
https://www.doc88.com/p-80980482981320.html 69/185
```


## File page 070

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
-68- 61850-7-4 © IEC:2010(E)

= _ Eee |
Instance-ID according to IEC 61850-7-2, Clause 22.

Data objects

[Comtrots

loncans [NC [Resenanie opeaioncaumer ————SSSCSC~S~wdC

Settings

[sever asc [stave fe

5.11.15 LN: Rotor protection Name: PRTR

For a description of this LN, see IEC 61850-5. Logical Node PRTR shall be used to represent a

field short-circuit protection using the 6" harmonic (300 Hz). The protection is normally

included in the excitation system.

| Eevenersire == [| |
Instance-ID according to IEC 61850-7-2, Clause 22.

Data objects

lo Act _[Opemte ps bah tals andgeremiorce) Ci

[Controts

[opcns [NG [Reetabieopuratoncowmer————SSSSCSCSC*~S~SS

Settings

[seve asc [stave

5.11.16 LN: Protection scheme Name: PSCH

This LN shall be used to model the logic scheme for line protection function co-ordination. The

protection scheme allows the exchange of the “operate” outputs of different protection

functions and conditions for line protection schemes. It includes data objects for teleprotection
if applicable. In this case, all appropriate data objects shall be subscribed.

—_| Ear |
Instance-ID according to 1EC 61850-7-2, Clause 22.

Data objects

SF
permissive signal)

aS =
iblocking signal)

Tir AGT [bet rp intrmation tobe vanamitea tome onerade ————*iO_|
Activation intormation RxPrm1 received from the other side(s), for logging |
purposes (teleprotection permissive signal received)

a
an
https:/www.doc88.com/p-80980482981320.htm! 70/185
```


## File page 071

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
61850-7-4 © IEC:2010(E) -69-
oe cd
name data class
=f Sea
ipurposes (teleprotection blocking signal received)
a a information RxTr1 received from the other side(s), for logging He |
‘purposes (direct trip signal received)
foo act operate
[Eenowor [SPS [a Pris boing sara cho sna arn case of weak end nied [0 _|
‘Additional indication that Op is the operate from the weak end infeed or _|T|
lecho function (typically with undervoltage control)
locus [NG_Resstabio opeationcooner———SSSSCS~S~S~«wd
Settings
greener [P|
type ACT
IRxSrcTr1___—— [ORG _—_[ Source for activation information RxTr, must refer to data of type ACT | [0 _ |
[crstnms no _[oo-rtnaton imerierbocingsaene———S~
[owrtmms _|[ING_Mnimum uration of TaPermin case of operate of PEGH ‘| JO _|
[unaiatog [ENG _|Unbuckwrcton mage orsceme ype «|
[urautnms |G lurecnroine ——SSSSS~diO
[weitog [ENG oe of weak erdinieaiuncton ————SSSCSCS~S~iO
[Weirnms wo _[o-onaton ine er weak ondieed sion | Jo |
5.11.17 LN: Sensitive directional earthfault Name: PSDE
For a general description of directed earth fault protection, see IEC 61850-5. This LN is used
for directional earthfault handling in compensated and isolated networks. The use of “operate”
is optional and depends both on protection philosophy and on instrument transformer capabi-
lities. For compensated networks, this function is often called wattmetric directional earthfault.
The very high accuracy needed for fault current measurement in compensated networks may
require phase angle compensation. This shall be realised by the related LN TCTR.
Same dmc) nn
ame
— | Sener [| |
Instance-ID according to IEC 61850-7-2, Clause 22,
Data objects
[opens No [Resoabe opoaioncauns SSS
runes [SPC __[earvautindcton.rsetabio «dO
[Settings
ina [ASG [re boteon vgs (and eae) [e
lonssr (ASG _forounsatanvae uy SSSSCS~S~S~S~«~C_
[onsop [AS oround operat vale re
Sc
NC
[ormoa ENG go [Ovectonaimage SSCS
an
https:/www.doc88.com/p-80980482981320.htm! TAN8S
```


## File page 072

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
-70- 61850-7-4 © IEC:2010(E)
5.11.18 LN: Transient earth fault Name: PTEF
For a description of this LN, see IEC 61850-5. This LN shall be used to detect ("start") transient
earth fault in compensated networks.
Data obj
Sram doco) nn
o—__|_ Eevaneraisrar == [| |
Instance-ID according to IEC 61850-7-2, Clause 22,
Data objects
cn
jor ___ fact ___foverateransienteantniouy te
|eomrote
[opens [Restate opoaioncaunr SSS
Frurars |e __[eatniautindeatonesetabo———SSCSCS~dC CS
Settings
[endsr_fas@oroundstatvaue Cf
lontos [eno _Jovestonsimoge SSS
5.11.19 LN: Thyristor protection Name: PTHF
Logical node PTHF shall be used to represent a thyristor (thyristor valve) protection. In a power
plant, this protection will typically be included in the excitation system.
== _| Eames = [|
Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
lo» Act operate ge bh fat GB and geroorow)——SSSSC=*iI
[comers
loncune [Wo [Rests opraton caer SSS
(Settings
2 A
5.11.20 LN: Time overcurrent Name: PTOC
For a description of this LN, see IEC 61850-5 (LN PTOC). This LN shall also be used to model
the directional time overcurrent (PDOC/IEEE device function number 67, from
IEEE C37.2:1996). The Definite Time overcurrent (also PTOC/IEEE device function number 51,
from IEEE C37.2:1996) shall be modelled by use of PTOC and selecting the related curve.
Data obj
a
=| aaa |
Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
an
https://www.doc88.com/p-80980482981320.html 72/185
```


## File page 073

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
61850-7-4 © IEC:2010(E) -1e-
[Controte
[once [NG [Reetabe opeioncaomer SSS
Settings
mR
fimacnas [oS ine cove earacoisicaninwon ————SSSCSCS~«wdC CS
frmast_ [esp |actwecurvecharactenate
foe [asc [otarvawe fo
fmm [aso _[tmeaaimunpir———SSSCSCSC~S~S~S~S
IMnoptams [NG Mninumopeaeume———SSSSCSC~S~S~«w
[Maxoptmms [ING [Maximum operaietime
Jonortmms _[iNG_—[Operatedelaytme ft
[TyeRscry en [Type otresetcuve
[Rsprtmms NG ___|Resetdotaytime
[bes [eno _[Dvectonsimose SSCS
5.11.21 LN: Overfrequency Name: PTOF
For a description of this LN, see IEC 61850-5 (LN PFRQ). This LN shall be used to model the
overfrequency part of PFRQ. One instance shall be used per stage.
—_ Ears |
linstance-ID according to IEC 61850-7-2, Clause 22.

Data objects
fe aco Sturt
janv _[se5_[octesecauseotvnnge SSS
[Controls
loncans [NG [Resenabie opeatoncoumer————SSSSSCSCS~S~dCi
(Settings
nS
janvar [ase Natagenockvaue ——SSSCS~Sw™
[oportmms ING [Operate delaytime
[Rsprtmms inc |Resetdetaytime

ry

Ly

8

an

https:/www.doc88.com/p-80980482981320.htm! 73/185
```


## File page 074

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)

-T2- 61850-7-4 © IEC:2010(E)

5.11.22 LN: Overvoltage Name: PTOV

For a description of this LN, see IEC 61850-5. For some applications such as transformer star-

point or delta supervision, “operate” may not be used.

Same doco) nm

name
— | Eeiaarciamer =" [|
Instance-ID according to IEC 61850-7-2, Clause 22.

Data objects

joe act operate

loncans [NG __[esetabiopeatoncoumer——SSSCS~S~dCiCCS

Settings

Hinvowss_[esG sine cave cacoisicaainton «dC

[rmvst_ [eso Active curve characteritie =f

fimwst (ASG —_[timedalmatpier ——SSSSCSC~S~S~

[Minoptmms [ING [Minimum operatetime Tf

5.11.23 LN: Protection trip conditioning Name: PTRC

This LN shall be used to connect the “operate” outputs of one or more protection functions to a

common “trip” to be transmitted to XCBR. In addition, or alternatively, any combination of

“operate” outputs of the protection functions may be combined to a new “operate” of PTRC.

Same docs) ee

name
— | amare" [|
Instance-ID according to IEC 61850-7-2, Clause 22.

Data objects

fot OOCOSCSOCC*dCzC=+Y

lon [act opera cotinaton of steed Op rom protection neions) | [|

Isr [Aco [Stat combination subsebed Si tom ptaton tuners) | [O

[opcns [NG [Resetabieopuratoncomer———SSSSSSCSCS~S~S~dCiC=CS

Settings

[weutnns [NG [tippusetine SSCS

a
an
https:/www.doc88.com/p-80980482981320.htm! TAN8S
```


## File page 075

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 718 > @ Q_ View A mark Y Annotations v | Q
61850-7-4 © IEC:2010(E) -73-
5.11.24 LN: Thermal overload Name: PTTR
For a description of this LN, see IEC 61850-5 (LNs PROL, PSOL). PTTR shall be used for all
thermal overload functions. Depending on the algorithm, the LN describes either a temperature
or a current (thermal model). Temperature data objects are also provided by other LNs.
Examples are the hot spot temperature in LN YPTR or the isolation gas temperature in LN
SIMG.
Data obj
Oram doco) nm
| ener [| |
Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
jor fact foperate
lamtim se [theater SSCS
laxrim [55 [ook coring command ior cre aber esas a heal anion | [O |
[Measured and metered values
RS
[nev ‘Temper ortemaiis —SS~*d
finpmi WW Retaton beeen enporare and masmum temperate | o_|
lockevre hv [ondrerevetotip SSS J
[Comrie
lopone 6 [Restabe oposioneaomr SSS
Settings
fintnpcw CURVE [onaacieraiccuve ortenpuratemeauenen _——_- > _|
fntnpcnas|cs0__ainecone chaacterateasinton ——————SSSSS—~*d
[imac [CURVE [onaactersti curve or curent measurement Themal mods’ | [O_|
Fimacnas [os ine cave careers aainon ———SSS«d
[tmtmest__foso__acwwecurvecharacterste fo
Finpuas [AS Maximum alowedtenparture SSS
joporimme [wa ___fOrerateceaytime tf
ct
[onstnsi [NG [Tine contanot ie ermaimoa’ —————SSSSSSCSC~«w
[repouvar [as [opt va fr Basing cesngcnmana———SSSS—~«id
NOTE TmAChr33 refers to the attribute TmACrv.setCharact = 33 etc. TmTmpChr33 refers to the attribute
TmTmpCrv.setCharact = 33 etc.
5.11.25 LN: Undercurrent Name: PTUC
For a description of this LN, see IEC 61850-5 (LN PUCP). This LN shall be used for the
undercurrent part of PUCP. The underpower part of LN PUCP is covered by PDUP. Different
instances shall be used for phase and ground.
a
an
https://www.doc88.com/p-80980482981320.html 75/185
```


## File page 076

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
-74- 61850-7-4 © IEC:2010(E)
= _ Eee |
Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
fe aco start
[Comtrots
loncans [NC [Resenanie opeaioncaumer ————SSSCSC~S~wdC
Settings
[rmacnras[cs¢ _—_[Muttine curve characterise detintion |
[mast feso___Active curve characterise =f
jstvar_ asc |statvwe
lopormms [ING [Operate deiaytime
frmmun [ASG [Time dias munipter
[Minoptmms [iNG [Minimum operatetime Tf
[Maxoptmms ING [Maximum operatetime
[TyeRscry enc [ype otreseteuve
[Rsprtmms [ina [Resetdetaytime
[Dirog ENG [Oirectionarmode
5.11.26 LN: Underfrequency Name: PTUF
For a description of this LN, see IEC 61850-5 (LN PFRQ). This LN shall be used to model the
underfrequency part of PFRQ. One instance shall be used per stage.
=| Enea |
Instance-ID according to IEC 61850-7-2, Clause 22.

Data objects
fe fac ftw
jaxv ses [atocked because ot vonage Cf
[Controls
fopcnrns INC ___[Resettabie operation counter fo
[Settings
[svar fasG [start vane requencyy
Jauvar fas Voltage block vawe Cf
[Rsortmms [in [Reset detaytime
5.11.27 LN: Undervoltage Name: PTUV
For a description of this LN, see IEC 61850-5. With an appropriate low operating curve, PTUV
works also as zero voltage relay.

a

Ly

8

an

https:/www.doc88.com/p-80980482981320.htm! 76/185
```


## File page 077

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
61850-7-4 © IEC:2010(E) -75-
=| Sima |
Instance-ID according to IEC 61850-7-2, Clause 22,
Data objects
fe aco start
|eomrots
Joncas [Wo [Resto opaton comer SSS
Settings
Frnvonas _|ese __Mutine cone chaaciosieawinzon———SS~S~S~S~«~ =
[rmvst__feso___Active curve characterise ft
jew fas@stanvewe fo
frowt aso [rime atmo SSS
[Minoptmms [iG [Minimum operatetime tf
[MaxOptmms [iG [Maximum operatetime if
[Reortmms __[wa___[Resetgeayme fo
5.11.28 LN: Underpower factor Name: PUPF
For a description of this LN, see IEC 61850-5 (LN PPFR). This LN shall be used for the
underpower factor part of PPFR.
_Smame domes) em
ame
— | iinereiree = [| |
Instance-ID according to IEC 61850-7-2, Clause 22.

Data objects
jee aco ott
jow_ fact foperate
jonk 5s locked eto minimum operating caren ——=SCSCS~«~didN—=*
lanv [5S [blokes bet minimum operating vonage ————SSSCS~«~dCO_
[comnts
loscuns [Wo [Restate opraton comer SSS
Settings
fswvar_ asa sunvwe Cf
[opornms [NG __fopeawanaytne ———SSSSCSCS~S
[Rsortmms _inG__[Resetdoiaytime
[onvaia [ASG [ea vane ninmum opening cure) ——SSSSSCSC~*dtCSO
lanvaw p86 ook vate riimum operating votape) —————~d
5.11.29 LN: Voltage controlled time overcurrent Name: PVOC
For a description of this LN, see IEC 61850-5.

a

Ly

8

an

https://www.doc88.com/p-80980482981320.html 77/185,
```


## File page 078

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
-76- 61850-7-4 © IEC:2010(E)
cc
name data class
—_| Se |
Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
(Status information
fse ac fstart
Joo act operate
(Controls
loncas [NC [Resstabeopoatoncoumer SSCS
Settings
lave ___[euRVE [Operating curve type (tor vottage controled curent curve) | [O_|
lavenaa [csc [Muttine curve characteristic detintion |
[rmacry __[cuRve Operating curve type tor eurent) JO
[rmacnras [csc ___[utiine curve characteristic definition
favs |oso___[Active curve characteriste fo
frmast_ oso [active curve characterste JO
fFmmut ASG time iat muttiper
[Minoptmms ING [Minimum operatotime
[Maxoptmms ING [Maximum operatetime fo
fopormms Inc ___[Operate detaytine Cf
[Twrscv ENG Typectreseteuwe
\Reprtmms __|ING_[Resetdelaywme
NOTE AVChr33 refers to the attribute AVCrv.setCharact = 33 etc. TmAChr33 refers to the attribute
5.11.30 LN: Volts per Hz Name: PVPH
For a description of this LN, see IEC 61850-5. One instance of PVPH shall be used per
protection stage.
—_ Ears
linstance-ID according to IEC 61850-7-2, Clause 22.
[Data objects
fe aco stm
[Controls
lopcans [NG [Reetaniopeaioncoumer———SSSSCSCS~S~S
Settings
|vrizcry [curve [operating cuverype
[vrizcnras [csc [Muti curve charactedsticdefinon
lveest_ oso [acto curvecharacterstio
[svat [asc vous perhertzstartvawe
[Twprscv [ENG [Type otresetcuve if
[RsorTmms [ING __|Resetdoiaytime
ry
Ly
8
an
https://www.doc88.com/p-80980482981320.html 78/185
```


## File page 079

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
61850-7-4 © IEC:2010(E) -TT-
Fk [imedatmitpie ——SSSCSCSC~S~S
|Minoptmms inc [Minimum operatetime if
5.11.31 LN: Zero speed or underspeed Name: PZSU
For a description of this LN, see IEC 61850-5.
Data obj
a
e _| eae |
Instance-ID according to IEC 61850-7-2, Clause 22.
[Data objects
joo act operate
[eomrots
Joncas [Wo [Roetanio operation comer SSS
Settings
eval [ASG [Start va pes [fo
5.12 Logical nodes for power quality events LN Group: Q
5.12.1 Modelling remarks
This group of logical nodes refers to the modelling of power quality events detection and
analysis functions. The models are based on the principles used for modelling protection
functions.
There is a one-to-one relationship between the power quality event logical nodes in
IEC 61850-5 and the logical node class definitions in this standard.
5.12.2 LN: Frequency variation Name: QFVR:
For a description of this LN, see IEC 61850-5.
Common
|_enetname (doimcoss, mem
| _Eaveneneinmas =e [|
Instance-ID according to 1EC 61850-7-2, Clause 22.
[Data objects
[Status information
Wars (SPS ‘(San of he equeneyvaraton event ———SSSS~*~S~S~riz CS
juss [ss [sat unertoquney variation ventnprprss) | fo _|
lowes [ses [su ovrtoqueneyvaratonventinpepes) | fo _|
[ens [ss [eve tnateaoutrotvest «SO
a
an
https://www.doc88.com/p-80980482981320.html 79/185
```


## File page 080

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
-78- 61850-7-4 © IEC:2010(E)
‘Common
data class
[Measured and metered values
[vatm wv roger eran raion ote ias completed went | fo _|
Inevanag wv |Fequoney variation magnitude ofthe iast completed vont | [0 _|
enc [HST _[evert counter istonam (evan. Havanas) ——————*d (|
[Comtrots
fopcnins inc __—[Resetabie counter operation
[Settings
[untssuvar [ASG [Unaarvequeneysetpent———SSSSCS~S~S
loweswar [aso _[ovotequney tga ————SSSSSS~S
5.12.3 LN: Current transient Name: QITR
For a description of this LN, see IEC 61850-5.
=| arse |
Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
Sc CS
Imaats fw Mannan caret varson'vave SSO
[enc [HST leven eouterntogam esta MaxaT@) ———SSSSSS~*d
[Controls
fopcere [inc ____[Resetatie couner operation Jo
Settings
5.12.4 LN: Current unbalance variation Name: QIUB
For a description of this LN, see IEC 61850-5.
-— | Eavunenmiames === [|
Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
[Status Information
jvesy ss stancttneevet
[Measured and metered values
Javatm [av [eurrent unbalance variaton duration J
[Mexava [uv __[aaximum unbatance devietinvawe Jo
[encnr [ust_, [Event courternistcgram fo
an
https://www.doc88.com/p-80980482981320.html 80/185
```


## File page 081

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
61850-7-4 © IEC:2010(E) -79-
scr we [Roetabio conor open —————SSSS~S~w
Settings
[vou ENG [Urbane etucionmetos———SSSSSS~wi
[svar fasG—[eurrent unbatance statvawe
5.12.5 LN: Voltage transient Name: QVTR
For a description of this LN, see IEC 61850-5.
| haar == |
Instance-ID according to IEC 61850-7-2, Clause 22,
[Data objects
jvsr__ srs stanottmeevert
lvaena_|ss_[Eventtinuneatutnotreset_ fo
\Measured and metered values
[stm fw ‘otagevansenta@iaion SSCS
levi Mosman vatage varsentvane ————SSSSCSCS~di
nn a
[omtrots
locas no [Resetaniecuneropeion SSS
Settings
[sve ase Nonage wansiontstatvawe
5.12.6 LN: Voltage unbalance variation Name: QVUB
For a description of this LN, see IEC 61850-5.
=| Seer |
Instance-ID according to IEC 61850-7-2, Clause 22,

Data objects
esr SPS iSanetnwown ——SSSCSC*~“~*~*~‘“‘~*~*~*~*~iC*d
[vaéna_[e6_[Everttrictestutrtrert tf
[Measured and metered values
lwatm —__|iw___‘(Valago unbalance varaton airaion ———SCS~S~S~wdi
jmawia sv [Mesum nbaneecovatonvaue «dO
[eon st [own counernatovam ————SSSS~w
[Controts
a
Settings
[noon eNotes ducionmeos————SSSSSS~di
Java [as otageuatnee tt vane———SSSSS~d i

a

Ly

8

an

https://www.doc88.com/p-80980482981320.html 81/185
```


## File page 082

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q

-80- 61850-7-4 © IEC:2010(E)
5.12.7 LN: Voltage variation Name: QVVR
For a description of this LN, see IEC 61850-5. This LN refers to one phase only.
Rtas
LNName ‘The name shall be composed of the class name, the LN-Prefix and LN-

Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
[ars (SPS [San votaneveraion wwrtinprovessy ————SSSS~*wdi
loose |s*5 _[sunonsse dp eveninproess) —————SSSSSCS~d
[use [55 [sun vonage swotvontinprowes) ———SSSSSS—~*d
[arse [sr [sa vonage ruption eventinpooess) ——————SSS—*d |
[wens |5P5 [event inanesnutrotet SS SSSSC=* IO
[wa _‘otagevaraion magne of eiest conoid went ————*( |
enon ST [evetcounernisogam ——SS—~*d
[rata jw Wotage variton dation attest conpitedeent |_|
[Contras
locas [WC [Resstabiecuneropeion ————SSSSSCSC~dO
Settings
loeswa as otapespartpont SSCS
[owsrvar [ASG otage evel eipont SSS
Imcwva (ASG _‘otape mirupion seat «LO
[mown [eNO |rvrapionenectn meting ———SS*d
5.13 Logical nodes for protection related functions LN Group: R
5.13.1 Modelling remarks
Table 8 gives the relation between IEC 61850-5 and this standard for protection related LNs.
Table 8 — Relation between IEC 61850-5 and IEC 61850-7-4
for protection related LN
Defined in | Modelied in
Functionality NEC 61850-5 | IEC 61850-7-4
by LN by LN
Carrier o pilot line wire PSCH is used for line protection
Protection schemes instead of RCPW
Directional element tor modelling
directed protection with Pxyz nodes
RDRE Basic functionality
Parone recording RADR Analogue channel
RBDR Binary channel
a
nw
https://www.doc88.com/p-80980482981320.html 82/185
```


## File page 083

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
61850-7-4 © IEC:2010(E) -81-
5.13.2 LN: Disturbance recorder channel analogue Name: RADR
In addition to the channel number, all attributes needed for the COMTRADE file
(IEEE C37.111:1999) are provided either by data objects from the TVTR or TCTR or by
attributes of the measured value (samples subscribed from TVTR or TCTR) itself or by data
objects from pseudo channels (calculated values, derived values of power quality devices). The
“circuit component” and “phase identification” is provided by the instance identification of the
LN RADR. Channels “1” to “n” are created by “1” to “n” instances.
ADR tas
= eee |
Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
[Status information
font _[sps__[tnannetwiggered
\Measured and metered values
Sn | dP
COMTRADE only
[Controts
loscnie [NO _[Revetabe oposioncaunr———SSSSS~S~S~S~«w
[Settings
Frowos [eno ruer move ral ger exeralorbony | o_|
[ews [eno fveonermese SS SSSC~*d YO
Fatale [AS [Nah poate gore SSCS
a
[Pretmms fina___[Prewriggertime fo
Penns No ostvapertme SSS
a
Condition C: multiple instances of ChNum are only allowed in case of compound data types (e.g. WYE). The order
lof these shall be the same as in the referenced data object.
5.13.3 LN: Disturbance recorder channel binary Name: RBDR
In addition to the channel number, all attributes needed for the COMTRADE file
(IEEE C37.111:1999) are provided by attributes of the binary input (subscribed from another
LN). The “circuit component" and “phase identification” is provided by the instance
identification of the LN RBDR. Channels “1” to “n” are created by “1” to “n" instances.
PBR ass
a
c
LNName The name shall be composed of the class name, the LN-Prefix and LN-
Instance-ID according to IEC 61850-7-2, Clause 22.
[Data objects
|Status information
lento ss enannetwiggered
Bite omy | Prmmnemme
COMTRADE only
[Controts
loscus [Wo [Rests opraton comer SSS
(Setings
Frome [ENG [Trauermose(nenalviane:exenaleroan) =O
nw
https://www.doc88.com/p-80980482981320.html 83/185
```


## File page 084

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)

-62- 61850-7-4 © IEC:2010(E)

ltevwod ena levettiggermege fo

[Pretmms inc [Pre-tiggertime

[Pstrmms fina ___Postwrggertume Cf

[erat ORG [Rorenceo ne EC etososoucowsiacver ‘| fo_|

indication). The order of these shall be the same as in the referenced data object.

5.13.4 LN: Breaker failure Name: RBRF

For a description of this LN, see IEC 61850-5.

— | Saar [| |

Instance-ID according to IEC 61850-7-2, Clause 22,

Data objects

a A

lope pct frat atve vip Coneraivy —————SSSSS~*

loom [AcT operate, etna we hie

[Comtrots

locus [No [Restate opratonaumer SSS

(Settings

Faas ENG [rete aive aetcionmage SSS

Fans NG _rehar flure ime iy ores wip ———SSSSS~«d

[serrtmms [NG _[Snalepoereniptme aay SSSS~w

ferermms [no [Tee pole ouptme soy SSO

IRetos eno [Revipmove SSCS

[Condition C: At least one of either data objects shall be used depending on the applied tripping schema.

5.13.5 LN: Directional element Name: RDIR

This LN shall be used to represent all directional data objects in a dedicated LN used for

directional relay settings. The protection function itself is modelled by the dedicated protection

LN. LN RDIR may be used with functions 21, 32 or 67 according to IEEE device function

number designation.

— | ere |

Instance-ID according to 1EC 61850-7-2, Clause 22.

[Data objects

|Status information

Settings

InFwng [AS6 Mn phase anleiniorwrsarecion ———SS~«wdd

a
an
https://www.doc88.com/p-80980482981320.html 84/185
```


## File page 085

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
61850-7-4 © IEC:2010(E) —83-
Data obj
Same dmc) menn
lnaviog [ASG inom pase angle neve arecion ———S~«~di
IMextwanog [ASG |Misimum phase angle intowargareston———SSS~«~dS
Imasvang [ASG Maximum phase ange nrevere recon ——————S~wd
a
lanvav [A50_ininom oposite ———SSSSCS~S~w
ray [eno rowrang quntty SSS
IMnpry [ASG Mnphase narevotase ——SSSSCS*~S«~di
5.13.6 LN: Disturbance recorder function Name: RDRE
For consistent modelling, the disturbance recorder function described as a requirement in
IEC 61850-5 is decomposed into one LN class for analogue channels (RADR) and another LN
class for binary channels (RBDR). The output refers to the “IEEE Standard Format for transient
data exchange (COMTRADE) for power systems” (see IEEE C37.111:1999). Disturbance
recorders are logical devices built up with one instance of LN RADR or LN RBDR per channel.
Since the content of logical devices (LD) are not standardised, other LNs may be inside the LD
“disturbance recorder” if applicable. All enabled channels are included in the recording,
independently of the trigger mode (TrgMod).
Data ob|
Same docs) em
— | Sevmnersisree = [| |
Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
[Reamace [ses [Recorsing made
fmm ins [raromber SSS
ourmon ns _foratoutnumber SSS
fess [srs [recwanostanes SSO
[Contos
rest (SC [Wiggerresorar—SSSCSC~S~S~S
[MemRs ___—«|SPC__|Reset recorder memory (set the pointer of memory start to the beginning) |T|O |
IMemcr _|sc _|oearmamery erase al content atte memen (TO |
loocame no [Reetaniepwratoncouner SS SSS~wdCO
Settings
FioWee EN [rger made (nonal ager enemalorbom)—————SSS*d
lewos [ene feveltogermose SS SSSSSCS~S~*C~_CS
ce a
etme no ostwaperane SSS
IMemwunnea NG [asimurnomberatreorae——SSSCSC~S«~iO CS
jretonos [sro [revermage —SSSSSCS~S~S
Pectatne no [Povoae ogeimeins SSO
a
Fcawoa en [Recorder peraton node atraton ww) —————SSS—~id
[arte wo [Soage rate. sping rat oft dtrbanc rarer | |
a
an
https:/www.doc88.com/p-80980482981320.htm! 85/185
```


## File page 086

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
-4- 61850-7-4 © IEC:2010(E)
PORE ss
a
name
NOTE 1 The trigger modes (TrgMod) of RORE, RADR and RBDR are not independent. If the trigger mode of
RORE is external, the trigger modes of RADR and RBDR may be external (no extension of trigger possibilities) or
internal (extension of the external trigger mode). If the trigger mode of RDRE is internal, the trigger modes of
RADR and RBDR should also be internal because otherwise, no trigger possibility is provided.
INOTE 2 The source of the extemal tigger le a local leave. t may be a contect ora signal from another logical
node.
NOTE 3. The source of the internal trigger is an event detected by the supervision of the channel. It may, for
analogue channels, be a limit violation or it may, for binary channels, be a status change. The trigger levels
(high/low) for analogue channels for internal triggering have to be set per channel
NOTE 4 Since in case of sensors providing the analogue data as samples, the sampling rate at the source (TVTR
land TCTR) as defined in IEC 61850-7-3 as data attribute, smpRate may be different from the sampling rate of the
recording unit. Therefore, in line with Table 8, the sampling rate of the RDRE is a data object called StoRte
meaning storage rate.
5.13.7 LN: Disturbance record handling Name: RDRS
For a description of this LN, see IEC 61850-5. This LN shall handle the disturbance records
acquired by some local function. This LN is normally located at station level.
RS less
mor
c
=| Saree |
Instance-ID according to IEC 61850-7-2, Clause 22.
[Data objects
[Comers
Iatcuoted [SPO [Avomate wend SSS
lowes [sro _Joowwneos SSCS J
5.13.8 LN: Fault locator Name: RFLO
For a description of this LN, see IEC 61850-5. In case of a fault, the fault location is calculated
ind.
PLO ass
= Ssaeeeare |
Instance-ID according to IEC 61850-7-2, Clause 22.
[Status information
\Measured and metered values °°
jz fom Favtimpecance
[roam fw frawtastece SSS
[Controts
locus [Wo [esstanie operation comer SSS
5.13.9 LN: Differential measurements Name: RMXU
This LN shall be used to provide locally calculated process values (phasors calculated out of samples
or the samples itself) representing the local current values which are sent to the remote end and which
are used for the local differential protection function (PDIF). Therefore, the LN RMXU together with
LN PDIF models the core functionality of the differential protection function number 87 according to the
IEEE designation (C37.2). In addition, the LNs RMXU on both sides of the line represents also the
function to synchronize the samples. Therefore, also the samples sent from the local TCTR to the local
PDIF are routed through ) function represented by RMXU. The local RMXU is therefore the source
nw
https://www.doc88.com/p-80980482981320.html 86/185
```


## File page 087

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)

61850-7-4 © IEC:2010(E) -85-
of synchronized samples or phasors from the local current sensor, which sends its information to the
local PDIF and to all required remote PDIF nodes.
| Sener [| |

Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
\Measured and metered values
Ice Wve [Coron pasa ate sal corentmansuenent ‘|e _|
|AmpLocPhsA __|SAV__|Current (sampled value) of the local current measurement (phase L1) | |C__|
|AmpLocPhsB [SAV __|Current (sampled value) of the local current measurement (phase 2) | |C__|
JAmpLocPhsC __|SAV__[Current (sampled value) of the local current measurement (phase L3) | |C__|
fametocnes sav Curent (sampled valve) of the loa! curent meseurement(reskuel le |

current)
5.13.10 LN: Power swing detection/blocking Name: RPSB
For a description of this LN, see IEC 61850-5. The power swing is characterised by slow
periodic changing of measured impedance. Such a moderate impedance change is tolerated,
but may result in tripping of the distance protection function. If the generator is out of step (pole
slipping), transient changes of impedance (one per slip) are measured. After a small number of
slips (MaxNumSlp) in a dedicated time window (EvTmms), the generator shall be tripped to
avoid mechanical damage (out of step tripping). The actual number of slips shall be reset
either by the trip or by the end of evaluation time.

Data obj

a
| iavenermiamese a [|

Instance-ID according to IEC 61850-7-2, Clause 22,
Data objects
jv AD (Ban ponersnng atc) ———SSSSCSCS~S~w
lon not operate ut of stp wpning) —————SSSSCSC~S |
lant [sr [oorngotp0S sere SSS
[Comtrots
locate NC [Resetabie opemioncaomer——SSSSS~S~diO
Settings
[NoGra____[s@___[Neoatve sequence curent superision enabled | J
[Maxéna_[sPG___|Maximum current supervision enabed |
[evove_ fase Power ewing dea fo
[Sworis [as [Power swingdetaR tf
[Sworeact [asc __[Powerswingdenax fo
fusautinns [wa funoioctingtime fo
[MaxNumSip ING ___|Maximum number of pole slips until ripping (Op, out of step tripping) | [0 _|
[EvTmms __|ING __ Evaluation time (time window, out of step tripping) [jo |
‘Condition C1: Mandatory if RPSB is used for “Power swing blocking”.
[Condition C2: Mandatory it PSB is used for “Out of step tripping”.

an
https://www.doc88.com/p-80980482981320.html 87/185
```


## File page 088

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
—86- 61850-7-4 © IEC:2010(E)
5.13.11 LN: Autoreclosing Name: RREC
For a description of this LN, see IEC 61850-5. The number of trigger modes (CycTrMod /) and
reclose times (RecTmmsi) is equal to the maximum allowed number of reclose cycles
(MaxCyc). The trigger for the activation of RREC can be the start signal of PTRC, or the report
“breaker open” of the circuit breaker, or any other signals and combination of signals. If
different types of protections are involved in the autoreclosing process, all relevant data objects
have to be published and subscribed by the allocated protection LNs. A principal diagram of
RREC is given in Annex G.
PRE tas
Data obj
a
| isan |
Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
\CycTrMod(n] and RecCyc) for the next trip to be subscribed by the
iprotection
[RecCyc __—_—_—|INS_| Actual reclose cycle (number between 1 and UseCyc) [Jo |
[once [act operation sos awien"sueeto cove me xcOR——————(t (|
ftonecst ENS [Adorecoangstine Si i
(sewngs
= Sa FF
‘requested in the cycle indicated by the DO index
Wwexcye [NG Masirurromberatresore eyes SSSS~dC CS
lusecye oso aca et maximum umber orecure ges | o_|
ll alll —ialeieenniaaieeee i
ipermitted
= Pe a
ithe cycle indicated by the DO index
FsciaTnmat [NG [Recoreimetorevoing tate ————SSSSSCS~di
scatnmsi NG [Recoee ine for hase auts SSS
jaytnms [NG [Time beweonsuccessirenaieadyaie —————~—S~S~=w
All settings with an index higher than 1 up to MaxCyc will appear if MaxCyc is higher than 1.
For the number of actual permitted (used) cycles holds: UseCyc < MaxCyc.
5.13.12 LN: Synchronism-check Name: RSYN
For a description of this LN, see IEC 61850-5. The voltage phasor difference from both sides of
an open breaker is calculated and compared with predefined switching conditions
(synchrocheck). Included is the case that one side is dead (example: energising a dead line).
PSN ss
p _| eerie = |
Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
[ind |s°5_Notage torneo nace SSS
nw
https://www.doc88.com/p-80980482981320.html 88/185
```


## File page 089

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 718 > @ Q_ View A mark Y Annotations v | Q
61850-7-4 © IEC:2010(E) -87-
fos [ss requaneyaterncendear SSCS
[Measured and metered values
lowce [Mv [Gakuiatedaterrceimvotape ——SSSCSCS~S~S~«~rCzCS
lomece Jw _[caeutesatornce nteaueney ————SSSS~w
[biangoe wv _[oleaed ditrence ot phase argo ———SSSSSS~sd
[Controts
fsmera SPC __[Stanand nop ecwocneck rors ———SSSCSCS~S~S~driC
Settings
a NT
[nix [ps0 _[oteence phase ang ——SSSSSCSCS~S
[woeaiicd [ena [ive deadmege SSS
[unvar as0_[tve evans SSCS
[Lvovever_ [asa uvebuevawe fo
Fertmms [NG [Total tine at ayncwonaing pocess———SSSCS~S~S~S~Ci
5.14 Logical nodes for supervision and monitoring LN Group: S
5.14.1 Modelling remarks
Table 9 gives the relation between IEC 61850-5 and this standard for supervision and
monitoring LNs.
Table 9 — Relation between IEC 61850-5 and IEC 61850-7-4
for supervision and monitoring LNs
mutennesenoovaen fous [SNe ncumcrgancnnesre |
Insulation medium supervision
SIMG Insulation gas such as SFe
XCBR
[sexonentorrme [ieee (sore—_[omennsensanarcanansxsm
[Powerivansomersipownon [vera [SPT _[SupownonpanorverR =|
[swicnsupervsen __—_‘[cswr [ssw [Superson pat orcswi =
[creut beaker supewson [COR | sc8R [Supervision panotxcaR =i
a
an
https://www.doc88.com/p-80980482981320.html 89/185
```


## File page 090

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
—88- 61850-7-4 © IEC:2010(E)
5.14.2 LN: Monitoring and diagnostics for arcs Name: SARC
For a description of this LN, see IEC 61850-5.
po | beeen = ||
instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
nn
[Comtrots
|OpCntAs __—i[INC_[Resettable operation counter (switch and fault arcs) | [O.
Inccnine [nc —_[swienareeasmer SSS
5.14.3 LN: Circuit breaker supervision Name: SCBR
For a description of this LN, see IEC 61850-5. This LN is used for supervision of circuit
breakers. Operating a breaker and especially tripping a short circuit causes always some
abrasion (or erosion) of the breaker contacts. The supervision is per phase since each phase
has its own contact.
Data object
|_rames" (decane) mm
=| Saimin [|
Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
[coopn [56S _[onenconmandofwpeai ———SSSCSCS~S~Ss
Irwin [S°5 [conser abrasion warning ——SSSSCSCS~S~S~
loormam _|S*5 _[swicnopwratng ne eceoses————SSSSCS~S~S~w
saa all =" —-salaaal i
level for number of operations
[Gpcnwin [SPS Number peratons modeled nthe XCBR) exces the waving | [O |
[ootmwm [5S Warning when operation me reaches he warningeve | (0 _|
lootmn [NS Time snc station rast mainancemhours__——*| fo _|
[Measured and metered values
Jaccabr [MV [oumuiated abrasion
[Swe [av___[urrent nat was interrupted during last open operation | [0
[actabe fav [Abrasion ot ast open operation
JAuxswtmopn [wv [Auxiiary switches timing open fF
[AuxSwrmcis [Mv ___[Auxiiary switches timingelose
[Rertmopn _[uv___[Reaction time measurement open
[Rettncis [uv [Reaction time measurement close LO
lonsacnn [uv __, foveraton speedopen fo
an
https:/www.doc88.com/p-80980482981320.htm! 90/185
```


## File page 091

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
61850-7-4 © IEC:2010(E) -89-
losrmce fw fopoatontine sure SSS
lownom fw _fowestoneopen SSCS JO
fine bW_[Tenpeatwe eg: ave mestanam ———————~=d
[Contras
nn A
Settings
JAbramuey _[as@__ [abrasion sum twesnoidtoraiarmstate J
Janewentey [asc [abrasion sum threshold orwamning tate J
[Opaimtmn [inc [Alarm level for operation time imhours | ft
Jopwentmn [ING [Waning level tor operation time inhours J
Jopamnum [wa [alarm ievettor number ot operators fo
[opwentwm [wa [Warning level tor umber of operations Jo
5.14.4 LN: Insulation medium supervision (gas) Name: SIMG
For a general description of this LN, see IEC 61850-5. Insulation medium is gas, for example
SF6 in gas isolated devices. For other measuring objects related to the same IED, a new
instance of SIMG may be used. If the new measuring point(s) is/are related to a new IED, in
this new IED a new instance of SIMG shall be used.
Data obj
a
LNName The name shall be composed of the class name, the LN-Prefix and LN-
Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
[Status information
[mean [ars [insulation ges crica rat istation medium)
[nse (SPS insulation gas not sate (back device operation) |
linstr _———«[SPS__| Insulation gas dangerous (trip for device isolation) [jo |
[Presaim [ss [insulation gas pressureairm
[Penain fers [insulation ges devatyatrm
Frmpkim [5° _|nuaton gas ompurature atm SSCS
linsLevMax [SPS __insulation gas level maximum (relates to predefined filling value) [jo |
[natewin [SPS sation gut eve imum (ate to redetinea ing vain) | fo
[Measured values
pres fu [inauiation gas pressure
[een fu inautation ges denaty
[ime [uv nsutation gastemperature tf
Pte oceltrmeressiee |F |
gas compartment
a
an
https://www.doc88.com/p-80980482981320.html 91/185
```


## File page 092

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
-90- 61850-7-4 © IEC:2010(E)
[Contras
loscuns [Wo [Reetabieopraton comer SSS
5.14.5 LN: Insulation medium supervision (liquid) Name: SIML
For a description of this LN, see IEC 61850-5. The insulation medium is a liquid such as oil,
like that used for example for some transformers and tap changers. For other measuring objects
related to the same IED, a new instance of SIML may be used. If the new measuring point(s)
is/are related to a new IED a new instance of SIML shall be used.
= Seer |
Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
linsaim [SPS [Insulation quia critica refit insulation medium) | IM
linsBk [SPS __ [Insulation quid not safe (block device operation) | 0
Insulation liquid dangerous (trip for device isolation) [jo |
[Fmpaim [SPS nsuation iquid temperature arm ft
[GasinsAim [SPS __|Gas in insulation liquid alarm (may be used for Buchholz alarm) [jo |
|Gasinstr [SPS __|Gas in insulation liquid trip (may be used for Buchholz trip) [jo |
[Gasriwtr [SPS __|Insulation quid flow trip because of gas (may be used for Buchholz tip) | [O |
linstevax [SPS Insulation iquid evel maximum | ft
[instevitin [ses Insulation iquid evel minimum
[ewen [ses [Hgwamningievel
[Mstaim [ses [Moisture aiarm
[Measured and metered values
[tmp fv insulation quid temperature
Lev fav Pinsuation ui Havel (usualy in m) lo _|
[Pres (MV insulatoniquidpressure
[H20 Ss |MV_[ Relative saturation of moisture in insulating liquid (in %) [lo |
[H20Pap [uv [Relative saturation of moisture in ineuiating paper in’) | [|
[eon [MV_[Retatve saturation of moisture in arin expansion volume (n%) | [0 |
[orm ww Tenporatre of rating hada point of gO measurnont | Jo |
leon WW (Messrementotysonen tainsem «dO
‘evom wv Measurement ofpinpem ——SSSSCS~S~CO_CS
[corp nv _Mrassroment ot COmsem SSS
[compm WW Massuenentotcoginsem —SSS~S~S
[cm WW MessvementotcHyinppm ——SSS~w
[kaon hv _Meatsronent of aignppmSSSS~s
[carom wv __Meassroment of at inom ———SSSSSS~S~«w YO
a
an
https:/www.doc88.com/p-80980482981320.htm! 92/185
```


## File page 093

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
61850-7-4 © IEC:2010(E) -S1-
Stas
_ONrame' dics) mmm
name
a
(2pm ww [Measurement f Oinpem ———SSSSCS~S~S~«~
lombucas _v___Mratsroment of toa seed combate gases OGG) | fo _
Fics fw [ramansvoinensucmoreay SS —*d
[Contras
locus [Wo [Roetabioopaton comer ————SSSS~S
5.14.6 LN: Tap changer supervision Name: SLTC
This LN is used for supervision of tap changer. It is used to assess the condition of the tap
changer.
ST lass
Oram doco) mmm
data class
e= _| aera = |
Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
nS
IMsonax [SPS Meer erve ovecurenibiocing SSS
[vacconin [srs foveut sas of aoum con ans) —————SSSSS—~d
low: ses —foumeruntwe SS SSSSSC~di
|Measured and metered values
fnwen —v_[Avrasion Gn ot pane wopcriower————SSSS~wd
[Controts
[opens [6 [Revetabe oponioncaunr SSS
5.14.7 LN: Supervision of operating mechanism Name: SOPM
This LN is used for supervision of operating mechanism for switches. It is used to assess the
condition of the operating mechanism and can be used to indicate a possible malfunction in the
future.
Today, different technologies for operating mechanisms are available. Typically operating
mechanisms for circuit breakers contain an energy storage to provide the required switching
energy within a short time. Examples for today’s storage medias are springs or compressed
gas. To operate the switch, the energy is transferred by means of a mechanical or hydraulical
linkage. A charger motor is used to compensate energy losses due to leakages or to recharge
the storage after a switch operation.
The proposed attributes cover the status of the relevant components both of the hydraulic
‘system and the spring system. Depending on the used technology, some of the attributes are
not applicable. This LN can also be used for simple operating mechanisms that are directly
driven by a motor.
a
nw
https://www.doc88.com/p-80980482981320.html 93/185
```


## File page 094

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
-92- 61850-7-4 © IEC:2010(E)
Po 80pm
a
name
=| aaa == |
Instance-ID according to IEC 61850-7-2, Clause 22.
a
IMtsvaim [SPS [Alarm or numberof mtor sats eaceeds ont =
[Hyam ses Hydrauicatarm
[Hom [5S [ok of operation wotoryaraue ———SSSCSCSC~S~S~di
fox [SPs _fenwoyoek —SSCSCS~S~S
[Nowmim [5S Motor opening tine exweaed ———SSSCS~S~«~_
[caine [NS Time area bowsen ast we charging operation ———~SCSC~*dri
Imesy [NS Nomberotmowrsune SS SC«
fn (av [towdeneay en. sores eerayorrenaninaeeoy ———«d(S_|
lHyPres |v [Ayorautie pressure
lHytmp wv varautic temperature
ca
[Mota fy [Motorcurent
fin bY __[enpeaeinsaeteawoamice ———SS~S~wdi
a
IMoaintns [NG [Aamivollornoiormntneins ———SSSSSC«dC
Imasivin [NG [Aarmvatornumeratmowrsure ———SSSSSCSC~dCi
IMosvtns NG [Time imal tr aequston otmowrstars———SSSSC~«dC
5.14.8 LN: Monitoring and diagnostics for partial discharges Name: SPDC
For a description of this LN, see IEC 61850-5. IEC 60270 should be applied.
Pome’ idencune| em
name
= Eee |
Instance-ID according to IEC 61850-7-2, Clause 22.

[Data objects
abscnain (SPS [Panalaacnageaarm ————SSSCSCSC~S~S«~i
fcuascn WW [Acstciewiciparialdestowe SS

Ly

8

an

https://www.doc88.com/p-80980482981320.html 94/185
```


## File page 095

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
61850-7-4 © IEC:2010(E) -93-
8 ts
feoratsen ww [paren charge of pail aacharge peokieveiPO)———*dC |
an
luwPadacn wv |UhF wel of pant ascnare SSS
[Controts
[pene JRE Reetaseowrtoncommer Cd
Settings
lenis ps0 [boner vogue of memsrenent unt accomerorecemre [|_|
Jaw [AS0_[banawihotmessurrent ont accrang oiecenaro + o_|
Condition C: depending on the functionality, at least one of the data objects AcuPaDsch, UHFPaDch, NQS,
AppPaDsch or PaDschAim shall be used.
5.14.9 LN: Power transformer supervision Name: SPTR
This LN is used for supervision of power transformer. It is used to asses the condition of the
power transformer.
PPT Css
Data object
fame
LNName [The name shall be composed of the class name, the LN-Prefix and LN-
Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
[eetmpaim SPS Winding houpotonpwratealem —————SSSSSS~*d
Inptmp0» [ss wining notpotonpuratue apoio ————SSSSSCS~dO
lyptmptr [ss |winang hotpotenparteve ————SSSSSSCS~*
jxkim [SPS ashage supervision alam of nk conser membrane | fo
coum srs _frowgonaawm —SSC*dO
lorinp WW otomtienperie SSS
lcoretme [uv [core temperate Cf
lnermpce jw [eases wnsrghoupatonperture —————SSSSC~=id
[Contras
[pene JRC Rewtaseowrtoncommer Cd
5.14.10 LN: Circuit switch supervision Name: SSWI
This LN is used for supervision of all switches, such as disconnectors, earthing switches, etc.
except circuit breakers. It is used to assess the condition of the switch and is closely related to
LN SOPM. Most attributes are used to supervise the operation time of the switch and contact
movement. Deviations from nominal values can be used to indicate a possible malfunction of
the switch in the future. Abrasion of parts gives an indication when to maintain the switch. For
the special requirements of a circuit breaker, the abrasion, etc. is defined in LN SCBR. The
supervision SSWI is per phase.
a
nw
https://www.doc88.com/p-80980482981320.html 95/185
```


## File page 096

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 718 > @ Q_ View A mark Y Annotations v | Q
-94- 61850-7-4 © IEC:2010(E)
a
fame

LNName ‘The name shall be composed of the class name, the LN-Prefix and LN-

Instance-ID according to IEC 61850-7-2, Clause 22.
loprmam [5S [Snichopeaiog me owneses SSCS

|Number of operations (modelled in XSWI) has exceeded the alarm level

itor number of operations
[pcm 5S ombwr of pratons (mooted XSW wxcende he waring tnt | |
[Cotmwn [5S aring when operation time rashes te warorgiewi |_|
lostmn —|NS__Tine aceinstaiaton or ast mainenanceinnous | fo
cca WW __[Comunodabanon para aectiowser ————SS—«d
Ianswrnoon ww [itary awicnes ting epen——SSSSS~*d YO
Inuswtmcis [ww [Aviary awtenes ming cose ———SSSSCS~S~S~«~
Fettnoon WW [Reacton ine measvonentopon————SSSSSSS—*d
erince Jw [Reston ine messuementcase ————SSS~wdO
[nspsopr fw _fopeaionspeesepen ——SSSSCSCS~«dCiO
lonseacs wv joneaton speedos SSS
lootmom WW fopoationtine open SSS
lostmcs wv __[operiontmecoss SSS
lowsiom hy _fowestoie open SSS
[im [w_[Temperaure ag mise ave nectanam ————=SSCSC~*~*~«~diN—=C*d
cc
lconmtan [WG [Asm eveltropeatontneinnows————SSS~«wd
lopwmtm [no Waring iovetor apoio ine mtous ————S~=d
loonmnm [wo [Warm wel tor umber ofpuratons_—__———S—SS—~i
[Commun wo Waring level tornamberofapeatons————SSSSSS~«wd
5.14.11 LN: Temperature supervision Name: STMP
Logical node STMP shall be used to represent various devices that supervise the temperatures
of major plant objects. It provides alarm and trip/shutdown functions. If more than one sensor
(LN TTMP) is connected, the LN STMP shall be instantiated for each sensor.

a
an
https://www.doc88.com/p-80980482981320.html 96/185
```


## File page 097

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
61850-7-4 © IEC:2010(E) -95-
a Be
mame" _| etn cas rn |
|The name shall be composed of the class name, the LN-Prefix and LN-
Instance-ID according to IEC 61850-7-2, Clause 22.
(Status information
iHeain [ENS [Exemalequpmentneatm ———SSC~S
lam [srs [Temperature alarm iveleacnes —————SSSSCSCS~di_
fee [srs [Tempore pieveireenee SSS
(Measured and metered values
frm remperauwe
\Controts
[scune RC esata wporaioncoomer SO
Settings
Fimpkinspr [ASG Tempera aarmieveisetpom SS
finoTenser [ASG [Temperature npiweisersoin ——S~S~Sw
5.14.12 LN: Vibration supervision Name: SVBR
Logical node SVBR shall be used to represent various devices that supervise the vibrations in
rotating plant objects such as shafts, turbines, generators etc. It provides alarm and trip /
shutdown functions. If more than one sensor (LN TVBR) is connected, the LN SVBR shall be
instantiated for each sensor.
Data obj
a
‘The name shall be composed of the class name, the LN-Prefix and LN-
Iinstance-ID according to IEC 61850-7-2, Clause 22.
Data objects
Jam srs [veratonatarmievetreacted
Iwo [ss _|vratonswpiewieacres———SSSCS~S~S~«~
(Measured and metered values
Ino [WW Totlasialdapaconen SSCS
Gc
[Settings
a
\errpspr [ASG _|vartonvpievelsetpom ——SSSS~S~Ci
Isoamspt [ASG [aval dspaconentatwmievlsetsont «dL
[soTrpse [nso [aia picomen vip evetsetpein ————SS~wCi
a
an
https:/www.doc88.com/p-80980482981320.htm! 97/185
```


## File page 098

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q

-96- 61850-7-4 © IEC:2010(E)

5.15 Logical nodes for instrument transformers and sensors LN Group: T

5.15.1 Modelling remarks

This group of logical nodes represents the sensors for all the different values which have to be

continuously sampled for monitoring their behaviour over time. These samples are used either

by dedicated processing logical node classes as for protection (see LN Group P) or by the

related supervision logical node classes (see LN group S). The sampling rate defines the time

resolution of the resulting figures of the processing logical node classes (group P, group S).

The modelling of samples are conditional since they are not exposed to communication in any

case, as T and S nodes may be implemented in the same IED.

5.15.2 LN: Angle Name: TANG

Logical node TANG shall be used to represent a measurement of an angle between two objects

(one of which might be a theoretical vertical or horizontal line). The measurement can be

returned optionally as degrees or radians (° or rad).

Po TANG lass
My
orc

— | Gaveereaatass ||

linstance-ID according to IEC 61850-7-2, Clause 22.

[Data objects

[enone R___[Eenaleqipmeninanesaie SSCS

a

therefore it is visible.

5.15.3 LN: Axial displacement Name: TAXD

Logical node TAXD shall be used to represent an axial displacement value. The axial
displacement can, depending on the application, be either longitudinal or transverse to the
shaft. This sensor is often used together with vibration sensors as input to a vibration
monitoring system.

Pacts

j— | aninareaniree ee ||

linstance-ID according to IEC 61850-7-2, Clause 22.

[Data objects

fenane [DP [Eteraleaionenmanepae

[AxDspSv SAV [Total axial displacement

a
nw
https://www.doc88.com/p-80980482981320.html 98/185
```


## File page 099

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
61850-7-4 © IEC:2010(E) -97-
[settings
jore gets no updated values (values are frozen). It is visible.
5.15.4 LN: Current transformer Name: TCTR
For a description of this LN, see IEC 61850-5. The current is delivered as sampled values. The
sampled values are transmitted as engineering values, i.e. as “true” (corrected) primary current
values. Therefore, the transformer ratio and the correction factors are of no interest for the
transmitted samples, but for maintenance purposes of an external conventional (magnetic)
transducer only. In addition, status information is provided and some other settings are
accepted from the LN TCTR.
The name shall be composed of the class name, the LN-Prefix and LN-
Instance-ID according to 1EC 61850-7-2, Clause 22.
JEEName __[DPL_[Extemaiequpmentnamepiate
EeHoatn ENS [Exeraloaupmentarm SSS
joptmn __|ws __oeratontime Cf
Measured and metered values
Settings
Fat [ASG wingng ato of a neal cent varatarmer ranaaicoy Wapteab| [O |
[cr [as _Jouron ator magntuse carecion fn entra cure vanstrmar_| |o3 |
fratar [aS _[euron phasor angle corecton ot an eter cuentvarstomer | |e? |
(crc [686 [our prasormagniose andangiearecton =i (|
[Condition C1: The data object is mandatory if the data object is transmitted over a communication link and|
therefore it is visible.
|Condition C2: If there are two or more correction pairs necessary, CorCrv should be used.
5.15.5 LN: Distance Name: TDST
Logical node TDST shall be used to represent a measurement of the distance to an object that
can move. It is intended to provide a measurement between a fixed location and a movable
object.
Common
data class
a er
linstance-ID according to IEC 61850-7-2, Clause 22.
leENane ORL [Btonalaqupnenimanepaie————SSSCSCS~dCiCS
‘Status information °
an
https://www.doc88.com/p-80980482981320.html 99/185
```


## File page 100

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
-98- 61850-7-4 © IEC:2010(E)
JMeasured and meteredvawes
Settings
snore [Ganong atesoung id
jore it is visible.
5.15.6 LN: Liquid flow Name: TFLW
Logical node TFLW shall be used to represent a measurement of media flow rate through the
device where it is located.
— | Ease |
linstance-ID according to IEC 61850-7-2, Clause 22.
Descriptions
[EEName ___[oPL__ [External equipmentnamepiate JO
(Status information,
[eeHeanm JENS __ [External equipmentheatn JO
JMeasured and metered vawes
JFwsy sav Liquid tiow ratetmas)
[Settings
ISmpmte [NG |Samping ate seting SSCS [|
refore it is visible.
5.15.7 LN: Frequency Name: TFRQ
Logical node TFRQ shall be used to represent a measurement of frequency. It is intended for
any frequency that is not related to electrical a.c. measurements. It can be used for example
for sound measurements, vibrations and timing of repeated occurrences. If a pure vibration is
to be measured, where the movement rather than the frequency is of interest, the TVBR logical
node is recommended.
Common
data class
CN A Se
linstance-ID according to IEC 61850-7-2, Clause 22.
[EEName __[DPL [Enteral equpmentramepate SO
[eeHeatm JENS _ [External equipmentheatn
JMeasured and metered vawes
[Hzsv SAV ga Frequency {Hz} related to non-electrical vaves | [C
an
https:/www.doc88.com/p-80980482981320.htm! 100/185
```


## File page 101

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
61850-7-4 © IEC:2010(E) -99-
Jsettings
therefore it is visible.
5.15.8 LN: Generic sensor Name: TGSN
Logical node TGSN shall be used to represent a generic sensor if there is no specific sensor
available. It can also be used for modeling the health and name of an external equipment
(sensor).
|The name shall be composed of the class name, the LN-Prefix and LN-
instance-ID according to |EC 61850-7-2, Clause 22.
[Descriptions
IEEName OPC Ewernal equipment nameplate SSS
[Status information
JEcHeann Jens ___[Externatequipmentheatn J
[Measured and metered values
[Gensv Sav [Generic sampedvawe
[settings
retore it is visible.
5.15.9 LN: Humidity Name: THUM
Logical Node THUM shall be used to represent a measurement of humidity in the media that is
monitored. The result is given in percent of maximum possible humidity.
|The name shall be composed of the class name, the LN-Prefix and LN-
linstance-ID according to |EC 61850-7-2, Clause 22.
[Descriptions
JEEName ___[OPL__External equipmentname plate J
(Status information
[EeHeanh ENS External equipment heath
JMeasured and metered vawes
[Humsy sav JHumicity toy)
[Settings
[smote [NG [Samping rate sotng SSS J
ondition C: The data object is mandatory if the data object is transmitted over a communication link and|
therefore it is visible.
°
an
https://www.doc88.com/p-80980482981320.html 101/185
```


## File page 102

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
— 100 - 61850-7-4 © IEC:2010(E)
5.15.10 LN: Media level Name: TLVL
Logical node TLVL shall be used to represent a measurement of the media level in the
container where it is located. The level is expressed as a percentage of full container. For a
measurement given as a distance from a base level, the HLVL logical node shall be used.
Potts
‘The name shall be composed of the class name, the LN-Prefix and LN-
linstance-ID according to IEC 61850-7-2, Clause 22.
[Data objects
[Descriptions
IEeName __[OPL___|Eemalequpmentname pate SCS |
[Status information
[eeHeatm [ENS _[Externalequipmentheatn CO
luevpctsv sav twit
[smote __[ING__|Samping rate setting __ iJ _|
Itherefore they are visible.
5.15.11 LN: Magnetic field Name: TMGF
Logical node TMGF shall be used to represent a measurement of the magnetic field strength at
the place where it is located.
Pt elas
‘The name shall be composed of the class name, the LN-Prefix and LN-
linstance-ID according to IEC 61850-7-2, Clause 22.
[Data objects
[Descriptions
IEeName ___[OPL__|Extemal equpmentname pate SCS |
IMaoFisy —__[SAV___[Magnatic field strength Nox denaiy ) ——————~di |
[smpfte [NG [Sanpingratesening SSCS |
Itherefore it is visible.
5.15.12 LN: Movement sensor Name: TMVM
Logical node TMVM shall be used to represent a measurement of movement or speed. It is
intended to provide a measurement of the speed, in m/s, with which two objects (one of which
may be fixed) are moving in relation to each other.
a
nw
https://www.doc88.com/p-80980482981320.html 102/185
```


## File page 103

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
61850-7-4 © IEC:2010(E) -101-
ee
linstance-ID according to IEC 61850-7-2, Clause 22.
[EeName —__JOPL [External equipment name plate ——————S YO
[Status information
[EeHeanh ENS [External equipment heath
[Measured and metered values
JMmmtesy [sav [Movement rateims}
Jsetngs
[Smpmte [NG Samping ae eoting SSCS [|
therefore it is visible.
5.15.13 LN: Position indicator Name: TPOS
Logical node TPOS shall be used to represent the position of a movable device, actuator or
similar. The position is given as a percentage of the full movement of the device being
monitored. Compare with TDST that returns the distance in m.
Dera |||
linstance-ID according to |EC 61850-7-2, Clause 22.
iEeName [OPC [Extemalequpmentname plate SCS
[Status information,
JEeHeath JENS __[Externaiequipmentheaty
JMeasured and metered vawes
oaPeisv SAV [Postion oven ae percentage of ul movement [> _|
[Settings
[smote [NG___|Samping ate seting SSCS [|
ore it is visible.
5.15.14 LN: Pressure sensor Name: TPRS
Logical node TPRS shall be used to represent the absolute pressure of a medium. The medium
might be air, water, oil, steam or any other substance, the pressure of which needs to be
supervised.
| uae [|
linstance-ID according to IEC 61850-7-2, Clause 22.
a
an
https://www.doc88.com/p-80980482981320.html 103/185
```


## File page 104

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
— 102 — 61850-7-4 © IEC:2010(E)
iEename [OPC |Exteralequpmentname pate SY
(Status information
[EeHeath ENS _[Externalequipmentheaty Cf
JMeasured and metered vawes
JPressv_ [av [Pressure otmesia(ra)
Jsetngs
[SsmpRie ING [Sampling rate setting Cf
ondition C: The data object is mandatory if the data object is transmitted over a communication link and|
ore it is visible.
5.15.15 LN: Rotation transmitter Name: TRTN
Logical node TRTN shall be used to represent the rotational speed of a rotating device.
Different measurement principles may be used, the presented result is however the same.
a
Iinstance-ID according to IEC 61850-7-2, Clause 22.
IEEName OPC [Ewernal equipment nameplate SSS
[EeHeath ENS [External equipment heath
[Measured and metered values
Rospasv [SAV [Rotaional speed iv] SSCS |
Jsetngs
[snore [NG Samping ate soting | Jo _|
therefore it is visible.
5.15.16 LN: Sound pressure sensor Name: TSND
Logical node TSND shall be used to represent the sound pressure level at the location where
the sensor is located.
— | anna ||
linstance-ID according to IEC 61850-7-2, Clause 22.
Descriptions
IEEName [OPC [External equipment name pate Sid |
[Status information,
[EEHeaim JENS g&° [External equipment hea TO
an
https://www.doc88.com/p-80980482981320.html 104/185
```


## File page 105

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
61850-7-4 © IEC:2010(E) — 103 —
[sossy «(SAV ——‘Soundpessueimwiey SSCS
IsnoRe [NG anping io soting SCS
Condition C: The data object is mandatory if the data object is transmitted over a communication link and]
Itherefore it is visible.
5.15.17 LN: Temperature sensor Name: TTMP
Logical node TTMP shall be used to represent a single temperature measurement.
[The name shall be composed of the class name, the LN-Prefix and LN-
linstance-ID according to IEC 61850-7-2, Clause 22.
[Descriptions
[EEName __[OPL__Eenal equpmeninamepiate SCS |
[Status information
[EeHeann [ENS [Extemalequpmentneats SSCS
[tmesv sav [Femperature(rcy
[snomte [NG Samping rate seting SSCS |
Condition C: The data object is mandatory if the data object is transmitted over a communication link and]
therefore it is visible.
5.15.18 LN: Mechanical tension / stress Name: TTNS
Logical node TTNS shall be used to represent a measurement of the mechanical tension in an
object.
‘The name shall be composed of the class name, the LN-Prefix and LN-
linstance-ID according to IEC 61850-7-2, Clause 22.
[Descriptions
[EEName [OL Exemal equement nameplate +d je |
[snore [WG |Samping rate seting SSCS [|
therefore it is visible.
a
nw
https://www.doc88.com/p-80980482981320.html 105/185
```


## File page 106

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
— 104 - 61850-7-4 © IEC:2010(E)
5.15.19 LN: Vibration sensor Name: TVBR
Logical node TVBR shall be used to represent a vibration level value. In case the vibration can
be defined as a frequency, the TFRQ logical node could be used instead.
|The name shall be composed of the class name, the LN-Prefix and LN-
linstance-ID according to |EC 61850-7-2, Clause 22.
[Descriptions
[EEName ___[OPL___Eweral equpmentnamepate——SCS
[Status information
[eeHeanm JENS [External equipment heath TJ
JMeasured and metered vawes
ese (SAV (vbratonfmmae) SSCS
[Settings
[Snore [NG ___[Sanping ate sotimg SCS J |
retore it is visible.
5.15.20 LN: Voltage transformer Name: TVTR
For a description of this LN, see IEC 61850-5. The voltage is delivered as sampled values. The
sampled values are transmitted as engineering values, that is as “true” (corrected) primary
voltage values. Therefore, the transformer ratio and the correction factors are of no interest for
the transmitted samples but for maintenance purposes of an external conventional (magnetic)
transducer only. In addition, status information is provided and some other settings are
accepted from the LN TVTR.
ame oom) men
name
| bevaneresmese == |
Instance-ID according to IEC 61850-7-2, Clause 22.
[Data objects
[EeName ___[OPL___[Ewmal equipment nameplate SSS
[eeHeanm [ens [External equipment heath YO
loptmn _|INS_[Operatontime SSS
[Fura |sPs [vt tusetamwe
[Measured and meteredvalues
[voy (SAV [Notage ampledvatwo) SSS
[Senge
(Wig dR aeavoge SSS
lHetg asa Ratedtrequency fo
[Rat _——|ASG_—_|Winding ratio of external voltage transformer (transducer) if applicable | 0 |
lca ps0 tage phasor magnitude coreton of ew votagevaatomer | [o|
Jracar [as otage phasor ari corecion of xenalvotgevarsomer | [o2 |
Ico |es6 our prasormagniuse andangieconecion ————=S=«d |
[Condition C1: The data object is mandatory if the data object is transmitted over a communication link and
Itherefore it is visible.
|Condition C2: If there are two or more correction pairs necessary, CorCrv should be used.
a
an
https://www.doc88.com/p-80980482981320.html 106/185
```


## File page 107

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
61850-7-4 © IEC:2010(E) — 105 —
5.15.21 LN: Water acidity Name: TWPH
Logical node TWPH shall be used to represent a water pH level value.
0
— | Seer |
linstance-ID according to IEC 61850-7-2, Clause 22.
[Data objects
[EEName [DL Eemal equpmentname pits Sw
[eHean [ENS _[Externalequipmentheatn
[Measured and meteredvalues
a
[seings
[snore [NG [Sampingrateseting SCS
therefore it is visible.
5.16 Logical nodes for switchgear LN Group: X
5.16.1 Modelling remarks
The logical nodes of this group provide data which are needed to represent the related
switchgear equipment in the automation system. There are only two logical nodes (XCBR,
XSWI) since all not current breaking switches are modelled by XSWI. Each logical node has
companion logical nodes in group S (like SCBR, SSWI) providing the detailed supervision
information if needed.
5.16.2 LN: Circuit breaker Name: XCBR
This LN is used for modelling switches with short circuit breaking capability. Additional LNs, for
example SIMS, etc. may be required to complete the logical modelling for the breaker being
represented. The closing and opening commands shall be subscribed from CSWI or CPOW if
applicable. If no “time activated control” service is available between CSWI or CPOW and
XCBR, the opening and closing commands shall be performed with a GSE-message (see
IEC 61850-7-2).
po BR ss
‘M/O/
c
=| Saarare |
Instance-ID according to IEC 61850-7-2, Clause 22.
[Data objects
Descriptions
|EEName _[DPL____ [External equipment nameplate JOT
Lockey [Local or remote key (local means without substation automation
fey PS [minemton mromredarectconra) en |
[oc 55 fecal canvoooravour—SSSSSS*d
[Opcnt_ ins [Operation counter IY
a
nw
https://www.doc88.com/p-80980482981320.html 107/185
```


## File page 108

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
— 106 — 61850-7-4 © IEC:2010(E)
[@ncap lens [oreutanteropaina cepanuiy SSS
jrowcap [ens Pont on wave swing epebity ————SSSSCSC~«w
[MaxOpCap _|INS _|Circuit breaker operating capability when fully charged [fo |
[unsware [ocr [Sumofavichosanpows,reamabo————SSSC~«di
[Controts
[vcs 0 [Suichng ashoriyatwatoniewst ———SSSSC«d
roe orc —_[swicnposton SSS
Ianope 5° cropenng i i
jamcis [spc alockctosing
lommoiéna [so forage mowrensbeg SSS
[Settings
[cstnms [NO [Goang ne otoraxer ———SSSSC~wi
5.16.3 LN: Circuit switch Name: XSWI
This LN is used for modelling switches without short circuit breaking capability, for example
disconnectors, air break switches, earthing switches, etc. Additional LNs, SIMS, etc. may be
required to complete the logical model for the switch being represented. The closing and
opening commands shall be subscribed from CSWI. If no “time activated control” service is
available between CSWI or CPOW and XSWI, the opening and closing commands shall be
performed with a GSE-message (see IEC 61850-7-2).
LNName ‘The name shall be composed of the class name, the LN-Prefix and LN-
Pare | IrstuneotSaecrangtonecciessrecanean || |
Data objects
|EEName _[DPL___ [External equipmentnameplate J
|EEHealth _—'(ENS _‘[External equipment heaith
Lockey SPS Local-remotekey
a
[Swtyp ENS [Switchtype
[Maropcap [NS fSttch ‘operating capability when fully charged. Obsolete. Kept for [Pe
Ibackwards compatibility with Ed.1
[Controts
lecsin SPC _[Swichingaumoriyatattoniewt SSO
[pos |PC Switch position
[BkOpn __|SPC__—[Blockopening
jpacis_—|SPC_Block closing
[ChaMotena___|SPC____[Chargermotorenabled 0
a
an
https://www.doc88.com/p-80980482981320.html 108/185
```


## File page 109

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
61850-7-4 © IEC:2010(E) -107-
5.17 Logical nodes for power transformers LN Group: Y
5.17.1 Modelling remarks
The logical nodes of this group provide data which are needed to represent the related
switchgear equipment in the automation system. This data may be complemented by
supervision logical nodes of group S if needed.
5.17.2 LN: Earth fault neutralizer (Petersen coil) Name: YEFN
For a description of this LN, see IEC 61850-5. This LN shall be used for suppression coils as
tap coils and plunge core coils.
=| Seamer |
Instance-ID according to IEC 61850-7-2, Clause 22.
[Data objects
|EEName _[DPL___[Externalequipmentnameplate CJ
|EEHeaith ENS [External equipmentheaith
|tocKey SPS localremotekey
a
[Optmh NS ___[Operationtime
nares |sP5_[endposion serene SSS JO
Ienapost [ss __[Endpostionowerrened ———SSCSC~S«~
roan [ss __[Powntonwrawm ———SS~S~S~S«™
[Motam ses [Motoraveaarm
[Measured and metered values
caPoek WW [Colpontionaenveatominearonp —_———SSS—~d
CE
Imavor _louv Nowaltogrunsvonage SSS
[Controls
locsta SO [Swtchng autor atsatoniew’ —————SSSSS~«wdi
[carapos isc _[ooivapposton ——SSSCSCS~S~S«~
lcaPos [APO _[pungecoe poston ———SSCS~S~S~Sw
|Coichg _—«|BAC__| Change coil position (higher, lower, stop) [jez _ |
Condition C1: Is only used if not fixed coil should be modelled.
Condition C2: At least one of the data objects should be mandatory.
5.17.3. LN: Tap changer Name: YLTC
For a description of this LN, see IEC 61850-5.
Data obj
a i
=| Sai [|
Instance-ID according to IEC 61850-7-2, Clause 22.
[Data objects
an
https://www.doc88.com/p-80980482981320.html 109/185
```


## File page 110

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
— 108 — 61850-7-4 © IEC:2010(E)
narosR [ss [endponion rave roche ————SSSSS~w i
IenaPost [ss [Ena postion oworrcned————SS~Ss
loose [5 owt stp ale: supervision ostrich ayehonam |_|
ical ll ="
lswitch operation
laxtovae SPS __Jposres yiowaivecony SSCS
[Contras
fasPos [8 [oange wp poston oacategponten —————S—S~d P|
Favcne [a6 onange ap poston op Ng, we He
used.
5.17.4 LN: Power shunt Name: YPSH
For a description of this LN, see IEC 61850-5. The LN class power shunt also includes the
‘switch for closing and opening the shunt.
Data ob|
__PMrome’ éencame) SS mmmnm
| Eanes = |
Instance-ID according to IEC 61850-7-2, Clause 22.
[Data objects
|EEHealth ENS External equipment health J
[stopcap [ens joperaing capabiiy SSS
jnsopcep is Power sunt eporaingcaabity when uly anes ——————| o_|
[Contras
roe —[oRO [Swenson SSS
Janopn [sc _[bockopening —SSSSCSCS~S~Sw
lance [so _[aoorcosng SSSI
(Crauoténe [sc _[orarermotorensbies———SSSSCSCS~S«~C_
5.17.5 LN: Power transformer Name: YPTR
For a description of this LN, see IEC 61850-5.
o~ _| Eevee ee [|
Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
|EEHeath [ENS _[Externalequipmentheaith J
[Optmh NS ___—[Operationtime 0
lonaios _[s°5—fopemtanatvoad——SSSSSS~«dO
an
https:/www.doc88.com/p-80980482981320.htm! 110/185
```


## File page 111

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)

61850-7-4 © IEC:2010(E) — 109 -
(onow [5S [operation st vervnage ——SSSSCSC~S«~CCS
[Measured and metered values
|LodFact_ [MV Load factor (apparent power/ratedpower) JO
[MaxPwr __|MV____[Calculated maximum permissible permanent power (overload) (W]__| |O__|
jovitm ____|MV_|Calculated maximum permissible overload time with cooling unit [min] | [O |
eo P|

jemergency case) [min]
Settings
jnveig [ASG __[Raesvorage havotage wey SSS
[lovmig [asa Rated votage tow votage eve) re
JPwrrig [asc [Ratedpower Cf
[MaxPwrSpt___|ASG_[Maximum permissible permanent power (overload) (W} | fo |
lOviTmSpt___|ASG_|Maximum permissible overload time with cooling unit min} __ | fo
[ovTemoser [ASG [Maximum permissible ‘overload time without cooling unit (emergency [Pe |
5.18 Logical nodes for further power system equipment LN Group: Z
5.18.1 Modelling remarks
The logical nodes of group Z refer all to power system objects which are reusable in other
power systems domains but not modelled in other LN groups of this standard.
5.18.2 LN: Auxiliary network Name: ZAXN
For a description of this LN, see IEC 61850-5. Auxiliary networks belong to the power supply
system of substations and other power systems installations.

Data obj

Oram domes) nm
— | Einar [| |

Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
|EEName __[DPL___[Externalequipmentnamepiate J
|EEHealth [ENS [External equipment heath
jVol___[Mv____ [Voltage of the auxiliary network 0
lamp [MV [Current of the auxiliary network
5.18.3 LN: Battery Name: ZBAT
For a description of this LN, see IEC 61850-5.

Data object

a
LNName The name shall be composed of the class name, the LN-Prefix and LN-

Instance-ID according to IEC 61850-7-2, Clause 22.

a
an
https://www.doc88.com/p-80980482981320.html 111/185
```


## File page 112

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)

-110- 61850-7-4 © IEC:2010(E)
Data objects
|EEName _[DPL__[Externalequipmentnamepiate CJ
[EEHeath [ENS [External equipmentheaitn
|TestRsi_ SPS [Batterytestresuts
|BatHi_ SPS [Battery high (voltage or charge - overcharge) |
[Batto__|SPS__[Battery 'ow (voltage orcharge) Cd
jVol__ [MV [Battery voltage
\VoChgRte___[MV_—|[Rate of battery voltage change
jamp__— [Mv [Battery drain current
[Controts
jBatrest_ _——[spC__—Startbatterytest_ CO
(Settings
5.18.4 LN: Bushing Name: ZBSH
For a description of this LN, see IEC 61850-5.
LNName [The name shall be composed of the class name, the LN-Prefix and LN-

Instance-ID according to IEC 61850-7-2, Clause 22.
[Data objects
[Descriptions
|EEHealth [ENS _‘|Externalequipmentheaith J
|Optmh__|iNS__[Operationtime 0
(Measured and metered values
|React__———'[MV__ [Relative capacitance of bushing related to the data object RefReact__| [M_|
lAbsReact___—[MV___—*[Online capacitance, absolutevawe J
|tosFact_ MV Loss factor (tandetta)
\Vol_____|MV__[Voltage of bushing measuringtap
[DispiA___MV__[Displacement current: apparent current at measuring tap | fO_
|teakA_—|MV__[Leakage current: active current at measuringtap_ |
(Settings
Ferneact [ASG [Reerencecapactance fr bushing stammsonng——«dsYO_|
nue [as Rteronce power actor for bushing at conmissonng | o_|
fra [ps0 [Rtrnce vonage orbstingacommissonng —_—————~||o_|
5.18.5 LN: Power cable Name: ZCAB
For a description of this LN, see IEC 61850-5.
Data object
name
LNName [The name shall be composed of the class name, the LN-Prefix and LN-
Instance-ID according to IEC 61850-7-2, Clause 22.
a
an
https:/www.doc88.com/p-80980482981320.htm! 112/185
```


## File page 113

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
61850-7-4 © IEC:2010(E) -Wt-
[Data objects
|EEName _[DPL___ [External equipmentnameplate J
|EEHeaith ENS [External equipmentheaith J
5.18.6 LN: Capacitor bank Name: ZCAP
For a description of this LN, see IEC 61850-5.
Data obj

a
LNName [The name shall be composed of the class name, the LN-Prefix and LN-

Instance-ID according to IEC 61850-7-2, Clause 22.
[Data objects
|Health ENS [External equipment health
iso [ss [loses ue tosaenerwe ——SSSCS~S~S~S
[Contos
[Capps [SPC [Capacitor bank device status MY
5.18.7 LN: Converter Name: ZCON
For a description of this LN, see IEC 61850-5.
=| ere |

Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
|EEName _‘[DPL__[Externalequipmentnamepiate CJ
|EEHeath ENS [External equipmentheath 0
(Settings
[vaio [ASG [Raesbarecionalvas——SSSSCSCS~S~dC CS
vio (aso [ratevonage ——SSSCSC~C~S~S~S
5.18.8 LN: Generator Name: ZGEN
For a description of this LN, see IEC 61850-5. ZGEN has to be used for all generators not
modelled elsewhere in IEC 61850.
po |“ Eaeineniamen == [|

Instance-ID according to IEC 61850-7-2, Clause 22.

a
an
https:/www.doc88.com/p-80980482981320.htm! 113/185
```


## File page 114

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)

-112- 61850-7-4 © IEC:2010(E)
Data objects
|EEName _[DPL__[Externalequipmentnamepiate CJ
|EEHeath ENS [External equipmentheaith
[onotos _[S°5_[Opeaionstroione SSS
[RotDir [ENS —_|Phase rotation (Clockwise | Counter-Clockwise | Unknown) [ [|
[onune [ss operation at naerexetton ——SSSS«d
[Presaim __[sPs___[lowpressureaiarm
[Controts
jGncu__ OPC Generatorcontro
[Dex SPC [De-excitation,
|ReactPwR_ ___|SPC___—[Reactivepowerraise Cd
|ReactPw___|SPC__[Reactivepowerlower
[Measured and metered values
|Gnspd_ MV Generatorspeed CJ
Settings
Jomarer fas [Demandedpower = Cf
lPwrRig [as [Ratedpower
a Sc
5.18.9 LN: Gas insulated line Name: ZGIL
For a description of this LN, see IEC 61850-5.

Data object
name
— | Eirias" ||
Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
(Status information
|EEHeath ENS _[Externalequipmentheaith
[Optmh INS _[Operationtime
5.18.10 LN: Power overhead line Name: ZLIN
For a description of this LN, see IEC 61850-5. ZLIN represents an overhead line with all
physical characteristics.
LNName [The name shall be composed of the class name, the LN-Prefix and LN-
Instance-ID according to IEC 61850-7-2, Clause 22.
a
an
https:/www.doc88.com/p-80980482981320.htm! 114/185
```


## File page 115

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
61850-7-4 © IEC:2010(E) -113-
Data objects
|EEName _[DPL__[Externalequipmentnamepiate CJ
[EEHeatn JENS [External equipment heath
Settings
[Lintenkm fas [uinetengthinkm
[RPs_____[ASG__[Positive-sequence line esistance |
ps fas [Postve-sequence inereactance
[Azer [ASG [zero-sequencetneresistence tO
przer fas [Rerovsequence tine reactance fF
[zPsMag___[ASG_|Positive-sequence line impedance awe | 0
[zPsang [ASG ___[Postive-sequence tine impedance angie | JO
[zzermag (ASG __[Zero-sequence ine impedance vawe | O
[zzerang [ASG __[Zero-sequence ine impedance angie | 0
[Rmzer asc [Mtuatresistance
Prmzer [ASG [Mutvalreactance
lzmzermag [ASG [Mutual impedance vawe
lzmzerang [ASG [Mutual impedance angle
5.18.11 LN: Motor Name: ZMOT
For a description of this LN, see IEC 61850-5.
— | iewmneraisranr == [| |
Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
[EEName _[OPL_[Exteralequpmentname nae SSSCSCSC~dCYN
[EEHeetn JENS [External equipment heath
[boson SPS flossofot
ltcevec ses [loss otvacwm
[Presaim ——[sPS_[Lowpressure aim
[omtrots
[bea SPC [Derexcitation
5.18.12 LN: Reactor Name: ZREA
For a description of this LN, see IEC 61850-5.
Data obj
Per [cxf Te
LNName The name shall be composed of the class name, the LN-Prefix and LN-
Instance-ID according to IEC 61850-7-2, Clause 22.

ry

Ly

8

an

https://www.doc88.com/p-80980482981320.html 115/185
```


## File page 116

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)

-114- 61850-7-4 © IEC:2010(E)

Data objects

[EEName___]oPt__ External equipment name plete 0 fo

[EEHeekn JENS [External equipment heats

Settings

a

[Rg [AS [Rates ppurentpower SSS

varie ps6 [Raesnsivepower SSS

5.18.13 LN: Resistor Name: ZRES

Logical Node ZRES shall be used to represent a ohmic resistor. A typical application is the

resistor of the starpoint (a neutral resistor). The resistor is normally not controlled.

Data obj
Name’ dics) nnn
=| ere |
Instance-ID according to IEC 61850-7-2, Clause 22.

Data objects

IEeName __OPL__|Evtemalequpmentnamepaie SSCS

[EeHeatn ENS [Eemalequpmentheatm «SY

5.18.14 LN: Rotating reactive component Name: ZRRC

For a description of this LN, see IEC 61850-5.

| evenermnnmese [|

Instance-ID according to IEC 61850-7-2, Clause 22.

Data objects

[EEName __[OPL___[Enemnalequpmentnanepiae SSCS

[EEHeekh JENS ___[Extornal equipment heath To

[oekey [S05 local orremotekey SSS J

a

[toe |5°5[tecarconrorbehaviow SSCS

fanet__ JENS Feomporenttnte

[Comtrots

[enci—[b [Component contol wan. sop) SSCS

[Measured and metered value

[Gnseg_ wv eSepeeg

a
an
https://www.doc88.com/p-80980482981320.html 116/185
```


## File page 117

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
61850-7-4 © IEC:2010(E) —115-
5.18.15 LN: Surge arrestor Name: ZSAR
For a description of this LN, see IEC 61850-5.
LNName ‘The name shall be composed of the class name, the LN-Prefix and LN-
instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
[EEName __[OPL__[Exteralequpmentnane pie SSCid
[EEHeekn JENS ___[Extornal equipment heats
(onser [5S [operation otsurgearenor———SSSSSCS*~S~
5.18.16 LN: Semi-conductor controlled rectifier Name: ZSCR
Logical node ZSCR shall be used to represent a controllable rectifier. A typical use is to
provide the controllable d.c. current within an excitation system.
Data obj
|_Mrame" |dnwcuse)| mmm
LNName [The name shall be composed of the class name, the LN-Prefix and LN-
Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
[EENane __[OPL__[Esteralequpmentnane pate ——SSSCSCS~S~S
0
loptmn INS fOperatontime SSCS
fim 5°5 Joon onto arm
[Contros
lOpWocRect [ENG [Convelmade wong AWW) SSCS
lampSet____[APC___|Curenttagetsetpont SSCS
[voiset [aPC "WWatage target setpoint SSSC*dC
[Settings
[sem [ASG [Curent seting Woperang ww atnedeuren) «iY
[Sew [aS Votage seting Gf operating toatiedvotage) i |
and controllable voltage or have both current and voltage controllable. If either voltage or current is fixed, the set-
point shall be given as a setting.
5.18.17 LN: Synchronous machine Name: ZSMC
Logical Node ZSMC shall be used to represent any type of synchronous machine. The logical
node only includes rating data.
a
an
https:/www.doc88.com/p-80980482981320.htm! 117/185
```


## File page 118

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 1184 > @Q_~ View A mark Y Annotations ¥ Q)
—116- 61850-7-4 © IEC:2010(E)
= | Ree |
linstance-ID according to IEC 61850-7-2, Clause 22.

Data obiects
[EEName [PL [External equipment name plate To
|EEHeeth JENS ____[Extemalequipmentheath 00
rac |eNs__ [Rotational direction (ochvse |Couterclocwise[Urkrown) | (0 _|
[Settings
[Pwrftg [ASG [Rated apparent poworiva)
|v ASG Ratedvotaget}
Jato ASG [Ratdstatorcuremtta)
[Seartg [ASG [Synchronous machine rated speeds" | |
[Seacet _*[aSG__|Synchronous machine cial speedo the generaoris"] | [|
[FisRistme [ASG __[Reterence temperature for field resistance (usualyin’c] | JO.
[SttRistme [ASG ___[Retorence temperature for statorresistancousualyinC]_ | JO
[statris ASG [Statrvesistanceform) = fF
JPrrig [asc [Ratedpowertactor
liner [ASG [Synchronous mactine moment of inertia kam’ | FO.
[Fiaamprig [ASG Ratedteldcurent(a)
[Fevmprino [ASG Novoadtasicmentioraessaorotpei ————SSSSS=*diO |
rons (as ——(Petrossmncotoms «dO
lasso [a6 [Basoperwntinpecacotormibtass) «LO
[Sateax [ASG [Stmorieskageeacanceiperunt)————SSSSSCS~S~S~S
fa [ASG [Dau sycronousieocarce Xs lpruntl inward) «|_|
fo [a86 [Dds vaso sycrenous reactance er unt sata) | (0 |
a ce
fia [ASG Cran syetonous reactance Xap un sabres ————SSS~*dO
fe [AS [Oausranconreaciance Ya peru rsa) «dO _|
fs [a8 [Oanis sarin ear fer unit saad «tO _|
fo [a80_[2ooseqvence reactance x0 feruntwwaurans) «| O_|
fe ASG hapatvesemance reactance 2 prvi satan) ————~—SS—*dO |
Fintae [aS [Das sor creut vars te constant Ts sana ————+| (0 _|
Fras [aS [Oaie star crut subanin ine conta TE usatraed | (0 _|
[rmtaop [ASG |O-ais open creut wansiont te costant TaD slunsauraes) ————_—+| fo _|
[imtaos [ASG [Dau open creutav>-vason ine constant TA hires) | |_|
[inte [ASG [acs son cut ranson tie ort Ta slinsatrawesy ————+| 0 _|
Ftos __[AS6__[Oan shor cut subarintine conta Te ueanraed) | (O _|
[tmtetp [ASG [aus open creat varsion ime constant Ta slinsaurais) ————*| O_|
Feds [ASG [taxis open crt sub wansint time constant Teds vsatwaied) | JO _|
[inta ASG Amanve tneconant Talia ————SC*d |
[sucnsio [ase [Sourtencoeticen'sio «dO
[sucnsie [aso [Satratencootcen'si2——S«dO

e’

cy

=)

nw

https://www.doc88.com/p-8098048298 1 320.htm! 118/185
```


## File page 119

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 718 > @ Q_ View A mark Y Annotations v | Q
61850-7-4 © IEC:2010(E) -17-
5.18.18 LN: Thyristor controlled frequency converter Name: ZTCF
For a description of this LN, see IEC 61850-5.
a= _| aera =e |
Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
[EEName __[OPL___[Extemalequpment nameplate ————SSSSCSC~S~w ON
[EEHeenn JENS [Extemal oqupmentheaty
Settings
5.18.19 LN: Thyristor controlled reactive component Name: ZTCR
For a description of this LN, see IEC 61850-5.
Data object
name
— | Ears |
Instance-ID according to IEC 61850-7-2, Clause 22.
Data objects
a
[EEHeeth JENS Eomel oqipmentheety
lontm [WS JOperatontime CO
6 Data object name semantics
In Table 10, the data objects used in Clause 5 are described. The meaning of Boolean values
are FALSE = 0, TRUE = 1.
Table 10 - Description of data objects
Data object
name
A [Praseaurens unuany SSS
Gaal =i Seen
new condition.
[aan conset svasionwaring SSCS
[aveReact | Onine capaciance,absoutevake SSCS
[Acaim [AC supptytaiwre
[AccAbr | Comte abrason pate smpctiowear SSS
[accce | Accoraton (change orate ot equeneyaiownes) ——SSCS~=S
an
https:/www.doc88.com/p-80980482981320.htm! 119/185
```


## File page 120

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 718 > @ Q_ View A mark Y Annotations v | Q
—118- 61850-7-4 © IEC:2010(E)
Data object
Number of access control failures detected: a data object that the client wanted to access exists
in the server, but based on the access view of the association with that client, an access to the
data object was refused.
| ActAbr _| Abrasion of last open operation
|AcuPaDsch _| Acoustic level of partial discharge in db
Detuning of the compensated network. Either in A or in %.
'getune = 'EFN ‘ce in A
deen — |
Igetune = EN —CE- 100 in %
lee
| ADetunSpt _| Setpoint for the detuning of the suppression coil
Adjustment message
' ~ Cancel
Aaist 3— New adjustments
4 - Under way
‘Adaptation angle, used to modify the measured phase-angle difference by a fixed analogue value
in the range from -179...0...+180 DEG.
ODES
Oto Oto
+180 DEG -179 DEG
+180 DEG
‘AdpAngDeg _| This setting allows e.g. to compensate the setting group of a step-up transformer between CB
and VT (see example), or to compensate small phase shifts caused by harmonics on one of the
two voltages. Example:
OD:
©)
OD:
|AgeRte _| Ageing rate, for example of transformer
|AimstOv _| TRUE = indication that the alarm list has overflowed
‘Alarm vgi 0 is the pre-set value for a measurand that when reached will result in an alarm.
an
https://ww.doc88.com/p-80980482981320.html 120/185
```


## File page 121

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 718 > @ Q_ View A mark Y Annotations v | Q
61850-7-4 © IEC:2010(E) -119-
Data object
a
[ane ure of ronstreeghae creat
Current (sampled value) of the local current measurement (phase L1)
Current (sampled value) of the local current measurement (phase L2)
Current (sampled value) of the local current measurement (phase L3)
Current (sampled value) of the local current measurement (residual current)
[anesr [caret anpiavauey
Phase angle correction of a phasor (used for example for instrument transformers/transducers)
This data object indicates whether the phase-angle difference (between the two voltages to be
synchronised) is within the set limits or not. FALSE = value within the limits; TRUE = value
outside the limits.
‘Angle for load area. The following is an example of the definition of load encroachment used for
the data objects AngLod and RisLod with polygonal characteristic, applicable also with MHO.
POIS1, PDIS2, and PDIS3 are different instances of the LN PDIS, one for each zone. See also
RisGndRch in this table.
Forward
: +o
ee SS]
| \--——_--—__——_—, |
| Posey |
i ee nr a
i posi} ff
| al
e .., > i i
~~ \ Le) RisLod
. . 5
Loss encroachment =) = R
| f} Formas *
fF i -
| i
| i
| j
| |
i Reverse j
| ee Stine |
. 1ec 110409
[Ann __| Analogue input used for generic W/O. It can be multiple in one LN-instance.
[aso [eat ang cup Ha be mle none Nimans,
‘Access point name to which this channel belongs; only needed if more than one access point and
‘more than one physical channel exists.
[srt — [cont sere acting fe conolbe eau st pit wih fot command |
‘Apparent charge of partial discharge, peak level (PD)
[nccute ——[accouner reset
a
an
https://ww.doc88.com/p-80980482981320.html 121/185
```


## File page 122

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
-120- 61850-7-4 © IEC:2010(E)
Data object
Resonance-point of the compensated network. At this coil position of the Petersen-coil the
current through the coil compensates the whole capacitive current to ground of the network.
[ARtg _| Rated current, intrinsic property of the device, which cannot be setichanged from remote
[AStr _| Current level: if this level is exceeded, the related functions start a dedicated action
The total calculated area of a power quality event (example voltage sag in figure)
v
a : :
- a
1 ee 43010
Number of authorisation failures: an association to the client could not be established due to an
authorisation failure.
This data object is responsible for the enabling or disabling of the output circuit of the automatic
controller; automatic (TRUE) = output circuit is enabled, not automatic (FALSE) = output circuit is
disabled. This data object is controllable, but with ctiIModel="status_only” it can also be used.
‘AutoRecSt This data object represents whether or not the auto reclosing is ready, in progress, or successful
[Ready
[Successl__ TS
[Circuit breaker ciosed |
[Cycle unsuccessiid
[Unsuccessful 1
[Aborted
The state “In Progress” is only used for backwards compatibility with edition 1. It is deprecated for
edition2.
TRUE = automatic uploading of the disturbance recorder files
[AuxSco _| TRUE = commands change over to operation from the auxiliary power supply
‘AuxSwTmCls | Timing of the close operation measured by auxiliary switches (usually displayed in ms).
Significant changes in timing can point to a malfunction of the mechanical link, @.g. missing
lubricant.
‘AuxSwTmOpn__ | Timing of the open operation measured by auxiliary switches (usually displayed in ms).
Description see AuxswTmCls
[AvAmps __| Average current in a defined evaluation interval (period)
‘Arithmetic average of the magnitude of current of the 3 phases.
Averagegt 0 ¥,Ic)
a
https://ww.doc88.com/p-80980482981320.html 122/185
```


## File page 123

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 718 > @ Q_ View A mark Y Annotations v | Q
61850-7-4 © IEC:2010(E) -121-
TAVGIa | Muti cave character dtinon Wi a mali ante Winn one I tance |
(voltage) and y is the current (A). The integers representing the different curves are given in the
definition of COC CURVE in IEC 61850-7-3.
a acerca |
Average(PFa, PFb, PFc)
|
Average(PhVa, PhVb, PhVc)
ial == aaa
Average(PPVa, PPVb, PPVc)
[Avst____| Delivers the active curve characterise
a
Average(VAa, VAb, VAc)
Average(VAra, VArb, VArc)
[aware [Average votage ina defined evatatonineralpereg———SSSSC*d
[aww [Average sve power a Goliad wanton mena peg
ise |
Average(Wa, Wb, Wc)
[await | Wate art fhe eal cureta he faut acaion nase ofan eariauk =|
ieee |
Average(Za, Zb, Zc)
[Rbep | Yo asia paceman fay mm
[momsy —_[Tawaiaaapuconon CS
[AOAISpl [Adal epacement am iewisepand =
a
[eat [TRUE - nse at Baten overcharge condion =
[eato | TRUE  neates tat atery vonage nas Gapped below apresetiewi =
6’
cy
8
a
https://www.doc88.com/p-80980482981320. html 123/185
```


## File page 124

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
-122- 61850-7-4 © IEC:2010(E)
Data object
Since the logical device controls all logical nodes that are part of the logical device, the mode of
the logical device (“LOMode" = LLNO.Mod) and the mode of a specitic logical node (-LNMode" =
.000C.Mod) are related. The behaviour of a logical node is therefore a combination of LLNO.Mod
‘and XXXX.Mod and is described in the "LNBeh” = XXXX.Beh. This data object is read-only and
has the same possible values as Mod (Mode). The value is determined according the following
table:
LNMode bMode LNBeh (read only)
2000. Mod LUNO.Mod- 10000. Beh_
on on ‘on
on on-blocked | on-blocked
on test test
on test/blocked test/blocked
on oft off
on-blocked | on ‘on-blocked
con-blocked | on-blocked —_| on-blocked
con-blocked | test test/blocked
‘on-blocked | test/tlocked | test/blacked
con-blocked | off off
test on test
test on-blocked —_| test/blocked
test test test
test test/blocked test/blocked
test off off
test/bbcked | on test/blocked
test/biocked | on-blocked —_| test/blocked
test/biocked | test test/blocked
test/biocked | test/tlocked | test/blocked
oft on oft
ott on-blocked off
ott test off
ott test/blocked off
9 oft off
[ser beer ote communion chavo Usdin css oe al communication cane!
[Bik __| Dynamically blocking of function described by the LN
[BA __| TRUE = operation is blocked by current reasons
|BIkAOv _| TRUE = switch operation is blocked by current limit overflow
This data object is used to block ‘close operation’ (for example, for XCBR, XSWI, YPSH) from
another logical node such as a protection node or from a local/remote switch. An example may be
the low insulation gas density. Block closing is not reflected in operating capability. TRUE = block
operation ‘close circuit breaker’.
[BIkEF —_—_| TRUE = switch activity blocked due to earth fault
The tap changer is blocked due the low viscosity because of the low temperature.
[BikLV——_—_| Control voltage below which auto lower commands blocked
This data object is used to block ‘open operation’ (for example to XCBR, XSWI, YPSH) from
BikOpn another logical node such as a protection node or from a local/remote switch. An example may be
the blocking of the buscoupler also for trips during busbar transter. Block opening is not reflected
in operating capability. TRUE = block operation ‘open circuit breaker’.
Blocking reference shows the receiving of dynamically blocking signal; this data object is multi-
instantiable.
[BikRV —_| Control voltage above which auto raise commands blocked
Block closing command for circuit breaker because of thermal condition. If the temperature of
protected equipment is still higher than a setting.
TRUE = operation is blocked for voltage reasons
BikVal When the measurements exceed (or drop below, in the case of a dropout function) this value, the
function operation is blocked.
BikValA Block value (minimum operating current)
[Bikvaiv | Block valu (minimum operating voltage)
a
https://ww.doc88.com/p-80980482981320.html 124/185
```


## File page 125

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
61850-7-4 © IEC:2010(E) —123-
Data object

BIkVLo Control voltage below which auto raise commands are blocked. If the control voltage is under
the limit of BIKVLo (e.g. because that part of the network is switched off), the ATCC issues
no raise commands until the control voltage exceeds the limit of BIkVLo.

BIKVHI Control voltage above which auto lower commands are blocked. If the control voltage is over
the limit of BIKVHi, the ATCC issues no lower commands until the control voltage exceeds
the limit of BIkVHi.

TRUE = Switch operation is blocked by voltage limit overflow
This data object is used by the power swing protection to block operation of protection tor a
specific protection zone i.e. the related instance of PDIS.
TRUE = blocked, FALSE = not blocked
[Bndctr —__—_| Centre of control bandwidth, forward power flow presumed
Band centre change (raise, lower), no status
Band width, i.e. the defined range of control voltage given either as voltage value or percentage
of the nominal voltage. Forward power flow is presumed i applicable.
‘Access service tracking for buffered report control block
Control service tracking for binary controlled step position information|
[Capacimb _| Capacitive imbalance of the network
[Capds __| TRUE = Capacitor bank is on line, or close. FALSE = capacitor bank off line or open
[C2H2ppm _| Measurement of CoH (Acetylene) in ppm
[C2H4ppm —_| Measurement of CoH, (Ethylene) in ppm
C2Héppm Measurement of Cog (Ethane) in ppm
[CarLev _| Power of received signal in case of an analogue communication channel

CBOpCap This is an enumeration representing the physical capabilities of the breaker to operate. It reflects
the switching energy as well as additional blocking due to some local problems.

CBOpCap is always less or equal to MaxOpCap.

[None

[Open

[Close—Open

[more TT
More values (6...n) describe higher operating capabilities. A new value, that is a new line in the
table, must start alternating with “close” and “open” and must end always with “open”
Closing time of breaker in ms. The time is used to compensate for the breaker closing time, i.e.,
the closing command will be given a defined time before phase coincidence. Closing time of
breaker including other delays until the operation of the breaker. This is a property of the breaker
that is subject to ageing.

Closing Phase

command coincidence
CBTmms
Closing
time
ec 43010
CBTmms Closing time of breaker including other delays until the operation of the breaker. This is a
Property of the breaker that is subject to ageing.
[cca __—_| Carbon production credit value
Control of automatic / manual operation (blocking). TRUE = automatic control of cooling
equipmeg’ ; ‘ocked (inhibited)
nw
https://ww.doc88.com/p-80980482981320.html 125/185
```


## File page 126

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 718 > @ Q_ View A mark Y Annotations v | Q
—124- 61850-7-4 © IEC:2010(E)
Data object
cect Control of complete cooling group (pumps and fans). TRUE = on, FALSE = off
CETmpin ‘Temperature of the behaviour cooling medium in a cooling equipment (input). Typically used for
the water temperature for water cooled power transformers (OFWF or ODWF)..
CETmpOut —_| Temperature of the behaviour cooling medium in a cooling equipment (output). Typically used for
the water temperature for water cooled power transformers (OFWF or ODWF).
Pressure of the behaviour cooling medium in a cooling equipment. Typically used for the water
pressure for water cooled power transformers (OFWF or ODWF).
Flow of the behaviour cooling medium in a cooling equipment. Typically used for the water flow
for water cooled power transformers (OFWF or ODWF).
[CGAim —_| TRUE = core ground alarm indicates that the insulation has broken down
[CH4ppm —_| Measurement of CHg (methane) in ppm
Time interval between last two charging operations
This data object is used to enable the charger motor; used to prevent overload of the power
supply atter a busbar trip. TRUE = enable charger motor, FALSE = disable charger motor
[Chtiv __| Physical channel status; true, if channel receives telegrams within a specified time interval
Timeout time for channel live supervision; default 5 s
[ChNum ___| Channel number being monitored (for example for COMTRADE)
The angle by which the current is displaced from the polarising quantity in order to obtain
maximum sensitivity.
‘Channel triggered. TRUE = channel started recording, FALSE = behaviour not started recording
Circa Measured circulating current, which circulates between transformers operated in parallel (one
‘component of transformer secondary current in a paralleling installation).
Indicates that the calculation period of a statistical logical node has expired.
This DATA OBJECTS shall be mandatory for all logical nodes that are intended to represent
statistical data object, indicated by the common data classes, for example, CDC MV, CMV, WYE,
etc.
CleintvT} Calculation interval type with possible values MS | PER_CYCLE | CYCLE | DAY | WEEK |
hd MONTH | YEAR | EXTERNAL
ry
an
https://www.doc88.com/p-80980482981320.html 126/185
```


## File page 127

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 718 > @ Q_ View A mark Y Annotations v | Q
61850-7-4 © IEC:2010(E) — 125 -
Data object
The calculation method specifies how the data attributes that represent analogue values have
been calculated. The calculation method shall be the same for all data objects of a given logical
node instance.
The possible values shall be:
[veue———[Owspion
Indicates that the calculation of the analogue values is unspecified
(ie. all common attributes | and 1). UNSPECIFIED is the default value.
TRUE_RMS Indicates that all analogue values (i.e. all common attributes | and f)
are true r.m.s. values.
PEAK FUNDA | indicates that all analogue values (i.e. all common attributes | and f)
MENTAL are peak fundamental values.
RMS_FUNDAM | indicates that all analogue valves (i.e. all common attributes | and f)
ENTAL are r.m.s. fundamental values.
Indicates that all analogue values (i.e. all common attributes i and f)
are minimum values.
Indicates that all analogue values (i.e. all common attributes | and f)
are maximum values.
Indicates that all analogue values (i. all common attributes | and f)
are average values.
SDV Indicates that all analogue values (i.e. all common attributes | and f)
are standard deviation values.
PREDICTION | indicates that all analogue valves (i.e. all common attributes | and f)
are long term changes over time.
Indicates that all analogue values (i.e. all common attributes i and f)
are actual changes over time calculated with the actual value and
value betore.
This DATA OBJECT shall be mandatory for all logical nodes that are intended to represent
statistical data, indicated by the common data classes, for example, CDC MV, CMV, WYE, etc.
NOTE 1 If different calculation periods are required for the data objects of a logical node, then
different logical nodes could be instantiated ~ with different calculation periods.
NOTE 2 The calculation algorithm and number of samples used for the calculation is an
implementation issue.
Calculation mode
Possible values are:
CC
TOTAL the total time from the first start of the device/application until the
current time
the periodical time cycle
[ SLIDING —__| sliding window trom now predefined window backwards
In case CicintvTyp equals to MS, PER-CYCLE, CYCLE, DAY, WEEK, MONTH, YEAR, number of
units to consider to calculate the calculation interval duration.
Remaining time up to the end of the current calculation interval - expressed in milliseconds
In case GicintvTyp equals to MS, PER-CYCLE, CYCLE, DAY, WEEK, MONTH, YEAR, number of
Units to consider to calculate the refreshment interval duration.
Refreshment interval typ. Allowed values: MS, PER-CYCLE, CYCLE, DAY, WEEK, MONTH,
YEAR, EXTERNAL
[aeeawn | Womb of eacuiion sauce ecededinatumatomansai moe |
The reference to the logical node whose analogue data attributes are used to calculate the value
contained in this logical node instance.
This DATA OBJECT shall be mandatory for all logical nodes that are intended to represent
statistical data, indicated by the common data classes, for example, CDC MV, CMV, WYE, etc.
ry
nw
https://ww.doc88.com/p-80980482981320.html 127/185
```


## File page 128

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 718 > @ Q_ View A mark Y Annotations v | Q
— 126 - 61850-7-4 © IEC:2010(E)
Enables the start calculation of statistical data. Either at once, or if available and set at operTm
of the control model.
o~  Eetreiarrammntsearersee caren |
statistical data, indicated by the common data classes, for example, CDC MV, CMV, WYE, etc.
CycTrMod This data object represents a type of trip function from RREC; Sphase means only Sphase
tripping possible, 1 or 3phase means PTAC with 1 and 3phase tripping possibility and first trip
depending on fault type. Specific means for example PTAC with 1phase and 2phase and 3phase
tripping possibilty and first trip depending on fault type.
—_—s
[spectic
[cistim [Limitation on closing of a device (% of fullopening) |
[cioudcwr | loudcoverteve
Joma __| Breaker closing command: TRUE issue-a comma-d FALSE issue no command |
[ons [Counter resettle)
|cozems [co,emissions
|cozppm [Measurement of CO, inppm
}corms [coemissions
[oak [Gartner or cnn =n I pon Gal nen)
[cotati Peon poner nino we eee
= |
Petersen coil
— a |
and converted as current in A for the case of a solid earthfault.
[entender von be te GO0SE news |
[cndct [Electrical conductivity of water in Siem?
Jom [Commas
a
ree
es
[este [Oey ie ne wo linn Teaco ww eter |
ow [cue shape
——e oc
[ere monecincanenvasng
[orct | Derivatwe action
[Damp [Damping of the zero-sequence-system |
[Detinsot___| Direct norr-I insolation [usually in W/m*}
cy
8
nw
https://www.doc88.com/p-8098048298 1 320.htm! 128/185
```


## File page 129

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 718 > @ Q_ View A mark Y Annotations v | Q
61850-7-4 © IEC:2010(E) -127-
Data object
Desens
|DeaBusVal _| Dead bus value: maximum value at which the bus voltage is still recognised as zero (dead)
Dead line value: maximum value at which the line voltage is still recognised as zero (dead)
[Den __| Density of insulating medium
|DenAim _| Density alarm because of an abnormal condition (FALSE = Normal, TRUE = alert)
Detection of synchronism (4): maximum value at which the frequency difference is ignored and
the two voltages are treated as synchronous sources
| DetvaiA _| Used to detect that the breaker has opened when the current is below that setting
[DExt ——_| TRUE = Command to de-excite the machine
| Ditinso! __—_| Diffuse insolation (usually in Wim*}
[Diag __—_| TRUE = diagnostic is running, FALSE = diagnostic is not running
When the voltage in at least one phase goes below the voltage dip set point, it will start the
DipSeVal voltage variation function and the timer that will measure the duration of the voltage variation
power quality event. The event ends when all monitored phase voltages return above the
threshold.
|DitAng _| Setting for the phase angle difference between two measured values
Calculated vaive for the phase-angle difference between two measured values. The accuracy and
calculation method is a local issue.
|DitAngNg _| Maximum phase-angle difference negative: absolute value in degrees
‘Maximum phase-angle difference positive: absolute value in degrees; this data object may be
used for both, the positive and negative limit in case the synchronizer supports only a common
parameter for positive and negative.
[Diz _| Setting for the frequency difference between two measured values
Calculated value for the frequency difference between two measured values. The accuracy and
Calculation method is a local issue.
|Dit#zNg _| Difference frequency negative: absolute value
Difference frequency positive; absolute value; this data object may be used for both, the positive
and negative limit in case the synchronizer supports only a common parameter for positive and
negative,
| DifPresHi _| Differential pressure high alarm level setting
[Ditv _| Setting for the voltage difference between two measured values
Calculated valve for the voltage difference (average value) between two measured values. The
accuracy and calculation method is a local issue.
[Di'VNg __| Maximum voltage difference (average value) negative
‘Maximum voltage difference (average value) positive; this data object may be used for both, the
positive and negative limit in case the synchronizer supports only a common parameter for
positive and negative.
[oe [te ection ot tut or pemerfow
This data object is used to enable operation when the following directional conditions are met:
[____Directionmode___|__Value__|
[Non directional
[Forward
[Reverse
| DiDur —_—_| Daylight duration (time elapsed between sunrise and sunset)
TRUE = delete the selected record
ry
an
https://ww.doc88.com/p-80980482981320.html 129/185
```


## File page 130

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 718 > @ Q_ View A mark Y Annotations v | Q
— 128 - 61850-7-4 © IEC:2010(E)
Data object
Supervision time for paralleling in ms; in case of synchronous conditions (no phase coincidence),
DiTmms the closing command may be given if all conditions have been fulfilled continuously during the
supervision time.
[Baar [onmandeg per
|Dmavarh _| Reactive energy demand (default demand direction: energy flow from busbar away)
|Dmawh _| Real energy demand (default demand direction: energy flow from busbar away)
[Dn __—_| Last count direction downward
Generic double point control. It can be multiple in one LN-instance.
| DpeTrk _| Control service tracking for controllable double point
Direct, quadrature, and zero axis quantity
o Droop controls allow the use of distributed controllers without inter-controller communication. The
roop droop is specified as percent change in effective setpoint at maximum action.
Dropout value for blocking closing command
TRUE = indicates that switch close action for capacitor bank is blocked due to the discharge state
of the bank
[Brame _[Detwaweume
DiSyntmms __| Time delay between receiving the start synchronising signal and starting the synchronising
iv process. This time is used to set up the actual values, before any interaction takes place.
Minimum duration of carrier signal sent by a communication based scheme in ms.
[oust [owsiparices swpercedinar STS
[EchoWei __| TxPrm is being sent as echo signal or in case of weak end infeed
Echoweidp __| Additional indication that Op is the operate from the weak end infeed or echo function (typically
with undervoltage control).
This information reflects the state of external equipment, for example circuit breaker controlled by
the logical node XCBR. The values are the same as for the health.
This information retlects the name plate of external equipment, for example the circuit breaker
XCBR controlled by the logical node CSWI
Energy available in the drive mechanism expressed in %, where 100 % corresponds to rated
value and 0 % to lowest block value
TRUE = supervision has detected an abnormal condition in the energy storing system, for
example loss of N2 or equivalent.
The interlocking function itself determines the status of this data object and thus permits the
closing of the device when TRUE. The control service checks this value before he controls
“Close/On" a switch.
The interlocking function itself determines the status of this data object and thus permits the
opening of the device when TRUE. The control service checks this value before he controls
*Open/Ott” a switch.
|EnBik _| TRUE = energy is too low for operation.
EncTrk Control service tracking for enumerated controllable. One EncTrk instance is dedicated to track one
Specific enumerated command. Therefore EncTrk can be multiple instances in one LN instance.
TRUE = load tap changer is in the maximum lower position
EndPosR TRUE = load tap changer is in the maximum raise position
|EnvHum —_| Humidity of environment (usually in %)
[Envree | euromerie restr of environment SSC
Temperature of environment
EqTmm Temperature equalisation time (min). For the duration of EqTmm, the thermal memory will be
kept, that is the thermal memory is frozen. This time is active after the motor is switched off.
|ErPar __| Error of parallel operation of transformer
ry
an
https://ww.doc88.com/p-80980482981320.html 130/185
```


## File page 131

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
61850-7-4 © IEC:2010(E) —129-
Data object
ErTerm Control loop termination error value; the function cannot be fulfilled because of external reasons;
the value gives difference between set point and actual value.
Event counter — counts the number of times that a power quality event detected by the logical
node occurred
Evalvation time in ms (time window) determines the lowest frequency
|FACntRs _| Fault arc counter, resettable
[FADet __| TRUE = alarm that fault arc has been detected
Circuit breaker failure detection mode.
[_____Detectionmode | Value _|
[Curent
[Breaker status
| 3
other
|FanA __| Motor drive current of a fan in A
FanCtiGen ~ Control of all fans
FanCt! - Control of a single tan
[___Fancontrol__—— | Value __|
finactve
More stages may be added with numbers greater than 4
|FctBik —_—_| Dynamically blocking of function described by the LN
[ranrer | okingretrence show isis hat Backs he Wncion has been eonveg
Frame error rate on this channel; count of erroneous (or missed, in case of redundancy)
messages for each 1 000 messages forwarded to the application
|FilAim _| Filter alarm, can be indicated by too high or too low differential pressure
Filter type: Low pass | High pass | Bandpass | Bandstop (notch) | Deadband
[reno | ahcomerradng
Size of the external Fixcoll. The ANCR can take into account this value for the calculation of the
necessary compensation. The size is given in Amperes and represents the current through the
coil in case of a solid earth fault.
Pas coe
No-load field current for rated stator voltage [A]
|FidRis __—_| Field resistance (ohm)
Relerence temperature for field resistance [usually in °C}
[ridasn |The dence oaautinaw SSS
[runata——eautindsaion. setae
ry
an
https://ww.doc88.com/p-80980482981320.html 131/185
```


## File page 132

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
— 130 - 61850-7-4 © IEC:2010(E)
Data object
[Phase Ctoground TS
[Phase AtophaseB Te
[PhaseBtophaseC TS
[Phase CtophaseA
[Others
[FitNum —__| Fault number (number allocation is local issue)
[rush Fermsnnginpogess SSS
Filter flushing counter (resettable)
|Flw _| Flow rate of water or other liquid [usually in m°/s]
|FuFail __| TRUE = indicates that the TVTR fuse has openeditailed
Insulation liquid (for example oil) flow trip because of gas (maybe used for Buchholz trip)
Gas in insulation liquid (tor example oil) alarm because of an abnormal condition (FALSE =
normal, TRUE « alert, may be used for Buchholz trip)
Gas in insulation liquid trip because of a dangerous condition (maybe used for Buchholz trip)
GenTrk ‘Common service tracking for all services for which no specific tracking data exists. The supported
trackable services by GenTrk are a local issue.
GndDIMod Operate time delay for single-phase ground mode. TRUE = on, FALSE = off
[GndDitmms | Operate time delay for single-phase ground faults in ms
Ground operate value (3 io)
When the ground measurements exceed this value (or drop below, in the case of a dropout
function), the operation of the related function is initiated.
Generator state
[____Generatorstate | Value _|
[Stated
[Staring
[Disabled TS
Reference to the subscribed GOOSE control block
GocbTrk ‘Access service tracking for GOOSE control block
This data object summarises different alarms, assigned via configuration. TRUE = indicates a
group alarm
This data object summarises different indications, assigned via configuration. TRUE indicates a
group indication
[cntag _[Greentsginomaion SSCS
Reference to a higher-level logical device (LD). The Mod of this higher-level (referenced) LD
influences the Beh of the LD and the Beh of all LNs in the LD where the GrRef is contained (see
Beh of Table 10). See also the concept of “Logical device management hierarchy" described in
IEC 61850-7-1.
GrdRxCmdRx | Alarm situation: Guard received together with the command, may indicate interference on the
channel. Used in case of an analogue communication channel.
Grid fault number is used for identification of disturbance records of a common fault (number
allocation is local issue).
a
an
https://www.doc88.com/p-80980482981320.html 132/185
```


## File page 133

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 718 > @ Q_ View A mark Y Annotations v | Q
61850-7-4 © IEC:2010(E) -131-
ne |
warning
|
amount of deterioration of the insulation system
[anim ip lr orgs compostion (FALSE = Norma TRUE =a) |
[raw [Hp waring rote componton
all
with HyOTmp.
Note that this is a measurement used in conjunction with HO
[ik | Poses sequence of amon or mhamonc cure forA BG.N Net Rea |
[Hamp |Nonphae-olted sequin of harmonics ornwhamans curent =|
[naciane | Norpase roid cuenta sin pak wart vauwsgioaanena) |
[Hanract _|NonphasevoategKracor Cid
[nahmanp | No-paseolied cure RS harmon onramone nsomaimea Tig) |
[nahavot | Norpase rod vonage AMS harmon of mhamoneonnemaleed Tha)
[rans [Number of te namoneatinbenamontoestermerant |
[raver | Nonphareveledrequnce of armeieartenamonsvotage |
[Havonpr | Nor phased squire of harmonics ornehamanis recive power —————_—|
[Hawa |Nonphae-lte squire of harmonics ornhamanis asive power =|
[nom | Phased cuenta (peak waveform vave'a(@yindanenia) |
[ncrpev | Paseo pase otae ce cnr pak wavlrm vans 2vondanent)
This information reflects the state of the logical node related HW and SW. More detailed
information related to the source of the problem may be provided by specific data objects. For
LLNO, this data object reflects the worst value of “health” of the logical nodes that are part of the
logical device associated with LLNO.
[___ Health state | Value _|
==
Se ||
Health states 1 ("green") and 3 ("red") are unambiguous by definition. The detailed meaning of
Health state 2 (‘yellow’) is a local issue depending from the dedicated function/device.
[Heataim [TRUE = supervision has detected an abnormal state of the heater |
[iBatvat [High battery tarmvawe
[ncaPos | non oston ots suppessonsa——SSSSSCS—S
[Hicw | host contol votagesneolasteset SSS
[HiDmdA —_[ Highest ci o »nt demand since last reset
Ly
8
an
https://www.doc88.com/p-80980482981320.html 133/185
```


## File page 134

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 718 > @ Q_ View A mark Y Annotations v | Q
-132- 61850-7-4 © IEC:2010(E)
in [ah it ses siete
fines |Wah int epont
[se | ah ope abe porcnge ote sonnatcurew
[nat] Pasoroate ucortorA. 6
a
[NerweDy ——[Worzonalwnd diecton
[wenv —[seqne a amon or whamons pata pound oagne AN BN OH NO |
[nervy [Segre tamer nharmenc or hae io pao wtage AB, 86,ch |
Rarer ern |
Thd) for A, B, C, N
oh TRE |
[anarPv [Pras 1 phase vata AUS harmon or amon [urrmalzed Tha for AB BO, CA
[nto |r entes tantorer aig cori 8.6 |
(Sal aeniceaiiaeenianandabaesiell
sum for A, 8, C
[iva [Pasta sequence of arn o interme appre power
[war |e ena sequence ot arene entamonieecve power fr, 8,0 |
i Eee ere |
control operation is initiated.
[iw Tene seine of rons ainarmencs ave oweriorA 86 |
(a
Po [Stee a ernro|
and 0 % to block value
a
fhe [Th eueey of apowegteminiie
ee
voltage or running voltage.
acc Frequent 2 i eer 859
— S|
synchronizer. TRUE = ON; FALSE = OFF
Ce a
controller). The value is 1/s.
~ See
synchronised) is within the set limits or not. FALSE = value within the limits; TRUE = value
outside the limits.
[HzChr __| Frequency adjustment characteristic
ry
Ly
8
nw
https://www.doc88.com/p-8098048298 1 320.htm! 134/185,
```


## File page 135

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 718 > @ Q_ View A mark Y Annotations v | Q
61850-7-4 © IEC:2010(E) — 133 —
Data object
[HzRtg _| Rated frequency, intrinsic property of the device, which cannot be set/changed from remote
[reset —|setipctatequney
‘Since frequency difference Af and phase angle difference a are depending on each other (da/dt ~
‘af, the phase angle difference becomes constant in case of Af = 0. Therefore, the target value of
the frequency matcher is usually not 0. This data object allows to adjust manually the target value
of the frequency matcher.
|H2vaMag _| Frequency variation magnitude of the last completed event
Frequency variation duration of the last completed event
tuim Anti-windup integral limit. This parameter limits the absolute value of the integral term in case the
loop is temporarily broken.
Deviation from the average phase current.
ImbA.phsX = Vtx-4avel with fave = (1/3) (In + be + Ic)
|imbNgA _| Current imbalance negative sequence method. ImbNgA = I /
[imbNgV _| Voltage imbalance negative sequence method. ImbNgV = V, / V,
Deviation from the average phase-to-phase voltage.
ImbPPV.phsXY = | Viy — PPVave| with PPVave = (1/3) x (Va + Voc + Ves)
_ Deviation trom the average phase-to-neutral voltage.
ImbV.phsX = | Vx ~ Vet with Vave = (1/3) x (Van + Von + Ven)
[imbZroA _| Current imbalance zero sequence method. ImbZroA = Ip! |
|imbZrov _| Voltage imbalance zero sequence method. ImbZroV = V,/ V,
[ine | mbar of asscinane wai ie omaciay SCS
Control service tracking for controllable integer
[ind __| General indication. It can be multiple in one LN-instance
[incr _| Increment of position change for open / close commands
|iner __—_—_| Synchronous machine moment of inertia J [kgm]
Voltage interruption detection method is the method used to detect the interruption condition
based on measured or calculated voltages, currents or the status of the breaker auxiliary
contacts.
|__Voltage interruption detection method _| Value _|
[Voltage
[Voltage andcurrent___ dT 2
[Voltage and normally open breaker contact__| 3 |
Voltage and normally closed breaker contact_ | 4 |
Votege endbodi nomaly pena nomealy | 6 |
|closed breaker contacts
[Normally open breaker contact____ |_|
[Normally closed breaker contacts __ | 7
Ben normaly open ardrematy coved [8 |
[breaker contacts
lates Time setting for restart inhibition (min). Once the Strinh is activated, the motor should not be
allowed to start until this time has elapsed.
[nn Retoroncn oot cctorwcheing SS
[inov ___| Input communications butter overtiow
[inet __| Reference to the data object what is binded to this input
Reference to trigger for archiving
TRUE = provides an alarm after a pre-set limit is reached, for example low insulation level.
Setting of the limits is a local issue and depends on the supervised media property. An
appropriate action may be to refill the insulation medium.
o
an
https://ww.doc88.com/p-80980482981320.html 135/185
```


## File page 136

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 718 > @ Q_ View A mark Y Annotations v | Q
— 134 - 61850-7-4 © IEC:2010(E)
Data object
TRUE = block the operation of the isolated device when the level is reached where operation is
not safe anymore. Setting of the limits is a local issue and depends on the supervised media
property.
TRUE = Insulation medium level has reached predetermined maximum level, mainly used for the
filling process
TRUE = Insulation medium level has dropped to a predetermined minimum level, mainly used for
the filing process
TRUE = The insulation of the device is not guaranteed anymore. The device has to switch off
Instr from the power system, i.e. it has to be isolated by tripping the surrounding breakers. Setting of
the limits is a local issue and depends on the supervised media property.
Object reference to the source of the external synchronization signal for the calculation interval
[intrStrval___| The voltage interruption set point. When the measured voltage goes below this value.
[intin _| Integer status input used for generic VO
[iscso__| Generic integer control output. It can be multiple in one LN-instance
[IscTrk __| Control service tracking for integer controis step position information
, is zero sequence compensation factor = (2, ~ Z,)/3Z, where Z, is zero sequence impedance,
and Z, is positive sequence impedance
KOFactAng _| Residual compensation factor angle for K,
‘The kicker pulse is a kind of frequency adjusting pulse, that may be issued if frequency difference
is very small in order to achieve phase coincidence between the two voltages. With this data
object, the kicker pulse function may be switched on or off. TRUE = ON; FALSE = OFF.
[eis |r canttieag
[kis Fitercontriing
[kp |Proponenaigan
Lower arc suppression coll position (Petersen coil)
[LaststNum _| Last state number of received GOOSE telegram
Line drop compensation. LDC is R&X or Z model TRUE = R&X, FALSE = Z
Line drop voltage due to line resistance component (FPF presumed) at rated current
Line drop voltage due to line reactance component (FPF presumed) at rated current
Line drop voltage due to line total impedance (FPF presumed) at rated current
[Leone [Resta gh omting ose, ws caaneantiooseu
[Lev __—_—_| Level of insulating medium or water level {m]
[LevPct _| Level in the tank (as percentage of full tank level)
LevMod Internal trigger mode for disturbance recording.
a
[Negative orfalling
aS a |
a ee
The disturbance recorder trigger mode is defined by TrgMod.
LevMod exists both for the disturbance recorder as a whole (RDRE) and for each of its individual
channels (RADR, RBDR). The interaction of both is determined by the individual disturbance
recorder.
TRUE = lower frequency, FALSE = no action
Current limit for overtiow blocking
The data object LodA current (percent) above which automatic commands suspended
a
an
https://ww.doc88.com/p-80980482981320.html 136/185
```


## File page 137

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 718 > @ Q_ View A mark Y Annotations v | Q
61850-7-4 © IEC:2010(E) - 135 -
[tinang tr ange ite fendrine impedance ae
[unten [The motte wine
[Lneuavar | Voge sting vedo dette bus fr example for auto ecsng
LivDeaMod —_| Live dead mode of operation under which switching may be carried out

[___Livedead mode |__ Value __|

a

[Live line, deadbus 2

[Deadiine. tive bus

aa a

Esseo

Se

Live line, dead bus OR

[Dead line, live bus
|toBatval [Low battery alarm vate
ee ere a

Lockey toggle switch may have a set of contacts from which the position can be read. This data object

indicates the switchover between local and remote operation; local = TRUE, remote = FALSE.
= paiemnerene em |

level, TRUE = allowed at this level). (See also Annex 8).

Locsta Control authority at station level (see Loc). Switch between station and higher level. TRUE =
= Sareea |
[LegFact | oad taco apparent power reed powe) ofa vaeiomer —SSSCSC=S
[LoaRevaim [Loadreserve totam
|togRet __[Reterencetolog
[Lotim [Low timit reached input signal equal to or below limit)
[Louimset [Minimum imi setpoint
[taser [Low operate value, percentage ofthe nominal eurent
[LosFact __[Losstactor(tan deta)
|toso__| TRUE = indicates that a loss of olhas been detected.
[tosvac | TRUE = indicates when vacuum drops below a predeterminedievel |
[Lotratev [Low (negative) tiggertevel
[Lovrtg __[Ratedvottage ow voltage lave)
TRUE = bieck (inhibit) automatic control of LTC blocked (inhibited)

Ly
8
nw
https://ww.doc88.com/p-80980482981320.html 137/185
```


## File page 138

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 718 > @ Q_ View A mark Y Annotations v | Q
— 136 — 61850-7-4 © IEC:2010(E)
Data object

LTCDraghs _| TRUE = reset LTC drag hands (high and low positions to present position)

TRUE = lower voltage, FALSE = no action

| MaxAmps _| Maximum current in a defined evaluation interval (period)
Maximum magnitude of current of the 3 phases.
Max(la,{b,le)

[MaxCyc __| Maximum number of allowed cycles for any cyclic process, e.g. used for the autorecloser

Operation instant difference (between intended and performed operation)
Monitoring of current exceeding a set value is enabled (TRUE) in order to detect a fault condition
during power swing in the system

| MaxFwdAng | Maximum phase angle in forward direction

MaxHzTmms | Maximum frequency adjustment pulse time. The value is used to define a maximum pulse length,
limiting the calculated variable pulse time.
‘Maximum deviation from the average current.
Max(Idev_a,ldev_b,Idev_c)
‘Maximum deviation from the average phase-to-phase voltage.
MaximbPPV = Max(PPVdev_a,PPVdev_b.PPVdev_c)
Maximum deviation from the average phase-to-neutral voltage.
MaximbV = Max(Vdev_a,Vdev_b,Vdev_c)

|MaxNumRicd —_ | Maximum number of records that can be recorded
Setting for the maximum number of starts. This data object is also used for the permissible
‘number of cold starts. For example, the motor manufacturer may state that three starts at the
maximum are allowed within 1 h. These parameters are intended for this. So MaxNumStr is set to
3 and MaxStrTmm is set to 60 (min).
This data object shall provide the information of the operation capability available when the switch
mechanism is fully charged. The maximum operating capability gives the information about the
‘max number of close-open cycles to be performed. For example when one Close-Open-cycle can
be performed then MaxOpCap equals 1.
The data object maximum operating time in ms for the LN is used for co-ordinating action of the
felated function
‘Maximum magnitude of power factor of the 3 phases.
Max(PFa, PFD, PFc)

MaxPhVPhs | Maximum magnitude of phase to reference voltage of the 3 phases.
Max(PhVa, PhVb, PhVc)
‘Maximum magnitude of phase to phase voltage of the 3 phases.
Max(PPVa, PPVb, PPVc)

|MaxPwr _| Maximum permissible permanent power (overload) (W]

|MaxStrTmm _ | The time period in which the maximum number of starts is allowed

| MaxTmms _| Maximum time after fault detection during which autoreclosing is permitted

[MaxVA _| Maximum apparent power in a defined evaluation interval (period)
‘Maximum magnitude of apparent power of the 3 phases.
Max(VAa, VAb, VAc)

‘Maximum reactive power in a defined evaluation interval (period)
Maximum magnitude of reactive power of the 3 phases.
Max(VAra, VArb, VArc)

[Maxvoits —_| ‘Maximum voltage in a defined evaluation interval (period)

‘Maximum voltage for live synchronisation
Maximum voltage adjustment pulse time: the value is used to define a maximum pulse length,
limiting jf 0 alculated variable pulse time

an
https:/mww.doc88.com/p-8098048298 1320.html 138/185
```


## File page 139

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 718 > @ Q_ View A mark Y Annotations v | Q
61850-7-4 © IEC:2010(E) - 137 -
Data object
[MaxvVa__| Maximum unbalance deviation value
[Maxw _| Maximum active power in a defined evaluation interval (period)
‘Maximum magnitude of active power of the 3 phases.
Max(Wa, Wb, We)
| Maxwrmstr _| Permissible number of warm starts, in most cases cold starts - 1
‘Maximum magnitude of impedance of the 3 phases.
Max(Za, Zb, Zc)
[MbrAim _| Leakage supervision alarm of tank conservator membrane
‘Supervision has detected an abnormal condition of the mechanical chain, derived for example
trom travel curve or operating times. The values are the same as for the health.
| MemFull_ __| This data object is the percentage at which to indicate memory is full.
|Memov | TRUE = memory overflow has occurred
|MemRs _| TRUE = resetting the memory in the recorder
|MemUsed _| Percentage of storage memory in use
| MinAmps __| Minimum current in a defined evaluation interval (period)
Minimum magnitude of current of the 3 phases.
Min(la,tb,lc)
| MinFwdAng | Minimum phase angle in forward direction
MinH2Tmms | Minimum frequency adjustment pulse time: the value is used to define a minimum pulse length,
limiting the calculated variable pulse time. In case of constant pulse length, this value may be
used to define the pulse length.
MinOpTmms __| The data object minimum operating time in ms for the LN is used for co-ordinating with older
electromechanical relays
Minimum magnitude of power factor of the 3 phases.
Min(PFa, PFD, PFc)
Minimum magnitude of phase to reference voltage of the 3 phases.
Min(PhVa, PhVb, PhVc)
MinPPV Minimum phase to phase voltage
Minimum magnitude of phase to phase voltage of the 3 phases.
MinPPVPhs | win(PPVa, PPVb, PPVc)
|MinRvAng __| Minimum phase angle in reverse direction
[MinVA __| Minimum apparent power in a defined evaluation interval (period).
Minimum magnitude of apparent power of the 3 phases.
Min(VAra, VArb, VArc)
|MinVAr ___| Minimum reactive power in a defined evaluation interval (period)
Minimum magnitude of reactive power of the 3 phases.
Min(VAra, VArb, VArc)
| Minvolts _| Minimum voltage in a defined evaluation interval (period)
[Mevsyn [Minimum vonage rive syrevonsaion
MinVTmms. Minimum voltage adjustment pulse time: the value is used to define a minimum pulse length,
limiting the calculated variable pulse time. In case of constant pulse length, this value may be
used to define the pulse length.
[MinW __| Minimum real power in a defined evaluation interval (period)
Minimum magnitude of active power of the 3 phases.
Min(Wa, Wb, We)
Minimum magnitude of impedance of the 3 phases.
Min(Za, 2b, Zc)
ry
an
https://ww.doc88.com/p-80980482981320.html 139/185
```


## File page 140

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /184 > QQ View A mark ¥ Annotations ¥ Q
- 138 - 61850-7-4 © IEC:2010(E)
“ eee |
‘command and multiple command modes of an automatic synchroniser. With this data object it
can be selected whether the synchroniser should only give one command within a synchronising
attempt, or it should always give a command if all conditions are fulfilled. TRUE = multiple
commands allowed; FALSE = single command only.
[wea [onto gua wo hanetebehavar oe LN yt opr ew Aone”)
Se
(detault) ~ no other control level allowed)
[Wain [Motor eaent anv dapeyeina) SSCS
[Wotan [TRUE «motor opeating time exceeded SSSSSSS~*d
[MaSinin [Am ewlformotrnnineins
[Moinning [Arm ol ornumberotmorstaig =
[MoiDva [Mowrarweewret SSCS
[MatOp [TRUE =motorisnnning
[wots [oor startup test This valve Were aor srg conaion
[MotsvAin [TRUE =motrstatsinwscenes
[usin [Noite alam FALSE =romal TRUE =nghmoswre)
[wsiwin [Moire wating (moisture has reached ihe warninglevs) id
[ware | Aces svevckirg or meas snot vee eoibeck i
[Mvm [Vaweis moving
[Neppm [Measurement of N2inppm
[Namen | Thi ste name pat of helogealnage SSCS
[NewAin [TRUE =newalatemispreset SS SSS~—S
—_ Se
condition during power swing in the system.
[Noxems [NOwemissions
[Noma ___[Normatising demand current usedin IEEE 519 TOD calculation
[Nos [Average paria discharge current
[NumcntRs [Number of times a counterisreset
[Numcye [Number of eyces ofthe basic frequency
[NumPwrup | The number of power up operations of the physical device since the lastreset_ |
[NumRed [Actual umber of records
[mumsubine |The numberof subinerval a clclatonpevedilevaluatonconang |
[oxcntvaasonpeninconbaiongae SS SCSC*S
[ozepm __[MossuvnentotO,mepm iS
CC CT
[ot | ote. or arson valves te ost rom vero othe anaogue vie
[our [TRUE =o tiatonsopwaionatmning
[omoiA _[Oterevston nowramwecuret C=
fortmsin —_[Ottenpwawrecsomrn
[orrmpou —|Oltemperatwecooerow SSCS
Out of step slarm: supervision of selector switch synchronism
6
Ly
8
a
https://ww.doc88.com/p-80980482981320.html 140/185
```


## File page 141

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
61850-7-4 © IEC:2010(E) — 139 -
Data object
‘Operate (common data classes ACT) indicates the trip decision of a protection function (LN). The
trip itselt is issued by PTRC.
[OpAimNum | Alarm limit for number of operations
Operation close switch. OpCls shall be used if no control service is available between CSWI and
XCBR and the GOOSE is used instead.
This data object represents a count of operations that is not resettable. In general, this type of
counter is included in the following LNs: XCBR, XSWI, and YLTC. The counter shall not be reset
from remote but maybe from local.
[Gecrinim [RUE numberof peratons has scneedibe warming
This data object represents a resettable LN operations counter. The use of the CDC INC. permits
Setting the counter to something other than “O°.
[OpCntwm _| TRUE = number of operations has exceeded the warning limit
[OpDiTmms —_| Time delay in milliseconds before operating once operate conditions have been met
‘Command to operate a device (motor, pump, fan or similar) that will continue running until the
command is negated
Trip of a breaker failure function to a circuit breaker other than the faulty one to switch off the “rid
fault (“external trip")
Retrip of a breaker failure function after a trip of a protection function was not successful
(internal trip")
‘OpModRect —_| This data is used to define what mode a controllable rectifier shall operate in.
|___ Operating mode of rectifier __| Value |
a
{2
a
‘OpModSyn This data object may be used to select the operating mode:
[Automatic synchronising mode |__|
[Automatic parallelingmode | 2
[Manualmode
{Testmode
Description of the different values:
1 Synchroniser matches voltage and frequency and closes the circuit breaker automatically
2 Synchroniser closes the circuit breaker automatically
3 Synchroniser releases an external paralleling command (continuous mode)
4 Synchroniser does not send a closing command to the circuit breaker
‘Open position limitation, temporary limitation of maximum opening of valve, actuator or other
device
TRUE « provides indication that power system devices is operating with no load
‘Open end position reached (valve cannot move further)
‘Operation open switch. OpOpn shall be used if no control service is available between CSWI and
XCBR and the GOOSE is used instead
[Opova | TRUE = device is operating under an overcurrent condition
[OpOvext _| TRUE = device operating in an over excited condition
[Opow —_ TRUE = device is operating under an overvoltage condition
|OpSar__—_| TRUE = surge arrestor operation detected
Operation speed of main contact during close operation (usually displayed in m/s)
Operation speed of main contact during open operation (usually displayed in m/s)
TRUE = Switch operating time exceeded
‘Operation timing of main contact during close operation (usually displayed in ms).
ry
nw
https://www.doc88.com/p-80980482981320.html 141/185
```


## File page 142

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 718 > @ Q_ View A mark Y Annotations v | Q
-140- 61850-7-4 © IEC:2010(E)
Data object
Optmh This data object indicates the operation time in hours of a physical device since start of the
operation. Details are LN speciic.
Operation timing of main contact during open operation (usually displayed in ms)
TRUE = operation time of a physical device exceeds the warning limit
[OpUnExt _| TRUE = device operated in an under-excited condition
|Opunv _| TRUE = device operating in an under voltage condition
|OpwrnNum | Warning limit for number of operations.
fou |Anaaqe ouptottwiinsion
This data object indicates that a buffer overflow occurred for the output butfer and important
annunciation’s may be lost (TRUE) for the communication. A general interrogation is
recommended or an integrity scan is started automatically.
|OvHzStr__| Start (overtrequency variation event in progress)
Calculated maximum permissible overload time with cooling unit [min]
OvITEmg Calculated maximum permissible overload time without cooling unit (emergency case) [min]
OviTmSpt Maximum permissible overload time with cooling unit [min]
OvITEmgSpt _| Maximum permissible overioad time without cooling unit (emergency case) {min}
‘The movement of the main contact during a close operation, which is over the end position
(usually displayed in mm)
‘OvStkOpn The movement of the main contact during an open operation, which is over the end position
(usually displayed in mm)
[Pht | repoionaacion SSCS
|PaDschAim _| TRUE = Partial discharge has reached pre-set alarm level
|ParOp __| Transformers or suppression coils are operating in parallel.
‘Mode of parallel operation of petersen coil (controllable)
a
/Master/slave with fixed slave
estore ||
Parco | ete ewe |e
Parallel operation without
onmn | |
|ParMod _| Set current regulator mode during control (master, slave, independent)
Parallel transformer mode. Defined values are:
—|
[Master 0
[Follower
ParTraMod |Powerfactor
[Negative reactance TS
[Circulating current___—s| 6 |
[Circulating reactive current (var balancing) |
(Circulating reactive current by equalizing | |
|calculated transformer power factor
ry
an
https://ww.doc88.com/p-80980482981320.html 142/185
```


## File page 143

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 718 > @ Q_ View A mark Y Annotations v | Q
61850-7-4 © IEC:2010(E) -141-
Data object
Distance characteristic offset in percent of the line length.
Y
Cf
4
REACH ‘ec 110800
ee ee
[pr [rho rour power asf pases 2, and, noung mage +d
[pre ratte
‘Specification of the power factor sign according a certain standard. Two possible values:
[___PFSign Value |
[Active power (usually named IEC) |
[Lead/Lag (usually named IEEE) [2
For explanations, see the following picture.
Power Factor Interpretation
‘Values tor power tactor are interpreted acconing to the conventions shown in the
diagram below.
Quetert? men
FF lendng FF Loggng
ove far oncom Fone Fade an comer
eee ‘ce =”
cn. fee
i i
NS Y\ I
A,
a} NS k
+ iS J Ii *
tome, | wee
wma go > sae
i Se eta) :
o i
i A \] i +
hy Ni
Quntort 3 Ores 4
ioe ines
one Par Sn conan one Fador won coment
ite = ‘c-
tee. ae bo
lec 43010
[Prag [Pre ago ah iat 1 GW 1.0 power anomig Tar porr tow |
[rors Operate for mutha ute nme
Classifier bins of last complete long interval for phase to ground (A, B, C)
Classifier bins of last complete short interval for phase to ground (A, B, C)
0
nw
https://ww.doc88.com/p-80980482981320.html 143/185
```


## File page 144

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 1184 > @ Q_ View A mark Y Annotations ¥ Q
-142- 61850-7-4 © IEC:2010(E)
[PaPanwav [Real me domodistea wavelom forphase To gaund(AB.C)SSSCSC~*
[rapiori [Output 1 min average of ouput Sor phase to ground measurements =|
[PaPitax | Ouput instantaneous peak P va for phase 1 ground mensuemenis =|
[Pap | Lonperm eke avr fast complet terval for hase 1 round measurements |
[Pnpst | Shorerm eke seer oft compete interfer hase o ground measurements |
- tnmaa |
value, the operation of the related function is initiated.
[Pav | Phase to ground votaes for pases 1.2 and. ncuargange =
[Pram |The te name plat ofthe prysel gave SSSSCSCS~*
[Pioxip [iO gor, Posse vals weiPJ1/01PIIPO]OIPD =
[PmpAim _|uossotpumpisindeaes SS SSSCSCSC~S~S
PmpCtiGen — Control of all pumps.
PmpCt! — Control of a single pump.
[___ Pump contro! | Value _|
More stages may be added with numbers greater than 4
[PmpGrcu —[Pumpovreunenve SSCS
named as phase to zero voltage.
=
[None
| 2 |
=
| os |
a
[ 7 |
[Pench | Polarveachis the daneterolite Mo dagran.seePaifch ——SSC=d
ae
Possible states for position are:
intermediate state | off | on | bad state.
[Pesk | Tis ata obet sha be uses for sting, wher single phase Asay be operated seperate |
[Poa [Thana obet sal be used for swiching, where angle phase 8 may be operated separate |
[resc | This ta ojet sha be used or sitcing, were single phase C may be operated separately |
[reschg [Chane va positon stp. rase.iow)
[Poscralner [Incremental change ofposteon SSCS
[posvw [vate positon
Py
Ly
8
a
https://ww.doc88.com/p-80980482981320.html 144/185
```


## File page 145

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
61850-7-4 © IEC:2010(E) — 143 -
Data object
Point on wave switching capability.
[Value |
[None
[ Close
[Open ts
[Close andopen
Classifier bins of last complete long interval for phase to phase (AB, BC, CA)
PPPcbLs Classifier bins of last complete short interval for phase to phase(AB, BC, CA)
Real time demodulated waveform spectra for phase to phase (AB, BC, CA)
Real time demodulated wavetorm for phase to phase (AB, BC, CA)
‘Output 4 — 1 min average of output 5 for phase to phase measurements
[PPPiMax _| Output 5 ~ Instantaneous peak P value for phase to phase measurements
Output 3 - Square root of output § for phase to phase measurements
[PPP _| Long-term flicker severity of last complete interval for phase to phase measurements
[PPPst __| Short-term flicker severity of last complete interval for phase to phase measurements.
a
[pes |Pressoein spesiewoune
[PresAim _| Pressure alarm because of an abnormal condition (FALSE = normal, TRUE = alert)
This is the time prior to trigger for which data object is recorded when a trigger occurs
TRUE « indicates that the protection function has received the information about a fault in
forward direction from the other end of the line.
Prote TRUE = indicates that the protection function has detected a fault in forward direction and has
transmitted this information to the other end of the line
TRUE = indicates that the LN (LPHD) is a proxy. This means that the LD embedding this LN is,
representing another physical device.
This is the time following the trigger that the data object capture is recorded
[Pwrdn __| Adevice power down has been detected if PwrDn is TRUE
[PwrFact _| Power factor not allocated to a phase
[ewig [Ratpowe
‘Alarm from power supply allocated to the physical device it PwrSupAim is TRUE. May be an
external contact. It reters always to the local power supply of the IED modeled by LPHD and not
to the health (EEHealth) of the complete external supply system.
[Pwrup __| A device power up has been detected if Pwrp is TRUE
[Rat __| Winding ratio of an instrument transformer/transducer
TRUE = disturbance recording complete
RedMod This data object defines whether the recording will stop when the memory is full or saturated, or
overwrite existing values.
[Recording mode] Value —]
[Overwrite existing values | |
TRUE = disturbance recording processes started
External command to trigger recorder (TRUE)
[ners | Reconstr cnet eta) ng
[RCo!__| Raise arc suppression coil position (Petersen coil)
RetTmCis Time difference between activation to first position change for a close operation (usually
displayed in ms)
RetTmOpn Time difference between activation to first position change for an open operation (usually
displayed ',, ms)
nw
https://ww.doc88.com/p-80980482981320.html 145/185
```


## File page 146

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
—144- 61850-7-4 © IEC:2010(E)
Data object

[React _| Relative capacitance of bushing related to reference capacitance for bushing at commissioning

TRUE = lower reactive power, FALSE = no action

[Reach [TRUE rive resco power FALSE =noactan

RectTmms | Reclose time for 1-phase faults i.e. time to reclose command after trip in the cycle indicated by
the DO index. Multiple instances allow to set the reclose time per cycle or step.

[Rect3Tmms | Reclose time for evolving faults. Multiple instances allow to set the reclose time per cycle or step.

Reclose time for 3-phase faults. Multiple instances allow to set the reclose time per cycle or step.
Number of the actual recluse cycle (1 to n, typically n = 3). Default value 0 if no autoreclosing is
going on.

[Recon | Poi caret sa tear chao
Frame error rate on redundant channel; count of missed messages on this channel for each 1 000
messages forwarded to the application

Number of received messages on redundant channel

[nePr | Retwees power coro bahing ateonmisenng =

[nereact [Reece epacance or uring atconmisonng

[Retv _| Reference voltage for bushing at commissioning
This data object indicates that all criteria are fulfilled and the switching/operation action is
released to proceed if value is TRUE, and blocked it FALSE.

ReiDeaBus __| Releasing dead bus / dead line function

ReTrgMod If the mode is true, the recorder will start a new recording if itis retriggered while stil collecting
samples on previous recording (during post fault time). If false, the recorder ignores the retrigger.
Retrip mode

[___Retripmode Value |
fom
[Withoutcheck _
[With current check
[With breaker status check _ Ta
[With current and breaker status check —
[Otherchecks
[RHz __| TRUE = raise frequency, FALSE = no action
ry
Aa
https://www.doc88.com/p-80980482981320.html 146/185
```


## File page 147

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 718 > @ Q_ View A mark Y Annotations v | Q
61850-7-4 © IEC:2010(E) ~145-
Data object
RisGndRch Resistive reach of the quadrilateral ground distance element shown as the difference between the
left and right resistive blinders in the diagram below. See also AngLod in this table.
DirMod = forward
(trom LN RDIR)
> a, Additional settings:
ix -KOFact
—KOFactAng
—TimDeiMod
x1 — OpTimDel
° en
i
'
\ !
'
:
:
:
H
'
RisPhRch :
'
a ° + >
| RisGndRch
a"
!
'
“Forward
‘ec 110609
Resistive reach for load area. See AngLod for an example of the definition of load encroachment
used for the data objects AngLod and RisLod with polygonal characteristic, applicable also with
MHO.
[anzer__|Muwalresitnce coping tom paranline C=
[Rmpoe [Ranpingrateonadowwarduend SSS
[rnpup | Ranping rate on an vpwertveng SSS
Trrokhv | Runbck ase votap is the conrlvotape above whch aloloer command iaued |
Rotational direction. Possible values are:
[RotDir Valve |
=
|Counter-clockwise (reverse) | 2
[Unknown |
[Rs | Postve-sequnce no esstnce SSCS
[neormms [Tine aay nme blo reset nce eet cnaionshavebeenmet =
[rest [Tisha jst resets doves statics of HBLN =
ry
a
https://ww.doc88.com/p-80980482981320.html 147/185
```


## File page 148

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
— 146 - 61850-7-4 © IEC:2010(E)
Data object
Identifies the restraint mode for the differential LN.
[_____Restraintmode | Value _|
[None
[2=harmonic
Js harmonic
J2Fands" harmonic TY
[Waveform analysis
2" harmonic and waveform analysis {|_|
jother
|S" harmonic and waveform analysis | 8
ee
analysis
[RV __| TRUE = raise voltage, FALSE = no action
‘Activation information RxBik1 received from the other side(s), for logging purposes (teleprotection
blocking signal received)
[mca [ber ot rcoved meeugeg
Activation intormation RxPrm1 received from the other side(s), for logging purposes
(Teleprotection permissive signal received)
[RxSrc __| Source for activation information RxPrm or RxBik, must refer to data of type ACT
[RxSretr __| Source for activation information RxTr, must refer to data of type ACT
‘Activation information RxTr1 received from the other side(s), for logging purposes (direct trip
signal received)
[Rear | zero-eguece ee eones
[saorsio —[seiainesetconstg
Saturation coefficient $1.2
Subscription needs commissioning
Subscription with simulation
[Sbsst___| Status of the subscription (True = active, False = not active)
Reference to the IEC 61850 source data object. If this source data object has several (analogue)
attributes, all shall be registered in the COMTRADE file, and an appropriate number of ChNum1
instances shall define the channel mapping to them with the number in the order of the attributes
as defined in IEC 61850-7-3,
Pickup security timer on loss of carrier guard signal in ms
[soe |seecton oneenwier
[seope | seeton pen swt?
[SeqA __| The absolute measured values of positive, negative and zero sequence current
[Seqv _| The absolute measured values of positive, negative and zero sequence voltage
‘Current setting for a limit in motor start-up (for example counting operate condition or thermal
stress). This setting is used in motor start-up protection.
——— Time setting for a limit in motor start-up (for example counting operate condition or thermal
stress). This setting is used in motor start-up protection.
‘Access service tracking for setting group control block
This is an enumeration representing the operating capabilities of the power shunt.
[ “Shunt operating capability [Value]
[None
ShOpCap [Open —____|2__]
[Close
[Qpenandciose
Simulated GOOSE valves or simulated sampled measurand values will be used instead of
original values since they are first received.
0
a
https://ww.doc88.com/p-80980482981320.html 148/185
```


## File page 149

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /184 > QQ View A mark ¥ Annotations ¥ Q
61850-7-4 © IEC:2010(E) -147-
Data object

[snst | San sowing at ealy Sm nesapes ae recovedardaceped =
[Sint ___| Salinity, Saline content of water (g/l]
[seems |sanping reset
|SnwOen —_| Density of snowfall (usually in g/cm*)
[eed [Saundpesueled SS
a
[sovea | Wat quae of now unely nam)
[sorems |sOvenisions =
[ses] Rotor apeesunvanyine
‘Synchronous machine critical speed of the generator [s ']
‘Synchronous machine rated speed [5 "]
[seesnc | suac speedo water flow nme)
[secre |Sepomernoeaseowey
[Seovaim—[oevatonairm
[sar [seo rion SSS
[Seton | asin gong ve over
SptEndSt Setpoint end status

[Setpoint end status | Value]

[Ended normally Tt

[Ended with overshoot | 2

(Cancelled: measurement was deviating | 3

= |

of communication with local

(erro

(Cancelled: loss of communication with the

lentes oainmenne |e

Cancelled: timeout T 7

|Cancelled: voluntarily _— | 8

|Cancelled: noisy environments |

|Cancelled: materialtailure [| 10

Cancelled: stability ime was reached | 13

ce | * |

[Unknowncauses |
[soup [Setpoint going up raising)

a
an
https://ww.doc88.com/p-80980482981320.html 149/185
```


## File page 150

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
— 148 - 61850-7-4 © IEC:2010(E)
Data object
[SrcRet___| Reference to the IEC 61850 source data object
Stator leakage reactance [per unit]
Reterence temperature for stator resistance [usually °C)
Stator resistance [ohm]
StCicTun Result of tuning:
{__StceTun Vatu
[Tuned
[Tuned but not compensated |
JUmax
[Umax _nC (Umax-but not compensated) | 5
[Umax_not compensated due to U
ouewinme es |S
Step size when turning from positive to negative direction
[Stepps _| Step size when turning from negative to positive direction.
Stroke of the last operation defined as distance between start and end position of the main
contact or at the place of travel measurement (usually displayed in mm)
[ere ‘Storage rate (often called sampling rate) of the disturbance recorder in samples per millisecond
(ms)
Start (common data classes ACD) indicates the detection of a fault or an unacceptable condition.
‘Str may contain phase and directional information,
Start calculation sequence to estimate the parameters of a network. In compensated networks,
this can be done either by variating the suppression coil (Petersen coil) or with an current
injection in the neutral point of the system.
[sow |saniewiene
[suortnms —|sunouyme SSC
Status information restart inhibited. After a limit is reached (for example maximum number of
Starts or permissible temperature), restart inhibit is activated.
Strinhtmm __ | Time setting for restart inhibition. Once the Strinh is activated, the motor should not be allowed to
start until this time has elapsed.
[stPOw —_| TRUE = start CPOW (for example by select) — Request by CSWI or REC.
Day of the start of the local week for statistical calculation.
{____StrWeekDay | Value |
[Tuesday
[Wednesday
[Thusday
[Friday
6
ee A
Valve is blocked (cannot move from present position). Device is blocked through external
influence (can not operate or move).
‘Sum of switched amperes, resettable. This data object indicates the sum or integration of all
switched currents since the last reset of the counter, for example atter maintenance of the
contacts, the nozzle and other aging parts.
[Supvarh _| Reactive energy supply (default supply direction: energy flow towards busbar)
[Supwn _| Real energy supply (default supply direction: energy tlow towards busbar)
a
an
https://ww.doc88.com/p-80980482981320.html 150/185
```


## File page 151

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
61850-7-4 © IEC:2010(E) -149-
Data object
SveViol Service violation: the data object that the client wanted to access exists in the access view for the
association with that client, but the requested service is not allowed.
TRUE = alarm that switch arc has been detected
|SwgReact —_| Value of the power swing reactance band, see figure under SwgVal
SwoRis Value of the power swing resistance band, see figure under SwgVal
Power swing detection time in ms
‘SwoVal Value of the power swing band.
x
z
Inner
Outer
Swing Line
a R
‘ec 10709
SwOpCap This is an enumeration representing the physical capabilities of the switch to operate. It includes
additional blocking due to some local problems.
[None
[Open
[Close
[Qpenandclose
SwTyp
[___Switchtype___ [Value _|
[Load break switch _—|
[Disconnector 2
[Earthing switch Ts
SsynPro Synchrocheck / Synchronizing in progress. Start/ Stop Synchrocheck/Synchronizing
[TakTyp __| Type of tank (pressure only, level only, both pressure and level)
Tap position of load tap changer where automatic lower commands blocked
Tap position of load tap changer where automatic raise commands blocked
[TapChg __| This data object represents the control of a process to raise or lower a single step or tap
TapOpR ‘Change tap position raise (shall be used if no control service is available and the GOOSE is used
instead)
‘Change tap position lower (shall be used if no control service is available and the GOOSE is used
instead)
TapOpStop _| Change tap position stop (shall be used if no control service is available and the GOOSE is used
instead)
TapPos Represents the discrete adjustment of a transformer such as used in a load tap changer to a
specified tap position
[Tadd __| Current total demand distortion (according to IEEE 519, phase-related)
|TddAmp __| Current total demand distortion (according to IEEE 519, non-phase-related)
Current total demand distortion (according to IEEE 519, even components, phase-related)
[TddEvnAmp | Current ig’ ° demand distortion (according to IEEE 519, even components, non-phase-related)
an
https://www.doc88.com/p-80980482981320.html 151/185
```


## File page 152

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 718 > @ Q_ View A mark Y Annotations v | Q
- 150 - 61850-7-4 © IEC:2010(E)
Data object
‘Current total demand distortion (according to IEEE 519, odd components, phase-related)
Current total demand distortion (according to IEEE 519, odd components, non-phase-related)
| Tests! _| Test results value is TRUE if passed and FALSE if failed
[ThdA __| Current total harmonic or interharmonic distortion (different methods, phase-related)
|ThdAmp _| Current total harmonic or interharmonic distortion (different methods, non-phase-related)
Total harmonic or interharmonic distortion current alarm delay time in ms after the ThdAVal has
been exceeded
Total harmonic or interharmonic distortion amperes alarm setting value entered in %. Thd
values above this threshold cause an alarm.
Current total harmonic or interharmonic distortion (even components, phase-related)
Current total harmonic or interharmonic distortion (different methods, even components, non-
phase-related)
Phase to ground voltage total harmonic or interharmonic distortion (different methods, even
components, phase-related)
Phase to phase voltage total harmonic or interharmonic distortion (different methods, even
components, phase-related)
ThaEvnVol Phase voltage total harmonic or interharmonic distortion (different methods, even components,
non-phase-related)
Current total harmonic or interharmonic distortion (different methods, odd components, phase-
‘ThdOddA nea
Current total harmonic or interharmonic distortion (different methods, odd components, non-
Thdouaphy __ | Phase to ground voltage total harmonic or interharmonic distortion (different methods, odd
components, phase-related)
Phase to phase voltage total harmonic or interharmonic distortion (different methods, odd
ThdOPPV | components, phase-related)
Phase to ground voltage total harmonic or interharmonic distortion (different methods, odd
components, non-phase-related)
ThaPhv Phase to ground voltage total harmonic or interharmonic distortion (different methods, phase-
related)
[orev | Phase to phase voltage total harmonic or interharmonic distortion (different methods, phase-
related)
Voltage total harmonic or interharmonic distortion (different methods, non-phase-related)
ThdVTmms | Total harmonic or interharmonic distortion voltage alarm time delay in ms after the ThdVVal has
been exceeded
Total harmonic or interharmonic distortion alarm setting — value entered in %. Thd values above
this threshold cause an alarm.
[Taine [Vine coneats me]
Time constant 1 (lead) [ms}
Time constant 2 [ms]
Time constant 2 (lead) [ms]
Number of significant bits in the Fraction Of Second in the time accuracy part of the time stamp.
See IEC 61850-7-2.
Multiline curve characteristic definition. It is not multiple instantiable within one LN instance
TmACry Characteristic curve for protection operation of the form: y = f(x), where x is the current (A) and y
is the time (Tm). The integers representing the different curves are given in the definition of CDC
CURVE in IEC 61850-7-3.
TmChgDayTm | Local time of next change to daylight saving time
TmChgStdTm _ | Local time of next change to standard time
°
nw
https://ww.doc88.com/p-80980482981320.html 152/185
```


## File page 153

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 718 > @ Q_ View A mark Y Annotations v | Q
61850-7-4 © IEC:2010(E) -151-
[near TRUE |
linverse characterise | FALSE |
[Taba [Operate tine doy mode TAUE=on FALSE a SSC—=S
[rma Tita oj ee ie al utr or tine a sing ma avo eecion |
[Tp |The tonperaue of epectid component rina specieg vue awalyin“@) |
[TmpAim | Temperature alarm besa of an abormalcondon FALSE = noma, TRUE = ain) |
[reper |Masinomionperne id
[Tmsre | Gurrenttime source
[rmsieSett [Tne soe seting(1ER5"n cae he ine source a EEE T5R8 sure aed Paso) |
Tmsyn Time synchronized according to IEC 61850-9-2
—
=
(Tmp) and y is the time (Tm). The integers representing the different curves are given in the
definition of COC CURVE in IEC 61850-7-3.
[Tatapst _[Deivers te ace cove chaactrate—SSSSCS—~S
[TnuseoT [Fag neaig i sion une dvi ewveg tne
is the time (Tm). The integers representing the different curves are given in the definition of CDC
CURVE in IEC 61850-7-3.
[Tavsi [Devers tw ace cove chaarate——SSC—=S
[toe foweome SSCS
[Towa [Toa appar povorina teephasecreut
[Totvan _| Net apparr.;* energy since last rest
cy
8
a
https://ww.doc88.com/p-80980482981320.html 153/185
```


## File page 154

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
-152- 61850-7-4 © IEC:2010(E)
Data object
name
cc
a
[TpcBik —_| Teleprotection in blocked state
Panay Teleprotection application mode in receive direction for each command (unused, blocking,
pel permissive, direct, unblocking, status)
TpoTabtod Teleprotection application mode in transmit direction for each command (unused, blocking.
Led permissive, direct, unblocking, status)
Trip is the command to open the breaker when issued in case of fault by PTRC
Indicates for the next Trip if single pole tripping is allowed or three-pole tripping requested.
[___Tripbehavior | Value __}
[Undefined (defaut)
TrgMod Disturbance recorder trigger mode. The source of the external trigger is a local issue.
[Trigger Mode Value)
fintemal
[Extenal
Trigger reference shows the receiving trigger signal. It can be multiple in one LN-Instance.
TrMod This data object represents a type of trip function; 3ph means only phase tripping possible, 1ph
or 3ph means PTAC with 1 and 3 phase tripping possibility and first trip depending on fault type.
Trip pulse time is the minimum pulse time for breaker operation.
[TxBik —_| Blocking information to be transmitted to the other side (teleprotection blocking signal)
[txPrm | Permissive information to be transmitted to the other side (teleprotection permissive signal)
Direct trip information to be transmitted to the other side
TypRsCrv This is the type of the reset curve that is used to co-ordinate the reset with electromechanical
relays that do not reset instantaneously.
[___Resetcurve | Value _|
[Nong
[Definite time detayedreset___ | 2
[inverse reset
[UHFPaDsch _| UHF level of partial discharge
Unbalance detection method is the method used to detect the unbalanced condition based on measured or
calculated phase or sequence components of the monitored by the logical node system parameters.
[____Unbalance detection method _|__ Value __|
[Zero sequence
[Phase vectors comparison TS
(Cihers
ry
Nn
https://www.doc88.com/p-8098048298 1 320.htm! 154/185,
```


## File page 155

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 718 > @ Q_ View A mark Y Annotations v | Q
61850-7-4 © IEC:2010(E) - 153 -
This data object is the unblock function mode.
[____Unblock functionmode | Value __|
[Time window
[unrest | Stat ndregiocyvaraton wentinpogess) id
[unravel —[Undetreueneystt pein SS
[ue |tanteoutarectn oped Sd
nen fener |
autorecloser
= a
reference voltage or running voltage.
= sma
variable voltage or incoming voltage.
[va [Prato apurntpower SSS
[va |Pnase rence poner
ia == la
TRUE = ON; FALSE = OFF
= mania seer |
The data object is used to correct measuring errors or deviation in the ratio of the VT.
[varsir_[Stanatte requney vaiaionevent SS SSC*d
[varéna __[Eventtinished but notreset
[vor Vibration evel mms
|voraimspt | Vibration alarm evel setpoint
fe femme ener |
value is 1/s.
[verwabi | Vers wed dresion id
[verwased [Average vertical wind speed {usually ins)
and y is the voltage (V). The integers representing the different curves are given in the definition
of CDC CURVE in IEC 61850-7-3.
[vHzst [Delivers the actwe curve characterise
voltages to be synchronised is within the set limits or not. FALSE = value within the limits; TRUE
= value outside the limits.
ia =e a
in case the measuring voltages are connected hardwired to the synchroniser.
[vin | oui one ot container, reson, am otek unvalyine] |
[vai [A votagenonptasevoateg or DG vonage
[vowmpr | Votamperes recive ol anonweesnase creat SSC—*d
&
cy
8
a
https://www.doc88.com/p-80980482981320. html 155/185
```


## File page 156

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
- 154 - 61850-7-4 © IEC:2010(E)

[vost | TRUE = indicates voageovrie cone sana

[veo [TR = etgeecton is cre ves edie va Dw eral tng |

[vreavar ——[Reucin of band cen percent wien wage opr acive |

[vis [Rat vas ini roe oft dvs wih cote sitinge an nie |

(a = -inimaieiaioeiicaeiaiondll
ion

[wate | verge win anain cite st cmpitdot

1

a

a
This data object is the weak end infeed function mode.

-
[Of
[Operate
[Echo

a

[wertmne —[o-ordnaton tn for wak end ted ueten nae

[wins |Te mob f war tars ma by hepa Govind Hani wet ————_|

later |e sequen tne wecnos

a

es re

a ar

fp [esr cro aes Hr wel ies

a a

fenzer [itl ce cut om pee

es sr

a ee

fee ot rr ate er ene

[zune opcce oa pera ee pane sem (LT RH

a rT |

azn [2 sence ower naptis nci oa@)

[zpaang ——|Pontvesequrce ne ane

zest [Poteet gn

|ZeroEna___| Zero sequ’~ce current supervision enabled (TRUE)

cy
=)
Aa
https://www.doc88.com/p-80980482981320.htm| 156/185
```


## File page 157

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /184 > @ Q_ View A mark - Annotations» Q
61850-7-4 © IEC:2010(E) — 155 -
[znzerang _[Mvalinpedarce coping Von pualeline ange
[znzening [Mua impedance coping om pra ine magniode =
a
Aa
https://www.doc88.com/p-80980482981320.html 187/185
```


## File page 158

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
— 156 — 61850-7-4 © IEC:2010(E)
Annex A
(normative)
Interpretation of mode and behaviour
Switching between the modes (Mod.stVal) should only happen as a result of an operator
command to the data object Mod. Mod and Beh are always accessible by the services. The
communication services for the data object Mod do not care about the status of the Beh of the
LN. Possible values of Mod and Beh are given in Table A.1.
Table A.1 — Values of mode and behaviour
on The application represented by the LN works.
All communication services work and get updated values
on-blocked ‘The application represented by the LN works.
No output data (digital by relays or analogue setting) will be issued to the process.
All communication services work and get updated values.
Data objects will be transmitted with quality “operatorBlocked”.
Control commands will be rejected.
See note below Table A.1
test ‘The application represented by the LN works.
‘All communication services work and get updated values.
Data objects will be transmitted with quality “test”.
Control commands with quality test will be accepted only by LNs in “test” or “test-
blocked" mode.
“Processed as valid” means that the application should react in the manner what is
foreseen for “test”,
testiblocked The application represented by the LN works.
No output data (digital by relays or analogue setting) will be issued to the process.
All communication services work and get updated valves.
Data objects will be transmitted with quality “test”.
Control commands with quality test will be accepted only by LNs in TEST or
TEST-Blocked mode.
oft The application represented by the LN doesn't work.
No process output is possible. No control command should be acknowledged
(negative response)
Only the data object Mod and Beh should be accessible by the services.
Table A.2 gives an overview over the definition of mode and behaviour.
In the lower lines is given the functional processing of the LNs in different behaviour states.
Logical nodes should process receiving data according to their quality information:
- “Processed as valid” means that the application should react according to the quality and
the behaviour of the LN.
— “Processes as invalid" means the application should react as if the quality of the data had
been invalid.
- “Processed as blocked” means that the application should decide how to react, besides no
process-related action based on the value is performed.
- Statements “Processed” and “Not Processed” don't belong to communication services and
therefore no quality bit can be evaluated.
a
nw
https://www.doc88.com/p-80980482981320.html 158/185
```


## File page 159

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 718 > @ Q_ View A mark Y Annotations v | Q
61850-7-4 © IEC:2010(E) - 157 -
€
32 ra “ H B 3 3 3 3
sell 2] Fy e]eyeyeyeis
3 2)2/2|)2/2]) 2
=% 2|/3]/e2]/3)]2
58 e]/3)e|3]2
8 ; s Z| 2
38 esi] 2} ea) e}e)/ ea 8).
at aall 3 3 B || 8
gga al] 3 3 Fa 3 & §
ge gs) 8) 8) e| EB
o e@/eg/e)/e|é 2
v|/3|}/2/ 8] 2 2
g elelg}alaya. 2
. bs 2/3] 2/3 3 3 3
3 Bye) se] el) eit =
A &/3)/é] 3] 2
i filet i
Blele2el|e2
2 _ 2 3 2 2 2
3 32 sle};ele}e2|| 3
— os 3 gsizgiez 3 F3
§ Si £
2 _ elele]é]é i
§
! S > & = é
2 ® e)e/2]ea}e 3 2
g
3 ¥ RRR: é
o 8 Fa 8 a
r 8) 3/8/38] 8
&}/3)é] 2] =
§
€
i
x]
8
£
i
i
<
E
2
Ly
=)
nw
https:/Awww.doc88.com/p-8098048298 1 320.htm! 159/185
```


## File page 160

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
— 158 — 61850-7-4 © IEC:2010(E)
Annex B
(normative)
Local / Remote concept

The data object LocKey represents the status of a physical key switch and allows to taking over

the control authority.

The data object Loc shows the control behaviour of the logical node.

The data object LocSta shows the switching authority at station level. If LocSta=false control,

commands are allowed from remote, e.g. network control center (NCC).

The data object MitLev shall be modelled in LLNO only. It shows if more than one source of

control commands is accepted at a certain level at the same time.

Example:

1) If MitLev=false, CSWI.Loc=false and CSWI.LocSta=true, only a control command with a
station level originator is allowed, what means only one level has the switching authority.

2) If MitLev=true, CSWI.Loc=false and CSWI.LocSta=true, additionally to the allowed
command from the station level, also commands from the bay level are allowed, which
means that the station level and the bay level have the switching authority. So the final
reaction on control commands regarding the different sources of the commands (specified
by the originator of the control command) is defined by data objects Loc, LocSta and
MitLev.

The concept is illustrated in Table B.1 and shall be applied.

a
nw
https://www.doc88.com/p-80980482981320.html 160/185
```


## File page 161

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 1184 > @ Q_ View A mark Y Annotations ¥ Q
61850-7-4 © IEC:2010(E) - 159 -
Table B.1 — Relationship between Loc/Rem data objects and control authority
Bay control | commandiom |
Mode of y
of Control at
ate simariy | ten
rocess)
osal Gerirall station level | (P' )
XCBR.Loc cswi.
XSWI.Loc LocSta
pe | oe fm [i [wt | | om |
ee ee ee
pe fe [fe fs Pw ft» Pw tm |
pe | os fm [i [wf ~ | | om |
ee ee ee
i ee ee
re ee ee
Loc status (behaviour of the LN reg. switching authority)
T= TRUE ‘command only allowed at this level
F = FALSE ‘command not allowed at this level
‘f.a. = not applicable the position of this Loc is of no importance
Command
AA = ALWAYS ALLOWED command always allowed
NA = NOT ALLOWED command not allowed
MitLev (mode of switching authority for local control)
F = only one level (Originator) at a time has the switching authority (default, if this data object is not present)
T = more than one level (Originator) at any time has the switching authority (e.g. station and bay level)
Conclusions:
The local control switch LocKey, if available, is always allowed to be switched on/oft.
a
https://ww.doc88.com/p-80980482981320.html 161/185
```


## File page 162

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
— 160 — 61850-7-4 © IEC:2010(E)
Annex C
(informative)
Deprecated logical node classes
C.1 General
In this annex, those logical nodes are listed that are obsolete (no longer needed because of
technical progress since the publication of the first edition (2003). They will be kept in the
standard for backward compatibility with namespace indication of edition 1
(IEC 61850-7-4:2003).
C.2 LN: Metering statistics Name: MSTA
The metered values are not always used directly, but as average values, minima and maxima
over a given evaluation period. The reporting may be started after the end of this period.
states
—_ Eee
Instance-ID according to IEC 61850-7-2, Clause 22.
(Metered values
Javvons [wv [Averagevonage
Iva pV [verge appaentponer—SSSSSCSCS~S~S~S
nC
[Minva |v [Minimum apparent power
Javw fy [average active power
nwa uv [arse easivesower «dO
[Minvar [ay [Minimum reactive power
[ontrots
Settings
[vtnms NG [Evauton ne (ine wsow)trevenpes oe ——SSSC«dCi
ry
Aa
https://www.doc88.com/p-80980482981320.html 162/185
```


## File page 163

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
61850-7-4 © IEC:2010(E) — 161 -
Annex D
(informative)
Relationship between this standard and IEC 61850-5
The logical nodes listed in IEC 61850-5 define requirements; the logical nodes listed in this
standard define the modelling. Some requirements of the LNs from IEC 61850-5 are modelled by
LNs which are not explicitly referred to in this standard. Its functionality is provided by the services
or by the communication stack. Some system support functions are too dependent on
implementation to be standardized in this standard. Examples are listed in Table C.1.
Table D.1 — Relationship between IEC 61850-5 and this standard
for some miscellaneous LNs
Defined in | Modelied in
IEC 61850-5 | IEC 61850-7-4
by LN by LN
Dedicated function in a not-modelled interface
Time master STIM Not applicable | IED, such as a GPS-receiver providing time from
some external source to the IEC 61850 system.
Dependent on functions in the system and
therefore, implemented distributed in the IEDs of
the system (Example: health information from
ssys Not applicable | the LNs and services information, such as
reception of GOOSE messages within Tina
Some dedicated system supervision is provided
by the system logical nodes (group L).
‘Complex not-modelled function depending on
Test generator GTES Not applicable | test services provided which cannot be allocated
to one single LN. For testing, see IEC 61850-10.
a
nw
https://www.doc88.com/p-80980482981320.html 163/185
```


## File page 164

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
- 162- 61850-7-4 © IEC:2010(E)
Annex E
(informative)
Algorithms used in logical nodes for automatic control

E.1 General
A number of logical nodes for control functions are based on the algorithms used rather than
the allocation in a functional structure. This annex provides more detailed information on the
functional content behind the formal logical node definitions.
The following logical nodes are described in this annex:
* FCSD -Curve shape description function
* FPID — PID regulator function
= FFIL — Filter function
= _FRMP - Set-point ramping function
* FSPT - Set-point control function
E£.2 Logical node FCSD (curve shape description)
The logical node is used to adapt an incoming value to a specific curve function. As an
example, it can be used to adjust non-linear transmitters to the correct physical values. The
curve is two-dimensional in nature, however a three-dimensional curve can be achieved by
using several instances of the LN FCSD.
In Figure E.1, we can see an example of a two-dimensional curve used for shaping a flow value
based on the gate position. The values entered in the table are based on statistical data
obtained following a series of index tests.
=
“e

FCSD.Crv.crvPts4.xVal oo L reso ty

5

FOSD.CrvcrvPts4.yval__| "0" 5 icin
pe , Fon

FCSD.Crv.crvPts10.yVal i wae

Figure E.1 - Example of curve based on an indexed gate position
providing water flow
E.3_ Logical node FCSV (curve shape group)
The logical node is used to adapt an incoming value to a specific curve function. As an
example, it can be “e to adjust non-linear transmitters to the correct physical values. The
“a
https:/www.doc88.com/p-80980482981320.htm! 164/185
```


## File page 165

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
61850-7-4 © IEC:2010(E) — 163 —
curve is two-dimensional in nature, however a three-dimensional curve can be achieved by
using several instances of the FCSG LN. The logical node is similar to FCSD with the
exception that they are modifiable online.
In Figure E.2, we can see an example of a three-dimensional curve used for defining a runner
blade position based on two variables: net head and guide vane position. To achieve such a
function, five logical nodes are required.
[A AL ef Fosatcwentxva_| <-Fosat
[AAS resotcnanmatw | X oer
[He reser ovenreav Sin
[LF Fo reson cenmsz |
[Ae Feservenrev mpi Ove
[TA Hr rosorcx arms | x FCSG1Ouputmag
[aif resorcraneaava | y
COS oe z
Cs
Lf Fescrcvevrarowar_|
Figure E.2 - Example of curve based on an indexed guide vane position (x axis) vs. net
head (y axis) giving an interpolated runner blade position (Z axis)
E.4 Logical node FPID (PID regulator function)
The PID logical node comprises the following basic functions:
— The proportional function
This logical node is used to amplify an incoming value.
Output(s)
t) = Kp - Input(t); (s) = — “=< K,
Output(t) = Kp - Input(t) Gis) ‘nput(s) ~ *P
— The integral function
This logical node is used to integrate an incoming value.
K Output(s) 1
Output(t) = = {input - dt; [s) = <P) = K.
m ii flu 89) Inputs) STi
- The differential function
This logical node is used to adapt an incoming value to a specified function.
-t
Td = Output(s) s:Td
Output(t) = Input(t)- K-—-eT ; G(s) ==" = K.
NO) = Input) Tt Gs) Input(s) 1+s-Tf
NOTE The symbols used come from IEC 61850-7-410.
In Figure E.3, a typical proportional-integral-derivate controller is shown. All of the control
algorithm parameters are mapped to the logical node FPID data attributes. The process value
can originate from a sensor or a cascaded controller. The set-point normally will originate from
a cascaded controller or a manual command.
a
nw
https://www.doc88.com/p-80980482981320.html 165/185
```


## File page 166

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 1184 > @ Q_ View A mark Y Annotations ¥ Q
~ 164 - 61850-7-4 © IEC:2010(E)
Input from LN : FPID Algorithm
other LN
oem | ——_| =)
=| | 4
a
oe
Ki
| | Hi
FPID.Ki.setMag
ar
oe ee
a
ee es
FPID.OTmms.setVal
|
—— TY
ee
(ec 43710
Figure E.3 — Example of a proportional-integral-derivate controller
E.5 Logical node FFIL (filter function)
The logical node is used to filter an incoming value.
Output(s) (1+s-T1)
(s) = ———" = K-
89) inputis) Wes. 13+(s-TOP
More complex logical devices such as power stabilisation systems make a multiple use of
filters. See Figure E.4.
ry
a
https://ww.doc88.com/p-80980482981320.html 166/185
```


## File page 167

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
61850-7-4 © IEC:2010(E) — 165 -
Lo Ps FFL
A+sT21d)
eo
d d lesT21d
Ces} frets) — ees |e
Ao,
i:
IesT21d)
KideeTiid LesT21d) 1+sT31d)
(eI ‘ease tara) w 08
Fhe FFA
Figure E.4 —- Example of a power stabilisation system
E.6 Logical node FRMP (set-point ramping function)
In the following example given in Figure E.5, the set-point is being ramped according to two
different ramp set levels (FRMP1.RmpUp.stVal # FRMP1.RmpDn.stVal). The time cycle for
each increment is given by the defined sample rate (FRMP1.Output.smpRate),
po
|FRMP1.RmpUp.stVal | |
IFRMP 1 .AmpUp Stepsize
IFRMP1.RmpUp.minVal x!
Srey
|FRMP1.RmpDn.Stepsize
IFRMP1.RmpDn.minVal
FRMPTOutmag
IFRMP1.Out.smpRate
|FRMP1.Out.q
JFRMP1.Outt
Ct
ec 439710
Figure E.5 - Example of a ramp generator
E.7 Logical node FSPT (setpoint control function)
The logical node covers some common characteristics that are used in most automatic contro!
or regulator functions. The LN FSPT can be used as a stand-alone function but will normally be
cascaded with other control logical nodes.
The example given in Figure E.6 shows a set-point control interface with a field set-point
positioning device.
“a
https:/www.doc88.com/p-80980482981320.htm! 167/185
```


## File page 168

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /184 > @ Q_ View A mark Y Annotations ¥ Q
— 166 - 61850-7-4 © IEC:2010(E)

~ [eo
| 3
er
a
a nr a
t +4 _/ |
fetemeee fp eT OF
rom —| |] |
ease —}—H |
ea, ot nea
rsrrspupava ~~ }-b
a a ||
fortienomemanez |] |
fertemsmcneeene | ———4]_ 1
a oie
Figure E.6 — Example of an interface with a set-point algorithm

Nn

https:/Awww.doc88.com/p-8098048298 1 320.htm! 168/185
```


## File page 169

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
61850-7-4 © IEC:2010(E) - 167-
Annex F
(normative)
Statistical calculation

F.1 Statistical calculation basis

Here are some rules that have to be understood when implementing a calculation method.

* A statistical calculation transforms an “original” flow of data (indicated by CicSrc )into
“statistical” data with considered settings. These settings define the mathematical function
to apply, the condition for starting, the calculation interval duration and possible sliding, the
rate of data refreshment.

« When a statistical calculation method is applied to a logical node, it is supposed to be
applied independently to any Instmag value specified in the considered statistical LN (refer
to Figure F.1).

* This also applies to complex CDC such as vectors CMV/DEL/WYE. If the statistical method
also applies to angle per phase, then the sum of the 3 angles may be larger than 360°.

* The common data class of a statistical data, resulting from a statistical calculation, is
exactly identical to the CDC of the original data it applies to (referenced by CicSrc), then a
calculation method does not change a vector into a scalar.

* The content of a statistical LN can't include object/attribute which were not present in the
original one.

« Vector time consistency supported by WYE, DEL common data class is not to be broken
down but is extended to the full refreshment interval duration. Time consistency is then
valid considering the refreshment interval duration (refer to Figure F.1).

* Statistical calculation may be chained. For example, a first LN can produce RMS value,
then a second statistical LN can calculate an average of the considered RMS value on a
certain period, then an other statistical LN can calculate the maximum of the calculated
average since the last reset of this maximum value.

Example for a MAX calculation method:

Considering a CMV common data class, applying a MAX calculation method will lead to

calculate independently the Maximum for mag and the Maximum for angle.

Considering DEL/WYE, applying a MAX calculation method will lead to calculate independently

the Maximum for the 3 phases values.

By applying a calculation method to a vector, on a defined (refreshment) interval, we obtain at

the end of the period a vector, tagged with the time reference of the end of the period, and no

refresh in between. Time references to each individual results (if any) are lost.

a
nw
https://www.doc88.com/p-80980482981320.html 169/185
```


## File page 170

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
— 168 — 61850-7-4 © IEC:2010(E)
MMXU.A (instCVal.mag.f)
(trom CleSre) At, Az
at
A.PhsC
ni 21
. t 2
MMXU.A (instCVal.mag.f) sa a
(ClcMth=MAX) Alt A212
WYB ait WYE B22
cit 2,2
lec 441/10
Figure F.1 — Statistical calculation of a vector

F.2 Time interval definitions (relating to statistical calculation)

Four different time-related parameters are needed to define properly the considered statistical

calculation:

* The mode of calculation to define whether the calculation has to be performed periodically
(sliding (SLIDING) or not sliding (PERIOD)) or not periodically (TOTAL).

Refer ClcMod common LN data.

* The calculation interval duration, i.e. the duration window between the starting time of one
interval up to the next starting time of the next interval. This duration can be based on
cycle, time (UTC or local), or can be defined by an external trigger.

The calculation interval duration shall be defined by using two data objects, ClcintvTyp and
CicintvPer.

CicintvTyp indicates the time unit to consider in defining the calculation interval duration (if
its value differs from EXTERNAL) or indicates that this duration is based on an EXTERNAL
trigger if its value equals to EXTERNAL): refer ClelntvTyp common LN data.

When the time unit refers to DAY, WEEK, MONTH, YEAR, the time reference to consider is
the local time (refer to LTIM LN).

CicintvPer indicates the number of units to consider: refer ClcIntvPer common LN data.

If the mode of calculation (CicMod) is of type TOTAL (i.e. not periodic), CicintvTyp shall be
of type EXTERNAL or ignored, and CicintvPer (if defined) shall be ignored.

* The calculation sub-interval duration. This parameter is specific to sliding window of
calculation, and enables the user to define the duration step between to contiguous sliding
windows.

Because sub-interval duration shall always be an exact divider of the calculation period,
only one data object is needed to define the calculation sub-interval duration, i.e.
NumSubintv, the number of sub-intervals a calculation period interval duration contains.

If the mode of calculation (ClcMod) is not of type SLIDING (i.e. not periodic), NumSubIntv
shall be ignored.

* The calculation refreshment interval duration, i.e. the duration between two updates of the
calculation result.

The calculation refreshment period duration shall be defined by using two data objects,
CicRfTyp and CicRfPer.
CicRfTyp indicates the time unit to consider in defining the calculation refreshment period
duration (if its value differs from EXTERNAL) or indicates that this duration is based on an
EXTERNAL Wiggers ts value equals to EXTERNAL): refer ClcRfTyp common LN data.
nw
https://www.doc88.com/p-80980482981320.html 170/185
```


## File page 171

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q

61850-7-4 © IEC:2010(E) - 169 —

When the time unit refers to DAY, WEEK, MONTH, YEAR, the time reference to consider is

the local time (refer to LTIM LN).

CicRfPer indicates the number of units to consider: refer CicRfPer common LN data.

The calculation refreshment interval duration shall be shorter or equal than the calculation

interval duration (if not EXTERNAL).

In case of SLIDING calculation mode, the calculation refreshment period duration shall be

shorter than or equal to the calculation sub-interval duration.

If the refreshment period is not defined, it is supposed to be equal to the calculation period.
F.24 Examples
In the graphics (Figure G.1) below, the horizontal axis represents the current time, grey zones
represent calculation interval. The symbol # indicates that the LN is producing a new
instantaneous value.

Periodic calculation (period = T).
Refreshment period is equal to
calculation period.

This can be met for example, for
demand calculation or min/max
Pot, Lo] —
T T T
Sub-interval is not used.
4 ba 4 tec 442/10
| SLIDING (single refresh) | Periodic sliding calculation (period
= T). Refreshment period length is
T equal to sub-interval duration.

This can be met for example, for
demand calculation.
Sub-interval is used to define the

[7 sliding duration “sub-T"

sub-T
tec 443/10

Periodic calculation (period = T).
with higher refresh rate.

T Refreshment period is equal to R.
ee ceeeens FE This can be met for example, for
REeEEE Sees Ree prediction demand calculation.

eS eC er
RTE RRU LED TL Pee Sub-interval is not used.
“a
https:/www.doc88.com/p-80980482981320.htm! 171/185
```


## File page 172

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
-170- 61850-7-4 © IEC:2010(E)
Periodic sliding calculation (period
= T). Refreshment period length
Sub-T T (R) is shorter than the sub-interval
—,*_ | duration (sub-T).
PEt Et bat tt tt PAN tt. | This can be met, for example, for
oe ee ee ee rediction demand calculation.
RTP TPR PALE |P
ee ae a Sub-interval is used to define the
tag tag, a, sliding duration “sub-T”.
1ec 445/10
| TOTAL (with periodic refresh) | Continuous calculation from the
last reset with a refreshment
— period duration (R) of the
roroforof TF H calculation result.
(a ee This can be met, for example, for
ees | maximum demand calculation from
“Sten, the last reset.
ec 44610
Figure F.2 - Examples of statistical calculations
F.3 Calculation start
F.34 Start of statistical calculation means that
* all analogue values of the LN will be set internally to their initial state;
* until a new refresh is available, Instmag data will keep their previous values with their
associated time-stamp (referring to the previous interval);
* for the very first calculation interval, they will be stated as “bad quality” until the first refresh
is available.
F.3.2 The three possible start conditions available in the model
F.3.2.1 Periodic re-start
The LN will re-start calculation at the beginning of each calculation interval.
Depending on the calculation interval definition, starting time such as “Start of the day”, “Start
of the week", Start of the month", Start of the year” will have to be implemented internally
based on LTIM settings.
F.3.2.2  Aperiodic start
Aperiodic start will happen in two phases:
+ Start enabler
An start enabler may be send on-demand to the LN using CicStr control which is part of
the common LN data of the considered statistical LN: the calculation will be enabled
depending on the CLCStr attributes: at time operTm (defined in UTC time reference)
from the control Fs 4el (if set) or immediately.
nw
https://www.doc88.com/p-80980482981320.html 172/185
```


## File page 173

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
61850-7-4 © IEC:2010(E) -171-
* Calculation start
If ClcMod is set to TOTAL, calculation will start as soon as it has been enabled.
If ClcMod is set to PERIOD or SLIDING, calculation will start at the next occurrence of
calculation interval (depending on the value of CicintvTyp and ClcintvPer). If CicintvTyp is
set to MS, calculation will start at the next multiple of calculation intervals from the start of
the day (example: if the calculation interval is set to 15 min (= 900 000 ms), calculation will
start at the next occurrence of a full quarter of an hour ( 00:00, 00:15, 00:30, ....).
Power on of the device may be considered as a start enabler.
F.3.2.3 External synchronisation
An external trigger can be defined using the InSyn ORG reference defined in the common LN
data part of the considered statistical LN. The referenced object shall be of BOOLEAN type.
If the calculation interval is explicitly set as EXTERNAL (ClcIntvTyp), each raising edge from
FALSE to TRUE of the value of the objet referenced by InSyn will produce an immediate re-
Start of the statistical calculation of the LN as described above.
If ClcintvTyp is not set to EXTERNAL, then the InSynch trigger shall be ignored.
Remaining time up to the end of the calculation interval:
If ClcMod is set to SLIDING or PERIOD, and if CicNxTmms is defined as part of the data of the
considered LN, ClcNxTmms will indicate the calculation remaining time up to the end of the
current calculation interval in milliseconds.
a
nw
https://www.doc88.com/p-80980482981320.html 173/185
```


## File page 174

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
-172- 61850-7-4 © IEC:2010(E)
Annex G
(normative)
Functional relationship of data objects of autorecloser RREC
G.1__ Principal diagram of autorecloser
Figure G.1 gives the functional diagram of the autorecloser with its different data objects (status,
settings etc.). In the bubbles are given the states of the autorecloser function as they are also specified
in Clause 6, Table 10.
FALSE
|
TRUE _| (7%)
or
QO) cesangem Pa] Pos
fre ~ OQ] "some Seren |O
Ss Opcis | AecTmmsPh» RecTmmsGen | Optis
_
Reclse
<
Rect
© ©
— Ss HEC 447/10
Figure G.1 — Diagram of autorecloser function
ry
Aa
https://www.doc88.com/p-80980482981320.html 174/185
```


## File page 175

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
61850-7-4 © IEC:2010(E) -173-
Annex H
(normative)
SCL enumerations
<EnumType id="AdjSt">
<EnumVal ord="1*>Completed</EnumVal>
<EnumVa! ord="2">Cancelled </EnumVal>
<EnumVal ord="3">New adjustments </EnumVal>
<EnumVal ord="4">Under way </EnumVal>
</EnumType>
<EnumType id»"AutoRecSt">
<EnumVal ord="1*>Ready</EnumVal>
<EnumVal! ord="2">In progress</EnumVal>
<EnumVal ord="3">Successtul</EnumVal>
<EnumVal ord="4">Waiting for trip</EnumVal>
<EnumVal ord="5">Trip issued by protection</EnumVal>
<EnumVal ord="6">Fault disappeared</EnumVal>
<EnumVal ord="7">Wait to complete</EnumVal>
<EnumVal ord="8">Circuit breaker closed</EnumVal>
<EnumVal ord="9">Cycle unsuccesstul</EnumVal>
<EnumVal ord="10">Unsuccesstul</EnumVal>
<EnumVal ord="11">Aborted</EnumVal>
</EnumType>
<EnumType id="Beh">
<EnumVal ord="1">0n</EnumVal>
<EnumVal ord="2">on-blocked</EnumVal>
<EnumVal ord="3">test</EnumVal>
<EnumVal ord="4">test/blocked</EnumVal>
<EnumVal ord="5*>off</EnumVal>
</EnumType>
<EnumType id="CicintvTyp">
<EnumVal ord="1">MS</EnumVal>
<EnumVal ord="2">PER_CYCLE</EnumVal>
<EnumVal ord="3">CYCLE</EnumVal>
<EnumVal ord="4">DAY</EnumVal>
<EnumVal ord="5">WEEK</EnumVal>
<EnumVal ord="6">MONTH</EnumVal>
<EnumVal ord="7">YEAR</EnumVal>
<EnumVal ord="8">EXTERNAL</EnumVal>
</EnumType>
<EnumType id="CicMth">
<EnumVal ord="1">UNSPECIFIED</EnumVal>
<EnumVal ord«"2">TRUE_RMS</EnumVal>
<EnumVal ord="3">PEAK_FUNDAMENTAL</EnumVal>
<EnumVa! ord="4">RMS_FUNDAMENTAL</EnumVal>
<EnumVal ord="5"*>MIN</EnumVal>
<EnumVal ord="6">MAX</EnumVal>
<EnumVal ord="7">AVG</EnumVal>
<EnumVal ord="8">SDV</EnumVal>
<EnumVal ord="9">PREDICTION</EnumVal>
<EnumVal ord="10">RATE</EnumVal>
</EnumType>
<EnumType id="CicMod">
<EnumVal ord="1">TOTAL</EnumVal>
<EnumVal ord="2">PERIOD</EnumVal>
<EnumVal ord="3">SLIDING</EnumVal>
</EnumType>
<EnumType id="CicRiTyp">
<EnumVal ord="1">MS</EnumVal>
<EnumVal ord="2">PER_CYCLE</EnumVal>
<EnumVal ord="3">CYCLE</EnumVal>
<EnumVal ord="4">DAY</EnumVal>
<EnumVal ord="5*>WEEK</EnumVal>
<EnumVal ord="6">MONTH</EnumVal>
<EnumVal ord="7">YEAR</EnumVal>
<EnumVal ord="8">EXTERNAL</EnumVal>
</EnumType>
<EnumType id="CicTotVA">
<EnumVal ord="1">Vector</EnumVal>
<EnumVa! ord="2">Arithmetic</EnumVal>
</EnumType>
<EnumType id="CBOaCap">
peal J="1">None</EnumVal>
a
https://ww.doc88.com/p-80980482981320.html 175/185
```


## File page 176

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
-17%4- 61850-7-4 © IEC:2010(E)
<EnumVal ord="2">Open</EnumVal>
<EnumVal ord="3">Close-Open</EnumVal>
<EnumVal ord="4">Open-Close-Open</EnumVal>
<EnumVal ord="5">Close-Open-Close-Open</EnumVal>
<EnumVal ord="6">Open-Close-Open-Close-Open </EnumVal>
<EnumVal ord="7*>more</EnumVal>
</EnumType>
<EnumType id="CycTrMod">
<EnumVal ord="1">three phase tripping</EnumVal>
<EnumVal ord="2">one ot three phase tripping</EnumVal>
<EnumVal ord="3">specific</EnumVal>
</EnumType>
<EnumType id="DirMod">
<EnumVal ord="1">NonDirectional</EnumVal>
<EnumVal ord«"2">Forward</EnumVal>
<EnumVal ord="3">Reverse</EnumVal>
</EnumType>
<EnumType id="EEHealth">
‘<EnumVal ord="1">Ok</EnumVal>
<EnumVal ord="2">Warning</EnumVal>
<EnumVal ord="3">Alarm</EnumVal>
</EnumType>
<EnumType id="FailMod">
<EnumVal ord="1*>Current</EnumVal>
<EnumVal ord="2">Breaker Status</EnumVal>
<EnumVal ord="3">Both current and breaker status</EnumVal>
<EnumVal ord="4">Other</EnumVal>
</EnumType>
<EnumType id="FanCtl">
<EnumVal ord="1">Inactive</EnumVal>
<EnumVal ord="2">Stage 1</EnumVal>
<EnumVal ord«"3">Stage 2</EnumVal>
<EnumVal ord="4">Stage 3</EnumVal>
</EnumType>
<EnumType id="FanCtlGen">
<EnumVal ord="1">Inactive</EnumVal>
<EnumVal ord="2">Stage 1</EnumVal>
<EnumVal ord="3">Stage 2</EnumVal>
<EnumVal ord="4">Stage 3</EnumVal>
</EnumType>
<EnumType id="FilTyp">
<EnumVal ord="1">Low pass</EnumVal>
<EnumVal ord="2">High pass</EnumVal>
<EnumVal ord="3">Bandpass</EnumVal>
<EnumVal ord="4">Bandstop</EnumVal>
<EnumVal ord="5">Deadband</EnumVal>
</EnumType>
<EnumType id="FitLoop">
<EnumVal ord="1">Phase A to Ground</EnumVal>
<EnumVal ord="2">Phase B to Ground</EnumVal>
<EnumVa! ord="3">Phase C to Ground</EnumVal>
<EnumVa! ord="4">Phase A to B</EnumVal>
<EnumVal ord="5*>Phase B to C</EnumVal>
<EnumVal ord="6">Phase C to A</EnumVal>
<EnumVa! ord="7">Other</EnumVal>
</EnumType>
<EnumType id="GnSt">
<EnumVal ord="1">Stopped</EnumVal>
<EnumVal ord="2">Stopping</EnumVal>
<EnumVal ord="3">Started</EnumVal>
<EnumVal ord="4">Starting</EnumVal>
<EnumVal ord="5">Disabled</EnumVal>
</EnumType>
<EnumType id="Health">
<EnumVal ord="1">Ok</EnumVal>
<EnumVal ord="2">Warning</EnumVal>
<EnumVal ord="3">Alarm</EnumVal>
</EnumType>
<EnumType id="IntrDetMth">
<EnumVal ord="1">Voltage</EnumVal>
<EnumVal ord="2">Voltage and Current </EnumVal>
<EnumVal ord="3">Voltage and Normally Open Breaker Contact </EnumVal>
<EnumVa! ord«"4*>Voltage and Normally Closed Breaker Contact </EnumVal>
<EnumVal ord="5">Voltage and both Normally Open and Normally Closed Breaker Contacts
<J/EnumVal>
<EnumVal ord="6"> Normally Open Breaker Contact </EnumVal>
<EnumVal Chalte Normally Closed Breaker Contacts </EnumVal>
<EnumVy-6="8"> Both Normally Open and Normally Closed Breaker Contacts </EnumVal>
Aa
https://www.doc88.com/p-80980482981320. html 176/185
```


## File page 177

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 1184 > @Q_~ View A mark Y Annotations ¥ Q)
61850-7-4 © IEC:2010(E) -175-
</EnumType>
<EnumType id="LevMod">
<EnumVa! ord="1">Positive or Rising</EnumVal>
<EnumVal ord="2">Negative or Falling</EnumVal>
<EnumVal ord="3">Both</EnumVal>
<EnumVal ord="4">Other</EnumVal>
</EnumType>
<EnumType id="LivDeaMod">
<EnumVal ord="1">Dead Line, Dead Bus</EnumVal>
<EnumVal ord="2">Live Line, Dead Bus</EnumVal>
<EnumVal ord«"3">Dead Line, Live Bus</EnumVal>
<EnumVal ord="4">Dead Line, Dead Bus OR Live Line, DeadBus</EnumVal>
<EnumVal ord="5">Dead Line, Dead Bus OR Dead Line, Live Bus</EnumVal>
<EnumVal ord="6">Live Line, Dead Bus OR Dead Line, Live Bus</EnumVal>
<EnumVal ord="7">Dead Line, Dead Bus OR Live Line, Dead Bus OR Dead Line, Live
Bus</EnumVal>
</EnumType>
<EnumType id="MechHealth">
<EnumVal ord="1">Ok</EnumVal>
<EnumVal ord="2">Warning</EnumVal>
<EnumVal ord="3">Alarm</EnumVal>
</EnumType>
<EnumType id="Mod">
<EnumVal ord="1">on</EnumVal>
<EnumVa! ord="2">0n-blocked</EnumVal>
<EnumVal ord="3">test</EnumVal>
<EnumVal ord="4">test/blocked</EnumVal>
<EnumVal ord="5*>off</EnumVal>
</EnumType>
<EnumType id="OpModRect">
<EnumVal ord="1">Current control mode</EnumVal>
<EnumVal ord="2">Voltage control mode</EnumVal>
<EnumVal ord="3">Active power control mode</EnumVal>
</EnumType>
<EnumType id="ParColMod">
<EnumVal ord="1*> Master/ Slave</EnumVal>
<EnumVal ord="2">Master/ Slave with fixed slave position</EnumVal>
<EnumVal ord="3">Master/ Slave with variable slave posiiton</EnumVal>
<EnumVal ord="4">Parallel operation without communication</EnumVal>
</EnumType>
<EnumType id="ParMod">
<EnumVal ord="1*>Master</EnumVal>
<EnumVal ord="2">Slave</EnumVal>
<EnumVal ord="3">Independent</EnumVal>
</EnumType>
<EnumType id="ParTraMod">
<EnumVal ord«"1"> No Mode Predefined</EnumVal>
<EnumVal ord="2"> Master</EnumVal>
<EnumVal ord="3"> Follower</EnumVal>
<EnumVal ord="4"> Power Factor</EnumVal>
<EnumVal ord="5"> Negative Reactance</EnumVal>
<EnumVal ord="6"> Circulating Current</EnumVal>
<EnumVal ord="7"> Circulating Reactive Current</EnumVal>
<EnumVal ord="8">
Circulating Reactive Current By Equalizing Calculated Transformer Power Factor</EnumVal>
</EnumType>
<EnumType id="PIDAIg">
<EnumVal ord="1">P</EnumVal>
<EnumVal ord="2">1</EnumVal>
<EnumVal ord="3">>D</EnumVal>
<EnumVal ord="4">Pl</EnumVal>
<EnumVal ord="5">PD</EnumVal>
<EnumVal ord="6">ID</EnumVal>
<EnumVal ord="7">PID</EnumVal>
</EnumType>
<EnumType id="PFSign">
<EnumVal ord="1">Active Power</EnumVal>
<EnumVal ord="2">Lead/Lag</EnumVal>
</EnumType>
<EnumType id="PhyHealth">
<EnumVal ord="1">Ok</EnumVal>
<EnumVal ord="2">Warning</EnumVal>
<EnumVal ord="3">Alarm</EnumVal>
</EnumType>
<EnumType id="PmpCt!">
<EnumVal ord="1">Inactive</EnumVal>
<EnumVal “jd="2">Stage1 </EnumVal>
<Enumvg- d="3">Stage2</EnumVal>
nw
https://www.doc88.com/p-80980482981320.html 177/185
```


## File page 178

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
- 176 - 61850-7-4 © IEC:2010(E)
<EnumVal ord="4">Stage3</EnumVal>
</EnumType>
<EnumType id="PmpCtiGen">
<EnumVal ord="1">Inactive</EnumVal>
<EnumVal ord="2">Stage1</EnumVal>
<EnumVal ord="3">Stage2</EnumVal>
<EnumVal ord="4">Stage3</EnumVal>
</EnumType>
<EnumType id="PolQty">
<EnumVal ord="1*>None</EnumVal>
<EnumVal! ord="2">Zero Sequence Current</EnumVal>
<EnumVal ord="3">Zero Sequence Voltage</EnumVal>
<EnumVal ord="4">Negative Sequence Voltage</EnumVal>
<EnumVal ord="5">Phase to Phase Voltages</EnumVal>
<EnumVal ord="6">Phase to Ground Voltages</EnumVal>
<EnumVal ord="7"> Positive sequence voltage </EnumVal>
</EnumType>
<EnumType id="POWCap">
<EnumVal ord="1*>None</EnumVal>
<EnumVal ord="2">Close</EnumVal>
<EnumVal ord="3">Open</EnumVal>
<EnumVal ord="4">Close and Open</EnumVal>
</EnumType>
‘<EnumType id="OpModSyn">
<EnumVal ord="1"> Automatic synchronising mode </EnumVal>
<EnumVal ord="2"> Automatic paralleling mode </EnumVal>
<EnumVal ord="3"> Manual mode</EnumVal>
<EnumVal ord="4"> Test mode</EnumVal>
</EnumType>
<EnumType id="RedMod">
'<EnumVal ord="1">Overwrite existing values</EnumVal>
<EnumVal ord="2">Stop when full or saturated</EnumVal>
</EnumType>
<EnumType id="ReTrMod">
<EnumVal ord="1">Off</EnumVal>
<EnumVal ord="2">Without Check</EnumVal>
<EnumVal ord="3">With Current Check</EnumVal>
<EnumVal ord="4">With Breaker Status Check</EnumVal>
<EnumVal ord="5">With Current and Breaker Status Check</EnumVal>
<EnumVal ord="6">Other Checks</EnumVal>
</EnumType>
<EnumType id="RotDir">
<EnumVal ord="1">Clockwise</EnumVal>
<EnumVa! ord="2*>Counter-Clockwise</EnumVal>
<EnumVal ord="3">Unknown</EnumVal>
</EnumType>
<EnumType id="RstMod">
<EnumVal ord="1">None</EnumVal>
<EnumVal ord="2">Harmonic2</EnumVal>
<EnumVal ord="3">Harmonic5</EnumVal>
<EnumVal ord="4">Harmonic2and5</EnumVal>
<EnumVal ord="5">WavetormAnalysis</EnumVal>
<EnumVal ord="6">WaveformAnalysisAndHarmonic2</EnumVal>
<EnumVal ord="7"> Other</EnumVal>
<EnumVal ord="8"> WaveformAnalysisAndHarmonic5</EnumVal>
<EnumVal ord="9"> WavetormAnalysisAndHarmonic2AndHarmonic5</EnumVal>
</EnumType>
<EnumType id="ShOpCap">
<EnumVal ord="1">None</EnumVal>
<EnumVal ord="2">Open</EnumVal>
<EnumVal ord="3">Close</EnumVal>
<EnumVal ord="4">Open and Close</EnumVal>
</€numType>
<EnumType id="SptEndSt">
<EnumVal ord="1">Ended normally</EnumVal>
<EnumVal ord="2">Ended with overshoot</EnumVal>
<EnumVal ord="3">Cancelled: measurement was deviating</EnumVal>
<EnumVal ord="4">Cancelled: loss of communication with dispatch centre</EnumVal>
<EnumVal ord«"5">Cancelled: loss of communication with local area network</EnumVal>
<EnumVal ord="6">Cancelled: loss of communication with the local intertace</EnumVal>
<EnumVal ord="7">Cancelled: timeout</EnumVal>
<EnumVal ord="8">Cancelled: voluntarily</EnumVal>
<EnumVal ord="9">Cancelled: noisy environments</EnumVal>
<EnumVal ord="10">Cancelled: material failure</EnumVal>
<EnumVal ord="11">Cancelled: new set-point request</EnumVal>
<EnumVal ord="12">Cancelled: improper environment (blockage)</EnumVal>
<EnumVal Cat |3°>Cancelled: stability time was reached</EnumVal>
<EnumV iy -d="14">Cancelled: immobilisation time was reached</EnumVal>
Aa
https://www.doc88.com/p-80980482981320.htm| 178/185
```


## File page 179

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< /18 > QQ View A mark Y Annotations Y Q)
61850-7-4 © IEC:2010(E) -17-
<EnumVal ord="15">Cancelled: equipment was in the wrong mode</EnumVal>
<EnumVal ord="16">Unknown causes</EnumVal>
</EnumType>
<EnumType id="StrWeekDay">
<EnumVal ord="1">Monday</EnumVal>
<EnumVal ord="2">Tuesday</EnumVal>
<EnumVal ord="3">Wednesday</EnumVal>
<EnumVal ord="4">Thursday</EnumVal>
<EnumVal ord="5">Friday</EnumVal>
<EnumVal ord="6">Saturday</EnumVal>
<EnumVal ord="7">Sunday</EnumVal>
</EnumType>
<EnumType id="StCicTun">
<EnumVal ord="1">Not tuned</EnumVal>
<EnumVal ord="2*>Tuned</EnumVal>
<EnumVal ord="3">Tuned but not compensated</EnumVal>
<EnumVal ord="4">Umax</EnumVal>
<EnumVal ord="5">Umax but not compensated</EnumVal>
<EnumVal ord="6">Umax but not compensated due to U continous limitation</EnumVal>
</EnumType>
<EnumType id="SwOpCap">
<EnumVal ord="1">None</EnumVal>
<EnumVal ord="2">Open</EnumVal>
<EnumVal ord="3">Close</EnumVal>
<EnumVal ord="4">Open and Close</EnumVal>
</EnumType>
<EnumType id="SwTyp">
<EnumVal ord="1">Load Break Switch</EnumVal>
<EnumVal ord="2">Disconnector</EnumVal>
<EnumVal ord="3">Earthing Switch</EnumVal>
<EnumVal ord="4">High Speed Earthing Switch</EnumVal>
</EnumType>
<EnumType id="TnkTyp">
<EnumVal ord="1">pressure only</EnumVal>
<EnumVal ord="2°>level only</EnumVal>
<EnumVal ord="3">both pressure and level</EnumVal>
</EnumType> — <EnumType id="TmSyn">
<EnumVal ord="2">Synchronized by a global area clock signal</EnumVal>
<EnumVal ord="1">Synchronized by a local area clock signal</EnumVal>
<EnumVal ord ="0">Not synchronized by a global area clock signal</EnumVal>
</EnumType>
<EnumType id="TpcRxMod">
<EnumVal ord="1">Unused</EnumVal>
<EnumVal ord="2">Blocking</EnumVal>
<EnumVal ord="3">Permissive</EnumVal>
<EnumVal ord="4">Direct</EnumVal>
<EnumVal ord="5">Unblocking</EnumVal>
<EnumVal ord="6">Status</EnumVal>
</EnumType>
<EnumType id="TpcTxMod">
<EnumVal ord="1">Unused</EnumVal>
<EnumVal ord="2">Blocking</EnumVal>
<EnumVal ord="3*>Permissive</EnumVal>
<EnumVal ord="4">Direct</EnumVal>
<EnumVal ord="5">Unblocking</EnumVal>
<EnumVal ord="6">Status</EnumVal>
</EnumType>
<EnumType id="TrBeh">
‘<EnumVal ord="1">Single Pole Tripping</EnumVal>
<EnumVal ord="2">Undetined</EnumVal>
<EnumVal ord="3">Three Pole Tripping</EnumVal>
</EnumType>
<EnumType id="TrgMod">
<EnumVal ord="1">Internal</EnumVal>
<EnumVal ord="2">External</EnumVal>
<EnumVal ord="3">Both</EnumVal>
</EnumType>
<EnumType id="TrMod">
‘<EnumVal ord="1">3 Phase Tripping</EnumVal>
<EnumVal ord="2">1 or 3 Phase Tripping</EnumVal>
<EnumVa! ord="3">Specitic</EnumVal>
<EnumVal ord="4">1 Phase Tripping</EnumVal>
</EnumType>
<EnumType id="TypRsCrv">
<EnumVal ord="1">None</EnumVal>
<EnumVal ord="2">Detinite Time Delayed Reset</EnumVal>
<EnumVal “jd="S*>lnverse Reset</EnumVal>
</EnumType> 6
Aa
https://www.doc88.com/p-80980482981320. html 179/185
```


## File page 180

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 1184 > @ Q_ View A mark Y Annotations ¥ Q
-178- 61850-7-4 © IEC:2010(E)
«<EnumType id="UnbDetMth">
<EnumVal ord="1">Negative Sequence</EnumVal>
<EnumVal ord="2">Zero Sequence</EnumVal>
<EnumVa! ord="3">Negative Sequence / Positive Sequence</EnumVal>
<EnumVal ord="4">Zero Sequence / Positive Sequence Direct</EnumVal>
<EnumVal ord«"5"> Phase vectors comparison</EnumVal>
<EnumVal ord="6"> Others</EnumVal>
</EnumType>
<EnumType id="UnBikMod">
<EnumVal ord="1">Otf</EnumVal>
<EnumVa! ord="2">Permanent</EnumVal>
<EnumVal ord="3">Time window</EnumVal>
</EnumType>
<EnumType id="WeiMod">
<EnumVal ord="1">Off</EnumVal>
<EnumVal ord="2">Operate</EnumVal>
<EnumVal ord="3">Echo</EnumVal>
<EnumVal ord="4">Echo and Operate</EnumVal>
</EnumType>
ry
a
https://ww.doc88.com/p-80980482981320.html 180/185
```


## File page 181

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
61850-7-4 © IEC:2010(E) -179-
Bibliography
IEC 60870-5-101, Telecontro! equipment and systems — Part 5-101: Transmission protocols —
Companion standard for basic telecontrol tasks
1EC 60870-5-103, Telecontrol equipment and systems — Part 5-103: Transmission protocols —
Companion standard for the informative interface of protection equipment
IEC 61000-4-30, Electromagnetic compatibility (EMC) — Part 4-30: Testing and measurement
techniques — Power quality measurement methods
IEC 61850-6, Communication networks and systems in substations — Part 6: Configuration
description language for communication in electrical substations related to IEDs
IEC 61850-7-410:2007, Communication networks and systems for power utility automation —
Part 7-410: Hydroelectric power plants - Communication for monitoring and control
IEC 61850-7-420, Communication networks and systems for power utility automation — Part 7-
420: Basic communication structure — Distributed energy resources logical nodes
EC 61850-8 (all parts), Communication networks and systems in substations — Part 8-x:
Specific communication service mapping (SCSM)
IEC 61850-9 (all parts), Communication networks and systems in substations - Part 9-x:
Specific communication service mapping (SCSM)
IEC 61850-10, Communication networks and systems in substations — Part 10: Conformance
testing
IEEE-SA TR 1550-1999, Utility Communications Architecture (UCA) Version 2.0, Part 4: UCA
Generic object models for substation and feeder equipment (GOMSFE)
CIGRE Report 34-03, Communication requirements in terms of data flow within substations,
December 1996
a
nw
https://www.doc88.com/p-80980482981320.html 181/185
```


## File page 182

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 1184 > @ Q_ View A mark Y Annotations ¥ Q
A
https://ww.doc88.com/p-80980482981320.html 182/185
```


## File page 183

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 1184 > @ Q_ View A mark Y Annotations ¥ Q
A
https://ww.doc88.com/p-80980482981320.html 183/185
```


## File page 184

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
INTERNATIONAL
ELECTROTECHNICAL
COMMISSION
3, rue de Varembé
PO Box 131
(CH-1211 Geneva 20
Switzerland
Tel: +41 229190211
Fax: + 41 22.919 03 00
into@iec.ch
www.iec.ch
“a
The full text reading has ended. Downloading this article requires [method/access].
198 points
https://www.doc88.com/p-80980482981320.html 184/185
```


## File page 185

```
9/18/26, 10:37 AM IEC 61850-7-4-2010 - Doc88
< 7184 > = @ Q_ View A mark Y Annotations v Q
document
Users who read this document also read these documents
IEC Standard IEC Standard National Standards IEC Specification _IEC lightning IEC standard
and IEC protection
Post a comment
Verification code: FDEP) change one CG anonymous comment
subm
about Us Help Center Follow us [OF
About Doc88 Website Statement Member Registration Sina Weibo
Talent Recruitment Site Map Document Download fa
Contact Us APP Download How to eam points Follow our Wec
Nn
https://www.doc88.com/p-8098048298 1 320.htm! 185/185,
```
