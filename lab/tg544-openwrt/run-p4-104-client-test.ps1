# P4-02 — build lib60870 lab client and test ccli CS104 slave on Eth_B.
param(
    [string]$DutAddr = "192.168.1.130",
    [int]$Port = 2404,
    [int]$Ca = 1,
    [int]$IoaTotW = 1001,
    [switch]$SkipBuild
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$Lab104 = Join-Path $Repo "lab\104"
$Client = Join-Path $Lab104 "p4_cs104_client"
$BuildSh = Join-Path $Lab104 "wsl-build-p4-client.sh"
$EvidenceDir = Join-Path $Repo "lab\evidence\phase4"
$Stamp = Get-Date -Format "yyyy-MM-dd_HHmmss"
$LogFile = Join-Path $EvidenceDir "P4_02_104_SESSION_$Stamp.txt"

New-Item -ItemType Directory -Force -Path $EvidenceDir | Out-Null

function Log([string]$Msg) {
    $line = "[$(Get-Date -Format 'HH:mm:ss')] $Msg"
    Write-Host $line
    Add-Content -Path $LogFile -Value $line
}

Log "=== P4-02 CS104 client test (lib60870) ==="
Log "DUT=${DutAddr}:${Port} CA=${Ca} IOA=${IoaTotW}"

$ethIp = (Get-NetIPAddress -AddressFamily IPv4 -ErrorAction SilentlyContinue |
    Where-Object { $_.InterfaceAlias -eq "Ethernet" }).IPAddress
Log "PC Ethernet IP: $ethIp"
if ($ethIp -notmatch "^192\.168\.1\.") {
    Log "WARN: PC not on 192.168.1.0/24 — run lab\set_pc_lan2_oa.cmd as Admin"
}

if (-not $SkipBuild) {
    Log "Building p4_cs104_client in WSL..."
    $wslLab = wsl wslpath -a $Lab104
    wsl bash -c "cd '$wslLab' && sed -i 's/\r$//' wsl-build-p4-client.sh && bash wsl-build-p4-client.sh" 2>&1 | ForEach-Object { Log $_ }
}

if (-not (Test-Path $Client)) {
    throw "Missing $Client — build failed"
}

Log "Ping DUT..."
$ping = Test-Connection -ComputerName $DutAddr -Count 2 -Quiet
Log "Ping ${DutAddr}: $(if ($ping) { 'OK' } else { 'FAIL' })"

Log "Running client..."
$wslClient = wsl wslpath -a $Client
$wslOut = wsl bash -c "'$wslClient' '$DutAddr' '$Port' '$Ca' '$IoaTotW'" 2>&1
$exitCode = $LASTEXITCODE
foreach ($line in $wslOut) { Log $line }

if ($exitCode -eq 0) {
    Log "P4-02 VERDICT: PASS"
} else {
    Log "P4-02 VERDICT: FAIL (exit $exitCode)"
}

Copy-Item $LogFile (Join-Path $EvidenceDir "P4_02_104_SESSION.txt") -Force
Write-Host "`nEvidence: $LogFile" -ForegroundColor Cyan
exit $exitCode
