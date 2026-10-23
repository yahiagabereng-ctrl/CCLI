# Datasheet filename aliases (user download → canonical)

| Canonical name | Original / alias on disk | MPN / topic |
|----------------|--------------------------|-------------|
| `WM1302_Seeed_LoRaWAN_Module.pdf` | `114992991.pdf` | WM1302 (verify vs schematic 4G modem) |
| `PCAQ7060MW-301_Prodtech_Spec.pdf` | `b74a8e8492ae61cd194d688b7ffc668e.pdf` | PCAQ7060 CM choke + **C89** |
| `SLB9673_Infineon_DataSheet_Rev1.4.pdf` | `infineon-slb9673-tpm20-i2c-fw26xx-ds-rev1-4-…pdf` | TPM 2.0 |
| `TLP291-4_Toshiba.pdf` | `TOSC-S-A0009571501-1.pdf` | DIO opto |
| `SK56L_MCC_Schottky.pdf` | `MCCC-S-A0018719587-1.pdf` | **SK56L-TP** + **SK510L-TP** (one MCC family DS; no separate SK510 PDF needed) |
| `SMBJ_Series_Littelfuse.pdf` | `LFSI-S-A0010723737-1.pdf` | SMBJ18CA / SMBJ33CA family |
| `SMBJ_Series_DataSheet_N0193_RevD.pdf` | `SMBJ SERIES N0193 REV.D.pdf` | Alternate SMBJ series doc |
| `B0303S-1WR3_Mornsun.pdf` | `B_S-1WR3.pdf` (Mornsun series DS) | **B0303S-1WR3** RS485 iso power |
| `SK510AFL-TP_MCC_Schottky.pdf` | `SK510AFL-TP.pdf` (University) | **VERIFY** vs schematic **SK510L-TP** |
| `SK52L_MCC_Schottky.pdf` | `SK52L.PDF` | MCC Schottky family |
| `B3D090L-C_GDT.pdf` | `B3D090L-C.PDF` | GDT on RS485/DIO |
| `SP3222B_RS232_Anap.pdf` | `SP3222B.PDF` | **SP3232** substitute family DS |
| `K0805L_Series_Littelfuse_PTC.pdf` | `C2892412.pdf` (LCSC) | **K0805L050BR** USB **F1** |
| `MVR0805-470G_Varistor.pdf` | `MVR0805-470G.PDF` | **Not** **MVR1812-470G** on sheet |

Rename with: `powershell -File scripts/rename-tg544-v2-datasheets.ps1`
