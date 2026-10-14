# Validate a CID against the 5-LN O.15 certificate model (PRODUCT_SPECIMEN_5LN).
# Usage:
#   powershell -File scripts\validate-5ln-cid.ps1
#   powershell -File scripts\validate-5ln-cid.ps1 -CidPath apps\ccli\config\icd\product\product_tr400_eth_a_5ln.cid

param(
    [string]$CidPath = ""
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
if (-not $CidPath) {
    $CidPath = Join-Path $Repo "apps\ccli\config\icd\product\product_tr400_eth_a_5ln.cid"
} elseif (-not [System.IO.Path]::IsPathRooted($CidPath)) {
    $CidPath = Join-Path $Repo ($CidPath -replace '/', '\')
}

function Get-MmsLnName {
    param($LnNode)
    $prefix = $LnNode.GetAttribute("prefix")
    $cls = $LnNode.GetAttribute("lnClass")
    $inst = $LnNode.GetAttribute("inst")
    if (-not $inst) { $inst = "1" }
    return "$prefix$cls$inst"
}

$requiredLns = @(
    "LPHD1",
    "PdCMMXU1",
    "WlimDWMX1",
    "WSdDAGC1"
)

$forbiddenLnPrefixes = @(
    "VArSd", "PFSP", "VArV", "PFW", "GenPV", "GenWi", "GenTer", "GenIdr",
    "DisFR", "PdC_Wi", "PdC_Wa", "PdC_Qi", "PdC_Qc", "PdC_VA", "SSGG", "SGG", "IDG", "St"
)

$errors = @()
$warnings = @()

if (-not (Test-Path $CidPath)) {
    Write-Host "FAIL: CID not found: $CidPath" -ForegroundColor Red
    Write-Host "Create product 5-LN CID before cert Compare (U-T1-02). See lab/conformance/PRODUCT_SPECIMEN_5LN.md"
    exit 2
}

[xml]$doc = Get-Content -Raw -Encoding UTF8 $CidPath
$ns = New-Object System.Xml.XmlNamespaceManager($doc.NameTable)
$ns.AddNamespace("scl", "http://www.iec.ch/61850/2003/SCL")

$iedName = $doc.SelectSingleNode("//scl:IED", $ns)
if ($iedName) { $iedName = $iedName.GetAttribute("name") } else { $iedName = "(unknown)" }

$lnNodes = $doc.SelectNodes("//scl:LDevice/scl:LN", $ns)
$ln0 = $doc.SelectNodes("//scl:LDevice/scl:LN0", $ns)
if (-not $ln0 -or $ln0.Count -lt 1) {
    $errors += "Missing LN0 (LLN0) under LDevice"
}

$found = @()
foreach ($ln in $lnNodes) {
    $name = Get-MmsLnName $ln
    $found += $name
    foreach ($bad in $forbiddenLnPrefixes) {
        if ($name -like "$bad*") {
            $errors += "Forbidden LN for 5-LN cert model: $name"
        }
    }
}

$missing = @()
foreach ($req in $requiredLns) {
    if ($found -notcontains $req) { $missing += $req }
}
if ($missing.Count -gt 0) {
    $errors += "Missing required LNs: $($missing -join ', ')"
}

if ($found.Count -ne 4) {
    $errors += "Expected exactly 4 <LN> under LDevice (LPHD + PdC + Wlim + WSd); found $($found.Count): $($found -join ', ')"
}

$extra = $found | Where-Object { $_ -notin $requiredLns }
if ($extra.Count -gt 0) {
    $errors += "Unexpected LNs (trim CID): $($extra -join ', ')"
}

# LLN0 reporting: only cert URCB (unbuffered)
$urcbNames = @()
$brcbNames = @()
foreach ($rc in $doc.SelectNodes("//scl:ReportControl", $ns)) {
    $n = $rc.GetAttribute("name")
    if ($rc.GetAttribute("buffered") -eq "true") { $brcbNames += $n }
    else { $urcbNames += $n }
}
foreach ($u in $urcbNames) {
    if ($u -ne "urcb_PdC_Mis4sec") {
        $errors += "5-LN cert CID should only expose urcb_PdC_Mis4sec (unbuffered); found: $u"
    }
}
if ($urcbNames -notcontains "urcb_PdC_Mis4sec") {
    $errors += "Missing ReportControl urcb_PdC_Mis4sec on LLN0"
}
if ($brcbNames.Count -gt 0) {
    $errors += "Buffered ReportControl present (remove for 5-LN cert CID): $($brcbNames -join ', ')"
}

$gse = $doc.SelectNodes("//scl:GSEControl", $ns)
if ($gse -and $gse.Count -gt 0) {
    $errors += "GSEControl present ($($gse.Count)) - remove for 5-LN cert CID (PICS GOOSE N/A)"
}

$ds = $doc.SelectNodes("//scl:DataSet[@name='DS_R_PdC_Mis4sec']", $ns)
if (-not $ds -or $ds.Count -lt 1) {
    $errors += "Missing DataSet DS_R_PdC_Mis4sec on LLN0"
}

Write-Host "5-LN CID validation: $CidPath" -ForegroundColor Cyan
Write-Host "IED: $iedName"
$mmsCount = 1 + $found.Count
Write-Host "MMS logical nodes: LLN0 + $($found.Count) LN(s) = $mmsCount (expect 5 for cert)"

foreach ($w in $warnings) { Write-Host "WARN: $w" -ForegroundColor Yellow }
foreach ($e in $errors) { Write-Host "FAIL: $e" -ForegroundColor Red }

if ($errors.Count -gt 0) {
    Write-Host ""
    Write-Host "Result: FAIL ($($errors.Count) error(s))" -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "Result: PASS (5-LN certificate model)" -ForegroundColor Green
exit 0
