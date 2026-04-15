param(
    [string]$Compiler = "llvm-mos",
    [string]$MesenPath = "",
    [bool]$CloseOnFinish = $false
)

$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent $PSScriptRoot
Set-Location $Root

$GitBash = "C:\Program Files\Git\bin\bash.exe"
if (!(Test-Path $GitBash)) { throw "Git Bash not found: $GitBash" }

Write-Host "[1/4] Build SPC payload" -ForegroundColor Cyan
Set-Location ..
& $GitBash -lc "cd /c/Users/Admin/Documents/snes-homebrew/snes-Celeste/spc700 && bash build.sh"
if ($LASTEXITCODE -ne 0) { throw "SPC build failed" }

Set-Location $Root
& "C:\Python37\python.exe" tools\gen_spc_payload.py
if ($LASTEXITCODE -ne 0) { throw "Payload generation failed" }

Write-Host "[2/4] Build ROM ($Compiler)" -ForegroundColor Cyan
& make COMPILER=$Compiler
if ($LASTEXITCODE -ne 0) { throw "ROM build failed" }

$rom = Join-Path $Root "build\mainBankZero_${Compiler}.smc"
if (!(Test-Path $rom)) { throw "ROM not found: $rom" }

Write-Host "[3/4] Resolve Mesen executable" -ForegroundColor Cyan
if ([string]::IsNullOrWhiteSpace($MesenPath)) {
    $candidates = @(
        "C:\Program Files\Mesen2\Mesen.exe",
        "C:\Program Files\Mesen\Mesen.exe",
        "C:\Users\Admin\Downloads\Mesen_2.1.0_Windows\Mesen.exe",
        "C:\Users\Admin\Downloads\Mesen_2.1.0_Windows(1)\Mesen.exe",
        "C:\Users\Admin\Documents\Mesen2\Backups\Mesen.2.1.1.exe",
        "C:\Users\Admin\Documents\Mesen2\Backups\Mesen.2.1.0.b21e273.exe"
    )
    foreach ($c in $candidates) {
        if (Test-Path $c) { $MesenPath = $c; break }
    }
}
if ([string]::IsNullOrWhiteSpace($MesenPath) -or !(Test-Path $MesenPath)) {
    throw "Mesen executable not found. Pass -MesenPath explicitly."
}

$lua = Join-Path $Root "scripts\mesen_spc_test.lua"
Write-Host "[4/4] Run Mesen test" -ForegroundColor Cyan
$args = @(
    "--fullscreen=0",
    "--movie", "",
    "--lua", "$lua",
    "$rom"
)

if ($CloseOnFinish) {
    $env:SPC_TEST_AUTOCLOSE = "1"
    & $MesenPath @args
    $exitCode = $LASTEXITCODE
    Write-Host "Mesen exit code: $exitCode"
    exit $exitCode
} else {
    $env:SPC_TEST_AUTOCLOSE = "0"
    $proc = Start-Process -FilePath $MesenPath -ArgumentList $args -PassThru
    Write-Host "Mesen launched windowed and left open (PID: $($proc.Id))."
    exit 0
}
