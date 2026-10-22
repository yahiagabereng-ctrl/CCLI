# Validate full bench HW: EMT432 TCP 1p, USB RS485 Sun mock, DIO relay/key, two-PC roles.
# Canonical yaml: apps/ccli/config/product_tr400.yaml
#
#   . .\lab\tg544-openwrt\lab-env.ps1
#   powershell -File scripts\validate-bench-hw-configuration.ps1 -RemoteDut -WriteEvidence
#   powershell -File scripts\validate-bench-hw-configuration.ps1 -ProbeMeter -WriteEvidence

param(
    [string]$DutHost = "192.168.1.130",
    [string]$MeterHost = "192.168.178.10",
    [string]$DutLan1 = "192.168.10.1",
    [string]$InverterCom = "COM5",
    [string]$ConfigYaml = "lab_tr400_emt432_lan3_tcp_102.yaml",
    [switch]$RemoteDut,
    [switch]$ProbeMeter,
    [switch]$WriteEvidence
)

$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
$date = Get-Date -Format "yyyy-MM-dd"
$ts = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
$Plink = "C:\Program Files\PuTTY\plink.exe"
$HostKey = "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE"

$canonical = Join-Path $Repo "apps\ccli\config\$ConfigYaml"
if (-not (Test-Path $canonical)) {
    $canonical = Join-Path $Repo "apps\ccli\config\product_tr400.yaml"
}
if (-not (Test-Path $canonical)) { throw "Missing config yaml: $ConfigYaml or product_tr400.yaml" }

function Test-LocalIp([string]$Prefix) {
    @(Get-NetIPAddress -AddressFamily IPv4 -ErrorAction SilentlyContinue |
        Where-Object { $_.IPAddress -like ($Prefix + "*") } |
        Select-Object -ExpandProperty IPAddress)
}

function Invoke-Dut([string]$Cmd) {
    if (-not $env:CCLI_TG544_PW -or -not (Test-Path $Plink)) { return "" }
    $prev = $ErrorActionPreference
    $ErrorActionPreference = "Continue"
    $out = (& $Plink -ssh "root@${DutHost}" -pw $env:CCLI_TG544_PW -hostkey $HostKey -batch $Cmd 2>&1 | Out-String).Trim()
    $ErrorActionPreference = $prev
    return $out
}

function Test-PingHost([string]$Target) {
    try {
        $p = New-Object System.Net.NetworkInformation.Ping
        return ($p.Send($Target, 1500).Status -eq "Success")
    } catch {
        return $false
    }
}

$checks = [ordered]@{}
$yamlText = Get-Content -Raw $canonical

$meterIpPattern = [regex]::Escape($MeterHost)
$checks["SPEC: Modbus TCP EMT432 1p"] =
    ($yamlText -match "backend:\s*tcp") -and ($yamlText -match $meterIpPattern) -and
    ($yamlText -match "reg_active_power:\s*932") -and ($yamlText -match "reg_line_voltage:\s*905")
$checks["SPEC: DIO1 relay + DIO2 key"] =
    ($yamlText -match "gpio:\s*1") -and ($yamlText -match "gpio:\s*2") -and
    ($yamlText -match "permissive_bypass:\s*false")
$checks["SPEC: Sun mock COM5 to ttyS1"] =
    ($yamlText -match "COM5") -and ($yamlText -match "/dev/ttyS1") -and ($yamlText -match "ccli_polls:\s*false")
$checks["SPEC: MMS 3782 + 104 2404 TLS"] =
    ($yamlText -match "192\.168\.10\.1") -and ($yamlText -match "3782") -and
    ($yamlText -match "192\.168\.1\.130") -and ($yamlText -match "2404")
$checks["SPEC: 5-LN model_cfg"] = $yamlText -match "product_tr400_eth_a_5ln\.cfg"
$checks["SPEC: use_dso_mock false"] = $yamlText -match "use_dso_mock:\s*false"
if ($ConfigYaml -match "emt432") {
    $checks["SPEC: Rogowski 85 mV/kA metadata"] =
        ($yamlText -match "rogowski_sensitivity_mv_per_ka:\s*85") -and
        ($yamlText -match "rogowski_reg_40235_v_per_a:\s*0\.000085")
    $checks["SPEC: EMT432 cert register pin doc"] =
        $yamlText -match "EMT432_CERT_REGISTER_PIN\.md"
}

$lan2 = Test-LocalIp "192.168.1."
$lan3 = Test-LocalIp "192.168.178."
$lan1 = Test-LocalIp "192.168.10."
# USB-RS485 often appears in GetPortNames but not Win32_SerialPort (PnP timing).
$comPorts = @(
    [System.IO.Ports.SerialPort]::GetPortNames()
    Get-CimInstance Win32_SerialPort -ErrorAction SilentlyContinue | ForEach-Object { $_.DeviceID }
) | Select-Object -Unique
if ($yamlText -match "pc_usb_rs485:\s*(\S+)") {
    $InverterCom = $Matches[1]
}

