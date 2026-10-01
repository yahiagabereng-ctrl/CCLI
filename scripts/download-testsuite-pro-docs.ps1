# Download / refresh Triangle MicroWorks Test Suite Pro public docs into CCLI corpus.
# Does NOT download the commercial installer (manual eval only).
param(
    [switch] $SkipFetch
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
$Raw = Join-Path $Repo "knowledge-base\08-engineering\testsuite-pro\raw"
New-Item -ItemType Directory -Path $Raw -Force | Out-Null
$Stamp = Get-Date -Format "yyyy-MM-dd"

$Pages = @(
    @{ Id = "overview"; Url = "https://www.trianglemicroworks.com/products/testing-and-configuration-tools/61850-test-suite-pro-pages/overview" },
    @{ Id = "system-requirements"; Url = "https://www.trianglemicroworks.com/products/testing-and-configuration-tools/61850-test-suite-pro-pages/system-requirements" },
    @{ Id = "whats-new"; Url = "https://www.trianglemicroworks.com/products/testing-and-configuration-tools/61850-test-suite-pro-pages/whats-new" },
    @{ Id = "test-sequencer"; Url = "https://www.trianglemicroworks.com/products/testing-and-configuration-tools/61850-test-suite-pro-pages/test-sequencer" },
    @{ Id = "report-viewer"; Url = "https://www.trianglemicroworks.com/products/testing-and-configuration-tools/61850-test-suite-pro-pages/report-viewer" },
    @{ Id = "downloads"; Url = "https://trianglemicroworks.com/products/downloads" }
)

Write-Host "Raw dir: $Raw"
Write-Host "Online Help (manual): copy install HTML to knowledge-base\08-engineering\testsuite-pro\help-offline\"
Write-Host "Eval installer: https://trianglemicroworks.com/products/downloads (do not commit)"

if ($SkipFetch) {
    Write-Host "SkipFetch set - listing existing raw files only"
    Get-ChildItem $Raw -File | Select-Object Name, Length, LastWriteTime
    exit 0
}

$reScript = '(?s)<script.*?</script>'
$reStyle = '(?s)<style.*?</style>'
$reTag = '<[^>]+>'

foreach ($p in $Pages) {
    $out = Join-Path $Raw ($p.Id + "_" + $Stamp + ".txt")
    try {
        Write-Host ("GET " + $p.Url)
        $resp = Invoke-WebRequest -Uri $p.Url -UseBasicParsing -TimeoutSec 60
        $text = [string]$resp.Content
        $text = [regex]::Replace($text, $reScript, " ")
        $text = [regex]::Replace($text, $reStyle, " ")
        $text = [regex]::Replace($text, $reTag, " ")
        $text = [regex]::Replace($text, "\s+", " ").Trim()
        $header = "# source: $($p.Url)`r`n# captured: $Stamp`r`n# id: $($p.Id)`r`n`r`n"
        Set-Content -Path $out -Value ($header + $text) -Encoding UTF8
        $len = (Get-Item $out).Length
        Write-Host ("  wrote " + $out + " bytes=" + $len)
    }
    catch {
        Write-Warning ("Failed " + $p.Id + ": " + $_)
    }
}

Write-Host "Done. Next: powershell -File scripts/ingest-testsuite-pro.ps1"
