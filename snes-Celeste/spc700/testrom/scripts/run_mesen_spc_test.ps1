param(
    [string]$Compiler = "llvm-mos",
    [string]$MesenPath = "",
    [bool]$CloseOnFinish = $false,
    [int]$TimeoutSeconds = 120
)

$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent $PSScriptRoot
Set-Location $Root
$SpcDir = Split-Path -Parent $Root
$ProjectRoot = Split-Path -Parent $SpcDir
$env:SPC_TEST_ROOT = ((Resolve-Path $Root).Path -replace '\\', '/')
$env:SNES_CELESTE_ROOT = ((Resolve-Path $ProjectRoot).Path -replace '\\', '/')

function Convert-ToGitBashPath([string]$Path) {
    $full = ((Resolve-Path $Path).Path -replace '\\', '/')
    if ($full -match '^([A-Za-z]):/(.*)$') {
        return "/" + $matches[1].ToLowerInvariant() + "/" + $matches[2]
    }
    return $full
}

$GitBash = if (![string]::IsNullOrWhiteSpace($env:GIT_BASH)) { $env:GIT_BASH } else { "C:\Program Files\Git\bin\bash.exe" }
if (!(Test-Path $GitBash)) { throw "Git Bash not found: $GitBash" }

Write-Host "[1/4] Build SPC payload" -ForegroundColor Cyan
Set-Location ..
$SpcDirBash = Convert-ToGitBashPath $SpcDir
& $GitBash -lc "cd '$SpcDirBash' && bash build.sh"
if ($LASTEXITCODE -ne 0) { throw "SPC build failed" }

Set-Location $Root
& python tools\gen_spc_payload.py
if ($LASTEXITCODE -ne 0) { throw "Payload generation failed" }

Write-Host "[2/4] Build ROM ($Compiler)" -ForegroundColor Cyan
& make COMPILER=$Compiler
if ($LASTEXITCODE -ne 0) { throw "ROM build failed" }

$rom = Join-Path $Root "build\mainBankZero_${Compiler}.smc"
if (!(Test-Path $rom)) { throw "ROM not found: $rom" }

Write-Host "[3/4] Resolve Mesen executable" -ForegroundColor Cyan
if ([string]::IsNullOrWhiteSpace($MesenPath)) {
    if (![string]::IsNullOrWhiteSpace($env:MESEN_PATH)) {
        $MesenPath = $env:MESEN_PATH
    }
}
if ([string]::IsNullOrWhiteSpace($MesenPath)) {
    $candidates = @(
        "C:\Program Files\Mesen2\Mesen.exe",
        "C:\Program Files\Mesen\Mesen.exe"
    )
    foreach ($c in $candidates) {
        if (Test-Path $c) { $MesenPath = $c; break }
    }
}
if ([string]::IsNullOrWhiteSpace($MesenPath) -or !(Test-Path $MesenPath)) {
    throw "Mesen executable not found. Pass -MesenPath explicitly."
}

$lua = Join-Path $Root "scripts\mesen_spc_test.lua"
 $resultPath = Join-Path $Root "spc_test_result.txt"
if (Test-Path $resultPath) { Remove-Item -LiteralPath $resultPath -Force }
Write-Host "[4/4] Run Mesen test" -ForegroundColor Cyan
$args = @(
    "--fullscreen=0",
    "--lua", "$lua",
    "$rom"
)

if ($CloseOnFinish) {
    $env:SPC_TEST_AUTOCLOSE = "1"
    # Ensure deterministic automation; Mesen single-instance mode can hijack launch.
    Get-Process | Where-Object { $_.ProcessName -like "Mesen*" } | ForEach-Object {
        try { Stop-Process -Id $_.Id -Force -ErrorAction Stop } catch { }
    }
    Start-Sleep -Milliseconds 250

    $before = @(Get-Process | Where-Object { $_.ProcessName -like "Mesen*" } | Select-Object -ExpandProperty Id)
    $proc = Start-Process -FilePath $MesenPath -ArgumentList $args -PassThru
    $deadline = (Get-Date).AddSeconds($TimeoutSeconds)
    $txt = "RUNNING"
    while ((Get-Date) -lt $deadline) {
        Start-Sleep -Milliseconds 250
        if (Test-Path $resultPath) {
            $txt = (Get-Content -Raw $resultPath).Trim()
            if ($txt -ne "RUNNING" -and $txt.Length -gt 0) { break }
        }
    }
    $after = @(Get-Process | Where-Object { $_.ProcessName -like "Mesen*" } | Select-Object -ExpandProperty Id)
    $newPids = $after | Where-Object { $_ -notin $before }
    foreach ($procId in $newPids) {
        try { Stop-Process -Id $procId -Force -ErrorAction Stop } catch { }
    }
    if (!(Test-Path $resultPath)) { throw "SPC test result not generated (timeout)." }
    $txt = (Get-Content -Raw $resultPath).Trim()
    Write-Host "SPC test result: $txt"
    if ($txt -notlike "PASS*") { throw "SPC test failed: $txt" }
    exit 0
} else {
    $env:SPC_TEST_AUTOCLOSE = "0"
    $proc = Start-Process -FilePath $MesenPath -ArgumentList $args -PassThru
    Write-Host "Mesen launched windowed and left open (PID: $($proc.Id))."
    exit 0
}
