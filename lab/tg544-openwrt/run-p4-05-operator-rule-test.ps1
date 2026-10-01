# P4-05 - O.14 param audit: monitor_only reject + command mode MSD accept (r20+).

param(

    [string]$DutAddr = "192.168.1.130",

    [string]$DeployHost = "192.168.1.130",

    [switch]$SkipDeploy,

    [switch]$SkipBuild,

    [switch]$SkipRestoreMonitor

)



$ErrorActionPreference = "Stop"

$Repo = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent

$Lab104 = Join-Path $Repo "lab\104"

$Client = Join-Path $Lab104 "p4_cs104_client"

$DeployScript = Join-Path $PSScriptRoot "deploy-ccli-session-fix.ps1"

$MonitorYaml = Join-Path $Repo "apps\ccli\config\lab_tr400_phase4_eth_b.yaml"

$CommandYaml = Join-Path $Repo "apps\ccli\config\lab_tr400_phase4_eth_b_command.yaml"

$EvidenceDir = Join-Path $Repo "lab\evidence\phase4"

$Stamp = Get-Date -Format "yyyy-MM-dd_HHmmss"

$LogFile = Join-Path $EvidenceDir "P4_05_OPERATOR_RULE_VERIFY_$Stamp.txt"

$Canonical = Join-Path $EvidenceDir "P4_05_OPERATOR_RULE_VERIFY.txt"



$Plink = "C:\Program Files\PuTTY\plink.exe"

$Pscp = "C:\Program Files\PuTTY\pscp.exe"

$HostKey = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"

$User = "root"



New-Item -ItemType Directory -Force -Path $EvidenceDir | Out-Null



function Log([string]$Msg) {

    $line = "[$(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')] $Msg"

    Write-Host $line

    Add-Content -Path $LogFile -Value $line

}



function LogBlock([string]$Title, [string]$Body) {

    Log ""

    Log "=== $Title ==="

    foreach ($line in ($Body -split "`n")) {

        if ($line.Trim() -ne "") { Log $line.TrimEnd() }

    }

}



function Restart-CcliOnDut {
    param(
        [string]$YamlPath,
        [string]$Pw
    )
    Log "Restart ccli: $(Split-Path $YamlPath -Leaf)"
    & $Pscp -scp -batch -pw $Pw -hostkey $HostKey $YamlPath "${User}@${DeployHost}:/etc/ccli/lab.yaml"
    & $Plink -ssh "${User}@${DeployHost}" -pw $Pw -hostkey $HostKey -batch "killall ccli 2>/dev/null; sleep 1"
    & $Plink -ssh "${User}@${DeployHost}" -pw $Pw -hostkey $HostKey -batch `
        "/usr/sbin/ccli --config /etc/ccli/lab.yaml >>/tmp/ccli.log 2>&1 &"
    Start-Sleep -Seconds 5
    $modeLine = & $Plink -ssh "${User}@${DeployHost}" -pw $Pw -hostkey $HostKey -batch `
        "grep 'Operating Rule mode=' /tmp/ccli.log | tail -1"
    Log $modeLine
    $listening = & $Plink -ssh "${User}@${DeployHost}" -pw $Pw -hostkey $HostKey -batch `
        "netstat -tln 2>/dev/null | grep 2404 || true"
    if (-not ($listening -match "2404")) {
        Log "WARN: :2404 not listening after restart"
    }
}



function Get-DutO14Log {
    param([string]$Pw, [string]$Marker)
    $cmd = "grep -E '${Marker}|O\.14 type=' /tmp/ccli.log | tail -15"
    return (& $Plink -ssh "${User}@${DeployHost}" -pw $Pw -hostkey $HostKey -batch $cmd 2>&1)
}



Log '# P4-05 - O.14 command audit with parameters (reject + accept)'

Log "Build: ccli-0.1.0-r20 | DUT=$DutAddr"



if (-not $SkipDeploy) {

    if (-not $env:CCLI_TG544_PW) { Write-Error "Set `$env:CCLI_TG544_PW for deploy." }

    Log "Deploying r20 + monitor_only lab.yaml ..."

    $prevEap = $ErrorActionPreference

    $ErrorActionPreference = "Continue"

    & $DeployScript -HostAddr $DeployHost -LabYaml $MonitorYaml -Changes @(

        "O.14 audit detail type+ioa+value+result",

        "command mode MSD stub O.10.3.1",

        "lab_tr400_phase4_eth_b_command.yaml"

    ) 2>&1 | ForEach-Object { Log $_ }

    $ErrorActionPreference = $prevEap

}



