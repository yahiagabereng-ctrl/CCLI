# Download public TG-544 V2 component datasheets into repo datasheets folder.
$ErrorActionPreference = "Continue"
$Root = Split-Path -Parent $PSScriptRoot
$Dest = Join-Path $Root "knowledge-base/08-engineering/reference/vendor/tespro/2026-10-22-tg544-v2-schematic/datasheets"
New-Item -ItemType Directory -Force -Path $Dest | Out-Null

$Downloads = @(
    @{
        File = "CA-IS3721HS_Chipanalog_CA-IS372x_V1.08_en.pdf"
        Url  = "https://e.chipanalog.com/Public/Uploads/uploadfile/files/20250123/CAIS372xdatasheetVersion1.08en20241217.pdf"
    }
    @{
        File = "BL3085_I47_Belling_V2.3_cn.pdf"
        Url  = "https://www.belling.com.cn/media/file_object/bel_product/BL3085(I47)/datasheet/BL3085(I47)_V2.3_cn.pdf"
    }
    @{
        File = "HK3FF-DC5V-SHG_HUIKE.pdf"
        Url  = "https://www.lcsc.com/datasheet/C27840.pdf"
    }
    @{
        File = "SMBJ33CA_Littelfuse.pdf"
        Url  = "https://www.littelfuse.com/~/media/electronics/datasheets/tvs_diodes/littelfuse_tvs_diode_smbj_datasheet.pdf.pdf"
    }
    @{
        File = "SMBJ18CA_Littelfuse.pdf"
        Url  = "https://www.littelfuse.com/~/media/electronics/datasheets/tvs_diodes/littelfuse_tvs_diode_smbj_datasheet.pdf.pdf"
    }
    @{
        File = "SMDJ36CA_Littelfuse.pdf"
        Url  = "https://www.littelfuse.com/~/media/electronics/datasheets/tvs_diodes/littelfuse_tvs_diode_smdj_datasheet.pdf.pdf"
    }
    @{
        File = "TLP291_Toshiba.pdf"
        Url  = "https://toshiba.semicon-storage.com/info/lookup/Datasheet/TLP291-4-GB-TP,SE-e.pdf"
    }
    @{
        File = "GD32F330CBT6_GigaDevice.pdf"
        Url  = "https://www.gd32mcu.com/download/down/document_id/68/type/0"
    }
    @{
        File = "DMP10H400SK3-13_DiodesInc.pdf"
        Url  = "https://www.diodes.com/assets/Datasheets/DMP10H400SK3.pdf"
    }
    @{
        File = "SP3232_Sipex_MaxLinear.pdf"
        Url  = "https://www.analog.com/media/en/technical-documentation/data-sheets/SP3232E.pdf"
    }
    @{
        File = "B0303S-1WR3_Mornsun.pdf"
        Url  = "https://www.mornsun-power.com/html/pdf/B0303S-1WR3.pdf"
    }
    @{
        File = "SLB9673_Infineon_FIPS_policy_ref.pdf"
        Url  = "https://www.infineon.com/dgdl/Infineon-SLB_9673-DataSheet-v01_00-EN.pdf?fileId=5546d462533600a4015355d1e2e01f7"
    }
    @{
        File = "K0805L050BR_Littelfuse.pdf"
        Url  = "https://www.littelfuse.com/~/media/electronics/datasheets/fuses/littelfuse_fuse_1206lt_k0603lt_datasheet.pdf.pdf"
    }
)

$log = @()
foreach ($d in $Downloads) {
    $out = Join-Path $Dest $d.File
    Write-Host "GET $($d.File) ..."
    try {
        Invoke-WebRequest -Uri $d.Url -OutFile $out -UseBasicParsing -TimeoutSec 120 -MaximumRedirection 5
        $size = (Get-Item $out).Length
        if ($size -lt 5000) {
            $log += "WARN small file ($size B): $($d.File)"
            Write-Host "  WARN: only $size bytes" -ForegroundColor Yellow
        }
        else {
            $log += "OK $($d.File) ($size B)"
            Write-Host "  OK $size bytes" -ForegroundColor Green
        }
    }
    catch {
        $log += "FAIL $($d.File): $($_.Exception.Message)"
        Write-Host "  FAIL: $($_.Exception.Message)" -ForegroundColor Red
    }
}

$manifest = Join-Path $Dest "DOWNLOAD_MANIFEST_$(Get-Date -Format yyyy-MM-dd).txt"
$log | Set-Content -Path $manifest -Encoding UTF8
Write-Host "`nManifest: $manifest"
