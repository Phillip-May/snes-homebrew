param(
    [string]$Compiler = "llvm-mos",
    [string]$MesenPath = "",
    [switch]$BuildFirst,
    [int]$TimeoutSeconds = 120
)

$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent $PSScriptRoot
Set-Location $Root
. (Join-Path $PSScriptRoot "mesen_runner_common.ps1")

if ($BuildFirst) {
    Write-Host "[1/3] Build ROM ($Compiler)" -ForegroundColor Cyan
    & make COMPILER=$Compiler
    if ($LASTEXITCODE -ne 0) { throw "ROM build failed" }
} else {
    Write-Host "[1/3] Build skipped" -ForegroundColor Cyan
}

$rom = Join-Path $Root "build\mainBankZero_${Compiler}.smc"
if (!(Test-Path $rom)) { throw "ROM not found: $rom" }
Invoke-LlvmMosMesenPrecheck -Root $Root -Compiler $Compiler -CheckLuaScripts

Write-Host "[2/3] Resolve Mesen executable" -ForegroundColor Cyan
$MesenPath = Resolve-MesenExecutable -MesenPath $MesenPath

$lua = Join-Path $Root "scripts\mesen_main_smoke.lua"
$result = Join-Path $Root "build\mesen_main_smoke_result.txt"
if (Test-Path $result) { Remove-Item -LiteralPath $result -Force }

# Mesen can mis-score this headerless LoROM as a copier-headered HiROM when
# the checksum fields are still zero. Feed the smoke test a temporary copier-
# headered copy so Mesen strips the dummy header and sees the real LoROM header.
$mesenRom = Join-Path $Root "build\mainBankZero_${Compiler}_mesen.smc"
New-MesenRomCopy -SourceRom $rom -OutputRom $mesenRom

Write-Host "[3/3] Run Mesen smoke test" -ForegroundColor Cyan
$args = @("--testRunner", "--doNotSaveSettings", "--timeout=$TimeoutSeconds", "--debug.scriptWindow.allowIoOsAccess=true", "$lua", "$mesenRom")
$txt = Invoke-MesenProcess -MesenPath $MesenPath -ArgumentList $args -ResultPath $result -TimeoutSeconds $TimeoutSeconds -IsFinal { param($text) ($text -like "PASS*" -or $text -like "FAIL*") -and $text.Length -gt 0 }

if (!(Test-Path $result)) {
    throw "Smoke result file not generated (timeout/boot failure): $result"
}

$txt = (Get-Content -Raw $result).Trim()
Write-Host "Smoke result: $txt"
if ($txt -notlike "PASS*") {
    throw "Smoke test failed: $txt"
}

exit 0