if (-not $SkipBuild) {

    Log "Building p4_cs104_client ..."

    $prevEap = $ErrorActionPreference

    $ErrorActionPreference = "Continue"

    wsl bash -lc "cd /mnt/c/Yahia/projects/CCLI/lab/104; bash wsl-build-p4-client.sh" 2>&1 | ForEach-Object { Log $_ }

    $ErrorActionPreference = $prevEap

}



if (-not (Test-Path $Client)) { throw "Missing $Client" }



$pw = $env:CCLI_TG544_PW

$rejectLog = ""

$acceptLog = ""



# --- Phase A: monitor_only reject with parameters ---

Log "Phase A: monitor_only --reject-test ..."

if ($pw -and (Test-Path $Plink)) {

    Restart-CcliOnDut -YamlPath $MonitorYaml -Pw $pw

    & $Plink -ssh "${User}@${DeployHost}" -pw $pw -hostkey $HostKey -batch `

        "echo '--- P4-05 reject-test ---' >> /tmp/ccli.log" | Out-Null

}

$ErrorActionPreference = "Continue"

$rejectOut = wsl bash -lc "/mnt/c/Yahia/projects/CCLI/lab/104/p4_cs104_client '$DutAddr' 2404 1 1001 --tls --reject-test" 2>&1

$rejectExit = $LASTEXITCODE

$ErrorActionPreference = "Stop"

LogBlock "Reject-test client" ($rejectOut -join "`n")

Start-Sleep -Seconds 2

if ($pw -and (Test-Path $Plink)) {

    $rejectLog = Get-DutO14Log -Pw $pw -Marker "reject-test"

    LogBlock "DUT log (reject)" ($rejectLog -join "`n")

}



$hasRejectParams = ($rejectLog -join "`n") -match "O\.14 type=45 ioa=2001.*result=reject"



# --- Phase B: command mode MSD accept ---

Log "Phase B: command mode --accept-test (IOA 3001, 42.5 kW) ..."

if ($pw -and (Test-Path $Plink)) {

    Restart-CcliOnDut -YamlPath $CommandYaml -Pw $pw

    & $Plink -ssh "${User}@${DeployHost}" -pw $pw -hostkey $HostKey -batch `

        "echo '--- P4-05 accept-test ---' >> /tmp/ccli.log" | Out-Null

}

$ErrorActionPreference = "Continue"

$acceptOut = wsl bash -lc "/mnt/c/Yahia/projects/CCLI/lab/104/p4_cs104_client '$DutAddr' 2404 1 1001 --tls --accept-test --msd-ioa 3001 --msd-kw 42.5" 2>&1

$acceptExit = $LASTEXITCODE

$ErrorActionPreference = "Stop"

LogBlock "Accept-test client" ($acceptOut -join "`n")

Start-Sleep -Seconds 2

if ($pw -and (Test-Path $Plink)) {

    $acceptLog = Get-DutO14Log -Pw $pw -Marker "accept-test|MSD set-point"

    LogBlock "DUT log (accept)" ($acceptLog -join "`n")

}



$hasAcceptParams = ($acceptLog -join "`n") -match "O\.14 type=50 ioa=3001 value=42\.500.*result=accept"

$hasMsdStub = ($acceptLog -join "`n") -match "O\.10\.3\.1 MSD set-point accepted"



# --- Restore monitor_only profile ---

if (-not $SkipRestoreMonitor -and $pw -and (Test-Path $Plink)) {

    Log "Restoring monitor_only lab.yaml on DUT ..."

    Restart-CcliOnDut -YamlPath $MonitorYaml -Pw $pw

}



$pass = ($rejectExit -eq 0) -and ($acceptExit -eq 0) -and $hasRejectParams -and $hasAcceptParams -and $hasMsdStub

Log ""

if ($pass) {

    Log "P4-05 VERDICT: PASS (O.14 params on reject + accept; MSD lab stub)"

} else {

    Log "P4-05 VERDICT: FAIL rejectExit=$rejectExit acceptExit=$acceptExit rejectParams=$hasRejectParams acceptParams=$hasAcceptParams msdStub=$hasMsdStub"

}



Copy-Item $LogFile $Canonical -Force

Write-Host ""

Write-Host "Evidence: $Canonical" -ForegroundColor Cyan

exit $(if ($pass) { 0 } else { 1 })


