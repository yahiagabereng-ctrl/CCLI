# Datasheet filename aliases (user download → canonical)

| Canonical name | Original / alias on disk | MPN / topic |
|----------------|--------------------------|-------------|
| `WM1302_Seeed_LoRaWAN_Module.pdf` | `114992991.pdf` | WM1302 (verify vs schematic 4G modem) |
| `PCAQ7060MW-301_Prodtech_Spec.pdf` | `b74a8e8492ae61cd194d688b7ffc668e.pdf` | PCAQ7060 CM choke + **C89** |
| `SLB9673_Infineon_DataSheet_Rev1.4.pdf` | `infineon-slb9673-tpm20-i2c-fw26xx-ds-rev1-4-…pdf` | TPM 2.0 |
| `TLP291-4_Toshiba.pdf` | `TOSC-S-A0009571501-1.pdf` | DIO opto |
| `SK56L_MCC_Schottky.pdf` | `MCCC-S-A0018719587-1.pdf` | **SK56L** (schematic also **SK510L**) |
| `SMBJ_Series_Littelfuse.pdf` | `LFSI-S-A0010723737-1.pdf` | SMBJ18CA / SMBJ33CA family |
| `SMBJ_Series_DataSheet_N0193_RevD.pdf` | `SMBJ SERIES N0193 REV.D.pdf` | Alternate SMBJ series doc |

Rename with: `powershell -File scripts/rename-tg544-v2-datasheets.ps1`