$checks["PC-A: LAN2 192.168.1.x"] = ($lan2.Count -gt 0)
$checks["PC-A: LAN3 192.168.178.x"] = ($lan3.Count -gt 0)
$checks["PC-B: LAN1 192.168.10.x on this PC"] = ($lan1.Count -gt 0)
$checks["PC-A: ping DUT LAN2"] = if ($lan2.Count -gt 0) { Test-PingHost $DutHost } else { $false }
$checks["PC-B path: ping DUT LAN1"] = Test-PingHost $DutLan1
$checks["Plant: ping EMT432"] = Test-PingHost $MeterHost
$checks["PC-A: USB RS485 $InverterCom"] = ($InverterCom -in $comPorts)

$dutSection = ""
if ($RemoteDut -and $env:CCLI_TG544_PW) {
    $ver = Invoke-Dut "/usr/sbin/ccli --version | head -1"
    $live = Invoke-Dut "grep backend /etc/ccli/lab.yaml; grep host /etc/ccli/lab.yaml; grep device /etc/ccli/lab.yaml; grep use_dso_mock /etc/ccli/lab.yaml; grep model_cfg /etc/ccli/lab.yaml"
    $modLog = Invoke-Dut "logread -e ccli | grep modbus | tail -3"
    $gd32 = Invoke-Dut "ubus call dido_v2 gd32.status"
    $modes = Invoke-Dut "uci get didoservice_v2.channel_1.mode; uci get didoservice_v2.channel_2.mode"

    $checks["DUT: modbus backend tcp"] = ($live -match "backend:\s*tcp") -and ($live -match [regex]::Escape($MeterHost))
    $checks["DUT: use_dso_mock false"] = ($live -match "use_dso_mock:\s*false") -or ($live -notmatch "use_dso_mock:\s*true")
    $checks["DUT: 5-LN model_cfg"] = $live -match "product_tr400_eth_a_5ln"
    $checks["DUT: modbus no read_fail"] = $modLog -notmatch "read_fail"
    $checks["DUT: GD32 online"] = $gd32 -match "online"
    $checks["DUT: ch1 DO ch2 DI"] = ($modes -match "do") -and ($modes -match "di")
    $checks["DUT: ccli r54+"] = $ver -match "r5[4-9]|r[6-9]"

    $dutSection = "DUT version: $ver`nDUT yaml:`n$live`nmodbus:`n$modLog`nuci:`n$modes`n"
}

$probeSection = ""
if ($ProbeMeter) {
    $probeScript = Join-Path $Repo "lab\modbus_emt432_tcp_probe.py"
    if (Test-Path $probeScript) {
        if (Test-PingHost $MeterHost) {
            $probeOut = & python $probeScript --host $MeterHost 2>&1 | Out-String
            $checks["METER: TCP probe"] = ($probeOut -match "OK|P1|V_L1") -and ($probeOut -notmatch "timeout")
            $probeSection = "Meter probe:`n$probeOut"
        } else {
            $checks["METER: TCP probe"] = $false
            $probeSection = "Meter probe skipped: no ping to $MeterHost"
        }
    }
}

Write-Host ""
Write-Host "=== Bench HW validation $ts ===" -ForegroundColor Cyan
Write-Host "This PC: LAN2=$($lan2 -join ',') LAN3=$($lan3 -join ',') LAN1=$($lan1 -join ',') COM=$($comPorts -join ',')"
Write-Host ""

$fail = 0
foreach ($k in $checks.Keys) {
    $ok = $checks[$k]
    if (-not $ok) { $fail++ }
    $tag = if ($ok) { "PASS" } else { "FAIL" }
    Write-Host "[$tag] $k" -ForegroundColor $(if ($ok) { "Green" } else { "Red" })
}

$lines = New-Object System.Collections.Generic.List[string]
$lines.Add("# BENCH_HW_VALIDATION - $date")
$lines.Add("generated: $ts")
$lines.Add("canonical: apps/ccli/config/$ConfigYaml")
$lines.Add("meter_host: $MeterHost")
$lines.Add("topology: PC-B DSO LAN1 TSP | PC-A logger LAN2+LAN3+COM5 | EMT TCP 1p/Rogowski | Sun A1/B1 | DIO1 relay DIO2 key")
$lines.Add("")
foreach ($k in $checks.Keys) {
    $mark = if ($checks[$k]) { "x" } else { " " }
    $lines.Add("- [$mark] $k")
}
if ($dutSection) { $lines.Add(""); $lines.Add($dutSection) }
if ($probeSection) { $lines.Add(""); $lines.Add($probeSection) }

if ($WriteEvidence) {
    $outDir = Join-Path $Repo "lab\evidence\hw"
    New-Item -ItemType Directory -Force -Path $outDir | Out-Null
    $out = Join-Path $outDir "BENCH_HW_VALIDATION_${date}.txt"
    Set-Content -Path $out -Value ($lines -join "`n") -Encoding UTF8
    Write-Host ""
    Write-Host "Wrote $out" -ForegroundColor Green
}

Write-Host ""
if ($fail -gt 0) {
    Write-Host "VERDICT: PART ($fail failed). Deploy product_tr400.yaml + r55; LAN3 178.x; COM5 Sun; PC-B LAN1 for TSP." -ForegroundColor Yellow
    exit 1
}
Write-Host "VERDICT: PASS" -ForegroundColor Green
exit 0
